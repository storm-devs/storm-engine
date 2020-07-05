#ifndef _SPYGLASS_H_
#define _SPYGLASS_H_

#include "..\common_h\dx8render.h"
#include "..\common_h\defines.h"

class ISPYGLASS : public ENTITY
{
	VDX8RENDER *rs;

public:
	ISPYGLASS();
	~ISPYGLASS();
	bool Init();
	void Execute(dword delta_time);
	void Realize(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE & message);

protected:
	bool	m_bVisible;

	long	m_idBackTex;
	long	m_idIconTex;
	long	m_idNationsTex;

	FRECT	m_frChargeUV[4];
	long	m_nationQuantity;
	FRECT	*m_frNationUV;

	// text parameters
	long	m_idFontOne;		// фонт для имени и типа корабля
	float	m_fFontOneScale;	// масштаб фонта имени и типа корабля
	float	m_fShipNameScale;	// масштаб для имени с учетом длинны слова
	float	m_fShipTypeScale;	// масштаб для типа с учетом длинны слова
	float	m_fStringMaxWidth;	// размер поля под надпись о типе и имени корабля
	long	m_idFontTwo;		// фонт для цифровых данных
	float	m_fFontTwoScale;	// масштаб фонта цифровых данных

	char *	m_sShipName;
	char *	m_sShipType;
	long	m_nShipHull;
	long	m_nShipSail;
	long	m_nShipCrew;
	float	m_fShipSpeed;
	float	m_fSailTo;     //boal
	long	m_nShipCannons;
	long	m_nShipCharge;
	long	m_nShipNation;

	// buffers
	long	m_idVBuf;
	long	m_idIBuf;
	long	m_nPrimitiveQuantity;

	// positions
	long	m_nLeftTextShipData;
	long	m_nTopTextShipName;
	long	m_nTopTextShipType;
	long	m_nTopDigitData;
	long	m_nLeftTextPos[7];

	float	m_fFadeSpeed;
	float	m_fCurAlpha;
	bool	m_bFadeOut;

protected:                         
	bool	SetParameters();
	void	AllRelease();

	void	Update(long chargeType);
	void	SetVisible(bool vflag);
};

#endif
