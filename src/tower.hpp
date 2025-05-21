#ifndef TOWER_HPP
#define TOWER_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include "enemy.hpp"

class Tower {
public:
    enum Type { ARCHER, MAGE, ARTILLERY };
    Tower(Type type, int x, int y);
    void update(float deltaTime, std::vector<std::shared_ptr<Enemy>>& enemies);
    void draw(sf::RenderWindow& window);
    int getCost() const;
    int getDamage() const;
    float getRange() const;
    float getAttackSpeed() const;
    Type getType() const { return type_; }
private:
    Type type_;
    sf::Vector2f position_;
    float attackCooldown_;
    float attackTimer_;
    int damage_;
    float range_;
    float attackSpeed_;
};

#endif