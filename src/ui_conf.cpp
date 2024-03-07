#include "../include/ui_conf.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
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

bool UIElements::Button::intersect(int x, int y) {
  return (x >= this->position.x && x <= this->position.x + this->size.x && y >= this->position.y && y <= this->position.y + this->size.y);
}

void UIElements::Button::draw(sf::RenderWindow& window) {
  sf::Sprite outerSprite(this->outer);
  sf::Sprite innerSprite(this->inner);
  sf::Text text(global.getFont(), this->text);

  outerSprite.setScale(sf::Vector2f(this->size.x / static_cast<float>(this->outer.getSize().x), this->size.y / static_cast<float>(this->outer.getSize().y)));
  outerSprite.setPosition(this->position);
  window.draw(outerSprite);

  if (this->inner.getSize().x != 0 && this->inner.getSize().y != 0) {
    sf::Vector2f innerSize = this->itemSize * static_cast<sf::Vector2f>(this->size);
    sf::Vector2f factors;
    if (lockAspect) {
      unsigned short smallestSide = (this->size.x < this->size.y) ? this->size.x : this->size.y;
      smallestSide *= this->itemSize;
      const float yToXAspect = this->inner.getSize().x / static_cast<float>(this->inner.getSize().y);
      factors = sf::Vector2f((smallestSide * yToXAspect) / static_cast<float>(this->inner.getSize().x), smallestSide / static_cast<float>(this->inner.getSize().y));
    } else {
      factors = sf::Vector2f(innerSize.x / static_cast<float>(this->inner.getSize().x), innerSize.y / static_cast<float>(this->inner.getSize().y));
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

// TODO: Refactor a bit. Turn textures into std::filesystem::path and in Button convert to sf::Texture.

UIElements::Inventory::Inventory(const std::vector<uint8_t>& newItems, const std::vector<int16_t>& newCounts, const sf::Texture& buttonOuter, const std::vector<sf::Texture>& textures)
: items(newItems), counts(newCounts) {
  for (sf::Texture texture : textures) {
    this->buttons.push_back(UIElements::Button(buttonOuter, sf::Vector2f(), sf::Vector2u(), texture));
  }
}

void UIElements::Inventory::setItems(std::vector<uint8_t>& newItems) {
  this->items = newItems;
  this->buttons = {};
  for (uint8_t item : newItems) {
    // this->buttons.push_back();
  }
}