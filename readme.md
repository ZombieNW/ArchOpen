# ArchOpen

### _An easy way to open games with RetroArch_

![Languages](https://badgen.net/badge/language/Node.JS/green) ![Platform](https://badgen.net/badge/platform/Windows/blue) ![License](https://badgen.net/badge/license/MIT/red)

![Usage Gif](https://zombienw.com/assets/archopen/gif.gif)

## What?

Sometimes I just want to double click a rom file and it just opens with the proper core in Retroarch.
If you share this desire, welcome!

## Building

Install [Node.JS](https://nodejs.org/en/) and then run the following commands.

```sh
npm run setup
npm run build
```

## Usage

Grab the [latest release](https://github.com/ZombieNW/ArchOpen/releases)

Generate an example config.json

```sh
archopen.exe --generate-config
```

(or copy the [example config](https://github.com/ZombieNW/ArchOpen/blob/main/example_config.json) as config.json in the executable directory)

Customize config.json so each extension has a core_name.dll matching a core in the cores directory of your RetroArch install.

Right click rom file, "Open With", "Choose an App on your PC", and select ArchOpen.exe

Enjoy !

## Checklist

- [x] Prototype CLI
- [ ] Configuration Tool
- [ ] UI?

## License

[MIT](https://choosealicense.com/licenses/mit/)
