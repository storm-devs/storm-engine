#ifndef InterfaceManager_h
#define InterfaceManager_h

#include "BaseManager.h"
#include "BaseManager.h"

class BI_BaseGroup;

class BI_InterfaceManager : public BI_ManagerBase
{
public:
	BI_InterfaceManager();
	~BI_InterfaceManager();

	bool Init();
	void Execute(dword delta_time);
	void Realize(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE & message);

	virtual BI_ManagerNodeBase* CreateImageNode(const char* texture, const FRECT& uv, const RECT& pos, dword color, long nPrioritet);
	virtual BI_ManagerNodeBase* CreateStringNode(const char* text, const char* font, dword color, float scale, const RECT& pos, long nHAlign, long nVAlign, long prioritet);
	virtual void DeleteNode(BI_ManagerNodeBase* pNod);

protected:
	long MsgLoadSheet(MESSAGE & message);
	long MsgCreateImage(MESSAGE & message);
	long MsgCreateString(MESSAGE & message);
	long MsgDeleteNode(MESSAGE & message);
	long MsgEvent(MESSAGE & message);

	array<BI_ManagerNodeBase*> m_aNodes;

	BI_BaseGroup* m_pInterfaceSheet;
};

#endif
