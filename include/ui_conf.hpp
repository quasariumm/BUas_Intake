#ifndef UI_CONF_H_
#define UI_CONF_H_

// Handles the UI and the configuration.

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>

namespace UIElements {

  class Button {
  public:
    
    Button(const sf::Texture& tOuter, const sf::Vector2u& vPos, const sf::Vector2u& vSize, const sf::Texture& tInner, const std::string buttonText = "") :
    outer(tOuter), inner(tInner), text(buttonText), position(vPos), size(vSize) {};

    bool intersect(int x, int y);
    
    virtual void draw();
  
  private:

    const float itemSize = 0.7f;

    sf::Texture outer;
    sf::Texture inner;
    std::string text;

    sf::Vector2u position;
    sf::Vector2u size;

  };

  class Inventory {
  public:

    Inventory(const std::vector<std::string>& newItems, const sf::Texture& buttonOuter, const std::vector<sf::Texture>& textures);
    ~Inventory();

    void setItems(std::vector<std::string>& newItems) {};
    std::vector<std::string>& getItems() {return items;};
  
  private:

    std::vector<std::string> items;
    std::vector<UIElements::Button> buttons;

  };

};

#endif //UI_CONF_H_