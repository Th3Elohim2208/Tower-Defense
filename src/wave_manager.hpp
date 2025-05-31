#ifndef WAVE_MANAGER_HPP
#define WAVE_MANAGER_HPP

#include <vector>
#include <memory>
#include <map>
#include "enemy.hpp"

class WaveManager {
public:
    WaveManager(float baseInterval, int baseWaveSize, float intervalReductionFactor, float sizeIncreaseFactor, sf::Font& font);
    bool shouldSpawnWave(float deltaTime, bool gameStarted);
    std::vector<std::shared_ptr<Enemy>> spawnWave();
    void setEnemyStats(std::shared_ptr<Enemy> enemy, float distance, float remainingHealth);
    int getCurrentWave() const;
    int getMutationCount() const;
    void liberarEnemigos();

private:
    struct EnemyStats {
        float distance;
        float remainingHealth;
    };

    float waveInterval_;
    int baseWaveSize_;
    float intervalReductionFactor_;
    float sizeIncreaseFactor_;
    int currentWave_;
    float timeSinceLastWave_;
    std::vector<std::shared_ptr<Enemy>> previousWave_;
    std::map<std::shared_ptr<Enemy>, EnemyStats> enemyStats_;
    int mutationCount_;
    sf::Font& font_;

    std::vector<std::shared_ptr<Enemy>> evolvePopulation(int waveSize);
};

#endif