#pragma once

#include "..//inode.h"

class CXI_QUESTTITLE : public CINODE
{
  public:
    CXI_QUESTTITLE(CXI_QUESTTITLE &&) = delete;
    CXI_QUESTTITLE(const CXI_QUESTTITLE &) = delete;
    CXI_QUESTTITLE();
    ~CXI_QUESTTITLE() override;

    void Draw(bool bSelected, uint32_t Delta_Time) override;
    bool Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs, XYRECT &hostRect,
              XYPOINT &ScreenSize) override;
    void ReleaseAll() override;
    int CommandExecute(int wActCode) override;
    bool IsClick(int buttonID, long xPos, long yPos) override;
    void MouseThis(float fX, float fY) override;
    void ChangePosition(XYRECT &rNewPos) override;
    void SaveParametersToIni() override;

    void SetNewTopQuest(ATTRIBUTES *pA, int topNum);

    float GetLineStep() const;
    void ScrollerChanged(float fPos);

  protected:
    bool GetLineNext(int fontNum, char *&pInStr, char *buf, int bufSize) const;
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;

    int m_iconWidth;
    int m_iconHeight;
    int m_iconVOffset;
    char *m_iconGroupName;
    FXYRECT m_texComplete;
    FXYRECT m_texNonComplete;
    long m_texId;

    long m_idFont;
    uint32_t m_dwNonCompleteColor;
    uint32_t m_dwCompleteColor;
    uint32_t m_dwSelectRectangleColor;
    int m_fontOffset;

    int m_stringQuantity;
    int m_allStrings;
    int m_vertOffset;
    int m_selectOffset;

    int m_curIdx;

    int m_nCommonQuantity;

    struct STRING_DESCRIBER
    {
        long lineQuantity;
        char *name[10];
        bool complete;
        uint32_t dwSpecColor;
    };

    STRING_DESCRIBER *m_strList;
};
