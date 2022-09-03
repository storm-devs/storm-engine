#include "info_handler.h"

#include "core.h"

#include "vma.hpp"

InfoHandler::InfoHandler() : m_rs(nullptr), tex(nullptr)
{
}

InfoHandler::~InfoHandler()
{
    if (tex)
        m_rs->Release(tex);
}

bool InfoHandler::Init()
{
    // get render service
    m_rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    if (!m_rs)
    {
        core.Trace("No service: dx9render");
        return false;
    }
    if (m_rs->IsInsideScene())
    {
        m_rs->MakePostProcess();
    }

    if(!DoPreOut())
    {
        core.Trace("DoPreOut failed");
        return false;
    }

    if (!m_rs->GetRenderTargetAsTexture(&tex))
    {
        core.Trace("[InfoHandler] GetRenderTargetAsTexture failed");
        return false;
    }

    D3DVIEWPORT9 vp;
    m_rs->GetViewport(&vp);
    const auto w = static_cast<float>(vp.Width);
    const auto h = static_cast<float>(vp.Height);
    if (w <= 0 || h <= 0)
        return false;

    drawbuf_base[0].x = 0.0f;
    drawbuf_base[0].y = 0.0f;
    drawbuf_base[0].z = 0.5f;
    drawbuf_base[0].rhw = 1.0f;
    drawbuf_base[0].u = 0.0f;
    drawbuf_base[0].v = 0.0f;
    drawbuf_base[1].x = w;
    drawbuf_base[1].y = 0.0f;
    drawbuf_base[1].z = 0.5f;
    drawbuf_base[1].rhw = 1.0f;
    drawbuf_base[1].u = 1.0f;
    drawbuf_base[1].v = 0.0f;
    drawbuf_base[2].x = 0.0f;
    drawbuf_base[2].y = h;
    drawbuf_base[2].z = 0.5f;
    drawbuf_base[2].rhw = 1.0f;
    drawbuf_base[2].u = 0.0f;
    drawbuf_base[2].v = 1.0f;
    drawbuf_base[3].x = 0.0f;
    drawbuf_base[3].y = h;
    drawbuf_base[3].z = 0.5f;
    drawbuf_base[3].rhw = 1.0f;
    drawbuf_base[3].u = 0.0f;
    drawbuf_base[3].v = 1.0f;
    drawbuf_base[4].x = w;
    drawbuf_base[4].y = 0.0f;
    drawbuf_base[4].z = 0.5f;
    drawbuf_base[4].rhw = 1.0f;
    drawbuf_base[4].u = 1.0f;
    drawbuf_base[4].v = 0.0f;
    drawbuf_base[5].x = w;
    drawbuf_base[5].y = h;
    drawbuf_base[5].z = 0.5f;
    drawbuf_base[5].rhw = 1.0f;
    drawbuf_base[5].u = 1.0f;
    drawbuf_base[5].v = 1.0f;

    return true;
}

void InfoHandler::Execute(uint32_t delta_time)
{
}

void InfoHandler::Realize(uint32_t delta_time) const
{
    if (tex == nullptr)
        return;
    m_rs->MakePostProcess();
    // keep the screen constant
    m_rs->SetTexture(0, tex);
    m_rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2, 2, drawbuf_base,
                        sizeof(drawbuf_base[0]), "Fader");
}

uint64_t InfoHandler::ProcessMessage(MESSAGE &message)
{
    const auto nMsgCode = message.Long();
    switch (nMsgCode)
    {
    case 1:
        Realize(0);
        break;
    }
    return 0;
}

bool InfoHandler::DoPreOut()
{
    if (AttributesPointer == nullptr)
        return false;

    auto isOK = false;
    uint32_t dwBCol, dwFCol;
    const char *inStrStart;
    char outStr[1048];
    D3DSURFACE_DESC desc;
    float fScale;
    int nOutOffset, nOutWidth, nBorderWidth, nInsideRectWidth, nInsideRectHeight;
    int nRowQ;

    inStrStart = AttributesPointer->GetAttribute("infoStr");
    if (inStrStart != nullptr)
    {
        dwBCol = AttributesPointer->GetAttributeAsDword("backColor", 0);
        dwFCol = AttributesPointer->GetAttributeAsDword("foreColor", 0);
        nBorderWidth = AttributesPointer->GetAttributeAsDword("borderWidth", 0);
        fScale = AttributesPointer->GetAttributeAsFloat("scale", 1.f);
        nOutOffset = AttributesPointer->GetAttributeAsDword("offset", m_rs->CharHeight(0));
    }

    const char *picTexureFile = AttributesPointer->GetAttribute("picfilename");
    const char *picBackTexureFile = AttributesPointer->GetAttribute("picbackfilename");
    const uint32_t TMP_VERTEX_FORMAT = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2);
    struct TMP_VERTEX
    {
        CVECTOR pos;
        float w;
        uint32_t col;
        float tu, tv;
    } pV[4];

    pV[0].pos.z = 1.f;
    pV[0].w = .5f;
    pV[1].pos.z = 1.f;
    pV[1].w = .5f;
    pV[2].pos.z = 1.f;
    pV[2].w = .5f;
    pV[3].pos.z = 1.f;
    pV[3].w = .5f;

    IDirect3DSurface9 *pRenderTarget;
    if (m_rs->GetRenderTarget(&pRenderTarget) != D3D_OK)
    {
        return false;
    }
    
    if (pRenderTarget->GetDesc(&desc) != D3D_OK)
    {
        m_rs->Release(pRenderTarget);
        return false;
    }

    if (inStrStart)
    {
        const auto ntmp = m_rs->StringWidth(inStrStart, 0, fScale);
        nOutWidth = static_cast<int>(sqrtf(4.f * ntmp * nOutOffset) + .9f);
        // record width is approximately four times the height
        if (nOutWidth > static_cast<int>(desc.Width))
            nOutWidth = desc.Width;
        nInsideRectWidth = nOutWidth + nBorderWidth * 2;
        if (nInsideRectWidth > static_cast<int>(desc.Width))
            nInsideRectWidth = desc.Width;

        nRowQ = 0;
        for (const auto * ps = inStrStart; ps != nullptr && *ps;)
        {
            ps = GetCutString(ps, nOutWidth, fScale);
            nRowQ++;
        }
        if (nRowQ * nOutOffset > static_cast<int>(desc.Height))
            nRowQ = desc.Height / nOutOffset;
        nInsideRectHeight = nRowQ * nOutOffset + nBorderWidth * 2;
        if (nInsideRectHeight > static_cast<int>(desc.Height))
            nInsideRectHeight = desc.Height;
    }

    isOK = m_rs->IsInsideScene();
    auto bMakeEndScene = false;
    if (!isOK)
    {
        bMakeEndScene = true;
        isOK = (m_rs->BeginScene() == D3D_OK);
    }

    if (isOK)
    {
        // show picture
        if (picBackTexureFile != nullptr)
        {
            const int picBackID = m_rs->TextureCreate(picBackTexureFile);
            if (picBackID >= 0)
            {
                m_rs->TextureSet(0, picBackID);
                pV[0].col = pV[1].col = pV[2].col = pV[3].col = 0xFFFFFFFF;
                pV[0].pos.x = 0.f;
                pV[0].pos.y = 0.f;
                pV[1].pos.x = 0.f;
                pV[1].pos.y = static_cast<float>(desc.Height);
                pV[2].pos.x = static_cast<float>(desc.Width);
                pV[2].pos.y = 0.f;
                pV[3].pos.x = static_cast<float>(desc.Width);
                pV[3].pos.y = static_cast<float>(desc.Height);
                pV[0].tu = 0.f;
                pV[0].tv = 0.f;
                pV[1].tu = 0.f;
                pV[1].tv = 1.f;
                pV[2].tu = 1.f;
                pV[2].tv = 0.f;
                pV[3].tu = 1.f;
                pV[3].tv = 1.f;
                m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, TMP_VERTEX_FORMAT, 2, &pV, sizeof(TMP_VERTEX),
                                      "iInfoShowerPic");
                m_rs->TextureRelease(picBackID);
                m_rs->SetProgressBackImage(picBackTexureFile);
            }
        }

        if (picTexureFile != nullptr)
        {
            const int picID = m_rs->TextureCreate(picTexureFile);
            if (picID >= 0)
            {
                m_rs->TextureSet(0, picID);
                float dy = 0.0f;
                float dx = ((float(desc.Width) - (4.0f * float(desc.Height) / 3.0f)) / 2.0f);
                if (dx < 10.0f)
                    dx = 0.0f;
                else
                {
                    dy = 25.0f;
                    dx = ((float(desc.Width) - (4.0f * (float(desc.Height) - 2.0f * dy) / 3.0f)) / 2.0f);
                }

                pV[0].col = pV[1].col = pV[2].col = pV[3].col = 0xFFFFFFFF;
                pV[0].pos.x = 0.f + dx;
                pV[0].pos.y = 0.f + dy;
                pV[1].pos.x = 0.f + dx;
                pV[1].pos.y = (float)desc.Height - dy;
                pV[2].pos.x = (float)desc.Width - dx;
                pV[2].pos.y = 0.f + dy;
                pV[3].pos.x = (float)desc.Width - dx;
                pV[3].pos.y = (float)desc.Height - dy;
                pV[0].tu = 0.f;
                pV[0].tv = 0.f;
                pV[1].tu = 0.f;
                pV[1].tv = 1.f;
                pV[2].tu = 1.f;
                pV[2].tv = 0.f;
                pV[3].tu = 1.f;
                pV[3].tv = 1.f;

                char _name[MAX_PATH];
                sprintf(_name, "interfaces\\int_border.tga");
                int tipsID = m_rs->TextureCreate(_name);
                if (tipsID)
                {
                    m_rs->SetTipsImage(_name);
                    m_rs->TextureSet(1, tipsID);
                }

                m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, TMP_VERTEX_FORMAT, 2, &pV, sizeof(TMP_VERTEX),
                                      "iInfoShowerPicWithTips");
                m_rs->TextureRelease(picID);
                m_rs->SetProgressImage(picTexureFile);
            }
        }

        if (inStrStart)
        {
            // show back
            pV[0].col = pV[1].col = pV[2].col = pV[3].col = dwBCol;
            pV[0].pos.x = 0.f;
            pV[0].pos.y = 0.f;
            pV[1].pos.x = 0.f;
            pV[1].pos.y = static_cast<float>(desc.Height);
            pV[2].pos.x = static_cast<float>(desc.Width);
            pV[2].pos.y = 0.f;
            pV[3].pos.x = static_cast<float>(desc.Width);
            pV[3].pos.y = static_cast<float>(desc.Height);
            m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, TMP_VERTEX_FORMAT, 2, &pV, sizeof(TMP_VERTEX), "iInfoShower");

            // show fore
            pV[0].col = pV[1].col = pV[2].col = pV[3].col = dwFCol;
            pV[1].pos.x = pV[0].pos.x = static_cast<float>(desc.Width - nInsideRectWidth) / 2;
            pV[3].pos.x = pV[2].pos.x = pV[0].pos.x + nInsideRectWidth;
            pV[2].pos.y = pV[0].pos.y = static_cast<float>(desc.Height - nInsideRectHeight) / 2;
            pV[3].pos.y = pV[1].pos.y = pV[0].pos.y + nInsideRectHeight;
            m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, TMP_VERTEX_FORMAT, 2, &pV, sizeof(TMP_VERTEX), "iInfoShower");

            // show strings
            auto ntmp = 0;
            const int topY = (desc.Height - nRowQ * nOutOffset) / 2;
            for (auto * ps = inStrStart; ps != nullptr && *ps;)
            {
                auto *const oldps = ps;
                ps = GetCutString(ps, nOutWidth, fScale);
                if (!ps || ps == oldps)
                    break;
                StringToBufer(outStr, sizeof(outStr), oldps, ps - oldps);
                m_rs->ExtPrint(0, 0xFFFFFFFF, 0, PR_ALIGN_CENTER, true, fScale, 0, 0, desc.Width / 2,
                               topY + ntmp * nOutOffset, outStr);
                ntmp++;
                if (ntmp >= nRowQ)
                    break;
            }
        }

        if (bMakeEndScene)
            m_rs->EndScene();
    }

    m_rs->Release(pRenderTarget);

    return isOK;
}

const char *InfoHandler::GetCutString(const char *pstr, int nOutWidth, float fScale) const
{
    auto spaceWait = false;
    char param[1024];

    // remove first line breaks
    while (pstr && (*pstr == '\n' || *pstr == '\r' || *pstr == 32))
        pstr++;

    const char *oldps = nullptr;
    const char *ps;
    for (ps = pstr; ps && *ps; ps++)
    {
        if (*ps == '\n' || *ps == '\r')
            break;

        if (*ps == 32)
        {
            if (spaceWait)
                continue;
            spaceWait = true;

            const int n = ps - pstr;
            if (n == 0)
                continue;
            if (n >= 1023)
                break;

            strncpy_s(param, pstr, n);
            param[n] = 0;
            const int j = m_rs->StringWidth(param, 0, fScale);
            if (j < nOutWidth)
            {
                oldps = ps;
                continue;
            }
            if (oldps)
                return oldps;
            return ps;
        }
        spaceWait = false;
    }

    int nt = ps - pstr;
    if (nt <= 0)
        return nullptr;
    if (nt >= 1023)
    {
        nt = 1023;
        nt -= utf8::u8_dec(pstr + nt);
        ps = pstr + nt;
    }

    strncpy_s(param, pstr, nt);
    param[nt] = 0;
    const int jt = m_rs->StringWidth(param, 0, fScale);
    if (jt < nOutWidth)
        return ps;
    if (oldps)
        return oldps;

    return ps;
}

void InfoHandler::StringToBufer(char *outStr, int sizeBuf, const char *inStr, int copySize) const
{
    if (outStr == nullptr || sizeBuf <= 0)
        return;
    outStr[0] = 0;
    if (inStr == nullptr)
        return;
    while (*inStr && (*inStr == '\n' || *inStr == '\r' || *inStr == 32))
        inStr++;

    int n = strlen(inStr);
    if (n > copySize)
        n = copySize;
    if (n > sizeBuf - 1)
        n = sizeBuf - 1;

    if (n < 1)
        return;

    strncpy_s(outStr, sizeBuf, inStr, n);
    outStr[n] = 0;
}
