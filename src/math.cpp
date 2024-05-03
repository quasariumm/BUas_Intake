/**
 * @file math.cpp
 * @author Patrick Vreeburg
 * @brief Handles some mathematical calculations.
 * @version 0.1
 * @date 2024-02-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../include/math.hpp"
#include <SFML/System.hpp>
#include <cmath>
#include <cstdlib>

float getDistance(const sf::Vector2f a, const sf::Vector2f b) {
  return static_cast<float>(sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
}

float getDistance(const float a, const float b, const float c, const sf::Vector2f point) {
  return static_cast<float>(std::abs(a * point.x + b * point.y + c) / sqrt(pow(a, 2)+pow(b, 2)));
}