#ifndef _XI_QUESTTEXTS_H_
#define _XI_QUESTTEXTS_H_

#include "..\\inode.h"

class CXI_QUESTTEXTS : public CINODE
{
public:
	CXI_QUESTTEXTS();
	~CXI_QUESTTEXTS();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);

	void	StartQuestShow(ATTRIBUTES * pA,int qn);

	float	GetLineStep();
	void	ScrollerChanged(float fPos);

protected:
	bool	GetLineNext(int fontNum,char* &pInStr,char* buf,int bufSize);
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	ReleaseStringes();
	void	GetStringListForQuestRecord( array<string> & asStringList, const char* pcRecText, const char* pcUserData );

	long	m_idFont;
	DWORD	m_dwNonCompleteColor;
	DWORD	m_dwCompleteColor;

	int		m_allStrings;
	int		m_vertOffset;

	struct STRING_DESCRIBER
	{
		int		strNum;
		char*	lineStr;
		bool	complete;
		STRING_DESCRIBER* next;
		STRING_DESCRIBER* prev;
		STRING_DESCRIBER* Add(char* ls,bool complete);
		STRING_DESCRIBER(char* ls);
	};
	STRING_DESCRIBER* m_listRoot;
	STRING_DESCRIBER* m_listCur;
	int		m_nAllTextStrings;
};

#endif
