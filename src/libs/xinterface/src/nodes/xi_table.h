#pragma once

#include "../inode.h"
#include "../xdefines.h"
#include "xi_image.h"

class CXI_TABLE;
class XI_TableLineDescribe;

class XI_TableCellDescribe
{
    struct StrDescribe
    {
        std::string str;
        FPOINT offset;
    };

    struct ImgDescribe
    {
        CXI_IMAGE *pImage;
        POINT offset;

        ImgDescribe()
        {
            pImage = nullptr;
            offset.x = offset.y = 0;
        }

        ~ImgDescribe()
        {
            STORM_DELETE(pImage);
        }
    };

  public:
    XI_TableCellDescribe(CXI_TABLE *pTable, XI_TableLineDescribe *pLine);
    ~XI_TableCellDescribe();

    void SetOwners(CXI_TABLE *pTable, XI_TableLineDescribe *pLine)
    {
        m_pTable = pTable;
        m_pLine = pLine;
    }

    void Draw(float fLeft, float fTop);
    void SetData(int32_t nColIndex, ATTRIBUTES *pAttr, bool bHeader);
    void LoadImageParam(ImgDescribe *pImg, ATTRIBUTES *pA) const;

  protected:
    CXI_TABLE *m_pTable;
    XI_TableLineDescribe *m_pLine;

    FPOINT m_TextOffset;
    int32_t m_nFontID;
    int32_t m_nFontIndex;
    uint32_t m_dwColor;
    float m_fScale;
    int32_t m_nAlignment;
    int32_t m_nVAlignment;
    float line_space_modifier;

    int32_t m_nLeftLineWidth;
    int32_t m_nTopLineHeight;

    std::vector<StrDescribe> m_aStrings;
    std::vector<ImgDescribe> m_aImage;

  public:
    int32_t m_nColIndex;
};

class XI_TableLineDescribe
{
  public:
    XI_TableLineDescribe(CXI_TABLE *pTable);
    ~XI_TableLineDescribe();

    void SetOwners(CXI_TABLE *pTable)
    {
        m_pTable = pTable;
    }

    void Draw(float fTop);
    void DrawSpecColor(float fTop) const;
    void SetData(int32_t nRowIndex, ATTRIBUTES *pLA, bool bHeader);

    int32_t GetLineHeight() const;

    void SetLineHeight(int32_t nHeight)
    {
        m_nHeight = nHeight;
    }

  protected:
    CXI_TABLE *m_pTable;
    std::vector<XI_TableCellDescribe *> m_aCell;

    bool m_bUseSpecColor;
    uint32_t m_dwSpecColor;
    int32_t m_nHeight;

  public:
    int32_t m_nRowIndex;
};

// picture
class CXI_TABLE : public CINODE
{
    friend XI_TableLineDescribe;
    friend XI_TableCellDescribe;

  public:
    CXI_TABLE(CXI_TABLE &&) = delete;
    CXI_TABLE(const CXI_TABLE &) = delete;
    CXI_TABLE();
    ~CXI_TABLE() override;

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

    bool GetInternalNameList(std::vector<std::string> &aStr) override;
    void SetInternalName(std::string &sName) override;

    void ScrollerChanged(float fRelativeScrollPos);

    float GetLineStep() const
    {
        if (m_nLineQuantity <= 0)
            return 0.f;
        if (m_nLineQuantity > 1)
            return 1.f / static_cast<float>(m_nLineQuantity - 1);
        return 1.f;
    }

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void UpdateBorders();
    void WriteSquare(XI_ONETEX_VERTEX *pV, int32_t nImgID, uint32_t dwCol, int32_t nX, int32_t nY, int32_t nW, int32_t nH) const;
    void UpdateTableCells();
    int32_t GetLineByPoint(const FXYPOINT &pnt);
    int32_t GetColByX(int32_t x);
    void SelectRow(int32_t nRowNum);
    void SelectRow(int32_t nRowNum, int32_t nColNum);
    void SelectLine(int32_t nLineNum);
    void SelectCol(int32_t nColNum);
    int32_t GetRowTop(int32_t nRow);
    int32_t GetColLeft(int32_t nCol);
    void SetTopIndexForSelect(int32_t nSelIndex);
    void UpdateSelectImage();
    void UpdateLineQuantity();
    void SetTopIndex(int32_t nTopIndex);
    void UpdateScroller() const;
    void RecalculateLineHeights();

    bool m_bFirstFrame;

    bool m_bVariableLineHeight;
    int32_t m_nNormalLineHeight;

    std::vector<XI_TableLineDescribe *> m_aLine;
    XI_TableLineDescribe *m_pHeader;

    std::vector<int32_t> m_anFontList;

    int32_t m_nFontCellID;
    uint32_t m_dwFontCellColor;
    float m_fFontCellScale;
    int32_t m_nFontCellAlignment;
    int32_t m_nFontCellVAlignment;

    int32_t m_nFontTitleID;
    uint32_t m_dwFontTitleColor;
    float m_fFontTitleScale;
    int32_t m_nFontTitleAlignment;
    int32_t m_nFontTitleVAlignment;

    // back describe
    bool m_bBackPresent;
    CXI_IMAGE m_BackImg;
    CXI_IMAGE m_SelectImg;
    bool m_bDoColsSelect;
    int32_t m_nSelectColIndex;

    std::string m_sBorderIconGroupName;
    int32_t m_idBorderTexture;
    int32_t m_idBorderVBuf;
    int32_t m_idBorderIBuf;
    int32_t m_nBorderSubQ;

    int32_t m_nBorderIcon_LeftTop;
    int32_t m_nBorderIcon_LeftBottom;
    int32_t m_nBorderIcon_RightTop;
    int32_t m_nBorderIcon_RightBottom;
    int32_t m_nBorderIcon_Left;
    int32_t m_nBorderIcon_Right;
    int32_t m_nBorderIcon_Top;
    int32_t m_nBorderIcon_Bottom;
    int32_t m_nBorderIcon_VLine;
    int32_t m_nBorderIcon_HLine;

    uint32_t m_dwBorderColor;
    int32_t m_nBorderWidth;
    int32_t m_nVLineWidth;
    int32_t m_nHLineHeight;
    int32_t m_nHeaderLineHeight;
    bool m_bHLineIsBreakable;
    XYPOINT m_pntBorderCornerSize;
    XYPOINT m_pntSpaceSize;

    int32_t m_nRowQuantity;
    int32_t m_nColQuantity;
    std::vector<int32_t> m_anRowsHeights;
    std::vector<int32_t> m_anColsWidth;

    struct EditModeDescribe
    {
        bool bAllEditable;
        bool bColsEditable;
        int32_t nEditableIndex;
    };

    EditModeDescribe m_EditData;

    int32_t m_nTopIndex;
    int32_t m_nSelectIndex;

    int32_t m_nLineQuantity;

    std::string m_sScrollerName;
};
