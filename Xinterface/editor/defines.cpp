#include "defines.h"



void GIEditorObject::LinkEvent( GIEditorEventHandler* pEventHandler, const GIEditorEvent& pEventFunction )
{
	pEventHandler->AddEventFunction( this, pEventFunction );
}



GIEditorEventHandler::GIEditorEventHandler() :
	m_aEventFuncs( _FL )
{
}

GIEditorEventHandler::~GIEditorEventHandler()
{
	m_aEventFuncs.DelAll();
}

bool GIEditorEventHandler::Execute()
{
	for( long n=0; n<m_aEventFuncs; n++ )
	{
		(m_aEventFuncs[n].pObj->*m_aEventFuncs[n].func)();
	}
	return true;
}

void GIEditorEventHandler::AddEventFunction( GIEditorObject* pObj, const GIEditorEvent& pEventFunction )
{
	FuncDescr fd;
	fd.pObj = pObj;
	fd.func = pEventFunction;
	m_aEventFuncs.Add( fd );
}
