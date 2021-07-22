#include "imgrender.h"
#include "storm_assert.h"
#include "string.h"

BIImageRender::BIImageRender(VDX9RENDER *pRS)
{
    m_pRS = pRS;
    m_fHScale = m_fVScale = 1.f;
    m_fHOffset = m_fVOffset = 0.f;
}

BIImageRender::~BIImageRender()
{
    Release();
}

void BIImageRender::Render()
{
    if (GetFirstPrioritetRange())
        do
        {
            CutPrioritetRangeByStrings();

            for (long n = 0; n < m_apMaterial.size(); n++)
                m_apMaterial[n]->Render(m_nBeginOutputPrioritet, m_nEndOutputPrioritet);

            for (long n = 0; n < m_apStrings.size(); n++)
                static_cast<BIString *>(m_apStrings[n])
                    ->RenderPrioritetRange(m_nBeginOutputPrioritet, m_nEndOutputPrioritet);
        } while (GetNextPrioritetRange());
}

uint64_t BIImageRender::ProcessMessage(MESSAGE &message)
{
    return 0;
}

IBIImage *BIImageRender::CreateImage(BIImageType type, const char *pcTextureName, uint32_t color, const FRECT &uv,
                                     long nLeft, long nTop, long nRight, long nBottom, long nPrior,
                                     const char *pcTechniqueName)
{
    auto *pMaterial = CreateMaterial(pcTextureName, pcTechniqueName);
    if (pMaterial)
        return (IBIImage *)pMaterial->CreateImage(type, color, uv, nLeft, nTop, nRight, nBottom, nPrior);
    return nullptr;
}

IBIImage *BIImageRender::CreateImage(BIImageType type, const char *pcTextureName, uint32_t color, const FRECT &uv,
                                     const RECT &pos, long nPrior, const char *pcTechniqueName)
{
    auto *pMaterial = CreateMaterial(pcTextureName, pcTechniqueName);
    if (pMaterial)
        return (IBIImage *)pMaterial->CreateImage(type, color, uv, pos.left, pos.top, pos.right, pos.bottom, nPrior);
    return nullptr;
}

BIImageMaterial *BIImageRender::FindMaterial(const char *pcTextureName, const char *pcTechniqueName)
{
    for (long n = 0; n < m_apMaterial.size(); n++)
        if (m_apMaterial[n]->IsUseTexture(pcTextureName) && m_apMaterial[n]->IsUseTechnique(pcTechniqueName))
            return m_apMaterial[n];
    return nullptr;
}

BIImageMaterial *BIImageRender::CreateMaterial(const char *pcTextureName, const char *pcTechniqueName)
{
    auto *pMaterial = FindMaterial(pcTextureName, pcTechniqueName ? pcTechniqueName : "battle_tex_col_Rectangle");
    if (!pMaterial)
    {
        pMaterial = new BIImageMaterial(m_pRS, this);
        Assert(pMaterial);
        pMaterial->SetTexture(pcTextureName);
        pMaterial->SetTechnique(pcTechniqueName);
        pMaterial->UpdateFlagOn();
        long n;
        for (n = 0; n < m_apMaterial.size(); n++)
            if (m_apMaterial[n]->GetMinPriority() > pMaterial->GetMinPriority())
                break;
        m_apMaterial.insert(m_apMaterial.begin() + n, pMaterial);
        // m_apMaterial.Insert(n);
        // m_apMaterial[n] = pMaterial;
    }
    return pMaterial;
}

void BIImageRender::DeleteMaterial(BIImageMaterial *pMat)
{
    // long n = m_apMaterial.Find( pMat );
    // if( n >= 0 )
    //    m_apMaterial.DelIndex( n );
    const auto it = std::find(m_apMaterial.begin(), m_apMaterial.end(), pMat);
    if (it != m_apMaterial.end())
        m_apMaterial.erase(it);
}

void BIImageRender::ReleaseAllImages()
{
    for (long n = 0; n < m_apMaterial.size(); n++)
        m_apMaterial[n]->ReleaseAllImages();
}

size_t BIImageRender::GetImageQuantity()
{
    size_t nRetVal = 0;
    for (long n = 0; n < m_apMaterial.size(); n++)
        nRetVal += m_apMaterial[n]->GetImageQuantity();
    return nRetVal;
}

void BIImageRender::MaterialSorting()
{
    for (auto bContinue = true; bContinue;)
    {
        bContinue = false;
        for (long n = 1; n < m_apMaterial.size(); n++)
        {
            if (m_apMaterial[n]->GetMinPriority() < m_apMaterial[n - 1]->GetMinPriority())
            {
                std::swap(m_apMaterial[n - 1], m_apMaterial[n]);
                // m_apMaterial.Swap(n-1, n);
                bContinue = true;
            }
        }
    }
}

void BIImageRender::ChangeMaterialPosByPrioritet(BIImageMaterial *pMat)
{
    MaterialSorting();
}

IBIString *BIImageRender::CreateString(const char *text, const char *font_name, float font_scale, uint32_t font_color,
                                       long valign, long halign, long nLeft, long nTop, long nRight, long nBottom,
                                       long nPrior)
{
    auto *pStr = new BIString(this, m_pRS);
    if (!pStr)
        return pStr;

    pStr->SetFont(font_name);
    pStr->SetColor(font_color);
    pStr->SetAlign(halign, valign);
    pStr->SetPosition(nLeft, nTop, nRight, nBottom);
    pStr->SetPrioritet(nPrior);
    pStr->SetScale(font_scale);
    pStr->SetString(text);

    return pStr;
}

IBIString *BIImageRender::CreateString(const char *text, const char *font_name, float font_scale, uint32_t font_color,
                                       long valign, long halign, const RECT &pos, long nPrior)
{
    return CreateString(text, font_name, font_scale, font_color, valign, halign, pos.left, pos.top, pos.right,
                        pos.bottom, nPrior);
}

void BIImageRender::DeleteString(IBIString *str)
{
    // long n = m_apStrings.Find( str );
    // if( n >= 0 )
    //    m_apStrings.DelIndex( n );
    const auto it = std::find(m_apStrings.begin(), m_apStrings.end(), str);
    if (it != m_apStrings.end())
        m_apStrings.erase(it);
}

void BIImageRender::CutPrioritetRangeByStrings()
{
    for (long n = 0; n < m_apStrings.size(); n++)
    {
        const auto iprior = static_cast<BIString *>(m_apStrings[n])->GetPrioritet();
        if (iprior < m_nBeginOutputPrioritet)
            continue;
        if (iprior > m_nEndOutputPrioritet)
            continue;
        m_nEndOutputPrioritet = iprior;
    }
}

void BIImageRender::SetBaseScreenSize(long nHSize, long nVSize, long nHOffset, long nVOffset)
{
    D3DVIEWPORT9 vp;
    m_pRS->GetViewport(&vp);

    if (vp.Width == nHSize && nHOffset == 0)
    {
        m_fHScale = 1.f;
        m_fHOffset = 0.f;
    }
    else
    {
        m_fHScale = static_cast<float>(vp.Width) / (nHSize + 2 * nHOffset);
    }

    if (vp.Height == nVSize && nVOffset == 0)
    {
        m_fVScale = 1.f;
        m_fVOffset = 0.f;
    }
    else
    {
        m_fVScale = static_cast<float>(vp.Height) / (nVSize + 2 * nVOffset);
    }
}

void BIImageRender::Release()
{
    // destructors themselves erase pointers from vectors

    while (m_apMaterial.size() > 0)
        delete m_apMaterial.front();

    while (m_apStrings.size() > 0)
        delete m_apStrings.front();
}

bool BIImageRender::GetFirstPrioritetRange()
{
    if (m_apMaterial.size() == 0)
        return false;
    m_nBeginOutputPrioritet = m_apMaterial[0]->GetMinPriority();
    m_nEndOutputPrioritet = m_apMaterial[0]->GetMaxPriority();

    if (m_apMaterial.size() > 1 && m_nEndOutputPrioritet > m_apMaterial[1]->GetMinPriority())
        m_nEndOutputPrioritet = m_apMaterial[1]->GetMinPriority();
    return true;
}

bool BIImageRender::GetNextPrioritetRange()
{
    m_nBeginOutputPrioritet = ++m_nEndOutputPrioritet;
    long n;
    for (n = 0; n < m_apMaterial.size(); n++)
    {
        if (m_apMaterial[n]->GetMaxPriority() >= m_nBeginOutputPrioritet)
        {
            m_nEndOutputPrioritet = m_apMaterial[n]->GetMaxPriority();
            for (long i = n + 1; i < m_apMaterial.size(); i++)
                if (m_nBeginOutputPrioritet < m_apMaterial[i]->GetMinPriority())
                    m_nEndOutputPrioritet = m_apMaterial[i]->GetMinPriority();
            break;
        }
    }
    return (n < m_apMaterial.size());
}
