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


#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../include/dialogue.hpp"
#include "../include/ui_conf.hpp"
#include "../include/globals.hpp"

TextBubble::TextBubble(const std::string text) : message(text),
UIElements::TextLabel(text, sf::Vector2f(0.5f * Globals::window->getSize().x, 14.f * Globals::unitSize), sf::Vector2f(12.f * Globals::unitSize, 2.f * Globals::unitSize), std::filesystem::path(RESOURCES_PATH).append("sprites/blank.png"), sf::Color::White, Globals::monoFont) {
  
}

void TextBubble::typewriterText() {
  const short DEFAULT_LINE_LENGTH = 48;
  // Determine the number of lines and add one character every couple tens of milliseconds
  // Fill the lines with spaces to the DEFAULT_LINE_LENGTH to prevent the text from scaling during this process
  uint8_t lines = 0;
  std::vector<short> lineLengths;
  std::stringstream stringstream(this->message);
  std::string to;
  while (std::getline(stringstream, to, '\n')) {
    ++lines;
    lineLengths.push_back(to.length());
  }

  std::string current = "";
  this->setText("", true);
  uint8_t currLine = 0;

  for (char character : this->message) {

    // Add the next character to the new string
    if (character == '\n') {
      current += std::string(DEFAULT_LINE_LENGTH - lineLengths[currLine], ' ') + '\n';
      ++currLine;
    } else {
      current += character;
    }

    const short NUM_SPACES = DEFAULT_LINE_LENGTH - (current.length() - DEFAULT_LINE_LENGTH * currLine - currLine);

    this->setText(current + std::string(NUM_SPACES, ' ') + std::string(lines - (currLine + 1), '\n'), true);

    sf::sleep(sf::milliseconds(50));
  }
}

void TextBubble::draw() {
  // Draw background
  sf::Texture backgrTexture;
  if (!backgrTexture.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("sprites/dialogueBackground.png"))) {
    throw std::runtime_error("Couldn't load the dialogue background sprite.");
  }

  sf::Sprite backgrSprite{backgrTexture};
  backgrSprite.setOrigin(sf::Vector2f(0.5f * backgrTexture.getSize().x, 0));
  backgrSprite.setScale(sf::Vector2f(12.f * Globals::unitSize / backgrTexture.getSize().x, 2.f * Globals::unitSize / backgrTexture.getSize().y));
  backgrSprite.setPosition(sf::Vector2f(0.5f * Globals::window->getSize().x, 14.f * Globals::unitSize));

  Globals::window->draw(backgrSprite);

  // Draw text
  // Magic number time
  this->setPos(sf::Vector2f(0.5f * Globals::window->getSize().x + 2.f * Globals::unitSize, 14.1f * Globals::unitSize));
  this->setSize(sf::Vector2f(10.f * Globals::unitSize, 2.2f * Globals::unitSize));
  static_cast<UIElements::TextLabel*>(this)->draw();
}