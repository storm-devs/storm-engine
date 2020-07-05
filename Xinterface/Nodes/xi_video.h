#ifndef _XI_VIDEO_H
#define _XI_VIDEO_H

#include "..\\inode.h"

// video
class CXI_VIDEO : public CINODE
{
public:
	CXI_VIDEO();
	~CXI_VIDEO();
	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);

    CVideoTexture *pTex;
	DWORD	m_dwColor;
    FXYRECT m_rectTex;
};

#endif
