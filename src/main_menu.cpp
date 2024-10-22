/**
 * @file main_menu.cpp
 * @author Patrick Vreeburg
 * @brief Handles the main menu and settings menu
 * @version 0.1
 * @date 2024-04-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../include/main_menu.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <stdexcept>
#include <string>

#include "../include/config.hpp"
#include "../include/globals.hpp"
#include "../include/level.hpp"
#include "../include/ui.hpp"

UIElements::Button* keybindEditing = nullptr;

//////////////////////////////////////
// MainMenu
//////////////////////////////////////

MainMenu::MainMenu(Level* _level, Config* _config) : level(_level), config(_config) {
  
  sf::Texture playSettings;
  if (!playSettings.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("sprites/genericButtonBackground.png"))) {
    throw std::runtime_error("Couldn't load the texture for the play and settings buttons.");
  }

  sf::Texture blank;
  if (!blank.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("sprites/blank.png"))) {
    throw std::runtime_error("Couldn't load the blank texture.");
  }

  this->play = UIElements::Button(
    playSettings, sf::Vector2f(0.5f * Globals::window->getSize().x, 0.5f * Globals::window->getSize().y + 2.5f * Globals::unitSize),
    sf::Vector2u(static_cast<unsigned>(8.f * Globals::unitSize), static_cast<unsigned>(2.f * Globals::unitSize)), "   Play   ", sf::Color::White
  );

  this->settings = UIElements::Button(
    playSettings, sf::Vector2f(0.5f * Globals::window->getSize().x, 0.5f * Globals::window->getSize().y + 5.5f * Globals::unitSize),
    sf::Vector2u(static_cast<unsigned>(8.f * Globals::unitSize), static_cast<unsigned>(2.f * Globals::unitSize)), "Settings", sf::Color::White
  );

  this->back = UIElements::Button(
    blank, sf::Vector2f(2.f * Globals::unitSize, 15.7f * Globals::unitSize),
    sf::Vector2u(static_cast<unsigned>(3.f * Globals::unitSize), static_cast<unsigned>(2.f * Globals::unitSize)), "Back", sf::Color::White
  );

  const int NUM_KEYBINDS = 7;
  const std::pair<std::string, std::string> KEYBIND_NAMES[NUM_KEYBINDS] = {
    {"ROTATE_CCW", "Rotate counterclockwise"},
    {"ROTATE_CW", "Rotate clockwise"},
    {"ROTATE_SMALL", "Rotate slower"},
    {"ROTATE_BIG", "Rotate faster"},
    {"MOVE", "Move"},
    {"DELETE", "Delete"},
    {"CANCEL", "Cancel"}
  };

  this->keybindNames = {
    "ROTATE_CCW",
    "ROTATE_CW", 
    "ROTATE_SMALL", 
    "ROTATE_BIG", 
    "MOVE",
    "DELETE",
    "CANCEL"
  };

  sf::Texture buttonBackground;
  if (!buttonBackground.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("sprites/blank.png"))) {
    throw std::runtime_error("Couldn't load the background for the keybind buttons");
  }

  for (short i = 0; i < NUM_KEYBINDS; ++i) {
    auto& keybind = KEYBIND_NAMES[i];
    std::string keybindDesc = sf::Keyboard::getDescription(this->config->getKeybind(keybind.first)).toAnsiString();
    if (keybindDesc.length() == 1) {
      // Made the one letter uppercase
      if (keybindDesc[0] >= 97 && keybindDesc[0] <= 122) {
        // a-z
        keybindDesc = std::string(1, static_cast<char>(keybindDesc[0]-32));
      }
    }

    this->controls.emplace_back(
      UIElements::TextLabel(
        keybind.second, sf::Vector2f(6.f * Globals::unitSize, 3.f * Globals::unitSize + 1.3f * i * Globals::unitSize),
        sf::Vector2f(7.f * Globals::unitSize, Globals::unitSize),
        std::filesystem::path(RESOURCES_PATH).append("sprites/blank.png"),
        sf::Color::White
      ),
      UIElements::Button(
        buttonBackground, sf::Vector2f(12.f * Globals::unitSize, 3.f * Globals::unitSize + 1.3f * i * Globals::unitSize),
        sf::Vector2u(static_cast<unsigned>(4.f * Globals::unitSize), static_cast<unsigned>(Globals::unitSize)),
        keybindDesc,
        sf::Color::White
      )
    );
  }

}

void MainMenu::loop_draw() {

  Globals::window->clear();

  // Draw background
  sf::RectangleShape background(static_cast<sf::Vector2f>(Globals::window->getSize()));
  background.setFillColor(sf::Color(14, 19, 20));

  Globals::window->draw(background);

  if (!this->settingsMenu) {

    sf::Texture titleTexture;
    if (!titleTexture.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("sprites/title.png"))) {
      throw std::runtime_error("Couldn't load the title texture.");
    }
    sf::Sprite title(titleTexture);
  
    title.setOrigin(0.5f * static_cast<sf::Vector2f>(titleTexture.getSize()));
    title.setScale(sf::Vector2f(12.f * Globals::unitSize / titleTexture.getSize().x, 6.f * Globals::unitSize / titleTexture.getSize().y));
    title.setPosition(sf::Vector2f(0.5f * Globals::window->getSize().x, 0.5f * Globals::window->getSize().y - 3.f * Globals::unitSize));
  
    Globals::window->draw(title);
    this->play.draw();
    this->settings.draw();

  } else if (this->settingsMenu) {

    UIElements::TextLabel controlsHeader(
      "Controls", sf::Vector2f(0.5f * Globals::window->getSize().x, 0.75f * Globals::unitSize),
      sf::Vector2f(6.f * Globals::unitSize, 2.f * Globals::unitSize), std::filesystem::path(RESOURCES_PATH).append("sprites/blank.png")
    );
    controlsHeader.draw();

    UIElements::TextLabel controlsIntruction(
      "Click a control and press any key to edit", sf::Vector2f(0.5f * Globals::window->getSize().x, 1.75f * Globals::unitSize),
      sf::Vector2f(10.f * Globals::unitSize, 2.f * Globals::unitSize), std::filesystem::path(RESOURCES_PATH).append("sprites/blank.png"),
      sf::Color(155,155,155)
    );
    controlsIntruction.draw();
    
    // Draw all of the controls
    for (size_t i = 0; i < this->controls.size(); ++i) {
      this->controls[i].first.draw();
      this->controls[i].second.draw();

      // Draw a divider line
      if (i == this->controls.size() - 1) continue;

      sf::RectangleShape line(sf::Vector2f(11.f * Globals::unitSize, 0.075f * Globals::unitSize));
      line.setOrigin(sf::Vector2f(5.5f * Globals::unitSize, 0.0375f * Globals::unitSize));
      line.setPosition(sf::Vector2f(0.5f * Globals::window->getSize().x, 3.7f * Globals::unitSize + 1.3f * i * Globals::unitSize));
      line.setFillColor(sf::Color(155,155,155));

      Globals::window->draw(line);
    }

    this->back.draw();

  }

  sf::Event event;
  while (Globals::window->pollEvent(event)) {
    switch (event.type) {

    case sf::Event::Closed:
      Globals::window->close();
      break;
    
    case sf::Event::MouseButtonPressed:
      if (event.mouseButton.button != sf::Mouse::Button::Left) break;
      // Check button clicks (main screen)
      if (this->play.intersect(sf::Mouse::getPosition(*Globals::window)) && !this->settingsMenu) {
        
        Globals::gameStarted = true;
        Globals::currentLevel = -1;

      }
      if (this->settings.intersect(sf::Mouse::getPosition(*Globals::window)) && !this->settingsMenu)
        this->settingsMenu = true;

      if (this->back.intersect(sf::Mouse::getPosition(*Globals::window)) && this->settingsMenu)
        this->settingsMenu = false;
      
      // Check button clicks (settings)
      for (size_t i = 0; i < this->controls.size(); ++i) {
        if (!this->controls[i].second.intersect(sf::Mouse::getPosition(*Globals::window)) || !this->settingsMenu) {
          continue;
        }
        keybindEditing = &this->controls[i].second;
      }

      break;

    case sf::Event::KeyPressed:
      if (keybindEditing != nullptr) {
        int index = static_cast<int>(std::round((keybindEditing->getPosition().y - 3.f * Globals::unitSize) / (1.3f * Globals::unitSize)));
        this->config->setKeybind(this->keybindNames[index], event.key.scancode);
        
        std::string keybindDesc = sf::Keyboard::getDescription(event.key.scancode).toAnsiString();
        if (keybindDesc.length() == 1) {
          // Made the one letter uppercase
          if (keybindDesc[0] >= 97 && keybindDesc[0] <= 122) {
            // a-z
            keybindDesc = std::string(1, static_cast<char>(keybindDesc[0]-32));
          }
        }

        this->controls[index].second.setText(keybindDesc);
        keybindEditing = nullptr;
      }
      break;
    
    default:
      break;

    }
  }

  Globals::window->display();

}