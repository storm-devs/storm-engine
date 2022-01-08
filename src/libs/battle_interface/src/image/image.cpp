#include "image.h"
#include "img_render.h"

BIImage::BIImage(VDX9RENDER *rs, BIImageMaterial *pMaterial)
    : m_BasePos(), m_BaseUV(), m_dwColor(0), m_eType()
{
    m_pRS = rs;
    m_pMaterial = pMaterial;
    m_nVertexQuantity = 4;
    m_nTriangleQuantity = 2;
    m_nPrioritet = ImagePrioritet_DefaultValue;
}

BIImage::~BIImage()
{
    Release();
}

void BIImage::FillBuffers(BI_IMAGE_VERTEX *pV, uint16_t *pT, size_t &nV, size_t &nT)
{
    int32_t n;
    auto ni = nT * 3;
    auto nv = nV;

    // index buffer
    if (m_eType == BIType_square)
    {
        for (n = 2; n < m_aRelPos.size(); n++) // the square is made according to the Triangle Strip principle
        {
            pT[ni++] = static_cast<uint16_t>(nv + n - 2);
            pT[ni++] = static_cast<uint16_t>(nv + n - 1);
            pT[ni++] = static_cast<uint16_t>(nv + n);
        }
        nT += n - 2;
    }
    else if (m_eType == BIType_clocksquare)
    {
        // square "clocks" are made according to the Triangle Fan principle
        for (n = 2; n < m_aRelPos.size(); n++)
        {
            pT[ni++] = static_cast<uint16_t>(nv);
            pT[ni++] = static_cast<uint16_t>(nv + n - 1);
            pT[ni++] = static_cast<uint16_t>(nv + n);
        }
        nT += n - 2;
    }
    else
        return;

    // vertex buffer
    for (n = 0; n < m_aRelPos.size(); n++)
    {
        pV[nv].pos.x = CalculateMidPos(m_BasePos.left, m_BasePos.right, m_aRelPos[n].x);
        pV[nv].pos.y = CalculateMidPos(m_BasePos.top, m_BasePos.bottom, m_aRelPos[n].y);
        pV[nv].pos.z = 1.f;
        pV[nv].w = 0.5f;
        pV[nv].col = m_dwColor;
        pV[nv].tu = CalculateMidPos(m_BaseUV.left, m_BaseUV.right, m_aRelPos[n].x);
        pV[nv].tv = CalculateMidPos(m_BaseUV.top, m_BaseUV.bottom, m_aRelPos[n].y);
        nv++;
    }
    nV += n;
}

void BIImage::SetColor(uint32_t color)
{
    m_dwColor = color;
    m_pMaterial->UpdateFlagOn();
}

void BIImage::SetPosition(int32_t nLeft, int32_t nTop, int32_t nRight, int32_t nBottom)
{
    m_pMaterial->GetImgRender()->TranslateBasePosToRealPos(static_cast<float>(nLeft), static_cast<float>(nTop),
                                                           m_BasePos.left, m_BasePos.top);
    m_pMaterial->GetImgRender()->TranslateBasePosToRealPos(static_cast<float>(nRight), static_cast<float>(nBottom),
                                                           m_BasePos.right, m_BasePos.bottom);
    m_pMaterial->UpdateFlagOn();
}

void BIImage::Set3DPosition(const CVECTOR &vPos, float fWidth, float fHeight)
{
}

void BIImage::SetUV(const FRECT &uv)
{
    m_BaseUV = uv;
    m_pMaterial->UpdateFlagOn();
}

void BIImage::SetType(BIImageType type)
{
    m_eType = type;
    if (type == BIType_square)
    {
        m_nVertexQuantity = 4;
        m_nTriangleQuantity = 2;
        CutSide(0.f, 0.f, 0.f, 0.f);
    }
    if (type == BIType_clocksquare)
    {
        m_nVertexQuantity = 3;
        m_nTriangleQuantity = 1;
        CutClock(0.f, 1.f, 0.5f);
    }
    m_pMaterial->UpdateFlagOn();
}

void BIImage::CutSide(float fleft, float fright, float ftop, float fbottom)
{
    if (m_eType != BIType_square)
        return;
    m_aRelPos.clear();
    FPOINT fp;
    fp.x = fleft;
    fp.y = 1.f - fbottom;
    m_aRelPos.push_back(fp);
    fp.x = fleft;
    fp.y = ftop;
    m_aRelPos.push_back(fp);
    fp.x = 1.f - fright;
    fp.y = 1.f - fbottom;
    m_aRelPos.push_back(fp);
    fp.x = 1.f - fright;
    fp.y = ftop;
    m_aRelPos.push_back(fp);
    m_pMaterial->UpdateFlagOn();
}

void BIImage::CutClock(float fBegin, float fEnd, float fFactor)
{
    if (m_eType != BIType_clocksquare)
        return;
    m_aRelPos.clear();
    FPOINT fp;
    fp.x = 0.5f;
    fp.y = 0.5f;
    m_aRelPos.push_back(fp);
    const auto fEndAng = fBegin + (fEnd - fBegin) * fFactor;
    // first / starting point on the clocks
    m_aRelPos.push_back(GetClockPoint(fBegin, fp));
    // next angles
    if (fBegin < fEndAng)
        for (auto fAng = GetNextClockCorner(fBegin); fAng < fEndAng; fAng = GetNextClockCorner(fAng))
            m_aRelPos.push_back(GetClockPoint(fAng, fp));
    else if (fBegin > fEndAng)
        for (auto fAng = GetPrevClockCorner(fBegin); fAng > fEndAng; fAng = GetPrevClockCorner(fAng))
            m_aRelPos.push_back(GetClockPoint(fAng, fp));
    // last / ending point on the clocks
    m_aRelPos.push_back(GetClockPoint(fEndAng, fp));
    m_nVertexQuantity = m_aRelPos.size();
    m_nTriangleQuantity = m_nVertexQuantity - 2;
    m_pMaterial->UpdateFlagOn();
}

FPOINT &BIImage::GetClockPoint(float fAng, FPOINT &fp)
{
    if (fAng < -1.f)
    {
        fp.x = 0.5f;
        fp.y = 0.f;
    }
    else if (fAng <= -0.875f)
    {
        fp.x = (1.125f + fAng) * 4.f;
        fp.y = 0.f;
    }
    else if (fAng <= -0.625f)
    {
        fp.x = 1.f;
        fp.y = (0.875f + fAng) * 4.f;
    }
    else if (fAng <= -0.375f)
    {
        fp.x = (-0.375f - fAng) * 4.f;
        fp.y = 1.f;
    }
    else if (fAng <= -0.125f)
    {
        fp.x = 0.f;
        fp.y = (-0.125f - fAng) * 4.f;
    }
    else if (fAng <= 0.125f)
    {
        fp.x = (0.125f + fAng) * 4.f;
        fp.y = 0.f;
    }
    else if (fAng <= 0.375f)
    {
        fp.x = 1.f;
        fp.y = (-0.125f + fAng) * 4.f;
    }
    else if (fAng <= 0.625f)
    {
        fp.x = (0.625f - fAng) * 4.f;
        fp.y = 1.f;
    }
    else if (fAng <= 0.875f)
    {
        fp.x = 0.f;
        fp.y = (0.875f - fAng) * 4.f;
    }
    else if (fAng <= 1.f)
    {
        fp.x = (fAng - 0.875f) * 4.f;
        fp.y = 0.f;
    }
    else
    {
        fp.x = 0.5f;
        fp.y = 0.f;
    }

    return fp;
}

float BIImage::GetNextClockCorner(float fAng)
{
    for (auto f = -0.875f; f < 1.f; f += 0.25f)
        if (fAng < f)
            return f;
    return fAng + .25f;
}

float BIImage::GetPrevClockCorner(float fAng)
{
    for (auto f = 0.875f; f > -1.f; f -= 0.25f)
        if (fAng > f)
            return f;
    return fAng - .25f;
}

void BIImage::Release() const
{
    m_pMaterial->DeleteImage(this);
}
