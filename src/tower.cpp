#include "tower.hpp"
#include "enemy.hpp"

const float CELL_SIZE = 60.0f;

Tower::Tower(Type type, int x, int y) : type_(type), x_(x), y_(y), cooldown_(0.0f) {
    switch (type) {
    case ARCHER:
        damage_ = 10; range_ = 3; attack_cooldown_ = 1.0f; cost_ = 50;
        break;
    case MAGE:
        damage_ = 20; range_ = 2; attack_cooldown_ = 2.0f; cost_ = 100;
        break;
    case ARTILLERY:
        damage_ = 50; range_ = 1; attack_cooldown_ = 3.0f; cost_ = 200;
        break;
    }
}

void Tower::draw(sf::RenderWindow& window) {
    sf::CircleShape shape(CELL_SIZE / 4);
    shape.setPosition(x_ * CELL_SIZE + CELL_SIZE / 4, y_ * CELL_SIZE + CELL_SIZE / 4);
    shape.setFillColor(type_ == ARCHER ? sf::Color::Green : type_ == MAGE ? sf::Color::Blue : sf::Color::Red);
    window.draw(shape);
}

void Tower::update(float deltaTime, std::vector<std::shared_ptr<Enemy>>& enemies) {
    cooldown_ -= deltaTime;
    if (cooldown_ <= 0.0f) {
        for (auto& enemy : enemies) {
            if (!enemy->isAlive()) continue;
            float dx = (x_ * CELL_SIZE + CELL_SIZE / 2) - enemy->getPosition().x;
            float dy = (y_ * CELL_SIZE + CELL_SIZE / 2) - enemy->getPosition().y;
            float distance = std::sqrt(dx * dx + dy * dy) / CELL_SIZE;
            if (distance <= range_) {
                enemy->takeDamage(damage_);
                cooldown_ = attack_cooldown_;
                break;
            }
        }
    }
}

int Tower::getCost() const { return cost_; }