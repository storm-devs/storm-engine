#include "xi_util.h"
#include "../inode.h"
#include <stdio.h>

CXI_UTILS *CXI_UTILS::pThis = nullptr;

CXI_UTILS::CXI_UTILS()
{
    pThis = this;
}

CXI_UTILS::~CXI_UTILS()
{
}

void CXI_UTILS::Init()
{
    for (long n = 0; n < UTILS_KEYS_QUANTITY; n++)
    {
        // keys[n].nAsyncKeyCode = -1; // not used
        keys[n].nPressedState = -1;
        keys[n].nAsyncKeyCode = n;
    }
    // language keys

    m_bLang2 = false;
    m_bIsKeyPressed = false;
    m_bFrameKeyPressedFlag = false;
}

void CXI_UTILS::Release()
{
}

void CXI_UTILS::FrameUpdate()
{
    if (m_bIsKeyPressed && m_bFrameKeyPressedFlag)
    {
        // there was no polling on this frame, which means all the keys can be reset (we left the polling cycle)
        for (long n = 0; n < UTILS_KEYS_QUANTITY; n++)
        {
            if (keys[n].nAsyncKeyCode < 0)
                continue;
            keys[n].nPressedState = -1;
        }
        m_bIsKeyPressed = false;
    }
    m_bFrameKeyPressedFlag = true;
}

char CXI_UTILS::GetKeyInput()
{
    pThis->m_bFrameKeyPressedFlag = false; // do not reset key states while polling occurs on each frame
    char cRetVal = 0;

    pThis->m_bIsKeyPressed = false;
    for (long n = 0; n < UTILS_KEYS_QUANTITY; n++)
    {
        if (pThis->keys[n].nAsyncKeyCode < 0)
            continue;

        if (GetAsyncKeyState(pThis->keys[n].nAsyncKeyCode) < 0)
        {
            pThis->m_bIsKeyPressed = true;

            if (pThis->keys[n].nPressedState < 0)
            {
                pThis->keys[n].nPressedState = 0;

                switch (n)
                {
                case VK_LEFT:
                    return SpecSymbol_left;
                    break;
                case VK_RIGHT:
                    return SpecSymbol_right;
                    break;
                case VK_UP:
                    return SpecSymbol_up;
                    break;
                case VK_DOWN:
                    return SpecSymbol_down;
                    break;
                case VK_BACK:
                    return SpecSymbol_back;
                    break;
                case VK_DELETE:
                    return SpecSymbol_delete;
                    break;
                case VK_HOME:
                    return SpecSymbol_home;
                    break;
                case VK_END:
                    return SpecSymbol_end;
                    break;
                case VK_TAB:
                    return SpecSymbol_tab;
                    break;
                case VK_RETURN:
                    return SpecSymbol_return;
                    break;
                case VK_ESCAPE:
                    return SpecSymbol_escape;
                    break;
                }

                uint8_t pKBState[256];
                uint16_t pcTmp[16]; // in general, need 2 characters (the rest just in case)
                GetKeyboardState(pKBState);
                if (ToAscii(n, MapVirtualKey(n, 0), pKBState, pcTmp, 0) == 1)
                {
                    cRetVal = static_cast<char>(pcTmp[0]);
                    return cRetVal;
                }
            }
            cRetVal = 0;
        }
        else
        {
            pThis->keys[n].nPressedState = -1;
        }
    }

    return cRetVal;
}

void CXI_UTILS::StringLeftClamp(char *&pcString)
{
    if (!pcString)
        return;
    while (pcString[0])
    {
        if (pcString[0] <= 0x20)
        {
            pcString++;
            continue;
        }
        break;
    }
}

const char *CXI_UTILS::StringGetTokenID(char *&pcString, char *pcBuffer, long nBufferSize)
{
    if (!pcString || !pcBuffer || nBufferSize <= 0)
        return nullptr;
    pcBuffer[0] = 0;

    StringLeftClamp(pcString);

    long n = 0;
    const auto q = nBufferSize - 1;
    for (auto cCur = pcString[0]; cCur && n < q; pcString++, cCur = pcString[0])
    {
        if (cCur == '=')
            break;
        if (cCur == ',')
        {
            n = 0;
            pcString++;
            StringLeftClamp(pcString);
            pcString--;
            continue;
        }
        if (cCur < 0x20)
            continue; // skip unused characters
        pcBuffer[n++] = cCur;
    }
    // remove the last spaces
    while (n > 0 && pcBuffer[n - 1] <= 0x20)
        n--;
    pcBuffer[n] = 0;

    // get rid of the '=' sign
    while (pcString[0] == '=')
        pcString++;

    if (pcBuffer[0])
        return pcBuffer;
    return nullptr;
}

const char *CXI_UTILS::StringGetTokenString(char *&pcString, char *pcBuffer, long nBufferSize)
{
    if (!pcString || !pcBuffer || nBufferSize <= 0)
        return nullptr;
    pcBuffer[0] = 0;

    StringLeftClamp(pcString);

    long n = 0;
    const auto q = nBufferSize - 1;
    long nQuote = 0;
    for (auto cCur = pcString[0]; cCur && n < q; pcString++, cCur = pcString[0])
    {
        if (cCur == ',' && nQuote == 0)
            break;
        if (cCur == '(' || cCur == '{' || cCur == '[')
            nQuote++;
        if (cCur == ')' || cCur == '}' || cCur == ']')
            nQuote--;
        if (cCur < 0x20)
            continue; // skip unused characters
        pcBuffer[n++] = cCur;
    }
    // remove the last spaces
    while (n > 0 && pcBuffer[n - 1] <= 0x20)
        n--;
    pcBuffer[n] = 0;

    // get rid of the ',' sign
    while (pcString[0] == ',')
        pcString++;

    if (pcBuffer[0])
        return pcBuffer;
    return nullptr;
}

long CXI_UTILS::StringGetTokenCode(const char *pcTokenID)
{
    if (_stricmp(pcTokenID, "color") == 0)
        return InterfaceToken_color;
    if (_stricmp(pcTokenID, "file") == 0)
        return InterfaceToken_file;
    if (_stricmp(pcTokenID, "piclist") == 0)
        return InterfaceToken_picture_list;
    if (_stricmp(pcTokenID, "picname") == 0)
        return InterfaceToken_picture_name;
    if (_stricmp(pcTokenID, "piccutuv") == 0)
        return InterfaceToken_picture_cut_uv;
    if (_stricmp(pcTokenID, "size") == 0)
        return InterfaceToken_size;
    if (_stricmp(pcTokenID, "rectUV") == 0)
        return InterfaceToken_rectUV;
    if (_stricmp(pcTokenID, "pos") == 0)
        return InterfaceToken_pos;
    if (_stricmp(pcTokenID, "text") == 0)
        return InterfaceToken_text;
    if (_stricmp(pcTokenID, "width") == 0)
        return InterfaceToken_width;
    if (_stricmp(pcTokenID, "class") == 0)
        return InterfaceToken_class;

    return InterfaceToken_unknown;
}

uint32_t CXI_UTILS::StringGetColor(const char *pcARGBString)
{
    const auto nA = StringGetLong(pcARGBString);
    const auto nR = StringGetLong(pcARGBString);
    const auto nG = StringGetLong(pcARGBString);
    const auto nB = StringGetLong(pcARGBString);
    return ARGB(nA, nR, nG, nB);
}

void CXI_UTILS::StringDoublicate(const char *pcSrc, char *&pcDst)
{
    delete pcDst;
    pcDst = nullptr;
    if (pcSrc)
    {
        const auto len = strlen(pcSrc) + 1;
        pcDst = new char[len];
        if (pcDst)
            memcpy(pcDst, pcSrc, len);
        else
            throw std::runtime_error("allocate memory error");
    }
}

void CXI_UTILS::StringTwoLong(const char *pcString, long &nLong1, long &nLong2)
{
    nLong1 = StringGetLong(pcString);
    nLong2 = StringGetLong(pcString);
}

long CXI_UTILS::StringGetLong(const char *&pcString)
{
    if (!pcString)
        return 0;
    long nRetVal = 0;
    for (; pcString[0]; pcString++)
    {
        if (pcString[0] <= 0x20)
            continue;
        if (pcString[0] == ',')
        {
            pcString++;
            break;
        }
        if (pcString[0] >= '0' && pcString[0] <= '9')
        {
            nRetVal = nRetVal * 10 + pcString[0] - '0';
        }
    }
    return nRetVal;
}

float CXI_UTILS::StringGetFloat(const char *&pcString)
{
    if (!pcString)
        return 0.f;
    char sTmp[128];
    long n = 0;
    for (; pcString[0] && n < sizeof(sTmp) - 1; pcString++)
    {
        if (pcString[0] <= 0x20)
            continue;
        if (pcString[0] == ',')
        {
            pcString++;
            break;
        }
        if ((pcString[0] >= '0' && pcString[0] <= '9') || pcString[0] == '.')
        {
            sTmp[n] = pcString[0];
            n++;
        }
    }
    sTmp[n] = 0;
    return static_cast<float>(atof(sTmp));
}

void CXI_UTILS::StringFourFloat(const char *pcString, float &f1, float &f2, float &f3, float &f4)
{
    f1 = StringGetFloat(pcString);
    f2 = StringGetFloat(pcString);
    f3 = StringGetFloat(pcString);
    f4 = StringGetFloat(pcString);
}

void CXI_UTILS::StringFillStringArray(const char *pcString, std::vector<std::string> &asStringsArray)
{
    char tmpstr[256];
    auto *pcSrcStr = (char *)pcString;

    // delete old
    asStringsArray.clear();

    // create new
    while (nullptr != StringGetTokenString(pcSrcStr, tmpstr, sizeof(tmpstr)))
    {
        if (!tmpstr[0])
            continue;
        asStringsArray.emplace_back(tmpstr);
    }
}

long CXI_UTILS::SplitStringByWidth(const char *pcText, long nFontID, float fFontScale, long nWidth,
                                   std::vector<std::string> &asOutStr)
{
    long nMaxUsingWidth = 0;
    const auto *const pcSrcStr = pcText;
    if (pcSrcStr == nullptr)
        return nMaxUsingWidth;
    auto *rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    asOutStr.clear();

    long nSrc = 0;
    long nDst = 0;
    long nDstPrev = 0;
    long nSrcPrev = 0;
    char param[1024];
    while (true)
    {
        const long nToken = GetCurrentTokenIntoString(&pcSrcStr[nSrc]);
        if (nToken == StrTokenType_common)
        {
            param[nDst++] = pcSrcStr[nSrc++];
            continue;
        }

        param[nDst] = 0;
        auto nW = rs->StringWidth(param, nFontID, fFontScale);
        if (nW < nWidth && nToken == StrTokenType_Space) // space but maybe not the last
        {
            nSrcPrev = nSrc;
            nDstPrev = nDst;
            while (pcSrcStr[nSrc] == 0x20)
                nSrc++;
            param[nDst++] = 0x20;
            continue;
        }

        if (nW > nWidth && nDstPrev > 0) // either the last space or just the end of the line
        {
            nSrc = nSrcPrev;
            nDst = nDstPrev;
            param[nDst++] = 0x20;
        }

        param[nDst] = 0;
        while (pcSrcStr[nSrc] == 0x20) // remove spaces from the string
            nSrc++;
        if (GetCurrentTokenIntoString(&pcSrcStr[nSrc]) == StrTokenType_NextLine) // when a line breaks, count it
        {
            nSrc += 2;
            while (nDst > 0 && param[nDst - 1] == 0x20) // result line must not end with a space
                param[--nDst] = 0;
        }

        nDstPrev = nDst = 0;
        asOutStr.emplace_back(param);
        // asOutStr[n] = param;
        nW = rs->StringWidth(param, nFontID, fFontScale);
        if (nW > nMaxUsingWidth)
            nMaxUsingWidth = nW;

        if (GetCurrentTokenIntoString(&pcSrcStr[nSrc]) == StrTokenType_End)
            break;
    }
    return nMaxUsingWidth;
}

float CXI_UTILS::GetByStrNumFromAttribute_Float(ATTRIBUTES *pA, const char *pStr, long num, float fDefValue)
{
    if (!pA)
        return fDefValue;
    char stmp[64];
    sprintf_s(stmp, sizeof(stmp), "%s%d", pStr, num);
    return pA->GetAttributeAsFloat(stmp, fDefValue);
}

void CXI_UTILS::WriteSquareToVertexBuffer(XI_ONETEX_VERTEX *pv, uint32_t color, FXYRECT &uv, XYRECT &rect)
{
    pv[0].color = color;
    pv[0].pos.x = static_cast<float>(rect.left);
    pv[0].pos.y = static_cast<float>(rect.top);
    pv[0].pos.z = 1.f;
    pv[0].tu = uv.left;
    pv[0].tv = uv.top;

    pv[1].color = color;
    pv[1].pos.x = static_cast<float>(rect.left);
    pv[1].pos.y = static_cast<float>(rect.bottom);
    pv[1].pos.z = 1.f;
    pv[1].tu = uv.left;
    pv[1].tv = uv.bottom;

    pv[2].color = color;
    pv[2].pos.x = static_cast<float>(rect.right);
    pv[2].pos.y = static_cast<float>(rect.top);
    pv[2].pos.z = 1.f;
    pv[2].tu = uv.right;
    pv[2].tv = uv.top;

    pv[3].color = color;
    pv[3].pos.x = static_cast<float>(rect.right);
    pv[3].pos.y = static_cast<float>(rect.bottom);
    pv[3].pos.z = 1.f;
    pv[3].tu = uv.right;
    pv[3].tv = uv.bottom;
}

void CXI_UTILS::WriteSquareToVertexBuffer(XI_ONETEX_VERTEX *pv, uint32_t color, FXYRECT &uv, long left, long top,
                                          long right, long bottom)
{
    pv[0].color = color;
    pv[0].pos.x = static_cast<float>(left);
    pv[0].pos.y = static_cast<float>(top);
    pv[0].pos.z = 1.f;
    pv[0].tu = uv.left;
    pv[0].tv = uv.top;

    pv[1].color = color;
    pv[1].pos.x = static_cast<float>(left);
    pv[1].pos.y = static_cast<float>(bottom);
    pv[1].pos.z = 1.f;
    pv[1].tu = uv.left;
    pv[1].tv = uv.bottom;

    pv[2].color = color;
    pv[2].pos.x = static_cast<float>(right);
    pv[2].pos.y = static_cast<float>(top);
    pv[2].pos.z = 1.f;
    pv[2].tu = uv.right;
    pv[2].tv = uv.top;

    pv[3].color = color;
    pv[3].pos.x = static_cast<float>(right);
    pv[3].pos.y = static_cast<float>(bottom);
    pv[3].pos.z = 1.f;
    pv[3].tu = uv.right;
    pv[3].tv = uv.bottom;
}

void CXI_UTILS::PrintTextIntoWindow(VDX9RENDER *pRender, long nFont, uint32_t dwColor, long wAlignment, bool bShadow,
                                    float fScale, long scrWidth, long scrHeight, long x, long y, const char *pcString,
                                    long left, long top, long width, long height)
{
    const auto nStrWidth = pRender->StringWidth((char *)pcString, nFont, fScale, 0);
    if (nStrWidth == 0)
        return;
    const auto right = left + width;

    long nL, nR;
    if (wAlignment == PR_ALIGN_RIGHT)
        nL = x - nStrWidth;
    else if (wAlignment == PR_ALIGN_CENTER)
        nL = x - nStrWidth / 2;
    else
        nL = x;
    nR = nL + nStrWidth;

    // fit into the window -> display as usual
    if (nL >= left && nR <= right)
    {
        pRender->ExtPrint(nFont, dwColor, 0, wAlignment, bShadow, fScale, scrWidth, scrHeight, x, y, "%s", pcString);
        return;
    }

    char tmpstr[4096];
    sprintf_s(tmpstr, sizeof(tmpstr), "%s", pcString);
    char *pc = tmpstr;

    // cut the left edge
    while (pc[0] && nL < left)
    {
        pc += utf8::u8_inc(pc);
        nL = nR - pRender->StringWidth(pc, nFont, fScale, 0);
    }

    // cut the right edge
    if (nR > right)
    {
        long n = strlen(pc);
        while (n > 0 && nR > right)
        {
            n -= utf8::u8_dec(pc + n);
            pc[n] = '\0';
            nR = nL + pRender->StringWidth(pc, nFont, fScale, 0);
        }
    }

    pRender->ExtPrint(nFont, dwColor, 0, PR_ALIGN_LEFT, bShadow, fScale, scrWidth, scrHeight, nL, y, "%s", pc);
}
