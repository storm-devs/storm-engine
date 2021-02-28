//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmWindRose
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _WdmWindRose_H_
#define _WdmWindRose_H_

#include "WdmRenderModel.h"

class WdmWindRose : public WdmRenderModel
{
    struct Vertex
    {
        float x, y, z;
        uint32_t c;
        float tu, tv;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmWindRose();
    virtual ~WdmWindRose();

    // Calculations
    void Update(float dltTime) override;

    void PRender(VDX9RENDER *rs) override{};

    void MRender(VDX9RENDER *rs) override{};
    void LRender(VDX9RENDER *rs) override;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    long shadowTexture;
};

#endif
