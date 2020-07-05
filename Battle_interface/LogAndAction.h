#ifndef __LOG_AND_ACTION_H_
#define __LOG_AND_ACTION_H_

#include "defines.h"
#include "utils.h"

class ILogAndActions : public ENTITY
{
	VDX8RENDER *rs;
	struct STRING_DESCR
	{
		char *	str;
		float	offset;
		float	alpha;
		STRING_DESCR * next;
	};
public:
	ILogAndActions();
	~ILogAndActions();
	bool Init();
	void Execute(dword delta_time);
	void Realize(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE & message);

protected:
	void Create(bool bFastComShow, bool bLogStringShow);
	void ActionChange(bool bFastComShow, bool bLogStringShow);
	void Release();

	void SetString(char * str, bool immortal);
	void SetAction(char * actionName);

protected:
	// log stings parameters
	//-----------------------
	long	m_nWindowWidth; // размер окна по X
	long	m_nWindowHeight; // размер окна по Y
	long	m_nWindowLeft;
	long	m_nWindowRight;
	long	m_nWindowUp;
	long	m_fontID;
	float	m_fFontScale;
	DWORD	m_dwColor;
	long	m_nStringBegin;
	long	m_nStringOffset;
	float   m_fShiftSpeed;
	float	m_fBlendSpeed;
	STRING_DESCR * m_sRoot;

	// Action parameters
	//-------------------
	bool	m_bThatRealAction;
	long	m_nIconWidth; // размер иконки по X
	long	m_nIconHeight; // размер иконки по Y
	long	m_nIconLeft;
	long	m_nIconUp;
	long	m_idIconTexture;
	long	m_horzDiv;
	long	m_vertDiv;
	BI_ONETEXTURE_VERTEX	m_IconVertex[4];
	char	m_sActionName[64];
	char	m_sOldActionName[64];

	bool	m_bShowActiveCommand;
	bool	m_bShowLogStrings;
	bool	m_bDontShowAll;

	long	m_nTimeCounter;

	BITextInfo m_ActionHint1;
	BITextInfo m_ActionHint2;
};

#endif
