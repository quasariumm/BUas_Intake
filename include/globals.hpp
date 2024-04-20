#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>

namespace Globals {
  extern sf::Font mainFont;
  extern sf::Font monoFont;

  void initFont();
  
  extern sf::RenderWindow* window;
  extern float unitSize;

  extern bool simulationOn;
}

#endif //GLOBALS_H_