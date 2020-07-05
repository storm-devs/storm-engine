#ifndef __XI_GLOWER_H_
#define __XI_GLOWER_H_

#include "..\inode.h"

#define MAX_USED_RECTANGLE	20

#define GLOW_ACTION_NONE	0
#define GLOW_ACTION_COLORUP	1
#define GLOW_ACTION_SHOW	2
#define GLOW_ACTION_BLEND	3

class CXI_GLOWER : public CINODE
{
	struct GLOW_DESCR
	{
		RS_RECT rect;
		long	allTime;
		long    curTime;
		float	angleSpeed;
		int		action;
	};
public:
	CXI_GLOWER();
	~CXI_GLOWER();
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

	long	m_nQuantity;
	// glow describe
	GLOW_DESCR	m_glows[MAX_USED_RECTANGLE];
	long	m_texID;

	long	m_minGlowTime;
	long	m_maxGlowTime;
	long	m_minShowTime;
	long	m_maxShowTime;

	long	m_nRandomMax;

	float	m_fAngleSpeedMin;
	float	m_fAngleSpeedMax;

	DWORD	m_dwMinColor;
	DWORD	m_dwMaxColor;
};

#endif
