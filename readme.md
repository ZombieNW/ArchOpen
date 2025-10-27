# ArchOpen - Next

### _An easy way to open games with RetroArch_

![Languages](https://badgen.net/badge/language/C++/blue) ![Platform](https://badgen.net/badge/platform/Windows/blue) ![License](https://badgen.net/badge/license/MIT/red)

![Usage Gif](https://zombienw.com/assets/archopen/gif.gif)

## Next?
This is a complete rewrite of ArchOpen! I took a hard look at the source code and realized writing the whole thing in Node.JS was not a great choice. So this is a ground up rewrite in C++. Binaries went from 60MB -> ~0.5MB; so this is MUCH more lightweight. Even though the version number is higher, more bugs may exist, so make issues and pull requests !

## What?

Sometimes I just want to double click a rom file and it just opens with the proper core in Retroarch.
If you share this desire, welcome!

## Building

This project uses g++ and a makefile.

```sh
make
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

## License

[MIT](https://choosealicense.com/licenses/mit/)
