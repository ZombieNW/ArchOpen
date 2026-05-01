#include <iostream>

#include "core/romlauncher.h"
#include "styles.h"
#include "main.h"
#include "cli/logger.h"

namespace commands {
    int showHelp() {
        std::cout << styles::underline("ArchOpen v" + version + " - By ZombieNW\n\n")
              << "Usage:\n"
              << "  archopen.exe [rompath]              Launch a ROM file\n"
              << "  archopen.exe --help, -h             Show this help\n"
              << "  archopen.exe --generate-config, -gc Generate example config\n"
              << "  archopen.exe --list-cores, -lc      List configured cores\n"
              << "  archopen.exe --verify, -v           Verify installation\n"
              << "  archopen.exe --migrate, -m          Update config.json to latest version\n"
              << "\nExamples:\n"
              << "  archopen.exe \"C:\\Roms\\game.smc\"\n"
              << "  archopen.exe --generate-config\n\n";
        return 0;
    }

    int generateConfig(bool force) {
        logger::logInfo("Generating config.json...");

        try {
            ConfigManager configManager;
            configManager.generate(force);
            return 0;
        } catch (const std::exception& e) {
            logger::logError(e.what());
            return 1;
        }
    }

    int listCores() {
        logger::logInfo("Listing cores...");

        try {
            ConfigManager configManager;
            const auto& config = configManager.load(false);

            // Check if we have cores
            if (!config.contains("cores") || config["cores"].empty() || !config["cores"].is_array()) {
                logger::logError("No cores configured.");
                return 0;
            }

            // Group extensions by core
            std::map<std::string, std::vector<std::string>> coreExtensions;
            for (const auto& core : config["cores"]) {
                coreExtensions[core["core"]].push_back(core["extension"]);
            }

            std::cout << "Configured cores:\n";
            for (const auto& [coreName, extensions] : coreExtensions) {
                std::cout << "  " << coreName << ":\n";
                for (const auto& ext : extensions) {
                    std::cout << "    - " << ext << "\n";
                }
            }

            return 0;
        }
        catch(const std::exception& e) {
            logger::logError(e.what());
            return 1;
        }
    }

    int migrateConfig() {
        logger::logInfo("Migrating config.json...");

        try {
            ConfigManager configManager;
            configManager.load(true);
            logger::logSuccess("config.json migrated successfully.");
            return 0;
        }
        catch(const std::exception& e) {
            logger::logError(e.what());
            return 1;
        }
    }

    int launchRom(std::string& romPath) {
        try {
            ConfigManager configManager;
            RomLauncher launcher(configManager);
            return launcher.launch(romPath) ? 0 : 1;
        } catch (const std::exception& e) {
            logger::logError(e.what());
            return 1;
        }
    }

    int verifyInstall() {
        logger::logInfo("Verifying ArchOpen installation...");

        try {
            ConfigManager configManager;
            RomLauncher launcher(configManager);
            return launcher.verify() ? 0 : 1;
        } catch (const std::exception& e) {
            logger::logError(e.what());
            return 1;
        }
    }
}
