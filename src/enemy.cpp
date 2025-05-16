#include "enemy.hpp"

const float CELL_SIZE = 60.0f;

Enemy::Enemy(float speed) : speed_(speed), health_(100), pathIndex_(0) {
    path_.push_back(sf::Vector2f(0 * CELL_SIZE + CELL_SIZE / 2, 0 * CELL_SIZE + CELL_SIZE / 2)); // (0,0)
    for (int y = 1; y < GRID_SIZE; ++y) {
        path_.push_back(sf::Vector2f(0 * CELL_SIZE + CELL_SIZE / 2, y * CELL_SIZE + CELL_SIZE / 2));
    }
    for (int x = 1; x < GRID_SIZE; ++x) {
        path_.push_back(sf::Vector2f(x * CELL_SIZE + CELL_SIZE / 2, 9 * CELL_SIZE + CELL_SIZE / 2));
    }
    position_ = path_[0];
}

void Enemy::update(float deltaTime) {
    if (pathIndex_ < path_.size() - 1) {
        sf::Vector2f target = path_[pathIndex_ + 1];
        sf::Vector2f direction = target - position_;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (distance < speed_* deltaTime) {
            pathIndex_++;
            position_ = target;
        }
        else {
            direction /= distance;
            position_ += direction * speed_ * deltaTime;
        }
    }
}

void Enemy::draw(sf::RenderWindow& window) {
    sf::CircleShape shape(CELL_SIZE / 4);
    shape.setPosition(position_.x - CELL_SIZE / 4, position_.y - CELL_SIZE / 4);
    shape.setFillColor(sf::Color::Red);
    window.draw(shape);
}

sf::Vector2f Enemy::getPosition() const { return position_; }
int Enemy::getHealth() const { return health_; }
void Enemy::takeDamage(int damage) { health_ -= damage; }
bool Enemy::isAlive() const { return health_ > 0; }
bool Enemy::hasReachedEnd() const { return pathIndex_ >= path_.size() - 1; }