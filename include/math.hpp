#ifndef MATH_H_
#define MATH_H_

#include <SFML/System.hpp>

/**
 * @brief Calculates the distance from a point to another point
 *
 * @param a Point A
 * @param b Point B
 *
 * @return The distance between the two points
 */
float getDistance(const sf::Vector2f a, const sf::Vector2f b);

/**
 * @brief Caclucates the distance from a point to a line
 *
 * @param a a from ax+by+c=0
 * @param b b from ax+by+c=0
 * @param c c from ax+by+c=0
 * @param point The point
 *
 * @return float Distance between the point and the line
 */
float getDistance(const float a, const float b, const float c, const sf::Vector2f point);

#endif //MATH_H_