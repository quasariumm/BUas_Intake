#ifndef LEVEL_H_
#define LEVEL_H_

#include "../include/physics.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <filesystem>
#include <vector>

class Tilemap {
public:

  /**
   * @brief Does what the name implies
   * 
   * @param path The path of the file that needs to be loaded
   */
  void loadFromFile(const std::filesystem::path path);

  /**
   * @brief Does what the name implies
   * 
   * @param window The window on which it needs to be drawn
   * @param walls The wall texture atlas
   * @param props The props texture atlas
   * @param tileSize The size in pixels of a single texture in the atlas
   * @param unitSize The size of one unit. This is defined in @a main.cpp
   */
  void drawPropsWalls(sf::RenderWindow& window, const sf::Texture& walls, const sf::Texture& props, const sf::Vector2i tileSize, const float unitSize);

  /**
   * @brief Does what the name implies
   * 
   * @param window The window on which it needs to be drawn
   * @param wholeTexture The pipe texture atlas
   * @param tileSize The size in pixels of a single texture in the atlas
   * @param unitSize The size of one unit. This is defined in @a main.cpp
   */
  void drawPipes(sf::RenderWindow& window, const sf::Texture& wholeTexture, const sf::Vector2i tileSize, const float unitSize);

private:

  unsigned map[18][18];

};

class BouncyObjects {
public:

  /**
   * @brief Generates the walls
   * 
   * @param window The window. Only needed for the size
   * @param unitSize The size of one unit. This is defined in @a main.cpp
   */
  void makeWalls(const sf::RenderWindow& window, const float unitSize);

  /**
   * @brief Generates a new BounyObject
   * 
   * @param points The points that make up the shape
   * @param cor Coefficient of restitution, which is the factor that the speed gets multiplied with upon colision
   * @param orientation The orientation of the object. This is a vector pointing to the right edge of the object
   */
  void makeBO(const std::vector<sf::Vector2f>& points, const float cor, const sf::Vector2f orientation = sf::Vector2f(1,0));

  /**
   * @brief Loads and generates the BouncyObjects from a level file (*.ql)
   * 
   * @param path The path to the level file
   * @param unitSize The size of one unit. This is defined in @a main.cpp
   */
  void loadFromFile(const std::filesystem::path path, const float unitSize);

  /**
   * @brief Get the list of BouncyObjects
   * 
   * @return std::vector<PhysicsObjects::BouncyObject>& A reference to the BouncyObjects list
   */
  std::vector<PhysicsObjects::BouncyObject>& getList() {return bo_list;};

private:

  std::vector<PhysicsObjects::BouncyObject> bo_list;

};

class Level {
public:

  /**
   * @brief Construct a new Level object
   * 
   * @param filePath The path to the level file (.ql extension)
   */
  Level(const std::filesystem::path filePath) : levelFilePath(filePath) {};

  /**
   * @brief Get the Tilemap object
   * 
   * @return Tilemap& A reference to the tilemap
   */
  Tilemap& getTilemap() {return tilemap;};

  /**
   * @brief Get the Bouncy Objects object
   * 
   * @return BouncyObjects& A reference to the BouncyObjects class
   */
  BouncyObjects& getBouncyObjects() {return bouncyObjects;};

  /**
   * @brief Initiates the level's tilemap and BouncyObjects
   * 
   * @param window The window it needs to be drawn on
   * @param unitSize The size of one unit. This is defined in @a main.cpp
   */
  void initLevel(sf::RenderWindow& window, const float unitSize, const sf::Texture& walls, const sf::Texture& props, const sf::Texture& pipes);

private:

  std::filesystem::path levelFilePath;
  Tilemap tilemap;
  BouncyObjects bouncyObjects;

};

#endif //LEVEL_H_