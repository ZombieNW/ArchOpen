#include <iostream>
#include <string>
#include <sys/stat.h>
#include <set>
#include <limits>
#include <cmath>
#include <filesystem>

#include "commands.h"
#include "main.h"


std::string version = "0.7.0";

int pauseAndExit(int exitCode) {
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear buffer
    exit(exitCode);
}

bool isFilePath(const std::string& path) {
    try {
        std::filesystem::path p(path);
        return p.has_filename() || p.has_parent_path(); 
    } catch (const std::filesystem::filesystem_error& e) {
        return false;
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            showHelp();
            pauseAndExit(0);
        }

        // Get user command
        std::string command = argv[1];
        
        // Command options
        const std::set<std::string> helpCommands = {"help", "-h", "--help"};
        const std::set<std::string> versionCommands = {"version", "-v", "--version"};
        const std::set<std::string> genConfigCommands = {"--generate-config", "-gc"};
        const std::set<std::string> listCommands = {"--list-cores", "-lc", "-l"};
        const std::set<std::string> verifyCommands = {"--verify", "-v"};
        const std::set<std::string> migrateCommands = {"--migrate", "-m"};

        if (helpCommands.count(command)) {
            showHelp();
        }

        else if (versionCommands.count(command)) {
            showVersion();
        }

        else if (genConfigCommands.count(command)) {
            generateConfig(true);
        }

        else if (listCommands.count(command)) {
            listCores();
        }

        else if (verifyCommands.count(command)) {
            std::cout << "ArchOpen Verify:\n";
        }

        else if (migrateCommands.count(command)) {
            migrateConfig();
        }

        else if (isFilePath(command)) {
            //launchROM(command);
        }
        
        else {
            std::cerr << "Unknown command: " << command << "\n";
            pauseAndExit(1);
        }

        pauseAndExit(0);
    }
    catch (const std::exception& e) {
        std::cerr << "Uh Oh: " << e.what() << '\n';
        pauseAndExit(1);
    }
};