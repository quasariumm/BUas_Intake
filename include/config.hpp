#ifndef CONFIG_H_
#define CONFIG_H_

#include <SFML/Window/Keyboard.hpp>
#include <filesystem>
#include <map>
#include <string>

class Config {
public:

  /**
   * @brief Loads the config from a file (*.qconf)
   * 
   * @param configFile 
   */
  void loadFromFile(const std::filesystem::path configFile);

  /**
   * @brief Get a keybind for a certain event
   * 
   * @param name The event name
   * @return int Keycode
   */
  sf::Keyboard::Scan getKeybind(const std::string name) {return keybinds[name];};

  /**
   * @brief Set a keybind for a certain event
   * 
   * @param name The event name
   * @param value The new keycode
   */
  void setKeybind(const std::string name, const sf::Keyboard::Scan value);

private:

  std::map<std::string, sf::Keyboard::Scan> keybinds;

  std::filesystem::path loadedConfigFile;

};

#endif //CONFIG_H_