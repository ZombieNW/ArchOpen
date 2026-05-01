#include <iostream>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <windows.h>

#include "romlauncher.h"
#include "cli/logger.h"

namespace fs = std::filesystem;

static std::string getExtension(const std::string& path) {
    std::string ext = fs::path(path).extension().string();
    if (!ext.empty() && ext.front() == '.') ext.erase(0, 1);
    return ext;
}

static bool launchProcess(const std::string& cmd) {
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi{};

    BOOL ok = CreateProcessA(nullptr, const_cast<char*>(cmd.c_str()),
                             nullptr, nullptr, FALSE, DETACHED_PROCESS,
                             nullptr, nullptr, &si, &pi);
    if (!ok) return false;

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

RomLauncher::RomLauncher(ConfigManager& configManager) : configManager(configManager) {}

std::optional<RomLauncher::CoreConfig> RomLauncher::findCoreForExtension(const nlohmann::json& config, const std::string& extension) const {
    if (!config.contains("cores") || !config["cores"].is_array()) {
        logger::logError("Config missing 'cores' array.");
        return std::nullopt;
    }

    // Iterate over the cores and find one that matches the extension
    std::vector<CoreConfig> matches;
    for (const auto& entry : config["cores"]) {
        if (!entry.contains("extension") || !entry.contains("core")) continue;
        if (entry["extension"].get<std::string>() != extension) continue;

        matches.push_back({
            .core = entry["core"].get<std::string>(),
            .extension = extension,
            .description = entry.value("description", ""),
            .priority = entry.value("priority", 0),
        });
    }

    if (matches.empty()) {
        logger::logError("No core found for extension: ." + extension);
        return std::nullopt;
    }

    std::sort(matches.begin(), matches.end(), [](const CoreConfig& a, const CoreConfig& b) { return a.priority < b.priority; });

    return matches.front();
}

std::string RomLauncher::buildLaunchCommand(const std::string& retroarchExe, const std::string& corePath, const std::string& romPath, bool fullscreen) const {
    std::string cmd = "\"" + retroarchExe + "\"";
    if (fullscreen) cmd += " --fullscreen";
    cmd += " -L \"" + corePath + "\" \"" + romPath + "\"";
    return cmd;
}

bool RomLauncher::launch(const std::string& romPath) {
    try {
        if (!fs::exists(romPath)) throw std::runtime_error("ROM not found: " + romPath);

        const std::string extension = getExtension(romPath);
        logger::logInfo("Loading ROM: " + romPath);

        // load config
        nlohmann::json config = configManager.load(false);
        std::string retroarchRoot = config["retroarch_install_path"];
        fs::path retroarchExe = fs::path(retroarchRoot) / "retroarch.exe";

        // Find retroarch executable
        if (config.value("auto_detect_retroarch", false) && !fs::exists(retroarchExe)) {
            retroarchRoot = configManager.autoDetectRetroArch();
            if (retroarchRoot.empty()) throw std::runtime_error("RetroArch installation not found.");
            retroarchExe = fs::path(retroarchRoot) / "retroarch.exe";
        }
        if (!fs::exists(retroarchExe)) throw std::runtime_error("RetroArch executable not found: " + retroarchExe.string());

        // Find core we want
        const auto core = findCoreForExtension(config, extension);
        if (!core) throw std::runtime_error("No core configured for extension: ." + extension);

        // Find the core itself
        const fs::path corePath = fs::path(retroarchRoot) / "cores" / core->core;
        if (!fs::exists(corePath)) throw std::runtime_error("Core file not found: " + corePath.string());
        std::cout << "  Using core: " << core->core << "\n";
        std::cout << "  Description: " << (core->description.empty() ? "No description" : core->description) << "\n\n";

        // prepare retroarch launch command
        const std::string cmd = buildLaunchCommand(retroarchExe.string(), corePath.string(), romPath, config.value("launch_fullscreen", false));
        logger::logInfo("Launching RetroArch...");
        std::cout << "  Command: " << cmd << "\n\n";

        // actually launch retroarch
        if (!launchProcess(cmd)) throw std::runtime_error("CreateProcess failed.");

        logger::logSuccess("RetroArch launched!\n");
        return true;

    } catch (const std::exception& e) {
        logger::logError("Launch failed: " + std::string(e.what()));
        return false;
    }
}

bool RomLauncher::verify() {
    try {
        // Check Config
        nlohmann::json config = configManager.load(false);
        logger::logSuccess("config.json loaded.\n");

        // Check RetroArch
        std::string retroarchPath = config["retroarch_install_path"];
        fs::path retroarchExe = fs::path(retroarchPath) / "retroarch.exe";

        if (!fs::exists(retroarchExe)) {
            logger::logError("RetroArch not found: " + retroarchExe.string());

            if (config.value("auto_detect_retroarch", false)) {
                const std::string detected = configManager.autoDetectRetroArch();
                if (!detected.empty()) {
                    logger::logInfo("Detected RetroArch at: " + detected);
                    logger::logInfo("Update 'retroarch_install_path' in config.json to apply.");
                }
            }
            return false;
        }

        logger::logSuccess("RetroArch found: " + retroarchExe.string());

        // Check cores
        fs::path coresPath = fs::path(retroarchPath) / "cores";
        std::vector<std::string> missingCores;

        for (const auto& core : config["cores"]) {
            if (!core.contains("core")) continue;

            fs::path corePath = coresPath / core["core"].get<std::string>();
            // Check if core file exists and avoid duplicates in missingCores
            if (!fs::exists(corePath) && std::find(missingCores.begin(), missingCores.end(), core["core"]) == missingCores.end()) {
                missingCores.push_back(core["core"]);
            }
        }

        if (!missingCores.empty()) {
            std::cerr << "Missing cores:\n";
            for (const auto& c : missingCores) std::cerr << "  - " << c << "\n";
            std::cerr << "\nInstall them via RetroArch > Online Updater.\n";
            return false;
        }

        logger::logSuccess("All configured cores are present.");
        return true;

    } catch (const std::exception& e) {
        logger::logError("Failed to verify installation: " + std::string(e.what()));
        return false;
    }
}
