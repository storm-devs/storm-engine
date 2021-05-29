#include "bgfx_utils.h"

#include "glm.hpp"

#include <vector>
#include <map>


#ifndef PRIMITIVE_RENDERER_DEF
#define PRIMITIVE_RENDERER_DEF


struct PRIMITIVE_SPRITE_VERTEX
{
    float x;
    float y;

    // uint32_t color;
    float tu, tv;

    static void Init()
    {
        pr_layout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            //.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true)
            .end();
    };

    static bgfx::VertexLayout pr_layout;
};

class PrimitiveRenderer
{
    bgfx::UniformHandle s_texColor;
    //bgfx::UniformHandle u_color;
    bgfx::ProgramHandle m_prog;
   
    long m_width;
    long m_height;

  public:
    std::shared_ptr<TextureResource> Texture;

    PrimitiveRenderer(long m_fbWidth, long m_fbHeight);
    ~PrimitiveRenderer();


    void ReleaseTexture(std::shared_ptr<TextureResource> texture);

    void UpdateVertexBuffer(std::vector<glm::vec3> &vertices, glm::vec2 &u, glm::vec2 &v, uint32_t &color,
                            float depth = 0.0f);
    void UpdateVertexBuffer(std::vector<glm::vec3> &vertices, std::vector<std::pair<float, float>>& uv, uint32_t &color,
                            float depth = 0.0f);

    void Submit(std::vector<glm::vec3> &vertices, glm::vec2 &u, glm::vec2 &v, uint32_t &color);
    void Submit(std::vector<glm::vec3> &vertices, std::vector<std::pair<float, float>> &uv, uint32_t &color);
    void Submit(std::vector<PRIMITIVE_SPRITE_VERTEX> &vertices);
};

#endif
