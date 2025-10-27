#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <filesystem>

class ConfigManager {
    public:
        ConfigManager();

        bool exists() const;
        void generate(bool force = false);
        nlohmann::json load(bool backup = true) const;
        std::string autoDetectRetroArch() const;
        
        const std::filesystem::path& getConfigPath() const { return configPath; }

    private:
        std::filesystem::path configPath;
        nlohmann::json defaultConfig;

        void save(const nlohmann::json& config) const;
        std::string createBackup(const nlohmann::json& config) const;
        static std::string getTimestamp();
        static std::filesystem::path getExecutableDir();
};