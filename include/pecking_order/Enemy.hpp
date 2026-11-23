#pragma once
#include <ostream>
#include <string>
#include "pecking_order/Stats.hpp"

namespace pecking_order {

class Enemy {
private:
    std::string name;
    Stats stats;
    float currentHp;
    float distanceToPlayer; 
    int xpReward;            
    int igcReward;           

public:
    Enemy(const std::string& name, const Stats& stats,
          float spawnDistance, int xpReward, int igcReward);

   
    Enemy createScaledCopy(int waveNumber) const;


    void advanceTowardsPlayer(float deltaTime);

    float takeDamage(float rawDamage);

    bool isInAttackRange() const;
    bool isAlive() const;

    float getDamage() const;
    float getArmor() const;
    float getDistanceToPlayer() const;
    int getXpReward() const;
    int getIgcReward() const;

    friend std::ostream& operator<<(std::ostream& os, const Enemy& enemy);
};

} // namespace pecking_order