#include "pecking_order/Arena.hpp"
#include <algorithm>
#include <limits>
#include <random>
#include <spdlog/spdlog.h>

namespace pecking_order {

void Arena::spawnWaveEnemies(int waveNumber) {
    activeEnemies.clear();
    if (enemyTemplates.empty()) {
        return;
    }

   
    const int enemyCount = 2 + waveNumber * 2;
    for (int i = 0; i < enemyCount; ++i) {
        const auto& tpl = enemyTemplates[static_cast<std::size_t>(i) % enemyTemplates.size()];
        activeEnemies.push_back(tpl.createScaledCopy(waveNumber));
    }
}

std::size_t Arena::findClosestAliveEnemyIndex() const {
    std::size_t bestIdx = activeEnemies.size();
    float minDist = std::numeric_limits<float>::max();

    for (std::size_t i = 0; i < activeEnemies.size(); ++i) {
        if (activeEnemies[i].isAlive() && activeEnemies[i].getDistanceToPlayer() < minDist) {
            minDist = activeEnemies[i].getDistanceToPlayer();
            bestIdx = i;
        }
    }
    return bestIdx;
}

Arena::Arena(const Character& selectedPlayer,
             const std::vector<Enemy>& enemyTemplates,
             const std::vector<Upgrade>& upgradePool)
    : player(selectedPlayer),
      enemyTemplates(enemyTemplates),
      upgradePool(upgradePool),
      currentWave(0),
      totalIgcEarned(0),
      enemiesDefeated(0) {}

bool Arena::simulateWave(int waveNumber) {
    if (!player.isAlive()) {
        return false;
    }

    currentWave = waveNumber;
    spawnWaveEnemies(waveNumber);
    spdlog::info("Wave {} started with {} enemies.", waveNumber, activeEnemies.size());

    constexpr float tickSeconds = 0.5f;
    constexpr int maxTicks = 200;

    for (int tick = 0; tick < maxTicks && player.isAlive(); ++tick) {
        const std::size_t targetIdx = findClosestAliveEnemyIndex();
        if (targetIdx == activeEnemies.size()) {
            break;
        }

        Enemy& target = activeEnemies[targetIdx];
        const float playerDamage = player.calculateEffectiveDps(target.getArmor()) * tickSeconds;
        target.takeDamage(playerDamage);

        if (!target.isAlive()) {
            ++enemiesDefeated;
            totalIgcEarned += target.getIgcReward();
            if (player.gainXp(target.getXpReward())) {
                spdlog::info("{} leveled up during wave {}!", player.getName(), waveNumber);
            }
        }

        for (auto& enemy : activeEnemies) {
            if (!enemy.isAlive()) {
                continue;
            }
            enemy.advanceTowardsPlayer(tickSeconds);
            if (enemy.isInAttackRange()) {
                player.takeDamage(enemy.getDamage() * tickSeconds);
                if (!player.isAlive()) {
                    spdlog::warn("{} fell in wave {}!", player.getName(), waveNumber);
                    return false;
                }
            }
        }
    }

    spdlog::info("Wave {} cleared! Total IGC so far: {}", waveNumber, totalIgcEarned);
    return player.isAlive();
}

std::vector<Upgrade> Arena::generateUpgradeChoices(int count) const {
    std::vector<Upgrade> choices;
    if (upgradePool.empty() || count <= 0) {
        return choices;
    }

    std::vector<Upgrade> available = upgradePool;
    // Seed determinist pe baza valului curent (util pentru reproductibilitate în teste și CI)
    std::mt19937 rng(static_cast<unsigned int>(42 + currentWave * 17));

    const int targetCount = std::min(count, static_cast<int>(available.size()));
    for (int i = 0; i < targetCount; ++i) {
        std::vector<int> weights;
        weights.reserve(available.size());
        for (const auto& upg : available) {
            weights.push_back(upg.getWeight());
        }

        std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());
        const std::size_t picked = dist(rng);
        choices.push_back(available[picked]);
        available.erase(available.begin() + static_cast<std::ptrdiff_t>(picked));
    }

    return choices;
}

void Arena::applyChosenUpgrade(const Upgrade& upgrade) {
    player.applyUpgrade(upgrade);
    spdlog::info("Applied upgrade: {}", upgrade.getName());
}

int Arena::finalizeRunIgc() const {
    // Bonus de 10 IGC pentru fiecare val atins
    return totalIgcEarned + currentWave * 10;
}

bool Arena::isPlayerAlive() const { return player.isAlive(); }
int Arena::getCurrentWave() const { return currentWave; }
int Arena::getTotalIgcEarned() const { return totalIgcEarned; }

std::ostream& operator<<(std::ostream& os, const Arena& arena) {
    os << "=== ARENA (Wave " << arena.currentWave
       << " | Defeated: " << arena.enemiesDefeated
       << " | IGC: " << arena.totalIgcEarned << ") ===\n"
       << arena.player << "\n"
       << "Active enemies (" << arena.activeEnemies.size() << "):";
    for (const auto& enemy : arena.activeEnemies) {
        os << "\n  * " << enemy;
    }
    return os;
}

} // namespace pecking_order