const ConfigManager = require('./config/ConfigManager');
const RomLauncher = require('./core/RomLauncher');
const Commands = require('./cli/Commands');
const log = require('./utils/logger');

class ArchOpen {
    constructor() {
        this.configManager = new ConfigManager();
        this.romLauncher = new RomLauncher(this.configManager);
        this.commands = new Commands(this.configManager, this.romLauncher);
        this.args = process.argv.slice(2);
    }

    pauseAndExit(code = 0) {
        if (process.platform === 'win32') {
            require('child_process').spawnSync('pause', { shell: true, stdio: 'inherit' });
        }
        process.exit(code);
    }

    async run() {
        try {
            const command = this.args[0];

            if (!command || ['help', '-h', '--help'].includes(command)) {
                this.commands.showHelp();
                this.pauseAndExit();
            }

            if (['--generate-config', '-gc'].includes(command)) {
                const force = this.args.includes('--force');
                this.commands.generateConfig(force);
                this.pauseAndExit();
            }

            if (['--list-cores', '-lc'].includes(command)) {
                try {
                    this.commands.listCores();
                } catch (error) {
                    log.error(error.message);
                }
                this.pauseAndExit();
            }

            if (['--verify', '-v'].includes(command)) {
                const success = this.commands.verify();
                this.pauseAndExit(success ? 0 : 1);
            }

            // If we got to this point, that probably means the user wants to launch a rom
            const romPath = this.args.join(' ');
            await this.commands.launchRom(romPath);
        } catch (error) {
            log.error(`Uh Oh, Main Process Error: ${error.message}`);
            this.pauseAndExit(1);
        }
    }
}

if (require.main === module) {
    const app = new ArchOpen();
    app.run();
}

module.exports = ArchOpen;