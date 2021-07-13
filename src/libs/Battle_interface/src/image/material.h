#pragma once

#include "image_defines.h"
#include <string>
#include <vector>

class BIImage;
class BIImageRender;

class BIImageMaterial
{
  public:
    BIImageMaterial(VDX9RENDER *pRS, BIImageRender *pImgRender);
    ~BIImageMaterial();

    void Render(long nBegPrior, long nEndPrior);

    bool IsUseTexture(const char *pcTextureName) const
    {
        return (m_sTextureName == pcTextureName);
    }
    bool IsUseTechnique(const char *pcTechniqueName) const
    {
        return (m_sTechniqueName == pcTechniqueName);
    }

    const BIImage *CreateImage(BIImageType type, uint32_t color, const FRECT &uv, long nLeft, long nTop, long nRight,
                               long nBottom, long nPrior);
    void DeleteImage(const BIImage *pImg);

    void SetTexture(const char *pcTextureName);
    void SetTechnique(const char *pcTechniqueName)
    {
        if (pcTechniqueName)
            m_sTechniqueName = pcTechniqueName;
    }

    void UpdateFlagOn()
    {
        m_bMakeBufferUpdate = true;
    }

    size_t GetImageQuantity() const
    {
        return m_apImage.size();
    }
    void ReleaseAllImages();

    long GetMinPriority() const
    {
        return m_nMinPriority;
    }
    long GetMaxPriority() const
    {
        return m_nMaxPriority;
    }

    BIImageRender *GetImgRender() const
    {
        return m_pImageRender;
    }

  protected:
    void Release();
    void UpdateImageBuffers(long nStartIdx, size_t nEndIdx);
    void RemakeBuffers();
    bool GetOutputRangeByPriority(long nBegPrior, long nEndPrior, size_t &nStartIndex, size_t &nTriangleQuantity);
    void RecalculatePriorityRange();
    void InsertImageToList(BIImage *pImg);

    VDX9RENDER *m_pRS;
    BIImageRender *m_pImageRender;

    std::string m_sTextureName;
    std::string m_sTechniqueName;

    long m_nTextureID;
    long m_nVBufID;
    long m_nIBufID;
    size_t m_nVertexQuantity;
    size_t m_nTriangleQuantity;

    std::vector<BIImage *> m_apImage;

    long m_nMinPriority;
    long m_nMaxPriority;

    bool m_bMakeBufferUpdate;
    bool m_bDeleteEverything;
};
