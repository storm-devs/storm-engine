#include "TDynamicSystem.h"
#include <algorithm>
#include <memory.h>

//--------------------------------------------------------------------
TDynamicSystem::TDynamicSystem()
{
    attractors.reserve(MAX_DYNAMIC_OBJECTS);
    deflectors.reserve(MAX_DYNAMIC_OBJECTS);
}

//--------------------------------------------------------------------
bool TDynamicSystem::AddAttractor(TDynamicObject *_obj)
{
    if (attractors.size() < MAX_DYNAMIC_OBJECTS)
    {
        attractors.push_back(_obj);
        return true;
    }
    return false;
}

//--------------------------------------------------------------------
bool TDynamicSystem::AddDeflector(TDynamicObject *_obj)
{
    if (deflectors.size() < MAX_DYNAMIC_OBJECTS)
    {
        deflectors.push_back(_obj);
        return true;
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
    // Why does this start at 1 rather than 0?
    for (auto i = 1; i < MAX_DYNAMIC_OBJECTS; i++)
    {
        if (attractors[i])
        {
            attractors[i]->Calculate(attractors, deflectors);
        }
        if (deflectors[i])
        {
            deflectors[i]->Calculate(attractors, deflectors);
        }
    }
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
