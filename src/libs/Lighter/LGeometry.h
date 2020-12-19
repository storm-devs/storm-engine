//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	LGeometry
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _LGeometry_H_
#define _LGeometry_H_

#include "LTypes.h"
#include "model.h"

class LGeometry
{
    struct Object
    {
        Object()
        {
            name = nullptr;
            nameReal = nullptr;
            m = nullptr;
            lBufSize = 0;
        };
        char *name;     //Путь то col файла
        char *nameReal; //Имя модельки
        MODEL *m;       //Указатель на модельку
        entid_t model;  //Моделька
        long lBufSize;  //Размер буфера цветов для этой модельки
    };

    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    LGeometry();
    virtual ~LGeometry();

    //Установить путь до моделек
    void SetModelsPath(const char *mPath);
    //Установить путь для текущей погоды
    void SetLightPath(const char *lPath);
    //Добавить объект
    void AddObject(const char *name, entid_t model);
    //Обработать данные
    bool Process(VDX9RENDER *rs, long numLights);
    //Нарисовать нормали
    void DrawNormals(VDX9RENDER *rs);
    //Обновить цвета в буферах
    void UpdateColors(VDX9RENDER *rs);
    //Протрейсить луч через все модели
    float Trace(const CVECTOR &src, const CVECTOR &dst);
    //Сохранить освещение
    bool Save();

    std::vector<Object> object;
    long numObjects;
    long maxObjects;

    std::vector<Vertex> vrt;
    long numVrt;
    long maxVrt;

    std::vector<Triangle> trg;
    long numTrg;
    long maxTrg;

    std::vector<VertexBuffer> vbuffer;
    long numVBuffers;
    long maxVBuffers;

    lighter::Shadow *shadows;

    CVECTOR min, max;
    float radius;

    bool useColor;

    CVECTOR *drawbuf;

    char modelsPath[512];
    char lightPath[512];
};

#endif
