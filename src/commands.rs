use std::{collections::HashMap, path::Path};

use stylic::Styleable;

use crate::{commands::config_manager::ConfigManager, logger};

#[path = "config_manager.rs"]
mod config_manager;

/// Displays version number and cli commands
pub fn help_command() {
    println!(
        "{}",
        format!("ArchOpen v{} - by ZombieNW", super::VERSION)
            .styled()
            .underlined()
    );

    println!(
        r#"
Usage:
    archopen.exe [rompath]                  Launch a ROM file
    archopen.exe --help, -h                 Show this message
    archopen.exe --generate-config, -g      Generate example config
    archopen.exe --cores, -c                List configured cores
    archopen.exe --verify, -vr              Verify configured cores
    archopen.exe --version, -v              See current version

Examples:
    archopen.exe -g
    archopen.exe "C:\roms\game.smc"
"#
    );
}

/// Displays the version
pub fn version_command() {
    println!("ArchOpen v{} - by ZombieNW", super::VERSION);
}

/// Writes the default config file to config.yml
pub fn generate_config_command(force: bool) {
    let config_manager = ConfigManager::new();
    config_manager.generate(force);
}

/// Lists all extensions sorted by core
pub fn list_cores_command() {
    let config_manager = ConfigManager::new();
    let config = match config_manager.load() {
        Ok(config) => config,
        Err(e) => {
            return logger::log_error(format!("Config not found: {}.", e));
        }
    };

    if config.cores.len() < 1 {
        return logger::log_error("No cores configured.".to_string());
    }

    let mut core_extensions: HashMap<String, Vec<String>> = HashMap::new();
    for core in &config.cores {
        core_extensions
            .entry(core.core.clone())
            .or_default()
            .push(core.extension.clone());
    }

    println!("Configured Cores:");
    for (core_name, extensions) in &core_extensions {
        println!("  {core_name}:");
        for extension in extensions {
            println!("      .{extension}");
        }
    }
}

/// Verify the existence of all cores and executables referenced in the config
pub fn verify_config_command() {
    let config_manager = ConfigManager::new();
    let config = match config_manager.load() {
        Ok(config) => config,
        Err(e) => {
            return logger::log_error(format!("Config not found: {}.", e));
        }
    };

    logger::log_info("Verifying config...".to_string());

    // RetroArch Executable
    let retroarch_executable = Path::new(&config.retroarch_install_path).join("retroarch.exe");
    if retroarch_executable.exists() {
        logger::log_success(format!(
            "RetroArch executable found at: {}",
            retroarch_executable.display()
        ));
    } else {
        logger::log_error(format!(
            "RetroArch executable not found, expected: {}",
            retroarch_executable.display()
        ));
    }

    // Cores
    let mut core_list: Vec<String> = Vec::new();
    for core in &config.cores {
        if !core_list.contains(&core.core) {
            core_list.push(core.core.clone());
        }
    }
    for core in &core_list {
        let core_path = Path::new(&config.retroarch_install_path)
            .join("cores")
            .join(core);
        if core_path.exists() {
            logger::log_success(format!("Core {core} found at: {}", core_path.display()));
        } else {
            logger::log_error(format!(
                "Core {core} not found, expected: {}",
                core_path.display()
            ));
        }
    }

    logger::log_info("Verification complete!".to_string());
}

pub fn launch_rom() {}
