#ifndef _GI_EDITOR_H
#define _GI_EDITOR_H

#include "inode.h"

class XINTERFACE;
class CXI_IMAGE;

class GIEditor
{
public:
	GIEditor(XINTERFACE* pInterface);
	~GIEditor();

	void Release();

	void Render();
	bool MakeControl();

	bool IsShowMode();
	void MakeShowMode(bool bShow);

	void SetEditNode(CINODE* pNode);

protected:
	XINTERFACE* m_pGIOwner;
	bool m_bShowMode;
	CINODE* m_pEditableNode;
	float m_fLastKeyPressTime;

	CXI_IMAGE* m_pImageBack;
};

#endif
