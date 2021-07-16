# Blender AN import

[Русская версия Readme](https://github.com/storm-devs/storm-engine/blob/develop/tools/blender-an-import/README_RUS.md)

## Requirements

In order to use this plugin you need:

- [NodeJs](https://nodejs.org/en/)
- animation in .an format

## How to
For correct plugin work animation file must be prepared (parsed into json).
In order to do this you need to launch console (terminal) in the folder with ``parse-an.js`` script and run
``node parse-an.js {filepath}``

E.g.:
``node parse-an.js "C:\animations\man.an"``

The results (``man.an.json``) will be saved in the folder with .an animation file

### Remark about Windows
There is an easier way for Windows users -
you can drag file with animation to the file ``parse-an.cmd`` and results will be saved in animation folder

## Blender Plugin Installation
```
Edit -> Preferences... -> Add-ons -> Install... -> Select import_json_an.py from io_import_json_an folder -> Enable "Import: JSON AN"
```

## Blender Usage
```
File -> Import... -> JSON AN Import(.an.json)
```

## Issues
Because of difference in rigging between Maya (original editor) and Blender, animation works correctly only if bone is very small (tends to small point)

## Author

[Artess999](https://github.com/Artess999)

