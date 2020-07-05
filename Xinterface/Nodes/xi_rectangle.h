#ifndef _XI_RECTANGLE_H_
#define _XI_RECTANGLE_H_

#include "..\inode.h"

class CXI_RECTANGLE : public CINODE
{
public:
	 CXI_RECTANGLE();
	~CXI_RECTANGLE();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);
	bool	IsGlowChanged() {return true;}

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	UpdateColors();

	DWORD m_dwTopColor;
	DWORD m_dwBottomColor;
	DWORD m_dwLeftColor;
	DWORD m_dwRightColor;
    bool  m_bBorder;
    DWORD m_dwBorderColor;

	XI_NOTEX_VERTEX m_pVert[4];
};

#endif
