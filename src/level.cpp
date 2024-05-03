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

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../include/physics.hpp"
#include "../include/globals.hpp"
#include "../include/ui.hpp"

const unsigned short NUM_WALLS = 16;
const unsigned short NUM_PIPES = 6;
const unsigned short NUM_PROPS = 0;

//////////////////////////////////////
// Tilemap
//////////////////////////////////////

void Tilemap::loadFromFile(const std::filesystem::path path) {

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
    std::string::size_type start, end;
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

void Tilemap::drawPropsWalls(const sf::Texture& walls, const sf::Texture& props, const sf::Vector2i tileSize) {
  
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

      tile.setScale({Globals::unitSize / tileSize.x, Globals::unitSize / tileSize.y});
      tile.setPosition({j * Globals::unitSize - 0.5f * Globals::unitSize, i * Globals::unitSize - 0.5f * Globals::unitSize});

      Globals::window->draw(tile);
    }
  }

}

void Tilemap::drawPipes(const sf::Texture& wholeTexture, const sf::Vector2i tileSize) {

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

      tile.setScale({Globals::unitSize / tileSize.x, Globals::unitSize / tileSize.y});
      tile.setPosition({j * Globals::unitSize - 0.5f * Globals::unitSize, i * Globals::unitSize - 0.5f * Globals::unitSize});

      Globals::window->draw(tile);
    }
  }

}

//////////////////////////////////////
// BouncyObjects
//////////////////////////////////////

void BouncyObjects::makeWalls() {

  sf::Vector2u windowSize = Globals::window->getSize();

  PhysicsObjects::BouncyObject floor;
  floor.setPoints({
    sf::Vector2f(0.5f * Globals::unitSize, windowSize.y - 0.5f * Globals::unitSize), 
    sf::Vector2f(windowSize.x - 0.5f * Globals::unitSize, windowSize.y - 0.5f * Globals::unitSize), 
    sf::Vector2f(windowSize.x - 0.5f * Globals::unitSize, windowSize.y + 100),
    sf::Vector2f(0.5f * Globals::unitSize, windowSize.y + 100)
  });
  floor.setCOR(0.8f);
  this->bo_list.push_back(floor);

  PhysicsObjects::BouncyObject right_wall;
  right_wall.setPoints({
    sf::Vector2f(windowSize.x - 0.5f * Globals::unitSize, 0.5f * Globals::unitSize), 
    sf::Vector2f(windowSize.x + 100, 0.5f * Globals::unitSize),
    sf::Vector2f(windowSize.x + 100, windowSize.y - 0.5f * Globals::unitSize),
    sf::Vector2f(windowSize.x - 0.5f * Globals::unitSize, windowSize.y - 0.5f * Globals::unitSize)
  });
  right_wall.setCOR(0.8f);
  this->bo_list.push_back(right_wall);

  PhysicsObjects::BouncyObject ceiling1, ceiling2;
  ceiling1.setPoints({
    sf::Vector2f(0.5f * Globals::unitSize, 0.5f * Globals::unitSize), 
    sf::Vector2f(1.5f * Globals::unitSize, 0.5f * Globals::unitSize),
    sf::Vector2f(1.5f * Globals::unitSize, -100),
    sf::Vector2f(0.5f * Globals::unitSize, -100)
  });
  ceiling1.setCOR(0.8f);
  this->bo_list.push_back(ceiling1);

  ceiling2.setPoints({
    sf::Vector2f(2.5f * Globals::unitSize, 0.5f * Globals::unitSize), 
    sf::Vector2f(windowSize.x - 0.5f * Globals::unitSize, 0.5f * Globals::unitSize),
    sf::Vector2f(windowSize.x - 0.5f * Globals::unitSize, -100),
    sf::Vector2f(2.5f * Globals::unitSize, -100)
  });
  ceiling2.setCOR(0.8f);
  this->bo_list.push_back(ceiling2);

  PhysicsObjects::BouncyObject left_wall;
  left_wall.setPoints({
    sf::Vector2f(0.5f * Globals::unitSize, 0.5f * Globals::unitSize), 
    sf::Vector2f(-100, 0.5f * Globals::unitSize),
    sf::Vector2f(-100, windowSize.y - 0.5f * Globals::unitSize),
    sf::Vector2f(0.5f * Globals::unitSize, windowSize.y - 0.5f * Globals::unitSize)
  });
  left_wall.setCOR(0.8f);
  this->bo_list.push_back(left_wall);

}

void BouncyObjects::makeBO(const std::vector<sf::Vector2f>& points, const float cor, const sf::Vector2f orientation) {

  PhysicsObjects::BouncyObject obj;
  obj.setPoints(points);
  obj.setCOR(cor);
  obj.setOrientation(orientation.normalized());

  this->bo_list.push_back(obj);

}

void BouncyObjects::loadFromFile(const std::filesystem::path path) {

  std::ifstream file;
  file.open(path, std::ios::in);

  if (!file.is_open()) {
    throw std::runtime_error("Couldn't open the level file.");
  }

  std::string lineStr;
  bool foundBO = false;
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
    startPoint.x = Globals::unitSize * std::stoi(lineStr.substr(one + 1, lineStr.find_first_of(',', one) - one));
    startPoint.y = Globals::unitSize * std::stoi(lineStr.substr(lineStr.find_first_of(',') + 1, lineStr.find_first_of(')', one) - lineStr.find_first_of(',')));

    startPoint -= sf::Vector2f(0.5f * Globals::unitSize, 0.5f * Globals::unitSize);

    one = lineStr.find_first_of(')');

    sf::Vector2f endPoint;
    int two = lineStr.find_first_of('(', one);
    endPoint.x = Globals::unitSize * std::stoi(lineStr.substr(two + 1, lineStr.find_first_of(',', two) - two)) + Globals::unitSize;
    endPoint.y = Globals::unitSize * std::stoi(lineStr.substr(lineStr.find_first_of(',', two) + 1, lineStr.find_first_of(')', two) - lineStr.find_first_of(',', two))) + Globals::unitSize;
    
    endPoint -= sf::Vector2f(0.5f * Globals::unitSize, 0.5f * Globals::unitSize);

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

//////////////////////////////////////
// MoneyBag
//////////////////////////////////////

MoneyBag::MoneyBag(const sf::Vector2f& newPos, const uint8_t newValue) : pos(newPos), value(newValue), collected(false) {
  std::filesystem::path texturePath = RESOURCES_PATH;
  texturePath.append("sprites/moneyBag.png");
  if (!this->texture.loadFromFile(texturePath)) {
    throw std::runtime_error("Couldn't load the money bag sprite.");
  }
}

bool MoneyBag::intersect(PhysicsObjects::Ball& ball) {
  std::vector<sf::Vector2f> points = {
    this->pos + sf::Vector2f(-0.3f * Globals::unitSize, 0.5f * Globals::unitSize),
    this->pos + sf::Vector2f(0.3f * Globals::unitSize, 0.5f * Globals::unitSize),
    this->pos + sf::Vector2f(0.3f * Globals::unitSize, -0.5f * Globals::unitSize),
    this->pos + sf::Vector2f(-0.3f * Globals::unitSize, -0.5f * Globals::unitSize),
  };
  // Now, out of simplicity I use AABB to check if at least one of 8 points on the ball is in the bag
  for (unsigned short i = 0; i < 8; ++i) {
    sf::Vector2f checkPoint = ball.getMidpoint() + (sf::Vector2f(1,0) * ball.getRadius()).rotatedBy(i * sf::degrees(45));

    if (checkPoint.x >= points[0].x && checkPoint.x <= points[1].x && checkPoint.y <= points[0].y && checkPoint.y >= points[2].y) {
      return true;
    }
  }
  return false;
}

void MoneyBag::draw() {
  sf::Sprite moneyBagSprite(this->texture);
  moneyBagSprite.setOrigin(0.5f * static_cast<sf::Vector2f>(this->texture.getSize()));
  moneyBagSprite.setScale(sf::Vector2f(Globals::unitSize / this->texture.getSize().x, Globals::unitSize / this->texture.getSize().y));
  moneyBagSprite.setPosition(this->pos);
  Globals::window->draw(moneyBagSprite);
}

void MoneyBag::fall(PhysicsObjects::Ball& ball, const unsigned windowHeight) {
  sf::Vector2f speed = 2.f * ball.getDirection();
  const float acceleraion = 9.81f;

  while (this->pos.y <= windowHeight + 0.5f * Globals::unitSize) {
    // Multiply the acceleration by deltaTime (a = dv/dt) and multiply the result by the direction
    sf::Vector2f direction(0, -0.01f * acceleraion);

    speed += direction;

    this->pos.x += speed.x;
    this->pos.y -= speed.y;

    sf::sleep(sf::milliseconds(10));

  }
}

//////////////////////////////////////
// Level
//////////////////////////////////////

Level::~Level() {
  for (MoneyBag* bag : this->moneyBags) {
    delete bag;
  }
}

void Level::initLevel() {

  this->moneyBags.clear();

  this->beginScore = this->scoreLabel.getScore();
  
  this->tilemap.loadFromFile(this->levelFilePath);
  this->tilemap.drawPropsWalls(this->walls, this->props, sf::Vector2i(128, 128));
  
  this->bouncyObjects.makeWalls();
  this->bouncyObjects.loadFromFile(this->levelFilePath);

  // Load the money bags from the level file
  std::ifstream levelStream;
  levelStream.open(this->levelFilePath, std::ios::in);

  if (!levelStream.is_open()) {
    throw std::runtime_error("Couldn't open the level file.");
  }

  std::string lineStr;
  bool foundInventoryHeader = false;
  bool foundMoneyBagHeader = false;

  std::vector<int8_t> invItems;
  std::vector<int16_t> invCounts;

  // Init the inventory
  while (std::getline(levelStream, lineStr)) {

    if (lineStr.find("[Inventory]") != std::string::npos) {
      foundInventoryHeader = true;
      continue;
    }

    if (!foundInventoryHeader) {
      continue;
    }
    if (lineStr.empty()) {
      break;
    }

    int one = lineStr.find_first_of('(');
    invItems.push_back(std::stoi(lineStr.substr(one + 1, lineStr.find_first_of(',', one) - one)));
    invCounts.push_back(std::stoi(lineStr.substr(lineStr.find_first_of(',') + 1, lineStr.find_first_of(')', one) - lineStr.find_first_of(','))));

  }

  this->inventory.setItems(invItems);
  this->inventory.setCounts(invCounts);

  while (std::getline(levelStream, lineStr)) {
    
    if (lineStr.find("[MoneyBags]") != std::string::npos) {
      foundMoneyBagHeader = true;
      continue;
    } else if (lineStr.find("[MoneyBagsNeeded]") != std::string::npos) {
      // Get the next line and set moneyBagsNeeded
      std::getline(levelStream, lineStr);
      this->moneyBagsNeeded = std::stoi(lineStr);
    }

    if (!foundMoneyBagHeader) {
      continue;
    }
    if (lineStr.empty()) {
      break;
    }

    sf::Vector2f pos;
    int one = lineStr.find_first_of('(');
    pos.x = Globals::unitSize * std::stof(lineStr.substr(one + 1, lineStr.find_first_of(',', one) - one));
    pos.y = Globals::unitSize * std::stof(lineStr.substr(lineStr.find_first_of(',') + 1, lineStr.find_first_of(')', one) - lineStr.find_first_of(',')));

    MoneyBag* bag = new MoneyBag(pos);
    this->moneyBags.push_back(bag);
  
  }

  this->neededScore = this->beginScore + this->moneyBagsNeeded * this->moneyBags[0]->getValue();

  // Init the ScoreLabel
  std::filesystem::path scoreLabelBackground = RESOURCES_PATH;
  scoreLabelBackground += "sprites/scoreLabelBackground.png";
  this->scoreLabel = UIElements::ScoreLabel("Money: $0", sf::Vector2f(0.5f * Globals::window->getSize().x, 0.325f * Globals::unitSize), sf::Vector2f(5.5f * Globals::unitSize, 0.65f * Globals::unitSize), scoreLabelBackground, sf::Color::Black);
  this->scoreLabel.setScore(this->beginScore);

  // Init the run button
  std::filesystem::path runButtonBackground = RESOURCES_PATH;
  runButtonBackground += "sprites/runButtonBackground.png";

  if (!this->runButtonOuter.loadFromFile(runButtonBackground)) {
    throw std::runtime_error("Couldn't load the run button background.");
  }

  this->runButton = UIElements::RunButton(this->runButtonOuter, sf::Vector2f(0.5f * Globals::window->getSize().x, Globals::unitSize ), sf::Vector2u(2.f * Globals::unitSize, 0.5f * Globals::unitSize));
}

void Level::resetMoneyBagPositions() {
  this->scoreLabel.setScore(this->beginScore);

  std::ifstream levelStream;
  levelStream.open(this->levelFilePath, std::ios::in);

  std::string lineStr;
  bool foundMoneyBagHeader = false;

  uint8_t ctr = 0;

  while (std::getline(levelStream, lineStr)) {

    if (lineStr.find("[MoneyBags]") != std::string::npos) {
      foundMoneyBagHeader = true;
      continue;
    }

    if (!foundMoneyBagHeader) {
      continue;
    }
    if (lineStr.empty()) {
      break;
    }

    sf::Vector2f pos;
    int one = lineStr.find_first_of('(');
    pos.x = Globals::unitSize * std::stof(lineStr.substr(one + 1, lineStr.find_first_of(',', one) - one));
    pos.y = Globals::unitSize * std::stof(lineStr.substr(lineStr.find_first_of(',') + 1, lineStr.find_first_of(')', one) - lineStr.find_first_of(',')));

    this->moneyBags[ctr]->setCollected(false);
    this->moneyBags[ctr]->setPosition(pos);

    ctr++;

  }
}