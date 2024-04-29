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

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>

#include "../include/physics.hpp"
#include "../include/level.hpp"
#include "../include/ui.hpp"
#include "../include/build.hpp"
#include "../include/globals.hpp"
#include "../include/dialogue.hpp"
#include "../include/config.hpp"
#include "../include/main_menu.hpp"
#include "../include/audio.hpp"
#include "SFML/Audio/Sound.hpp"

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

sf::Texture backgroundTexture;

// Background sprite
sf::Sprite backgroundSprite(backgroundTexture);

// Things that the player can click on
UserObjects::EditableObjectList editableObjects;

UserObjects::EditableObject* editing = nullptr;
UIElements::EditGUI editGUI{sf::Vector2f(), sf::Vector2f()};
UIElements::BuildGUI buildGUI{sf::Vector2f(), sf::Vector2f()};

// Keyboard-specific variables
bool rotate;

// Score
long score;

// User config
Config playerConf;

// Main menu
MainMenu* mainMenu = nullptr;

// Current rendered level
short renderedLevel = -2;
// This variable is necessary due to the fact that someone can finish the level
// before the dialogue is finished.
bool levelCompleted = false;

// Ball bounce buffers
sf::SoundBuffer bouncePadBuffer, bounceWallBuffer;

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
    if (object.getCOR() == 0.95f) {
      // Bounce pad
      Globals::threads.emplace_back(playSound, bouncePadBuffer);
      Globals::threads.back().detach();
    } else {
      // Walls
      Globals::threads.emplace_back(playSound, bounceWallBuffer);
      Globals::threads.back().detach();
    }
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
    if (obj->intersect(sf::Mouse::getPosition(*Globals::window)) && !Globals::simulationOn) {
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
  if (sf::Keyboard::isKeyPressed(playerConf.getKeybind("ROTATE_CCW")) || sf::Keyboard::isKeyPressed(playerConf.getKeybind("ROTATE_CW"))) {
    rotate = true;
  } else if (sf::Keyboard::isKeyPressed(playerConf.getKeybind("MOVE")) && editing != nullptr) {
    // Delete the object and enter building mode
    
    uint8_t itemId = editing->getItemId();

    UserObjects::initBuilding(editing->getSize(), editing->getTexturePath(), itemId, editing->getRotation());

    // Don't touch this line. If the line beneath this line is placed at the end of the if-statement, it breaks.
    auto tmpEditing = editing;
    
    editing = nullptr;

    std::vector<UserObjects::EditableObject*>::iterator it;
    if ((it = std::find(editableObjects.getObjects().begin(), editableObjects.getObjects().end(), tmpEditing)) != editableObjects.getObjects().end()) {
      delete *it;
      editableObjects.getObjects().erase(it);
    }

    inventory.changeCount(itemId, 1);

  } else if (sf::Keyboard::isKeyPressed(playerConf.getKeybind("DELETE")) && editing != nullptr) {
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

  } else if (sf::Keyboard::isKeyPressed(playerConf.getKeybind("CANCEL"))) {
    // Cancel building or editing
    if (UserObjects::getBuilding()->getSize().length() != 0) UserObjects::clearBuilding();

    if (editing != nullptr) editing = nullptr;
  }
}

void keyReleasedEvent() {
  if (!sf::Keyboard::isKeyPressed(playerConf.getKeybind("ROTATE_CCW")) && !sf::Keyboard::isKeyPressed(playerConf.getKeybind("ROTATE_CW"))) {
    rotate = false;
  }
}

// Loop

void loop(sf::RenderWindow& window, PhysicsObjects::Ball& ball, Level& level, UIElements::Inventory& inventory, float deltaTime, Dialogue& dialogue, TextBubble& textBubble, UIElements::TextLabel& dialogueTextLabel) {

  if (!Globals::gameStarted) {
    mainMenu->loop_draw();
    return;
  }

  // Check if the level is completed and that the dialogue is finished
  // If so, increment the Globals::CurrentLevel
  if (levelCompleted && !Globals::dialoguePlaying) {
    ++Globals::currentLevel;
    // Clear the editableObjects
    for (UserObjects::EditableObject* object : editableObjects.getObjects()) {
      delete object;
    }
    editableObjects.getObjects().clear();
    // Clear the BouncyObjects
    level.getBouncyObjects().getList().clear();
    levelCompleted = false;
  }

  // Check if another level needs to be loaded
  if (Globals::currentLevel != renderedLevel) {
    if (Globals::currentLevel == -1) {
      // Intro
      renderedLevel = -1;
      dialogue.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("dialogues/intro.qd"));
      Globals::threads.emplace_back(std::bind(&Dialogue::play, &dialogue, &textBubble, &dialogueTextLabel));
      Globals::threads.back().detach();
      // Set the keys in the Edit and Build GUI to the configured keybinds
      editGUI.setCorrectText(playerConf);
      buildGUI.setCorrectText(playerConf);
    } else if (Globals::currentLevel == 3) {
      // Credits -> don't load a new level and dialogue
      renderedLevel = 3;
    } else {
      level.setLevelFilePath(std::filesystem::path(RESOURCES_PATH).append("levels/level" + std::to_string(Globals::currentLevel) + ".ql"));
      level.initLevel();
      dialogue.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("dialogues/level" + std::to_string(Globals::currentLevel) + ".qd"));
      Globals::threads.emplace_back(std::bind(&Dialogue::play, &dialogue, &textBubble, &dialogueTextLabel));
      Globals::threads.back().detach();
      renderedLevel = Globals::currentLevel;
    }
  }

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

  // Shows the credits if needed
  if (renderedLevel == 3) {

    sf::RectangleShape background(static_cast<sf::Vector2f>(window.getSize()));
    background.setFillColor(sf::Color(14, 19, 20));
    window.draw(background);

    UIElements::TextLabel credits{
      "Credits:\n\n\n\n\nPatrick Vreeburg (Quasarium)\n\nExternal resources used:\nsvgrepo.com\nGoogle Fonts\nkbs.im (Keyboard sounds)\nsamplefocus.com (Bounce sample)",
      sf::Vector2f(0.5f * window.getSize().x, 7.f * Globals::unitSize), sf::Vector2f(window.getSize().x, 6.f * Globals::unitSize), std::filesystem::path(RESOURCES_PATH).append("sprites/blank.png"),
      sf::Color::White, Globals::mainFont, static_cast<int>(0.5f * Globals::unitSize)
    };
    credits.draw();

    sf::Texture quasarLogoTexture;
    if (!quasarLogoTexture.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("sprites/quasarLogo.png"))) {
      throw std::runtime_error("Couldn't load the Quasar logo sprite.");
    }
    sf::Sprite quasarLogo(quasarLogoTexture);
    quasarLogo.setScale(sf::Vector2f(1.5f * unitSize / quasarLogoTexture.getSize().x, 1.5f * unitSize / quasarLogoTexture.getSize().y));
    quasarLogo.setPosition(sf::Vector2f(4.5f * unitSize, 4.5f * unitSize));

    window.draw(quasarLogo);

    UIElements::TextLabel madeAs{
      "This was made as the intake assignment for", sf::Vector2f(0.5f * window.getSize().x, 13.f * unitSize),
      sf::Vector2f(window.getSize().x, 2.f * unitSize), std::filesystem::path(RESOURCES_PATH).append("sprites/blank.png"),
      sf::Color::White, Globals::mainFont, static_cast<int>(0.5f * unitSize)
    };
    madeAs.draw();

    sf::Texture BUasLogoTexture;
    if (!BUasLogoTexture.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("sprites/BUasLogo.png"))) {
      throw std::runtime_error("Couldn't load the Quasar logo sprite.");
    }
    sf::Sprite BUasLogo(BUasLogoTexture);
    BUasLogo.setScale(sf::Vector2f(4.5f * unitSize / BUasLogoTexture.getSize().x, 1.5f * unitSize / BUasLogoTexture.getSize().y));
    BUasLogo.setPosition(sf::Vector2f(0.5f * window.getSize().x - 2.25f * unitSize, 14.f * unitSize));

    window.draw(BUasLogo);

    window.display();
    return;
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

      // Check if the needed money bags have been collected
      // If so, load the next level when the dialogue is finished
      if (level.getScoreLabel().getScore() == level.getNeededScore()) {
        levelCompleted = true;
      } else {
        level.resetMoneyBagPositions();
      }
    }
  }

  if (Globals::currentLevel >= 0){
    window.draw(backgroundSprite);

    level.getTilemap().drawPropsWalls(wallsTexture, propsTexture, sf::Vector2i(128, 128));
  
    window.draw(ball);
  
    level.getTilemap().drawPipes(pipesTexture, sf::Vector2i(128, 128));
  }

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

        // Check if the needed money bags have been collected
        // If so, load the next level when the dialogue is finished
        if (level.getScoreLabel().getScore() == level.getNeededScore()) {
          levelCompleted = true;
        } else {
          level.resetMoneyBagPositions();
        }
      }
    }
    if (obj->hasBooster()) {
      short collSide = obj->getBooster().checkBallCollision(ball);
      if (!obj->getBooster().getJustBoosted() && collSide != NULL_VALUE) {
        obj->getBooster().boost(ball);
      } else if (obj->getBooster().getJustBoosted() && collSide == NULL_VALUE) {
        obj->getBooster().setJustBoosted(false);
      }
    }
  }

  if (Globals::currentLevel >= 0) {
    // Display the money bags and check if the ball hits the bag.
    // If the ball hits the bag, increase the score and make it fall.
    for (MoneyBag* bag : level.getMoneyBags()) {
      bag->draw();
      if (!bag->intersect(ball) || bag->getCollected()) {
        continue;
      }
      bag->setCollected(true);
      level.getScoreLabel().setScore(level.getScoreLabel().getScore() + bag->getValue());
      
      Globals::threads.emplace_back(std::bind(&MoneyBag::fall, bag, ball, window.getSize().y));
      Globals::threads.back().detach();
    }

    // Draw the UI
    inventory.draw();
    level.getScoreLabel().draw();
    if (!levelCompleted)
      level.getRunButton().draw();
  }

  dialogueTextLabel.draw();
  textBubble.draw();

  if (editing != nullptr) {
    editGUI.drawBackground();
    editGUI.draw();
  }
  if (UserObjects::getBuilding()->getSize().length() != 0) {
    buildGUI.drawBackground();
    buildGUI.draw();
  }

  // Determine if the player is building something. If so, call the ghost object's loop()
  if (UserObjects::getBuilding()->getSize().length() != 0) {
    UserObjects::getBuilding()->loop(rotate, playerConf);
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
  unitSize = std::round(unitSize);
  std::cout << "Unit size is: " << unitSize <<std::endl;
  windowSize = window.getSize();

  // Set the window-related global variables and initialise the global font
  Globals::unitSize = unitSize;
  Globals::window = &window;
  Globals::initFont();

  // Set the ball origin
  ballOrigin = {2.f * unitSize, 0.0f * unitSize};

  // Init the editGUI
  editGUI = UIElements::EditGUI(sf::Vector2f(2.f * Globals::unitSize, 14.f * Globals::unitSize), sf::Vector2f(4.f * Globals::unitSize, 4.f * Globals::unitSize));
  buildGUI = UIElements::BuildGUI(sf::Vector2f(2.f * Globals::unitSize, 14.f * Globals::unitSize), sf::Vector2f(4.f * Globals::unitSize, 4.f * Globals::unitSize));

  sf::Texture ballTexture;
  loadTexture("sprites/ball.png", ballTexture);

  ballTexture.setSmooth(true);

  PhysicsObjects::Ball ball{ballTexture, ballOrigin, 0.1f, 0.25f * unitSize};

  // Initialise the button outer texture and the items
  sf::Texture itemOuter;
  loadTexture("sprites/itemBackground.png", itemOuter);

  // Create the inventory
  UIElements::Inventory inventory{{0}, {0}, itemOuter};

  // Load all of the texture atlases
  loadTexture("sprites/tilemapWall.png", wallsTexture);
  loadTexture("sprites/tilemapProps.png", propsTexture);
  loadTexture("sprites/tilemapPipes.png", pipesTexture);

  loadTexture("sprites/background.png", backgroundTexture);

  // Configure the background sprite
  backgroundSprite.setTexture(backgroundTexture, true);

  backgroundSprite.setOrigin(0.5f * static_cast<sf::Vector2f>(backgroundTexture.getSize()));
  backgroundSprite.setPosition(0.5f * static_cast<sf::Vector2f>(window.getSize()));
  backgroundSprite.setScale(sf::Vector2f(window.getSize().x / static_cast<float>(backgroundTexture.getSize().x), window.getSize().y / static_cast<float>(backgroundTexture.getSize().y)));

  // Initialise the first level, just temporary
  std::filesystem::path tmppath = RESOURCES_PATH;
  tmppath += "levels/level0.ql";

  Level level{tmppath, wallsTexture, propsTexture, pipesTexture, inventory};

  // Initiate the dialogue text elements
  TextBubble textBubble(std::string(48, ' '));
  UIElements::TextLabel dialogueTextLabel = UIElements::TextLabel{
    "",
    sf::Vector2f(0.5f * Globals::window->getSize().x, 0.5f * Globals::window->getSize().x - Globals::unitSize),
    sf::Vector2f(10.f * Globals::unitSize, 4.f * Globals::unitSize),
    std::filesystem::path(RESOURCES_PATH).append("sprites/blank.png"),
    sf::Color::White, Globals::mainFont, static_cast<int>(0.6f * Globals::unitSize)
  };
  Dialogue dialogue;
  dialogue.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("dialogues/intro.qd"));

  // Load the config
  playerConf.loadFromFile(std::filesystem::path(DATA_PATH).append("playerConfig.qconf"));

  // Initialise the main menu
  mainMenu = new MainMenu(&level, &playerConf);

  // Initialise the ball bounce sounds
  if (!bouncePadBuffer.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("audio/bounce_pad.wav"))) {
    throw std::runtime_error("Couldn't load the bounce pad bounce sound.");
  }
  if (!bounceWallBuffer.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("audio/bounce_wall.wav"))) {
    throw std::runtime_error("Couldn't load the wall bounce sound.");
  }

  // Delta time clock
  sf::Clock dt_clock;

  while (window.isOpen()) {
    float deltaTime = dt_clock.restart().asSeconds();
    loop(window, ball, level, inventory, deltaTime, dialogue, textBubble, dialogueTextLabel);
  }

  // Clean main menu pointer
  delete mainMenu;

  return 0;
}