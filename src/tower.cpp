#include "tower.hpp"
#include "constants.hpp"
#include <cmath>

Tower::Tower(Type type, int x, int y) : type_(type), position_(x* CELL_SIZE + CELL_SIZE / 2, y* CELL_SIZE + CELL_SIZE / 2), attackTimer_(0.0f) {
    if (type_ == ARCHER) {
        damage_ = 10;
        range_ = 150.0f;
        attackSpeed_ = 1.0f;
    }
    else if (type_ == MAGE) {
        damage_ = 20;
        range_ = 200.0f;
        attackSpeed_ = 2.0f;
    }
    else { // ARTILLERY
        damage_ = 50;
        range_ = 100.0f;
        attackSpeed_ = 3.0f;
    }
    attackCooldown_ = attackSpeed_;
}

void Tower::update(float deltaTime, std::vector<std::shared_ptr<Enemy>>& enemies) {
    attackTimer_ += deltaTime;
    if (attackTimer_ >= attackCooldown_) {
        for (auto& enemy : enemies) {
            if (!enemy->isAlive()) continue;
            sf::Vector2f enemyPos = enemy->getPosition();
            float distance = std::sqrt(std::pow(enemyPos.x - position_.x, 2) + std::pow(enemyPos.y - position_.y, 2));
            if (distance <= range_) {
                enemy->takeDamage(damage_);
                attackTimer_ = 0.0f;
                break;
            }
        }
    }
}

void Tower::draw(sf::RenderWindow& window) {
    sf::RectangleShape shape(sf::Vector2f(CELL_SIZE / 2, CELL_SIZE / 2));
    shape.setPosition(position_.x - CELL_SIZE / 4, position_.y - CELL_SIZE / 4);
    if (type_ == ARCHER) {
        shape.setFillColor(sf::Color::Green);
    }
    else if (type_ == MAGE) {
        shape.setFillColor(sf::Color(128, 0, 128));
    }
    else { // ARTILLERY
        shape.setFillColor(sf::Color(139, 69, 19));
    }
    window.draw(shape);

    sf::CircleShape rangeCircle(range_);
    rangeCircle.setPosition(position_.x - range_, position_.y - range_);
    rangeCircle.setFillColor(sf::Color(255, 255, 255, 50));
    window.draw(rangeCircle);
}

int Tower::getCost() const {
    if (type_ == ARCHER) return 50;
    if (type_ == MAGE) return 100;
    return 200; // ARTILLERY
}

int Tower::getDamage() const { return damage_; }
float Tower::getRange() const { return range_; }
float Tower::getAttackSpeed() const { return attackSpeed_; }