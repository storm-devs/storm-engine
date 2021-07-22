#include "DataUV.h"
#include "vmodule_api.h"

// constructor / destructor
DataUV::DataUV()
{
}

DataUV::~DataUV()
{
}

// Get the value [x, y = UV1; z, w = UV2]
const Vector4 &DataUV::GetValue(uint32_t FrameNum)
{
    const uint32_t TotalFrames = Frames.size();
    FrameNum = FrameNum % TotalFrames;
    return Frames[FrameNum];
}

// Set values
void DataUV::SetValues(const Vector4 *_Frames, uint32_t FramesCount)
{
    Frames.clear();

    for (uint32_t n = 0; n < FramesCount; n++)
    {
        Frames.push_back(_Frames[n]);
    }
}

// Get the number of frames
uint32_t DataUV::GetFrameCount() const
{
    return Frames.size();
}

void DataUV::Load(MemFile *File)
{
    uint32_t ElementCount = 0;
    File->ReadType(ElementCount);
    for (uint32_t n = 0; n < ElementCount; n++)
    {
        Vector4 rFrame;
        File->ReadType(rFrame.x);
        File->ReadType(rFrame.y);
        File->ReadType(rFrame.z);
        File->ReadType(rFrame.w);

        Vector4 newFrame;
        newFrame = rFrame;
        newFrame.z += newFrame.x;
        newFrame.w += newFrame.y;
        Frames.push_back(newFrame);
    }

    static char AttribueName[128];
    uint32_t NameLength = 0;
    File->ReadType(NameLength);
    Assert(NameLength < 128);
    File->Read(AttribueName, NameLength);

    SetName(AttribueName);
}

void DataUV::SetName(const char *szName)
{
    // core.Trace("DataUV::SetName - '%s'", szName);
    Name = szName;
}

const char *DataUV::GetName() const
{
    return Name.c_str();
}

void DataUV::Write(MemFile *File)
{
    auto ElementCount = GetFrameCount();
    File->WriteType(ElementCount);
    for (uint32_t n = 0; n < ElementCount; n++)
    {
        auto w = Frames[n].z - Frames[n].x;
        auto h = Frames[n].w - Frames[n].y;
        File->WriteType(Frames[n].x);
        File->WriteType(Frames[n].y);
        File->WriteType(w);
        File->WriteType(h);
    }

    // save name
    const uint32_t NameLength = Name.size();
    auto NameLengthPlusZero = NameLength + 1;
    File->WriteType(NameLengthPlusZero);
    Assert(NameLength < 128);
    File->Write(Name.c_str(), NameLength);
    File->WriteZeroByte();
}
