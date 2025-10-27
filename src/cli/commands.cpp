#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "core/romlauncher.h"
#include "styles.h"
#include "main.h"

namespace commands {
    int showHelp() {
        std::cout << styles::underline("ArchOpen v" + version + " - By ZombieNW\n\n")
              << "Usage:\n"
              << "  archopen.exe [rompath]              Launch a ROM file\n"
              << "  archopen.exe --help, -h             Show this help\n"
              << "  archopen.exe --generate-config, -gc Generate example config\n"
              << "  archopen.exe --list-cores, -lc      List configured cores\n"
              << "  archopen.exe --verify, -v           Verify installation\n"
              << "\nExamples:\n"
              << "  archopen.exe \"C:\\Roms\\game.smc\"\n"
              << "  archopen.exe --generate-config\n\n";
        return 0;
    }

    int generateConfig(bool force) {
        std::cout << "Generating config.json...\n";
    
        try {
            ConfigManager configManager;
            configManager.generate(force);
            return 0;
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return 1;
        }
    }

    int listCores() {
        std::cout << "Listing cores...\n";

        try {
            ConfigManager configManager;
            const nlohmann::json& config = configManager.load(false);

            // Group extensions by core
            std::unordered_map<std::string, std::unordered_set<std::string>> coreExtensions;
            for (const auto& core : config["cores"]) {
                std::string coreName = core["core"].get<std::string>();
                std::string extension = core["extension"].get<std::string>();
                coreExtensions[coreName].insert(extension);
            }

            std::cout << "Configured cores:\n";
            for (const auto& [coreName, extensions] : coreExtensions) {
                std::cout << "  " << coreName << ":\n";
                for (const auto& ext : extensions) {
                    std::cout << "    " << ext << "\n";
                }
            }

            return 0;
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            return 1;
        }
    }

    int migrateConfig() {
        std::cout << "Migrating config.json...\n";
        
        try {
            ConfigManager configManager;
            configManager.load(true);
            return 0;
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            return 1;
        }
    }

    int launchRom(std::string& romPath) {
        try {
            ConfigManager configManager;
            RomLauncher launcher(configManager);
            return launcher.launch(romPath) ? 0 : 1;
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return 1;
        }
    }

    int verifyInstall() {
        std::cout << "Verifying ArchOpen installation...\n";
        
        try {
            ConfigManager configManager;
            RomLauncher launcher(configManager);
            return launcher.verify() ? 0 : 1;
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return 1;
        }
    }
}
