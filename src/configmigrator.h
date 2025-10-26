#pragma once
#include <nlohmann/json.hpp>
#include <map>
#include <functional>
#include <string>
#include <vector>

class ConfigMigrator {
    public:
        ConfigMigrator();

        bool needsMigration(const nlohmann::json& config, const std::string& targetVersion);
        int compareVersions(const std::string& a, const std::string& b);
        nlohmann::json migrate(const nlohmann::json& oldConfig, const std::string& targetVersion);

    private:
        std::map<std::string, std::function<nlohmann::json(const nlohmann::json&)>> migrations;

        nlohmann::json migrateFrom050to060(const nlohmann::json& oldConfig);
        nlohmann::json migrateFrom060to070(const nlohmann::json& oldConfig);
};
