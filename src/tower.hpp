#ifndef TOWER_HPP
#define TOWER_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Tower {
public:
    enum Type { ARCHER, MAGE, ARTILLERY };
    Tower(Type type, int x, int y);
    void draw(sf::RenderWindow& window);
    void update(float deltaTime, std::vector<std::shared_ptr<class Enemy>>& enemies);
    int getCost() const;
private:
    Type type_;
    int x_, y_;
    int damage_;
    float range_;
    float attack_cooldown_;
    int cost_;
    float cooldown_;
};


#endif