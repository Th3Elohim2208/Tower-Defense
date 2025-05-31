#include "enemy.hpp"
#include "map.hpp"
#include "constants.hpp"
#include <cmath>
#include <limits>

Enemy::Enemy(Type type, sf::Font& font)
    : pathIndex_(0), removeWithoutGold_(false), pathVersion_(0), type_(type), fitness_(0.0f) {
    position_ = sf::Vector2f(0, 0);
    shape_.setRadius(CELL_SIZE / 4);
    shape_.setOrigin(CELL_SIZE / 8, CELL_SIZE / 8);

    switch (type_) {
    case OGRE:
        health_ = 100;
        speed_ = 50.0f;
        arrowResistance_ = 0.5f;
        magicResistance_ = 1.5f;
        artilleryResistance_ = 1.5f;
        shape_.setFillColor(sf::Color(0, 100, 0));
        break;
    case DARK_ELF:
        health_ = 80;
        speed_ = 100.0f;
        arrowResistance_ = 1.5f;
        magicResistance_ = 0.5f;
        artilleryResistance_ = 1.5f;
        shape_.setFillColor(sf::Color(128, 0, 128));
        break;
    case HARPY:
        health_ = 60;
        speed_ = 75.0f;
        arrowResistance_ = 1.0f;
        magicResistance_ = 1.0f;
        artilleryResistance_ = 0.0f;
        shape_.setFillColor(sf::Color(255, 165, 0));
        break;
    case MERCENARY:
        health_ = 90;
        speed_ = 60.0f;
        arrowResistance_ = 0.5f;
        magicResistance_ = 1.5f;
        artilleryResistance_ = 0.5f;
        shape_.setFillColor(sf::Color::Red);
        break;
    }

    fitnessText_.setFont(font);
    fitnessText_.setCharacterSize(12);
    fitnessText_.setFillColor(sf::Color::White);
    float initialFitness = calculateFitness(0.0f, static_cast<float>(health_));
    fitnessText_.setString(std::to_string(static_cast<int>(initialFitness)));
    // Calcular el origen después de establecer el string para centrar el texto
    fitnessText_.setOrigin(fitnessText_.getLocalBounds().width / 2, fitnessText_.getLocalBounds().height / 2 + fitnessText_.getLocalBounds().top);
    // Posicionar el texto con un desplazamiento de 15 píxeles a la derecha
    fitnessText_.setPosition(position_.x - 10, position_.y - 10);
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
            position_ = target;
            pathIndex_++;
        }
        else {
            direction /= distance;
            position_ += direction * step;
        }
    }
    // Actualizar la posición del texto con desplazamiento de 15 píxeles a la derecha
    fitnessText_.setPosition(position_.x - 10, position_.y - 10);
    float currentDistance = 0.0f;
    for (size_t i = 0; i < pathIndex_; ++i) {
        float dx = path_[i + 1].x - path_[i].x;
        float dy = path_[i + 1].y - path_[i].y;
        currentDistance += std::sqrt(dx * dx + dy * dy);
    }
    float currentFitness = calculateFitness(currentDistance, static_cast<float>(health_));
    fitness_ = currentFitness;
    fitnessText_.setString(std::to_string(static_cast<int>(currentFitness)));
    // Recalcular el origen después de actualizar el string para mantener el texto centrado
    fitnessText_.setOrigin(fitnessText_.getLocalBounds().width / 2, fitnessText_.getLocalBounds().height / 2 + fitnessText_.getLocalBounds().top);
}

void Enemy::draw(sf::RenderWindow& window) {
    shape_.setPosition(position_.x - CELL_SIZE / 4, position_.y - CELL_SIZE / 4);
    window.draw(shape_);
    window.draw(fitnessText_);
}

sf::Vector2f Enemy::getPosition() const { return position_; }
int Enemy::getHealth() const { return health_; }
void Enemy::setHealth(int health) { health_ = health; }
float Enemy::getSpeed() const { return speed_; }
void Enemy::setSpeed(float speed) { speed_ = speed; }

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
Enemy::Type Enemy::getType() const {
    return type_;
}

float Enemy::calculateFitness(float distance, float remainingHealth) {
    float fitness;
    switch (type_) {
    case OGRE:
        fitness = 0.3f * distance + 0.7f * remainingHealth;
        break;
    case DARK_ELF:
        fitness = 0.7f * distance + 0.3f * remainingHealth;
        break;
    case HARPY:
        fitness = 0.5f * distance + 0.5f * remainingHealth;
        break;
    case MERCENARY:
        fitness = 0.4f * distance + 0.6f * remainingHealth;
        break;
    default:
        fitness = remainingHealth;
    }
    return fitness;
}