#pragma once

// ANIMATION_HEADER
// int32_t parentIndex[nJoints]
// CVECTOR jointPos[nJoints]

// Frames:
// CVECTOR rootPos[nFrames]
// CVECTOR jointAngle[nJoints][nFrames]

#pragma pack(push, 1)

namespace ANFILE
{
struct HEADER
{
    int32_t nFrames;
    int32_t nJoints;
    float framesPerSec;
};
}; // namespace ANFILE

#pragma pack(pop)
