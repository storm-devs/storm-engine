#ifndef _XI_FOURIMAGE_
#define _XI_FOURIMAGE_

#include "..\inode.h"

class CXI_FOURIMAGE : public CINODE
{
public:
	 CXI_FOURIMAGE();
	~CXI_FOURIMAGE();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	XYRECT	GetCursorRect();
	bool	IsGlowChanged() {return true;}

    void    ChangeItem(int nItemNum);
protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void    FillVertex();
	void	Update(bool bSelected,dword DeltaTime);

protected:
	bool    m_bUsed[4];			// selected picture
	// images parametes
	char*	*m_sGroupName;
	long	*m_nTextureId;
	long	m_nTexturesQuantity;
	char*	m_sBorderGroupName; // picture list name for border image
	long	m_texBorder;		// border texture identify
	XYRECT  m_imgRect[4];		// images position
	long	m_nBorderPicture;	// image id for border picture
	long	m_oneImgID[4];		// image id for one image
	long	m_twoImgID[4];		// image id for two image
	long	m_oneTexID[4];		// texture id for one image
	long	m_twoTexID[4];		// texture id for two image
	long	m_oneBadTexture;	// текстура дл€ картики замен€ющей первую "плохую"
	long	m_twoBadTexture;	// текстура дл€ картики замен€ющей вторую "плохую"

	// select parameters
	bool	m_bShowBorder;			// do show border
	int		m_nSelectItem;			// select image
	DWORD	m_dwBaseColor;			// vertex color
	DWORD	m_dwLightSelectColor;   // vertex color
	DWORD	m_dwDarkSelectColor;    // vertex color

	// one string parameters
	bool	bUseOneString;	 // use one string
	long	m_xOneOffset;	 // hirizontal offset from center position for one string
	long	m_nOneStrOffset; // string offset from top image position
	long    m_oneStr[4];	 // one strings identificators
	char *	m_pOneStr[4];	 // one strings
	long	m_oneStrFont;    // one string font number
	DWORD   m_foreColOneStr; // one string foreground color
	DWORD   m_backColOneStr; // one string background color

	// two string parameters
	bool	bUseTwoString;   // use two string
	long	m_xTwoOffset;	 // hirizontal offset from center position for two string
	long	m_nTwoStrOffset; // string offset from top image position
	long	m_twoStr[4]; 	 // two strings identificators
	char *	m_pTwoStr[4];	 // two strings
	long    m_twoStrFont;    // two string font number
	DWORD   m_foreColTwoStr; // one string foreground color
	DWORD   m_backColTwoStr; // one string background color

	// picture index & vertex buffers
	long	vBuf;  // identificator of the vertex buffer

	// blind parameters
	bool	m_bDoBlind;			// blind flag
	bool	m_bColorType;		// current type of color for blind (true - ligth, false - dark)
	int		m_nBlindCounter;	// last frames counter for change of color type
	int		m_nMaxBlindCounter;	// maximum frames counter for change of color type
	DWORD   m_dwCurSelectColor; // current color for select item show
};

#endif
