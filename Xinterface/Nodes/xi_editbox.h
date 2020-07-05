#ifndef _XI_EDITBOX_H_
#define _XI_EDITBOX_H_

#include "..\\inode.h"

// edit box
class CXI_EDITBOX : public CINODE
{
public:
	CXI_EDITBOX();
	~CXI_EDITBOX();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY);
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	SetNewCurSymbol(int h, int v);

protected:
	char	m_alpha[512];
	int		m_nAlphaQuantity;
	int		m_nCurAlphaNum;
	int		m_nFirstChar;
	int		m_nHorz, m_nVert;
	int		m_nMaxSize;
	int		m_bUpChrRegistrOffset;

	FXYRECT	m_frNormTex;
	FXYRECT	m_frSelTex;

	long	m_idBtnTex;
	DWORD	m_dwEditBoxColor;
	DWORD	m_dwBorderColor;

	int		m_nTopStringPos;
	int		m_nTopOffset;
	int		m_nLeftOffset;

	long	m_nChrFontNum;
	long	m_nStrFontNum;
	float	m_fChrScale;
	float	m_fStrScale;
	float	m_fChrTopOffset;

	bool	m_bOldSelected;

	long	m_idVBRect;
	long	m_idVB;
	long	m_idIB;
	long	m_nVQ;
	long	m_nIQ;

	float	m_fVAdd;
	float	m_fHAdd;
};

#endif
