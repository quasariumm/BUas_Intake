#include "../include/physics.hpp"
#include <SFML/System.hpp>

void Physics::Ball::applyForce(double deltaTime, double F, sf::Vector2f direction) {

  // Get the acceleraion from the formula F = m * a
  double acceleraion = F / mass;

  // Multiply the acceleration by deltaTime (a = dv/dt) and multiply the result by the direction.
  direction.x *= acceleraion * deltaTime;
  direction.y *= acceleraion * deltaTime;

  // Update the velocity by adding the result to the current velocity
  velocityVector += direction;

}