const packageInfo = require('../../package.json');
const log = require('../utils/logger');

class ConfigMigrator {
    constructor() {
        this.migrations = new Map([
            ['0.5.0', this.migrateFrom050to060],
        ]);
    }

    needsMigration(config, targetVersion = packageInfo.version) {
        if (!config || !config.version) return true; // If you're that cooked you probably need a migration
        return this.compareVersions(config.version, targetVersion) < 0;
    }

    compareVersions(a, b) {
        const parseVersion = (v) => v.split('.').map(n => parseInt(n, 10));
        const versionA = parseVersion(a);
        const versionB = parseVersion(b);

        for (let i = 0; i < Math.max(versionA.length, versionB.length); i++) {
            const numA = versionA[i] || 0;
            const numB = versionB[i] || 0;

            if (numA < numB) return -1;
            if (numA > numB) return 1;
        }
        return 0;
    }

    migrate(oldConfig, targetVersion = packageInfo.version) {
        const oldVersion = oldConfig.version || "0.5.0";
        log.info(`Migrating config from v${oldVersion} to v${targetVersion}...`);

        let migratedConfig = { ...oldConfig, version: targetVersion };
        const sortedVersions = Array.from(this.migrations.keys()).sort(this.compareVersions);

        for (const version of sortedVersions) {
            if (this.compareVersions(oldVersion, version) <= 0 &&
                this.compareVersions(targetVersion, version) >= 0) {
                const migrationFunction = this.migrations.get(version);
                if (migrationFunction) {
                    log.info(`Applying migration for v${version}...`);
                    migratedConfig = migrationFunction.call(this, migratedConfig);
                }
            }
        }

        return migratedConfig;
    }

    migrateFrom050to060(oldConfig) {
        const migrated = {
            version: packageInfo.version,
            retroarch_install_path: oldConfig.retroarch_install_path || 'C:\\RetroArch-Win64',
            auto_detect_retroarch: true,
            launch_fullscreen: false,
            cores: []
        }

        if (oldConfig.cores && Array.isArray(oldConfig.cores)) {
            migrated.cores = oldConfig.cores.map(core => ({
                ...core
            }));
        }

        return migrated;
    }
}

module.exports = ConfigMigrator;