#include "wave_manager.hpp"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>

WaveManager::WaveManager(float baseInterval, int baseWaveSize, float intervalReductionFactor, float sizeIncreaseFactor, sf::Font& font)
    : waveInterval_(baseInterval), baseWaveSize_(baseWaveSize), currentWave_(0),
    intervalReductionFactor_(intervalReductionFactor), sizeIncreaseFactor_(sizeIncreaseFactor),
    timeSinceLastWave_(0.0f), mutationCount_(0), font_(font) {
}

bool WaveManager::shouldSpawnWave(float deltaTime, bool gameStarted) {
    if (!gameStarted) {
        timeSinceLastWave_ = 0.0f;
        liberarEnemigos();
        return false;
    }
    timeSinceLastWave_ += deltaTime;
    float currentInterval = waveInterval_ * std::max(0.5f, 1.0f - currentWave_ * intervalReductionFactor_);
    if (timeSinceLastWave_ >= currentInterval) {
        timeSinceLastWave_ = 0.0f;
        return true;
    }
    return false;
}

void WaveManager::liberarEnemigos() {
    previousWave_.clear();
    enemyStats_.clear();
}

std::vector<std::shared_ptr<Enemy>> WaveManager::spawnWave() {
    currentWave_++;
    int waveSize = baseWaveSize_ + static_cast<int>(currentWave_ * sizeIncreaseFactor_);
    std::vector<std::shared_ptr<Enemy>> newWave;

    std::cout << "Generando oleada " << currentWave_ << " con tamaño " << waveSize << std::endl;

    if (currentWave_ == 1 || previousWave_.empty()) {
        for (int i = 0; i < waveSize; ++i) {
            Enemy::Type type = static_cast<Enemy::Type>(rand() % 4);
            auto enemy = std::make_shared<Enemy>(type, font_);
            newWave.push_back(enemy);
        }
        std::cout << "Oleada " << currentWave_ << " generada sin evolución (primera oleada o previousWave_ vacía)" << std::endl;
    }
    else {
        std::cout << "Evolucionando población para oleada " << currentWave_ << std::endl;
        newWave = evolvePopulation(waveSize);
    }

    previousWave_ = newWave;
    std::cout << "previousWave_ ahora tiene " << previousWave_.size() << " enemigos" << std::endl;
    return newWave;
}

void WaveManager::setEnemyStats(std::shared_ptr<Enemy> enemy, float distance, float remainingHealth) {
    enemyStats_[enemy] = { distance, remainingHealth };
    std::cout << "Estadísticas registradas para enemigo: distancia=" << distance << ", salud restante=" << remainingHealth << std::endl;
}

int WaveManager::getCurrentWave() const {
    return currentWave_;
}

int WaveManager::getMutationCount() const {
    return mutationCount_;
}

std::vector<std::shared_ptr<Enemy>> WaveManager::evolvePopulation(int waveSize) {
    std::cout << "Iniciando evolvePopulation para oleada " << currentWave_ << " con tamaño " << waveSize << std::endl;
    std::cout << "Tamaño de previousWave_: " << previousWave_.size() << std::endl;
    std::cout << "Tamaño de enemyStats_: " << enemyStats_.size() << std::endl;

    std::vector<std::pair<std::shared_ptr<Enemy>, float>> fitnessScores;
    for (const auto& enemy : previousWave_) {
        auto stats = enemyStats_.find(enemy);
        if (stats == enemyStats_.end()) {
            std::cout << "Advertencia: No se encontraron estadísticas para un enemigo en previousWave_" << std::endl;
            continue;
        }

        float distance = stats->second.distance;
        float remainingHealth = stats->second.remainingHealth;
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
        }
        fitnessScores.push_back({ enemy, fitness });
        std::cout << "Fitness calculado: " << fitness << " para enemigo tipo " << enemy->getType() << std::endl;
    }

    std::vector<std::shared_ptr<Enemy>> parents;
    int numParents = std::max(2, static_cast<int>(fitnessScores.size() * 0.5f));
    std::sort(fitnessScores.begin(), fitnessScores.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
        });
    for (int i = 0; i < numParents && i < fitnessScores.size(); ++i) {
        parents.push_back(fitnessScores[i].first);
    }
    std::cout << "Número de padres seleccionados: " << parents.size() << std::endl;

    std::vector<std::shared_ptr<Enemy>> newPopulation;
    int remainingEnemies = waveSize;
    int randomCount = 0;
    int targetRandom = static_cast<int>(waveSize * 0.40); // 40% de enemigos aleatorios

    while (remainingEnemies > 0) {
        Enemy::Type childType;
        float childHealth, childSpeed;

        // Calcular la proporción actual de enemigos aleatorios generados
        float currentRandomRatio = (newPopulation.size() > 0) ? static_cast<float>(randomCount) / newPopulation.size() : 0.0f;
        bool generateRandom = false;

        // Si no hemos alcanzado el objetivo de aleatorios y la proporción actual es menor al 40%,
        // o si estamos cerca del final y necesitamos cumplir el objetivo
        if (randomCount < targetRandom && (currentRandomRatio < 0.40 || remainingEnemies + randomCount <= targetRandom)) {
            int chance = (remainingEnemies > 1) ? static_cast<int>((0.40 - currentRandomRatio) * 100) + 40 : 100;
            generateRandom = (rand() % 100) < chance;
        }
        else if (randomCount >= targetRandom) {
            generateRandom = false; // Ya cumplimos el objetivo, generar evolucionados
        }

        if (generateRandom || parents.empty()) {
            // Generar enemigo aleatorio
            childType = static_cast<Enemy::Type>(rand() % 4);
            auto enemy = std::make_shared<Enemy>(childType, font_);
            childHealth = enemy->getHealth();
            childSpeed = enemy->getSpeed();
            newPopulation.push_back(enemy);
            randomCount++;
            std::cout << "Generado enemigo aleatorio (forzado) tipo " << childType << std::endl;
        }
        else {
            // Generar hijo a partir de padres con posibilidad de tipo aleatorio
            auto parent1 = parents[rand() % parents.size()];
            auto parent2 = parents[rand() % parents.size()];

            // 50% de probabilidad de heredar tipo de padre, 50% de elegir aleatorio
            if (rand() % 2 == 0) {
                childType = (rand() % 2 == 0) ? parent1->getType() : parent2->getType();
            }
            else {
                childType = static_cast<Enemy::Type>(rand() % 4);
            }

            float w = static_cast<float>(rand()) / RAND_MAX;
            childHealth = (w * parent1->getHealth() + (1 - w) * parent2->getHealth());
            childSpeed = (w * parent1->getSpeed() + (1 - w) * parent2->getSpeed());

            if (childType == Enemy::OGRE) {
                childHealth *= 1.2f;
                childSpeed *= 0.8f;
            }
            else if (childType == Enemy::DARK_ELF) {
                childHealth *= 0.8f;
                childSpeed *= 1.2f;
            }
            else if (childType == Enemy::HARPY) {
                childHealth *= 0.9f;
                childSpeed *= 1.1f;
            }
            else if (childType == Enemy::MERCENARY) {
                childHealth *= 1.1f;
                childSpeed *= 0.9f;
            }

            childHealth = std::max(50.0f, std::min(200.0f, childHealth));
            childSpeed = std::max(50.0f, std::min(150.0f, childSpeed));

            auto child = std::make_shared<Enemy>(childType, font_);
            child->setHealth(static_cast<int>(childHealth));
            child->setSpeed(childSpeed);

            newPopulation.push_back(child);
            std::cout << "Generado hijo tipo " << childType << " con Salud=" << childHealth << ", Velocidad=" << childSpeed << std::endl;
        }

        // Probabilidad de mutación del 20% (para depuración, luego puedes volver al 5%)
        if ((rand() % 100) < 20) {
            bool mutateHealth = (childType == Enemy::OGRE || childType == Enemy::MERCENARY) ?
                (rand() % 100 < 70) : (rand() % 100 < 30);
            float adjustment = (rand() % 2 == 0) ? 1.2f : 0.8f;
            if (mutateHealth) {
                childHealth *= adjustment;
                childHealth = std::max(50.0f, std::min(200.0f, childHealth));
                newPopulation.back()->setHealth(static_cast<int>(childHealth));
            }
            else {
                childSpeed *= adjustment;
                childSpeed = std::max(50.0f, std::min(150.0f, childSpeed));
                newPopulation.back()->setSpeed(childSpeed);
            }
            std::cout << "Mutación aplicada: Salud=" << childHealth << ", Velocidad=" << childSpeed << std::endl;
            mutationCount_++;
        }

        remainingEnemies--;
    }

    std::cout << "Población evolucionada generada con " << newPopulation.size() << " enemigos" << std::endl;
    std::cout << "Mutaciones totales en esta oleada: " << mutationCount_ << std::endl;
    std::cout << "Total de enemigos aleatorios generados: " << randomCount << std::endl;
    return newPopulation;
}