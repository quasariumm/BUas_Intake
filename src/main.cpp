#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Clock.hpp>
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <vector>

#include "../include/physics.hpp"
#include "SFML/System/Vector2.hpp"

const float WINDOW_SIZE_FACTOR = 0.9f;
const short NULL_VALUE = -1;

float unitSize; // The conversion factor from SFML coordinates to meters

void applyForces(PhysicsObjects::Ball& ball, float deltaTime) {
  ball.applyForce(deltaTime, ball.getMass() * 9.81, {0,1});

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

  short collisionSide = bouncyObjectList[0].checkBallCollision(ball);
  if (collisionSide != NULL_VALUE) {
    std::cout << collisionSide << std::endl;
  }

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

  PhysicsObjects::Ball ball{ballTexture, {25.f,25.f}};

  // Temp
  sf::Vector2u windowSize = window.getSize();
  PhysicsObjects::BouncyObject floor;
  floor.setPoints({
    sf::Vector2f(0, windowSize.y), 
    sf::Vector2f(windowSize.x, windowSize.y), 
    sf::Vector2f(windowSize.x, windowSize.y + 100),
    sf::Vector2f(0, windowSize.y + 100)
  });

  std::vector<PhysicsObjects::BouncyObject> boList = {floor};

  sf::Clock dt_clock;

  while (window.isOpen()) {
    float deltaTime = dt_clock.restart().asSeconds();
    loop(window, ball, boList, deltaTime);
  }

  return 0;
}