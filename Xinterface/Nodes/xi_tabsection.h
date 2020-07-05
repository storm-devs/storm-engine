#ifndef __XI_TABSECTION_H__
#define __XI_TABSECTION_H__

#include "..\inode.h"
#include "xi_image.h"

class CXI_TABSECTION : public CINODE
{
public:
	CXI_TABSECTION();
	~CXI_TABSECTION();

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
	void	UpdateImage();

	string m_sIconGroupName;
	long m_idIconTexture;
	long m_idVBuf;
	long m_idIBuf;
	long m_nSubQ;

	long m_nIcon_TopBorder;
	long m_nIcon_BottomBorder;
	long m_nIcon_LeftArrow;
	long m_nIcon_RightArrow;
	long m_nIcon_LeftBookmarkPart;
	long m_nIcon_RightBookmarkPart;
	long m_nIcon_MiddleBookmarkPart;

	long m_nTopBorderHeight;
	long m_nBottomBorderHeight;
	long m_nTopBorderOffset;
	long m_nBottomBorderOffset;
	long m_nArrowOffset;
	long m_nBookmarkSpace;

	XYPOINT m_pntArrowSize;
	XYPOINT m_pntLeftBookmarkSize;
	XYPOINT m_pntRightBookmarkSize;
	XYPOINT m_pntMiddleBookmarkSize;

	dword m_dwBorderColor;
	dword m_dwArrowColor;
	dword m_dwBookmarkColor;
};

#endif
