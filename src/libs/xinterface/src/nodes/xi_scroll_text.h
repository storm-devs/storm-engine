#pragma once

#include "../inode.h"

#define PICE_TYPE_STRING 0      // string descriptor
#define PICE_TYPE_FONTCHANGE 1  // new font descriptor
#define PICE_TYPE_COLORCHANGE 2 // new color descriptor
#define PICE_TYPE_NEXTLINE 3    // linefeed specifier
#define PICE_TYPE_NEXTTABLINE 4 // linefeed specifier of the new paragraph

struct STRING_PICE
{
    int32_t type;

    union {
        struct
        {
            int32_t startOffset;
            int32_t charQuantity;
        } strDescr;

        int32_t fontID;
        uint32_t color;
    } data;
};

class CXI_SCROLLTEXT : public CINODE
{
  public:
    CXI_SCROLLTEXT();
    ~CXI_SCROLLTEXT() override;

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

    void SetText(const char *newText);

  protected:
    void ClearText();
    int32_t FillPices(char *pt, size_t beg, size_t size, int32_t &idx, STRING_PICE *spl, int32_t wid);

    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;

    void TextDownShift(float);
    void TextUpShift();

  protected:
    CINODE *m_pScroller; // link to the scroller (!!! must be registered beforehand)
    int32_t m_nMaxStringes; // the number of lines in the displayed window

    uint32_t m_dwFontColor; // current color
    int32_t m_idFont;          // current font

    char *m_pText;
    int32_t m_nPiceQuantity;
    STRING_PICE *m_pStrList;

    int32_t GetStringWord(char *pstr, char *buff, size_t size);
};
