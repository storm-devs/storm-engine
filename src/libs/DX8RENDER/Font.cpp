#include "font.h"

#include <cstdint>
#include <stdio.h>

static char Buffer1024[1024];

namespace
{
/* is c the start of a utf8 sequence? */
#define isutf(c) (((c)&0xC0) != 0x80)

// taken from https://gist.github.com/MightyPork/52eda3e5677b4b03524e40c9f0ab1da5
int CodepointToUtf8(char *out, uint32_t codepoint)
{
    if (codepoint <= 0x7F)
    {
        // Plain ASCII
        out[0] = (char)codepoint;
        out[1] = 0;
        return 1;
    }
    else if (codepoint <= 0x07FF)
    {
        // 2-byte unicode
        out[0] = (char)(((codepoint >> 6) & 0x1F) | 0xC0);
        out[1] = (char)(((codepoint >> 0) & 0x3F) | 0x80);
        out[2] = 0;
        return 2;
    }
    else if (codepoint <= 0xFFFF)
    {
        // 3-byte unicode
        out[0] = (char)(((codepoint >> 12) & 0x0F) | 0xE0);
        out[1] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
        out[2] = (char)(((codepoint >> 0) & 0x3F) | 0x80);
        out[3] = 0;
        return 3;
    }
    else if (codepoint <= 0x10FFFF)
    {
        // 4-byte unicode
        out[0] = (char)(((codepoint >> 18) & 0x07) | 0xF0);
        out[1] = (char)(((codepoint >> 12) & 0x3F) | 0x80);
        out[2] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
        out[3] = (char)(((codepoint >> 0) & 0x3F) | 0x80);
        out[4] = 0;
        return 4;
    }
    else
    {
        // error - use replacement character
        out[0] = (char)0xEF;
        out[1] = (char)0xBF;
        out[2] = (char)0xBD;
        out[3] = 0;
        return 0;
    }
}

// taken from http://www.zedwood.com/article/cpp-utf8-char-to-codepoint
int Utf8ToCodepoint(const char *utf8)
{
    int l = strlen(utf8);

    if (l < 1)
        return -1;
    unsigned char u0 = utf8[0];
    if (u0 >= 0 && u0 <= 0x7F)
        return u0;

    if (l < 2)
        return -1;
    unsigned char u1 = utf8[1];
    if (u0 >= 0xC0 && u0 <= 0xDF)
        return (u0 - 192) * 64 + (u1 - 128);

    if (utf8[0] == 0xed && (utf8[1] & 0xa0) == 0xa0)
        return -1; // code points, 0xd800 to 0xdfff

    if (l < 3)
        return -1;
    unsigned char u2 = utf8[2];
    if (u0 >= 224 && u0 <= 239)
        return (u0 - 224) * 4096 + (u1 - 128) * 64 + (u2 - 128);

    if (l < 4)
        return -1;
    unsigned char u3 = utf8[3];
    if (u0 >= 240 && u0 <= 247)
        return (u0 - 240) * 262144 + (u1 - 128) * 4096 + (u2 - 128) * 64 + (u3 - 128);
    return -1;
}

int Utf8StringLength(const char *s)
{
    long s_num = 0;
    while (*s)
        s_num += (*s++ & 0xC0) != 0x80;

    return s_num;
}

// taken from https://www.cprogramming.com/tutorial/utf8.c
void u8_inc(const char *s, int *i)
{
    bool utf8 = false;
    while (*s && !utf8)
    {
        s++;
        (*i)++;
        utf8 = isutf(*s);
    }
}
} // namespace

FONT::FONT()
{
    api = (VAPI *)_CORE_API;
    RenderService = 0;
    Device = 0;
    VBuffer = 0;
    TextureID = -1;
    Color = oldColor = 0xffffffff;
    ZeroMemory(CharT, sizeof(CharT));
    ZeroMemory(&Pos, sizeof(Pos));
    bInverse = bOldInverse = false;
    techniqueName = NULL;
    textureName = NULL;
}

FONT::~FONT()
{
    if (techniqueName != NULL)
    {
        delete techniqueName;
        techniqueName = NULL;
    }
    if (textureName != NULL)
    {
        delete textureName;
        textureName = NULL;
    }
    if (VBuffer)
        VBuffer->Release();
    if (RenderService)
    {
        if (TextureID >= 0)
            RenderService->TextureRelease(TextureID);
        // api->FreeService("dx8render");
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
        else
        {
            if (pData[index] == ',')
            {
                pData[index] = 0;
                *result = atol(pData);
                *pDataPointer += (index + 1);
                return true;
            }
        }
        index++;
        if (index > 8)
            SE_THROW_MSG(inf loop);
    }
    return false;
}

bool FONT::Init(char *font_name, char *iniName, IDirect3DDevice9 *_device, VDX8RENDER *_render)
{
    INIFILE *ini;
    char key_name[MAX_PATH];
    char buffer[MAX_PATH];
    long codepoint;
    long ltmp;
    char *pData;

    ini = api->fio->OpenIniFile(iniName);
    if (ini == 0)
        return false;

    m_fAspectRatioH = 1.f;
    // m_fAspectRatioV = _render->GetHeightDeformator();
    m_fAspectRatioV = 1.f;

    if (ini->GetLong(font_name, "AspectHeightConstant", 0) == 1)
    {
        m_fAspectRatioH = 1.f / m_fAspectRatioV;
        m_fAspectRatioV = 1.f;
    }

    float _fscale = 1.f;
#ifndef _XBOX
    _fscale = ini->GetFloat(font_name, "pcscale", 1.f);
#else
    _fscale = ini->GetFloat(font_name, "xboxscale", 1.f);
#endif
    if (_fscale != 1.f)
    {
        m_fAspectRatioH *= _fscale;
        m_fAspectRatioV *= _fscale;
    }

    Height = ini->GetLong(font_name, "Height", 0);
    Height = (long)(Height * m_fAspectRatioV);

    if (ini->ReadString(font_name, "Texture", buffer, sizeof(buffer) - 1, ""))
    {
        if ((textureName = NEW char[strlen(buffer) + 1]) == NULL)
            SE_THROW_MSG("allocate memory error")
        strcpy(textureName, buffer);
    }
    if (ini->ReadString(font_name, "Techniques", buffer, sizeof(buffer) - 1, ""))
    {
        if ((techniqueName = NEW char[strlen(buffer) + 1]) == NULL)
            SE_THROW_MSG("allocate memory error")
        strcpy(techniqueName, buffer);
    }
    Texture_XSize = ini->GetLong(font_name, "Texture_xsize", 1);
    Texture_YSize = ini->GetLong(font_name, "Texture_ysize", 1);
    Symbol_interval = ini->GetLong(font_name, "Symbol_interval", 0);

    bShadow = bOldShadow = false;
    if (ini->TestKey(font_name, "Shadow", 0))
        bShadow = bOldShadow = true;

    Shadow_offsetx = ini->GetLong(font_name, "Shadow_offsetx", 2);
    Shadow_offsety = ini->GetLong(font_name, "Shadow_offsety", 2);
    Spacebar = (long)(ini->GetLong(font_name, "Spacebar", 8) * m_fAspectRatioH);

    ini->CaseSensitive(true);
    for (codepoint = 30; codepoint < USED_CODES; codepoint++)
    {
        char utf8[5];
        CodepointToUtf8(utf8, codepoint);
        if (codepoint >= 'a' && codepoint <= 'z')
        {
            wsprintf(key_name, "char_%s_", utf8);
        }
        else if (codepoint == '=')
            wsprintf(key_name, "char_equ");
        else
            wsprintf(key_name, "char_%s", utf8);

        if (!ini->ReadString(font_name, key_name, buffer, sizeof(buffer), ""))
            continue;
        pData = buffer;
        if (!MakeLong(&pData, &ltmp))
            SE_THROW_MSG(invalid font record);
        CharT[codepoint].Pos.x1 = 0;
        CharT[codepoint].Tuv.x1 = (float)(ltmp + .5f) / (float)Texture_XSize;
        if (!MakeLong(&pData, &ltmp))
            SE_THROW_MSG(invalid font record);
        CharT[codepoint].Pos.y1 = 0.f;
        CharT[codepoint].Tuv.y1 = (float)(ltmp + .5f) / (float)Texture_YSize;

        if (!MakeLong(&pData, &ltmp))
            SE_THROW_MSG(invalid font record);
        CharT[codepoint].Pos.x2 = (float)((long)(ltmp * m_fAspectRatioH));
        CharT[codepoint].Tuv.x2 = CharT[codepoint].Tuv.x1 + (float)(ltmp - 1.f) / (float)Texture_XSize;
        if (!MakeLong(&pData, &ltmp))
            SE_THROW_MSG(invalid font record);
        CharT[codepoint].Pos.y1 = (float)(Height - (long)(ltmp * m_fAspectRatioV));
        CharT[codepoint].Pos.y2 = (float)Height; //((long)(ltmp*m_fAspectRatioV));
        CharT[codepoint].Tuv.y2 = CharT[codepoint].Tuv.y1 + (float)(ltmp - 1.f) / (float)Texture_YSize;
    }
    ini->CaseSensitive(false);

    delete ini;

    RenderService = _render;
    Device = _device;

    IMAGE_VERTEX *pVertex;
    Device->CreateVertexBuffer(sizeof(IMAGE_VERTEX) * MAX_SYMBOLS * SYM_VERTEXS, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                               IMAGE_FVF, D3DPOOL_SYSTEMMEM, &VBuffer, NULL);
    if (VBuffer == 0)
        SE_THROW_MSG(vbuffer error);
    VBuffer->Lock(0, sizeof(IMAGE_VERTEX) * MAX_SYMBOLS * SYM_VERTEXS, (void **)&pVertex, 0);
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
        _CORE_API->Trace("Not Found Texture: %s", textureName);
        return false;
    }

    return true;
}

void FONT::Realize(DWORD DeltaTime)
{
}

void FONT::SetColor(DWORD color)
{
    Color = color;
}

long _cdecl FONT::Printf(long x, long y, char *data_PTR, ...)
{
    va_list args;
    va_start(args, data_PTR);
    _vsnprintf(Buffer1024, sizeof(Buffer1024), data_PTR, args);
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
    if (Text == NULL)
        return 0;
    float xoffset = 0;
    long s_num = strlen(Text);

    for (int i = 0; i < s_num; u8_inc(Text + i, &i))
    {
        uint32_t Codepoint = Utf8ToCodepoint(Text + i);
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

    return (long)xoffset;
}

long FONT::UpdateVertexBuffer(long x, long y, char *data_PTR, int utf8length)
{
    long s_num;
    long n;
    float xoffset;
    IMAGE_VERTEX *pVertex;

    s_num = strlen(data_PTR);

    VBuffer->Lock(0, sizeof(IMAGE_VERTEX) * utf8length * SYM_VERTEXS, (void **)&pVertex, 0);

    xoffset = 0;

    for (int i = 0, curLetter = 0; i < s_num; u8_inc(data_PTR + i, &i), curLetter++)
    {
        Assert(curLetter < utf8length);

        int Codepoint = Utf8ToCodepoint(data_PTR + i);
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
        OffsetFRect(pos, (float)x + xoffset, (float)y);
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
    return (long)xoffset;
}

long FONT::Print(long x, long y, char *data_PTR)
{
    if (data_PTR == NULL || techniqueName == NULL)
        return 0;
    long xoffset = 0L;

    long s_num = Utf8StringLength(data_PTR);
    if (s_num == 0)
        return 0;

    bool bDraw = RenderService->TechniqueExecuteStart(techniqueName);
    if (!bDraw)
        return xoffset;

    RenderService->TextureSet(0, TextureID);
    Device->SetVertexShader(NULL);
    Device->SetFVF(IMAGE_FVF);
    Device->SetStreamSource(0, VBuffer, 0, sizeof(IMAGE_VERTEX));
    Device->SetIndices(0);

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
    if (RenderService != NULL)
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
