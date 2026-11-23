#include "pecking_order/Arena.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace pecking_order {

// Funcție helper vizibilă doar în acest fișier (.cpp) pentru a converti un obiect JSON în Stats
namespace {

Stats parseStats(const nlohmann::json& j) {
    return Stats(
        j.at("maxHp").get<float>(),
        j.at("damage").get<float>(),
        j.at("attackSpeed").get<float>(),
        j.at("moveSpeed").get<float>(),
        j.at("armor").get<float>(),
        j.at("critChance").get<float>()
    );
}

} // namespace

std::vector<Character> loadCharactersFromJson(const std::string& filePath) {
    std::vector<Character> result;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        spdlog::error("Could not open characters file: {}", filePath);
        return result;
    }

    nlohmann::json data;
    file >> data;
    for (const auto& item : data) {
        result.emplace_back(
            item.at("name").get<std::string>(),
            parseStats(item.at("stats")),
            item.at("unlockCostIgc").get<int>()
        );
    }
    return result;
}

std::vector<Enemy> loadEnemiesFromJson(const std::string& filePath) {
    std::vector<Enemy> result;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        spdlog::error("Could not open enemies file: {}", filePath);
        return result;
    }

    nlohmann::json data;
    file >> data;
    for (const auto& item : data) {
        result.emplace_back(
            item.at("name").get<std::string>(),
            parseStats(item.at("stats")),
            item.at("spawnDistance").get<float>(),
            item.at("xpReward").get<int>(),
            item.at("igcReward").get<int>()
        );
    }
    return result;
}

std::vector<Upgrade> loadUpgradesFromJson(const std::string& filePath) {
    std::vector<Upgrade> result;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        spdlog::error("Could not open upgrades file: {}", filePath);
        return result;
    }

    nlohmann::json data;
    file >> data;
    for (const auto& item : data) {
        result.emplace_back(
            item.at("name").get<std::string>(),
            item.at("rarity").get<std::string>(),
            item.at("weight").get<int>(),
            parseStats(item.at("bonus"))
        );
    }
    return result;
}

} // namespace pecking_order