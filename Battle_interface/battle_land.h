#ifndef _BATTLE_LAND_H_
#define _BATTLE_LAND_H_

#include "..\common_h\dx8render.h"
#include "..\common_h\defines.h"
#include "battle_command.h"

class BATTLE_LAND_INTERFACE : public ENTITY
{
	VDX8RENDER	*rs;
	BATTLE_COMMAND	*m_pCommandMenu;
	MESSAGE_ICONS	*m_pMessageIcons;

public:
	BATTLE_LAND_INTERFACE();
	~BATTLE_LAND_INTERFACE();
	bool Init();
	void Execute(dword delta_time);
	void Realize(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE & message);

protected:
	bool	m_bShowCommandos;

	bool	m_bEnableCommand;
	int		m_nShowCommand;

	long	m_idVBufCommandos;
	long	m_idIBufCommandos;
	long	m_idVBufHealth;
	long	m_idIBufHealth;
	long	m_idIBufGunShoot;

	long	m_left,m_dist,m_width;
	long	m_leftA,m_widthA;
	float	m_fTexAOff;

	long	m_idShadowTexture;
	long	m_idIconTexture[4];
	long	m_idStateTexture;
	long	m_idAlarmTexture;
	long	m_idGunChargeTexture;

	DWORD	m_argbAlarmLightColor;
	DWORD	m_argbAlarmDarkColor;
	DWORD	m_argbPoisonLightColor;
	DWORD	m_argbPoisonDarkColor;
	bool	m_bAlarmUp;
	float	m_fAlarmBlindUpSpeed;
	float	m_fAlarmBlindDownSpeed;
	float	m_fCurAlarmBlind;

	int		m_nMaxCommandos;
	int		m_nCommandos;
	int		m_nGunCharge;

	int		m_nHQ,m_nVQ;

protected:
	void SetShowParameters();
	void SetParameters();
	void UpdateCommandos();
	void UpdateAlarm();
	void EndShow();
	void Release();
	void EnableMessageIcons(VDATA * pvdat);
	void DisableMessageIcons();
};

#endif
