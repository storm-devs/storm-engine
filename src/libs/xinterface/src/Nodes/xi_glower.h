#pragma once

#include "../inode.h"

#define MAX_USED_RECTANGLE 20

#define GLOW_ACTION_NONE 0
#define GLOW_ACTION_COLORUP 1
#define GLOW_ACTION_SHOW 2
#define GLOW_ACTION_BLEND 3

class CXI_GLOWER : public CINODE
{
    struct GLOW_DESCR
    {
        RS_RECT rect;
        long allTime;
        long curTime;
        float angleSpeed;
        int action;
    };

  public:
    CXI_GLOWER();
    ~CXI_GLOWER() override;
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

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;

    long m_nQuantity;
    // glow describe
    GLOW_DESCR m_glows[MAX_USED_RECTANGLE];
    long m_texID;

    long m_minGlowTime;
    long m_maxGlowTime;
    long m_minShowTime;
    long m_maxShowTime;

    long m_nRandomMax;

    float m_fAngleSpeedMin;
    float m_fAngleSpeedMax;

    uint32_t m_dwMinColor;
    uint32_t m_dwMaxColor;
};
