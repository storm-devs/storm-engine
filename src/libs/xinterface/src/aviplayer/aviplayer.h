#pragma once

#include "../base_video.h"
#include <amstream.h>
#include <ddraw.h>

#define XI_AVIVIDEO_FVF (D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

struct XI_AVIVIDEO_VERTEX
{
    CVECTOR pos;
    float w;
    float tu, tv;
};

class CAviPlayer : public xiBaseVideo
{
    VDX9RENDER *rs;
    bool m_bShowVideo;

  public:
    CAviPlayer();
    ~CAviPlayer() override;
    bool Init() override;
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
    uint64_t ProcessMessage(MESSAGE &message) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    void SetShowVideo(bool bShowVideo) override
    {
        m_bShowVideo = bShowVideo;
    }

    IDirect3DTexture9 *GetCurrentVideoTexture() override
    {
        return pTex;
    }

  protected:
    bool m_bContinue;

    IDirectDraw *pDD;
    IDirectDrawSurface *pPrimarySurface;
    IDirectDrawSurface *pVideoSurface;

    IAMMultiMediaStream *pAMStream;
    IMediaStream *pPrimaryVidStream;
    IDirectDrawMediaStream *pDDStream;
    IDirectDrawStreamSample *pSample;

    POINT dstPnt;
    RECT lockRect;

    XI_AVIVIDEO_VERTEX v[4];

    IDirect3DSurface9 *pTmpRenderTarget;
    IDirect3DTexture9 *pTex;

    void ReleaseAll();
    bool PlayMedia(const char *fileName);
    bool GetInterfaces();
    void CleanupInterfaces();

    bool m_bFirstDraw;
    bool m_bMakeUninitializeDD;

    std::string filename;
    bool bLoop{false};
};
