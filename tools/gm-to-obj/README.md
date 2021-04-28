# .GM to .OBJ file converter

## Requirements

In order to run this script you need:

- Python 3
- Some .gm files from a storm game (TEHO, COAS, ...)

## How to

### Convert 1 file

To run the script, just type in the console
``python .\gm_to_obj_converter.py {filepath}`` 

E.g. ``python .\gm_to_obj_converter.py "E:\SteamLibrary\steamapps\common\Sea Dogs To Each His Own\RESOURCE\MODELS\fish01.gm" -o myCoolFish.obj``

Option | Short | Description | Default
------ | ----- | ----------- | -------
``--output``|``-o``| The name of the ``.obj`` file to create| [fileName] with ``.obj`` extension
``--script-folder``|``-sf``|Put the output file in the same folder as the script|False
``--quiet``|``-q``|Stop printing status|False

### Convert all files

This script will convert all files in one folder and subfolders. It will also print a list of failed items in a .txt file

Run ``python .\convert_all.py {folder}``

E.g. ``python .\convert_all.py "E:\SteamLibrary\steamapps\common\Sea Dogs To Each His Own\RESOURCE\MODELS\Ammo"``

## Issues

The following files will not convert, as they cannot be representend as .obj:

TEHO

- `big_sky.gm`
- `light.gm`
- `sky.gm`
- `small_sky.gm`
- `sphere.gm`

## Author

[Andrea Mangione]( https://github.com/MangioneAndrea)
