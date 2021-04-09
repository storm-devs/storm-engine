#pragma once

#include "..//inode.h"

// picture
class CXI_LRCHANGER : public CINODE
{
  public:
    CXI_LRCHANGER();
    ~CXI_LRCHANGER();

    void Draw(bool bSelected, uint32_t Delta_Time) override;
    bool Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs, XYRECT &hostRect,
              XYPOINT &ScreenSize) override;
    void ReleaseAll() override;
    int CommandExecute(int wActCode) override;
    bool IsClick(int buttonID, long xPos, long yPos) override;

    void MouseThis(float fX, float fY) override
    {
    }

    void ChangePosition(XYRECT &rNewPos) override;
    void SaveParametersToIni() override;
    long GetClickState() override;

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;

  protected:
    char *m_sGroupName;
    long m_idTex; // texture identity

    FXYRECT m_tLRect;
    FXYRECT m_tRRect;
    FXYRECT m_posLRect;
    FXYRECT m_posRRect;

    uint32_t m_dwShadowColor;
    uint32_t m_dwFaceColor;
    long m_dwLightSelCol;
    long m_dwDarkSelCol;

    long m_dwBlindDelay;
    long m_dwCurBlindState;
    bool m_bBlindIncrement;

    FXYPOINT m_ShadowShift;
    FXYPOINT m_PressShadowShift;
    FXYPOINT m_PressShift;

    int nPressedDelay;
    int nMaxDelay;

    bool m_bLeftPress;
};
