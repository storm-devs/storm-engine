//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmCounter
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "wdm_render_model.h"

#define WMD_NUM_SKYS 8

class WdmCounter : public WdmRenderModel
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmCounter();
    ~WdmCounter() override;

    bool Init();

    // Calculations
    void Update(float dltTime) override;

    void PRender(VDX9RENDER *rs) override{};

    void MRender(VDX9RENDER *rs) override{};
    void LRender(VDX9RENDER *rs) override;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    bool LoadModel(WdmRenderModel *&pnt, const char *name, const char *tech) const;
    void DrawNum(VDX9RENDER *rs, WdmRenderModel *m, float u, float v);

  private:
    WdmRenderModel *sky;
    WdmRenderModel *d[2];
    WdmRenderModel *m[2];
    WdmRenderModel *y[4];

    int32_t skytx[WMD_NUM_SKYS];
    int32_t lastSkys[2];
    int32_t skyseq[8];
    int32_t dayCounter;
    int32_t skyCounter;

    static const char *skytex[WMD_NUM_SKYS];
};
