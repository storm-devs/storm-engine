#ifndef _INTERFACE_EDITOR_H_
#define _INTERFACE_EDITOR_H_

#include "..\common_h\dx8render.h"
#include "..\common_h\vmodule_api.h"
#include "..\xinterface\xinterface.h"

class EditorManager;

class InterfaceEditor : public ENTITY
{
public:
	InterfaceEditor();
	~InterfaceEditor();

	virtual bool Init();
	virtual dword _cdecl ProcessMessage( MESSAGE& message );
	virtual void Realize( dword dwDeltaTime );

protected:
	VDX8RENDER* m_pRender;
	XINTERFACE* m_pInterface;

	EditorManager* m_pManager;
};

#endif
