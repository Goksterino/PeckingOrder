#include <algorithm>
#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include "pecking_order/Arena.hpp"

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

bool approxEqual(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) <= eps;
}

void testStatsCombatAndClamping() {
    using namespace pecking_order;

    Stats tankStats(100.0f, 20.0f, 1.0f, 100.0f, 50.0f, 0.0f);
    expect(approxEqual(tankStats.computeMitigatedDamage(40.0f), 20.0f),
           "50 armor should mitigate 50% of incoming damage");
    expect(approxEqual(tankStats.computeMitigatedDamage(0.1f), 1.0f),
           "Mitigated damage should never drop below 1.0");

    Stats invalidStats(-50.0f, -10.0f, 50.0f, 0.0f, 250.0f, 2.5f);
    expect(approxEqual(invalidStats.getMaxHp(), 1.0f), "maxHp should clamp to >= 1.0");
    expect(approxEqual(invalidStats.getDamage(), 1.0f), "damage should clamp to >= 1.0");
    expect(approxEqual(invalidStats.getArmor(), 100.0f), "armor should clamp to <= 100.0");
}

void testCharacterUpgradesXpAndCopy() {
    using namespace pecking_order;

    Character original("Test Rooster", Stats(100.0f, 10.0f, 1.0f, 100.0f, 0.0f, 0.0f), 0);
    Upgrade dmgUpgrade("Sharp Beak", "Common", 50,
                       Stats(20.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f));

    Character copyChar(original);
    copyChar.applyUpgrade(dmgUpgrade);

    expect(approxEqual(original.getCurrentStats().getDamage(), 10.0f),
           "Original character should remain unchanged when copy is upgraded");
    expect(approxEqual(copyChar.getCurrentStats().getDamage(), 15.0f),
           "Copied character should gain +5 damage from upgrade");
    expect(approxEqual(copyChar.getCurrentStats().getMaxHp(), 120.0f),
           "Copied character should gain +20 maxHp from upgrade");

    expect(copyChar.gainXp(60), "Character should level up at 60 XP");
    expect(copyChar.isAlive(), "Character should be alive");
}

void testEnemyMovementAndScaling() {
    using namespace pecking_order;

    Enemy fox("Stray Fox", Stats(50.0f, 10.0f, 1.0f, 20.0f, 0.0f, 0.0f), 30.0f, 15, 5);
    expect(!fox.isInAttackRange(), "Enemy at 30m should not be in attack range");

    fox.advanceTowardsPlayer(1.5f);
    expect(fox.isInAttackRange(), "Enemy should reach attack range after advancing");

    Enemy wave3Fox = fox.createScaledCopy(3);
    expect(wave3Fox.getDamage() > fox.getDamage(), "Wave 3 enemy should deal more damage");
    expect(wave3Fox.getIgcReward() > fox.getIgcReward(), "Wave 3 enemy should reward more IGC");
}

void testArenaSimulationAndJsonLoader() {
    using namespace pecking_order;

    const auto characters = loadCharactersFromJson("data/characters.json");
    const auto enemies = loadEnemiesFromJson("data/enemies.json");
    const auto upgrades = loadUpgradesFromJson("data/upgrades.json");

    expect(!characters.empty() && !enemies.empty() && !upgrades.empty(),
           "JSON data files should load non-empty collections");

    Arena jsonArena(characters[0], enemies, upgrades);
    const bool jsonSurvived = jsonArena.simulateWave(1);
    expect(jsonArena.getCurrentWave() == 1, "Current wave should be 1");
    expect(jsonSurvived == jsonArena.isPlayerAlive(),
           "simulateWave return value should match isPlayerAlive()");

    Character champion("Arena Tester", Stats(300.0f, 45.0f, 2.0f, 120.0f, 15.0f, 0.25f), 0);
    Arena arena(champion, enemies, upgrades);
    const bool survived = arena.simulateWave(1);
    expect(survived, "Strong test character should survive Wave 1");
    expect(arena.getTotalIgcEarned() > 0, "Player should earn IGC after clearing Wave 1");

    const auto choices = arena.generateUpgradeChoices(3);
    const std::size_t expectedCount = std::min<std::size_t>(3U, upgrades.size());
    expect(choices.size() == expectedCount,
           "Should generate expected number of distinct upgrade choices");

    arena.applyChosenUpgrade(choices[0]);
    expect(arena.finalizeRunIgc() == arena.getTotalIgcEarned() + 10,
           "Final IGC should include +10 bonus per wave");
}

} // namespace

int main() {
    try {
        testStatsCombatAndClamping();
        testCharacterUpgradesXpAndCopy();
        testEnemyMovementAndScaling();
        testArenaSimulationAndJsonLoader();
    } catch (const std::exception& e) {
        std::cerr << "[FAIL] " << e.what() << "\n";
        return 1;
    }

    std::cout << "All Pecking Order unit tests passed!\n";
    return 0;
}