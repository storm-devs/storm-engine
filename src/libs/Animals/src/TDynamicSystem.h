#pragma once

#include "TDynamicObject.h"

#define MAX_DYNAMIC_OBJECTS 150

//--------------------------------------------------------------------
class TDynamicSystem
{
  public:
    TDynamicSystem();
    virtual ~TDynamicSystem();

    bool AddAttractor(TDynamicObject *_obj);
    bool AddDeflector(TDynamicObject *_obj);
    void Recalculate();

    // void Draw(HDC _dc);

  protected:
    TDynamicObject *attractors[MAX_DYNAMIC_OBJECTS];
    TDynamicObject *deflectors[MAX_DYNAMIC_OBJECTS];
};

//--------------------------------------------------------------------
