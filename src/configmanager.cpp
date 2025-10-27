#include <fstream>
#include <iostream>
#include <ctime>
#include <array>
#include <windows.h>

#include "configmanager.h"
#include "configmigrator.h"
#include "main.h"

namespace fs = std::filesystem;

ConfigManager::ConfigManager() 
    : configPath(getExecutableDir() / "config.json")
{
    defaultConfig = {
        {"version", version},
        {"retroarch_install_path", "C:\\RetroArch-Win64"},
        {"auto_detect_retroarch", true},
        {"launch_fullscreen", false},
        {"backup_configs", true},
        {"cores", {
            {
                {"extension", "nes"},
                {"core", "nestopia_libretro.dll"},
                {"description", "Nintendo Entertainment System"}
            },
            {
                {"extension", "sfc"},
                {"core", "snes9x_libretro.dll"},
                {"description", "Super Nintendo"}
            },
            {
                {"extension", "snes"},
                {"core", "snes9x_libretro.dll"},
                {"description", "Super Nintendo"}
            },
            {
                {"extension", "smd"},
                {"core", "genesis_plus_gx_libretro.dll"},
                {"description", "Sega Genesis/Mega Drive"}
            },
            {
                {"extension", "gen"},
                {"core", "genesis_plus_gx_libretro.dll"},
                {"description", "Sega Genesis"}
            },
            {
                {"extension", "z64"},
                {"core", "mupen64plus_next_libretro.dll"},
                {"description", "Nintendo 64 (big endian)"}
            },
            {
                {"extension", "n64"},
                {"core", "mupen64plus_next_libretro.dll"},
                {"description", "Nintendo 64"}
            },
            {
                {"extension", "v64"},
                {"core", "mupen64plus_next_libretro.dll"},
                {"description", "Nintendo 64 (byte-swapped)"}
            },
            {
                {"extension", "chd"},
                {"core", "swanstation_libretro.dll"},
                {"description", "PlayStation (CHD)"}
            },
            {
                {"extension", "cue"},
                {"core", "swanstation_libretro.dll"},
                {"description", "PlayStation (CUE/BIN)"}
            },
            {
                {"extension", "iso"},
                {"core", "swanstation_libretro.dll"},
                {"description", "PlayStation (ISO)"}
            },
            {
                {"extension", "gcm"},
                {"core", "dolphin_libretro.dll"},
                {"description", "GameCube"}
            },
            {
                {"extension", "iso"},
                {"core", "dolphin_libretro.dll"},
                {"description", "GameCube/Wii (ISO)"},
                {"priority", 2}
            },
            {
                {"extension", "wbfs"},
                {"core", "dolphin_libretro.dll"},
                {"description", "Wii (WBFS)"}
            },
            {
                {"extension", "gba"},
                {"core", "mgba_libretro.dll"},
                {"description", "Game Boy Advance"}
            },
            {
                {"extension", "gb"},
                {"core", "gambatte_libretro.dll"},
                {"description", "Game Boy"}
            },
            {
                {"extension", "gbc"},
                {"core", "gambatte_libretro.dll"},
                {"description", "Game Boy Color"}
            }
        }}
    };
};

// I Hate windows path handling in C++
fs::path ConfigManager::getExecutableDir() {
    wchar_t buffer[MAX_PATH]; // Character buffer, MAX_PATH is 260
    DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH); // Use windows api to get the current executable path and store it in buffer
    
    if (length == 0 || length == MAX_PATH) {
        throw std::runtime_error("Failed to get executable path");
    }

    return fs::path(buffer).parent_path(); // Convert wchar_t buffer to filesystem path
}

bool ConfigManager::exists() const {
    return fs::exists(configPath);
}

nlohmann::json ConfigManager::load(bool backup) const {
    // Error checking
    if (!exists()) {
        throw std::runtime_error("config.json doesn't exist");
    }

    std::ifstream file(configPath);
    if (!file.is_open()) {
        throw std::runtime_error("Couldn't open config file: " + configPath.string());
    }
    
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
        std::cout << "Config version is outdated, migrating...\n";

        if (backup) {
            std::string backupPath = createBackup(config);
            if (!backupPath.empty()) {
                std::cout << "Backup created at: " << backupPath << "\n";
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
        std::cerr << "config.json already exists! (Use --force to overwrite it.)\n";
        return;
    }

    // Backup existing config if overwriting
    if (exists() && force) {
        std::cout << "Overwriting existing config.json...\n";

        try {
            nlohmann::json existingConfig = load(false);
            std::string backupPath = createBackup(existingConfig);

            if (!backupPath.empty()) {
                std::cout << "Backup created at: " << backupPath << "\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Uh Oh, couldn't backup: " << e.what() << "\n";
        }
    }

    // Store default config in a config file
    save(defaultConfig);
}

// Backup existing config to file
std::string ConfigManager::createBackup(const nlohmann::json& config) const {
    std::string backupPath = configPath.string() + "." + getTimestamp() + ".bak";
    
    try {
        std::ofstream backup(backupPath);
        if (!backup.is_open()) {
            throw std::runtime_error("Couldn't open backup file for writing");
        }

        backup << config.dump(4);
        return backupPath;
    }
    catch (const std::exception& e) {
        std::cerr << "Uh Oh, Couldn't Create Backup: " << e.what() << "\n";
        return "";
    }
}

// Save config to file
void ConfigManager::save(const nlohmann::json& config) const {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        throw std::runtime_error("Couldn't open file for writing: " + configPath.string());
    }

    // Write config
    file << config.dump(4);
    std::cout << "Yipee! config.json has been generated at: " << configPath.string() << "\n";
}

// Get potential retroarch paths and see if any are valid
std::string ConfigManager::autoDetectRetroArch() const {
    const char* username = std::getenv("USERNAME");
    std::string userPath = username ? std::string(username) : "Default";
    
    const std::array<std::string, 6> possiblePaths = {
        "C:\\Program Files\\RetroArch",
        "C:\\Program Files (x86)\\RetroArch",
        "C:\\RetroArch",
        "C:\\RetroArch-Win64",
        "C:\\Users\\" + userPath + "\\AppData\\Local\\Programs\\RetroArch",
        "C:\\Users\\" + userPath + "\\AppData\\Roaming\\RetroArch"
    };

    // Check each possible path
    for (const auto& path : possiblePaths) {
        if (fs::exists(fs::path(path) / "retroarch.exe")) {
            return path;
        }
    }

    return "";
}

std::string ConfigManager::getTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm local_time{};
    localtime_s(&local_time, &now);

    std::stringstream ss;
    ss << std::put_time(&local_time, "%Y-%m-%d-%H-%M-%S");
    return ss.str();
}
