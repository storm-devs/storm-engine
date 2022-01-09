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

    void Render(int32_t nBegPrior, int32_t nEndPrior);

    bool IsUseTexture(const char *pcTextureName) const
    {
        return (m_sTextureName == pcTextureName);
    }

    bool IsUseTechnique(const char *pcTechniqueName) const
    {
        return (m_sTechniqueName == pcTechniqueName);
    }

    const BIImage *CreateImage(BIImageType type, uint32_t color, const FRECT &uv, int32_t nLeft, int32_t nTop, int32_t nRight,
                               int32_t nBottom, int32_t nPrior);
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

    int32_t GetMinPriority() const
    {
        return m_nMinPriority;
    }

    int32_t GetMaxPriority() const
    {
        return m_nMaxPriority;
    }

    BIImageRender *GetImgRender() const
    {
        return m_pImageRender;
    }

  protected:
    void Release();
    void UpdateImageBuffers(int32_t nStartIdx, size_t nEndIdx);
    void RemakeBuffers();
    bool GetOutputRangeByPriority(int32_t nBegPrior, int32_t nEndPrior, size_t &nStartIndex, size_t &nTriangleQuantity);
    void RecalculatePriorityRange();
    void InsertImageToList(BIImage *pImg);

    VDX9RENDER *m_pRS;
    BIImageRender *m_pImageRender;

    std::string m_sTextureName;
    std::string m_sTechniqueName;

    int32_t m_nTextureID;
    int32_t m_nVBufID;
    int32_t m_nIBufID;
    size_t m_nVertexQuantity;
    size_t m_nTriangleQuantity;

    std::vector<BIImage *> m_apImage;

    int32_t m_nMinPriority;
    int32_t m_nMaxPriority;

    bool m_bMakeBufferUpdate;
    bool m_bDeleteEverything;
};
