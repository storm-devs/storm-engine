//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmRenderObject
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "WdmRenderObject.h"

WdmRenderObject * WdmRenderObject::firstObject = null;

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmRenderObject::WdmRenderObject()
{
	killMe = false;
	isEnablePause = true;
	if(firstObject)
	{
		for(prevObject = firstObject; prevObject->nextObject; prevObject = prevObject->nextObject);
		prevObject->nextObject = this;
		nextObject = null;
	}else{
		firstObject = this;
		prevObject = null;
		nextObject = null;
	}
}

WdmRenderObject::~WdmRenderObject()
{
	if(prevObject == null)
	{
		Assert(WdmRenderObject::firstObject == this);
		WdmRenderObject::firstObject = nextObject;
		if(nextObject) nextObject->prevObject = null;
	}else{
		Assert(WdmRenderObject::firstObject != this);
		prevObject->nextObject = nextObject;
		if(nextObject) nextObject->prevObject = prevObject;
	}
}

void WdmRenderObject::DeleteAllObjects()
{
	while(firstObject) delete firstObject;
}