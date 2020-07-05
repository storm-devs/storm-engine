#ifndef _XI_TWOPICTURE_H_
#define _XI_TWOPICTURE_H_

#include "..\\inode.h"

// video
class CXI_TWOPICTURE : public CINODE
{
public:
	CXI_TWOPICTURE();
	~CXI_TWOPICTURE();
	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY);

	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();

	void	SetNewPicture(char * sNewTexName);

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	UpdateRectangles();

	bool m_bMouseInsideIndifferent;

    long	m_idOneTex;
	long	m_idTwoTex;

	FXYPOINT	m_picSize;
	FXYPOINT	m_leftPicCenter;
	FXYPOINT	m_rightPicCenter;

	DWORD	m_dwDarkColor;
	DWORD	m_dwSelectColor;
	DWORD	m_dwShadowColor;

	bool	m_bLeftSelect;
	bool	m_leftClick;

	FXYPOINT	m_ShadowOffset;
	FXYPOINT	m_PressShadowOffset;
	FXYPOINT	m_PressOffset;

	XI_ONETEX_VERTEX m_vOne[4];
	XI_ONETEX_VERTEX m_vTwo[4];
	XI_NOTEX_VERTEX	 m_vSOne[4];
	XI_NOTEX_VERTEX	 m_vSTwo[4];
};

#endif
