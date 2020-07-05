#ifndef _BATTLE_LAND_H_
#define _BATTLE_LAND_H_

#include "..\..\common_h\dx8render.h"
#include "..\..\common_h\defines.h"
#include "..\utils.h"

class BIManSign;

class BATTLE_LAND_INTERFACE : public ENTITY
{
	// metods
public:
	BATTLE_LAND_INTERFACE();
	~BATTLE_LAND_INTERFACE();
	bool Init();
	void Execute(dword delta_time);
	void Realize(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE & message);

protected:
	void SetShowParameters();
	void SetParameters();
	void UpdateCommandos();
	void UpdateAlarm();
	void EndShow();
	void Release();
	void EnableMessageIcons(VDATA * pvdat);
	void DisableMessageIcons();
	void SetTextData();

	// data
protected:
	VDX8RENDER* m_pRS;
	bool m_bShowCommandos;

	BIManSign* m_pManSign;

	array<BITextInfo> m_TextInfo;

	BIImagesInfo m_Images;
};

#endif
