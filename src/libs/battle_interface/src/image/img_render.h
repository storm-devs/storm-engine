#pragma once

#include "material.h"

class BIImageRender
{
  public:
    BIImageRender(VDX9RENDER *pRS);
    ~BIImageRender();

    void Render();
    uint64_t ProcessMessage(MESSAGE &message);

    IBIImage *CreateImage(BIImageType type, const char *pcTextureName, uint32_t color, const FRECT &uv, int32_t nLeft,
                          int32_t nTop, int32_t nRight, int32_t nBottom, int32_t nPrior = ImagePrioritet_DefaultValue,
                          const char *pcTechniqueName = nullptr);
    IBIImage *CreateImage(BIImageType type, const char *pcTextureName, uint32_t color, const FRECT &uv, const RECT &pos,
                          int32_t nPrior = ImagePrioritet_DefaultValue, const char *pcTechniqueName = nullptr);

    BIImageMaterial *FindMaterial(const char *pcTextureName, const char *pcTechniqueName);
    BIImageMaterial *CreateMaterial(const char *pcTextureName, const char *pcTechniqueName = nullptr);
    void DeleteMaterial(BIImageMaterial *pMat);

    void ReleaseAllImages();
    size_t GetImageQuantity();

    void MaterialSorting();
    void ChangeMaterialPosByPrioritet(BIImageMaterial *pMat);

    void TranslateBasePosToRealPos(float fXBase, float fYBase, float &fXReal, float &fYReal) const;

    IBIString *CreateString(const char *text, const char *font_name, float font_scale, uint32_t font_color, int32_t valign,
                            int32_t halign, int32_t nLeft, int32_t nTop, int32_t nRight, int32_t nBottom,
                            int32_t nPrior = ImagePrioritet_DefaultValue);
    IBIString *CreateString(const char *text, const char *font_name, float font_scale, uint32_t font_color, int32_t valign,
                            int32_t halign, const RECT &pos, int32_t nPrior = ImagePrioritet_DefaultValue);
    void DeleteString(IBIString *str);
    void CutPrioritetRangeByStrings();

    void SetBaseScreenSize(int32_t nHSize, int32_t nVSize, int32_t nHOffset, int32_t nVOffset);

  protected:
    void Release();
    bool GetFirstPrioritetRange();
    bool GetNextPrioritetRange();

    int32_t m_nBeginOutputPrioritet;
    int32_t m_nEndOutputPrioritet;

    VDX9RENDER *m_pRS;
    std::vector<BIImageMaterial *> m_apMaterial;
    std::vector<IBIString *> m_apStrings;
    // bool m_bDeleteEverything;

    float m_fHScale, m_fVScale;
    float m_fHOffset, m_fVOffset;
};

inline void BIImageRender::TranslateBasePosToRealPos(float fXBase, float fYBase, float &fXReal, float &fYReal) const
{
    fXReal = (m_fHOffset + fXBase) * m_fHScale;
    fYReal = (m_fVOffset + fYBase) * m_fVScale;
}
