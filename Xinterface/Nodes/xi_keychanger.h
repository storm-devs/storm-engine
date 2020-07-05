#ifndef _XI_KEYCHANGER_H_
#define _XI_KEYCHANGER_H_

#include "..\inode.h"

class CXI_KEYCHANGER : public CINODE
{
public:
	CXI_KEYCHANGER();
	~CXI_KEYCHANGER();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode) {return -1;}
	bool	IsClick(int buttonID,long xPos,long yPos) {return false;}
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2) {}
protected:
	void	SetChoosingControls(ATTRIBUTES * pA);

protected:
	long *	m_pControlsID;
	bool *	m_pbControlsStick;
	long	m_keysQuantity;
	bool	m_bKeyCheck;
};

#endif
