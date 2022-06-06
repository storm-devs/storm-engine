#include "xi_video_rect.h"
#include "../base_video.h"
#include "entity.h"

CXI_VIDEORECT::CXI_VIDEORECT()
    : m_dwColor(0), m_eiVideo(0)
{
    m_rs = nullptr;
    m_nNodeType = NODETYPE_VIDEORECT;
}

CXI_VIDEORECT::~CXI_VIDEORECT()
{
    ReleaseAll();
}

void CXI_VIDEORECT::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        if (auto *const ptr = core.GetEntityPointer(m_eiVideo))
        {
            auto *const pTex = static_cast<xiBaseVideo *>(ptr)->GetCurrentVideoTexture();
            if (pTex != nullptr)
            {
                // Create rectangle
                XI_ONETEX_VERTEX v[4];
                v[0].pos.x = static_cast<float>(m_rect.left);
                v[0].pos.y = static_cast<float>(m_rect.top);
                v[0].tu = m_rectTex.left;
                v[0].tv = m_rectTex.bottom;
                v[1].pos.x = static_cast<float>(m_rect.left);
                v[1].pos.y = static_cast<float>(m_rect.bottom);
                v[1].tu = m_rectTex.left;
                v[1].tv = m_rectTex.top;
                v[2].pos.x = static_cast<float>(m_rect.right);
                v[2].pos.y = static_cast<float>(m_rect.top);
                v[2].tu = m_rectTex.right;
                v[2].tv = m_rectTex.bottom;
                v[3].pos.x = static_cast<float>(m_rect.right), v[3].pos.y = static_cast<float>(m_rect.bottom);
                v[3].tu = m_rectTex.right;
                v[3].tv = m_rectTex.top;
                for (auto i = 0; i < 4; i++)
                {
                    v[i].color = m_dwColor;
                    v[i].pos.z = 1.f;
                }

                m_rs->SetTexture(0, pTex);
                m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, v, sizeof(XI_ONETEX_VERTEX), "iVideo");
            }
        }
    }
}

bool CXI_VIDEORECT::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                         XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    SetGlowCursor(false);
    return true;
}

void CXI_VIDEORECT::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    char param[255];

    m_dwFlags = GetIniLong(ini1, name1, ini2, name2, "flags", 0);
    m_rectTex = GetIniFloatRect(ini1, name1, ini2, name2, "textureRect", FXYRECT(0.f, 0.f, 1.f, 1.f));
    m_dwColor = GetIniARGB(ini1, name1, ini2, name2, "color", ARGB(255, 128, 128, 128));
    if (ReadIniString(ini1, name1, ini2, name2, "videoFile", param, sizeof(param), ""))
        StartVideoPlay(param);
}

void CXI_VIDEORECT::ReleaseAll()
{
    core.EraseEntity(m_eiVideo);
}

void CXI_VIDEORECT::ChangePosition(XYRECT &rNewPos)
{
    m_rect = rNewPos;
}

void CXI_VIDEORECT::SaveParametersToIni()
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

int CXI_VIDEORECT::CommandExecute(int wActCode)
{
    return -1;
}

uint32_t CXI_VIDEORECT::MessageProc(int32_t msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // start video
    {
        const std::string &param = message.String();
        StartVideoPlay(param.c_str());
    }
    break;
    }

    return 0;
}

void CXI_VIDEORECT::StartVideoPlay(const char *videoFileName)
{
    if (core.GetEntityPointer(m_eiVideo))
    {
        core.EraseEntity(m_eiVideo);
    }
    if (videoFileName == nullptr)
        return;

    m_eiVideo = core.CreateEntity("CAviPlayer");
    m_rectTex.bottom = 1.f - m_rectTex.bottom;
    m_rectTex.top = 1.f - m_rectTex.top;
    if (auto *const ptr = core.GetEntityPointer(m_eiVideo))
        static_cast<xiBaseVideo *>(ptr)->SetShowVideo(false);
    core.Send_Message(m_eiVideo, "ll", MSG_SET_VIDEO_FLAGS, m_dwFlags);
    core.Send_Message(m_eiVideo, "ls", MSG_SET_VIDEO_PLAY, videoFileName);
}
