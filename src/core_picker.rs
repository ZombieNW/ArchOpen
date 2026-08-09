use crate::config_manager::Cores;
use dialoguer::Select;
use stylic::Styleable;

pub fn core_picker(cores: &[Cores], file_name: &str) -> Option<Cores> {
    let options: Vec<&str> = cores.iter().map(|core| core.core.as_str()).collect();

    let selection = Select::new()
        .with_prompt(format!(
            "Select a core for {}",
            file_name.styled().bold().underlined()
        ))
        .items(&options)
        .default(0)
        .interact_opt()
        .ok()??;

    return Some(cores[selection].clone());
}
