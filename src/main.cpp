#include <iostream>
#include <string>
#include <set>
#include <limits>

#include "cli/commands.h"
#include "main.h"
#include "cli/logger.h"
#include <filesystem>


std::string version = "0.8.0";

int pauseAndExit(int exitCode) {
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear buffer
    exit(exitCode);
}

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            commands::showHelp();
            pauseAndExit(0);
        }

        // User Command
        std::string command = argv[1];

        // Command table
        const std::set<std::string> helpCommands = {"help", "-h", "--help"};
        const std::set<std::string> versionCommands = {"version", "-v", "--version"};
        const std::set<std::string> genConfigCommands = {"--generate-config", "-gc"};
        const std::set<std::string> listCommands = {"--list-cores", "-lc", "-l"};
        const std::set<std::string> verifyCommands = {"--verify", "-vr"};
        const std::set<std::string> migrateCommands = {"--migrate", "-m"};

        // Handle commands
        if (helpCommands.count(command)) {
            int result = commands::showHelp();
            pauseAndExit(result);
        }
        
        else if (versionCommands.count(command)) {
            std::cout << "ArchOpen v" << version << "\n";
            pauseAndExit(0);
        }
        
        else if (genConfigCommands.count(command)) {
            int result = commands::generateConfig(true);
            pauseAndExit(result);
        }
        
        else if (listCommands.count(command)) {
            int result = commands::listCores();
            pauseAndExit(result);
        }
        
        else if (migrateCommands.count(command)) {
            int result = commands::migrateConfig();
            pauseAndExit(result);
        }
        
        else if (verifyCommands.count(command)) {
            int result = commands::verifyInstall();
            pauseAndExit(result);
        }

        // Probably a file path, rom launch tme !!!
        if (std::filesystem::is_regular_file(command)) {
            int result = commands::launchRom(command);
            if (result == 0) {
                exit(0);
            }
            else {
                pauseAndExit(result);
            }
        }

        // Unknown command
        logger::logError("Unknown command: " + command);
        pauseAndExit(1);
        
    }
    catch (const std::exception& e) {
        logger::logError(e.what());
        pauseAndExit(1);
    }
};