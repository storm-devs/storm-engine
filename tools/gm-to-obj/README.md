# .GM to .OBJ file converter

## Requirements

In order to run this script you need:

- Python 3
- Some .gm files from a storm game (TEHO, COAS, ...)

## How to

To run the script, just type in the console
``python .\gm-to-obj.py {filepath}`` 

E.g. ``python .\gm-to-obj.py "E:\SteamLibrary\steamapps\common\Sea Dogs To Each His Own\RESOURCE\MODELS\fish01.gm" -o myCoolFish.obj``

Option | Short | Description | Default
------ | ----- | ----------- | -------
``--output``|``-o``| The name of the ``.obj`` file to create| [fileName] with ``.obj`` extension
``--script-folder``|``-sf``|Put the output file in the same folder as the script|False

## Author

[Andrea Mangione]( https://github.com/MangioneAndrea)
