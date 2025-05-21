#include <SFML/Graphics.hpp>
#include "map.hpp"
#include "tower.hpp"
#include "enemy.hpp"
#include "constants.hpp"
#include <sstream>

class Game {
public:
    Game() : window_(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Tower Defense"), gold_(1000), spawnTimer_(0.0f), selectedTowerType_(Tower::ARCHER) {
        if (!font_.loadFromFile("arial.ttf")) {
            if (!font_.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
                // Si no se puede cargar, el texto no se mostrará
            }
        }

        // Configurar el texto del oro
        goldText_.setFont(font_);
        goldText_.setCharacterSize(24);
        goldText_.setFillColor(sf::Color::Yellow);
        goldText_.setPosition(MAP_WIDTH + 10.f, 10.f);

        // Configurar los botones y etiquetas de las torres
        for (int i = 0; i < 3; ++i) {
            towerButtons_[i].setSize(sf::Vector2f(UI_WIDTH - 20.f, 100.f));
            towerButtons_[i].setPosition(MAP_WIDTH + 10.f, 50.f + i * 120.f);
            towerLabels_[i].setFont(font_);
            towerLabels_[i].setCharacterSize(16);
            towerLabels_[i].setFillColor(sf::Color::White);
            float buttonY = 50.f + i * 120.f;
            float marginTop = 5.f;
            float textY = buttonY + marginTop;
            towerLabels_[i].setPosition(MAP_WIDTH + 20.f, textY);
        }

        towerButtons_[0].setFillColor(sf::Color::Green);
        towerButtons_[1].setFillColor(sf::Color(128, 0, 128));
        towerButtons_[2].setFillColor(sf::Color(139, 69, 19));

        updateTowerLabels();
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
    void updateTowerLabels() {
        std::stringstream ss;
        ss << "Archer\nCost: 50\nDamage: 10\nRange: 150\nAttack Speed: 1.0s";
        towerLabels_[0].setString(ss.str());
        ss.str("");
        ss << "Mage\nCost: 100\nDamage: 20\nRange: 200\nAttack Speed: 2.0s";
        towerLabels_[1].setString(ss.str());
        ss.str("");
        ss << "Artillery\nCost: 200\nDamage: 50\nRange: 100\nAttack Speed: 3.0s";
        towerLabels_[2].setString(ss.str());
    }

    void handleEvents() {
        sf::Event event;
        while (window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window_.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window_);
                for (int i = 0; i < 3; ++i) {
                    if (towerButtons_[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectedTowerType_ = static_cast<Tower::Type>(i);
                        break;
                    }
                }
                if (mousePos.x < MAP_WIDTH) {
                    int x = static_cast<int>(mousePos.x / CELL_SIZE);
                    int y = static_cast<int>(mousePos.y / CELL_SIZE);
                    if (map_.canPlaceTower(x, y)) {
                        auto tower = std::make_shared<Tower>(selectedTowerType_, x, y);
                        int cost = tower->getCost();
                        if (gold_ >= cost) {
                            map_.placeTower(x, y, tower);
                            gold_ -= cost;
                        }
                    }
                }
            }
        }
    }

    void update(float deltaTime) {
        spawnTimer_ += deltaTime;
        if (spawnTimer_ >= 2.0f) {
            auto enemy = std::make_shared<Enemy>(50.0f);
            auto path = map_.getCurrentPath();
            enemy->setPath(path);
            enemy->setPathVersion(map_.getPathVersion());
            enemies_.push_back(enemy);
            spawnTimer_ = 0.0f;
        }
        map_.update(deltaTime, enemies_);
        for (auto it = enemies_.begin(); it != enemies_.end();) {
            auto& enemy = *it;
            if (!enemy->isAlive() || enemy->hasReachedEnd() || enemy->shouldRemoveWithoutGold()) {
                if (!enemy->isAlive() && !enemy->shouldRemoveWithoutGold()) {
                    gold_ += 10;
                }
                it = enemies_.erase(it);
            }
            else {
                ++it;
            }
        }
        goldText_.setString("Gold: " + std::to_string(gold_));
    }

    void render() {
        window_.clear(sf::Color::Black);
        // Dibujar el fondo negro para la interfaz
        sf::RectangleShape uiBackground(sf::Vector2f(UI_WIDTH, WINDOW_HEIGHT));
        uiBackground.setPosition(MAP_WIDTH, 0);
        uiBackground.setFillColor(sf::Color::Black);
        window_.draw(uiBackground);

        // Dibujar el mapa y los enemigos
        map_.draw(window_);
        for (const auto& enemy : enemies_) {
            enemy->draw(window_);
        }

        // Dibujar la interfaz
        window_.draw(goldText_);
        for (int i = 0; i < 3; ++i) {
            window_.draw(towerButtons_[i]);
            window_.draw(towerLabels_[i]);
        }

        window_.display();
    }

    sf::RenderWindow window_;
    Map map_;
    int gold_;
    float spawnTimer_;
    std::vector<std::shared_ptr<Enemy>> enemies_;
    sf::Font font_;
    sf::Text goldText_;
    Tower::Type selectedTowerType_;
    sf::RectangleShape towerButtons_[3];
    sf::Text towerLabels_[3];
};

int main() {
    Game game;
    game.run();
    return 0;
}