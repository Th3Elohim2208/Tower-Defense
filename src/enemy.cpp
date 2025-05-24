#include "enemy.hpp"
#include "map.hpp"
#include "constants.hpp"
#include <cmath>
#include <limits>

Enemy::Enemy(Type type)
    : pathIndex_(0), removeWithoutGold_(false), pathVersion_(0), type_(type) {
    position_ = sf::Vector2f(0, 0);
    shape_.setRadius(CELL_SIZE / 4);
    shape_.setOrigin(CELL_SIZE / 8, CELL_SIZE / 8);

    switch (type_) {
    case OGRE:
        health_ = 100;
        speed_ = 50.0f;
        arrowResistance_ = 0.5f; // Resistente: 50% del daño
        magicResistance_ = 1.5f; // Débil: 150% del daño
        artilleryResistance_ = 1.5f; // Débil: 150% del daño
        shape_.setFillColor(sf::Color(0, 100, 0));
        break;
    case DARK_ELF:
        health_ = 80;
        speed_ = 100.0f;
        arrowResistance_ = 1.5f; // Débil: 150% del daño
        magicResistance_ = 0.5f; // Resistente: 50% del daño
        artilleryResistance_ = 1.5f; // Débil: 150% del daño
        shape_.setFillColor(sf::Color(128, 0, 128));
        break;
    case HARPY:
        health_ = 60;
        speed_ = 75.0f;
        arrowResistance_ = 1.0f; // Normal: 100% del daño
        magicResistance_ = 1.0f; // Normal: 100% del daño
        artilleryResistance_ = 0.0f; // Inmune: 0% del daño
        shape_.setFillColor(sf::Color(255, 165, 0));
        break;
    case MERCENARY:
        health_ = 90;
        speed_ = 60.0f;
        arrowResistance_ = 0.5f; // Resistente: 50% del daño
        magicResistance_ = 1.5f; // Débil: 150% del daño
        artilleryResistance_ = 0.5f; // Resistente: 50% del daño
        shape_.setFillColor(sf::Color::Red);
        break;
    }
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
        float step = speed_ * deltaTime;

        if (distance <= step) {
            position_ = target; // Llegamos al punto exacto
            pathIndex_++;
        }
        else {
            direction /= distance; // Normalizamos la dirección
            position_ += direction * step; // Movimiento suave
        }
    }
}

void Enemy::draw(sf::RenderWindow& window) {
    shape_.setPosition(position_.x - CELL_SIZE / 4, position_.y - CELL_SIZE / 4);
    window.draw(shape_);
}

sf::Vector2f Enemy::getPosition() const { return position_; }
int Enemy::getHealth() const { return health_; }

void Enemy::takeDamage(int damage, Tower::Type towerType) {
    float effectiveDamage = static_cast<float>(damage);
    switch (towerType) {
    case Tower::ARCHER:
        effectiveDamage *= arrowResistance_;
        break;
    case Tower::MAGE:
        effectiveDamage *= magicResistance_;
        break;
    case Tower::ARTILLERY:
        effectiveDamage *= artilleryResistance_;
        break;
    }
    health_ -= static_cast<int>(effectiveDamage);
    if (health_ < 0) health_ = 0;
}

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
Enemy::Type Enemy::getType() const { // Definición correcta con el tipo de retorno adecuado
    return type_;
}