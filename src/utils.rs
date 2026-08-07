use chrono::{Datelike, Local, Timelike};
use std::path::Path;

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
