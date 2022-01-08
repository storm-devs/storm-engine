#pragma once

#include "../inode.h"

struct HELPEntity
{
    char *nodeName;
    CINODE *pNode;
    int32_t idHelpString;
};

class CXI_CONTEXTHELP : public CINODE
{
  public:
    CXI_CONTEXTHELP(CXI_CONTEXTHELP &&) = delete;
    CXI_CONTEXTHELP(const CXI_CONTEXTHELP &) = delete;
    CXI_CONTEXTHELP();
    ~CXI_CONTEXTHELP() override;

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
    void ChangeNode(CINODE *pNode);
    void SetTempHelp(const char *string);

    int32_t m_helpQuantity;
    HELPEntity *m_pHelpList;

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    char *GetCurrentHelpString(uint32_t deltaTime);

    uint32_t m_dwColor;
    uint32_t m_dwBorderColor;
    uint32_t m_dwFontColor;

    bool m_bBorder;
    int32_t m_offset;
    int32_t m_nHelpWidth;

    HELPEntity *m_curHelp;
    int32_t m_defaultString;

    int32_t m_nMaxDelayCounter;
    int32_t m_nCurDelayCounter;
    int32_t m_idTempString;
    char *m_sTempString;

    int32_t m_idFont;
    float m_fCurScale;
    float m_fMaxScale;

    XI_NOTEX_VERTEX m_pVert[4];
    RS_LINE m_pLines[8];
};
