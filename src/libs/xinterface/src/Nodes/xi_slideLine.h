#pragma once

#include "..//inode.h"

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
    bool IsClick(int buttonID, long xPos, long yPos) override;
    void MouseThis(float fX, float fY) override;
    void ChangePosition(XYRECT &rNewPos) override;
    void SaveParametersToIni() override;
    uint32_t MessageProc(long msgcode, MESSAGE &message) override;

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void SetNewValue(long newValue);
    void DoMouseControl();

    bool m_bDoChangeSlider;

    long m_idTexLine;
    long m_idTexSelLine;
    long m_idTexPointer;
    long m_idVBuf;

    uint32_t m_dwDisableColor;

    long m_nPointerWidth;
    long m_nPointerHeight;

    long m_nBaseLeft;
    long m_nPointerLeft;

    long m_nGrateQuantity;
    long m_nCurValue;
    long m_nSpeedSlide;

    long m_nMaxValue;
    long m_nMinValue;
};
