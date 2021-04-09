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

#include "WdmRenderModel.h"

#define WMD_NUM_SKYS 8

class WdmCounter : public WdmRenderModel
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmCounter();
    virtual ~WdmCounter();

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

    long skytx[WMD_NUM_SKYS];
    long lastSkys[2];
    long skyseq[8];
    long dayCounter;
    long skyCounter;

    static const char *skytex[WMD_NUM_SKYS];
};
