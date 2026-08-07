use stylic::Styleable;

pub fn log_info(message: String) {
    println!("{} {message}", "INFO:".styled().blue());
}

pub fn log_warning(message: String) {
    println!("{} {message}", "INFO:".styled().yellow());
}

pub fn log_error(message: String) {
    println!("{} {message}", "INFO:".styled().red());
}

pub fn log_debug(message: String) {
    println!("{} {message}", "INFO:".styled().magenta());
}

pub fn log_success(message: String) {
    println!("{} {message}", "INFO:".styled().green());
}
