#include "../include/ui_conf.hpp"
#include "SFML/System/Vector2.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <vector>

bool UIElements::Button::intersect(int x, int y) {
  return (x >= this->position.x && x <= this->position.x + this->size.x && y >= this->position.y && y <= this->position.y + this->size.y);
}

void UIElements::Button::draw() {
  return;
}

UIElements::Inventory::Inventory(const std::vector<std::string>& newItems, const sf::Texture& buttonOuter, const std::vector<sf::Texture>& textures)
: items(newItems) {
  for (sf::Texture texture : textures) {
    this->buttons.push_back(UIElements::Button(buttonOuter, sf::Vector2u(), sf::Vector2u(), texture));
  }
}