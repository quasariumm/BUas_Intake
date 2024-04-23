#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <vector>
#include <thread>

namespace Globals {
  extern sf::Font mainFont;
  extern sf::Font monoFont;

  void initFont();
  
  extern sf::RenderWindow* window;
  extern float unitSize;

  extern bool simulationOn;

  // Game values
  extern bool gameStarted;
  extern short currentLevel;
  extern bool dialoguePlaying;

  // Threads
  extern std::vector<std::thread> threads;
}

#endif //GLOBALS_H_