#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

namespace PhysicsObjects {
  
  class Ball : public sf::Sprite {

  public:

    Ball(sf::Texture& texture, sf::Vector2f mid, float m = 1, float r = 50);
    
    void setMass(const float newMass) {mass = newMass;};
    float getMass() {return mass;};

    void setMidpoint(const sf::Vector2f newMidpoint) {midpoint = newMidpoint;};
    sf::Vector2f getMidpoint() {return midpoint;};

    void setVelocity(sf::Vector2f& newVelocity) {velocityVector = newVelocity;};
    float getVelocity() {return velocityVector.length();};
    sf::Vector2f getDirection() {return velocityVector.normalized();};

    void setRadius(const float newRadius);
    float getRadius() {return radius;};

    /// Applies force F in Newton (kg m s^-2) in the given 2D direction vector
    void applyForce(float deltaTime, float F, sf::Vector2f direction);

    /// Updates the position based on the current velocity.
    void updatePoistion(float deltaTime);

  private:

    const float G = 9.81; // Gravitational acceleration constant

    float mass = 1; // Mass in kg
    sf::Vector2f midpoint;
    sf::Vector2f velocityVector;
    float radius;

  };

  class BouncyObject {
  
  public:

    void setPoints(const std::vector<sf::Vector2f>& newPoints) {points = newPoints;};
    auto& getPoints() {return points;};

    void setOrientation(const sf::Vector2f newOrientation) {orientation = newOrientation;};
    sf::Vector2f& getOrientation() {return orientation;};

    void setCOR(const float newCor) {cor = newCor;};

    /// Checks if the ball is in this object. If so, return the side
    int checkBallCollision(Ball& ball);

    /// Bounces the ball. It calculates the needed force based on the elasticity of the ball and the surface and applies it.
    void bounce(Ball& ball, int side);
  
  private:

    // The points are in the following order:
    // top-left, top-right, bottom-left, bottom-right
    std::vector<sf::Vector2f> points;

    sf::Vector2f orientation = {1, 0};
    float cor; // Coefficient of restitution e. This is the factor with which the ball gets slowed down upon impact.

  };

}

#endif // PHYSICS_H_