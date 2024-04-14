#ifndef UI_CONF_H_
#define UI_CONF_H_

// Handles the UI and the configuration.

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace UIElements {

  class Button {
  public:
    
    /**
     * @brief Construct a new Button object
     * @attention This is a minimal constructor for the Button class. Please use the other constructor if possible.
     * 
     */
    Button() = default;
    
    /**
     * @brief Construct a new Button object, either an image button or a text button
     * 
     * @param tOuter The outer texture of the button
     * @param vPos The position of the button
     * @param vSize The size of the button
     * @param buttonText Text that needs to be displayed on the button (optional)
     */
    Button(
      const sf::Texture& tOuter, const sf::Vector2f& vPos, const sf::Vector2u& vSize, std::string buttonText = "", const sf::Color& newColor = sf::Color::Black
    ) :
    outer(tOuter), text(buttonText), position(vPos), size(vSize), textSize(0.8f), textColor(newColor) {};

    /**
     * @brief Set the text
     * 
     * @param newText The new text
     */
    void setText(const std::string newText) {text = newText;};

    /**
     * @brief Get the text
     * 
     * @return std::string& 
     */
    std::string& getText() {return text;};

    /**
     * @brief Set the text color
     * 
     * @param newColor The new text color
     */
    void setTextColor(const sf::Color& newColor) {textColor = newColor;};

    /**
     * @brief Get the text color
     * 
     * @return sf::Color& 
     */
    sf::Color& getTextColor() {return textColor;};
    
    /**
     * @brief Set the position
     * 
     * @param newPos The new position
     */
    void setPosition(const sf::Vector2f newPos) {position = newPos;};

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
    void setSize(const sf::Vector2u newSize) {size = newSize;};

    /**
     * @brief Get the size
     * 
     * @return sf::Vector2u& A reference to the size vector.
     */
    sf::Vector2u& getSize() {return size;};

    /**
     * @brief Set the outer texture object
     * 
     * @param newTexture 
     */
    void setOuterTexture(const sf::Texture& newTexture) {outer = newTexture;};
    
    /**
     * @brief Get the outer texture
     * 
     * @return sf::Texture& 
     */
    sf::Texture& getOuterTexture() {return outer;};
    
    /**
     * @brief Checks if a point is in the button using simple AABB
     * 
     * @param pos The position to check
     * @return true The point is in the button
     * @return false The point is not in the button
     */
    bool intersect(const sf::Vector2i pos);
    
    /**
     * @brief Draws the button
     *
     */
    virtual void draw();

    /**
     * @brief The function to call when the button gets clicked
     * 
     */
    virtual void onClick();
  
  private:

    // Percentage/margin the text
    float textSize;

    sf::Texture outer;
    std::string text;
    sf::Color textColor;

    sf::Vector2f position;
    sf::Vector2u size;

  };

  class RunButton : public Button {
  public:

    /**
     * @brief Construct a new Run Button object
     * @attention This is a minimal constructor for the RunButton class. Please use the other constructor if possible.
     * 
     */
    RunButton() = default;

    /**
     * @brief Construct a new Button object, either an image button or a text button
     * 
     * @param tOuter The outer texture of the button
     * @param vPos The position of the button
     * @param vSize The size of the button
     * @param buttonText Text that needs to be displayed on the button (optional)
     */
    RunButton(
      const sf::Texture& tOuter, const sf::Vector2f& vPos, const sf::Vector2u& vSize, std::string buttonText = "Run", const sf::Color& newColor = sf::Color::Black
    ) : Button(tOuter, vPos, vSize, buttonText, newColor) {};

    /**
     * @brief The function to call then the button gets clicked
     * 
     */
    void onClick() override;

  };

  class InventoryButton : public Button {

  public:

    /**
     * @brief Construct a new Button object, either an image button or a text button
     * 
     * @param itemId The ID of the item to build. See @a UIElements::Inventory for what ID corresponds with what item
     * @param tOuter The outer texture of the button
     * @param vPos The position of the button
     * @param vSize The size of the button
     * @param pathInner The path to the image file of the item (optional)
     * @param itemRealSize The size that the item has to be when built.
     * @param newCount How many of the item the player has (optional)
     * @param lockAspectRario Locks the aspect ratio of the item sprite
     */
    InventoryButton(
      const uint8_t itemId, const sf::Texture& tOuter, const sf::Vector2f& vPos, const sf::Vector2u& vSize, const std::filesystem::path pathInner,
      const sf::Vector2f& itemRealSize, int16_t newCount = -1, bool lockAspectRario = false
    ) : Button(tOuter, vPos, vSize), innerPath(pathInner), count(newCount), lockAspect(lockAspectRario), itemSize(0.7f), innerSize(itemRealSize) {};

    /**
     * @brief Get the path of the texture of the item
     * 
     * @return std::filesystem::path& 
     */
    std::filesystem::path& getItemPath() {return innerPath;};

    /**
     * @brief Set the count
     * 
     * @param newCount The new count
     */
    void setCount(int16_t newCount) {count = newCount;};

    /**
     * @brief Get the count
     * 
     * @return int16_t& 
     */
    int16_t& getCount() {return count;};

    /**
     * @brief Draws the button
     * 
     */
    void draw() override;

    /**
     * @brief The function to call when the button gets clicked
     * 
     */
    void onClick() override;

  private:

    uint8_t itemId;
    
    // Percentage/margin for the inner sprite
    float itemSize;

    std::filesystem::path innerPath;
    sf::Vector2f innerSize;

    bool lockAspect;
    int16_t count; // Diaplyed at the bottom right of the button if needed (-1 to turn off)

  };

  class Inventory {
  public:

    /**
     * @brief Construct a new Inventory object
     *
     * @param newItems Item IDs of the items in the inventory
     * @param newCounts A list of all the item's count
     * @param buttonOuter The texture for the background of the button
     */
    Inventory(const std::vector<uint8_t>& newItems, const std::vector<int16_t>& newCounts, sf::Texture& buttonOuter);

    /**
     * @brief Destroy the Inventory object
     * 
     */
    ~Inventory();

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
    void setCounts(std::vector<int16_t>& newCounts);

    /**
     * @brief Get the count list
     * 
     * @return std::vector<int16_t>& A reference to the count list
     */
    std::vector<int16_t>& getCounts() {return counts;};

    /**
     * @brief Get the buttons
     * 
     * @return std::vector<UIElements::Button>& 
     */
    std::vector<UIElements::InventoryButton*>& getButtons() {return buttons;};

    /**
     * @brief Draws the inventory by drawing all of the individual buttons
     * 
     */
    void draw();
  
  private:

    std::vector<uint8_t> items;
    std::vector<int16_t> counts;
    std::vector<UIElements::InventoryButton*> buttons;

    sf::Texture& outerTexture;

    std::string spritePath = std::string(RESOURCES_PATH) + "sprites/";

    std::map<uint8_t, std::filesystem::path> itemIdToPath = {
      {0, std::filesystem::path(spritePath + "bouncePad.png")},
      {1, std::filesystem::path(spritePath + "ball.png")},
      {2, std::filesystem::path(spritePath + "ball.png")},
    };

    std::map<uint8_t, sf::Vector2f> itemIdToSize = { // ! The size is in units. Multiply by unitSize before displaying
      {0, sf::Vector2f(2, 1)},
      {1, sf::Vector2f(1, 1)},
      {2, sf::Vector2f(1, 1)}
    };

  };

  class TextLabel : public sf::Text, public sf::Sprite {
  public:

    /**
     * @brief Construct a new Text Label object
     * @attention This is a minimal constructor for the TextLabel class. Please use the other constructor if possible.
     * 
     */
    TextLabel();
    
    /**
     * @brief Construct a new Text Label object
     * 
     * @param newText The text that needs to be displayed
     * @param newPos The position of the label
     * @param newSize The size of the label
     * @param backgroundPath The path to the background texture
     * @param textColor The color to fill the text with
     */
    TextLabel(const std::wstring newText, const sf::Vector2f& newPos, const sf::Vector2f& newSize, const std::filesystem::path backgroundPath, const sf::Color& textColor = sf::Color::White);

    /**
     * @brief Set the text
     * 
     * @param newText The new text
     */
    void setText(const std::wstring newText);

    /**
     * @brief Get the Text object
     * 
     * @return std::string 
     */
    std::wstring getText() {return text;};

    /**
     * @brief Set the position
     * 
     * @param newPos The new position
     */
    void setPos(const sf::Vector2f& newPos) {pos = newPos;};

    /**
     * @brief Get the position
     * 
     * @return sf::Vector2f& 
     */
    sf::Vector2f& getPos() {return pos;};

    /**
     * @brief Set the size
     * 
     * @param newSize The new size
     */
    void setSize(const sf::Vector2f& newSize) {size = newSize;};

    /**
     * @brief Get the size
     * 
     * @return sf::Vector2f& 
     */
    sf::Vector2f& getSize() {return size;};

    /**
     * @brief Get the background texture
     * 
     * @return sf::Texture& 
     */
    sf::Texture& getBackground() {return background;};

    /**
     * @brief Draws the text label on the screen
     * 
     */
    void draw();

  private:

    sf::Texture background;
    
    std::wstring text;
    sf::Vector2f pos;
    sf::Vector2f size;

  };

  class ScoreLabel : public TextLabel {
  public:

    /**
     * @brief Construct a new Score Label object
     * @attention This is a minimal constructor for the ScoreLabel class. Please use the other constructor if possible.
     * 
     */
    ScoreLabel() = default;
    
    /**
     * @brief Construct a new Score Label object
     * 
     * @param newText The text that needs to be displayed
     * @param newPos The position of the label
     * @param newSize The size of the label
     * @param backgroundPath The path to thebackground texture
     * @param textColor The color to fill the text with
     */
    ScoreLabel(const std::wstring newText, const sf::Vector2f& newPos, const sf::Vector2f& newSize, const std::filesystem::path backgroundPath, const sf::Color& textColor = sf::Color::White)
    : TextLabel(newText, newPos, newSize, backgroundPath, textColor) {};

    /**
     * @brief Set the score
     * 
     * @param newScore The new score
     */
    void setScore(const uint8_t newScore);

    /**
     * @brief Get the score
     * 
     * @return uint8_t 
     */
    uint8_t getScore() {return score;};
  
  private:

    uint8_t score = 0;

  };

  class EditGUI : public TextLabel {
  public:

    /**
     * @brief Construct a new Edit GUI object
     * 
     * @param newPos The new position
     * @param newSize The new size
     */
    EditGUI(const sf::Vector2f& newPos, const sf::Vector2f& newSize);

  };

};

#endif //UI_CONF_H_