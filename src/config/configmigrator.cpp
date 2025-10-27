#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

#include "configmigrator.h"
#include "main.h"

ConfigMigrator::ConfigMigrator() {
    migrations["0.5.0"] = migrateFrom050to060;
    migrations["0.6.0"] = migrateFrom060to070;
}

// Checks if a config needs to be migrated based on its version
bool ConfigMigrator::needsMigration(const nlohmann::json& config, std::string_view targetVersion) const {
    if (!config.contains("version") || !config["version"].is_string()) {
        return true; // No version = probably unc status
    }

    std::string currentVersion = config["version"].get<std::string>();
    return compareVersions(currentVersion, targetVersion) < 0;
}

// Basically string to float comparator with more evil involved
int ConfigMigrator::compareVersions(std::string_view a, std::string_view b) {
    // Split helper function
    auto parseVersion = [](std::string_view v) {
        std::vector<int> parts;
        std::string str(v);
        std::stringstream ss(str);
        std::string token;
        
        while (std::getline(ss, token, '.')) {
            try {
                parts.push_back(std::stoi(token));
            } catch (...) {
                parts.push_back(0);
            }
        }
        return parts;
    };

    // Set and mix
    auto versionA = parseVersion(a);
    auto versionB = parseVersion(b);
    size_t maxLen = std::max(versionA.size(), versionB.size());

    // Compare and return
    for (size_t i = 0; i < maxLen; ++i) {
        int numA = (i < versionA.size()) ? versionA[i] : 0;
        int numB = (i < versionB.size()) ? versionB[i] : 0;
        
        if (numA < numB) return -1;
        if (numA > numB) return 1;
    }
    return 0;
}

nlohmann::json ConfigMigrator::migrate(const nlohmann::json& oldConfig, std::string_view targetVersion) const {
    std::string oldVersion = oldConfig.value("version", "0.5.0"); // 0.5.0 is default
    std::cout << "Migrating config from v" << oldVersion << " to v" << targetVersion << "...\n";

    nlohmann::json migratedConfig = oldConfig;

    // Sort migration versions in ascending order
    std::vector<std::string> sortedVersions;
    sortedVersions.reserve(migrations.size());
    for (const auto& [v, _] : migrations) {
        sortedVersions.push_back(v);
    }

    std::sort(sortedVersions.begin(), sortedVersions.end(), 
        [](const std::string& a, const std::string& b) {
            return compareVersions(a, b) < 0;
        });

    // Migrate one function at a time
    for (const auto& version : sortedVersions) {
        if (compareVersions(oldVersion, version) <= 0 && 
            compareVersions(targetVersion, version) >= 0) {
            std::cout << "Applying migration for v" << version << "...\n";
            migratedConfig = migrations.at(version)(migratedConfig);
        }
    }

    migratedConfig["version"] = std::string(targetVersion);
    return migratedConfig;
}

nlohmann::json ConfigMigrator::migrateFrom050to060(const nlohmann::json& oldConfig) {
    nlohmann::json migrated;

    // Migrate settings
    migrated["version"] = "0.6.0";
    migrated["retroarch_install_path"] = oldConfig.value("retroarch_install_path", "C:\\RetroArch-Win64");
    migrated["auto_detect_retroarch"] = true;
    migrated["launch_fullscreen"] = false;
    migrated["backup_configs"] = true;

    // Migrate cores
    if (oldConfig.contains("cores") && oldConfig["cores"].is_array()) {
        migrated["cores"] = oldConfig["cores"];
    } else {
        migrated["cores"] = nlohmann::json::array();
    }

    return migrated;
}

nlohmann::json ConfigMigrator::migrateFrom060to070(const nlohmann::json& oldConfig) {
    nlohmann::json migrated = oldConfig;

    migrated["version"] = "0.7.0";

    return migrated;
}