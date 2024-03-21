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

    /**
     * @brief Initializes the font
     * 
     */
    void initFont();

    /**
     * @brief Get the font
     * 
     * @return sf::Font& A reference to the font
     */
    sf::Font& getFont() {return font;};

  private:

    sf::Font font;

  };

  class Button {
  public:
    
    /**
     * @brief Construct a new Button object, either an image button or a text button
     * 
     * @param tOuter The outer texture of the button
     * @param vPos The position of the button
     * @param vSize The size of the button
     * @param pathInner The path to the image file of the item (optional)
     * @param buttonText Text that needs to be displayed on the button (optional)
     * @param newCount How many of the item the player has (optional)
     * @param lockAspectRario Locks the aspect ratio of the item sprite
     */
    Button(
      const sf::Texture& tOuter, const sf::Vector2f& vPos, const sf::Vector2u& vSize, const std::filesystem::path pathInner,
      const std::string buttonText = "", const unsigned short newCount = -1, const bool lockAspectRario = false
    ) :
    outer(tOuter), innerPath(pathInner), text(buttonText), position(vPos), size(vSize), count(newCount), lockAspect(lockAspectRario), itemSize(0.7f), textSize(0.6f) {};

    /**
     * @brief Set the position
     * 
     * @param newPos The new position
     */
    void setPosition(sf::Vector2f newPos) {position = newPos;};

    /**
     * @brief Get the position
     *
     * @return sf::Vector2f& A reference to the position vector
     */
    sf::Vector2f& getPosition() {return position;};

    /**
     * @brief Set the size
     * 
     * @param newSize The new size
     */
    void setSize(sf::Vector2u newSize) {size = newSize;};

    /**
     * @brief Get the size
     * 
     * @return sf::Vector2u& A reference to the size vector.
     */
    sf::Vector2u& getSize() {return size;};
    
    /**
     * @brief Checks if a point is in the button using simple AABB
     * 
     * @param pos The position to check
     * @return true The point is in the button
     * @return false The point is not in the button
     */
    bool intersect(sf::Vector2f& pos);
    
    /**
     * @brief Draws the button
     *
     * @param window The window on which it needs to be drawn
     */
    void draw(sf::RenderWindow& window);
  
  private:

    // Percentages/margins for both the inner sprite and the text
    float itemSize;
    float textSize;

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

    /**
     * @brief Construct a new Inventory object
     *
     * @param newItems Item IDs of the items in the inventory
     * @param newCounts A list of all the item;s count
     * @param buttonOuter The texture for the background of the button
     */
    Inventory(const std::vector<uint8_t>& newItems, const std::vector<int16_t>& newCounts, sf::Texture& buttonOuter);

    /**
     * @brief Set the items
     * 
     * @param newItems The new items
     */
    void setItems(std::vector<uint8_t>& newItems);

    /**
     * @brief Get the item list
     * 
     * @return std::vector<uint8_t>& A reference to the item list
     */
    std::vector<uint8_t>& getItems() {return items;};

    /**
     * @brief Set the counts
     * 
     * @param newCounts The new counts
     */
    void setCounts(std::vector<int16_t>& newCounts) {counts = newCounts;};

    /**
     * @brief Get the count list
     * 
     * @return std::vector<int16_t>& A reference to the count list
     */
    std::vector<int16_t>& getCounts() {return counts;};

    /**
     * @brief Draws the inventory by drawing all of the individual buttons
     * 
     * @param window The window on which it needs to be drawn
     * @param unitSize The size of a unit. This is defined in @a main.cpp
     */
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