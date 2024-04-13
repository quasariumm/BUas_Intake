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
#include "../include/math.hpp"
#include "../include/globals.hpp"

const unsigned short NUM_WALLS = 16;
const unsigned short NUM_PIPES = 6;
const unsigned short NUM_PROPS = 0;

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

  PhysicsObjects::BouncyObject ceiling;
  ceiling.setPoints({
    sf::Vector2f(0.5f * Globals::unitSize, 0.5f * Globals::unitSize), 
    sf::Vector2f(windowSize.x - 0.5f * Globals::unitSize, 0.5f * Globals::unitSize),
    sf::Vector2f(windowSize.x - 0.5f * Globals::unitSize, -100),
    sf::Vector2f(0.5f * Globals::unitSize, -100)
  });
  ceiling.setCOR(0.8f);
  this->bo_list.push_back(ceiling);

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
  // Now, I just use a more simple version of the collision system in physics.cpp
  for (unsigned short i = 0; i < 4; ++i) {
    // Get the formula
    // The a and b can be filled in by using the normal of the side.

    sf::Vector2f point1 = points[i];
    sf::Vector2f point2 = points[(i+1)%4];

    sf::Vector2f direction_vector = (point2 - point1).normalized();
    sf::Vector2f normal = direction_vector.perpendicular();

    float a, b, c;

    a = normal.x;
    b = normal.y;

    c = -1 * (a * point1.x + b * point1.y);

    float distanceToMidpoint = getDistance(a, b, c, ball.getMidpoint());

    if (distanceToMidpoint >= ball.getRadius()) {
      return false;
    }
    // Hurray, at least the line is in the circle.
    // Now to check if it is in the actual object, I multiply the normal with the distance and add it to the midpoint.
    // If that point is on the line and not outside the side, it collides.

    sf::Vector2f checkPoint = ball.getMidpoint() + distanceToMidpoint * normal;
    if (std::round(a * checkPoint.x + b * checkPoint.y + c) == 0) {
      // If xDirection is true, the side is more horizontal than vertical
      bool xDirection = std::abs(point2.x - point1.x) > std::abs(point2.y - point1.y);
      // Get the smallest and the biggest point based on the x or y coordinate (depending on xDirection).
      sf::Vector2f smallest = 
        (xDirection)
        ? ((point1.x < point2.x) ? point1 : point2)
        : ((point1.y < point2.y) ? point1 : point2);
      sf::Vector2f biggest = (smallest == point1) ? point2 : point1;
      // Check if the point is between the edges, thus in the object. If so, we collided with the side.
      if (
        (xDirection && checkPoint.x >= smallest.x && checkPoint.x <= biggest.x) ||
        (!xDirection && checkPoint.y >= smallest.y && checkPoint.y <= biggest.y)
      ) {
        return true;
      }
    }

    checkPoint = ball.getMidpoint() + distanceToMidpoint * -normal;
    // This is the same as the if-statement above.
    if (std::round(a * checkPoint.x + b * checkPoint.y + c) == 0) {
      // If xDirection is true, the side is more horizontal than vertical
      bool xDirection = std::abs(point2.x - point1.x) > std::abs(point2.y - point1.y);
      // Get the smallest and the biggest point based on the x or y coordinate (depending on xDirection).
      sf::Vector2f smallest = 
        (xDirection)
        ? ((point1.x < point2.x) ? point1 : point2)
        : ((point1.y < point2.y) ? point1 : point2);
      sf::Vector2f biggest = (smallest == point1) ? point2 : point1;
      // Check if the point is between the edges, thus in the object. If so, we collided with the side.
      if (
        (xDirection && checkPoint.x >= smallest.x && checkPoint.x <= biggest.x) ||
        (!xDirection && checkPoint.y >= smallest.y && checkPoint.y <= biggest.y)
      ) {
        return true;
      }
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
  const float mass = 0.1f;
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

Level::~Level() {
  for (MoneyBag* bag : this->moneyBags) {
    delete bag;
  }
}

void Level::initLevel(const sf::Texture& walls, const sf::Texture& props, const sf::Texture& pipes) {

  this->tilemap.loadFromFile(this->levelFilePath);
  this->tilemap.drawPropsWalls(walls, props, sf::Vector2i(128, 128));
  
  this->bouncyObjects.makeWalls();
  this->bouncyObjects.loadFromFile(this->levelFilePath);

  // Load the money bags from the level file
  std::ifstream levelStream;
  levelStream.open(this->levelFilePath, std::ios::in);

  if (!levelStream.is_open()) {
    throw std::runtime_error("Couldn't open the level file.");
  }

  std::string lineStr;
  bool foundMoneyBagHeader = false;

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
    pos.x = Globals::unitSize * std::stoi(lineStr.substr(one + 1, lineStr.find_first_of(',', one) - one));
    pos.y = Globals::unitSize * std::stoi(lineStr.substr(lineStr.find_first_of(',') + 1, lineStr.find_first_of(')', one) - lineStr.find_first_of(',')));

    MoneyBag* bag = new MoneyBag(pos);
    this->moneyBags.push_back(bag);
  
  }

  // Init the ScoreLabel
  std::filesystem::path scoreLabelBackground = RESOURCES_PATH;
  scoreLabelBackground += "sprites/scoreLabelBackground.png";
  this->scoreLabel = UIElements::ScoreLabel(std::wstring(L"Money: $0"), sf::Vector2f(0.5f * Globals::window->getSize().x, 0), sf::Vector2f(5.5f * Globals::unitSize, 0.65f * Globals::unitSize), scoreLabelBackground, sf::Color::Black);

  // ! This is just for testing. The following is temporaty
  float windowYSize = Globals::window->getSize().y;
  this->bouncyObjects.makeBO({
    sf::Vector2f(0, windowYSize - 70),
    sf::Vector2f(200, windowYSize - 30),
    sf::Vector2f(200, windowYSize + 100),
    sf::Vector2f(0, windowYSize + 100),
  }, 0.8f, sf::Vector2f(200, -40));
}