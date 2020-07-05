#ifndef _FORMTTEXT_H_
#define _FORMTTEXT_H_

#include "..\inode.h"

class CXI_FORMATEDTEXT : public CINODE
{
	struct STRING_DESCRIBER
	{
		int		strNum;
		int		strGroup;
		char*	lineStr;
		DWORD	color;
		STRING_DESCRIBER* next;
		STRING_DESCRIBER* prev;
		struct TagInfo{
			dword dwColor;
			string str;
		};
		array<TagInfo> m_tags;

		STRING_DESCRIBER* Add(char* ls,int groupNum);
		STRING_DESCRIBER(char* ls);
		STRING_DESCRIBER() : m_tags(_FL) {}
	};

public:
	CXI_FORMATEDTEXT();
	~CXI_FORMATEDTEXT();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY);
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);
	XYRECT	GetCursorRect();
	bool	IsGlowChanged() {return true;}

	void	SetFormatedText(char * str);
	void	SetPointer(float fPos);
	float	GetLineStep();
	float	GetCurPos();
	void	SetColor(dword dwCol);

	long	GetAllHeight();

protected:
	bool	GetLineNext(int fontNum,char* &pInStr,char* buf,int bufSize);
	void	GetOneLine(int fontNum,char* pStr,char* buf,int bufSize);
	void	MakeTagChecking( bool& tagState, dword& tagColor, dword normColor, STRING_DESCRIBER* pStrDescr );
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	ReleaseString( STRING_DESCRIBER * pCur );
	void	ReleaseStringes();
	long	AddFormatedText(const char * str);
	long	FindUpGroup(long grNum);
	long	FindDownGroup(long grNum);
	void	SetVertexToNewGroup(bool bUpDirect, long upIdx, long downIdx);
	void	CheckScrollButtons();
	void	SetSpecialStrings(ATTRIBUTES * pARoot);
	void	ControlSyncronouseNodes();
	long	GetFirstGroupNum();
	void	SetCurrentGroupNum(long nFirstNum, long nSelectNum);
	void	ReplaceString( long nGrpNum, const char * pSrcStr );
	void	InsertStringBefore( STRING_DESCRIBER * pNextDescr, const char * pSrcStr, long nGrpNum, dword dwColor );
	void	RecalculateStringNumber();
	void	VAlignment(long nAlign);

protected:
	long	m_idFont;
	DWORD	m_dwColor;
	float	m_fFontScale;
	long	m_nAlignment;

	int		m_allStrings;
	int		m_vertOffset;
	long	m_leftOffset;
	long	m_nVAlignmentOffset;

	STRING_DESCRIBER* m_listRoot;
	STRING_DESCRIBER* m_listCur;
	int		m_nAllTextStrings;
	int		m_nStringGroupQuantity;

	bool	m_bSelectableCursor;
	CVideoTexture* m_pVidTex;
	long	m_idVBuf;
	long	m_nCurGroupNum;
	float	m_fHorzScale;

	bool	m_bUpEnable;
	bool	m_bDownEnable;
	long	m_idUpEnableTexture;
	long	m_idUpDisableTexture;
	long	m_idDownEnableTexture;
	long	m_idDownDisableTexture;
	XYRECT	m_frUpPos;
	XYRECT	m_frDownPos;
	FXYRECT	m_frUpEnableUV;
	FXYRECT	m_frUpDisableUV;
	FXYRECT	m_frDownEnableUV;
	FXYRECT	m_frDownDisableUV;

	XYRECT	m_rectCursorPosition;

	long	m_nCompareWidth;
	long	m_nPrintLeftOffset;

	bool	m_bBackRectangle;
	DWORD	m_dwBackColor;
	XYRECT	m_rBorderOffset;
	long	m_nUpRectOffset;

	char*	m_sScrollerName;

	array<string> m_asSyncNodes;

	bool	m_bUseOneStringAdding;

	bool	m_bFrized;

	//void	SetCurLine(long nNewCurLine);
	void	SetCurLine(STRING_DESCRIBER* pNewCurLine);
	void	ScrollerUpdate();
};


#endif
