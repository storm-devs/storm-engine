#include "font.h"
#include "core.h"
#include "storm/config.hpp"
#include "utf8.h"

#include <fmt/format.h>

namespace
{

constexpr size_t MAX_SYMBOLS = 512;
constexpr size_t SYM_VERTEXS = 6;
constexpr size_t USED_CODES = 0x2070; // end of https://unicode-table.com/en/blocks/general-punctuation/

constexpr auto FONT_CHAR_FVF = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2);

struct FONT_CHAR_VERTEX
{
    CVECTOR pos;
    float rhw;
    uint32_t color;
    float tu, tv;
};

bool MakeLong(char **pDataPointer, int32_t *result)
{
    int32_t index;
    char *pData;
    pData = *pDataPointer;
    index = 0;
    while (true)
    {
        if (pData[index] == 0)
        {
            if (index == 0)
                return false;
            *result = atol(pData);
            return true;
        }
        if (pData[index] == ',')
        {
            pData[index] = 0;
            *result = atol(pData);
            *pDataPointer += (index + 1);
            return true;
        }
        index++;
        if (index > 8)
            throw std::runtime_error("inf loop");
    }
    return false;
}

} // namespace

FONT::FONT(VDX9RENDER &renderer, IDirect3DDevice9 &device)
    : charDescriptors_(USED_CODES), renderService_(renderer), device_(device)
{
}

FONT::~FONT()
{
    if (vertexBuffer_)
        vertexBuffer_->Release();
    if (textureHandle_ >= 0)
        renderService_.TextureRelease(textureHandle_);
    // core.FreeService("dx9render");
}

bool FONT::Init(const char *font_name, const char *iniName)
{
    if (initialized_)
    {
        throw std::runtime_error("Font already initialized");
    }
    initialized_ = true;

    char key_name[MAX_PATH];
    char buffer[MAX_PATH];
    int32_t codepoint;
    int32_t ltmp;
    char *pData;

    auto ini = fio->OpenIniFile(iniName);
    if (ini == nullptr)
        return false;

    if (ini->GetInt(font_name, "AspectHeightConstant", 0) == 1)
    {
        horizontalAspectRatio_ = 1.f / verticalAspectRatio_;
        verticalAspectRatio_ = 1.f;
    }

    auto _fscale = 1.f;
    _fscale = ini->GetFloat(font_name, "pcscale", 1.f);
    if (_fscale != 1.f)
    {
        horizontalAspectRatio_ *= _fscale;
        verticalAspectRatio_ *= _fscale;
    }

    height_ = ini->GetInt(font_name, "Height", 0);
    height_ = static_cast<int32_t>(height_ * verticalAspectRatio_);

    if (ini->ReadString(font_name, "Texture", buffer, sizeof(buffer) - 1, ""))
    {
        textureName_ = buffer;
    }
    if (ini->ReadString(font_name, "Techniques", buffer, sizeof(buffer) - 1, ""))
    {
        techniqueName_ = buffer;
    }
    textureSizeX_ = ini->GetInt(font_name, "Texture_xsize", 1);
    textureSizeY_ = ini->GetInt(font_name, "Texture_ysize", 1);
    symbolInterval_ = ini->GetInt(font_name, "Symbol_interval", 0);

    if (ini->TestKey(font_name, "Shadow", nullptr))
        drawShadows_ = true;

    shadowOffsetX_ = ini->GetInt(font_name, "Shadow_offsetx", 2);
    shadowOffsetY_ = ini->GetInt(font_name, "Shadow_offsety", 2);
    spacebarWidth_ = static_cast<int32_t>(ini->GetInt(font_name, "Spacebar", 8) * horizontalAspectRatio_);

    ini->CaseSensitive(true);
    for (codepoint = 30; codepoint < USED_CODES; codepoint++)
    {
        char utf8[5];
        utf8::CodepointToUtf8(utf8, codepoint);
        if (codepoint >= 'a' && codepoint <= 'z')
        {
            sprintf_s(key_name, "char_%s_", utf8);
        }
        else if (codepoint == '=')
            sprintf_s(key_name, "char_equ");
        else
            sprintf_s(key_name, "char_%s", utf8);
        if (!ini->ReadString(font_name, key_name, buffer, sizeof(buffer), ""))
            continue;
        pData = buffer;
        if (!MakeLong(&pData, &ltmp))
            throw std::runtime_error("invalid font record");
        charDescriptors_[codepoint].Pos.x1 = 0;
        charDescriptors_[codepoint].Tuv.x1 = static_cast<float>(ltmp + .5f) / static_cast<float>(textureSizeX_);
        if (!MakeLong(&pData, &ltmp))
            throw std::runtime_error("invalid font record");
        charDescriptors_[codepoint].Pos.y1 = 0.f;
        charDescriptors_[codepoint].Tuv.y1 = static_cast<float>(ltmp + .5f) / static_cast<float>(textureSizeY_);

        if (!MakeLong(&pData, &ltmp))
            throw std::runtime_error("invalid font record");
        charDescriptors_[codepoint].Pos.x2 = static_cast<float>(static_cast<int32_t>(ltmp * horizontalAspectRatio_));
        charDescriptors_[codepoint].Tuv.x2 =
            charDescriptors_[codepoint].Tuv.x1 + static_cast<float>(ltmp - 1.f) / static_cast<float>(textureSizeX_);
        if (!MakeLong(&pData, &ltmp))
            throw std::runtime_error("invalid font record");
        charDescriptors_[codepoint].Pos.y1 =
            static_cast<float>(height_ - static_cast<int32_t>(ltmp * verticalAspectRatio_));
        charDescriptors_[codepoint].Pos.y2 = static_cast<float>(height_); //((int32_t)(ltmp*m_fAspectRatioV));
        charDescriptors_[codepoint].Tuv.y2 =
            charDescriptors_[codepoint].Tuv.y1 + static_cast<float>(ltmp - 1.f) / static_cast<float>(textureSizeY_);
    }
    ini->CaseSensitive(false);

    FONT_CHAR_VERTEX *pVertex;
    device_.CreateVertexBuffer(sizeof(FONT_CHAR_VERTEX) * MAX_SYMBOLS * SYM_VERTEXS,
                               D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, FONT_CHAR_FVF, D3DPOOL_SYSTEMMEM, &vertexBuffer_,
                               nullptr);
    if (vertexBuffer_ == nullptr)
        throw std::runtime_error("vbuffer error");
    vertexBuffer_->Lock(0, sizeof(FONT_CHAR_VERTEX) * MAX_SYMBOLS * SYM_VERTEXS, (void **)&pVertex, 0);
    for (codepoint = 0; codepoint < MAX_SYMBOLS * SYM_VERTEXS; codepoint++)
    {
        pVertex[codepoint].pos.z = 0.5f;
    }
    vertexBuffer_->Unlock();

    textureHandle_ = renderService_.TextureCreate(textureName_.c_str());
    if (textureHandle_ < 0)
    {
        core.Trace("Not Found Texture: %s", textureName_.c_str());
        return false;
    }

    return true;
}

void OffsetFRect(FLOAT_RECT &fr, float dx, float dy)
{
    fr.x1 += dx;
    fr.x2 += dx;
    fr.y1 += dy;
    fr.y2 += dy;
}

int32_t FONT::GetStringWidth(const std::string_view &text, std::optional<float> scale_override) const
{
    if (text.empty())
        return 0;
    float xoffset = 0;
    const int32_t s_num = text.size();

    const float scale = scale_override.value_or(scale_);

    for (int32_t i = 0; i < s_num; i += utf8::u8_inc(text.data() + i))
    {
        uint32_t Codepoint = utf8::Utf8ToCodepoint(text.data() + i);

        if (Codepoint > USED_CODES) {
            core.Trace("Invalid codepoint: %d", Codepoint);
            if constexpr(storm::kIsDebug) {
                throw std::runtime_error(fmt::format("Invalid codepoint: {}", Codepoint));
            }
            continue;
        }

        FLOAT_RECT pos = charDescriptors_[Codepoint].Pos;

        if (scale != 1.f)
        {
            pos.x1 *= scale;
            pos.x2 *= scale;
        }
        xoffset += pos.x2 - pos.x1 + symbolInterval_ * scale;
        if (Codepoint == ' ')
        {
            xoffset += spacebarWidth_ * scale;
        }
    }

    return static_cast<int32_t>(xoffset);
}

int32_t FONT::UpdateVertexBuffer(int32_t x, int32_t y, char *data_PTR, int utf8length, float scale, uint32_t color)
{
    int32_t s_num;
    int32_t n;
    float xoffset;
    FONT_CHAR_VERTEX *pVertex;

    s_num = strlen(data_PTR);

    vertexBuffer_->Lock(0, sizeof(FONT_CHAR_VERTEX) * utf8length * SYM_VERTEXS, (void **)&pVertex, 0);

    xoffset = 0;

    for (int i = 0, curLetter = 0; i < s_num; i += utf8::u8_inc(data_PTR + i), curLetter++)
    {
        Assert(curLetter < utf8length);

        int Codepoint = utf8::Utf8ToCodepoint(data_PTR + i);

        if (Codepoint > USED_CODES) {
            core.Trace("Invalid codepoint: %d", Codepoint);
            if constexpr(storm::kIsDebug) {
                throw std::runtime_error(fmt::format("Invalid codepoint: {}", Codepoint));
            }
            continue;
        }

        n = curLetter * 6;
        FLOAT_RECT pos = charDescriptors_[Codepoint].Pos;
        if (scale != 1.f)
        {
            pos.x1 *= scale;
            pos.x2 *= scale;
            pos.y1 *= scale;
            pos.y2 *= scale;
        }
        OffsetFRect(pos, static_cast<float>(x) + xoffset, static_cast<float>(y));
        xoffset += pos.x2 - pos.x1 + symbolInterval_ * scale;

        if (Codepoint == ' ')
        {
            pos.x1 = pos.x2 = pos.y1 = pos.y2 = 0;
            xoffset += spacebarWidth_ * scale;
        }

        pVertex[n + 0].pos.x = pos.x1;
        pVertex[n + 1].pos.x = pos.x1;
        pVertex[n + 2].pos.x = pos.x2;

        pVertex[n + 3].pos.x = pos.x1;
        pVertex[n + 4].pos.x = pos.x2;
        pVertex[n + 5].pos.x = pos.x2;

        pVertex[n + 0].pos.y = pos.y1;
        pVertex[n + 1].pos.y = pos.y2;
        pVertex[n + 2].pos.y = pos.y1;

        pVertex[n + 3].pos.y = pos.y2;
        pVertex[n + 4].pos.y = pos.y2;
        pVertex[n + 5].pos.y = pos.y1;

        FLOAT_RECT tuv = charDescriptors_[Codepoint].Tuv;

        pVertex[n + 0].tu = tuv.x1;
        pVertex[n + 1].tu = tuv.x1;
        pVertex[n + 2].tu = tuv.x2;

        pVertex[n + 3].tu = tuv.x1;
        pVertex[n + 4].tu = tuv.x2;
        pVertex[n + 5].tu = tuv.x2;

        pVertex[n + 0].tv = tuv.y1;
        pVertex[n + 1].tv = tuv.y2;
        pVertex[n + 2].tv = tuv.y1;

        pVertex[n + 3].tv = tuv.y2;
        pVertex[n + 4].tv = tuv.y2;
        pVertex[n + 5].tv = tuv.y1;

        pVertex[n + 0].color = pVertex[n + 1].color = pVertex[n + 2].color = pVertex[n + 3].color =
            pVertex[n + 4].color = pVertex[n + 5].color = color;

        pVertex[n + 0].rhw = pVertex[n + 1].rhw = pVertex[n + 2].rhw = pVertex[n + 3].rhw = pVertex[n + 4].rhw =
            pVertex[n + 5].rhw = scale;
    }
    vertexBuffer_->Unlock();
    return static_cast<int32_t>(xoffset);
}

int32_t FONT::Print(int32_t x, int32_t y, char *data_PTR, const FONT_PRINT_OVERRIDES &overrides)
{
    if (data_PTR == nullptr || techniqueName_.empty())
        return 0;
    auto xoffset = 0L;
    int32_t s_num = utf8::Utf8StringLength(data_PTR);
    if (s_num == 0)
        return 0;

    const auto bDraw = renderService_.TechniqueExecuteStart(techniqueName_.c_str());
    if (!bDraw)
        return xoffset;

    renderService_.TextureSet(0, textureHandle_);
    device_.SetFVF(FONT_CHAR_FVF);
    device_.SetStreamSource(0, vertexBuffer_, 0, sizeof(FONT_CHAR_VERTEX));
    // Device->SetIndices(0);

    const bool drawShadows = overrides.shadow.value_or(drawShadows_);
    const float scale = overrides.scale.value_or(scale_);
    const uint32_t color = overrides.color.value_or(color_);
    if (drawShadows)
    {
        UpdateVertexBuffer(x + shadowOffsetX_, y + shadowOffsetY_, data_PTR, s_num, scale, color);

        device_.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
        device_.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        device_.DrawPrimitive(D3DPT_TRIANGLELIST, 0, s_num * 2);
    }
    xoffset = UpdateVertexBuffer(x, y, data_PTR, s_num, scale, color);
    device_.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device_.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    device_.DrawPrimitive(D3DPT_TRIANGLELIST, 0, s_num * 2);
    while (renderService_.TechniqueExecuteNext())
        ;

    return xoffset;
}

void FONT::TempUnload()
{
    if (textureHandle_ != -1L)
        renderService_.TextureRelease(textureHandle_);
    textureHandle_ = -1L;
}

void FONT::RepeatInit()
{
    if (textureHandle_ == -1L)
        textureHandle_ = renderService_.TextureCreate(textureName_.c_str());
}
