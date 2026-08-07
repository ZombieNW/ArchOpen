use std::{env, fs, path::PathBuf};

use noyalib::{from_str, to_string};

use crate::{
    logger,
    utils::{self, get_timestamp},
};

#[derive(serde::Serialize, serde::Deserialize)]
pub struct Config {
    pub version: String,
    pub retroarch_install_path: String,
    pub launch_fullscreen: bool,
    pub cores: Vec<Cores>,
}

#[derive(serde::Serialize, serde::Deserialize, Clone)]
pub struct Cores {
    pub extension: String,
    pub core: String,
}

pub struct ConfigManager {
    config_path: PathBuf,
}

impl ConfigManager {
    pub fn new() -> Self {
        let exe_path = env::current_exe().expect("Failed to get executable path");
        let config_path = PathBuf::from(exe_path)
            .parent()
            .unwrap()
            .join("config.yaml");

        ConfigManager { config_path }
    }

    /// Ensures config file exists
    fn exists(&self) -> bool {
        return self.config_path.exists();
    }

    /// Loads config file
    pub fn load(&self) -> Result<Config, Box<dyn std::error::Error>> {
        if !self.exists() {
            return Err("config.yaml doesn't exist".into());
        }

        let config_content = fs::read_to_string(&self.config_path)?;
        let config: Config = from_str(&config_content)?;

        // TODO Migration

        return Ok(config);
    }

    /// Generates config from default
    pub fn generate(&self, force: bool) {
        // Backup if it exists
        if self.exists() && !force {
            logger::log_error(
                "config.yaml already exists! (Use --force to overwrite it.)".to_string(),
            );
            return;
        }
        if self.exists() && force {
            let config = match self.load() {
                Ok(config) => config,
                Err(e) => {
                    return logger::log_error(format!("Failed to load config: {}.", e));
                }
            };

            self.backup(&config);
        }

        // Save default to config.yaml
        self.save(&Config::default());
    }

    /// Saves config to file
    fn save(&self, config: &Config) {
        let config_str = to_string(config).expect("Failed to serialize config");
        fs::write(&self.config_path, config_str).expect("Failed to write config file");
        logger::log_success(format!(
            "config.yaml written to: {}",
            self.config_path.display()
        ));
    }

    /// Backs up current config
    fn backup(&self, config: &Config) {
        let backup_path = self
            .config_path
            .with_added_extension(format!("{}.bak", get_timestamp()));

        let config_str = to_string(config).expect("Failed to serialize config");
        fs::write(&backup_path, config_str).expect("Failed to write backup config");

        logger::log_info(format!(
            "config.yaml backed up to: {}",
            backup_path.display()
        ));
    }
}

impl Config {
    pub fn default() -> Self {
        Config {
            version: super::VERSION.to_string(),
            retroarch_install_path: match utils::detect_retroarch() {
                Some(path) => path,
                None => "C:\\RetroArch-Win64".to_string(),
            },
            launch_fullscreen: false,
            cores: vec![
                Cores {
                    extension: "nes".to_string(),
                    core: "nestopia_libretro.dll".to_string(),
                },
                Cores {
                    extension: "sfc".to_string(),
                    core: "snes9x_libretro.dll".to_string(),
                },
                Cores {
                    extension: "smd".to_string(),
                    core: "genesis_plus_gx_libretro.dll".to_string(),
                },
                Cores {
                    extension: "gen".to_string(),
                    core: "genesis_plus_gx_libretro.dll".to_string(),
                },
                Cores {
                    extension: "z64".to_string(),
                    core: "mupen64plus_next_libretro.dll".to_string(),
                },
                Cores {
                    extension: "n64".to_string(),
                    core: "mupen64plus_next_libretro.dll".to_string(),
                },
                Cores {
                    extension: "v64".to_string(),
                    core: "mupen64plus_next_libretro.dll".to_string(),
                },
                Cores {
                    extension: "chd".to_string(),
                    core: "swanstation_libretro.dll".to_string(),
                },
                Cores {
                    extension: "cue".to_string(),
                    core: "swanstation_libretro.dll".to_string(),
                },
                Cores {
                    extension: "iso".to_string(),
                    core: "swanstation_libretro.dll".to_string(),
                },
                Cores {
                    extension: "gcm".to_string(),
                    core: "dolphin_libretro.dll".to_string(),
                },
                Cores {
                    extension: "wbfs".to_string(),
                    core: "dolphin_libretro.dll".to_string(),
                },
                Cores {
                    extension: "gba".to_string(),
                    core: "mgba_libretro.dll".to_string(),
                },
                Cores {
                    extension: "gb".to_string(),
                    core: "gambatte_libretro.dll".to_string(),
                },
                Cores {
                    extension: "gbc".to_string(),
                    core: "gambatte_libretro.dll".to_string(),
                },
            ],
        }
    }
}
