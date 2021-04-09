#pragma once

#include "image_defines.h"
#include <string>

class BIImageRender;

class BIString : public IBIString
{
  public:
    BIString(BIString &&) = delete;
    BIString(const BIString &) = delete;
    BIString(BIImageRender *pImgRender, VDX9RENDER *rs);
    ~BIString();

    long GetPrioritet() const
    {
        return m_nPrioritet;
    }
    void SetPrioritet(long nPrior)
    {
        m_nPrioritet = nPrior;
    }

    void Render();

    void RenderPrioritetRange(long nBegPrior, long nEndPrior)
    {
        if (m_nPrioritet >= nBegPrior && m_nPrioritet <= nEndPrior)
            Render();
    }

    void SetColor(uint32_t color) override
    {
        m_dwColor = color;
    }
    void SetScale(float fScale) override
    {
        m_fScale = fScale;
    }
    void SetFont(const char *pcFontName) override;
    void SetAlign(long nHorzAlign, long nVertAlign) override;
    void SetPosition(long nLeft, long nTop, long nRight, long nBottom) override;
    void SetString(const char *pcStr) override;

  protected:
    void Release();
    void UpdateString();

    BIImageRender *m_pImgRender;
    VDX9RENDER *m_pRS;
    FRECT m_BasePos;
    long m_nPrioritet;
    std::string m_sText;

    uint32_t m_dwColor;
    float m_fScale;
    long m_nFontID;

    POINT m_pntOutPos;

    bool m_bUpdateString;
};
