#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>

namespace Globals {
  extern sf::Font mainFont;

  void initFont();
  
  extern sf::RenderWindow* window;
  extern float unitSize;
}

#endif //GLOBALS_H_