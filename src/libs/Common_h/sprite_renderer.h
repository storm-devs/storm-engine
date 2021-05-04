#include "bgfx_utils.h"

#include "glm.hpp"

#include <vector>
#include <map>


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
        sr_layout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            //.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true)
            .end();
    };

    static bgfx::VertexLayout sr_layout;
};

struct SpriteInfo
{
    std::vector<SPRITE_VERTEX> vertices;
    float depth;
    std::shared_ptr<TextureResource> texture;
};

static const uint32_t InitialQueueSize = 1024;

class SpriteRenderer
{
    static const uint32_t MaxBatchSize = 2048;

    bgfx::UniformHandle s_texColor;
    //bgfx::UniformHandle u_color;
    bgfx::ProgramHandle m_prog;

    
    long m_width;
    long m_height;

    std::vector<SpriteInfo> m_spriteQueue;

    std::vector<SpriteInfo const *> m_sortedSprites;

    uint32_t m_spriteQueueSize;
    uint32_t m_spriteQueueCount;

    std::shared_ptr<TextureResource> m_backbufferTexture;

  public:
    std::shared_ptr<TextureResource> Texture;

    SpriteRenderer(long m_fbWidth, long m_fbHeight);
    //SpriteRenderer();

    ~SpriteRenderer();
    void SetBackbufferTexture(std::shared_ptr<TextureResource> texture);
    void ReleaseTexture(std::shared_ptr<TextureResource> texture);

    void SetViewProjection();

    void UpdateIndexBuffer(std::vector<uint16_t> indices);
    void UpdateVertexBuffer(std::vector<glm::vec3> &vertices, glm::vec2 &u, glm::vec2 &v, uint32_t &color,
                            float depth = 0.0f);
    void UpdateVertexBuffer(std::vector<glm::vec3> &vertices, std::vector<std::pair<float, float>>& uv, uint32_t &color,
                            float depth = 0.0f);

    void Submit();
};

#endif
