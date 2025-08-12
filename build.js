const exe = require('@angablue/exe');
const packageInfo = require('./package.json');

const build = exe({
    entry: './src/ArchOpen.js',
    out: './dist/ArchOpen.exe',
    version: packageInfo.version,
    pkg: ['-C', 'GZip'],
    target: 'node18-win-x64',
    icon: './assets/icon.ico',
    executionLevel: "asInvoker",
    properties: {
        FileDescription: 'ArchOpen',
        ProductName: 'ArchOpen',
        LegalCopyright: 'ZombieNW',
        OriginalFilename: 'ArchOpen.exe'
    }
});

build.then(() => console.log('Build done'));