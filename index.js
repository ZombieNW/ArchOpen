//Libs
const fs = require('fs');
const path = require('path');
const { exec } = require("child_process");

//Vars/Config
var passedArguments = process.argv.slice(2);//Remove location arguments
var romPath = passedArguments.join(" ");//Get passed in rom path
var romFileExtension = romPath.split('.').pop();//Get file extension
console.log(process.cwd() + '\\config.json');
var config = JSON.parse(fs.readFileSync(path.dirname(process.execPath) + '\\config.json'));//Get config
var coreConfig = config.cores.find(({extension}) => extension === romFileExtension);//Find config settings for the core cooresponding to the file extension
var retroarchPath = `${config.retroarch_install_path}\\retroarch.exe`;//Get executable
var corePath = `${config.retroarch_install_path}\\cores\\${coreConfig.core}`;//Get core dll

//Execute
console.log(`"${retroarchPath}" -L "${corePath}" "${romPath}"`);//Harmless debugging left in prod
exec(`"${retroarchPath}" -L "${corePath}" "${romPath}"`);//100% slice proof
process.abort(0);