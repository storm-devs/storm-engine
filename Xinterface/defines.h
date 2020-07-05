#ifndef _XINTERFACE_DEFAULT_H_
#define _XINTERFACE_DEFAULT_H_

#include "..\\common_h\\cvector.h"
#include "..\\common_h\\stringService.h"
#include "vxservice.h"
#include "QuestFileReader\\QuestFileReader.h"
#include "..\\engine\\program\\interface\\messages.h"


#define PI	3.14159265358979323846f

#define ALPHA(x)		((x)>>24)
#define RED(x)			(((x)>>16)&0xFF)
#define GREEN(x)		(((x)>>8)&0xFF)
#define BLUE(x)			((x)&0xFF)
//#define ARGB(a,r,g,b)	(((a)<<24)|((r)<<16)|((g)<<8)|b)

struct COMMANDDESCR
{
	char*	sName;
	int		code;
};
extern COMMANDDESCR pCommandsList[COMMAND_QUANTITY];
extern int FindCommand(const char* comName);
extern int FindCommand(int comID);

struct SAVE_DATA_HANDLE
{
	long StringDataSize;
	long SurfaceDataSize;
};

struct XYPOINT
{
	long x,y;
	XYPOINT() {x=y=0;}
	XYPOINT(long ix, long iy)	{x=ix; y=iy;}
	void operator = (XYPOINT &lp)
	{
		this->x=lp.x;	this->y=lp.y;
	}
};
struct XYRECT
{
	long left,top,right,bottom;
	XYRECT() {left=top=right=bottom=0;}
	XYRECT(long ileft, long itop, long iright, long ibottom)
	{
		left = ileft;
		top = itop;
		right = iright;
		bottom = ibottom;
	}
	void operator = (XYRECT & ir)
	{
		this->left=ir.left;		this->top=ir.top;
		this->right=ir.right;	this->bottom=ir.bottom;
	}
};
struct FXYPOINT
{
	float x,y;

	FXYPOINT() {x=y=0.f;}
	FXYPOINT(FXYPOINT& fp) {x=fp.x; y=fp.y;}
	FXYPOINT(float fx,float fy)
	{
		this->x = fx; this->y = fy;
	}

	FXYPOINT& operator = (const FXYPOINT &fp)
	{
		this->x=fp.x;	this->y=fp.y;
		return (*this);
	}
	FXYPOINT operator + (FXYPOINT &fp)
	{
		FXYPOINT np;
		np.x = this->x+fp.x;	np.y = this->y+fp.y;
		return np;
	}
	FXYPOINT operator - (FXYPOINT &fp)
	{
		FXYPOINT np;
		np.x = this->x-fp.x;	np.y = this->y-fp.y;
		return np;
	}
};
struct FXYRECT
{
	float left,top,right,bottom;
	FXYRECT() {left=top=right=bottom=0.f;}
	FXYRECT(float fleft, float ftop, float fright, float fbottom)
	{
		left = fleft;
		top = ftop;
		right = fright;
		bottom = fbottom;
	}
	void operator = (FXYRECT & fr)
	{
		this->left=fr.left;		this->top=fr.top;
		this->right=fr.right;	this->bottom=fr.bottom;
	}
	void operator += (FXYRECT & fr)
	{
		this->left+=fr.left;	this->top+=fr.top;
		this->right+=fr.right;	this->bottom+=fr.bottom;
	}
	void operator -= (FXYRECT & fr)
	{
		this->left-=fr.left;	this->top-=fr.top;
		this->right-=fr.right;	this->bottom-=fr.bottom;
	}
	FXYRECT operator + (FXYRECT & fr)
	{
		FXYRECT tmp;
		tmp.left = this->left+fr.left;		tmp.top = this->top+fr.top;
		tmp.right = this->right+fr.right;	tmp.bottom = this->bottom+fr.bottom;
		return tmp;
	}
	FXYRECT operator - (FXYRECT & fr)
	{
		FXYRECT tmp;
		tmp.left = this->left-fr.left;		tmp.top = this->top-fr.top;
		tmp.right = this->right-fr.right;	tmp.bottom = this->bottom-fr.bottom;
		return tmp;
	}
	FXYRECT operator + (FXYPOINT & fp)
	{
		FXYRECT tmp;
		tmp.left = this->left+fp.x;		tmp.top = this->top+fp.y;
		tmp.right = this->right+fp.x;	tmp.bottom = this->bottom+fp.y;
		return tmp;
	}
	FXYRECT operator - (FXYPOINT & fp)
	{
		FXYRECT tmp;
		tmp.left = this->left-fp.x;		tmp.top = this->top-fp.y;
		tmp.right = this->right-fp.x;	tmp.bottom = this->bottom-fp.y;
		return tmp;
	}
};

// vertex formates
#define XI_NOTEX_FVF		(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX0)
struct XI_NOTEX_VERTEX
{
	CVECTOR pos;
	DWORD color;
};
#define XI_ONLYONETEX_FVF		(D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2)
struct XI_ONLYONETEX_VERTEX
{
	CVECTOR pos;
	float tu,tv;
};
#define XI_ONETEX_FVF		(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2)
struct XI_ONETEX_VERTEX
{
	CVECTOR pos;
	DWORD color;
	float tu,tv;
};
#define XI_TWOTEX_FVF		(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2|D3DFVF_TEXTUREFORMAT2)
struct XI_TWOTEX_VERTEX
{
	CVECTOR pos;
	DWORD color;
	float tu1,tv1;
	float tu2,tv2;
};
#define XI_THREETEX_FVF		(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX3|D3DFVF_TEXTUREFORMAT2)
struct XI_THREETEX_VERTEX
{
	CVECTOR pos;
	DWORD color;
	float tu1,tv1;
	float tu2,tv2;
	float tu3,tv3;
};

#define PTR_DELETE(p) if(p!=NULL) {delete p; p=NULL;}
#define TEXTURE_RELEASE(rs,idtex) if(rs!=NULL && idtex!=-1) {rs->TextureRelease(idtex); idtex=-1;}
#define VERTEX_BUF_RELEASE(rs,vb) if(rs!=NULL && vb!=-1) {rs->ReleaseVertexBuffer(vb); vb=-1;}
#define INDEX_BUF_RELEASE(rs,ib) if(rs!=NULL && ib!=-1) {rs->ReleaseIndexBuffer(ib); ib=-1;}
#define PICTURE_TEXTURE_RELEASE(ps,gn,tex) if(tex!=-1 && ps!=NULL) {ps->ReleaseTextureID(gn); tex=-1;}
#define FONT_RELEASE(rs,font) if(rs!=NULL && font!=-1) {rs->UnloadFont(font); font=-1;}
#define VIDEOTEXTURE_RELEASE(rs,tex) if(rs!=NULL && tex!=NULL) {rs->ReleaseVideoTexture(tex); tex=NULL;}

extern ENTITY_ID g_idInterface;

extern DWORD ColorInterpolate(DWORD sCol,DWORD dCol,float m);
extern void DublicateString( char* &pDstStr, const char* pSrcStr );

#endif
