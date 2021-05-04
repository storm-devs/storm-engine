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

    float tu, tv;
    uint32_t color;

    static void Init()
    {
        pr_layout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
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

    void Submit(std::vector<glm::vec3> &vertices, glm::vec2 &u, glm::vec2 &v, std::vector<uint32_t> &colors);
    void Submit(std::vector<glm::vec3> &vertices, std::vector<std::pair<float, float>> &uv, std::vector<uint32_t> &colors);
    void Submit(std::vector<PRIMITIVE_SPRITE_VERTEX> &vertices);
};

#endif
