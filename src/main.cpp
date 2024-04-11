/**
 * @file main.cpp
 * @author Patrick Vreeburg
 * @brief The main file.
 * @version 0.1
 * @date 2024-01-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "../include/physics.hpp"
#include "../include/level.hpp"
#include "../include/ui_conf.hpp"
#include "../include/build.hpp"
#include "../include/globals.hpp"

#define Key sf::Keyboard::Key

const float WINDOW_SIZE_FACTOR = 0.9f;
const short NULL_VALUE = -1;

float unitSize = 80.f; // The conversion factor from SFML coordinates to meters

sf::Vector2u windowSize;

// Textures
sf::Texture wallsTexture;
sf::Texture propsTexture;
sf::Texture pipesTexture;

// Things that the player can click on
std::vector<UIElements::Button*> buttons;
UserObjects::EditableObjectList editableObjects;

// Keyboard-specific variables
std::string modifier; // The modifier pressed ("Ctrl", "Shift", "Alt" or empty)
bool rotate;

// Score
long score;

// Threading
std::vector<std::thread> threads;

void applyForces(PhysicsObjects::Ball& ball, float deltaTime) {
  ball.applyForce(deltaTime, ball.getMass() * (unitSize * 9.81), {0,-1});

  ball.updatePoistion(deltaTime);
}

void checkCollision(PhysicsObjects::BouncyObject& object, PhysicsObjects::Ball& ball) {
  short collisionSide = object.checkBallCollision(ball);
  if (object.getJustBounced() != collisionSide && collisionSide != NULL_VALUE) {
    object.bounce(ball, collisionSide);
  } else if (object.getJustBounced() != NULL_VALUE && collisionSide == NULL_VALUE) {
    // This prevents the ball from inevitably staying in the first object it made contact with
    object.setJustBounced(NULL_VALUE);
  }
}

void loop(sf::RenderWindow& window, PhysicsObjects::Ball& ball, Level& level, UIElements::Inventory& inventory, float deltaTime) {

  sf::Event event;
  while (window.pollEvent(event)) {

    switch (event.type) {

      case sf::Event::Closed:
        window.close();
        break;
      
      case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button != sf::Mouse::Button::Left) break;
        // Check if the mouse clicked on any of the registered buttons
        if (UserObjects::getBuilding()->getSize().length() != 0) {
          UserObjects::getBuilding()->place(editableObjects);
        }
        for (UIElements::Button* pButton : buttons) {
          if (!pButton->intersect(sf::Mouse::getPosition(window))) continue;
          pButton->onClick();
        }
        break;
      
      case sf::Event::KeyPressed:
        if (sf::Keyboard::isKeyPressed(Key::LControl)) {
          modifier = "Ctrl";
        } else if (sf::Keyboard::isKeyPressed(Key::LShift)) {
          modifier = "Shift";
        } else if (sf::Keyboard::isKeyPressed(Key::LAlt)) {
          modifier = "Alt";
        } else if (sf::Keyboard::isKeyPressed(Key::R) || sf::Keyboard::isKeyPressed(Key::T)) {
          rotate = true;
        }
        break;
      
      case sf::Event::KeyReleased:
        if (!sf::Keyboard::isKeyPressed(Key::LShift) && !sf::Keyboard::isKeyPressed(Key::LControl) && !sf::Keyboard::isKeyPressed(Key::LAlt)) {
          modifier = "";
        } else if (!sf::Keyboard::isKeyPressed(Key::R) && !sf::Keyboard::isKeyPressed(Key::T)) {
          rotate = false;
        }
        break;
      
      default:
        break;

    }

  }
  window.clear();

  // Just for debugging's sake. Stops the bal from abruptly teleporting down due to lag at the start.
  if (deltaTime > 1) {
    // Skip this frame
    deltaTime = 0;
  }

  applyForces(ball, deltaTime);

  // std::clog << "\033[K\rBall speed: " << ball.getVelocity() << " pixels per second." << std::flush;

  for (PhysicsObjects::BouncyObject& object : level.getBouncyObjects().getList()) {
    checkCollision(object, ball);
  }

  level.getTilemap().drawPropsWalls(wallsTexture, propsTexture, sf::Vector2i(128, 128));
  
  window.draw(ball);

  level.getTilemap().drawPipes(pipesTexture, sf::Vector2i(128, 128));

  // Display the user's objects
  for (UserObjects::EditableObject* obj : editableObjects.getObjects()) {
    obj->draw();
    if (obj->hasBouncyObject()) {
      checkCollision(obj->getBouncyObject(), ball);
    }
  }

  // Display the money bags and check if the ball hits the bag.
  // If the ball hits the bag, increase the score and make it fall.
  for (MoneyBag* bag : level.getMoneyBags()) {
    bag->draw();
    if (!bag->intersect(ball) || bag->getCollected()) {
      continue;
    }
    bag->setCollected(true);
    
    threads.emplace_back(std::bind(&MoneyBag::fall, bag, ball, window.getSize().y));
    threads.back().detach();
    std::clog << "The ball hit a bag!" << std::endl;
  }

  inventory.draw();

  // Determine if the player is building something. If so, call the ghost object's loop()
  if (UserObjects::getBuilding()->getSize().length() != 0) {
    UserObjects::getBuilding()->loop(rotate, modifier);
  }
  
  window.display();

}

void loadTexture(std::string pathFromRes, sf::Texture& value) {

  std::filesystem::path imgPath = RESOURCES_PATH;
  imgPath.append(pathFromRes);

  if (!value.loadFromFile(imgPath)) {
    throw std::runtime_error("Failed to load sprite!");
  }
  value.setSmooth(true);

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

  // Set the window-related global variables and initialise the global font
  Globals::unitSize = unitSize;
  Globals::window = &window;
  Globals::initFont();

  sf::Texture ballTexture;
  loadTexture("sprites/ball.png", ballTexture);

  ballTexture.setSmooth(true);

  PhysicsObjects::Ball ball{ballTexture, {75.f,75.f}, 0.1f, 0.25f * unitSize};

  // std::cout << "Ball midpoint: " << ball.getMidpoint().x << "," << ball.getMidpoint().y << std::endl;
  // std::cout << "Ball radius: " << ball.getRadius()<< std::endl;
  // std::cout << "Ball origin: " << ball.getOrigin().x << "," << ball.getOrigin().y << std::endl;
  // std::cout << "Ball bound offset: " << ball.getGlobalBounds().left << "," << ball.getGlobalBounds().top << std::endl;
  // std::cout << "Ball bound offset (local): " << ball.getLocalBounds().left << "," << ball.getLocalBounds().top << std::endl;
  // std::cout << "Ball bounds size: " << ball.getGlobalBounds().width << "," << ball.getGlobalBounds().height << std::endl;
  // std::cout << "Ball bounds size (local): " << ball.getLocalBounds().width << "," << ball.getLocalBounds().height << std::endl;

  // Initialise a test level
  std::filesystem::path tmppath = RESOURCES_PATH;
  tmppath += "levels/level0.ql";

  Level tmpLevel{tmppath};
  
  // Load all of the texture atlases
  loadTexture("sprites/tilemapWall.png", wallsTexture);
  loadTexture("sprites/tilemapProps.png", propsTexture);
  loadTexture("sprites/tilemapPipes.png", pipesTexture);

  // Initialise the button outer texture and the items
  sf::Texture buttonOuter;
  loadTexture("sprites/buttonBackground.png", buttonOuter);

  tmpLevel.initLevel(wallsTexture, propsTexture, pipesTexture);
  std::clog << "BouncyObject list size: " << tmpLevel.getBouncyObjects().getList().size() << std::endl;

  // Create the inventory
  UIElements::Inventory inventory{std::vector<uint8_t>{0, 1, 2}, std::vector<int16_t>{5, 6, 12}, buttonOuter};
  for (UIElements::InventoryButton* button : inventory.getButtons()) {
    buttons.push_back(button);
  }

  sf::Clock dt_clock;

  while (window.isOpen()) {
    float deltaTime = dt_clock.restart().asSeconds();
    loop(window, ball, tmpLevel, inventory, deltaTime);
  }

  return 0;
}