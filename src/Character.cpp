#include "pecking_order/Character.hpp"
#include <algorithm>

namespace pecking_order {

void Character::recalculateStats() {
    const float oldMaxHp = currentStats.getMaxHp();
    currentStats = baseStats;

    for (const auto& upgrade : activeUpgrades) {
        currentStats.applyBonus(upgrade.getBonusStats());
    }

    const float hpGain = currentStats.getMaxHp() - oldMaxHp;
    if (hpGain > 0.0f) {
        currentHp += hpGain;
    }
    currentHp = std::min(currentHp, currentStats.getMaxHp());
}

int Character::xpRequiredForNextLevel() const {
    return 50 + (level - 1) * 35;
}

Character::Character(const std::string& name, const Stats& baseStats, int unlockCostIgc)
    : name(name),
      baseStats(baseStats),
      currentStats(baseStats),
      currentHp(baseStats.getMaxHp()),
      level(1),
      currentXp(0),
      unlockCostIgc(std::max(0, unlockCostIgc)) {}

Character::Character(const Character& other)
    : name(other.name),
      baseStats(other.baseStats),
      currentStats(other.currentStats),
      currentHp(other.currentHp),
      level(other.level),
      currentXp(other.currentXp),
      unlockCostIgc(other.unlockCostIgc),
      activeUpgrades(other.activeUpgrades) {}


Character& Character::operator=(const Character& other) {
    if (this != &other) {
        name = other.name;
        baseStats = other.baseStats;
        currentStats = other.currentStats;
        currentHp = other.currentHp;
        level = other.level;
        currentXp = other.currentXp;
        unlockCostIgc = other.unlockCostIgc;
        activeUpgrades = other.activeUpgrades;
    }
    return *this;
}

Character::~Character() = default;

void Character::applyUpgrade(const Upgrade& upgrade) {
    activeUpgrades.push_back(upgrade);
    recalculateStats();
}

float Character::takeDamage(float rawDamage) {
    const float finalDamage = currentStats.computeMitigatedDamage(rawDamage);
    currentHp = std::max(0.0f, currentHp - finalDamage);
    return finalDamage;
}

bool Character::gainXp(int amount) {
    if (amount <= 0) {
        return false;
    }
    currentXp += amount;
    bool leveledUp = false;

    while (currentXp >= xpRequiredForNextLevel()) {
        currentXp -= xpRequiredForNextLevel();
        ++level;
        baseStats.applyBonus(Stats(8.0f, 1.5f, 0.05f, 2.0f, 0.5f, 0.01f));
        recalculateStats();
        currentHp = std::min(currentStats.getMaxHp(), currentHp + 15.0f);
        leveledUp = true;
    }
    return leveledUp;
}

float Character::calculateEffectiveDps(float targetArmor) const {
    return currentStats.computeExpectedDps(targetArmor);
}

bool Character::isAlive() const { return currentHp > 0.0f; }
const std::string& Character::getName() const { return name; }
int Character::getUnlockCostIgc() const { return unlockCostIgc; }
const Stats& Character::getCurrentStats() const { return currentStats; }

std::ostream& operator<<(std::ostream& os, const Character& character) {
    os << "Character: " << character.name
       << " (Lv. " << character.level
       << " | HP: " << character.currentHp << "/" << character.currentStats.getMaxHp()
       << " | Cost: " << character.unlockCostIgc << " IGC)\n"
       << "  Stats: " << character.currentStats << "\n"
       << "  Upgrades (" << character.activeUpgrades.size() << "):";
    for (const auto& upg : character.activeUpgrades) {
        os << "\n    - " << upg;
    }
    return os;
}

} // namespace pecking_order