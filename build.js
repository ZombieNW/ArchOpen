const exe = require('@angablue/exe');
const packageInfo = require('./package.json');
const build = exe({
    entry: './src/ArchOpen.js',
    out: './dist/ArchOpen.exe',
    pkg: ['-C', 'GZip'],
    version: packageInfo.version,
    target: 'node16-win-x64',
    icon: './assets/icon.ico',
    properties: {
        FileDescription: 'ArchOpen',
        ProductName: 'ArchOpen',
        LegalCopyright: 'ZombieNW',
        OriginalFilename: 'ArchOpen.exe'
    }
});
build.then(() => console.log('Build done'));