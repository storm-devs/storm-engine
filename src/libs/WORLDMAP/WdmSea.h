//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmSea
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _WdmSea_H_
#define _WdmSea_H_

#include "WdmRenderObject.h"

class WdmSea : public WdmRenderObject
{
#pragma pack(push, 1)

    struct Triangle
    {
        uint16_t index[3];
    };

    struct Vertex
    {
        float x, y, z;
        uint32_t color;
        float tu, tv;
    };

    struct WhiteHorses
    {
        float x;
        float z;
        float angle;
        float size;
        float speed;
        float curTime;
        float kTime;
        long textureIndex;
    };

    struct Flare
    {
        long index;  //Индекс используемого ректа
        float time;  //Время жизни от 0 до 1
        float k;     //Скорость течения времени
        float phase; //Фаза, задающая направление вращения
    };

#pragma pack(pop)

    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    WdmSea();
    virtual ~WdmSea();

    void Update(float dltTime) override;

    void PRender(VDX9RENDER *rs) override;
    void LRender(VDX9RENDER *rs) override;

    //--------------------------------------------------------------------------------------------
    //Инкапсуляция
    //--------------------------------------------------------------------------------------------
  private:
    //Нарисовать
    void Render(VDX9RENDER *rs, const char *tech = nullptr) const;
    //Настроить преобразования текстурных координат
    void PresetMain(VDX9RENDER *rs) const;

  private:
    //Буфера для геометрии
    long vb, ib;
    //Барашки
    WhiteHorses wh[16];
    // Textures
    long baseTexture;
    long aniTextures[32];
    float aniFrame;
    long whiteHorses[1];
    float seaSizeX;
    float seaSizeZ;
    float aniTiling;
    //Вспышки
    Flare flare[1024];
    RS_RECT flareRect[1024];
    long flareCount;
    long flareTexture;
    float flareCerateCounter;
};

#endif
