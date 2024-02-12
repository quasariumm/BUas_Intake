#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

const double WINDOW_SIZE_FACTOR = 0.9;

int main() {

  // The window is square with a side of 80% of the shortest edge of the screen
  sf::Vector2u monitorSize = sf::VideoMode::getDesktopMode().size;
  unsigned shortestEdge = (monitorSize.x > monitorSize.y) ? monitorSize.y : monitorSize.x;

  sf::RenderWindow window(sf::VideoMode(
    {static_cast<unsigned>(WINDOW_SIZE_FACTOR * shortestEdge), static_cast<unsigned>(WINDOW_SIZE_FACTOR * shortestEdge)}),
    "BUas Intake Game"
  );

  // NOTE: Just temporary
  window.setMaximumSize(window.getSize());
  window.setMinimumSize(window.getSize());

  window.setFramerateLimit(0);
  window.setVerticalSyncEnabled(true);

  sf::CircleShape shape(100.f);
  shape.setFillColor(sf::Color::Green);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    window.clear();
    window.draw(shape);
    window.display();
  }

  return 0;
}