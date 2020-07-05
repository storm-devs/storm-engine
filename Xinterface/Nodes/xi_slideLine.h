#ifndef _XI_SLIDELINE_H_
#define _XI_SLIDELINE_H_

#include "..\\inode.h"

// video
class CXI_SLIDELINE : public CINODE
{
public:
	CXI_SLIDELINE();
	~CXI_SLIDELINE();
	void		Draw(bool bSelected,dword Delta_Time);
	bool		Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void		ReleaseAll();
	int			CommandExecute(int wActCode);
	bool		IsClick(int buttonID,long xPos,long yPos);
	void		MouseThis(float fX, float fY);
	void		ChangePosition( XYRECT &rNewPos );
	void		SaveParametersToIni();
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	SetNewValue(long newValue);
	void	DoMouseControl();

	bool	m_bDoChangeSlider;

    long	m_idTexLine;
    long	m_idTexSelLine;
	long	m_idTexPointer;
	long	m_idVBuf;

	dword	m_dwDisableColor;

	long	m_nPointerWidth;
	long	m_nPointerHeight;

	long	m_nBaseLeft;
	long	m_nPointerLeft;

	long	m_nGrateQuantity;
	long	m_nCurValue;
	long	m_nSpeedSlide;

	long	m_nMaxValue;
	long	m_nMinValue;
};

#endif
