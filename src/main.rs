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
        commands::help_command();
    } else if VERSION_COMMANDS.contains(&command.as_str()) {
        commands::version_command();
    } else if GENERATE_CONFIG_COMMANDS.contains(&command.as_str()) {
        let force = args.get(2).map(|s| s.as_str()) == Some("--force");
        commands::generate_config_command(force);
    } else if LIST_CORES_COMMANDS.contains(&command.as_str()) {
        commands::list_cores_command();
    } else if VERIFY_CONFIG_COMMANDS.contains(&command.as_str()) {
        commands::verify_config_command();
    } else if utils::is_file(command) {
        commands::launch_rom(command);
    } else {
        logger::log_error(format!("Unknown Command: {}", command));
    }
}
