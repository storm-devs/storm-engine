#include "bgfx_utils.h"
#include "brtshaderc.h"

#ifndef SPRITE_RENDERER_DEF
#define SPRITE_RENDERER_DEF


struct SPRITE_VERTEX
{
    float x;
    float y;

    // uint32_t color;
    float tu, tv;

    static void Init()
    {
        ms_layout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            //.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true)
            .end();
    };

    static bgfx::VertexLayout ms_layout;
};



static SPRITE_VERTEX s_spriteVertices[] = {
    {0.5, 0.5, 1, 0}, 
    {0.5, -0.5, 1, 1}, 
    {-0.5, -0.5, 0, 1}, 
    {-0.5, 0.5, 0, 0}
};

static constexpr uint16_t s_spriteIndices[6] = {3, 2, 1, 3, 1, 0};


struct Rect
{
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
};

class SpriteRenderer
{
    bgfx::DynamicVertexBufferHandle m_fvbh;
    bgfx::DynamicIndexBufferHandle m_fibh;

    //bgfx::VertexBufferHandle m_sfvbh;
    //bgfx::IndexBufferHandle m_sfibh;

    bgfx::UniformHandle s_texColor;
    //bgfx::UniformHandle u_color;
    bgfx::ProgramHandle m_prog;

    
    void GetVertices(std::shared_ptr<TextureResource> texture, Rect source, SPRITE_VERTEX *vertices);

    //long m_width;
    //long m_height;

  public:
    std::shared_ptr<TextureResource> Texture;

    //SpriteRenderer(long m_fbWidth, long m_fbHeight);
    SpriteRenderer();

    ~SpriteRenderer();

    void SetViewProjection();

    void UpdateVertexBuffer();

    void Submit();
};

#endif
