#include "map.hpp"
#include "tower.hpp"
#include "enemy.hpp"
#include "constants.hpp"
#include <cmath>

Map::Map() : grid_(GRID_SIZE, std::vector<int>(GRID_SIZE, 0)), pathVersion_(0) {
    grid_[0][0] = 1; // Entrada
    grid_[7][7] = 2; // Puente
    updatePath();
}

void Map::clearPath() {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (grid_[i][j] == 4) {
                grid_[i][j] = 0;
            }
        }
    }
}

void Map::markPath() {
    for (const auto& point : currentPath_) {
        int x = point.x;
        int y = point.y;
        if (grid_[x][y] != 1 && grid_[x][y] != 2 && grid_[x][y] != 3) {
            grid_[x][y] = 4;
        }
    }
}

void Map::updatePath() {
    clearPath();
    currentPath_ = findPath(0, 0, 7, 7);
    pathVersion_++;
    markPath();
}

bool Map::canPlaceTower(int x, int y) {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) return false;
    if (grid_[x][y] != 0 && grid_[x][y] != 4) return false;

    int originalValue = grid_[x][y];
    grid_[x][y] = 3;
    std::vector<sf::Vector2i> tempPath = findPath(0, 0, 7, 7);
    grid_[x][y] = originalValue;

    if (tempPath.empty()) return false;
    return true;
}

void Map::placeTower(int x, int y, std::shared_ptr<Tower> tower) {
    bool wasOnPath = (grid_[x][y] == 4);
    grid_[x][y] = 3;
    towers_.push_back(tower);
    if (wasOnPath) {
        updatePath();
    }
}

void Map::update(float deltaTime, std::vector<std::shared_ptr<Enemy>>& enemies) {
    for (auto& tower : towers_) {
        tower->update(deltaTime, enemies);
    }
    for (auto it = enemies.begin(); it != enemies.end();) {
        auto& enemy = *it;
        float radius = CELL_SIZE / 4.0f;
        sf::Vector2f pos = enemy->getPosition();
        int xMin = static_cast<int>((pos.x - radius) / CELL_SIZE);
        int xMax = static_cast<int>((pos.x + radius) / CELL_SIZE);
        int yMin = static_cast<int>((pos.y - radius) / CELL_SIZE);
        int yMax = static_cast<int>((pos.y + radius) / CELL_SIZE);

        xMin = std::max(0, xMin);
        xMax = std::min(GRID_SIZE - 1, xMax);
        yMin = std::max(0, yMin);
        yMax = std::min(GRID_SIZE - 1, yMax);

        bool onPath = false;
        for (int x = xMin; x <= xMax; ++x) {
            for (int y = yMin; y <= yMax; ++y) {
                if (grid_[x][y] == 4 || grid_[x][y] == 1 || grid_[x][y] == 2) {
                    onPath = true;
                    break;
                }
            }
            if (onPath) break;
        }

        if (!onPath) {
            enemy->markForRemovalDueToPathFailure();
            it = enemies.erase(it);
            continue;
        }

        if (enemy->getPathVersion() != pathVersion_) {
            enemy->setPath(currentPath_);
            enemy->setPathVersion(pathVersion_);
        }

        enemy->update(deltaTime);
        ++it;
    }
}

void Map::draw(sf::RenderWindow& window) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            float xPos = i * CELL_SIZE + 1;
            if (xPos >= MAP_WIDTH) continue;
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
            cell.setPosition(xPos, j * CELL_SIZE + 1);
            if (grid_[i][j] == 1) cell.setFillColor(sf::Color::Cyan);
            else if (grid_[i][j] == 2) cell.setFillColor(sf::Color::Magenta);
            else if (grid_[i][j] == 3) cell.setFillColor(sf::Color::White);
            else if (grid_[i][j] == 4) cell.setFillColor(sf::Color(150, 150, 150));
            else cell.setFillColor(sf::Color(215, 215, 215));
            window.draw(cell);
        }
    }
    for (auto& tower : towers_) {
        tower->draw(window);
    }
}

std::vector<sf::Vector2i> Map::findPath(int startX, int startY, int endX, int endY) {
    std::priority_queue<Node*, std::vector<Node*>, CompareNode> openSet;
    std::unordered_set<Node*> allNodes;
    std::vector<std::vector<bool>> visited(GRID_SIZE, std::vector<bool>(GRID_SIZE, false));

    Node* start = new Node(startX, startY);
    start->g = 0;
    start->h = std::abs(startX - endX) + std::abs(startY - endY);
    start->f = start->g + start->h;
    openSet.push(start);
    allNodes.insert(start);
    visited[startX][startY] = true;

    int dx[] = { 0, 0, 1, -1 };
    int dy[] = { 1, -1, 0, 0 };

    while (!openSet.empty()) {
        Node* current = openSet.top();
        openSet.pop();

        if (current->x == endX && current->y == endY) {
            std::vector<sf::Vector2i> path;
            while (current) {
                path.push_back(sf::Vector2i(current->x, current->y));
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
            for (auto* node : allNodes) {
                delete node;
            }
            return path;
        }

        for (int i = 0; i < 4; ++i) {
            int newX = current->x + dx[i];
            int newY = current->y + dy[i];
            if (newX < 0 || newX >= GRID_SIZE || newY < 0 || newY >= GRID_SIZE) continue;
            if (grid_[newX][newY] == 3) continue;
            if (visited[newX][newY]) continue;

            visited[newX][newY] = true;
            float g = current->g + 1.0f;
            float h = std::abs(newX - endX) + std::abs(newY - endY);
            float f = g + h;

            Node* neighbor = new Node(newX, newY);
            neighbor->g = g;
            neighbor->h = h;
            neighbor->f = f;
            neighbor->parent = current;
            openSet.push(neighbor);
            allNodes.insert(neighbor);
        }
    }

    std::vector<sf::Vector2i> emptyPath;
    for (auto* node : allNodes) {
        delete node;
    }
    return emptyPath;
}

std::vector<sf::Vector2i> Map::findPathBasedOnPosition(sf::Vector2f enemyPos, int endX, int endY) {
    int startX = static_cast<int>(enemyPos.x / CELL_SIZE);
    int startY = static_cast<int>(enemyPos.y / CELL_SIZE);
    return findPath(startX, startY, endX, endY);
}

std::vector<sf::Vector2i> Map::getCurrentPath() const {
    return currentPath_;
}