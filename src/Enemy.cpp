#include "pecking_order/Enemy.hpp"
#include <algorithm>

namespace pecking_order {

Enemy::Enemy(const std::string& name, const Stats& stats,
             float spawnDistance, int xpReward, int igcReward)
    : name(name),
      stats(stats),
      currentHp(stats.getMaxHp()),
      distanceToPlayer(std::max(1.0f, spawnDistance)),
      xpReward(std::max(0, xpReward)),
      igcReward(std::max(0, igcReward)) {}

Enemy Enemy::createScaledCopy(int waveNumber) const {
    const float scale = 1.0f + static_cast<float>(std::max(0, waveNumber - 1)) * 0.20f;
    return Enemy(
        name,
        stats.scaledBy(scale),
        distanceToPlayer,
        static_cast<int>(static_cast<float>(xpReward) * scale),
        static_cast<int>(static_cast<float>(igcReward) * scale)
    );
}

void Enemy::advanceTowardsPlayer(float deltaTime) {
    if (deltaTime <= 0.0f || !isAlive()) {
        return;
    }
    distanceToPlayer = std::max(0.0f, distanceToPlayer - stats.getMoveSpeed() * deltaTime);
}

float Enemy::takeDamage(float rawDamage) {
    const float finalDamage = stats.computeMitigatedDamage(rawDamage);
    currentHp = std::max(0.0f, currentHp - finalDamage);
    return finalDamage;
}

bool Enemy::isInAttackRange() const { return distanceToPlayer <= 5.0f; }
bool Enemy::isAlive() const { return currentHp > 0.0f; }
float Enemy::getDamage() const { return stats.getDamage(); }
float Enemy::getArmor() const { return stats.getArmor(); }
float Enemy::getDistanceToPlayer() const { return distanceToPlayer; }
int Enemy::getXpReward() const { return xpReward; }
int Enemy::getIgcReward() const { return igcReward; }

std::ostream& operator<<(std::ostream& os, const Enemy& enemy) {
    os << "Enemy: " << enemy.name
       << " [HP: " << enemy.currentHp << "/" << enemy.stats.getMaxHp()
       << " | Dist: " << enemy.distanceToPlayer << "m] "
       << enemy.stats;
    return os;
}

} // namespace pecking_order