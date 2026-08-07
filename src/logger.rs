use stylic::Styleable;

pub fn log_info(message: String) {
    println!("{} {message}", "INFO:".styled().blue());
}

pub fn log_warning(message: String) {
    println!("{} {message}", "WARNING:".styled().yellow());
}

pub fn log_error(message: String) {
    println!("{} {message}", "ERROR:".styled().red());
}

pub fn log_debug(message: String) {
    println!("{} {message}", "DEBUG:".styled().magenta());
}

pub fn log_success(message: String) {
    println!("{} {message}", "SUCCESS:".styled().green());
}
