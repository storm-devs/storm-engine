#pragma once

#ifdef _WIN32 // Effects
#include "effects.h"
#else
#include "technique.h"
#endif
#include "font.h"
#include "video_texture.h"
#include "dx9render.h"
#include "vma.hpp"
#include "platform/platform.hpp"

#include "d3d9types.h"
#include "script_libriary.h"

#include <stack>
#include <vector>

#define MAX_STEXTURES 10240
#define MAX_BUFFERS 10240
#define MAX_FONTS 256

struct D3DERRORS
{
    HRESULT err;
    char *descrition;
};

struct texpaths_t
{
    char str[MAX_PATH];
};

struct STEXTURE
{
    IDirect3DBaseTexture9 *d3dtex;
    char *name;
    uint32_t hash;
    int32_t ref;
    uint32_t dwSize;
    bool isCubeMap;
    bool loaded;
};

//-----------buffers-----------
struct VERTEX_BUFFER
{
    uint32_t dwNumLocks;
    uint32_t dwUsage;
    int32_t type;
    int32_t size;
    IDirect3DVertexBuffer9 *buff;
};

struct INDEX_BUFFER
{
    uint32_t dwNumLocks;
    uint32_t dwUsage;
    int32_t size;
    IDirect3DIndexBuffer9 *buff;
};

struct FONTEntity
{
    char *name;
    uint32_t hash;
    FONT *font;
    int32_t ref;
};

struct VideoTextureEntity
{
    char *name;
    uint32_t hash;
    int32_t ref;
    entid_t videoTexture_id;
    CVideoTexture *VideoTexture;
    VideoTextureEntity *next;
};

// ----- Sound statistics -----
extern uint32_t dwSoundBuffersCount;
extern uint32_t dwSoundBytes;
extern uint32_t dwSoundBytesCached;

class DX9RENDER_SCRIPT_LIBRIARY : public SCRIPT_LIBRIARY
{
  public:
    DX9RENDER_SCRIPT_LIBRIARY(){};

    ~DX9RENDER_SCRIPT_LIBRIARY() override{};
    bool Init() override;
};

//-----------SDEVICE-----------
class DX9RENDER : public VDX9RENDER
{
#define RS_RECT_VERTEX_FORMAT (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

  public:
    static DX9RENDER *pRS;

    DX9RENDER();
    ~DX9RENDER() override;

    // DX9Render: Init/Release
    bool InitDevice(bool windowed, HWND hwnd, int32_t width, int32_t height) override;
    bool ReleaseDevice() override;

    // DX9Render: Animation
    void RenderAnimation(int32_t ib, void *src, int32_t numVrts, int32_t minv, int32_t numv, int32_t startidx, int32_t numtrg,
                         bool isUpdateVB) override;

    // DX9Render: Return d3d9 device
    virtual void *GetDevice()
    {
        return d3d9;
    };

    void *GetD3DDevice() override
    {
        return d3d9;
    };

    // DX9Render: Render Target/Begin/End/Clear
    bool DX9Clear(int32_t type) override; // D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
    bool DX9BeginScene() override;
    bool DX9EndScene() override;

    // DX9Render: Materials/Lights Section
    bool SetLight(uint32_t dwIndex, const D3DLIGHT9 *pLight) override;
    bool LightEnable(uint32_t dwIndex, bool bOn) override;
    bool SetMaterial(D3DMATERIAL9 &material) override;
    bool GetLightEnable(uint32_t dwIndex, BOOL *pEnable) override;
    bool GetLight(uint32_t dwIndex, D3DLIGHT9 *pLight) override;

    // DX9Render: Screenshot Section
    void SaveShoot() override;

    // DX9Render: Clip Planes Section
    HRESULT SetClipPlane(uint32_t Index, CONST float *pPlane) override;
    PLANE *GetPlanes() override;

    // DX9Render: Camera Section
    void SetTransform(int32_t type, D3DMATRIX *mtx) override;
    void GetTransform(int32_t type, D3DMATRIX *mtx) override;

    bool SetCamera(const CVECTOR &pos, const CVECTOR &ang, float perspective) override;
    bool SetCamera(const CVECTOR &pos, const CVECTOR &ang) override;
    bool SetCamera(CVECTOR lookFrom, CVECTOR lookTo, CVECTOR up) override;
    bool SetPerspective(float perspective, float fAspectRatio = -1.0f) override;
    void GetCamera(CVECTOR &pos, CVECTOR &ang, float &perspective) override;

    bool SetCurrentMatrix(D3DMATRIX *mtx) override;

    // DX9Render: Textures Section
    int32_t TextureCreate(const char *fname) override;
    int32_t TextureCreate(UINT width, UINT height, UINT levels, uint32_t usage, D3DFORMAT format, D3DPOOL pool) override;
    bool TextureSet(int32_t stage, int32_t texid) override;
    bool TextureRelease(int32_t texid) override;
    bool TextureIncReference(int32_t texid) override;

    // DX9Render: Fonts Section
    int32_t Print(int32_t x, int32_t y, const char *format, ...) override;
    int32_t Print(int32_t nFontNum, uint32_t color, int32_t x, int32_t y, const char *format, ...) override;
    int32_t ExtPrint(int32_t nFontNum, uint32_t foreColor, uint32_t backColor, int wAlignment, bool bShadow, float fScale,
                  int32_t scrWidth, int32_t scrHeight, int32_t x, int32_t y, const char *format, ...) override;
    int32_t StringWidth(const char *string, int32_t nFontNum = 0, float fScale = 1.f, int32_t scrWidth = 0) override;
    int32_t CharWidth(utf8::u8_char, int32_t nFontNum = 0, float fScale = 1.f, int32_t scrWidth = 0) override;
    int32_t CharHeight(int32_t fontID) override;
    int32_t LoadFont(const char *fontName) override;   // returns the number \ font id, or -1 on error
    bool UnloadFont(const char *fontName) override; // returns true if the font is still in use
    bool UnloadFont(int32_t fontID) override;          // returns true if the font is still in use
    bool IncRefCounter(int32_t fontID) override;       // increase reference counter if object is being copied
    bool SetCurFont(const char *fontName) override; // returns true if the given font is installed
    bool SetCurFont(int32_t fontID) override;          // returns true if the given font is installed
    int32_t GetCurFont() override;
    char *GetFontIniFileName() override;
    bool SetFontIniFileName(const char *iniName) override;

    // DX9Render: Techniques Section
    bool TechniqueExecuteStart(const char *cBlockName) override;
    bool TechniqueExecuteNext() override;

    // DX9Render: Draw Section
    void DrawRects(RS_RECT *pRSR, uint32_t dwRectsNum, const char *cBlockName = nullptr, uint32_t dwSubTexturesX = 1,
                   uint32_t dwSubTexturesY = 1, float fScaleX = 1.0f, float fScaleY = 1.0f) override;
    void DrawSprites(RS_SPRITE *pRSS, uint32_t dwSpritesNum, const char *cBlockName = nullptr) override;
    void DrawLines(RS_LINE *pRSL, uint32_t dwLinesNum, const char *cBlockName = nullptr) override;
    void DrawLines2D(RS_LINE2D *pRSL2D, size_t dwLinesNum, const char *cBlockName = nullptr) override;

    void DrawBuffer(int32_t vbuff, int32_t stride, int32_t ibuff, int32_t minv, size_t numv, size_t startidx, size_t numtrg,
                    const char *cBlockName = nullptr) override;
    void DrawIndexedPrimitiveNoVShader(D3DPRIMITIVETYPE dwPrimitiveType, int32_t iVBuff, int32_t iStride, int32_t iIBuff,
                                       int32_t iMinV, int32_t iNumV, int32_t iStartIdx, int32_t iNumTrg,
                                       const char *cBlockName = nullptr) override;
    void DrawPrimitive(D3DPRIMITIVETYPE dwPrimitiveType, int32_t iVBuff, int32_t iStride, int32_t iStartV, int32_t iNumPT,
                       const char *cBlockName = nullptr) override;
    void DrawPrimitiveUP(D3DPRIMITIVETYPE dwPrimitiveType, uint32_t dwVertexBufferFormat, uint32_t dwNumPT,
                         const void *pVerts, uint32_t dwStride, const char *cBlockName = nullptr) override;
    void DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE dwPrimitiveType, uint32_t dwMinIndex, uint32_t dwNumVertices,
                                uint32_t dwPrimitiveCount, const void *pIndexData, D3DFORMAT IndexDataFormat,
                                const void *pVertexData, uint32_t dwVertexStride,
                                const char *cBlockName = nullptr) override;

    // DX9Render: Video Section
    void PlayToTexture() override;
    CVideoTexture *GetVideoTexture(const char *sVideoName) override;
    void ReleaseVideoTexture(CVideoTexture *pVTexture) override;

    // DX9Render: Vertex/Index Buffers Section
    int32_t CreateVertexBuffer(int32_t type, size_t nverts, uint32_t usage) override;
    int32_t CreateIndexBuffer(size_t ntrgs, uint32_t dwUsage = D3DUSAGE_WRITEONLY) override;

    IDirect3DVertexBuffer9 *GetVertexBuffer(int32_t id) override;
    int32_t GetVertexBufferFVF(int32_t id) override;
    void *LockVertexBuffer(int32_t id, uint32_t dwFlags = 0) override;
    void UnLockVertexBuffer(int32_t id) override;
    int32_t GetVertexBufferSize(int32_t id) override;
    void *LockIndexBuffer(int32_t id, uint32_t dwFlags = 0) override;
    void UnLockIndexBuffer(int32_t id) override;
    void ReleaseVertexBuffer(int32_t id) override;
    void ReleaseIndexBuffer(int32_t id) override;

    // DX9Render: Render/Texture States Section
    uint32_t SetRenderState(uint32_t State, uint32_t Value) override;
    uint32_t GetRenderState(uint32_t State, uint32_t *pValue) override;
    uint32_t GetSamplerState(uint32_t Sampler, D3DSAMPLERSTATETYPE Type, uint32_t *pValue) override;
    uint32_t SetSamplerState(uint32_t Sampler, D3DSAMPLERSTATETYPE Type, uint32_t Value) override;
    uint32_t SetTextureStageState(uint32_t Stage, uint32_t Type, uint32_t Value) override;
    uint32_t GetTextureStageState(uint32_t Stage, uint32_t Type, uint32_t *pValue) override;

    // aspect ratio section
    float GetHeightDeformator() override
    {
        return m_fHeightDeformator;
    }

    POINT GetScreenSize() override
    {
        return screen_size;
    }

    // ===============================================================================================
    // --------------------===================== D3D SECTION =====================--------------------
    // ===============================================================================================

    // D3D Device/Viewport Section
    HRESULT GetViewport(D3DVIEWPORT9 *pViewport) override;
    HRESULT SetViewport(const D3DVIEWPORT9 *pViewport) override;
    HRESULT GetDeviceCaps(D3DCAPS9 *pCaps) override;

    // D3D
    HRESULT SetStreamSource(UINT StreamNumber, void *pStreamData, UINT Stride) override;
    HRESULT SetIndices(void *pIndexData) override;
    HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) override;
    HRESULT Release(IUnknown *pSurface) override;

    // Vertex/Index Buffers Section
    HRESULT CreateVertexBuffer(UINT Length, uint32_t Usage, uint32_t fvf, D3DPOOL Pool,
                               IDirect3DVertexBuffer9 **ppVertexBuffer) override;
    HRESULT VBLock(IDirect3DVertexBuffer9 *pVB, UINT OffsetToLock, UINT SizeToLock, uint8_t **ppbData,
                   uint32_t Flags) override;
    void VBUnlock(IDirect3DVertexBuffer9 *pVB) override;

    // D3D Textures/Surfaces Section
    HRESULT GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface) override;
    HRESULT GetCubeMapSurface(IDirect3DCubeTexture9 *ppCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level,
                              IDirect3DSurface9 **ppCubeMapSurface) override;
    HRESULT CreateTexture(UINT Width, UINT Height, UINT Levels, uint32_t Usage, D3DFORMAT Format, D3DPOOL Pool,
                          IDirect3DTexture9 **ppTexture) override;
    HRESULT CreateCubeTexture(UINT EdgeLength, UINT Levels, uint32_t Usage, D3DFORMAT Format, D3DPOOL Pool,
                              IDirect3DCubeTexture9 **ppCubeTexture) override;
    HRESULT CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format,
                                        IDirect3DSurface9 **ppSurface) override;
    HRESULT CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample,
                                      IDirect3DSurface9 **ppSurface) override;
    HRESULT SetTexture(uint32_t Stage, IDirect3DBaseTexture9 *pTexture) override;
    HRESULT GetLevelDesc(IDirect3DTexture9 *ppTexture, UINT Level, D3DSURFACE_DESC *pDesc) override;
    HRESULT GetLevelDesc(IDirect3DCubeTexture9 *ppCubeTexture, UINT Level, D3DSURFACE_DESC *pDesc) override;
    HRESULT LockRect(IDirect3DCubeTexture9 *ppCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level,
                     D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, uint32_t Flags) override;
    HRESULT LockRect(IDirect3DTexture9 *ppTexture, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect,
                     uint32_t Flags) override;
    HRESULT UnlockRect(IDirect3DCubeTexture9 *pCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level) override;
    HRESULT UnlockRect(IDirect3DTexture9 *pTexture, UINT Level) override;
    HRESULT GetSurfaceLevel(IDirect3DTexture9 *ppTexture, UINT Level, IDirect3DSurface9 **ppSurfaceLevel) override;
    HRESULT UpdateSurface(IDirect3DSurface9 *pSourceSurface, CONST RECT *pSourceRectsArray, UINT cRects,
                          IDirect3DSurface9 *pDestinationSurface, CONST POINT *pDestPointsArray) override;
    HRESULT StretchRect(IDirect3DSurface9 *pSourceSurface, const RECT *pSourceRect, IDirect3DSurface9 *pDestSurface,
                        const RECT *pDestRect, D3DTEXTUREFILTERTYPE Filter) override;
    HRESULT GetRenderTargetData(IDirect3DSurface9 *pRenderTarget, IDirect3DSurface9 *pDestSurface) override;

    // D3D Pixel/Vertex Shaders Section
    HRESULT CreateVertexDeclaration(CONST D3DVERTEXELEMENT9 *pVertexElements,
                                    IDirect3DVertexDeclaration9 **ppDecl) override;
    HRESULT SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl) override;
    HRESULT CreatePixelShader(CONST uint32_t *pFunction, IDirect3DPixelShader9 **ppShader) override;
    HRESULT CreateVertexShader(CONST uint32_t *pFunction, IDirect3DVertexShader9 **ppShader) override;
    HRESULT DeletePixelShader(IDirect3DPixelShader9 *pShader) override;
    HRESULT DeleteVertexShader(IDirect3DVertexShader9 *pShader) override;
    HRESULT SetVertexShader(IDirect3DVertexShader9 *pShader) override;
    HRESULT SetPixelShader(IDirect3DPixelShader9 *pShader) override;
    /*virtual HRESULT SetFVFConstant(DWORD Register, CONST void* pConstantData, DWORD  ConstantCount );*/
    HRESULT SetVertexShaderConstantF(UINT StartRegister, CONST float *pConstantData, UINT Vector4iCount) override;
    HRESULT SetPixelShaderConstantF(UINT StartRegister, CONST float *pConstantData, UINT Vector4iCount) override;
    HRESULT SetFVF(uint32_t handle) override;
    HRESULT GetVertexShader(IDirect3DVertexShader9 **ppShader) override;
    HRESULT GetPixelShader(IDirect3DPixelShader9 **ppShader) override;
#ifdef _WIN32 // Effects
    ID3DXEffect *GetEffectPointer(const char *techniqueName) override;
#endif

    // D3D Render Target/Begin/End/Clear
    HRESULT GetRenderTarget(IDirect3DSurface9 **ppRenderTarget) override;
    HRESULT SetRenderTarget(IDirect3DSurface9 *pRenderTarget, IDirect3DSurface9 *pNewZStencil) override;
    HRESULT Clear(uint32_t Count, CONST D3DRECT *pRects, uint32_t Flags, D3DCOLOR Color, float Z,
                  uint32_t Stencil) override;
    HRESULT BeginScene() override;
    HRESULT EndScene() override;

    HRESULT ImageBlt(const char *pName, RECT *pDstRect, RECT *pSrcRect) override;
    HRESULT ImageBlt(int32_t nTextureId, RECT *pDstRect, RECT *pSrcRect) override;

    void MakeScreenShot();
    bool LoadTextureSurface(std::fstream &fileS, IDirect3DSurface9 *suface, uint32_t mipSize, uint32_t width,
                            uint32_t height, bool isSwizzled);
    uint32_t LoadCubmapSide(std::fstream &fileS, IDirect3DCubeTexture9 *tex, D3DCUBEMAP_FACES face, uint32_t numMips,
                            uint32_t mipSize, uint32_t size, bool isSwizzled);

    // core interface
    bool Init() override;
    void RunStart() override;
    void RunEnd() override;

    uint32_t RunSection() override
    {
        return SECTION_REALIZE;
    };
    bool LoadState(ENTITY_STATE *state) override;
    bool CreateState(ENTITY_STATE_GEN *state_gen) override;

    void ProcessScriptPosAng(const CVECTOR &vPos, const CVECTOR &vAng);
    void FindPlanes(IDirect3DDevice9 *d3dDevice);

    void SetCommonStates();
    void SetProgressImage(const char *image) override;
    void SetProgressBackImage(const char *image) override;
    void SetTipsImage(const char *image) override;
    void StartProgressView() override;
    void ProgressView() override;
    void EndProgressView() override;

    static const uint32_t rectsVBuffer_SizeInRects;
    IDirect3DVertexBuffer9 *rectsVBuffer;

    char *progressImage;
    int32_t progressImageSize;
    int32_t backTexture;
    char *progressBackImage;
    int32_t progressBackImageSize;
    int32_t back0Texture;
    int32_t progressTexture;
    char *progressTipsImage;
    int32_t progressTipsImageSize;
    int32_t progressTipsTexture;

    int32_t loadFrame;
    int32_t progressSafeCounter;
    bool isInPViewProcess;
    uint32_t progressUpdateTime;
    float progressFramesPosX;
    float progressFramesPosY;
    float progressFramesWidth;
    float progressFramesHeight;
    int32_t progressFramesCountX;
    int32_t progressFramesCountY;

    // new renderer settings
    bool vSyncEnabled;
    uint32_t msaa;
    uint32_t videoAdapterIndex;

    D3DPRESENT_PARAMETERS d3dpp;

    CMatrix mView, mWorld, mProjection;

    CVECTOR vWordRelationPos;
    CVECTOR vViewRelationPos;

    bool bUseLargeBackBuffer;

    bool resourcesReleased = false;

    bool IsInsideScene() override
    {
        return bInsideScene;
    }

    char *GetTipsImage() override;

    void SetColorParameters(float fGamma, float fBrightness, float fContrast) override;
    void DrawSphere(const CVECTOR &vPos, float fRadius, uint32_t dwColor) override;
    void DrawEllipsoid(const CVECTOR &vPos, float a, float b, float c, float ay, uint32_t dwColor) override;

    void GetNearFarPlane(float &fNear, float &fFar) override;
    void SetNearFarPlane(float fNear, float fFar) override;

    void SetLoadTextureEnable(bool bEnable = true) override;
    bool ResetDevice();

    void MakeDrawVector(RS_LINE *pLines, uint32_t dwNumSubLines, const CMatrix &mMatrix, CVECTOR vUp, CVECTOR v1,
                        CVECTOR v2, float fScale, uint32_t dwColor);
    void DrawVector(const CVECTOR &v1, const CVECTOR &v2, uint32_t dwColor,
                    const char *pTechniqueName = "DXVector") override;
    IDirect3DBaseTexture9 *GetBaseTexture(int32_t iTexture) override;

    bool PushRenderTarget() override;
    bool PopRenderTarget() override;
    bool SetRenderTarget(IDirect3DCubeTexture9 *pCubeTex, uint32_t dwFaceType, uint32_t dwLevel,
                         IDirect3DSurface9 *pNewZStencil) override;
    void SetView(const CMatrix &mView) override;
    void SetWorld(const CMatrix &mView) override;
    void SetProjection(const CMatrix &mView) override;
    const CMatrix &GetView() override;
    const CMatrix &GetWorld() override;
    const CMatrix &GetProjection() override;

    IDirect3DVolumeTexture9 *CreateVolumeTexture(uint32_t Width, uint32_t Height, uint32_t Depth, uint32_t Levels,
                                                 uint32_t Usage, D3DFORMAT Format, D3DPOOL Pool) override;

    void MakePostProcess() override;
    void SetGLOWParams(float _fBlurBrushSize, int32_t _GlowIntensity, int32_t _GlowPasses) override;

    IDirect3DBaseTexture9 *GetTextureFromID(int32_t nTextureID) override;

    bool GetRenderTargetAsTexture(IDirect3DTexture9 **tex) override;

    void LostRender();
    void RestoreRender();

    void RecompileEffects();

private:
    struct RECT_VERTEX
    {
        CVECTOR pos;
        uint32_t color;
        float u, v;
    };

    struct RenderTarget
    {
        IDirect3DSurface9 *pRenderTarget;
        IDirect3DSurface9 *pDepthSurface;
        D3DVIEWPORT9 ViewPort;
    };

    IDirect3DDevice9 *d3d9;
    IDirect3D9 *d3d;
    HWND hwnd;

    CVECTOR Pos, Ang;
    float Fov;

    float FovMultiplier{ 1.0f };

#ifdef _WIN32 // Effects
    Effects effects_;
#else
    std::unique_ptr<CTechnique> pTechnique;
#endif

    char *fontIniFileName;
    int32_t nFontQuantity;
    FONTEntity FontList[MAX_FONTS]{};
    int32_t idFontCurrent;

    VideoTextureEntity *pVTL;

    int32_t nTextureDegradation;
    float aspectRatio;
    float m_fHeightDeformator;

    bool bSafeRendering;
    bool bShowFps, bShowExInfo;
    bool bInsideScene;

    PLANE viewplane[4];

    STEXTURE Textures[MAX_STEXTURES]{};
    INDEX_BUFFER IndexBuffers[MAX_BUFFERS]{};
    VERTEX_BUFFER VertexBuffers[MAX_BUFFERS]{};

    bool MakeAvi;
    IDirect3DSurface9 *ImageBuffer;

    //-------- post process

    struct QuadVertex
    {
        Vector4 vPos;

        float u0;
        float v0;

        float u1;
        float v1;

        float u2;
        float v2;

        float u3;
        float v3;
    };

    QuadVertex PostProcessQuad[4];
    QuadVertex qv[32 * 32];
    uint16_t qi[31 * 31 * 2 * 3];

    float fSmallWidth;
    float fSmallHeight;
    IDirect3DTexture9 *pPostProcessTexture;
    IDirect3DSurface9 *pPostProcessSurface;

    IDirect3DTexture9 *pSmallPostProcessTexture;
    IDirect3DSurface9 *pSmallPostProcessSurface;

    IDirect3DTexture9 *pSmallPostProcessTexture2;
    IDirect3DSurface9 *pSmallPostProcessSurface2;

    IDirect3DSurface9 *pOriginalScreenSurface;
    IDirect3DSurface9 *pOriginalDepthSurface;

    D3DVIEWPORT9 OriginalViewPort;

    void CreateRenderQuad(float fWidth, float fHeight, float fSrcWidth, float fSrcHeight, float fMulU = 1.0f,
                          float fMulV = 1.0f);

    void ClearPostProcessSurface(IDirect3DSurface9 *pSurf);
    void BlurGlowTexture();
    void CopyGlowToScreen();
    void CopyPostProcessToScreen();

    void SetPostProcessTextureAsRenderTarget();
    void SetScreenAsRenderTarget();

    float fBlurSize;
    int GlowIntensity;
    int iBlurPasses;

    bool bNeedCopyToScreen;

    bool bPostProcessEnabled;
    bool bPostProcessError;

    bool bSeaEffect;
    float fSeaEffectSize;
    float fSeaEffectSpeed;
    uint32_t dwBackColor;

    //-------- post process

    // state save/load ex
    POINT screen_size;
    D3DFORMAT screen_bpp;
    D3DFORMAT stencil_format;

    bool bMakeShoot;
    bool bWindow;
    bool bBackBufferCanLock;

    IDirect3DVertexBuffer9 *aniVBuffer;
    int32_t numAniVerteces;

    IDirect3DVertexBuffer9 *pDropConveyorVBuffer;

    uint32_t dwNumDrawPrimitive, dwNumLV, dwNumLI;
    float fG, fB, fC;

    float fNearClipPlane, fFarClipPlane;

    bool bLoadTextureEnabled;

    bool bTrace;
    int32_t iSetupPath;
    uint64_t dwSetupNumber;
    texpaths_t TexPaths[4]{};

    bool bDropVideoConveyor;

    std::stack<RenderTarget> stRenderTarget;

#ifdef _WIN32 // Screenshot
    D3DXIMAGE_FILEFORMAT screenshotFormat;
#endif
    std::string screenshotExt;

    bool TextureLoad(int32_t texid);
    bool TextureLoadUsingD3DX(const char *path, int32_t texid);
};
