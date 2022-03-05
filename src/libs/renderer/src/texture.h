//================================================================
// Texture file .tx
//================================================================

#pragma once

//================================================================
//
//  File structure:
//
//        TEXTURE_FILE  info header
//
//        pallette      if(TEXTURE_FILE.flags & TX_FLAGS_PALLETTE != 0)
//
//        mip levels            \
//        width x heigth         | TEXTURE_FILE.nmips
//        width/2 x heigth/2     |
//        ...                    /
//
//   Remarks:
//        TEXTURE_FORMAT equal of a DX9 D3DFORMAT
//        pallette color TX_PALETTE_COLOR equal of a PALETTEENTRY (std. windows structure)
//    CubeMap:
//        always is quads
//        sequence order: front, right, back, left, top, bottom
//
//================================================================

// Flags
#define TX_FLAGS_NONE 0
#define TX_FLAGS_PALLETTE 1
#define TX_FLAGS_CUBEMAP 2

// Formats
enum TX_FORMAT
{
    TXF_A8R8G8B8 = 21,
    TXF_X8R8G8B8 = 22,
    TXF_R5G6B5 = 23,
    TXF_A1R5G5B5 = 25,
    TXF_A4R4G4B4 = 26,
    TXF_P8 = 41,
    TXF_L8 = 50,
    TXF_V8U8 = 60,
    TXF_L6V5U5 = 61,
    TXF_DXT1 = '1TXD',
    TXF_DXT2 = '2TXD',
    TXF_DXT3 = '3TXD',
    TXF_DXT4 = '4TXD',
    TXF_DXT5 = '5TXD',
};

#pragma pack(push, 1)

// File header
struct TX_FILE_HEADER
{
    int32_t flags;         // info flags
    int32_t width, height; // texture size
    int32_t nmips;         // number mip levels
    TX_FORMAT format;   // texture format
    int32_t mip_size;      // size of mip 0 (width*height*pixel_size)
};

// Pallette color
struct TX_PALETTE_COLOR
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t f;
};

// Texture pallete
struct TX_PALETTE
{
    TX_PALETTE_COLOR color[256];
};

#pragma pack(pop)

// Info
// Pallette size
#define TX_PALETTE_SIZE sizeof(TX_PALETTE)
// Start pointer into pallette
#define TX_PNT_PALETTE sizeof(TX_FILE_HEADER)
// Start pointer into first mip level
// without pallette
#define TX_PNT_FMIP_WO_PAL TX_PNT_PALETTE
// with pallette
#define TX_PNT_FMIP_W_PAL (TX_PNT_PALETTE + TX_PALETTE_SIZE)
