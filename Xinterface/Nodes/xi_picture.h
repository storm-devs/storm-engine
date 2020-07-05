#ifndef _XI_PICTURE_H_
#define _XI_PICTURE_H_

#include "..\\inode.h"

class XINTERFACE;

// video
class CXI_PICTURE : public CINODE
{
	friend XINTERFACE;
public:
	CXI_PICTURE();
	~CXI_PICTURE();
	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);
	virtual void ChangeUV( FXYRECT &frNewUV );
	void	ChangeColor( dword dwColor );
	void	SetPictureSize(long& nWidth, long& nHeight);

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	SetNewPicture(bool video, char * sNewTexName);
	void	SetNewPictureFromDir(char * dirName);
	void	SetNewPictureByGroup( char* groupName, char* picName );
	void	SetNewPictureByPointer( IDirect3DBaseTexture8* pTex );
	void	ReleasePicture();

	char* m_pcGroupName;
    long m_idTex;
	IDirect3DBaseTexture8* m_pD3D8Texture;
	CVideoTexture* m_pTex;
	XI_ONETEX_VERTEX m_v[4];

	bool m_bMakeBlind;
	float m_fCurBlindTime;
	bool m_bBlindUp;
	float m_fBlindUpSpeed;
	float m_fBlindDownSpeed;
	dword m_dwBlindMin;
	dword m_dwBlindMax;
};

#endif
