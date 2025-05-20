#include "map.hpp"
#include "enemy.hpp"
#include "constants.hpp"

Map::Map() : grid_(GRID_SIZE, std::vector<int>(GRID_SIZE, 0)) {
    grid_[0][0] = 1; // Entrada
    grid_[9][9] = 2; // Puente
}

bool Map::canPlaceTower(int x, int y) {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) return false;
    return grid_[x][y] == 0; // Permitir en celdas vacías
}

void Map::placeTower(int x, int y, std::shared_ptr<Tower> tower) {
    grid_[x][y] = 3;
    towers_.push_back(tower);
}

void Map::update(float deltaTime, std::vector<std::shared_ptr<Enemy>>& enemies) {
    for (auto& tower : towers_) {
        tower->update(deltaTime, enemies);
    }
}

void Map::draw(sf::RenderWindow& window) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
            cell.setPosition(i * CELL_SIZE + 1, j * CELL_SIZE + 1);
            if (grid_[i][j] == 1) cell.setFillColor(sf::Color::Cyan);
            else if (grid_[i][j] == 2) cell.setFillColor(sf::Color::Magenta);
            else if (grid_[i][j] == 3) cell.setFillColor(sf::Color::White);
            else if (grid_[i][j] == 4) cell.setFillColor(sf::Color(150, 150, 150));
            else cell.setFillColor(sf::Color::White);
            window.draw(cell);
        }
    }
    for (auto& tower : towers_) {
        tower->draw(window);
    }
}

struct Node {
    int x, y;
    float f, g, h;
    Node* parent;
    Node(int x, int y) : x(x), y(y), f(0), g(0), h(0), parent(nullptr) {}
};

struct CompareNode {
    bool operator()(const Node* a, const Node* b) {
        return a->f > b->f;
    }
};

std::vector<sf::Vector2i> Map::findPath(int startX, int startY, int endX, int endY) {
    std::priority_queue<Node*, std::vector<Node*>, CompareNode> openSet;
    std::unordered_map<int, Node*> allNodes;
    std::vector<Node*> closedSet;

    Node* start = new Node(startX, startY);
    openSet.push(start);
    allNodes[startX * GRID_SIZE + startY] = start;

    int dx[] = { 0, 0, 1, -1 };
    int dy[] = { 1, -1, 0, 0 };

    while (!openSet.empty()) {
        Node* current = openSet.top();
        openSet.pop();
        closedSet.push_back(current);

        if (current->x == endX && current->y == endY) {
            std::vector<sf::Vector2i> path;
            while (current) {
                path.push_back(sf::Vector2i(current->x, current->y));
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
            for (auto* node : closedSet) delete node;
            for (auto& pair : allNodes) delete pair.second;
            return path;
        }

        for (int i = 0; i < 4; ++i) {
            int newX = current->x + dx[i];
            int newY = current->y + dy[i];
            if (newX < 0 || newX >= GRID_SIZE || newY < 0 || newY >= GRID_SIZE) continue;
            if (grid_[newX][newY] == 3) continue; // Evitar torres

            float g = current->g + 1.0f;
            float h = std::sqrt((newX - endX) * (newX - endX) + (newY - endY) * (newY - endY));
            float f = g + h;

            int key = newX * GRID_SIZE + newY;
            Node* neighbor = allNodes[key];
            if (!neighbor) {
                neighbor = new Node(newX, newY);
                allNodes[key] = neighbor;
                openSet.push(neighbor);
            }
            else if (g >= neighbor->g) {
                continue;
            }

            neighbor->parent = current;
            neighbor->g = g;
            neighbor->h = h;
            neighbor->f = f;
        }
    }

    std::vector<sf::Vector2i> emptyPath;
    for (auto* node : closedSet) delete node;
    for (auto& pair : allNodes) delete pair.second;
    return emptyPath; // No se encontró camino
}