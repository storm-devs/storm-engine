#ifndef _WIN32 // Effects
#include "technique.h"

#include "core.h"
#include "debug-trap.h"
#include "math_inlines.h"
#include "string_compare.hpp"
#include "vma.hpp"

#include <algorithm>
#include <ranges>

#define USE_FX // Will load techniques from fx files

#ifdef USE_FX
#define SHA_DIR "resource\\techniques"
#define SHA_EXT "*.fx"
#else
#define SHA_DIR "resource\\techniques-sha"
#define SHA_EXT "*.sha"
#endif

#define PASS_OK 0
#define PASS_ERROR 1

#define TECHNIQUE_OK 0
#define TECHNIQUE_ERROR 1

#define BLOCK_OK 0
#define BLOCK_ERROR 1

#define SHADER_OK 0
#define SHADER_ERROR 1

#define COMMENT1 "//"
#define COMMENT2 ";"
#define BLOCK "block"
#define TECHNIQUE "technique"
#define PIXEL_SHADER "pixelshader"
#define VERTEX_SHADER "vertexshader"
#define VERTEX_SHADER_CONST "vertexshaderconstant"
#define PIXEL_SHADER_CONST "pixelshaderconstant"
#define VERTEX_DECL "decl"
#define ASM_SHADER "asm"
#define PASS "pass"
#define INCLUDE "#include"
#define DEFINE "#define"
#define BEGIN_BRACKET "{"
#define END_BRACKET "}"
#define NOSAVE_STATE "!"

#define STSS_CHECK "["
#define TEXTURE_CHECK "texture["
#define RENDER_CHECK "render."
#define INPARAM_CHECK "<"
#define TRANSFORM_CHECK "transform."
#define WORLD_TRANSFORM_CHECK ".world"
#define RESTORE_STATES_CHECK "restore."

#define VDECL_STREAM_CHECK "stream"
#define VDECL_FLOAT_CHECK "float"
#define VDECL_COLOR_CHECK "color"
#define VDECL_UBYTE4_CHECK "ubyte4"

#define DEFSIZE(x) x, (sizeof(x) / sizeof(SRSPARAM)), 0, 0

// codes

#define CODE_SRS 0       // SetRenderState
#define CODE_STSS 1      // SetTextureStageState
#define CODE_TEXTURE 2   // SetTexture
#define CODE_TRANSFORM 3 // SetTransform
#define CODE_RENDER 4    // Render
#define CODE_RESTORE 5   // Restore
#define CODE_SVS 6       // Set vertex shader
#define CODE_SPS 7       // Set pixel shader
#define CODE_SVSCONST 8  // Set vertex shader constant
#define CODE_SPSCONST 9  // Set pixel shader constant
#define CODE_SAMP 10     // SetSamplerState

// SetFVFConstant subcodes
#define SUBCODE_SVSCONST_WORLDVIEWPROJ 0

// render subcodes
#define SUBCODE_RENDER_DRAW 0

// restore subcodes
#define SUBCODE_RESTORE_STATES 100
#define SUBCODE_RESTORE_CLEAR 101

// errors
#define INVALID_INDEX 0xFFFFFFFF
#define INVALID_SRS_INDEX 0xFFFFFFFF
#define INVALID_STSS_INDEX 0xFFFFFFFF
#define INVALID_SHADER_INDEX 0xFFFFFFFF
#define INVALID_SHADER_HANDLE 0xFFFFFFFF

//
#define FLAGS_CODE_RESTORE (0x1 << 0x10)
#define FLAGS_CODE_IN_PARAM (0x1 << 0x11) // use [in] parameter
#define FLAGS_CODE_NUMBER (0x1 << 0x12)   // use [in] parameter

// parameters flags

// common defines
#define SAVED_STATES_ADD 128

namespace
{

inline char *tolwr(char *str)
{
    char *result = str;
    while (*str != '\0')
    {
        *str = tolower(*str);
        str++;
    }
    return result;
}

}

char *SkipToken(char *str, const char *cmp)
{
    int flag;
    char *str1;
    const char *cmp1;

    if (str == nullptr || cmp == nullptr)
        return nullptr;
    cmp1 = cmp;

    if ((str == nullptr) || (cmp == nullptr))
        return nullptr;
    if (strlen(str) == 0)
        return nullptr;

    int lens = strlen(str);
    int lenc = strlen(cmp);
    //	if (strcmp(str,cmp) != 0) return NULL;
    for (int i = 0; i < lens; i++)
    {
        flag = 0;
        str1 = str;
        if (*str == *cmp)
        {
            flag = 1;
            for (int j = 0; j < lenc; j++)
                if (*str++ != *cmp++)
                {
                    flag = 0;
                    break;
                }
            if (!flag)
            {
                cmp = cmp1;
                str = str1;
            }
        }
        if (flag)
            break;
        str++;
    }
    if (flag)
    {
        while (*str == 32)
            str++;
        return str;
    }
    return nullptr;
}

char *FindToken(char *str, const char *cmp)
{
    int flag;
    char *str1;
    const char *cmp1;

    if (str == nullptr || cmp == nullptr)
        return nullptr;
    cmp1 = cmp;

    if ((str == nullptr) || (cmp == nullptr))
        return nullptr;
    if (strlen(str) == 0)
        return nullptr;

    int lens = strlen(str);
    int lenc = strlen(cmp);
    //	if (strcmp(str,cmp) != 0) return NULL;
    for (int i = 0; i < lens; i++)
    {
        flag = 0;
        str1 = str;
        if (*str == *cmp)
        {
            flag = 1;
            for (int j = 0; j < lenc; j++)
                if (*str++ != *cmp++)
                {
                    flag = 0;
                    break;
                }
            if (!flag)
            {
                cmp = cmp1;
                str = str1;
            }
        }
        if (flag)
            break;
        str++;
    }
    if (flag)
        return str;
    return nullptr;
}

// Get token before symbol skip
char *GetTokenWhile(char *src, char *dst, const char *skip)
{
    if (!src || !dst || !skip)
        return nullptr;
    dst[0] = 0;
    char *dst1 = dst;
    int32_t lensrc = strlen(src);
    int32_t dstlen = 0;
    while (lensrc && src[0] == ' ')
    {
        src++;
        lensrc--;
    };
    while ((lensrc--) && (*src != *skip))
    {
        *dst1++ = *src++;
        dstlen++;
    }
    *dst1++ = 0;
    char *pRet = ((lensrc > 0) ? ++src : src);
    if (lensrc)
        lensrc--;
    while (lensrc && pRet[0] == 32)
    {
        pRet++;
        lensrc--;
    };
    while (dstlen && dst[dstlen - 1] == ' ')
    {
        dstlen--;
        dst[dstlen] = 0;
    }
    return pRet;
}

typedef struct
{
    SRSPARAM *pParam;
    uint32_t dwParamNum;
    bool bUse;
    int32_t dwUseSubCode;
    uint32_t SrsType;
    const char *cName;
    D3DRENDERSTATETYPE State;
} SRS;

typedef struct
{
    SRSPARAM *pParam;
    uint32_t dwParamNum;
    bool bUse;
    int32_t dwUseSubCode;
    uint32_t SrsType;
    const char *cName;
    D3DTEXTURESTAGESTATETYPE State;
} STSS;

typedef struct
{
    SRSPARAM *pParam;
    uint32_t dwParamNum;
    bool bUse;
    int32_t dwUseSubCode;
    uint32_t SrsType;
    const char *cName;
    D3DSAMPLERSTATETYPE State;
} SAMP;

#define SRS_DWORD 0
#define SRS_FLOAT 1

// ---------------------------------------------------------------------------------

SRSPARAM MYDECLUSAGE[] = {{"position", D3DDECLUSAGE_POSITION},
                          {"blendweight", D3DDECLUSAGE_BLENDWEIGHT},
                          {"blendindices", D3DDECLUSAGE_BLENDINDICES},
                          {"normal", D3DDECLUSAGE_NORMAL},
                          {"psize", D3DDECLUSAGE_PSIZE},
                          {"texcoord", D3DDECLUSAGE_TEXCOORD},
                          {"tangent", D3DDECLUSAGE_TANGENT},
                          {"binormal", D3DDECLUSAGE_BINORMAL},
                          {"tessfactor", D3DDECLUSAGE_TESSFACTOR},
                          {"positiont", D3DDECLUSAGE_POSITIONT},
                          {"color", D3DDECLUSAGE_COLOR},
                          {"fog", D3DDECLUSAGE_FOG},
                          {"depth", D3DDECLUSAGE_DEPTH},
                          {"sample", D3DDECLUSAGE_SAMPLE}};

SRSPARAM MYSETVERTEXSHADERCONSTANT[] = {{"worldviewproj", SUBCODE_SVSCONST_WORLDVIEWPROJ}};

// SetRenderState parameters
SRSPARAM MYD3DZB[] = {{"false", D3DZB_FALSE}, {"true", D3DZB_TRUE}, {"usew", D3DZB_USEW}};

SRSPARAM MYD3DFILL[] = {{"point", D3DFILL_POINT}, {"wireframe", D3DFILL_WIREFRAME}, {"solid", D3DFILL_SOLID}};

SRSPARAM MYD3DSHADE[] = {{"flat", D3DSHADE_FLAT},
                         {"gouraud", D3DSHADE_GOURAUD}
#ifndef _XBOX
                         ,
                         {"phong", D3DSHADE_PHONG}
#endif
};

SRSPARAM MYD3DBLEND[] = {{"zero", D3DBLEND_ZERO},
                         {"one", D3DBLEND_ONE},
                         {"srccolor", D3DBLEND_SRCCOLOR},
                         {"invsrccolor", D3DBLEND_INVSRCCOLOR},
                         {"srcalpha", D3DBLEND_SRCALPHA},
                         {"invsrcalpha", D3DBLEND_INVSRCALPHA},
                         {"destalpha", D3DBLEND_DESTALPHA},
                         {"invdestalpha", D3DBLEND_INVDESTALPHA},
                         {"destcolor", D3DBLEND_DESTCOLOR},
                         {"invdestcolor", D3DBLEND_INVDESTCOLOR},
                         {"srcalphasat", D3DBLEND_SRCALPHASAT},
#ifndef _XBOX
                         {"bothsrcalpha", D3DBLEND_BOTHSRCALPHA},
                         {"bothinvsrcalpha", D3DBLEND_BOTHINVSRCALPHA}
#endif
};

SRSPARAM MYD3DCULL[] = {{"none", D3DCULL_NONE}, {"cw", D3DCULL_CW}, {"ccw", D3DCULL_CCW}};

SRSPARAM MYD3DBLENDOP[] = {{"add", D3DBLENDOP_ADD},
                           {"subtract", D3DBLENDOP_SUBTRACT},
                           {"revsubtract", D3DBLENDOP_REVSUBTRACT},
                           {"min", D3DBLENDOP_MIN},
                           {"max", D3DBLENDOP_MAX}};

SRSPARAM MYD3DCMPFUNC[] = {{"never", D3DCMP_NEVER},
                           {"less", D3DCMP_LESS},
                           {"equal", D3DCMP_EQUAL},
                           {"lessequal", D3DCMP_LESSEQUAL},
                           {"greater", D3DCMP_GREATER},
                           {"notequal", D3DCMP_NOTEQUAL},
                           {"greaterequal", D3DCMP_GREATEREQUAL},
                           {"always", D3DCMP_ALWAYS}};

SRSPARAM MYD3DFOGMODE[] = {
    {"none", D3DFOG_NONE}, {"exp", D3DFOG_EXP}, {"exp2", D3DFOG_EXP2}, {"linear", D3DFOG_LINEAR}};

SRSPARAM MYD3DSTENCILOP[] = {{"keep", D3DSTENCILOP_KEEP},       {"zero", D3DSTENCILOP_ZERO},
                             {"replace", D3DSTENCILOP_REPLACE}, {"incrsat", D3DSTENCILOP_INCRSAT},
                             {"decrsat", D3DSTENCILOP_DECRSAT}, {"invert", D3DSTENCILOP_INVERT},
                             {"incr", D3DSTENCILOP_INCR},       {"decr", D3DSTENCILOP_DECR}};

SRSPARAM MYD3DMATERIALCOLORSOURCE[] = {
    {"material", D3DMCS_MATERIAL}, {"color1", D3DMCS_COLOR1}, {"color2", D3DMCS_COLOR2}};

SRSPARAM MYD3DVERTEXBLENDFLAGS[] = {
    {"disable", D3DVBF_DISABLE},   {"1weights", D3DVBF_1WEIGHTS}, {"2weights", D3DVBF_2WEIGHTS},
    {"3weights", D3DVBF_3WEIGHTS},
#ifndef _XBOX
    {"tweening", D3DVBF_TWEENING}, {"0WEIGHTS ", D3DVBF_0WEIGHTS}
#endif
};

SRSPARAM MYD3DPATCHEDGESTYLE[] = {{"discrete", D3DPATCHEDGE_DISCRETE}, {"continuous", D3DPATCHEDGE_CONTINUOUS}};

SRSPARAM MYD3DCOLORWRITEMASK[] = {{"alpha", D3DCOLORWRITEENABLE_ALPHA},
                                  {"red", D3DCOLORWRITEENABLE_RED},
                                  {"green", D3DCOLORWRITEENABLE_GREEN},
                                  {"blue", D3DCOLORWRITEENABLE_BLUE}};

SRSPARAM MYD3DWRAP[] = {{"wrapcoord0", D3DWRAPCOORD_0}, {"wrapcoord1", D3DWRAPCOORD_1}, {"wrapcoord2", D3DWRAPCOORD_2},
                        {"wrapcoord3", D3DWRAPCOORD_3}, {"wrapu", D3DWRAP_U},           {"wrapv", D3DWRAP_V}};

SRSPARAM MYTRUEFALSE[] = {{"true", true}, {"false", false}};

SRS RenderStates[] = {
    {DEFSIZE(MYD3DZB), SRS_DWORD, "ZEnable", D3DRS_ZENABLE},
    {DEFSIZE(MYD3DFILL), SRS_DWORD, "FillMode", D3DRS_FILLMODE},
    {DEFSIZE(MYD3DSHADE), SRS_DWORD, "ShadeMode", D3DRS_SHADEMODE},
#ifndef _XBOX
//{ DEFSIZE(0) , SRS_DWORD, "LinePattern",						D3DRS_LINEPATTERN },
#endif
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "ZWriteEnable", D3DRS_ZWRITEENABLE},
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "AlphaTestEnable", D3DRS_ALPHATESTENABLE},
#ifndef _XBOX
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "LastPixel", D3DRS_LASTPIXEL},
#endif
    {DEFSIZE(MYD3DBLEND), SRS_DWORD, "SrcBlend", D3DRS_SRCBLEND},
    {DEFSIZE(MYD3DBLEND), SRS_DWORD, "DestBlend", D3DRS_DESTBLEND},
    {DEFSIZE(MYD3DCULL), SRS_DWORD, "CullMode", D3DRS_CULLMODE},
    {DEFSIZE(MYD3DCMPFUNC), SRS_DWORD, "ZFunc", D3DRS_ZFUNC},
    {DEFSIZE(0), SRS_DWORD, "AlphaRef", D3DRS_ALPHAREF},
    {DEFSIZE(MYD3DCMPFUNC), SRS_DWORD, "AlphaFunc", D3DRS_ALPHAFUNC},
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "DitherEnable", D3DRS_DITHERENABLE},
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "AlphaBlendEnable", D3DRS_ALPHABLENDENABLE},
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "FogEnable", D3DRS_FOGENABLE},
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "SpecularEnable", D3DRS_SPECULARENABLE},
#ifndef _XBOX
//{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "ZVisible",					D3DRS_ZVISIBLE },
#endif
    {DEFSIZE(0), SRS_DWORD, "FogColor", D3DRS_FOGCOLOR},
    {DEFSIZE(MYD3DFOGMODE), SRS_DWORD, "FogTableMode", D3DRS_FOGTABLEMODE},
    {DEFSIZE(0), SRS_FLOAT, "FogStart", D3DRS_FOGSTART},
    {DEFSIZE(0), SRS_FLOAT, "FogEnd", D3DRS_FOGEND},
    {DEFSIZE(0), SRS_FLOAT, "FogDensity", D3DRS_FOGDENSITY},
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "EdgeAntialias", D3DRS_ANTIALIASEDLINEENABLE},
#ifdef USE_FX
    {DEFSIZE(0), SRS_DWORD, "DepthBias", D3DRS_DEPTHBIAS},
    {DEFSIZE(0), SRS_DWORD, "SlopeScaleDepthBias", D3DRS_SLOPESCALEDEPTHBIAS},
#else
    {DEFSIZE(0), SRS_DWORD, "Zbias", D3DRS_DEPTHBIAS},
    {DEFSIZE(0), SRS_DWORD, "SSZbias", D3DRS_SLOPESCALEDEPTHBIAS},
#endif
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "RangeFogEnable", D3DRS_RANGEFOGENABLE},
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "StencilEnable", D3DRS_STENCILENABLE},
    {DEFSIZE(MYD3DSTENCILOP), SRS_DWORD, "StencilFail", D3DRS_STENCILFAIL},
    {DEFSIZE(MYD3DSTENCILOP), SRS_DWORD, "StencilZFail", D3DRS_STENCILZFAIL},
    {DEFSIZE(MYD3DSTENCILOP), SRS_DWORD, "StencilPass", D3DRS_STENCILPASS},
    {DEFSIZE(MYD3DCMPFUNC), SRS_DWORD, "StencilFunc", D3DRS_STENCILFUNC},
    {DEFSIZE(0), SRS_DWORD, "StencilRef", D3DRS_STENCILREF},
    {DEFSIZE(0), SRS_DWORD, "StencilMask", D3DRS_STENCILMASK},
    {DEFSIZE(0), SRS_DWORD, "StencilWriteMask", D3DRS_STENCILWRITEMASK},
    {DEFSIZE(0), SRS_DWORD, "TextureFactor", D3DRS_TEXTUREFACTOR},
    {DEFSIZE(MYD3DWRAP), SRS_DWORD, "Wrap0", D3DRS_WRAP0},
    {DEFSIZE(MYD3DWRAP), SRS_DWORD, "Wrap1", D3DRS_WRAP1},
    {DEFSIZE(MYD3DWRAP), SRS_DWORD, "Wrap2", D3DRS_WRAP2},
    {DEFSIZE(MYD3DWRAP), SRS_DWORD, "Wrap3", D3DRS_WRAP3},
#ifndef _XBOX
    {DEFSIZE(MYD3DWRAP), SRS_DWORD, "Wrap4", D3DRS_WRAP4},
    {DEFSIZE(MYD3DWRAP), SRS_DWORD, "Wrap5", D3DRS_WRAP5},
    {DEFSIZE(MYD3DWRAP), SRS_DWORD, "Wrap6", D3DRS_WRAP6},
    {DEFSIZE(MYD3DWRAP), SRS_DWORD, "Wrap7", D3DRS_WRAP7},
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "Clipping", D3DRS_CLIPPING},
#endif
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "Lighting", D3DRS_LIGHTING},
    {DEFSIZE(0), SRS_DWORD, "Ambient", D3DRS_AMBIENT},
#ifndef _XBOX
    {DEFSIZE(MYD3DFOGMODE), SRS_DWORD, "FogVertexMode", D3DRS_FOGVERTEXMODE},
#endif
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "ColorVertex", D3DRS_COLORVERTEX},
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "LocalViewer", D3DRS_LOCALVIEWER},
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "NormalizeNormals", D3DRS_NORMALIZENORMALS},
    {DEFSIZE(MYD3DMATERIALCOLORSOURCE), SRS_DWORD, "DiffuseMaterialSource", D3DRS_DIFFUSEMATERIALSOURCE},
    {DEFSIZE(MYD3DMATERIALCOLORSOURCE), SRS_DWORD, "SpecularMaterialSource", D3DRS_SPECULARMATERIALSOURCE},
    {DEFSIZE(MYD3DMATERIALCOLORSOURCE), SRS_DWORD, "AmbientMaterialSource", D3DRS_AMBIENTMATERIALSOURCE},
    {DEFSIZE(MYD3DMATERIALCOLORSOURCE), SRS_DWORD, "EmissiveMaterialSource", D3DRS_EMISSIVEMATERIALSOURCE},
    {DEFSIZE(MYD3DVERTEXBLENDFLAGS), SRS_DWORD, "VertexBlend", D3DRS_VERTEXBLEND},
#ifndef _XBOX
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "ClipPlaneEnable", D3DRS_CLIPPLANEENABLE},
//{ DEFSIZE(MYTRUEFALSE) , SRS_DWORD, "SoftwareVertexProcessing",	D3DRS_SOFTWAREVERTEXPROCESSING },
#endif
    {DEFSIZE(0), SRS_FLOAT, "PointSize", D3DRS_POINTSIZE},
    {DEFSIZE(0), SRS_FLOAT, "PointSize_Min", D3DRS_POINTSIZE_MIN},
    {DEFSIZE(0), SRS_FLOAT, "PointSize_Max", D3DRS_POINTSIZE_MAX},
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "PointSpriteEnable", D3DRS_POINTSPRITEENABLE},
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "PointScaleEnable", D3DRS_POINTSCALEENABLE},
    {DEFSIZE(0), SRS_FLOAT, "PointScale_A", D3DRS_POINTSCALE_A},
    {DEFSIZE(0), SRS_FLOAT, "PointScale_B", D3DRS_POINTSCALE_B},
    {DEFSIZE(0), SRS_FLOAT, "PointScale_C", D3DRS_POINTSCALE_C},
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "MultiSampleAntialias", D3DRS_MULTISAMPLEANTIALIAS},
    {DEFSIZE(0), SRS_DWORD, "MultiSampleMask", D3DRS_MULTISAMPLEMASK},
    //{ DEFSIZE(0), SRS_FLOAT, "PatchSegments",						D3DRS_PATCHSEGMENTS },
    {DEFSIZE(MYD3DPATCHEDGESTYLE), SRS_DWORD, "PatchEdgeStyle", D3DRS_PATCHEDGESTYLE},
#ifndef _XBOX
    {DEFSIZE(MYTRUEFALSE), SRS_DWORD, "IndexedVertexBlendEnable", D3DRS_INDEXEDVERTEXBLENDENABLE},
#endif
    {DEFSIZE(MYD3DCOLORWRITEMASK), SRS_DWORD, "ColorWriteEnable", D3DRS_COLORWRITEENABLE},
#ifndef _XBOX
    {DEFSIZE(0), SRS_FLOAT, "TweenFactor", D3DRS_TWEENFACTOR},
#endif
    {DEFSIZE(MYD3DBLENDOP), SRS_DWORD, "BlendOp", D3DRS_BLENDOP}};

// ---------------------------------------------------------------------------------
// SetTextureStageState parameters
SRSPARAM MYD3DTEXTUREOP[] = {{"disable", D3DTOP_DISABLE},
                             {"selectarg1", D3DTOP_SELECTARG1},
                             {"selectarg2", D3DTOP_SELECTARG2},
                             {"modulate", D3DTOP_MODULATE},
                             {"modulate2x", D3DTOP_MODULATE2X},
                             {"modulate4x", D3DTOP_MODULATE4X},
                             {"add", D3DTOP_ADD},
                             {"addsigned", D3DTOP_ADDSIGNED},
                             {"addsigned2x", D3DTOP_ADDSIGNED2X},
                             {"substract", D3DTOP_SUBTRACT},
                             {"addsmooth", D3DTOP_ADDSMOOTH},
                             {"blenddiffusealpha", D3DTOP_BLENDDIFFUSEALPHA},
                             {"blendtexturealpha", D3DTOP_BLENDTEXTUREALPHA},
                             {"blendfactoralpha", D3DTOP_BLENDFACTORALPHA},
                             {"blendcurrentalpha", D3DTOP_BLENDCURRENTALPHA},
                             {"blendtexturealphapm", D3DTOP_BLENDTEXTUREALPHAPM},
                             {"premodulate", D3DTOP_PREMODULATE},
                             {"modulatealpha_addcolor", D3DTOP_MODULATEALPHA_ADDCOLOR},
                             {"modulatecolor_addalpha", D3DTOP_MODULATECOLOR_ADDALPHA},
                             {"modulateinvalpha_addcolor", D3DTOP_MODULATEINVALPHA_ADDCOLOR},
                             {"modulateinvcolor_addalpha", D3DTOP_MODULATEINVCOLOR_ADDALPHA},
                             {"bumpenvmap", D3DTOP_BUMPENVMAP},
                             {"bumpenvmapluminance", D3DTOP_BUMPENVMAPLUMINANCE},
                             {"dotproduct3", D3DTOP_DOTPRODUCT3},
                             {"multiplyadd", D3DTOP_MULTIPLYADD},
                             {"lerp", D3DTOP_LERP}};

SRSPARAM MYD3DTAFLAGS[] = {{"current", D3DTA_CURRENT},
                           {"diffuse", D3DTA_DIFFUSE},
                           {"selectmask", D3DTA_SELECTMASK},
                           {"specular", D3DTA_SPECULAR},
                           {"temp", D3DTA_TEMP},
                           {"texture", D3DTA_TEXTURE},
                           {"tfactor", D3DTA_TFACTOR},
                           {"alphareplicate", D3DTA_ALPHAREPLICATE},
                           {"complement", D3DTA_COMPLEMENT},
                           {"current_alphareplicate", D3DTA_CURRENT | D3DTA_ALPHAREPLICATE},
                           {"diffuse_alphareplicate", D3DTA_DIFFUSE | D3DTA_ALPHAREPLICATE},
                           {"specular_alphareplicate", D3DTA_SPECULAR | D3DTA_ALPHAREPLICATE},
                           {"temp_alphareplicate", D3DTA_TEMP | D3DTA_ALPHAREPLICATE},
                           {"texture_alphareplicate", D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE},
                           {"tfactor_alphareplicate", D3DTA_TFACTOR | D3DTA_ALPHAREPLICATE},
                           {"current_complement", D3DTA_CURRENT | D3DTA_COMPLEMENT},
                           {"diffuse_complement", D3DTA_DIFFUSE | D3DTA_COMPLEMENT},
                           {"specular_complement", D3DTA_SPECULAR | D3DTA_COMPLEMENT},
                           {"temp_complement", D3DTA_TEMP | D3DTA_COMPLEMENT},
                           {"texture_complement", D3DTA_TEXTURE | D3DTA_COMPLEMENT},
                           {"tfactor_complement", D3DTA_TFACTOR | D3DTA_COMPLEMENT}};

SRSPARAM MYD3DTEXCOORDINDEX[] = {{"passthru", D3DTSS_TCI_PASSTHRU},
                                 {"cameraspacenormal", D3DTSS_TCI_CAMERASPACENORMAL},
                                 {"cameraspaceposition", D3DTSS_TCI_CAMERASPACEPOSITION},
                                 {"cameraspacereflectionvector", D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR}};

SRSPARAM MYD3DTEXTUREADDRESS[] = {{"wrap", D3DTADDRESS_WRAP},
                                  {"mirror", D3DTADDRESS_MIRROR},
                                  {"clamp", D3DTADDRESS_CLAMP},
                                  {"border", D3DTADDRESS_BORDER},
#ifndef _XBOX
                                  {"mirroronce", D3DTADDRESS_MIRRORONCE}
#endif
};

SRSPARAM MYD3DTEXTUREFILTERTYPE[] = {
    {"none", D3DTEXF_NONE}, {"point", D3DTEXF_POINT}, {"linear", D3DTEXF_LINEAR}, {"anisotropic", D3DTEXF_ANISOTROPIC},
#ifndef _XBOX
//{ "flatcubic", D3DTEXF_FLATCUBIC },
#endif
    //{ "gaussiancubic", D3DTEXF_GAUSSIANCUBIC } };
};

SRSPARAM MYD3DTEXTURETRANSFORMFLAGS[] = {{"disable", D3DTTFF_DISABLE}, {"count1", D3DTTFF_COUNT1},
                                         {"count2", D3DTTFF_COUNT2},   {"count3", D3DTTFF_COUNT3},
                                         {"count4", D3DTTFF_COUNT4},   {"projected", D3DTTFF_PROJECTED}};

STSS TexturesStageStates[] = {
    {DEFSIZE(MYD3DTEXTUREOP), SRS_DWORD, "colorop", D3DTSS_COLOROP},
    {DEFSIZE(MYD3DTAFLAGS), SRS_DWORD, "colorarg1", D3DTSS_COLORARG1},
    {DEFSIZE(MYD3DTAFLAGS), SRS_DWORD, "colorarg2", D3DTSS_COLORARG2},
    {DEFSIZE(MYD3DTEXTUREOP), SRS_DWORD, "alphaop", D3DTSS_ALPHAOP},
    {DEFSIZE(MYD3DTAFLAGS), SRS_DWORD, "alphaarg1", D3DTSS_ALPHAARG1},
    {DEFSIZE(MYD3DTAFLAGS), SRS_DWORD, "alphaarg2", D3DTSS_ALPHAARG2},
    {DEFSIZE(0), SRS_FLOAT, "bumpenvmat00", D3DTSS_BUMPENVMAT00},
    {DEFSIZE(0), SRS_FLOAT, "bumpenvmat01", D3DTSS_BUMPENVMAT01},
    {DEFSIZE(0), SRS_FLOAT, "bumpenvmat10", D3DTSS_BUMPENVMAT10},
    {DEFSIZE(0), SRS_FLOAT, "bumpenvmat11", D3DTSS_BUMPENVMAT11},
    {DEFSIZE(MYD3DTEXCOORDINDEX), SRS_DWORD, "texcoordindex", D3DTSS_TEXCOORDINDEX},
    /* { DEFSIZE(MYD3DTEXTUREADDRESS) , SRS_DWORD, "addressu",		D3DTSS_ADDRESSU },
     { DEFSIZE(MYD3DTEXTUREADDRESS) , SRS_DWORD, "addressv",		D3DTSS_ADDRESSV },
     { DEFSIZE(0), SRS_DWORD, "bordercolor",						D3DTSS_BORDERCOLOR },
     { DEFSIZE(MYD3DTEXTUREFILTERTYPE) , SRS_DWORD, "magfilter", D3DTSS_MAGFILTER },
     { DEFSIZE(MYD3DTEXTUREFILTERTYPE) , SRS_DWORD, "minfilter", D3DTSS_MINFILTER },
     { DEFSIZE(MYD3DTEXTUREFILTERTYPE) , SRS_DWORD, "mipfilter", D3DTSS_MIPFILTER },
     { DEFSIZE(0), SRS_FLOAT, "mipmaplodbias",					D3DTSS_MIPMAPLODBIAS },
     { DEFSIZE(0), SRS_DWORD, "maxmiplevel",						D3DTSS_MAXMIPLEVEL },
     { DEFSIZE(0), SRS_DWORD, "maxanisotropy",					D3DTSS_MAXANISOTROPY },*/
    {DEFSIZE(0), SRS_FLOAT, "bumpenvlscale", D3DTSS_BUMPENVLSCALE},
    {DEFSIZE(0), SRS_FLOAT, "bumpenvloffset", D3DTSS_BUMPENVLOFFSET},
    {DEFSIZE(MYD3DTEXTURETRANSFORMFLAGS), SRS_DWORD, "texturetransformflags", D3DTSS_TEXTURETRANSFORMFLAGS},
    // { DEFSIZE(MYD3DTEXTUREADDRESS) , SRS_DWORD, "addressw", D3DTSS_ADDRESSW },
    {DEFSIZE(MYD3DTAFLAGS), SRS_DWORD, "colorarg0", D3DTSS_COLORARG0},
    {DEFSIZE(MYD3DTAFLAGS), SRS_DWORD, "alphaarg0", D3DTSS_ALPHAARG0},
    {DEFSIZE(MYD3DTAFLAGS), SRS_DWORD, "resultarg", D3DTSS_RESULTARG}};

SAMP SampleStates[] = {{DEFSIZE(MYD3DTEXTUREADDRESS), SRS_DWORD, "addressu", D3DSAMP_ADDRESSU},
                       {DEFSIZE(MYD3DTEXTUREADDRESS), SRS_DWORD, "addressv", D3DSAMP_ADDRESSV},
                       {DEFSIZE(0), SRS_DWORD, "bordercolor", D3DSAMP_BORDERCOLOR},
                       {DEFSIZE(MYD3DTEXTUREFILTERTYPE), SRS_DWORD, "magfilter", D3DSAMP_MAGFILTER},
                       {DEFSIZE(MYD3DTEXTUREFILTERTYPE), SRS_DWORD, "minfilter", D3DSAMP_MINFILTER},
                       {DEFSIZE(MYD3DTEXTUREFILTERTYPE), SRS_DWORD, "mipfilter", D3DSAMP_MIPFILTER},
                       {DEFSIZE(0), SRS_FLOAT, "mipmaplodbias", D3DSAMP_MIPMAPLODBIAS},
                       {DEFSIZE(0), SRS_DWORD, "maxmiplevel", D3DSAMP_MAXMIPLEVEL},
                       {DEFSIZE(0), SRS_DWORD, "maxanisotropy", D3DSAMP_MAXANISOTROPY},
                       {DEFSIZE(MYD3DTEXTUREADDRESS), SRS_DWORD, "addressw", D3DSAMP_ADDRESSW}};

// ---------------------------------------------------------------------------------
// transform block
SRSPARAM TRANSFORM_TYPES[] = {{"view", D3DTS_VIEW},     {"projection", D3DTS_PROJECTION}, {"world", D3DTS_WORLD},
                              {"tex0", D3DTS_TEXTURE0}, {"tex1", D3DTS_TEXTURE1},         {"tex2", D3DTS_TEXTURE2},
                              {"tex3", D3DTS_TEXTURE3},
#ifndef _XBOX
                              {"tex4", D3DTS_TEXTURE4}, {"tex5", D3DTS_TEXTURE5},         {"tex6", D3DTS_TEXTURE6},
                              {"tex7", D3DTS_TEXTURE7}
#endif
};

// ---------------------------------------------------------------------------------
// params section for block
#define PARAM_MATRIX 0
#define PARAM_TEXTURE 1
#define PARAM_DWORD 2
#define PARAM_FLOAT 3

SRSPARAM PARAM_TYPES[] = {
    {"matrix", PARAM_MATRIX}, {"texture", PARAM_TEXTURE}, {"dword", PARAM_DWORD}, {"float", PARAM_FLOAT}};

// restores types

SRSPARAM RESTORE_TYPES[] = {{"states", SUBCODE_RESTORE_STATES}, {"clear", SUBCODE_RESTORE_CLEAR}};

CTechnique::CTechnique(VDX9RENDER *_pRS)
{
    dwNumBlocks = 0;
    dwNumShaders = 0;
    dwCurBlock = 0;
    dwNumParams = 0;
    dwCurMaxSavedSize = 0;
    dwNumSavedStates = 0;
    dwCurSavedStatesPos = 0;
    dwCurParamsMax = 0;

    pShaders = nullptr;
    pBlocks = nullptr;
    pParams = nullptr;
    pSavedStates = nullptr;
    pCurParams = nullptr;

    pRS = _pRS;

    std::ranges::fill(sDelimTable, 0);
    char sDelimeters[] = " ,.[]-+\0\n\r\t";
    size_t len = strlen(sDelimeters);
    for (uint32_t i = 0; i < len; i++)
        sDelimTable[sDelimeters[i]] = 1;
}

CTechnique::~CTechnique()
{
    uint32_t i, j, k;
    for (i = 0; i < dwNumBlocks; i++)
    {
        block_t *pB = &pBlocks[i];
        for (j = 0; j < pB->dwNumTechniques; j++)
        {
            technique_t *pTech = &pB->pTechniques[j];
            for (k = 0; k < pTech->dwNumPasses; k++)
                STORM_DELETE(pTech->pPasses[k].pPass);
            STORM_DELETE(pTech->pPasses);
        }
        STORM_DELETE(pB->pParams);
        STORM_DELETE(pB->pBlockName);
        STORM_DELETE(pB->pTechniques);
    }
    for (i = 0; i < dwNumShaders; i++)
    {
        STORM_DELETE(pShaders[i].pName);
        STORM_DELETE(pShaders[i].pDecl);
        if (pShaders[i].pPixelShader != nullptr)
        {
            pShaders[i].pPixelShader->Release();
            pShaders[i].pPixelShader = nullptr;
        }
        if (pShaders[i].pVertexShader != nullptr)
        {
            pShaders[i].pVertexShader->Release();
            pShaders[i].pVertexShader = nullptr;
        }
    }
    STORM_DELETE(pShaders);
    STORM_DELETE(pBlocks);
    STORM_DELETE(pSavedStates);
    STORM_DELETE(pCurParams);
}

#define END_TEST (uint32_t(pCurrent - pBegin) >= dwSize - 2)

char *GetString(char *pBegin, uint32_t dwSize, char *pCurrent)
{
    if (END_TEST)
        return nullptr;
    // go for a null, or 0xA, or 0xD
    if (pCurrent != pBegin) // for first string
    {
        while ((*pCurrent && *pCurrent != 0xa && *pCurrent != 0xd) && !END_TEST)
            *pCurrent++; //~!~
        if (END_TEST)
            return nullptr;
    }
    // skip all nulls, or 0xA, or 0xD
    while ((!(*pCurrent) || (*pCurrent == 0xa && *pCurrent == 0xd)) && !END_TEST)
        *pCurrent++; //~!~
    if (END_TEST)
        return nullptr;
    return pCurrent;
}

void ClearComment(char *pStr)
{
    // search comment and delete all symbols after
    if (!pStr || !pStr[0])
        return;
    int32_t iLen = strlen(pStr);
    char *pSkip1 = FindToken(pStr, COMMENT1);
    int32_t iStart1 = iLen, iStart2 = iLen;
    if (pSkip1)
        iStart1 = (pSkip1 - pStr) - strlen(COMMENT1);
    char *pSkip2 = FindToken(pStr, COMMENT2);
    if (pSkip2)
        iStart2 = (pSkip2 - pStr) - strlen(COMMENT2);
    int32_t iStart = (iStart2 > iStart1) ? iStart1 : iStart2;
    for (int32_t i = iStart; i < iLen; i++)
        pStr[i] = 0;
}

bool isVertexDeclaration(char *pStr)
{
    return (SkipToken(pStr, VERTEX_DECL)) ? true : false;
}
bool isAsm(char *pStr)
{
    return (SkipToken(pStr, ASM_SHADER)) ? true : false;
}
bool isVertexShaderConst(char *pStr)
{
    return (SkipToken(pStr, VERTEX_SHADER_CONST)) ? true : false;
}
bool isPixelShaderConst(char *pStr)
{
    return (SkipToken(pStr, PIXEL_SHADER_CONST)) ? true : false;
}
bool isVertexShader(char *pStr)
{
    return (SkipToken(pStr, VERTEX_SHADER)) ? true : false;
}
bool isPixelShader(char *pStr)
{
    return (SkipToken(pStr, PIXEL_SHADER)) ? true : false;
}
bool isBlock(char *pStr)
{
    return (SkipToken(pStr, BLOCK)) ? true : false;
}
bool isTechnique(char *pStr)
{
    return (SkipToken(pStr, TECHNIQUE)) ? true : false;
}
bool isPass(char *pStr)
{
    return (SkipToken(pStr, PASS)) ? true : false;
}
bool isInclude(char *pStr)
{
    return (SkipToken(pStr, INCLUDE)) ? true : false;
}
bool isDefine(char *pStr)
{
    return (SkipToken(pStr, DEFINE)) ? true : false;
}
bool isBeginBracket(char *pStr)
{
    return (SkipToken(pStr, BEGIN_BRACKET)) ? true : false;
}
bool isEndBracket(char *pStr)
{
    return (SkipToken(pStr, END_BRACKET)) ? true : false;
}
bool isDigit(char *pStr)
{
    return ((pStr) ? (pStr[0] >= '0' && pStr[0] <= '9' || pStr[0] == '-') : false);
}
bool hasPoint(const char *pStr)
{
    while (*pStr != '\0')
    {
        if (*pStr == '.')
        {
            return true;
        }
        pStr++;
    }
    return false;
}

uint32_t CTechnique::GetSRSIndex(char *pStr)
{
    uint32_t dwNumParam = sizeof(RenderStates) / sizeof(STSS);
    for (uint32_t i = 0; i < dwNumParam; i++)
        if (storm::iEquals(pStr, RenderStates[i].cName))
            return i;
    core.Trace("ERROR: SetRenderState: unknown parameter type <%s> in <%s> file, technique <%s>", pStr,
               sCurrentFileName, sCurrentBlockName);
    // THROW;
    return INVALID_SRS_INDEX;
}

uint32_t CTechnique::GetSTSSIndex(char *pStr)
{
    uint32_t dwNumParam = sizeof(TexturesStageStates) / sizeof(STSS);
    for (uint32_t i = 0; i < dwNumParam; i++)
        if (storm::iEquals(pStr, TexturesStageStates[i].cName))
            return i;
    // api->Trace("ERROR: SetTextureStageState: unknown parameter type <%s> in <%s> file, technique
    // <%s>",pStr,sCurrentFileName,sCurrentBlockName); THROW;
    return INVALID_INDEX;
}

uint32_t CTechnique::GetSAMPIndex(char *pStr)
{
    uint32_t dwNumParam = sizeof(SampleStates) / sizeof(SAMP);
    for (uint32_t i = 0; i < dwNumParam; i++)
        if (storm::iEquals(pStr, SampleStates[i].cName))
            return i;
    // api->Trace("ERROR: SetSamplerState: unknown parameter type <%s> in <%s> file, technique <%s>", pStr,
    // sCurrentFileName, sCurrentBlockName); THROW;
    return INVALID_INDEX;
}

uint32_t CTechnique::GetIndex(char *pStr, SRSPARAM *pParam, uint32_t dwNumParam, bool bCanBeNumber)
{
    for (uint32_t i = 0; i < dwNumParam; i++)
        if (storm::iEquals(pStr, pParam[i].cName))
            return i;
    if (!bCanBeNumber)
        core.Trace("ERROR: Unknown parameter type <%s> in <%s> file, technique <%s>", pStr, sCurrentFileName,
                   sCurrentBlockName);
    // THROW;
    return INVALID_INDEX;
}

uint32_t CTechnique::GetCode(char *pStr, SRSPARAM *pParam, uint32_t dwNumParam, uint32_t *pPassCode, bool bCanBeNumber)
{
    Assert(pStr);

    uint32_t dwCode = 0;
    if (pParam && dwNumParam)
    {
        uint32_t dwIndex = GetIndex(pStr, pParam, dwNumParam, bCanBeNumber);
        if (dwIndex != INVALID_INDEX)
            return pParam[dwIndex].dwCode;
    }
    // check for numbers

    if (isDigit(pStr))
    {
        if (hasPoint(pStr))
        {
            float fvalue;
            sscanf(pStr, "%f", &fvalue);
            dwCode = F2DW(fvalue);
        }
        else
        {
            sscanf(pStr, (SkipToken(pStr, "0x")) ? "%x" : "%d", &dwCode);
        }
        if (pPassCode)
            *pPassCode |= FLAGS_CODE_NUMBER;
        return dwCode;
    }
    else
    {
        core.Trace("ERROR: unknown parameter type <%s> in <%s> file, technique <%s>", pStr, sCurrentFileName,
                   sCurrentBlockName);
        return 0;
    }
}

uint32_t CTechnique::AddShader(char *pShaderName)
{
    // find exist shader or create empty new
    for (uint32_t i = 0; i < dwNumShaders; i++)
        if (storm::iEquals(pShaders[i].pName, pShaderName))
            return i;
    pShaders = (shader_t *)realloc(pShaders, sizeof(shader_t) * (dwNumShaders + 1));
    shader_t *pS = &pShaders[dwNumShaders];
    pShaders[dwNumShaders] = {};
    const auto len = strlen(pShaderName) + 1;
    pS->pName = new char[len];
    memcpy(pS->pName, pShaderName, len);
    pS->dwHashName = MakeHashValue(pShaderName);
    pS->pVertexDecl = nullptr;
    pS->pVertexShader = nullptr;
    pS->pPixelShader = nullptr;
    dwNumShaders++;
    return (dwNumShaders - 1);
}

uint32_t CTechnique::ProcessPass(char *pFile, uint32_t dwSize, char **pStr)
{
    block_t *pB = &pBlocks[dwNumBlocks];
    technique_t *pTechniques = &pB->pTechniques[pB->dwNumTechniques];

    pTechniques->pPasses = (pass_t *)realloc(pTechniques->pPasses, sizeof(pass_t) * (pTechniques->dwNumPasses + 1));
    pTechniques->pPasses[pTechniques->dwNumPasses] = {};

    uint32_t *pPass = pPassStorage;
    uint32_t *pPassBegin = pPass;
    uint32_t dwTextureIndex, dwAdditionalFlags = 0;
    char *pTemp, temp[256];
    while (nullptr != (*pStr = GetString(pFile, dwSize, *pStr)))
    {
#define SKIP3                                                                                                          \
    {                                                                                                                  \
        (*pStr)++;                                                                                                     \
        continue;                                                                                                      \
    }
        tolwr(*pStr);
        ClearComment(*pStr);
        if (isEndBracket(*pStr))
        {
#ifdef USE_FX // fx files has no "Render." and "Restore." strings
            *pPass++ = CODE_RENDER;
            *pPass++ = SUBCODE_RENDER_DRAW;

            *pPass++ = CODE_RESTORE;
            *pPass++ = SUBCODE_RESTORE_STATES;
            // clear STSS and SRS bUse
            ClearSRS_STSS_bUse();
#endif
            break; // end of pass
        }

        // check right side of expression for [in] parameters
        bool bIn = false;
        uint32_t dwInParamIndex = 0;
#ifdef false // current sha files has no params. Last usage was in
             // ENGINE/modules/techniques/weather/shaders/SeaFoam.sha. Removed here:
             // https://github.com/storm-devs/thunderstorm-engine/commit/f290ca08ff33d3f86fcdf6aef26b0e20e36ab54c
        if (pTemp = SkipToken(*pStr, INPARAM_CHECK)) // STSS or SetTexture
        {
            GetTokenWhile(pTemp, &temp[0], ">");
            bIn = true;
            dwInParamIndex = GetIndex(temp, pParams, dwNumParams, false);
            dwAdditionalFlags |= FLAGS_CODE_IN_PARAM;
        }
#endif

        // check for additional flags such as "no restore states" and other
        dwAdditionalFlags |= FLAGS_CODE_RESTORE;
        if (SkipToken(*pStr, NOSAVE_STATE))
        {
            *pStr = SkipToken(*pStr, NOSAVE_STATE);
            dwAdditionalFlags &= (~FLAGS_CODE_RESTORE);
        }

#ifndef USE_FX // fx files has no "Restore." strings
        // restore states check
        if (pTemp = SkipToken(*pStr, RESTORE_STATES_CHECK))
        {
            *pPass++ = CODE_RESTORE;
            *pPass++ = GetCode(pTemp, RESTORE_TYPES, sizeof(RESTORE_TYPES) / sizeof(SRSPARAM), nullptr);
            // clear STSS and SRS bUse
            ClearSRS_STSS_bUse();
            SKIP3;
        }
#endif
        if (isPixelShaderConst(*pStr))
        {
            if (!bIn)
                psnip_trap();
            uint32_t dwIndex = 0;
            *pPass++ = CODE_SPSCONST | dwAdditionalFlags;
            GetTokenWhile(SkipToken(*pStr, "["), &temp[0], "]");
            sscanf(temp, "%u", &dwIndex);
            *pPass++ = dwIndex;
            *pPass++ = dwInParamIndex;
            // GetTokenWhile(SkipToken(*pStr,"="),temp,";");
            //*pPass++ = GetCode(temp,0,0,null,false);
            SKIP3;
        }

        if (isVertexShaderConst(*pStr))
        {
            uint32_t dwIndex = 0;
            *pPass++ = CODE_SVSCONST | dwAdditionalFlags;
            GetTokenWhile(SkipToken(*pStr, "["), &temp[0], "]");
            sscanf(temp, "%u", &dwIndex);
            *pPass++ = dwIndex;
            GetTokenWhile(SkipToken(*pStr, "="), temp, ";");
            *pPass++ = GetCode(temp, &MYSETVERTEXSHADERCONSTANT[0],
                               sizeof(MYSETVERTEXSHADERCONSTANT) / sizeof(SRSPARAM), nullptr, false);
            SKIP3;
        }

        // check for '[' if exist mean what STSS used
        if (pTemp = SkipToken(*pStr, STSS_CHECK)) // STSS or SetTexture
        {
            // get index = [index]
            GetTokenWhile(pTemp, &temp[0], "]");
            sscanf(temp, "%d", &dwTextureIndex);

            // check for "texture["
            if (SkipToken(*pStr, TEXTURE_CHECK))
            {
                // texture set
                *pPass++ = CODE_TEXTURE | dwAdditionalFlags;
                *pPass++ = dwTextureIndex;
                if (bIn)
                    *pPass++ = dwInParamIndex;
                else
                {
                }
                SKIP3;
            }
            // SetTextureStageState
            uint32_t *pPassCode = pPass;

            // get left side of expression
            GetTokenWhile(*pStr, temp, "[");
            uint32_t dwIndex = GetSTSSIndex(temp);
            if (dwIndex != INVALID_INDEX)
            {
                *pPass++ = CODE_STSS | dwAdditionalFlags;
                *pPass++ = dwTextureIndex;
                if (TexturesStageStates[dwIndex].bUse &&
                    TexturesStageStates[dwIndex].dwUseSubCode & (1 << dwTextureIndex) &&
                    dwAdditionalFlags & FLAGS_CODE_RESTORE)
                    core.Trace("WARN: STSS: dup restore param type <%s[%d]> in <%s> file, technique <%s>", temp,
                               dwTextureIndex, sCurrentFileName, sCurrentBlockName);
                if (dwAdditionalFlags & FLAGS_CODE_RESTORE)
                {
                    TexturesStageStates[dwIndex].bUse = true;
                    TexturesStageStates[dwIndex].dwUseSubCode |= (1 << dwTextureIndex);
                }
                *pPass++ = (uint32_t)TexturesStageStates[dwIndex].State;

                if (bIn)
                    *pPass++ = dwInParamIndex;
                else
                {
                    // get right side of expression
                    GetTokenWhile(SkipToken(*pStr, "="), temp, ";");
                    *pPass++ = GetCode(temp, TexturesStageStates[dwIndex].pParam,
                                       TexturesStageStates[dwIndex].dwParamNum, pPassCode, true);
                }
            }
            else
            {
                *pPass++ = CODE_SAMP | dwAdditionalFlags;
                *pPass++ = dwTextureIndex;
                dwIndex = GetSAMPIndex(temp);
                if (dwIndex != INVALID_INDEX)
                {
                    if (SampleStates[dwIndex].bUse && SampleStates[dwIndex].dwUseSubCode & (1 << dwTextureIndex) &&
                        dwAdditionalFlags & FLAGS_CODE_RESTORE)
                        core.Trace("WARN: SAMP: dup restore param type <%s[%d]> in <%s> file, technique <%s>", temp,
                                   dwTextureIndex, sCurrentFileName, sCurrentBlockName);
                    if (dwAdditionalFlags & FLAGS_CODE_RESTORE)
                    {
                        SampleStates[dwIndex].bUse = true;
                        SampleStates[dwIndex].dwUseSubCode |= (1 << dwTextureIndex);
                    }
                    *pPass++ = (uint32_t)SampleStates[dwIndex].State;

                    if (bIn)
                        *pPass++ = dwInParamIndex;
                    else
                    {
                        // get right side of expression
                        GetTokenWhile(SkipToken(*pStr, "="), temp, ";");
                        *pPass++ = GetCode(temp, SampleStates[dwIndex].pParam, SampleStates[dwIndex].dwParamNum,
                                           pPassCode, true);
                    }
                }
                else
                    core.Trace("ERROR: unknown parameter type <%s> in <%s> file, technique <%s>", pStr,
                               sCurrentFileName, sCurrentBlockName);
            }
            SKIP3;
        }

        // PIXEL_SHADER, VERTEX_SHADER
        if (SkipToken(*pStr, PIXEL_SHADER))
        {
            *pPass++ = CODE_SPS;
#ifdef USE_FX
            pTemp = SkipToken(*pStr, "=");
            GetShaderBinPath(pTemp, CODE_SPS, temp);

            auto i = AddShader(temp);
            shader_t *pS = &pShaders[i];
            if (pS->dwShaderType == 0)
            {
                pS->dwShaderType = CODE_SPS;
                ProcessShaderBin(pS, temp, CODE_SPS);
            }
#else
            GetTokenWhile(SkipToken(*pStr, "\""), temp, "\"");
            auto i = AddShader(temp);
#endif
            *pPass++ = i;
            SKIP3;
        }

        if (SkipToken(*pStr, VERTEX_SHADER))
        {
            *pPass++ = CODE_SVS;
#ifdef USE_FX
            pTemp = SkipToken(*pStr, "=");
            GetShaderBinPath(pTemp, CODE_SVS, temp);

            auto i = AddShader(temp);
            shader_t *pS = &pShaders[i];
            if (pS->dwShaderType == 0)
            {
                pS->dwShaderType = CODE_SVS;
                ProcessShaderBin(pS, temp, CODE_SVS);
            }
#else
            GetTokenWhile(SkipToken(*pStr, "\""), temp, "\"");
            auto i = AddShader(temp);
#endif
            *pPass++ = i;
            SKIP3;
        }

#ifndef USE_FX // fx files has no "Render." strings
        // RENDER
        if (SkipToken(*pStr, RENDER_CHECK))
        {
            *pPass++ = CODE_RENDER;
            *pPass++ = SUBCODE_RENDER_DRAW;
            SKIP3;
        }
#endif

#ifdef false // current sha files has no "transform." strings
        // transform
        if (SkipToken(*pStr, TRANSFORM_CHECK))
        {
            //
            GetTokenWhile(SkipToken(*pStr, "."), temp, "=");
            uint32_t dwCode = GetCode(temp, TRANSFORM_TYPES, sizeof(TRANSFORM_TYPES) / sizeof(SRSPARAM), nullptr);
            if (dwCode == 0xFFFFFFFF) // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            {
                // maybe world0-world256
                if (nullptr == (pTemp = SkipToken(*pStr, WORLD_TRANSFORM_CHECK)))
                    throw std::runtime_error("transform. error!");
                sscanf(pTemp, "%u", &dwCode);
                dwCode = (uint32_t)D3DTS_WORLDMATRIX(dwCode);
            }
            *pPass++ = CODE_TRANSFORM;
            *pPass++ = dwCode;
            *pPass++ = dwInParamIndex;
            SKIP3;
        }
#endif

        // SetRenderState
        if (SkipToken(*pStr, "="))
        {
            // get left side of expression
            GetTokenWhile(*pStr, temp, "=");
            uint32_t dwSRSIndex = GetSRSIndex(temp);
            Assert(dwSRSIndex != INVALID_SRS_INDEX);
            if (RenderStates[dwSRSIndex].bUse && dwAdditionalFlags & FLAGS_CODE_RESTORE)
                core.Trace("WARN: SRS: dup restore param type <%s> in <%s> file, technique <%s>", temp,
                           sCurrentFileName, sCurrentBlockName);
            if (dwAdditionalFlags & FLAGS_CODE_RESTORE)
                RenderStates[dwSRSIndex].bUse = true;
            uint32_t *pPassCode = pPass;
            *pPass++ = CODE_SRS | dwAdditionalFlags;
            *pPass++ = (uint32_t)RenderStates[dwSRSIndex].State;

            if (bIn)
                *pPass++ = dwInParamIndex;
            else
            {
                // get right side of expression
                GetTokenWhile(SkipToken(*pStr, "="), temp, ";");
                *pPass++ = GetCode(temp, RenderStates[dwSRSIndex].pParam, RenderStates[dwSRSIndex].dwParamNum,
                                   pPassCode, true);
            }
            SKIP3;
        }

        // other skip
        SKIP3;
    }

    uint32_t dwSizePass = pPass - pPassBegin;
    pPassBegin = new uint32_t[dwSizePass]; //(uint32_t*)RESIZE(pPassBegin,dwSizePass*sizeof(uint32_t));
    memcpy(pPassBegin, pPassStorage, dwSizePass * sizeof(uint32_t));
    pTechniques->pPasses[pTechniques->dwNumPasses].dwSize = dwSizePass;
    pTechniques->pPasses[pTechniques->dwNumPasses].isValidate = true;
    pTechniques->pPasses[pTechniques->dwNumPasses].pPass = pPassBegin;

    pTechniques->dwNumPasses++;
    return PASS_OK;
}

uint32_t CTechnique::ProcessTechnique(char *pFile, uint32_t dwSize, char **pStr)
{
    block_t *pB = &pBlocks[dwNumBlocks];
    pB->pTechniques = (technique_t *)realloc(pB->pTechniques, sizeof(technique_t) * (pB->dwNumTechniques + 1));
    pB->pTechniques[pB->dwNumTechniques] = {};
    // clear STSS and SRS bUse
    ClearSRS_STSS_bUse();
    // search for pass and '}'
    while (nullptr != (*pStr = GetString(pFile, dwSize, *pStr)))
    {
#define SKIP2                                                                                                          \
    {                                                                                                                  \
        (*pStr)++;                                                                                                     \
        continue;                                                                                                      \
    }
        tolwr(*pStr);
        ClearComment(*pStr);
        // if (isComment(*pStr)) SKIP2;
        if (isEndBracket(*pStr))
            break; // end of technique
        if (isPass(*pStr))
            ProcessPass(pFile, dwSize, pStr);
        SKIP2;
    }

    pB->dwNumTechniques++;
    return TECHNIQUE_OK;
}

#define TOTAL_SKIP                                                                                                     \
    {                                                                                                                  \
        (*pStr)++;                                                                                                     \
        continue;                                                                                                      \
    }

void AddDeclElement(shader_t *pS)
{
    pS->dwDeclSize++;
    pS->pDecl = (D3DVERTEXELEMENT9 *)realloc(pS->pDecl, sizeof(D3DVERTEXELEMENT9) * pS->dwDeclSize);
    pS->pDecl[pS->dwDeclSize - 1].Method = D3DDECLMETHOD_DEFAULT;
}

D3DVERTEXELEMENT9 *CurDeclElement(shader_t *pS)
{
    return &pS->pDecl[pS->dwDeclSize - 1];
}

uint32_t CTechnique::ProcessVertexDeclaration(shader_t *pS, char *pFile, uint32_t dwSize, char **pStr)
{
    uint32_t dwTemp;
    ;
    uint32_t dwStream = 0, dwOffset = 0;
    uint32_t dwIndexes[std::size(MYDECLUSAGE)] = {0};

    while (nullptr != (*pStr = GetString(pFile, dwSize, *pStr)))
    {
        tolwr(*pStr);
        ClearComment(*pStr);
        if (isEndBracket(*pStr))
            break; // end of declaration
        if (SkipToken(*pStr, VDECL_STREAM_CHECK))
        {
            sscanf(SkipToken(*pStr, "["), "%d", &dwTemp);
            dwStream = dwTemp;
        }
        if (SkipToken(*pStr, VDECL_FLOAT_CHECK))
        {
            sscanf(SkipToken(*pStr, "["), "%u", &dwTemp);
            switch (dwTemp)
            {
            case 1:
                dwTemp = D3DDECLTYPE_FLOAT1;
                break;
            case 2:
                dwTemp = D3DDECLTYPE_FLOAT2;
                break;
            case 3:
                dwTemp = D3DDECLTYPE_FLOAT3;
                break;
            case 4:
                dwTemp = D3DDECLTYPE_FLOAT4;
                break;
            }
            AddDeclElement(pS);
            CurDeclElement(pS)->Offset = dwOffset;
            CurDeclElement(pS)->Stream = dwStream;
            CurDeclElement(pS)->Type = dwTemp;
            dwOffset += 4 * (dwTemp + 1);

            // Usage
            dwTemp = ~0;
            for (int i = 0; i < std::size(MYDECLUSAGE); i++)
            {
                if (SkipToken(*pStr, MYDECLUSAGE[i].cName))
                {
                    dwTemp = MYDECLUSAGE[i].dwCode;
                    break;
                }
            }
            CurDeclElement(pS)->Usage = dwTemp;
            if (dwTemp != ~0)
                CurDeclElement(pS)->UsageIndex = dwIndexes[dwTemp]++;
        }
        if (SkipToken(*pStr, VDECL_COLOR_CHECK))
        {
            AddDeclElement(pS);
            CurDeclElement(pS)->Offset = dwOffset;
            CurDeclElement(pS)->Stream = dwStream;
            CurDeclElement(pS)->Type = D3DDECLTYPE_D3DCOLOR;
            CurDeclElement(pS)->Usage = D3DDECLUSAGE_COLOR;
            CurDeclElement(pS)->UsageIndex = dwIndexes[D3DDECLUSAGE_COLOR]++;
            dwOffset += 4;
        }

        TOTAL_SKIP;
    }

    AddDeclElement(pS);
    *CurDeclElement(pS) = D3DDECL_END();

    HRESULT hr = pRS->CreateVertexDeclaration(pS->pDecl, &pS->pVertexDecl);
    if (hr != S_OK)
        core.Trace("ERROR: invalid shader declaration <%s>", pS->pName);

    return 0;
}

char *CTechnique::GetToken(char *pToken, char *pResult, bool &bToken)
{
    char *pStart = pToken;
    bToken = false;
    while (*pToken && !sDelimTable[*pToken])
        pToken++;
    pResult[pToken - pStart] = 0;
    if (pToken - pStart == 0 && *pToken == 0)
        return nullptr;
    if (pToken - pStart == 0 && sDelimTable[*pToken])
    {
        bToken = true;
        pResult[0] = *pToken;
        pResult[1] = 0;
        return pToken + 1;
    }
    strncpy_s(pResult, pToken - pStart + 1, pStart, pToken - pStart);
    return pToken;
}

char *CTechnique::Preprocessor(char *pBuffer, uint32_t &dwSize)
{
    bool bToken;
    char sValue[256], sName[256], sToken[1024];
    char *pStr = pBuffer;
    uint32_t dwBufSize = strlen(pBuffer);
    uint32_t i, dwNumDefines = 0;
    char *pProgram = nullptr;
    uint32_t dwProgramSize = 0;
    define_t *pDefines = nullptr;
    uint32_t dwMaxDefineElements = 0;
    uint32_t dwMaxProgramElements = 0;
    uint32_t dwNewSize;

    // change all 0xd 0xa to 0x0
    for (i = 0; i < dwSize; i++)
    {
        if (pBuffer[i] == 0xa || pBuffer[i] == 0xd)
            pBuffer[i] = 0;
        if (pBuffer[i] == 0x9)
            pBuffer[i] = ' ';
    }

    while (nullptr != (pStr = GetString(pBuffer, dwBufSize, pStr)))
    {
        // tolwr(pStr);
        ClearComment(pStr);
        if (isDefine(pStr))
        {
            GetTokenWhile(SkipToken(pStr, DEFINE), sName, " ");
            GetTokenWhile(SkipToken(pStr, sName), sValue, " ");
            dwNewSize = 16 * ((dwNumDefines / 16) + 1);
            if (dwMaxDefineElements != dwNewSize)
            {
                dwMaxDefineElements = dwNewSize;
                pDefines = (define_t *)realloc(pDefines, sizeof(define_t) * dwMaxDefineElements);
            }
            auto len = strlen(sName) + 1;
            pDefines[dwNumDefines].pName = new char[len];
            memcpy(pDefines[dwNumDefines].pName, sName, len);

            pDefines[dwNumDefines].dwNameLen = len - 1;

            len = strlen(sValue) + 1;
            pDefines[dwNumDefines].pValue = new char[len];
            memcpy(pDefines[dwNumDefines].pValue, sValue, len);

            dwNumDefines++;
            pStr++;
            continue;
        }
        // else real code string
        bool bNewString = false;
        char *pToken = pStr;
        while (nullptr != (pToken = GetToken(pToken, sToken, bToken)))
        {
            uint32_t dwTempLen = strlen(sToken);
            if (!bToken)
            {
                // find define
                for (i = 0; i < dwNumDefines; i++)
                    if (dwTempLen == pDefines[i].dwNameLen && storm::iEquals(pDefines[i].pName, sToken))
                    {
                        if (pDefines[i].pValue)
                            strcpy_s(sToken, pDefines[i].pValue);
                        dwTempLen = strlen(sToken);
                        break;
                    }
            }
            // add sToken to programm
            dwNewSize = 8192 * (((dwTempLen + dwProgramSize + 4) / 8192) + 1);
            if (dwNewSize > dwMaxProgramElements)
            {
                dwMaxProgramElements = dwNewSize;
                pProgram = (char *)realloc(pProgram, dwMaxProgramElements);
            }
            strcpy_s(&pProgram[dwProgramSize], dwNewSize - dwProgramSize, sToken);
            dwProgramSize += dwTempLen;
            bNewString = true;
        }
        if (pProgram && bNewString)
        {
            pProgram[dwProgramSize] = 0xd;
            pProgram[dwProgramSize + 1] = 0xa;
            pProgram[dwProgramSize + 2] = 0x0;
            dwProgramSize += 2;
        }
        pStr++;
        continue;
    }
    STORM_DELETE(pBuffer);
    if (pProgram)
        pProgram[dwProgramSize] = 0x0;
    dwSize = dwProgramSize;
    // api->Trace(pProgram);
    for (i = 0; i < dwNumDefines; i++)
    {
        STORM_DELETE(pDefines[i].pName);
        STORM_DELETE(pDefines[i].pValue);
    }
    STORM_DELETE(pDefines);

    return pProgram;
}

uint32_t CTechnique::ProcessShaderAsm(shader_t *pS, char *pFile, uint32_t dwSize, char **pStr, uint32_t dwShaderType,
                                      bool HLSL)
{
    uint32_t dwFileSize;
    uint32_t dwTotalLen = 0;
    char *pBuffer = nullptr;
    while (nullptr != (*pStr = GetString(pFile, dwSize, *pStr)))
    {
        // tolwr(*pStr);
        ClearComment(*pStr);
        if (isBeginBracket(*pStr))
            TOTAL_SKIP;
        if (isEndBracket(*pStr))
            break; // end of declaration

        char *pTemp = *pStr;
        char *pTempBuffer = nullptr;

        if (isInclude(*pStr))
        {
            char sIncFileName[256], sName[256];
            GetTokenWhile(SkipToken(*pStr, "\""), &sName[0], "\"");
            sprintf_s(sIncFileName, "%s\\%s", sCurrentDir, sName);
            if (!fio->LoadFile(sIncFileName, &pTempBuffer, &dwFileSize))
            {
                core.Trace("ERROR: in file %s, file not found : %s", sCurrentFileName, sIncFileName);
                TOTAL_SKIP;
            }
            pTempBuffer[dwFileSize - 1] = 0x0;
            pTemp = pTempBuffer;
        }

        int32_t iLen = strlen(pTemp) - 1;
        pBuffer = (char *)realloc(pBuffer, dwTotalLen + iLen + 4);
        strcpy_s(&pBuffer[dwTotalLen], iLen + 4, pTemp);
        strcpy_s(&pBuffer[dwTotalLen + iLen + 1], 3, "\r\n");
        dwTotalLen += iLen + 2;
        STORM_DELETE(pTempBuffer);
        TOTAL_SKIP;
    }
#ifdef _WIN32 // replace D3DXAssembleShader with D3DCompile or similar
#ifndef _XBOX
    ID3DXBuffer *CompiledShader = nullptr, *ErrorShader = nullptr;
#else
    LPXGBUFFER CompiledShader = null, ErrorShader = null;
#endif
    HRESULT hr;
    pBuffer = Preprocessor(pBuffer, dwTotalLen);

    if (HLSL)
    {
        if (dwShaderType == CODE_SVS)
            hr = D3DXCompileShader(pBuffer, dwTotalLen, nullptr, nullptr, "main", "vs_3_0",
                                   D3DXSHADER_OPTIMIZATION_LEVEL3, &CompiledShader, &ErrorShader, nullptr);
        else
            hr = D3DXCompileShader(pBuffer, dwTotalLen, nullptr, nullptr, "main", "ps_3_0",
                                   D3DXSHADER_OPTIMIZATION_LEVEL3, &CompiledShader, &ErrorShader, nullptr);
    }
    else
        hr = D3DXAssembleShader(pBuffer, dwTotalLen, nullptr, nullptr, 0, &CompiledShader, &ErrorShader);

    //#ifndef _XBOX
    /*#else
        hr = XGAssembleShader(0, pBuffer, dwTotalLen, 0, 0, &CompiledShader, &ErrorShader, 0, 0, 0,
    0);//D3DXAssembleShader(pBuffer,dwTotalLen,0,0,&CompiledShader,&ErrorShader); #endif*/
    if (D3D_OK != hr && ErrorShader)
    {
        const auto len = ErrorShader->GetBufferSize() + 1;
        char *pErrStr = new char[len];
        strncpy_s(pErrStr, len, (char *)ErrorShader->GetBufferPointer(), ErrorShader->GetBufferSize());
        pErrStr[ErrorShader->GetBufferSize()] = 0;
        core.Trace("ERROR: in compile shader %s\nerror:\n%s", pS->pName, pErrStr);
        RELEASE(CompiledShader);
        RELEASE(ErrorShader);
        STORM_DELETE(pBuffer);
        STORM_DELETE(pErrStr);
        return 0;
    }
    if (dwShaderType == CODE_SVS)
        hr = pRS->CreateVertexShader((uint32_t *)CompiledShader->GetBufferPointer(), &pS->pVertexShader);
    else
        hr = pRS->CreatePixelShader((uint32_t *)CompiledShader->GetBufferPointer(), &pS->pPixelShader);

    /*#ifndef _XBOX
    #else
            hr = pRS->CreatePixelShader((uint32_t*)(D3DPIXELSHADERDEF
    *)CompiledShader->GetBufferPointer(),&pS->dwShaderHandle); #endif*/

    RELEASE(CompiledShader);
    RELEASE(ErrorShader);
#endif
    STORM_DELETE(pBuffer);
    return 0;
}

void CTechnique::GetShaderBinPath(char *pShaderStr, uint32_t dwShaderType, char *dest)
{
    // dest will contain path to shader file "currentFilename_shaderName.vso" (or .pso) in folder of sCurrentFileName
    char sShaderName[256];
    if (pShaderStr[strlen(pShaderStr)-2] == '(' && pShaderStr[strlen(pShaderStr)-1] == ')')
    {
        // for pShaderStr like "compile vs_1_0 main()"
        GetTokenWhile(SkipToken(SkipToken(pShaderStr, " "), " "), sShaderName, "(");
    }
    else
    {
        strcpy_s(sShaderName, pShaderStr);
    }

    GetTokenWhile(sCurrentFileName, dest, ".");
    sprintf(dest + strlen(dest), "_%s", sShaderName);
    if (dwShaderType == CODE_SVS)
        sprintf(dest + strlen(dest), ".vso");
    else
        sprintf(dest + strlen(dest), ".pso");
}

uint32_t CTechnique::ProcessShaderBin(shader_t *pS, char *pFile, uint32_t dwShaderType)
{
    char *pBuffer = nullptr;
    if (!fio->LoadFile(pFile, &pBuffer, nullptr))
    {
        core.Trace("ERROR: in file %s, file not found : %s", sCurrentFileName, pFile);
        return 0;
    }

    HRESULT hr;
    if (dwShaderType == CODE_SVS)
        hr = pRS->CreateVertexShader((uint32_t *)pBuffer, &pS->pVertexShader);
    else
        hr = pRS->CreatePixelShader((uint32_t *)pBuffer, &pS->pPixelShader);

    if (hr != D3D_OK)
        core.Trace("ERROR: can't create shader from %s\nfrom file: %s", pS->pName, pFile);

    STORM_DELETE(pBuffer);
    return 0;
}

uint32_t CTechnique::ProcessVertexShader(char *pFile, uint32_t dwSize, char **pStr)
{
    char temp[1024];

    // add shader
    char *pName = SkipToken(*pStr, VERTEX_SHADER);
    GetTokenWhile(pName, &temp[0], " ");

    auto i = AddShader(temp);
    shader_t *pS = &pShaders[i];
    pS->dwShaderType = CODE_SVS;

    while (nullptr != (*pStr = GetString(pFile, dwSize, *pStr)))
    {
        tolwr(*pStr);
        ClearComment(*pStr);
        if (isEndBracket(*pStr))
            break; // end of vertex shader
        if (isVertexDeclaration(*pStr))
            ProcessVertexDeclaration(pS, pFile, dwSize, pStr);
        if (isAsm(*pStr))
            ProcessShaderAsm(pS, pFile, dwSize, pStr, CODE_SVS);
        TOTAL_SKIP;
    }

    return SHADER_OK;
}

uint32_t CTechnique::ProcessPixelShader(char *pFile, uint32_t dwSize, char **pStr)
{
    char temp[1024];

    // add shader
    char *pName = SkipToken(*pStr, PIXEL_SHADER);
    GetTokenWhile(pName, &temp[0], " ");

    auto i = AddShader(temp);
    shader_t *pS = &pShaders[i];
    pS->dwShaderType = CODE_SPS;

    while (nullptr != (*pStr = GetString(pFile, dwSize, *pStr)))
    {
        tolwr(*pStr);
        ClearComment(*pStr);
        if (isEndBracket(*pStr))
            break; // end of vertex shader
        if (isAsm(*pStr))
            ProcessShaderAsm(pS, pFile, dwSize, pStr, CODE_SPS);
        TOTAL_SKIP;
    }

    return SHADER_OK;
}

uint32_t CTechnique::ProcessBlock(char *pFile, uint32_t dwSize, char **pStr)
{
    uint32_t i;
    char temp[1024];
#ifndef USE_FX // fx files has no parameters
    char pTempParamStr[1024];
#endif

    // resize
    pBlocks = (block_t *)realloc(pBlocks, sizeof(block_t) * (dwNumBlocks + 1));

    block_t *pB = &pBlocks[dwNumBlocks];
    *pB = {};

    dwNumParams = 0;

    // get name this block
#ifdef USE_FX
    char *pName = SkipToken(*pStr, TECHNIQUE);
#else
    char *pName = SkipToken(*pStr, BLOCK);
#endif
    strcpy_s(sCurrentBlockName, pName);
    GetTokenWhile(pName, &temp[0], "(");
    pB->dwHashBlockName = MakeHashValue(temp);
    const auto len = strlen(temp) + 1;
    pB->pBlockName = new char[len];
    memcpy(pB->pBlockName, temp, len);
    for (i = 0; i < dwNumBlocks; i++)
        if (pBlocks[i].dwHashBlockName == pB->dwHashBlockName &&
            (storm::iEquals(pBlocks[i].pBlockName, pB->pBlockName)))
        {
            core.Trace("ERROR: Techniques: Find duplicate technique name: %s", pB->pBlockName);
            break;
        }

    htBlocks[pB->pBlockName] = dwNumBlocks;
    // get parameters
#ifndef USE_FX // fx files has no parameters
    pTempParamStr[0] = 0;
    GetTokenWhile(SkipToken(*pStr, "("), &pTempParamStr[0], ")");
    char *pParamStr = &pTempParamStr[0];
    while (true)
    {
        // get type
        pParamStr = GetTokenWhile(pParamStr, &temp[0], " ");
        if (!pParamStr[0])
            break;
        pParams = (SRSPARAM *)realloc(pParams, sizeof(SRSPARAM) * (dwNumParams + 1));
        pB->pParams = (uint32_t *)realloc(pB->pParams, sizeof(uint32_t) * (dwNumParams + 1));
        pParams[dwNumParams].dwCode = GetCode(&temp[0], PARAM_TYPES, sizeof(PARAM_TYPES) / sizeof(SRSPARAM));
        pB->pParams[dwNumParams] = pParams[dwNumParams].dwCode;
        // get parameter name
        pParamStr = GetTokenWhile(pParamStr, &temp[0], ",");
        const auto len = strlen(temp) + 1;
        pParams[dwNumParams].cName = new char[len];
        strcpy_s(pParams[dwNumParams].cName, len, temp);
        dwNumParams++;
        pB->dwNumParams++;
    }
#endif

#ifdef USE_FX
    // HACK: remove after drop sha files support
    // For all lines except the first one, we need to get the previous line
    // in order to get correct line when entering the loop
    if (pFile != *pStr)
        (*pStr)--;
#endif

    // search for technique and '}'
    while (nullptr != (*pStr = GetString(pFile, dwSize, *pStr)))
    {
#define SKIP1                                                                                                          \
    {                                                                                                                  \
        (*pStr)++;                                                                                                     \
        continue;                                                                                                      \
    }
        tolwr(*pStr);
        ClearComment(*pStr);
        // if (isComment(*pStr)) SKIP1;
        if (isEndBracket(*pStr))
            break; // end of block
        if (isTechnique(*pStr))
        {
            ProcessTechnique(pFile, dwSize, pStr);
#ifdef USE_FX
            // HACK: remove after drop sha files support
            // fx files don't have "block" => it has 1 less level. Entry into ProcessBlock occurred
            // by "technique" and the exit from it should use SKIP1 for keepeng "{"
            (*pStr)--;
            continue;
#endif
        }
        SKIP1;
    }

    // delete parameters
    for (i = 0; i < dwNumParams; i++)
        STORM_DELETE(pParams[i].cName);
    dwNumParams = 0;
    STORM_DELETE(pParams);

    sCurrentBlockName[0] = 0;

    return BLOCK_OK;
}

void CTechnique::ClearSRS_STSS_bUse()
{
    uint32_t i, dwNumParam;

    dwNumParam = sizeof(RenderStates) / sizeof(SRS);
    for (i = 0; i < dwNumParam; i++)
        RenderStates[i].bUse = false;

    dwNumParam = sizeof(TexturesStageStates) / sizeof(STSS);
    for (i = 0; i < dwNumParam; i++)
    {
        TexturesStageStates[i].bUse = false;
        TexturesStageStates[i].dwUseSubCode = 0;
    }

    dwNumParam = sizeof(SampleStates) / sizeof(SAMP);
    for (i = 0; i < dwNumParam; i++)
    {
        SampleStates[i].bUse = false;
        SampleStates[i].dwUseSubCode = 0;
    }
}

void CTechnique::DecodeFiles(char *sub_dir)
{
    uint64_t dwRDTSC;
    RDTSC_B(dwRDTSC);
    pPassStorage = new uint32_t[16384];

    InnerDecodeFiles(sub_dir);

    STORM_DELETE(pPassStorage);
    RDTSC_E(dwRDTSC);
    core.Trace("Techniques: %d shaders compiled.", dwNumShaders);
    core.Trace("Techniques: %d techniques compiled.", dwNumBlocks);
    core.Trace("Techniques: compiled by %d ticks.", dwRDTSC);

    // some optimize
    for (uint32_t i = 0; i < dwNumBlocks; i++)
        STORM_DELETE(pBlocks[i].pBlockName);
}

void CTechnique::InnerDecodeFiles(char *sub_dir)
{
    sprintf(sCurrentDir, "%s%s", SHA_DIR, (sub_dir) ? sub_dir : "");

    const auto vFilenames = fio->_GetPathsOrFilenamesByMask(sCurrentDir, SHA_EXT, true, false, true, true);
    for (std::string path : vFilenames)
    {
        DecodeFile(path);
    }
}

bool CTechnique::DecodeFile(std::string sname)
{
    char fname[256];

    sprintf(fname, "%s", sname.c_str());
    strcpy(sCurrentFileName, fname);
    auto fileS = fio->_CreateFile(fname, std::ios::binary | std::ios::in);
    auto dwSize = fio->_GetFileSize(fname);
    Assert(dwSize != 0);
    char *pFile = new char[dwSize];
    fio->_ReadFile(fileS, pFile, dwSize);
    fio->_CloseFile(fileS);

    // change 0xd and 0xa to 0x0
    for (uint32_t i = 0; i < dwSize; i++)
    {
        if (pFile[i] == 0xD || pFile[i] == 0xA)
            pFile[i] = 0x0;
        if (pFile[i] == 0x9)
            pFile[i] = ' ';
    }

    char *pStr = pFile;

    // dwCurBlock = 0;

    while (nullptr != (pStr = GetString(pFile, dwSize, pStr)))
    {
#define SKIP                                                                                                           \
    {                                                                                                                  \
        pStr++;                                                                                                        \
        continue;                                                                                                      \
    }
        tolwr(pStr);
        ClearComment(pStr);
        // if (isComment(pStr)) SKIP;
        if (isPixelShader(pStr))
        {
            uint32_t dwRes = ProcessPixelShader(pFile, dwSize, &pStr);
            if (dwRes == SHADER_ERROR)
                return false;
        }
        if (isVertexShader(pStr))
        {
            uint32_t dwRes = ProcessVertexShader(pFile, dwSize, &pStr);
            if (dwRes == SHADER_ERROR)
                return false;
        }
#ifdef USE_FX
        if (isTechnique(pStr)) // fx files has no blocks
#else
        if (isBlock(pStr))
#endif
        {
            uint32_t dwRes = ProcessBlock(pFile, dwSize, &pStr);
            if (dwRes == BLOCK_ERROR)
                return false;
            dwNumBlocks++;
        }
        SKIP;
    }

    STORM_DELETE(pFile);
    return true;
}

// return true for drawbuffer, and false for exit
bool CTechnique::ExecutePassStart()
{
    // search block
    // ~!~
    auto it = htBlocks.find(sCurrentBlockName);
    if (it == htBlocks.end())
        return false;
    dwCurBlock = it->second;
    // for (uint32_t i=0;i<dwNumBlocks;i++) if (pBlocks[i].dwHashBlockName == dwHashCode &&
    // (storm::iEquals(pBlocks[i].pBlockName,sCurrentBlockName))) break; if (i==dwNumBlocks) return false;	// don't
    // find concur block Assert(i == dwFind); dwCurBlock = i;

    block_t *pB = &pBlocks[dwCurBlock];
    if (pB->dwNumTechniques == 0)
        return false; // not found any techniques

    // search first validated technique and execute
    // while execute technique 0
    dwCurTechnique = 0;
    dwCurNumPass = pB->pTechniques[dwCurTechnique].dwNumPasses;
    if (dwCurNumPass == 0)
        return false; // not found any passes in techniques

    dwCurPass = 0;

    // clear old saved states
    ClearSavedStates();

    return ExecutePass(true);
}

// return true for drawbuffer, and false for exit
bool CTechnique::ExecutePassNext()
{
    bool bExecute = ExecutePass(false);
    if (bExecute)
        return true; // if once again render in these pass
    dwCurPass++;
    return ExecutePass(true);
}

bool CTechnique::ExecutePassClose()
{
    ClearSavedStates();
    pRS->SetPixelShader(nullptr);
    return true;
}

uint32_t CTechnique::GetPassParameter(uint32_t dwParam, uint32_t dwFlags)
{
    if (dwFlags & FLAGS_CODE_IN_PARAM)
    {
        if (dwParam >= dwCurNumParams)
            throw std::runtime_error("Technique: Exceeded parameter numbers!");
        return pCurParams[dwParam];
    }

    return dwParam;
}

bool CTechnique::ExecutePass(bool bStart)
{
    uint32_t dwStage, dwValue, dwSaveValue;
    uint32_t dwCode, dwSubCode, dwRenderSubCode, dwRestoreSubCode;
    bool bSave;

    if (dwCurPass >= dwCurNumPass)
    {
        ExecutePassClose();
        return false;
    }

    pass_t *pCurPass = &pBlocks[dwCurBlock].pTechniques[dwCurTechnique].pPasses[dwCurPass];

    if (!bStart && dwCurPassSize == dwCurPassPos)
        return false;
    if (bStart)
    {
        dwCurPassPos = 0;
        dwCurPassSize = pCurPass->dwSize;
    }
    // begin or continue execute current pass
    uint32_t *pPassStart = &pCurPass->pPass[0];
    uint32_t *pPass = &pCurPass->pPass[dwCurPassPos];

    while (uint32_t(pPass - pPassStart) < dwCurPassSize)
    {
        dwCode = (*pPass++);
        dwSubCode = dwCode & 0xFFFF0000;
        dwCode &= 0xFFFF;
        bSave = (dwSubCode & FLAGS_CODE_RESTORE) ? true : false;

        switch (dwCode)
        {
            // SRS	type:	CODE_SRS,State index,value
            // STSS type:	CODE_STSS,Stage,State index,value
            // RENDER:		CODE_RENDER,SUBCODE_RENDER
            // TRANSFOR:	CODE_TRANSFORM,D3DTRANSFORMSTATETYPE,Value
            // TEXTURE:		CODE_TEXTURE,Stage,Value
            // RESTORE:		CODE_RESTORE
        case CODE_SRS: // SetRenderState(
        {
            D3DRENDERSTATETYPE State = (D3DRENDERSTATETYPE)(*pPass++);
            dwValue = GetPassParameter(*pPass++, dwSubCode);
            if (bSave)
            {
                pRS->GetRenderState(State, &dwSaveValue);
                if (dwSaveValue == dwValue)
                    break;
                AddState2Restore3(dwCode, State, dwSaveValue);
            }
            pRS->SetRenderState(State, dwValue);
        }
        break;
        case CODE_STSS: // SetTextureStageState(
        {
            dwStage = *pPass++;
            D3DTEXTURESTAGESTATETYPE StageState = (D3DTEXTURESTAGESTATETYPE)(*pPass++);
            dwValue = GetPassParameter(*pPass++, dwSubCode);
            if (bSave)
            {
                pRS->GetTextureStageState(dwStage, StageState, &dwSaveValue);
                if (dwSaveValue == dwValue)
                    break;
                AddState2Restore3(dwCode, dwStage, StageState);
                AddState2Restore(dwSaveValue);
            }
            pRS->SetTextureStageState(dwStage, StageState, dwValue);
        }
        break;
        case CODE_SAMP: // SetSamplerState(
        {
            dwStage = *pPass++;
            D3DSAMPLERSTATETYPE StageState = (D3DSAMPLERSTATETYPE)(*pPass++);
            dwValue = GetPassParameter(*pPass++, dwSubCode);
            if (bSave)
            {
                pRS->GetSamplerState(dwStage, StageState, &dwSaveValue);
                if (dwSaveValue == dwValue)
                    break;
                AddState2Restore3(dwCode, dwStage, StageState);
                AddState2Restore(dwSaveValue);
            }
            pRS->SetSamplerState(dwStage, StageState, dwValue);
        }
        break;
        case CODE_TEXTURE: // SetTexture(
        {
            uint32_t dwTextureIndex = *pPass++;
            int32_t iTextureIndex = (int32_t)*pPass++;
            if (iTextureIndex >= 0)
                iTextureIndex = (int32_t)GetPassParameter(*pPass++, dwSubCode);
            pRS->TextureSet(dwTextureIndex, iTextureIndex);
        }
        break;
        case CODE_TRANSFORM: // SetTransform(
        {
            D3DTRANSFORMSTATETYPE TransformType = (D3DTRANSFORMSTATETYPE)*pPass++;
            throw std::runtime_error("conversion from uint32_t to D3DMATRIX*");
            // pRS->SetTransform(TransformType,(D3DMATRIX*)GetPassParameter(*pPass++,dwSubCode));
        }
        break;
        case CODE_RENDER: // Render functions
        {
            dwRenderSubCode = *pPass++;
            switch (dwRenderSubCode)
            {
            case SUBCODE_RENDER_DRAW:
                dwCurPassPos = pPass - pPassStart;
                return true;
                break;
            }
        }
        break;
        case CODE_SVS: {
            uint32_t dwShaderIndex = *pPass++;
            if (dwShaderIndex != INVALID_SHADER_INDEX && pShaders[dwShaderIndex].pVertexShader != nullptr)
            {
                if (bSave)
                {
                    pRS->GetVertexShader((IDirect3DVertexShader9 **)&dwSaveValue);
                    AddState2Restore2(dwCode, dwSaveValue);
                }

                if (pShaders[dwShaderIndex].pVertexDecl != nullptr)
                    pRS->SetVertexDeclaration(pShaders[dwShaderIndex].pVertexDecl);

                pRS->SetVertexShader(pShaders[dwShaderIndex].pVertexShader);
            }
        }
        break;
        case CODE_SPS: {
            uint32_t dwShaderIndex = *pPass++;
            if (dwShaderIndex != INVALID_SHADER_INDEX && pShaders[dwShaderIndex].pPixelShader != nullptr)
            {
                if (bSave)
                {
                    pRS->GetPixelShader((IDirect3DPixelShader9 **)&dwSaveValue);
                    AddState2Restore2(dwCode, dwSaveValue);
                }

                pRS->SetPixelShader(pShaders[dwShaderIndex].pPixelShader);
            }
        }
        break;
        case CODE_SPSCONST: {
            uint32_t dwShaderConstIndex = *pPass++;
            throw std::runtime_error("conversion from uint32_t to const float *");
            // pRS->SetPixelShaderConstantF(dwShaderConstIndex, (const float*)GetPassParameter(*pPass++, dwSubCode), 1);
            //~!~
        }
        break;
        case CODE_SVSCONST: {
            uint32_t dwShaderConstIndex = *pPass++;
            uint32_t dwCode = *pPass++;
            switch (dwCode)
            {
            case SUBCODE_SVSCONST_WORLDVIEWPROJ: {
#ifdef _WIN32 // replace D3DXMATRIX
                D3DXMATRIX matTemp, matWorldViewProj;

                D3DXMATRIX matWorld;
                pRS->GetTransform(D3DTS_WORLD, &matWorld);
                D3DXMATRIX matProj;
                pRS->GetTransform(D3DTS_PROJECTION, &matProj);
                D3DXMATRIX matView;
                pRS->GetTransform(D3DTS_VIEW, &matView);

                D3DXMatrixMultiply(&matTemp, &matWorld, &matView);
                D3DXMatrixMultiply(&matWorldViewProj, &matTemp, &matProj);

                // Projection to clip space
                D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
                pRS->SetVertexShaderConstantF(dwShaderConstIndex, &matWorldViewProj(0, 0), 4);
#else
                throw std::runtime_error("replace D3DXMATRIX in technique.cpp");
#endif
            }
            break;
            }
        }
        break;
        case CODE_RESTORE: // restore states
            dwRestoreSubCode = *pPass++;
            switch (dwRestoreSubCode)
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

void inline CTechnique::AddState2Restore2(uint32_t dwState, uint32_t dw1)
{
    // add new memory if need
    if ((dwCurSavedStatesPos + 2) >= dwCurMaxSavedSize / sizeof(pSavedStates[0]))
    {
        pSavedStates =
            (uint32_t *)realloc(pSavedStates, dwCurMaxSavedSize + SAVED_STATES_ADD * sizeof(pSavedStates[0]));
        dwCurMaxSavedSize += SAVED_STATES_ADD;
    }

    // save state (or part of state)
    pSavedStates[dwCurSavedStatesPos++] = dwState;
    pSavedStates[dwCurSavedStatesPos++] = dw1;
}

void inline CTechnique::AddState2Restore3(uint32_t dwState, uint32_t dw1, uint32_t dw2)
{
    // add new memory if need
    if ((dwCurSavedStatesPos + 3) >= dwCurMaxSavedSize / sizeof(pSavedStates[0]))
    {
        pSavedStates =
            (uint32_t *)realloc(pSavedStates, dwCurMaxSavedSize + SAVED_STATES_ADD * sizeof(pSavedStates[0]));
        dwCurMaxSavedSize += SAVED_STATES_ADD;
    }

    // save state (or part of state)
    pSavedStates[dwCurSavedStatesPos++] = dwState;
    pSavedStates[dwCurSavedStatesPos++] = dw1;
    pSavedStates[dwCurSavedStatesPos++] = dw2;
}

void inline CTechnique::AddState2Restore(uint32_t dwState)
{
    // add new memory if need
    if (dwCurSavedStatesPos >= dwCurMaxSavedSize / sizeof(pSavedStates[0]))
    {
        pSavedStates =
            (uint32_t *)realloc(pSavedStates, dwCurMaxSavedSize + SAVED_STATES_ADD * sizeof(pSavedStates[0]));
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
    uint32_t dwCode, dwValue;
    // restore saved render states and texture stage states

    // for walking
    uint32_t dwTempSavedStatesPos = 0;

    while (dwTempSavedStatesPos < dwCurSavedStatesPos)
    {
        dwCode = pSavedStates[dwTempSavedStatesPos++];
        switch (dwCode)
        {
        case CODE_SRS: {
            D3DRENDERSTATETYPE State = (D3DRENDERSTATETYPE)pSavedStates[dwTempSavedStatesPos++];
            dwValue = pSavedStates[dwTempSavedStatesPos++];
            pRS->SetRenderState(State, dwValue);
        }
        break;
        case CODE_STSS: {
            uint32_t dwStage = pSavedStates[dwTempSavedStatesPos++];
            D3DTEXTURESTAGESTATETYPE StageState = (D3DTEXTURESTAGESTATETYPE)pSavedStates[dwTempSavedStatesPos++];
            dwValue = pSavedStates[dwTempSavedStatesPos++];
            pRS->SetTextureStageState(dwStage, StageState, dwValue);
        }
        break;
        case CODE_SAMP: {
            uint32_t dwStage = pSavedStates[dwTempSavedStatesPos++];
            D3DSAMPLERSTATETYPE StageState = (D3DSAMPLERSTATETYPE)pSavedStates[dwTempSavedStatesPos++];
            dwValue = pSavedStates[dwTempSavedStatesPos++];
            pRS->SetSamplerState(dwStage, StageState, dwValue);
        }
        break;
        case CODE_SPS:
            dwValue = pSavedStates[dwTempSavedStatesPos++];
            throw std::runtime_error("conversion from uint32_t to IDirect3DPixelShader9 *");
            // pRS->SetPixelShader((IDirect3DPixelShader9*)dwValue);
            break;
        case CODE_SVS:
            dwValue = pSavedStates[dwTempSavedStatesPos++];
            throw std::runtime_error("conversion from uint32_t to IDirect3DPixelShader9 *");
            // pRS->SetVertexShader((IDirect3DVertexShader9*)dwValue);
            break;
        }
    }

    ClearSavedStates();
}

void CTechnique::SetCurrentBlock(const char *name, uint32_t _dwNumParams, void *pParams)
{
    if (name && name[0])
    {

        strcpy_s(sCurrentBlockName, name);
        tolwr(sCurrentBlockName);
        // dwHashCode = MakeHashValue(sCurrentBlockName);
        dwCurNumParams = _dwNumParams;

        if (dwCurNumParams > dwCurParamsMax)
        {
            while (dwCurNumParams > dwCurParamsMax)
                dwCurParamsMax += 10;
            pCurParams = (uint32_t *)realloc(pCurParams, sizeof(uint32_t) * dwCurParamsMax);
        }

        for (uint32_t i = 0; i < _dwNumParams; i++)
            pCurParams[i] = ((uint32_t *)pParams)[i];
    }
    else
    {
        core.Trace("ERROR: SetCurrentBlock: unknown technique <%s> first character is <%s> ", name, name[0]);
    }
}
#endif // _WIN32
