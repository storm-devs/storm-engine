#pragma once

struct CVECTOR4
{
  public:
    union {
        struct
        {
            float x, y, z, w;
        };

        float v[4];
    };

    CVECTOR4(){};

    CVECTOR4(float a)
    {
        x = a;
        y = a;
        z = a;
        w = 1.0f;
    };

    CVECTOR4(float _x, float _y, float _z, float _w)
    {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    };

    CVECTOR4(float a[4])
    {
        x = a[0];
        y = a[1];
        z = a[2];
        w = a[3];
    };
};
