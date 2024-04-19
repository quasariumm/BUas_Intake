/**
 * @file dialogue.cpp
 * @author Patrick Vreeburg
 * @brief Handles the dialogue system
 * @version 0.1
 * @date 2024-04-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <filesystem>
#include <string>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include "../include/dialogue.hpp"
#include "../include/ui_conf.hpp"
#include "globals.hpp"

TextBubble::TextBubble(const std::string text) : message(text),
UIElements::TextLabel(text, sf::Vector2f(0.5f * Globals::window->getSize().x, 14.f * Globals::unitSize), sf::Vector2f(12.f * Globals::unitSize, 3.f * Globals::unitSize), std::filesystem::path(RESOURCES_PATH).append("sprites/blank.png"), sf::Color::Cyan) {
  
}

void TextBubble::draw() {
  static_cast<UIElements::TextLabel*>(this)->draw();
  // Draw extra sprites
}