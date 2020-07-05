#ifndef _XI_BOUNDER_H_
#define _XI_BOUNDER_H_

#include "..\inode.h"

class CXI_BOUNDER : public CINODE
{
public:
	CXI_BOUNDER();
	~CXI_BOUNDER();

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

protected:
	// texture parameters
	char	*m_sGroupName;
	long	m_idTex;
	DWORD   m_dwColor;

	// picture index & vertex buffers
	long	m_idVBuf; // identificator of the vertex buffer
	long	m_idIBuf; // identificator of the index buffer
	long	m_nVert;  // vertex quantity
	long	m_nIndx;  // index quantity

	float	m_fAngleWidth;
	float	m_fAngleHeight;

	long	m_idAngle;
	long	m_idHorzLine;

	long	m_nHorzLineQuantity;
	long	m_nVertLineQuantity;
};

#endif
