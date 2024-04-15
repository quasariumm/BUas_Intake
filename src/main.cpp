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
#include <algorithm>
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

//////////////////////////////////////
// Variables
//////////////////////////////////////

#define Key sf::Keyboard::Key

const float WINDOW_SIZE_FACTOR = 0.9f;
const short NULL_VALUE = -1;

float unitSize = 80.f; // The conversion factor from SFML coordinates to meters

sf::Vector2u windowSize;

// Ball origin
sf::Vector2f ballOrigin;

// Textures
sf::Texture wallsTexture;
sf::Texture propsTexture;
sf::Texture pipesTexture;

// Things that the player can click on
UserObjects::EditableObjectList editableObjects;

UserObjects::EditableObject* editing = nullptr;
UIElements::EditGUI editGUI{sf::Vector2f(), sf::Vector2f()};

// Keyboard-specific variables
std::string modifier; // The modifier pressed ("Ctrl", "Shift", "Alt" or empty)
bool rotate;

// Score
long score;

// Threading
std::vector<std::thread> threads;

//////////////////////////////////////
// Functions
//////////////////////////////////////

// Ball-related functions

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

// Event functions

void mousePressedEvent(sf::Event& event, UIElements::Inventory& inventory, Level& level) {
  if (event.mouseButton.button != sf::Mouse::Button::Left) return;
  // Check if the mouse clicked on any of the registered buttons
  if (UserObjects::getBuilding()->getSize().length() != 0) {
    UserObjects::getBuilding()->place(editableObjects, inventory);
  }

  std::vector<UIElements::Button*> allButtons;
  allButtons.push_back(static_cast<UIElements::Button*>(&level.getRunButton()));
  for (UIElements::InventoryButton* button : inventory.getButtons()) {
    allButtons.push_back(static_cast<UIElements::Button*>(button));
  }


  for (UIElements::Button* pButton : allButtons) {
    if (!pButton->intersect(sf::Mouse::getPosition(*Globals::window))) continue;
    pButton->onClick();
  }
  // Check click on EditableObjects
  UserObjects::EditableObject* clicked = nullptr;
  for (UserObjects::EditableObject* obj : editableObjects.getObjects()) {
    if (obj->intersect(sf::Mouse::getPosition(*Globals::window))) {
      clicked = obj;
    }
  }
  if (editing != nullptr && clicked == nullptr) {
    // Cancel the editing
    editing = nullptr;
  } else if (clicked != editing) {
    editing = clicked;
  }
}

void keyPressedEvent(UIElements::Inventory& inventory) {
  if (sf::Keyboard::isKeyPressed(Key::LControl)) {
    modifier = "Ctrl";
  } else if (sf::Keyboard::isKeyPressed(Key::LShift)) {
    modifier = "Shift";
  } else if (sf::Keyboard::isKeyPressed(Key::LAlt)) {
    modifier = "Alt";
  } else if (sf::Keyboard::isKeyPressed(Key::R) || sf::Keyboard::isKeyPressed(Key::T)) {
    rotate = true;
  } else if (sf::Keyboard::isKeyPressed(Key::F) && editing != nullptr) {
    // Delete the object and enter building mode
    
    uint8_t itemId = editing->getItemId();

    UserObjects::initBuilding(editing->getSize(), editing->getTexturePath(), itemId);

    // Don't touch this line. If the line beneath this line is placed at the end of the if-statement, it breaks.
    auto tmpEditing = editing;
    
    editing = nullptr;

    std::vector<UserObjects::EditableObject*>::iterator it;
    if ((it = std::find(editableObjects.getObjects().begin(), editableObjects.getObjects().end(), tmpEditing)) != editableObjects.getObjects().end()) {
      delete *it;
      editableObjects.getObjects().erase(it);
    }

    inventory.changeCount(itemId, 1);

  } else if (sf::Keyboard::isKeyPressed(Key::G) && editing != nullptr) {
    // Delete the object and add one to the count in the inventory
    uint8_t itemId = editing->getItemId();

    // Don't touch this line. If the line beneath this line is placed at the end of the if-statement, it breaks.
    auto tmpEditing = editing;

    editing = nullptr;

    std::vector<UserObjects::EditableObject*>::iterator it;
    if ((it = std::find(editableObjects.getObjects().begin(), editableObjects.getObjects().end(), tmpEditing)) != editableObjects.getObjects().end()) {
      delete *it;
      editableObjects.getObjects().erase(it);
    }
    
    inventory.changeCount(itemId, 1);

  }
}

void keyReleasedEvent() {
  if (!sf::Keyboard::isKeyPressed(Key::LShift) && !sf::Keyboard::isKeyPressed(Key::LControl) && !sf::Keyboard::isKeyPressed(Key::LAlt)) {
    modifier = "";
  } else if (!sf::Keyboard::isKeyPressed(Key::R) && !sf::Keyboard::isKeyPressed(Key::T)) {
    rotate = false;
  }
}

// Loop

void loop(sf::RenderWindow& window, PhysicsObjects::Ball& ball, Level& level, UIElements::Inventory& inventory, float deltaTime) {
  
  window.clear();

  sf::Event event;
  while (window.pollEvent(event)) {

    switch (event.type) {

      case sf::Event::Closed:
        window.close();
        break;
      
      case sf::Event::MouseButtonPressed:
        mousePressedEvent(event, inventory, level);
        break;
      
      case sf::Event::KeyPressed:
        keyPressedEvent(inventory);
        break;
      
      case sf::Event::KeyReleased:
        keyReleasedEvent();
        break;
      
      default:
        break;

    }

  }

  // Just for debugging's sake. Stops the bal from abruptly teleporting down due to lag at the start.
  if (deltaTime > 1) {
    // Skip this frame
    deltaTime = 0;
  }

  if (Globals::simulationOn) applyForces(ball, deltaTime);


  for (PhysicsObjects::BouncyObject& object : level.getBouncyObjects().getList()) {
    checkCollision(object, ball);
    // Stop the simulation right before the ball falls through the ground
    // or when the ball has glitched through a wall of the floor and is outside of the level
    if (
      (ball.getVelocity() < 25.f && object.getJustBounced() != -1 && Globals::simulationOn)
      || (ball.getMidpoint().x < 0 || ball.getMidpoint().x > window.getSize().x || ball.getMidpoint().y < 0 || ball.getMidpoint().y > window.getSize().y)
    ) {
      Globals::simulationOn = false;
      ball.setPosition(ballOrigin - 0.5f * ball.getGlobalBounds().getSize());
      ball.setMidpoint(ballOrigin);
      ball.setVelocity(sf::Vector2f());
      level.getRunButton().setPosition(level.getRunButton().getPosition() + sf::Vector2f(0, 1e3));
    }
  }

  level.getTilemap().drawPropsWalls(wallsTexture, propsTexture, sf::Vector2i(128, 128));
  
  window.draw(ball);

  level.getTilemap().drawPipes(pipesTexture, sf::Vector2i(128, 128));

  // Display the user's objects
  for (UserObjects::EditableObject* obj : editableObjects.getObjects()) {
    obj->draw();
    if (obj->hasBouncyObject()) {
      checkCollision(obj->getBouncyObject(), ball);
      if (
        (ball.getVelocity() < 25.f && obj->getBouncyObject().getJustBounced() != -1 && Globals::simulationOn)
        || (ball.getMidpoint().x < 0 || ball.getMidpoint().x > window.getSize().x || ball.getMidpoint().y < 0 || ball.getMidpoint().y > window.getSize().y)
      ) {
        Globals::simulationOn = false;
        ball.setPosition(ballOrigin - 0.5f * ball.getGlobalBounds().getSize());
        ball.setMidpoint(ballOrigin);
        ball.setVelocity(sf::Vector2f());
        level.getRunButton().setPosition(level.getRunButton().getPosition() + sf::Vector2f(0, 1e3));
      }
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
    level.getScoreLabel().setScore(level.getScoreLabel().getScore() + bag->getValue());
    
    threads.emplace_back(std::bind(&MoneyBag::fall, bag, ball, window.getSize().y));
    threads.back().detach();
    std::clog << "The ball hit a bag!" << std::endl;
  }

  // Draw the UI
  inventory.draw();
  level.getScoreLabel().draw();
  level.getRunButton().draw();

  if (editing != nullptr) editGUI.draw();

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

  // Set the ball origin
  ballOrigin = {1.5f * unitSize, 1.5f * unitSize};

  // Init the editGUI
  editGUI = UIElements::EditGUI(sf::Vector2f(2.5f * unitSize, 14.f * unitSize), sf::Vector2f(.8f * unitSize, .8f * unitSize));

  sf::Texture ballTexture;
  loadTexture("sprites/ball.png", ballTexture);

  ballTexture.setSmooth(true);

  PhysicsObjects::Ball ball{ballTexture, ballOrigin, 0.1f, 0.25f * unitSize};

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

  sf::Clock dt_clock;

  while (window.isOpen()) {
    float deltaTime = dt_clock.restart().asSeconds();
    loop(window, ball, tmpLevel, inventory, deltaTime);
  }

  return 0;
}