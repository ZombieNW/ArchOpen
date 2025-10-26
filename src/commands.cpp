#include "commands.h"
#include "main.h"
#include <iostream>
#include "configmanager.h"

int showHelp() {
    std::cout << "ArchOpen v" << version << " - By ZombieNW\n";
    std::cout << "\n";
    std::cout << "Usage:\n";
	std::cout << "  archopen.exe [rompath]              Launch a ROM file\n";
	std::cout << "  archopen.exe --help, -h            Show this help\n";
	std::cout << "  archopen.exe --generate-config, -gc Generate example config\n";
	std::cout << "  archopen.exe --list-cores, -lc     List configured cores\n";
	std::cout << "  archopen.exe --verify, -v          Verify installation\n";
	std::cout << "\n";
	std::cout << "Examples:\n";
	std::cout << "  archopen.exe \"C:\\Roms\\game.smc\"\n";
	std::cout << "  archopen.exe --generate-config\n";
    std::cout << "\n";
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