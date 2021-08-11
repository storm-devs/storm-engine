#ifndef PRIMITIVE_RENDERER_DEF
#define PRIMITIVE_RENDERER_DEF

#include "bgfx_utils.h"

#include "glm.hpp"

#include <vector>
#include <map>

struct VERTEX_POSITION_COLOR
{
    float x;
    float y;
    float z;

    uint32_t color;

    static void Init()
    {
        pc_layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    };

    static bgfx::VertexLayout pc_layout;
};

struct VERTEX_POSITION_TEXTURE
{
    float x;
    float y;
    float z;

    float tu, tv;

    static void Init()
    {
        pt_layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true)
            .end();
    };

    static bgfx::VertexLayout pt_layout;
};

struct VERTEX_POSITION_TEXTURE_COLOR
{
    float x;
    float y;
    float z;

    float tu, tv;
    uint32_t color;

    VERTEX_POSITION_TEXTURE_COLOR(float x, float y, float z, float tu, float tv, uint32_t color)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->tu = tu;
        this->tv = tv;
        this->color = color;
    }

    VERTEX_POSITION_TEXTURE_COLOR(const VERTEX_POSITION_TEXTURE_COLOR &other) 
    {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;

        this->tu = other.tu;
        this->tv = other.tv;
        this->color = other.color;
    } 

    static void Init()
    {
        ptc_layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    };

    static bgfx::VertexLayout ptc_layout;
};



class PrimitiveRenderer
{
    bgfx::UniformHandle s_texture;
    bgfx::ProgramHandle m_primitiveProg;
    bgfx::ProgramHandle m_texturedProg;
    bgfx::ProgramHandle m_texturedColorProg;
    
    long m_width;
    long m_height;

  public:
    std::shared_ptr<TextureResource> Texture;

    PrimitiveRenderer(long m_fbWidth, long m_fbHeight);
    ~PrimitiveRenderer();

    void PushVertices(std::vector<VERTEX_POSITION_COLOR> &vertices);
    void PushVertices(std::vector<VERTEX_POSITION_TEXTURE> &vertices);
    void PushVertices(std::vector<VERTEX_POSITION_TEXTURE_COLOR> &vertices);

    void TestSubmit(std::vector<VERTEX_POSITION_TEXTURE_COLOR> &vertices);
    void Submit(std::vector<glm::vec3> &vertices, std::vector<std::pair<float, float>> &uv,
                std::vector<uint32_t> &colors);

  private:
    void ReleaseTexture(std::shared_ptr<TextureResource> texture);
    void Submit(std::vector<VERTEX_POSITION_COLOR> &vertices);
    void Submit(std::vector<VERTEX_POSITION_TEXTURE> &vertices);
    void Submit(std::vector<VERTEX_POSITION_TEXTURE_COLOR> &vertices);
};

#endif
