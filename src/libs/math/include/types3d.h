#pragma once

#pragma pack(push)
#pragma pack(1)

#ifdef SQR
#undef SQR
#endif

#define SQR(x) ((x) * (x))

using VECTOR = struct
{
    float x;
    float y;
    float z;
};

using VECTOR4 = struct
{
    float x;
    float y;
    float z;
    float w;
};

using PLANE = struct
{
    float Nx;
    float Ny;
    float Nz;
    float D;
};

using CHECK_SPHERE = struct
{
    float cx;
    float cy;
    float cz;
    float R;
};

using COLOR = struct
{
    float r;
    float g;
    float b;
};

using COLORA = struct
{
    float r;
    float g;
    float b;
    float a;
};

using MATRIX = struct
{
    union {
        float matrix[16];
        float m[4][4];
    };
};

#pragma pack(pop)
