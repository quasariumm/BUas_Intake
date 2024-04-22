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
#include <fstream>
#include <ios>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../include/dialogue.hpp"
#include "../include/ui.hpp"
#include "../include/globals.hpp"

//////////////////////////////////////
// TextBubble => TextLabel
//////////////////////////////////////

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
  if (!enabled) {
    return;
  }

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
  this->setPos(sf::Vector2f(0.5f * Globals::window->getSize().x + .5f * Globals::unitSize, 14.3f * Globals::unitSize));
  this->setSize(sf::Vector2f(12.f * Globals::unitSize, 2.2f * Globals::unitSize));
  static_cast<UIElements::TextLabel*>(this)->draw();
}

//////////////////////////////////////
// Dialogue
//////////////////////////////////////

void Dialogue::loadFromFile(const std::filesystem::path dialogueFile) {

  this->isIntro = dialogueFile.filename() == "intro.qd";

  std::ifstream stream;
  stream.open(dialogueFile, std::ios::in);

  if (!stream.is_open()) {
    throw std::runtime_error("Couldn't open the dialogue file.");
  }

  std::string lineStr;
  while (std::getline(stream, lineStr)) {
    std::string instruction = lineStr.substr(0, lineStr.find(' '));
    std::string argument = lineStr.substr(lineStr.find(' ') + 1);

    // In the argument, replace all "\n" with real new-line characters
    short pos;
    while ((pos = argument.find("\\n")) != std::string::npos) {
      argument.replace(pos, 2, 1, '\n');
    }

    this->instructions.push_back(std::make_pair(instruction, argument));
  }
}

void Dialogue::play(TextBubble* textBubble, UIElements::TextLabel* textLabel) {

  textBubble->setEnabled(true);

  for (auto& [instruction, argument] : instructions) {

    if (instruction == "SAY") {

      textBubble->setMessage(argument.substr(1, argument.length() - 2));
      textBubble->typewriterText();

    } else if (instruction == "WAIT") {
      
      float seconds = std::stof(argument);
      sf::sleep(sf::seconds(seconds));

    } else if (instruction == "TEXT") {
    
      textLabel->setText(argument.substr(1, argument.length() - 2));

    } else if (instruction == "CLEAR_TEXT") {

      textLabel->setText("");

    } else if (instruction == "CLEAR_DIALOGUE") {

      textBubble->setEnabled(false);

    } else if (instruction == "ENABLE_DIALOGUE") {

      textBubble->setEnabled(true);

    }

  }

  if (this->isIntro) {
    ++Globals::currentLevel;
  }

}