use chrono::{Datelike, Local, Timelike};
use std::{env, path::Path};

pub fn is_file(path_name: &String) -> bool {
    let path = Path::new(path_name);
    return path.is_file();
}

pub fn get_timestamp() -> String {
    let now = Local::now();
    format!(
        "{:04}-{:02}-{:02}-{:02}:{:02}:{:02}",
        now.year(),
        now.month(),
        now.day(),
        now.hour(),
        now.minute(),
        now.second()
    )
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
