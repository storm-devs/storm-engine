#pragma once

#include "../inode.h"

// video
class CXI_SLIDELINE : public CINODE
{
  public:
    CXI_SLIDELINE();
    ~CXI_SLIDELINE() override;
    void Draw(bool bSelected, uint32_t Delta_Time) override;
    bool Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs, XYRECT &hostRect,
              XYPOINT &ScreenSize) override;
    void ReleaseAll() override;
    int CommandExecute(int wActCode) override;
    bool IsClick(int buttonID, int32_t xPos, int32_t yPos) override;
    void MouseThis(float fX, float fY) override;
    void ChangePosition(XYRECT &rNewPos) override;
    void SaveParametersToIni() override;
    uint32_t MessageProc(int32_t msgcode, MESSAGE &message) override;

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void SetNewValue(int32_t newValue);
    void DoMouseControl();

    bool m_bDoChangeSlider;

    int32_t m_idTexLine;
    int32_t m_idTexSelLine;
    int32_t m_idTexPointer;
    int32_t m_idVBuf;

    uint32_t m_dwDisableColor;

    int32_t m_nPointerWidth;
    int32_t m_nPointerHeight;

    int32_t m_nBaseLeft;
    int32_t m_nPointerLeft;

    int32_t m_nGrateQuantity;
    int32_t m_nCurValue;
    int32_t m_nSpeedSlide;

    int32_t m_nMaxValue;
    int32_t m_nMinValue;
};
