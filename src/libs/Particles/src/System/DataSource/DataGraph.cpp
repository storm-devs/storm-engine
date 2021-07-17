#include "DataGraph.h"
#include "../../ICommon/GraphTime.h"
#include "../../ICommon/Types.h"
#include "vmodule_api.h"

#pragma warning(disable : 4800)

extern uint32_t GraphRead;

// Linear interpolation
float Lerp(float val1, float val2, float lerp_k)
{
    return (val1 + (val2 - val1) * lerp_k);
}

// Take a random number from a range
float RandomRange(float Min, float Max)
{
    float Temp;
    if (Min > Max)
    {
        Temp = Max;
        Max = Min;
        Min = Temp;
    }
    auto Rand = static_cast<float>(rand() % 1000) / 1000.0f;
    Rand *= (Max - Min);
    return Rand + Min;
}

// constructor / destructor
DataGraph::DataGraph()
{
    bRelative = false;
    bNegative = false;

    ResetCachedTime();
}

DataGraph::~DataGraph()
{
}

// Set values
void DataGraph::SetValues(const GraphVertex *MinValues, uint32_t MinValuesSize, const GraphVertex *MaxValues,
                          uint32_t MaxValuesSize)
{
    MinGraph.clear();
    MaxGraph.clear();

    uint32_t n = 0;
    for (n = 0; n < MinValuesSize; n++)
    {
        MinGraph.push_back(MinValues[n]);
    }

    for (n = 0; n < MaxValuesSize; n++)
    {
        MaxGraph.push_back(MaxValues[n]);
    }

    ResetCachedTime();
}

// Set the "default"
void DataGraph::SetDefaultValue(float MaxValue, float MinValue)
{
    //~!~
    MinGraph.clear();
    MaxGraph.clear();

    GraphVertex Min;
    Min.Val = MinValue;
    Min.Time = MIN_GRAPH_TIME;
    MinGraph.push_back(Min);
    // Min.Time = MAX_GRAPH_TIME;
    MinGraph.push_back(Min);

    GraphVertex Max;
    Max.Val = MaxValue;
    // Max.Time = MIN_GRAPH_TIME;
    MaxGraph.push_back(Max);
    Max.Time = MAX_GRAPH_TIME;
    MaxGraph.push_back(Max);

    ResetCachedTime();
}

// Get the count in the minimum graph
uint32_t DataGraph::GetMinCount() const
{
    return MinGraph.size();
}

// Get the count in the maximum graph
uint32_t DataGraph::GetMaxCount() const
{
    return MaxGraph.size();
}

// Get the value by index from the minimum graph
const GraphVertex &DataGraph::GetMinVertex(uint32_t Index)
{
    return MinGraph[Index];
}

// Get the value by index from the maximum graph
const GraphVertex &DataGraph::GetMaxVertex(uint32_t Index)
{
    return MaxGraph[Index];
}

void DataGraph::ResetCachedTime()
{
    MaxCachedTime = NOT_INITED_CACHE_VALUE;
    MinCachedTime = NOT_INITED_CACHE_VALUE;
}

void DataGraph::Load(MemFile *File)
{
    MinGraph.clear();
    MaxGraph.clear();

    uint32_t dwNegative = 0;
    File->ReadType(dwNegative);
    SetNegative(dwNegative);

    uint32_t dwRelative = 0;
    File->ReadType(dwRelative);
    SetRelative(dwRelative);

    uint32_t MaxGraphItemsCount = 0;
    File->ReadType(MaxGraphItemsCount);

    uint32_t i = 0;
    for (i = 0; i < MaxGraphItemsCount; i++)
    {
        auto fTime = 0.0f;
        File->ReadType(fTime);

        auto fValue = 0.0f;
        File->ReadType(fValue);

        if (!bNegative && fValue < 0.0f)
            fValue = 0.0f;

        GraphVertex MaxVertex;
        MaxVertex.Time = fTime;
        MaxVertex.Val = fValue;
        MaxGraph.push_back(MaxVertex);

        // core.Trace("Max value %d = %3.2f, %3.2f", i, fTime, fValue);
    }

    uint32_t MinGraphItemsCount = 0;
    File->ReadType(MinGraphItemsCount);

    for (i = 0; i < MinGraphItemsCount; i++)
    {
        auto fTime = 0.0f;
        File->ReadType(fTime);

        auto fValue = 0.0f;
        File->ReadType(fValue);

        if (!bNegative && fValue < 0.0f)
            fValue = 0.0f;

        GraphVertex MinVertex;
        MinVertex.Time = fTime;
        MinVertex.Val = fValue;
        MinGraph.push_back(MinVertex);

        // core.Trace("Min value %d = %3.2f, %3.2f", i, fTime, fValue);
    }

    static char AttribueName[128];
    uint32_t NameLength = 0;
    File->ReadType(NameLength);
    Assert(NameLength < 128);
    File->Read(AttribueName, NameLength);

    // core.Trace("Name %s", AttribueName);

    SetName(AttribueName);

    // HACK! For backward compatibility
    // convert after loading the graphs into the desired format
    // if (_stricmp (AttribueName, EMISSION_DIR_X) == 0)    ConvertDegToRad ();
    // if (_stricmp (AttribueName, EMISSION_DIR_Y) == 0)    ConvertDegToRad ();
    // if (_stricmp (AttribueName, EMISSION_DIR_Z) == 0)    ConvertDegToRad ();
    // if (_stricmp (AttribueName, PARTICLE_DRAG) == 0)    NormalToPercent();
    // if (_stricmp (AttribueName, PARTICLE_TRANSPARENCY) == 0)    NormalToAlpha();
}

// Set / get can be negative values in the graph or not
void DataGraph::SetNegative(bool _bNegative)
{
    bNegative = _bNegative;
}

bool DataGraph::GetNegative() const
{
    return bNegative;
}

// Set / get relative schedule or not
void DataGraph::SetRelative(bool _bRelative)
{
    bRelative = _bRelative;
}

bool DataGraph::GetRelative() const
{
    return bRelative;
}

void DataGraph::SetName(const char *szName)
{
    // core.Trace("DataGraph::SetName - '%s'", szName);
    Name = szName;
}

const char *DataGraph::GetName() const
{
    return Name.c_str();
}

void DataGraph::ConvertRadToDeg()
{
    MultiplyBy(MUL_RADTODEG);
}

void DataGraph::ConvertDegToRad()
{
    MultiplyBy(MUL_DEGTORAD);
}

void DataGraph::MultiplyBy(float Val)
{
    uint32_t n;
    for (n = 0; n < MaxGraph.size(); n++)
        MaxGraph[n].Val *= Val;

    for (n = 0; n < MinGraph.size(); n++)
        MinGraph[n].Val *= Val;
}

float DataGraph::GetMinAtTime(float Time, float LifeTime)
{
    if (bRelative)
        Time = Time / LifeTime * 100.0f;

    const uint32_t Count = MinGraph.size();
    uint32_t Index;
    if (MinCachedTime < Time)
        Index = MinCachedIndex;
    else
        Index = 0;

    for (; Index < (Count - 1); Index++)
    {
        const auto ToTime = MinGraph[Index + 1].Time;

        // If the time is in the correct range
        // if ((Time >= FromTime) && (Time <= ToTime))
        if (Time <= ToTime)
        {
            const auto FromTime = MinGraph[Index].Time;

            const auto SegmentDeltaTime = ToTime - FromTime;
            const auto ValueDeltaTime = Time - FromTime;
            float blend_k;
            if (SegmentDeltaTime > 0.001f)
                blend_k = ValueDeltaTime / SegmentDeltaTime;
            else
                blend_k = 0.0f;

            const auto ValueFirst = MinGraph[Index].Val;
            const auto ValueSecond = MinGraph[Index + 1].Val;

            MinCachedTime = Time;
            MinCachedIndex = Index;
            return Lerp(ValueFirst, ValueSecond, blend_k);
        }
    }

    return 0.0f;
}

float DataGraph::GetMaxAtTime(float Time, float LifeTime)
{
    if (bRelative)
        Time = Time / LifeTime * 100.0f;

    const uint32_t Count = MaxGraph.size();

    uint32_t Index;

    if (MaxCachedTime < Time)
        Index = MaxCachedIndex;
    else
        Index = 0;

    for (; Index < (Count - 1); Index++)
    {
        const auto ToTime = MaxGraph[Index + 1].Time;

        // If the time is in the correct range
        // if ((Time >= FromTime) && (Time <= ToTime))
        if (Time <= ToTime)
        {
            const auto FromTime = MaxGraph[Index].Time;

            const auto SegmentDeltaTime = ToTime - FromTime;
            const auto ValueDeltaTime = Time - FromTime;
            float blend_k;
            if (SegmentDeltaTime > 0.001f)
                blend_k = ValueDeltaTime / SegmentDeltaTime;
            else
                blend_k = 0.0f;

            const auto ValueFirst = MaxGraph[Index].Val;
            const auto ValueSecond = MaxGraph[Index + 1].Val;

            MaxCachedTime = Time;
            MaxCachedIndex = Index;
            return Lerp(ValueFirst, ValueSecond, blend_k);
        }
    }

    return 0.0f;
}

float DataGraph::GetValue(float Time, float LifeTime, float K_rand)
{
    GraphRead++;
    const auto pMax = GetMaxAtTime(Time, LifeTime);
    const auto pMin = GetMinAtTime(Time, LifeTime);
    return Lerp(pMin, pMax, K_rand);
}

float DataGraph::GetRandomValue(float Time, float LifeTime)
{
    GraphRead++;
    const auto pMax = GetMaxAtTime(Time, LifeTime);
    const auto pMin = GetMinAtTime(Time, LifeTime);
    return RandomRange(pMin, pMax);
}

void DataGraph::Clamp(float MinValue, float MaxValue)
{
    uint32_t n;
    for (n = 0; n < MaxGraph.size(); n++)
    {
        if (MaxGraph[n].Val > MaxValue)
            MaxGraph[n].Val = MaxValue;
        if (MaxGraph[n].Val < MinValue)
            MaxGraph[n].Val = MinValue;
    }

    for (n = 0; n < MinGraph.size(); n++)
    {
        if (MinGraph[n].Val > MaxValue)
            MinGraph[n].Val = MaxValue;
        if (MinGraph[n].Val < MinValue)
            MinGraph[n].Val = MinValue;
    }
}

void DataGraph::Reverse()
{
    uint32_t n;
    for (n = 0; n < MaxGraph.size(); n++)
        MaxGraph[n].Val = 1.0f - MaxGraph[n].Val;

    for (n = 0; n < MinGraph.size(); n++)
        MinGraph[n].Val = 1.0f - MinGraph[n].Val;
}

void DataGraph::NormalToPercent()
{
    MultiplyBy(0.01f);
    Reverse();
    Clamp(0.0f, 1.0f);
}

void DataGraph::PercentToNormal()
{
    Reverse();
    MultiplyBy(100.0f);
}

void DataGraph::NormalToAlpha()
{
    NormalToPercent();
    MultiplyBy(255.0f);
}

void DataGraph::AlphaToNormal()
{
    MultiplyBy(0.00392156862745098f);
    PercentToNormal();
}

float DataGraph::GetMaxTime()
{
    auto MaxVal = 10.0f;
    auto MinVal = 10.0f;
    const uint32_t MaxCount = MaxGraph.size();
    const uint32_t MinCount = MinGraph.size();

    if (MaxCount > 2)
        MaxVal = MaxGraph[MaxCount - 2].Time;
    if (MinCount > 2)
        MinVal = MinGraph[MinCount - 2].Time;

    if (MaxVal > MinVal)
        return MaxVal;
    return MinVal;
}

void DataGraph::Write(MemFile *File)
{
    uint32_t dwNegative = GetNegative();
    File->WriteType(dwNegative);

    uint32_t dwRelative = GetRelative();
    File->WriteType(dwRelative);

    uint32_t MaxGraphItemsCount = MaxGraph.size();
    File->WriteType(MaxGraphItemsCount);

    uint32_t i = 0;
    for (i = 0; i < MaxGraphItemsCount; i++)
    {
        auto fTime = MaxGraph[i].Time;
        File->WriteType(fTime);

        auto fValue = MaxGraph[i].Val;
        File->WriteType(fValue);
    }

    uint32_t MinGraphItemsCount = MinGraph.size();
    File->WriteType(MinGraphItemsCount);

    for (i = 0; i < MinGraphItemsCount; i++)
    {
        auto fTime = MinGraph[i].Time;
        File->WriteType(fTime);

        auto fValue = MinGraph[i].Val;
        File->WriteType(fValue);
    }

    // save name
    const uint32_t NameLength = Name.size();
    auto NameLengthPlusZero = NameLength + 1;
    File->WriteType(NameLengthPlusZero);
    Assert(NameLength < 128);
    File->Write(Name.c_str(), NameLength);
    File->WriteZeroByte();
}
