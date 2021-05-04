#include "primitive_renderer.h"
#include "brtshaderc.h"

#include "matrix.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <vector>
#include <algorithm>

bgfx::VertexLayout PRIMITIVE_SPRITE_VERTEX::pr_layout;


PrimitiveRenderer::PrimitiveRenderer(long m_fbWidth, long m_fbHeight)
//SpriteRenderer::SpriteRenderer()
{
    PRIMITIVE_SPRITE_VERTEX::Init();

    m_width = m_fbWidth;
    m_height = m_fbHeight;

    
    const bgfx::Memory *memVsh = shaderc::compileShader(shaderc::ST_VERTEX, "shaders/dx9/vs_primitive.sc");
    const bgfx::Memory *memFsh = shaderc::compileShader(shaderc::ST_FRAGMENT, "shaders/dx9/fs_primitive.sc");

    bgfx::ShaderHandle vsh = bgfx::createShader(memVsh);
    bgfx::ShaderHandle fsh = bgfx::createShader(memFsh);

    m_prog = bgfx::createProgram(vsh, fsh, true);

    s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    // u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
}

PrimitiveRenderer::~PrimitiveRenderer()
{
    bgfx::destroy(m_prog);
}
 

void PrimitiveRenderer::ReleaseTexture(std::shared_ptr<TextureResource> texture)
{
    bgfx::destroy(*texture->textureHandle);
}

void PrimitiveRenderer::Submit(std::vector<glm::vec3> &vertices, 
                                glm::vec2 &u, 
                                glm::vec2 &v, 
                                std::vector<uint32_t> &colors)
{
    // m_color[0] = m_color[1] = m_color[2] = m_color[3] = 1.0f;

    // bgfx::setUniform(u_color, &m_color);
    const bgfx::Memory *mem = bgfx::alloc(vertices.size() * sizeof(PRIMITIVE_SPRITE_VERTEX));

    PRIMITIVE_SPRITE_VERTEX *vertex = (PRIMITIVE_SPRITE_VERTEX *)mem->data;

    int index = 0;

    vertex[index + 0] =
        PRIMITIVE_SPRITE_VERTEX{vertices[index + 0].x, vertices[index + 0].y, u.x, v.x, colors[index + 0]}; // top left
    vertex[index + 1] =
        PRIMITIVE_SPRITE_VERTEX{vertices[index + 1].x, vertices[index + 1].y, u.y, v.x, colors[index + 1]}; // top right
    vertex[index + 2] =
        PRIMITIVE_SPRITE_VERTEX{vertices[index + 2].x, vertices[index + 2].y, u.x, v.y, colors[index + 2]}; // bottom left
    vertex[index + 3] =
        PRIMITIVE_SPRITE_VERTEX{vertices[index + 3].x, vertices[index + 3].y, u.y, v.y, colors[index + 3]}; // bottom right

    std::vector<PRIMITIVE_SPRITE_VERTEX> storageVertices = std::vector<PRIMITIVE_SPRITE_VERTEX>{
        vertex[index + 0], vertex[index + 1], vertex[index + 2], vertex[index + 3]};

    Submit(storageVertices);
}


void PrimitiveRenderer::Submit(std::vector<glm::vec3> &vertices, 
                               std::vector<std::pair<float, float>>& uv, 
                               std::vector<uint32_t> &colors)
{
    const bgfx::Memory *mem = bgfx::alloc(vertices.size() * sizeof(PRIMITIVE_SPRITE_VERTEX));

    PRIMITIVE_SPRITE_VERTEX *vertex = (PRIMITIVE_SPRITE_VERTEX *)mem->data;

    int index = 0;

    vertex[index + 0] = PRIMITIVE_SPRITE_VERTEX{vertices[index + 0].x, vertices[index + 0].y, uv[index + 0].first,
                                                uv[index + 0].second, colors[index + 0]}; // top left
    vertex[index + 1] = PRIMITIVE_SPRITE_VERTEX{vertices[index + 1].x, vertices[index + 1].y, uv[index + 1].first,
                                                uv[index + 1].second, colors[index + 1]}; // top right
    vertex[index + 2] = PRIMITIVE_SPRITE_VERTEX{vertices[index + 2].x, vertices[index + 2].y, uv[index + 2].first,
                                                uv[index + 2].second, colors[index + 2]}; // bottom left
    vertex[index + 3] = PRIMITIVE_SPRITE_VERTEX{vertices[index + 3].x, vertices[index + 3].y, uv[index + 3].first,
                                                uv[index + 3].second, colors[index + 3]}; // bottom right

    std::vector<PRIMITIVE_SPRITE_VERTEX> storageVertices = std::vector<PRIMITIVE_SPRITE_VERTEX>{
        vertex[index + 0], vertex[index + 1], vertex[index + 2], vertex[index + 3]};

    
    Submit(storageVertices);    
}


void PrimitiveRenderer::Submit(std::vector<PRIMITIVE_SPRITE_VERTEX> &vertices)
{

    bgfx::TransientVertexBuffer vb;
    bgfx::allocTransientVertexBuffer(&vb, vertices.size(), PRIMITIVE_SPRITE_VERTEX::pr_layout);
    PRIMITIVE_SPRITE_VERTEX *vertex = (PRIMITIVE_SPRITE_VERTEX *)vb.data;

    for (uint32_t i = 0, v = 0; i < (vertices.size() / 4); i += 4, v += 4)
    {
        vertex[v + 0] = vertices[i + 0];
        vertex[v + 1] = vertices[i + 1];
        vertex[v + 2] = vertices[i + 2];
        vertex[v + 3] = vertices[i + 3];
    }

    const uint32_t indicesCount = (vertices.size() / 4) * 6;

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

    bgfx::TransientIndexBuffer ib;

    bgfx::allocTransientIndexBuffer(&ib, indicesCount);

    auto ibPtr = reinterpret_cast<uint16_t *>(ib.data);

    memcpy(ibPtr, indices.data(), indices.size());

    uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA | BGFX_STATE_BLEND_ALPHA;

    bgfx::setState(state);

    bgfx::setTexture(0, s_texColor, *Texture->textureHandle);
    bgfx::setVertexBuffer(0, &vb);
    bgfx::setIndexBuffer(&ib, 0, (vertices.size() / 4) * 6);
    bgfx::submit(0, m_prog);

}