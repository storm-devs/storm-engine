#include "data_color.h"

#include "../../i_common/graph_time.h"
#include "storm_assert.h"
#include "v_module_api.h"

// constructor / destructor
DataColor::DataColor() : ZeroColor(0xFFFFFFFF)
{
}

DataColor::~DataColor()
{
}

// Get the value (Current time, Total lifetime, Random factor [0..1])
Color DataColor::GetValue(float Time, float LifeTime, float K_rand)
{
    // The time for a color chart is always relative ...
    Time = (Time / LifeTime);

    uint32_t Count = ColorGraph.size();
    uint32_t StartIndex = 0;
    for (auto n = StartIndex; n < (Count - 1); n++)
    {
        auto FromTime = ColorGraph[n].Time;
        auto ToTime = ColorGraph[n + 1].Time;

        // If the time is in the correct range ...
        if ((Time >= FromTime) && (Time <= ToTime))
        {
            auto SegmentDeltaTime = ColorGraph[n + 1].Time - ColorGraph[n].Time;
            auto ValueDeltaTime = Time - ColorGraph[n].Time;
            auto blend_k = 0.0f;
            if (SegmentDeltaTime > 0.001f)
                blend_k = ValueDeltaTime / SegmentDeltaTime;

            auto ValueFirstMax = ColorGraph[n].MaxValue;
            auto ValueSecondMax = ColorGraph[n + 1].MaxValue;
            auto ValueFirstMin = ColorGraph[n].MinValue;
            auto ValueSecondMin = ColorGraph[n + 1].MinValue;

            Color MaxVal;
            MaxVal.Lerp(ValueFirstMax, ValueSecondMax, blend_k);
            Color MinVal;
            MinVal.Lerp(ValueFirstMin, ValueSecondMin, blend_k);

            Color Result;
            Result.Lerp(MinVal, MaxVal, K_rand);

            return Result;
        }
    }

    return ZeroColor;
}

// Sets the "default"
// two indices, Min = Max = Value
void DataColor::SetDefaultValue(const Color &Value)
{
    ColorGraph.clear();

    ColorVertex pMinVertex;
    pMinVertex.Time = MIN_GRAPH_TIME;
    pMinVertex.MinValue = Value;
    pMinVertex.MaxValue = Value;
    ColorGraph.push_back(pMinVertex);

    ColorVertex pMaxVertex;
    pMinVertex.Time = 1.0f;
    pMinVertex.MinValue = Value;
    pMinVertex.MaxValue = Value;
    ColorGraph.push_back(pMinVertex);
}

// Set values
void DataColor::SetValues(const ColorVertex *Values, uint32_t Count)
{
    ColorGraph.clear();
    for (uint32_t n = 0; n < Count; n++)
    {
        ColorGraph.push_back(Values[n]);
    }
}

// Get count of values
uint32_t DataColor::GetValuesCount() const
{
    return ColorGraph.size();
}

// Get min. value (by index)
const Color &DataColor::GetMinValue(uint32_t Index)
{
    return ColorGraph[Index].MinValue;
}

// Get max. value (by index)
const Color &DataColor::GetMaxValue(uint32_t Index)
{
    return ColorGraph[Index].MaxValue;
}

void DataColor::Load(MemFile *File)
{
    uint32_t dwColorCount = 0;
    File->ReadType(dwColorCount);

    for (uint32_t n = 0; n < dwColorCount; n++)
    {
        auto Time = 0.0f;
        File->ReadType(Time);

        Color clrMax;
        File->ReadType(clrMax);

        Color clrMin;
        File->ReadType(clrMin);

        ColorVertex pColor;
        pColor.Time = Time;
        pColor.MinValue = clrMin;
        pColor.MaxValue = clrMax;
        ColorGraph.push_back(pColor);
    }

    static char AttribueName[128];
    uint32_t NameLength = 0;
    File->ReadType(NameLength);
    Assert(NameLength < 128);
    File->Read(AttribueName, NameLength);

    SetName(AttribueName);
}

void DataColor::SetName(const char *szName)
{
    // core.Trace("DataColor::SetName - '%s'", szName);
    Name = szName;
}

const char *DataColor::GetName() const
{
    return Name.c_str();
}

const ColorVertex &DataColor::GetByIndex(uint32_t Index)
{
    return ColorGraph[Index];
}

void DataColor::Write(MemFile *File)
{
    uint32_t dwColorCount = ColorGraph.size();
    File->WriteType(dwColorCount);

    for (uint32_t n = 0; n < dwColorCount; n++)
    {
        auto Time = ColorGraph[n].Time;
        File->WriteType(Time);

        auto clrMax = ColorGraph[n].MaxValue;
        File->WriteType(clrMax);

        auto clrMin = ColorGraph[n].MinValue;
        File->WriteType(clrMin);
    }

    // save name
    uint32_t NameLength = Name.size();
    auto NameLengthPlusZero = NameLength + 1;
    File->WriteType(NameLengthPlusZero);
    Assert(NameLength < 128);
    File->Write(Name.c_str(), NameLength);
    File->WriteZeroByte();
}
