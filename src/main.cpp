#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <vector>

#include "../include/physics.hpp"
#include "../include/level.hpp"

const float WINDOW_SIZE_FACTOR = 0.9f;
const short NULL_VALUE = -1;

float unitSize = 80.f; // The conversion factor from SFML coordinates to meters

sf::Vector2u windowSize;

sf::Texture wallsTexture;
sf::Texture propsTexture;

void applyForces(PhysicsObjects::Ball& ball, float deltaTime) {
  ball.applyForce(deltaTime, ball.getMass() * (unitSize * 9.81), {0,-1});

  ball.updatePoistion(deltaTime);
}

void loop(sf::RenderWindow& window, PhysicsObjects::Ball& ball, std::vector<PhysicsObjects::BouncyObject>& bouncyObjectList, Tilemap& tm, float deltaTime) {

  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window.close();
    }
  }
  window.clear();

  // Just for debugging's sake
  if (deltaTime > 1) {
    // Skip this frame
    deltaTime = 0;
  }

  applyForces(ball, deltaTime);

  for (PhysicsObjects::BouncyObject& object : bouncyObjectList) {
    short collisionSide = object.checkBallCollision(ball, unitSize);
    //std::cout << collisionSide << "=>" << object.justBounced;
    if (object.getJustBounced() != collisionSide && collisionSide != NULL_VALUE) {
      object.bounce(ball, collisionSide);
    } else if (object.getJustBounced() != NULL_VALUE && collisionSide == NULL_VALUE) {
      // This prevents the ball from inevitably staying in the first object it made contact with
      object.setJustBounced(NULL_VALUE);
    }
  }

  tm.drawPropsWalls(window, wallsTexture, propsTexture, sf::Vector2i(128, 128), unitSize);
  window.draw(ball);
  window.display();

}

int main() {

  // The window is square with a side of 80% of the shortest edge of the screen
  sf::Vector2u monitorSize = sf::VideoMode::getDesktopMode().size;
  unsigned shortestEdge = (monitorSize.x > monitorSize.y) ? monitorSize.y : monitorSize.x;

  sf::RenderWindow window(sf::VideoMode(
    {static_cast<unsigned>(WINDOW_SIZE_FACTOR * shortestEdge), static_cast<unsigned>(WINDOW_SIZE_FACTOR * shortestEdge)}),
    "BUas Intake Game"
  );

  // NOTE: Just temporary
  window.setMaximumSize(window.getSize());
  window.setMinimumSize(window.getSize());

  window.setFramerateLimit(0);
  window.setVerticalSyncEnabled(true);

  // Set the unit size (conversion from pixels to 'meters') to 1/18 (16x16 for the level, 1 on eacht side for the wall tiles) of the screen width
  unitSize = static_cast<float>(window.getSize().y) / 17.f;
  std::cout << "Unit size is: " << unitSize <<std::endl;
  windowSize = window.getSize();

  sf::Texture ballTexture;
  std::filesystem::path ballTexturePath = RESOURCES_PATH;
  ballTexturePath += "sprites/ball.png";

  if (!ballTexture.loadFromFile(ballTexturePath)) {
    throw std::runtime_error("Failed to load ball sprite!");
  }
  ballTexture.setSmooth(true);

  PhysicsObjects::Ball ball{ballTexture, {75.f,75.f}, 0.1, 0.25f * unitSize};

  // std::cout << "Ball midpoint: " << ball.getMidpoint().x << "," << ball.getMidpoint().y << std::endl;
  // std::cout << "Ball radius: " << ball.getRadius()<< std::endl;
  // std::cout << "Ball origin: " << ball.getOrigin().x << "," << ball.getOrigin().y << std::endl;
  // std::cout << "Ball bound offset: " << ball.getGlobalBounds().left << "," << ball.getGlobalBounds().top << std::endl;
  // std::cout << "Ball bound offset (local): " << ball.getLocalBounds().left << "," << ball.getLocalBounds().top << std::endl;
  // std::cout << "Ball bounds size: " << ball.getGlobalBounds().width << "," << ball.getGlobalBounds().height << std::endl;
  // std::cout << "Ball bounds size (local): " << ball.getLocalBounds().width << "," << ball.getLocalBounds().height << std::endl;

  // BouncyObjects
  std::filesystem::path tmppath = RESOURCES_PATH;
  tmppath += "levels/level0.ql";

  BouncyObjects bo;
  bo.makeWalls(window, unitSize);
  bo.makeBO({
    sf::Vector2f(0, windowSize.y - 70),
    sf::Vector2f(200, windowSize.y - 30),
    sf::Vector2f(200, windowSize.y + 100),
    sf::Vector2f(0, windowSize.y + 100),
  }, 0.8f, sf::Vector2f(200, -40));

  bo.loadFromFile(tmppath, unitSize);

  std::clog << "BouncyObject list size: " << bo.getList().size() << std::endl;

  //TEST
  std::filesystem::path wallsPath = RESOURCES_PATH;
  wallsPath += "sprites/tilemapWall.png";

  if (!wallsTexture.loadFromFile(wallsPath)) {
    throw std::runtime_error("Failed to load wall tilemap!");
  }

  std::filesystem::path propsPath = RESOURCES_PATH;
  propsPath += "sprites/tilemapProps.png";

  if (!propsTexture.loadFromFile(propsPath)) {
    throw std::runtime_error("Failed to load props tilemap!");
  }

  Tilemap tm;
  tm.loadFromFile(tmppath);

  sf::Clock dt_clock;

  while (window.isOpen()) {
    float deltaTime = dt_clock.restart().asSeconds();
    loop(window, ball, bo.getList(), tm, deltaTime);
  }

  return 0;
}