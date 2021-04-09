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
    long id;                // File ID
    long space0;            // Unused
    long ver;               // File version
    long space1;            // Unused
    long isSea;             // Writes whether the patch is for a marine location or not
    long space2;            // Unused
    long numTriangles;      // Number of triangles
    long numVerteces;       // Number of indexed vertices
    long numNormals;        // Number of normals
    long mapL, mapW;        // Collision map dimensions
    long numIndeces;        // Index table size
    long lineSize;          // The size of the row in the path lookup table
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
    long start; // The beginning of the table of polygons included in the map cell
    long size;  // Size of the table of polygons included in the map cell
};

struct PtcMaterials
{
    long numMaterials;     // Number of materials
    char material[16][16]; // Material table
};

//============================================================================================

#pragma pack(pop)
