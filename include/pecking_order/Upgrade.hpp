#pragma once
#include <ostream>
#include <string>
#include "pecking_order/Stats.hpp"

namespace pecking_order {

class Upgrade {
private:
    std::string name;
    std::string rarity;
    int weight;        
    Stats bonusStats; 

public:
    Upgrade(const std::string& name, const std::string& rarity,
            int weight, const Stats& bonusStats);

    const std::string& getName() const;
    const std::string& getRarity() const;
    int getWeight() const;
    const Stats& getBonusStats() const;

    friend std::ostream& operator<<(std::ostream& os, const Upgrade& upgrade);
};

}