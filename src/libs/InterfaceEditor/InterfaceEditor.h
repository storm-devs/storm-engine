#ifndef _INTERFACE_EDITOR_H_
#define _INTERFACE_EDITOR_H_

#include "..\xinterface\xinterface.h"
#include "dx8render.h"
#include "vmodule_api.h"

class EditorManager;

class InterfaceEditor : public ENTITY
{
  public:
    InterfaceEditor();
    ~InterfaceEditor();

    virtual bool Init();
    virtual dword _cdecl ProcessMessage(MESSAGE &message);
    virtual void Realize(dword dwDeltaTime);

  protected:
    VDX8RENDER *m_pRender;
    XINTERFACE *m_pInterface;

    EditorManager *m_pManager;
};

#endif
