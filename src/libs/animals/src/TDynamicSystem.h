#pragma once

#include <vector>
#include <memory>

#include "TDynamicObject.h"

#define MAX_DYNAMIC_OBJECTS 150

//--------------------------------------------------------------------
class TDynamicSystem
{
  public:
    TDynamicSystem();
    virtual ~TDynamicSystem() = default;

    bool AddAttractor(TDynamicObject* _obj);
    bool AddDeflector(TDynamicObject* _obj);
    void Recalculate();

    // void Draw(HDC _dc);

  protected:
    std::vector<TDynamicObject *> attractors;
    std::vector<TDynamicObject *> deflectors;
    //TDynamicObject *attractors[MAX_DYNAMIC_OBJECTS];
    //TDynamicObject *deflectors[MAX_DYNAMIC_OBJECTS];
};

//--------------------------------------------------------------------
