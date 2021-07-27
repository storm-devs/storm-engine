# Blender GM import

## Требования

Для корректной работы необходимо:

- Модели в формате .gm и текстуры в формате .tga

Текстуры должны находиться в той же папке, что и модели, в подпапке textures.
Пример:
```
C/
    example-folder/
        model.gm
        textures/
            texture1.tga
            texture2.tga
```

В случае, если текстура отсутствует, в Blender будет создана серая текстура-заглушка с именем исходной текстуры

![placeholder.img](readme-img/placeholder.png)

## Установка плагина в Blender
```
Edit -> Preferences... -> Add-ons -> Install... -> Выбрать import_gm.py из папки io_import_gm -> Включить "Import: SeaDogs GM"
```

## Использование плагина в Blender
```
File -> Import... -> GM Import(.gm)
```

Текстуры должны находиться в той же папке, что и модели, в подпапке textures.
Пример:
```
C/
    example-folder/
        model.gm
        textures/
            texture1.tga
            texture2.tga
```

## Автор

[Artess999](https://github.com/Artess999)

