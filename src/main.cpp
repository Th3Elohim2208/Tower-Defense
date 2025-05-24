#include <SFML/Graphics.hpp>
#include "map.hpp"
#include "tower.hpp"
#include "enemy.hpp"
#include "constants.hpp"
#include <sstream>
#include <ctime>
#include <memory> // Añadimos para shared_ptr

class Game {
public:
    Game() : window_(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Tower Defense"), gold_(11000), spawnTimer_(0.0f), selectedTowerType_(Tower::ARCHER), selectedTower_(nullptr) {
        if (!font_.loadFromFile("arial.ttf")) {
            if (!font_.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
                // Si no se carga, el texto no se mostrará
            }
        }

        // Configurar el texto del oro
        goldText_.setFont(font_);
        goldText_.setCharacterSize(24);
        goldText_.setFillColor(sf::Color::Yellow);
        goldText_.setPosition(MAP_WIDTH + 10.f, 10.f);

        // Configurar los botones y etiquetas de las torres
        for (int i = 0; i < 3; ++i) {
            towerButtons_[i].setSize(sf::Vector2f(230.f, 100.f));
            towerButtons_[i].setPosition(MAP_WIDTH + 20.f, 50.f + i * 120.f);
            towerLabels_[i].setFont(font_);
            towerLabels_[i].setCharacterSize(14);
            towerLabels_[i].setFillColor(sf::Color::White);
            float buttonY = 50.f + i * 120.f;
            float marginTop = 0.f;
            float textY = buttonY + marginTop;
            towerLabels_[i].setPosition(MAP_WIDTH + 30.f, textY);
        }

        // Botón de upgrade
        upgradeButton_.setSize(sf::Vector2f(210.f, 30.f));
        upgradeButton_.setPosition(MAP_WIDTH + 20.f, 50.f + 3 * 120.f);
        upgradeButton_.setFillColor(sf::Color(150, 150, 150));
        upgradeText_.setFont(font_);
        upgradeText_.setCharacterSize(14);
        upgradeText_.setFillColor(sf::Color::Black);
        upgradeText_.setPosition(MAP_WIDTH + 30.f, 55.f + 3 * 120.f);

        towerButtons_[0].setFillColor(sf::Color::Green);
        towerButtons_[1].setFillColor(sf::Color::Blue);
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
        ss << "Archer\nCost: 50\nDamage: 10/20/35/50\nRange: 150/200/225/250\nAttack Speed: 1.0/0.75/0.65/0.5s\nSpecial Attack: 20%";
        towerLabels_[0].setString(ss.str());
        ss.str("");
        ss << "Mage\nCost: 100\nDamage: 20/35/50/75\nRange: 200/225/250/275\nAttack Speed: 2.0/1.5/1.25/1.0s\nSpecial Attack: 15%";
        towerLabels_[1].setString(ss.str());
        ss.str("");
        ss << "Artillery\nCost: 200\nDamage: 50/75/100/125\nRange: 100/125/150/175\nAttack Speed: 3.0/2.5/2.0/1.5s\nSpecial Attack: 10%";
        towerLabels_[2].setString(ss.str());
    }

    void updateUpgradeText() {
        std::stringstream ss;
        if (selectedTower_) {
            ss << "Upgrade " << selectedTower_->getUpgradeCost() << " gold, Level " << (selectedTower_->getUpgradeLevel() + 1);
        }
        else {
            ss << "Select a tower to upgrade";
        }
        upgradeText_.setString(ss.str());
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
                if (mousePos.x < MAP_WIDTH) {
                    int x = static_cast<int>(mousePos.x / CELL_SIZE);
                    int y = static_cast<int>(mousePos.y / CELL_SIZE);
                    selectedTower_ = nullptr;
                    for (auto& tower : map_.getTowers()) {
                        if (tower->getX() == x && tower->getY() == y) {
                            selectedTower_ = tower;
                            break;
                        }
                    }
                    updateUpgradeText();
                }
                if (upgradeButton_.getGlobalBounds().contains(mousePos.x, mousePos.y) && selectedTower_) {
                    int upgradeCost = selectedTower_->getUpgradeCost();
                    if (gold_ >= upgradeCost && selectedTower_->canUpgrade()) {
                        if (selectedTower_->upgrade()) {
                            gold_ -= upgradeCost;
                            updateUpgradeText();
                        }
                    }
                }
            }
        }
    }

    void update(float deltaTime) {
        spawnTimer_ += deltaTime;
        if (spawnTimer_ >= 2.0f) {
            Enemy::Type enemyType = static_cast<Enemy::Type>(rand() % 4); // Generar tipo aleatorio
            auto enemy = std::make_shared<Enemy>(enemyType);
            auto path = map_.getCurrentPath();
            enemy->setPath(path);
            enemy->setPathVersion(map_.getPathVersion());
            enemies_.push_back(enemy);
            spawnTimer_ = 0.0f;
        }
        map_.update(deltaTime, enemies_);
        for (auto it = enemies_.begin(); it != enemies_.end();) {
            auto& enemy = *it;
            if (!enemy->isAlive()) {
                // Otorgar oro según el tipo de enemigo
                switch (enemy->getType()) {
                case Enemy::OGRE:
                    gold_ += 10;
                    break;
                case Enemy::DARK_ELF:
                    gold_ += 15;
                    break;
                case Enemy::HARPY:
                    gold_ += 20;
                    break;
                case Enemy::MERCENARY:
                    gold_ += 12;
                    break;
                }
                it = enemies_.erase(it);
            }
            else if (enemy->hasReachedEnd()) {
                // Simplemente eliminamos al enemigo sin penalización de oro
                it = enemies_.erase(it);
            }
            else if (enemy->shouldRemoveWithoutGold()) {
                // Eliminar sin otorgar oro si se salió del camino
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
        sf::RectangleShape uiBackground(sf::Vector2f(UI_WIDTH, WINDOW_HEIGHT));
        uiBackground.setPosition(MAP_WIDTH, 0);
        uiBackground.setFillColor(sf::Color::Black);
        window_.draw(uiBackground);

        map_.draw(window_);
        for (const auto& enemy : enemies_) {
            enemy->draw(window_);
        }

        window_.draw(goldText_);
        for (int i = 0; i < 3; ++i) {
            window_.draw(towerButtons_[i]);
            window_.draw(towerLabels_[i]);
        }
        window_.draw(upgradeButton_);
        window_.draw(upgradeText_);

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
    sf::RectangleShape upgradeButton_;
    sf::Text upgradeText_;
    std::shared_ptr<Tower> selectedTower_;
};

int main() {
    srand(time(NULL));
    Game game;
    game.run();
    return 0;
}