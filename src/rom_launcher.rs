use std::{path::Path, process::Command};

use crate::{
    commands::config_manager::{Config, ConfigManager, Cores},
    logger,
};

pub struct RomLauncher {
    config_manager: ConfigManager,
}

impl RomLauncher {
    pub fn new(config_manager: ConfigManager) -> Self {
        RomLauncher { config_manager }
    }

    fn find_core_for_extension(&self, config: &Config, extension: &str) -> Option<Cores> {
        return config
            .cores
            .iter()
            .find(|core| core.extension == extension)
            .cloned();
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

        let extension = rom_path
            .extension()
            .and_then(|s| s.to_str())
            .expect("No extension found on ROM path");

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

        let Some(core) = self.find_core_for_extension(&config, &extension) else {
            return logger::log_error(format!("No core found for extension: {}", extension));
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
