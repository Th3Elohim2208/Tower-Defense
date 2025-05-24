#ifndef MAP_HPP
#define MAP_HPP
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <unordered_set>
#include "tower.hpp"
#include "enemy.hpp"
#include "constants.hpp"

class Map {
public:
    Map();
    void update(float deltaTime, std::vector<std::shared_ptr<Enemy>>& enemies);
    void draw(sf::RenderWindow& window);
    bool canPlaceTower(int x, int y);
    void placeTower(int x, int y, std::shared_ptr<Tower> tower);
    std::vector<sf::Vector2i> getCurrentPath() const;
    int getPathVersion() const { return pathVersion_; } // Añadir getter para pathVersion_
    std::vector<std::shared_ptr<Tower>>& getTowers() { return towers_; } // Nuevo método

private:
    struct Node {
        int x, y;
        float f, g, h;
        Node* parent;
        Node(int x, int y) : x(x), y(y), f(0), g(0), h(0), parent(nullptr) {}
    };

    struct CompareNode {
        bool operator()(const Node* a, const Node* b) const {
            return a->f > b->f;
        }
    };

    std::vector<std::vector<int>> grid_;
    std::vector<std::shared_ptr<Tower>> towers_;
    std::vector<sf::Vector2i> currentPath_;
    int pathVersion_; // Añadir para rastrear cambios en el camino

    void clearPath();
    void markPath();
    void updatePath();
    std::vector<sf::Vector2i> findPath(int startX, int startY, int endX, int endY);
    std::vector<sf::Vector2i> findPathBasedOnPosition(sf::Vector2f enemyPos, int endX, int endY);
};

#endif