#include "map.hpp"
#include "enemy.hpp"

const int GRID_SIZE = 10;
const float CELL_SIZE = 60.0f;

Map::Map() : grid_(GRID_SIZE, std::vector<int>(GRID_SIZE, 0)) {
    grid_[0][0] = 1; // Entrada
    grid_[9][9] = 2; // Puente
    // Camino inicial (ajustaremos con A* más adelante)
    for (int y = 0; y < GRID_SIZE; ++y) {
        grid_[0][y] = 4;
    }
    for (int x = 0; x < GRID_SIZE; ++x) {
        grid_[x][9] = 4;
    }
    grid_[0][0] = 1; // Restaurar entrada
    grid_[9][9] = 2; // Restaurar puente
}

bool Map::canPlaceTower(int x, int y) {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) return false;
    return grid_[x][y] == 0; // Solo en celdas vacías
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