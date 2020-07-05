#ifndef __XI_SCROLLBAR_H__
#define __XI_SCROLLBAR_H__

#include "..\\inode.h"

// picture
class CXI_SCROLLBAR : public CINODE
{
public:
	CXI_SCROLLBAR();
	~CXI_SCROLLBAR();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	XYRECT	GetCursorRect();
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	UpdatePosition();

	void	WriteDataToAttribute();
	void	ChangeValue( bool bGrowing, bool bMultiply );

protected:
	char    *m_sGroupName;
	long	m_idTex; // texture identity

	bool	m_bPrevSelectStatus;
	FXYRECT m_rectCenterTex;
	FXYRECT m_rectSelectCenterTex;
	long	m_nBarWidth;
	long	m_nSideWidth;

	DWORD   m_dwShadowColor;
	DWORD   m_dwFaceColor;

	float   m_fXShadow;
	float   m_fYShadow;
	float   m_fXShadowPress;
	float   m_fYShadowPress;

	float   m_fXDeltaPress;
	float   m_fYDeltaPress;

	int		m_nPressedDelay;
	int     m_nMaxDelay;
	bool    m_bRightPress;
	int     m_clickType;

	// picture index & vertex buffers
	long	m_idVBuf; // identificator of the vertex buffer
	long	m_idIBuf; // identificator of the index buffer
	long	m_nVert;  // vertex quantity
	long	m_nIndx;  // index quantity

	FXYRECT m_frLeftTex;
	FXYRECT m_frRightTex;

	long m_nFontID;
	dword m_dwFontColor;
	float m_fFontScale;
	XYPOINT m_pntFontOffset;

	bool m_bShowString;
	float m_fCurValue;
	float m_fMinValue;
	float m_fMaxValue;
	float m_fStartValue;
	float m_fStepValue;
	float m_fSpeedMultiplay;

	struct MouseClickDescr {
		bool bExecute;
		float fCurMouseTime;
		float fDelayMouseTime;
		float fNextClickTime;
		FXYPOINT OldMousePoint;
	} m_MouseClickParam;
};

#endif
