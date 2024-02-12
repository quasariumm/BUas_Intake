#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>

namespace Physics {
  
  class Ball : public sf::Sprite {

  public:

    Ball(sf::Texture texture, double m = 1, double r = 50, int xCoord = 0, int yCoord = 0):
      mass(m), radius(r), x(xCoord), y(yCoord), Ball::Sprite(texture) {};
    
    void setMass(double newMass) {mass = newMass;};
    double getMass() {return mass;};

    void setCoordinates(int newX, int newY) {x = newX; y = newY;};
    sf::Vector2i getCoordinates() {return sf::Vector2i(x, y);};

    double getVelocity() {return velocityVector.length();};
    sf::Vector2f getDirection() {return velocityVector.normalized();};

    void setRadius(double newRadius) {radius = newRadius;};
    double getRadius() {return radius;};

    /// Applies force F in Newton (kg m s^-2) in the given 2D direction vector
    void applyForce(double deltaTime, double F, sf::Vector2f direction);

    /// Updates the position based on the current velocity.
    void updatePoistion(double deltaTime);

  private:

    const double G = 9.81; // Gravitational acceleration constant

    double mass = 1; // Mass in kg
    int x, y; // In pixels
    sf::Vector2f velocityVector;
    double radius;

  };

  class BouncyObject {
  
  public:

    void setPoints(std::vector<sf::Vector2u> newPoints) {points = newPoints;};
    auto& getPoints() {return points;};

    void setOrientation(sf::Vector2f newOrientation) {orientation = newOrientation;};
    sf::Vector2f& getOrientation() {return orientation;};

    void setCOR(double newCor) {cor = newCor;};

    /// Checks if the ball is in this object.
    void checkBallCollision(Ball& ball);

    /// Bounces the ball. It calculates the needed force based on the elasticity of the ball and the surface and applies it.
    void bounce(Ball& ball);
  
  private:

    // The points are in the following order:
    // top-left, top-right, bottom-left, bottom-right
    std::vector<sf::Vector2u> points;

    sf::Vector2f orientation;
    double cor; // Coefficient of restitution e. This is the factor with which the ball gets slowed down upon impact.

  };

}

#endif // PHYSICS_H_