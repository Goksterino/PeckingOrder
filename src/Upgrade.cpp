#include "pecking_order/Upgrade.hpp"

namespace pecking_order {

Upgrade::Upgrade(const std::string& name, const std::string& rarity,
                 int weight, const Stats& bonusStats)
    : name(name),
      rarity(rarity),
      weight(weight > 0 ? weight : 1),
      bonusStats(bonusStats) {}

const std::string& Upgrade::getName() const { return name; }
const std::string& Upgrade::getRarity() const { return rarity; }
int Upgrade::getWeight() const { return weight; }
const Stats& Upgrade::getBonusStats() const { return bonusStats; }

std::ostream& operator<<(std::ostream& os, const Upgrade& upgrade) {
    os << upgrade.name << " (" << upgrade.rarity << ") -> " << upgrade.bonusStats;
    return os;
}

} // namespace pecking_order