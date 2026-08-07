use stylic::Styleable;

use crate::commands::config_manager::ConfigManager;

#[path = "config_manager.rs"]
mod config_manager;

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

pub fn version_command() {
    println!("ArchOpen v{} - by ZombieNW", super::VERSION);
}

pub fn generate_config_command() {
    let config_manager = ConfigManager::new();
    config_manager.generate();
}

pub fn list_cores_command() {}

pub fn verify_config_command() {}

pub fn launch_rom() {}
