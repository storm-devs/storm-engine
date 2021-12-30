#pragma once

#include "../inode.h"

class INIFILE;

class CXI_KEYCHANGER : public CINODE
{
  public:
    CXI_KEYCHANGER();
    ~CXI_KEYCHANGER() override;

    void Draw(bool bSelected, uint32_t Delta_Time) override;
    bool Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs, XYRECT &hostRect,
              XYPOINT &ScreenSize) override;
    void ReleaseAll() override;

    int CommandExecute(int wActCode) override
    {
        return -1;
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
    uint32_t MessageProc(long msgcode, MESSAGE &message) override;

    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override
    {
    }

  protected:
    void SetChoosingControls(ATTRIBUTES *pA);

  protected:
    long *m_pControlsID;
    bool *m_pbControlsStick;
    long m_keysQuantity;
    bool m_bKeyCheck;
};
