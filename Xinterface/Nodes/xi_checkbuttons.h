#ifndef __XI_CHECKBUTTONS_H__
#define __XI_CHECKBUTTONS_H__

#include "..\inode.h"
#include "xi_image.h"

struct ButtonDescribe
{
	ButtonDescribe():aStr(_FL) { pImg=null; bSetPos=false; }
	~ButtonDescribe() { DELETE( pImg ); }

	struct StrDescribe
	{
		string str;
		float fX;
	};
	array<StrDescribe> aStr;
	bool bChoose;
	bool bDisable;

	bool bSetPos;
	FXYPOINT pos;

	CXI_IMAGE* pImg;
};

// picture
class CXI_CHECKBUTTONS : public CINODE
{
public:
	CXI_CHECKBUTTONS();
	~CXI_CHECKBUTTONS();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);

	virtual bool GetInternalNameList( array<string>& aStr );
	virtual void SetInternalName( string& sName );

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	AddButton( const char* pcText, bool bDisable, bool bSelect );
	void	ChangeText( long nButtonNum, const char* pcText );

	void	CheckMouseClick( const FXYPOINT & pntMouse );
	void	SetButtonOn( long nButtonNum );
	void	SetButtonOff( long nButtonNum );
	void	SetAllButtonsToOff();
	void	SetCheckToButton( long nButtonNum, bool bCheck );

	void	UpdateAllTextInfo();
	void	UpdateTextInfo( long nButtonNum );
	void	WriteToAttributeButtonState( long nButtonIndex );

	bool m_bExclusiveChoose;

	long m_nFontNum;
	float m_fFontScale;
	dword m_dwNormalFontColor;
	dword m_dwSelectFontColor;
	dword m_dwDisableFontColor;
	FXYRECT m_frTextOffset;
	float m_fTextLineHeight;
	float m_fTextSectionInterval;
	long m_nFontAlignment;

	bool m_bClickIntoTextActive;
	bool m_bIndividualPos;
	array<ButtonDescribe*> m_aButton;
	long m_nEditableSectionIndex;

	FXYPOINT m_fpIconSize;
	FXYPOINT m_fpIconOffset;
	string m_sIconGroupName;
	//
	string m_sNormalPicture;
	dword m_dwNormalPicColor;
	//
	string m_sSelectPicture;
	dword m_dwSelectPicColor;
	//
	string m_sDisablePicture;
	dword m_dwDisablePicColor;
};

#endif
