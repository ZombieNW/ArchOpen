#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ctime>
#include <set>
#include <windows.h>

#include "configmanager.h"
#include "main.h"

ConfigManager::ConfigManager() {
    this->configPath = this->getConfigPath();
    this->defaultConfig = {
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
std::string ConfigManager::getConfigPath() {
    wchar_t buffer[MAX_PATH]; // Character buffer, MAX_PATH is 260
    DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH); // Use windows api to get the current executable path and store it in buffer
    if (length == 0 || length == MAX_PATH) {
        throw std::runtime_error("Failed to get executable path");
    }

    std::filesystem::path exePath(buffer); // Convert wchar_t buffer to filesystem path
    return (exePath.parent_path() / "config.json").string();
}

bool ConfigManager::exists() {
    return std::filesystem::exists(this->configPath);
}

nlohmann::json ConfigManager::load() {
    // Error checking
    if(!this->exists()) {
        throw std::runtime_error("config.json doesn't exist");
    }

    std::ifstream file(this->configPath); // Load file
    if (!file.is_open()) {
        throw std::runtime_error("Couldn't open config file: " + this->configPath);
    }
    
    // Parse JSON
    nlohmann::json config;
    try {
        file >> config;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error(std::string("JSON parse error: ") + e.what());
    }

    return config;
}

// Save default config to file
void ConfigManager::generate(bool force) {
    if (this->exists() && !force) {
        std::cerr << "config.json already exists ! (You can use --force to overwrite it.)\n";
        return;
    }

    // Backup old config if they force it
    if (this->exists() && force) {
        std::cout << "Overwriting existing config.json...\n";

        try {
            // Load existing config and backup
            nlohmann::json existingConfig = this->load();
            std::string backupPath = this->createBackup(existingConfig);

            if (!backupPath.empty()) {
                std::cout << "Backup created at: " << backupPath << "\n";
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Uh Oh, Couldn't Backup: " << e.what() << "\n";
            return;
        }
    }

    // Store default config in a config file
    this->save(this->defaultConfig);
}

// Backup existing config to file
std::string ConfigManager::createBackup(nlohmann::json& config) {
    std::string backupPath = this->configPath + "." + this->getTimestamp() + ".bak";
    
    try {
        std::ofstream backup(backupPath);
        if (!backup.is_open()) {
            throw std::runtime_error("Couldn't open backup file for writing");
        }

        backup << config.dump(4);
        backup.close();
        return backupPath;
    }
    catch (const std::exception& e) {
        std::cerr << "Uh Oh, Couldn't Create Backup: " << e.what() << "\n";
        return "";
    }
}

// Save config to file
void ConfigManager::save(nlohmann::json& config) {
    try {
        // Open file
        std::ofstream file(this->configPath);
        if (!file.is_open()) {
            throw std::runtime_error("Couldn't open file for writing: " + this->configPath);
        }

        // Write config
        file << config.dump(4);
        file.close();
        std::cout << "Yipee! config.json has been generated at: " << this->configPath << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Uh Oh, couldn't save config: " << e.what() << "\n";
    }
}

// Get potential retroarch paths and see if any are valid
std::string ConfigManager::autoDetectRetroArch() {
    std::set<std::string> possibleRetroArchs = {
        "C:\\Program Files\\RetroArch",
        "C:\\Program Files (x86)\\RetroArch",
        "C:\\RetroArch",
        "C:\\RetroArch-Win64",
        "C:\\Users\\" + std::string(std::getenv("USERNAME")) + "\\AppData\\Local\\Programs\\RetroArch",
        "C:\\Users\\" + std::string(std::getenv("USERNAME")) + "\\AppData\\Roaming\\RetroArch"
    };

    // Check each possible path
    for (const auto& path : possibleRetroArchs) {
        if (std::filesystem::exists(path + "\\retroarch.exe")) {
            return path;
        }
    }

    return "";
}

std::string ConfigManager::getVersion() {
    try {
        nlohmann::json config = this->load();
        if (config.contains("version")) {
            return config["version"].get<std::string>();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Uh Oh, Couldn't Get Version: " << e.what() << "\n";
    }
    return "0.5.0";
}

std::string ConfigManager::getTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm local_time{};
    localtime_s(&local_time, &now);

    std::stringstream ss;
    ss << std::put_time(&local_time, "%Y-%m-%d-%H-%M-%S");
    return ss.str();
}