#pragma once

#include "..//inode.h"

// picture
class CXI_GLOWCURSOR : public CINODE
{
  public:
    CXI_GLOWCURSOR();
    ~CXI_GLOWCURSOR() override;

    void Draw(bool bSelected, uint32_t Delta_Time) override;
    bool Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs, XYRECT &hostRect,
              XYPOINT &ScreenSize) override;
    void ReleaseAll() override;

    int CommandExecute(int wActCode) override
    {
        return 0;
    }

    bool IsClick(int buttonID, long xPos, long yPos) override
    {
        return false;
    }

    void MouseThis(float fX, float fY) override
    {
    }

    void ChangePosition(XYRECT &rNewPos) override;
    void SaveParametersToIni() override;

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void SetRectanglesToPosition(const XYRECT &rectXY);

    bool m_bShowGlow;
    bool m_bGlowToBack;

    bool m_bUseBlind;
    uint32_t m_dwFoneColor;
    uint32_t m_dwBlindColor;
    uint32_t m_dwCurColor;

    bool m_bUpBlind;
    float m_fCurM;
    float m_fCurM_UpSpeed;
    float m_fCurM_DownSpeed;

    float m_xOffset, m_yOffset;

    long m_idBackTex;
    CVideoTexture *m_pBackTex;

    XI_ONETEX_VERTEX m_pTexVert[14];

    CINODE *m_pPrevNode;
};
