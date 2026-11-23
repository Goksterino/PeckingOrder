#pragma once
#include <ostream>
#include <string>
#include <vector>
#include "pecking_order/Character.hpp"
#include "pecking_order/Enemy.hpp"
#include "pecking_order/Upgrade.hpp"

namespace pecking_order {

class Arena {
private:
    Character player;
    std::vector<Enemy> enemyTemplates;  
    std::vector<Enemy> activeEnemies;    
    std::vector<Upgrade> upgradePool;    
    int currentWave;
    int totalIgcEarned;
    int enemiesDefeated;

   
    void spawnWaveEnemies(int waveNumber);

   
    std::size_t findClosestAliveEnemyIndex() const;

public:
    Arena(const Character& selectedPlayer,
          const std::vector<Enemy>& enemyTemplates,
          const std::vector<Upgrade>& upgradePool);

   
    bool simulateWave(int waveNumber);

   
    std::vector<Upgrade> generateUpgradeChoices(int count) const;

   
    void applyChosenUpgrade(const Upgrade& upgrade);

   
    int finalizeRunIgc() const;

    bool isPlayerAlive() const;
    int getCurrentWave() const;
    int getTotalIgcEarned() const;

    friend std::ostream& operator<<(std::ostream& os, const Arena& arena);
};


std::vector<Character> loadCharactersFromJson(const std::string& filePath);
std::vector<Enemy> loadEnemiesFromJson(const std::string& filePath);
std::vector<Upgrade> loadUpgradesFromJson(const std::string& filePath);

}