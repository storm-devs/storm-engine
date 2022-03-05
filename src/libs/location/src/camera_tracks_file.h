// ===========================================================================================================================
//  Animation temporary file
//  ANT file
//  Ver 1.0
// ===========================================================================================================================
//
// File structure:
//
//     AntFileHeader
//     char[AntFileHeader.stringsTableSize]
//     AntFileBone[AntFileHeader.bonesCount]
//     AntFileTrackElement[AntFileHeader.framesCount]
//
//
// ===========================================================================================================================
#pragma once

#define ANTFILE_ID (uint32_t)(('A' << 0) | ('N' << 8) | ('T' << 16) | (' ' << 24))
#define ANTFILE_VER (uint32_t)(('1' << 0) | ('.' << 8) | ('0' << 16) | (' ' << 24))
#define ANTFILE_NOPARENT (-1L)

#pragma pack(push, 1)

// File header
struct AntFileHeader
{
    uint32_t id;               // File ID
    uint32_t ver;              // File version
    uint32_t bonesCount;       // Number of bones in file >= 1
    uint32_t framesCount;      // Number of animation frames >= 1
    uint32_t stringsTableSize; // Size of the Bone names table
    float defaultFPS;               // Animation speed
};

// Bone description
struct AntFileBone
{
    int32_t parentIndex;        // Parent bone index, less than current
    uint32_t nameIndex; // The index of the row in the string table
};

// Track item description
struct AntFileTrackElement
{
    float qx, qy, qz, qw; // Quaternion
    float px, py, pz;     // Position
    float sx, sy, sz;     // Scaling factors
};

#pragma pack(pop)
