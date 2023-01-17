# ArchOpen
### _An easy way to open games with RetroArch_
![Languages](https://badgen.net/badge/language/Node.JS/green) ![Platform](https://badgen.net/badge/language/Windows/blue) ![License](https://badgen.net/badge/language/MIT/red)

![Usage Gif](https://zombienw.com/assets/archopen/gif.gif)

## What?
I have a nice rom folder, and sometimes I just wanna be able to double click a file and it opens, but Retroarch doesn't natively support that. So I want to make a user friendly way to be able to open games. For right now the goal is just file extension association, but maybe down the line I'll add a UI similar to OpenEmu, as personally, I think many of the emulator frontends of today are too complicated.

## Building
Install [Node.JS](https://nodejs.org/en/) and then run the following commands.
```sh
npm run setup
npm run build
npm run test
```

## Usage
Grab the [latest release](https://github.com/ZombieNW/ArchOpen/releases)
You can generate an example config.json by running
```sh
archopen.exe --generate-config
```
or copying the [example config](https://github.com/ZombieNW/ArchOpen/blob/main/example_config.json) into config.json in the same directory as the executable.
Customize your config so each extension corresponds with the name of the core dll in the cores directory of your RetroArch install.
Once your config is set up just set the executable to always open a file type in Windows Explorer.
(And I mean make sure you have RetroArch and the respective cores specified in the config installed)

## Checklist

- [x] Prototype CLI
- [ ] Configuration Tool
- [ ] UI?

## License
[MIT](https://choosealicense.com/licenses/mit/)
