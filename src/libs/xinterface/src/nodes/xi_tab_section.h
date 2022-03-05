#pragma once

#include "../inode.h"

class CXI_TABSECTION : public CINODE
{
  public:
    CXI_TABSECTION();
    ~CXI_TABSECTION() override;

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

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void UpdateImage();

    std::string m_sIconGroupName;
    int32_t m_idIconTexture;
    int32_t m_idVBuf;
    int32_t m_idIBuf;
    int32_t m_nSubQ;

    int32_t m_nIcon_TopBorder;
    int32_t m_nIcon_BottomBorder;
    int32_t m_nIcon_LeftArrow;
    int32_t m_nIcon_RightArrow;
    int32_t m_nIcon_LeftBookmarkPart;
    int32_t m_nIcon_RightBookmarkPart;
    int32_t m_nIcon_MiddleBookmarkPart;

    int32_t m_nTopBorderHeight;
    int32_t m_nBottomBorderHeight;
    int32_t m_nTopBorderOffset;
    int32_t m_nBottomBorderOffset;
    int32_t m_nArrowOffset;
    int32_t m_nBookmarkSpace;

    XYPOINT m_pntArrowSize;
    XYPOINT m_pntLeftBookmarkSize;
    XYPOINT m_pntRightBookmarkSize;
    XYPOINT m_pntMiddleBookmarkSize;

    uint32_t m_dwBorderColor;
    uint32_t m_dwArrowColor;
    uint32_t m_dwBookmarkColor;
};
