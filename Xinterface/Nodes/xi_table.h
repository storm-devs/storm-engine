#ifndef __XI_TABLE_H__
#define __XI_TABLE_H__

#include "..\inode.h"
#include "xi_image.h"

class CXI_TABLE;
class XI_TableLineDescribe;

class XI_TableCellDescribe
{
	struct StrDescribe
	{
		string str;
		FPOINT offset;
	};

	struct ImgDescribe
	{
		CXI_IMAGE* pImage;
		POINT offset;
		ImgDescribe() {pImage=0; offset.x=offset.y=0;}
		~ImgDescribe() {DELETE(pImage);}
	};

public:
	XI_TableCellDescribe(CXI_TABLE* pTable, XI_TableLineDescribe* pLine);
	~XI_TableCellDescribe();
	void SetOwners( CXI_TABLE* pTable, XI_TableLineDescribe* pLine ) {m_pTable=pTable; m_pLine=pLine;}

	void Draw( float fLeft, float fTop );
	void SetData( long nColIndex, ATTRIBUTES* pAttr, bool bHeader );
	void LoadImageParam(ImgDescribe* pImg,ATTRIBUTES* pA);

protected:
	CXI_TABLE* m_pTable;
	XI_TableLineDescribe* m_pLine;

	FPOINT m_TextOffset;
	long m_nFontID;
	long m_nFontIndex;
	dword m_dwColor;
	float m_fScale;
	long m_nAlignment;
	long m_nVAlignment;

	long m_nLeftLineWidth;
	long m_nTopLineHeight;

	array<StrDescribe> m_aStrings;
	array<ImgDescribe> m_aImage;

public:
	long m_nColIndex;
};

class XI_TableLineDescribe
{
public:
	XI_TableLineDescribe(CXI_TABLE* pTable);
	~XI_TableLineDescribe();
	void SetOwners( CXI_TABLE* pTable ) {m_pTable=pTable;}

	void Draw( float fTop );
	void DrawSpecColor( float fTop );
	void SetData( long nRowIndex, ATTRIBUTES* pLA, bool bHeader );

	long GetLineHeight();
	void SetLineHeight(long nHeight) {m_nHeight = nHeight;}

protected:
	CXI_TABLE* m_pTable;
	array<XI_TableCellDescribe*> m_aCell;

	bool m_bUseSpecColor;
	dword m_dwSpecColor;
	long m_nHeight;

public:
	long m_nRowIndex;
};

// picture
class CXI_TABLE : public CINODE
{
	friend XI_TableLineDescribe;
	friend XI_TableCellDescribe;
public:
	CXI_TABLE();
	~CXI_TABLE();

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

	void	ScrollerChanged( float fRelativeScrollPos );
	float	GetLineStep() {if(m_nLineQuantity<=0) return 0.f; if(m_nLineQuantity>1) return 1.f/(float)(m_nLineQuantity-1); return 1.f;}

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	void	UpdateBorders();
	void	WriteSquare( XI_ONETEX_VERTEX* pV, long nImgID, dword dwCol, long nX, long nY, long nW, long nH );
	void	UpdateTableCells();
	long	GetLineByPoint( FXYPOINT& pnt );
	long	GetColByX( long x );
	void	SelectRow( long nRowNum );
	void	SelectRow( long nRowNum, long nColNum );
	void	SelectLine( long nLineNum );
	void	SelectCol( long nColNum );
	long	GetRowTop( long nRow );
	long	GetColLeft( long nCol );
	void	SetTopIndexForSelect( long nSelIndex );
	void	UpdateSelectImage();
	void	UpdateLineQuantity();
	void	SetTopIndex( long nTopIndex );
	void	UpdateScroller();
	void	RecalculateLineHeights();

	bool m_bFirstFrame;

	bool m_bVariableLineHeight;
	long m_nNormalLineHeight;

	array<XI_TableLineDescribe*> m_aLine;
	XI_TableLineDescribe* m_pHeader;

	array<long> m_anFontList;

	long m_nFontCellID;
	dword m_dwFontCellColor;
	float m_fFontCellScale;
	long m_nFontCellAlignment;
	long m_nFontCellVAlignment;

	long m_nFontTitleID;
	dword m_dwFontTitleColor;
	float m_fFontTitleScale;
	long m_nFontTitleAlignment;
	long m_nFontTitleVAlignment;

	// back describe
	bool m_bBackPresent;
	CXI_IMAGE m_BackImg;
	CXI_IMAGE m_SelectImg;
	bool m_bDoColsSelect;
	long m_nSelectColIndex;

	string m_sBorderIconGroupName;
	long m_idBorderTexture;
	long m_idBorderVBuf;
	long m_idBorderIBuf;
	long m_nBorderSubQ;

	long m_nBorderIcon_LeftTop;
	long m_nBorderIcon_LeftBottom;
	long m_nBorderIcon_RightTop;
	long m_nBorderIcon_RightBottom;
	long m_nBorderIcon_Left;
	long m_nBorderIcon_Right;
	long m_nBorderIcon_Top;
	long m_nBorderIcon_Bottom;
	long m_nBorderIcon_VLine;
	long m_nBorderIcon_HLine;

	dword m_dwBorderColor;
	long m_nBorderWidth;
	long m_nVLineWidth;
	long m_nHLineHeight;
	long m_nHeaderLineHeight;
	bool m_bHLineIsBreakable;
	XYPOINT m_pntBorderCornerSize;
	XYPOINT m_pntSpaceSize;

	long m_nRowQuantity;
	long m_nColQuantity;
	array<long> m_anRowsHeights;
	array<long> m_anColsWidth;

	struct EditModeDescribe
	{
		bool bAllEditable;
		bool bColsEditable;
		long nEditableIndex;
	};
	EditModeDescribe m_EditData;

	long m_nTopIndex;
	long m_nSelectIndex;

	long m_nLineQuantity;

	string m_sScrollerName;
};

#endif
