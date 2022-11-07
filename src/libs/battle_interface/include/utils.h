#pragma once

#include "bi_defines.h"
#include <string>
#include <vector>

class BITextInfo final
{
  public:
    BITextInfo(BITextInfo &&text_info) noexcept;

    BITextInfo(const BITextInfo &text_info);

    BITextInfo();
    ~BITextInfo();
    void Release();
    void Init(VDX9RENDER *rs, ATTRIBUTES *pA);
    void Print();

    VDX9RENDER *pRS;
    std::string sText;
    POINT pos;
    float fScale;
    int32_t nFont;
    uint32_t dwColor;
    bool bShadow;

    ATTRIBUTES *pARefresh;
};

class BILinesInfo
{
  public:
    BILinesInfo();
    ~BILinesInfo();
    void Release();
    void Init(VDX9RENDER *rs, ATTRIBUTES *pA);
    void Draw();

    VDX9RENDER *pRS;
    std::vector<RS_LINE2D> lines;
};

class IBIImage;
class BIImageRender;

class BIImagesInfo
{
  public:
    BIImagesInfo();
    ~BIImagesInfo();
    void Release();
    void Init(VDX9RENDER *rs, ATTRIBUTES *pA);
    void Draw() const;

    VDX9RENDER *pRS;
    BIImageRender *pImgRender;
    std::vector<IBIImage *> images;
};

class BIBorderInfo
{
  public:
    BIBorderInfo();
    ~BIBorderInfo();
    void Release();
    void Init(VDX9RENDER *rs, ATTRIBUTES *pA);
    void Draw();

    VDX9RENDER *pRS;
    int32_t nVBuf;
    int32_t nTexID;
    FRECT ext_pos;
    FRECT int_pos1;
    FRECT int_pos2;
    uint32_t dwColor1;
    uint32_t dwColor2;
    float fCur;
    float fSpeed;
    bool bUp;
    bool bUsed;
};

class BIUtils
{
    //---------------------------------------
  public: // functions
    static int32_t GetIntFromAttr(ATTRIBUTES *pA, const char *name, int32_t defVal);
    static float GetFloatFromAttr(ATTRIBUTES *pA, const char *name, float defVal);
    static bool ReadStringFromAttr(ATTRIBUTES *pA, const char *name, char *buf, int32_t bufSize, const char *defVal);
    static const char *GetStringFromAttr(ATTRIBUTES *pA, const char *name, const char *defVal);
    static int32_t GetTextureFromAttr(VDX9RENDER *rs, ATTRIBUTES *pA, const char *sAttrName);
    static bool ReadRectFromAttr(ATTRIBUTES *pA, const std::string_view &name, FRECT &rOut, FRECT &rDefault);
    static bool ReadRectFromAttr(ATTRIBUTES *pA, const std::string_view &name, RECT &rOut, RECT &rDefault);
    static bool ReadPosFromAttr(ATTRIBUTES *pA, const char *name, float &fX, float &fY, float fXDef, float fYDef);
    static bool ReadPosFromAttr(ATTRIBUTES *pA, const char *name, int32_t &nX, int32_t &nY, int32_t nXDef, int32_t nYDef);
    static int32_t GetAlignmentFromAttr(ATTRIBUTES *pA, const char *name, int32_t nDefAlign);
    static int32_t GetFontIDFromAttr(ATTRIBUTES *pA, const char *name, VDX9RENDER *rs, const char *pcDefFontName);
    static bool ReadVectorFormAttr(ATTRIBUTES *pA, const char *name, CVECTOR &vOut, const CVECTOR &vDef);

    static bool ComparePoint(POINT &p1, POINT &p2);

    static ATTRIBUTES *GetAttributesFromPath(ATTRIBUTES *pA, ...);

    static uint32_t GetIntervalColor(uint32_t minV, uint32_t maxV, float fpar);
    static bool GetIntervalRect(float fk, const FRECT &r1, const FRECT &r2, FRECT &rOut);

    static int32_t GetMaxFromFourLong(int32_t n1, int32_t n2, int32_t n3, int32_t n4);

    static float GetFromStr_Float(const char *&pcStr, float fDefault);

    static void FillTextInfoArray(VDX9RENDER *pRS, ATTRIBUTES *pA, std::vector<BITextInfo> &tia);
    static void PrintTextInfoArray(std::vector<BITextInfo> &tia);
    //---------------------------------------
    //---------------------------------------
  public: // data
    static entid_t idBattleInterface;
    static uint32_t g_dwBlinkColor;
    //---------------------------------------
};
