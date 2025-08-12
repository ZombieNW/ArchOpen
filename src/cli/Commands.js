const clc = require('cli-color');
const packageInfo = require('../../package.json');
const log = require('../utils/logger');

class Commands {
	constructor(configManager, romLauncher) {
		this.configManager = configManager;
		this.romLauncher = romLauncher;
	}

	showHelp() {
		console.log(clc.underline.bold(`ArchOpen v${packageInfo.version} - By ZombieNW`));
		console.log('');
		console.log(clc.bold('Usage:'));
		console.log('  archopen.exe [rompath]              Launch a ROM file');
		console.log('  archopen.exe --help, -h            Show this help');
		console.log('  archopen.exe --generate-config, -gc Generate example config');
		console.log('  archopen.exe --list-cores, -lc     List configured cores');
		console.log('  archopen.exe --verify, -v          Verify installation');
		console.log('');
		console.log(clc.bold('Examples:'));
		console.log('  archopen.exe "C:\\Roms\\game.smc"');
		console.log('  archopen.exe --generate-config');
	}

	listCores() {
		const config = this.configManager.load();
		console.log(clc.bold('Configured Cores:'));
		console.log('');

		const grouped = {};
		config.cores.forEach((core) => {
			if (!grouped[core.core]) {
				grouped[core.core] = [];
			}
			grouped[core.core].push(core);
		});

		Object.entries(grouped).forEach(([coreName, extensions]) => {
			console.log(clc.blue.bold(coreName) + ':');
			extensions.forEach((extension) => {
				const desc = extension.description || 'No description';
				const priority = extension.priority ? ` (priority: ${extension.priority})` : '';
				console.log(`  - ${clc.green(extension.extension)} - ${desc}${priority}`);
			});
			console.log('');
		});
	}

	generateConfig(force = false) {
		try {
			this.configManager.generate(force);
		} catch (error) {
			log.error('Uh Oh, ' + error.message);
		}
	}

	verify() {
		return this.romLauncher.verify();
	}

	async launchRom(romPath) {
		try {
			await this.romLauncher.launch(romPath);
		} catch (error) {
			log.error(error.message);
			throw error;
		}
	}
}

module.exports = Commands;
