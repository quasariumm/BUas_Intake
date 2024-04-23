#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_

#include <SFML/Graphics/Sprite.hpp>
#include <utility>
#include <vector>

#include "../include/config.hpp"
#include "../include/dialogue.hpp"
#include "../include/level.hpp"
#include "../include/ui.hpp"

class MainMenu {
public:

  /**
   * @brief Construct a new Main Menu object
   * 
   * @param _level The level object
   * @param _config The user config object
   */
  MainMenu(Level* _level, Config* _config);

  /**
   * @brief Draws the menu on the screen and checks for button clicks
   * 
   */
  void loop_draw();

private:

  Level* level;
  Config* config;
  Dialogue* dialogue;

  UIElements::Button play;
  UIElements::Button settings;

  std::vector<std::pair<UIElements::TextLabel, UIElements::Button>> controls;
  UIElements::Button back;

  std::vector<std::string> keybindNames;

  bool settingsMenu = false;

};

#endif //MAIN_MENU_H_