# Blender AN export

[Русская версия Readme](README_RUS.md)

## Export .an Blender
Select an object with armature

![choose.img](readme-img/choose.png)

```
File -> Export... -> AN Export(.an)
```

Exported animation works correctly only if bone is very small (tends to small point)

X and Y coordinates must be the same, Z differ by 0.00001.
Also Roll should be 90°

![choose.img](readme-img/bone.png)

In animations only root bone location could be changed

Angle changes will work for all bones (Angles MUST be in quaternions)

![angles.img](readme-img/angles.png)

Each bone in each frame should have keyframes location and rotation

![keyframes1.img](readme-img/keyframes1.png)

![keyframes2.img](readme-img/keyframes2.png)

## Blender Plugin Installation
```
Edit -> Preferences... -> Add-ons -> Install... -> Select export_an.py from io_export_an folder -> Enable "Export: SeaDogs AN"
```

## Blender Usage
```
File -> Export... -> AN Export(.an)
```

## Author

[Artess999](https://github.com/Artess999)

