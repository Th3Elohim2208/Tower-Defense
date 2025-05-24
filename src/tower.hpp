#ifndef TOWER_HPP
#define TOWER_HPP

#include "constants.hpp"
#include <SFML/Graphics.hpp>
#include <memory> // Añadimos para shared_ptr
#include <vector>

// Declaraciones forward
class Enemy;
class Projectile;

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
    int getUpgradeLevel() const { return upgradeLevel_; }
    bool canUpgrade() const { return upgradeLevel_ < 3; }
    int getUpgradeCost() const;
    bool upgrade();
    int getX() const { return static_cast<int>(position_.x / CELL_SIZE); }
    int getY() const { return static_cast<int>(position_.y / CELL_SIZE); }

private:
    Type type_;
    sf::Vector2f position_;
    float attackCooldown_;
    float attackTimer_;
    int damage_;
    float range_;
    float attackSpeed_;
    int upgradeLevel_;
    sf::RectangleShape shape;
    sf::Text levelText;
    sf::Font font;
    std::vector<std::shared_ptr<Projectile>> projectiles_;

    float specialAttackCooldown;
    float specialAttackInterval;
    float specialAttackChance;
    void performSpecialAttack(std::vector<std::shared_ptr<Enemy>>& enemies);

    void initializeAttributes();
    void updateLevelText();
};

#endif