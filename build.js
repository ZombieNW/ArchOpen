const exe = require('@angablue/exe');
const build = exe({
    entry: './index.js',
    out: './dist/ArchOpen.exe',
    pkg: ['-C', 'GZip'],
    version: '0.1.0',
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