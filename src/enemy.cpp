#include "enemy.hpp"
#include "map.hpp"
#include "constants.hpp"

Enemy::Enemy(float speed) : speed_(speed), health_(100), pathIndex_(0), removeWithoutGold_(false), pathVersion_(0) {
    position_ = sf::Vector2f(0, 0);
}

void Enemy::setPath(const std::vector<sf::Vector2i>& path) {
    path_.clear();
    for (const auto& point : path) {
        path_.push_back(sf::Vector2f(point.x * CELL_SIZE + CELL_SIZE / 2, point.y * CELL_SIZE + CELL_SIZE / 2));
    }
    if (position_ == sf::Vector2f(0, 0) && !path_.empty()) {
        position_ = path_[0];
        pathIndex_ = 0;
    }
    else if (!path_.empty()) {
        float minDistance = std::numeric_limits<float>::max();
        size_t closestIndex = 0;
        for (size_t i = 0; i < path_.size(); ++i) {
            sf::Vector2f target = path_[i];
            float distance = std::sqrt(std::pow(target.x - position_.x, 2) + std::pow(target.y - position_.y, 2));
            if (distance < minDistance) {
                minDistance = distance;
                closestIndex = i;
            }
        }
        pathIndex_ = closestIndex;
    }
}

void Enemy::update(float deltaTime) {
    if (pathIndex_ < path_.size() - 1) {
        sf::Vector2f target = path_[pathIndex_ + 1];
        sf::Vector2f direction = target - position_;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (distance < speed_ * deltaTime) {
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
std::vector<sf::Vector2f> Enemy::getPath() const { return path_; }
bool Enemy::isOnCurrentPath(const std::vector<sf::Vector2i>& currentPath) const {
    sf::Vector2i currentCell(static_cast<int>(position_.x / CELL_SIZE), static_cast<int>(position_.y / CELL_SIZE));
    for (const auto& point : currentPath) {
        if (point == currentCell) return true;
    }
    return false;
}

void Enemy::markForRemovalDueToPathFailure() {
    removeWithoutGold_ = true;
}

bool Enemy::shouldRemoveWithoutGold() const {
    return removeWithoutGold_;
}