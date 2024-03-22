#include "../include/ui_conf.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <cmath>
#include <cstdint>
#include <filesystem>
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

bool UIElements::Button::intersect(sf::Vector2f& pos) {
  return (pos.x >= this->position.x && pos.x <= this->position.x + this->size.x && pos.y >= this->position.y && pos.y <= this->position.y + this->size.y);
}

void UIElements::Button::draw(sf::RenderWindow& window) {
  sf::Sprite outerSprite(this->outer);
  
  sf::Texture innerTexture;

  if (!innerTexture.loadFromFile(this->innerPath)) {
    throw std::runtime_error("Failed to load the item texture.");
  }

  sf::Sprite innerSprite(innerTexture);
  sf::Text text(global.getFont(), this->text);

  outerSprite.setScale(sf::Vector2f(this->size.x / static_cast<float>(this->outer.getSize().x), this->size.y / static_cast<float>(this->outer.getSize().y)));
  outerSprite.setPosition(this->position);
  window.draw(outerSprite);

  if (innerTexture.getSize().x != 0 && innerTexture.getSize().y != 0) {
    sf::Vector2f innerSize = this->itemSize * static_cast<sf::Vector2f>(this->size);
    sf::Vector2f factors;
    if (lockAspect) {
      unsigned short smallestSide = (this->size.x < this->size.y) ? this->size.x : this->size.y;
      smallestSide *= this->itemSize;
      const float yToXAspect = innerTexture.getSize().x / static_cast<float>(innerTexture.getSize().y);
      factors = sf::Vector2f((smallestSide * yToXAspect) / static_cast<float>(innerTexture.getSize().x), smallestSide / static_cast<float>(innerTexture.getSize().y));
    } else {
      factors = sf::Vector2f(innerSize.x / static_cast<float>(innerTexture.getSize().x), innerSize.y / static_cast<float>(innerTexture.getSize().y));
    }
    innerSprite.setScale(factors);
    innerSprite.setPosition(this->position + 0.5f * (1.f - this->itemSize) * static_cast<sf::Vector2f>(this->size));
    window.draw(innerSprite);
  }

  if (!this->text.empty()) {
    text.setCharacterSize((static_cast<sf::Vector2f>(this->size) * this->textSize).x / this->text.length());
    text.setPosition(this->position + 0.5f * (1.f - this->textSize) * static_cast<sf::Vector2f>(this->size));
    window.draw(text);
  }

  if (this->count > -1) {
    std::string countStr = std::to_string(this->count);
    sf::Text countText(global.getFont(), countStr);
    text.setCharacterSize(0.1 * this->size.x);
    sf::Vector2f sizeF = static_cast<sf::Vector2f>(this->size);
    sf::Vector2f bottomRightPadded = this->position + sizeF - 0.05f * sizeF;
    text.setPosition(bottomRightPadded - sf::Vector2f(0.1f * sizeF.x * static_cast<float>(countStr.length()), 0));
  }
}

UIElements::Inventory::Inventory(const std::vector<uint8_t>& newItems, const std::vector<int16_t>& newCounts, sf::Texture& buttonOuter)
: items(newItems), counts(newCounts), outerTexture(buttonOuter) {
  for (uint8_t item : newItems) {
    this->buttons.push_back(UIElements::Button(buttonOuter, sf::Vector2f(), sf::Vector2u(), this->itemIdToPath[item]));
  }
}

void UIElements::Inventory::setItems(std::vector<uint8_t>& newItems) {
  this->items = newItems;
  this->buttons = {};
  for (uint8_t item : newItems) {
    this->buttons.push_back(UIElements::Button(this->outerTexture, sf::Vector2f(), sf::Vector2u(), this->itemIdToPath[item]));
  }
}

void UIElements::Inventory::draw(sf::RenderWindow& window, const float unitSize) {
  // Update the position of the buttons to display the items properly in the center of the screen.
  // If the number of items is odd, make one item the middle one and offset the rest accordingly.
  // If the number of items is even, set the middle to the middle of the screen and offset the items accordingly.
  sf::Vector2u windowSize = window.getSize();

  sf::Vector2f middle = sf::Vector2f(windowSize.x / 2.f, windowSize.y * 0.8f);
  float offset;
  const float PADDING = 0.3f * unitSize;

  const sf::Vector2u SIZE = sf::Vector2u(2 * unitSize, 2 * unitSize);

  std::vector<UIElements::Button>::iterator left, right;
  if (this->items.size() % 2 == 0) {

    left = this->buttons.begin() + (this->buttons.size() / 2) - 1;
    right = ++left;

    offset = -0.5f * PADDING;

  } else {
    
    std::vector<UIElements::Button>::iterator middleButton = this->buttons.begin() + floor(static_cast<float>(this->buttons.size()) / 2.f);
    middleButton->setSize(SIZE);
    middleButton->setPosition(middle - 0.5f * static_cast<sf::Vector2f>(SIZE));
    middleButton->draw(window);

    offset = unitSize;

    left = --middleButton;
    right = ++middleButton;

  }

  // Place the buttons
  uint8_t count = 1;
  for (;left >= this->buttons.begin() && right <= this->buttons.end(); left--, right++, count++) {
    left->setSize(SIZE);
    left->setPosition(middle - sf::Vector2f(offset + count * SIZE.x + count * PADDING, 0));
    left->draw(window);

    right->setSize(SIZE);
    right->setPosition(middle + sf::Vector2f(offset + count * SIZE.x + count * PADDING, 0));
    right->draw(window);
  }
}