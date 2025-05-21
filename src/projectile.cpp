#include "projectile.hpp"
#include <cmath>

Projectile::Projectile(sf::Vector2f startPos, std::shared_ptr<Enemy> target, int damage, float speed, sf::Color color)
    : position_(startPos), target_(target), damage_(damage), speed_(speed), color_(color) {
    shape_.setRadius(3.f); // Tamaño pequeño para el "punto"
    shape_.setFillColor(color);
    shape_.setPosition(position_);
}

void Projectile::update(float deltaTime) {
    if (!target_ || !target_->isAlive()) return;

    sf::Vector2f targetPos = target_->getPosition();
    sf::Vector2f direction = targetPos - position_;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (distance < speed_ * deltaTime) {
        position_ = targetPos;
    }
    else {
        direction /= distance;
        position_ += direction * speed_ * deltaTime;
    }
    shape_.setPosition(position_);
}

void Projectile::draw(sf::RenderWindow& window) {
    window.draw(shape_);
}

bool Projectile::hasReachedTarget() const {
    if (!target_ || !target_->isAlive()) return true;
    sf::Vector2f targetPos = target_->getPosition();
    float distance = std::sqrt(std::pow(targetPos.x - position_.x, 2) + std::pow(targetPos.y - position_.y, 2));
    return distance < 5.f; // Consideramos que llegó si está muy cerca
}