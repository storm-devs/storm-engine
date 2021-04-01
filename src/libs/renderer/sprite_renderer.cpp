#include "sprite_renderer.h"

#include "matrix.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <vector>
#include <algorithm>

bgfx::VertexLayout SPRITE_VERTEX::ms_layout;


SpriteRenderer::SpriteRenderer(long m_fbWidth, long m_fbHeight)
    : m_spriteQueueSize(0), m_spriteQueueCount(0), m_backbufferTexture(nullptr)
//SpriteRenderer::SpriteRenderer()
{
    SPRITE_VERTEX::Init();

    m_width = m_fbWidth;
    m_height = m_fbHeight;

    /*m_fvbh = bgfx::createDynamicVertexBuffer(bgfx::makeRef(s_spriteVertices, sizeof(s_spriteVertices)),
                                             SPRITE_VERTEX::ms_layout);*/
    //m_fibh = bgfx::createDynamicIndexBuffer(bgfx::makeRef(s_spriteIndices, sizeof(s_spriteIndices)));

    const uint32_t indicesCount = MaxBatchSize * 6;

    std::vector<uint16_t> indices;
    indices.reserve(indicesCount);

    for (uint32_t i = 0; i < indicesCount; i += 4)
    {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);

        indices.push_back(i + 1);
        indices.push_back(i + 3);
        indices.push_back(i + 2);
    }

    m_ibh = bgfx::createIndexBuffer(bgfx::copy(indices.data(), indicesCount * sizeof(uint16_t)));


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

    std::vector<unsigned char> byteOutput;

    byteOutput.resize(m_width * m_height * 4);
}

SpriteRenderer::~SpriteRenderer()
{
    //bgfx::destroy(m_fvbh);
    //bgfx::destroy(m_fibh);
    bgfx::destroy(m_ibh);
    bgfx::destroy(m_prog);
}

void SpriteRenderer::SetBackbufferTexture(std::shared_ptr<TextureResource> texture)
{
    m_backbufferTexture = texture;
}

void SpriteRenderer::ReleaseTexture(std::shared_ptr<TextureResource> texture)
{
    bgfx::destroy(*texture->textureHandle);
}


void SpriteRenderer::SetViewProjection()
{
    const bx::Vec3 at = {0.0f, 0.0f, 0.0};
    const bx::Vec3 eye = {0.0f, 0.0f, -3.0};

    // Set view and projection matrix for view 0.
    {
        /*glm::vec2 m_position;
        float m_zoom;
        float m_angle;
        glm::vec2 m_bounds;

        m_bounds.x = 1920;
        m_bounds.y = 1080;

        
        m_zoom = 0.0f;
        m_angle = 0.0f;

        auto mtx_view = glm::translate(glm::mat4(1.f), glm::vec3(m_bounds * 0.5f, 0.f)) *
                    glm::scale(glm::mat4(1.f), glm::vec3(m_zoom)) *
                    glm::rotate(glm::mat4(1.f), m_angle, glm::vec3(0, 0, 1.f)) *
                    glm::translate(glm::mat4(1.f), glm::vec3(-m_position, 0.f));*/


        glm::mat4 view(1);
        bx::mtxLookAt(glm::value_ptr(view), eye, at);

        //glm::mat4 proj(1);
        //bx::mtxProj(glm::value_ptr(proj), 60.0f, float(1920) / float(1080), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

        glm::mat4 proj(1);
        bx::mtxOrtho(glm::value_ptr(proj), 0.0f, float(m_width), float(m_height), 0.0f, 0.0f, 1000.0f, 0.0f,
        bgfx::getCaps()->homogeneousDepth);

        //bgfx::setViewTransform(0, glm::value_ptr(mtx_view), proj);
        bgfx::setViewTransform(0, glm::value_ptr(view), glm::value_ptr(proj));

        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(1920), uint16_t(1080));
    }
}

void SpriteRenderer::UpdateIndexBuffer(std::vector<uint16_t> indices){
    /*const bgfx::Memory *mem = bgfx::alloc(indices.size() * sizeof(uint16_t));

    memcpy(mem->data, indices.data(), indices.size() * sizeof(uint16_t));

    bgfx::update(m_fibh, 0, mem);*/

    throw std::exception("Does not apply for static index buffers");
}

void SpriteRenderer::UpdateVertexBuffer(std::vector<glm::vec3> &vertices, glm::vec2 &u, glm::vec2 &v, uint32_t &color, float depth)
{
    // m_color[0] = m_color[1] = m_color[2] = m_color[3] = 1.0f;

    // bgfx::setUniform(u_color, &m_color);
    const bgfx::Memory *mem = bgfx::alloc(vertices.size() * sizeof(SPRITE_VERTEX));

    SPRITE_VERTEX *vertex = (SPRITE_VERTEX *)mem->data;
    
    int index = 0;

    vertex[index + 0] = SPRITE_VERTEX{vertices[index + 0].x, vertices[index + 0].y, u.x, v.x /*, color*/};
    vertex[index + 1] = SPRITE_VERTEX{vertices[index + 1].x, vertices[index + 1].y, u.y, v.x /*, color*/};
    vertex[index + 2] = SPRITE_VERTEX{vertices[index + 2].x, vertices[index + 2].y, u.x, v.y /*, color*/};
    vertex[index + 3] = SPRITE_VERTEX{vertices[index + 3].x, vertices[index + 3].y, u.y, v.y /*, color*/};

    std::vector<SPRITE_VERTEX> storageVertices =
        std::vector<SPRITE_VERTEX>{vertex[index + 0], vertex[index + 1], vertex[index + 2], vertex[index + 3]};

    /*
        sprite->tl = glm::vec2(points[0].x, points[0].y);
        sprite->tr = glm::vec2(points[1].x, points[1].y);
        sprite->bl = glm::vec2(points[2].x, points[2].y);
        sprite->br = glm::vec2(points[3].x, points[3].y);

    */
    /*vertex[0].x = vertices[0].x;
    vertex[0].y = vertices[0].y;
    vertex[1].x = vertices[2].x;
    vertex[1].y = vertices[2].y;
    vertex[2].x = vertices[3].x;
    vertex[2].y = vertices[3].y;
    vertex[3].x = vertices[1].x;
    vertex[3].y = vertices[1].y;*/


    // auto source = Rect{0, 0, m_width, m_heigt};

    // GetTextureMappingsHalfIn(tex, source, vertex);

    if (m_spriteQueueCount >= m_spriteQueue.size())
    {
        uint32_t newSize = std::max(InitialQueueSize, (uint32_t)m_spriteQueue.size() * 2);
        m_spriteQueue.resize(newSize);

        m_sortedSprites.clear();
    }

    SpriteInfo *sprite = &m_spriteQueue[m_spriteQueueCount];

    sprite->texture = Texture;
    sprite->vertices = storageVertices;
    sprite->depth = depth;
    ++m_spriteQueueCount;


    //bgfx::update(m_fvbh, 0, mem);
}


void SpriteRenderer::Submit()
{
    // Set vertex and index buffer.
    //bgfx::setVertexBuffer(0, m_fvbh);
    //bgfx::setIndexBuffer(m_fibh);
    if (m_sortedSprites.size() < m_spriteQueueCount)
    {
        uint32_t previousSize = (uint32_t)m_sortedSprites.size();
        m_sortedSprites.resize(m_spriteQueueCount);
        for (size_t i = previousSize; i < m_spriteQueueCount; ++i)
        {
            m_sortedSprites[i] = &m_spriteQueue[i];
        }

        std::sort(std::begin(m_sortedSprites), std::begin(m_sortedSprites) + m_spriteQueueCount,
        [](const SpriteInfo *x, const SpriteInfo *y) { return x->depth < y->depth; });

    }


    bgfx::TransientVertexBuffer vb;
    bgfx::allocTransientVertexBuffer(&vb, 4 * m_spriteQueueCount, SPRITE_VERTEX::ms_layout);
    SPRITE_VERTEX *vertex = (SPRITE_VERTEX *)vb.data;

    for (uint32_t i = 0, v = 0; i < m_spriteQueueCount; ++i, v += 4)
    {
        const SpriteInfo *s = m_sortedSprites[i];
        vertex[v + 0] = s->vertices[0];
        vertex[v + 1] = s->vertices[1];
        vertex[v + 2] = s->vertices[2];
        vertex[v + 3] = s->vertices[3];
    }

    std::shared_ptr<TextureResource> batchTexture(nullptr);
    uint32_t batchStart = 0;

    uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA | BGFX_STATE_BLEND_ALPHA;

    for (uint32_t pos = 0; pos < m_spriteQueueCount; ++pos)
    {
        auto texture = m_sortedSprites[pos]->texture;

        if (texture != batchTexture)
        {
            if (pos > batchStart)
            {
                // TODO maybe hax
                bgfx::setState(state);
                bgfx::setTexture(0, s_texColor, *batchTexture->textureHandle);
                bgfx::setVertexBuffer(0, &vb);
                bgfx::setIndexBuffer(m_ibh, batchStart * 6, (pos - batchStart) * 6);
                bgfx::submit(0, m_prog);
            }
            batchTexture = texture;
            batchStart = pos;
        }
    }

    bgfx::setState(state);

    bgfx::setTexture(0, s_texColor, *batchTexture->textureHandle);
    bgfx::setVertexBuffer(0, &vb);
    bgfx::setIndexBuffer(m_ibh, batchStart * 6, (m_spriteQueueCount - batchStart) * 6);
    bgfx::submit(0, m_prog);

    m_spriteQueueCount = 0;

    m_sortedSprites.clear();

    if (m_backbufferTexture != nullptr)
    {
        ReleaseTexture(m_backbufferTexture);
    }
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