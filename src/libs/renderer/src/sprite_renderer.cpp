#include "sprite_renderer.h"
#include "brtshaderc.h"

#include "matrix.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <vector>
#include <algorithm>

bgfx::VertexLayout SPRITE_VERTEX::sr_layout;


SpriteRenderer::SpriteRenderer(long m_fbWidth, long m_fbHeight)
    : m_spriteQueueSize(0), m_spriteQueueCount(0), m_backbufferTexture(nullptr)
//SpriteRenderer::SpriteRenderer()
{
    SPRITE_VERTEX::Init();

    m_width = m_fbWidth;
    m_height = m_fbHeight;

    const bgfx::Memory *memVsh = shaderc::compileShader(shaderc::ST_VERTEX, "shaders/dx9/vs_font.sc");
    const bgfx::Memory *memFsh = shaderc::compileShader(shaderc::ST_FRAGMENT, "shaders/dx9/fs_font.sc");

    bgfx::ShaderHandle vsh = bgfx::createShader(memVsh);
    bgfx::ShaderHandle fsh = bgfx::createShader(memFsh);

    m_prog = bgfx::createProgram(vsh, fsh, true);

    s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    // u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);

    m_backbufferMemory.resize(m_width * m_height * 4);

    auto rttTex = loadMemoryTexture("Sprite backbuffer", bgfx::TextureFormat::BGRA8, m_width, m_height, 1, false, false,
                                    0, nullptr);
    
    SetBackbufferTexture(rttTex);
}

SpriteRenderer::~SpriteRenderer()
{
    //bgfx::destroy(m_fvbh);
    //bgfx::destroy(m_fibh);
    ReleaseTexture(this->GetBackBufferTexture());
    bgfx::destroy(m_prog);
}

std::shared_ptr<TextureResource> SpriteRenderer::GetBackBufferTexture()
{
    return m_backbufferTexture;
}

void SpriteRenderer::SetBackbufferTexture(std::shared_ptr<TextureResource> texture)
{
    m_backbufferTexture = texture;
}

void SpriteRenderer::BlitBackBuffer(unsigned char *data)
{
    memcpy((void *)m_backbufferMemory.data(), data, m_width * m_height * 4);
    bgfx::updateTexture2D(*m_backbufferTexture->textureHandle, 0, 0, 0, 0, m_width, m_height, bgfx::makeRef(m_backbufferMemory.data(), m_backbufferMemory.size()));

}

void SpriteRenderer::ReleaseTexture(std::shared_ptr<TextureResource> texture)
{
    bgfx::destroy(*texture->textureHandle);
}

/*
#include "Matrix.h"

class MatrixState {
public:

    MatrixState(std::vector<float> world, 
                std::vector<float> view, 
                std::vector<float> projection)
    {
        World = world;
        View = view;
        Projection = projection;
    }

    std::vector<float> GetWorld()
    {
        return World;
    }

    std::vector<float> GetView()
    {
        return View;
    }

    std::vector<float> GetProjection()
    {
        return Projection;
    }

private:
    std::vector<float> World;
    std::vector<float> View;
    std::vector<float> Projection;
};

static std::vector<MatrixState> matrixStates;*/

void SpriteRenderer::SetViewProjection()
{
    const bx::Vec3 at = {0.0f, 0.0f, 0.0};
    const bx::Vec3 eye = {0.0f, 0.0f, -3.0};

    glm::mat4 model(1);

    bx::mtxIdentity(glm::value_ptr(model));

    bgfx::setTransform(glm::value_ptr(model));

    glm::mat4 view(1);
    bx::mtxLookAt(glm::value_ptr(view), eye, at);
    glm::mat4 proj(1);
    bx::mtxOrtho(glm::value_ptr(proj), 0.0f, float(m_width), float(m_height), 0.0f, 0.0f, 10.0f, 0.0f,
    bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(0, glm::value_ptr(view), glm::value_ptr(proj));
}

void SpriteRenderer::UpdateIndexBuffer(std::vector<uint16_t> indices){
    /*const bgfx::Memory *mem = bgfx::alloc(indices.size() * sizeof(uint16_t));

    memcpy(mem->data, indices.data(), indices.size() * sizeof(uint16_t));

    bgfx::update(m_fibh, 0, mem);*/

    throw std::exception("Does not apply for static index buffers");
}

void SpriteRenderer::PushVertices(std::vector<SPRITE_VERTEX> vertices, float depth)
{
    if (m_spriteQueueCount >= m_spriteQueue.size())
    {
        uint32_t newSize = std::max(InitialQueueSize, (uint32_t)m_spriteQueue.size() * 2);
        m_spriteQueue.resize(newSize);

        m_sortedSprites.clear();
    }

    SpriteInfo *sprite = &m_spriteQueue[m_spriteQueueCount];

    sprite->texture = Texture;
    sprite->vertices = vertices;
    sprite->depth = depth;
    ++m_spriteQueueCount;
}

void SpriteRenderer::Submit()
{

    if (m_spriteQueueCount == 0)
    {
        return;
    }

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
    bgfx::allocTransientVertexBuffer(&vb, 4 * m_spriteQueueCount, SPRITE_VERTEX::sr_layout);
    SPRITE_VERTEX *vertex = (SPRITE_VERTEX *)vb.data;

    for (uint32_t i = 0, v = 0; i < m_spriteQueueCount; ++i, v += 4)
    {
        const SpriteInfo *s = m_sortedSprites[i];
        vertex[v + 0] = s->vertices[0];
        vertex[v + 1] = s->vertices[1];
        vertex[v + 2] = s->vertices[2];
        vertex[v + 3] = s->vertices[3];
    }

    const uint32_t indicesCount = m_spriteQueueCount * 6;

    std::vector<uint16_t> indices;
    indices.reserve(indicesCount);

    for (uint16_t i = 0; i < indicesCount; i += 4)
    {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);

        indices.push_back(i + 1);
        indices.push_back(i + 3);
        indices.push_back(i + 2);
    }

    bgfx::TransientIndexBuffer ib;

    bgfx::allocTransientIndexBuffer(&ib, indicesCount, false);

    auto ibPtr = reinterpret_cast<uint16_t *>(ib.data);

    for (long i = 0; i < indices.size(); ++i)
    {
        ibPtr[i] = indices[i];
    }

    std::shared_ptr<TextureResource> batchTexture(nullptr);
    uint16_t batchStart = 0;

    uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA;

    for (uint16_t pos = 1; pos < m_spriteQueueCount + 1; ++pos)
    {
        auto texture = m_sortedSprites[pos - 1]->texture;
        //int32_t depth = static_cast<int32_t>(m_sortedSprites[pos]->depth);
        if (texture != batchTexture)
        {
            if (pos > batchStart)
            {
                bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);
                bgfx::setState(state);
                bgfx::setTexture(0, s_texColor, *texture->textureHandle);
                bgfx::setVertexBuffer(0, &vb);
                bgfx::setIndexBuffer(&ib, batchStart * 6, (pos - batchStart) * 6);
                //bgfx::setIndexBuffer(m_ibh, batchStart * 6, (pos - batchStart) * 6);
                bgfx::submit(0, m_prog);
                //bgfx::submit(1, m_prog);
            }
            batchTexture = texture;
            batchStart = pos;
        }
    }

    
    m_spriteQueueCount = 0;

    m_sortedSprites.clear();

    /*if (m_backbufferTexture != nullptr)
    {
        ReleaseTexture(m_backbufferTexture);
    }*/
}