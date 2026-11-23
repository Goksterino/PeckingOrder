#include "pecking_order/Stats.hpp"
#include <algorithm>
#include <iomanip>

namespace pecking_order {

void Stats::clamp() {
    
    maxHp = std::max(1.0f, maxHp);
    damage = std::max(1.0f, damage);
    attackSpeed = std::clamp(attackSpeed, 0.2f, 10.0f);
    moveSpeed = std::clamp(moveSpeed, 10.0f, 500.0f);
    armor = std::clamp(armor, 0.0f, 100.0f);
    critChance = std::clamp(critChance, 0.0f, 1.0f);
}

Stats::Stats(float maxHp, float damage, float attackSpeed,
             float moveSpeed, float armor, float critChance)
    : maxHp(maxHp),
      damage(damage),
      attackSpeed(attackSpeed),
      moveSpeed(moveSpeed),
      armor(armor),
      critChance(critChance) {
    clamp();
}

void Stats::applyBonus(const Stats& bonus) {
    maxHp += bonus.maxHp;
    damage += bonus.damage;
    attackSpeed += bonus.attackSpeed;
    moveSpeed += bonus.moveSpeed;
    armor += bonus.armor;
    critChance += bonus.critChance;
    clamp();
}

Stats Stats::scaledBy(float factor) const {
    return Stats(
        maxHp * factor,
        damage * factor,
        attackSpeed * (1.0f + (factor - 1.0f) * 0.25f),
        moveSpeed * (1.0f + (factor - 1.0f) * 0.10f),
        armor * factor,
        critChance
    );
}

float Stats::computeMitigatedDamage(float rawDamage) const {
    const float reduction = armor / (armor + 50.0f);
    return std::max(1.0f, rawDamage * (1.0f - reduction));
}

float Stats::computeExpectedDps(float targetArmor) const {

    constexpr float critMultiplier = 1.75f;
    const float avgHit = damage * (1.0f + critChance * (critMultiplier - 1.0f));
    const float armorReduction = targetArmor / (targetArmor + 50.0f);
    return avgHit * attackSpeed * (1.0f - armorReduction);
}

float Stats::getMaxHp() const { return maxHp; }
float Stats::getDamage() const { return damage; }
float Stats::getMoveSpeed() const { return moveSpeed; }
float Stats::getArmor() const { return armor; }

std::ostream& operator<<(std::ostream& os, const Stats& stats) {
    os << std::fixed << std::setprecision(1)
       << "[HP: " << stats.maxHp
       << " | DMG: " << stats.damage
       << " | AS: " << stats.attackSpeed
       << " | SPD: " << stats.moveSpeed
       << " | ARM: " << stats.armor
       << " | CRIT: " << (stats.critChance * 100.0f) << "%]";
    return os;
}

} // namespace pecking_order