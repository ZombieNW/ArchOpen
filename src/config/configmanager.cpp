#include <fstream>
#include <ctime>
#include <array>
#include <windows.h>
#include <format>

#include "configmanager.h"
#include "configmigrator.h"
#include "main.h"
#include "cli/logger.h"

namespace fs = std::filesystem;

ConfigManager::ConfigManager() {
    // Resolve config path next to the executable
    wchar_t buffer[MAX_PATH]{};
    DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    if (length == 0 || length == MAX_PATH) {
        throw std::runtime_error("Failed to get executable path");
    }
    configPath = fs::path(buffer).parent_path() / "config.json";

    defaultConfig = {
        {"version", version},
        {"retroarch_install_path", "C:\\RetroArch-Win64"},
        {"auto_detect_retroarch", true},
        {"launch_fullscreen", false},
        {"backup_configs", true},
        {"cores", {
            {{"extension", "nes"},  {"core", "nestopia_libretro.dll"},        {"description", "Nintendo Entertainment System"}},
            {{"extension", "sfc"},  {"core", "snes9x_libretro.dll"},          {"description", "Super Nintendo"}},
            {{"extension", "snes"}, {"core", "snes9x_libretro.dll"},          {"description", "Super Nintendo"}},
            {{"extension", "smd"},  {"core", "genesis_plus_gx_libretro.dll"}, {"description", "Sega Genesis/Mega Drive"}},
            {{"extension", "gen"},  {"core", "genesis_plus_gx_libretro.dll"}, {"description", "Sega Genesis"}},
            {{"extension", "z64"},  {"core", "mupen64plus_next_libretro.dll"},{"description", "Nintendo 64 (big-endian)"}},
            {{"extension", "n64"},  {"core", "mupen64plus_next_libretro.dll"},{"description", "Nintendo 64"}},
            {{"extension", "v64"},  {"core", "mupen64plus_next_libretro.dll"},{"description", "Nintendo 64 (byte-swapped)"}},
            {{"extension", "chd"},  {"core", "swanstation_libretro.dll"},     {"description", "PlayStation (CHD)"}},
            {{"extension", "cue"},  {"core", "swanstation_libretro.dll"},     {"description", "PlayStation (CUE/BIN)"}},
            {{"extension", "iso"},  {"core", "swanstation_libretro.dll"},     {"description", "PlayStation (ISO)"}},
            {{"extension", "gcm"},  {"core", "dolphin_libretro.dll"},         {"description", "GameCube"}},
            {{"extension", "iso"},  {"core", "dolphin_libretro.dll"},         {"description", "GameCube/Wii (ISO)"}, {"priority", 2}},
            {{"extension", "wbfs"}, {"core", "dolphin_libretro.dll"},         {"description", "Wii (WBFS)"}},
            {{"extension", "gba"},  {"core", "mgba_libretro.dll"},            {"description", "Game Boy Advance"}},
            {{"extension", "gb"},   {"core", "gambatte_libretro.dll"},        {"description", "Game Boy"}},
            {{"extension", "gbc"},  {"core", "gambatte_libretro.dll"},        {"description", "Game Boy Color"}},
        }}
    };
}

bool ConfigManager::exists() const {
    return fs::exists(configPath);
}

nlohmann::json ConfigManager::load(bool backup) const {
    if (!exists()) throw std::runtime_error("config.json doesn't exist");

    std::ifstream file(configPath);
    if (!file.is_open()) throw std::runtime_error("Couldn't open config file: " + configPath.string());

    // Parse JSON
    nlohmann::json config;
    try {
        file >> config;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error(std::string("JSON parse error: ") + e.what());
    }

    // Handle migration if needed
    ConfigMigrator migrator;
    if (migrator.needsMigration(config, version)) {
        logger::logInfo("Config version is outdated, migrating...");

        if (backup) {
            std::string backupPath = createBackup(config);
            if (!backupPath.empty()) {
                logger::logInfo("Backup created at: " + backupPath);
            } else {
                logger::logError("Couldn't create backup!");
            }
        }

        config = migrator.migrate(config, version);
        save(config);
    }

    return config;
}

// Save default config to file
void ConfigManager::generate(bool force) {
    if (exists() && !force) {
        logger::logError("config.json already exists! (Use --force to overwrite it.)\n");
        return;
    }

    // Backup existing config if overwriting
    if (exists() && force) {
        logger::logInfo("Overwriting existing config.json...\n");

        try {
            nlohmann::json existingConfig = load(false);
            std::string backupPath = createBackup(existingConfig);

            if (!backupPath.empty()) {
                logger::logInfo("Backup created at: " + backupPath);
            } else {
                logger::logError("Couldn't create backup!");
            }
        } catch (const std::exception& e) {
            logger::logError("Couldn't create backup: " + std::string(e.what()) + "\n");
        }
    }

    // Store default config in a config file
    save(defaultConfig);
}


std::string ConfigManager::autoDetectRetroArch() const {
    const char* username = std::getenv("USERNAME");
    const std::string userHome = username
        ? std::string("C:\\Users\\") + username
        : "C:\\Users\\Default";

    const std::array<std::string, 6> candidates = {
        "C:\\Program Files\\RetroArch",
        "C:\\Program Files (x86)\\RetroArch",
        "C:\\RetroArch",
        "C:\\RetroArch-Win64",
        userHome + "\\AppData\\Local\\Programs\\RetroArch",
        userHome + "\\AppData\\Roaming\\RetroArch",
    };

    for (const auto& path : candidates) {
        if (fs::exists(fs::path(path) / "retroarch.exe")) {
            return path;
        }
    }

    return {};
}

void ConfigManager::save(const nlohmann::json& config) const {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        throw std::runtime_error("Couldn't open file for writing: " + configPath.string());
    }
    file << config.dump(4);
    logger::logSuccess("config.json written to: " + configPath.string() + "\n");
}

std::string ConfigManager::createBackup(const nlohmann::json& config) const {
    const std::string backupPath = configPath.string() + "." + getTimestamp() + ".bak";
    try {
        std::ofstream backup(backupPath);
        if (!backup.is_open()) {
            throw std::runtime_error("Couldn't open backup file for writing");
        }
        backup << config.dump(4);
        return backupPath;
    } catch (const std::exception& e) {
        logger::logError("Couldn't create backup: " + std::string(e.what()) + "\n");
        return {};
    }
}

std::string ConfigManager::getTimestamp() {
    const std::time_t now = std::time(nullptr);
    std::tm t{};
    localtime_s(&t, &now);
    return std::format("{:04}-{:02}-{:02}-{:02}-{:02}-{:02}",
        t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
        t.tm_hour, t.tm_min, t.tm_sec);
}
