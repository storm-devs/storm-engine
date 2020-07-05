#include "Technique.h"
#include "..\common_h\defines.h"

#ifdef _XBOX
#include <D3dx8.h>
#endif

#define SHA_DIR		"modules\\Techniques"
#define SHA_EXT		".sha"

#define PASS_OK				0
#define PASS_ERROR			1

#define TECHNIQUE_OK		0
#define TECHNIQUE_ERROR		1

#define BLOCK_OK			0
#define BLOCK_ERROR			1

#define SHADER_OK			0
#define SHADER_ERROR		1

#define	COMMENT1			"//"
#define	COMMENT2			";"
#define	BLOCK				"block"
#define	TECHNIQUE			"technique"
#define PIXEL_SHADER		"pixelshader"
#define VERTEX_SHADER		"vertexshader"
#define VERTEX_SHADER_CONST	"vertexshaderconstant"
#define PIXEL_SHADER_CONST	"pixelshaderconstant"
#define VERTEX_DECL			"decl"
#define ASM_SHADER			"asm"
#define	PASS				"pass"
#define INCLUDE				"#include"
#define DEFINE				"#define"
#define BEGIN_BRACKET		"{"
#define END_BRACKET			"}"
#define NOSAVE_STATE		"!"

#define STSS_CHECK				"["
#define TEXTURE_CHECK			"texture["
#define RENDER_CHECK			"render."
#define INPARAM_CHECK			"<"
#define TRANSFORM_CHECK			"transform."
#define WORLD_TRANSFORM_CHECK	".world"
#define RESTORE_STATES_CHECK	"restore."

#define VDECL_STREAM_CHECK		"stream"
#define VDECL_FLOAT_CHECK		"float"
#define VDECL_COLOR_CHECK		"color"
#define VDECL_UBYTE4_CHECK		"ubyte4"

#define DEFSIZE(x)		x, (sizeof(x) / sizeof(SRSPARAM)), 0, 0

// codes

#define CODE_SRS			0		// SetRenderState
#define CODE_STSS			1		// SetTextureStageState
#define CODE_TEXTURE		2		// SetTexture
#define CODE_TRANSFORM		3		// SetTransform
#define CODE_RENDER			4		// Render
#define CODE_RESTORE		5		// Restore
#define CODE_SVS			6		// Set vertex shader
#define CODE_SPS			7		// Set pixel shader
#define CODE_SVSCONST		8		// Set vertex shader constant
#define CODE_SPSCONST		9		// Set pixel shader constant

// SetVertexShaderConstant subcodes
#define SUBCODE_SVSCONST_WORLDVIEWPROJ		0

// render subcodes
#define SUBCODE_RENDER_DRAW		0

// restore subcodes
#define SUBCODE_RESTORE_STATES	100
#define SUBCODE_RESTORE_CLEAR	101

// errors
#define INVALID_INDEX			0xFFFFFFFF
#define INVALID_SRS_INDEX		0xFFFFFFFF
#define INVALID_STSS_INDEX		0xFFFFFFFF
#define INVALID_SHADER_INDEX	0xFFFFFFFF
#define INVALID_SHADER_HANDLE	0xFFFFFFFF

// 
#define FLAGS_CODE_RESTORE		(0x1 << 0x10)
#define FLAGS_CODE_IN_PARAM		(0x1 << 0x11)		// use [in] parameter
#define FLAGS_CODE_NUMBER		(0x1 << 0x12)		// use [in] parameter

// parameters flags

// common defines 
#define SAVED_STATES_ADD		128

char* SkipToken(char* str, char* cmp) 
{
	int flag;
	char *str1,*cmp1;

	if (str==0 || cmp==0) return 0;
	cmp1 = cmp;

	if ((str == NULL)||(cmp == NULL)) return NULL;
	if (strlen(str)==0) return NULL;

	int lens = strlen(str);
	int lenc = strlen(cmp);
//	if (strcmp(str,cmp) != 0) return NULL;
	for (int i=0;i<lens;i++)
	{
		flag = 0;
		str1 = str;
		if (*str == *cmp)
		{
			flag = 1;
			for (int j=0;j<lenc;j++)
				if (*str++ != *cmp++) {	flag = 0;	break;	}
			if (!flag)
			{
				cmp = cmp1;
				str = str1;
			}
		}
		if (flag) break;
		str++;
	}
	if (flag) 
	{
		while (*str == 32) str++;
		return str;
	}
	return NULL;
}

char* FindToken(char* str,char* cmp) 
{
	int flag;
	char *str1,*cmp1;

	if (str==0 || cmp==0) return 0;
	cmp1 = cmp;

	if ((str == NULL)||(cmp == NULL)) return NULL;
	if (strlen(str)==0) return NULL;

	int lens = strlen(str);
	int lenc = strlen(cmp);
//	if (strcmp(str,cmp) != 0) return NULL;
	for (int i=0;i<lens;i++)
	{
		flag = 0;
		str1 = str;
		if (*str == *cmp)
		{
			flag = 1;
			for (int j=0;j<lenc;j++)
				if (*str++ != *cmp++) {	flag = 0;	break;	}
			if (!flag)
			{
				cmp = cmp1;
				str = str1;
			}
		}
		if (flag) break;
		str++;
	}
	if (flag) return str;
	return NULL;
}

// Get token before symbol skip
char*	GetTokenWhile(char* src, char* dst, char* skip)
{
	if (!src || !dst || !skip) return 0;
	dst[0] = 0;
	char *dst1 = dst;
	long lensrc = strlen(src);
	long dstlen = 0;
	while (lensrc && src[0]==' ') { src++; lensrc--; };
	while ((lensrc--)&&(*src!=*skip))
	{
		*dst1++ = *src++;
		dstlen++;
	}
	*dst1++ = 0;
	char *pRet = ((lensrc>0) ? ++src : src); if (lensrc) lensrc--;
	while (lensrc && pRet[0]==32) { pRet++; lensrc--; };
	while (dstlen && dst[dstlen-1]==' ') { dstlen--; dst[dstlen] = 0; }
	return pRet;
}

typedef struct
{
	SRSPARAM				*pParam;
	dword					dwParamNum;
	bool					bUse;
	long					dwUseSubCode;
	dword					SrsType;
	char					*cName;
	D3DRENDERSTATETYPE		State;
} SRS;

typedef struct
{
	SRSPARAM					*pParam;
	dword						dwParamNum;
	bool						bUse;
	long						dwUseSubCode;
	dword						SrsType;
	char						*cName;
	D3DTEXTURESTAGESTATETYPE	State;
} STSS;

#define SRS_DWORD			0
#define SRS_FLOAT			1

// ---------------------------------------------------------------------------------

SRSPARAM MYSETVERTEXSHADERCONSTANT[] = {
	{ "worldviewproj", SUBCODE_SVSCONST_WORLDVIEWPROJ } };

// SetRenderState parameters
SRSPARAM	MYD3DZB[] = {
	{ "false",	D3DZB_FALSE },
	{ "true",	D3DZB_TRUE },
	{ "usew",	D3DZB_USEW } };

SRSPARAM	MYD3DFILL[] = {
	{ "point", D3DFILL_POINT },
	{ "wireframe", D3DFILL_WIREFRAME },
	{ "solid", D3DFILL_SOLID } };

SRSPARAM	MYD3DSHADE[] = {
	{ "flat", D3DSHADE_FLAT },
	{ "gouraud", D3DSHADE_GOURAUD }
#ifndef _XBOX	
	,{ "phong", D3DSHADE_PHONG } 
#endif
};

SRSPARAM	MYD3DBLEND[] = {
	{ "zero", D3DBLEND_ZERO },
	{ "one", D3DBLEND_ONE },
	{ "srccolor", D3DBLEND_SRCCOLOR },
	{ "invsrccolor", D3DBLEND_INVSRCCOLOR },
	{ "srcalpha", D3DBLEND_SRCALPHA },
	{ "invsrcalpha", D3DBLEND_INVSRCALPHA },
	{ "destalpha", D3DBLEND_DESTALPHA },
	{ "invdestalpha", D3DBLEND_INVDESTALPHA },
	{ "destcolor", D3DBLEND_DESTCOLOR },
	{ "invdestcolor", D3DBLEND_INVDESTCOLOR },
	{ "srcalphasat", D3DBLEND_SRCALPHASAT },
#ifndef _XBOX	
	{ "bothsrcalpha", D3DBLEND_BOTHSRCALPHA },
	{ "bothinvsrcalpha", D3DBLEND_BOTHINVSRCALPHA } 
#endif
};

SRSPARAM	MYD3DCULL[] = {
	{ "none", D3DCULL_NONE },
	{ "cw", D3DCULL_CW },
	{ "ccw", D3DCULL_CCW } };

SRSPARAM	MYD3DBLENDOP[] = {
	{ "add", D3DBLENDOP_ADD }, 
	{ "subtract", D3DBLENDOP_SUBTRACT }, 
	{ "revsubtract", D3DBLENDOP_REVSUBTRACT }, 
	{ "min", D3DBLENDOP_MIN }, 
	{ "max", D3DBLENDOP_MAX } };

SRSPARAM	MYD3DCMPFUNC[] = {
	{ "never", D3DCMP_NEVER },
	{ "less", D3DCMP_LESS },
	{ "equal", D3DCMP_EQUAL },
	{ "lessequal", D3DCMP_LESSEQUAL },
	{ "greater", D3DCMP_GREATER },
	{ "notequal", D3DCMP_NOTEQUAL },
	{ "greaterequal", D3DCMP_GREATEREQUAL },
	{ "always", D3DCMP_ALWAYS } };

SRSPARAM	MYD3DFOGMODE[] = {
	{ "none", D3DFOG_NONE },
	{ "exp", D3DFOG_EXP },
	{ "exp2", D3DFOG_EXP2 },
	{ "linear", D3DFOG_LINEAR } };

SRSPARAM	MYD3DSTENCILOP[] = {
	{ "keep", D3DSTENCILOP_KEEP },
	{ "zero", D3DSTENCILOP_ZERO },
	{ "replace", D3DSTENCILOP_REPLACE },
	{ "incrsat", D3DSTENCILOP_INCRSAT },
	{ "decrsat", D3DSTENCILOP_DECRSAT },
	{ "invert", D3DSTENCILOP_INVERT },
	{ "incr", D3DSTENCILOP_INCR },
	{ "decr", D3DSTENCILOP_DECR } };

SRSPARAM	MYD3DMATERIALCOLORSOURCE[] = {
	{ "material", D3DMCS_MATERIAL },
	{ "color1", D3DMCS_COLOR1 },
	{ "color2", D3DMCS_COLOR2 } };

SRSPARAM	MYD3DVERTEXBLENDFLAGS[] = {
	{ "disable", D3DVBF_DISABLE },
	{ "1weights", D3DVBF_1WEIGHTS },
	{ "2weights", D3DVBF_2WEIGHTS },
	{ "3weights", D3DVBF_3WEIGHTS },
#ifndef _XBOX	
	{ "tweening", D3DVBF_TWEENING }
	,{ "0WEIGHTS ", D3DVBF_0WEIGHTS } 
#endif
};

SRSPARAM	MYD3DPATCHEDGESTYLE[] = {
	{ "discrete", D3DPATCHEDGE_DISCRETE },
	{ "continuous", D3DPATCHEDGE_CONTINUOUS } };

SRSPARAM	MYD3DCOLORWRITEMASK[] = {
	{ "alpha", D3DCOLORWRITEENABLE_ALPHA},
	{ "red", D3DCOLORWRITEENABLE_RED },
	{ "green", D3DCOLORWRITEENABLE_GREEN},
	{ "blue", D3DCOLORWRITEENABLE_BLUE} };

SRSPARAM	MYD3DWRAP[] = {
	{ "wrapcoord0", D3DWRAPCOORD_0 },
	{ "wrapcoord1", D3DWRAPCOORD_1 },
	{ "wrapcoord2", D3DWRAPCOORD_2 },
	{ "wrapcoord3", D3DWRAPCOORD_3 },
	{ "wrapu", D3DWRAP_U},
	{ "wrapv", D3DWRAP_V} };

SRSPARAM	MYTRUEFALSE[] = {
	{ "true", true },
	{ "false", false } };

SRS	RenderStates[] = {
	{ DEFSIZE(MYD3DZB)	, SRS_DWORD, "ZEnable",						D3DRS_ZENABLE},
	{ DEFSIZE(MYD3DFILL) , SRS_DWORD, "FillMode",					D3DRS_FILLMODE},
	{ DEFSIZE(MYD3DSHADE) , SRS_DWORD, "ShadeMode",					D3DRS_SHADEMODE},
#ifndef _XBOX	
	{ DEFSIZE(0) , SRS_DWORD, "LinePattern",						D3DRS_LINEPATTERN },
#endif
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "ZWriteEnable",				D3DRS_ZWRITEENABLE },
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "AlphaTestEnable",			D3DRS_ALPHATESTENABLE },
#ifndef _XBOX	
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "LastPixel",				D3DRS_LASTPIXEL },
#endif
	{ DEFSIZE(MYD3DBLEND) , SRS_DWORD, "SrcBlend",					D3DRS_SRCBLEND },
	{ DEFSIZE(MYD3DBLEND) , SRS_DWORD, "DestBlend",					D3DRS_DESTBLEND },
	{ DEFSIZE(MYD3DCULL) , SRS_DWORD, "CullMode",					D3DRS_CULLMODE },
	{ DEFSIZE(MYD3DCMPFUNC) , SRS_DWORD, "ZFunc",					D3DRS_ZFUNC },
	{ DEFSIZE(0) , SRS_DWORD, "AlphaRef",							D3DRS_ALPHAREF },
	{ DEFSIZE(MYD3DCMPFUNC) , SRS_DWORD, "AlphaFunc",				D3DRS_ALPHAFUNC },
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "DitherEnable",				D3DRS_DITHERENABLE },
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "AlphaBlendEnable",			D3DRS_ALPHABLENDENABLE },
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "FogEnable",				D3DRS_FOGENABLE },
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "SpecularEnable",			D3DRS_SPECULARENABLE },
#ifndef _XBOX	
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "ZVisible",					D3DRS_ZVISIBLE },
#endif
	{ DEFSIZE(0) , SRS_DWORD, "FogColor",							D3DRS_FOGCOLOR },
	{ DEFSIZE(MYD3DFOGMODE) , SRS_DWORD, "FogTableMode",			D3DRS_FOGTABLEMODE },
	{ DEFSIZE(0) , SRS_FLOAT, "FogStart",							D3DRS_FOGSTART },
	{ DEFSIZE(0), SRS_FLOAT, "FogEnd",								D3DRS_FOGEND },
	{ DEFSIZE(0), SRS_FLOAT, "FogDensity",							D3DRS_FOGDENSITY },
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "EdgeAntialias",			D3DRS_EDGEANTIALIAS },
	{ DEFSIZE(0), SRS_DWORD, "Zbias",								D3DRS_ZBIAS },
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "RangeFogEnable",			D3DRS_RANGEFOGENABLE },
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "StencilEnable",			D3DRS_STENCILENABLE },
	{ DEFSIZE(MYD3DSTENCILOP) , SRS_DWORD, "StencilFail",			D3DRS_STENCILFAIL },
	{ DEFSIZE(MYD3DSTENCILOP) , SRS_DWORD, "StencilZFail",			D3DRS_STENCILZFAIL },
	{ DEFSIZE(MYD3DSTENCILOP) , SRS_DWORD, "StencilPass",			D3DRS_STENCILPASS },
	{ DEFSIZE(MYD3DCMPFUNC) , SRS_DWORD, "StencilFunc",				D3DRS_STENCILFUNC },
	{ DEFSIZE(0), SRS_DWORD, "StencilRef",							D3DRS_STENCILREF },
	{ DEFSIZE(0), SRS_DWORD, "StencilMask",							D3DRS_STENCILMASK },
	{ DEFSIZE(0), SRS_DWORD, "StencilWriteMask",					D3DRS_STENCILWRITEMASK },
	{ DEFSIZE(0), SRS_DWORD, "TextureFactor",						D3DRS_TEXTUREFACTOR },
	{ DEFSIZE(MYD3DWRAP) , SRS_DWORD, "Wrap0",						D3DRS_WRAP0 },
	{ DEFSIZE(MYD3DWRAP) , SRS_DWORD, "Wrap1",						D3DRS_WRAP1 },
	{ DEFSIZE(MYD3DWRAP) , SRS_DWORD, "Wrap2",						D3DRS_WRAP2 },
	{ DEFSIZE(MYD3DWRAP) , SRS_DWORD, "Wrap3",						D3DRS_WRAP3 },
#ifndef _XBOX	
	{ DEFSIZE(MYD3DWRAP) , SRS_DWORD, "Wrap4",						D3DRS_WRAP4 },
	{ DEFSIZE(MYD3DWRAP) , SRS_DWORD, "Wrap5",						D3DRS_WRAP5 },
	{ DEFSIZE(MYD3DWRAP) , SRS_DWORD, "Wrap6",						D3DRS_WRAP6 },
	{ DEFSIZE(MYD3DWRAP) , SRS_DWORD, "Wrap7",						D3DRS_WRAP7 },
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "Clipping",					D3DRS_CLIPPING },
#endif
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "Lighting",					D3DRS_LIGHTING },
	{ DEFSIZE(0), SRS_DWORD, "Ambient",								D3DRS_AMBIENT },
#ifndef _XBOX
	{ DEFSIZE(MYD3DFOGMODE) , SRS_DWORD, "FogVertexMode",			D3DRS_FOGVERTEXMODE },
#endif
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "ColorVertex",				D3DRS_COLORVERTEX },
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "LocalViewer",				D3DRS_LOCALVIEWER },
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "NormalizeNormals",			D3DRS_NORMALIZENORMALS },
	{ DEFSIZE(MYD3DMATERIALCOLORSOURCE) , SRS_DWORD, "DiffuseMaterialSource",		D3DRS_DIFFUSEMATERIALSOURCE },
	{ DEFSIZE(MYD3DMATERIALCOLORSOURCE) , SRS_DWORD, "SpecularMaterialSource",		D3DRS_SPECULARMATERIALSOURCE },
	{ DEFSIZE(MYD3DMATERIALCOLORSOURCE) , SRS_DWORD, "AmbientMaterialSource",		D3DRS_AMBIENTMATERIALSOURCE },
	{ DEFSIZE(MYD3DMATERIALCOLORSOURCE) , SRS_DWORD, "EmissiveMaterialSource",		D3DRS_EMISSIVEMATERIALSOURCE },
	{ DEFSIZE(MYD3DVERTEXBLENDFLAGS) , SRS_DWORD, "VertexBlend",	D3DRS_VERTEXBLEND },
#ifndef _XBOX
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "ClipPlaneEnable",			D3DRS_CLIPPLANEENABLE },
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "SoftwareVertexProcessing",	D3DRS_SOFTWAREVERTEXPROCESSING },
#endif
	{ DEFSIZE(0), SRS_FLOAT, "PointSize",							D3DRS_POINTSIZE },
	{ DEFSIZE(0), SRS_FLOAT, "PointSize_Min",						D3DRS_POINTSIZE_MIN },
	{ DEFSIZE(0), SRS_FLOAT, "PointSize_Max",						D3DRS_POINTSIZE_MAX	},
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "PointSpriteEnable",		D3DRS_POINTSPRITEENABLE },
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "PointScaleEnable",			D3DRS_POINTSCALEENABLE },
	{ DEFSIZE(0), SRS_FLOAT, "PointScale_A",						D3DRS_POINTSCALE_A },
	{ DEFSIZE(0), SRS_FLOAT, "PointScale_B",						D3DRS_POINTSCALE_B },
	{ DEFSIZE(0), SRS_FLOAT, "PointScale_C",						D3DRS_POINTSCALE_C },
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "MultiSampleAntialias",		D3DRS_MULTISAMPLEANTIALIAS },
	{ DEFSIZE(0), SRS_DWORD, "MultiSampleMask",						D3DRS_MULTISAMPLEMASK },
	{ DEFSIZE(0), SRS_FLOAT, "PatchSegments",						D3DRS_PATCHSEGMENTS },
	{ DEFSIZE(MYD3DPATCHEDGESTYLE) , SRS_DWORD, "PatchEdgeStyle",	D3DRS_PATCHEDGESTYLE },
#ifndef _XBOX
	{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "IndexedVertexBlendEnable",	D3DRS_INDEXEDVERTEXBLENDENABLE },
#endif
	{ DEFSIZE(MYD3DCOLORWRITEMASK) , SRS_DWORD, "ColorWriteEnable",	D3DRS_COLORWRITEENABLE },
#ifndef _XBOX
	{ DEFSIZE(0), SRS_FLOAT, "TweenFactor",							D3DRS_TWEENFACTOR },
#endif
	{ DEFSIZE(MYD3DBLENDOP) , SRS_DWORD, "BlendOp",					D3DRS_BLENDOP } };

// ---------------------------------------------------------------------------------
// SetTextureStageState parameters
SRSPARAM	MYD3DTEXTUREOP[] = {
	{ "disable", D3DTOP_DISABLE },
	{ "selectarg1", D3DTOP_SELECTARG1 },
	{ "selectarg2", D3DTOP_SELECTARG2 },
	{ "modulate", D3DTOP_MODULATE },
	{ "modulate2x", D3DTOP_MODULATE2X },
	{ "modulate4x", D3DTOP_MODULATE4X },
	{ "add", D3DTOP_ADD },
	{ "addsigned", D3DTOP_ADDSIGNED },
	{ "addsigned2x", D3DTOP_ADDSIGNED2X },
	{ "substract", D3DTOP_SUBTRACT },
	{ "addsmooth", D3DTOP_ADDSMOOTH },
	{ "blenddiffusealpha", D3DTOP_BLENDDIFFUSEALPHA },
	{ "blendtexturealpha", D3DTOP_BLENDTEXTUREALPHA },
	{ "blendfactoralpha", D3DTOP_BLENDFACTORALPHA },
	{ "blendcurrentalpha", D3DTOP_BLENDCURRENTALPHA },
	{ "blendtexturealphapm", D3DTOP_BLENDTEXTUREALPHAPM },
	{ "premodulate", D3DTOP_PREMODULATE },
	{ "modulatealpha_addcolor", D3DTOP_MODULATEALPHA_ADDCOLOR },
	{ "modulatecolor_addalpha", D3DTOP_MODULATECOLOR_ADDALPHA },
	{ "modulateinvalpha_addcolor", D3DTOP_MODULATEINVALPHA_ADDCOLOR },
	{ "modulateinvcolor_addalpha", D3DTOP_MODULATEINVCOLOR_ADDALPHA },
	{ "bumpenvmap", D3DTOP_BUMPENVMAP },
	{ "bumpenvmapluminance", D3DTOP_BUMPENVMAPLUMINANCE },
	{ "dotproduct3", D3DTOP_DOTPRODUCT3 },
	{ "multiplyadd", D3DTOP_MULTIPLYADD },
	{ "lerp", D3DTOP_LERP } };

SRSPARAM	MYD3DTAFLAGS[] = {
	{ "current", D3DTA_CURRENT },
	{ "diffuse", D3DTA_DIFFUSE },
	{ "selectmask", D3DTA_SELECTMASK },
	{ "specular", D3DTA_SPECULAR },
	{ "temp", D3DTA_TEMP },
	{ "texture", D3DTA_TEXTURE },
	{ "tfactor", D3DTA_TFACTOR },
	{ "alphareplicate", D3DTA_ALPHAREPLICATE },
	{ "complement", D3DTA_COMPLEMENT },
	{ "current_alphareplicate", D3DTA_CURRENT | D3DTA_ALPHAREPLICATE },
	{ "diffuse_alphareplicate", D3DTA_DIFFUSE | D3DTA_ALPHAREPLICATE },
	{ "specular_alphareplicate", D3DTA_SPECULAR | D3DTA_ALPHAREPLICATE },
	{ "temp_alphareplicate", D3DTA_TEMP | D3DTA_ALPHAREPLICATE },
	{ "texture_alphareplicate", D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE },
	{ "tfactor_alphareplicate", D3DTA_TFACTOR | D3DTA_ALPHAREPLICATE},
	{ "current_complement", D3DTA_CURRENT | D3DTA_COMPLEMENT },
	{ "diffuse_complement", D3DTA_DIFFUSE | D3DTA_COMPLEMENT },
	{ "specular_complement", D3DTA_SPECULAR | D3DTA_COMPLEMENT },
	{ "temp_complement", D3DTA_TEMP | D3DTA_COMPLEMENT },
	{ "texture_complement", D3DTA_TEXTURE | D3DTA_COMPLEMENT },
	{ "tfactor_complement", D3DTA_TFACTOR | D3DTA_COMPLEMENT } };

SRSPARAM	MYD3DTEXCOORDINDEX[] = {
	{ "passthru", D3DTSS_TCI_PASSTHRU },
	{ "cameraspacenormal", D3DTSS_TCI_CAMERASPACENORMAL },
	{ "cameraspaceposition", D3DTSS_TCI_CAMERASPACEPOSITION },
	{ "cameraspacereflectionvector", D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR } };

SRSPARAM	MYD3DTEXTUREADDRESS[] = {
	{ "wrap", D3DTADDRESS_WRAP },
	{ "mirror", D3DTADDRESS_MIRROR },
	{ "clamp", D3DTADDRESS_CLAMP },
	{ "border", D3DTADDRESS_BORDER },
#ifndef _XBOX	
	{ "mirroronce", D3DTADDRESS_MIRRORONCE } 
#endif
};

SRSPARAM	MYD3DTEXTUREFILTERTYPE[] = {
	{ "none", D3DTEXF_NONE },
	{ "point", D3DTEXF_POINT },
	{ "linear", D3DTEXF_LINEAR },
	{ "anisotropic", D3DTEXF_ANISOTROPIC },
#ifndef _XBOX	
	{ "flatcubic", D3DTEXF_FLATCUBIC },
#endif
	{ "gaussiancubic", D3DTEXF_GAUSSIANCUBIC } };

SRSPARAM	MYD3DTEXTURETRANSFORMFLAGS[] = {
	{ "disable", D3DTTFF_DISABLE },
	{ "count1", D3DTTFF_COUNT1 },
	{ "count2", D3DTTFF_COUNT2 },
	{ "count3", D3DTTFF_COUNT3 },
	{ "count4", D3DTTFF_COUNT4 },
	{ "projected", D3DTTFF_PROJECTED } };

STSS TexturesStageStates[] = {
	{ DEFSIZE(MYD3DTEXTUREOP) , SRS_DWORD, "colorop",			D3DTSS_COLOROP },
    { DEFSIZE(MYD3DTAFLAGS) , SRS_DWORD, "colorarg1",			D3DTSS_COLORARG1 },
    { DEFSIZE(MYD3DTAFLAGS) , SRS_DWORD, "colorarg2",			D3DTSS_COLORARG2 },
    { DEFSIZE(MYD3DTEXTUREOP) , SRS_DWORD, "alphaop",			D3DTSS_ALPHAOP },
    { DEFSIZE(MYD3DTAFLAGS) , SRS_DWORD, "alphaarg1",			D3DTSS_ALPHAARG1 },
    { DEFSIZE(MYD3DTAFLAGS) , SRS_DWORD, "alphaarg2",			D3DTSS_ALPHAARG2 },
    { DEFSIZE(0), SRS_FLOAT, "bumpenvmat00",					D3DTSS_BUMPENVMAT00 },
    { DEFSIZE(0), SRS_FLOAT, "bumpenvmat01",					D3DTSS_BUMPENVMAT01 },
    { DEFSIZE(0), SRS_FLOAT, "bumpenvmat10",					D3DTSS_BUMPENVMAT10 },
    { DEFSIZE(0), SRS_FLOAT, "bumpenvmat11",					D3DTSS_BUMPENVMAT11 },
    { DEFSIZE(MYD3DTEXCOORDINDEX) , SRS_DWORD, "texcoordindex", D3DTSS_TEXCOORDINDEX },
    { DEFSIZE(MYD3DTEXTUREADDRESS) , SRS_DWORD, "addressu",		D3DTSS_ADDRESSU },
    { DEFSIZE(MYD3DTEXTUREADDRESS) , SRS_DWORD, "addressv",		D3DTSS_ADDRESSV },
    { DEFSIZE(0), SRS_DWORD, "bordercolor",						D3DTSS_BORDERCOLOR },
    { DEFSIZE(MYD3DTEXTUREFILTERTYPE) , SRS_DWORD, "magfilter", D3DTSS_MAGFILTER },
    { DEFSIZE(MYD3DTEXTUREFILTERTYPE) , SRS_DWORD, "minfilter", D3DTSS_MINFILTER },
    { DEFSIZE(MYD3DTEXTUREFILTERTYPE) , SRS_DWORD, "mipfilter", D3DTSS_MIPFILTER },
    { DEFSIZE(0), SRS_FLOAT, "mipmaplodbias",					D3DTSS_MIPMAPLODBIAS },
    { DEFSIZE(0), SRS_DWORD, "maxmiplevel",						D3DTSS_MAXMIPLEVEL },
    { DEFSIZE(0), SRS_DWORD, "maxanisotropy",					D3DTSS_MAXANISOTROPY },
    { DEFSIZE(0), SRS_FLOAT, "bumpenvlscale",					D3DTSS_BUMPENVLSCALE },
    { DEFSIZE(0), SRS_FLOAT, "bumpenvloffset",					D3DTSS_BUMPENVLOFFSET },
    { DEFSIZE(MYD3DTEXTURETRANSFORMFLAGS) , SRS_DWORD, "texturetransformflags", D3DTSS_TEXTURETRANSFORMFLAGS },
    { DEFSIZE(MYD3DTEXTUREADDRESS) , SRS_DWORD, "addressw", D3DTSS_ADDRESSW },
    { DEFSIZE(MYD3DTAFLAGS) , SRS_DWORD, "colorarg0",			D3DTSS_COLORARG0 },
    { DEFSIZE(MYD3DTAFLAGS) , SRS_DWORD, "alphaarg0",			D3DTSS_ALPHAARG0 },
    { DEFSIZE(MYD3DTAFLAGS) , SRS_DWORD, "resultarg",			D3DTSS_RESULTARG } };

// ---------------------------------------------------------------------------------
// transform block
SRSPARAM	TRANSFORM_TYPES[] = {
	{ "view", D3DTS_VIEW },
	{ "projection", D3DTS_PROJECTION },
	{ "world", D3DTS_WORLD },
	{ "tex0", D3DTS_TEXTURE0 },
	{ "tex1", D3DTS_TEXTURE1 },
	{ "tex2", D3DTS_TEXTURE2 },
	{ "tex3", D3DTS_TEXTURE3 },
#ifndef _XBOX	
	{ "tex4", D3DTS_TEXTURE4 },
	{ "tex5", D3DTS_TEXTURE5 },
	{ "tex6", D3DTS_TEXTURE6 },
	{ "tex7", D3DTS_TEXTURE7 } 
#endif
};

// ---------------------------------------------------------------------------------
// params section for block
#define PARAM_MATRIX				0
#define PARAM_TEXTURE				1
#define PARAM_DWORD					2
#define PARAM_FLOAT					3

SRSPARAM	PARAM_TYPES[] = {
	{ "matrix", PARAM_MATRIX },
	{ "texture", PARAM_TEXTURE },
	{ "dword", PARAM_DWORD },
	{ "float", PARAM_FLOAT } };

// restores types

SRSPARAM	RESTORE_TYPES[] = {
	{ "states", SUBCODE_RESTORE_STATES },
	{ "clear", SUBCODE_RESTORE_CLEAR } };

CTechnique::CTechnique(VDX8RENDER * _pRS) : htBlocks(_FL_)
{
	dwNumBlocks = 0;
	dwNumShaders = 0;
	dwCurBlock = 0;
	dwNumParams = 0;
	dwCurMaxSavedSize = 0;
	dwNumSavedStates = 0;
	dwCurSavedStatesPos = 0;
	dwCurParamsMax = 0;

	pShaders = null;
	pBlocks = null;
	pParams = null;
	pSavedStates = null;
	pCurParams = null;

	pRS = _pRS;
	
	ZERO(sDelimTable);
	char sDelimeters[] = " ,.[]-+\0\n\r\t";
	for (dword i=0;i<strlen(sDelimeters);i++) sDelimTable[sDelimeters[i]] = 1;
}

CTechnique::~CTechnique()
{
	dword i, j, k;
	for (i=0;i<dwNumBlocks;i++)
	{
		block_t *pB = &pBlocks[i];
		for (j=0;j<pB->dwNumTechniques;j++)
		{
			technique_t *pTech = &pB->pTechniques[j];
			for (k=0;k<pTech->dwNumPasses;k++) DELETE(pTech->pPasses[k].pPass);
			DELETE(pTech->pPasses);
		}
		DELETE(pB->pParams);
		DELETE(pB->pBlockName);
		DELETE(pB->pTechniques);
	}
	for (i=0;i<dwNumShaders;i++)
	{
		DELETE(pShaders[i].pName);
		DELETE(pShaders[i].pDecl);
		if (pShaders[i].dwShaderHandle != INVALID_SHADER_HANDLE)
		{
			if (pShaders[i].dwShaderType == CODE_SVS )
				pRS->DeleteVertexShader(pShaders[i].dwShaderHandle);
			if (pShaders[i].dwShaderType == CODE_SPS)
				pRS->DeletePixelShader(pShaders[i].dwShaderHandle);
		}
	}
	DELETE(pShaders);
	DELETE(pBlocks);
	DELETE(pSavedStates);
	DELETE(pCurParams);
}

#define END_TEST	(dword(pCurrent - pBegin) >= dwSize-2)

char *GetString(char * pBegin, dword dwSize, char * pCurrent)
{
	if (END_TEST) return 0;
	// go for a null, or 0xA, or 0xD
	if (pCurrent != pBegin)		// for first string
	{
		while ((*pCurrent && *pCurrent!=0xa && *pCurrent!=0xd) && !END_TEST) *pCurrent++;
		if (END_TEST) return 0;
	}
	// skip all nulls, or 0xA, or 0xD
	while ((!(*pCurrent) || (*pCurrent==0xa && *pCurrent==0xd)) && !END_TEST) *pCurrent++;
	if (END_TEST) return 0;
	return pCurrent;
}

void ClearComment(char * pStr)
{
	// search comment and delete all symbols after
	if (!pStr || !pStr[0]) return;
	long iLen = strlen(pStr);
	char *pSkip1 = FindToken(pStr,COMMENT1);
	long iStart1 = iLen, iStart2 = iLen;
	if (pSkip1) iStart1 = (pSkip1 - pStr) - strlen(COMMENT1);
	char *pSkip2 = FindToken(pStr,COMMENT2);
	if (pSkip2) iStart2 = (pSkip2 - pStr) - strlen(COMMENT2);
	long iStart = (iStart2 > iStart1) ? iStart1 : iStart2;
	for (long i=iStart;i<iLen;i++) pStr[i] = 0;
}

bool isVertexDeclaration(char *pStr) { return (SkipToken(pStr,VERTEX_DECL)) ? true : false; }
bool isAsm(char *pStr) { return (SkipToken(pStr,ASM_SHADER)) ? true : false; }
bool isVertexShaderConst(char *pStr) { return (SkipToken(pStr,VERTEX_SHADER_CONST)) ? true : false; }
bool isPixelShaderConst(char *pStr) { return (SkipToken(pStr,PIXEL_SHADER_CONST)) ? true : false; }
bool isVertexShader(char *pStr) { return (SkipToken(pStr,VERTEX_SHADER)) ? true : false; }
bool isPixelShader(char *pStr) { return (SkipToken(pStr,PIXEL_SHADER)) ? true : false; }
bool isBlock(char *pStr) { return (SkipToken(pStr,BLOCK)) ? true : false; }
bool isTechnique(char *pStr) { return (SkipToken(pStr,TECHNIQUE)) ? true : false; }
bool isPass(char *pStr) { return (SkipToken(pStr,PASS)) ? true : false; }
bool isInclude(char *pStr) { return (SkipToken(pStr,INCLUDE)) ? true : false; }
bool isDefine(char *pStr) { return (SkipToken(pStr,DEFINE)) ? true : false; }
bool isBeginBracket(char *pStr) { return (SkipToken(pStr,BEGIN_BRACKET)) ? true : false; }
bool isEndBracket(char *pStr) { return (SkipToken(pStr,END_BRACKET)) ? true : false; }
bool isDigit(char * pStr) { return ((pStr) ? (pStr[0]>='0' && pStr[0]<='9') : false); }

dword CTechnique::GetSRSIndex(char *pStr)
{
	dword dwNumParam = sizeof(RenderStates) / sizeof(STSS);
	for (dword i=0;i<dwNumParam;i++) if (stricmp(pStr,RenderStates[i].cName)==0) return i;
	api->Trace("ERROR: SetRenderState: unknown parameter type <%s> in <%s> file, technique <%s>",pStr,sCurrentFileName,sCurrentBlockName);
	//THROW;
	return INVALID_SRS_INDEX;
}

dword CTechnique::GetSTSSIndex(char *pStr)
{
	dword dwNumParam = sizeof(TexturesStageStates) / sizeof(STSS);
	for (dword i=0;i<dwNumParam;i++) if (stricmp(pStr,TexturesStageStates[i].cName)==0) return i;
	api->Trace("ERROR: SetTextureStageState: unknown parameter type <%s> in <%s> file, technique <%s>",pStr,sCurrentFileName,sCurrentBlockName);
	//THROW;
	return INVALID_STSS_INDEX;
}

dword CTechnique::GetIndex(char *pStr, SRSPARAM *pParam, dword dwNumParam, bool bCanBeNumber)
{
	for (dword i=0;i<dwNumParam;i++) if (stricmp(pStr,pParam[i].cName)==0) return i;
	if (!bCanBeNumber) api->Trace("ERROR: Unknown parameter type <%s> in <%s> file, technique <%s>",pStr,sCurrentFileName,sCurrentBlockName);
	//THROW;
	return INVALID_INDEX;
}

dword CTechnique::GetCode(char *pStr, SRSPARAM *pParam, dword dwNumParam, dword *pPassCode, bool bCanBeNumber)
{
	Assert(pStr);

	dword dwCode = 0;
	if (pParam && dwNumParam)
	{
		dword dwIndex = GetIndex(pStr,pParam,dwNumParam,bCanBeNumber);
		if (dwIndex != INVALID_INDEX) return pParam[dwIndex].dwCode;
	}
	// check for numbers

	if (isDigit(pStr))
	{
		sscanf(pStr,(SkipToken(pStr,"0x")) ? "%x" : "%d",&dwCode);
		if (pPassCode) *pPassCode |= FLAGS_CODE_NUMBER;
		return dwCode;
	}
	else
	{
		api->Trace("ERROR: unknown parameter type <%s> in <%s> file, technique <%s>",pStr,sCurrentFileName,sCurrentBlockName);
		return 0;
	}
}

dword CTechnique::AddShader(char *pShaderName)
{
	// find exist shader or create empty new
	for (dword i=0;i<dwNumShaders;i++) if (stricmp(pShaders[i].pName,pShaderName)==0) return i;
	pShaders = (shader_t*)RESIZE(pShaders,sizeof(shader_t) * (dwNumShaders+1));
	shader_t *pS = &pShaders[dwNumShaders];
	ZERO(pShaders[dwNumShaders]);
	COPY_STRING(pS->pName,pShaderName);
	pS->dwHashName = hash_string(pShaderName);
	pS->dwShaderHandle = INVALID_SHADER_HANDLE;
	dwNumShaders++;
	return (dwNumShaders-1);
}

dword CTechnique::ProcessPass(char * pFile, dword dwSize, char **pStr)
{
	block_t	* pB = &pBlocks[dwNumBlocks];
	technique_t * pTechniques = &pB->pTechniques[pB->dwNumTechniques];

	pTechniques->pPasses = (pass_t*)RESIZE(pTechniques->pPasses, sizeof(pass_t) * (pTechniques->dwNumPasses + 1));
	PZERO(&pTechniques->pPasses[pTechniques->dwNumPasses], sizeof pass_t);

	DWORD	* pPass = pPassStorage;
	DWORD	* pPassBegin = pPass;
	dword	dwTextureIndex, dwAdditionalFlags = 0;
	char	* pTemp, temp[256];
	while (0 != (*pStr = GetString(pFile,dwSize,*pStr)))
	{
#define SKIP3	{ (*pStr)++; continue; }
		*pStr = _strlwr(*pStr);
		ClearComment(*pStr);
		//if (isComment(*pStr)) SKIP3;
		if (isEndBracket(*pStr)) break;		// end of pass

		// check right side of expression for [in] parameters
		bool	bIn = false;
		dword	dwInParamIndex = 0;
		if (pTemp = SkipToken(*pStr,INPARAM_CHECK))	// STSS or SetTexture
		{
			GetTokenWhile(pTemp,&temp[0],">");
			bIn = true;
			dwInParamIndex = GetIndex(temp,pParams,dwNumParams,false);
			dwAdditionalFlags |= FLAGS_CODE_IN_PARAM;
		}

		// check for additional flags such as "no restore states" and other
			dwAdditionalFlags |= FLAGS_CODE_RESTORE;
			if (SkipToken(*pStr,NOSAVE_STATE))
			{
				*pStr = SkipToken(*pStr,NOSAVE_STATE);
				dwAdditionalFlags &= (~FLAGS_CODE_RESTORE);
			}

		// restore states check
			if (pTemp = SkipToken(*pStr,RESTORE_STATES_CHECK))	
			{
				*pPass++ = CODE_RESTORE;
				*pPass++ = GetCode(pTemp,RESTORE_TYPES,sizeof(RESTORE_TYPES)/sizeof(SRSPARAM),false);
				// clear STSS and SRS bUse
				ClearSRS_STSS_bUse();
				SKIP3;
			}
			if (isPixelShaderConst(*pStr))
			{
				dword dwIndex = 0;
				*pPass++ = CODE_SPSCONST | dwAdditionalFlags;
				GetTokenWhile(SkipToken(*pStr,"["),&temp[0],"]");
				sscanf(temp,"%d",&dwIndex);
				*pPass++ = dwIndex;
				if (bIn)
				{
					*pPass++ = dwInParamIndex;
				}
				else
				{
					GetTokenWhile(SkipToken(*pStr,"="),temp,";");
				}
				//*pPass++ = GetCode(temp,0,0,null,false);
				SKIP3;
			}

			if (isVertexShaderConst(*pStr))
			{
				dword dwIndex = 0;
				*pPass++ = CODE_SVSCONST | dwAdditionalFlags;
				GetTokenWhile(SkipToken(*pStr,"["),&temp[0],"]");
				sscanf(temp,"%d",&dwIndex);
				*pPass++ = dwIndex;
				GetTokenWhile(SkipToken(*pStr,"="),temp,";");
				*pPass++ = GetCode(temp,&MYSETVERTEXSHADERCONSTANT[0],sizeof(MYSETVERTEXSHADERCONSTANT) / sizeof(SRSPARAM),null,false);
				SKIP3;
			}

		// check for '[' if exist mean what STSS used
			if (pTemp = SkipToken(*pStr,STSS_CHECK))	// STSS or SetTexture
			{
				// get index = [index]
				GetTokenWhile(pTemp,&temp[0],"]");
				sscanf(temp,"%d",&dwTextureIndex);

				// check for "texture["
				if (SkipToken(*pStr,TEXTURE_CHECK))
				{
					// texture set
					*pPass++ = CODE_TEXTURE | dwAdditionalFlags;
					*pPass++ = dwTextureIndex;
					if (bIn) *pPass++ = dwInParamIndex;
					else
					{
					}
					SKIP3;
				}
				// SetTextureStageState
				dword *pPassCode = pPass;
				*pPass++ = CODE_STSS | dwAdditionalFlags;
				*pPass++ = dwTextureIndex;

				// get left side of expression
				GetTokenWhile(*pStr,temp,"[");
				dword dwSTSSIndex = GetSTSSIndex(temp);
				Assert(dwSTSSIndex!=INVALID_STSS_INDEX);
				if (TexturesStageStates[dwSTSSIndex].bUse && TexturesStageStates[dwSTSSIndex].dwUseSubCode & (1 << dwTextureIndex) && dwAdditionalFlags & FLAGS_CODE_RESTORE)
					api->Trace("WARN: STSS: dup restore param type <%s[%d]> in <%s> file, technique <%s>", temp, dwTextureIndex, sCurrentFileName, sCurrentBlockName);
				if (dwAdditionalFlags & FLAGS_CODE_RESTORE) 
				{
					TexturesStageStates[dwSTSSIndex].bUse = true;
					TexturesStageStates[dwSTSSIndex].dwUseSubCode |= (1 << dwTextureIndex);
				}
				*pPass++ = (DWORD)TexturesStageStates[dwSTSSIndex].State;

				if (bIn) *pPass++ = dwInParamIndex;
				else
				{
					// get right side of expression
					GetTokenWhile(SkipToken(*pStr,"="),temp,";");
					*pPass++ = GetCode(temp,TexturesStageStates[dwSTSSIndex].pParam,TexturesStageStates[dwSTSSIndex].dwParamNum,pPassCode,true);
				}
				SKIP3;
			}

		// PIXEL_SHADER, VERTEX_SHADER
			if (SkipToken(*pStr,PIXEL_SHADER))	
			{
				*pPass++ = CODE_SPS;
				GetTokenWhile(SkipToken(*pStr,"\""),temp,"\"");
				*pPass++ = AddShader(temp);
				SKIP3;
			}

			if (SkipToken(*pStr,VERTEX_SHADER))	
			{
				*pPass++ = CODE_SVS;
				GetTokenWhile(SkipToken(*pStr,"\""),temp,"\"");
				*pPass++ = AddShader(temp);
				SKIP3;
			}

		// RENDER
			if (SkipToken(*pStr,RENDER_CHECK))	
			{
				*pPass++ = CODE_RENDER;
				*pPass++ = SUBCODE_RENDER_DRAW;
				SKIP3;
			}

		// transform
			if (SkipToken(*pStr,TRANSFORM_CHECK))	
			{
				// 
				GetTokenWhile(SkipToken(*pStr,"."),temp,"=");
				dword dwCode = GetCode(temp,TRANSFORM_TYPES,sizeof(TRANSFORM_TYPES)/sizeof(SRSPARAM), false);
				if (dwCode==0xFFFFFFFF)	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				{
					// maybe world0-world256
					if (0==(pTemp = SkipToken(*pStr,WORLD_TRANSFORM_CHECK))) THROW("transform. error!");
					sscanf(pTemp,"%d",&dwCode);
					dwCode = (DWORD)D3DTS_WORLDMATRIX(dwCode);
				}
				*pPass++ = CODE_TRANSFORM;
				*pPass++ = dwCode;
				*pPass++ = dwInParamIndex;
				SKIP3;
			}

		// SetRenderState
			if (SkipToken(*pStr,"="))	
			{
				// get left side of expression
				GetTokenWhile(*pStr, temp, "=");
				dword dwSRSIndex = GetSRSIndex(temp);
				Assert(dwSRSIndex!=INVALID_SRS_INDEX);
				if (RenderStates[dwSRSIndex].bUse && dwAdditionalFlags & FLAGS_CODE_RESTORE)
					api->Trace("WARN: SRS: dup restore param type <%s> in <%s> file, technique <%s>", temp, sCurrentFileName, sCurrentBlockName);
				if (dwAdditionalFlags & FLAGS_CODE_RESTORE) RenderStates[dwSRSIndex].bUse = true;
				dword * pPassCode = pPass;
				*pPass++ = CODE_SRS|dwAdditionalFlags;
				*pPass++ = (DWORD)RenderStates[dwSRSIndex].State;

				if (bIn) *pPass++ = dwInParamIndex;
				else
				{
					// get right side of expression
					GetTokenWhile(SkipToken(*pStr,"="),temp,";");
					*pPass++ = GetCode(temp,RenderStates[dwSRSIndex].pParam,RenderStates[dwSRSIndex].dwParamNum,pPassCode,true);
				}
				SKIP3;
			}
		
		// other skip
			SKIP3;
	}

	dword dwSizePass = pPass - pPassBegin;
	pPassBegin = NEW dword[dwSizePass];//(dword*)RESIZE(pPassBegin,dwSizePass*sizeof(dword));
	memcpy(pPassBegin, pPassStorage, dwSizePass * sizeof(dword));
	pTechniques->pPasses[pTechniques->dwNumPasses].dwSize = dwSizePass;
	pTechniques->pPasses[pTechniques->dwNumPasses].isValidate = true;
	pTechniques->pPasses[pTechniques->dwNumPasses].pPass = pPassBegin;

	pTechniques->dwNumPasses++;
	return PASS_OK;
}

dword CTechnique::ProcessTechnique(char *pFile, dword dwSize, char **pStr)
{
	block_t	*pB = &pBlocks[dwNumBlocks];
	pB->pTechniques = (technique_t*)RESIZE(pB->pTechniques,sizeof(technique_t)*(pB->dwNumTechniques+1));
	PZERO(&pB->pTechniques[pB->dwNumTechniques],sizeof technique_t);
	// clear STSS and SRS bUse
	ClearSRS_STSS_bUse();
	// search for pass and '}'
	while (0 != (*pStr = GetString(pFile,dwSize,*pStr)))
	{
#define SKIP2	{ (*pStr)++; continue; }
		*pStr = _strlwr(*pStr);
		ClearComment(*pStr);
		//if (isComment(*pStr)) SKIP2;
		if (isEndBracket(*pStr)) break;		// end of technique
		if (isPass(*pStr)) ProcessPass(pFile, dwSize, pStr);
		SKIP2;
	}

	pB->dwNumTechniques++;
	return TECHNIQUE_OK;
}

#define TOTAL_SKIP	{ (*pStr)++; continue; }

dword* AddDeclElement(shader_t *pS)
{
	pS->dwDeclSize++;
	pS->pDecl = (dword*)RESIZE(pS->pDecl,sizeof(dword) * pS->dwDeclSize);
	return &pS->pDecl[pS->dwDeclSize-1];
}

dword CTechnique::ProcessVertexDeclaration(shader_t *pS, char *pFile, dword dwSize, char **pStr)
{
	dword dwTemp,dwTemp1;
	while (0 != (*pStr = GetString(pFile,dwSize,*pStr)))
	{
		*pStr = _strlwr(*pStr);
		ClearComment(*pStr);
		//if (isComment(*pStr)) SKIP2;
		if (isEndBracket(*pStr)) break;		// end of declaration
		if (SkipToken(*pStr,VDECL_STREAM_CHECK))
		{
			sscanf(SkipToken(*pStr,"["),"%d",&dwTemp);
			*AddDeclElement(pS) = D3DVSD_STREAM(dwTemp);
		}
		if (SkipToken(*pStr,VDECL_FLOAT_CHECK))
		{
			sscanf(SkipToken(*pStr,"v"),"%d",&dwTemp1);
			sscanf(SkipToken(*pStr,"["),"%d",&dwTemp);
			switch (dwTemp)
			{
				case 1: dwTemp = D3DVSDT_FLOAT1; break;
				case 2: dwTemp = D3DVSDT_FLOAT2; break;
				case 3: dwTemp = D3DVSDT_FLOAT3; break;
				case 4: dwTemp = D3DVSDT_FLOAT4; break;
			}
			*AddDeclElement(pS) = D3DVSD_REG(dwTemp1, dwTemp);
		}
		if (SkipToken(*pStr,VDECL_COLOR_CHECK))
		{
			sscanf(SkipToken(*pStr,"v"),"%d",&dwTemp1);
			*AddDeclElement(pS) = D3DVSD_REG(dwTemp1, D3DVSDT_D3DCOLOR);
		}
		if (SkipToken(*pStr,VDECL_UBYTE4_CHECK))
		{
			sscanf(SkipToken(*pStr,"v"),"%d",&dwTemp1);
#ifndef _XBOX
			*AddDeclElement(pS) = D3DVSD_REG(dwTemp1, D3DVSDT_UBYTE4);
#endif
		}
		TOTAL_SKIP;
	}
	*AddDeclElement(pS) = D3DVSD_END();
	return 0;
}

char *CTechnique::GetToken(char *pToken, char *pResult, bool & bToken)
{
	char *pStart = pToken;
	bToken = false;
	while (*pToken && !sDelimTable[*pToken]) pToken++;
	pResult[pToken - pStart] = 0;
	if (pToken - pStart == 0 && *pToken == 0) return null;
	if (pToken - pStart == 0 && sDelimTable[*pToken]) 
	{
		bToken = true;
		pResult[0] = *pToken;
		pResult[1] = 0; 
		return pToken+1;
	}
	strncpy(pResult,pStart,pToken - pStart);
	return pToken;
}

char *CTechnique::Preprocessor(char *pBuffer, dword & dwSize)
{
	bool		bToken;
	char		sValue[256], sName[256], sToken[1024];
	char		*pStr = pBuffer;
	dword		dwBufSize = strlen(pBuffer);
	dword		i, dwNumDefines = 0;
	char		* pProgram = null;
	dword		dwProgramSize = 0;
	define_t	* pDefines = null;
	dword		dwMaxDefineElements = 0;
	dword		dwMaxProgramElements = 0;
	dword		dwNewSize;

	// change all 0xd 0xa to 0x0
	for (i=0;i<dwSize;i++) 
	{
		if (pBuffer[i] == 0xa || pBuffer[i] == 0xd) pBuffer[i] = 0;
		if (pBuffer[i] == 0x9) pBuffer[i] = ' ';
	}

	while (0 != (pStr = GetString(pBuffer,dwBufSize,pStr)))
	{
		//pStr = _strlwr(pStr);
		ClearComment(pStr);
		if (isDefine(pStr))
		{
			GetTokenWhile(SkipToken(pStr,DEFINE),sName," ");
			GetTokenWhile(SkipToken(pStr,sName),sValue," ");
			dwNewSize = 16 * ((dwNumDefines / 16) + 1);
			if (dwMaxDefineElements != dwNewSize)
			{
				dwMaxDefineElements = dwNewSize;
				pDefines = (define_t*)RESIZE(pDefines,sizeof(define_t) * dwMaxDefineElements);
			}
			COPY_STRING(pDefines[dwNumDefines].pName,sName);
			COPY_STRING(pDefines[dwNumDefines].pValue,sValue);
			pDefines[dwNumDefines].dwNameLen = strlen(sName);
			dwNumDefines++;
			pStr++; continue;
		}
		// else real code string
		bool bNewString = false;
		char *pToken = pStr;
		while(0 != (pToken = GetToken(pToken,sToken,bToken)))
		{
			dword dwTempLen = strlen(sToken);
			if (!bToken)
			{
				// find define
				for (i=0;i<dwNumDefines;i++) if (dwTempLen==pDefines[i].dwNameLen && stricmp(pDefines[i].pName,sToken)==0)
				{
					if (pDefines[i].pValue) strcpy(sToken,pDefines[i].pValue);
					dwTempLen = strlen(sToken);
					break;
				}
			}
			// add sToken to programm
			dwNewSize = 8192 * (((dwTempLen + dwProgramSize + 4) / 8192) + 1);
			if (dwNewSize > dwMaxProgramElements)
			{
				dwMaxProgramElements = dwNewSize;
				pProgram = (char*)RESIZE(pProgram,dwMaxProgramElements);
			}
			strcpy(&pProgram[dwProgramSize],sToken);
			dwProgramSize += dwTempLen;
			bNewString = true;
		}
		if (pProgram && bNewString)
		{
			pProgram[dwProgramSize] = 0xd;
			pProgram[dwProgramSize+1] = 0xa;
			pProgram[dwProgramSize+2] = 0x0;
			dwProgramSize += 2;
		}
		pStr++; continue;
	}
	DELETE(pBuffer);
	if (pProgram) pProgram[dwProgramSize] = 0x0;
	dwSize = dwProgramSize;
	//api->Trace(pProgram);
	for (i=0;i<dwNumDefines;i++)
	{
		DELETE(pDefines[i].pName);
		DELETE(pDefines[i].pValue);
	}
	DELETE(pDefines);

	return pProgram;
}

dword CTechnique::ProcessShaderAsm(shader_t * pS, char *pFile, dword dwSize, char **pStr, dword dwShaderType)
{
	dword dwFileSize;
	dword dwTotalLen = 0;
	char *pBuffer = null;
	while (0 != (*pStr = GetString(pFile,dwSize,*pStr)))
	{
		//*pStr = _strlwr(*pStr);
		ClearComment(*pStr);
		if (isBeginBracket(*pStr)) TOTAL_SKIP;
		if (isEndBracket(*pStr)) break;		// end of declaration

		char *pTemp = *pStr;
		char *pTempBuffer = null;

		if (isInclude(*pStr))
		{
			char sIncFileName[256],sName[256];
			GetTokenWhile(SkipToken(*pStr,"\""),&sName[0],"\"");
			sprintf(sIncFileName,"%s\\%s",sCurrentDir,sName);
			if (!fio->LoadFile(sIncFileName,&pTempBuffer,&dwFileSize))
			{
				api->Trace("ERROR: in file %s, file not found : %s", sCurrentFileName, sIncFileName);
				TOTAL_SKIP;
			}
			pTempBuffer[dwFileSize-1] = 0x0;
			pTemp = pTempBuffer;
		}

		long iLen = strlen(pTemp)-1;
		pBuffer = (char*)RESIZE(pBuffer,dwTotalLen + iLen + 4);
		strcpy(&pBuffer[dwTotalLen],pTemp);
		strcpy(&pBuffer[dwTotalLen+iLen+1],"\r\n\0");
		dwTotalLen += iLen + 2;
		DELETE(pTempBuffer);
		TOTAL_SKIP;
	}	
#ifndef _XBOX
	ID3DXBuffer * CompiledShader = null, * ErrorShader = null;
#else
	LPXGBUFFER	CompiledShader = null, ErrorShader = null;
#endif
	HRESULT hr;
	pBuffer = Preprocessor(pBuffer, dwTotalLen);
#ifndef _XBOX
	hr = D3DXAssembleShader(pBuffer, dwTotalLen, 0, 0, &CompiledShader, &ErrorShader);
#else
	hr = XGAssembleShader(0, pBuffer, dwTotalLen, 0, 0, &CompiledShader, &ErrorShader, 0, 0, 0, 0);//D3DXAssembleShader(pBuffer,dwTotalLen,0,0,&CompiledShader,&ErrorShader);
#endif
	if (D3D_OK != hr && ErrorShader)
	{
		char * pErrStr = NEW char[ErrorShader->GetBufferSize() + 1];
		strncpy(pErrStr, (char*)ErrorShader->GetBufferPointer(), ErrorShader->GetBufferSize());
		pErrStr[ErrorShader->GetBufferSize()] = 0;
		api->Trace("ERROR: in compile shader %s\nerror:\n%s", pS->pName, pErrStr);
		RELEASE(CompiledShader);
		RELEASE(ErrorShader);
		DELETE(pBuffer);
		DELETE(pErrStr);
		return 0;
	}
	if (dwShaderType == CODE_SVS)
		hr = pRS->CreateVertexShader(pS->pDecl,(const dword*)CompiledShader->GetBufferPointer(),&pS->dwShaderHandle,0);
	else
	{
#ifndef _XBOX
		hr = pRS->CreatePixelShader((dword*)CompiledShader->GetBufferPointer(),&pS->dwShaderHandle);
#else
		hr = pRS->CreatePixelShader((dword*)(D3DPIXELSHADERDEF *)CompiledShader->GetBufferPointer(),&pS->dwShaderHandle);
#endif
	}
	RELEASE(CompiledShader);
	RELEASE(ErrorShader);
	DELETE(pBuffer);
	return 0;
}

dword CTechnique::ProcessVertexShader(char *pFile, dword dwSize, char **pStr)
{
	char	temp[1024];

	// add shader
	char *pName = SkipToken(*pStr,VERTEX_SHADER);
	GetTokenWhile(pName,&temp[0]," ");

	shader_t *pS = &pShaders[AddShader(temp)];
	pS->dwShaderType = CODE_SVS;

	while (0 != (*pStr = GetString(pFile,dwSize,*pStr)))
	{
		*pStr = _strlwr(*pStr);
		ClearComment(*pStr);
		if (isEndBracket(*pStr)) break;		// end of vertex shader
		if (isVertexDeclaration(*pStr)) ProcessVertexDeclaration(pS, pFile, dwSize, pStr);
		if (isAsm(*pStr)) ProcessShaderAsm(pS, pFile, dwSize, pStr, CODE_SVS);
		TOTAL_SKIP;
	}

	return SHADER_OK;
}

dword CTechnique::ProcessPixelShader(char *pFile, dword dwSize, char **pStr)
{
	char	temp[1024];

	// add shader
	char *pName = SkipToken(*pStr,PIXEL_SHADER);
	GetTokenWhile(pName,&temp[0]," ");

	shader_t *pS = &pShaders[AddShader(temp)];
	pS->dwShaderType = CODE_SPS;

	while (0 != (*pStr = GetString(pFile,dwSize,*pStr)))
	{
		*pStr = _strlwr(*pStr);
		ClearComment(*pStr);
		if (isEndBracket(*pStr)) break;		// end of vertex shader
		if (isAsm(*pStr)) ProcessShaderAsm(pS, pFile, dwSize, pStr, CODE_SPS);
		TOTAL_SKIP;
	}

	return SHADER_OK;
}

dword CTechnique::ProcessBlock(char * pFile, dword dwSize, char **pStr)
{
	dword	i;
	char	temp[1024], pTempParamStr[1024];

	// resize
	pBlocks = (block_t*)RESIZE(pBlocks,sizeof(block_t)*(dwNumBlocks+1));

	block_t	* pB = &pBlocks[dwNumBlocks];
	PZERO(pB,sizeof block_t);

	dwNumParams = 0;

	// get name this block
		char *pName = SkipToken(*pStr,BLOCK);
		strcpy(sCurrentBlockName,pName);
		GetTokenWhile(pName,&temp[0],"(");
		pB->dwHashBlockName = hash_string(temp);
		COPY_STRING(pB->pBlockName,temp);
		for (i=0;i<dwNumBlocks;i++) 
			if (pBlocks[i].dwHashBlockName == pB->dwHashBlockName && (stricmp(pBlocks[i].pBlockName,pBlocks[i].pBlockName)==0)) 
			{
				api->Trace("ERROR: Techniques: Find duplicate technique name: %s",pB->pBlockName);
				break;
			}

		htBlocks.Add(pB->pBlockName, dwNumBlocks);
	// get parameters
		pTempParamStr[0] = 0;
		GetTokenWhile(SkipToken(*pStr,"("),&pTempParamStr[0],")");
		char *pParamStr = &pTempParamStr[0];
		while(true)
		{
			// get type
			pParamStr = GetTokenWhile(pParamStr,&temp[0]," ");
			if (!pParamStr[0]) break;
			pParams = (SRSPARAM*)RESIZE(pParams,sizeof(SRSPARAM)*(dwNumParams+1));
			pB->pParams = (dword*)RESIZE(pB->pParams,sizeof(dword)*(dwNumParams+1));
			pParams[dwNumParams].dwCode = GetCode(&temp[0],PARAM_TYPES,sizeof(PARAM_TYPES) / sizeof(SRSPARAM));
			pB->pParams[dwNumParams] = pParams[dwNumParams].dwCode;
			// get parameter name
			pParamStr = GetTokenWhile(pParamStr,&temp[0],",");
			pParams[dwNumParams].cName = NEW char[strlen(temp)+1];
			strcpy(pParams[dwNumParams].cName,temp);
			dwNumParams++;
			pB->dwNumParams++;
		}
	
	// search for technique and '}'
	while (0 != (*pStr = GetString(pFile,dwSize,*pStr)))
	{
#define SKIP1	{ (*pStr)++; continue; }
		*pStr = _strlwr(*pStr);
		ClearComment(*pStr);
		//if (isComment(*pStr)) SKIP1;
		if (isEndBracket(*pStr)) break;		// end of block
		if (isTechnique(*pStr)) ProcessTechnique(pFile, dwSize, pStr);
		SKIP1;
	}

	// delete parameters
	for (i=0;i<dwNumParams;i++) DELETE(pParams[i].cName);
	dwNumParams = 0;
	DELETE(pParams);

	sCurrentBlockName[0] = 0;
	
	return BLOCK_OK;
}

void CTechnique::ClearSRS_STSS_bUse()
{
	dword i, dwNumParam;

	dwNumParam = sizeof(RenderStates) / sizeof(SRS);
	for (i=0; i<dwNumParam; i++) RenderStates[i].bUse = false;

	dwNumParam = sizeof(TexturesStageStates) / sizeof(STSS);
	for (i=0; i<dwNumParam; i++) 
	{
		TexturesStageStates[i].bUse = false;
		TexturesStageStates[i].dwUseSubCode = 0;
	}
}

void CTechnique::DecodeFiles(char *sub_dir)
{
	dword dwRDTSC;
	RDTSC_B(dwRDTSC);
	pPassStorage = NEW dword[16384];

	InnerDecodeFiles(sub_dir);
	
	DELETE(pPassStorage);
	RDTSC_E(dwRDTSC);
	api->Trace("Techniques: %d shaders compiled.",dwNumShaders);
	api->Trace("Techniques: %d techniques compiled.",dwNumBlocks);
	api->Trace("Techniques: compiled by %d ticks.",dwRDTSC);

	// some optimize
	for (dword i=0; i<dwNumBlocks; i++) DELETE(pBlocks[i].pBlockName);
}

void CTechnique::InnerDecodeFiles(char *sub_dir)
{
	char				file_mask[256];
	WIN32_FIND_DATA		wfd;

	sprintf(sCurrentDir,"%s%s",SHA_DIR,(sub_dir) ? sub_dir : "");
	sprintf(file_mask,"%s\\*.*",sCurrentDir);

	HANDLE hFile = fio->_FindFirstFile(file_mask,&wfd);
	if (hFile != INVALID_HANDLE_VALUE) 
	{
		do 
		{ 
			if (wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				if (wfd.cFileName[0] == '.') continue;
				sprintf(file_mask,"%s\\%s",(sub_dir) ? sub_dir : "",wfd.cFileName);
				InnerDecodeFiles(file_mask);
			}	
			else
			{
				if (SkipToken(wfd.cFileName,SHA_EXT)) 
				{
					sprintf(file_mask,"%s\\%s",(sub_dir) ? sub_dir : "",wfd.cFileName);
					DecodeFile(file_mask); 
				}
			}
		} while (fio->_FindNextFile(hFile,&wfd));

		fio->_FindClose(hFile);
	}
}

bool CTechnique::DecodeFile(char *sname)
{
	char fname[256];

	sprintf(fname,"%s%s",SHA_DIR,sname);
	strcpy(sCurrentFileName,sname);
	HANDLE hFile = fio->_CreateFile(fname,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
	dword dwSize = fio->_GetFileSize(hFile,0);
	Assert(dwSize!=0);
	char *pFile = NEW char[dwSize];
	fio->_ReadFile(hFile,pFile,dwSize,0);
	fio->_CloseHandle(hFile);

	// change 0xd and 0xa to 0x0
	for (dword i=0;i<dwSize;i++) 
	{
		if (pFile[i]==0xD || pFile[i]==0xA) pFile[i] = 0x0;
		if (pFile[i]==0x9) pFile[i] = ' ';
	}

	char *pStr = pFile;

	//dwCurBlock = 0;

	while (0 != (pStr = GetString(pFile, dwSize, pStr)))
	{
#define SKIP	{ pStr++; continue; }
		pStr = _strlwr(pStr);
		ClearComment(pStr);
		//if (isComment(pStr)) SKIP;
		if (isPixelShader(pStr))
		{
			dword dwRes = ProcessPixelShader(pFile, dwSize, &pStr);
			if (dwRes == SHADER_ERROR) return false;
		}
		if (isVertexShader(pStr))
		{
			dword dwRes = ProcessVertexShader(pFile, dwSize, &pStr);
			if (dwRes == SHADER_ERROR) return false;
		}
		if (isBlock(pStr))
		{
			dword dwRes = ProcessBlock(pFile, dwSize, &pStr);
			if (dwRes == BLOCK_ERROR) return false;
			dwNumBlocks++;
		}
		SKIP;
	}

	DELETE(pFile);
	return true;		
}

// return true for drawbuffer, and false for exit
bool CTechnique::ExecutePassStart()	
{
	// search block
	if (!htBlocks.Find(dwHashCode, sCurrentBlockName, dwCurBlock)) return false;
	//for (dword i=0;i<dwNumBlocks;i++) if (pBlocks[i].dwHashBlockName == dwHashCode && (stricmp(pBlocks[i].pBlockName,sCurrentBlockName)==0)) break;
	//if (i==dwNumBlocks) return false;	// don't find concur block
	//Assert(i == dwFind);
	//dwCurBlock = i;

	block_t *pB = &pBlocks[dwCurBlock];
	if (pB->dwNumTechniques==0) return false; // not found any techniques
	
	// search first validated technique and execute
	// while execute technique 0
	dwCurTechnique = 0;
	dwCurNumPass = pB->pTechniques[dwCurTechnique].dwNumPasses;
	if (dwCurNumPass==0) return false; // not found any passes in techniques

	dwCurPass = 0;

	// clear old saved states 
	ClearSavedStates();

	return ExecutePass(true);
}

// return true for drawbuffer, and false for exit
bool CTechnique::ExecutePassNext()	
{
	bool bExecute = ExecutePass(false);
	if (bExecute) return true;	// if once again render in these pass
	dwCurPass++;
	return ExecutePass(true);
}

bool CTechnique::ExecutePassClose()	
{
	ClearSavedStates();
	pRS->SetPixelShader(0);
	return true;
}

dword CTechnique::GetPassParameter(dword dwParam, dword dwFlags)
{
	if (dwFlags&FLAGS_CODE_IN_PARAM) 
	{
		if (dwParam>=dwCurNumParams) THROW("Technique: Exceeded parameter numbers!");
		return pCurParams[dwParam];
	}

	return dwParam;
}

bool CTechnique::ExecutePass(bool bStart)	
{
	dword	dwStage, dwValue, dwSaveValue;
	dword	dwCode, dwSubCode, dwRenderSubCode, dwRestoreSubCode;
	bool	bSave;

	if (dwCurPass >= dwCurNumPass) 
	{
		ExecutePassClose();
		return false;
	}

	pass_t *pCurPass = &pBlocks[dwCurBlock].pTechniques[dwCurTechnique].pPasses[dwCurPass];

	if (!bStart && dwCurPassSize==dwCurPassPos) return false;
	if (bStart)
	{
		dwCurPassPos = 0;
		dwCurPassSize = pCurPass->dwSize;
	}
	// begin or continue execute current pass
	dword *pPassStart = &pCurPass->pPass[0];
	dword *pPass = &pCurPass->pPass[dwCurPassPos];

	while (dword(pPass - pPassStart)<dwCurPassSize)
	{
		dwCode = (*pPass++);
		dwSubCode = dwCode&0xFFFF0000;
		dwCode &= 0xFFFF;
		bSave = (dwSubCode&FLAGS_CODE_RESTORE) ? true : false;
		
		switch(dwCode)
		{
// SRS	type:	CODE_SRS,State index,value
// STSS type:	CODE_STSS,Stage,State index,value
// RENDER:		CODE_RENDER,SUBCODE_RENDER
// TRANSFOR:	CODE_TRANSFORM,D3DTRANSFORMSTATETYPE,Value
// TEXTURE:		CODE_TEXTURE,Stage,Value
// RESTORE:		CODE_RESTORE
			case CODE_SRS:			// SetRenderState(
				{
					D3DRENDERSTATETYPE State = (D3DRENDERSTATETYPE)(*pPass++);
					dwValue = GetPassParameter(*pPass++,dwSubCode);
					if (bSave)
					{
						pRS->GetRenderState(State,&dwSaveValue);
						if (dwSaveValue == dwValue) break;
						AddState2Restore3(dwCode,State,dwSaveValue);
					}
					pRS->SetRenderState(State,dwValue);
				}
			break;
			case CODE_STSS:			// SetTextureStageState(
				{
					dwStage = *pPass++;
					D3DTEXTURESTAGESTATETYPE StageState = (D3DTEXTURESTAGESTATETYPE)(*pPass++);
					dwValue = GetPassParameter(*pPass++,dwSubCode);
					if (bSave)
					{
						pRS->GetTextureStageState(dwStage,StageState,&dwSaveValue);
						if (dwSaveValue == dwValue) break;
						AddState2Restore3(dwCode,dwStage,StageState);
						AddState2Restore(dwSaveValue);
					}
					pRS->SetTextureStageState(dwStage,StageState,dwValue);
				}
			break;
			case CODE_TEXTURE:		// SetTexture(
				{
					dword dwTextureIndex = *pPass++;
					long iTextureIndex = (long)*pPass++;
					if (iTextureIndex>=0) iTextureIndex = (long)GetPassParameter(*pPass++,dwSubCode); 
					pRS->TextureSet(dwTextureIndex,iTextureIndex);
				}
			break;
			case CODE_TRANSFORM:	// SetTransform(
				{
					D3DTRANSFORMSTATETYPE TransformType = (D3DTRANSFORMSTATETYPE)*pPass++;
					pRS->SetTransform(TransformType,(D3DMATRIX*)GetPassParameter(*pPass++,dwSubCode));
				}
			break;
			case CODE_RENDER:		// Render functions
				{
					dwRenderSubCode = *pPass++;
					switch(dwRenderSubCode)
					{
						case SUBCODE_RENDER_DRAW:
							dwCurPassPos = pPass - pPassStart;
							return true;
						break;
					}
				}
			break;
			case CODE_SVS:
				{
					dword dwShaderIndex = *pPass++;
					if (bSave)
					{
						pRS->GetVertexShader(&dwSaveValue);
						AddState2Restore2(dwCode,dwSaveValue);
					}
					if (dwShaderIndex != INVALID_SHADER_INDEX && pShaders[dwShaderIndex].dwShaderHandle != INVALID_SHADER_HANDLE)
						pRS->SetVertexShader(pShaders[dwShaderIndex].dwShaderHandle);
				}
			break;
			case CODE_SPSCONST:
				{
					dword dwShaderConstIndex = *pPass++;
					pRS->SetPixelShaderConstant(dwShaderConstIndex, (const void *)GetPassParameter(*pPass++, dwSubCode), 1);
				}
			break;
			case CODE_SVSCONST:
				{
					dword dwShaderConstIndex = *pPass++;
					dword dwCode = *pPass++;
					switch (dwCode)
					{
						case SUBCODE_SVSCONST_WORLDVIEWPROJ:
						{
							D3DXMATRIX matTemp, matWorldViewProj;

							D3DXMATRIX matWorld; pRS->GetTransform(D3DTS_WORLD, &matWorld);
							D3DXMATRIX matProj; pRS->GetTransform(D3DTS_PROJECTION, &matProj);
							D3DXMATRIX matView; pRS->GetTransform(D3DTS_VIEW, &matView);

							D3DXMatrixMultiply(&matTemp, &matWorld, &matView);
							D3DXMatrixMultiply(&matWorldViewProj, &matTemp, &matProj);
							
							// Projection to clip space
							D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
							pRS->SetVertexShaderConstant(dwShaderConstIndex, &matWorldViewProj(0, 0), 4);
						}
						break;
					}
				}
			break;
			case CODE_SPS:
				{
					dword dwShaderIndex = *pPass++;
					if (bSave)
					{
						pRS->GetPixelShader(&dwSaveValue);
						AddState2Restore2(dwCode,dwSaveValue);
					}
					if (dwShaderIndex != INVALID_SHADER_INDEX && pShaders[dwShaderIndex].dwShaderHandle != INVALID_SHADER_HANDLE)
						pRS->SetPixelShader(pShaders[dwShaderIndex].dwShaderHandle);
				}
			break;
			case CODE_RESTORE:		// restore states 
				dwRestoreSubCode = *pPass++;
				switch(dwRestoreSubCode)
				{
					case SUBCODE_RESTORE_CLEAR:
						ClearSavedStates();
					break;
					case SUBCODE_RESTORE_STATES:
						RestoreSavedStates();
					break;
				}
			break;
		}
	}
	
	dwCurPassPos = pPass - pPassStart;
	return false;
}

void inline CTechnique::AddState2Restore2(dword dwState, dword dw1)
{
	// add new memory if need
	if ((dwCurSavedStatesPos+2) >= dwCurMaxSavedSize / sizeof(pSavedStates[0]))
	{
		pSavedStates = (dword*)RESIZE(pSavedStates,dwCurMaxSavedSize + SAVED_STATES_ADD * sizeof(pSavedStates[0]));
		dwCurMaxSavedSize += SAVED_STATES_ADD;
	}

	// save state (or part of state)
	pSavedStates[dwCurSavedStatesPos++] = dwState;
	pSavedStates[dwCurSavedStatesPos++] = dw1;
}

void inline CTechnique::AddState2Restore3(dword dwState, dword dw1, dword dw2)
{
	// add new memory if need
	if ((dwCurSavedStatesPos+3) >= dwCurMaxSavedSize / sizeof(pSavedStates[0]))
	{
		pSavedStates = (dword*)RESIZE(pSavedStates,dwCurMaxSavedSize + SAVED_STATES_ADD *sizeof(pSavedStates[0]));
		dwCurMaxSavedSize += SAVED_STATES_ADD;
	}

	// save state (or part of state)
	pSavedStates[dwCurSavedStatesPos++] = dwState;
	pSavedStates[dwCurSavedStatesPos++] = dw1;
	pSavedStates[dwCurSavedStatesPos++] = dw2;
}

void inline CTechnique::AddState2Restore(dword dwState)
{
	// add new memory if need
	if (dwCurSavedStatesPos >= dwCurMaxSavedSize / sizeof(pSavedStates[0]))
	{
		pSavedStates = (dword*)RESIZE(pSavedStates,dwCurMaxSavedSize + SAVED_STATES_ADD *sizeof(pSavedStates[0]));
		dwCurMaxSavedSize += SAVED_STATES_ADD;
	}

	// save state (or part of state)
	pSavedStates[dwCurSavedStatesPos++] = dwState;
}

void CTechnique::ClearSavedStates()
{
	dwNumSavedStates = 0;
	dwCurSavedStatesPos = 0;
}

void CTechnique::RestoreSavedStates()
{
	dword dwCode,dwValue;
	// restore saved render states and texture stage states	
	
	// for walking
	dword dwTempSavedStatesPos = 0;

	while (dwTempSavedStatesPos < dwCurSavedStatesPos)
	{
		dwCode = pSavedStates[dwTempSavedStatesPos++];
		switch (dwCode)
		{
			case CODE_SRS:
				{
					D3DRENDERSTATETYPE State = (D3DRENDERSTATETYPE)pSavedStates[dwTempSavedStatesPos++];
					dwValue = pSavedStates[dwTempSavedStatesPos++];
					pRS->SetRenderState(State,dwValue);
				}
			break;
			case CODE_STSS:
				{
					dword dwStage = pSavedStates[dwTempSavedStatesPos++];
					D3DTEXTURESTAGESTATETYPE StageState = (D3DTEXTURESTAGESTATETYPE)pSavedStates[dwTempSavedStatesPos++];
					dwValue = pSavedStates[dwTempSavedStatesPos++];
					pRS->SetTextureStageState(dwStage,StageState,dwValue);
				}
			break;
			case CODE_SPS:
					dwValue = pSavedStates[dwTempSavedStatesPos++];
					pRS->SetPixelShader(dwValue);
			break;
			case CODE_SVS:
					dwValue = pSavedStates[dwTempSavedStatesPos++];
					pRS->SetVertexShader(dwValue);
			break;
		}
	}

	ClearSavedStates();
}

void CTechnique::SetCurrentBlock(const char * name, dword _dwNumParams, void * pParams)
{
	if(name && name[0])
	{
		
	strcpy(sCurrentBlockName,name);	_strlwr(sCurrentBlockName);
	dwHashCode = hash_string(sCurrentBlockName);
	dwCurNumParams = _dwNumParams;

	if (dwCurNumParams > dwCurParamsMax)
	{
		while (dwCurNumParams>dwCurParamsMax) dwCurParamsMax += 10;
		pCurParams = (dword*)RESIZE(pCurParams, sizeof(dword) * dwCurParamsMax);
	}

	for (dword i=0; i<_dwNumParams; i++) 
		pCurParams[i] = ((dword*)pParams)[i];
	}
	else
	{
		api->Trace("ERROR: SetCurrentBlock: unknown technique <%s> first character is <%s> ",name,name[0]);
	}
}
