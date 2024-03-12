#ifndef UI_CONF_H_
#define UI_CONF_H_

// Handles the UI and the configuration.

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace UIElements {

  class Global {
  public:
    void initFont();
    sf::Font& getFont() {return font;};
  private:
    sf::Font font;
  };

  class Button {
  public:
    
    Button(
      const sf::Texture& tOuter, const sf::Vector2f& vPos, const sf::Vector2u& vSize, const std::filesystem::path pathInner,
      const std::string buttonText = "", const unsigned short newCount = -1, const bool lockAspectRario = false
    ) :
    outer(tOuter), innerPath(pathInner), text(buttonText), position(vPos), size(vSize), count(newCount), lockAspect(lockAspectRario) {};

    void setPosition(sf::Vector2f newPos) {position = newPos;};
    sf::Vector2f& getPosition() {return position;};

    void setSize(sf::Vector2u newSize) {size = newSize;};
    sf::Vector2u& getSize() {return size;};
    
    bool intersect(int x, int y);
    
    void draw(sf::RenderWindow& window);
  
  private:

    // Percentages/margins for both the inner sprite and the text.
    const float itemSize = 0.7f;
    const float textSize = 0.6f;

    sf::Texture outer;
    std::filesystem::path innerPath;
    bool lockAspect;
    std::string text;
    int16_t count; // Diaplyed at the bottom right of the button if needed (-1 to turn off)

    sf::Vector2f position;
    sf::Vector2u size;

  };

  class Inventory {
  public:

    Inventory(const std::vector<uint8_t>& newItems, const std::vector<int16_t>& newCounts, sf::Texture& buttonOuter);
    ~Inventory();

    void setItems(std::vector<uint8_t>& newItems);
    std::vector<uint8_t>& getItems() {return items;};

    void setCounts(std::vector<int16_t>& newCounts) {counts = newCounts;};
    std::vector<int16_t>& getCounts() {return counts;};

    void draw(sf::RenderWindow& window, const float unitSize);
  
  private:

    std::vector<uint8_t> items;
    std::vector<int16_t> counts;
    std::vector<UIElements::Button> buttons;

    sf::Texture& outerTexture;

    std::string spritePath = std::string(RESOURCES_PATH) + "/res/sprites/";

    std::map<uint8_t, std::filesystem::path> itemIdToPath = {
      {0, std::filesystem::path(spritePath + "ball.png")},
      {1, std::filesystem::path(spritePath + "ball.png")},
      {2, std::filesystem::path(spritePath + "ball.png")},
    };

  };

};

#endif //UI_CONF_H_