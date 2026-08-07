use std::env;

mod commands;
mod logger;
mod utils;

const VERSION: &str = "0.9.0";

const HELP_COMMANDS: &[&str] = &["help", "--help", "-h"];
const VERSION_COMMANDS: &[&str] = &["--version", "-v"];
const GENERATE_CONFIG_COMMANDS: &[&str] = &["--generate-config", "-g"];
const LIST_CORES_COMMANDS: &[&str] = &["--cores", "--list-cores", "-c", "-lc", "-l"];
const VERIFY_CONFIG_COMMANDS: &[&str] = &["--verify", "-vr"];

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        return commands::help_command();
    }
    let command = &args[1];

    if HELP_COMMANDS.contains(&command.as_str()) {
        return commands::help_command();
    } else if VERSION_COMMANDS.contains(&command.as_str()) {
        return commands::version_command();
    } else if GENERATE_CONFIG_COMMANDS.contains(&command.as_str()) {
        return commands::generate_config_command();
    } else if LIST_CORES_COMMANDS.contains(&command.as_str()) {
        return commands::list_cores_command();
    } else if VERIFY_CONFIG_COMMANDS.contains(&command.as_str()) {
        return commands::verify_config_command();
    }

    if utils::is_file(command) {
        return commands::launch_rom();
    }

    return logger::log_error(format!("Unknown Command: {}", command));
}
