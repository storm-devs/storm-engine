#ifndef _XI_IMAGECOLLECTION_H
#define _XI_IMAGECOLLECTION_H

#include "..\..\common_h\vmodule_api.h"
#include "..\..\common_h\templates\array.h"
#include "..\..\common_h\templates\string.h"
#include "..\inode.h"

// static inactive images into one object
class CXI_IMGCOLLECTION : public CINODE
{
public:
	 CXI_IMGCOLLECTION();
	~CXI_IMGCOLLECTION();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();
	dword _cdecl MessageProc(long msgcode, MESSAGE & message);
	bool	GetInternalNameList( array<string>& aStr );
	void	SetInternalName( string& sName );

	void	AddImage( const char* pcPicName, dword dwColor, XYRECT pos );

protected:
	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);
	// set into vertex&index buffers value for image number <rectNum> : screen&texture position
	void	SetBuffers(XI_ONETEX_VERTEX *pVBuf, WORD *pIBuf, int rectNum,
		            XYRECT &scrRect, FXYRECT &texRect, DWORD dwColor);
	void	UpdateBuffers();
protected:
	bool	m_bRelativeRect;
	char	*sGroupName;	// image list name
	long	texl;		// texture identificator
	long	vBuf;		// vertex buffer identificator
	long	iBuf;		// index buffer identificator
	long	nVert;		// vertex quantity
	long	nIndx;		// index quantity

	struct PicEditInfo {
		string sName;
		long nLeft,nTop, nRight,nBottom;
		dword dwColor;
		bool bNative;
	};
	array<PicEditInfo> m_aEditInfo;

	struct PicEditSection {
		string sName;
		long nStartNum;
		long nQuantity;
	};
	array<PicEditSection> m_aSections;
	long m_nCurSection;

	XYPOINT m_xyCommonOffset;
};

#endif
