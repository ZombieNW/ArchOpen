const fs = require('fs');
const path = require('path');
const packageInfo = require('../../package.json');
const ConfigMigrator = require('./ConfigMigrator');
const log = require('../utils/logger');

class ConfigManager {
	constructor() {
		this.configPath = path.join(path.dirname(process.execPath), 'config.json');
		this.migrator = new ConfigMigrator();
		this.defaultConfig = {
			version: packageInfo.version,
			retroarch_install_path: 'C:\\RetroArch-Win64',
			auto_detect_retroarch: true,
			launch_fullscreen: false,
			backup_configs: true,
			cores: [
				{ extension: 'nes', core: 'nestopia_libretro.dll', description: 'Nintendo Entertainment System' },
				{ extension: 'sfc', core: 'snes9x_libretro.dll', description: 'Super Nintendo' },
				{ extension: 'snes', core: 'snes9x_libretro.dll', description: 'Super Nintendo' },
				{ extension: 'smd', core: 'genesis_plus_gx_libretro.dll', description: 'Sega Genesis/Mega Drive' },
				{ extension: 'gen', core: 'genesis_plus_gx_libretro.dll', description: 'Sega Genesis' },
				{ extension: 'z64', core: 'mupen64plus_next_libretro.dll', description: 'Nintendo 64 (big endian)' },
				{ extension: 'n64', core: 'mupen64plus_next_libretro.dll', description: 'Nintendo 64' },
				{ extension: 'v64', core: 'mupen64plus_next_libretro.dll', description: 'Nintendo 64 (byte-swapped)' },
				{ extension: 'chd', core: 'swanstation_libretro.dll', description: 'PlayStation (CHD)' },
				{ extension: 'cue', core: 'swanstation_libretro.dll', description: 'PlayStation (CUE/BIN)' },
				{ extension: 'iso', core: 'swanstation_libretro.dll', description: 'PlayStation (ISO)' },
				{ extension: 'gcm', core: 'dolphin_libretro.dll', description: 'GameCube' },
				{ extension: 'iso', core: 'dolphin_libretro.dll', description: 'GameCube/Wii (ISO)', priority: 2 },
				{ extension: 'wbfs', core: 'dolphin_libretro.dll', description: 'Wii (WBFS)' },
				{ extension: 'gba', core: 'mgba_libretro.dll', description: 'Game Boy Advance' },
				{ extension: 'gb', core: 'gambatte_libretro.dll', description: 'Game Boy' },
				{ extension: 'gbc', core: 'gambatte_libretro.dll', description: 'Game Boy Color' },
			],
		};
	}

	exists() {
		return fs.existsSync(this.configPath);
	}

	createBackup(config) {
		const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
		const backupPath = this.configPath + '.' + timestamp + '.bak';

		try {
			fs.writeFileSync(backupPath, JSON.stringify(config, null, 4));
			log.info(`Created backup ${path.basename(backupPath)}`);
			return backupPath;
		} catch (error) {
			log.error(`Uh Oh, Couldn't Create Backup: ${error.message}`);
			return null;
		}
	}

	generate(force = false) {
		if (this.exists() && !force) {
			throw new Error('config.json already exists ! (You can use --force to overwrite it.)');
		}

		// Backup old config if they force it whether they like it or not
		if (this.exists() && force) {
			try {
				const existingConfig = JSON.parse(fs.readFileSync(this.configPath, 'utf8'));
				this.createBackup(existingConfig);
			} catch (error) {
				log.error(`Uh Oh, Couldn't Backup: ${error.message}`);
			}
		}

		fs.writeFileSync(this.configPath, JSON.stringify(this.defaultConfig, null, 4));
		log.success('Yipee! config.json has been generated !');
		log.info('File Location: ' + this.configPath);
		log.info('You can now edit it to your liking.');
	}

	load() {
		try {
			if (!this.exists()) {
				throw new Error('config.json does not exist! You can use --generate-config to create one.');
			}

			const rawConfig = JSON.parse(fs.readFileSync(this.configPath, 'utf8'));

			if (this.migrator.needsMigration(rawConfig, packageInfo.version)) {
				log.info('Hmm, looks like you just updated. Migrating config to the latest version...');

				if (rawConfig?.backup_configs !== false) {
					this.createBackup(rawConfig);
				}

				const migratedConfig = this.migrator.migrate(rawConfig, packageInfo.version);

				fs.writeFileSync(this.configPath, JSON.stringify(migratedConfig, null, 4));
				log.success('Yipee! Config migration complete!');

				return migratedConfig;
			}

			this.validateConfig(rawConfig);
			return rawConfig;
		} catch (error) {
			if (error.code === 'ENOENT') {
				throw new Error('config.json not found ! Run with --generate-config to create one.');
			}
			if (error instanceof SyntaxError) {
				throw new Error(`Invalid JSON in config.json: ${error.message}`);
			}
			throw new Error(`Failed to load config: ${error.message}`);
		}
	}

	save(config) {
		fs.writeFileSync(this.configPath, JSON.stringify(config, null, 4));
		log.success('Yipee! config.json has been saved !');
		log.info('File Location: ' + this.configPath);
	}

	validateConfig(config) {
		const required = ['version', 'retroarch_install_path', 'cores'];
		const missing = required.filter((field) => !config[field]);

		if (missing.length > 0) {
			throw new Error(`Config missing required fields: ${missing.join(', ')}`);
		}

		if (!Array.isArray(config.cores)) {
			throw new Error('Config field "cores" must be an array');
		}

		config.cores.forEach((core, index) => {
			if (!core.extension || !core.core) {
				throw new Error(`Core at index ${index} missing required fields (extension, core)`);
			}
		});
	}

	autoDetectRetroArch() {
		const commonPaths = ['C:\\RetroArch-Win64', 'C:\\RetroArch', 'C:\\Program Files\\RetroArch', 'C:\\Program Files (x86)\\RetroArch', path.join(process.env.LOCALAPPDATA || '', 'RetroArch'), path.join(process.env.APPDATA || '', 'RetroArch')];

		for (const testPath of commonPaths) {
			const retroarchExe = path.join(testPath, 'retroarch.exe');
			if (fs.existsSync(retroarchExe)) {
				log.success(`RetroArch found at: ${testPath}`);
				return testPath;
			}
		}

		return null;
	}

	getVersion() {
		try {
			if (!this.exists()) return null;
			const config = JSON.parse(fs.readFileSync(this.configPath, 'utf8'));
			return config.version || '0.5.0'; // 0.5.0 was the first version published with a config so it's probably that version
		} catch (error) {
			return null;
		}
	}

	listBackups() {
		const configDir = path.dirname(this.configPath);
		const configName = path.basename(this.configPath, '.json');

		try {
			const files = fs.readdirSync(configDir);
			const backups = files
				.filter((file) => file.startsWith(`${configName}_backup_`) && file.endsWith('.json'))
				.map((file) => ({
					file: file,
					path: path.join(configDir, file),
					date: this.extractDateFromBackup(file),
				}))
				.sort((a, b) => b.date - a.date); // recent first

			return backups;
		} catch (error) {
			return [];
		}
	}

	extractDateFromBackup(filename) {
		const match = filename.match(/backup_(.+)\.json$/);
		if (match) {
			try {
				return new Date(match[1].replace(/-/g, ':'));
			} catch (error) {
				return new Date(0);
			}
		}
		return new Date(0);
	}
}

module.exports = ConfigManager;
