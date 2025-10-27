#pragma once

#include <string>
#include "config/configmanager.h"

namespace commands {
    int showHelp();
    int showVersion();
    int generateConfig(bool force = false);
    int listCores();
    int migrateConfig();
    int verifyInstall();
    int launchRom(std::string& romPath);
}