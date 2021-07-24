# Blender AN export

## Требования

Для корректной работы необходимо:

- [NodeJs](https://nodejs.org/ru/)

## Экспорт .an.json из Blender
Выбрать объект со скелетом

![choose.img](readme-img/choose.png)

```
File -> Export... -> JSON AN Export(.an.json)
```

Для корректной работы экспортированной анимации кости скелета должны быть очень маленькими (размеры стремиться к точке)

Координаты по X и Y должны быть одинаковыми, Z отличаться на 0.00001.
Также Roll должен быть 90°

![choose.img](readme-img/bone.png)

## Конвертирование в .an
Экспортированный из Blender .an.json необходимо конвертировать в .an.
Чтобы это сделать, необходимо запустить консоль (терминал) в папке со скриптом ``parse-an-json.js`` и ввести команду
``node parse-an-json.js {filepath}``

Пример:
``node parse-an-json.js "C:\animations\man.an.json"``

В папке с анимацией будет сохранен файл с результатами ``man.an``

### Примечание
Для пользователей Windows есть более простой способ —
можно перетащить на файл ``parse-an-json.cmd`` файл с распаршенной анимацией (.an.json) и в папке с анимацией будет сохранен файл с результатами

## Установка плагина в Blender
```
Edit -> Preferences... -> Add-ons -> Install... -> Выбрать export_json_an.py из папки io_export_json_an -> Включить "Export: JSON AN"
```

## Автор

[Artess999](https://github.com/Artess999)

