#include "xi_slidepicture.h"
#include <stdio.h>

void SetTextureCoordinate(XI_ONETEX_VERTEX v[4], FXYRECT tr, float angle)
{
    if (angle == 0)
    {
        v[0].tu = v[1].tu = tr.left;
        v[2].tu = v[3].tu = tr.right;
        v[0].tv = v[2].tv = tr.top;
        v[1].tv = v[3].tv = tr.bottom;
    }
    else
    {
        const auto x = (tr.left + tr.right) * .5f;
        const auto y = (tr.top + tr.bottom) * .5f;
        const auto width = tr.right - tr.left;
        const auto height = tr.bottom - tr.top;
        const auto ca = cosf(angle);
        const auto sa = sinf(angle);
        const auto wca = width / 2 * ca;
        const auto wsa = width / 2 * sa;
        const auto hca = height / 2 * ca;
        const auto hsa = height / 2 * sa;
        v[0].tu = x + (-wca + hsa);
        v[0].tv = y + (-wsa - hca);
        v[1].tu = x + (-wca - hsa);
        v[1].tv = y + (-wsa + hca);
        v[2].tu = x + (wca + hsa);
        v[2].tv = y + (wsa - hca);
        v[3].tu = x + (wca - hsa);
        v[3].tv = y + (wsa + hca);
    }
}

CXI_SLIDEPICTURE::CXI_SLIDEPICTURE()
{
    nLifeTime = 0;
    m_rs = nullptr;
    m_idTex = -1L;
    m_nNodeType = NODETYPE_SLIDEPICTURE;
    pSlideSpeedList = nullptr;
    nSlideListSize = 0;
    strTechniqueName = nullptr;
}

CXI_SLIDEPICTURE::~CXI_SLIDEPICTURE()
{
    ReleaseAll();
}

void CXI_SLIDEPICTURE::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        Update(Delta_Time);
        m_rs->TextureSet(0, m_idTex);
        m_rs->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        m_rs->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
        if (strTechniqueName == nullptr)
            m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, m_v, sizeof(XI_ONETEX_VERTEX), "iVideo");
        else
            m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, m_v, sizeof(XI_ONETEX_VERTEX),
                                  strTechniqueName);
    }
}

bool CXI_SLIDEPICTURE::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                            XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    SetGlowCursor(false);
    return true;
}

void CXI_SLIDEPICTURE::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    int i;
    char param[255];
    FXYPOINT fPos;

    STORM_DELETE(strTechniqueName);
    if (ReadIniString(ini1, name1, ini2, name2, "techniqueName", param, sizeof(param), ""))
    {
        const auto len = strlen(param) + 1;
        if (strlen(param) > 1)
        {
            strTechniqueName = new char[len];
            if (strTechniqueName == nullptr)
            {
                throw std::exception("allocate memory error");
            }
            memcpy(strTechniqueName, param, len);
        }
    }

    m_idTex = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "textureName", param, sizeof(param), ""))
        m_idTex = m_rs->TextureCreate(param);

    m_texRect = GetIniFloatRect(ini1, name1, ini2, name2, "textureRect", FXYRECT(0.f, 0.f, 1.f, 1.f));

    const auto color = GetIniARGB(ini1, name1, ini2, name2, "color", 0xFFFFFFFF);

    // Create rectangle
    m_v[0].pos.x = static_cast<float>(m_rect.left);
    m_v[0].pos.y = static_cast<float>(m_rect.top);
    m_v[0].tu = m_texRect.left;
    m_v[0].tv = m_texRect.top;
    m_v[1].pos.x = static_cast<float>(m_rect.left);
    m_v[1].pos.y = static_cast<float>(m_rect.bottom);
    m_v[1].tu = m_texRect.left;
    m_v[1].tv = m_texRect.bottom;
    m_v[2].pos.x = static_cast<float>(m_rect.right);
    m_v[2].pos.y = static_cast<float>(m_rect.top);
    m_v[2].tu = m_texRect.right;
    m_v[2].tv = m_texRect.top;
    m_v[3].pos.x = static_cast<float>(m_rect.right), m_v[3].pos.y = static_cast<float>(m_rect.bottom);
    m_v[3].tu = m_texRect.right;
    m_v[3].tv = m_texRect.bottom;
    for (i = 0; i < 4; i++)
    {
        m_v[i].color = color;
        m_v[i].pos.z = 1.f;
    }

    curAngle = 0.f;
    curRotate = 0.f;
    fPos = GetIniFloatPoint(ini1, name1, ini2, name2, "rotate", FXYPOINT(0.f, 0.f));
    minRotate = fPos.x;
    deltaRotate = fPos.y;

    nLifeTime = 0;
    nCurSlide = 0;
    nSlideListSize = 0;
    pSlideSpeedList = nullptr;

    auto bUse1Ini = true;
    // Calculating the size of the speed table
    if (ini1->ReadString(name1, "speed", param, sizeof(param) - 1, ""))
    {
        do
            nSlideListSize++;
        while (ini1->ReadStringNext(name1, "speed", param, sizeof(param) - 1));
    }
    else
    {
        if (ini2->ReadString(name2, "speed", param, sizeof(param) - 1, ""))
        {
            bUse1Ini = false;
            do
                nSlideListSize++;
            while (ini2->ReadStringNext(name2, "speed", param, sizeof(param) - 1));
        }
    }

    if (nSlideListSize > 0)
    {
        pSlideSpeedList = new SLIDE_SPEED[nSlideListSize];
        if (pSlideSpeedList == nullptr)
        {
            throw std::exception("allocate memory error");
        }
    }

    // fill in the speed table
    if (bUse1Ini)
    {
        ini1->ReadString(name1, "speed", param, sizeof(param) - 1, "");
        for (i = 0; i < nSlideListSize; i++)
        {
            pSlideSpeedList[i].time = 0;
            pSlideSpeedList[i].xspeed = 0;
            pSlideSpeedList[i].yspeed = 0;
            GetDataStr(param, "lff", &pSlideSpeedList[i].time, &pSlideSpeedList[i].xspeed, &pSlideSpeedList[i].yspeed);
            ini1->ReadStringNext(name1, "speed", param, sizeof(param) - 1);
        }
    }
    else
    {
        ini2->ReadString(name2, "speed", param, sizeof(param) - 1, "");
        for (i = 0; i < nSlideListSize; i++)
        {
            pSlideSpeedList[i].time = 0;
            pSlideSpeedList[i].xspeed = 0;
            pSlideSpeedList[i].yspeed = 0;
            GetDataStr(param, "lff", &pSlideSpeedList[i].time, &pSlideSpeedList[i].xspeed, &pSlideSpeedList[i].yspeed);
            ini2->ReadStringNext(name2, "speed", param, sizeof(param) - 1);
        }
    }
}

void CXI_SLIDEPICTURE::ReleaseAll()
{
    TEXTURE_RELEASE(m_rs, m_idTex);
    STORM_DELETE(pSlideSpeedList);
    STORM_DELETE(strTechniqueName);
    nSlideListSize = 0;
}

int CXI_SLIDEPICTURE::CommandExecute(int wActCode)
{
    return -1;
}

bool CXI_SLIDEPICTURE::IsClick(int buttonID, long xPos, long yPos)
{
    return false;
}

void CXI_SLIDEPICTURE::ChangePosition(XYRECT &rNewPos)
{
    m_rect = rNewPos;
    m_v[0].pos.x = static_cast<float>(m_rect.left);
    m_v[0].pos.y = static_cast<float>(m_rect.top);
    m_v[1].pos.x = static_cast<float>(m_rect.left);
    m_v[1].pos.y = static_cast<float>(m_rect.bottom);
    m_v[2].pos.x = static_cast<float>(m_rect.right);
    m_v[2].pos.y = static_cast<float>(m_rect.top);
    m_v[3].pos.x = static_cast<float>(m_rect.right), m_v[3].pos.y = static_cast<float>(m_rect.bottom);
}

void CXI_SLIDEPICTURE::SaveParametersToIni()
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

void CXI_SLIDEPICTURE::SetNewPicture(char *sNewTexName)
{
    if (m_idTex != -1L)
        m_rs->TextureRelease(m_idTex);
    m_idTex = m_rs->TextureCreate(sNewTexName);
}

void CXI_SLIDEPICTURE::Update(uint32_t Delta_Time)
{
    if (nCurSlide >= nSlideListSize)
        return;

    nLifeTime -= Delta_Time;
    if (nLifeTime < 0)
    {
        // changing speed
        nCurSlide++;
        if (nCurSlide >= nSlideListSize)
            nCurSlide = 0;
        nLifeTime = pSlideSpeedList[nCurSlide].time;
        curRotate = minRotate + rand() * deltaRotate / RAND_MAX;
    }

    const auto xadd = pSlideSpeedList[nCurSlide].xspeed * (Delta_Time / 1000.f);
    const auto yadd = pSlideSpeedList[nCurSlide].yspeed * (Delta_Time / 1000.f);

    curAngle += curRotate * Delta_Time / 1000.f;

    m_texRect.left += xadd;
    m_texRect.right += xadd;
    m_texRect.top += yadd;
    m_texRect.bottom += yadd;

    while (m_texRect.left < -10)
    {
        m_texRect.left += 10;
        m_texRect.right += 10;
    }
    while (m_texRect.right > 10)
    {
        m_texRect.left -= 10;
        m_texRect.right -= 10;
    }
    while (m_texRect.top < -10)
    {
        m_texRect.top += 10;
        m_texRect.bottom += 10;
    }
    while (m_texRect.top > 10)
    {
        m_texRect.top -= 10;
        m_texRect.bottom -= 10;
    }

    SetTextureCoordinate(m_v, m_texRect, curAngle);
}
