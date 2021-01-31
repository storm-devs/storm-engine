//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmClouds
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _WdmClouds_H_
#define _WdmClouds_H_

#include "WdmRenderObject.h"

class WdmCloud;

#define WDMCLOUDSMAX 32

class WdmClouds : public WdmRenderObject
{
    static IDirect3DVertexDeclaration9 *vertexDecl_;
    void CreateVertexDeclaration(VDX9RENDER *rs);

    struct Cloud
    {
        struct Cld
        {
            CVECTOR pos;    //Позиция
            float angle;    //Угол поворота
            float size;     //Размер
            float alpha;    //Прозрачность
            uint32_t color; //Цвет
            uint16_t pict;  //Индекс картинки
            uint16_t index; //Индекс подтягиваемого партикла
            float aspd;     //Скорость поворота
            CVECTOR dir;    //Текущий вектор корректировки движения
            CVECTOR rdr;    //Добавочный вектор рандомного тяготения
        };

        Cloud();

        //Если облако переродилось то возвращает true
        bool Reset(bool isFirstTime = false);
        //Обновить состояние облака
        void Update(float dltTime);
        //Заполнить массив прямоугольников
        long FillRects(RS_RECT *rects, long cnt, float galpha);
        //Получить центр сферы и радиус
        float GetBound(CVECTOR &_center) const;
        //Запустить механизм удаления облака если есть пересечение
        void Kill(const Cloud &cld);

      private:
        Cld cloud[WDMCLOUDSMAX]; //Позиции
        long count;              //Количество
        float alpha;             //Общая прозрачность
        float lifeTime;          //Время жизни
        CVECTOR center;          //Центр облака
        float radius;            //Радиус облака
    };

    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    WdmClouds();
    virtual ~WdmClouds();

    //Расчёты
    void Update(float dltTime) override;
    //Рисование
    void LRender(VDX9RENDER *rs) override;

    long texture, light;
    Cloud clouds[16];
    RS_RECT rects[WDMCLOUDSMAX * 16];
};

#endif
