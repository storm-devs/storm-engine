#ifndef _XI_STRCOLLECTION_H
#define _XI_STRCOLLECTION_H

#include "..\\inode.h"

// text collection
class CXI_STRCOLLECTION : public CINODE
{
public:
	 CXI_STRCOLLECTION();
	~CXI_STRCOLLECTION();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	bool	GetInternalNameList( array<string>& aStr );
	void	SetInternalName( string& sName );
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);

	void	ChangeString(long num, const char* sValue);
	void	ChangeStringColor(long num, dword dwColor);
	void	ChangeStringPos(long num, XYPOINT& pntNewPos);

protected:
	struct STRINGDESCR
	{
		char *		sFontName;
		int			nFontNum;
		long		strNum;
		char *		strStr;
		char *		strID;
		XYPOINT		scrPos;	    // screen position for string show
		DWORD		foreColor;  // color of font
		DWORD		backColor;  // color of the font background
		int			wAlignment; // print text alignment
		bool		bShadow;    // do shadow
		float		fScale;	    // scale for string out
	};

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	CXI_STRCOLLECTION::STRINGDESCR * CreateNewDinamicString(char * strID, char * strStr);

protected:
	int			m_nStr;			// string quantity
	STRINGDESCR *m_pStrDescr;	// pointer to array of the string descriptors
	long		m_nEditIndex;
};

#endif
