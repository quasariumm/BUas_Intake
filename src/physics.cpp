#include "../include/physics.hpp"
#include "../include/math.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Angle.hpp>
#include <cmath>

const unsigned short NUM_SIDES = 4;
enum sides {TOP, RIGHT, BOTTOM, LEFT};

void PhysicsObjects::Ball::applyForce(float deltaTime, float F, sf::Vector2f direction) {

  // Get the acceleraion from the formula F = m * a
  float acceleraion = F / mass;

  // Multiply the acceleration by deltaTime (a = dv/dt) and multiply the result by the direction.
  direction *= static_cast<float>(acceleraion * deltaTime);

  // Update the velocity by adding the result to the current velocity
  this->velocityVector += direction;

}

PhysicsObjects::Ball::Ball(sf::Texture& texture, sf::Vector2f mid, float m, float r) :
      mass(m), radius(r), midpoint(mid), Ball::Sprite(texture)
{
  float factor = (2 * r) / static_cast<float>(texture.getSize().x);
  this->setScale({factor, factor});
}

void PhysicsObjects::Ball::setRadius(float newRadius) {
  radius = newRadius;
  float factor = (2 * newRadius) / static_cast<float>(this->getTexture().getSize().x);
  this->setScale({factor, factor});
}

void PhysicsObjects::Ball::updatePoistion(float deltaTime) {

  this->midpoint += this->velocityVector * deltaTime;
  this->setPosition(this->midpoint);

}

int PhysicsObjects::BouncyObject::checkBallCollision(PhysicsObjects::Ball& ball) {

  // This collision system is very sketchy, but that's game dev for you. No need to be fully realistic.
  // For each side, get the formula of the line and check if its distance to the midpoint of the ball is smaller than the radius of the ball.
  // If so, check if the found point on the line is inside the object.
  // If that is also the case, the ball collides

  for (unsigned short i = 0; i < NUM_SIDES; ++i) {

    // Get the formula
    // The a and b can be filled in by using the normal of the side.

    sf::Vector2f direction_vector = (this->points[(i+1)%4] - this->points[i]).normalized();
    sf::Vector2f normal = direction_vector.perpendicular();

    float a, b, c;

    a = normal.x;
    b = normal.y;

    c = -1 * (a * points[i].x + b * points[i].y);

    float distanceToMidpoint = getDistance(a, b, c, ball.getMidpoint());

    if (distanceToMidpoint < ball.getRadius()) {
      // Hurray, at least the line is in the circle.
      // Now to check if it is in the actual object, I multiply the normal with the distance and add it to the midpoint.
      // If that point is on the line and not outside the side, it collides.
      sf::Vector2f checkPoint = ball.getMidpoint() + distanceToMidpoint * normal;
      if (std::round(a * checkPoint.x + b * checkPoint.y + c) == 0) {
        return i;
      }
      checkPoint = ball.getMidpoint() + distanceToMidpoint * -normal;
      if (std::round(a * checkPoint.x + b * checkPoint.y + c) == 0) {
        return i;
      }
    }
    
  }

  return -1;

}

void PhysicsObjects::BouncyObject::bounce(Ball& ball, int side) {

  // Take the inverse of the velocity vector of the ball and mirror it relative to this object's normal.
  sf::Vector2f invVelocity = -ball.getDirection();
  // sf::Vector2f newVelocity = cor * invVelocity.rotatedBy(invVelocity.angleTo(this->orientation.rotatedBy(90)));

}