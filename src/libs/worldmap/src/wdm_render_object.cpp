//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmRenderObject
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "wdm_render_object.h"
#include "storm_assert.h"

WdmRenderObject *WdmRenderObject::firstObject = nullptr;

// ============================================================================================
// Construction, destruction
// ============================================================================================

WdmRenderObject::WdmRenderObject()
{
    killMe = false;
    isEnablePause = true;
    if (firstObject)
    {
        for (prevObject = firstObject; prevObject->nextObject; prevObject = prevObject->nextObject)
            ;
        prevObject->nextObject = this;
        nextObject = nullptr;
    }
    else
    {
        firstObject = this;
        prevObject = nullptr;
        nextObject = nullptr;
    }
}

WdmRenderObject::~WdmRenderObject()
{
    if (prevObject == nullptr)
    {
        Assert(WdmRenderObject::firstObject == this);
        firstObject = nextObject;
        if (nextObject)
            nextObject->prevObject = nullptr;
    }
    else
    {
        Assert(WdmRenderObject::firstObject != this);
        prevObject->nextObject = nextObject;
        if (nextObject)
            nextObject->prevObject = prevObject;
    }
}

void WdmRenderObject::DeleteAllObjects()
{
    while (firstObject)
        delete firstObject;
}
