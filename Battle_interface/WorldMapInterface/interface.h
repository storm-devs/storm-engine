#ifndef _WM_INTERFACE_H_
#define _WM_INTERFACE_H_

#include "..\..\common_h\dx8render.h"
#include "..\..\common_h\defines.h"
#include "..\utils.h"
#include "..\..\common_h\templates\array.h"

class WMShipIcon;
class WMShipCommandList;

class WM_INTERFACE : public ENTITY
{
	VDX8RENDER *rs;

public:
	WM_INTERFACE();
	~WM_INTERFACE();
	bool Init();
	void Realize(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE & message);
	dword AttributeChanged(ATTRIBUTES * pAttr);

protected:
	WMShipIcon* m_pShipIcon; // иконки команд и целеуказаний в боевом меню
	WMShipCommandList* m_pCommandList;
	bool m_bVisible;
	long m_nCommandListVerticalOffset;
	long m_nMainCharIndex;
	long m_nCommandMode;

protected:
	void LoadIniFile();

	void MakeControl();
	void ExecuteCommand( long command );
	void UpdateCommandList();
	long GetCurrentCommandTopLine();
	long GetCurrentCommandCharacterIndex();
	long GetCurrentCommandMode();

	bool IsCommandMenuActive();
};

#endif