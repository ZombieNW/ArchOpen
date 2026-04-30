#pragma once

#include <string>

namespace commands {
    int showHelp();
    int showVersion();
    int generateConfig(bool force = false);
    int listCores();
    int migrateConfig();
    int verifyInstall();
    int launchRom(std::string& romPath);
}
