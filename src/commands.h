#ifndef COMMANDS_H
    #define COMMANDS_H

    int showHelp();
    int generateConfig(bool force);
    int showVersion();
    int migrateConfig();
    int listCores();
    int launchRom(const std::string& romPath);
#endif

