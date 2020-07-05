#ifndef _XI_SCROLLIMAGE_H
#define _XI_SCROLLIMAGE_H

#include "..\\inode.h"

// scroll image
class CXI_SCROLLIMAGE : public CINODE
{
	struct SCROLLENTITY
	{
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

		long    str1,str2;		// string identificators into string service
		char    *string1,*string2;

		void	Release(int nQnt);
		void	Clear(int nQnt);
	};

public:
	CXI_SCROLLIMAGE();
	~CXI_SCROLLIMAGE();
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
	float	ChangeDinamicParameters(float fXDelta);
	int		FindClickedImageNum();
	int		GetRightQuantity();
	int		GetLeftQuantity();
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
	XYPOINT  m_pntCenterOffset;
	XYRECT	 m_rAbsolutePosition;
	float    m_fScale;
	long     m_lDelta;
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

	// one string parameters
	float	 m_nOneStrScale;
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
	DWORD    m_dwTwoStrBackColor; // Font background color for second string

	SCROLLENTITY	*m_pScroll;
	int				m_nCurImage;
	int				m_nListSize;
	IMAGEDESCRIBE	*m_Image;
};

#endif
