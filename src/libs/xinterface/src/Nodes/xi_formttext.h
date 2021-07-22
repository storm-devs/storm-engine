#pragma once

#include "../inode.h"

class CXI_FORMATEDTEXT : public CINODE
{
    struct STRING_DESCRIBER
    {
        int strNum;
        int strGroup;
        char *lineStr;
        uint32_t color;
        STRING_DESCRIBER *next;
        STRING_DESCRIBER *prev;

        struct TagInfo
        {
            uint32_t dwColor;
            std::string str;
        };

        std::vector<TagInfo> m_tags;

        STRING_DESCRIBER *Add(char *ls, int groupNum);
        STRING_DESCRIBER(char *ls);

        STRING_DESCRIBER()
        {
        }
    };

  public:
    CXI_FORMATEDTEXT(CXI_FORMATEDTEXT &&) = delete;
    CXI_FORMATEDTEXT(const CXI_FORMATEDTEXT &) = delete;
    CXI_FORMATEDTEXT();
    ~CXI_FORMATEDTEXT() override;

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
    XYRECT GetCursorRect() override;

    bool IsGlowChanged() override
    {
        return true;
    }

    void SetFormatedText(const char *str);
    void SetPointer(float fPos);
    float GetLineStep() const;
    float GetCurPos() const;
    void SetColor(uint32_t dwCol) const;

    long GetAllHeight();

  protected:
    bool GetLineNext(int fontNum, char *&pInStr, char *buf, int bufSize) const;
    void GetOneLine(int fontNum, char *pStr, char *buf, int bufSize) const;
    void MakeTagChecking(bool &tagState, uint32_t &tagColor, uint32_t normColor, STRING_DESCRIBER *pStrDescr);
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void ReleaseString(STRING_DESCRIBER *pCur);
    void ReleaseStringes();
    long AddFormatedText(const char *str);
    long FindUpGroup(long grNum);
    long FindDownGroup(long grNum);
    void SetVertexToNewGroup(bool bUpDirect, long upIdx, long downIdx);
    void CheckScrollButtons();
    void SetSpecialStrings(ATTRIBUTES *pARoot);
    void ControlSyncronouseNodes();
    long GetFirstGroupNum();
    void SetCurrentGroupNum(long nFirstNum, long nSelectNum);
    void ReplaceString(long nGrpNum, const char *pSrcStr);
    void InsertStringBefore(STRING_DESCRIBER *pNextDescr, const char *pSrcStr, long nGrpNum, uint32_t dwColor);
    void RecalculateStringNumber();
    void VAlignment(long nAlign);

  protected:
    long m_idFont;
    uint32_t m_dwColor;
    float m_fFontScale;
    long m_nAlignment;

    int m_allStrings;
    int m_vertOffset;
    long m_leftOffset;
    long m_nVAlignmentOffset;

    STRING_DESCRIBER *m_listRoot;
    STRING_DESCRIBER *m_listCur;
    int m_nAllTextStrings;
    int m_nStringGroupQuantity;

    bool m_bSelectableCursor;
    CVideoTexture *m_pVidTex;
    long m_idVBuf;
    long m_nCurGroupNum;
    float m_fHorzScale;

    bool m_bUpEnable;
    bool m_bDownEnable;
    long m_idUpEnableTexture;
    long m_idUpDisableTexture;
    long m_idDownEnableTexture;
    long m_idDownDisableTexture;
    XYRECT m_frUpPos;
    XYRECT m_frDownPos;
    FXYRECT m_frUpEnableUV;
    FXYRECT m_frUpDisableUV;
    FXYRECT m_frDownEnableUV;
    FXYRECT m_frDownDisableUV;

    XYRECT m_rectCursorPosition;

    long m_nCompareWidth;
    long m_nPrintLeftOffset;

    bool m_bBackRectangle;
    uint32_t m_dwBackColor;
    XYRECT m_rBorderOffset;
    long m_nUpRectOffset;

    char *m_sScrollerName;

    std::vector<std::string> m_asSyncNodes;

    bool m_bUseOneStringAdding;

    bool m_bFrized;

    // void    SetCurLine(long nNewCurLine);
    void SetCurLine(STRING_DESCRIBER *pNewCurLine);
    void ScrollerUpdate();
};
