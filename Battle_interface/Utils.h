#ifndef _BI_UTILS_H_
#define _BI_UTILS_H_

#include "defines.h"
#include "..\common_h\templates\string.h"
#include "..\common_h\templates\array.h"

class BITextInfo
{
public:
	BITextInfo();
	~BITextInfo();
	void Release();
	void Init(VDX8RENDER* rs, ATTRIBUTES *pA);
	void Print();

	VDX8RENDER* pRS;
	string sText;
	POINT pos;
	float fScale;
	long nFont;
	dword dwColor;
	bool bShadow;

	ATTRIBUTES* pARefresh;
};

class BILinesInfo
{
public:
	BILinesInfo();
	~BILinesInfo();
	void Release();
	void Init(VDX8RENDER* rs, ATTRIBUTES *pA);
	void Draw();

	VDX8RENDER* pRS;
	array<RS_LINE2D> lines;
};

class IBIImage;
class BIImageRender;
class BIImagesInfo
{
public:
	BIImagesInfo();
	~BIImagesInfo();
	void Release();
	void Init(VDX8RENDER* rs, ATTRIBUTES *pA);
	void Draw();

	VDX8RENDER* pRS;
	BIImageRender* pImgRender;
	array<IBIImage*> images;
};

class BIBorderInfo
{
public:
	BIBorderInfo();
	~BIBorderInfo();
	void Release();
	void Init(VDX8RENDER* rs, ATTRIBUTES *pA);
	void Draw();

	VDX8RENDER* pRS;
	long nVBuf;
	long nTexID;
	FRECT ext_pos;
	FRECT int_pos1;
	FRECT int_pos2;
	dword dwColor1;
	dword dwColor2;
	float fCur;
	float fSpeed;
	bool bUp;
	bool bUsed;
};

class BIUtils
{
	//---------------------------------------
public: // functions
	static long GetLongFromAttr(ATTRIBUTES * pA, const char * name, long defVal);
	static float GetFloatFromAttr(ATTRIBUTES * pA, const char * name, float defVal);
	static bool ReadStringFromAttr(ATTRIBUTES * pA, const char * name, char * buf, long bufSize, const char *defVal);
	static char * GetStringFromAttr(ATTRIBUTES * pA, const char * name, const char *defVal);
	static long GetTextureFromAttr(VDX8RENDER * rs,ATTRIBUTES * pA,const char * sAttrName);
	static bool ReadRectFromAttr(ATTRIBUTES * pA,const char * name,FRECT& rOut, FRECT& rDefault);
	static bool ReadRectFromAttr(ATTRIBUTES * pA,const char * name,RECT& rOut, RECT& rDefault);
	static bool ReadPosFromAttr(ATTRIBUTES * pA,const char * name,float& fX,float& fY, float fXDef,float fYDef);
	static bool ReadPosFromAttr(ATTRIBUTES * pA,const char * name,long& nX,long& nY, long nXDef,long nYDef);
	static long GetAlignmentFromAttr(ATTRIBUTES * pA,const char * name,long nDefAlign);
	static long GetFontIDFromAttr(ATTRIBUTES * pA,const char * name,VDX8RENDER* rs,const char* pcDefFontName);
	static bool ReadVectorFormAttr(ATTRIBUTES * pA,const char * name,CVECTOR& vOut,const CVECTOR& vDef);

	static bool ComparePoint(POINT& p1,POINT& p2);

	static ATTRIBUTES * _cdecl GetAttributesFromPath(ATTRIBUTES * pA,...);

	static DWORD GetIntervalColor(DWORD minV,DWORD maxV, float fpar);
	static bool GetIntervalRect(float fk, const FRECT& r1, const FRECT& r2, FRECT& rOut);

	static long GetMaxFromFourLong( long n1, long n2, long n3, long n4 );

	static float GetFromStr_Float(const char* &pcStr, float fDefault);

	static void FillTextInfoArray(VDX8RENDER* pRS, ATTRIBUTES* pA, array<BITextInfo>& tia);
	static void PrintTextInfoArray(array<BITextInfo>& tia);
	//---------------------------------------
	//---------------------------------------
public: // data
	static ENTITY_ID idBattleInterface;
	static DWORD g_dwBlinkColor;
	//---------------------------------------
};

#endif
