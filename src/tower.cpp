#include "tower.hpp"
#include "enemy.hpp" // Incluimos la definición completa de Enemy
#include "projectile.hpp" // Incluimos la definición completa de Projectile
#include "constants.hpp"
#include <cmath>
#include <sstream>
#include <cstdlib>
#include <memory> // Añadimos para shared_ptr

Tower::Tower(Type type, int x, int y) : type_(type), position_(x* CELL_SIZE + CELL_SIZE / 2, y* CELL_SIZE + CELL_SIZE / 2), attackTimer_(0.0f), upgradeLevel_(0), specialAttackCooldown(0.f) {
    if (!font.loadFromFile("arial.ttf")) {
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            // Si no se carga, el texto no se mostrará
        }
    }

    initializeAttributes();
    shape.setSize(sf::Vector2f(CELL_SIZE / 2, CELL_SIZE / 2));
    if (type_ == ARCHER) shape.setFillColor(sf::Color::Green);
    else if (type_ == MAGE) shape.setFillColor(sf::Color::Blue);
    else shape.setFillColor(sf::Color(139, 69, 19));
    shape.setPosition(position_.x - CELL_SIZE / 4, position_.y - CELL_SIZE / 4);
    levelText.setFont(font);
    levelText.setCharacterSize(12);
    levelText.setFillColor(sf::Color::Black);
    levelText.setPosition(position_.x - 6, position_.y - 6);
    updateLevelText();

    specialAttackInterval = 5.0f;
    if (type_ == ARCHER) specialAttackChance = 0.2f;
    else if (type_ == MAGE) specialAttackChance = 0.15f;
    else specialAttackChance = 0.1f;
}

void Tower::initializeAttributes() {
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
    else {
        damage_ = 50;
        range_ = 100.0f;
        attackSpeed_ = 3.0f;
    }
    attackCooldown_ = attackSpeed_;
}

void Tower::update(float deltaTime, std::vector<std::shared_ptr<Enemy>>& enemies) {
    attackTimer_ += deltaTime;
    specialAttackCooldown -= deltaTime;

    // Actualizar proyectiles
    for (auto it = projectiles_.begin(); it != projectiles_.end();) {
        (*it)->update(deltaTime);
        if ((*it)->hasReachedTarget()) {
            auto target = (*it)->getTarget();
            if (target && target->isAlive()) {
                target->takeDamage((*it)->getDamage(), type_); // Usamos Tower::Type
            }
            it = projectiles_.erase(it);
        }
        else {
            ++it;
        }
    }

    // Disparo normal
    if (attackTimer_ >= attackCooldown_) {
        for (auto& enemy : enemies) {
            if (!enemy->isAlive()) continue;
            sf::Vector2f enemyPos = enemy->getPosition();
            float distance = std::sqrt(std::pow(enemyPos.x - position_.x, 2) + std::pow(enemyPos.y - position_.y, 2));
            if (distance <= range_) {
                sf::Color projectileColor;
                if (type_ == ARCHER) projectileColor = sf::Color::Green;
                else if (type_ == MAGE) projectileColor = sf::Color::Blue;
                else projectileColor = sf::Color(139, 69, 19);
                auto projectile = std::make_shared<Projectile>(position_, enemy, damage_, 200.f, projectileColor);
                projectiles_.push_back(projectile);
                attackTimer_ = 0.0f;
                break;
            }
        }
    }

    // Ataque especial
    if (specialAttackCooldown <= 0) {
        if ((rand() % 100) / 100.0f < specialAttackChance) {
            performSpecialAttack(enemies);
        }
        specialAttackCooldown = specialAttackInterval;
    }
}

void Tower::performSpecialAttack(std::vector<std::shared_ptr<Enemy>>& enemies) {
    for (auto& enemy : enemies) {
        if (!enemy->isAlive()) continue;
        sf::Vector2f enemyPos = enemy->getPosition();
        float distance = std::sqrt(std::pow(enemyPos.x - position_.x, 2) + std::pow(enemyPos.y - position_.y, 2));
        if (distance <= range_) {
            auto projectile = std::make_shared<Projectile>(position_, enemy, damage_ * 2, 200.f, sf::Color::Cyan);
            projectiles_.push_back(projectile);
        }
    }
}

void Tower::draw(sf::RenderWindow& window) {
    sf::CircleShape rangeCircle(range_);
    rangeCircle.setPosition(position_.x - range_, position_.y - range_);
    rangeCircle.setFillColor(sf::Color(255, 255, 255, 50));
    window.draw(rangeCircle);
    window.draw(shape);
    window.draw(levelText);
    for (auto& projectile : projectiles_) {
        projectile->draw(window);
    }
}

int Tower::getCost() const {
    if (type_ == ARCHER) return 50;
    if (type_ == MAGE) return 100;
    return 200;
}

int Tower::getDamage() const { return damage_; }
float Tower::getRange() const { return range_; }
float Tower::getAttackSpeed() const { return attackSpeed_; }

int Tower::getUpgradeCost() const {
    if (type_ == ARCHER) {
        switch (upgradeLevel_) {
        case 0: return 50;
        case 1: return 100;
        case 2: return 150;
        default: return 0;
        }
    }
    else if (type_ == MAGE) {
        switch (upgradeLevel_) {
        case 0: return 100;
        case 1: return 200;
        case 2: return 300;
        default: return 0;
        }
    }
    else {
        switch (upgradeLevel_) {
        case 0: return 200;
        case 1: return 300;
        case 2: return 400;
        default: return 0;
        }
    }
}

bool Tower::upgrade() {
    if (upgradeLevel_ >= 3) return false;

    if (type_ == ARCHER) {
        switch (upgradeLevel_) {
        case 0:
            damage_ = 20;
            range_ = 200.0f;
            attackSpeed_ = 0.75f;
            break;
        case 1:
            damage_ = 35;
            range_ = 225.0f;
            attackSpeed_ = 0.65f;
            break;
        case 2:
            damage_ = 50;
            range_ = 250.0f;
            attackSpeed_ = 0.50f;
            break;
        }
    }
    else if (type_ == MAGE) {
        switch (upgradeLevel_) {
        case 0:
            damage_ = 35;
            range_ = 225.0f;
            attackSpeed_ = 1.5f;
            break;
        case 1:
            damage_ = 50;
            range_ = 250.0f;
            attackSpeed_ = 1.25f;
            break;
        case 2:
            damage_ = 75;
            range_ = 275.0f;
            attackSpeed_ = 1.0f;
            break;
        }
    }
    else {
        switch (upgradeLevel_) {
        case 0:
            damage_ = 75;
            range_ = 125.0f;
            attackSpeed_ = 2.5f;
            break;
        case 1:
            damage_ = 100;
            range_ = 150.0f;
            attackSpeed_ = 2.0f;
            break;
        case 2:
            damage_ = 125;
            range_ = 175.0f;
            attackSpeed_ = 1.5f;
            break;
        }
    }
    attackCooldown_ = attackSpeed_;
    upgradeLevel_++;
    updateLevelText();
    return true;
}

void Tower::updateLevelText() {
    std::stringstream ss;
    ss << upgradeLevel_;
    levelText.setString(ss.str());
}