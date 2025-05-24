#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "constants.hpp" 
#include <SFML/Graphics.hpp>
#include <vector>
#include "tower.hpp" // Mantenemos para Tower::Type

class Enemy {
public:
    enum Type { OGRE, DARK_ELF, HARPY, MERCENARY };
    Enemy(Type type);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    sf::Vector2f getPosition() const;
    int getHealth() const;
    void takeDamage(int damage, Tower::Type towerType); // Ajustamos a Tower::Type
    bool isAlive() const;
    bool hasReachedEnd() const;
    std::vector<sf::Vector2f> getPath() const;
    bool isOnCurrentPath(const std::vector<sf::Vector2i>& currentPath) const;
    void setPath(const std::vector<sf::Vector2i>& path);
    void markForRemovalDueToPathFailure();
    bool shouldRemoveWithoutGold() const;
    int getPathVersion() const { return pathVersion_; }
    void setPathVersion(int version) { pathVersion_ = version; }
    Type getType() const; // Declaración correcta de getType()

private:
    sf::Vector2f position_;
    std::vector<sf::Vector2f> path_;
    size_t pathIndex_;
    float speed_;
    int health_;
    bool removeWithoutGold_;
    int pathVersion_;
    Type type_;
    float arrowResistance_;
    float magicResistance_;
    float artilleryResistance_;
    sf::CircleShape shape_;
};

#endif