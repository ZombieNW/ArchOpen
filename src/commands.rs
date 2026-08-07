use stylic::Styleable;

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
    archopen.exe --generate-config, -gc     Generate example config
    archopen.exe --list-cores, -lc          List configured cores
    archopen.exe --verify, -v               Verify config
    archopen.exe --migrate, -m              Update config to latest version

Examples:
    archopen.exe -gc
    archopen.exe "C:\roms\game.smc"
"#
    );
}

pub fn version_command() {
    println!("ArchOpen v{} - by ZombieNW", super::VERSION);
}

pub fn generate_config_command() {}

pub fn list_cores_command() {}

pub fn verify_config_command() {}

pub fn migrate_config_command() {}

pub fn launch_rom() {}
