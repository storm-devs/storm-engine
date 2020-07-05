#ifndef __XI_TITLE_H__
#define __XI_TITLR_H__

#include "..\inode.h"

class CXI_WINDOW : public CINODE
{
public:
	CXI_WINDOW();
	~CXI_WINDOW();

	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();

	void	SetShow( bool bShow );
	bool	GetShow() {return m_bShow;}

	void	SetActive( bool bActive );
	bool	GetActive() {return m_bActive;}

	void	AddNode( const char* pcNodeName );

	virtual int		CommandExecute(int wActCode) { return -1; };
	virtual void	Draw(bool bSelected,dword Delta_Time) {};
	virtual void	ReleaseAll() {};
	virtual bool	IsClick(int buttonID,long xPos,long yPos) { return false; };
	virtual void	MouseThis(float fX, float fY) {};

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);

protected:
	array<string> m_aNodeNameList;

	bool m_bShow;
	bool m_bActive;
};

#endif
