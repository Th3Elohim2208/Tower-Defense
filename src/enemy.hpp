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
private:
    float speed_;
    int health_;
    sf::Vector2f position_;
    std::vector<sf::Vector2f> path_;
    size_t pathIndex_;
};

extern const float CELL_SIZE;

#endif