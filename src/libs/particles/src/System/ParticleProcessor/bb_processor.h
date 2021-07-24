//****************************************************************
//*
//*  Author : Sergey Makeev aka Joker, 2003
//*
//*
//****************************************************************

#pragma once

#include "dx9render.h"
#include "math3d/Matrix.h"
#include "particles/gmx_QSort.h"

#include "../../ICommon/Particle.h"
#include "../DataSource/FieldList.h"

class ParticleSystem;

class BillBoardProcessor
{
    static IDirect3DVertexDeclaration9 *vertexDecl_;
    void CreateVertexDeclaration() const;

    struct RECT_VERTEX
    {
        Vector vRelativePos;
        uint32_t dwColor;
        float tu1, tv1;
        float tu2, tv2;
        float angle;
        float BlendK;
        Vector vParticlePos;
        float AddPowerK;
    };

    VDX9RENDER *pRS;
    // Buffers for rendering billboards
    long pVBuffer;
    long pIBuffer;

    struct MemArrayItem
    {
        BB_ParticleData pData;
        bool Free;

        MemArrayItem()
        {
            Free = true;
        }
    };

    MemArrayItem *pMemArray;

    std::vector<BB_ParticleData *> Particles;

    GMXQSort<BB_ParticleData *> ParticleSorter;

    // Counts distance to billboards
    uint32_t CalcDistanceToCamera();

    // Compare function when sorting
    static BOOL CompareFunction(BB_ParticleData *e1, BB_ParticleData *e2);

    BB_ParticleData *AllocParticle() const;
    void FreeParticle(BB_ParticleData *pItem) const;

  public:
    BillBoardProcessor();
    ~BillBoardProcessor();

    void AddParticle(ParticleSystem *pSystem, const Vector &velocity_dir, const Vector &pos, const Matrix &matWorld,
                     float EmitterTime, float EmitterLifeTime, FieldList *pFields, uint32_t *pActiveCount,
                     uint32_t dwGUID);

    void Process(float DeltaTime);
    void Draw();

    uint32_t GetCount() const;

    void DeleteWithGUID(uint32_t dwGUID, uint32_t GUIDRange = GUIDSTEP);

    void Clear();
};
