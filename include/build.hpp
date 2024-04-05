#ifndef BUILD_H_
#define BUILD_H_

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <filesystem>
#include <vector>
#include <string>

namespace UserObjects {

  class GhostObject {
  public:

    /**
     * @brief Construct a new Ghost Object
     * 
     * @param newSize The size of the ghost object
     */
    GhostObject(const sf::Vector2f newSize, const std::filesystem::path newTexturePath) : size(newSize), texturePath(newTexturePath) {};
    
    /**
     * @brief Set the size
     * 
     * @param newSize The new size
     */
    void setSize(const sf::Vector2f newSize) {size = newSize;};

    /**
     * @brief Get the size
     * 
     * @return sf::Vector2f& 
     */
    sf::Vector2f& getSize() {return size;};

    /**
     * @brief Set the texture
     * 
     * @param texturePath The path to the texture file
     */
    void setTexture(const std::filesystem::path newTexturePath) {texturePath = newTexturePath;};

    /**
     * @brief A function that is called on the main loop. It updates the position and rotation if the correct key is pressed
     * 
     * @param window The window the ghost object needs to be drawn on
     * @param rotateKeyPressed Whether or not one of the rotate keys is pressed
     * @param modifier The pressed modifier key. If no modifier keys are pressed, this is empty
     */
    void loop(sf::RenderWindow& window, const bool rotateKeyPressed, const std::string modifier, const float unitSize);

  private:

    sf::Vector2f size;
    std::filesystem::path texturePath;
    float rotation = 0;

  };

  class EditableObject {
  public:

    /**
     * @brief Checks if the editable object is clicked on
     * 
     * @param pos The position to check (the position of the mouse once clicked)
     * @return true if the user has clicked the object
     * @return false if the user clicked somewhere else
     */
    bool intersect(const sf::Vector2i pos);

  private:

    sf::Vector2f pos;
    sf::Vector2f orientation;
    std::vector<sf::Vector2f> points;

  };

  /**
   * @brief Initialises the building of an obejct
   * 
   * @param newSize The size of the object
   * @param texturePath The path to the texture of the object
   */
  void initBuilding(const sf::Vector2f newSize, const std::filesystem::path texturePath);

  /**
   * @brief Get the object that the player is building. If the player is not builing anything, it returns a GhostObject with size (0,0)
   * 
   * @return GhostObject*
   */
  GhostObject* getBuilding();

}

#endif //BUILD_H_