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

#define ANTFILE_ID (unsigned long)(('A' << 0) | ('N' << 8) | ('T' << 16) | (' ' << 24))
#define ANTFILE_VER (unsigned long)(('1' << 0) | ('.' << 8) | ('0' << 16) | (' ' << 24))
#define ANTFILE_NOPARENT (-1L)

#pragma pack(push, 1)

// File header
struct AntFileHeader
{
    unsigned long id;               // File ID
    unsigned long ver;              // File version
    unsigned long bonesCount;       // Number of bones in file >= 1
    unsigned long framesCount;      // Number of animation frames >= 1
    unsigned long stringsTableSize; // Size of the Bone names table
    float defaultFPS;               // Animation speed
};

// Bone description
struct AntFileBone
{
    long parentIndex;        // Parent bone index, less than current
    unsigned long nameIndex; // The index of the row in the string table
};

// Track item description
struct AntFileTrackElement
{
    float qx, qy, qz, qw; // Quaternion
    float px, py, pz;     // Position
    float sx, sy, sz;     // Scaling factors
};

#pragma pack(pop)
