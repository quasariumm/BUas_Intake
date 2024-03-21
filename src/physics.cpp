/**
 * @file physics.cpp
 * @author Patrick Vreeburg
 * @brief Handles the physics of the game
 * @version 0.1
 * @date 2024-02-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../include/physics.hpp"
#include "../include/math.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Angle.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <vector>

const unsigned short NUM_SIDES = 4;
enum sides {TOP, RIGHT, BOTTOM, LEFT};

void printv(sf::Vector2f vec) {
  std::cout << "(" << vec.x << "," << vec.y << ")" << std::endl;
}

void PhysicsObjects::Ball::applyForce(float deltaTime, float F, sf::Vector2f direction) {

  // Get the acceleraion from the formula F = m * a
  float acceleraion = F / mass;

  // Multiply the acceleration by deltaTime (a = dv/dt) and multiply the result by the direction
  direction *= static_cast<float>(acceleraion * deltaTime);

  // Update the velocity by adding the result to the current velocity
  this->velocityVector += direction;

}

PhysicsObjects::Ball::Ball(sf::Texture& texture, sf::Vector2f mid, float m, float r) :
      mass(m), radius(r), midpoint(mid), Ball::Sprite(texture)
{
  float factor = (2 * r) / static_cast<float>(texture.getSize().x);
  this->setScale({factor, factor});
  this->setPosition(mid - sf::Vector2f(r, r));
}

void PhysicsObjects::Ball::setRadius(float newRadius) {
  radius = newRadius;
  float factor = (2 * newRadius) / static_cast<float>(this->getTexture().getSize().x);
  this->setScale({factor, factor});
  this->setOrigin({newRadius, newRadius});
  this->setPosition(this->midpoint - sf::Vector2f(newRadius, newRadius));
}

void PhysicsObjects::Ball::updatePoistion(float deltaTime) {

  // Because the SFML coordinate system has (0,0) top-left, the y has to be -='ed. I want to have my physics be normal, but "down is up" in SFML...
  this->midpoint.x += this->velocityVector.x * deltaTime;
  this->midpoint.y -= this->velocityVector.y * deltaTime;
  this->setPosition(this->midpoint - sf::Vector2f(this->radius, this->radius));

}

sf::Vector2f PhysicsObjects::Ball::getDirection() {
  if (velocityVector.length() == 0) {
    return sf::Vector2f();
  }
  return velocityVector.normalized();
}

unsigned short getBestSide(PhysicsObjects::Ball& ball, std::vector<unsigned short>& sides, std::vector<float>& distances) {
  // Get the distance to both sides. Grab the smallest distance (x) and get the position of the ball x-1 pixels back.
  // Then, check wich side is closest.
  float smallestDistance = *std::min_element(distances.begin(), distances.end());
  sf::Vector2f ballBackPos = ball.getPosition() - (smallestDistance - 1) * ball.getDirection();
  // TODO: Check collision again without getting in an endless loop.
}

int PhysicsObjects::BouncyObject::checkBallCollision(PhysicsObjects::Ball& ball, float unitSize) {

  // This collision system is very sketchy, but that's game dev for you. No need to be fully realistic ;).
  // For each side, get the formula of the line and check if its distance to the midpoint of the ball is smaller than the radius of the ball.
  // If so, project the midpoint of the ball on the line and check if it is inside the object.
  // If that is also the case, the ball collides.

  std::vector<unsigned short> collSides;
  std::vector<float> distances;

  for (unsigned short i = 0; i < NUM_SIDES; ++i) {

    // Get the formula
    // The a and b can be filled in by using the normal of the side.

    sf::Vector2f point1 = this->points[i];
    sf::Vector2f point2 = this->points[(i+1)%4];

    sf::Vector2f direction_vector = (point2 - point1).normalized();
    sf::Vector2f normal = direction_vector.perpendicular();

    float a, b, c;

    a = normal.x;
    b = normal.y;

    c = -1 * (a * point1.x + b * point1.y);

    float distanceToMidpoint = getDistance(a, b, c, ball.getMidpoint());

    if (distanceToMidpoint < ball.getRadius()) {
      // Hurray, at least the line is in the circle.
      // Now to check if it is in the actual object, I multiply the normal with the distance and add it to the midpoint.
      // If that point is on the line and not outside the side, it collides.
      sf::Vector2f checkPoint = ball.getMidpoint() + distanceToMidpoint * normal;
      if (std::round(a * checkPoint.x + b * checkPoint.y + c) == 0) {
        bool xDirection = std::abs(point2.x - point1.x) > std::abs(point2.y - point1.y);
        sf::Vector2f smallest = 
          (xDirection)
          ? ((point1.x < point2.x) ? point1 : point2)
          : ((point1.y < point2.y) ? point1 : point2);
        sf::Vector2f biggest = (smallest == point1) ? point2 : point1;
        if (
          (xDirection && checkPoint.x >= smallest.x && checkPoint.x <= biggest.x) ||
          (!xDirection && checkPoint.y >= smallest.y && checkPoint.y <= biggest.y)
        ) {
          collSides.push_back(i);
          distances.push_back(distanceToMidpoint);
        }
      }
      checkPoint = ball.getMidpoint() + distanceToMidpoint * -normal;
      if (std::round(a * checkPoint.x + b * checkPoint.y + c) == 0) {
        bool xDirection = std::abs(point2.x - point1.x) > std::abs(point2.y - point1.y);
        sf::Vector2f smallest = 
          (xDirection)
          ? ((point1.x < point2.x) ? point1 : point2)
          : ((point1.y < point2.y) ? point1 : point2);
        sf::Vector2f biggest = (smallest == point1) ? point2 : point1;
        if (
          (xDirection && checkPoint.x >= smallest.x && checkPoint.x <= biggest.x) ||
          (!xDirection && checkPoint.y >= smallest.y && checkPoint.y <= biggest.y)
        ) {
          collSides.push_back(i);
          distances.push_back(distanceToMidpoint);
        }
      }
    }
    
  }

  for (unsigned short i = 1; i < distances.size(); ++i) {
    if (std::abs(distances[i] - distances[i-1]) <= 0.1f * unitSize) {
      std::clog << "In range" << std::endl;
      std::vector<unsigned short> sides = std::vector<unsigned short>({static_cast<unsigned short>(i - 1), i});
      return getBestSide(sides, distances);
    }
  }

  if (collSides.size() == 1) {
    return collSides[0];
  }
  // If more than one side collided, check which is the most plausible
  // So we look which side the ball is closest to
  else if (collSides.size() > 1) {
    std::clog << "Hit on corner" << std::endl;
    return getBestSide(collSides, distances);
  }

  return -1;

}

void PhysicsObjects::BouncyObject::bounce(Ball& ball, int side) {

  // Take the inverse of the velocity vector of the ball and mirror it relative to this object's normal.
  sf::Vector2f invVelocity = -(ball.getDirection() * ball.getVelocity());
  sf::Vector2f newVelocity = cor * invVelocity.rotatedBy(2 * invVelocity.angleTo(this->orientation.rotatedBy(sf::degrees(-90 * (side-1)))));

  ball.setVelocity(newVelocity);
  this->setJustBounced(side);

}