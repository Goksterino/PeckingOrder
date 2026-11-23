#pragma once
#include <ostream>
#include <string>
#include <vector>
#include "pecking_order/Stats.hpp"
#include "pecking_order/Upgrade.hpp"

namespace pecking_order {


class Character {
private:
    std::string name;
    Stats baseStats;                     
    Stats currentStats;                  
    float currentHp;
    int level;
    int currentXp;
    int unlockCostIgc;                   
    std::vector<Upgrade> activeUpgrades;  

  
    void recalculateStats();

    
    int xpRequiredForNextLevel() const;

public:
    Character(const std::string& name, const Stats& baseStats, int unlockCostIgc);

    
    Character(const Character& other);
    Character& operator=(const Character& other);
    ~Character();

    
    void applyUpgrade(const Upgrade& upgrade);

   
    float takeDamage(float rawDamage);

   
    bool gainXp(int amount);

   
    float calculateEffectiveDps(float targetArmor) const;

    bool isAlive() const;
    const std::string& getName() const;
    int getUnlockCostIgc() const;
    const Stats& getCurrentStats() const;

    friend std::ostream& operator<<(std::ostream& os, const Character& character);
};

} // namespace pecking_order