#include <nlohmann/json.hpp>
#include <string>

#ifndef CONFIGMANAGER_H
    #define CONFIGMANAGER_H

    class ConfigManager {
        public:
            ConfigManager();

            nlohmann::json defaultConfig;
            std::string configPath;

            bool exists();
            void generate(bool force = false);
            void save(nlohmann::json& config);
            std::string getVersion();
            nlohmann::json load(bool backup = false);
            std::string autoDetectRetroArch();
            std::string createBackup(nlohmann::json& config);
            std::string getConfigPath();
        
        private:
            std::string getTimestamp();
    };
#endif

