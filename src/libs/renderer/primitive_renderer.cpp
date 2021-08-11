#include "primitive_renderer.h"
#include "brtshaderc.h"

#include "matrix.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <vector>
#include <algorithm>

bgfx::VertexLayout VERTEX_POSITION_COLOR::pc_layout;
bgfx::VertexLayout VERTEX_POSITION_TEXTURE::pt_layout;
bgfx::VertexLayout VERTEX_POSITION_TEXTURE_COLOR::ptc_layout;


PrimitiveRenderer::PrimitiveRenderer(long m_fbWidth, long m_fbHeight)
//SpriteRenderer::SpriteRenderer()
{

    VERTEX_POSITION_COLOR::Init();
    VERTEX_POSITION_TEXTURE::Init();
    VERTEX_POSITION_TEXTURE_COLOR::Init();
    


    m_width = m_fbWidth;
    m_height = m_fbHeight;

    
    const bgfx::Memory *memPrimitiveVsh = shaderc::compileShader(shaderc::ST_VERTEX, "shaders/dx9/vs_primitive.sc");
    const bgfx::Memory *memPrimitiveFsh = shaderc::compileShader(shaderc::ST_FRAGMENT, "shaders/dx9/fs_primitive.sc");

    bgfx::ShaderHandle primitiveVsh = bgfx::createShader(memPrimitiveVsh);
    bgfx::ShaderHandle primitiveFsh = bgfx::createShader(memPrimitiveFsh);

    m_primitiveProg = bgfx::createProgram(primitiveVsh, primitiveFsh, true);

    const bgfx::Memory *memTexturedVsh = shaderc::compileShader(shaderc::ST_VERTEX, "shaders/dx9/vs_textured.sc");
    const bgfx::Memory *memTexturedFsh = shaderc::compileShader(shaderc::ST_FRAGMENT, "shaders/dx9/fs_textured.sc");

    bgfx::ShaderHandle texturedVsh = bgfx::createShader(memTexturedVsh);
    bgfx::ShaderHandle texturedFsh = bgfx::createShader(memTexturedFsh);

    m_texturedProg = bgfx::createProgram(texturedVsh, texturedFsh, true);

    const bgfx::Memory *memTexturedColorVsh = shaderc::compileShader(shaderc::ST_VERTEX, "shaders/dx9/vs_textured_color.sc");
    const bgfx::Memory *memTexturedColorFsh = shaderc::compileShader(shaderc::ST_FRAGMENT, "shaders/dx9/fs_textured_color.sc");

    bgfx::ShaderHandle texturedColorVsh = bgfx::createShader(memTexturedColorVsh);
    bgfx::ShaderHandle texturedColorFsh = bgfx::createShader(memTexturedColorFsh);

    m_texturedColorProg = bgfx::createProgram(texturedColorVsh, texturedColorFsh, true);


    s_texture = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    // u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
}

PrimitiveRenderer::~PrimitiveRenderer()
{
    bgfx::destroy(m_primitiveProg);
    bgfx::destroy(m_texturedProg);
    bgfx::destroy(m_texturedColorProg);
}
 

void PrimitiveRenderer::ReleaseTexture(std::shared_ptr<TextureResource> texture)
{
    bgfx::destroy(*texture->textureHandle);
}

void PrimitiveRenderer::PushVertices(std::vector<VERTEX_POSITION_COLOR> &vertices)
{
    Submit(vertices);
}

void PrimitiveRenderer::PushVertices(std::vector<VERTEX_POSITION_TEXTURE> &vertices)
{
    Submit(vertices);
}

void PrimitiveRenderer::PushVertices(std::vector<VERTEX_POSITION_TEXTURE_COLOR> &vertices)
{
    Submit(vertices);
}

void PrimitiveRenderer::TestSubmit(std::vector<VERTEX_POSITION_TEXTURE_COLOR> &vertices)
{
    const bgfx::Memory *mem = bgfx::alloc(vertices.size() * sizeof(VERTEX_POSITION_TEXTURE_COLOR));

    VERTEX_POSITION_TEXTURE_COLOR *vertex = (VERTEX_POSITION_TEXTURE_COLOR *)mem->data;

    int index = 0;

    vertex[index + 0] = VERTEX_POSITION_TEXTURE_COLOR{vertices[index + 0].x,  vertices[index + 0].y,
                                                      vertices[index + 0].z,  vertices[index + 0].tu,
                                      vertices[index + 0].tv, vertices[index + 0].color}; // top left

    vertex[index + 1] = VERTEX_POSITION_TEXTURE_COLOR{vertices[index + 1].x,  vertices[index + 1].y,
                                                      vertices[index + 0].z,  vertices[index + 1].tu,
                                      vertices[index + 1].tv, vertices[index + 1].color}; // top left

    vertex[index + 2] = VERTEX_POSITION_TEXTURE_COLOR{vertices[index + 2].x,  vertices[index + 2].y,
                                                      vertices[index + 0].z,  vertices[index + 2].tu,
                                      vertices[index + 2].tv, vertices[index + 2].color}; // top left

    vertex[index + 3] = VERTEX_POSITION_TEXTURE_COLOR{vertices[index + 3].x,  vertices[index + 3].y,
                                                      vertices[index + 0].z,  vertices[index + 3].tu,
                                      vertices[index + 3].tv, vertices[index + 3].color}; // top left
    
    std::vector<VERTEX_POSITION_TEXTURE_COLOR> storageVertices = std::vector<VERTEX_POSITION_TEXTURE_COLOR>{
        vertex[index + 0], vertex[index + 1], vertex[index + 2], vertex[index + 3]};

    Submit(storageVertices);
}

void PrimitiveRenderer::Submit(std::vector<glm::vec3> &vertices, std::vector<std::pair<float, float>> &uv,
                               std::vector<uint32_t> &colors)
{
    const bgfx::Memory *mem = bgfx::alloc(vertices.size() * sizeof(VERTEX_POSITION_TEXTURE_COLOR));

    VERTEX_POSITION_TEXTURE_COLOR *vertex = (VERTEX_POSITION_TEXTURE_COLOR *)mem->data;

    int index = 0;

    vertex[index + 0] = VERTEX_POSITION_TEXTURE_COLOR{vertices[index + 0].x, vertices[index + 0].y, 0, uv[index + 0].first,
                                                uv[index + 0].second, colors[index + 0]}; // top left
    vertex[index + 1] = VERTEX_POSITION_TEXTURE_COLOR{vertices[index + 1].x, vertices[index + 1].y, 0, uv[index + 1].first,
                                                uv[index + 1].second, colors[index + 1]}; // top right
    vertex[index + 2] = VERTEX_POSITION_TEXTURE_COLOR{vertices[index + 2].x, vertices[index + 2].y, 0, uv[index + 2].first,
                                                uv[index + 2].second, colors[index + 2]}; // bottom left
    vertex[index + 3] = VERTEX_POSITION_TEXTURE_COLOR{vertices[index + 3].x, vertices[index + 3].y, 0, uv[index + 3].first,
                                                uv[index + 3].second, colors[index + 3]}; // bottom right

    std::vector<VERTEX_POSITION_TEXTURE_COLOR> storageVertices = std::vector<VERTEX_POSITION_TEXTURE_COLOR>{
        vertex[index + 0], vertex[index + 1], vertex[index + 2], vertex[index + 3]};

    Submit(storageVertices);
}



void PrimitiveRenderer::Submit(std::vector<VERTEX_POSITION_COLOR> &vertices)
{

    bgfx::TransientVertexBuffer vb;
    bgfx::allocTransientVertexBuffer(&vb, vertices.size(), VERTEX_POSITION_COLOR::pc_layout);
    VERTEX_POSITION_COLOR *vertex = (VERTEX_POSITION_COLOR *)vb.data;

    for (uint16_t i = 0, v = 0; i < (vertices.size() / 4); i += 4, v += 4)
    {
        vertex[v + 0] = vertices[i + 0];
        vertex[v + 1] = vertices[i + 1];
        vertex[v + 2] = vertices[i + 2];
        vertex[v + 3] = vertices[i + 3];
    }

    const uint16_t indicesCount = (vertices.size() / 4) * 6;

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

    bgfx::allocTransientIndexBuffer(&ib, indicesCount);

    auto ibPtr = reinterpret_cast<uint16_t *>(ib.data);

    for (long i = 0; i < indices.size(); ++i)
    {
        ibPtr[i] = indices[i];
    }

    uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA;

    bgfx::setViewFrameBuffer(1, BGFX_INVALID_HANDLE);

    bgfx::setState(state);

    bgfx::setVertexBuffer(0, &vb);
    bgfx::setIndexBuffer(&ib, 0, (vertices.size() / 4) * 6);
    bgfx::submit(1, m_primitiveProg);
}


void PrimitiveRenderer::Submit(std::vector<VERTEX_POSITION_TEXTURE> &vertices)
{

    bgfx::TransientVertexBuffer vb;
    bgfx::allocTransientVertexBuffer(&vb, vertices.size(), VERTEX_POSITION_TEXTURE::pt_layout);
    VERTEX_POSITION_TEXTURE *vertex = (VERTEX_POSITION_TEXTURE *)vb.data;

    for (uint16_t i = 0, v = 0; i < (vertices.size() / 4); i += 4, v += 4)
    {
        vertex[v + 0] = vertices[i + 0];
        vertex[v + 1] = vertices[i + 1];
        vertex[v + 2] = vertices[i + 2];
        vertex[v + 3] = vertices[i + 3];
    }

    const uint16_t indicesCount = (vertices.size() / 4) * 6;

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

    bgfx::allocTransientIndexBuffer(&ib, indicesCount);

    auto ibPtr = reinterpret_cast<uint16_t *>(ib.data);

    for (long i = 0; i < indices.size(); ++i)
    {
        ibPtr[i] = indices[i];
    }

    uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA;

    bgfx::setViewFrameBuffer(1, BGFX_INVALID_HANDLE);

    bgfx::setState(state);

    bgfx::setTexture(0, s_texture, *Texture->textureHandle);
    bgfx::setVertexBuffer(0, &vb);
    bgfx::setIndexBuffer(&ib, 0, (vertices.size() / 4) * 6);
    bgfx::submit(1, m_texturedProg);
}

void PrimitiveRenderer::Submit(std::vector<VERTEX_POSITION_TEXTURE_COLOR> &vertices)
{

    bgfx::TransientVertexBuffer vb;
    bgfx::allocTransientVertexBuffer(&vb, vertices.size(), VERTEX_POSITION_TEXTURE_COLOR::ptc_layout);
    VERTEX_POSITION_TEXTURE_COLOR *vertex = (VERTEX_POSITION_TEXTURE_COLOR *)vb.data;

    for (uint16_t i = 0, v = 0; i < (vertices.size() / 4); i += 4, v += 4)
    {
        vertex[v + 0] = vertices[i + 0];
        vertex[v + 1] = vertices[i + 1];
        vertex[v + 2] = vertices[i + 2];
        vertex[v + 3] = vertices[i + 3];
    }

    const uint16_t indicesCount = (vertices.size() / 4) * 6;

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

    bgfx::allocTransientIndexBuffer(&ib, indicesCount);

    auto ibPtr = reinterpret_cast<uint16_t *>(ib.data);

    for (long i = 0; i < indices.size(); ++i)
    {
        ibPtr[i] = indices[i];
    }

    uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA;

    bgfx::setViewFrameBuffer(1, BGFX_INVALID_HANDLE);

    bgfx::setState(state);

    bgfx::setTexture(0, s_texture, *Texture->textureHandle);
    bgfx::setVertexBuffer(0, &vb);
    bgfx::setIndexBuffer(&ib, 0, (vertices.size() / 4) * 6);
    bgfx::submit(1, m_texturedColorProg);

}