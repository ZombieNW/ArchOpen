#include <iostream>
#include <string>
#include <sys/stat.h>
#include <set>
#include <limits>
#include <cmath>

#include "commands.h"
#include "main.h"


double version = 0.7;

int pauseAndExit(int exitCode) {
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear buffer
    exit(exitCode);
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
            std::cout << "ArchOpen List Cores:\n";
        }

        else if (verifyCommands.count(command)) {
            std::cout << "ArchOpen Verify:\n";
        }
        
        else { //TODO make this a check if the command is a file path
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