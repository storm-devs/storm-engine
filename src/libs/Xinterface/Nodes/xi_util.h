#pragma once

#define UTILS_KEYS_QUANTITY 256

#define UTILS_KEYS_DELAY_FOR_REPEAT 800

#include "../xdefines.h"
#include "dx9render.h"

enum InterfaceStringCurTokenType
{
    StrTokenType_End,
    StrTokenType_Space,
    StrTokenType_NextLine,

    StrTokenType_common = 0xFFFFFFFE,
    StrTokenType_unknown = 0xFFFFFFFF
};

enum InterfaceSpecSymbolsCode
{
    SpecSymbol_left = 1,
    SpecSymbol_right,
    SpecSymbol_up,
    SpecSymbol_down,
    SpecSymbol_back,
    SpecSymbol_delete,
    SpecSymbol_home,
    SpecSymbol_end,
    SpecSymbol_tab,
    SpecSymbol_return,
    SpecSymbol_escape
};

enum InterfaceTokenCode
{
    InterfaceToken_color,
    InterfaceToken_file,
    InterfaceToken_picture_list,
    InterfaceToken_picture_name,
    InterfaceToken_picture_cut_uv,
    InterfaceToken_size,
    InterfaceToken_rectUV,
    InterfaceToken_pos,
    InterfaceToken_text,
    InterfaceToken_width,
    InterfaceToken_class,

    InterfaceToken_unknown = 0xFFFFFFFF
};

class CXI_UTILS
{
    struct KeyDescribe
    {
        int nAsyncKeyCode;
        long nPressedState;
    };

  public:
    CXI_UTILS();
    ~CXI_UTILS();

    void Init();
    void Release();

    void FrameUpdate();

    // key input
    static char GetKeyInput();

    // string processing
    static void StringLeftClamp(char *&pcString);
    static const char *StringGetTokenID(char *&pcString, char *pcBuffer, long nBufferSize);
    static const char *StringGetTokenString(char *&pcString, char *pcBuffer, long nBufferSize);
    static long StringGetTokenCode(const char *pcTokenID);
    static uint32_t StringGetColor(const char *pcARGBString);
    static void StringDoublicate(const char *pcSrc, char *&pcDst);
    static void StringTwoLong(const char *pcString, long &nLong1, long &nLong2);
    static long StringGetLong(const char *&pcString);
    static float StringGetFloat(const char *&pcString);
    static void StringFourFloat(const char *pcString, float &f1, float &f2, float &f3, float &f4);
    static void StringFillStringArray(const char *pcString, std::vector<std::string> &asStringsArray);
    static long SplitStringByWidth(const char *pcText, long nFontID, float fFontScale, long nWidth,
                                   std::vector<std::string> &asOutStr);

    static InterfaceStringCurTokenType GetCurrentTokenIntoString(const char *pcStr)
    {
        if (!pcStr)
            return StrTokenType_unknown;
        if (!pcStr[0])
            return StrTokenType_End;
        if (pcStr[0] == 0x20)
            return StrTokenType_Space;
        if (pcStr[0] == '\\' && (pcStr[1] == 'n' || pcStr[1] == 'N'))
            return StrTokenType_NextLine;
        return StrTokenType_common;
    }

    // attribute functions
    static float GetByStrNumFromAttribute_Float(ATTRIBUTES *pA, const char *pStr, long num, float fDefValue);

    // vertex processing
    static void WriteSquareToVertexBuffer(XI_ONETEX_VERTEX *pv, uint32_t color, FXYRECT &uv, XYRECT &rect);
    static void WriteSquareToVertexBuffer(XI_ONETEX_VERTEX *pv, uint32_t color, FXYRECT &uv, long left, long top,
                                          long right, long bottom);

    static void PrintTextIntoWindow(VDX9RENDER *pRender, long nFont, uint32_t dwColor, long wAlignment, bool bShadow,
                                    float fScale, long scrWidth, long scrHeight, long x, long y, const char *pcString,
                                    long left, long top, long width, long height);

  protected:
    static CXI_UTILS *pThis;

    bool m_bIsKeyPressed;
    bool m_bFrameKeyPressedFlag;
    bool m_bLang2;
    KeyDescribe keys[UTILS_KEYS_QUANTITY];
};
