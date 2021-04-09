#pragma once

typedef struct tagTGA_H
{
    uint8_t byte1; // = 0
    uint8_t byte2; // = 0
    uint8_t type;
    uint8_t byte4_9[9]; // = 0
    uint16_t width;
    uint16_t height;
    uint8_t bpp; // bit per pixel
    union {
        uint8_t attr8;

        struct
        {
            uint8_t attr : 4;
            uint8_t rez : 1;
            uint8_t origin : 1;
            uint8_t storage : 2;
        };
    };
} TGA_H;
