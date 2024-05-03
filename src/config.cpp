/**
 * @file config.cpp
 * @author Patrick Vreeburg
 * @brief Handles the user config
 * @version 0.1
 * @date 2024-04-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/String.hpp>
#include <cstddef>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../include/config.hpp"

void Config::loadFromFile(const std::filesystem::path configFile) {

  this->loadedConfigFile = configFile;

  std::ifstream fileStream;
  fileStream.open(configFile);

  if (!fileStream.is_open()) {
    throw std::runtime_error("Couldn't load the config file.");
  }

  std::string linestr;
  bool controlsHeaderFound = false;
  while (std::getline(fileStream, linestr)) {
    
    if (linestr.find('%') != std::string::npos) {
      // Comment
      continue;
    }

    if (linestr.find("[Keybinds]") != std::string::npos) {
      controlsHeaderFound = true;
      continue;
    }

    if (!controlsHeaderFound) {
      continue;
    }
    if (linestr.empty()) {
      break;
    }

    size_t spacePos = linestr.find(' ');

    this->keybinds.emplace(linestr.substr(0, spacePos), static_cast<sf::Keyboard::Scan>(std::stoi(linestr.substr(spacePos + 1))));
  }

  for (auto& [name, keybind] : this->keybinds) {
    std::clog << name << " is set to " << sf::Keyboard::getDescription(keybind).toAnsiString() << std::endl;
  }
}

void Config::setKeybind(const std::string name, const sf::Keyboard::Scan value) {
  this->keybinds[name] = value;
  // Write the changes to the config file.
  std::ifstream fileStream;
  fileStream.open(this->loadedConfigFile);

  if (!fileStream.is_open()) {
    throw std::runtime_error("Couldn't load the config file.");
  }

  std::vector<std::string> lines;
  std::string linestr;

  while (std::getline(fileStream, linestr)) {
    if (linestr.find(name) != std::string::npos) {
      linestr = name + ' ' + std::to_string(static_cast<int>(value));
    }
    lines.push_back(linestr);
  }
  fileStream.close();

  std::ofstream outFile;
  outFile.open(this->loadedConfigFile);

  if (!outFile.is_open()) {
    throw std::runtime_error("Couldn't load the config file.");
  }
  
  for (std::string line : lines) {
    outFile << line << '\n';
  }
}