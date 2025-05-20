#ifndef MAP_HPP
#define MAP_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <queue>
#include <unordered_map>
#include "tower.hpp"

class Map {
public:
    Map();
    bool canPlaceTower(int x, int y);
    void placeTower(int x, int y, std::shared_ptr<Tower> tower);
    void update(float deltaTime, std::vector<std::shared_ptr<class Enemy>>& enemies);
    void draw(sf::RenderWindow& window);
    std::vector<sf::Vector2i> findPath(int startX, int startY, int endX, int endY); // Nuevo método A*
private:
    std::vector<std::vector<int>> grid_; // 0: vacío, 1: entrada, 2: puente, 3: torre, 4: camino
    std::vector<std::shared_ptr<Tower>> towers_;
};

#endif