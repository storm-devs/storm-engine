#pragma once

#include "../../ICommon/ColorVertex.h"
#include "../../ICommon/MemFile.h"
#include <string>
#include <vector>

class DataColor
{
    std::string Name;

    const Color ZeroColor;

    std::vector<ColorVertex> ColorGraph;

  public:
    //конструктор/деструктор
    DataColor();
    virtual ~DataColor();

    //Получить значение (Текущее время, Общее время жизни, Коэфицент рандома[0..1])
    Color GetValue(float Time, float LifeTime, float K_rand);

    //Устанавливает "значение по умолчанию"
    //два индекса, Min=Max=Value
    void SetDefaultValue(const Color &Value);

    //Установить значения
    void SetValues(const ColorVertex *Values, uint32_t Count);

    //Получить кол-во значений
    uint32_t GetValuesCount() const;

    //Получить мин. значение (по индексу)
    const Color &GetMinValue(uint32_t Index);

    //Получить макс. значение (по индексу)
    const Color &GetMaxValue(uint32_t Index);

    void Load(MemFile *File);
    void Write(MemFile *File);

    void SetName(const char *szName);
    const char *GetName() const;

    const ColorVertex &GetByIndex(uint32_t Index);
};
