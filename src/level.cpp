/**
 * @file level.cpp
 * @author Patrick Vreeburg
 * @brief Handles the reading and drawing of the levels.
 * @version 0.1
 * @date 2024-02-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../include/level.hpp"
#include "physics.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

const unsigned short NUM_WALLS = 16;
const unsigned short NUM_PIPES = 6;
const unsigned short NUM_PROPS = 0;

void Tilemap::loadFromFile(std::filesystem::path path) {

  std::ifstream file;
  file.open(path, std::ios::in);

  std::string lineText;
  bool foundTileMap = false;
  unsigned short ctr = 0;
  if (!file.is_open()) {
    throw std::runtime_error("Couldn't read the level file.");
  }

  while(std::getline(file, lineText)) {
    if (lineText.find("[Tilemap]") != std::string::npos) {
      foundTileMap = true;
      continue;
    }
    if (ctr > 17) {
      break;
    }
    if (!foundTileMap) {
      continue;
    }
    int start, end;
    start = end = 0;
    int ctr2 = 0;
    while ((start = lineText.find_first_not_of(" ", end)) != std::string::npos) {
      end = lineText.find(" ", start);
      this->map[ctr][ctr2] = std::stoi(lineText.substr(start, end - start), nullptr, 36);
      ++ctr2;
    }

    ++ctr;
  }

  file.close();

}

void Tilemap::drawPropsWalls(sf::RenderWindow& window, sf::Texture& walls, sf::Texture& props, sf::Vector2i tileSize, float unitSize) {
  
  unsigned short wallsCols = walls.getSize().x / tileSize.x;
  
  unsigned short propsCols = props.getSize().x / tileSize.x;

  for (unsigned short i = 0; i < 18; ++i) {
    for (unsigned short j = 0; j < 18; ++j) {
      if (this->map[i][j] == 0 || (this->map[i][j] > NUM_WALLS && this->map[i][j] <= NUM_WALLS + NUM_PIPES)) {
        continue;
      }

      sf::Sprite tile(walls);

      if (this->map[i][j] <= NUM_WALLS) {
        // Place the right wall
        tile.setTextureRect(
          sf::IntRect(sf::Vector2i(
            (this->map[i][j] - 1) % wallsCols * tileSize.x,
            std::floor((this->map[i][j] - 1) / wallsCols) * tileSize.y), 
            tileSize)
        );
      } else if (this->map[i][j] <= NUM_WALLS + NUM_PIPES + NUM_PROPS) {
        tile.setTexture(props);
        // Place the right prop
        tile.setTextureRect(
          sf::IntRect(sf::Vector2i(
            (this->map[i][j] - NUM_WALLS - NUM_PIPES - 1) % propsCols * tileSize.x,
            std::floor((this->map[i][j] - NUM_WALLS - NUM_PIPES - 1) / propsCols) * tileSize.y), 
            tileSize)
        );
      }

      tile.setScale({unitSize / tileSize.x, unitSize / tileSize.y});
      tile.setPosition({j * unitSize - 0.5f * unitSize, i * unitSize - 0.5f * unitSize});

      window.draw(tile);
    }
  }

}

void Tilemap::drawPipes(sf::RenderWindow& window, sf::Texture& wholeTexture, sf::Vector2i tileSize, float unitSize) {

  unsigned short pipesCols = wholeTexture.getSize().x / tileSize.x;

  for (unsigned short i = 0; i < 18; ++i) {
    for (unsigned short j = 0; j < 18; ++j) {
      if (this->map[i][j] == 0 || this->map[i][j] <= NUM_WALLS || this->map[i][j] > NUM_WALLS + NUM_PIPES) {
        continue;
      }

      sf::Sprite tile(wholeTexture);

      // Place the right pipe
      tile.setTextureRect(sf::IntRect(sf::Vector2i(
        (this->map[i][j] - NUM_WALLS - 1) % pipesCols * tileSize.x,
        std::floor((this->map[i][j] - NUM_WALLS - 1) / pipesCols) * tileSize.y), 
        tileSize
      ));

      tile.setScale({unitSize / tileSize.x, unitSize / tileSize.y});
      tile.setPosition({j * unitSize - 0.5f * unitSize, i * unitSize - 0.5f * unitSize});

      window.draw(tile);
    }
  }

}

void BouncyObjects::makeWalls(sf::RenderWindow& window, float unitSize) {

  sf::Vector2u windowSize = window.getSize();

  PhysicsObjects::BouncyObject floor;
  floor.setPoints({
    sf::Vector2f(0.5f * unitSize, windowSize.y - 0.5f * unitSize), 
    sf::Vector2f(windowSize.x - 0.5f * unitSize, windowSize.y - 0.5f * unitSize), 
    sf::Vector2f(windowSize.x - 0.5f * unitSize, windowSize.y + 100),
    sf::Vector2f(0.5f * unitSize, windowSize.y + 100)
  });
  floor.setCOR(0.8f);
  this->bo_list.push_back(floor);

  PhysicsObjects::BouncyObject right_wall;
  right_wall.setPoints({
    sf::Vector2f(windowSize.x - 0.5f * unitSize, 0.5f * unitSize), 
    sf::Vector2f(windowSize.x + 100, 0.5f * unitSize),
    sf::Vector2f(windowSize.x + 100, windowSize.y - 0.5f * unitSize),
    sf::Vector2f(windowSize.x - 0.5f * unitSize, windowSize.y - 0.5f * unitSize)
  });
  right_wall.setCOR(0.8f);
  this->bo_list.push_back(right_wall);

  PhysicsObjects::BouncyObject ceiling;
  ceiling.setPoints({
    sf::Vector2f(0.5f * unitSize, 0.5f * unitSize), 
    sf::Vector2f(windowSize.x - 0.5f * unitSize, 0.5f * unitSize),
    sf::Vector2f(windowSize.x - 0.5f * unitSize, -100),
    sf::Vector2f(0.5f * unitSize, -100)
  });
  ceiling.setCOR(0.8f);
  this->bo_list.push_back(ceiling);

  PhysicsObjects::BouncyObject left_wall;
  left_wall.setPoints({
    sf::Vector2f(0.5f * unitSize, 0.5f * unitSize), 
    sf::Vector2f(-100, 0.5f * unitSize),
    sf::Vector2f(-100, windowSize.y - 0.5f * unitSize),
    sf::Vector2f(0.5f * unitSize, windowSize.y - 0.5f * unitSize)
  });
  left_wall.setCOR(0.8f);
  this->bo_list.push_back(left_wall);

}

void BouncyObjects::makeBO(const std::vector<sf::Vector2f>& points, float cor, const sf::Vector2f orientation) {

  PhysicsObjects::BouncyObject obj;
  obj.setPoints(points);
  obj.setCOR(cor);
  obj.setOrientation(orientation.normalized());

  this->bo_list.push_back(obj);

}

void BouncyObjects::loadFromFile(std::filesystem::path path, float unitSize) {

  std::ifstream file;
  file.open(path, std::ios::in);

  if (!file.is_open()) {
    throw std::runtime_error("Couldn't open the level file.");
  }

  std::string lineStr;
  bool foundBO;
  while (std::getline(file, lineStr)) {

    if (lineStr.find("[BouncyObjects]") != std::string::npos) {
      foundBO = true;
      continue;
    }
    if (!foundBO) {
      continue;
    }
    if (lineStr.empty()) {
      break;
    }

    PhysicsObjects::BouncyObject obj;

    std::vector<sf::Vector2f> points;
    int one = lineStr.find_first_of('(');
    sf::Vector2f startPoint;
    startPoint.x = unitSize * std::stoi(lineStr.substr(one + 1, lineStr.find_first_of(',', one) - one));
    startPoint.y = unitSize * std::stoi(lineStr.substr(lineStr.find_first_of(',') + 1, lineStr.find_first_of(')', one) - lineStr.find_first_of(',')));

    startPoint -= sf::Vector2f(0.5f * unitSize, 0.5f * unitSize);

    one = lineStr.find_first_of(')');

    sf::Vector2f endPoint;
    int two = lineStr.find_first_of('(', one);
    endPoint.x = unitSize * std::stoi(lineStr.substr(two + 1, lineStr.find_first_of(',', two) - two)) + unitSize;
    endPoint.y = unitSize * std::stoi(lineStr.substr(lineStr.find_first_of(',', two) + 1, lineStr.find_first_of(')', two) - lineStr.find_first_of(',', two))) + unitSize;
    
    endPoint -= sf::Vector2f(0.5f * unitSize, 0.5f * unitSize);

    points = {sf::Vector2f(startPoint.x, startPoint.y), sf::Vector2f(endPoint.x, startPoint.y), sf::Vector2f(endPoint.x, endPoint.y), sf::Vector2f(startPoint.x, endPoint.y)};

    obj.setPoints(points);
    int digitStart = lineStr.find_first_of(')', two) + 2;
    obj.setCOR(std::stof(lineStr.substr(digitStart, lineStr.find_first_of(' ', digitStart) - digitStart)));
    
    sf::Vector2f orientation;
    int orientationStart = lineStr.find_first_of('(', digitStart) + 1;
    orientation.x = std::stof(lineStr.substr(orientationStart, lineStr.find_first_of(',', orientationStart) - orientationStart));
    orientation.y = std::stof(lineStr.substr(lineStr.find_first_of(',', orientationStart) + 1, lineStr.find_first_of(')', orientationStart) - lineStr.find_first_of(',', orientationStart) - 1));

    obj.setOrientation(orientation.normalized());

    this->bo_list.push_back(obj);

  }

}

void Level::initLevel(sf::RenderWindow& window, float unitSize, sf::Texture& walls, sf::Texture& props, sf::Texture& pipes) {

  this->tilemap.loadFromFile(this->levelFilePath);
  this->tilemap.drawPropsWalls(window, walls, props, sf::Vector2i(128, 128), unitSize);
  
  this->bouncyObjects.makeWalls(window, unitSize);
  this->bouncyObjects.loadFromFile(this->levelFilePath, unitSize);

  // ! This is just for testing. The following is temporaty
  float windowYSize = window.getSize().y;
  this->bouncyObjects.makeBO({
    sf::Vector2f(0, windowYSize - 70),
    sf::Vector2f(200, windowYSize - 30),
    sf::Vector2f(200, windowYSize + 100),
    sf::Vector2f(0, windowYSize + 100),
  }, 0.8f, sf::Vector2f(200, -40));
}