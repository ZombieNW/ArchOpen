use std::{
    env, fs,
    path::{Path, PathBuf},
};

use chrono::Local;
use toml::{from_str, to_string};

use crate::logger;

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
        let config_path = env::current_exe()
            .ok()
            .and_then(|p| p.parent().map(|p| p.to_path_buf()))
            .unwrap_or_else(|| PathBuf::from("."))
            .join("config.toml");

        ConfigManager { config_path }
    }

    /// Ensures config file exists
    fn exists(&self) -> bool {
        return self.config_path.exists();
    }

    /// Loads config file
    pub fn load(&self) -> Result<Config, Box<dyn std::error::Error>> {
        if !self.exists() {
            return Err("config.toml doesn't exist".into());
        }

        let config_content = fs::read_to_string(&self.config_path)?;
        let config: Config = from_str(&config_content)?;

        // Migration goes here

        return Ok(config);
    }

    /// Generates config from default
    pub fn generate(&self, force: bool) {
        // Backup if it exists
        if self.exists() && !force {
            logger::log_error(
                "config.toml already exists! (Use --force to overwrite it.)".to_string(),
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

        // Save default to config.toml
        self.save(&Config::default());
    }

    /// Saves config to file
    fn save(&self, config: &Config) {
        let config_str = to_string(config).expect("Failed to serialize config");
        fs::write(&self.config_path, config_str).expect("Failed to write config file");
        logger::log_success(format!(
            "config.toml written to: {}",
            self.config_path.display()
        ));
    }

    /// Backs up current config
    fn backup(&self, config: &Config) {
        let timestamp = Local::now().format("%Y-%m-%d-%H-%M-%S").to_string();
        let backup_path = self
            .config_path
            .with_added_extension(format!("{}.bak", timestamp));

        let config_str = to_string(config).expect("Failed to serialize config");
        fs::write(&backup_path, config_str).expect("Failed to write backup config");

        logger::log_info(format!(
            "config.toml backed up to: {}",
            backup_path.display()
        ));
    }
}

impl Default for Config {
    fn default() -> Self {
        Config {
            version: super::VERSION.to_string(),
            retroarch_install_path: match detect_retroarch() {
                Some(path) => path,
                None => "C:\\RetroArch-Win64".to_string(),
            },
            launch_fullscreen: false,
            cores: vec![
                // NES
                Cores {
                    extension: "nes".into(),
                    core: "nestopia_libretro.dll".into(),
                },
                Cores {
                    extension: "fds".into(),
                    core: "nestopia_libretro.dll".into(),
                },
                // SNES
                Cores {
                    extension: "sfc".into(),
                    core: "snes9x_libretro.dll".into(),
                },
                // Sega
                Cores {
                    extension: "smd".into(),
                    core: "genesis_plus_gx_libretro.dll".into(),
                },
                Cores {
                    extension: "gen".into(),
                    core: "genesis_plus_gx_libretro.dll".into(),
                },
                // N64
                Cores {
                    extension: "z64".into(),
                    core: "mupen64plus_next_libretro.dll".into(),
                },
                Cores {
                    extension: "n64".into(),
                    core: "mupen64plus_next_libretro.dll".into(),
                },
                Cores {
                    extension: "v64".into(),
                    core: "mupen64plus_next_libretro.dll".into(),
                },
                // PSX
                Cores {
                    extension: "chd".into(),
                    core: "swanstation_libretro.dll".into(),
                },
                Cores {
                    extension: "cue".into(),
                    core: "swanstation_libretro.dll".into(),
                },
                Cores {
                    extension: "iso".into(),
                    core: "swanstation_libretro.dll".into(),
                },
                // GC/Wii
                Cores {
                    extension: "gcm".into(),
                    core: "dolphin_libretro.dll".into(),
                },
                Cores {
                    extension: "wbfs".into(),
                    core: "dolphin_libretro.dll".into(),
                },
                Cores {
                    extension: "iso".into(),
                    core: "dolphin_libretro.dll".into(),
                },
                // GBA
                Cores {
                    extension: "gba".into(),
                    core: "mgba_libretro.dll".into(),
                },
                // GB
                Cores {
                    extension: "gb".into(),
                    core: "gambatte_libretro.dll".into(),
                },
                Cores {
                    extension: "gbc".into(),
                    core: "gambatte_libretro.dll".into(),
                },
            ],
        }
    }
}

pub fn detect_retroarch() -> Option<String> {
    let username = env::var("USERNAME").ok()?;
    let user_home = format!("C:\\Users\\{}", username);

    let candidates = [
        "C:\\Program Files\\RetroArch".to_string(),
        "C:\\Program Files (x86)\\RetroArch".to_string(),
        "C:\\RetroArch".to_string(),
        "C:\\RetroArch-Win64".to_string(),
        format!("{}\\AppData\\Local\\Programs\\RetroArch", user_home),
        format!("{}\\AppData\\Roaming\\RetroArch", user_home),
    ];

    for candidate in candidates {
        if Path::new(&candidate).exists() {
            return Some(candidate);
        }
    }
    return None;
}
