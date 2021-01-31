#ifndef _PARTICLE_DATA_GRAPH_H_
#define _PARTICLE_DATA_GRAPH_H_

#include "../../ICommon/MemFile.h"

#include "../../ICommon/GraphVertex.h"
#include <string>
#include <vector>

class DataGraph
{
    std::string Name;

    //С какого времени последний раз забирали значение
    float MaxCachedTime;
    float MinCachedTime;
    //Какой был индекс у этого времени
    uint32_t MaxCachedIndex;
    uint32_t MinCachedIndex;

    std::vector<GraphVertex> MinGraph;
    std::vector<GraphVertex> MaxGraph;

    void ResetCachedTime();

    float GetMinAtTime(float Time, float LifeTime);
    float GetMaxAtTime(float Time, float LifeTime);

    bool bRelative;
    bool bNegative;

  public:
    //конструктор/деструктор
    DataGraph();
    virtual ~DataGraph();

    //Установить/получить могут быть отрицательные значения в графике или нет...
    void SetNegative(bool _bNegative);
    bool GetNegative() const;

    //Установить/получить относительный график или нет...
    void SetRelative(bool _bRelative);
    bool GetRelative() const;

    //Получить значение (Текущее время, Коэфицент рандома[0..1])
    float GetValue(float Time, float LifeTime, float K_rand);
    float GetRandomValue(float Time, float LifeTime);

    //Установить значения...
    void SetValues(const GraphVertex *MinValues, uint32_t MinValuesSize, const GraphVertex *MaxValues,
                   uint32_t MaxValuesSize);

    //Устанавливает "значение по умолчанию"
    void SetDefaultValue(float MaxValue, float MinValue);

    //Получить кол-во в графике минимума
    uint32_t GetMinCount() const;

    //Получить кол-во в графике максимума
    uint32_t GetMaxCount() const;

    //Получить значение по индексу из графика минимума
    const GraphVertex &GetMinVertex(uint32_t Index);

    //Получить значение по индексу из графика максимума
    const GraphVertex &GetMaxVertex(uint32_t Index);

    void Load(MemFile *File);
    void Write(MemFile *File);

    void SetName(const char *szName);
    const char *GetName() const;

    float GetMaxTime();

    void ConvertRadToDeg();
    void ConvertDegToRad();
    void MultiplyBy(float Val);
    void Clamp(float MinValue, float MaxValue);
    void Reverse(); // Graphs = 1.0f - Graph
    void NormalToPercent();
    void PercentToNormal();
    void NormalToAlpha();
    void AlphaToNormal();
};

#endif
