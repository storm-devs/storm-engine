# Unused scripts functions finder

## Requirements

In order to run this script you need:

- NodeJs
- Installed dependencies
- Some scripts files from a storm game (TEHO, COAS, ...)

## Setup
First you need to install dependencies, type in the console
``npm i``

## How to
To run the script, just type in the console
``node index.js {filepath}`` 

E.g. ``node index.js "E:\SteamLibrary\steamapps\common\Sea Dogs To Each His Own\PROGRAM"``

Results will be saved in tool folder in 'result.txt'


## Issues

Functions that begins with 'lai_' will not be processed, because they can be called dynamically
```
//Процессируем соответствующий тип
string func = chr.chr_ai.type;
if(func != "")
{
    func = "LAi_type_" + func + "_CharacterUpdate";
    call func(chr, dltTime);
}
//Процессируем соответствующий шаблон
func = chr.chr_ai.tmpl;
if(func == "") return;
func = "LAi_tmpl_" + func + "_CharacterUpdate";
call func(chr, dltTime);
```
There might be problems with `call` and other tricky things, so check results before deleting

## Author

[Artess999](https://github.com/Artess999)
