// ============================================================================================
// Spirenkov Maxim, 2001
// --------------------------------------------------------------------------------------------
// Patch
// --------------------------------------------------------------------------------------------
// ptc
// --------------------------------------------------------------------------------------------
// Data file required to find the path and collision of characters with the ground
// ============================================================================================

#pragma once

#pragma pack(push, 1)

#include <cstdint>

// ============================================================================================
//
// Ptc file structure:
//
//
// PtcHeader - File Header
// PtcTriangle[PtcHeader.numTriangles]
// PtcVertex[PtcHeader.numVerteces]
// PtcNormal[PtcHeader.numNormals]
// PtcMap[PtcHeader.mapL*PtcHeader.mapW]
// unsigned short [numIndeces]
// path lines
// PtcMaterials
//
// ============================================================================================

#define PTC_ID ' CTP'          // File ID
#define PTC_VERSION '10.1'     // File version
#define PTC_PREVERSION1 '00.1' // Previous version of the file
#define PTC_SPACEFLD '    '    // Spaces for easy reading
#define PTC_WITHSEA ' AES'     // Marine location patch
#define PTC_NOSEA ' HTE'       // Patch for a location without the sea

// File header
struct PtcHeader
{
    int32_t id;                // File ID
    int32_t space0;            // Unused
    int32_t ver;               // File version
    int32_t space1;            // Unused
    int32_t isSea;             // Writes whether the patch is for a marine location or not
    int32_t space2;            // Unused
    int32_t numTriangles;      // Number of triangles
    int32_t numVerteces;       // Number of indexed vertices
    int32_t numNormals;        // Number of normals
    int32_t mapL, mapW;        // Collision map dimensions
    int32_t numIndeces;        // Index table size
    int32_t lineSize;          // The size of the row in the path lookup table
    float minX, minY, minZ; // Minimum box border
    float maxX, maxY, maxZ; // Maximum box border
};

struct PtcTriangle
{
    unsigned short i[3]; // Vertex indices
    unsigned short n;    // Normal to triangle
    short nb[3];         // Neighbor indices
    char mtl;            // Material index
    char flags;
};

struct PtcVertex
{
    float x; // X Position
    float y; // Y position
    float z; // Z Position
};

struct PtcNormal
{
    float x; // X direction
    float y; // Y direction
    float z; // Z direction
};

struct PtcMap
{
    int32_t start; // The beginning of the table of polygons included in the map cell
    int32_t size;  // Size of the table of polygons included in the map cell
};

struct PtcMaterials
{
    int32_t numMaterials;     // Number of materials
    char material[16][16]; // Material table
};

//============================================================================================

#pragma pack(pop)
