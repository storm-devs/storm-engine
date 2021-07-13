//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmRenderObject
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "attributes.h"
#include "dx9render.h"

class WdmRenderObject
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmRenderObject();
    virtual ~WdmRenderObject();

    // Calculations
    virtual void Update(float dltTime){};
    // Rendering
    virtual void PRender(VDX9RENDER *rs){};

    virtual void MRender(VDX9RENDER *rs){};

    virtual void LRender(VDX9RENDER *rs){};

    static void DeleteAllObjects();

    bool killMe;
    bool isEnablePause;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    WdmRenderObject *prevObject;
    WdmRenderObject *nextObject;
    static WdmRenderObject *firstObject;
};
