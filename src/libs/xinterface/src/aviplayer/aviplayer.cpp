#include "aviplayer.h"
#include <cstdio>

#include "core.h"

#include "entity.h"
#include "shared/interface/messages.h"

#define IRELEASE(i)                                                                                                    \
    {                                                                                                                  \
        if (i)                                                                                                         \
            i->Release();                                                                                              \
        i = NULL;                                                                                                      \
    }

#define VIDEO_DIRECTORY "resource\\videos"

int32_t AVI_GetTextureSize(int32_t width)
{
    int32_t i = 2;
    while ((width >>= 1L) > 0)
        i <<= 1L;
    return i;
}

CAviPlayer::CAviPlayer()
    : rs(nullptr), dstPnt(), lockRect(), v{}
{
    m_bContinue = true;
    m_bShowVideo = true;

#ifdef _WIN32 // FIX_LINUX ddraw.h and amstream.h
    pDD = nullptr;
    pPrimarySurface = nullptr;
    pVideoSurface = nullptr;
    pAMStream = nullptr;
    pPrimaryVidStream = nullptr;
    pDDStream = nullptr;
    pSample = nullptr;
#endif

    pTmpRenderTarget = nullptr;
    pTex = nullptr;
    m_bFirstDraw = true;
    m_bMakeUninitializeDD = false;
}

CAviPlayer::~CAviPlayer()
{
    ReleaseAll();
}

bool CAviPlayer::Init()
{
    if ((rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"))) == nullptr)
    {
        throw std::runtime_error("Can`t create render service");
    }

    // core.LayerCreate("vRealize",true,false);
    core.SetLayerType(VIDEO_REALIZE, layer_type_t::realize);
    core.AddToLayer(VIDEO_REALIZE, GetId(), -1);

    // core.LayerCreate("vExecute",true,false);
    core.SetLayerType(VIDEO_EXECUTE, layer_type_t::execute);
    core.AddToLayer(VIDEO_EXECUTE, GetId(), 1);

    if (!GetInterfaces())
    {
        m_bContinue = false;
        CleanupInterfaces();
    }

    return true;
}

void CAviPlayer::Execute(uint32_t delta_time)
{
    if (m_bContinue == false)
    {
#ifdef _WIN32 // FIX_LINUX ddraw.h and amstream.h
        if (pAMStream != nullptr)
            pAMStream->SetState(STREAMSTATE_STOP);
#endif
        CleanupInterfaces();
        core.Event("ievntEndVideo");
    }
}

void CAviPlayer::Realize(uint32_t delta_time)
{
    //~!~
    // rs->BeginScene();
#ifdef _WIN32 // FIX_LINUX ddraw.h and amstream.h
    int i;
    HRESULT hr;
    DDSURFACEDESC ddsd;
    ddsd.dwSize = sizeof(ddsd);
    if (pSample == nullptr)
    {
        m_bContinue = false;
        return;
    }

    if (m_bFirstDraw)
    {
        rs->Clear(0, nullptr, D3DCLEAR_TARGET, 0, 0, 0);
        m_bFirstDraw = false;
    }

    hr = pSample->Update(0, nullptr, nullptr, NULL);
    if (hr == S_OK)
    {
        hr = pVideoSurface->Lock(nullptr, &ddsd, 0, nullptr);
        if (hr != S_OK)
            return;

        D3DLOCKED_RECT d3dlkRect;
        hr = pTex->LockRect(0, &d3dlkRect, &lockRect, 0);
        if (hr != S_OK)
            return;

        auto nPitch = d3dlkRect.Pitch;
        auto *pOutData = static_cast<char *>(d3dlkRect.pBits);
        auto *pInData = static_cast<char *>(ddsd.lpSurface);
        const auto copySize = ddsd.lPitch < d3dlkRect.Pitch ? ddsd.lPitch : d3dlkRect.Pitch;
        for (i = 0; i < static_cast<int>(ddsd.dwHeight); i++)
        {
            memcpy(pOutData, pInData, copySize);
            pInData += ddsd.lPitch;
            pOutData += d3dlkRect.Pitch;
        }

        pTex->UnlockRect(0);
        pVideoSurface->Unlock(nullptr);

        if (m_bShowVideo)
        {
            rs->SetTexture(0, pTex);
            rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_AVIVIDEO_FVF, 2, v, sizeof(XI_AVIVIDEO_VERTEX), "battle_icons");
        }
    }
    else if (hr == MS_S_ENDOFSTREAM && bLoop)
    {
        CleanupInterfaces();
        if(!GetInterfaces() || !PlayMedia(filename.c_str()))
        {
            m_bContinue = false;
        }
    }
    else
    {
        m_bContinue = false;
    }
#else
    m_bContinue = false;
#endif
}

uint64_t CAviPlayer::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case MSG_SET_VIDEO_PLAY: {
        const std::string &param = message.String();
        const std::string vidName = fmt::format("{}\\{}", VIDEO_DIRECTORY, param);
        filename = vidName;
        if (!PlayMedia(vidName.c_str()))
        {
            CleanupInterfaces();
            core.PostEvent("ievntEndVideo", 1, nullptr);
        }
        break;
    }
    case MSG_SET_VIDEO_FLAGS: {
        constexpr auto loop_flag = 1 << 0;
        auto flags = message.Long();
        bLoop = flags & loop_flag;
        break;
    }
    }
    return 0;
}

bool CAviPlayer::PlayMedia(const char *fileName)
{
#ifdef _WIN32 // FIX_LINUX ddraw.h and amstream.h
    auto hr = S_OK;
    DDSURFACEDESC ddsd;

    WCHAR wPath[MAX_PATH]; // wide (32-bit) string name
    MultiByteToWideChar(CP_ACP, 0, fileName, -1, wPath, sizeof(wPath) / sizeof(wPath[0]));

    if (pAMStream == nullptr)
        return false;

    hr = pAMStream->OpenFile(wPath, 0);
    if (FAILED(hr))
    {
        core.Trace("Video Error!!!(0x%8x) Can`t load video file = %s", hr, fileName);
        return false;
    }

    hr = pAMStream->GetMediaStream(MSPID_PrimaryVideo, &pPrimaryVidStream);
    if (FAILED(hr))
    {
        core.Trace("Video Error!!! Can`t get media stream");
        return false;
    }
    hr = pPrimaryVidStream->QueryInterface(IID_IDirectDrawMediaStream, (void **)&pDDStream);
    if (FAILED(hr))
    {
        core.Trace("Video Error!!! Can`t query interface DirectDrawMediaStream");
        return false;
    }
    ddsd.dwSize = sizeof(ddsd);
    hr = pDDStream->GetFormat(&ddsd, nullptr, nullptr, nullptr);
    if (FAILED(hr))
    {
        core.Trace("Video Error!!! Can`t get stream format");
        return false;
    }
    int32_t srcWidth = ddsd.dwWidth;
    int32_t srcHeight = ddsd.dwHeight;
    hr = pDD->CreateSurface(&ddsd, &pVideoSurface, nullptr);
    if (FAILED(hr))
    {
        core.Trace("Video Error!!! Can`t create surface for video imaging");
        return false;
    }

    lockRect.left = 0;
    lockRect.top = 0;
    lockRect.right = srcWidth;
    lockRect.bottom = srcHeight;

    hr = pDDStream->CreateSample(static_cast<IDirectDrawSurface *>(pVideoSurface), nullptr, 0, &pSample);
    if (FAILED(hr))
    {
        core.Trace("Video Error!!! Can`t create sample for this video");
        return false;
    }

    RECT dstRect;
    GetWindowRect(static_cast<HWND>(core.GetWindow()->OSHandle()), &dstRect);
    auto dstWidth = dstRect.right - dstRect.left;
    auto dstHeight = dstRect.bottom - dstRect.top;

    auto horzK = static_cast<float>(dstWidth) / srcWidth;
    auto vertK = static_cast<float>(dstHeight) / srcHeight;
    if (horzK < vertK)
        vertK = horzK;
    else
        horzK = vertK;

    dstRect.left = dstRect.top = 0;
    dstRect.right = static_cast<int32_t>(srcWidth * horzK + .5f);
    dstRect.bottom = static_cast<int32_t>(srcHeight * vertK + .5f);

    dstRect.left += (dstWidth - dstRect.right) / 2;
    dstRect.top += (dstHeight - dstRect.bottom) / 2;
    dstRect.right += dstRect.left;
    dstRect.bottom += dstRect.top;

    D3DSURFACE_DESC d3d9surf_desc;

    IDirect3DSurface9 *pd3dsurf = nullptr;
    rs->GetRenderTarget(&pd3dsurf);
    pd3dsurf->GetDesc(&d3d9surf_desc);
    pd3dsurf->Release();

    hr = rs->CreateTexture(AVI_GetTextureSize(srcWidth), AVI_GetTextureSize(srcHeight), 1, 0, d3d9surf_desc.Format,
                           D3DPOOL_MANAGED, &pTex);
    if (FAILED(hr))
    {
        core.Trace("Video Error!!! Can`t create texture for this video");
        return false;
    }

    for (int i = 0; i < 4; i++)
    {
        v[i].w = .5f;
        v[i].pos.z = 1.f;
    }
    v[0].pos.x = static_cast<float>(dstRect.left);
    v[0].pos.y = static_cast<float>(dstRect.top);
    v[0].tu = 0.f;
    v[0].tv = 0.f;
    v[1].pos.x = static_cast<float>(dstRect.left);
    v[1].pos.y = static_cast<float>(dstRect.bottom);
    v[1].tu = 0.f;
    v[1].tv = static_cast<float>(lockRect.bottom) / AVI_GetTextureSize(srcHeight);
    v[2].pos.x = static_cast<float>(dstRect.right);
    v[2].pos.y = static_cast<float>(dstRect.top);
    v[2].tu = static_cast<float>(lockRect.right) / AVI_GetTextureSize(srcWidth);
    v[2].tv = 0.f;
    v[3].pos.x = static_cast<float>(dstRect.right), v[3].pos.y = static_cast<float>(dstRect.bottom);
    v[3].tu = static_cast<float>(lockRect.right) / AVI_GetTextureSize(srcWidth);
    v[3].tv = static_cast<float>(lockRect.bottom) / AVI_GetTextureSize(srcHeight);

    hr = pAMStream->SetState(STREAMSTATE_RUN);
    if (FAILED(hr))
    {
        core.Trace("Video Error!!! Can`t run media stream");
        return false;
    }

    return true;
#else
    return false;
#endif
}

bool CAviPlayer::GetInterfaces()
{
    HRESULT hr = S_OK;

#ifdef _WIN32 // FIX_LINUX ddraw.h and amstream.h
    // Initialize COM
    if (FAILED(hr = CoInitialize(NULL)))
        return false;
    m_bMakeUninitializeDD = true;

    hr = DirectDrawCreate(nullptr, &pDD, nullptr);
    if (FAILED(hr))
    {
        core.Trace("Video Error!!! Can`t create DirectDraw interface");
        return false;
    }
    hr = pDD->SetCooperativeLevel(static_cast<HWND>(core.GetWindow()->OSHandle()), DDSCL_NORMAL);
    if (FAILED(hr))
    {
        core.Trace("Video Error!!! Can`t SetCooperativeLevel for DirectDraw");
        return false;
    }
    hr = CoCreateInstance(CLSID_AMMultiMediaStream, nullptr, CLSCTX_INPROC_SERVER, IID_IAMMultiMediaStream,
                          (void **)&pAMStream);
    if (FAILED(hr))
    {
        core.Trace("Video Error!!! Can`t create interface AMMultiMediaStream");
        return false;
    }

    hr = pAMStream->Initialize(STREAMTYPE_READ, AMMSF_NOGRAPHTHREAD, nullptr);
    if (FAILED(hr))
    {
        core.Trace("Video Error!!! Can`t initialize interface AMMultiMediaStream");
        return false;
    }
    hr = pAMStream->AddMediaStream(pDD, &MSPID_PrimaryVideo, 0, nullptr);
    if (FAILED(hr))
    {
        core.Trace("Video Error!!! Can`t add video stream");
        return false;
    }
    hr = pAMStream->AddMediaStream(nullptr, &MSPID_PrimaryAudio, AMMSF_ADDDEFAULTRENDERER, nullptr);
    if (FAILED(hr))
    {
        core.Trace("Video Error!!! Can`t add audio stream");
        return false;
    }

    return true;
#else
    return false;
#endif
}

void CAviPlayer::CleanupInterfaces()
{
#ifdef _WIN32 // FIX_LINUX ddraw.h and amstream.h
    IRELEASE(pSample);
    IRELEASE(pDDStream);
    IRELEASE(pPrimaryVidStream);

    IRELEASE(pAMStream);
    IRELEASE(pVideoSurface);
    IRELEASE(pPrimarySurface);
    IRELEASE(pDD);

    IRELEASE(pTmpRenderTarget);

    if (pTex != nullptr && rs != nullptr)
        rs->Release(pTex);
    pTex = nullptr;

    if (m_bMakeUninitializeDD)
        CoUninitialize();
#endif
    m_bMakeUninitializeDD = false;
}

void CAviPlayer::ReleaseAll()
{
    CleanupInterfaces();
}
