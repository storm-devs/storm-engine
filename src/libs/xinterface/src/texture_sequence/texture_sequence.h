#pragma once

#include "video_texture.h"

//-----------------------------------------------------------------------------
// Name: class VideoToTexture
// Desc: play video into texture
//-----------------------------------------------------------------------------
class TextureSequence : public CVideoTexture
{
    VDX9RENDER *m_pRS;
    int32_t m_xQuantity;
    int32_t m_yQuantity;
    int32_t m_texWidth;
    int32_t m_texHeight;
    bool m_bCicled;
    uint32_t m_dwDeltaTime;
    int32_t m_maxCurNum;
    bool m_bHorzFlip;
    bool m_bVertFlip;

    int32_t m_curNum;
    uint32_t m_dwCurDeltaTime;

    int32_t m_AllTex;

    void ToTextureRender(float blendValue) const;

  public:
    TextureSequence();
    ~TextureSequence() override;

    IDirect3DTexture9 *Initialize(VDX9RENDER *pRS, const char *cTSfileName, bool bCicled) override;
    bool FrameUpdate() override;
    void Release() override;
    void LostRender() const;
    void RestoreRender();

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::lost_render:
            LostRender();
            break;
        case Stage::restore_render:
            RestoreRender();
            break;
        }
    }

    bool Init() override
    {
        return true;
    }
};
