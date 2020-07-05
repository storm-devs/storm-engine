#ifndef _XI_VIDEORECT_H
#define _XI_VIDEORECT_H

#include "..\\inode.h"

// video
class CXI_VIDEORECT : public CINODE
{
public:
	CXI_VIDEORECT();
	~CXI_VIDEORECT();
	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos) {return false;}
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	StartVideoPlay(char * videoFile);

	DWORD	m_dwColor;
	FXYRECT	m_rectTex;
	ENTITY_ID m_eiVideo;
};

#endif
