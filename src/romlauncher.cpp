#include <iostream>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <windows.h>

#include "romlauncher.h"

namespace fs = std::filesystem;

RomLauncher::RomLauncher(ConfigManager& configManager) : configManager(configManager) {}

std::optional<RomLauncher::CoreConfig> RomLauncher::findCoreForExtension(const nlohmann::json& config, const std::string& extension) const {
    if (!config.contains("cores") || !config["cores"].is_array()) {
        std::cerr << "Uh Oh, config missing 'cores' array.\n";
        return std::nullopt;
    }

    std::vector<CoreConfig> matches;

    for (const auto& core : config["cores"]) {
        if (!core.contains("extension") || !core.contains("core")) {
            continue;
        }

        std::string ext = core["extension"].get<std::string>();
        if (ext == extension) {
            CoreConfig coreConfig;
            coreConfig.core = core["core"].get<std::string>();
            coreConfig.extension = ext;
            coreConfig.description = core.value("description", "");
            coreConfig.priority = core.value("priority", 0);
            matches.push_back(coreConfig);
        }
    }

    if (matches.empty()) {
        std::cerr << "Uh Oh, no core found for ." << extension << "\n";
        return std::nullopt;
    }

    // Sort by priority
    std::sort(matches.begin(), matches.end(),
        [](const CoreConfig& a, const CoreConfig& b) {
            return a.priority < b.priority;
        });

    return matches[0];
}

bool RomLauncher::launch(const std::string& romPath) {
    try {
        if (!fs::exists(romPath)) {
            throw std::runtime_error("ROM path doesn't exist: " + romPath);
        }

        // Extract extension
        std::string extension = fs::path(romPath).extension().string();
        if (!extension.empty() && extension[0] == '.') {
            extension.erase(0, 1);
        }

        std::cout << "Launching ROM: " << romPath << "\n";

        // Load config
        nlohmann::json config = configManager.load();
        std::string retroarchInstallPath = config["retroarch_install_path"];
        fs::path retroarchExe = fs::path(retroarchInstallPath) / "retroarch.exe";

        // Auto-detect retroarch if needed
        if (config.value("auto_detect_retroarch", false) && !fs::exists(retroarchExe)) {
            std::string detected = configManager.autoDetectRetroArch();
            if (!detected.empty()) {
                retroarchInstallPath = detected;
                retroarchExe = fs::path(retroarchInstallPath) / "retroarch.exe";
            } else {
                throw std::runtime_error("RetroArch installation not found");
            }
        }

        // Find core for extension
        auto coreConfig = findCoreForExtension(config, extension);
        if (!coreConfig) {
            throw std::runtime_error("No core found for ." + extension);
        }

        std::cout << "  Using core: " << coreConfig->core << "\n";
        std::cout << "  Description: " << (coreConfig->description.empty() ? "No description" : coreConfig->description) << "\n";

        // Get core path
        fs::path corePath = fs::path(retroarchInstallPath) / "cores" / coreConfig->core;

        // Verify both paths exist
        if (!fs::exists(retroarchExe)) {
            throw std::runtime_error("RetroArch executable not at: " + retroarchExe.string());
        }
        if (!fs::exists(corePath)) {
            throw std::runtime_error("Core not found: " + corePath.string());
        }

        // Build command
        std::string cmd = "\"" + retroarchExe.string() + "\"";
        if (config.value("launch_fullscreen", false)) cmd += " --fullscreen";
        cmd += " -L \"" + corePath.string() + "\" \"" + romPath + "\"";

        std::cout << "Launching RetroArch...\n";
        std::cout << "  Command: " << cmd << "\n";

        // Launch process
        STARTUPINFOA si = {sizeof(si)};
        PROCESS_INFORMATION pi{};

        BOOL success = CreateProcessA(
            nullptr,
            cmd.data(),
            nullptr,
            nullptr,
            FALSE,
            DETACHED_PROCESS,
            nullptr,
            nullptr,
            &si,
            &pi
        );

        if (!success) {
            throw std::runtime_error("Failed to launch RetroArch process");
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        std::cout << "RetroArch launched successfully!\n";
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Launch failed: " << e.what() << "\n";
        return false;
    }
}

bool RomLauncher::verify() {
    try {
        // Check Config
        nlohmann::json config = configManager.load(false);
        std::cout << "config.json loaded successfully.\n";

        // Check RetroArch
        std::string retroarchPath = config["retroarch_install_path"];
        fs::path retroarchExe = fs::path(retroarchPath) / "retroarch.exe";

        if (!fs::exists(retroarchExe)) {
            std::cerr << "[ERROR] RetroArch not found at: " << retroarchExe.string() << "\n";

            if (config.value("auto_detect_retroarch", false)) {
                std::string detectedPath = configManager.autoDetectRetroArch();
                if (!detectedPath.empty()) {
                    std::cout << "[INFO] Found RetroArch at: " << detectedPath << "\n";
                    config["retroarch_install_path"] = detectedPath;
                    
                    // Note: Can't call save on const config, need to expose save publicly
                    // or return the updated config
                    std::cout << "[INFO] Update config manually with detected path.\n";
                    return false;
                }
            }
            return false;
        }

        std::cout << "[SUCCESS] RetroArch executable found at: " << retroarchExe.string() << "\n";

        // Check cores
        fs::path coresPath = fs::path(retroarchPath) / "cores";
        std::vector<std::string> missingCores;

        for (const auto& core : config["cores"]) {
            if (!core.contains("core")) continue;
            
            fs::path corePath = coresPath / core["core"].get<std::string>();
            if (!fs::exists(corePath)) {
                missingCores.push_back(core["core"]);
            }
        }

        if (!missingCores.empty()) {
            std::cerr << "Missing cores: ";
            for (const auto& c : missingCores) {
                std::cerr << c << " ";
            }
            std::cerr << "\nYou can install the missing cores through RetroArch's Online Updater.\n";
            return false;
        }

        std::cout << "All cores mentioned in config found!\n";
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Failed to verify installation: " << e.what() << "\n";
        return false;
    }
}