#ifndef _XI_TOOLTIP_H_
#define _XI_TOOLTIP_H_

#include "..\inode.h"

class CXI_ToolTip
{
public:
	CXI_ToolTip( VXSERVICE* pPicService, VSTRSERVICE* pStrService, XYPOINT& pntScrSize );
	~CXI_ToolTip();

	void Draw();
	void SetByFormatString( XYRECT& rectOwner, INIFILE* pDefIni, const char* pFmtStr );

	void MousePos( float fDeltaTime, long nX, long nY );

protected:
	void ReleaseAll();

	void CreateIndexBuffer();
	void CreateVertexBuffer();
	void UpdateIndexBuffer();
	void UpdateVertexBuffer();

	void ReplaceRectangle( long x, long y );

protected:
	VDX8RENDER* m_rs;
	XI_ONETEX_VERTEX* m_pV;
	WORD* m_pI;
	long m_nSquareQ;
	dword m_dwBackColor;
	long m_nLeftSideWidth;
	long m_nRightSideWidth;

	FXYRECT m_uvBackLeft;
	FXYRECT m_uvBackRight;
	FXYRECT m_uvBackMiddle;

	string m_sGroupName;
	long m_nTextureID;
	long m_nPicIndex_Left;
	long m_nPicIndex_Right;
	long m_nPicIndex_Middle;

	XYPOINT m_pntScreenSize;
	XYPOINT m_pntTextOffset;
	XYRECT m_rPos;
	XYRECT m_rActiveZone;

	long m_nXRectangleOffset;
	long m_nYRectangleOffsetUp;
	long m_nYRectangleOffsetDown;

	string m_sText;
	array<string> m_aSubText;
	long m_nFontID;
	dword m_dwFontColor;
	float m_fFontScale;
	long m_nMaxStrWidth;
	long m_nUseWidth;
	long m_nUseHeight;

	bool m_bDisableDraw;
	float m_fTurnOnDelay;
	float m_fCurTimeLeft;
	long m_nMouseX;
	long m_nMouseY;

	VXSERVICE* m_pPicService;
	VSTRSERVICE* m_pStrService;
};

#endif
