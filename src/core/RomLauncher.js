const fs = require('fs');
const path = require('path');
const { spawn } = require('child_process');
const log = require('../utils/logger');

class RomLauncher {
    constructor(configManager) {
        this.configManager = configManager;
    }

    findCoreForExtension(config, extension) {
        const matches = config.cores.filter(core => core.extension.toLowerCase() === extension.toLowerCase());

        if (matches.length === 0) {
            log.error(`Uh Oh, No core found for .${extension}`);
            return null;
        }

        return matches.sort((a, b) => (a.priority || 0) - (b.priority || 0))[0];
    }

    async launch(romPath) {
        try {
            if (!fs.existsSync(romPath)) {
                throw new Error(`ROM file not found: ${romPath}`);
            }

            const extension = path.extname(romPath).substring(1).toLowerCase();
            log.debug(`ROM extension: .${extension}`);
            log.info(`Launching ROM: ${romPath}`);

            const config = this.configManager.load();

            let retroarchInstallPath = config.retroarch_install_path;
            if (config.auto_detect_retroarch && !fs.existsSync(path.join(retroarchInstallPath, 'retroarch.exe'))) {
                const detected = this.configManager.autoDetectRetroArch();
                if (detected) {
                    retroarchInstallPath = detected;
                } else {
                    throw new Error('RetroArch installation not found');
                }
            }

            // Now actually find the core
            const coreConfig = this.findCoreForExtension(config, extension);
            if (!coreConfig) {
                throw new Error(`No core found for .${extension}`);
            }

            log.info(`Using core: ${coreConfig.core} (${coreConfig.description || 'No description'})`);

            // Build paths
            const retroarchPath = path.join(retroarchInstallPath, 'retroarch.exe');
            const corePath = path.join(retroarchInstallPath, 'cores', coreConfig.core);

            if (!fs.existsSync(retroarchPath)) {
                throw new Error(`Uh Oh, RetroArch executable not at: ${retroarchPath}`);
            }
            if (!fs.existsSync(corePath)) {
                throw new Error(`Uh Oh, Core not found: ${corePath}`);
            }

            // Build commands
            const args = ['-L', corePath, romPath];
            if (config.launch_fullscreen) {
                args.unshift('--fullscreen');
            }

            log.info(`Launching RetroArch...`);
            log.debug(`Command: "${retroarchPath}" ${args.map(arg => `"${arg}"`).join(' ')}`);

            const child = spawn(retroarchPath, args, {
                detached: true,
                stdio: 'ignore'
            });

            child.unref();
            log.success('RetroArch launched successfully!');

        } catch (error) {
            throw error;
        }
    }

    verify() {
        try {
            const config = this.configManager.load();
            log.info('config.json loaded successfully.');

            // Check RetroArch
            const retroarchPath = path.join(config.retroarch_install_path, 'retroarch.exe');
            if (!fs.existsSync(retroarchPath)) {
                log.error(`Uh Oh, RetroArch not at: ${retroarchPath}`);

                if (config.auto_detect_retroarch) {
                    const detectedPath = this.configManager.autoDetectRetroArch();

                    if (detectedPath) {
                        log.info(`Found RetroArch at: ${detectedPath}`);
                        config.retroarch_install_path = detectedPath;
                        this.configManager.save(config);
                        log.success('Updated config with detected RetroArch path.');
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            } else {
                log.success(`RetroArch executable found at: ${retroarchPath}`);
            }

            // Check cores
            const coresPath = path.join(config.retroarch_install_path, 'cores');
            const missingCores = [];

            config.cores.forEach(coreConfig => {
                const corePath = path.join(coresPath, coreConfig.core);
                if (!fs.existsSync(corePath)) {
                    missingCores.push(coreConfig.core);
                }
            });

            if (missingCores.length > 0) {
                log.error(`Uh Oh, Missing cores: ${missingCores.join(', ')}`);
                log.info('You can install the missing cores through RetroArch\'s online updater !');
            } else {
                log.success('Yipee! All cores mentioned in the config were found!');
            }

            return true;
        } catch (error) {
            log.error(`Failed to verify installation: ${error.message}`);
            return false;
        }
    }
}

module.exports = RomLauncher;