//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Pillar
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "vma.hpp"

#define TRND_PI 3.141592654f

struct CVECTOR;

class Pillar
{
    // Tornado pillar section
    struct Section
    {
        float radius; // Section radius
        float alpha;  // Section transparency
        float x, z;   // Offset relative to tornado position
        float y;      // Height position
    };

    // Segment direction vector
    struct Segment
    {
        float x, z;
    };

  public:
    struct Vertex
    {
        float x, y, z;
        uint32_t color;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Pillar();
    virtual ~Pillar();

    void Update(float dltTime);
    void FillVertexBuffer(Vertex *buffer);
    void FillIndexBuffer(uint16_t *buffer);

    int32_t GetNumVerteces();
    int32_t GetNumTriangles();

    float GetHeight();
    float GetKHeight(float y);
    float GetRaduis(float y);
    float GetX(float y);
    float GetZ(float y);
    float X();
    float Z();

    float RandomPos(CVECTOR &pos);

    void SetGlobalAlpha(float a);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    Section section[24];
    Segment segment[16];
    float x, z;
    float dx, dz;
    float phaseX, phaseZ;
    float phaseX1, phaseZ1;
    float phaseX2, phaseZ2;
    float phaseX3, phaseZ3;
    float galpha;
};

inline float Pillar::X()
{
    return x;
}

inline float Pillar::Z()
{
    return z;
}
