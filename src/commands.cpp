#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "configmanager.h"
#include "commands.h"
#include "main.h"

int showHelp() {
    std::cout << "ArchOpen v" << version << " - By ZombieNW\n";
    std::cout << "\n";
    std::cout << "Usage:\n";
	std::cout << "  archopen.exe [rompath]              Launch a ROM file\n";
	std::cout << "  archopen.exe --help, -h             Show this help\n";
	std::cout << "  archopen.exe --generate-config, -gc Generate example config\n";
	std::cout << "  archopen.exe --list-cores, -lc      List configured cores\n";
	std::cout << "  archopen.exe --verify, -v           Verify installation\n";
	std::cout << "\n";
	std::cout << "Examples:\n";
	std::cout << "  archopen.exe \"C:\\Roms\\game.smc\"\n";
	std::cout << "  archopen.exe --generate-config\n";
    std::cout << "\n";
    return 0;
}

int showVersion() {
    std::cout << "ArchOpen v" << version << "\n";
    return 0;
}

int generateConfig(bool force = false) {
    std::cout << "Generating config.json...\n";
    ConfigManager configManager;
    try {
        configManager.generate(force);
        return 0;
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}

int listCores() {
    std::cout << "Listing cores...\n";
    ConfigManager configManager;
    try {
        const nlohmann::json& config = configManager.load(false);

        std::unordered_map<std::string, std::unordered_set<std::string>> coreExtensions;
        for (const auto& core : config["cores"]) {
            const std::string coreName = core["core"].get<std::string>();
            const std::string extension = core["extension"].get<std::string>();
            coreExtensions[coreName].insert(extension);
        }

        std::cout << "Configured cores:\n";
        for (const auto& coreExtension : coreExtensions) {
            std::cout << "  " << coreExtension.first << " - " << ":\n";
            for (const auto& extension : coreExtension.second) {
                std::cout << "    " << extension << "\n";
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
    ConfigManager configManager;
    try {
        configManager.load(true);
        return 0;
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}