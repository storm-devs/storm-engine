#include "InfoHandler.h"

#include "core.h"

#include "vmodule_api.h"

InfoHandler::InfoHandler()
{
    m_pSurface = nullptr;
    m_pRenderTarget = nullptr;
}

InfoHandler::~InfoHandler()
{
    if (m_pSurface)
        m_rs->Release(m_pSurface);
    if (m_pRenderTarget)
        m_rs->Release(m_pRenderTarget);
}

bool InfoHandler::Init()
{
    // get render service
    m_rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!m_rs)
    {
        core.Trace("No service: dx9render");
        return false;
    }
    if (m_rs->IsInsideScene())
    {
        m_rs->MakePostProcess();
    }

    if (m_rs->GetRenderTarget(&m_pRenderTarget) != D3D_OK || !m_pRenderTarget)
    {
        core.Trace("Can`t get render target");
        return false;
    }

    auto isOk = false;
    D3DSURFACE_DESC desc;
    if (m_pRenderTarget->GetDesc(&desc) == D3D_OK)
    {
        if (m_rs->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, &m_pSurface) == D3D_OK)
        {
            if (DoPreOut())
            {
                if (m_rs->GetRenderTargetData(m_pRenderTarget, m_pSurface) == D3D_OK)
                {
                    isOk = true;
                }
            }
        }
    }
    if (!isOk)
    {
        core.Trace("Screen shot for info shower not created!");
        if (m_pSurface)
        {
            m_rs->Release(m_pSurface);
            m_pSurface = nullptr;
        }
        if (m_pRenderTarget)
        {
            m_rs->Release(m_pRenderTarget);
            m_pRenderTarget = nullptr;
        }
        return false;
    }

    return true;
}

void InfoHandler::Execute(uint32_t delta_time)
{
}

void InfoHandler::Realize(uint32_t delta_time) const
{
    if (m_pSurface == nullptr || m_pRenderTarget == nullptr)
        return;
    m_rs->MakePostProcess();
    // keep the screen constant
    if (m_rs->UpdateSurface(m_pSurface, nullptr, 0, m_pRenderTarget, nullptr) != D3D_OK)
    {
        core.Trace("Can't copy fader screen shot to render target!");
    }
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
    char *inStrStart;
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
    auto *const picTexureFile = AttributesPointer->GetAttribute("picfilename");
    auto *const picBackTexureFile = AttributesPointer->GetAttribute("picbackfilename");
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

    if (m_pRenderTarget->GetDesc(&desc) != D3D_OK)
        return false;

    auto ntmp = 0;
    char *ps = nullptr;
    if (inStrStart)
    {
        ntmp = m_rs->StringWidth(inStrStart, 0, fScale);
        nOutWidth = static_cast<int>(sqrtf(4.f * ntmp * nOutOffset) + .9f);
        // record width is approximately four times the height
        if (nOutWidth > static_cast<int>(desc.Width))
            nOutWidth = desc.Width;
        nInsideRectWidth = nOutWidth + nBorderWidth * 2;
        if (nInsideRectWidth > static_cast<int>(desc.Width))
            nInsideRectWidth = desc.Width;

        nRowQ = 0;
        for (ps = inStrStart; ps != nullptr && *ps;)
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
            ntmp = 0;
            const int topY = (desc.Height - nRowQ * nOutOffset) / 2;
            for (ps = inStrStart; ps != nullptr && *ps;)
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

    return isOK;
}

char *InfoHandler::GetCutString(char *pstr, int nOutWidth, float fScale) const
{
    auto spaceWait = false;
    char param[1024];

    // remove first line breaks
    while (pstr && (*pstr == '\n' || *pstr == '\r' || *pstr == 32))
        pstr++;

    char *oldps = nullptr;
    char *ps;
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

void InfoHandler::StringToBufer(char *outStr, int sizeBuf, char *inStr, int copySize) const
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

/*
void InfoHandler::LostRender()
{
    if (m_pSurface) m_rs->Release(m_pSurface);
    if (m_pRenderTarget) m_rs->Release(m_pRenderTarget);
}

void InfoHandler::RestoreRender()
{
    if (m_rs->GetRenderTarget(&m_pRenderTarget) != D3D_OK)
    {
        core.Trace("Can`t get render target");
        return;
    }

    bool isOk = false;
    D3DSURFACE_DESC desc;
    if (m_pRenderTarget->GetDesc(&desc) == D3D_OK)
    {
        if (m_rs->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, &m_pSurface) == D3D_OK)
        {
            if (DoPreOut())
            {
                if (m_rs->GetRenderTargetData(m_pRenderTarget, m_pSurface) == D3D_OK)
                {
                    isOk = true;
                }
            }
        }
    }
    if (!isOk)
    {
        core.Trace("Screen shot for info shower not created!");
        if (m_pSurface)
        {
            m_rs->Release(m_pSurface); m_pSurface = 0;
        }
        if (m_pRenderTarget)
        {
            m_rs->Release(m_pRenderTarget); m_pRenderTarget = 0;
        }
    }
}*/
