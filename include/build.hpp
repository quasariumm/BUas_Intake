#ifndef BUILD_H_
#define BUILD_H_

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <filesystem>
#include <vector>
#include <string>

#include "../include/physics.hpp"

namespace UserObjects {
  class EditableObject {
  public:

    /**
     * @brief Construct a new Editable Object object
     * 
     * @param newSize The size of the object
     * @param newTexturePath The path to the texture file of the object
     * @param newRotation The angle at which the object needs to be rotated
     */
    EditableObject(const sf::Vector2i newPos, const sf::Vector2f newSize, const std::filesystem::path newTexturePath, const float unitSize, const float newRotation = 0, const bool bouncy = false, const float cor = 0.8f);
    
    /**
     * @brief Returns whether or not the object has a BouncyObject "linked to" it.
     *
     */
    bool hasBouncyObject() {return bouncyObject;};

    /**
     * @brief Get the Bouncy Object
     * 
     * @return PhysicsObjects::BouncyObject& 
     */
    PhysicsObjects::BouncyObject& getBouncyObject() {return bo;};
    
    /**
     * @brief Checks if the editable object is clicked on
     * 
     * @param pos The position to check (the position of the mouse once clicked)
     * @return true if the user has clicked the object
     * @return false if the user clicked somewhere else
     */
    bool intersect(const sf::Vector2i pos);

    /**
     * @brief Draws the object on the screen
     * 
     * @param window The window it needs to be drawn on
     * @param unitSize The size of one unit. This is defined in @a main.cpp
     */
    void draw(sf::RenderWindow& window, const float unitSize);

  private:

    sf::Vector2i pos;
    sf::Vector2f size;
    std::filesystem::path texturePath;
    sf::Texture texture;
    float rotation = 0;

    // BouncyObject (optional)
    bool bouncyObject = false;
    float cor;
    PhysicsObjects::BouncyObject bo;

  };

  // Just a small class to keep track of all of the user's placed objects
  class EditableObjectList {
  public:

    /**
     * @brief Destroy the Editable Object List object. Also deletes the EditableObjects from memory
     * 
     */
    ~EditableObjectList();
    
    /**
     * @brief Adds an EditableObject to the list
     * 
     * @param object The editable object
     */
    void addObject(EditableObject* object) {editableObjects.push_back(object);};

    /**
     * @brief Get the EditableObjects
     * 
     * @return std::vector<EditableObject*>& 
     */
    std::vector<EditableObject*>& getObjects() {return editableObjects;};

  private:

    std::vector<EditableObject*> editableObjects;

  };

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
     * @brief Set the rotation
     * 
     * @param newRotation The new rotation
     */
    void setRotation(float newRotation) {rotation = newRotation;};

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

    /**
     * @brief Places the object that is currently bein built
     * 
     * @param window The window it needs to be drawn on
     * @param unitSize The size of one unit. This is defined in @a main.cpp
     * @param objList The list of EditableObjects
     */
    void place(sf::RenderWindow& window, const float unitSize, UserObjects::EditableObjectList& objList);

  private:

    sf::Vector2f size;
    std::filesystem::path texturePath;
    float rotation = 0;

  };

  /**
   * @brief Initialises the building of an obejct
   * 
   * @param newSize The size of the object
   * @param texturePath The path to the texture of the object
   */
  void initBuilding(const sf::Vector2f newSize, const std::filesystem::path texturePath);

  /**
   * @brief Clears the currently building object
   * 
   */
  void clearBuilding();

  /**
   * @brief Get the object that the player is building. If the player is not builing anything, it returns a GhostObject with size (0,0)
   * 
   * @return GhostObject*
   */
  GhostObject* getBuilding();

}

#endif //BUILD_H_