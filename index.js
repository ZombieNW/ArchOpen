//Libs
const fs = require('fs');
const path = require('path');
const { exec } = require("child_process");
var clc = require("cli-color");
var error = clc.red.bold;
var warn = clc.yellow;
var info = clc.blue;

//Vars/Config
var passedArguments = process.argv.slice(2);//Remove location arguments
if(!passedArguments[0] || passedArguments[0] == "help" || passedArguments[0] == "-h" || passedArguments[0] == "--help"){
    //Help
    console.log(clc.underline("ArchOpen - By ZombieNW"));
    console.log(clc.bold("Usage:"));
    console.log("archopen.exe [rompath]\n");
    console.log("Then make sure your config file if properly configured, to generate an example one, do archopen.exe --generate-config");
    require('child_process').spawnSync("pause", {shell: true, stdio: [0, 1, 2]});//Pause
    process.exit();
}
else if(passedArguments[0] == "--generate-config" || passedArguments[0] == "-gc"){
    if(fs.existsSync(path.dirname(process.execPath) + '\\config.json')){
        console.log(error("Error! ") + warn("config.json already exists!"));
        require('child_process').spawnSync("pause", {shell: true, stdio: [0, 1, 2]});//Pause
        process.exit();
    }
    else{
        console.log("Writing to config...");
        let configExample = `{
    "version": "0.1.0",
    "retroarch_install_path": "C:\\\\RetroArch-Win64",
    "cores":[
        {"extension": "nes", "core": "nestopia_libretro.dll"},
        {"extension": "sfc", "core": "snes9x_libretro.dll"},
        {"extension": "smd", "core": "genesis_plus_gx_libretro.dll"},
        {"extension": "z64", "core": "mupen64plus_next_libretro.dll"},
        {"extension": "chd", "core": "swanstation_libretro.dll"},
        {"extension": "gcm", "core": "dolphin_libretro.dll"}
    ]
}`;
        fs.writeFileSync(path.dirname(process.execPath) + '\\config.json', configExample, (err) => {
            if (err) console.log(err);
        });
        console.log("Config Successfully Generated!");

        require('child_process').spawnSync("pause", {shell: true, stdio: [0, 1, 2]});//Pause
        process.exit();
    }
}
else{
    //Execute
    console.log("Generating Arguments...");
    var romPath = passedArguments.join(" ");//Get passed in rom path
    var romFileExtension = romPath.split('.').pop();//Get file extension
    console.log("Rom Type: " + romFileExtension + "...");
    console.log("Finding Config...");
    try{
        var config = JSON.parse(fs.readFileSync(path.dirname(process.execPath) + '\\config.json'));//Get config
        console.log("Config Found...");
    }
    catch{
        console.log(error("Error! ") + warn("config.json not found!"));
        console.log(info("For additional help, do archopen.exe --help"));
        console.log(info("Or to generate a config, run archopen.exe --generate-config"));
        require('child_process').spawnSync("pause", {shell: true, stdio: [0, 1, 2]});//Pause
        process.exit();
    }
    console.log("Finding Extension Core Settings...");
    try{
        var coreConfig = config.cores.find(({extension}) => extension === romFileExtension);//Find config settings for the core cooresponding to the file extension
        var retroarchPath = `${config.retroarch_install_path}\\retroarch.exe`;//Get executable
        var corePath = `${config.retroarch_install_path}\\cores\\${coreConfig.core}`;//Get core dll
    }
    catch{
        console.log(error("Error! ") + warn("File extension core setting or file not found!"));
        console.log(info("For additional help, do archopen.exe --help"));
        console.log(info("Or to generate a config, run archopen.exe --generate-config"));
        require('child_process').spawnSync("pause", {shell: true, stdio: [0, 1, 2]});//Pause
        process.exit();
    }
    console.log(`Executing '"${retroarchPath}" -L "${corePath}" "${romPath}"'...`);
    exec(`"${retroarchPath}" -L "${corePath}" "${romPath}"`);//100% slice proof
    process.abort(0);
}
process.exit();