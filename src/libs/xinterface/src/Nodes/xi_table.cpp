#include "xi_table.h"
#include "strutils.h"
#include "xi_scroller.h"
#include "xi_util.h"
#include <stdio.h>

#define ALIGN_BOTTOM 16
#define ALIGN_TOP 17
#define NOTUSE_OFFSET -1000.f

XI_TableLineDescribe::XI_TableLineDescribe(CXI_TABLE *pTable)
{
    m_pTable = pTable;
    m_nRowIndex = -1;
    m_bUseSpecColor = false;
    if (m_pTable && m_pTable->m_bVariableLineHeight)
        m_nHeight = m_pTable->m_nNormalLineHeight;
    else
        m_nHeight = 0;
}

XI_TableLineDescribe::~XI_TableLineDescribe()
{
    // m_aCell.DelAllWithPointers();
    for (const auto &cell : m_aCell)
        delete cell;

    m_pTable = nullptr;
}

void XI_TableLineDescribe::Draw(float fTop)
{
    auto fX = static_cast<float>(m_pTable->m_rect.left);
    for (long n = 0; n < m_aCell.size(); n++)
    {
        m_aCell[n]->Draw(fX, fTop);
        fX += m_pTable->m_anColsWidth[n];
    }
}

void XI_TableLineDescribe::DrawSpecColor(float fTop) const
{
    if (m_bUseSpecColor)
    {
        XI_NOTEX_VERTEX v[4];
        v[0].color = v[1].color = v[2].color = v[3].color = m_dwSpecColor;
        v[0].pos.z = v[1].pos.z = v[2].pos.z = v[3].pos.z = 1.f;
        const auto fBottom = fTop + static_cast<float>(m_pTable->m_anRowsHeights[m_nRowIndex]);
        const auto fLeft = static_cast<float>(m_pTable->m_rect.left);
        const auto fRight = static_cast<float>(m_pTable->m_rect.right);
        v[0].pos.x = fLeft;
        v[0].pos.y = fTop;
        v[1].pos.x = fLeft;
        v[1].pos.y = fBottom;
        v[2].pos.x = fRight;
        v[2].pos.y = fTop;
        v[3].pos.x = fRight;
        v[3].pos.y = fBottom;
        m_pTable->m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_NOTEX_FVF, 2, v, sizeof(XI_NOTEX_VERTEX), "iRectangle");
    }
}

void XI_TableLineDescribe::SetData(long nRowIndex, ATTRIBUTES *pLA, bool bHeader)
{
    m_nRowIndex = nRowIndex;
    char pcAttrName[64];
    m_bUseSpecColor = false;
    uint32_t dwSpecColor = 0;
    if (pLA)
        dwSpecColor = pLA->GetAttributeAsDword("speccolor", 0);
    if (dwSpecColor != 0)
    {
        m_bUseSpecColor = true;
        m_dwSpecColor = dwSpecColor;
    }

    if (pLA && m_pTable->m_bVariableLineHeight)
    {
        m_nHeight = pLA->GetAttributeAsDword("height", m_pTable->m_nNormalLineHeight);
    }
    else
        m_nHeight = m_pTable->m_anRowsHeights[nRowIndex];

    long c;
    for (c = 0; c < 10000; c++)
    {
        sprintf_s(pcAttrName, "td%d", c + 1);
        auto *const pA = (pLA ? pLA->GetAttributeClass(pcAttrName) : nullptr);
        if (!pA && c >= m_pTable->m_nColQuantity)
            break;
        XI_TableCellDescribe *pTD = nullptr;
        if (c >= m_aCell.size())
        {
            pTD = new XI_TableCellDescribe(m_pTable, this);
            m_aCell.push_back(pTD);
        }
        else
        {
            pTD = m_aCell[c];
        }
        Assert(pTD);
        pTD->SetData(c, pA, bHeader);
    }
    while (c < m_aCell.size())
    {
        STORM_DELETE(m_aCell[c]);
        m_aCell.erase(m_aCell.begin() + c);
    }
}

long XI_TableLineDescribe::GetLineHeight() const
{
    if (!m_pTable)
        return 0;
    if (m_pTable->m_bVariableLineHeight)
        return m_nHeight;
    if (m_nRowIndex < 0 && m_nRowIndex >= m_pTable->m_anRowsHeights.size())
        return 0;
    return m_pTable->m_anRowsHeights[m_nRowIndex];
}

XI_TableCellDescribe::XI_TableCellDescribe(CXI_TABLE *pTable, XI_TableLineDescribe *pLine)
{
    m_pTable = pTable;
    m_pLine = pLine;
    m_nFontIndex = -1;
}

XI_TableCellDescribe::~XI_TableCellDescribe()
{
    m_aImage.clear();
    m_aStrings.clear();
    m_pLine = nullptr;
    m_pTable = nullptr;
}

void XI_TableCellDescribe::Draw(float fLeft, float fTop)
{
    long n;

    fLeft += m_pTable->m_pntSpaceSize.x;
    fTop += m_pTable->m_pntSpaceSize.y;
    if (m_aImage.size() > 0)
    {
        const auto nX = static_cast<long>(fLeft) + m_nLeftLineWidth;
        const auto nY = static_cast<long>(fTop) + m_nTopLineHeight;
        for (n = 0; n < m_aImage.size(); n++)
            if (m_aImage[n].pImage)
                m_aImage[n].pImage->Draw(nX + m_aImage[n].offset.x, nY + m_aImage[n].offset.y, IPType_LeftTop);
    }

    fLeft += m_TextOffset.x;
    fTop += m_TextOffset.y;
    auto fY = 0.f;
    for (n = 0; n < m_aStrings.size(); n++)
    {
        if (m_aStrings[n].offset.y != NOTUSE_OFFSET)
            fY = m_aStrings[n].offset.y;
        const auto fNewY = fY + m_pTable->m_rs->CharHeight(m_nFontID) * m_fScale;
        if (fNewY >= m_pLine->GetLineHeight())
            break; // no longer fits into the table

        CXI_UTILS::PrintTextIntoWindow(
            m_pTable->m_rs, m_nFontIndex < 0 ? m_nFontID : m_pTable->m_anFontList[m_nFontIndex], m_dwColor,
            PR_ALIGN_LEFT, true, m_fScale, m_pTable->m_screenSize.x, m_pTable->m_screenSize.y,
            static_cast<long>(fLeft + m_aStrings[n].offset.x), static_cast<long>(fTop + fY), m_aStrings[n].str.c_str(),
            static_cast<long>(fLeft), static_cast<long>(fTop), m_pTable->m_anColsWidth[m_nColIndex], 20);

        fY = fNewY;
    }
}

void XI_TableCellDescribe::SetData(long nColIndex, ATTRIBUTES *pAttr, bool bHeader)
{
    if (!pAttr)
    {
        // empty cell
        m_aStrings.clear();
        m_aImage.clear();
        return;
    }
    ATTRIBUTES *pA;
    const char *pcStr, *pcTmpStr;

    m_nColIndex = nColIndex;

    m_nLeftLineWidth = nColIndex == 0 ? m_pTable->m_nBorderWidth : m_pTable->m_nVLineWidth;
    m_nTopLineHeight = bHeader ? m_pTable->m_nHeaderLineHeight : m_pTable->m_nHLineHeight;

    // read the picture
    long nIconQuantity = 0;
    pA = pAttr->GetAttributeClass("icon");
    if (pA)
    {
        if (m_aImage.empty())
            m_aImage.push_back(ImgDescribe{});
        LoadImageParam(&m_aImage[nIconQuantity], pA);
        nIconQuantity++;
    }
    else
    {
        char tmpaname[16];
        while (true)
        {
            sprintf_s(tmpaname, sizeof(tmpaname), "icon%d", nIconQuantity + 1);
            pA = pAttr->GetAttributeClass(tmpaname);
            if (!pA)
                break;
            if (static_cast<long>(m_aImage.size()) <= nIconQuantity)
                m_aImage.push_back(ImgDescribe{});
            LoadImageParam(&m_aImage[nIconQuantity], pA);
            nIconQuantity++;
        }
    }
    // delete unnecessary pictures
    while (static_cast<long>(m_aImage.size()) > nIconQuantity)
        m_aImage.erase(m_aImage.begin() + nIconQuantity);

    // read the line
    m_dwColor =
        pAttr->GetAttributeAsDword("color", bHeader ? m_pTable->m_dwFontTitleColor : m_pTable->m_dwFontCellColor);
    m_fScale = pAttr->GetAttributeAsFloat("scale", bHeader ? m_pTable->m_fFontTitleScale : m_pTable->m_fFontCellScale);
    m_nFontID = bHeader ? m_pTable->m_nFontTitleID : m_pTable->m_nFontCellID;
    m_nFontIndex = pAttr->GetAttributeAsDword("fontidx", -1);
    if (m_nFontIndex < 0 || m_nFontIndex >= m_pTable->m_anFontList.size())
        m_nFontIndex = -1;
    m_nAlignment = bHeader ? m_pTable->m_nFontTitleAlignment : m_pTable->m_nFontCellAlignment;
    m_nVAlignment = bHeader ? m_pTable->m_nFontTitleVAlignment : m_pTable->m_nFontCellVAlignment;
    pcTmpStr = pAttr->GetAttribute("align");
    if (pcTmpStr)
    {
        if (_stricmp(pcTmpStr, "left") == 0)
            m_nAlignment = PR_ALIGN_LEFT;
        if (_stricmp(pcTmpStr, "center") == 0)
            m_nAlignment = PR_ALIGN_CENTER;
        if (_stricmp(pcTmpStr, "right") == 0)
            m_nAlignment = PR_ALIGN_RIGHT;
    }
    pcTmpStr = pAttr->GetAttribute("valign");
    if (pcTmpStr)
    {
        if (_stricmp(pcTmpStr, "top") == 0)
            m_nVAlignment = ALIGN_TOP;
        if (_stricmp(pcTmpStr, "center") == 0)
            m_nVAlignment = PR_ALIGN_CENTER;
        if (_stricmp(pcTmpStr, "bottom") == 0)
            m_nVAlignment = ALIGN_BOTTOM;
    }

    m_TextOffset.x = m_TextOffset.y = 0;
    if (pAttr->GetAttribute("textoffset"))
        sscanf(pAttr->GetAttribute("textoffset"), "%f,%f", &m_TextOffset.x, &m_TextOffset.y);
    m_TextOffset.y += m_nTopLineHeight;

    auto nWidth = m_pTable->m_anColsWidth[nColIndex] - 2 * m_pTable->m_pntSpaceSize.x - m_nLeftLineWidth -
                  static_cast<long>(m_TextOffset.x);
    if (nWidth <= 0)
    {
        nWidth = 32;
    }

    pcStr = pAttr->GetAttribute("str");
    std::vector<std::string> asStr;
    CXI_UTILS::SplitStringByWidth(pcStr, m_nFontID, m_fScale, nWidth, asStr);

    if (m_nVAlignment != ALIGN_TOP)
    {
        auto fVOffset =
            static_cast<float>(m_pLine->GetLineHeight() - 2 * m_pTable->m_pntSpaceSize.y - m_nTopLineHeight) -
            m_fScale * m_pTable->m_rs->CharHeight(m_nFontID) * asStr.size();
        if (m_nVAlignment == PR_ALIGN_CENTER)
            fVOffset *= .5f;
        m_TextOffset.y += fVOffset;
    }

    m_aStrings.clear();
    pA = pAttr->GetAttributeClass("textoffsets");
    for (long n = 0; n < asStr.size(); n++)
    {
        // m_aStrings.Add();
        // asStr[n].TrimRight();
        m_aStrings.push_back(StrDescribe{});
        TOREMOVE::rtrim(asStr[n]);

        m_aStrings[n].str = asStr[n];

        m_aStrings[n].offset.y = CXI_UTILS::GetByStrNumFromAttribute_Float(pA, "s", n + 1, NOTUSE_OFFSET);
        m_aStrings[n].offset.x = static_cast<float>(m_nLeftLineWidth);
        if (m_nAlignment == PR_ALIGN_CENTER)
            m_aStrings[n].offset.x += static_cast<float>(
                (nWidth - m_pTable->m_rs->StringWidth((char *)asStr[n].c_str(), m_nFontID, m_fScale)) / 2);
        else if (m_nAlignment == PR_ALIGN_RIGHT)
            m_aStrings[n].offset.x +=
                static_cast<float>(nWidth - m_pTable->m_rs->StringWidth((char *)asStr[n].c_str(), m_nFontID, m_fScale));
    }
}

void XI_TableCellDescribe::LoadImageParam(ImgDescribe *pImg, ATTRIBUTES *pA) const
{
    Assert(pImg && pA);
    const char *pcStr;
    long nW, nH, nImgAlign;

    if (!pImg->pImage)
    {
        pImg->pImage = new CXI_IMAGE;
        Assert(pImg->pImage);
    }

    if (pA->GetAttribute("group"))
    {
        pImg->pImage->LoadFromBase(pA->GetAttribute("group"), pA->GetAttribute("image"), true);
    }
    else
    {
        if (pA->GetAttribute("texture"))
            pImg->pImage->LoadFromFile(pA->GetAttribute("texture"));

        if (pA->GetAttribute("texturepointer"))
            pImg->pImage->SetPointerToTexture((IDirect3DTexture9 *)pA->GetAttributeAsPointer("texturepointer"));

        auto fL = 0.f, fT = 0.f, fR = 1.f, fB = 1.f;
        if (pA->GetAttribute("uv"))
            sscanf(pA->GetAttribute("uv"), "%f,%f,%f,%f", &fL, &fT, &fR, &fB);
        pImg->pImage->SetUV(fL, fT, fR, fB);
    }
    nW = pA->GetAttributeAsDword("width", 0);
    nH = pA->GetAttributeAsDword("height", 0);
    if (pA->GetAttributeAsDword("cellsize", 0) > 0)
    {
        nW = m_pTable->m_anColsWidth[m_nColIndex] - m_nLeftLineWidth;
        nH = m_pLine->GetLineHeight() - m_nTopLineHeight;
    }
    if (nW > 0 && nH > 0)
        pImg->pImage->SetSize(nW, nH);
    pImg->offset.x = pImg->offset.y = 0;
    if (pA->GetAttribute("offset"))
        sscanf(pA->GetAttribute("offset"), "%d,%d", &pImg->offset.x, &pImg->offset.y);
    if ((pcStr = pA->GetAttribute("valign")) != nullptr)
    {
        nImgAlign = ALIGN_TOP;
        if (_stricmp(pcStr, "center") == 0)
            nImgAlign = PR_ALIGN_CENTER;
        else if (_stricmp(pcStr, "bottom") == 0)
            nImgAlign = ALIGN_BOTTOM;
        if (nImgAlign != ALIGN_TOP)
        {
            nH = m_pLine->GetLineHeight() - 2 * m_pTable->m_pntSpaceSize.y - m_nTopLineHeight -
                 pImg->pImage->GetHeight();
            if (nImgAlign == PR_ALIGN_CENTER)
                nH /= 2;
            pImg->offset.y += nH;
        }
    }
}

CXI_TABLE::CXI_TABLE()
{
    m_rs = nullptr;
    m_bClickable = true;
    m_nNodeType = NODETYPE_TABLE;

    m_bBackPresent = false;

    m_idBorderTexture = -1;
    m_idBorderVBuf = -1;
    m_idBorderIBuf = -1;
    m_nBorderSubQ = 0;

    m_EditData.bAllEditable = true;
    m_EditData.bColsEditable = true;
    m_EditData.nEditableIndex = 0;

    m_nTopIndex = 0;

    m_SelectImg.DisableDraw(true);
    m_pHeader = nullptr;
    m_bDoColsSelect = false;

    m_bFirstFrame = true;
    m_bVariableLineHeight = false;
    m_nNormalLineHeight = 32;

    m_nFontCellID = -1;
    m_nFontTitleID = -1;
}

CXI_TABLE::~CXI_TABLE()
{
    ReleaseAll();
}

void CXI_TABLE::Draw(bool bSelected, uint32_t Delta_Time)
{
    m_bFirstFrame = false;

    // Drawing the background
    if (m_bBackPresent)
    {
        m_BackImg.Draw();
    }

    // rendering special colors
    auto fY = static_cast<float>(m_rect.top);
    if (m_pHeader)
    {
        m_pHeader->DrawSpecColor(fY);
        fY += m_anRowsHeights[0];
    }
    for (long n = 0; n < m_aLine.size(); n++)
    {
        m_aLine[n]->DrawSpecColor(fY);
        fY += m_anRowsHeights[n + (m_pHeader ? 1 : 0)];
    }

    // drawing selection of the selected row
    m_SelectImg.Draw();

    // Drawing the frame
    if (m_idBorderTexture != -1 && m_idBorderVBuf != -1 && m_idBorderIBuf != -1)
    {
        m_rs->TextureSet(0, m_idBorderTexture);
        m_rs->DrawBuffer(m_idBorderVBuf, sizeof(XI_ONETEX_VERTEX), m_idBorderIBuf, 0, m_nBorderSubQ * 4, 0,
                         m_nBorderSubQ * 2, "iIcon");
    }

    // Line output
    fY = static_cast<float>(m_rect.top);
    if (m_pHeader)
    {
        m_pHeader->Draw(fY);
        fY += m_anRowsHeights[0];
    }
    for (long n = 0; n < m_aLine.size(); n++)
    {
        m_aLine[n]->Draw(fY);
        fY += m_anRowsHeights[n + (m_pHeader ? 1 : 0)];
    }
}

bool CXI_TABLE::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                     XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    return true;
}

void CXI_TABLE::ReleaseAll()
{
    // release all rows
    // m_aLine.DelAllWithPointers();
    for (const auto &line : m_aLine)
        delete line;
    m_aLine.clear();
    STORM_DELETE(m_pHeader);

    // release back image
    m_bBackPresent = false;
    m_BackImg.Unload();

    // release border data
    PICTURE_TEXTURE_RELEASE(pPictureService, m_sBorderIconGroupName.c_str(), m_idBorderTexture);
    VERTEX_BUFFER_RELEASE(m_rs, m_idBorderVBuf);
    INDEX_BUFFER_RELEASE(m_rs, m_idBorderIBuf);

    // release fonts from list
    FONT_RELEASE(m_rs, m_nFontCellID);
    FONT_RELEASE(m_rs, m_nFontTitleID);
    for (long n = 0; n < m_anFontList.size(); n++)
    {
        FONT_RELEASE(m_rs, m_anFontList[n]);
    }
    m_anFontList.clear();

    m_nBorderSubQ = 0;
}

int CXI_TABLE::CommandExecute(int wActCode)
{
    if (m_bUse)
    {
        switch (wActCode)
        {
        case ACTION_ACTIVATE:
            if (m_nSelectIndex >= 0)
                core.Event("TableActivate", "sl", m_nodeName, m_nSelectIndex);
            break;

        case ACTION_MOUSEDBLCLICK:
            if (m_nSelectIndex >= 0)
                core.Event("TableActivate", "sl", m_nodeName, m_nSelectIndex);
            break;

        case ACTION_UPSTEP:
            if (m_nSelectIndex > 0)
            {
                m_nSelectIndex--;
                SetTopIndexForSelect(m_nSelectIndex);
                SelectLine(m_nSelectIndex);
            }
            break;

        case ACTION_SPEEDUP:
            if (m_nSelectIndex > 0)
            {
                m_nSelectIndex -= m_anRowsHeights.size() - (m_pHeader ? 1 : 0);
                if (m_nSelectIndex < 0)
                    m_nSelectIndex = 0;
                SetTopIndexForSelect(m_nSelectIndex);
                SelectLine(m_nSelectIndex);
            }
            break;

        case ACTION_DOWNSTEP:
            if (m_nSelectIndex < m_nLineQuantity - 1)
            {
                m_nSelectIndex++;
                SetTopIndexForSelect(m_nSelectIndex);
                SelectLine(m_nSelectIndex);
            }
            break;

        case ACTION_SPEEDDOWN:
            if (m_nSelectIndex < m_nLineQuantity - 1)
            {
                m_nSelectIndex += m_anRowsHeights.size() - (m_pHeader ? 1 : 0);
                if (m_nSelectIndex > m_nLineQuantity - 1)
                    m_nSelectIndex = m_nLineQuantity - 1;
                SetTopIndexForSelect(m_nSelectIndex);
                SelectLine(m_nSelectIndex);
            }
            break;

        case ACTION_LEFTSTEP:
        case ACTION_SPEEDLEFT:
            if (m_bDoColsSelect && m_nSelectIndex != -1 && m_nSelectColIndex > 0)
            {
                m_nSelectColIndex--;
                SelectCol(m_nSelectColIndex);
            }
            break;

        case ACTION_RIGHTSTEP:
        case ACTION_SPEEDRIGHT:
            if (m_bDoColsSelect && m_nSelectIndex != -1 &&
                m_nSelectColIndex < static_cast<long>(m_anColsWidth.size()) - 1)
            {
                m_nSelectColIndex++;
                SelectCol(m_nSelectColIndex);
            }
            break;
            // boal -->
        case ACTION_MOUSERCLICK: // just position the cursor
        {
            const auto n = GetLineByPoint(ptrOwner->GetMousePoint());
            if (n >= 0 && n <= m_nLineQuantity - m_nTopIndex - (m_pHeader ? 0 : 1))
            {
                const auto nCol = GetColByX(static_cast<long>(ptrOwner->GetMousePoint().x));
                if (m_bDoColsSelect)
                    SelectRow(n, nCol);
                else
                    SelectRow(n);
                // core.Event( "OnTableClick", "sll", m_nodeName, m_nSelectIndex, nCol+1 );
            }
        }
        break;
            // boal <--
        case ACTION_MOUSECLICK: {
            const auto n = GetLineByPoint(ptrOwner->GetMousePoint());
            if (n >= 0 && n <= m_nLineQuantity - m_nTopIndex - (m_pHeader ? 0 : 1))
            {
                const auto nCol = GetColByX(static_cast<long>(ptrOwner->GetMousePoint().x));
                if (m_bDoColsSelect)
                    SelectRow(n, nCol);
                else
                    SelectRow(n);
                // core.Event( "OnTableClick", "sll", m_nodeName, (m_pHeader?n:(n+1)), nCol+1 );
                core.Event("OnTableClick", "sll", m_nodeName, m_nSelectIndex, nCol + 1);
            }
        }
        break;
        }
    }
    return -1;
}

bool CXI_TABLE::IsClick(int buttonID, long xPos, long yPos)
{
    if (xPos >= m_rect.left && xPos <= m_rect.right && yPos >= m_rect.top && yPos <= m_rect.bottom && m_bClickable &&
        m_bSelected && m_bUse)
        return true;
    return false;
}

void CXI_TABLE::ChangePosition(XYRECT &rNewPos)
{
    long n, nWAdd, nHAdd;

    if (m_EditData.bAllEditable)
    {
        nWAdd = (rNewPos.right - rNewPos.left) - (m_rect.right - m_rect.left);
        nHAdd = (rNewPos.bottom - rNewPos.top) - (m_rect.bottom - m_rect.top);
        n = m_anColsWidth.size() - 1;
        if (n > 0 && nWAdd > 0)
            m_anColsWidth[n] += nWAdd;
        for (; n >= 0 && nWAdd < 0; n--)
        {
            m_anColsWidth[n] += nWAdd;
            if (m_anColsWidth[n] < 0)
            {
                nWAdd = m_anColsWidth[n];
                m_anColsWidth[n] = 0;
            }
            else
                nWAdd = 0;
        }
        n = m_anRowsHeights.size() - 1;
        if (n > 0 && nHAdd > 0)
            m_anRowsHeights[n] += nHAdd;
        for (; n >= 0 && nHAdd < 0; n--)
        {
            m_anRowsHeights[n] += nHAdd;
            if (m_anRowsHeights[n] < 0)
            {
                nHAdd = m_anRowsHeights[n];
                m_anRowsHeights[n] = 0;
            }
            else
                nHAdd = 0;
        }
        //
        m_rect.right = m_rect.left = rNewPos.left;
        for (n = 0; n < m_anColsWidth.size(); n++)
            m_rect.right += m_anColsWidth[n];
        m_rect.right += m_nBorderWidth;
        //
        m_rect.bottom = m_rect.top = rNewPos.top;
        for (n = 0; n < m_anRowsHeights.size(); n++)
            m_rect.bottom += m_anRowsHeights[n];
        m_rect.bottom += m_nBorderWidth;
    }

    else
    {
        if (m_EditData.bColsEditable)
        {
            nWAdd = rNewPos.right - m_rect.right;
            if (nWAdd == 0)
                nWAdd = rNewPos.left - m_rect.left;
            if (m_EditData.nEditableIndex < m_anColsWidth.size())
            {
                m_anColsWidth[m_EditData.nEditableIndex] += nWAdd;
                m_rect.right += nWAdd;
            }
        }
        else
        {
            nHAdd = rNewPos.bottom - m_rect.bottom;
            if (nHAdd == 0)
                nHAdd = rNewPos.top - m_rect.top;
            if (m_EditData.nEditableIndex < m_anRowsHeights.size())
            {
                m_anRowsHeights[m_EditData.nEditableIndex] += nHAdd;
                m_rect.bottom += nHAdd;
            }
        }
    }

    UpdateBorders();
    UpdateTableCells();
    m_BackImg.SetPosition(m_rect);
    UpdateSelectImage();
}

void CXI_TABLE::SaveParametersToIni()
{
    char pcWriteParam[2048];

    auto pIni = fio->OpenIniFile(ptrOwner->m_sDialogFileName.c_str());
    if (!pIni)
    {
        core.Trace("Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.c_str());
        return;
    }

    // save position
    sprintf_s(pcWriteParam, sizeof(pcWriteParam), "%d,%d,%d,%d", m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
    pIni->WriteString(m_nodeName, "position", pcWriteParam);

    // save cols width
    std::string sTmp;
    for (long n = 0; n < m_anColsWidth.size(); n++)
    {
        if (n > 0)
            sTmp += ",";
        sTmp += m_anColsWidth[n];
    }
    pIni->WriteString(m_nodeName, "colswidth", (char *)sTmp.c_str());

    // save rows height
    sTmp = "";
    for (long n = 0; n < m_anRowsHeights.size(); n++)
    {
        if (n > 0)
            sTmp += ",";
        sTmp += m_anRowsHeights[n];
    }
    pIni->WriteString(m_nodeName, "rowsheight", (char *)sTmp.c_str());
}

uint32_t CXI_TABLE::MessageProc(long msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // update rows in a table
        UpdateTableCells();
        break;
    case 1: // get the line number on which the mouse pointer is located (-1 - the pointer is out of bounds)
        return GetLineByPoint(ptrOwner->GetMousePoint());
        break;
    }

    return 0;
}

bool CXI_TABLE::GetInternalNameList(std::vector<std::string> &aStr)
{
    aStr.clear();
    aStr.push_back("all");
    for (long n = 0; n < m_nColQuantity; n++)
        aStr.push_back("col" + std::to_string(n + 1));

    for (long n = 0; n < m_nRowQuantity; n++)
        aStr.push_back("row" + std::to_string(n + 1));

    return true;
}

void CXI_TABLE::SetInternalName(std::string &sName)
{
    if (sName == "all")
    {
        m_EditData.bAllEditable = true;
    }
    else
    {
        m_EditData.bAllEditable = false;
        m_EditData.bColsEditable = (_strnicmp(sName.c_str(), "col", 3) == 0);
        m_EditData.nEditableIndex = atoi(&sName.c_str()[3]) - 1;
    }
}

void CXI_TABLE::ScrollerChanged(float fRelativeScrollPos)
{
    if (m_nLineQuantity <= 1)
        return; // the only one
    const long n = static_cast<long>(fRelativeScrollPos * (m_nLineQuantity - 1));
    if (n != m_nSelectIndex)
    {
        SetTopIndexForSelect(n);
        SelectLine(n);
    }
}

void CXI_TABLE::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    SetGlowCursor(false);

    char param[1024], pctmp[128];
    long n, nCommonWidth, nUsedQ;

    // font list
    if (ini1 && name1)
    {
        for (n = 0; n < 100; n++)
        {
            sprintf_s(pctmp, sizeof(pctmp), "fontlist%d", n + 1);
            if (!ini1->ReadString(name1, pctmp, param, sizeof(param), ""))
                break;
            m_anFontList.push_back(m_rs->LoadFont(param));
        }
    }

    if (ReadIniString(ini1, name1, ini2, name2, "fontcell", param, sizeof(param), ""))
        m_nFontCellID = m_rs->LoadFont(param);

    // cell font
    if (ReadIniString(ini1, name1, ini2, name2, "fontcell", param, sizeof(param), ""))
        m_nFontCellID = m_rs->LoadFont(param);
    m_dwFontCellColor = GetIniARGB(ini1, name1, ini2, name2, "fontcellcolor", ARGB(255, 255, 255, 255));
    m_fFontCellScale = GetIniFloat(ini1, name1, ini2, name2, "fontcellscale", 1.f);
    m_nFontCellAlignment = PR_ALIGN_LEFT;
    m_nFontCellVAlignment = ALIGN_TOP;
    if (ReadIniString(ini1, name1, ini2, name2, "fontcellalignment", param, sizeof(param), ""))
    {
        if (_stricmp(param, "center") == 0)
            m_nFontCellAlignment = PR_ALIGN_CENTER;
        if (_stricmp(param, "right") == 0)
            m_nFontCellAlignment = PR_ALIGN_RIGHT;
    }
    if (ReadIniString(ini1, name1, ini2, name2, "fontcellvalignment", param, sizeof(param), ""))
    {
        if (_stricmp(param, "center") == 0)
            m_nFontCellVAlignment = PR_ALIGN_CENTER;
        if (_stricmp(param, "bottom") == 0)
            m_nFontCellVAlignment = ALIGN_BOTTOM;
    }

    // title font
    if (ReadIniString(ini1, name1, ini2, name2, "fonttitle", param, sizeof(param), ""))
        m_nFontTitleID = m_rs->LoadFont(param);
    m_dwFontTitleColor = GetIniARGB(ini1, name1, ini2, name2, "fonttitlecolor", ARGB(255, 255, 255, 255));
    m_fFontTitleScale = GetIniFloat(ini1, name1, ini2, name2, "fonttitlescale", 1.f);
    m_nFontTitleAlignment = PR_ALIGN_LEFT;
    m_nFontTitleVAlignment = ALIGN_TOP;
    if (ReadIniString(ini1, name1, ini2, name2, "fonttitlealignment", param, sizeof(param), ""))
    {
        if (_stricmp(param, "center") == 0)
            m_nFontTitleAlignment = PR_ALIGN_CENTER;
        if (_stricmp(param, "right") == 0)
            m_nFontTitleAlignment = PR_ALIGN_RIGHT;
    }
    if (ReadIniString(ini1, name1, ini2, name2, "fonttitlevalignment", param, sizeof(param), ""))
    {
        if (_stricmp(param, "center") == 0)
            m_nFontTitleVAlignment = PR_ALIGN_CENTER;
        if (_stricmp(param, "bottom") == 0)
            m_nFontTitleVAlignment = ALIGN_BOTTOM;
    }

    // select image
    if (ReadIniString(ini1, name1, ini2, name2, "selectimage", param, sizeof(param), ""))
    {
        m_SelectImg.LoadAccordingToString(param);
    }
    m_bDoColsSelect = GetIniBool(ini1, name1, ini2, name2, "iscolselect", false);
    if (m_bDoColsSelect)
        m_nSelectColIndex = 0;

    // border
    if (ReadIniString(ini1, name1, ini2, name2, "bordericongroup", param, sizeof(param), ""))
    {
        m_sBorderIconGroupName = param;
        m_idBorderTexture = pPictureService->GetTextureID(param);
    }
    //
    m_nBorderIcon_LeftTop = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "borderlefttop", param, sizeof(param), ""))
        m_nBorderIcon_LeftTop = pPictureService->GetImageNum(m_sBorderIconGroupName.c_str(), param);
    //
    m_nBorderIcon_LeftBottom = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "borderleftbottom", param, sizeof(param), ""))
        m_nBorderIcon_LeftBottom = pPictureService->GetImageNum(m_sBorderIconGroupName.c_str(), param);
    //
    m_nBorderIcon_RightTop = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "borderrighttop", param, sizeof(param), ""))
        m_nBorderIcon_RightTop = pPictureService->GetImageNum(m_sBorderIconGroupName.c_str(), param);
    //
    m_nBorderIcon_RightBottom = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "borderrightbottom", param, sizeof(param), ""))
        m_nBorderIcon_RightBottom = pPictureService->GetImageNum(m_sBorderIconGroupName.c_str(), param);
    //
    m_nBorderIcon_Left = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "borderleft", param, sizeof(param), ""))
        m_nBorderIcon_Left = pPictureService->GetImageNum(m_sBorderIconGroupName.c_str(), param);
    //
    m_nBorderIcon_Right = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "borderright", param, sizeof(param), ""))
        m_nBorderIcon_Right = pPictureService->GetImageNum(m_sBorderIconGroupName.c_str(), param);
    //
    m_nBorderIcon_Top = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "bordertop", param, sizeof(param), ""))
        m_nBorderIcon_Top = pPictureService->GetImageNum(m_sBorderIconGroupName.c_str(), param);
    //
    m_nBorderIcon_Bottom = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "borderbottom", param, sizeof(param), ""))
        m_nBorderIcon_Bottom = pPictureService->GetImageNum(m_sBorderIconGroupName.c_str(), param);
    //
    m_nBorderIcon_VLine = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "bordervline", param, sizeof(param), ""))
        m_nBorderIcon_VLine = pPictureService->GetImageNum(m_sBorderIconGroupName.c_str(), param);
    //
    m_nBorderIcon_HLine = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "borderhline", param, sizeof(param), ""))
        m_nBorderIcon_HLine = pPictureService->GetImageNum(m_sBorderIconGroupName.c_str(), param);

    m_dwBorderColor = GetIniARGB(ini1, name1, ini2, name2, "bordercolor", ARGB(255, 128, 128, 128));
    m_nBorderWidth = GetIniLong(ini1, name1, ini2, name2, "borderwidth", 0);
    m_nVLineWidth = GetIniLong(ini1, name1, ini2, name2, "vlinewidth", 0);
    m_nHLineHeight = GetIniLong(ini1, name1, ini2, name2, "hlineheight", 0);
    m_nHeaderLineHeight = GetIniLong(ini1, name1, ini2, name2, "headerlineheight", 0);
    m_bHLineIsBreakable = GetIniBool(ini1, name1, ini2, name2, "hlineisbreakable", true);
    m_pntBorderCornerSize.x = m_pntBorderCornerSize.y = 9;
    m_pntBorderCornerSize = GetIniLongPoint(ini1, name1, ini2, name2, "bordercornersize", m_pntBorderCornerSize);
    m_pntSpaceSize.x = m_pntSpaceSize.y = 4;
    m_pntSpaceSize = GetIniLongPoint(ini1, name1, ini2, name2, "cellspacesize", m_pntSpaceSize);

    m_nRowQuantity = GetIniLong(ini1, name1, ini2, name2, "rowquantity", 1);
    m_nColQuantity = GetIniLong(ini1, name1, ini2, name2, "colquantity", 1);

    m_bVariableLineHeight = GetIniBool(ini1, name1, ini2, name2, "IsVariableLineHeight", false);
    m_nNormalLineHeight = GetIniLong(ini1, name1, ini2, name2, "NormalLineHeight", 32);

    nUsedQ = 0;       // hard-coded strings
    nCommonWidth = 0; // the total height of these lines
    m_anRowsHeights.clear();
    if (ReadIniString(ini1, name1, ini2, name2, "rowsheight", param, sizeof(param), ""))
    {
        const char *pcTmp = param;
        char pcTmpBuf[256];
        while (pcTmp && pcTmp[0])
        {
            pcTmp = GetSubStr(pcTmp, pcTmpBuf, sizeof(pcTmpBuf));
            long nTmp = atol(pcTmpBuf);
            if (nTmp <= 0)
                nTmp = 0;
            else
                nUsedQ++;
            nCommonWidth += nTmp;
            m_anRowsHeights.push_back(nTmp);
        }
    }
    for (n = m_anRowsHeights.size(); n < m_nRowQuantity; n++)
        m_anRowsHeights.push_back(0);
    if (m_nRowQuantity > nUsedQ && m_rect.bottom - m_rect.top > nCommonWidth)
    {
        const long nH = (m_rect.bottom - m_rect.top - nCommonWidth) / (m_nRowQuantity - nUsedQ);
        for (n = 0; n < m_anRowsHeights.size(); n++)
            if (m_anRowsHeights[n] == 0)
                m_anRowsHeights[n] = nH;
    }
    // bring the height of the table to the total value
    m_rect.bottom = m_rect.top + m_nBorderWidth;
    for (n = 0; n < m_anRowsHeights.size(); n++)
    {
        m_rect.bottom += m_anRowsHeights[n];
    }

    // filling columns
    nUsedQ = 0;       // hard-coded columns
    nCommonWidth = 0; // the total width of these columns
    m_anColsWidth.clear();
    // read from INI
    if (ReadIniString(ini1, name1, ini2, name2, "colswidth", param, sizeof(param), ""))
    {
        const char *pcTmp = param;
        char pcTmpBuf[256];
        while (pcTmp && pcTmp[0])
        {
            pcTmp = GetSubStr(pcTmp, pcTmpBuf, sizeof(pcTmpBuf));
            long nTmp = atol(pcTmpBuf);
            if (nTmp <= 0)
                nTmp = 0; // width 0 - default
            else
                nUsedQ++; // otherwise this value is hardcoded
            nCommonWidth += nTmp;
            m_anColsWidth.push_back(nTmp);
        }
    }
    for (n = m_anColsWidth.size(); n < m_nColQuantity; n++)
        m_anColsWidth.push_back(0);
    if (m_nColQuantity > nUsedQ && m_rect.right - m_rect.left > nCommonWidth)
    {
        const long nW = (m_rect.right - m_rect.left - m_nBorderWidth - nCommonWidth) / (m_nColQuantity - nUsedQ);
        for (n = 0; n < m_anColsWidth.size(); n++)
            if (m_anColsWidth[n] == 0)
                m_anColsWidth[n] = nW;
    }
    // change the width of the table to the total value
    m_rect.right = m_rect.left + m_nBorderWidth;
    for (n = 0; n < m_anColsWidth.size(); n++)
    {
        m_rect.right += m_anColsWidth[n];
    }

    // back
    m_bBackPresent = false;
    if (ReadIniString(ini1, name1, ini2, name2, "backimage", param, sizeof(param), ""))
    {
        m_bBackPresent = true;
        m_BackImg.LoadAccordingToString(param);
        m_BackImg.SetPosition(m_rect);
    }

    if (ReadIniString(ini1, name1, ini2, name2, "scroller", param, sizeof(param), ""))
        m_sScrollerName = param;
    m_nSelectIndex = 0;

    UpdateBorders();

    // read table rows
    UpdateTableCells();
    SelectLine(0);
}

void CXI_TABLE::UpdateBorders()
{
    long n, q, r, c, nTop, nLeft;

    // counting the number of lines in a frame
    q = 0;
    if (m_nHeaderLineHeight > 0 && m_nRowQuantity > 0) // horizontal line under the heading
        q += 1;
    if (m_nHLineHeight > 0 && m_nRowQuantity > 2) // horizontal lines
        q += m_nRowQuantity - 2;
    if (m_bHLineIsBreakable)
        q *= m_nColQuantity;                     // the break line is split by the number of columns
    if (m_nVLineWidth > 0 && m_nColQuantity > 1) // vertical lines
        q += m_nColQuantity - 1;
    q += 4 + 4; // 4 corners and lines on the sides

    if (m_nBorderSubQ != q) // rearranging buffers
    {
        m_nBorderSubQ = q;
        VERTEX_BUFFER_RELEASE(m_rs, m_idBorderVBuf);
        INDEX_BUFFER_RELEASE(m_rs, m_idBorderIBuf);

        // index buffer
        m_idBorderIBuf = m_rs->CreateIndexBuffer(q * 6 * sizeof(uint16_t));
        Assert(m_idBorderIBuf != -1);
        // fill in
        auto *pT = static_cast<uint16_t *>(m_rs->LockIndexBuffer(m_idBorderIBuf));
        for (n = 0; n < q; n++)
        {
            pT[n * 6 + 0] = static_cast<uint16_t>(n * 4 + 0);
            pT[n * 6 + 1] = static_cast<uint16_t>(n * 4 + 1);
            pT[n * 6 + 2] = static_cast<uint16_t>(n * 4 + 2);
            pT[n * 6 + 3] = static_cast<uint16_t>(n * 4 + 3);
            pT[n * 6 + 4] = static_cast<uint16_t>(n * 4 + 1);
            pT[n * 6 + 5] = static_cast<uint16_t>(n * 4 + 2);
        }
        m_rs->UnLockIndexBuffer(m_idBorderIBuf);

        // vertex buffer
        m_idBorderVBuf = m_rs->CreateVertexBuffer(XI_ONETEX_FVF, q * 4 * sizeof(XI_ONETEX_VERTEX), D3DUSAGE_WRITEONLY);
        Assert(m_idBorderVBuf != -1);
    }

    // fill the vertex buffer
    auto *pV = static_cast<XI_ONETEX_VERTEX *>(m_rs->LockVertexBuffer(m_idBorderVBuf));
    // horizontal lines
    nTop = m_rect.top;
    for (r = 0, n = 0; r < m_nRowQuantity - 1; r++)
    {
        nTop += m_anRowsHeights[r];
        nLeft = m_rect.left;
        q = (r == 0 ? m_nHeaderLineHeight : m_nHLineHeight);
        if (q > 0)
        {
            if (m_bHLineIsBreakable)
            {
                // separate lines for each column
                for (c = 0; c < m_nColQuantity; c++)
                {
                    WriteSquare(&pV[n], m_nBorderIcon_HLine, m_dwBorderColor, nLeft, nTop, m_anColsWidth[c], q);
                    nLeft += m_anColsWidth[c];
                    n += 4;
                }
            } // one line for all columns
            else
            {
                WriteSquare(&pV[n], m_nBorderIcon_HLine, m_dwBorderColor, m_rect.left + m_nBorderWidth, nTop,
                            m_rect.right - m_rect.left - 2 * m_nBorderWidth, q);
                n += 4;
            }
        }
    }
    // vertical lines
    if (m_nVLineWidth > 0)
    {
        nLeft = m_rect.left;
        for (c = 0; c < m_nColQuantity - 1; c++)
        {
            nLeft += m_anColsWidth[c];
            WriteSquare(&pV[n], m_nBorderIcon_VLine, m_dwBorderColor, nLeft, m_rect.top + m_nBorderWidth, m_nVLineWidth,
                        m_rect.bottom - m_rect.top - 2 * m_nBorderWidth);
            n += 4;
        }
    }
    // main border
    WriteSquare(&pV[n], m_nBorderIcon_LeftTop, m_dwBorderColor, m_rect.left, m_rect.top, m_pntBorderCornerSize.x,
                m_pntBorderCornerSize.y);
    n += 4;
    WriteSquare(&pV[n], m_nBorderIcon_RightTop, m_dwBorderColor, m_rect.right - m_pntBorderCornerSize.x, m_rect.top,
                m_pntBorderCornerSize.x, m_pntBorderCornerSize.y);
    n += 4;
    WriteSquare(&pV[n], m_nBorderIcon_LeftBottom, m_dwBorderColor, m_rect.left, m_rect.bottom - m_pntBorderCornerSize.y,
                m_pntBorderCornerSize.x, m_pntBorderCornerSize.y);
    n += 4;
    WriteSquare(&pV[n], m_nBorderIcon_RightBottom, m_dwBorderColor, m_rect.right - m_pntBorderCornerSize.x,
                m_rect.bottom - m_pntBorderCornerSize.y, m_pntBorderCornerSize.x, m_pntBorderCornerSize.y);
    n += 4;
    WriteSquare(&pV[n], m_nBorderIcon_Top, m_dwBorderColor, m_rect.left + m_pntBorderCornerSize.x, m_rect.top,
                m_rect.right - m_rect.left - 2 * m_pntBorderCornerSize.x, m_nBorderWidth);
    n += 4;
    WriteSquare(&pV[n], m_nBorderIcon_Bottom, m_dwBorderColor, m_rect.left + m_pntBorderCornerSize.x,
                m_rect.bottom - m_nBorderWidth, m_rect.right - m_rect.left - 2 * m_pntBorderCornerSize.x,
                m_nBorderWidth);
    n += 4;
    WriteSquare(&pV[n], m_nBorderIcon_Left, m_dwBorderColor, m_rect.left, m_rect.top + m_pntBorderCornerSize.y,
                m_nBorderWidth, m_rect.bottom - m_rect.top - 2 * m_pntBorderCornerSize.y);
    n += 4;
    WriteSquare(&pV[n], m_nBorderIcon_Right, m_dwBorderColor, m_rect.right - m_nBorderWidth,
                m_rect.top + m_pntBorderCornerSize.y, m_nBorderWidth,
                m_rect.bottom - m_rect.top - 2 * m_pntBorderCornerSize.y);
    n += 4;
    // finish
    m_rs->UnLockVertexBuffer(m_idBorderVBuf);
}

void CXI_TABLE::WriteSquare(XI_ONETEX_VERTEX *pV, long nImgID, uint32_t dwCol, long nX, long nY, long nW, long nH) const
{
    if (!pV)
        return;

    FXYRECT uv;
    pPictureService->GetTexturePos(nImgID, uv);

    pV[0].color = dwCol;
    pV[0].pos.x = static_cast<float>(nX);
    pV[0].pos.y = static_cast<float>(nY);
    pV[0].pos.z = 1.f;
    pV[0].tu = uv.left;
    pV[0].tv = uv.top;

    pV[1].color = dwCol;
    pV[1].pos.x = static_cast<float>(nX);
    pV[1].pos.y = static_cast<float>(nY + nH);
    pV[1].pos.z = 1.f;
    pV[1].tu = uv.left;
    pV[1].tv = uv.bottom;

    pV[2].color = dwCol;
    pV[2].pos.x = static_cast<float>(nX + nW);
    pV[2].pos.y = static_cast<float>(nY);
    pV[2].pos.z = 1.f;
    pV[2].tu = uv.right;
    pV[2].tv = uv.top;

    pV[3].color = dwCol;
    pV[3].pos.x = static_cast<float>(nX + nW);
    pV[3].pos.y = static_cast<float>(nY + nH);
    pV[3].pos.z = 1.f;
    pV[3].tu = uv.right;
    pV[3].tv = uv.bottom;
}

void CXI_TABLE::UpdateTableCells()
{
    // Set new current image
    ATTRIBUTES *pARoot = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
    if (!pARoot)
    {
        m_nLineQuantity = 0;
        return;
    }

    long r, q;
    char pcTmp[64];

    long nY = m_rect.top;

    long nNewSel = pARoot->GetAttributeAsDword("select", m_nSelectIndex + 1) - 1;

    // read lines from attributes
    m_nTopIndex = pARoot->GetAttributeAsDword("top", 0); // first index of the attribute being read
    // first check for header
    ATTRIBUTES *pAttr = pARoot->GetAttributeClass("hr");
    if (pAttr)
    {
        if (!m_pHeader)
            m_pHeader = new XI_TableLineDescribe(this);
        Assert(m_pHeader);
        m_pHeader->SetData(0, pAttr, true);
        nY += m_anRowsHeights[0];
    }
    else
    {
        STORM_DELETE(m_pHeader);
    }
    // then
    q = m_bVariableLineHeight ? 1000 : (m_nRowQuantity - (m_pHeader ? 1 : 0));
    for (r = 0; (nY < m_rect.bottom) && (r < q); r++)
    {
        sprintf_s(pcTmp, "tr%d", r + m_nTopIndex + 1);
        pAttr = pARoot->GetAttributeClass(pcTmp);
        if (!pAttr)
            break;
        XI_TableLineDescribe *pTL = nullptr;
        if (r >= m_aLine.size())
        {
            pTL = new XI_TableLineDescribe(this);
            m_aLine.push_back(pTL);
        }
        else
            pTL = m_aLine[r];
        Assert(pTL);
        pTL->SetData(r + (m_pHeader ? 1 : 0), pAttr, false);
        nY += pTL->GetLineHeight();
    }
    // delete extra lines
    while (static_cast<long>(m_aLine.size()) > r)
    {
        STORM_DELETE(m_aLine[r]);
        m_aLine.erase(m_aLine.begin() + r);
    }

    // if the height of the lines is variable, then recalculate it
    if (m_bVariableLineHeight)
    {
        RecalculateLineHeights();
        UpdateSelectImage();
    }

    UpdateLineQuantity();
    if (m_bVariableLineHeight)
        UpdateBorders();
    if (nNewSel >= m_nLineQuantity)
        nNewSel = m_nLineQuantity - 1;
    if (nNewSel != m_nSelectIndex)
    {
        m_nSelectIndex = nNewSel;
        UpdateSelectImage();
    }
}

long CXI_TABLE::GetLineByPoint(const FXYPOINT &pnt)
{
    if (pnt.x < m_rect.left || pnt.x > m_rect.right || pnt.y < m_rect.top || pnt.y > m_rect.bottom)
        return -1;
    long nTop = m_rect.top;
    for (long n = 0; n < m_anRowsHeights.size(); n++)
    {
        nTop += m_anRowsHeights[n];
        if (pnt.y < nTop)
            return n;
    }
    return -1;
}

long CXI_TABLE::GetColByX(long x)
{
    x -= m_rect.left;
    if (x < 0)
        return -1;
    for (long n = 0; n < m_anColsWidth.size(); n++)
    {
        if (m_anColsWidth[n] >= x)
            return n;
        x -= m_anColsWidth[n];
    }
    return -1;
}

void CXI_TABLE::SelectRow(long nRowNum)
{
    if (nRowNum < (m_pHeader ? 1 : 0) || nRowNum >= m_anRowsHeights.size())
        return;
    const long n = m_nTopIndex + nRowNum - (m_pHeader ? 1 : 0);
    SelectLine(n);
}

void CXI_TABLE::SelectRow(long nRowNum, long nColNum)
{
    if (m_bDoColsSelect)
    {
        if (nColNum < 0 || nColNum >= m_anColsWidth.size())
            m_nSelectColIndex = -1;
        else
            m_nSelectColIndex = nColNum;
    }
    if (nRowNum < (m_pHeader ? 1 : 0) || nRowNum >= m_anRowsHeights.size())
        return;
    const long n = m_nTopIndex + nRowNum - (m_pHeader ? 1 : 0);
    SelectLine(n);
}

void CXI_TABLE::SelectLine(long nLineNum)
{
    if (nLineNum < 0)
        m_nSelectIndex = -1;
    else
        m_nSelectIndex = nLineNum;
    UpdateSelectImage();
    UpdateScroller();
    if (!m_bFirstFrame)
        core.Event("TableSelectChange", "sll", m_nodeName, m_nSelectIndex + 1, m_nSelectColIndex + 1);
}

void CXI_TABLE::SelectCol(long nColNum)
{
    if (!m_bDoColsSelect)
        return;
    if (nColNum < 0 || nColNum >= m_anColsWidth.size())
        m_nSelectColIndex = -1;
    else
        m_nSelectColIndex = nColNum;
    UpdateSelectImage();
    if (!m_bFirstFrame)
        core.Event("TableSelectChange", "sll", m_nodeName, m_nSelectIndex + 1, m_nSelectColIndex + 1);
}

void CXI_TABLE::UpdateSelectImage()
{
    const long nRow = m_nSelectIndex - m_nTopIndex + (m_pHeader ? 1 : 0);
    if (m_nSelectIndex < 0 || nRow < 0 || nRow >= m_anRowsHeights.size())
    {
        m_SelectImg.DisableDraw(true);
    }
    else
    {
        const long nCol = m_nSelectColIndex;
        m_SelectImg.DisableDraw(false);
        XYRECT pos;
        pos.top = GetRowTop(nRow);
        pos.bottom = pos.top + m_anRowsHeights[nRow];
        if (m_bDoColsSelect && nCol >= 0 && nCol < m_anColsWidth.size())
        {
            pos.left = GetColLeft(nCol);
            pos.right = pos.left + m_anColsWidth[nCol];
        }
        else
        {
            pos.left = m_rect.left;
            pos.right = m_rect.right;
        }
        m_SelectImg.SetPosition(pos);
    }
    ATTRIBUTES *pA = ptrOwner->AttributesPointer->GetAttributeClass(m_nodeName);
    if (!pA)
        pA = ptrOwner->AttributesPointer->CreateSubAClass(ptrOwner->AttributesPointer, m_nodeName);
    if (pA)
    {
        pA->SetAttributeUseDword("select", m_nSelectIndex + 1);
        pA->SetAttributeUseDword("selectcol", m_nSelectColIndex + 1);
    }
}

long CXI_TABLE::GetRowTop(long nRow)
{
    if (nRow < 0 || nRow >= m_anRowsHeights.size())
        return m_rect.top; // error situation
    long nTop = m_rect.top;
    for (long n = 0; n < nRow; n++)
    {
        nTop += m_anRowsHeights[n];
    }
    return nTop;
}

long CXI_TABLE::GetColLeft(long nCol)
{
    if (nCol < 0 || nCol >= m_anColsWidth.size())
        return m_rect.left; // error situation
    long nLeft = m_rect.left;
    for (long n = 0; n < nCol; n++)
    {
        nLeft += m_anColsWidth[n];
    }
    return nLeft;
}

void CXI_TABLE::SetTopIndexForSelect(long nSelIndex)
{
    if (nSelIndex < 0)
        return; // error situation

    if (nSelIndex < m_nTopIndex) // to the top - then put the selection on top
    {
        SetTopIndex(nSelIndex);
        UpdateTableCells();
    }
    else if (nSelIndex >= m_nTopIndex + static_cast<long>(m_anRowsHeights.size()) - (m_pHeader ? 1 : 0))
    // after the last line - then put the selection down
    {
        nSelIndex = nSelIndex - m_anRowsHeights.size() + (m_pHeader ? 1 : 0) + 1;
        if (nSelIndex < 0)
            nSelIndex = 0;
        SetTopIndex(nSelIndex);
        UpdateTableCells();
    }
}

void CXI_TABLE::UpdateLineQuantity()
{
    m_nLineQuantity = 0;
    ATTRIBUTES *pARoot = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
    if (!pARoot)
        return;

    if (!pARoot->GetAttributeClass("tr1"))
        return; // no one

    long nmin = 1;
    char pcAttrName[64];

    // find the minimum element
    while (true)
    {
        sprintf_s(pcAttrName, "tr%d", nmin * 2);
        if (pARoot->GetAttributeClass(pcAttrName))
            nmin *= 2;
        else
            break; // the next element of the binary tree does not exist
    }

    // binary search between min and max indices
    long nmax = nmin * 2;
    while (true)
    {
        const long n = (nmin + nmax) / 2;
        if (n == nmin)
            break;
        sprintf_s(pcAttrName, "tr%d", n);
        if (pARoot->GetAttributeClass(pcAttrName)) // there is an element - it means that the older half can have an end
        {
            nmin = n;
        }
        else // otherwise the end is somewhere in the first half
        {
            nmax = n;
        }
    }

    m_nLineQuantity = nmin;
}

void CXI_TABLE::SetTopIndex(long nTopIndex)
{
    m_nTopIndex = nTopIndex;
    ATTRIBUTES *pA = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
    if (pA)
        pA->SetAttributeUseDword("top", nTopIndex);
}

void CXI_TABLE::UpdateScroller() const
{
    if (m_sScrollerName.empty())
        return;
    CINODE *pNode = ptrOwner->FindNode(m_sScrollerName.c_str(), nullptr);
    if (!pNode || pNode->m_nNodeType != NODETYPE_SCROLLER)
        return;
    auto *pScroll = static_cast<CXI_SCROLLER *>(pNode);

    if (m_nLineQuantity <= 1)
        pScroll->SetRollerPos(0.f);
    else
        pScroll->SetRollerPos(static_cast<float>(m_nSelectIndex) / static_cast<float>(m_nLineQuantity - 1));
}

void CXI_TABLE::RecalculateLineHeights()
{
    long nY = m_rect.top; // current top of line
    long n = 0;           // index in the array of heights

    // header accounting
    if (m_pHeader)
        nY += m_anRowsHeights[n++];

    // take the actual sizes of the lines
    for (long i = 0; i < m_aLine.size(); i++, n++)
    {
        if (n < m_anRowsHeights.size())
            m_anRowsHeights[n] = m_aLine[i]->GetLineHeight();
        else
            m_anRowsHeights.push_back(m_aLine[i]->GetLineHeight());
        nY += m_aLine[i]->GetLineHeight();
    }

    // if the last line goes beyond the dimensions, then shorten it in height
    if (nY > m_rect.bottom && m_aLine.size() > 0)
    {
        // long nLine = m_aLine.Last();
        m_aLine.back()->SetLineHeight(m_aLine.back()->GetLineHeight() - (nY - m_rect.bottom));
        nY = m_rect.bottom;
    }

    // the missing lines are set to the standard height
    for (; nY < m_rect.bottom; n++)
    {
        long nH = m_nNormalLineHeight;
        if (nY + nH > m_rect.bottom)
            nH = m_rect.bottom - nY;
        if (n < m_anRowsHeights.size())
            m_anRowsHeights[n] = nH;
        else
            m_anRowsHeights.push_back(nH);
        nY += nH;
    }

    // delete the remaining lines
    while (n < m_anRowsHeights.size())
        m_anRowsHeights.erase(m_anRowsHeights.begin() + n);
}
