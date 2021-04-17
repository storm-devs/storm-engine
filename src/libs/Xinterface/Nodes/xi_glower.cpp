#include "xi_glower.h"
#include <stdio.h>

CXI_GLOWER::CXI_GLOWER()
{
    m_nQuantity = 0L;
    m_texID = -1L;
    m_nNodeType = NODETYPE_GLOWER;
}

CXI_GLOWER::~CXI_GLOWER()
{
    ReleaseAll();
}

void CXI_GLOWER::Draw(bool bSelected, uint32_t Delta_Time)
{
    m_rs->TextureSet(0, m_texID);
    for (long i = 0; i < m_nQuantity; i++)
    {
        if (m_glows[i].action != GLOW_ACTION_NONE)
        {
            // recalculate color
            if (m_glows[i].action == GLOW_ACTION_COLORUP)
                m_glows[i].rect.dwColor = ColorInterpolate(m_dwMaxColor, m_dwMinColor,
                                                           static_cast<float>(m_glows[i].curTime) / m_glows[i].allTime);
            if (m_glows[i].action == GLOW_ACTION_BLEND)
                m_glows[i].rect.dwColor = ColorInterpolate(
                    m_dwMaxColor, m_dwMinColor, 1.f - static_cast<float>(m_glows[i].curTime) / m_glows[i].allTime);
            // show this rectangle
            m_rs->DrawRects(&m_glows[i].rect, 1, "iGlow");

            m_glows[i].rect.fAngle += m_glows[i].angleSpeed * Delta_Time;
            while (m_glows[i].rect.fAngle > 2.f * PI)
                m_glows[i].rect.fAngle -= 2.f * PI;
            if ((m_glows[i].curTime -= Delta_Time) < 0)
            {
                switch (m_glows[i].action)
                {
                case GLOW_ACTION_COLORUP:
                    m_glows[i].action = GLOW_ACTION_SHOW;
                    m_glows[i].rect.dwColor = m_dwMaxColor;
                    m_glows[i].curTime = m_glows[i].allTime =
                        static_cast<long>(m_minShowTime + (m_maxShowTime - m_minShowTime) * rand() / RAND_MAX);
                    break;
                case GLOW_ACTION_SHOW:
                    m_glows[i].action = GLOW_ACTION_BLEND;
                    m_glows[i].curTime = m_glows[i].allTime = static_cast<long>(
                        m_minGlowTime + static_cast<float>(m_maxGlowTime - m_minGlowTime) * rand() / RAND_MAX);
                    break;
                case GLOW_ACTION_BLEND:
                    m_glows[i].action = GLOW_ACTION_NONE;
                    break;
                }
            }
        }
        else
        {
            // to can or not to turn on the glow
            if (rand() < m_nRandomMax)
            {
                // yes! this new glow
                m_glows[i].action = GLOW_ACTION_COLORUP;
                m_glows[i].curTime = m_glows[i].allTime = static_cast<long>(
                    m_minGlowTime + static_cast<float>(m_maxGlowTime - m_minGlowTime) * rand() / RAND_MAX);
                m_glows[i].angleSpeed = -(m_fAngleSpeedMin + (m_fAngleSpeedMax - m_fAngleSpeedMin) * rand() / RAND_MAX);
            }
        }
    }
}

bool CXI_GLOWER::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                      XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    SetGlowCursor(false);
    return true;
}

void CXI_GLOWER::ReleaseAll()
{
    TEXTURE_RELEASE(m_rs, m_texID);
}

int CXI_GLOWER::CommandExecute(int wActCode)
{
    return -1;
}

bool CXI_GLOWER::IsClick(int buttonID, long xPos, long yPos)
{
    return false;
}

void CXI_GLOWER::ChangePosition(XYRECT &rNewPos)
{
    // no this action
}

void CXI_GLOWER::SaveParametersToIni()
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

void CXI_GLOWER::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    int i;
    char param[255];
    long x, y;

    // get texture
    m_texID = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "texture", param, sizeof(param), ""))
        m_texID = m_rs->TextureCreate(param);

    // calculate glow blinks quantity
    m_nQuantity =
        std::min(static_cast<std::size_t>(MAX_USED_RECTANGLE), ini1->ForEachString(name1, "pos", [](auto v) {}));

    // set common default value
    for (i = 0; i < m_nQuantity; i++)
    {
        m_glows[i].action = GLOW_ACTION_NONE;
        m_glows[i].rect.dwSubTexture = 0;
        m_glows[i].rect.fAngle = 0;
        m_glows[i].rect.vPos.z = 1.f;
    }

    // fill rectangle pos
    ini1->ForEachString(name1, "pos", [&](auto i, auto param) {
        if (i < m_nQuantity)
        {
            GetDataStr(param, "ll", &x, &y);
            m_glows[i].rect.vPos.x = static_cast<float>(x) + m_hostRect.left;
            m_glows[i].rect.vPos.y = static_cast<float>(y) + m_hostRect.top;

            return true;
        }
        return false;
    });

    // fill rectangles size
    x = GetIniLong(ini1, name1, ini2, name2, "spriteSize", 8);
    for (i = 0; i < m_nQuantity; i++)
        m_glows[i].rect.fSize = static_cast<float>(x);

    // set colors
    m_dwMinColor = GetIniARGB(ini1, name1, ini2, name2, "minColor", 0);
    m_dwMaxColor = GetIniARGB(ini1, name1, ini2, name2, "maxColor", 0xFFFFFFFF);

    m_minGlowTime = GetIniLong(ini1, name1, ini2, name2, "minGlowTime", 200);
    m_maxGlowTime = GetIniLong(ini1, name1, ini2, name2, "maxGlowTime", 600);
    m_minShowTime = GetIniLong(ini1, name1, ini2, name2, "minShowTime", 200);
    m_maxShowTime = GetIniLong(ini1, name1, ini2, name2, "maxShowTime", 600);

    m_nRandomMax = static_cast<long>(GetIniFloat(ini1, name1, ini2, name2, "createProbability", 0.1f) * RAND_MAX);

    m_fAngleSpeedMin = GetIniFloat(ini1, name1, ini2, name2, "minRotateSpeed", 15.f) * PI / 180000.f;
    m_fAngleSpeedMax = GetIniFloat(ini1, name1, ini2, name2, "maxRotateSpeed", 180.f) * PI / 180000.f;
}
