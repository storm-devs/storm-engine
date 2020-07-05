#ifndef __XI_BUTTON_H__
#define __XI_BUTTON_H__

#include "..\\inode.h"

// picture
class CXI_BUTTON : public CINODE
{
public:
	CXI_BUTTON();
	~CXI_BUTTON();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	void	NotUsingTime(dword Delta_Time) {nPressedDelay=0;}
	void	SetUsing(bool bUsing);
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);

protected:
	char    *m_sGroupName;
	long	m_idTex; // texture identity
	CVideoTexture *m_pTex;

	FXYRECT m_tRect;

	DWORD	m_argbDisableColor;
	DWORD   m_dwShadowColor;
	DWORD   m_dwFaceColor;
	DWORD	m_dwFontColor;
	DWORD   m_dwLightColor;
	DWORD   m_dwDarkColor;
	float	m_fBlindSpeed;
	float	m_fCurBlind;
	bool	m_bUpBlind;

	float   fXShadow;
	float   fYShadow;
	float   fXShadowPress;
	float   fYShadowPress;

	float   fXDeltaPress;
	float   fYDeltaPress;

	int		nPressedDelay;
	int     nMaxDelay;

	int		m_nFontNum;
	long	m_idString;
	int		m_dwStrOffset;
};

#endif
