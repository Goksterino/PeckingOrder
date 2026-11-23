#include <iostream>
#include <string>
#include <vector>
#include <tabulate/table.hpp>
#include "pecking_order/Arena.hpp"

int main() {
    using namespace pecking_order;

    const auto characters = loadCharactersFromJson("data/characters.json");
    const auto enemies = loadEnemiesFromJson("data/enemies.json");
    const auto upgrades = loadUpgradesFromJson("data/upgrades.json");

    if (characters.empty() || enemies.empty() || upgrades.empty()) {
        std::cerr << "Eroare: datele jocului nu au putut fi incarcate din folderul data/.\n";
        return 1;
    }

    tabulate::Table charTable;
    charTable.add_row({"#", "Caracter", "Cost (IGC)", "HP", "DMG", "Armura"});
    for (std::size_t i = 0; i < characters.size(); ++i) {
        const auto& st = characters[i].getCurrentStats();
        charTable.add_row({
            std::to_string(i + 1),
            characters[i].getName(),
            std::to_string(characters[i].getUnlockCostIgc()),
            std::to_string(static_cast<int>(st.getMaxHp())),
            std::to_string(static_cast<int>(st.getDamage())),
            std::to_string(static_cast<int>(st.getArmor()))
        });
    }
    charTable.format().locale("C");
    std::cout << "=== PECKING ORDER ===\n" << charTable << "\n\nAlege un caracter: ";

    std::size_t charChoice = 1;
    if (!(std::cin >> charChoice) || charChoice < 1 || charChoice > characters.size()) {
        charChoice = 1;
    }

    Character selectedCopy(characters[charChoice - 1]);
    Character player = selectedCopy;
    player = selectedCopy;

    std::cout << "\nAi ales:\n" << player << "\n"
              << "DPS estimat initial vs armura 10: "
              << player.calculateEffectiveDps(10.0f) << "\n\n";

    Arena arena(player, enemies, upgrades);

    for (int wave = 1; wave <= 3 && arena.isPlayerAlive(); ++wave) {
        const bool survived = arena.simulateWave(wave);
        std::cout << arena << "\n\n";

        if (!survived) {
            break;
        }

        const auto choices = arena.generateUpgradeChoices(3);
        if (!choices.empty()) {
            std::cout << "Alege un upgrade pentru valul urmator:\n";
            for (std::size_t i = 0; i < choices.size(); ++i) {
                std::cout << i + 1 << ". [" << choices[i].getRarity() << "] "
                          << choices[i] << "\n";
            }

            std::size_t upgChoice = 1;
            if (!(std::cin >> upgChoice) || upgChoice < 1 || upgChoice > choices.size()) {
                upgChoice = 1;
            }
            arena.applyChosenUpgrade(choices[upgChoice - 1]);
        }
    }

    const int finalIgc = arena.finalizeRunIgc();
    std::cout << "\nRun incheiat la valul " << arena.getCurrentWave()
              << "! IGC din inamici: " << arena.getTotalIgcEarned()
              << " | Total cu bonus: " << finalIgc << " IGC\n";

    for (const auto& c : characters) {
        if (c.getUnlockCostIgc() > 0 && finalIgc >= c.getUnlockCostIgc()) {
            std::cout << "-> Poti debloca caracterul: " << c.getName() << "!\n";
        }
    }

    return 0;
}