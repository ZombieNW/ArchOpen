#pragma once

#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include "configmanager.h"

class RomLauncher {
    public:
        explicit RomLauncher(ConfigManager& configManager);

        bool launch(const std::string& romPath);
        bool verify();
    
    private:
        struct CoreConfig {
            std::string core;
            std::string extension;
            std::string description;
            int priority = 0;
        };

        ConfigManager& configManager;

        std::optional<CoreConfig> findCoreForExtension(
            const nlohmann::json& config, 
            const std::string& extension
        ) const;
};