#ifndef _XI_SCROLLEDPICTURE_H_
#define _XI_SCROLLEDPICTURE_H_

#include "xi_picture.h"
#include "xi_image.h"

class CXI_SCROLLEDPICTURE : public CXI_PICTURE
{
public:
	CXI_SCROLLEDPICTURE();
	~CXI_SCROLLEDPICTURE();
	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);
	void	MoveMouseOutScreen( float fX, float fY );
	virtual void ChangeUV( FXYRECT &frNewUV );

protected:
	void LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void SetNewPicture(bool video, char * sNewTexName);
	void SetNewPictureFromDir(char * dirName);
	void RecalculateTexPerPixel();
	void UpdateBuildenImages();
	void SetPosToCenter( float fX, float fY );
	void SetScale( long nScaleIdx );
	void SetScale( float fsx, float fsy );

	float m_fUTexPerPixel;
	float m_fVTexPerPixel;

	FXYPOINT m_fpBaseSize;

	struct BuildinImage
	{
		CXI_IMAGE* pImg;
		FXYPOINT fpPos;
		FXYPOINT fpSize;
		bool bShow;
	};

	long m_nScaleNum;
	array<FXYPOINT> m_aScale;
	array<BuildinImage> m_aImg;
};

#endif
