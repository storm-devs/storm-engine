#pragma once

#include "bgfx/bgfx.h"
#include "bgfx_utils.h"
#include "brtshaderc.h"

#include "dx9render.h"
#include "matrix.h"
#include "vmodule_api.h"
//#include "iimage.h"

#define USED_CODES 0x2070 // end of https://unicode-table.com/en/blocks/general-punctuation/
#define MAX_SYMBOLS 512
#define SYM_VERTEXS 6

#ifndef IMAGE_VERTEX_DEF
#define IMAGE_VERTEX_DEF
struct IMAGE_VERTEX
{
    CVECTOR pos;
    float rhw;
    uint32_t color;
    float tu, tv;
};

struct BGFX_FONT_VERTEX
{
    CVECTOR pos;
    uint32_t color;
    float tu, tv;


    static void init()
    {
        ms_layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
    };

    friend class constructor;

    struct constructor
    {
        constructor()
        {
            BGFX_FONT_VERTEX::init();
        }
    };

    static constructor vertexcons;

    static bgfx::VertexLayout ms_layout;
};


#endif

#ifndef IMAGE_SHADER_DEF
#define IMAGE_SHADER_DEF

static const float s_texcoord = 5.0f;

static BGFX_FONT_VERTEX s_fontVertices[] = 
{
    {{-1.0f, -1.0f, 0.0f}, encodeNormalRgba8(0.0f, 1.0f, 0.0f), -1, -1}, 
    {{-1.0f, 1.0f, 0.0f}, encodeNormalRgba8(0.0f, 1.0f, 0.0f), -1, 1}, 
    {{1.0f, 1.0f, 0.0f}, encodeNormalRgba8(0.0f, 1.0f, 0.0f), 1, 1}, 
    {{1.0f, -1.0f, 0.0f}, encodeNormalRgba8(0.0f, 1.0f, 0.0f), 1, -1}, 
};


static const uint16_t s_fontIndices[] = {
    0, 1, 2, 1, 3, 2,
};


struct RenderState
{
	enum Enum
	{
		Default = 0,
		Count
	};

	uint64_t m_state;
	uint32_t m_blendFactorRgba;
	uint32_t m_fstencil;
	uint32_t m_bstencil;
};

static RenderState s_renderStates[RenderState::Count] =
{
	{ // Default
		0
		| BGFX_STATE_WRITE_RGB
		| BGFX_STATE_WRITE_A
		| BGFX_STATE_DEPTH_TEST_LESS
		| BGFX_STATE_WRITE_Z
		| BGFX_STATE_CULL_CCW
		| BGFX_STATE_MSAA
		, UINT32_MAX
		, BGFX_STENCIL_NONE
		, BGFX_STENCIL_NONE
	}
};


static bgfx::DynamicVertexBufferHandle m_vbh;
static bgfx::DynamicIndexBufferHandle m_ibh;

static bgfx::UniformHandle s_texColor;
static bgfx::ProgramHandle m_progFont;

struct BGFX_FONT_SHADER
{
    friend class constructor;

    struct constructor
    {
        constructor()
        {
            m_progFont = BGFX_INVALID_HANDLE;
            const bgfx::Memory *memVsh = shaderc::compileShader(shaderc::ST_VERTEX, "vs_cubes.sc");
            const bgfx::Memory *memFsh = shaderc::compileShader(shaderc::ST_FRAGMENT, "fs_font.sc");

            bgfx::ShaderHandle vsh = bgfx::createShader(memVsh);
            bgfx::ShaderHandle fsh = bgfx::createShader(memFsh);

            m_progFont = bgfx::createProgram(vsh, fsh, true);

            s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
        }

        ~constructor()
        {
            bgfx::destroy(s_texColor);

            bgfx::destroy(m_ibh);
            bgfx::destroy(m_vbh);
            bgfx::destroy(m_progFont);
        }
    };

    static void SetViewProjectionMatrix()
    {
        const bx::Vec3 at = {0.0f, 0.0f, -1.0f};
        const bx::Vec3 eye = {0.0f, 0.0f, 0.0f};

        float view[16];
        bx::mtxLookAt(view, eye, at);

        float proj[16];
        bx::mtxProj(proj, 60.0f, float(1920) / float(1080), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);

        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(1920), uint16_t(1080));
    }

    static void UpdateVertexBuffer()
    {
        const bgfx::Memory *mem = bgfx::copy(s_fontVertices, sizeof(s_fontVertices));
        BGFX_FONT_VERTEX *vertex = (BGFX_FONT_VERTEX *)mem->data;
        for (uint32_t ii = 0; ii < BX_COUNTOF(s_fontVertices); ++ii)
        {
            vertex[ii].color = encodeNormalRgba8(0.0f, 0.0f, 1.0f);
            vertex[ii].tu = 0.5f;
            vertex[ii].tv = 0.5f;
        }
        bgfx::update(m_vbh, 0, mem);
    }

    static void Submit()
    {
    
    	bgfx::setState(0 | BGFX_STATE_DEFAULT);

        // Submit primitive for rendering to view 0.
        bgfx::submit(0, m_progFont);
    }
    

    static constructor shadercons;
};

#endif

#define IMAGE_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

#ifndef FLOAT_RECT_DEF
#define FLOAT_RECT_DEF

struct FLOAT_RECT
{
    float x1, y1, x2, y2;
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
    VDX9RENDER *RenderService;
    IDirect3DDevice9 *Device;
    IDirect3DVertexBuffer9 *VBuffer;
    char *techniqueName;
    char *textureName;
    long TextureID;
    uint32_t Color, oldColor;
    float fScale, fOldScale;
    FLOAT_RECT Pos;
    FONT_SYMBOL CharT[USED_CODES];
    long Height;
    long Texture_XSize;
    long Texture_YSize;
    long Symbol_interval;

    bool bShadow, bOldShadow;
    long Shadow_offsetx;
    long Shadow_offsety;

    long Spacebar;

    bool bInverse, bOldInverse;

    float m_fAspectRatioH;
    float m_fAspectRatioV;

  public:
    FONT();
    ~FONT();
    bool Init(const char *font_name, const char *iniName, IDirect3DDevice9 *_device, VDX9RENDER *_render);
    void TempUnload();
    void RepeatInit();
    void Realize(uint32_t DeltaTime);
    void SetColor(uint32_t color);
    void SetScale(float scale);
    void SetShadow(bool s);
    void SetCenter(bool c);
    bool MakeLong(char **pDataPointer, long *result);
    long Printf(long x, long y, char *Text, ...);
    long Print(long x, long y, char *Text);
    long UpdateVertexBuffer(long x, long y, char *data_PTR, int utf8length);
    long GetStringWidth(const char *Text);
    long GetHeight()
    {
        return Height;
    };
    void Inverse(bool v);
    void SetShadowOffset(long ox, long oy);
    void GetShadowOffset(long &ox, long &oy);
    void StoreFontParameters()
    {
        oldColor = Color;
        fOldScale = fScale;
        bOldShadow = bShadow;
        bOldInverse = bInverse;
    };
    void RestoreFontParameters()
    {
        Color = oldColor;
        fScale = fOldScale;
        bShadow = bOldShadow;
        bInverse = bOldInverse;
    };
};
