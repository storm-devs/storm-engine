#ifndef _XI_SLIDEPICTURE_H_
#define _XI_SLIDEPICTURE_H_

#include "..\\inode.h"

// video
class CXI_SLIDEPICTURE : public CINODE
{
public:
	CXI_SLIDEPICTURE();
	~CXI_SLIDEPICTURE();
	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();

	void	SetNewPicture(char * sNewTexName);

protected:
	void LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void Update(dword Delta_Time);
    long	m_idTex;
	XI_ONETEX_VERTEX m_v[4];
	FXYRECT m_texRect;

	float minRotate;
	float deltaRotate;
	float curRotate;
	float curAngle;

	long nLifeTime;
	long nCurSlide;
	struct SLIDE_SPEED
	{
		DWORD time;
		float xspeed;
		float yspeed;
	} *pSlideSpeedList;
	long nSlideListSize;

	char * strTechniqueName;
};

#endif
