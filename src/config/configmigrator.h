#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <map>
#include <functional>

class ConfigMigrator {
    public:
        ConfigMigrator();

        bool needsMigration(const nlohmann::json& config, std::string_view targetVersion) const;
        nlohmann::json migrate(const nlohmann::json& oldConfig, std::string_view targetVersion) const;

    private:
        using MigrationFunc = std::function<nlohmann::json(const nlohmann::json&)>;
        std::map<std::string, MigrationFunc> migrations;

        static int compareVersions(std::string_view a, std::string_view b);
        static nlohmann::json migrateFrom050to060(const nlohmann::json& oldConfig);
        static nlohmann::json migrateFrom060to070(const nlohmann::json& oldConfig);
};