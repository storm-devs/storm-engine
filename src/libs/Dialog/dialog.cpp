#include "dialog.h"
#include "../SoundService/VSoundService.h"
#include "defines.h"

#define CNORMAL 0xFFFFFFFF
#define UNFADE_TIME 1000

#define DIALOG_BOTTOM_LINESPACE 12
#define DIALOG_TOP_LINESPACE 12

INTERFACE_FUNCTION
CREATE_CLASS(DIALOG)

VDX9RENDER *DIALOG::RenderService = nullptr;
FRECT DIALOG::m_frScreenData;

inline void SetVerticesForSquare(XI_TEX_VERTEX *pV, FRECT uv, float left, float top, float right, float bottom)
{
    pV[0].pos.x = left;
    pV[0].pos.y = top;
    pV[0].pos.z = 1.f;
    pV[0].rhw = 0.5f;
    pV[0].color = 0xFFFFFFFF;
    pV[0].u = uv.left;
    pV[0].v = uv.top;

    pV[1].pos.x = right;
    pV[1].pos.y = top;
    pV[1].pos.z = 1.f;
    pV[1].rhw = 0.5f;
    pV[1].color = 0xFFFFFFFF;
    pV[1].u = uv.right;
    pV[1].v = uv.top;

    pV[2].pos.x = left;
    pV[2].pos.y = bottom;
    pV[2].pos.z = 1.f;
    pV[2].rhw = 0.5f;
    pV[2].color = 0xFFFFFFFF;
    pV[2].u = uv.left;
    pV[2].v = uv.bottom;

    pV[3].pos.x = right;
    pV[3].pos.y = bottom;
    pV[2].pos.z = 1.f;
    pV[3].rhw = 0.5f;
    pV[3].color = 0xFFFFFFFF;
    pV[3].u = uv.right;
    pV[3].v = uv.bottom;
}

void DIALOG::DlgTextDescribe::ChangeText(const char *pcText)
{
    asText.clear();
    anPageEndIndex.clear();
    if (!pcText)
        return;

    long i = 0;
    for (long n = 0; pcText[n] != 0; n++)
    {
        if (pcText[n] == '\\' && (pcText[n + 1] == 'n' || pcText[n + 1] == 'N'))
        {
            auto *const pcTmp = new char[4 + n - i];
            Assert(pcTmp);
            if (n - i > 0)
                memcpy(pcTmp, &pcText[i], (n - i) * sizeof(char));
            memcpy(&pcTmp[n - i], "...", 4 * sizeof(char));
            i = n + 2;
            n++;
            AddToStringArrayLimitedByWidth(pcTmp, nFontID, fScale, nWindowWidth, asText, &anPageEndIndex,
                                           nShowQuantity);
            if (anPageEndIndex.size() == 0 || anPageEndIndex[anPageEndIndex.size() - 1] != asText.size())
                anPageEndIndex.push_back(asText.size());
            delete[] pcTmp;
        }
    }
    AddToStringArrayLimitedByWidth(&pcText[i], nFontID, fScale, nWindowWidth, asText, &anPageEndIndex, nShowQuantity);
    //	if( anPageEndIndex.size()!=0 && anPageEndIndex[anPageEndIndex.size()-1]!=asText.size() )
    //		anPageEndIndex.Add( asText.size() );
    nStartIndex = 0;
    nSelectLine = -1;
}

void DIALOG::DlgTextDescribe::Init(VDX9RENDER *pRS, D3DVIEWPORT9 &vp, INIFILE *pIni)
{
    Assert(pRS);
    rs = pRS;

    offset.x = 20;
    offset.y = 0;
    nWindowWidth = vp.Width - 2 * offset.x;
    offset.x += vp.X;

    char FName[MAX_PATH];
    if (pIni)
        pIni->ReadString("DIALOG", "mainfont", FName, MAX_PATH, "DIALOG2");
    else
        strcpy_s(FName, "DIALOG2");
    nFontID = rs->LoadFont(FName);

    dwColor = ARGB(255, 210, 227, 227);
    dwColor = pIni ? pIni->GetLong("DIALOG", "mainFontColor", dwColor) : dwColor;
    fScale = GetScrHeight(pIni ? pIni->GetFloat("DIALOG", "mainFontScale", 1.f) : 1.f);
    nLineInterval = static_cast<long>(rs->CharHeight(nFontID) * fScale);

    nStartIndex = 0;
    nShowQuantity = 5;
    if (pIni)
        nShowQuantity = pIni->GetLong("DIALOG", "maxtextlines", nShowQuantity);
    nSelectLine = -1;
}

long DIALOG::DlgTextDescribe::GetShowHeight()
{
    long n;
    for (n = 0; n < anPageEndIndex.size(); n++)
        if (nStartIndex < anPageEndIndex[n])
            break;
    if (n < anPageEndIndex.size())
        n = anPageEndIndex[n] - nStartIndex;
    else
        n = asText.size() - nStartIndex;
    if (n > nShowQuantity)
        n = nShowQuantity;
    return (n * nLineInterval);
}

void DIALOG::DlgTextDescribe::Show(long nY)
{
    long n, i, y, nEnd;

    y = nY + offset.y;
    i = 0;
    for (n = 0; n < anPageEndIndex.size(); n++)
        if (anPageEndIndex[n] > nStartIndex)
            break;
    if (n < anPageEndIndex.size())
        nEnd = anPageEndIndex[n];
    else
        nEnd = asText.size();
    for (n = nStartIndex; i < nShowQuantity && n < nEnd; i++, n++)
    {
        rs->ExtPrint(nFontID, dwColor, 0, PR_ALIGN_LEFT, true, fScale, 0, 0, offset.x, y, "%s", asText[n].c_str());
        y += nLineInterval;
    }
}

bool DIALOG::DlgTextDescribe::IsLastPage()
{
    long n;
    for (n = 0; n < anPageEndIndex.size(); n++)
        if (anPageEndIndex[n] > nStartIndex)
            break;
    if (n >= anPageEndIndex.size() || anPageEndIndex[n] >= asText.size())
        return true;
    return false;
}

void DIALOG::DlgTextDescribe::PrevPage()
{
    long n;
    for (n = anPageEndIndex.size() - 1; n >= 0; n--)
        if (anPageEndIndex[n] < nStartIndex)
            break;
    if (n >= 0)
        nStartIndex = anPageEndIndex[n];
    else
        nStartIndex = 0;
}

void DIALOG::DlgTextDescribe::NextPage()
{
    long n;
    for (n = 0; n < anPageEndIndex.size(); n++)
        if (anPageEndIndex[n] > nStartIndex)
            break;
    if (n < anPageEndIndex.size() && anPageEndIndex[n] < asText.size())
        nStartIndex = anPageEndIndex[n];
}

void DIALOG::DlgLinkDescribe::ChangeText(ATTRIBUTES *pALinks)
{
    nEditLine = -1;
    asText.clear();
    anLineEndIndex.clear();
    if (!pALinks)
        return;

    for (long n = 0; n < static_cast<long>(pALinks->GetAttributesNum()); n++)
    {
        auto *pA = pALinks->GetAttributeClass(n);
        if (pA)
        {
            // long i = asText.Add();
            // asText[i] = pA->GetThisAttr();
            if (pA->GetAttribute("edit"))
            {
                nEditLine = n;
                nEditVarIndex = pA->GetAttributeAsDword("edit", 0);
                nEditCharIndex = 0;
            }
            AddToStringArrayLimitedByWidth(pA->GetThisAttr(), nFontID, fScale, nWindowWidth, asText, nullptr, 100);
            anLineEndIndex.push_back(asText.size());
        }
    }

    nStartIndex = 0;
    nSelectLine = 0;
    fCursorCurrentTime = 0.f;
}

void DIALOG::DlgLinkDescribe::Init(VDX9RENDER *pRS, D3DVIEWPORT9 &vp, INIFILE *pIni)
{
    Assert(pRS);
    rs = pRS;

    offset.x = 20;
    offset.y = 0;
    nWindowWidth = vp.Width - 2 * offset.x;
    offset.x += vp.X;

    char FName[MAX_PATH];
    if (pIni)
        pIni->ReadString("DIALOG", "subfont", FName, MAX_PATH, "DIALOG3");
    else
        strcpy_s(FName, "DIALOG3");
    nFontID = rs->LoadFont(FName);

    dwColor = 0xFF808080;
    dwSelColor = 0xFFFFFFFF;
    dwColor = pIni ? pIni->GetLong("DIALOG", "subFontColor", dwColor) : dwColor;
    dwSelColor = pIni ? pIni->GetLong("DIALOG", "subFontColorSelect", dwSelColor) : dwSelColor;
    fScale = GetScrHeight(pIni ? pIni->GetFloat("DIALOG", "subFontScale", 1.f) : 1.f);
    nLineInterval = static_cast<long>(rs->CharHeight(nFontID) * fScale * .9f);

    nStartIndex = 0;
    nShowQuantity = 5;
    if (pIni)
        nShowQuantity = pIni->GetLong("DIALOG", "maxlinkslines", nShowQuantity);
    nSelectLine = 0;

    nEditLine = -1;
    fCursorVisibleTime = 0.8f;
    fCursorInvisibleTime = 0.2f;
}

long DIALOG::DlgLinkDescribe::GetShowHeight()
{
    long n = asText.size() - nStartIndex;
    if (n > nShowQuantity)
        n = nShowQuantity;
    return (n * nLineInterval);
}

void DIALOG::DlgLinkDescribe::Show(long nY)
{
    long n, nBeg, nEnd, i, y;

    y = nY + offset.y;
    n = nStartIndex;
    nBeg = 0;
    if (nSelectLine < anLineEndIndex.size() && nSelectLine >= 0)
    {
        if (nSelectLine > 0)
            nBeg = anLineEndIndex[nSelectLine - 1];
        nEnd = anLineEndIndex[nSelectLine];
    }
    else
        nEnd = -1;
    for (i = 0; i < nShowQuantity && n < asText.size(); i++, n++)
    {
        rs->ExtPrint(nFontID, (n >= nBeg && n < nEnd) ? dwSelColor : dwColor, 0, PR_ALIGN_LEFT, true, fScale, 0, 0,
                     offset.x, y, "%s", asText[n].c_str());

        if (nEditLine != -1 && (n >= nBeg && n < nEnd) && nSelectLine == nEditLine)
        {
            if (pDlg)
                pDlg->bEditMode = true;
            ShowEditMode(offset.x, y, n);
        }
        else if (pDlg)
            pDlg->bEditMode = false;

        y += nLineInterval;
    }
}

void DIALOG::DlgLinkDescribe::ShowEditMode(long nX, long nY, long nTextIdx)
{
    const auto nKeyQ = api->Controls->GetKeyBufferLength();
    if (nKeyQ > 0)
    {
        const auto *const pKeys = api->Controls->GetKeyBuffer();
        if (pKeys)
        {
            for (long n = 0; n < nKeyQ; n++)
            {
                if (pKeys[n].bSystem)
                {
                    switch (pKeys[n].ucVKey)
                    {
                    case VK_LEFT:
                        if (nEditCharIndex > 0)
                            nEditCharIndex--;
                        break;
                    case VK_BACK:
                        if (nEditCharIndex > 0)
                        {
                            nEditCharIndex--;
                            asText[nTextIdx].erase(nEditCharIndex, 1);
                        }
                        break;
                    case VK_RIGHT:
                        if (nEditCharIndex < static_cast<long>(asText[nTextIdx].size()))
                            nEditCharIndex++;
                        break;
                    case VK_DELETE:
                        if (nEditCharIndex >= 0 && nEditCharIndex < static_cast<long>(asText[nTextIdx].size()))
                        {
                            asText[nTextIdx].erase(nEditCharIndex, 1);
                        }
                        break;
                    }
                    continue;
                }
                if (pKeys[n].ucVKey < 0x20)
                    continue;

                char pcTmp[2];
                pcTmp[0] = pKeys[n].ucVKey;
                pcTmp[1] = 0;
                if (rs->StringWidth((char *)asText[nTextIdx].c_str(), nFontID, fScale, 0) +
                        rs->CharWidth(pcTmp[0], nFontID, fScale) <=
                    nWindowWidth)
                {
                    asText[nTextIdx].insert(nEditCharIndex, pcTmp);
                    nEditCharIndex++;
                }
            }
        }
    }

    // show cursor
    fCursorCurrentTime += api->GetRDeltaTime() * 0.001f;
    if (fCursorCurrentTime > fCursorVisibleTime + fCursorInvisibleTime)
        fCursorCurrentTime -= fCursorVisibleTime + fCursorInvisibleTime;
    if (fCursorCurrentTime <= fCursorVisibleTime)
    {
        long nW = 0;
        if (!asText[nTextIdx].empty())
        {
            if (nEditCharIndex < static_cast<long>(asText[nTextIdx].size()))
            {
                const auto cTmp = asText[nTextIdx][nEditCharIndex];
                asText[nTextIdx][nEditCharIndex] = 0;
                nW = rs->StringWidth((char *)asText[nTextIdx].c_str(), nFontID, fScale, 0);
                asText[nTextIdx][nEditCharIndex] = cTmp;
            }
            else
                nW = rs->StringWidth((char *)asText[nTextIdx].c_str(), nFontID, fScale, 0);
        }
        rs->ExtPrint(nFontID, dwSelColor, 0, PR_ALIGN_LEFT, true, fScale, 0, 0, nX + nW, nY, "_");
    }

    if (nEditVarIndex >= 0 && nEditVarIndex < 10)
    {
        auto *pDat = static_cast<VDATA *>(api->GetScriptVariable("dialogEditStrings"));
        if (pDat)
            pDat->Set((char *)asText[nTextIdx].c_str(), nEditVarIndex);
    }
}

//--------------------------------------------------------------------
DIALOG::DIALOG()
{
    curSnd = 0;
    forceEmergencyClose = false;
    play = -1;
    start = true;

    RenderService = nullptr;

    m_idVBufBack = -1;
    m_idIBufBack = -1;
    m_idVBufButton = -1;
    m_idIBufButton = -1;

    m_dwButtonState = 0; // нет ни одной кнопки

    m_nCharNameTextFont = -1;
    m_dwCharNameTextColor = 0xFFFFFFFF;

    m_bDlgChanged = true;

    strcpy_s(charDefSnd, "\0");

    bEditMode = false;
    m_DlgLinks.SetDlg(this);
}

//--------------------------------------------------------------------
DIALOG::~DIALOG()
{
    api->SetTimeScale(1.f);

    if (m_idVBufBack != -1)
        RenderService->ReleaseVertexBuffer(m_idVBufBack);
    m_idVBufBack = -1;
    if (m_idIBufBack != -1)
        RenderService->ReleaseIndexBuffer(m_idIBufBack);
    m_idIBufBack = -1;

    if (m_idVBufButton != -1)
        RenderService->ReleaseVertexBuffer(m_idVBufButton);
    m_idVBufButton = -1;
    if (m_idIBufButton == -1)
        RenderService->ReleaseIndexBuffer(m_idIBufButton);
    m_idIBufButton = -1;

    m_DlgText.Release();
    m_DlgLinks.Release();

    if (m_nCharNameTextFont != -1)
        RenderService->UnloadFont(m_nCharNameTextFont);
    m_nCharNameTextFont = -1;
}

void DIALOG::CreateBack()
{
    const long nSquareQuantity = 9 + 3 + 1; // 9-for back, 3-for name & 1-for divider
    m_nIQntBack = 6 * nSquareQuantity;      // 6 индексов в одном прямоугольнике
    m_nVQntBack = 4 * nSquareQuantity;      // 4 вертекса в одном прямоуголькнике

    if (m_idVBufBack == -1)
        m_idVBufBack =
            RenderService->CreateVertexBuffer(XI_TEX_FVF, m_nVQntBack * sizeof(XI_TEX_VERTEX), D3DUSAGE_WRITEONLY);
    if (m_idIBufBack == -1)
        m_idIBufBack = RenderService->CreateIndexBuffer(m_nIQntBack * sizeof(uint16_t));

    auto *const pI = static_cast<uint16_t *>(RenderService->LockIndexBuffer(m_idIBufBack));
    if (pI)
    {
        for (long n = 0; n < nSquareQuantity; n++)
        {
            pI[n * 6 + 0] = static_cast<uint16_t>(n * 4 + 0);
            pI[n * 6 + 1] = static_cast<uint16_t>(n * 4 + 2);
            pI[n * 6 + 2] = static_cast<uint16_t>(n * 4 + 1);
            pI[n * 6 + 3] = static_cast<uint16_t>(n * 4 + 1);
            pI[n * 6 + 4] = static_cast<uint16_t>(n * 4 + 2);
            pI[n * 6 + 5] = static_cast<uint16_t>(n * 4 + 3);
        }
        RenderService->UnLockIndexBuffer(m_idIBufBack);
    }

    // создаем размеры
    m_BackParams.m_frBorderInt.left = m_BackParams.m_frBorderExt.left + m_BackParams.frBorderRect.left;
    m_BackParams.m_frBorderInt.right = m_BackParams.m_frBorderExt.right - m_BackParams.frBorderRect.right;
    m_BackParams.m_frBorderInt.top = m_BackParams.m_frBorderExt.top + m_BackParams.frBorderRect.top;
    m_BackParams.m_frBorderInt.bottom = m_BackParams.m_frBorderExt.bottom - m_BackParams.frBorderRect.bottom;
}

void DIALOG::FillBack()
{
    if (m_idVBufBack == -1)
        return;

    auto *pV = static_cast<XI_TEX_VERTEX *>(RenderService->LockVertexBuffer(m_idVBufBack));
    // center
    SetVerticesForSquare(&pV[0], m_BackParams.m_frCenterUV, m_BackParams.m_frBorderInt.left,
                         m_BackParams.m_frBorderInt.top, m_BackParams.m_frBorderInt.right,
                         m_BackParams.m_frBorderInt.bottom);
    // top
    SetVerticesForSquare(&pV[4], m_BackParams.m_frTopUV, m_BackParams.m_frBorderInt.left,
                         m_BackParams.m_frBorderExt.top, m_BackParams.m_frBorderInt.right,
                         m_BackParams.m_frBorderInt.top);
    // bottom
    SetVerticesForSquare(&pV[8], m_BackParams.m_frBottomUV, m_BackParams.m_frBorderInt.left,
                         m_BackParams.m_frBorderInt.bottom, m_BackParams.m_frBorderInt.right,
                         m_BackParams.m_frBorderExt.bottom);
    // left
    SetVerticesForSquare(&pV[12], m_BackParams.m_frLeftUV, m_BackParams.m_frBorderExt.left,
                         m_BackParams.m_frBorderInt.top, m_BackParams.m_frBorderInt.left,
                         m_BackParams.m_frBorderInt.bottom);
    // right
    SetVerticesForSquare(&pV[16], m_BackParams.m_frRightUV, m_BackParams.m_frBorderInt.right,
                         m_BackParams.m_frBorderInt.top, m_BackParams.m_frBorderExt.right,
                         m_BackParams.m_frBorderInt.bottom);
    // left top
    SetVerticesForSquare(&pV[20], m_BackParams.m_frLeftTopUV, m_BackParams.m_frBorderExt.left,
                         m_BackParams.m_frBorderExt.top, m_BackParams.m_frBorderInt.left,
                         m_BackParams.m_frBorderInt.top);
    // right top
    SetVerticesForSquare(&pV[24], m_BackParams.m_frRightTopUV, m_BackParams.m_frBorderInt.right,
                         m_BackParams.m_frBorderExt.top, m_BackParams.m_frBorderExt.right,
                         m_BackParams.m_frBorderInt.top);
    // left bottom
    SetVerticesForSquare(&pV[28], m_BackParams.m_frLeftBottomUV, m_BackParams.m_frBorderExt.left,
                         m_BackParams.m_frBorderInt.bottom, m_BackParams.m_frBorderInt.left,
                         m_BackParams.m_frBorderExt.bottom);
    // right bottom
    SetVerticesForSquare(&pV[32], m_BackParams.m_frRightBottomUV, m_BackParams.m_frBorderInt.right,
                         m_BackParams.m_frBorderInt.bottom, m_BackParams.m_frBorderExt.right,
                         m_BackParams.m_frBorderExt.bottom);

    FRECT frTL;
    frTL.top = m_BackParams.m_frBorderExt.top + m_BackParams.fpCharacterNameOffset.y;
    frTL.bottom = frTL.top + m_BackParams.fCharacterNameRectHeight;
    // left name rectangle
    frTL.left = m_BackParams.m_frBorderExt.left + m_BackParams.fpCharacterNameOffset.x;
    frTL.right = frTL.left + m_BackParams.fCharacterNameRectLeftWidth;
    SetVerticesForSquare(&pV[36], m_BackParams.frCharacterNameRectLeftUV, frTL.left, frTL.top, frTL.right, frTL.bottom);
    // medium name rectangle
    frTL.left = frTL.right;
    frTL.right += m_BackParams.fCharacterNameRectCenterWidth;
    SetVerticesForSquare(&pV[40], m_BackParams.frCharacterNameRectCenterUV, frTL.left, frTL.top, frTL.right,
                         frTL.bottom);
    // right name rectangle
    frTL.left = frTL.right;
    frTL.right += m_BackParams.fCharacterNameRectRightWidth;
    SetVerticesForSquare(&pV[44], m_BackParams.frCharacterNameRectRightUV, frTL.left, frTL.top, frTL.right,
                         frTL.bottom);

    RenderService->UnLockVertexBuffer(m_idVBufBack);
}

void DIALOG::FillDivider()
{
    if (m_idVBufBack == -1)
        return;
    if (!m_BackParams.bShowDivider)
        return;

    XI_TEX_VERTEX *pV = static_cast<XI_TEX_VERTEX *>(RenderService->LockVertexBuffer(m_idVBufBack));
    const auto fDividerY = static_cast<float>(textViewport.Y + m_BackParams.nDividerOffsetY);
    SetVerticesForSquare(&pV[m_nVQntBack - 4], m_BackParams.m_frDividerUV,
                         m_BackParams.m_frBorderInt.left + m_BackParams.nDividerOffsetX, fDividerY,
                         m_BackParams.m_frBorderInt.right - m_BackParams.nDividerOffsetX,
                         fDividerY + m_BackParams.nDividerHeight);
    RenderService->UnLockVertexBuffer(m_idVBufBack);
}

void DIALOG::DrawBack()
{
    RenderService->TextureSet(0, m_BackParams.m_idBackTex);
    if (m_BackParams.bShowDivider)
        RenderService->DrawBuffer(m_idVBufBack, sizeof(XI_TEX_VERTEX), m_idIBufBack, 0, m_nVQntBack, 0, m_nIQntBack / 3,
                                  "texturedialogfon");
    else
        RenderService->DrawBuffer(m_idVBufBack, sizeof(XI_TEX_VERTEX), m_idIBufBack, 0, m_nVQntBack - 4, 0,
                                  m_nIQntBack / 3 - 2, "texturedialogfon");
}

void DIALOG::CreateButtons()
{
    m_nIQntButton = 6 * 2; // 6 индексов в одном прямоугольнике
    m_nVQntButton = 4 * 2; // 4 вертекса на один прямоугольник

    if (m_idVBufButton == -1)
        m_idVBufButton =
            RenderService->CreateVertexBuffer(XI_TEX_FVF, m_nVQntButton * sizeof(XI_TEX_VERTEX), D3DUSAGE_WRITEONLY);
    if (m_idIBufButton == -1)
        m_idIBufButton = RenderService->CreateIndexBuffer(m_nIQntButton * sizeof(uint16_t));

    auto *pI = static_cast<uint16_t *>(RenderService->LockIndexBuffer(m_idIBufButton));
    if (pI)
    {
        for (long n = 0; n < 2; n++)
        {
            pI[n * 6 + 0] = static_cast<uint16_t>(n * 4 + 0);
            pI[n * 6 + 1] = static_cast<uint16_t>(n * 4 + 2);
            pI[n * 6 + 2] = static_cast<uint16_t>(n * 4 + 1);
            pI[n * 6 + 3] = static_cast<uint16_t>(n * 4 + 1);
            pI[n * 6 + 4] = static_cast<uint16_t>(n * 4 + 2);
            pI[n * 6 + 5] = static_cast<uint16_t>(n * 4 + 3);
        }
        RenderService->UnLockIndexBuffer(m_idIBufButton);
    }
}

void DIALOG::FillButtons()
{
    if (m_idVBufButton == -1)
        return;

    m_dwButtonState &= ~(BUTTON_STATE_UPENABLE | BUTTON_STATE_DOWNENABLE);

    if ((!m_DlgText.IsLastPage()) || (m_DlgLinks.nStartIndex + m_DlgLinks.nShowQuantity < m_DlgLinks.asText.size()))
        m_dwButtonState |= BUTTON_STATE_DOWNENABLE;

    if (m_DlgText.IsLastPage() && m_DlgLinks.nStartIndex > 0)
        m_dwButtonState |= BUTTON_STATE_UPENABLE;

    if (m_DlgText.nStartIndex > 0)
        m_dwButtonState |= BUTTON_STATE_UPENABLE;

    XI_TEX_VERTEX *pV = static_cast<XI_TEX_VERTEX *>(RenderService->LockVertexBuffer(m_idVBufButton));
    if (m_dwButtonState & BUTTON_STATE_UPLIGHT)
    {
        SetVerticesForSquare(
            &pV[0], m_ButtonParams.frUpLightButtonUV,
            m_BackParams.m_frBorderInt.right - m_ButtonParams.fRightOffset - m_ButtonParams.fpButtonSize.x,
            m_BackParams.m_frBorderInt.top + m_ButtonParams.fTopOffset,
            m_BackParams.m_frBorderInt.right - m_ButtonParams.fRightOffset,
            m_BackParams.m_frBorderInt.top + m_ButtonParams.fTopOffset + m_ButtonParams.fpButtonSize.y);
    }
    else
    {
        SetVerticesForSquare(
            &pV[0], m_ButtonParams.frUpNormalButtonUV,
            m_BackParams.m_frBorderInt.right - m_ButtonParams.fRightOffset - m_ButtonParams.fpButtonSize.x,
            m_BackParams.m_frBorderInt.top + m_ButtonParams.fTopOffset,
            m_BackParams.m_frBorderInt.right - m_ButtonParams.fRightOffset,
            m_BackParams.m_frBorderInt.top + m_ButtonParams.fTopOffset + m_ButtonParams.fpButtonSize.y);
    }

    if (m_dwButtonState & BUTTON_STATE_DOWNLIGHT)
    {
        SetVerticesForSquare(
            &pV[4], m_ButtonParams.frDownLightButtonUV,
            m_BackParams.m_frBorderInt.right - m_ButtonParams.fRightOffset - m_ButtonParams.fpButtonSize.x,
            m_BackParams.m_frBorderInt.bottom - m_ButtonParams.fBottomOffset - m_ButtonParams.fpButtonSize.y,
            m_BackParams.m_frBorderInt.right - m_ButtonParams.fRightOffset,
            m_BackParams.m_frBorderInt.bottom - m_ButtonParams.fBottomOffset);
    }
    else
    {
        SetVerticesForSquare(
            &pV[4], m_ButtonParams.frDownNormalButtonUV,
            m_BackParams.m_frBorderInt.right - m_ButtonParams.fRightOffset - m_ButtonParams.fpButtonSize.x,
            m_BackParams.m_frBorderInt.bottom - m_ButtonParams.fBottomOffset - m_ButtonParams.fpButtonSize.y,
            m_BackParams.m_frBorderInt.right - m_ButtonParams.fRightOffset,
            m_BackParams.m_frBorderInt.bottom - m_ButtonParams.fBottomOffset);
    }
    RenderService->UnLockVertexBuffer(m_idVBufButton);
}

void DIALOG::DrawButtons()
{
    RenderService->TextureSet(0, m_ButtonParams.m_idTexture);
    if (m_dwButtonState & BUTTON_STATE_UPENABLE)
    {
        if (m_dwButtonState & BUTTON_STATE_DOWNENABLE)
            RenderService->DrawBuffer(m_idVBufButton, sizeof(XI_TEX_VERTEX), m_idIBufButton, 0, m_nVQntButton, 0, 2 * 2,
                                      "texturedialogfon");
        else
            RenderService->DrawBuffer(m_idVBufButton, sizeof(XI_TEX_VERTEX), m_idIBufButton, 0, m_nVQntButton, 0, 2 * 1,
                                      "texturedialogfon");
    }
    else if (m_dwButtonState & BUTTON_STATE_DOWNENABLE)
        RenderService->DrawBuffer(m_idVBufButton, sizeof(XI_TEX_VERTEX), m_idIBufButton, 0, m_nVQntButton, 6, 2 * 1,
                                  "texturedialogfon");
}

void DIALOG::LoadFromIni()
{
    INIFILE *pIni = fio->OpenIniFile("resource\\ini\\dialog.ini");
    if (!pIni)
    {
        api->Trace("Warning! DIALOG: Can`t open ini file %s", "resource\\ini\\dialog.ini");
        return;
    }

    char param[512];
    pIni->ReadString("BACKPARAM", "texture", param, sizeof(param) - 1, "dialog\\interface.tga");
    m_BackParams.m_idBackTex = RenderService->TextureCreate(param);

    FPOINT fpScrSize, fpScrOffset;
    GetPointFromIni(pIni, "BACKPARAM", "baseScreenSize", fpScrSize);
    GetPointFromIni(pIni, "BACKPARAM", "baseScreenOffset", fpScrOffset);
    if (fpScrSize.x <= 0)
        fpScrSize.x = 800.f;
    if (fpScrSize.y <= 0)
        fpScrSize.y = 600.f;
    m_nScrBaseWidth = static_cast<long>(fpScrSize.x);
    m_nScrBaseHeight = static_cast<long>(fpScrSize.y);
    D3DVIEWPORT9 vp;
    RenderService->GetViewport(&vp);
    m_frScreenData.right = vp.Width / (fpScrSize.x + fpScrOffset.x);
    m_frScreenData.bottom = vp.Height / (fpScrSize.y + fpScrOffset.y);
    m_frScreenData.left = fpScrOffset.x / (fpScrSize.x + fpScrOffset.x);
    m_frScreenData.top = fpScrOffset.y / (fpScrSize.y + fpScrOffset.y);

    GetRectFromIni(pIni, "BACKPARAM", "uvLeftTop", m_BackParams.m_frLeftTopUV);
    GetRectFromIni(pIni, "BACKPARAM", "uvRightTop", m_BackParams.m_frRightTopUV);
    GetRectFromIni(pIni, "BACKPARAM", "uvLeftBottom", m_BackParams.m_frLeftBottomUV);
    GetRectFromIni(pIni, "BACKPARAM", "uvRightBottom", m_BackParams.m_frRightBottomUV);
    GetRectFromIni(pIni, "BACKPARAM", "uvLeft", m_BackParams.m_frLeftUV);
    GetRectFromIni(pIni, "BACKPARAM", "uvRight", m_BackParams.m_frRightUV);
    GetRectFromIni(pIni, "BACKPARAM", "uvTop", m_BackParams.m_frTopUV);
    GetRectFromIni(pIni, "BACKPARAM", "uvBottom", m_BackParams.m_frBottomUV);
    GetRectFromIni(pIni, "BACKPARAM", "uvCenter", m_BackParams.m_frCenterUV);
    GetRectFromIni(pIni, "BACKPARAM", "uvDivider", m_BackParams.m_frDividerUV);

    GetRectFromIni(pIni, "BACKPARAM", "uvChrNameLeft", m_BackParams.frCharacterNameRectLeftUV);
    GetRectFromIni(pIni, "BACKPARAM", "uvChrNameRight", m_BackParams.frCharacterNameRectRightUV);
    GetRectFromIni(pIni, "BACKPARAM", "uvChrNameCenter", m_BackParams.frCharacterNameRectCenterUV);
    GetPointFromIni(pIni, "BACKPARAM", "chrNameOffset", m_BackParams.fpCharacterNameOffset);
    m_BackParams.fpCharacterNameOffset.x = GetScrWidth(m_BackParams.fpCharacterNameOffset.x);
    m_BackParams.fpCharacterNameOffset.y = GetScrHeight(m_BackParams.fpCharacterNameOffset.y);
    m_BackParams.fCharacterNameRectHeight = GetScrHeight(pIni->GetFloat("BACKPARAM", "ChrNameHeight", 32.f));
    m_BackParams.fCharacterNameRectLeftWidth = GetScrWidth(pIni->GetFloat("BACKPARAM", "ChrNameLeftWidth", 16.f));
    m_BackParams.fCharacterNameRectCenterWidth = GetScrWidth(pIni->GetFloat("BACKPARAM", "ChrNameCenterWidth", 128.f));
    m_BackParams.fCharacterNameRectRightWidth = GetScrWidth(pIni->GetFloat("BACKPARAM", "ChrNameRightWidth", 16.f));

    m_BackParams.bShowDivider = false;
    m_BackParams.nDividerHeight = GetScrHeight(static_cast<float>(pIni->GetLong("BACKPARAM", "dividerHeight", 8)));
    m_BackParams.nDividerOffsetX = GetScrWidth(static_cast<float>(pIni->GetLong("BACKPARAM", "dividerOffsetX", 8)));
    m_BackParams.nDividerOffsetY = 0;

    GetRectFromIni(pIni, "BACKPARAM", "backBorderOffset", m_BackParams.frBorderRect);
    m_BackParams.frBorderRect.left = GetScrWidth(m_BackParams.frBorderRect.left);
    m_BackParams.frBorderRect.right = GetScrWidth(m_BackParams.frBorderRect.right);
    m_BackParams.frBorderRect.top = GetScrHeight(m_BackParams.frBorderRect.top);
    m_BackParams.frBorderRect.bottom = GetScrHeight(m_BackParams.frBorderRect.bottom);

    GetRectFromIni(pIni, "BACKPARAM", "backPosition", m_BackParams.m_frBorderExt);
    m_BackParams.m_frBorderExt.left = GetScrX(m_BackParams.m_frBorderExt.left);
    m_BackParams.m_frBorderExt.right = GetScrX(m_BackParams.m_frBorderExt.right);
    m_BackParams.m_frBorderExt.top = GetScrY(m_BackParams.m_frBorderExt.top);
    m_BackParams.m_frBorderExt.bottom = GetScrY(m_BackParams.m_frBorderExt.bottom);

    m_ButtonParams.m_idTexture = m_BackParams.m_idBackTex;
    GetRectFromIni(pIni, "BUTTON", "uvUpNormal", m_ButtonParams.frUpNormalButtonUV);
    GetRectFromIni(pIni, "BUTTON", "uvDownNormal", m_ButtonParams.frDownNormalButtonUV);
    GetRectFromIni(pIni, "BUTTON", "uvUpLight", m_ButtonParams.frUpLightButtonUV);
    GetRectFromIni(pIni, "BUTTON", "uvDownLight", m_ButtonParams.frDownLightButtonUV);
    GetPointFromIni(pIni, "BUTTON", "buttonSize", m_ButtonParams.fpButtonSize);
    m_ButtonParams.fRightOffset = GetScrWidth(pIni->GetFloat("BUTTON", "rightoffset", 0.f));
    m_ButtonParams.fTopOffset = GetScrHeight(pIni->GetFloat("BUTTON", "topoffset", 0.f));
    m_ButtonParams.fBottomOffset = GetScrHeight(pIni->GetFloat("BUTTON", "bottomoffset", 0.f));

    char FName[MAX_PATH];
    pIni->ReadString("DIALOG", "charnamefont", FName, MAX_PATH, "DIALOG2");
    m_nCharNameTextFont = RenderService->LoadFont(FName);
    m_dwCharNameTextColor = pIni->GetLong("DIALOG", "charnamecolor", 0xFFFFFFFF);
    m_fCharNameTextScale = pIni->GetFloat("DIALOG", "charnamescale", 1.f);
    GetPointFromIni(pIni, "DIALOG", "charnameoffset", m_fpCharNameTextOffset);
    m_fpCharNameTextOffset.x = GetScrWidth(m_fpCharNameTextOffset.x);
    m_fpCharNameTextOffset.y = GetScrHeight(m_fpCharNameTextOffset.y);

    delete pIni;
}

void DIALOG::GetRectFromIni(INIFILE *ini, const char *pcSection, const char *pcKey, FRECT &frect)
{
    frect.left = frect.top = 0.f;
    frect.right = frect.bottom = 1.f;
    if (!ini)
        return;

    char param[512];
    if (!ini->ReadString((char *)pcSection, (char *)pcKey, param, sizeof(param) - 1, ""))
        return;
    sscanf(param, "%f,%f,%f,%f", &frect.left, &frect.top, &frect.right, &frect.bottom);
}

void DIALOG::GetPointFromIni(INIFILE *ini, const char *pcSection, const char *pcKey, FPOINT &fpoint)
{
    fpoint.x = fpoint.y = 0.f;
    if (!ini)
        return;

    char param[512];
    if (!ini->ReadString((char *)pcSection, (char *)pcKey, param, sizeof(param) - 1, ""))
        return;
    sscanf(param, "%f,%f", &fpoint.x, &fpoint.y);
}

void DIALOG::AddToStringArrayLimitedByWidth(const char *pcSrcText, long nFontID, float fScale, long nLimitWidth,
                                            std::vector<std::string> &asOutTextList, std::vector<long> *panPageIndices,
                                            long nPageSize)
{
    if (!pcSrcText)
        return;
    if (nLimitWidth < 20)
        nLimitWidth = 20;

    long n = 0;
    char param[1024];
    long nCur = 0;
    long nPrevIdx = 0;
    if (panPageIndices && panPageIndices->size() > 0)
        nPrevIdx = panPageIndices->back();
    while (true)
    {
        if ((pcSrcText[nCur] == 0x20 && pcSrcText[nCur + 1] != 0) || pcSrcText[nCur] == 0) // space
        {
            // boal fix пробел в конце строки
            param[n] = 0;
            const long nW = RenderService->StringWidth(param, nFontID, fScale);
            if (nW < nLimitWidth && pcSrcText[nCur] != 0)
            {
                // пока еще не перешли предел по ширине
                param[n++] = 0x20;
            }
            else
            {
                if (nW > nLimitWidth)
                {
                    // find prev space;
                    long nPrev = nCur;
                    while (nPrev > 0 && pcSrcText[nPrev - 1] != 0x20)
                        nPrev--;
                    if (nPrev <= 0) // нет пробелов
                    {
                        // ищем первый попавший в диапазон набор символов
                        while (n > 0 && RenderService->StringWidth(param, nFontID, fScale) > nLimitWidth)
                        {
                            n--;
                            nCur--;
                            param[n] = 0;
                        }
                    }
                    else
                    {
                        n -= nCur - nPrev;
                        Assert(n > 0);
                        param[n] = 0;
                        nCur = nPrev;
                    }
                }
                asOutTextList.push_back(param);
                n = 0;

                if (panPageIndices && asOutTextList.size() - nPrevIdx == nPageSize)
                {
                    nPrevIdx = asOutTextList.size();
                    panPageIndices->push_back(nPrevIdx);
                }
            }
            while (pcSrcText[nCur] == 0x20 && pcSrcText[nCur + 1] != 0)
                nCur++; // boal fix
            if (pcSrcText[nCur] == 0)
                break;
        }
        else
        {
            param[n++] = pcSrcText[nCur++];
        }
    }
}

//--------------------------------------------------------------------
bool DIALOG::Init()
{
    forceEmergencyClose = false;
    selectedLinkName[0] = 0;
    api->SetTimeScale(0.f);
    unfadeTime = 0;

    RenderService = static_cast<VDX9RENDER *>(api->CreateService("dx9render"));
    Assert(RenderService);

    snd = static_cast<VSoundService *>(api->CreateService("SoundService"));
    // Assert( snd );

    //----------------------------------------------------------
    LoadFromIni();

    CreateBack();
    FillBack();
    FillDivider();

    textViewport.X = static_cast<long>(m_BackParams.m_frBorderInt.left + GetScrWidth(4));
    textViewport.Y = static_cast<long>(GetScrY(437));
    textViewport.Width = static_cast<long>(m_BackParams.m_frBorderInt.right - GetScrWidth(4)) - textViewport.X;
    textViewport.Height = static_cast<unsigned long>(GetScrHeight(66));
    textViewport.MinZ = 0.0f;
    textViewport.MaxZ = 1.0f;

    INIFILE *ini = fio->OpenIniFile("Resource\\Ini\\dialog.ini");
    m_DlgText.Init(RenderService, textViewport, ini);
    m_DlgLinks.Init(RenderService, textViewport, ini);
    STORM_DELETE(ini);

    CreateButtons();
    FillButtons();

    return true;
}

//--------------------------------------------------------------------
void DIALOG::Realize(uint32_t Delta_Time)
{
    RenderService->MakePostProcess();
    // замедленный выход из паузы
    if (unfadeTime <= UNFADE_TIME)
    {
        unfadeTime += static_cast<int>(api->GetRDeltaTime());
        float timeK = static_cast<float>(unfadeTime) / UNFADE_TIME;
        if (timeK > 1.f)
            timeK = 1.f;
        api->SetTimeScale(timeK);
    }

    // играем спич
    if (play == 0 && soundName[0] && snd)
    {
        curSnd = snd->SoundPlay(soundName, PCM_STEREO, VOLUME_SPEECH);
        play = 1;
    }

    // нет аттрибутов диалога - нет дальнейшего разговора
    if (!AttributesPointer)
        return;

    if (m_bDlgChanged)
    {
        UpdateDlgTexts();
        UpdateDlgViewport();
        FillDivider();
        FillButtons();
    }

    CONTROL_STATE cs;
    CONTROL_STATE cs1; // boal
    CONTROL_STATE cs2;

    // прерывание диалога
    api->Controls->GetControlState("DlgCancel", cs);
    if (cs.state == CST_ACTIVATED)
        api->Event("DialogCancel");

    bool bDoUp = false;
    bool bDoDown = false;
    //
    api->Controls->GetControlState("DlgUp", cs);
    if (cs.state == CST_ACTIVATED)
        bDoUp = true;
    if (!bEditMode)
    {
        api->Controls->GetControlState("DlgUp2", cs);
        if (cs.state == CST_ACTIVATED)
            bDoUp = true;
        api->Controls->GetControlState("DlgUp3", cs);
        if (cs.state == CST_ACTIVATED)
            bDoUp = true;
    }
    //
    api->Controls->GetControlState("DlgDown", cs);
    if (cs.state == CST_ACTIVATED)
        bDoDown = true;
    if (!bEditMode)
    {
        api->Controls->GetControlState("DlgDown2", cs);
        if (cs.state == CST_ACTIVATED)
            bDoDown = true;
        api->Controls->GetControlState("DlgDown3", cs);
        if (cs.state == CST_ACTIVATED)
            bDoDown = true;
    }

    // переходим вверх (к предыдущей строке)
    if (bDoUp)
    {
        // игрем щелчок нажатой клавиши
        if (snd)
            snd->SoundPlay(TICK_SOUND, PCM_STEREO, VOLUME_FX);

        if (m_DlgText.IsLastPage())
        {
            if (m_DlgLinks.nSelectLine > 0)
                m_DlgLinks.nSelectLine--;
            const long nTmp = (m_DlgLinks.nSelectLine > 0) ? m_DlgLinks.anLineEndIndex[m_DlgLinks.nSelectLine - 1] : 0;
            if (m_DlgLinks.nStartIndex > nTmp)
            {
                m_DlgLinks.nStartIndex = nTmp;
                FillButtons();
            }
        }
    }
    // переход вниз - к следующей строке
    if (bDoDown)
    {
        // игрем щелчок нажатой клавиши
        if (snd)
            snd->SoundPlay(TICK_SOUND, PCM_STEREO, VOLUME_FX);

        if (m_DlgText.IsLastPage())
        {
            if (m_DlgLinks.nSelectLine < static_cast<long>(m_DlgLinks.anLineEndIndex.size()) - 1)
                m_DlgLinks.nSelectLine++;
            if (m_DlgLinks.nStartIndex + m_DlgLinks.nShowQuantity < m_DlgLinks.anLineEndIndex[m_DlgLinks.nSelectLine])
            {
                m_DlgLinks.nStartIndex = m_DlgLinks.anLineEndIndex[m_DlgLinks.nSelectLine] - m_DlgLinks.nShowQuantity;
                FillButtons();
            }
        }
    }
    // страница вверх
    api->Controls->GetControlState("DlgScrollUp", cs);
    if (cs.state == CST_ACTIVATED)
    {
        if (snd)
            snd->SoundPlay(TICK_SOUND, PCM_STEREO, VOLUME_FX);
        if (m_DlgText.nStartIndex > 0)
        {
            m_DlgText.PrevPage();
            UpdateDlgViewport();
            FillDivider();
            FillButtons();
        }
    }
    // страница вниз
    api->Controls->GetControlState("DlgScrollDown", cs);
    if (cs.state == CST_ACTIVATED)
    {
        if (snd)
            snd->SoundPlay(TICK_SOUND, PCM_STEREO, VOLUME_FX);
        if (!m_DlgText.IsLastPage())
        {
            m_DlgText.NextPage();
            UpdateDlgViewport();
            FillDivider();
            FillButtons();
        }
    }
    // действие
    api->Controls->GetControlState("DlgAction", cs);
    api->Controls->GetControlState("DlgAction2", cs2);
    api->Controls->GetControlState("DlgAction1", cs1); // boal
    if (bEditMode)
    {
        cs.state = CST_INACTIVE;
    }
    if (cs.state == CST_ACTIVATED || cs2.state == CST_ACTIVATED || cs1.state == CST_ACTIVATED) // boal
    {
        // игрем щелчок нажатой клавиши
        if (snd)
            snd->SoundPlay(TICK_SOUND, PCM_STEREO, VOLUME_FX);

        if (m_DlgText.IsLastPage())
        {
            // показываем варианты ответа
            ATTRIBUTES *pA = AttributesPointer->GetAttributeClass("links");
            if (pA)
                pA = pA->GetAttributeClass(m_DlgLinks.nSelectLine);
            if (pA)
            {
                char *goName = pA->GetAttribute("go");
                if (!goName || _stricmp(goName, selectedLinkName) == 0)
                    EmergencyExit();
                else
                {
                    AttributesPointer->SetAttribute("CurrentNode", goName);
                    strcpy_s(selectedLinkName, goName);

                    // set default
                    strcpy_s(soundName, charDefSnd);
                    api->Event("DialogEvent");
                }
            }
        }
        else
        {
            // пока еще показываем текст
            m_DlgText.NextPage();
            UpdateDlgViewport();
            FillDivider();
            FillButtons();
        }
    }

    DrawBack();
    DrawButtons();

    RenderService->ExtPrint(m_nCharNameTextFont, m_dwCharNameTextColor, 0, PR_ALIGN_LEFT, true, m_fCharNameTextScale, 0,
                            0, static_cast<long>(m_BackParams.m_frBorderExt.left + m_fpCharNameTextOffset.x),
                            static_cast<long>(m_BackParams.m_frBorderExt.top + m_fpCharNameTextOffset.y), "%s",
                            m_sTalkPersName.c_str());

    m_DlgText.Show(textViewport.Y);
    if (m_DlgText.IsLastPage())
        m_DlgLinks.Show(static_cast<long>(textViewport.Y + m_BackParams.nDividerOffsetY + m_BackParams.nDividerHeight));

    if (snd && !snd->SoundIsPlaying(curSnd))
    {
        // stop animation
        if (play == 1) // if person speech
            play = -1;
    }
}

//--------------------------------------------------------------------
uint32_t DIALOG::AttributeChanged(ATTRIBUTES *pA)
{
    // search for default settings
    bool parLinks = false;
    ATTRIBUTES *par = pA->GetParent();
    if (par != nullptr)
    {
        const char *parname = par->GetThisName();
        if (parname != nullptr && _stricmp(parname, "Links") == 0)
            parLinks = true;
    }

    const char *nm = pA->GetThisName();

    // play sound d.speech
    if (!parLinks && nm && _stricmp(nm, "greeting") == 0) // was "snd"
    {
        strcpy_s(soundName, pA->GetThisAttr());
        if (start)
            play = 0;
    }

    m_bDlgChanged = true;
    return 0;
}

//--------------------------------------------------------------------
uint64_t DIALOG::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
        // get character ID
    case 0:
        persId = message.EntityID();
        persMdl = message.EntityID();
        break;
        // get person ID
    case 1:
        charId = message.EntityID();
        charMdl = message.EntityID();
        const char *attr = nullptr;
        if (attr = api->Entity_GetAttribute(charId, "name"))
            m_sTalkPersName = attr;
        if (m_sTalkPersName.size() > 0)
            m_sTalkPersName += " ";
        if (attr = api->Entity_GetAttribute(charId, "lastname"))
            m_sTalkPersName += attr;
        m_BackParams.fCharacterNameRectCenterWidth =
            4.f +
            RenderService->StringWidth((char *)m_sTalkPersName.c_str(), m_nCharNameTextFont, m_fCharNameTextScale);
        break;
    }
    return 0;
}

//--------------------------------------------------------------------
void DIALOG::EmergencyExit()
{
    if (forceEmergencyClose)
        return;
    forceEmergencyClose = true;
    api->Trace("DIALOG: Invalid links, emergency exit! (last link = %s)", selectedLinkName);
    api->Event("EmergencyDialogExit");
}

void DIALOG::UpdateDlgTexts()
{
    if (!AttributesPointer)
        return;

    m_DlgText.ChangeText(AttributesPointer->GetAttribute("Text"));
    m_DlgLinks.ChangeText(AttributesPointer->GetAttributeClass("Links"));

    bEditMode = false;
    m_bDlgChanged = false;
}

void DIALOG::UpdateDlgViewport()
{
    const long nTextHeight = m_DlgText.GetShowHeight();
    const long nLinksHeight = m_DlgText.IsLastPage() ? m_DlgLinks.GetShowHeight() : 0;

    long nAllHeight = nTextHeight;
    if (nLinksHeight > 0)
    {
        m_BackParams.bShowDivider = true;
        m_BackParams.nDividerOffsetY = nTextHeight;
        nAllHeight += nLinksHeight + static_cast<long>(m_BackParams.nDividerHeight);
    }
    else
    {
        m_BackParams.bShowDivider = false;
    }

    textViewport.Height = nAllHeight;
    textViewport.Y = static_cast<unsigned long>(static_cast<long>(m_BackParams.m_frBorderInt.bottom) - nAllHeight -
                                                GetScrHeight(DIALOG_BOTTOM_LINESPACE));

    const float fTopBorder = textViewport.Y - GetScrHeight(DIALOG_TOP_LINESPACE);
    if (m_BackParams.m_frBorderInt.top != fTopBorder)
    {
        m_BackParams.m_frBorderInt.top = fTopBorder;
        m_BackParams.m_frBorderExt.top = fTopBorder - m_BackParams.frBorderRect.top;

        FillBack();
    }
}
