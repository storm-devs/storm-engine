#include "Font.h"
#include "core.h"
#include "defines.h"
#include "utf8.h"

static char Buffer1024[1024];

FONT::FONT()
{
    RenderService = nullptr;
    Device = nullptr;
    VBuffer = nullptr;
    TextureID = -1;
    Color = oldColor = 0xffffffff;
    PZERO(CharT, sizeof(CharT));
    PZERO(&Pos, sizeof(Pos));
    bInverse = bOldInverse = false;
    techniqueName = nullptr;
    textureName = nullptr;
}

FONT::~FONT()
{
    if (techniqueName != nullptr)
    {
        delete techniqueName;
        techniqueName = nullptr;
    }
    if (textureName != nullptr)
    {
        delete textureName;
        textureName = nullptr;
    }
    if (VBuffer)
        VBuffer->Release();
    if (RenderService)
    {
        if (TextureID >= 0)
            RenderService->TextureRelease(TextureID);
        // core.FreeService("dx9render");
    }
}

void FONT::Inverse(bool v)
{
    bInverse = v;
}

bool FONT::MakeLong(char **pDataPointer, long *result)
{
    long index;
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

bool FONT::Init(const char *font_name, const char *iniName, IDirect3DDevice9 *_device, VDX9RENDER *_render)
{
    char key_name[MAX_PATH];
    char buffer[MAX_PATH];
    long codepoint;
    long ltmp;
    char *pData;

    auto ini = fio->OpenIniFile(iniName);
    if (ini == nullptr)
        return false;

    m_fAspectRatioH = 1.f;
    // m_fAspectRatioV = _render->GetHeightDeformator();
    m_fAspectRatioV = 1.f; // ugeen - for TEHO so that fonts are not crushed
    if (ini->GetLong(font_name, "AspectHeightConstant", 0) == 1)
    {
        m_fAspectRatioH = 1.f / m_fAspectRatioV;
        m_fAspectRatioV = 1.f;
    }

    auto _fscale = 1.f;
    _fscale = ini->GetFloat(font_name, "pcscale", 1.f);
    if (_fscale != 1.f)
    {
        m_fAspectRatioH *= _fscale;
        m_fAspectRatioV *= _fscale;
    }

    Height = ini->GetLong(font_name, "Height", 0);
    Height = static_cast<long>(Height * m_fAspectRatioV);

    if (ini->ReadString(font_name, "Texture", buffer, sizeof(buffer) - 1, ""))
    {
        const auto len = strlen(buffer) + 1;
        if ((textureName = new char[len]) == nullptr)
            throw std::runtime_error("allocate memory error");
        strcpy_s(textureName, len, buffer);
    }
    if (ini->ReadString(font_name, "Techniques", buffer, sizeof(buffer) - 1, ""))
    {
        const auto len = strlen(buffer) + 1;
        if ((techniqueName = new char[len]) == nullptr)
            throw std::runtime_error("allocate memory error");
        strcpy_s(techniqueName, len, buffer);
    }
    Texture_XSize = ini->GetLong(font_name, "Texture_xsize", 1);
    Texture_YSize = ini->GetLong(font_name, "Texture_ysize", 1);
    Symbol_interval = ini->GetLong(font_name, "Symbol_interval", 0);

    bShadow = bOldShadow = false;
    if (ini->TestKey(font_name, "Shadow", nullptr))
        bShadow = bOldShadow = true;

    Shadow_offsetx = ini->GetLong(font_name, "Shadow_offsetx", 2);
    Shadow_offsety = ini->GetLong(font_name, "Shadow_offsety", 2);
    Spacebar = static_cast<long>(ini->GetLong(font_name, "Spacebar", 8) * m_fAspectRatioH);

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
        CharT[codepoint].Pos.x1 = 0;
        CharT[codepoint].Tuv.x1 = static_cast<float>(ltmp + .5f) / static_cast<float>(Texture_XSize);
        if (!MakeLong(&pData, &ltmp))
            throw std::runtime_error("invalid font record");
        CharT[codepoint].Pos.y1 = 0.f;
        CharT[codepoint].Tuv.y1 = static_cast<float>(ltmp + .5f) / static_cast<float>(Texture_YSize);

        if (!MakeLong(&pData, &ltmp))
            throw std::runtime_error("invalid font record");
        CharT[codepoint].Pos.x2 = static_cast<float>(static_cast<long>(ltmp * m_fAspectRatioH));
        CharT[codepoint].Tuv.x2 =
            CharT[codepoint].Tuv.x1 + static_cast<float>(ltmp - 1.f) / static_cast<float>(Texture_XSize);
        if (!MakeLong(&pData, &ltmp))
            throw std::runtime_error("invalid font record");
        CharT[codepoint].Pos.y1 = static_cast<float>(Height - static_cast<long>(ltmp * m_fAspectRatioV));
        CharT[codepoint].Pos.y2 = static_cast<float>(Height); //((long)(ltmp*m_fAspectRatioV));
        CharT[codepoint].Tuv.y2 =
            CharT[codepoint].Tuv.y1 + static_cast<float>(ltmp - 1.f) / static_cast<float>(Texture_YSize);
    }
    ini->CaseSensitive(false);

    RenderService = _render;
    Device = _device;

    IMAGE_VERTEX *pVertex;
    Device->CreateVertexBuffer(sizeof(IMAGE_VERTEX) * MAX_SYMBOLS * SYM_VERTEXS, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                               IMAGE_FVF, D3DPOOL_SYSTEMMEM, &VBuffer, nullptr);
    if (VBuffer == nullptr)
        throw std::runtime_error("vbuffer error");
    VBuffer->Lock(0, sizeof(IMAGE_VERTEX) * MAX_SYMBOLS * SYM_VERTEXS, (VOID **)&pVertex, 0);
    for (codepoint = 0; codepoint < MAX_SYMBOLS * SYM_VERTEXS; codepoint++)
    {
        pVertex[codepoint].pos.z = 0.5f;
    }
    VBuffer->Unlock();

    fScale = fOldScale = 1.f;
    Color = oldColor = 0xFFFFFFFF;

    TextureID = RenderService->TextureCreate(textureName);
    if (TextureID < 0)
    {
        core.Trace("Not Found Texture: %s", textureName);
        return false;
    }

    return true;
}

void FONT::Realize(uint32_t DeltaTime)
{
}

void FONT::SetColor(uint32_t color)
{
    Color = color;
}

long FONT::Printf(long x, long y, char *data_PTR, ...)
{
    va_list args;
    va_start(args, data_PTR);
    _vsnprintf_s(Buffer1024, sizeof(Buffer1024), data_PTR, args);
    va_end(args);
    return Print(x, y, Buffer1024);
}

void OffsetFRect(FLOAT_RECT &fr, float dx, float dy)
{
    fr.x1 += dx;
    fr.x2 += dx;
    fr.y1 += dy;
    fr.y2 += dy;
}

long FONT::GetStringWidth(const char *Text)
{
    if (Text == nullptr)
        return 0;
    float xoffset = 0;
    const long s_num = strlen(Text);
    //  core.Trace("%s", Text);

    for (long i = 0; i < s_num; i += utf8::u8_inc(Text + i))
    {
        uint32_t Codepoint = utf8::Utf8ToCodepoint(Text + i);
        Assert(Codepoint < USED_CODES);

        FLOAT_RECT pos = CharT[Codepoint].Pos;

        if (fScale != 1.f)
        {
            pos.x1 *= fScale;
            pos.x2 *= fScale;
        }
        xoffset += pos.x2 - pos.x1 + Symbol_interval * fScale;
        if (Codepoint == ' ')
        {
            xoffset += Spacebar * fScale;
        }
    }

    return static_cast<long>(xoffset);
}

long FONT::UpdateVertexBuffer(long x, long y, char *data_PTR, int utf8length)
{
    long s_num;
    long n;
    float xoffset;
    uint8_t sym;
    IMAGE_VERTEX *pVertex;

    s_num = strlen(data_PTR);

    VBuffer->Lock(0, sizeof(IMAGE_VERTEX) * utf8length * SYM_VERTEXS, (VOID **)&pVertex, 0);

    xoffset = 0;

    for (int i = 0, curLetter = 0; i < s_num; i += utf8::u8_inc(data_PTR + i), curLetter++)
    {
        Assert(curLetter < utf8length);

        int Codepoint = utf8::Utf8ToCodepoint(data_PTR + i);
        Assert(Codepoint < USED_CODES);

        n = curLetter * 6;
        FLOAT_RECT pos = CharT[Codepoint].Pos;
        if (fScale != 1.f)
        {
            pos.x1 *= fScale;
            pos.x2 *= fScale;
            pos.y1 *= fScale;
            pos.y2 *= fScale;
        }
        OffsetFRect(pos, static_cast<float>(x) + xoffset, static_cast<float>(y));
        xoffset += pos.x2 - pos.x1 + Symbol_interval * fScale;

        if (Codepoint == ' ')
        {
            pos.x1 = pos.x2 = pos.y1 = pos.y2 = 0;
            xoffset += Spacebar * fScale;
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

        FLOAT_RECT tuv = CharT[Codepoint].Tuv;

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
            pVertex[n + 4].color = pVertex[n + 5].color = Color;

        pVertex[n + 0].rhw = pVertex[n + 1].rhw = pVertex[n + 2].rhw = pVertex[n + 3].rhw = pVertex[n + 4].rhw =
            pVertex[n + 5].rhw = fScale;
    }
    VBuffer->Unlock();
    return static_cast<long>(xoffset);
}

long FONT::Print(long x, long y, char *data_PTR)
{
    if (data_PTR == nullptr || techniqueName == nullptr)
        return 0;
    auto xoffset = 0L;
    long s_num = utf8::Utf8StringLength(data_PTR);
    if (s_num == 0)
        return 0;

    const auto bDraw = RenderService->TechniqueExecuteStart(techniqueName);
    if (!bDraw)
        return xoffset;

    RenderService->TextureSet(0, TextureID);
    Device->SetFVF(IMAGE_FVF);
    Device->SetStreamSource(0, VBuffer, 0, sizeof(IMAGE_VERTEX));
    // Device->SetIndices(0);

    if (bInverse)
    {
        if (bShadow)
        {
            UpdateVertexBuffer(x + Shadow_offsetx, y + Shadow_offsety, data_PTR, s_num);

            Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

            Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, s_num * 2);
        }

        xoffset = UpdateVertexBuffer(x, y, data_PTR, s_num);

        Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
        Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, s_num * 2);
    }
    else
    {
        if (bShadow)
        {
            UpdateVertexBuffer(x + Shadow_offsetx, y + Shadow_offsety, data_PTR, s_num);

            Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
            Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

            Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, s_num * 2);
        }

        xoffset = UpdateVertexBuffer(x, y, data_PTR, s_num);

        Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, s_num * 2);
    }
    while (RenderService->TechniqueExecuteNext())
        ;

    return xoffset;
}

void FONT::SetShadowOffset(long ox, long oy)
{
    Shadow_offsetx = ox;
    Shadow_offsety = oy;
}

void FONT::GetShadowOffset(long &ox, long &oy)
{
    ox = Shadow_offsetx;
    oy = Shadow_offsety;
}

void FONT::SetShadow(bool s)
{
    bShadow = s;
}

void FONT::SetScale(float scale)
{
    fScale = scale;
}

void FONT::TempUnload()
{
    if (RenderService != nullptr)
    {
        if (TextureID != -1L)
            RenderService->TextureRelease(TextureID);
        TextureID = -1L;
    }
}

void FONT::RepeatInit()
{
    if (TextureID == -1L)
        TextureID = RenderService->TextureCreate(textureName);
}
