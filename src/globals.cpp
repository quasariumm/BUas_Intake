/**
 * @file globals.cpp
 * @author Patrick Vreeburg
 * @brief Holds all of the global variables and some functions
 * @version 0.1
 * @date 2024-04-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../include/globals.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <filesystem>
#include <stdexcept>
#include <thread>
#include <vector>

sf::Font Globals::mainFont;
sf::Font Globals::monoFont;

void Globals::initFont() {
  std::filesystem::path fontPath = RESOURCES_PATH;
  fontPath.append("font/NotoSans-Regular.ttf");
  if (!mainFont.loadFromFile(fontPath)) {
    throw std::runtime_error("Couldn't load the font. Did you set the hardcoded variable right?");
  }
  if (!monoFont.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("font/NotoSansMono-Regular.ttf"))) {
    throw std::runtime_error("Couldn't load the monospace font. Did you set the hardcoded variable right?");
  }
}

sf::RenderWindow* Globals::window;
float Globals::unitSize;

bool Globals::simulationOn = false;

bool Globals::gameStarted = false;
// -2: main menu
// -1: intro
short Globals::currentLevel = -2;
bool Globals::dialoguePlaying = false;

std::vector<std::thread> Globals::threads;