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
float getDistance(sf::Vector2f a, sf::Vector2f b);

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
float getDistance(float a, float b, float c, sf::Vector2f point);

#endif //MATH_H_