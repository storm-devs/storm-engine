#include "xi_glowCursor.h"

CXI_GLOWCURSOR::CXI_GLOWCURSOR()
{
    m_nNodeType = NODETYPE_GLOWCURSOR;
    m_idBackTex = -1;
    m_pBackTex = nullptr;
    m_pPrevNode = nullptr;
    m_bShowGlow = false;
    m_bGlowToBack = false;
}

CXI_GLOWCURSOR::~CXI_GLOWCURSOR()
{
    ReleaseAll();
}

void CXI_GLOWCURSOR::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        if (Delta_Time > 0)
        {
            if (m_pPrevNode != ptrOwner->GetCurrentNode())
            {
                m_pPrevNode = static_cast<CINODE *>(ptrOwner->GetCurrentNode());
                if (m_pPrevNode)
                {
                    m_bShowGlow = m_pPrevNode->IsShowGlowCursor();
                    m_bGlowToBack = m_pPrevNode->IsGlowCursorBack();
                    if (m_bShowGlow)
                    {
                        SetRectanglesToPosition(m_pPrevNode->GetCursorRect());
                    }
                }
            }
            else
            {
                if (m_pPrevNode)
                {
                    m_bShowGlow = m_pPrevNode->IsShowGlowCursor();
                    if (m_bShowGlow && m_pPrevNode->IsGlowChanged())
                    {
                        const auto rectXY = m_pPrevNode->GetCursorRect();
                        if (static_cast<int>(m_pTexVert[0].pos.x) != rectXY.left ||
                            static_cast<int>(m_pTexVert[0].pos.y) != rectXY.top ||
                            static_cast<int>(m_pTexVert[3].pos.x) != rectXY.right ||
                            static_cast<int>(m_pTexVert[3].pos.y) != rectXY.bottom)
                        {
                            SetRectanglesToPosition(rectXY);
                        }
                    }
                }
            }
            if (m_bShowGlow)
            {
                if (m_bUseBlind)
                {
                    m_dwCurColor = ColorInterpolate(m_dwFoneColor, m_dwBlindColor, m_fCurM);
                    if (m_bUpBlind)
                    {
                        m_fCurM += m_fCurM_UpSpeed * Delta_Time;
                        if (m_fCurM >= 1.f)
                        {
                            m_fCurM = 1.f;
                            m_bUpBlind = false;
                        }
                    }
                    else
                    {
                        m_fCurM -= m_fCurM_DownSpeed * Delta_Time;
                        if (m_fCurM <= 0.f)
                        {
                            m_fCurM = 0.f;
                            m_bUpBlind = true;
                        }
                    }
                    for (auto j = 0; j < 14; j++)
                    {
                        m_pTexVert[j].color = m_dwCurColor;
                    }
                }

                if (m_idBackTex >= 0)
                {
                    m_rs->TextureSet(0, m_idBackTex);
                    m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, m_pTexVert, sizeof(XI_ONETEX_VERTEX),
                                          "iGlow");
                }
                if (m_pBackTex != nullptr)
                {
                    m_rs->SetTexture(0, m_pBackTex->m_pTexture);
                    m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, m_pTexVert, sizeof(XI_ONETEX_VERTEX),
                                          "iGlow");
                }
            }
        }
        if ((m_bGlowToBack && Delta_Time > 0) || (Delta_Time == 0 && !m_bGlowToBack))
        {
            if (m_bShowGlow)
            {
                if (m_idBackTex >= 0)
                {
                    m_rs->TextureSet(0, m_idBackTex);
                    m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 8, &m_pTexVert[4],
                                          sizeof(XI_ONETEX_VERTEX), "iGlow");
                }
                if (m_pBackTex != nullptr)
                {
                    m_rs->SetTexture(0, m_pBackTex->m_pTexture);
                    m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 8, &m_pTexVert[4],
                                          sizeof(XI_ONETEX_VERTEX), "iGlow");
                }
            }
        }
    }
}

bool CXI_GLOWCURSOR::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                          XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    return true;
}

void CXI_GLOWCURSOR::ReleaseAll()
{
    TEXTURE_RELEASE(m_rs, m_idBackTex);
    VIDEOTEXTURE_RELEASE(m_rs, m_pBackTex);
}

void CXI_GLOWCURSOR::ChangePosition(XYRECT &rNewPos)
{
    SetRectanglesToPosition(rNewPos);
}

void CXI_GLOWCURSOR::SaveParametersToIni()
{
    char pcWriteParam[2048];

    auto pIni = fio->OpenIniFile(ptrOwner->m_sDialogFileName.c_str());
    if (!pIni)
    {
        core.Trace("Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.c_str());
        return;
    }

    // save position
    sprintf_s(pcWriteParam, sizeof(pcWriteParam), "%d,%d,%d,%d", m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
    pIni->WriteString(m_nodeName, "position", pcWriteParam);
}

void CXI_GLOWCURSOR::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    int i;
    char param[255];
    FXYPOINT fPnt;

    m_rect.left = m_rect.top = m_rect.right = m_rect.bottom = 0;

    // get fone color
    m_dwFoneColor = GetIniARGB(ini1, name1, ini2, name2, "foneColor", ARGB(255, 128, 128, 128));
    m_dwCurColor = m_dwFoneColor;

    // get Blind color
    m_dwBlindColor = GetIniARGB(ini1, name1, ini2, name2, "blindColor", m_dwFoneColor);
    m_bUseBlind = m_dwBlindColor != m_dwFoneColor;

    m_bUpBlind = true;
    m_fCurM = 0.f;
    m_fCurM_UpSpeed = GetIniFloat(ini1, name1, ini2, name2, "blindUpTime", 1.f);
    m_fCurM_DownSpeed = GetIniFloat(ini1, name1, ini2, name2, "blindDownTime", 0.5f);

    if (m_fCurM_UpSpeed > 0.1f)
        m_fCurM_UpSpeed = 0.001f / m_fCurM_UpSpeed;
    else
        m_fCurM_UpSpeed = 1.f;
    if (m_fCurM_DownSpeed > 0.1f)
        m_fCurM_DownSpeed = 0.001f / m_fCurM_DownSpeed;
    else
        m_fCurM_DownSpeed = 1.f;

    // get outside picture offset
    fPnt = GetIniFloatPoint(ini1, name1, ini2, name2, "offset", FXYPOINT(40.f, 30.f));
    m_xOffset = fPnt.x;
    m_yOffset = fPnt.y;

    // get video texture (for inside picture)
    m_pBackTex = nullptr;
    if (ReadIniString(ini1, name1, ini2, name2, "videoTexture", param, sizeof(param), ""))
        m_pBackTex = m_rs->GetVideoTexture(param);

    if (ReadIniString(ini1, name1, ini2, name2, "backTexture", param, sizeof(param), ""))
        m_idBackTex = m_rs->TextureCreate(param);

    // set constant buffers data
    for (i = 0; i < 14; i++)
    {
        m_pTexVert[i].color = m_dwFoneColor;
        m_pTexVert[i].pos.z = 1.f;
    }

    m_pTexVert[4].tu = m_pTexVert[10].tu = m_pTexVert[12].tu = 0.f;
    m_pTexVert[6].tu = m_pTexVert[8].tu = 1.f;
    m_pTexVert[4].tv = m_pTexVert[6].tv = m_pTexVert[12].tv = 0.f;
    m_pTexVert[8].tv = m_pTexVert[10].tv = 1.f;

    XYRECT rectXY;
    PZERO(&rectXY, sizeof(rectXY));
    SetRectanglesToPosition(rectXY);
}

void CXI_GLOWCURSOR::SetRectanglesToPosition(const XYRECT &rectXY)
{
    auto fW = static_cast<float>(rectXY.right - rectXY.left);
    if (fW < 1.f)
        m_bShowGlow = false;
    auto fH = static_cast<float>(rectXY.bottom - rectXY.top);
    if (fH < 1.f)
        m_bShowGlow = false;
    if (!m_bShowGlow)
        return;

    auto fxOffset = m_xOffset;
    auto fyOffset = m_yOffset;
    if (m_pPrevNode)
        m_pPrevNode->UpdateGlowOffsets(fxOffset, fyOffset);

    fW = fW / (fW + fxOffset * 2.f);
    fH = fH / (fH + fyOffset * 2.f);

    // set front rectangle position
    m_pTexVert[4].pos.x = m_pTexVert[10].pos.x = m_pTexVert[12].pos.x = rectXY.left - fxOffset;
    m_pTexVert[4].pos.y = m_pTexVert[6].pos.y = m_pTexVert[12].pos.y = rectXY.top - fyOffset;
    m_pTexVert[6].pos.x = m_pTexVert[8].pos.x = rectXY.right + fxOffset;
    m_pTexVert[8].pos.y = m_pTexVert[10].pos.y = rectXY.bottom + fyOffset;

    m_pTexVert[0].pos.x = m_pTexVert[2].pos.x = m_pTexVert[5].pos.x = m_pTexVert[11].pos.x = m_pTexVert[13].pos.x =
        static_cast<float>(rectXY.left);
    m_pTexVert[0].pos.y = m_pTexVert[1].pos.y = m_pTexVert[5].pos.y = m_pTexVert[7].pos.y = m_pTexVert[13].pos.y =
        static_cast<float>(rectXY.top);
    m_pTexVert[1].pos.x = m_pTexVert[3].pos.x = m_pTexVert[7].pos.x = m_pTexVert[9].pos.x =
        static_cast<float>(rectXY.right);
    m_pTexVert[2].pos.y = m_pTexVert[3].pos.y = m_pTexVert[9].pos.y = m_pTexVert[11].pos.y =
        static_cast<float>(rectXY.bottom);

    m_pTexVert[0].tu = m_pTexVert[2].tu = m_pTexVert[5].tu = m_pTexVert[11].tu = m_pTexVert[13].tu = (1.f - fW) * .5f;
    m_pTexVert[0].tv = m_pTexVert[1].tv = m_pTexVert[5].tv = m_pTexVert[7].tv = m_pTexVert[13].tv = (1.f - fH) * .5f;
    m_pTexVert[1].tu = m_pTexVert[3].tu = m_pTexVert[7].tu = m_pTexVert[9].tu = (1.f + fW) * .5f;
    m_pTexVert[2].tv = m_pTexVert[3].tv = m_pTexVert[9].tv = m_pTexVert[11].tv = (1.f + fH) * .5f;
}
