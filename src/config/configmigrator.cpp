#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "configmigrator.h"
#include "cli/logger.h"

ConfigMigrator::ConfigMigrator() {
    migrations["0.6.0"] = migrateFrom050to060;
    migrations["0.7.0"] = migrateFrom060to070;
}

nlohmann::json ConfigMigrator::migrate(const nlohmann::json& oldConfig, std::string_view targetVersion) const {
    // oldest default if no version
    std::string currentVersion = oldConfig.value("version", "0.5.0");
    logger::logInfo("Migrating config from v" + currentVersion + " to v" + std::string(targetVersion) + "...");

    // collect/sort migration keys
    std::vector<std::string> steps;
    steps.reserve(migrations.size());
    for (const auto& [v, _] : migrations) {
        steps.push_back(v);
    }
    std::sort(steps.begin(), steps.end(),
        [](const std::string& a, const std::string& b) {
            return compareVersions(a, b) < 0;
        });

    nlohmann::json config = oldConfig;

    // apply each migration in order
    for (const auto& stepVersion : steps) {
        if (compareVersions(currentVersion, stepVersion) < 0 &&
            compareVersions(stepVersion, targetVersion) <= 0) {
            logger::logInfo("Applying migration -> v" + stepVersion + "...");
            config = migrations.at(stepVersion)(config);
            currentVersion = stepVersion;
        }
    }

    config["version"] = std::string(targetVersion);
    return config;
}

// ====================
// Helpers
// ====================

// Checks if a config needs to be migrated based on its version
bool ConfigMigrator::needsMigration(const nlohmann::json& config, std::string_view targetVersion) const {
    if (!config.contains("version") || !config["version"].is_string()) {
        return true; // treat as oldest
    }
    return compareVersions(config["version"].get<std::string>(), targetVersion) < 0;
}

// Basically string to float comparator with more evil involved
int ConfigMigrator::compareVersions(std::string_view a, std::string_view b) {
    auto parse = [](std::string_view sv) {
        std::vector<int> parts;
        std::stringstream ss{std::string(sv)};
        std::string token;
        while (std::getline(ss, token, '.')) {
            try   { parts.push_back(std::stoi(token)); }
            catch (...) { parts.push_back(0); }
        }
        return parts;
    };

    auto va = parse(a);
    auto vb = parse(b);
    const size_t len = std::max(va.size(), vb.size());

    for (size_t i = 0; i < len; ++i) {
        const int na = (i < va.size()) ? va[i] : 0;
        const int nb = (i < vb.size()) ? vb[i] : 0;
        if (na < nb) return -1;
        if (na > nb) return  1;
    }
    return 0;
}

// ====================
// Individual Migration Steps
// ====================

nlohmann::json ConfigMigrator::migrateFrom050to060(const nlohmann::json& old) {
    nlohmann::json migrated;
    migrated["version"] = "0.6.0";
    migrated["retroarch_install_path"] = old.value("retroarch_install_path", "C:\\RetroArch-Win64");
    migrated["auto_detect_retroarch"] = true;
    migrated["launch_fullscreen"] = false;
    migrated["backup_configs"] = true;
    migrated["cores"] = old.contains("cores") && old["cores"].is_array() ? old["cores"] : nlohmann::json::array();
    return migrated;
}

nlohmann::json ConfigMigrator::migrateFrom060to070(const nlohmann::json& old) {
    nlohmann::json migrated = old;
    migrated["version"] = "0.7.0";
    return migrated;
}
