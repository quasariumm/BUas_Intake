/**
 * @file build.cpp
 * @author Patrick Vreeburg
 * @brief Handles the building/placing of items
 * @version 0.1
 * @date 2024-03-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../include/build.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <filesystem>
#include <stdexcept>
#include <string>

#define Key sf::Keyboard::Key

UserObjects::GhostObject building{sf::Vector2f(), RESOURCES_PATH};

void UserObjects::initBuilding(const sf::Vector2f newSize, const std::filesystem::path texturePath) {
  building = UserObjects::GhostObject{newSize, texturePath};
}

UserObjects::GhostObject* UserObjects::getBuilding() {
  return &building;
}

void UserObjects::GhostObject::loop(sf::RenderWindow& window, const bool rotateKeyPressed, const std::string modifier, const float unitSize) {

  if (rotateKeyPressed) {
    float rotateAngle = 0;
    if (sf::Keyboard::isKeyPressed(Key::R)) {
      rotateAngle = -1;
    } else if (sf::Keyboard::isKeyPressed(Key::T)) {
      rotateAngle = 1;
    }
    if (modifier == "Shift") {
      rotateAngle /= 5;
    } else if (modifier == "Ctrl") {
      rotateAngle *= 3;
    }
    this->rotation += rotateAngle;
  }

  // Place the points to match the orientation and position
  sf::Vector2i mousePos = sf::Mouse::getPosition(window);
  
  sf::Texture ghostTexture;
  if (!ghostTexture.loadFromFile(this->texturePath)) {
    throw std::runtime_error("Couldn't load the ghost sprite texture.");
  }
  ghostTexture.setSmooth(true);
  sf::Vector2f ghostTextureSize = static_cast<sf::Vector2f>(ghostTexture.getSize());

  sf::Sprite ghostSprite(ghostTexture);
  ghostSprite.setOrigin(0.5f * ghostTextureSize);
  ghostSprite.setRotation(sf::degrees(this->rotation));
  ghostSprite.setScale(sf::Vector2f((this->size.x * unitSize) / ghostTextureSize.x, (this->size.y * unitSize) / ghostTextureSize.y));
  ghostSprite.setPosition(static_cast<sf::Vector2f>(mousePos) + 0.5f * this->size);

  // Set the ghost sprite to be transparent
  ghostSprite.setColor(sf::Color(255, 255, 255, 128));

  window.draw(ghostSprite);

}
