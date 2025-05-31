#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "map.hpp"
#include "tower.hpp"
#include "enemy.hpp"
#include "wave_manager.hpp"
#include "constants.hpp"
#include <sstream>
#include <ctime>
#include <memory>
#include <cmath>
#include <vector>

class Game {
public:
    Game() : window_(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Tower Defense"),
        gold_(1000), selectedTowerType_(Tower::ARCHER), selectedTower_(nullptr),
        waveManager_(30.0f, 10, 0.05f, 2.0f, font_),
        initialDelay_(5.0f), spawnTimer_(0.0f),
        totalMutations_(0), enemiesKilledPerWave_(0) {
        if (!font_.loadFromFile("arial.ttf")) {
            if (!font_.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
                // Si no se carga, el texto no se mostrará
            }
        }

        goldText_.setFont(font_);
        goldText_.setCharacterSize(24);
        goldText_.setFillColor(sf::Color::Yellow);
        goldText_.setPosition(MAP_WIDTH + 10.f, 10.f);

        statsText_.resize(3);
        for (int i = 0; i < 3; ++i) {
            statsText_[i].setFont(font_);
            statsText_[i].setCharacterSize(18);
            statsText_[i].setFillColor(sf::Color::White);
            statsText_[i].setPosition(MAP_WIDTH + 10.f, 70.f + i * 20.f);
        }

        for (int i = 0; i < 3; ++i) {
            towerButtons_[i].setSize(sf::Vector2f(230.f, 100.f));
            towerButtons_[i].setPosition(MAP_WIDTH + 20.f, 200.f + i * 120.f);
            towerLabels_[i].setFont(font_);
            towerLabels_[i].setCharacterSize(14);
            towerLabels_[i].setFillColor(sf::Color::White);
            float buttonY = 200.f + i * 120.f;
            float marginTop = 0.f;
            float textY = buttonY + marginTop;
            towerLabels_[i].setPosition(MAP_WIDTH + 30.f, textY);
        }

        upgradeButton_.setSize(sf::Vector2f(210.f, 30.f));
        upgradeButton_.setPosition(MAP_WIDTH + 20.f, 200.f + 3 * 120.f);
        upgradeButton_.setFillColor(sf::Color(150, 150, 150));
        upgradeText_.setFont(font_);
        upgradeText_.setCharacterSize(14);
        upgradeText_.setFillColor(sf::Color::Black);
        upgradeText_.setPosition(MAP_WIDTH + 30.f, 205.f + 3 * 120.f);

        towerButtons_[0].setFillColor(sf::Color::Green);
        towerButtons_[1].setFillColor(sf::Color::Blue);
        towerButtons_[2].setFillColor(sf::Color(139, 69, 19));

        updateTowerLabels();
        updateStatsText();
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

    void updateStatsText() {
        std::stringstream ss;

        ss << "Enemies Killed: " << enemiesKilledPerWave_;
        statsText_[0].setString(ss.str());
        ss.str("");

        ss << "Mutation Probability: 20%";
        statsText_[1].setString(ss.str());
        ss.str("");

        totalMutations_ = waveManager_.getMutationCount();
        ss << "Total Mutations: " << totalMutations_;
        statsText_[2].setString(ss.str());
    }

    float calculateFitness(const std::shared_ptr<Enemy>& enemy) {
        float distance = 0.0f;
        float remainingHealth = enemy->getHealth();
        float fitness;
        switch (enemy->getType()) {
        case Enemy::OGRE:
            fitness = 0.3f * distance + 0.7f * remainingHealth;
            break;
        case Enemy::DARK_ELF:
            fitness = 0.7f * distance + 0.3f * remainingHealth;
            break;
        case Enemy::HARPY:
            fitness = 0.5f * distance + 0.5f * remainingHealth;
            break;
        case Enemy::MERCENARY:
            fitness = 0.4f * distance + 0.6f * remainingHealth;
            break;
        default:
            fitness = remainingHealth;
        }
        return fitness;
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
        initialDelay_ -= deltaTime;
        if (initialDelay_ > 0.0f) {
            goldText_.setString("Gold: " + std::to_string(gold_) + "\nGame starts in: " + std::to_string(static_cast<int>(initialDelay_ + 1)));
            return;
        }

        bool gameStarted = (initialDelay_ <= 0.0f);

        if (waveManager_.getCurrentWave() == 0 && gameStarted) {
            auto newWave = waveManager_.spawnWave();
            currentWaveEnemies_ = newWave;
            spawnIndex_ = 0;
            spawnTimer_ = 0.0f;
        }

        if (waveManager_.shouldSpawnWave(deltaTime, gameStarted)) {
            auto newWave = waveManager_.spawnWave();
            currentWaveEnemies_ = newWave;
            spawnIndex_ = 0;
            spawnTimer_ = 0.0f;
        }

        if (!currentWaveEnemies_.empty() && spawnIndex_ < currentWaveEnemies_.size()) {
            spawnTimer_ += deltaTime;
            if (spawnTimer_ >= 0.5f) {
                auto enemy = currentWaveEnemies_[spawnIndex_];
                auto path = map_.getCurrentPath();
                enemy->setPath(path);
                enemy->setPathVersion(map_.getPathVersion());
                enemies_.push_back(enemy);
                spawnIndex_++;
                spawnTimer_ = 0.0f;
            }
        }

        map_.update(deltaTime, enemies_);
        for (auto it = enemies_.begin(); it != enemies_.end();) {
            auto& enemy = *it;
            if (!enemy->isAlive()) {
                auto path = enemy->getPath();
                float distance = 0.0f;
                size_t currentIndex = std::min(enemy->getPathIndex(), path.size() - 1);
                for (size_t i = 0; i < currentIndex && i < path.size() - 1; ++i) {
                    float dx = path[i + 1].x - path[i].x;
                    float dy = path[i + 1].y - path[i].y;
                    distance += std::sqrt(dx * dx + dy * dy);
                }
                waveManager_.setEnemyStats(enemy, distance, 0.0f);
                enemiesKilledPerWave_++;

                switch (enemy->getType()) {
                case Enemy::OGRE: gold_ += 15; break;
                case Enemy::DARK_ELF: gold_ += 10; break;
                case Enemy::HARPY: gold_ += 5; break;
                case Enemy::MERCENARY: gold_ += 12; break;
                }
                it = enemies_.erase(it);
            }
            else if (enemy->hasReachedEnd()) {
                auto path = enemy->getPath();
                float distance = 0.0f;
                for (size_t i = 0; i < path.size() - 1; ++i) {
                    float dx = path[i + 1].x - path[i].x;
                    float dy = path[i + 1].y - path[i].y;
                    distance += std::sqrt(dx * dx + dy * dy);
                }
                waveManager_.setEnemyStats(enemy, distance, static_cast<float>(enemy->getHealth()));
                it = enemies_.erase(it);
            }
            else if (enemy->shouldRemoveWithoutGold()) {
                auto path = enemy->getPath();
                float distance = 0.0f;
                size_t currentIndex = std::min(enemy->getPathIndex(), path.size() - 1);
                for (size_t i = 0; i < currentIndex && i < path.size() - 1; ++i) {
                    float dx = path[i + 1].x - path[i].x;
                    float dy = path[i + 1].y - path[i].y;
                    distance += std::sqrt(dx * dx + dy * dy);
                }
                waveManager_.setEnemyStats(enemy, distance, static_cast<float>(enemy->getHealth()));
                it = enemies_.erase(it);
            }
            else {
                ++it;
            }
        }
        goldText_.setString("Gold: " + std::to_string(gold_) + "\nWave: " + std::to_string(waveManager_.getCurrentWave()));
        updateStatsText();
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
        for (const auto& text : statsText_) {
            window_.draw(text);
        }
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
    std::vector<std::shared_ptr<Enemy>> enemies_;
    sf::Font font_;
    sf::Text goldText_;
    std::vector<sf::Text> statsText_;
    Tower::Type selectedTowerType_;
    sf::RectangleShape towerButtons_[3];
    sf::Text towerLabels_[3];
    sf::RectangleShape upgradeButton_;
    sf::Text upgradeText_;
    std::shared_ptr<Tower> selectedTower_;
    WaveManager waveManager_;
    float initialDelay_;
    std::vector<std::shared_ptr<Enemy>> currentWaveEnemies_;
    size_t spawnIndex_;
    float spawnTimer_;
    int totalMutations_;
    int enemiesKilledPerWave_;
};

int main() {
    sf::Clock clock;
    srand(static_cast<unsigned>(clock.getElapsedTime().asMicroseconds()));
    Game game;
    game.run();
    return 0;
}

#endif