#pragma once

#include "..//inode.h"

class CXI_QUESTTEXTS : public CINODE
{
  public:
    CXI_QUESTTEXTS(CXI_QUESTTEXTS &&) = delete;
    CXI_QUESTTEXTS(const CXI_QUESTTEXTS &) = delete;
    CXI_QUESTTEXTS();
    ~CXI_QUESTTEXTS();

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
    uint32_t MessageProc(long msgcode, MESSAGE &message) override;

    void StartQuestShow(ATTRIBUTES *pA, int qn);

    float GetLineStep() const;
    void ScrollerChanged(float fPos);

  protected:
    bool GetLineNext(int fontNum, char *&pInStr, char *buf, int bufSize) const;
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void ReleaseStringes();
    void GetStringListForQuestRecord(std::vector<std::string> &asStringList, const char *pcRecText,
                                     const char *pcUserData) const;

    long m_idFont;
    uint32_t m_dwNonCompleteColor;
    uint32_t m_dwCompleteColor;

    int m_allStrings;
    int m_vertOffset;

    struct STRING_DESCRIBER
    {
        int strNum;
        char *lineStr;
        bool complete;
        STRING_DESCRIBER *next;
        STRING_DESCRIBER *prev;
        STRING_DESCRIBER *Add(const char *ls, bool complete);
        STRING_DESCRIBER(const char *ls);
    };

    STRING_DESCRIBER *m_listRoot;
    STRING_DESCRIBER *m_listCur;
    int m_nAllTextStrings;
};
