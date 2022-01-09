#include "utils.h"

#include "core.h"

#include "image/img_render.h"
#include "v_module_api.h"

// extern data
entid_t BIUtils::idBattleInterface;
uint32_t BIUtils::g_dwBlinkColor;

int32_t BIUtils::GetIntFromAttr(ATTRIBUTES *pA, const char *name, int32_t defVal)
{
    if (pA == nullptr || name == nullptr)
        return defVal;
    return pA->GetAttributeAsDword(name, defVal);
}

float BIUtils::GetFloatFromAttr(ATTRIBUTES *pA, const char *name, float defVal)
{
    if (pA == nullptr || name == nullptr)
        return defVal;
    return pA->GetAttributeAsFloat(name, defVal);
}

bool BIUtils::ReadStringFromAttr(ATTRIBUTES *pA, const char *name, char *buf, int32_t bufSize, const char *defVal)
{
    if (buf == nullptr || bufSize < 1)
        return false;
    buf[0] = 0;
    const char *strGet;
    auto bRet = true;
    if (pA == nullptr || (strGet = pA->GetAttribute(name)) == nullptr)
    {
        strGet = defVal;
        bRet = false;
    }

    if (strGet != nullptr)
    {
        if (static_cast<int>(strlen(strGet)) > bufSize - 1)
        {
            strncpy_s(buf, bufSize, strGet, bufSize - 1);
            buf[bufSize - 1] = 0;
        }
        else
            strcpy_s(buf, bufSize, strGet);
    }
    else
        bRet = false;

    return bRet;
}

char *BIUtils::GetStringFromAttr(ATTRIBUTES *pA, const char *name, const char *defVal)
{
    if (pA == nullptr || name == nullptr)
        return (char *)defVal;
    auto *const aVal = pA->GetAttribute(name);
    if (aVal == nullptr)
        return (char *)defVal;
    return aVal;
}

int32_t BIUtils::GetTextureFromAttr(VDX9RENDER *rs, ATTRIBUTES *pA, const char *sAttrName)
{
    if (!rs || !pA)
        return -1;
    auto *const sname = pA->GetAttribute(sAttrName);
    if (!sname || sname[0] == 0)
        return -1;
    return rs->TextureCreate(sname);
}

bool BIUtils::ReadRectFromAttr(ATTRIBUTES *pA, const char *name, FRECT &rOut, FRECT &rDefault)
{
    rOut = rDefault;
    if (pA && name)
    {
        auto *const pcStr = pA->GetAttribute(name);
        if (pcStr)
        {
            sscanf(pcStr, "%f,%f,%f,%f", &rOut.left, &rOut.top, &rOut.right, &rOut.bottom);
            return true;
        }
    }
    return false;
}

bool BIUtils::ReadRectFromAttr(ATTRIBUTES *pA, const char *name, RECT &rOut, RECT &rDefault)
{
    rOut = rDefault;
    if (pA && name)
    {
        auto *const pcStr = pA->GetAttribute(name);
        if (pcStr)
        {
            sscanf(pcStr, "%d,%d,%d,%d", &rOut.left, &rOut.top, &rOut.right, &rOut.bottom);
            return true;
        }
    }
    return false;
}

bool BIUtils::ReadPosFromAttr(ATTRIBUTES *pA, const char *name, float &fX, float &fY, float fXDef, float fYDef)
{
    fX = fXDef;
    fY = fYDef;
    if (pA && name)
    {
        auto *const pcStr = pA->GetAttribute(name);
        if (pcStr)
        {
            sscanf(pcStr, "%f,%f", &fX, &fY);
            return true;
        }
    }
    return false;
}

bool BIUtils::ReadPosFromAttr(ATTRIBUTES *pA, const char *name, int32_t &nX, int32_t &nY, int32_t nXDef, int32_t nYDef)
{
    nX = nXDef;
    nY = nYDef;
    if (pA && name)
    {
        char *pcStr = pA->GetAttribute(name);
        if (pcStr)
        {
            sscanf(pcStr, "%d,%d", &nX, &nY);
            return true;
        }
    }
    return false;
}

int32_t BIUtils::GetAlignmentFromAttr(ATTRIBUTES *pA, const char *name, int32_t nDefAlign)
{
    if (pA && name)
    {
        char *pcTmp = pA->GetAttribute(name);
        if (pcTmp)
        {
            if (storm::iEquals(pcTmp, "left"))
                return PR_ALIGN_LEFT;
            if (storm::iEquals(pcTmp, "center"))
                return PR_ALIGN_CENTER;
            if (storm::iEquals(pcTmp, "right"))
                return PR_ALIGN_RIGHT;
        }
    }
    return nDefAlign;
}

int32_t BIUtils::GetFontIDFromAttr(ATTRIBUTES *pA, const char *name, VDX9RENDER *rs, const char *pcDefFontName)
{
    if (rs && pA && name)
    {
        char *pcTmp = pA->GetAttribute(name);
        if (pcTmp)
            return rs->LoadFont(pcTmp);
    }
    if (rs && pcDefFontName)
        return rs->LoadFont((char *)pcDefFontName);
    return -1;
}

bool BIUtils::ReadVectorFormAttr(ATTRIBUTES *pA, const char *name, CVECTOR &vOut, const CVECTOR &vDef)
{
    vOut = vDef;
    if (pA && name)
    {
        ATTRIBUTES *pAttr = pA->CreateSubAClass(pA, name);
        if (pAttr)
        {
            vOut.x = pAttr->GetAttributeAsFloat("x", vDef.x);
            vOut.y = pAttr->GetAttributeAsFloat("y", vDef.y);
            vOut.z = pAttr->GetAttributeAsFloat("z", vDef.z);
            return true;
        }
    }
    return false;
}

bool BIUtils::ComparePoint(POINT &p1, POINT &p2)
{
    return ((p1.x == p2.x) && (p1.y == p2.y));
}

ATTRIBUTES *BIUtils::GetAttributesFromPath(ATTRIBUTES *pA, ...)
{
    va_list arglist;

    char *sName;
    ATTRIBUTES *pTmpAttr = pA;
    va_start(arglist, pA);
    while ((sName = va_arg(arglist, char *)) != nullptr)
    {
        if (pTmpAttr == nullptr)
            return nullptr;
        pTmpAttr = pTmpAttr->GetAttributeClass(sName);
    }
    va_end(arglist);

    return pTmpAttr;
}

uint32_t BIUtils::GetIntervalColor(uint32_t minV, uint32_t maxV, float fpar)
{
    int32_t a = minV >> 24L;
    const int32_t ad = static_cast<int32_t>(maxV >> 24L) - a;
    int32_t r = (minV & 0xFF0000) >> 16;
    const int32_t rd = static_cast<int32_t>((maxV & 0xFF0000) >> 16) - r;
    int32_t g = (minV & 0xFF00) >> 8;
    const int32_t gd = static_cast<int32_t>((maxV & 0xFF00) >> 8) - g;
    int32_t b = minV & 0xFF;
    const int32_t bd = static_cast<int32_t>(maxV & 0xFF) - b;

    a += static_cast<int32_t>(ad * fpar);
    r += static_cast<int32_t>(rd * fpar);
    g += static_cast<int32_t>(gd * fpar);
    b += static_cast<int32_t>(bd * fpar);

    return ARGB(a, r, g, b);
}

bool BIUtils::GetIntervalRect(float fk, const FRECT &r1, const FRECT &r2, FRECT &rOut)
{
    rOut.left = r1.left + fk * (r2.left - r1.left);
    rOut.top = r1.top + fk * (r2.top - r1.top);
    rOut.right = r1.right + fk * (r2.right - r1.right);
    rOut.bottom = r1.bottom + fk * (r2.bottom - r1.bottom);
    return true;
}

int32_t BIUtils::GetMaxFromFourLong(int32_t n1, int32_t n2, int32_t n3, int32_t n4)
{
    if (n1 >= n2 && n1 >= n3 && n1 >= n4)
        return n1;
    if (n2 >= n3 && n2 >= n4)
        return n2;
    if (n3 >= n4)
        return n3;
    return n4;
}

float BIUtils::GetFromStr_Float(const char *&pcStr, float fDefault)
{
    if (!pcStr)
        return fDefault;
    int32_t n;
    char ctmp[64];
    for (n = 0; pcStr[0] != ',' && pcStr[0]; pcStr++)
    {
        if (pcStr[0] <= 32)
            continue;
        if (n < sizeof(ctmp) - 1)
            ctmp[n++] = pcStr[0];
    }
    ctmp[n] = 0;
    while (pcStr[0] == ',')
        pcStr++;
    return static_cast<float>(atof(ctmp));
}

void BIUtils::FillTextInfoArray(VDX9RENDER *pRS, ATTRIBUTES *pA, std::vector<BITextInfo> &tia)
{
    if (!pA)
        return;
    tia.clear();

    const size_t q = pA->GetAttributesNum();
    tia.reserve(q);
    for (int32_t n = 0; n < q; n++)
    {
        tia.emplace_back().Init(pRS, pA->GetAttributeClass(n));
    }
}

void BIUtils::PrintTextInfoArray(std::vector<BITextInfo> &tia)
{
    for (int32_t n = 0; n < tia.size(); n++)
        tia[n].Print();
}

BITextInfo::BITextInfo(BITextInfo &&text_info) noexcept
{
    pRS = text_info.pRS;
    sText = std::move(text_info.sText);
    pos = std::move(text_info.pos);
    fScale = text_info.fScale;
    nFont = text_info.nFont;
    dwColor = text_info.dwColor;
    bShadow = text_info.bShadow;
    pARefresh = text_info.pARefresh;

    text_info.nFont = -1;
}

BITextInfo::BITextInfo(const BITextInfo &text_info)
{
    pRS = text_info.pRS;
    sText = text_info.sText;
    pos = text_info.pos;
    fScale = text_info.fScale;
    nFont = text_info.nFont;
    dwColor = text_info.dwColor;
    bShadow = text_info.bShadow;
    pARefresh = text_info.pARefresh;

    pRS->IncRefCounter(nFont);
}

BITextInfo::BITextInfo()
{
    pRS = nullptr;
    nFont = -1;
}

BITextInfo::~BITextInfo()
{
    Release();
}

void BITextInfo::Release()
{
    sText = "";
    FONT_RELEASE(pRS, nFont);
}

void BITextInfo::Init(VDX9RENDER *rs, ATTRIBUTES *pA)
{
    FONT_RELEASE(pRS, nFont);
    pRS = rs;
    if (!pRS || !pA)
        return;

    nFont = pRS->LoadFont(pA->GetAttribute("font"));
    fScale = pA->GetAttributeAsFloat("scale", 1.f);
    dwColor = pA->GetAttributeAsDword("color", 0xFFFFFFFF);
    bShadow = pA->GetAttributeAsDword("shadow", 1) != 0;

    ATTRIBUTES *pAttr = pA->GetAttributeClass("pos");
    if (pAttr)
    {
        pos.x = pAttr->GetAttributeAsDword("x", 0);
        pos.y = pAttr->GetAttributeAsDword("y", 0);
    }
    else
    {
        pos.x = pos.y = 0;
    }

    char *textAttr = pA->GetAttribute("text");
    sText = textAttr ? textAttr : "";

    pARefresh = nullptr;
    if (pA->GetAttributeAsDword("refreshable", 0))
        pARefresh = pA;
}

void BITextInfo::Print()
{
    if (nFont != -1)
    {
        if (pARefresh)
        {
            char *textAttr = pARefresh->GetAttribute("text");
            sText = textAttr ? textAttr : "";
        }
        if (!sText.empty())
            pRS->ExtPrint(nFont, dwColor, 0, PR_ALIGN_CENTER, bShadow, fScale, 0, 0, pos.x, pos.y, "%s", sText.c_str());
    }
}

BILinesInfo::BILinesInfo()
{
    pRS = nullptr;
}

BILinesInfo::~BILinesInfo()
{
    Release();
}

void BILinesInfo::Release()
{
    lines.clear();
}

void BILinesInfo::Init(VDX9RENDER *rs, ATTRIBUTES *pA)
{
    pRS = rs;
    if (!pA)
        return;

    const size_t q = pA->GetAttributesNum();
    for (int32_t n = 0; n < q; n++)
    {
        ATTRIBUTES *pAttr = pA->GetAttributeClass(n);
        if (!pAttr)
            break;

        // int32_t bi = lines.Add();
        // int32_t ei = lines.Add();
        lines.push_back(RS_LINE2D{});
        const auto bi = lines.size() - 1;
        lines.push_back(RS_LINE2D{});
        const auto ei = lines.size() - 1;

        lines[bi].rhw = lines[ei].rhw = 0.5f;
        lines[bi].vPos.z = lines[ei].vPos.z = 1.f;
        lines[bi].dwColor = lines[ei].dwColor = pAttr->GetAttributeAsDword("color");
        ATTRIBUTES *pAPos = pAttr->GetAttributeClass("begin");
        if (pAPos)
        {
            lines[bi].vPos.x = static_cast<float>(pAPos->GetAttributeAsDword("x", 0));
            lines[bi].vPos.y = static_cast<float>(pAPos->GetAttributeAsDword("y", 0));
        }
        else
        {
            lines[bi].vPos.x = lines[bi].vPos.y = 0.f;
        }
        pAPos = pAttr->GetAttributeClass("end");
        if (pAPos)
        {
            lines[ei].vPos.x = lines[bi].vPos.x + pAPos->GetAttributeAsDword("x", 0);
            lines[ei].vPos.y = lines[bi].vPos.y + pAPos->GetAttributeAsDword("y", 0);
        }
        else
        {
            lines[ei].vPos.x = lines[ei].vPos.y = 0.f;
        }
    }
}

void BILinesInfo::Draw()
{
    pRS->DrawLines2D(&lines[0], lines.size() / 2, "Line");
}

BIImagesInfo::BIImagesInfo()
{
    pRS = nullptr;
    pImgRender = nullptr;
}

BIImagesInfo::~BIImagesInfo()
{
    Release();
}

void BIImagesInfo::Release()
{
    // images.DelAllWithPointers();
    for (const auto &image : images)
        delete image;
    STORM_DELETE(pImgRender);
}

void BIImagesInfo::Init(VDX9RENDER *rs, ATTRIBUTES *pA)
{
    if (!pA || !rs)
        return;

    pRS = rs;
    pImgRender = new BIImageRender(rs);
    if (!pImgRender)
        return;

    const size_t q = pA->GetAttributesNum();
    for (int32_t n = 0; n < q; n++)
    {
        ATTRIBUTES *pAImg = pA->GetAttributeClass(n);
        if (!pAImg)
            continue;
        FRECT rUV;
        FULLRECT(rUV);
        BIUtils::ReadRectFromAttr(pAImg, "uv", rUV, rUV);
        RECT rPos;
        ZERO(rPos);
        BIUtils::ReadRectFromAttr(pAImg, "pos", rPos, rPos);
        IBIImage *pCurImg =
            pImgRender->CreateImage(BIType_square, pAImg->GetAttribute("texture"),
                                    pAImg->GetAttributeAsDword("color", ARGB(255, 128, 128, 128)), rUV, rPos);
        if (pCurImg)
            images.push_back(pCurImg);
    }
}

void BIImagesInfo::Draw() const
{
    if (pImgRender)
        pImgRender->Render();
}

BIBorderInfo::BIBorderInfo() : dwColor1(0), dwColor2(0), fCur(0), fSpeed(0)
{
    pRS = nullptr;
    nVBuf = -1;
    nTexID = -1;
    bUp = true;
    bUsed = false;
}

BIBorderInfo::~BIBorderInfo()
{
    Release();
}

void BIBorderInfo::Release()
{
    VERTEX_BUFFER_RELEASE(pRS, nVBuf);
    TEXTURE_RELEASE(pRS, nTexID);
}

void BIBorderInfo::Init(VDX9RENDER *rs, ATTRIBUTES *pA)
{
    pRS = rs;
    nVBuf = rs->CreateVertexBuffer(BI_COLOR_VERTEX_FORMAT, 2 * 5 * sizeof(BI_COLOR_VERTEX), D3DUSAGE_WRITEONLY);
    dwColor1 = dwColor2 = ARGB(255, 255, 255, 255);
    ext_pos.left = 0.f;
    ext_pos.top = 0.f;
    ext_pos.right = 1024.f;
    ext_pos.bottom = 768.f;
    int_pos1.left = 20.f;
    int_pos1.top = 20.f;
    int_pos1.right = 1004.f;
    int_pos1.bottom = 748.f;
    int_pos2 = int_pos1;
    fCur = 0.f;
    fSpeed = 1.f;
    nTexID = -1;
    bUp = true;
    bUsed = true;
    if (!pA)
        return;
    dwColor1 = pA->GetAttributeAsDword("color1", dwColor1);
    dwColor2 = pA->GetAttributeAsDword("color2", dwColor2);
    BIUtils::ReadRectFromAttr(pA, "extpos", ext_pos, ext_pos);
    BIUtils::ReadRectFromAttr(pA, "intpos1", int_pos1, int_pos1);
    BIUtils::ReadRectFromAttr(pA, "intpos2", int_pos2, int_pos2);
    fSpeed = pA->GetAttributeAsFloat("speed", fSpeed * 1000.f) * 0.001f;
    nTexID = pRS->TextureCreate(pA->GetAttribute("texture"));
    bUsed = pA->GetAttributeAsDword("used", 0) != 0;
}

void BIBorderInfo::Draw()
{
    if (!bUsed || nVBuf < 0)
        return;
    auto *pV = static_cast<BI_COLOR_VERTEX *>(pRS->LockVertexBuffer(nVBuf));
    if (!pV)
        return;

    if (bUp)
    {
        fCur += fSpeed * core.GetDeltaTime();
        if (fCur > 1.f)
        {
            fCur = 1.f;
            bUp = false;
        }
    }
    else
    {
        fCur -= fSpeed * core.GetDeltaTime();
        if (fCur < 0.f)
        {
            fCur = 0.f;
            bUp = true;
        }
    }
    const uint32_t dwColor = BIUtils::GetIntervalColor(dwColor1, dwColor2, fCur);
    FRECT int_pos;
    BIUtils::GetIntervalRect(fCur, int_pos1, int_pos2, int_pos);

    for (int32_t n = 0; n < 10; n++)
    {
        pV[n].col = dwColor;
        pV[n].w = 0.5f;
        pV[n].pos.z = 1.f;
    }
    //
    pV[0].tu = pV[1].tu = pV[4].tu = pV[5].tu = pV[8].tu = pV[9].tu = 0.f;
    pV[2].tu = pV[3].tu = pV[6].tu = pV[7].tu = 0.96f;
    pV[0].tv = pV[2].tv = pV[4].tv = pV[6].tv = pV[8].tv = 0.f;
    pV[1].tv = pV[3].tv = pV[5].tv = pV[7].tv = pV[9].tv = 0.96f;
    //
    pV[0].pos.x = ext_pos.left;
    pV[0].pos.y = ext_pos.top;
    pV[1].pos.x = int_pos.left;
    pV[1].pos.y = int_pos.top;
    pV[2].pos.x = ext_pos.right;
    pV[2].pos.y = ext_pos.top;
    pV[3].pos.x = int_pos.right;
    pV[3].pos.y = int_pos.top;
    pV[4].pos.x = ext_pos.right;
    pV[4].pos.y = ext_pos.bottom;
    pV[5].pos.x = int_pos.right;
    pV[5].pos.y = int_pos.bottom;
    pV[6].pos.x = ext_pos.left;
    pV[6].pos.y = ext_pos.bottom;
    pV[7].pos.x = int_pos.left;
    pV[7].pos.y = int_pos.bottom;
    pV[8].pos.x = ext_pos.left;
    pV[8].pos.y = ext_pos.top;
    pV[9].pos.x = int_pos.left;
    pV[9].pos.y = int_pos.top;
    pRS->UnLockVertexBuffer(nVBuf);

    pRS->TextureSet(0, nTexID);
    pRS->DrawPrimitive(D3DPT_TRIANGLESTRIP, nVBuf, sizeof(BI_COLOR_VERTEX), 0, 8, "battle_msg");
}
