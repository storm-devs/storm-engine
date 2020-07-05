#ifndef __XI_LRCHANGER_H__
#define __XI_LRCHANGER_H__

#include "..\\inode.h"

// picture
class CXI_LRCHANGER : public CINODE
{
public:
	CXI_LRCHANGER();
	~CXI_LRCHANGER();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	long	GetClickState();

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);

protected:
	char    *m_sGroupName;
	long	m_idTex; // texture identity

	FXYRECT m_tLRect;
	FXYRECT m_tRRect;
	FXYRECT m_posLRect;
	FXYRECT m_posRRect;

	DWORD   m_dwShadowColor;
	DWORD   m_dwFaceColor;
	long	m_dwLightSelCol;
	long	m_dwDarkSelCol;

	long	m_dwBlindDelay;
	long	m_dwCurBlindState;
	bool	m_bBlindIncrement;

	FXYPOINT m_ShadowShift;
	FXYPOINT m_PressShadowShift;
	FXYPOINT m_PressShift;

	int		nPressedDelay;
	int     nMaxDelay;

	bool	m_bLeftPress;
};

#endif
