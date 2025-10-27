#pragma once

#include <string>
#include "configmanager.h"

namespace commands {
    int showHelp();
    int showVersion();
    int generateConfig(bool force = false);
    int listCores();
    int migrateConfig();
    int verifyInstall();
    int launchRom(std::string& romPath);
}