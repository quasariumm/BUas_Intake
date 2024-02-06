#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <SFML/System.hpp>

namespace Physics {
  
  class Ball {

  public:

    double getVelocity() {return velocityVector.length();};

    void setRadius(double r) {radius = r;};
    double getRadius() {return radius;};

    /// Applies force F in Newton (kg m s^-2) in the given 2D direction vector
    void applyForce(double deltaTime, double F, sf::Vector2f direction);

  private:

    const double G = 9.81; // Gravitational acceleration constant
    const double COR = 0.8; // Coefficient of restitution e. The chosen value is just an estimate.

    double mass = 1; // Mass in kg
    double x, y;
    sf::Vector2f velocityVector;
    double radius;

  };

  class BouncyObject {
  
  public:
    /// Bounces the ball. It calculates the needed force based on the elasticity of the ball and the surface and applies the needed force.
    void bounce(Ball& ball);
  
  private:
    sf::Vector2f orientation;
    sf::Vector2f normal;

  };

}

#endif // PHYSICS_H_