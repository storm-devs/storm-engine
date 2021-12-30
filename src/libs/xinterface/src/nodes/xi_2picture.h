#pragma once

#include "..//inode.h"

// video
class CXI_TWOPICTURE : public CINODE
{
  public:
    CXI_TWOPICTURE();
    ~CXI_TWOPICTURE() override;
    void Draw(bool bSelected, uint32_t Delta_Time) override;
    bool Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs, XYRECT &hostRect,
              XYPOINT &ScreenSize) override;
    void ReleaseAll() override;
    int CommandExecute(int wActCode) override;
    bool IsClick(int buttonID, long xPos, long yPos) override;
    void MouseThis(float fX, float fY) override;

    void ChangePosition(XYRECT &rNewPos) override;
    void SaveParametersToIni() override;

    void SetNewPicture(char *sNewTexName);

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void UpdateRectangles();

    bool m_bMouseInsideIndifferent;

    long m_idOneTex;
    long m_idTwoTex;

    FXYPOINT m_picSize;
    FXYPOINT m_leftPicCenter;
    FXYPOINT m_rightPicCenter;

    uint32_t m_dwDarkColor;
    uint32_t m_dwSelectColor;
    uint32_t m_dwShadowColor;

    bool m_bLeftSelect;
    bool m_leftClick;

    FXYPOINT m_ShadowOffset;
    FXYPOINT m_PressShadowOffset;
    FXYPOINT m_PressOffset;

    XI_ONETEX_VERTEX m_vOne[4];
    XI_ONETEX_VERTEX m_vTwo[4];
    XI_NOTEX_VERTEX m_vSOne[4];
    XI_NOTEX_VERTEX m_vSTwo[4];
};
