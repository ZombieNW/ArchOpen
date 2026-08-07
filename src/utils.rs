use std::path::Path;

pub fn is_file(path_name: &String) -> bool {
    let path = Path::new(path_name);
    return path.is_file();
}
