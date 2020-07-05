#ifndef _XI_STATUSLINE_H
#define _XI_STATUSLINE_H

#include "..\\inode.h"

// static inactive images into one object
class CXI_STATUSLINE : public CINODE
{
public:
	 CXI_STATUSLINE();
	~CXI_STATUSLINE();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	Refresh();

protected:
	char	*m_sGroupName;// image list name
	long	m_idTex;	// texture identificator
	long	m_vBuf;		// vertex buffer identificator
	long	m_iBuf;		// index buffer identificator
	long	m_nVert;	// vertex quantity
	long	m_nIndx;	// index quantity
	float	m_fLineOffset;// смещение от края для рамки
	FXYRECT	m_texRect1;
	FXYRECT	m_texRect2;
};

#endif
