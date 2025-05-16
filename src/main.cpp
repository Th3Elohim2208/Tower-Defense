#include <SFML/Graphics.hpp>
#include "map.hpp"
#include "tower.hpp"
#include "enemy.hpp"

class Game {
public:
    Game() : window_(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Tower Defense"), gold_(1000), spawnTimer_(0.0f) {
        if (!font_.loadFromFile("arial.ttf")) {
            // Manejo de error si no se carga la fuente
        }
        goldText_.setFont(font_);
        goldText_.setCharacterSize(24);
        goldText_.setFillColor(sf::Color::Yellow);
        goldText_.setPosition(10, 10);
    }
    void run() {
        sf::Clock clock;
        while (window_.isOpen()) {
            float deltaTime = clock.restart().asSeconds();
            handleEvents();
            update(deltaTime);
            render();
        }
    }
private:
    void handleEvents() {
        sf::Event event;
        while (window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window_.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                int x = event.mouseButton.x / CELL_SIZE;
                int y = event.mouseButton.y / CELL_SIZE;
                if (map_.canPlaceTower(x, y)) {
                    Tower::Type type = Tower::ARCHER;
                    int cost = 50;
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
                        type = Tower::ARCHER;
                        cost = 50;
                    }
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
                        type = Tower::MAGE;
                        cost = 100;
                    }
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
                        type = Tower::ARTILLERY;
                        cost = 200;
                    }
                    if (gold_ >= cost) {
                        auto tower = std::make_shared<Tower>(type, x, y);
                        map_.placeTower(x, y, tower);
                        gold_ -= cost;
                    }
                }
            }
        }
    }
    void update(float deltaTime) {
        spawnTimer_ += deltaTime;
        if (spawnTimer_ >= 2.0f) {
            enemies_.push_back(std::make_shared<Enemy>(50.0f));
            spawnTimer_ = 0.0f;
        }
        map_.update(deltaTime, enemies_);
        for (auto it = enemies_.begin(); it != enemies_.end();) {
            auto& enemy = *it;
            enemy->update(deltaTime);
            if (!enemy->isAlive() || enemy->hasReachedEnd()) {
                if (!enemy->isAlive()) gold_ += 10;
                it = enemies_.erase(it);
            }
            else {
                ++it;
            }
        }
        goldText_.setString("Gold: " + std::to_string(gold_));
    }
    void render() {
        window_.clear();
        map_.draw(window_);
        for (const auto& enemy : enemies_) {
            enemy->draw(window_);
        }
        window_.draw(goldText_);
        window_.display();
    }
    sf::RenderWindow window_;
    Map map_;
    int gold_;
    float spawnTimer_;
    std::vector<std::shared_ptr<Enemy>> enemies_;
    sf::Font font_;
    sf::Text goldText_;
};

const int GRID_SIZE = 10;
const float CELL_SIZE = 60.0f;
const int WINDOW_WIDTH = GRID_SIZE * CELL_SIZE;
const int WINDOW_HEIGHT = GRID_SIZE * CELL_SIZE;

int main() {
    Game game;
    game.run();
    return 0;
}