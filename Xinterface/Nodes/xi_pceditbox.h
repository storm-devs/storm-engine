#ifndef _XI_PCEDITBOX_H_
#define _XI_PCEDITBOX_H_

#include "..\\inode.h"

class CXI_IMAGE;

// edit box
class CXI_PCEDITBOX : public CINODE
{
public:
	CXI_PCEDITBOX();
	~CXI_PCEDITBOX();

	void	ReleaseAll();
	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY){};
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	UpdateString( string & str );
	char	GetInputSymbol();
	bool	IsEditMode() {return IsCurrentNode();}
	void	ShowCursorPosition( string & str );
	void	InsertSymbol( string & str, char chr );
	void	DisguiseString( string &str );
	bool	IsExcludeChar( char chr );

protected:
	long	m_nFontID;			// font id
	DWORD	m_dwFontColor;		// color for font
	XYPOINT	m_pntFontOffset;	// string offset
	long	m_nStringAlign;		// string alignment
	float	m_fFontScale;		// font scale

	long	m_nEditPos;			// position of current edited character
	long	m_nMaxSize;			// max character quantity
	long	m_nMaxWidth;		// max string width

	long	m_nFirstShowCharacterIndex; // первый символ для вывода строки

	CXI_IMAGE * m_pLeftImage;
	CXI_IMAGE * m_pRightImage;
	CXI_IMAGE * m_pMiddleImage;

	bool	m_bWaitKeyRelease;
	bool	m_bDisguiseString;

	string	m_sExcludeChars;
};

#endif
