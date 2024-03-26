/**
 * @file ui_conf.cpp
 * @author Patrick Vreeburg
 * @brief Handles the UI elements and the user config
 * @version 0.1
 * @date 2024-03-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "../include/ui_conf.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

UIElements::Global global;

void UIElements::Global::initFont() {
  std::filesystem::path fontPath = RESOURCES_PATH;
  fontPath += "font/Roboto.ttf";
  if (!this->font.loadFromFile(fontPath)) {
    throw std::runtime_error("Failed to load the font.");
  }
}

bool UIElements::Button::intersect(sf::Vector2i pos) {
  return (pos.x >= this->position.x && pos.x <= this->position.x + this->size.x && pos.y >= this->position.y && pos.y <= this->position.y + this->size.y);
}

void UIElements::Button::draw(sf::RenderWindow& window) {
  sf::Sprite outerSprite(this->outer);

  sf::Text text(global.getFont(), this->text);

  outerSprite.setScale(sf::Vector2f(this->size.x / static_cast<float>(this->outer.getSize().x), this->size.y / static_cast<float>(this->outer.getSize().y)));
  outerSprite.setPosition(this->position);
  window.draw(outerSprite);

  if (!this->text.empty()) {
    text.setCharacterSize((static_cast<sf::Vector2f>(this->size) * this->textSize).x / this->text.length());
    text.setPosition(this->position + 0.5f * (1.f - this->textSize) * static_cast<sf::Vector2f>(this->size));
    window.draw(text);
  }
}

void UIElements::Button::onClick() {
  std::clog << "Button click" << std::endl;
}

void UIElements::InventoryButton::draw(sf::RenderWindow& window) {
  sf::Sprite outerSprite(this->getOuterTexture());
  
  sf::Texture innerTexture;

  if (!innerTexture.loadFromFile(this->innerPath)) {
    throw std::runtime_error("Failed to load the item texture.");
  }

  sf::Sprite innerSprite(innerTexture);

  outerSprite.setScale(sf::Vector2f(this->getSize().x / static_cast<float>(this->getOuterTexture().getSize().x), this->getSize().y / static_cast<float>(this->getOuterTexture().getSize().y)));
  outerSprite.setPosition(this->getPosition());
  window.draw(outerSprite);

  if (innerTexture.getSize().x != 0 && innerTexture.getSize().y != 0) {
    sf::Vector2f innerSize = this->itemSize * static_cast<sf::Vector2f>(this->getSize());
    sf::Vector2f factors;
    if (lockAspect) {
      unsigned short smallestSide = (this->getSize().x < this->getSize().y) ? this->getSize().x : this->getSize().y;
      smallestSide *= this->itemSize;
      const float yToXAspect = innerTexture.getSize().x / static_cast<float>(innerTexture.getSize().y);
      factors = sf::Vector2f((smallestSide * yToXAspect) / static_cast<float>(innerTexture.getSize().x), smallestSide / static_cast<float>(innerTexture.getSize().y));
    } else {
      factors = sf::Vector2f(innerSize.x / static_cast<float>(innerTexture.getSize().x), innerSize.y / static_cast<float>(innerTexture.getSize().y));
    }
    innerSprite.setScale(factors);
    innerSprite.setPosition(this->getPosition() + 0.5f * (1.f - this->itemSize) * static_cast<sf::Vector2f>(this->getSize()));
    window.draw(innerSprite);
  }

  if (this->count > -1) {
    std::string countStr = std::to_string(this->count);
    sf::Text countText(global.getFont(), countStr);
    countText.setCharacterSize(0.25 * this->getSize().x);
    sf::Vector2f sizeF = static_cast<sf::Vector2f>(this->getSize());

    sf::Vector2f bottomRightPadded = this->getPosition() + sizeF - 0.1f * sizeF;
    bottomRightPadded.y -= 0.05f * sizeF.y;
    countText.setPosition(bottomRightPadded - sf::Vector2f(countText.getLocalBounds().width, countText.getLocalBounds().height));

    countText.setFillColor(sf::Color(255, 255, 255));

    window.draw(countText);
  }
}

void UIElements::InventoryButton::onClick() {
  std::clog << this->count << std::endl;
}

UIElements::Inventory::Inventory(const std::vector<uint8_t>& newItems, const std::vector<int16_t>& newCounts, sf::Texture& buttonOuter)
: items(newItems), counts(newCounts), outerTexture(buttonOuter) {
  // Initialise the font
  global.initFont();

  for (unsigned short i = 0; i < newItems.size(); ++i) {
    uint8_t item = newItems[i];
    int16_t count = newCounts[i];
    this->buttons.push_back(new UIElements::InventoryButton(buttonOuter, sf::Vector2f(), sf::Vector2u(0,0), this->itemIdToPath[item], count, true));
  }
}

UIElements::Inventory::~Inventory() {
  for (UIElements::InventoryButton* button : this->buttons) {
    delete button;
  }
}

void UIElements::Inventory::setItems(std::vector<uint8_t>& newItems) {
  this->items = newItems;

  for (UIElements::InventoryButton* button : this->buttons) {
    delete button;
  }

  for (unsigned short i = 0; i < newItems.size(); ++i) {
    uint8_t item = newItems[i];
    int16_t count = this->counts[i];
    this->buttons.push_back(new UIElements::InventoryButton(this->outerTexture, sf::Vector2f(), sf::Vector2u(0,0), this->itemIdToPath[item], count, true));
  }
}

void UIElements::Inventory::setCounts(std::vector<int16_t>& newCounts) {
  if (newCounts.size() != this->items.size()) {
    throw std::runtime_error("The new counts list has a size that is not equal to the item list size.");
  }

  this->counts = newCounts;

  for (UIElements::InventoryButton* button : this->buttons) {
    delete button;
  }

  for (unsigned short i = 0; i < this->items.size(); ++i) {
    uint8_t item = this->items[i];
    int16_t count = newCounts[i];
    this->buttons.push_back(new UIElements::InventoryButton(this->outerTexture, sf::Vector2f(), sf::Vector2u(0,0), this->itemIdToPath[item], count, true));
  }
}

void UIElements::Inventory::draw(sf::RenderWindow& window, const float unitSize) {
  // Update the position of the buttons to display the items properly in the center of the screen.
  // If the number of items is odd, make one item the middle one and offset the rest accordingly.
  // If the number of items is even, set the middle to the middle of the screen and offset the items accordingly.
  sf::Vector2u windowSize = window.getSize();

  sf::Vector2f middle = sf::Vector2f(windowSize.x / 2.f, windowSize.y * 0.9f);
  float offset = 0.f;
  const float PADDING = 0.3f * unitSize;

  const sf::Vector2u SIZE = sf::Vector2u(1.5f * unitSize, 1.5f * unitSize);

  std::vector<UIElements::InventoryButton*>::iterator left, right;
  if (this->items.size() % 2 == 0) {

    left = this->buttons.begin() + (this->buttons.size() / 2) - 1;
    right = left + 1;

    offset = 0.5f * PADDING + 0.5f * SIZE.x;

  } else {
    
    std::vector<UIElements::InventoryButton*>::iterator middleButton = this->buttons.begin() + floor(static_cast<float>(this->buttons.size()) / 2.f);
    (*middleButton)->setSize(SIZE);
    (*middleButton)->setPosition(middle - 0.5f * static_cast<sf::Vector2f>(SIZE));
    (*middleButton)->draw(window);

    offset = SIZE.x + PADDING;

    left = middleButton - 1;
    right = middleButton + 1;

  }

  // Place the buttons
  uint8_t count = 0;
  for (;left >= this->buttons.begin() && right != this->buttons.end(); left--, right++, count++) {
    (*left)->setSize(SIZE);
    (*left)->setPosition(middle - sf::Vector2f(offset + count * SIZE.x + count * PADDING, 0) - 0.5f * static_cast<sf::Vector2f>(SIZE));
    (*left)->draw(window);

    (*right)->setSize(SIZE);
    (*right)->setPosition(middle + sf::Vector2f(offset + count * SIZE.x + count * PADDING, 0) - 0.5f * static_cast<sf::Vector2f>(SIZE));
    (*right)->draw(window);
  }
}