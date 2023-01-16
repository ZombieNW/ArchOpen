# ArchOpen
### _An easy way to open games with RetroArch_
![Languages](https://badgen.net/badge/language/Node.JS/green) ![Platform](https://badgen.net/badge/language/Windows/blue) ![License](https://badgen.net/badge/language/MIT/red)

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
Just go over to the releases tab and grab the latest build.
Extract it and then customize your config.json, an example one should be included with release 1.0.0

## Checklist

- [ ] Prototype CLI
- [ ] Configuration Tool
- [ ] UI?

## License
[MIT](https://choosealicense.com/licenses/mit/)