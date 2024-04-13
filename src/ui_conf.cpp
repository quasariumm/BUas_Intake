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
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../include/build.hpp"
#include "../include/globals.hpp"

sf::Texture tmpTexture;

bool UIElements::Button::intersect(const sf::Vector2i pos) {
  return (pos.x >= this->position.x && pos.x <= this->position.x + this->size.x && pos.y >= this->position.y && pos.y <= this->position.y + this->size.y);
}

void UIElements::Button::draw() {
  sf::Sprite outerSprite(this->outer);

  sf::Text text(Globals::mainFont, this->text);

  outerSprite.setScale(sf::Vector2f(this->size.x / static_cast<float>(this->outer.getSize().x), this->size.y / static_cast<float>(this->outer.getSize().y)));
  outerSprite.setPosition(this->position);
  Globals::window->draw(outerSprite);

  if (!this->text.empty()) {
    text.setCharacterSize((static_cast<sf::Vector2f>(this->size) * this->textSize).x / this->text.length());
    text.setPosition(this->position + 0.5f * (1.f - this->textSize) * static_cast<sf::Vector2f>(this->size));
    Globals::window->draw(text);
  }
}

void UIElements::Button::onClick() {
  std::clog << "Button click" << std::endl;
}

void UIElements::RunButton::onClick() {
  // Set the physics of the ball 'on'
  Globals::simulationOn = true;
  // Make the button 'disappear'
  this->setPosition(this->getPosition() + sf::Vector2f(0, -1e3));
}

void UIElements::InventoryButton::draw() {
  sf::Sprite outerSprite(this->getOuterTexture());
  
  sf::Texture innerTexture;

  if (!innerTexture.loadFromFile(this->innerPath)) {
    throw std::runtime_error("Failed to load the item texture.");
  }

  sf::Sprite innerSprite(innerTexture);

  outerSprite.setScale(sf::Vector2f(this->getSize().x / static_cast<float>(this->getOuterTexture().getSize().x), this->getSize().y / static_cast<float>(this->getOuterTexture().getSize().y)));
  outerSprite.setPosition(this->getPosition());
  Globals::window->draw(outerSprite);

  if (innerTexture.getSize().x != 0 && innerTexture.getSize().y != 0) {
    sf::Vector2f innerSize = this->itemSize * static_cast<sf::Vector2f>(this->getSize());
    sf::Vector2f factors;
    if (lockAspect) {
      float smallestSide = (this->getSize().x < this->getSize().y) ? this->getSize().x : this->getSize().y;
      smallestSide *= this->itemSize * (innerSprite.getTexture().getSize().y / static_cast<float>(innerSprite.getTexture().getSize().x));
      const float yToXAspect = innerTexture.getSize().x / static_cast<float>(innerTexture.getSize().y);
      factors = sf::Vector2f((smallestSide * yToXAspect) / static_cast<float>(innerTexture.getSize().x), smallestSide / static_cast<float>(innerTexture.getSize().y));
    } else {
      factors = sf::Vector2f(innerSize.x / static_cast<float>(innerTexture.getSize().x), innerSize.y / static_cast<float>(innerTexture.getSize().y));
    }
    innerSprite.setScale(factors);
    sf::Vector2f basePos = this->getPosition() + 0.5f * (1.f - this->itemSize) * static_cast<sf::Vector2f>(this->getSize());
    if (innerTexture.getSize().x != innerTexture.getSize().y) {
      innerSprite.setPosition(basePos + sf::Vector2f(0, 0.5f * (innerSize.y - (innerTexture.getSize().y * innerSprite.getScale().y))));
    } else {
      innerSprite.setPosition(basePos);
    }
    Globals::window->draw(innerSprite);
  }

  if (this->count > -1) {
    std::string countStr = std::to_string(this->count);
    sf::Text countText(Globals::mainFont, countStr);
    countText.setCharacterSize(0.25 * this->getSize().x);
    sf::Vector2f sizeF = static_cast<sf::Vector2f>(this->getSize());

    sf::Vector2f bottomRightPadded = this->getPosition() + sizeF - 0.1f * sizeF;
    bottomRightPadded.y -= 0.05f * sizeF.y;
    countText.setPosition(bottomRightPadded - sf::Vector2f(countText.getLocalBounds().width, countText.getLocalBounds().height));

    countText.setFillColor(sf::Color(255, 255, 255));

    Globals::window->draw(countText);
  }
}

void UIElements::InventoryButton::onClick() {
  UserObjects::initBuilding(this->innerSize, this->innerPath);
}

UIElements::Inventory::Inventory(const std::vector<uint8_t>& newItems, const std::vector<int16_t>& newCounts, sf::Texture& buttonOuter)
: items(newItems), counts(newCounts), outerTexture(buttonOuter) {
  for (unsigned short i = 0; i < newItems.size(); ++i) {
    uint8_t item = newItems[i];
    int16_t count = newCounts[i];
    this->buttons.push_back(new UIElements::InventoryButton(buttonOuter, sf::Vector2f(), sf::Vector2u(0,0), this->itemIdToPath[item], this->itemIdToSize[item], count, true));
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
    this->buttons.push_back(new UIElements::InventoryButton(this->outerTexture, sf::Vector2f(), sf::Vector2u(0,0), this->itemIdToPath[item], this->itemIdToSize[item], count, true));
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
    this->buttons.push_back(new UIElements::InventoryButton(this->outerTexture, sf::Vector2f(), sf::Vector2u(0,0), this->itemIdToPath[item], this->itemIdToSize[item], count, true));
  }
}

void UIElements::Inventory::draw() {
  // Update the position of the buttons to display the items properly in the center of the screen.
  // If the number of items is odd, make one item the middle one and offset the rest accordingly.
  // If the number of items is even, set the middle to the middle of the screen and offset the items accordingly.
  sf::Vector2u windowSize = Globals::window->getSize();

  sf::Vector2f middle = sf::Vector2f(windowSize.x / 2.f, windowSize.y * 0.9f);
  float offset = 0.f;
  const float PADDING = 0.3f * Globals::unitSize;

  const sf::Vector2u SIZE = sf::Vector2u(1.5f * Globals::unitSize, 1.5f * Globals::unitSize);

  std::vector<UIElements::InventoryButton*>::iterator left, right;
  if (this->items.size() % 2 == 0) {

    left = this->buttons.begin() + (this->buttons.size() / 2) - 1;
    right = left + 1;

    offset = 0.5f * PADDING + 0.5f * SIZE.x;

  } else {
    
    std::vector<UIElements::InventoryButton*>::iterator middleButton = this->buttons.begin() + floor(static_cast<float>(this->buttons.size()) / 2.f);
    (*middleButton)->setSize(SIZE);
    (*middleButton)->setPosition(middle - 0.5f * static_cast<sf::Vector2f>(SIZE));
    (*middleButton)->draw();

    offset = SIZE.x + PADDING;

    left = middleButton - 1;
    right = middleButton + 1;

  }

  // Place the buttons
  uint8_t count = 0;
  for (;left >= this->buttons.begin() && right != this->buttons.end(); left--, right++, count++) {
    (*left)->setSize(SIZE);
    (*left)->setPosition(middle - sf::Vector2f(offset + count * SIZE.x + count * PADDING, 0) - 0.5f * static_cast<sf::Vector2f>(SIZE));
    (*left)->draw();

    (*right)->setSize(SIZE);
    (*right)->setPosition(middle + sf::Vector2f(offset + count * SIZE.x + count * PADDING, 0) - 0.5f * static_cast<sf::Vector2f>(SIZE));
    (*right)->draw();
  }
}

UIElements::TextLabel::TextLabel() : Text(Globals::mainFont), Sprite(tmpTexture) {};

UIElements::TextLabel::TextLabel(const std::wstring newText, const sf::Vector2f& newPos, const sf::Vector2f& newSize, const std::filesystem::path backgroundPath, const sf::Color& textColor)
 : text(newText), pos(newPos), size(newSize), Text(Globals::mainFont, newText), Sprite(tmpTexture) {
  if (!this->background.loadFromFile(backgroundPath)) {
    throw std::runtime_error("Couldn't load the background of a TextLabel.");
  }
  this->setTexture(this->background, true);
  std::clog << ((sf::Sprite*)this)->getLocalBounds().getSize().x << "," << ((sf::Sprite*)this)->getLocalBounds().getSize().y << std::endl;

  const float TEXT_SIZE = 0.7f; // Relative to the background
  this->setCharacterSize(static_cast<int>(TEXT_SIZE * newSize.y));

  this->setFillColor(textColor);
}

void UIElements::TextLabel::setText(const std::wstring newString) {
  this->setString(newString);
  this->text = newString;
}

void UIElements::TextLabel::draw() {
  // Set the right size and position
  sf::Sprite* sprite = static_cast<sf::Sprite*>(this);
  sf::Text* text = static_cast<sf::Text*>(this);

  const float TEXT_SIZE = 0.7f; // Relative to the background

  sprite->setTexture(this->background, true);
  
  sprite->setOrigin(sf::Vector2f(0.5f * sprite->getLocalBounds().getSize().x, 0));
  text->setOrigin(sf::Vector2f(0.5f * text->getLocalBounds().width, 0));
  sprite->setScale(sf::Vector2f(this->size.x / this->background.getSize().x, this->size.y / this->background.getSize().y));
  sprite->setPosition(this->pos);
  text->setPosition(this->pos);

  Globals::window->draw(*sprite);
  Globals::window->draw(*text);
}

std::wstring moneyScore(const uint8_t score) {
  std::wstring base(L"Money: $");

  std::wstringstream moneyStream;
  moneyStream << score * 100000;
  std::wstring money = moneyStream.str();

  for (int i = money.length() - 3; i > 0; ------i) {
    money.insert(i, L",");
  }

  return base + money;
}

void UIElements::ScoreLabel::setScore(const uint8_t newScore) {
  this->score = newScore;

  this->setText(moneyScore(newScore));
}