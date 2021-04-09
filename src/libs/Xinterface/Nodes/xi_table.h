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
    void SetData(long nColIndex, ATTRIBUTES *pAttr, bool bHeader);
    void LoadImageParam(ImgDescribe *pImg, ATTRIBUTES *pA) const;

  protected:
    CXI_TABLE *m_pTable;
    XI_TableLineDescribe *m_pLine;

    FPOINT m_TextOffset;
    long m_nFontID;
    long m_nFontIndex;
    uint32_t m_dwColor;
    float m_fScale;
    long m_nAlignment;
    long m_nVAlignment;

    long m_nLeftLineWidth;
    long m_nTopLineHeight;

    std::vector<StrDescribe> m_aStrings;
    std::vector<ImgDescribe> m_aImage;

  public:
    long m_nColIndex;
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
    void SetData(long nRowIndex, ATTRIBUTES *pLA, bool bHeader);

    long GetLineHeight() const;
    void SetLineHeight(long nHeight)
    {
        m_nHeight = nHeight;
    }

  protected:
    CXI_TABLE *m_pTable;
    std::vector<XI_TableCellDescribe *> m_aCell;

    bool m_bUseSpecColor;
    uint32_t m_dwSpecColor;
    long m_nHeight;

  public:
    long m_nRowIndex;
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
    ~CXI_TABLE();

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
    void WriteSquare(XI_ONETEX_VERTEX *pV, long nImgID, uint32_t dwCol, long nX, long nY, long nW, long nH) const;
    void UpdateTableCells();
    long GetLineByPoint(const FXYPOINT &pnt);
    long GetColByX(long x);
    void SelectRow(long nRowNum);
    void SelectRow(long nRowNum, long nColNum);
    void SelectLine(long nLineNum);
    void SelectCol(long nColNum);
    long GetRowTop(long nRow);
    long GetColLeft(long nCol);
    void SetTopIndexForSelect(long nSelIndex);
    void UpdateSelectImage();
    void UpdateLineQuantity();
    void SetTopIndex(long nTopIndex);
    void UpdateScroller() const;
    void RecalculateLineHeights();

    bool m_bFirstFrame;

    bool m_bVariableLineHeight;
    long m_nNormalLineHeight;

    std::vector<XI_TableLineDescribe *> m_aLine;
    XI_TableLineDescribe *m_pHeader;

    std::vector<long> m_anFontList;

    long m_nFontCellID;
    uint32_t m_dwFontCellColor;
    float m_fFontCellScale;
    long m_nFontCellAlignment;
    long m_nFontCellVAlignment;

    long m_nFontTitleID;
    uint32_t m_dwFontTitleColor;
    float m_fFontTitleScale;
    long m_nFontTitleAlignment;
    long m_nFontTitleVAlignment;

    // back describe
    bool m_bBackPresent;
    CXI_IMAGE m_BackImg;
    CXI_IMAGE m_SelectImg;
    bool m_bDoColsSelect;
    long m_nSelectColIndex;

    std::string m_sBorderIconGroupName;
    long m_idBorderTexture;
    long m_idBorderVBuf;
    long m_idBorderIBuf;
    long m_nBorderSubQ;

    long m_nBorderIcon_LeftTop;
    long m_nBorderIcon_LeftBottom;
    long m_nBorderIcon_RightTop;
    long m_nBorderIcon_RightBottom;
    long m_nBorderIcon_Left;
    long m_nBorderIcon_Right;
    long m_nBorderIcon_Top;
    long m_nBorderIcon_Bottom;
    long m_nBorderIcon_VLine;
    long m_nBorderIcon_HLine;

    uint32_t m_dwBorderColor;
    long m_nBorderWidth;
    long m_nVLineWidth;
    long m_nHLineHeight;
    long m_nHeaderLineHeight;
    bool m_bHLineIsBreakable;
    XYPOINT m_pntBorderCornerSize;
    XYPOINT m_pntSpaceSize;

    long m_nRowQuantity;
    long m_nColQuantity;
    std::vector<long> m_anRowsHeights;
    std::vector<long> m_anColsWidth;

    struct EditModeDescribe
    {
        bool bAllEditable;
        bool bColsEditable;
        long nEditableIndex;
    };

    EditModeDescribe m_EditData;

    long m_nTopIndex;
    long m_nSelectIndex;

    long m_nLineQuantity;

    std::string m_sScrollerName;
};
