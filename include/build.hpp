#ifndef BUILD_H_
#define BUILD_H_

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <filesystem>
#include <vector>

#include "../include/physics.hpp"
#include "../include/ui.hpp"
#include "../include/config.hpp"

namespace UserObjects {
  class EditableObject {
  public:

    /**
     * @brief Construct a new Editable Object object
     * 
     * @param newPos The new position
     * @param newSize The size of the object
     * @param newTexturePath The path to the texture file of the object
     * @param itemId The ID of the item to build. See @a UIElements::Inventory for what ID corresponds with what item
     * @param newRotation The angle at which the object needs to be rotated
     * @param bouncy Whether or not the item is bouncy and should have a @a PhysicsObjects::BouncyObject in it
     * @param cor The coefficient of restitution. Determines with what factor the speed will be multiplied when the ball bounces
     * @param booster Whether or not the item is a booster
     */
    EditableObject(const sf::Vector2i newPos, const sf::Vector2f newSize, const std::filesystem::path newTexturePath, const int8_t itemId, const float newRotation = 0, const bool bouncy = false, const float cor = 0.8f, const bool booster = false);
    
    /**
     * @brief Returns whether or not the object has a BouncyObject "linked to" it
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
     * @brief Returns whether or not the object has a Booster "linked to" it
     * 
     */
    bool hasBooster() {return booster;};

    /**
     * @brief Get the Booster object
     * 
     * @return PhysicsObjects::Booster& 
     */
    PhysicsObjects::Booster& getBooster() {return boost;};

    /**
     * @brief Get the item ID
     * 
     * @return uint8_t 
     */
    int8_t getItemId() {return itemID;};

    /**
     * @brief Set the position
     * 
     * @param newPos The new position
     */
    void setPos(const sf::Vector2i& newPos) {pos = newPos;};
    
    /**
     * @brief Get the position
     * 
     * @return sf::Vector2i
     */
    sf::Vector2i getPos() {return pos;};

    /**
     * @brief Set the size
     * 
     * @param newSize The new size
     */
    void setSize(const sf::Vector2f& newSize) {size = newSize;};
    
    /**
     * @brief Get the size
     * 
     * @return sf::Vector2f
     */
    sf::Vector2f getSize() {return size;};

    /**
     * @brief Set the rotation
     * 
     * @param newRotation The new rotation
     */
    void setRotation(const float newRotation) {rotation = newRotation;};

    /**
     * @brief Get the rotation
     * 
     * @return float 
     */
    float getRotation() {return rotation;};

    /**
     * @brief Get the texture path
     * 
     * @return std::filesystem::path
     */
    std::filesystem::path getTexturePath() {return texturePath;};
    
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
     */
    void draw();

  private:

    int8_t itemID;

    sf::Vector2i pos;
    sf::Vector2f size;
    std::filesystem::path texturePath;
    sf::Texture texture;
    float rotation = 0;

    // BouncyObject (optional)
    bool bouncyObject = false;
    float cor;
    PhysicsObjects::BouncyObject bo;

    // Booster (optional)
    bool booster = false;
    PhysicsObjects::Booster boost;

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
     * @param newTexturePath The path to the texture file
     * @param itemId The ID of the item to build. See @a UIElements::Inventory for what ID corresponds with what item
     * @param rotation (optional) The angle in degrees at which the object is rotated
     */
    GhostObject(const sf::Vector2f newSize, const std::filesystem::path newTexturePath, const int8_t itemId, const float rotation = 0.0f) : itemID(itemId), size(newSize), texturePath(newTexturePath), rotation(rotation) {};
    
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
     * @param rotateKeyPressed Whether or not one of the rotate keys is pressed
     * @param playerConf The player config object. Used to check the controls
     */
    void loop(const bool rotateKeyPressed, Config& playerConf);

    /**
     * @brief Places the object that is currently bein built
     * 
     * @param objList The list of EditableObjects
     * @param inventory The inventory
     */
    void place(UserObjects::EditableObjectList& objList, UIElements::Inventory& inventory);

  private:

    int8_t itemID;

    sf::Vector2f size;
    std::filesystem::path texturePath;
    float rotation = 0;

  };

  /**
   * @brief Initialises the building of an obejct
   * 
   * @param newSize The size of the object
   * @param texturePath The path to the texture of the object
   * @param itemId The ID of the item to build. See @a UIElements::Inventory for what ID corresponds with what item
   * @param rotation (optional) The angle in degrees at which the object is rotated
   */
  void initBuilding(const sf::Vector2f newSize, const std::filesystem::path texturePath, const int8_t itemId, const float rotation = 0.0f);

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