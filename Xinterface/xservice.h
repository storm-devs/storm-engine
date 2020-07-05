#ifndef _XISERVICE_H
#define _XISERVICE_H

#include "defines.h"

class XSERVICE : public VXSERVICE
{
	struct IMAGELISTDESCR
	{
		char	*sImageListName;
		char	*sTextureName;
		long	textureID;
		int		textureQuantity;

		long	textureWidth;
		long    textureHeight;
		long	pictureQuantity;
		long	pictureStart;
	};

	struct  PICTUREDESCR
	{
		char	*sPictureName;
		XYRECT	pTextureRect;
	};

public:
	 XSERVICE();
	~XSERVICE();

	// initialization of service
	void Init(VDX8RENDER* pRS,long lWidth,long lHight);

	// get texture identificator for image group
	long GetTextureID(const char* sImageListName);
	long FindGroup(const char* sImageListName);
	bool ReleaseTextureID(const char* sImageListName);

	// get texture positon for select picture
	bool GetTexturePos(long pictureNum, FXYRECT &texRect);
	bool GetTexturePos(long pictureNum, XYRECT &texRect);
	bool GetTexturePos(const char* sImageListName, const char* sImageName, FXYRECT &texRect);
	bool GetTexturePos(const char* sImageListName, const char* sImageName, XYRECT &texRect);
    bool GetTexturePos(int nTextureModify,long pictureNum, FXYRECT &texRect);
    bool GetTexturePos(int nTextureModify,const char* sImageListName, const char* sImageName, FXYRECT &texRect);

	void GetTextureCutForSize( const char* pcImageListName, const FXYPOINT &pntLeftTopUV, const XYPOINT &pntSize, long nSrcWidth, long nSrcHeight, FXYRECT &outUV );

	long GetImageNum(const char* sImageListName, const char* sImageName);

	void ReleaseAll();

protected:
	void			LoadAllPicturesInfo();

protected:
	VDX8RENDER	    *m_pRS;

	long			m_dwListQuantity;
	long			m_dwImageQuantity;
	IMAGELISTDESCR  *m_pList;
	PICTUREDESCR	*m_pImage;

	// Масштабные коэффициенты
	float	m_fWScale;
	float	m_fHScale;
	// параметры ошибки при масштабировании
	float	m_fWAdd;
	float	m_fHAdd;
};

class ComboString
{
	VDX8RENDER	    *m_pRS;

	struct STRING_DESCR
	{
		int x,y;
		int idFont;
		char * str;
	};
	struct PICS_DESCR
	{
		XYRECT pos;
		int	idTex;
		FXYRECT texUV;
	};
	struct STRING_COMBINE
	{
		int	* pFontsID;
		int fontQ;

		STRING_DESCR * pStr;
		int strQ;

		PICS_DESCR * pPics;
		int picQ;
	};

	STRING_COMBINE * pComboStr;

	void AddToCombo(char * fontName, const XYPOINT & posStrStart, char * str);
	void AddToCombo(XYRECT posPic, char * picTexName, FXYRECT picUV);

public:
	ComboString();
	~ComboString();

	void	PrintComboString(int comboStrID);
	int		GetComboString(int align, int x,int y, int needWidth, int needHeight, int * allHeight, char * formatStr, char * fontlist);
	void	ComboStringRelease(int comboStrID);
};

#endif
