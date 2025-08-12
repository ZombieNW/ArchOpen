const clc = require('cli-color');

const log = {
	error: (msg) => console.log(clc.red.bold('Error: ') + msg),
	warn: (msg) => console.log(clc.yellow('Warn: ') + msg),
	info: (msg) => console.log(clc.blue('Info: ') + msg),
	success: (msg) => console.log(clc.green('Success: ') + msg),
	debug: (msg) => process.env.DEBUG && console.log(clc.magenta('Debug: ') + msg),
};

module.exports = log;
