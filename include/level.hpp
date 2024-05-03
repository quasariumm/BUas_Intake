#ifndef LEVEL_H_
#define LEVEL_H_

#include "../include/physics.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <filesystem>
#include <vector>

#include "../include/ui.hpp"
#include "../include/dialogue.hpp"

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
   * @param walls The wall texture atlas
   * @param props The props texture atlas
   * @param tileSize The size in pixels of a single texture in the atlas
   */
  void drawPropsWalls(const sf::Texture& walls, const sf::Texture& props, const sf::Vector2i tileSize);

  /**
   * @brief Does what the name implies
   * 
   * @param wholeTexture The pipe texture atlas
   * @param tileSize The size in pixels of a single texture in the atlas
   */
  void drawPipes(const sf::Texture& wholeTexture, const sf::Vector2i tileSize);

private:

  unsigned map[18][18];

};

class BouncyObjects {
public:

  /**
   * @brief Generates the walls
   * 
   */
  void makeWalls();

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
   */
  void loadFromFile(const std::filesystem::path path);

  /**
   * @brief Get the list of BouncyObjects
   * 
   * @return std::vector<PhysicsObjects::BouncyObject>& A reference to the BouncyObjects list
   */
  std::vector<PhysicsObjects::BouncyObject>& getList() {return bo_list;};

private:

  std::vector<PhysicsObjects::BouncyObject> bo_list;

};

class MoneyBag {
public:
  
  /**
   * @brief Construct a new Money Bag object
   * 
   * @param newPos The new position
   * @param newValue The amount of money in the bag. The actual value is this variable times $100.000,-
   */
  MoneyBag(const sf::Vector2f& newPos, const uint8_t newValue = 5);
  
  /**
   * @brief Checks collision between the ball and the money bag.
   * 
   * @param ball The ball
   * @returns whether of or the ball hits the money bag
   */
  bool intersect(PhysicsObjects::Ball& ball);

  /**
   * @brief Set collected
   * 
   * @param newCollected The new value of collected
   */
  void setCollected(const bool newCollected) {collected = newCollected;};

  /**
   * @brief Get the value of collected
   * 
   */
  bool getCollected() {return collected;};

  /**
   * @brief Set the value of the bag
   * 
   * @param newValue The new value in dollar ($)
   */
  void setValue(const uint8_t& newValue) {value = newValue;};
  
  /**
   * @brief Get the value of the bag
   * 
   * @return uint8_t& 
   */
  uint8_t& getValue() {return value;};

  /**
   * @brief Set the position
   * 
   * @param newPos The new position
   */
  void setPosition(const sf::Vector2f& newPos) {pos = newPos;};

  /**
   * @brief Draws the money bag on the screen
   * 
   */
  void draw();

  /**
   * @brief Makes the money bag fall. Stops when it is outside of the screen
   * @attention Make sure you use a thread on this, as this uses a while-loop and must be run asynchronously
   * 
   * @param ball The ball
   * @param windowHeight The height of the window
   */
  void fall(PhysicsObjects::Ball& ball, const unsigned windowHeight);

private:

  sf::Vector2f pos;
  uint8_t value;

  sf::Texture texture;

  bool collected;

};

class Level {
public:

  /**
   * @brief Construct a new Level object
   * 
   * @param filePath The path to the level file (.ql extension)
   * @param _walls The texture for the walls
   * @param _props The texture for the props
   * @param _pipes The texture for the pipes
   * @param _inventory The inventory
   */
  Level(const std::filesystem::path filePath, sf::Texture& _walls, sf::Texture& _props, sf::Texture& _pipes, UIElements::Inventory& _inventory) 
  : walls(_walls), props(_props), pipes(_pipes), inventory(_inventory), levelFilePath(filePath),
	tilemap(), moneyBagsNeeded(0), beginScore(0), neededScore(0) {};

  /**
   * @brief Destroy the Level object
   * 
   */
  ~Level();

  /**
   * @brief Set the level file path
   * 
   * @param newPath The new level path (*.ql)
   */
  void setLevelFilePath(const std::filesystem::path newPath) {levelFilePath = newPath;};

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
   * @brief Get the money bag vector
   * 
   * @return std::vector<MoneyBag*>&
   */
  std::vector<MoneyBag*>& getMoneyBags() {return moneyBags;};

  /**
   * @brief Get the ScoreLabel
   * 
   * @return UIElements::ScoreLabel& 
   */
  UIElements::ScoreLabel& getScoreLabel() {return scoreLabel;};

  /**
   * @brief Get the run button
   * 
   * @return UIElements::RunButton& 
   */
  UIElements::RunButton& getRunButton() {return runButton;};

  /**
   * @brief Get the Dialogue object
   * 
   * @return Dialogue& 
   */
  Dialogue& getDialogue() {return dialogue;};

  /**
   * @brief Get the Needed Score object
   * 
   * @return uint16_t 
   */
  uint16_t getNeededScore() {return neededScore;};

  /**
   * @brief Initiates the level's tilemap and BouncyObjects
   * 
   */
  void initLevel();

  /**
   * @brief Resets the money bags and the score
   * 
   */
  void resetMoneyBagPositions();

private:

  sf::Texture& walls;
  sf::Texture& props;
  sf::Texture& pipes;
  UIElements::Inventory& inventory;

  std::filesystem::path levelFilePath;
  Tilemap tilemap;
  BouncyObjects bouncyObjects;

  std::vector<MoneyBag*> moneyBags;
  uint8_t moneyBagsNeeded;

  UIElements::ScoreLabel scoreLabel;
  UIElements::RunButton runButton;

  sf::Texture runButtonOuter;

  uint8_t beginScore = 0;
  uint8_t neededScore = 0;

  Dialogue dialogue;

};

#endif //LEVEL_H_