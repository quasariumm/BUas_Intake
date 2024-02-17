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

  void loadFromFile(std::filesystem::path path);

  void drawPropsWalls(sf::RenderWindow& window, sf::Texture& walls, sf::Texture& props, sf::Vector2i tileSize, float unitSize);

  void drawPipes(sf::RenderWindow& window, sf::Texture& wholeTexture, sf::Vector2i tileSize, float unitSize);

private:

  unsigned map[18][18];

};

class BouncyObjects {
public:

  void makeWalls(sf::RenderWindow& window, float unitSize);

  void makeBO(const std::vector<sf::Vector2f>& points, float cor, const sf::Vector2f orientation = sf::Vector2f(1,0));

  void loadFromFile(std::filesystem::path path, float unitSize);

  std::vector<PhysicsObjects::BouncyObject>& getList() {return bo_list;};

private:

  std::vector<PhysicsObjects::BouncyObject> bo_list;

};

#endif //LEVEL_H_