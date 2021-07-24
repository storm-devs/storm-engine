# Skybox texture conversion

Python script to convert _Pirates of the Caribbean_ sky textures to _Storm Engine 2.5+_ sky textures.

(Unpacks the side texture into four separate textures)

## Usage

```
find RESOURCE/Textures/WEATHER/SKIES -name sky_side.tga.tx | xargs -n 1 python sky_convert.py
```