use std::{env, path::Path};

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        return println!("Help Command");
    }
    let command = &args[1];

    let help_commands = ["help", "--help", "-h"];
    let version_commands = ["--version", "-v"];
    let generate_config_commands = ["--generate-config", "-gc"];
    let list_cores_commands = ["--list-cores", "-lc", "-l"];
    let verify_config_commands = ["--verify", "-vr"];
    let migrate_config_commands = ["--migrate", "-m"];

    if help_commands.contains(&command.as_str()) {
        println!("Help Command");
    } else if version_commands.contains(&command.as_str()) {
        println!("Version Command");
    } else if generate_config_commands.contains(&command.as_str()) {
        println!("Generate Config Command");
    } else if list_cores_commands.contains(&command.as_str()) {
        println!("List Cores Command");
    } else if verify_config_commands.contains(&command.as_str()) {
        println!("Verify Config Command");
    } else if migrate_config_commands.contains(&command.as_str()) {
        println!("Migrate Config Command");
    } else if is_file(command) {
        println!("Rom time!");
    } else {
        println!("Unknown Command");
    }
}

fn is_file(path_name: &String) -> bool {
    let path = Path::new(path_name);
    return path.is_file();
}
