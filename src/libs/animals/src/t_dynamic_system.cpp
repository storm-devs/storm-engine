#include "t_dynamic_system.h"
#include <algorithm>
#include <memory.h>

//--------------------------------------------------------------------
TDynamicSystem::TDynamicSystem()
{
    std::fill(attractors, attractors + MAX_DYNAMIC_OBJECTS, nullptr);
    std::fill(deflectors, deflectors + MAX_DYNAMIC_OBJECTS, nullptr);
}

//--------------------------------------------------------------------
TDynamicSystem::~TDynamicSystem()
{
}

//--------------------------------------------------------------------
bool TDynamicSystem::AddAttractor(TDynamicObject *_obj)
{
    for (auto i = 0; i < MAX_DYNAMIC_OBJECTS; i++)
    {
        if (!attractors[i])
        {
            attractors[i] = _obj;
            return true;
        }
    }

    return false;
}

//--------------------------------------------------------------------
bool TDynamicSystem::AddDeflector(TDynamicObject *_obj)
{
    for (auto i = 0; i < MAX_DYNAMIC_OBJECTS; i++)
    {
        if (!deflectors[i])
        {
            deflectors[i] = _obj;
            return true;
        }
    }

    return false;
}

//--------------------------------------------------------------------
/*
void TDynamicSystem::Draw(HDC _dc)
{
    static const DWORD ATTRACTOR_COLOR  = (DWORD) RGB(0xFF, 0, 0);
    static const DWORD DEFLECTOR_COLOR  = (DWORD) RGB(0, 0, 0xFF);
    static const DWORD ATTRACTOR_COLOR2 = (DWORD) RGB(0x7F, 0, 0);
    static const DWORD DEFLECTOR_COLOR2 = (DWORD) RGB(0, 0, 0x7F);

    for (int i = 0; i<1; i++)
    {
        if (attractors[i])
            attractors[i]->Draw(_dc, ATTRACTOR_COLOR2);
        if (deflectors[i])
            deflectors[i]->Draw(_dc, DEFLECTOR_COLOR2);
    }
    for (i = 1; i<MAX_DYNAMIC_OBJECTS; i++)
    {
        if (attractors[i])
            attractors[i]->Draw(_dc, ATTRACTOR_COLOR);
        if (deflectors[i])
            deflectors[i]->Draw(_dc, DEFLECTOR_COLOR);
    }
}
*/
//--------------------------------------------------------------------
void TDynamicSystem::Recalculate()
{
    for (auto i = 1; i < MAX_DYNAMIC_OBJECTS; i++)
    {
        if (attractors[i])
        {
            attractors[i]->Calculate(attractors, MAX_DYNAMIC_OBJECTS, deflectors, MAX_DYNAMIC_OBJECTS);
        }

        if (deflectors[i])
        {
            deflectors[i]->Calculate(attractors, MAX_DYNAMIC_OBJECTS, deflectors, MAX_DYNAMIC_OBJECTS);
        }
    }
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
