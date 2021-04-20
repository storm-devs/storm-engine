# [Entities]
_[back to Index](../index.md)_

![Storm Engine Logo](../media/SE_logo.png)

**Related articles**: [Scripting Functions](0003-scripting-functions.md)

Each object in the scripting language can be an instance of an entity (predefined during engine compilation), or class archetypes. 

In order to become one, an entity must be registered as such (via `CreateClass` or `CreateEntity` function) and added to the respective layers. 

### Layers

There're two base layer types: `EXECUTE` and `REALIZE`. 

Each frame, the game will go through all its layers and make `execute` and `realize` passes.

During the `execute` pass, game logic for the entity is updated: light flickers, seagulls react to world state, etc. During the `realize` pass, graphics are sent for rendering or sounds for reproduction.

Layer order is important as well as the entity priority inside a given layer. The smaller layer numbers and priority levels are executed first.

At the end of each frame, a cleanup is made, and entities marked for removal are deleted from each layer.

Some entity types have specialized execute and realize layers, while others (purely graphical effects like rain drops or blood splatters) only have realize layers.

Currently implemented layers (in order of priority): 

| Layer name            | Layer Id | Type    |
| ---                   | ---      | ---     |
| EXECUTE               |  0       | Execute |
| REALIZE               |  1       | Realize |
| SEA_EXECUTE           |  2       | Execute |
| SEA_REALIZE           |  3       | Realize |
| INTERFACE_EXECUTE     |  4       | Execute |
| INTERFACE_REALIZE     |  5       | Realize |
| FADER_EXECUTE         |  6       | Execute |
| FADER_REALIZE         |  7       | Realize |
| LIGHTER_EXECUTE       |  8       | Execute |
| LIGHTER_REALIZE       |  9       | Realize |
| VIDEO_EXECUTE         |  10      | Execute |
| VIDEO_REALIZE         |  11      | Realize |
| EDITOR_REALIZE        |  12      | Execute |
| INFO_REALIZE          |  13      | Realize |
| SOUND_DEBUG_REALIZE   |  14      | Realize |
| SEA_REFLECTION        |  15      | Realize |
| SEA_REFLECTION2       |  16      | Realize |
| SEA_SUNROAD           |  17      | Realize |
| SUN_TRACE             |  18      | Realize |
| SAILS_TRACE           |  19      | Realize |
| HULL_TRACE            |  20      | Realize |
| MAST_ISLAND_TRACE     |  21      | Realize |
| MAST_SHIP_TRACE       |  22      | Realize |
| SHIP_CANNON_TRACE     |  23      | Realize |
| FORT_CANNON_TRACE     |  24      | Realize |
| ISLAND_TRACE          |  25      | Realize |
| SHADOW                |  26      | Realize |
| BLOOD                 |  27      | Realize |
| RAIN_DROPS            |  28      | Realize |

## Entity Type Index

Depending on usage, each entity can be considered a class, a service or a script library.

These are the following: 

### Animals
* `ANIMALS`: Class

### Animation
* `AnimationServiceImp`: Service

### Ball Splash
* `BALLSPLASH`: Class

### Battle Interface
* `BATTLE_INTERFACE`: Class
* `ILogAndActions`: Class
* `IBoardingStatus`: Class
* `BATTLE_LAND_INTERFACE`: Class
* `ISPYGLASS`: Class
* `SHIPPOINTER`: Class
* `ActivePerkShower`: Class
* `BITimer`: Class
* `ItemEntity`: Class
* `WM_INTERFACE`: Class
* `BI_InterfaceManager`: Class

### Blade
* `BLADE`: Class

### Blot
* `Blots`: Class

### Collide
* `COLL`: Service

### Dialog
* `DIALOG`: Class

### Geometry
* `GEOMETRY`: Service

### Island
* `ISLAND`: Class
* `CoastFoam`: Class

### Lighter
* `Lighter`: Class

### Location
* `Location`: Class
* `NPCharacter`: Class
* `Player`: Class
* `LocationCamera`: Class
* `Fader`: Class
* `Grass`: Class
* `Lights`: Class
* `WideScreen`: Class
* `CharacterAnimationKipper`: Class
* `LocationEffects`: Class
* `CharactersGroups`: Class
* `LocEagle`: Class
* `Lizards`: Class
* `LocRats`: Class
* `LocCrabs`: Class
* `LocModelRealizer`: Class
* `Blood`: Class
* `ScriptLocationLibrary`: Script Library

### Locator
* `LOCATOR`: Class
* `BLAST`: Class

### Mast
* `MAST`: Class

### Model
* `MODELR`: Class

### Particles
* `ParticleService`: Service
* `PARTICLES`: Class

### PCS Controls
* `PCS_CONTROLS`: Service

### Renderer
* `DX9RENDER`: Service
* `DX9RENDER_SCRIPT_LIBRIARY`: Script Library

### Rigging
* `SAIL`: Class
* `FLAG`: Class
* `ROPE`: Class
* `VANT`: Class
* `VANTL`: Class
* `VANTZ`: Class
* `SCRIPT_RIGGING_FILES`: Script Library

### Sailors
* `Sailors`: Class
* `SailorsEditor`: Class

### Script_library
* `SCRIPT_LIBRIARY_TEST`: Script Library

### Sea
* `SEA`: Class

### Seacreatures
* `Sharks`: Class

### Seafoam
* `SEAFOAM`: Class

### Sea_ai
* `SEA_AI`: Class
* `AIFort`: Class
* `AIBalls`: Class
* `AISeaGoods`: Class
* `SeaLocatorShow`: Class

### Sea_cameras
* `SEA_CAMERAS`: Class
* `FREE_CAMERA`: Class
* `SHIP_CAMERA`: Class
* `DECK_CAMERA`: Class

### Sea_operator
* `SEA_OPERATOR`: Class

### Shadow
* `Shadow`: Class

### Ship
* `SHIP`: Class
* `ShipLights`: Class
* `ShipTracks`: Class

### Sink_effect
* `SINKEFFECT`: Class

### Sound
* `SOUND`: Class

### Soundservice
* `SoundService`: Service
* `SoundVisualisationEntity`: Class

### Teleport
* `TMPTELEPORT`: Class
* `FINDFILESINTODIRECTORY`: Class
* `FINDDIALOGNODES`: Class

### Tornado
* `Tornado`: Class

### Touch
* `TOUCH`: Class

### Waterrings
* `WaterRings`: Class

### Weather
* `WEATHER`: Class
* `RAIN`: Class
* `SUNGLOW`: Class
* `LIGHTNING`: Class
* `SKY`: Class
* `WATERFLARE`: Class
* `Astronomy`: Class

### Worldmap
* `WorldMap`: Class

### Xinterface
* `STRSERVICE`: Service
* `SCRIPT_INTERFACE_FUNCTIONS`: Script Library
* `OBJ_STRSERVICE`: Class
* `TextureSequence`: Class
* `XINTERFACE`: Class
* `SCRSHOTER`: Class
* `HELPCHOOSER`: Class
* `InfoHandler`: Class
* `CONTROLS_CONTAINER`: Class
* `InterfaceBackScene`: Class
* `CAviPlayer`: Class

### Steam
* `SteamApiScriptLib`: Script Library


---

| [Home](../index.md) | [Site Map](../site-map.md) | 