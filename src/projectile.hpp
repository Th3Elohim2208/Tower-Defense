#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <SFML/Graphics.hpp>
#include "enemy.hpp"
#include <memory>

class Projectile {
public:
    Projectile(sf::Vector2f startPos, std::shared_ptr<Enemy> target, int damage, float speed, sf::Color color);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    bool hasReachedTarget() const;
    std::shared_ptr<Enemy> getTarget() const { return target_; }
    int getDamage() const { return damage_; }

private:
    sf::Vector2f position_;
    std::shared_ptr<Enemy> target_;
    int damage_;
    float speed_;
    sf::Color color_;
    sf::CircleShape shape_;
};

#endif