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
    bool IsClick(int buttonID, int32_t xPos, int32_t yPos) override;
    void MouseThis(float fX, float fY) override;
    void ChangePosition(XYRECT &rNewPos) override;
    void SaveParametersToIni() override;
    uint32_t MessageProc(int32_t msgcode, MESSAGE &message) override;
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

    int32_t GetAllHeight();

  protected:
    bool GetLineNext(int fontNum, const char *&pInStr, char *buf, int bufSize) const;
    void GetOneLine(int fontNum, const char *pStr, char *buf, int bufSize) const;
    void MakeTagChecking(bool &tagState, uint32_t &tagColor, uint32_t normColor, STRING_DESCRIBER *pStrDescr);
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void ReleaseString(STRING_DESCRIBER *pCur);
    void ReleaseStringes();
    int32_t AddFormatedText(const char *str);
    int32_t FindUpGroup(int32_t grNum);
    int32_t FindDownGroup(int32_t grNum);
    void SetVertexToNewGroup(bool bUpDirect, int32_t upIdx, int32_t downIdx);
    void CheckScrollButtons();
    void SetSpecialStrings(ATTRIBUTES *pARoot);
    void ControlSyncronouseNodes();
    int32_t GetFirstGroupNum();
    void SetCurrentGroupNum(int32_t nFirstNum, int32_t nSelectNum);
    void ReplaceString(int32_t nGrpNum, const char *pSrcStr);
    void InsertStringBefore(STRING_DESCRIBER *pNextDescr, const char *pSrcStr, int32_t nGrpNum, uint32_t dwColor);
    void RecalculateStringNumber();
    void VAlignment(int32_t nAlign);
    void RefreshAlignment();

  protected:
    int32_t m_idFont;
    uint32_t m_dwColor;
    float m_fFontScale;
    int32_t m_nAlignment;

    int m_allStrings;
    int m_vertOffset;
    int32_t m_leftOffset;
    int32_t m_nVAlignmentOffset;

    STRING_DESCRIBER *m_listRoot;
    STRING_DESCRIBER *m_listCur;
    int m_nAllTextStrings;
    int m_nStringGroupQuantity;

    bool m_bSelectableCursor;
    CVideoTexture *m_pVidTex;
    int32_t m_idVBuf;
    int32_t m_nCurGroupNum;
    float m_fHorzScale;

    bool m_bUpEnable;
    bool m_bDownEnable;
    int32_t m_idUpEnableTexture;
    int32_t m_idUpDisableTexture;
    int32_t m_idDownEnableTexture;
    int32_t m_idDownDisableTexture;
    XYRECT m_frUpPos;
    XYRECT m_frDownPos;
    FXYRECT m_frUpEnableUV;
    FXYRECT m_frUpDisableUV;
    FXYRECT m_frDownEnableUV;
    FXYRECT m_frDownDisableUV;

    XYRECT m_rectCursorPosition;

    int32_t m_nCompareWidth;
    int32_t m_nPrintLeftOffset;

    bool m_bBackRectangle;
    uint32_t m_dwBackColor;
    XYRECT m_rBorderOffset;
    int32_t m_nUpRectOffset;

    char *m_sScrollerName;

    std::vector<std::string> m_asSyncNodes;

    bool m_bUseOneStringAdding;

    bool m_bFrized;

    // void    SetCurLine(int32_t nNewCurLine);
    void SetCurLine(STRING_DESCRIBER *pNewCurLine);
    void ScrollerUpdate();
};
