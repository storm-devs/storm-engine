#pragma once

//#include <windows.h>

#pragma pack(push)
#pragma pack(1)

#ifdef SQR
#undef SQR
#endif

#define SQR(x) ((x) * (x))

typedef struct
{
    float x;
    float y;
    float z;
} VECTOR;

typedef struct
{
    float x;
    float y;
    float z;
    float w;
} VECTOR4;

typedef struct
{
    float Nx;
    float Ny;
    float Nz;
    float D;
} PLANE;

typedef struct
{
    float cx;
    float cy;
    float cz;
    float R;
} CHECK_SPHERE;

typedef struct
{
    float r;
    float g;
    float b;
} COLOR;

typedef struct
{
    float r;
    float g;
    float b;
    float a;
} COLORA;

typedef struct
{
    union {
        float matrix[16];
        float m[4][4];
    };
} MATRIX;

#pragma pack(pop)
