#pragma once

#include "../inode.h"

class CXI_RECTANGLE : public CINODE
{
  public:
    CXI_RECTANGLE();
    ~CXI_RECTANGLE() override;

    void Draw(bool bSelected, uint32_t Delta_Time) override;
    bool Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs, XYRECT &hostRect,
              XYPOINT &ScreenSize) override;
    void ReleaseAll() override;
    int CommandExecute(int wActCode) override;
    bool IsClick(int buttonID, int32_t xPos, int32_t yPos) override;

    void MouseThis(float fX, float fY) override
    {
    }

    void ChangePosition(XYRECT &rNewPos) override;
    void SaveParametersToIni() override;
    uint32_t MessageProc(int32_t msgcode, MESSAGE &message) override;

    bool IsGlowChanged() override
    {
        return true;
    }

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void UpdateColors();

    uint32_t m_dwTopColor;
    uint32_t m_dwBottomColor;
    uint32_t m_dwLeftColor;
    uint32_t m_dwRightColor;
    bool m_bBorder;
    uint32_t m_dwBorderColor;

    XI_NOTEX_VERTEX m_pVert[4];
};
