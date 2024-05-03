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

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Angle.hpp>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../include/math.hpp"
#include "../include/globals.hpp"
#include "../include/audio.hpp"

/**
 * @brief A way to represent a side as a line of the format ax+by=c. Also includes the side index.
 * 
 */
struct Side {
  float a, b, c;
  unsigned short i;
};

const unsigned short NUM_SIDES = 4;
enum sides {TOP, RIGHT, BOTTOM, LEFT};

void printv(sf::Vector2f vec) {
  std::cout << "(" << vec.x << "," << vec.y << ")" << std::endl;
}

//////////////////////////////////////
// Ball
//////////////////////////////////////

void PhysicsObjects::Ball::applyForce(const float deltaTime, const float F, sf::Vector2f direction) {

  // Get the acceleraion from the formula F = m * a
  float acceleraion = F / this->mass;

  // Multiply the acceleration by deltaTime (a = dv/dt) and multiply the result by the direction
  direction *= static_cast<float>(acceleraion * deltaTime);

  // Update the velocity by adding the result to the current velocity
  this->velocityVector += direction;

}

PhysicsObjects::Ball::Ball(const sf::Texture& texture, const sf::Vector2f mid, const float m, const float r) :
      Sprite(texture), mass(m), midpoint(mid), radius(r)
{
  float factor = (2 * r) / static_cast<float>(texture.getSize().x);
  this->setScale({factor, factor});
  this->setPosition(mid - sf::Vector2f(r, r));
}

void PhysicsObjects::Ball::setRadius(const float newRadius) {
  radius = newRadius;
  float factor = (2 * newRadius) / static_cast<float>(this->getTexture().getSize().x);
  this->setScale({factor, factor});
  this->setOrigin({newRadius, newRadius});
  this->setPosition(this->midpoint - sf::Vector2f(newRadius, newRadius));
}

void PhysicsObjects::Ball::updatePoistion(const float deltaTime) {

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

//////////////////////////////////////
// BouncyObject
//////////////////////////////////////

unsigned short getBestSide(PhysicsObjects::Ball& ball, std::vector<Side>& sides, std::vector<float>& distances) {
  // Get the distance to both sides. Grab the smallest distance (x) and get the position of the ball x-1 pixels back.
  // Then, check wich side is closest.
  float smallestDistance = *std::min_element(distances.begin(), distances.end());
  sf::Vector2f ballBackPos = ball.getMidpoint() - (smallestDistance - 3) * ball.getDirection();
  
  std::vector<unsigned short> collSides;
  for (Side side : sides) {
    float distanceToMidpoint = getDistance(side.a, side.b, side.c, ballBackPos);
    if (distanceToMidpoint < ball.getRadius()) {
      collSides.push_back(side.i);
    }
  }

  if (collSides.size() == 0) {
    return sides[0].i;
  }

  return collSides[0];
}

int PhysicsObjects::BouncyObject::checkBallCollision(PhysicsObjects::Ball& ball) {

  // This collision system is very sketchy, but that's game dev for you. No need to be fully realistic ;).
  // For each side, get the formula of the line and check if its distance to the midpoint of the ball is smaller than the radius of the ball.
  // If so, project the midpoint of the ball on the line and check if it is inside the object.
  // If that is also the case, the ball collides.

  std::vector<Side> collSides;
  std::vector<Side> allSides;
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

    allSides.push_back({a, b, c, i});

    float distanceToMidpoint = getDistance(a, b, c, ball.getMidpoint());
    distances.push_back(distanceToMidpoint);

    if (distanceToMidpoint < ball.getRadius()) {
      // Hurray, at least the line is in the circle.
      // Now to check if it is in the actual object, I multiply the normal with the distance and add it to the midpoint.
      // If that point is on the line and not outside the side, it collides.

      sf::Vector2f checkPoint = ball.getMidpoint() + distanceToMidpoint * normal;
      if (std::round(a * checkPoint.x + b * checkPoint.y + c) == 0) {
        // If xDirection is true, the side is more horizontal than vertical
        bool xDirection = std::abs(point2.x - point1.x) > std::abs(point2.y - point1.y);
        // Get the smallest and the biggest point based on the x or y coordinate (depending on xDirection).
        sf::Vector2f smallest = 
          (xDirection)
          ? ((point1.x < point2.x) ? point1 : point2)
          : ((point1.y < point2.y) ? point1 : point2);
        sf::Vector2f biggest = (smallest == point1) ? point2 : point1;
        // Check if the point is between the edges, thus in the object. If so, we collided with the side.
        if (
          (xDirection && checkPoint.x >= smallest.x && checkPoint.x <= biggest.x) ||
          (!xDirection && checkPoint.y >= smallest.y && checkPoint.y <= biggest.y)
        ) {
          collSides.push_back({a,b,c, i});
        }
      }

      checkPoint = ball.getMidpoint() + distanceToMidpoint * -normal;
      // This is the same as the if-statement above.
      if (std::round(a * checkPoint.x + b * checkPoint.y + c) == 0) {
        // If xDirection is true, the side is more horizontal than vertical
        bool xDirection = std::abs(point2.x - point1.x) > std::abs(point2.y - point1.y);
        // Get the smallest and the biggest point based on the x or y coordinate (depending on xDirection).
        sf::Vector2f smallest = 
          (xDirection)
          ? ((point1.x < point2.x) ? point1 : point2)
          : ((point1.y < point2.y) ? point1 : point2);
        sf::Vector2f biggest = (smallest == point1) ? point2 : point1;
        // Check if the point is between the edges, thus in the object. If so, we collided with the side.
        if (
          (xDirection && checkPoint.x >= smallest.x && checkPoint.x <= biggest.x) ||
          (!xDirection && checkPoint.y >= smallest.y && checkPoint.y <= biggest.y)
        ) {
          collSides.push_back({a,b,c, i});
        }
      }
    }
    
  }

  // Check if the ball is near a corner. If so, determine and return the side that it hit first.
  for (unsigned short i = 1; i < distances.size(); ++i) {
    // Check if both sides of a corner are even close enough to the ball.
    if (!(distances[i] <= ball.getRadius() && distances[i-1] <= ball.getRadius())) {
      continue;
    }
    if (std::abs(distances[i] - distances[i-1]) <= 0.1f * Globals::unitSize) {
      std::vector<Side> sides = std::vector<Side>({allSides[i-1], allSides[i]});
      return getBestSide(ball, sides, distances);
    }
  }

  if (collSides.size() == 1) {
    return collSides[0].i;
  }
  // If more than one side collided, check which is the most plausible
  // So we look which side the ball is closest to
  else if (collSides.size() > 1) {
    return getBestSide(ball, collSides, distances);
  }

  return -1;

}

void PhysicsObjects::BouncyObject::bounce(PhysicsObjects::Ball& ball, const short side) {

  // Take the inverse of the velocity vector of the ball and mirror it relative to this object's normal.
  sf::Vector2f invVelocity = -(ball.getDirection() * ball.getVelocity());
  sf::Vector2f newVelocity = cor * invVelocity.rotatedBy(2.f * invVelocity.angleTo(this->orientation.rotatedBy(sf::degrees(-90.f * (side-1)))));

  ball.setVelocity(newVelocity);
  this->setJustBounced(side);

}

//////////////////////////////////////
// Booster => BouncyObject
//////////////////////////////////////

PhysicsObjects::Booster::Booster(const sf::Vector2i& newPos, const sf::Vector2f& newSize, const float newRotation, const float boostExtra)
: pos(newPos), size(newSize), rotation(newRotation), boostExtra(boostExtra), justBoosted(false) {
  this->setOrientation(sf::Vector2f(1,0).rotatedBy(sf::degrees(90.f - newRotation)));

  // For the points of the BouncyObject I use a RectangleShape and get its points
  // Also used in build.cpp
  sf::RectangleShape rect(newSize * Globals::unitSize);
  rect.setOrigin(0.5f * Globals::unitSize * newSize);
  rect.setRotation(sf::degrees(newRotation));
  rect.setPosition(static_cast<sf::Vector2f>(newPos));
  sf::Transform transform = rect.getTransform();
  this->setPoints({
    transform.transformPoint(rect.getPoint(1)),
    transform.transformPoint(rect.getPoint(2)),
    transform.transformPoint(rect.getPoint(3)),
    transform.transformPoint(rect.getPoint(0))
  });
}

void PhysicsObjects::Booster::boost(PhysicsObjects::Ball& ball) {
  // This adds boosterExtra of the speed of the ball, rotated to face the arrow's direction
  const sf::Vector2f ARROW_DIRECTION = this->getOrientation().normalized();

  const float BEGIN_VELOCITY = ball.getVelocity();

  ball.setVelocity((ball.getVelocity() * ball.getDirection()) + boostExtra * ball.getVelocity() * ARROW_DIRECTION);

  this->setJustBoosted(true);

  // Play a sound depending on whether the ball accelerates or slows down
  sf::SoundBuffer boostBuffer;
  if (BEGIN_VELOCITY < ball.getVelocity()) {
    if (!boostBuffer.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("audio/boost.wav"))) {
      throw std::runtime_error("Couldn't load the boost sound.");
    }
  } else {
    if (!boostBuffer.loadFromFile(std::filesystem::path(RESOURCES_PATH).append("audio/slower.wav"))) {
      throw std::runtime_error("Couldn't load the boost slower sound.");
    }
  }
  Globals::threads.emplace_back(playSound, boostBuffer);
  Globals::threads.back().detach();
}