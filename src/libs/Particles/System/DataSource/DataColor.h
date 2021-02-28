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
    // constructor / destructor
    DataColor();
    virtual ~DataColor();

    // Get the value (Current time, Total lifetime, Random factor [0..1])
    Color GetValue(float Time, float LifeTime, float K_rand);

    // Sets the "default"
    // two indices, Min = Max = Value
    void SetDefaultValue(const Color &Value);

    // Set values
    void SetValues(const ColorVertex *Values, uint32_t Count);

    // Get count of values
    uint32_t GetValuesCount() const;

    // Get min. value (by index)
    const Color &GetMinValue(uint32_t Index);

    // Get max. value (by index)
    const Color &GetMaxValue(uint32_t Index);

    void Load(MemFile *File);
    void Write(MemFile *File);

    void SetName(const char *szName);
    const char *GetName() const;

    const ColorVertex &GetByIndex(uint32_t Index);
};
