#pragma once

#include "../inode.h"

enum ImagePointType
{
    IPType_LeftTop,
    IPType_Center,
    IPType_RightTop,

    IPType_ForceDword = 0xFFFFFFFF
};

class CXI_IMAGE
{
  public:
    CXI_IMAGE();
    ~CXI_IMAGE();

    void LoadFromFile(const char *sFileName);
    void LoadFromBase(const char *sListName, const char *sPictureName, bool bGetSizeFromSource = false);
    void LoadAccordingToString(const char *pcImageParam);
    void SetPointerToTexture(IDirect3DTexture9 *pTex);

    void Draw();
    void Draw(int32_t nX, int32_t nY, ImagePointType alignment);

    void SetSize(int32_t nWidth, int32_t nHeight);
    void SetPosition(int32_t nX, int32_t nY, ImagePointType ptype = IPType_LeftTop);
    void SetPosition(XYRECT &pos);
    void SetUV(float fLeft, float fTop, float fRight, float fBottom);
    void SetDifferentPosition(int32_t nLeft, int32_t nTop, int32_t nWidth, int32_t nHeight, ImagePointType ptype = IPType_LeftTop);

    void SetColor(uint32_t dwColor);

    uint32_t GetColor() const
    {
        return m_dwImageColor;
    }

    int32_t GetWidth() const
    {
        return m_pntSize.x;
    }

    int32_t GetHeight() const
    {
        return m_pntSize.y;
    }

    void Unload();

    void SetThisRectangleNotUseTexture(bool bUseOnlyColor)
    {
        m_bThisIsColorRectangle = bUseOnlyColor;
    }

    bool IsImagePresent() const
    {
        return (m_nTextureID >= 0 || m_pTexture);
    }

    bool IsPointInside(int32_t nX, int32_t nY) const;

    void DisableDraw(bool bDisable)
    {
        m_bDisableDraw = bDisable;
    }

  protected:
    VDX9RENDER *m_rs;

    bool m_bDisableDraw;
    char *m_pcPictureListName;
    int32_t m_nPictureNum;
    FXYPOINT m_nLeftTopCutUV;

    int32_t m_nTextureID;
    IDirect3DTexture9 *m_pTexture;

    FXYRECT m_frUV;

    XI_ONETEX_VERTEX m_vrtx[4];
    uint32_t m_dwImageColor;

    XYPOINT m_pntSize;
    XYPOINT m_pntLeftTop;

    bool m_bThisIsColorRectangle;

    void UpdateTexture();
    void UpdatePosition();
};
