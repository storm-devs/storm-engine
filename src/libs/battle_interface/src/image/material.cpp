#include "material.h"
#include "../bi_defines.h"
#include "image.h"
#include "img_render.h"

BIImageMaterial::BIImageMaterial(VDX9RENDER *pRS, BIImageRender *pImgRender)
{
    m_pRS = pRS;
    m_pImageRender = pImgRender;
    m_sTechniqueName = "battle_tex_col_Rectangle";

    m_nTextureID = -1;
    m_nVBufID = -1;
    m_nIBufID = -1;
    m_nVertexQuantity = 0;
    m_nTriangleQuantity = 0;

    m_bMakeBufferUpdate = false;
    m_bDeleteEverything = false;

    m_nMinPriority = ImagePrioritet_DefaultValue;
    m_nMaxPriority = ImagePrioritet_DefaultValue;
}

BIImageMaterial::~BIImageMaterial()
{
    Release();
}

void BIImageMaterial::Render(int32_t nBegPrior, int32_t nEndPrior)
{
    if (m_bMakeBufferUpdate)
        RemakeBuffers();

    size_t nStartIndex = 0;
    size_t nTriangleQuantity = m_nTriangleQuantity;
    if (!GetOutputRangeByPriority(nBegPrior, nEndPrior, nStartIndex, nTriangleQuantity))
        return;

    if (m_nTextureID >= 0 && m_nVBufID >= 0 && m_nIBufID >= 0)
    {
        m_pRS->TextureSet(0, m_nTextureID);
        m_pRS->DrawBuffer(m_nVBufID, sizeof(BI_IMAGE_VERTEX), m_nIBufID, 0, m_nVertexQuantity, nStartIndex,
                          nTriangleQuantity, m_sTechniqueName.c_str());
    }
}

const BIImage *BIImageMaterial::CreateImage(BIImageType type, uint32_t color, const FRECT &uv, int32_t nLeft, int32_t nTop,
                                            int32_t nRight, int32_t nBottom, int32_t nPrior)
{
    auto *pImg = new BIImage(m_pRS, this);
    Assert(pImg);
    pImg->SetColor(color);
    pImg->SetPosition(nLeft, nTop, nRight, nBottom);
    pImg->SetUV(uv);
    pImg->SetType(type);
    pImg->SetPrioritet(nPrior);
    InsertImageToList(pImg);
    // m_apImage.Add( pImg );
    m_bMakeBufferUpdate = true;
    RecalculatePriorityRange();
    return pImg;
}

void BIImageMaterial::DeleteImage(const BIImage *pImg)
{
    if (m_bDeleteEverything)
        return;
    for (int32_t n = 0; n < m_apImage.size(); n++)
        if (m_apImage[n] == pImg)
        {
            m_apImage.erase(m_apImage.begin() + n);
            m_bMakeBufferUpdate = true;
            RecalculatePriorityRange();
            break;
        }
}

void BIImageMaterial::SetTexture(const char *pcTextureName)
{
    if (pcTextureName == m_sTextureName)
        return; // this texture is already there
    m_sTextureName = pcTextureName;
    m_pRS->TextureRelease(m_nTextureID);
    m_nTextureID = m_pRS->TextureCreate(pcTextureName);
}

void BIImageMaterial::ReleaseAllImages()
{
    m_bDeleteEverything = true;
    for (const auto &image : m_apImage)
        delete image;
    m_apImage.clear();
    // m_apImage.DelAllWithPointers();
    m_bDeleteEverything = false;
    m_bMakeBufferUpdate = true;
}

void BIImageMaterial::Release()
{
    m_bDeleteEverything = true;
    m_pImageRender->DeleteMaterial(this);
    for (const auto &image : m_apImage)
        delete image;
    // m_apImage.DelAllWithPointers();
    TEXTURE_RELEASE(m_pRS, m_nTextureID);
    VERTEX_BUFFER_RELEASE(m_pRS, m_nVBufID);
    INDEX_BUFFER_RELEASE(m_pRS, m_nVBufID);

    m_nVertexQuantity = 0;
    m_nTriangleQuantity = 0;
}

void BIImageMaterial::UpdateImageBuffers(int32_t nStartIdx, size_t nEndIdx)
{
    // fool check
    if (m_nIBufID < 0 || m_nVBufID < 0)
        return;
    if (nStartIdx >= static_cast<int32_t>(m_apImage.size()))
        return;
    if (nEndIdx >= static_cast<int32_t>(m_apImage.size()))
        nEndIdx = m_apImage.size() - 1;

    auto *pT = static_cast<uint16_t *>(m_pRS->LockIndexBuffer(m_nIBufID));
    auto *pV = static_cast<BI_IMAGE_VERTEX *>(m_pRS->LockVertexBuffer(m_nVBufID));

    // get before
    size_t nV = 0;
    size_t nT = 0;
    int32_t n;
    for (n = 0; n < nStartIdx; n++)
    {
        nV += m_apImage[n]->GetVertexQuantity();
        nT += m_apImage[n]->GetTriangleQuantity();
    }

    for (; n <= nEndIdx; n++)
    {
        m_apImage[n]->FillBuffers(pV, pT, nV, nT);
    }

    m_pRS->UnLockVertexBuffer(m_nVBufID);
    m_pRS->UnLockIndexBuffer(m_nIBufID);
}

void BIImageMaterial::RemakeBuffers()
{
    size_t nVQ = 0;
    size_t nTQ = 0;
    for (int32_t n = 0; n < m_apImage.size(); n++)
    {
        nVQ += m_apImage[n]->GetVertexQuantity();
        nTQ += m_apImage[n]->GetTriangleQuantity();
    }

    if (nVQ == 0 || nTQ == 0)
        return;

    if (m_nVertexQuantity != nVQ)
    {
        m_pRS->ReleaseVertexBuffer(m_nVBufID);
        m_nVBufID =
            m_pRS->CreateVertexBuffer(BI_IMAGE_VERTEX_FORMAT, nVQ * sizeof(BI_IMAGE_VERTEX), D3DUSAGE_WRITEONLY);
        m_nVertexQuantity = nVQ;
    }
    if (m_nTriangleQuantity != nTQ)
    {
        m_pRS->ReleaseIndexBuffer(m_nIBufID);
        m_nIBufID = m_pRS->CreateIndexBuffer(nTQ * 3 * sizeof(uint16_t));
        m_nTriangleQuantity = nTQ;
    }
    UpdateImageBuffers(0, m_apImage.size() - 1);
    m_bMakeBufferUpdate = false;
}

bool BIImageMaterial::GetOutputRangeByPriority(int32_t nBegPrior, int32_t nEndPrior, size_t &nStartIndex,
                                               size_t &nTriangleQuantity)
{
    if (m_apImage.size() == 0)
        return false;
    if (m_apImage[0]->GetPrioritet() > nEndPrior || m_apImage.back()->GetPrioritet() < nBegPrior)
        return false;

    nStartIndex = 0;
    int32_t n;
    for (n = 0; n < m_apImage.size(); n++)
    {
        if (m_apImage[n]->GetPrioritet() >= nBegPrior)
            break;
        nStartIndex += m_apImage[n]->GetTriangleQuantity() * 3;
    }

    nTriangleQuantity = 0;
    for (; n < m_apImage.size(); n++)
    {
        if (m_apImage[n]->GetPrioritet() > nEndPrior)
            break;
        nTriangleQuantity += m_apImage[n]->GetTriangleQuantity();
    }

    return (nTriangleQuantity != 0);
}

void BIImageMaterial::RecalculatePriorityRange()
{
    if (m_apImage.size() == 0)
        return;
    const auto oldMin = m_nMinPriority;
    const auto oldMax = m_nMaxPriority;
    m_nMinPriority = m_nMaxPriority = m_apImage[0]->GetPrioritet();
    for (int32_t n = 1; n < m_apImage.size(); n++)
    {
        const auto p = m_apImage[n]->GetPrioritet();
        if (p < m_nMinPriority)
            m_nMinPriority = p;
        else if (p > m_nMaxPriority)
            m_nMaxPriority = p;
    }
    if (oldMin != m_nMinPriority || oldMax != m_nMaxPriority)
        m_pImageRender->MaterialSorting();
}

void BIImageMaterial::InsertImageToList(BIImage *pImg)
{
    Assert(pImg);
    const auto nPrior = pImg->GetPrioritet();
    int32_t n;
    for (n = 0; n < m_apImage.size(); n++)
        if (m_apImage[n]->GetPrioritet() > nPrior)
            break;

    m_apImage.insert(m_apImage.begin() + n, pImg);
    // m_apImage.Insert( n );
    // m_apImage[n] = pImg;
}
