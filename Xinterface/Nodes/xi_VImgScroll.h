#ifndef _XI_V_IMAGE_SCROLL_H
#define _XI_V_IMAGE_SCROLL_H

#include "..\\inode.h"

// scroll image
class CXI_VIMAGESCROLL : public CINODE
{
	struct SCROLLENTITY
	{
		bool	bCurNotUse;
		FXYPOINT pCenter;
		float    fCurScale;
		int		 imageNum;
		float    colorMul;
		SCROLLENTITY * next;
	};
	long	m_nSlotsQnt;
	struct IMAGEDESCRIBE
	{
		bool	*bUseSpecTechnique;
		long	*tex;
		IDirect3DTexture8*	*ptex;
		char*	*saveName;
		long	*img;

		//long    str1,str2;		// string identificators into string service
		//char    *string1,*string2;
		long*	strNum;
		char**	strSelf;

		void	SetStringData( int nStr );
		void	Release(int nQnt, int nStr);
		void	Clear(int nQnt, int nStr);
	};

public:
	CXI_VIMAGESCROLL();
	~CXI_VIMAGESCROLL();

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

    void    ChangeScroll(int nScrollItemNum);
	void	DeleteImage(int imgNum);
	void	RefreshScroll();

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	float	ChangeDinamicParameters(float fYDelta);
	int		FindClickedImageNum();
	int		GetTopQuantity();
	int		GetBottomQuantity();
	float	GetShiftDistance(int shiftIdx);
	void	UpdateTexturesGroup();
	int		FindTexGroupFromOld(char * *pGroupList, char * groupName, int listSize);
	long	GetMousePointedPictureNum();
protected:
	// parameters for moving to scrolling
	bool     m_bDoMove;
	float    m_fDeltaMove;
	float	 m_fDeltaMoveBase;
	float	 m_fCurrentDistance;
	float    m_fMoveDistance;
	int		 m_nSpeedMul;
	int		 m_nNotUsedQuantity;

	long     *m_pPicOffset;

	bool	 m_bShowBorder;
	int		 m_nShowOrder;

	// size & blend parameters
	XYPOINT  m_pCenter;
	XYPOINT  m_ImageSize;
	float    m_fScale;
	long     m_nVDelta;
    DWORD    *m_dwNormalColor;
    DWORD    *m_dwSelectColor;
	DWORD	 m_dwBlendColor;

    // blind parameters
	bool	 m_bDoBlind;		// blind flag
	bool	 m_bColorType;		// current type of color for blind (true - ligth, false - dark)
	int		 m_nBlindCounter;	// last time counter for change of color type
	int		 m_nMaxBlindCounter;// maximum time counter for change of color type
	DWORD    *m_dwCurColor;		// current color for select item show

	// textures parameters
	char *  *m_sGroupName;
	long 	*m_nGroupTex;
	int		 m_nGroupQuantity;
	char *   m_sBorderGroupName;	//
	long	 m_texBorder;			// select border texture identificator
	long	 m_nBorderPicture;		// select border picture identificator
	long	 *m_idBadTexture;		// текстура картинки для замены несуществующих
	long	 *m_idBadPic;			// картинка для замены несуществующих

	char *	 m_sSpecTechniqueName;
	DWORD	 m_dwSpecTechniqueARGB;

	struct StringParams
	{
		float	 m_fScale;
		int		 m_nFont;
		int		 m_nAlign;		// alignment string
		long	 m_nStrX;		// Offset from rectangle center for X coordinate
		long     m_nStrY;		// Offset from top rectangle of list for Y coordinate of string1
		DWORD    m_dwForeColor;	// Font foreground color for first string
		DWORD    m_dwBackColor;	// Font background color for first string
	};

	long			m_nStringQuantity;
	StringParams *	m_pStrParam;

	// one string parameters
	/*float	 m_nOneStrScale;
	int		 m_nOneStrFont;
	int		 m_nOneStrAlign;	  // alignment string
	long	 m_lOneStrX;		  // Offset from rectangle center for X coordinate
	bool     m_bUseOneString;     // out to screen the one text string
	long     m_lOneStrOffset;     // Offset from top rectangle of list for Y coordinate of string1
	DWORD    m_dwOneStrForeColor; // Font foreground color for first string
	DWORD    m_dwOneStrBackColor; // Font background color for first string
	// two string parameters
	float	 m_nTwoStrScale;
	int		 m_nTwoStrFont;
	int		 m_nTwoStrAlign;	  // alignment string
	long	 m_lTwoStrX;		  // Offset from rectangle center for X coordinate
	bool     m_bUseTwoString;     // out to screen the two text string
	long     m_lTwoStrOffset;     // Offset from top rectangle of list for Y coordinate of string2
	DWORD    m_dwTwoStrForeColor; // Font foreground color for second string
	DWORD    m_dwTwoStrBackColor; // Font background color for second string*/

	SCROLLENTITY	*m_pScroll;
	int				m_nCurImage;
	int				m_nListSize;
	IMAGEDESCRIBE	*m_Image;

	XYRECT		m_rAbsolutePosition;
	XYPOINT		m_pntCenterOffset;

	long		m_leftTextLimit;
	long		m_rightTextLimit;
};

#endif
