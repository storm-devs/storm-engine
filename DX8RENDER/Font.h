#ifndef _FONT_H_
#define _FONT_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\vfile_service.h"
#include "..\common_h\matrix.h"
//#include "iimage.h"

#define USED_CODES	256
#define MAX_SYMBOLS	512
#define SYM_VERTEXS	6

#ifndef IMAGE_VERTEX_DEF 
#define IMAGE_VERTEX_DEF 
struct IMAGE_VERTEX
{
	CVECTOR pos;
	float rhw;
	DWORD color;
	float tu,tv;
};
#endif

#define IMAGE_FVF (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2)

#ifndef FLOAT_RECT_DEF
#define FLOAT_RECT_DEF

struct FLOAT_RECT
{
	float x1,y1,x2,y2;
};
#endif

#ifndef FONT_SYMBOL_DEF
#define FONT_SYMBOL_DEF
struct FONT_SYMBOL
{
	FLOAT_RECT Pos;
	FLOAT_RECT Tuv;
};
#endif

class FONT
{
	VAPI * api;
	VDX8RENDER * RenderService;
	IDirect3DDevice8 * Device;
	IDirect3DVertexBuffer8 * VBuffer;
    char *techniqueName;
    char *textureName;
	long  TextureID;
	DWORD Color,oldColor;
	float fScale,fOldScale;
	FLOAT_RECT Pos;
	FONT_SYMBOL CharT[USED_CODES];
	long Height;
	long Texture_XSize;
	long Texture_YSize;
	long Symbol_interval;
	
	bool bShadow,bOldShadow;
	long Shadow_offsetx;
	long Shadow_offsety;

	long Spacebar;

	bool bInverse,bOldInverse;

	float m_fAspectRatioH;
	float m_fAspectRatioV;
public:
	FONT();
	~FONT();
	bool Init(char * font_name, char * iniName, IDirect3DDevice8 * _device, VDX8RENDER * _render);
    void TempUnload();
    void RepeatInit();
	void Realize(DWORD DeltaTime);
	void SetColor(DWORD color);
	void SetScale(float scale);
	void SetShadow(bool s);
	void SetCenter(bool c);
	bool MakeLong(char * * pDataPointer, long * result);
	long _cdecl Printf(long x, long y, char * Text, ...);
	long Print(long x, long y, char * Text);
	long UpdateVertexBuffer(long x, long y, char * data_PTR);
	long GetStringWidth(const char * Text);
	long GetHeight(){return Height;};
	void Inverse(bool v);
	void SetShadowOffset(long ox, long oy);
	void GetShadowOffset(long & ox, long & oy);
	void StoreFontParameters(){oldColor=Color;fOldScale=fScale;bOldShadow=bShadow;bOldInverse=bInverse;};
	void RestoreFontParameters(){Color=oldColor;fScale=fOldScale;bShadow=bOldShadow;bInverse=bOldInverse;};
};

#endif