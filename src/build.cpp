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
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <filesystem>
#include <stdexcept>
#include <string>

#include "../include/physics.hpp"
#include "../include/globals.hpp"
#include "../include/ui.hpp"
#include "../include/config.hpp"

#define Key sf::Keyboard::Key

UserObjects::GhostObject building{sf::Vector2f(), RESOURCES_PATH, 0};

void UserObjects::initBuilding(const sf::Vector2f newSize, const std::filesystem::path texturePath, const int8_t itemId, const float rotation) {
  building = UserObjects::GhostObject{newSize, texturePath, itemId, rotation};
}

void UserObjects::clearBuilding() {
  // This about does the job. Might not be the cleanest way to do it, but at least it doesn't draw a 👻 object
  building.setSize(sf::Vector2f());
  building.setRotation(0);
}

UserObjects::GhostObject* UserObjects::getBuilding() {
  return &building;
}

//////////////////////////////////////
// GhostObject
//////////////////////////////////////

void UserObjects::GhostObject::loop(const bool rotateKeyPressed, Config& playerConf) {

  if (rotateKeyPressed) {
    float rotateAngle = 0;
    if (sf::Keyboard::isKeyPressed(playerConf.getKeybind("ROTATE_CCW"))) {
      rotateAngle = -1;
    } else if (sf::Keyboard::isKeyPressed(playerConf.getKeybind("ROTATE_CW"))) {
      rotateAngle = 1;
    }
    if (sf::Keyboard::isKeyPressed(playerConf.getKeybind("ROTATE_SMALL"))) {
      rotateAngle /= 5;
    } else if (sf::Keyboard::isKeyPressed(playerConf.getKeybind("ROTATE_BIG"))) {
      rotateAngle *= 3;
    }
    this->rotation += rotateAngle;
  }

  // Place the points to match the orientation and position
  sf::Vector2i mousePos = sf::Mouse::getPosition(*Globals::window);
  
  sf::Texture ghostTexture;
  if (!ghostTexture.loadFromFile(this->texturePath)) {
    throw std::runtime_error("Couldn't load the ghost sprite texture.");
  }
  ghostTexture.setSmooth(true);
  sf::Vector2f ghostTextureSize = static_cast<sf::Vector2f>(ghostTexture.getSize());

  sf::Sprite ghostSprite(ghostTexture);
  ghostSprite.setOrigin(0.5f * ghostTextureSize);
  ghostSprite.setRotation(sf::degrees(this->rotation));
  ghostSprite.setScale(sf::Vector2f((this->size.x * Globals::unitSize) / ghostTextureSize.x, (this->size.y * Globals::unitSize) / ghostTextureSize.y));
  ghostSprite.setPosition(static_cast<sf::Vector2f>(mousePos) + 0.5f * this->size);

  // Set the ghost sprite to be transparent
  ghostSprite.setColor(sf::Color(255, 255, 255, 200));

  Globals::window->draw(ghostSprite);

}

void UserObjects::GhostObject::place(UserObjects::EditableObjectList& objList, UIElements::Inventory& inventory) {
  // Decrement the count in the inventory
  inventory.changeCount(this->itemID, -1);

  const bool BOUNCY = this->texturePath.filename() == "bouncePad.png";

  const bool BOOSTER = this->texturePath.filename() == "booster.png";

  objList.addObject(new UserObjects::EditableObject(sf::Mouse::getPosition(*Globals::window), this->size, this->texturePath, this->itemID, this->rotation, BOUNCY, (this->texturePath.filename() == "bouncePad.png") ? 0.95f : 0.8f, BOOSTER));
  clearBuilding();
}

//////////////////////////////////////
// EditableObject
//////////////////////////////////////

UserObjects::EditableObject::EditableObject(const sf::Vector2i newPos, const sf::Vector2f newSize, const std::filesystem::path newTexturePath, const int8_t itemId, const float newRotation, const bool bouncy, const float cor, const bool booster) 
: pos(newPos), size(newSize), texturePath(newTexturePath), itemID(itemId), rotation(newRotation), bouncyObject(bouncy), cor(cor), booster(booster) {
  // Load the texture and store it
  if (!this->texture.loadFromFile(newTexturePath)) {
    throw std::runtime_error("Couldn't load the EditableObject's texture.");
  }
  this->texture.setSmooth(true);

  if (bouncy) {
    this->bo = PhysicsObjects::BouncyObject();
    this->bo.setCOR(cor);
    this->bo.setOrientation(sf::Vector2f(1,0).rotatedBy(sf::degrees(90.f - newRotation)));

    // For the points of the BouncyObject I use a RectangleShape and get its points
    sf::RectangleShape rect(newSize * Globals::unitSize);
    rect.setOrigin(0.5f * Globals::unitSize * newSize);
    rect.setRotation(sf::degrees(newRotation));
    rect.setPosition(static_cast<sf::Vector2f>(newPos));
    sf::Transform transform = rect.getTransform();
    // transform.translate(static_cast<sf::Vector2f>(this->pos) + 0.5f * (this->size * unitSize))
    //          .rotate(sf::degrees(this->rotation))
    //          .scale(sf::Vector2f((this->size.x * unitSize) / this->texture.getSize().x, (this->size.y * unitSize) / this->texture.getSize().y));
    this->bo.setPoints({
      transform.transformPoint(rect.getPoint(1)),
      transform.transformPoint(rect.getPoint(2)),
      transform.transformPoint(rect.getPoint(3)),
      transform.transformPoint(rect.getPoint(0))
    });
  }
  if (booster) {
    this->boost = PhysicsObjects::Booster(newPos, newSize, newRotation, 0.5f);
  }
};

bool UserObjects::EditableObject::intersect(const sf::Vector2i pos) {
  // I'm not going to do a very complicated way of knowing if it intersects.
  // I'm just going to get the global bounds and use its contain function
  // Fist, make a temporary RectangleShape to get the global bounds of
  sf::RectangleShape rect(this->size * Globals::unitSize);
  rect.setOrigin(0.5f * Globals::unitSize * this->size);
  rect.setPosition(static_cast<sf::Vector2f>(this->pos));
  rect.setRotation(sf::degrees(this->rotation));
  sf::FloatRect rectBounds = rect.getGlobalBounds();
  return rectBounds.contains(static_cast<sf::Vector2f>(pos));
}

void UserObjects::EditableObject::draw() {
  sf::Sprite objSprite(this->texture);
  objSprite.setOrigin(0.5f * static_cast<sf::Vector2f>(this->texture.getSize()));
  objSprite.setScale(sf::Vector2f((this->size.x * Globals::unitSize) / static_cast<float>(this->texture.getSize().x), (this->size.y * Globals::unitSize) / static_cast<float>(this->texture.getSize().y)));
  // The `+ 0.5f * this->size` can't be added when the object is created, because otherwise when editing its position will be wrong
  objSprite.setPosition(static_cast<sf::Vector2f>(this->pos) + 0.5f * this->size);
  objSprite.setRotation(sf::degrees(this->rotation));

  Globals::window->draw(objSprite);
}

//////////////////////////////////////
// EditableObjectList
//////////////////////////////////////

UserObjects::EditableObjectList::~EditableObjectList() {
  for (UserObjects::EditableObject* pObj : this->editableObjects) {
    delete pObj;
  }
}