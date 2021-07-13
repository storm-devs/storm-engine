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
    void Draw(long nX, long nY, ImagePointType alignment);

    void SetSize(long nWidth, long nHeight);
    void SetPosition(long nX, long nY, ImagePointType ptype = IPType_LeftTop);
    void SetPosition(XYRECT &pos);
    void SetUV(float fLeft, float fTop, float fRight, float fBottom);
    void SetDifferentPosition(long nLeft, long nTop, long nWidth, long nHeight, ImagePointType ptype = IPType_LeftTop);

    void SetColor(uint32_t dwColor);
    uint32_t GetColor() const
    {
        return m_dwImageColor;
    }

    long GetWidth() const
    {
        return m_pntSize.x;
    }
    long GetHeight() const
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

    bool IsPointInside(long nX, long nY) const;

    void DisableDraw(bool bDisable)
    {
        m_bDisableDraw = bDisable;
    }

  protected:
    VDX9RENDER *m_rs;

    bool m_bDisableDraw;
    char *m_pcPictureListName;
    long m_nPictureNum;
    FXYPOINT m_nLeftTopCutUV;

    long m_nTextureID;
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
