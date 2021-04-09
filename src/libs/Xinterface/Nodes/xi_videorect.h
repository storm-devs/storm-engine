#pragma once

#include "..//inode.h"

// video
class CXI_VIDEORECT : public CINODE
{
  public:
    CXI_VIDEORECT();
    ~CXI_VIDEORECT();
    void Draw(bool bSelected, uint32_t Delta_Time) override;
    bool Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs, XYRECT &hostRect,
              XYPOINT &ScreenSize) override;
    void ReleaseAll() override;
    int CommandExecute(int wActCode) override;
    bool IsClick(int buttonID, long xPos, long yPos) override
    {
        return false;
    }

    void MouseThis(float fX, float fY) override
    {
    }

    void ChangePosition(XYRECT &rNewPos) override;
    void SaveParametersToIni() override;
    uint32_t MessageProc(long msgcode, MESSAGE &message) override;

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void StartVideoPlay(char *videoFile);

    uint32_t m_dwColor;
    FXYRECT m_rectTex;
    entid_t m_eiVideo;
};
