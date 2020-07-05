#ifndef __BATTLE_INTERFACE_H_
#define __BATTLE_INTERFACE_H_

#include "..\common_h\dx8render.h"
#include "..\common_h\defines.h"
#include "battle_command.h"
#include "battle_navigator.h"

class BIShipIcon;

class BATTLE_INTERFACE : public ENTITY
{
	VDX8RENDER *rs;

public:
	BATTLE_INTERFACE();
	~BATTLE_INTERFACE();
	bool Init();
	void Execute(dword delta_time);
	void Realize(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE & message);
	dword AttributeChanged(ATTRIBUTES * pAttr);

protected:
	//BATTLE_COMMAND		CommandMenu; // иконки команд и целеуказаний в боевом меню
	BATTLE_NAVIGATOR	BattleNavigator; // навигационные иконки
	MESSAGE_ICONS *		m_pMessageIcons;
	BIShipIcon*			m_pShipIcon;
	bool	m_bShowCommandMenu;
	bool	m_bShowBattleNavigator;
	bool	m_bYesShowAll;
	bool	m_bVisible;

	float	m_fBlinkSpeed;
	float	m_fCurBlinkTime;

	bool	m_bNeedIslandSet;

	bool	m_bMyShipView;

protected:
	void LoadIniFile();
	void CheckSeaState();
	void EnableMessageIcons(VDATA * pvdat);
};

#endif