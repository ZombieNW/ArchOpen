use std::{path::Path, process::Command};

use crate::{
    config_manager::{Config, ConfigManager, Cores},
    logger,
};

pub struct RomLauncher {
    config_manager: ConfigManager,
}

impl RomLauncher {
    pub fn new(config_manager: ConfigManager) -> Self {
        RomLauncher { config_manager }
    }

    fn find_cores_for_extension(&self, config: &Config, extension: &str) -> Vec<Cores> {
        return config
            .cores
            .iter()
            .filter(|core| core.extension.eq_ignore_ascii_case(extension))
            .cloned()
            .collect();
    }

    fn build_launch_command(
        &self,
        retroarch_exe: &Path,
        core_path: &Path,
        rom_path: &Path,
        fullscreen: bool,
    ) -> Command {
        let mut cmd = Command::new(retroarch_exe);

        if fullscreen {
            cmd.arg("-f");
        }

        cmd.arg("-L").arg(core_path).arg(rom_path);

        return cmd;
    }

    pub fn launch(&self, rom_path: &Path) {
        if !rom_path.exists() {
            return logger::log_error(format!("ROM not found: {}", rom_path.display()));
        }

        let Some(extension) = rom_path.extension().and_then(|s| s.to_str()) else {
            return logger::log_error(format!(
                "File has no valid extension: {}",
                rom_path.display()
            ));
        };

        logger::log_info(format!("Loading ROM: {}", rom_path.display()));

        let config = match self.config_manager.load() {
            Ok(config) => config,
            Err(e) => {
                return logger::log_error(format!("Failed to load config: {}", e));
            }
        };

        let retroarch_root = Path::new(&config.retroarch_install_path);
        let retroarch_exe = retroarch_root.join("retroarch.exe");

        if !retroarch_exe.exists() {
            return logger::log_error(format!(
                "RetroArch executable not found: {}",
                retroarch_exe.display()
            ));
        }

        let cores = self.find_cores_for_extension(&config, extension);

        let Some(core) = (match cores.len() {
            0 => {
                logger::log_error(format!("No core found for extension: {}", extension));
                None
            }
            1 => cores.first(),
            _ => {
                logger::log_error(format!("Multiple cores found for extension: {}", extension));
                None
            }
        }) else {
            return;
        };

        let core_path = retroarch_root.join("cores").join(&core.core);
        if !core_path.exists() {
            return logger::log_error(format!("Core file not found: {}", core_path.display()));
        }

        logger::log_info(format!("Using core: {}", core.core));

        let mut cmd = self.build_launch_command(
            &retroarch_exe,
            &core_path,
            rom_path,
            config.launch_fullscreen,
        );

        logger::log_info(format!("Launching RetroArch..."));

        match cmd.spawn() {
            Ok(_child) => {
                logger::log_info("RetroArch spawned successfully.".to_string());
            }
            Err(e) => {
                logger::log_error(format!("Failed to spawn RetroArch process: {}", e));
            }
        }
    }
}
