#ifndef __XI_TITLE_H__
#define __XI_TITLR_H__

#include "..\inode.h"

class CXI_TITLE : public CINODE
{
public:
	CXI_TITLE();
	~CXI_TITLE();
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
	void	FillVertexBuffer();

protected:
	char    *m_sGroupName;
	long	m_idTex; // texture identity

	long	m_idString;
	XYPOINT m_StringCenter;
	DWORD	m_fontColor;
	DWORD	m_backColor;
	float	m_fontScale;
	long    m_fontID;

	long	m_idVBuf;		// vertex buffer identificator
	long	m_idIBuf;		// index buffer identificator
	long	m_nVert;		// vertex quantity
	long	m_nIndx;		// index quantity

	long	m_nStringWidth;
	XYRECT  m_mRect, m_tRect;
	long	m_nTiledQuantity;
};

#endif
