#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class Enemy {
public:
    Enemy(float speed);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    sf::Vector2f getPosition() const;
    int getHealth() const;
    void takeDamage(int damage);
    bool isAlive() const;
    bool hasReachedEnd() const;
    std::vector<sf::Vector2f> getPath() const;
    bool isOnCurrentPath(const std::vector<sf::Vector2i>& currentPath) const;
    void setPath(const std::vector<sf::Vector2i>& path);
    void markForRemovalDueToPathFailure();
    bool shouldRemoveWithoutGold() const;
    int getPathVersion() const { return pathVersion_; } // Añadir getter
    void setPathVersion(int version) { pathVersion_ = version; } // Añadir setter
private:
    sf::Vector2f position_;
    std::vector<sf::Vector2f> path_;
    size_t pathIndex_;
    float speed_;
    int health_;
    bool removeWithoutGold_;
    int pathVersion_; // Añadir para rastrear la versión del camino
};

#endif