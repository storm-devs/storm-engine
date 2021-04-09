// ============================================================================================
// Spirenkov Maxim, 2002
// --------------------------------------------------------------------------------------------
//
// --------------------------------------------------------------------------------------------
// grs
// --------------------------------------------------------------------------------------------
// The grs file format
// ============================================================================================

#pragma once

//============================================================================================

#pragma pack(push, 1)

//============================================================================================
//
//        GRSHeader
//        GRSMiniMapElement[miniZ][miniX]
//        GRSMapElement[numElements]
//
//============================================================================================

#define GRASS_ID 'ssrG'
#define GRASS_VER '00.1'

#define GRASS_BLK_DST 1.0f                          // Block size
#define GRASS_CNT_MIN 16                            // The minimum number of blades of grass in a block
#define GRASS_CNT_DLT 8                             // Spread in the number of blades of grass
#define GRASS_STEP (GRASS_BLK_DST / GRASS_BLK_SIZE) // Grass mesh step
#define GRASS_BLK_POW 5                             // Block size power 2^N
#define GRASS_BLK_SIZE (1 << GRASS_BLK_POW)         // Block grid size M = 2^N
#define GRASS_BLK_MASK (GRASS_BLK_SIZE - 1)         // Block size mask

struct GRSHeader
{
    unsigned long id;     // Identifier
    unsigned long ver;    // Version
    long miniX, miniZ;    // Minimap sizes
    float startX, startZ; // Map position
    long numElements;     // Number of elements (blades of grass)
};

struct GRSMiniMapElement
{
    long start;      // Block start
    long num[4];     // The amount of grass in the block
    float minHeight; // Minimum height in block
    float maxHeight; // Maximum height in block
};

struct GRSMapElement
{
    unsigned char x, z;         // Blade position in the block
    unsigned char w : 4, h : 4; // Width and Height
    unsigned char frame : 4;    // Frame
    unsigned char ang : 4;      // Rotation angle
    float y;                    // Height in world coordinates
};

//============================================================================================

#pragma pack(pop)

//============================================================================================
