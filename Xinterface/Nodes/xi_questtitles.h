#ifndef _XI_QUESTTITLES_H_
#define _XI_QUESTTITLES_H_

#include "..\\inode.h"

class CXI_QUESTTITLE : public CINODE
{
public:
	CXI_QUESTTITLE();
	~CXI_QUESTTITLE();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY);
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();

	void	SetNewTopQuest(ATTRIBUTES * pA,int topNum);
	
	float	GetLineStep();
	void	ScrollerChanged( float fPos );

protected:
	bool	GetLineNext(int fontNum,char* &pInStr,char* buf,int bufSize);
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);

	int		m_iconWidth;
	int		m_iconHeight;
	int		m_iconVOffset;
	char	*m_iconGroupName;
	FXYRECT	m_texComplete;
	FXYRECT	m_texNonComplete;
	long	m_texId;

	long	m_idFont;
	DWORD	m_dwNonCompleteColor;
	DWORD	m_dwCompleteColor;
	DWORD	m_dwSelectRectangleColor;
	int		m_fontOffset;

	int		m_stringQuantity;
	int		m_allStrings;
	int		m_vertOffset;
	int		m_selectOffset;

	int		m_curIdx;

	int		m_nCommonQuantity;

	struct STRING_DESCRIBER
	{
		long	lineQuantity;
		char*	name[10];
		bool	complete;
		dword	dwSpecColor;
	};
	STRING_DESCRIBER * m_strList;
};

#endif
