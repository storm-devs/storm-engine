#include "xi_changer.h"

#include "core.h"

#include "vfile_service.h"

CXI_CHANGER::CXI_CHANGER()
{
    m_nPlaceQuantity = 0;
    m_pPlace = nullptr;

    m_dwFoneColor = ARGB(255, 0, 0, 0);
    m_xOffset = 0.f;
    m_yOffset = 0.f;

    m_idBackTex = -1;
    IDirect3DTexture9 *m_pTex = nullptr;

    m_bClickable = true;
    m_nNodeType = NODETYPE_CHANGER;
}

CXI_CHANGER::~CXI_CHANGER()
{
    ReleaseAll();
}

void CXI_CHANGER::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
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
            m_pTexVert[4].color = m_pTexVert[5].color = m_pTexVert[6].color = m_pTexVert[7].color = m_dwCurColor;
        }

        if (m_idBackTex >= 0)
        {
            m_rs->TextureSet(0, m_idBackTex);
            m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, &m_pTexVert[4], sizeof(XI_ONETEX_VERTEX),
                                  "iGlow");
        }
        else
        {
            m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, &m_pTexVert[4], sizeof(XI_ONETEX_VERTEX),
                                  "iRectangle");
        }
        m_rs->SetTexture(0, m_pTex ? m_pTex->m_pTexture : nullptr);
        m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, m_pTexVert, sizeof(XI_ONETEX_VERTEX), "iChanger");
    }
}

bool CXI_CHANGER::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                       XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    return true;
}

void CXI_CHANGER::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    int i;
    char param[255];
    FXYPOINT fPnt;

    const auto bRelativeRect = !GetIniLong(ini1, name1, ini2, name2, "bAbsoluteRectangle", 0);

    // get position quantity
    m_nPlaceQuantity = 0;
    if (ini1->ReadString(name1, "place", param, sizeof(param) - 1, ""))
        do
            m_nPlaceQuantity++;
        while (ini1->ReadStringNext(name1, "place", param, sizeof(param) - 1));

    // create position array
    if (m_nPlaceQuantity > 0)
    {
        m_pPlace = new XYRECT[m_nPlaceQuantity];
        if (m_pPlace == nullptr)
            throw std::exception("Allocate memory error");
        PZERO(m_pPlace, sizeof(XYRECT) * m_nPlaceQuantity);
    }

    // get rectangle positions
    ini1->ReadString(name1, "place", param, sizeof(param) - 1, "");
    for (i = 0; i < m_nPlaceQuantity; i++)
    {
        GetDataStr(param, "llll", &m_pPlace[i].left, &m_pPlace[i].top, &m_pPlace[i].right, &m_pPlace[i].bottom);
        if (bRelativeRect)
            GetRelativeRect(m_pPlace[i]);
        ini1->ReadStringNext(name1, "place", param, sizeof(param) - 1);
    }

    // get fone color
    m_dwFoneColor = GetIniARGB(ini1, name1, ini2, name2, "foneColor", ARGB(255, 255, 255, 255));
    m_dwCurColor = m_dwFoneColor;

    // get Blind color
    m_dwBlindColor = GetIniARGB(ini1, name1, ini2, name2, "blindColor", m_dwFoneColor);
    m_bUseBlind = m_dwBlindColor != m_dwFoneColor;

    m_bUpBlind = true;
    m_fCurM = 0.f;
    m_fCurM_UpSpeed = GetIniFloat(ini1, name1, ini2, name2, "blindUpTime", 1.f);
    m_fCurM_DownSpeed = GetIniFloat(ini1, name1, ini2, name2, "blindDownTime", m_fCurM_UpSpeed);

    if (m_fCurM_UpSpeed > 0.1f)
        m_fCurM_UpSpeed = 0.001f / m_fCurM_UpSpeed;
    else
        m_fCurM_UpSpeed = 1.f;
    if (m_fCurM_DownSpeed > 0.1f)
        m_fCurM_DownSpeed = 0.001f / m_fCurM_DownSpeed;
    else
        m_fCurM_DownSpeed = 1.f;

    // get outside picture offset
    fPnt = GetIniFloatPoint(ini1, name1, ini2, name2, "offset", FXYPOINT(0.f, 0.f));
    m_xOffset = fPnt.x;
    m_yOffset = fPnt.y;

    // get video texture (for inside picture)
    m_pTex = nullptr;
    if (ReadIniString(ini1, name1, ini2, name2, "videoTexture", param, sizeof(param), ""))
        m_pTex = m_rs->GetVideoTexture(param);

    if (ReadIniString(ini1, name1, ini2, name2, "backTexture", param, sizeof(param), ""))
        m_idBackTex = m_rs->TextureCreate(param);

    // set constant buffers data
    for (i = 0; i < 8; i++)
    {
        m_pTexVert[i].color = m_dwFoneColor;
        m_pTexVert[i].pos.z = 1.f;
    }
    m_pTexVert[4].tu = m_pTexVert[6].tu = m_pTexVert[0].tu = m_pTexVert[2].tu = 0.f;
    m_pTexVert[5].tu = m_pTexVert[7].tu = m_pTexVert[1].tu = m_pTexVert[3].tu = 1.f;
    m_pTexVert[4].tv = m_pTexVert[5].tv = m_pTexVert[0].tv = m_pTexVert[1].tv = 1.f;
    m_pTexVert[6].tv = m_pTexVert[7].tv = m_pTexVert[2].tv = m_pTexVert[3].tv = 0.f;

    m_nCurrentPos = 0;
    auto *pAttr = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
    if (pAttr != nullptr)
        m_nCurrentPos = pAttr->GetAttributeAsDword("current", 0);

    SetRectanglesToPosition(0);
}

void CXI_CHANGER::ReleaseAll()
{
    STORM_DELETE(m_pPlace);
    TEXTURE_RELEASE(m_rs, m_idBackTex);
    VIDEOTEXTURE_RELEASE(m_rs, m_pTex);
}

int CXI_CHANGER::CommandExecute(int wActCode)
{
    if (m_bUse)
    {
        auto bChangePosition = false;
        switch (wActCode)
        {
        case ACTION_MOUSECLICK:
        case ACTION_MOUSERCLICK:
            if (m_nClickPlaceNum >= 0 && m_nClickPlaceNum < m_nPlaceQuantity)
                if (m_nClickPlaceNum != m_nCurrentPos)
                {
                    m_nCurrentPos = m_nClickPlaceNum;
                    bChangePosition = true;
                }
                else
                {
                    bool bBreakPress;
                    if (m_bIncreaseChange)
                        DoAction(ACTION_RIGHTSTEP, bBreakPress, false);
                    else
                        DoAction(ACTION_LEFTSTEP, bBreakPress, false);
                }
            break;
        case ACTION_UPSTEP:
        case ACTION_SPEEDUP:
            bChangePosition = true;
            if (m_nCurrentPos > 0)
                m_nCurrentPos--;
            else
                m_nCurrentPos = m_nPlaceQuantity - 1;
            break;
        case ACTION_DOWNSTEP:
        case ACTION_SPEEDDOWN:
            bChangePosition = true;
            if (m_nCurrentPos < m_nPlaceQuantity - 1)
                m_nCurrentPos++;
            else
                m_nCurrentPos = 0;
            break;
        }

        if (bChangePosition && m_nCurrentPos >= 0 && m_nCurrentPos < m_nPlaceQuantity)
        {
            SetRectanglesToPosition(m_nCurrentPos);
            // set attribute to new position
            auto *pAttr = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
            if (pAttr != nullptr)
                pAttr->SetAttributeUseDword("current", m_nCurrentPos);
        }
    }
    return -1;
}

void CXI_CHANGER::SetRectanglesToPosition(int nPos)
{
    float left, top, right, bottom;

    left = static_cast<float>(m_pPlace[nPos].left);
    top = static_cast<float>(m_pPlace[nPos].top);
    right = static_cast<float>(m_pPlace[nPos].right);
    bottom = static_cast<float>(m_pPlace[nPos].bottom);

    // set background rectangle position
    m_pTexVert[4].pos.x = left - m_xOffset;
    m_pTexVert[4].pos.y = top - m_yOffset;
    m_pTexVert[5].pos.x = right + m_xOffset;
    m_pTexVert[5].pos.y = top - m_yOffset;
    m_pTexVert[6].pos.x = left - m_xOffset;
    m_pTexVert[6].pos.y = bottom + m_yOffset;
    m_pTexVert[7].pos.x = right + m_xOffset;
    m_pTexVert[7].pos.y = bottom + m_yOffset;
    // set front rectangle position
    m_pTexVert[0].pos.x = left;
    m_pTexVert[0].pos.y = top;
    m_pTexVert[1].pos.x = right;
    m_pTexVert[1].pos.y = top;
    m_pTexVert[2].pos.x = left;
    m_pTexVert[2].pos.y = bottom;
    m_pTexVert[3].pos.x = right;
    m_pTexVert[3].pos.y = bottom;
}

bool CXI_CHANGER::IsClick(int buttonID, long xPos, long yPos)
{
    if (m_bClickable && m_bUse)
    {
        for (auto i = 0; i < m_nPlaceQuantity; i++)
            if (xPos > m_pPlace[i].left && xPos < m_pPlace[i].right && yPos > m_pPlace[i].top &&
                yPos < m_pPlace[i].bottom)
            {
                m_nClickPlaceNum = i;
                if (buttonID == MOUSE_LBUTTON)
                    m_bIncreaseChange = true;
                else if (buttonID == MOUSE_RBUTTON)
                    m_bIncreaseChange = false;
                else
                    return false;
                return true;
            }
    }
    return false;
}

void CXI_CHANGER::ChangePosition(XYRECT &rNewPos)
{
    // no this action
}

void CXI_CHANGER::SaveParametersToIni()
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

XYRECT CXI_CHANGER::GetCursorRect()
{
    XYRECT rectPos;

    if (m_nCurrentPos < 0 || m_nCurrentPos >= m_nPlaceQuantity)
    {
        rectPos.left = rectPos.right = rectPos.top = rectPos.bottom = 0;
    }
    else
    {
        rectPos.left = m_pPlace[m_nCurrentPos].left;
        rectPos.top = m_pPlace[m_nCurrentPos].top;
        rectPos.right = m_pPlace[m_nCurrentPos].right;
        rectPos.bottom = m_pPlace[m_nCurrentPos].bottom;
    }
    return rectPos;
}

uint32_t CXI_CHANGER::MessageProc(long msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // Set position to N
    {
        const int n = message.Long();
        if (n >= 0 && n < m_nPlaceQuantity && n != m_nCurrentPos)
        {
            m_nCurrentPos = n;
            SetRectanglesToPosition(m_nCurrentPos);
            // set attribute to new position
            auto *pAttr = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
            if (pAttr != nullptr)
                pAttr->SetAttributeUseDword("current", m_nCurrentPos);
        }
    }
    break;
    }

    return -1;
}
