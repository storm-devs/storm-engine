#pragma once
#include "dx9render.h"

//-----------------------------------------------------------------------------
// Name: class VideoToTexture
// Desc: play video into texture
//-----------------------------------------------------------------------------
class CVideoTexture : public Entity
{
  public:
    virtual IDirect3DTexture9 *Initialize(VDX9RENDER *pRS, const char *sFileName, bool bCicled) = 0;
    virtual bool FrameUpdate() = 0;
    virtual void Release() = 0;
    IDirect3DTexture9 *m_pTexture;

    operator IDirect3DTexture9 *() const
    {
        return m_pTexture;
    }

    operator IDirect3DBaseTexture9 *() const
    {
        return m_pTexture;
    }
};
