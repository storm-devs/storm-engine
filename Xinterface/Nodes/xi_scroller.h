#ifndef _XI_SCROLLER_H
#define _XI_SCROLLER_H

#include "..\\inode.h"

// static inactive images into one object
class CXI_SCROLLER : public CINODE
{
public:
	 CXI_SCROLLER();
	~CXI_SCROLLER();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();

	void	SetRollerPos(float pos);
	void	LinkNodeChanged(float fPos);

	array<string> m_asOwnedNodes;

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	MakeOwnedControl();
	void	UpPress();
	void	DownPress();
	float	GetOwnedStep();

	void	FillVertexBuffer();
	void	MouseMove();

protected:
	FXYRECT		m_upButtonPos;
	FXYRECT		m_downButtonPos;
	FXYPOINT	m_pressOffset;

	long	m_idBaseTex; // border texture identificator
	long	m_idRollerTex; // roller texture identificator

	long	m_idVBuf;

	FXYRECT	m_rollerPlace;
	FXYRECT m_rollerCur;
	float	m_rollerHeight;

	FXYPOINT	m_curMousePos;

	bool	m_bDragRoll;
	float	m_fPos;

	float	m_fOffTexHeight;
	float	m_fOffHeight;
	FXYRECT m_ScrollTexRect;
	FXYRECT m_RollTexRect;
};

#endif
