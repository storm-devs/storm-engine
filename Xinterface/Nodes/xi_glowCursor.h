#ifndef __XI_GLOWCURSOR_H__
#define __XI_GLOWCURSOR_H__

#include "..\\inode.h"

// picture
class CXI_GLOWCURSOR : public CINODE
{
public:
	CXI_GLOWCURSOR();
	~CXI_GLOWCURSOR();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode) {return 0;}
	bool	IsClick(int buttonID,long xPos,long yPos) {return false;}
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	SetRectanglesToPosition(XYRECT & rectXY);

	bool	m_bShowGlow;
	bool	m_bGlowToBack;

	bool	m_bUseBlind;
    DWORD	m_dwFoneColor;
	DWORD	m_dwBlindColor;
	DWORD	m_dwCurColor;

	bool	m_bUpBlind;
	float	m_fCurM;
	float	m_fCurM_UpSpeed;
	float	m_fCurM_DownSpeed;

    float	m_xOffset,m_yOffset;

	long	m_idBackTex;
    CVideoTexture *m_pBackTex;

    XI_ONETEX_VERTEX	m_pTexVert[14];

	CINODE * m_pPrevNode;
};

#endif
