#pragma once

#include "collide.h"
#include "dx9render.h"
#include "model.h"
#include "vma.hpp"

class Shadow : public Entity
{
    VDX9RENDER *rs;
    COLLIDE *col;
    void FindPlanes(const CMatrix &view, const CMatrix &proj);
    PLANE planes[6];
    entid_t entity;
    void Smooth();
    float shading;
    uint32_t blendValue;

  public:
#define SHADOW_FVF (D3DFVF_XYZ | D3DFVF_TEXTUREFORMAT2 | D3DFVF_TEX1)

    struct SHADOW_VERTEX
    {
        CVECTOR pos;
        float tu, tv;
    };

    Shadow();
    ~Shadow() override;
    bool Init() override;
    void Realize(uint32_t Delta_Time);
    uint64_t ProcessMessage(MESSAGE &message) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
            // case Stage::execute:
            //    Execute(delta); break;
        case Stage::realize:
            Realize(delta);
            break;
        case Stage::lost_render:
            LostRender();
            break;
        case Stage::restore_render:
            RestoreRender();
            break;
        }
    }

    void LostRender();
    void RestoreRender();
};
