#pragma once

#include "../../ICommon/MemFile.h"

#include "math3d.h"
#include <string>
#include <vector>

class DataUV
{
    std::string Name;

    std::vector<Vector4> Frames;

  public:
    // constructor / destructor
    DataUV();
    virtual ~DataUV();

    // Get the value [x, y = UV1; z, w = UV2]
    const Vector4 &GetValue(uint32_t FrameNum);

    // Set values
    void SetValues(const Vector4 *_Frames, uint32_t FramesCount);

    // Get the number of frames
    uint32_t GetFrameCount() const;

    void Load(MemFile *File);
    void Write(MemFile *File);

    void SetName(const char *szName);
    const char *GetName() const;
};
