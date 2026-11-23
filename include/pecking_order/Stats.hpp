#pragma once
#include <ostream>

namespace pecking_order {

class Stats {
private:
    float maxHp;       
    float damage;     
    float attackSpeed;  
    float moveSpeed;   
    float armor;       
    float critChance;   

    void clamp();

public:
    Stats(float maxHp, float damage, float attackSpeed,
          float moveSpeed, float armor, float critChance);

    void applyBonus(const Stats& bonus);

    Stats scaledBy(float factor) const;

    float computeMitigatedDamage(float rawDamage) const;

    float computeExpectedDps(float targetArmor) const;

    float getMaxHp() const;
    float getDamage() const;
    float getMoveSpeed() const;
    float getArmor() const;

    friend std::ostream& operator<<(std::ostream& os, const Stats& stats);
};

} // namespace pecking_order