#ifndef _HELPCHOOSER_H_
#define _HELPCHOOSER_H_

#include "..\\..\\common_h\\dx8render.h"

class HELPCHOOSER : public ENTITY
{
	VDX8RENDER  *rs;

public:
	 HELPCHOOSER();
	~HELPCHOOSER();
	void SetDevice();
	bool Init();
    void Execute(dword Delta_Time);
	void Realize(dword Delta_Time);
    dword _cdecl ProcessMessage(MESSAGE & message);

protected:
	void	AllRelease();
	bool	RunChooser(char * ChooserGroup);
	void	SetRectangle(long newRectNum);
	long	GetRectangleLeft();
	long	GetRectangleRight();
	long	GetRectangleUp();
	long	GetRectangleDown();
	bool	MouseMove();
	long	GetRectangleFromPos(float x, float y);

protected:
	long	m_idMouseTexture;
	long	m_idPicTexture;
	long	m_idBackTexture;
	long	m_idVBuf;

	float	m_fScreenWidth;
	float	m_fScreenHeight;
	FRECT *	m_pRectList;
	char* *	m_psRectName;
	long	m_nRectQ;
	long	m_nCurRect;

	// mouse data
	float	m_fCurMouseX;
	float	m_fCurMouseY;
	int		m_nMouseCornerX;
	int		m_nMouseCornerY;
	int		m_nMouseWidth;
	int		m_nMouseHeight;
};

#endif
