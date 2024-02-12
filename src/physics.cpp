#include "../include/physics.hpp"
#include <SFML/System.hpp>

enum sides {TOP, RIGHT, BOTTOM, LEFT};

void Physics::Ball::applyForce(double deltaTime, double F, sf::Vector2f direction) {

  // Get the acceleraion from the formula F = m * a
  double acceleraion = F / mass;

  // Multiply the acceleration by deltaTime (a = dv/dt) and multiply the result by the direction.
  direction *= static_cast<float>(acceleraion * deltaTime);

  // Update the velocity by adding the result to the current velocity
  this->velocityVector += direction;

}

void Physics::Ball::updatePoistion(double deltaTime) {

  this->x += this->velocityVector.x * deltaTime;
  this->y += this->velocityVector.y * deltaTime;

}

/// Checks if the given coordinate is in the given (rotated) rectangle
short checkIntersect(std::vector<sf::Vector2u> points, int x, int y) {
  // For every edge pair of the object (top & bottom, left & right), determine the tangent
  // Then, determine if the given coordinate is between the pair.
  // If so, check if it is also between the other pair and return the closest edge.

  // TODO: Fix this drunk logic please or rewirte this more efficiently.

  bool betweenTopBottom, betweenLeftRight;

  sf::Vector2u differenceTopBottom = points[1] - points[0];
  double tangentTopBottom = differenceTopBottom.y / static_cast<float>(differenceTopBottom.x);
  if (
    !(x < ((points[0].x < points[2].x) ? points[0].x : points[2].x) ||
    x > ((points[1].x > points[3].x) ? points[1].x : points[3].x))
  ) {
    betweenTopBottom = 
      y < (points[0].y + (x - points[0].x) * tangentTopBottom) ||
      y > (points[2].y + (x - points[2].x) * tangentTopBottom);
  }
  sf::Vector2u differenceLeftRight = points[0] - points[3];
  double tangentLeftRight = differenceLeftRight.x / static_cast<float>(differenceLeftRight.y);
  if (
    !(y > ((points[0].y > points[1].y) ? points[0].y : points[2].y) ||
    y < ((points[2].y < points[3].y) ? points[1].y : points[3].y))
  ) {
    betweenLeftRight = 
      x < (points[0].x + (y - points[0].x) * tangentLeftRight) ||
      x > (points[2].x + (y - points[2].x) * tangentLeftRight);
  }

  if (betweenTopBottom && betweenLeftRight) {
    // TODO: Determine closest edge
  }

  return -1;
}

void Physics::BouncyObject::checkBallCollision(Physics::Ball& ball) {

  

}