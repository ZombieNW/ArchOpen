# ArchOpen

_An easy way to open games with RetroArch_

![Languages](https://badgen.net/badge/language/Rust/red) ![Platform](https://badgen.net/badge/platform/Windows/blue) ![License](https://badgen.net/badge/license/MIT/red)

## What?

ArchOpen is a tool for opening ROM's directly from Windows File Explorer by mapping file extensions to their corresponding emulator cores.

## Usage

Grab the [latest release](https://github.com/ZombieNW/ArchOpen/releases)

Generate a config file

```sh
archopen.exe --generate-config
```

Customize `config.yml` so each extension has a `core_name.dll` matching an installed RetroArch core.

Within file explorer, right click your rom, "Open With", "Choose an App on your PC", and select `archopen.exe`

Enjoy!

## _The Third Complete Rewrite_

First written in [Node](https://nodejs.org/en), then [C++](https://en.wikipedia.org/wiki/C%2B%2B), and now [Rust](https://rust-lang.org/)!

Rust is a language I've wanted to learn for some time, so I figured a good way to get my hands dirty with it would be to rewrite a codebase I was already disillusioned with.

I am pretty happy with this rewrite so far, it's much more maintainable and I can see the project sticking with it for some time.

### Breaking Changes

This version currently does not have a config migrator. I do not plan to add migration for `< v0.9` versions to avoid bloat (would require a JSON parser). Config migration will be added if/when the new yaml system needs it.
