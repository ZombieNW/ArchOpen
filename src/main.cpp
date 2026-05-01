#include <filesystem>
#include <iostream>
#include <string>
#include <set>

#include "cli/commands.h"
#include "main.h"
#include "cli/logger.h"

const std::string version = "0.9.0";

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            return commands::showHelp();
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
            return commands::showHelp();
        }

        else if (versionCommands.count(command)) {
            std::cout << "ArchOpen v" << version << "\n";
            return 0;
        }

        else if (genConfigCommands.count(command)) {
            return commands::generateConfig(true);
        }

        else if (listCommands.count(command)) {
            return commands::listCores();
        }

        else if (migrateCommands.count(command)) {
            return commands::migrateConfig();
        }

        else if (verifyCommands.count(command)) {
            return commands::verifyInstall();
        }

        // Probably a file path, rom launch tme !!!
        if (std::filesystem::is_regular_file(command)) {
            return commands::launchRom(command);
        }

        // Unknown command
        logger::logError("Unknown command: " + command);
        return 1;

    }
    catch (const std::exception& e) {
        logger::logError(e.what());
        return 1;
    }
};
