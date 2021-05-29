#include "sprite_renderer.h"

bgfx::VertexLayout SPRITE_VERTEX::ms_layout;


//SpriteRenderer::SpriteRenderer(long m_fbWidth, long m_fbHeight)
SpriteRenderer::SpriteRenderer()
{
    SPRITE_VERTEX::Init();

    //m_width = m_fbWidth;
    //m_height = m_fbHeight;

    m_fvbh = bgfx::createDynamicVertexBuffer(bgfx::makeRef(s_spriteVertices, sizeof(s_spriteVertices)),
                                             SPRITE_VERTEX::ms_layout);
    m_fibh = bgfx::createDynamicIndexBuffer(bgfx::makeRef(s_spriteIndices, sizeof(s_spriteIndices)));

    /*m_sfvbh = bgfx::createVertexBuffer(bgfx::makeRef(s_fontVertices, sizeof(s_fontVertices)),
                                               BGFX_FONT_VERTEX::ms_layout);
            m_sfibh = bgfx::createIndexBuffer(bgfx::makeRef(s_fontIndices, sizeof(s_fontIndices)));*/

    const bgfx::Memory *memVsh = shaderc::compileShader(shaderc::ST_VERTEX, "shaders/dx9/vs_font.sc");
    const bgfx::Memory *memFsh = shaderc::compileShader(shaderc::ST_FRAGMENT, "shaders/dx9/fs_font.sc");

    bgfx::ShaderHandle vsh = bgfx::createShader(memVsh);
    bgfx::ShaderHandle fsh = bgfx::createShader(memFsh);

    m_prog = bgfx::createProgram(vsh, fsh, true);

    s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    // u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
}

SpriteRenderer::~SpriteRenderer()
{
    bgfx::destroy(m_fvbh);
    bgfx::destroy(m_fibh);
    bgfx::destroy(m_prog);
}

void SpriteRenderer::SetViewProjection()
{
    const bx::Vec3 at = {0.0f, 0.0f, 0.0};
    const bx::Vec3 eye = {0.0f, 0.0f, -3.0};

    // Set view and projection matrix for view 0.
    {
        float view[16];
        bx::mtxLookAt(view, eye, at);

        float proj[16];
        bx::mtxProj(proj, 60.0f, float(1920) / float(1080), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

        /*bx::mtxOrtho(proj, -camera.orthoSize * camera.aspectRatio,
                     camera.orthoSize * camera.aspectRatio, -camera.orthoSize, camera.orthoSize, camera.cnear,
                     camera.cfar, 0, bgfx::getCaps()->homogeneousDepth);*/

        bgfx::setViewTransform(0, view, proj);

        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(1920), uint16_t(1080));
    }
}

void SpriteRenderer::UpdateVertexBuffer()
{
    // m_color[0] = m_color[1] = m_color[2] = m_color[3] = 1.0f;

    // bgfx::setUniform(u_color, &m_color);

    const bgfx::Memory *mem = bgfx::copy(s_spriteVertices, sizeof(s_spriteVertices));
    SPRITE_VERTEX *vertex = (SPRITE_VERTEX *)mem->data;

    // auto source = Rect{0, 0, m_width, m_heigt};

    // GetTextureMappingsHalfIn(tex, source, vertex);

    bgfx::update(m_fvbh, 0, mem);
}

void SpriteRenderer::Submit()
{
    // Set vertex and index buffer.
    bgfx::setVertexBuffer(0, m_fvbh);
    bgfx::setIndexBuffer(m_fibh);

    bgfx::setTexture(0, s_texColor, *Texture->textureHandle);

    uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA |
                     BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_DST_COLOR, BGFX_STATE_BLEND_ZERO);

    // Set render states.
    bgfx::setState(state);

    // Submit primitive for rendering to view 0.
    bgfx::submit(0, m_prog);
}

void SpriteRenderer::GetVertices(std::shared_ptr<TextureResource> texture, Rect source, SPRITE_VERTEX *vertices)
{
    // Calculate texture source mappings
    float uLeft = 2 * (source.x / texture->size.width) - 1;
    float uRight = 2 * ((source.x + source.width) / texture->size.width) - 1;
    float vTop = 2 * ((source.y) / texture->size.height) - 1;
    float vBottom = 2 * ((source.y + source.height) / texture->size.height) - 1;

    vertices[0].x = uLeft;
    vertices[0].y = vTop;
    vertices[1].x = uLeft;
    vertices[1].y = vBottom;
    vertices[2].x = uRight;
    vertices[2].y = vBottom;
    vertices[3].x = uRight;
    vertices[3].y = vTop;
}