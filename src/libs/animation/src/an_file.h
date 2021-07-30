#pragma once

// ANIMATION_HEADER
// long parentIndex[nJoints]
// Vector jointPos[nJoints]

// Frames:
// Vector rootPos[nFrames]
// Vector jointAngle[nJoints][nFrames]

#pragma pack(push, 1)

namespace ANFILE
{
struct HEADER
{
    long nFrames;
    long nJoints;
    float framesPerSec;
};
}; // namespace ANFILE

#pragma pack(pop)
