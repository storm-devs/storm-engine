#include "../xinterface.h"
#include "font.h"

GIFont::GIFont(GIEditor *pEditor, const char *pcFontName)
{
    m_pEditor = pEditor;
    Assert(m_pEditor);

    m_nFontID = m_pEditor->m_pRS->LoadFont((char *)pcFontName);

    m_dwColor = 0xFF000000;
    m_dwBackColor = 0;
    SetHeight(16.f);
}

GIFont::~GIFont()
{
    Release();
}

void GIFont::Release() const
{
    if (m_nFontID >= 0)
        m_pEditor->m_pRS->UnloadFont(m_nFontID);
}

void GIFont::Print(float x, float y, const char *pcFormat, ...) const
{
    char param[4096];
    va_list args;
    va_start(args, pcFormat);
    vsnprintf(param, sizeof(param), pcFormat, args);
    va_end(args);
    m_pEditor->m_pRS->ExtPrint(m_nFontID, m_dwColor, m_dwBackColor, PR_ALIGN_LEFT, false, m_fScale,
                               m_pEditor->m_pGIOwner->GetScreenWidth(), m_pEditor->m_pGIOwner->GetScreenHeight(),
                               static_cast<int32_t>(x), static_cast<int32_t>(y), "%s", param);
}

void GIFont::PrintIntoWindow(const char *pcFormat, ...)
{
}

void GIFont::SetHeight(float fHeight)
{
    m_fHeight = fHeight;
    const auto fRealH = static_cast<float>(m_pEditor->m_pRS->CharHeight(m_nFontID));
    if (fRealH > 0.f)
        m_fScale = m_fHeight / fRealH;
}

void GIFont::SetColor(uint32_t dwColor)
{
    m_dwColor = dwColor;
}

void GIFont::SetBackColor(uint32_t dwColor)
{
    m_dwBackColor = dwColor;
}
