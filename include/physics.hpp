#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

namespace PhysicsObjects {
  
  class Ball : public sf::Sprite {

  public:

    /**
     * @brief Construct a new Ball object
     * 
     * @param texture The texture for the ball
     * @param mid Midpoint
     * @param m Mass
     * @param r Radius
     */
    Ball(sf::Texture& texture, sf::Vector2f mid, float m = 1, float r = 50);
    
    /**
     * @brief Set the mass
     * 
     * @param newMass The new mass
     */
    void setMass(const float newMass) {mass = newMass;};

    /**
     * @brief Get the mass
     *
     * @return float The mass
     */
    float getMass() {return mass;};

    /**
     * @brief Set the midpoint
     * 
     * @param newMidpoint The new midpoint
     */
    void setMidpoint(const sf::Vector2f newMidpoint) {midpoint = newMidpoint;};

    /**
     * @brief Get the midpoint
     * 
     * @return sf::Vector2f& A reference to the midpoint vector 
     */
    sf::Vector2f& getMidpoint() {return midpoint;};

    /**
     * @brief Set the velocity
     * 
     * @param newVelocity The new velocity
     */
    void setVelocity(const sf::Vector2f& newVelocity) {velocityVector = newVelocity;};

    /**
     * @brief Get the velocity
     * 
     * @return float The velocity (length of the velocityVector).
     */
    float getVelocity() {return velocityVector.length();};
    
    /**
     * @brief Get the direction
     * 
     * @return sf::Vector2f The direction in wich the ball moves. It is velocityVector normalized
     */
    sf::Vector2f getDirection();

    /**
     * @brief Set the radius
     * 
     * @param newRadius The new radius
     */
    void setRadius(const float newRadius);

    /**
     * @brief Get the radius
     * 
     * @return float The radius
     */
    float getRadius() {return radius;};

    /**
     * @brief Applies force F in Newton (kg m s^-2) in the given 2D direction vector
     * 
     * @param deltaTime The time passed between the last frame and now
     * @param F The force
     * @param direction The direction
     */
    void applyForce(float deltaTime, float F, sf::Vector2f direction);

    /**
     * @brief Updates the position based on the current velocity.
     *
     * @param deltaTime The time passed between the last frame and now
     */
    void updatePoistion(float deltaTime);

  private:

    const float G = 9.81; // Gravitational acceleration constant

    float mass = 1; // Mass in kg
    sf::Vector2f midpoint;
    sf::Vector2f velocityVector;
    float radius;

  };

  // TODO: document
  class BouncyObject {
  
  public:

    void setPoints(const std::vector<sf::Vector2f>& newPoints) {points = newPoints;};
    std::vector<sf::Vector2f>& getPoints() {return points;};

    void setOrientation(const sf::Vector2f newOrientation) {orientation = newOrientation;};
    sf::Vector2f& getOrientation() {return orientation;};

    void setCOR(const float newCor) {cor = newCor;};

    /// Checks if the ball is in this object. If so, return the side
    int checkBallCollision(Ball& ball, float unitSize);

    /// Bounces the ball. It calculates the needed force based on the elasticity of the ball and the surface and applies it.
    void bounce(Ball& ball, int side);
  
    void setJustBounced(short jb) {justBounced = jb;};
    short getJustBounced() {return justBounced;};

  private:

    // The points are in the following order:
    // top-left, top-right, bottom-left, bottom-right
    std::vector<sf::Vector2f> points;

    sf::Vector2f orientation = {1, 0};
    float cor; // Coefficient of restitution e. This is the factor with which the ball gets slowed down upon impact.

    short justBounced = -1;

  };

}

#endif // PHYSICS_H_