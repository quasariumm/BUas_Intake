#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <vector>

#include "../include/physics.hpp"

const float WINDOW_SIZE_FACTOR = 0.9f;
const short NULL_VALUE = -1;

float unitSize; // The conversion factor from SFML coordinates to meters

void applyForces(PhysicsObjects::Ball& ball, float deltaTime) {
  ball.applyForce(deltaTime, ball.getMass() * (20 * 9.81), {0,-1});

  ball.updatePoistion(deltaTime);
}

void loop(sf::RenderWindow& window, PhysicsObjects::Ball& ball, std::vector<PhysicsObjects::BouncyObject>& bouncyObjectList, float deltaTime) {

  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window.close();
    }
  }
  window.clear();
  applyForces(ball, deltaTime);

  for (PhysicsObjects::BouncyObject& object : bouncyObjectList) {
    short collisionSide = object.checkBallCollision(ball);
    //std::cout << collisionSide << "=>" << object.justBounced;
    if (object.getJustBounced() != collisionSide && collisionSide != NULL_VALUE) {
      object.bounce(ball, collisionSide);
    } else if (object.getJustBounced() != NULL_VALUE && collisionSide == NULL_VALUE) {
      // This prevents the ball from inevitably staying in the first object it made contact with
      object.setJustBounced(NULL_VALUE);
    }
  }

  // Just visual
  sf::ConvexShape convex;

  convex.setPointCount(3);

  sf::Vector2u windowSize = window.getSize();
  convex.setPoint(0, sf::Vector2f(0, windowSize.y - 40));
  convex.setPoint(1, sf::Vector2f(0, windowSize.y));
  convex.setPoint(2, sf::Vector2f(200, windowSize.y));

  convex.setFillColor(sf::Color::Green);

  window.draw(convex);
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

  sf::Texture ballTexture;
  std::filesystem::path ballTexturePath = RESOURCES_PATH;
  ballTexturePath += "sprites/ball.png";

  if (!ballTexture.loadFromFile(ballTexturePath)) {
    throw std::runtime_error("Failed to load ball sprite!");
  }
  ballTexture.setSmooth(true);

  PhysicsObjects::Ball ball{ballTexture, {50.f,50.f}, 2, 30};

  // std::cout << "Ball midpoint: " << ball.getMidpoint().x << "," << ball.getMidpoint().y << std::endl;
  // std::cout << "Ball radius: " << ball.getRadius()<< std::endl;
  // std::cout << "Ball origin: " << ball.getOrigin().x << "," << ball.getOrigin().y << std::endl;
  // std::cout << "Ball bound offset: " << ball.getGlobalBounds().left << "," << ball.getGlobalBounds().top << std::endl;
  // std::cout << "Ball bound offset (local): " << ball.getLocalBounds().left << "," << ball.getLocalBounds().top << std::endl;
  // std::cout << "Ball bounds size: " << ball.getGlobalBounds().width << "," << ball.getGlobalBounds().height << std::endl;
  // std::cout << "Ball bounds size (local): " << ball.getLocalBounds().width << "," << ball.getLocalBounds().height << std::endl;

  // Temp
  sf::Vector2u windowSize = window.getSize();
  PhysicsObjects::BouncyObject floor;
  floor.setPoints({
    sf::Vector2f(0, windowSize.y), 
    sf::Vector2f(windowSize.x, windowSize.y), 
    sf::Vector2f(windowSize.x, windowSize.y + 100),
    sf::Vector2f(0, windowSize.y + 100)
  });
  floor.setCOR(0.8f);

  PhysicsObjects::BouncyObject right_wall;
  right_wall.setPoints({
    sf::Vector2f(windowSize.x, 0), 
    sf::Vector2f(windowSize.x + 100, 0),
    sf::Vector2f(windowSize.x + 100, windowSize.y),
    sf::Vector2f(windowSize.x, windowSize.y)
  });
  right_wall.setCOR(0.8f);

  PhysicsObjects::BouncyObject obj1;
  obj1.setPoints({
    sf::Vector2f(0, windowSize.y - 40),
    sf::Vector2f(200, windowSize.y),
    sf::Vector2f(200, windowSize.y + 100),
    sf::Vector2f(0, windowSize.y + 100),
  });
  obj1.setCOR(0.8f);
  obj1.setOrientation(sf::Vector2f(200, -40).normalized());

  std::vector<PhysicsObjects::BouncyObject> boList = {floor, right_wall, obj1};

  sf::Clock dt_clock;

  while (window.isOpen()) {
    float deltaTime = dt_clock.restart().asSeconds();
    loop(window, ball, boList, deltaTime);
  }

  return 0;
}