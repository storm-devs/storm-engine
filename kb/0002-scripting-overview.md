# Scripting System Overview
_[back to Index](../index.md)_

![Storm Engine Logo](../media/SE_logo.png)

**Related articles**: TBD

Upon startup, the game (`ENGINE.exe`) will attempt to load and compile the game logic located in `program` directory. These "scripts" are written in C using the game's API, and compiled at runtime using the built-in compiler. 

## `Program/` File Structure

Each folder inside `program` is roughly responsible for its own area of interest as indicated below. In practice, they also serve as databases for related areas of interest: thus, `characters` will contain all the information and logic related to characters, `items` will have all the stats on items, etc. 

Please note that these do not include any other media than text: all the animations, models, textures, sounds, UI layouts, sounds and videos are located in the `resource` directory. 

The following list is sorted alphabetically and is based on the latest build of _Sea Dogs: To Each His Own_. Because the contents largely define each game, they differ from one title to another.

* `program\` - shared and startup code.
* `program\battle_interface\` - UI logic on land and sea, quick travel.
* `program\BIManager\` - Battle Interface manager.
* `program\cannons\` - cannon generation and game logic.
* `program\characters\` - NPC generation and game logic: faces, names, spawn locations, etc.
* `program\colonies\` - definition of each colony default values.
* `program\controls\` - keybindings and related functionality.
* `program\dialogs\` - quests and conversation logic.
* `program\encounters\` - events on the land and sea.
* `program\interface\` - generic UI logic.
* `program\islands\` - island and city generation: trade/black market data, fort, various locations, etc.
* `program\items\` - everything item-related: types, descriptions, usage...
* `program\loc_ai\` - NPC AI behavior in any situation.
* `program\locations\` - location maps, loading NPC, etc.
* `program\nations\` - nations generation and definitions (relationships et al.)
* `program\net\` - multiplayer module (currently inactive)
* `program\questBook\` - all the journal entries.
* `program\quests\` - quest event handlers, various related modules.
* `program\scripts\` misc. game logic (rumours, sieges, duels, bounty hunters, etc.)
* `program\sea_ai\` - Player and AI behaviour on sea.
* `program\ships\` - ships generation and definitions.
* `program\store\` - merchants logic and database (items, prices, etc.)
* `program\weather\` - weather system definitions and usage.
* `program\worldMap\` - everything world map related.



---

| [Home](../index.md) | [Site Map](../site-map.md) | 