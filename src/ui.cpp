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
#include "../include/ui.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
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
#include "../include/config.hpp"

sf::Texture tmpTexture;

//////////////////////////////////////
// Button
//////////////////////////////////////

bool UIElements::Button::intersect(const sf::Vector2i pos) {
  return (
    pos.x >= this->position.x - 0.5f * this->size.x 
    && pos.x <= this->position.x + 0.5f * this->size.x 
    && pos.y >= this->position.y - 0.5f * this->size.y
    && pos.y <= this->position.y + 0.5f * this->size.y
  );
}

void UIElements::Button::draw() {
  sf::Sprite outerSprite(this->outer);

  sf::Text buttonText(Globals::mainFont, this->text);

  outerSprite.setOrigin(0.5f * static_cast<sf::Vector2f>(this->outer.getSize()));
  outerSprite.setScale(sf::Vector2f(this->size.x / static_cast<float>(this->outer.getSize().x), this->size.y / static_cast<float>(this->outer.getSize().y)));
  outerSprite.setPosition(this->position);
  Globals::window->draw(outerSprite);

  if (!this->text.empty()) {
    buttonText.setFillColor(this->textColor);
    if (this->fontSize > 0) {
      buttonText.setCharacterSize(this->fontSize);
    } else {
      buttonText.setCharacterSize(256);
    }
    const sf::FloatRect TEXT_RECT = buttonText.getLocalBounds();
    
    if (this->fontSize == -1) {
      const float FACTOR = std::min(this->size.x / TEXT_RECT.width, this->size.y / TEXT_RECT.height);
      buttonText.setScale(sf::Vector2f(FACTOR * this->textSize, FACTOR * this->textSize));
    }
    // ↓ Source: https://en.sfml-dev.org/forums/index.php?topic=26805.0 ↓
    buttonText.setOrigin(sf::Vector2f(TEXT_RECT.left + 0.5f * TEXT_RECT.width, TEXT_RECT.top + 0.5f * TEXT_RECT.height));
    
    buttonText.setPosition(this->position);
    Globals::window->draw(buttonText);
  }
}

void UIElements::Button::onClick() {
  std::clog << "Button click" << std::endl;
}

//////////////////////////////////////
// RunButton => Button
//////////////////////////////////////

void UIElements::RunButton::onClick() {
  // Set the physics of the ball 'on'
  Globals::simulationOn = true;
  // Make the button 'disappear'
  this->setPosition(this->getPosition() + sf::Vector2f(0, -1e3));
}

//////////////////////////////////////
// InventoryButton => Button
//////////////////////////////////////

void UIElements::InventoryButton::draw() {
  sf::Sprite outerSprite(this->getOuterTexture());
  
  sf::Texture innerTexture;

  if (!innerTexture.loadFromFile(this->innerPath)) {
    throw std::runtime_error("Failed to load the item texture.");
  }

  sf::Sprite innerSprite(innerTexture);

  outerSprite.setOrigin(0.5f * static_cast<sf::Vector2f>(this->getOuterTexture().getSize()));
  outerSprite.setScale(sf::Vector2f(this->getSize().x / static_cast<float>(this->getOuterTexture().getSize().x), this->getSize().y / static_cast<float>(this->getOuterTexture().getSize().y)));
  outerSprite.setPosition(this->getPosition());
  Globals::window->draw(outerSprite);

  if (innerTexture.getSize().x != 0 && innerTexture.getSize().y != 0) {
    sf::Vector2f innerSizeVector = this->itemSize * static_cast<sf::Vector2f>(this->getSize());
    sf::Vector2f factors;

    sf::Vector2f textureSize = static_cast<sf::Vector2f>(innerTexture.getSize());

    if (lockAspect) {
      float smallestSide = static_cast<float>( (this->getSize().x < this->getSize().y) ? this->getSize().x : this->getSize().y );
      smallestSide *= this->itemSize * (textureSize.y / textureSize.x);
      const float yToXAspect = textureSize.x / textureSize.y;
      factors = sf::Vector2f((smallestSide * yToXAspect) / textureSize.x, smallestSide / textureSize.y);
    } else {
      factors = sf::Vector2f(innerSizeVector.x / textureSize.x, innerSizeVector.y / textureSize.y);
    }
    innerSprite.setOrigin(0.5f * textureSize);
    innerSprite.setScale(factors);
    sf::Vector2f basePos = this->getPosition();
    innerSprite.setPosition(basePos);
    Globals::window->draw(innerSprite);
  }

  if (this->count > -1) {
    std::string countStr = std::to_string(this->count);
    sf::Text countText(Globals::mainFont, countStr);
    countText.setCharacterSize(static_cast<unsigned>(0.25f * this->getSize().x));
    sf::Vector2f sizeF = static_cast<sf::Vector2f>(this->getSize());

    sf::Vector2f bottomRightPadded = this->getPosition() + 0.4f * sizeF;
    bottomRightPadded.y -= 0.05f * sizeF.y;
    countText.setPosition(bottomRightPadded - sf::Vector2f(countText.getLocalBounds().width + 0.05f * Globals::unitSize, countText.getLocalBounds().height));

    countText.setFillColor(sf::Color(255, 255, 255));

    Globals::window->draw(countText);
  }
}

void UIElements::InventoryButton::onClick() {
  if (this->count == 0 || Globals::simulationOn) return;
  UserObjects::initBuilding(this->innerSize, this->innerPath, this->itemId);
}

//////////////////////////////////////
// Inventory
//////////////////////////////////////

UIElements::Inventory::Inventory(const std::vector<int8_t>& newItems, const std::vector<int16_t>& newCounts, sf::Texture& buttonOuter)
: items(newItems), counts(newCounts), outerTexture(buttonOuter) {
  for (unsigned short i = 0; i < newItems.size(); ++i) {
    int8_t item = newItems[i];
    int16_t count = newCounts[i];
    this->buttons.push_back(new UIElements::InventoryButton(item, buttonOuter, sf::Vector2f(), sf::Vector2u(0,0), this->itemIdToPath[item], this->itemIdToSize[item], count, true));
  }
}

UIElements::Inventory::~Inventory() {
  for (UIElements::InventoryButton* button : this->buttons) {
    delete button;
  }
}

void UIElements::Inventory::setItems(std::vector<int8_t>& newItems) {
  this->items = newItems;

  for (UIElements::InventoryButton* button : this->buttons) {
    delete button;
  }

  this->buttons.clear();

  for (unsigned short i = 0; i < newItems.size(); ++i) {
    int8_t item = newItems[i];
    this->buttons.push_back(new UIElements::InventoryButton(item, this->outerTexture, sf::Vector2f(), sf::Vector2u(0,0), this->itemIdToPath[item], this->itemIdToSize[item], 0, true));
  }
}

void UIElements::Inventory::setCounts(std::vector<int16_t>& newCounts) {
  if (newCounts.size() != this->items.size()) {
    throw std::runtime_error("The new counts list has a size that is not equal to the item list size.");
  }

  this->counts = newCounts;

  for (uint8_t i = 0; i < this->buttons.size(); ++i) {
    this->buttons[i]->setCount(newCounts[i]);
  }
}

void UIElements::Inventory::changeCount(int8_t itemId, int8_t difference) {
  auto itemIter = std::find(this->items.begin(), this->items.end(), itemId);
  auto itemIndex = std::distance(this->items.begin(), itemIter);
    
  this->counts[itemIndex] += difference;

  this->buttons[itemIndex]->setCount(this->counts[itemIndex]);
}

void UIElements::Inventory::draw() {
  // Update the position of the buttons to display the items properly in the center of the screen.
  // If the number of items is odd, make one item the middle one and offset the rest accordingly.
  // If the number of items is even, set the middle to the middle of the screen and offset the items accordingly.
  sf::Vector2u windowSize = Globals::window->getSize();

  sf::Vector2f middle = sf::Vector2f(windowSize.x / 2.f, windowSize.y * 0.9f);
  float offset = 0.f;
  const float PADDING = 0.3f * Globals::unitSize;

  const sf::Vector2u SIZE = static_cast<unsigned>(1.5f * Globals::unitSize) * sf::Vector2u(1,1);

  std::vector<UIElements::InventoryButton*>::iterator left, right;
  if (this->items.size() % 2 == 0) {

    left = this->buttons.begin() + (this->buttons.size() / 2) - 1;
    right = left + 1;

    offset = 0.5f * PADDING + 0.5f * SIZE.x;

  } else {
    
    std::vector<UIElements::InventoryButton*>::iterator middleButton = this->buttons.begin() + static_cast<int>(floor(static_cast<float>(this->buttons.size()) / 2.f));
    (*middleButton)->setSize(SIZE);
    (*middleButton)->setPosition(middle);
    (*middleButton)->draw();

    if (this->items.size() == 1) return;

    offset = SIZE.x + PADDING;

    left = middleButton - 1;
    right = middleButton + 1;

  }

  // Place the buttons
  uint8_t count = 0;
  while (true) {
    (*left)->setSize(SIZE);
    (*left)->setPosition(middle - sf::Vector2f(offset + count * SIZE.x + count * PADDING, 0));
    (*left)->draw();

    (*right)->setSize(SIZE);
    (*right)->setPosition(middle + sf::Vector2f(offset + count * SIZE.x + count * PADDING, 0));
    (*right)->draw();

    if (++right == this->buttons.end()) break;
    left--;
    count++;
  }
}

//////////////////////////////////////
// TextLabel
//////////////////////////////////////

UIElements::TextLabel::TextLabel() : Text(Globals::mainFont), Sprite(tmpTexture), fontSize(0) {};

UIElements::TextLabel::TextLabel(const std::string newText, const sf::Vector2f& newPos, const sf::Vector2f& newSize, const std::filesystem::path backgroundPath, const sf::Color& textColor, const sf::Font& font, const int newFontSize)
 : Text(font, newText), Sprite(tmpTexture), text(newText), pos(newPos), size(newSize), fontSize(newFontSize) {
  if (!this->background.loadFromFile(backgroundPath)) {
    throw std::runtime_error("Couldn't load the background of a TextLabel.");
  }
  this->setTexture(this->background, true);

  const float TEXT_SIZE = 0.7f; // Relative to the background
  if (newFontSize > 0) {
    this->setCharacterSize(newFontSize);
  } else {
    this->setCharacterSize(256);
    sf::Vector2f textSize = static_cast<sf::Text*>(this)->getLocalBounds().getSize();

    const float FACTOR = std::min(newSize.x / textSize.x, newSize.y / textSize.y);

    static_cast<sf::Text*>(this)->setScale(sf::Vector2f(TEXT_SIZE * FACTOR, TEXT_SIZE * FACTOR));
  }

  this->setFillColor(textColor);
}

void UIElements::TextLabel::setText(const std::string newString, const bool minimal) {
  this->setString(newString);
  this->text = newString;

  if (minimal) {
    return;
  }

  const float TEXT_SIZE = 0.7f; // Relative to the background
  if (this->fontSize > 0) {
    this->setCharacterSize(this->fontSize);
  } else {
    this->setCharacterSize(256);
    sf::Vector2f textSize = static_cast<sf::Text*>(this)->getLocalBounds().getSize();

    const float FACTOR = std::min(this->size.x / textSize.x, this->size.y / textSize.y);

    static_cast<sf::Text*>(this)->setScale(sf::Vector2f(TEXT_SIZE * FACTOR, TEXT_SIZE * FACTOR));
  }
}

void UIElements::TextLabel::draw() {
  // Set the right size and position
  sf::Sprite* pSprite = static_cast<sf::Sprite*>(this);
  sf::Text* pText = static_cast<sf::Text*>(this);

  pSprite->setTexture(this->background, true);

  const sf::FloatRect SPRITE_RECT = pSprite->getLocalBounds();
  const sf::FloatRect TEXT_RECT = pText->getLocalBounds();
  
  pSprite->setOrigin(0.5f * SPRITE_RECT.getSize());
  // ↓ Source: https://en.sfml-dev.org/forums/index.php?topic=26805.0 ↓
  pText->setOrigin(sf::Vector2f(TEXT_RECT.left + 0.5f * TEXT_RECT.width, TEXT_RECT.top + 0.5f * TEXT_RECT.height));
  pSprite->setScale(sf::Vector2f(this->size.x / this->background.getSize().x, this->size.y / this->background.getSize().y));
  pSprite->setPosition(this->pos);
  pText->setPosition(this->pos);

  Globals::window->draw(*pSprite);
  Globals::window->draw(*pText);
}

//////////////////////////////////////
// ScoreLabel => TextLabel
//////////////////////////////////////

std::string moneyScore(const uint8_t score) {
  std::string base = "Money: $";

  std::stringstream moneyStream;
  moneyStream << score * 100000;
  std::string money = moneyStream.str();

  if (money.length() <= 3) return base + money;

  for (int i = static_cast<int>(money.length() - 3); i > 0; ------i) {
    money.insert(i, 1, ',');
  }

  return base + money;
}

void UIElements::ScoreLabel::setScore(const uint8_t newScore) {
  this->score = newScore;

  this->setText(moneyScore(newScore));
}

//////////////////////////////////////
// EditGUI => TextLabel
//////////////////////////////////////

UIElements::EditGUI::EditGUI(const sf::Vector2f& newPos, const sf::Vector2f& newSize) : TextLabel("", newPos, newSize, std::filesystem::path(RESOURCES_PATH).append("sprites/blank.png"), sf::Color::White, Globals::mainFont, static_cast<int>(0.5f * Globals::unitSize)) {
  this->setText("F: Move/Rotate\nG: Delete");
}

void UIElements::EditGUI::setCorrectText(Config& config) {
  std::string moveKey = sf::Keyboard::getDescription(config.getKeybind("MOVE")).toAnsiString();
  if (moveKey.length() == 1) {
    // Made the one letter uppercase
    if (moveKey[0] >= 97 && moveKey[0] <= 122) {
      // a-z
      moveKey = std::string(1, static_cast<char>(moveKey[0]-32));
    }
  }
  std::string deleteKey = sf::Keyboard::getDescription(config.getKeybind("DELETE")).toAnsiString();
  if (deleteKey.length() == 1) {
    // Made the one letter uppercase
    if (deleteKey[0] >= 97 && deleteKey[0] <= 122) {
      // a-z
      deleteKey = std::string(1, static_cast<char>(deleteKey[0]-32));
    }
  }
  std::string editText = moveKey + ": Move\n" + deleteKey + ": Detele";
  this->setText(editText);

  this->setPos(sf::Vector2f(0.6f * Globals::unitSize, 13.f * Globals::unitSize) + 0.5f * static_cast<sf::Text*>(this)->getLocalBounds().getSize());
}

void UIElements::EditGUI::drawBackground() {
  const sf::Vector2f SIZE = static_cast<sf::Text*>(this)->getLocalBounds().getSize() + sf::Vector2f(0.5f * Globals::unitSize, 0.5f * Globals::unitSize);
  sf::RectangleShape backgroundShape(SIZE);
  backgroundShape.setOrigin(0.5f * SIZE);

  const sf::FloatRect TEXT_GLOBAL = static_cast<sf::Text*>(this)->getGlobalBounds();
  backgroundShape.setPosition(TEXT_GLOBAL.getCenter());

  backgroundShape.setFillColor(sf::Color(20,20,20,65));

  Globals::window->draw(backgroundShape);
}

//////////////////////////////////////
// BuildGUI => TextLabel
//////////////////////////////////////

UIElements::BuildGUI::BuildGUI(const sf::Vector2f& newPos, const sf::Vector2f& newSize) : TextLabel("", newPos, newSize, std::filesystem::path(RESOURCES_PATH).append("sprites/blank.png"), sf::Color::White, Globals::mainFont, static_cast<int>(0.5f * Globals::unitSize)) {
  this->setText("R: Rotate CCW\nT: Rotate CW\nEsc: Cancel");
}

void UIElements::BuildGUI::setCorrectText(Config& config) {
  std::string rotateCCW = sf::Keyboard::getDescription(config.getKeybind("ROTATE_CCW")).toAnsiString();
  if (rotateCCW.length() == 1) {
    // Made the one letter uppercase
    if (rotateCCW[0] >= 97 && rotateCCW[0] <= 122) {
      // a-z
      rotateCCW = std::string(1, static_cast<char>(rotateCCW[0]-32));
    }
  }
  std::string rotateCW = sf::Keyboard::getDescription(config.getKeybind("ROTATE_CW")).toAnsiString();
  if (rotateCW.length() == 1) {
    // Made the one letter uppercase
    if (rotateCW[0] >= 97 && rotateCW[0] <= 122) {
      // a-z
      rotateCW = std::string(1, static_cast<char>(rotateCW[0]-32));
    }
  }
  std::string smallStep = sf::Keyboard::getDescription(config.getKeybind("ROTATE_SMALL")).toAnsiString();
  if (smallStep.length() == 1) {
    // Made the one letter uppercase
    if (smallStep[0] >= 97 && smallStep[0] <= 122) {
      // a-z
      smallStep = std::string(1, static_cast<char>(smallStep[0]-32));
    }
  }
  std::string bigStep = sf::Keyboard::getDescription(config.getKeybind("ROTATE_BIG")).toAnsiString();
  if (bigStep.length() == 1) {
    // Made the one letter uppercase
    if (bigStep[0] >= 97 && bigStep[0] <= 122) {
      // a-z
      bigStep = std::string(1, static_cast<char>(bigStep[0]-32));
    }
  }
  std::string cancel = sf::Keyboard::getDescription(config.getKeybind("CANCEL")).toAnsiString();
  if (cancel.length() == 1) {
    // Made the one letter uppercase
    if (cancel[0] >= 97 && cancel[0] <= 122) {
      // a-z
      cancel = std::string(1, static_cast<char>(cancel[0]-32));
    }
  }

  std::string buildText = rotateCCW + ": Rotate CCW\n" + rotateCW + ": Rotate CW\n" + smallStep + ": Rotate slower\n" + bigStep + ": Rotate faster\n" + cancel + ": Cancel"; 
  this->setText(buildText);

  this->setPos(sf::Vector2f(0.6f * Globals::unitSize, 13.f * Globals::unitSize) + 0.5f * static_cast<sf::Text*>(this)->getLocalBounds().getSize());
}

void UIElements::BuildGUI::drawBackground() {
  const sf::Vector2f SIZE = static_cast<sf::Text*>(this)->getLocalBounds().getSize() + sf::Vector2f(0.5f * Globals::unitSize, 0.5f * Globals::unitSize);
  sf::RectangleShape backgroundRect(SIZE);
  backgroundRect.setOrigin(0.5f * SIZE);

  const sf::FloatRect TEXT_GLOBAL = static_cast<sf::Text*>(this)->getGlobalBounds();
  backgroundRect.setPosition(TEXT_GLOBAL.getCenter());

  backgroundRect.setFillColor(sf::Color(20,20,20,65));

  Globals::window->draw(backgroundRect);
}