#ifndef __XI_CHANGER_H__
#define __XI_CHANGER_H__

#include "..\\inode.h"

// picture
class CXI_CHANGER : public CINODE
{
public:
	CXI_CHANGER();
	~CXI_CHANGER();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);
	XYRECT	GetCursorRect();
	bool	IsGlowChanged() {return true;}

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
    void    SetRectanglesToPosition(int nPos);

protected:
    int     m_nPlaceQuantity;
    XYRECT  *m_pPlace;

	bool	m_bUseBlind;
    DWORD	m_dwFoneColor;
	DWORD	m_dwBlindColor;
	DWORD	m_dwCurColor;
    float	m_xOffset,m_yOffset;
	bool	m_bUpBlind;
	float	m_fCurM;
	float	m_fCurM_UpSpeed;
	float	m_fCurM_DownSpeed;

	long	m_idBackTex;
    CVideoTexture *m_pTex;

    int     m_nCurrentPos;

    XI_ONETEX_VERTEX	m_pTexVert[8];

    int     m_nClickPlaceNum;
    bool    m_bIncreaseChange;
};

#endif
