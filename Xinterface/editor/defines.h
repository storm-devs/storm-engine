#ifndef _GI_EDITOR_DEFINES_H_
#define _GI_EDITOR_DEFINES_H_

#include "..\..\common_h\vmodule_api.h"
#include "..\..\common_h\templates\array.h"

enum GIEditorStates
{
	GIState_Nothing = 0,

	GIState_ListChange = 1,

	GIState_ForcedDword = 0xFFFFFFFF
};

class GIEditorEventHandler;
class GIEditorObject;

typedef void (_cdecl GIEditorObject::*GIEditorEvent)();


class GIEditorObject
{
public:
	virtual ~GIEditorObject(){}

public:
	void LinkEvent( GIEditorEventHandler* pEventHandler, const GIEditorEvent& pEventFunction );
};

class GIEditorEventHandler
{
public:
	GIEditorEventHandler();
	~GIEditorEventHandler();

	bool Execute();

	void AddEventFunction( GIEditorObject* pObj, const GIEditorEvent& pEventFunction );

protected:
	struct FuncDescr {
		GIEditorObject* pObj;
		GIEditorEvent func;
	};
	array<FuncDescr> m_aEventFuncs;
};

#endif
