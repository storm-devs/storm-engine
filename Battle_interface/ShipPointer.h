#ifndef _SHIPPOINTER_H_
#define _SHIPPOINTER_H_

#include "..\common_h\dx8render.h"
#include "..\common_h\defines.h"
#include "..\common_h\sd2_h\VAI_ObjBase.h"

class SHIPPOINTER : public ENTITY
{
	VDX8RENDER *rs;

public:
	SHIPPOINTER();
	~SHIPPOINTER();
	bool Init();
	void Execute(dword delta_time);
	void Realize(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE & message);

protected:
	void UpdateShipPointer();
	VAI_OBJBASE * FindShipByChrIndex(long chrIdx);

	bool	m_bVisible;
	bool	m_bFriend;

	long	m_idFriendTex;
	long	m_idEnemyTex;

	long	m_idVBuf;

	float	m_fShiftVal;
	float	m_fShiftSpeed;
	float	m_fShiftAmp;
	float	m_fShiftWSize;
	float	m_fShiftHSize;
	float	m_fShiftTop;

	VAI_OBJBASE * m_pShip;
};

#endif
