//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmRenderModel
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "wdm_objects.h"
#include "wdm_render_object.h"

#include "matrix.h"

class GEOS;
class VGEOMETRY;

class WdmRenderModel : public WdmRenderObject
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmRenderModel();
    ~WdmRenderModel() override;

    virtual bool Load(const char *modelName);

    void PRender(VDX9RENDER *rs) override;
    void MRender(VDX9RENDER *rs) override;
    void LRender(VDX9RENDER *rs) override;

    void SetTech(const char *t, const char *ta);
    int32_t GetTexture(int32_t stage) const;
    void SetTexture(int32_t stage, int32_t id) const;

    CMatrix mtx;
    float alpha;

  public:
    void Render(VDX9RENDER *rs) const;

    GEOS *geo;
    const char *tech;
    const char *techa;

    // Positioned circle
    CVECTOR center;
    float radius;

    bool drawCircle;
};
