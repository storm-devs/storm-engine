#pragma once

#include <cstdint>
#include <d3d9.h>
#ifdef _WIN32
#include <d3dx9.h>
#endif
#include <string_view>

#include "entity.h"
#include "matrix.h"
#include "service.h"
#include "storm_assert.h"
#include "types3d.h"
#include "utf8.h"

#define FONT_DEFAULT 0

// define print text alignments
#define PR_ALIGN_LEFT 0
#define PR_ALIGN_RIGHT 1
#define PR_ALIGN_CENTER 2

#define INVALID_BUFFER_ID 0xFFFFFFFF
#define INVALID_LIGHT_ID 0xFFFFFFFF
#define INVALID_TEXTURE_ID 0xFFFFFFFF
#define INVALID_RS_ID 0xFFFFFFFF

using RS_ID = uint32_t;

using renderer_handle = uint32_t;

struct FVF_VERTEX
{
    float x, y, z;
    float Nx, Ny, Nz;
    float tu, tv;
};

struct RS_RECT
{
    CVECTOR vPos;
    float fSize;
    float fAngle;
    uint32_t dwColor;
    uint32_t dwSubTexture;
};

struct RS_LINE
{
    CVECTOR vPos;
    uint32_t dwColor;
};

struct RS_SPRITE
{
    CVECTOR vPos;
    float rhw;
    uint32_t dwColor;
    float tu, tv;
};

inline void FillSpriteVertex(RS_SPRITE &pV, float x, float y, float z = 1.0f, uint32_t dwColor = 0xFFFFFF,
                             float tu = 0.0f, float tv = 0.0f)
{
    pV.vPos.x = x;
    pV.vPos.y = y;
    pV.vPos.z = z;
    pV.dwColor = dwColor;
    pV.rhw = 0.5f;
    pV.tu = tu;
    pV.tv = tv;
}

struct RS_LINE2D
{
    CVECTOR vPos;
    float rhw;
    uint32_t dwColor;
};


struct IPOINT
{
    int32_t x, y;
};

struct FPOINT
{
    float x, y;
};

struct FRECT
{
    union {
        struct
        {
            float x1, y1, x2, y2;
        };

        struct
        {
            float xs, ys, xe, ye;
        };

        struct
        {
            float x_start, y_start, x_end, y_end;
        };

        struct
        {
            float left, top, right, bottom;
        };
    };
};


//TODO: remove
#define COLOR2VECTOR(a) CVECTOR(float((a & 0xFF0000) >> 0x10), float((a & 0xFF00) >> 0x8), float(a & 0xFF));
#define COLOR2VECTOR4(a)                                                                                               \
    CVECTOR4(float((a & 0xFF0000) >> 0x10) / 255.0f, float((a & 0xFF00) >> 0x8) / 255.0f, float(a & 0xFF) / 255.0f,    \
             float((a & 0xFF000000) >> 0x18) / 255.0f);
#define ARGB(a, r, g, b) (uint32_t(b) | (uint32_t(g) << 8L) | (uint32_t(r) << 16L) | (uint32_t(a) << 24L))
constexpr auto makeRGB(uint32_t r, uint32_t g, uint32_t b)
{
    return b | g << 8L | r << 16L;
}
inline uint32_t F2DW(float f)
{
    return *reinterpret_cast<uint32_t *>(&f);
}
#define TEXTURE_RELEASE(rs, idtex)                                                                                     \
    {                                                                                                                  \
        if (rs != NULL && idtex != -1)                                                                                 \
        {                                                                                                              \
            rs->TextureRelease(idtex);                                                                                 \
            idtex = -1;                                                                                                \
        }                                                                                                              \
    }

#define FONT_RELEASE(rs, font)                                                                                         \
    {                                                                                                                  \
        if (rs != NULL && font != -1)                                                                                  \
        {                                                                                                              \
            rs->UnloadFont(font);                                                                                      \
            font = -1;                                                                                                 \
        }                                                                                                              \
    }

#define VERTEX_BUFFER_RELEASE(rs, vb)                                                                                  \
    {                                                                                                                  \
        if (rs != NULL && vb != -1)                                                                                    \
        {                                                                                                              \
            rs->ReleaseVertexBuffer(vb);                                                                               \
            vb = -1;                                                                                                   \
        }                                                                                                              \
    }

#define INDEX_BUFFER_RELEASE(rs, ib)                                                                                   \
    {                                                                                                                  \
        if (rs != NULL && ib != -1)                                                                                    \
        {                                                                                                              \
            rs->ReleaseIndexBuffer(ib);                                                                                \
            ib = -1;                                                                                                   \
        }                                                                                                              \
    }
//

class CVideoTexture;

class VDX9RENDER : public SERVICE
{
  public:
    // DX9Render: Construct/Destruct
    ~VDX9RENDER() override{};

    // DX9Render: Init/Release
    virtual bool InitDevice(bool windowed, HWND hwnd, int32_t width, int32_t height) = 0;
    virtual bool ReleaseDevice() = 0;

    // DX9Render: Animation
    virtual void RenderAnimation(int32_t ib, void *src, int32_t numVrts, int32_t minv, int32_t numv, int32_t startidx, int32_t numtrg,
                                 bool isUpdateVB) = 0;

    // DX9Render: Return d3d9 device
    // virtual void * GetDevice() = 0;            // (very bad function/don't use!!!)
    virtual void *GetD3DDevice() = 0; // (very bad function/don't use!!!)

    // DX9Render: Render Target/Begin/End/Clear
    virtual bool DX9Clear(int32_t type) = 0; // D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
    virtual bool DX9BeginScene() = 0;
    virtual bool DX9EndScene() = 0;

    // DX9Render: Materials/Lights Section
    virtual bool SetLight(uint32_t dwIndex, const D3DLIGHT9 *pLight) = 0;
    virtual bool LightEnable(uint32_t dwIndex, bool bOn) = 0;
    virtual bool SetMaterial(D3DMATERIAL9 &material) = 0;
    virtual bool GetLightEnable(uint32_t dwIndex, BOOL *pEnable) = 0;
    virtual bool GetLight(uint32_t dwIndex, D3DLIGHT9 *pLight) = 0;

    // DX9Render: Screenshot Section
    virtual void SaveShoot() = 0;

    // DX9Render: Clip Planes Section
    virtual HRESULT SetClipPlane(uint32_t Index, const float *pPlane) = 0;
    virtual PLANE *GetPlanes() = 0;

    // DX9Render: Camera Section
    virtual void SetTransform(int32_t type, D3DMATRIX *mtx) = 0;
    virtual void GetTransform(int32_t type, D3DMATRIX *mtx) = 0;

    virtual bool SetCamera(const CVECTOR &pos, const CVECTOR &ang, float perspective) = 0;
    virtual bool SetCamera(const CVECTOR &pos, const CVECTOR &ang) = 0;
    virtual bool SetCamera(CVECTOR lookFrom, CVECTOR lookTo, CVECTOR up) = 0;
    virtual bool SetPerspective(float perspective, float fAspectRatio = -1.0f) = 0;
    virtual void GetCamera(CVECTOR &pos, CVECTOR &ang, float &perspective) = 0;

    virtual bool SetCurrentMatrix(D3DMATRIX *mtx) = 0;

    // DX9Render: Textures Section
    virtual int32_t TextureCreate(const char *fname) = 0;
    virtual int32_t TextureCreate(UINT width, UINT height, UINT levels, uint32_t usage, D3DFORMAT format, D3DPOOL pool) = 0;
    virtual bool TextureSet(int32_t stage, int32_t texid) = 0;
    virtual bool TextureRelease(int32_t texid) = 0;
    virtual bool TextureIncReference(int32_t texid) = 0;

    // DX9Render: Fonts Section
    virtual int32_t Print(int32_t x, int32_t y, const char *format, ...) = 0;
    virtual int32_t Print(int32_t nFontNum, uint32_t color, int32_t x, int32_t y, const char *format, ...) = 0;
    virtual int32_t ExtPrint(int32_t nFontNum, uint32_t foreColor, uint32_t backColor, int wAlignment, bool bShadow,
                          float fScale, int32_t scrWidth, int32_t scrHeight, int32_t x, int32_t y, const char *format, ...) = 0;
    [[deprecated("Pass string as string_view instead")]]
    virtual int32_t StringWidth(const char *string, int32_t nFontNum = 0, float fScale = 1.f, int32_t scrWidth = 0) = 0;
    virtual int32_t StringWidth(const std::string_view &string, int32_t nFontNum = 0, float fScale = 1.f, int32_t scrWidth = 0) = 0;
    virtual int32_t CharWidth(utf8::u8_char ucVKey, int32_t nFontNum = 0, float fScale = 1.f, int32_t scrWidth = 0) = 0;
    virtual int32_t CharHeight(int32_t fontID) = 0;
    virtual int32_t LoadFont(const char *fontName) = 0;   // returns the number \ font id, or -1 on error
    virtual bool UnloadFont(const char *fontName) = 0; // returns true if the font is still in use
    virtual bool UnloadFont(int32_t fontID) = 0;          // returns true if the font is still in use
    virtual bool IncRefCounter(int32_t fontID) = 0;       // increase reference counter if object is being copied
    virtual bool SetCurFont(const char *fontName) = 0; // returns true if the given font is installed
    virtual bool SetCurFont(int32_t fontID) = 0;          // returns true if the given font is installed
    virtual int32_t GetCurFont() = 0;
    virtual char *GetFontIniFileName() = 0;
    virtual bool SetFontIniFileName(const char *iniName) = 0;

    // DX9Render: Techniques Section
    virtual bool TechniqueExecuteStart(const char *cBlockName) = 0;
    virtual bool TechniqueExecuteNext() = 0;

    // DX9Render: Draw Section
    virtual void DrawRects(RS_RECT *pRSR, uint32_t dwRectsNum, const char *cBlockName = nullptr,
                           uint32_t dwSubTexturesX = 1, uint32_t dwSubTexturesY = 1, float fScaleX = 1.0f,
                           float fScaleY = 1.0f) = 0;
    virtual void DrawSprites(RS_SPRITE *pRSS, uint32_t dwSpritesNum, const char *cBlockName = nullptr) = 0;
    virtual void DrawLines(RS_LINE *pRSL, uint32_t dwLinesNum, const char *cBlockName = nullptr) = 0;
    virtual void DrawVector(const CVECTOR &v1, const CVECTOR &v2, uint32_t dwColor,
                            const char *pTechniqueName = "DXVector") = 0;
    virtual void DrawLines2D(RS_LINE2D *pRSL2D, size_t dwLinesNum, const char *cBlockName = nullptr) = 0;

    virtual void DrawBuffer(int32_t vbuff, int32_t stride, int32_t ibuff, int32_t minv, size_t numv, size_t startidx, size_t numtrg,
                            const char *cBlockName = nullptr) = 0;
    virtual void DrawIndexedPrimitiveNoVShader(D3DPRIMITIVETYPE dwPrimitiveType, int32_t iVBuff, int32_t iStride, int32_t iIBuff,
                                               int32_t iMinV, int32_t iNumV, int32_t iStartIdx, int32_t iNumTrg,
                                               const char *cBlockName = nullptr) = 0;
    virtual void DrawPrimitive(D3DPRIMITIVETYPE dwPrimitiveType, int32_t iVBuff, int32_t iStride, int32_t iStartV, int32_t iNumPT,
                               const char *cBlockName = nullptr) = 0;
    virtual void DrawPrimitiveUP(D3DPRIMITIVETYPE dwPrimitiveType, uint32_t dwVertexBufferFormat, uint32_t dwNumPT,
                                 const void *pVerts, uint32_t dwStride, const char *cBlockName = nullptr) = 0;
    virtual void DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE dwPrimitiveType, uint32_t dwMinIndex, uint32_t dwNumVertices,
                                        uint32_t dwPrimitiveCount, const void *pIndexData, D3DFORMAT IndexDataFormat,
                                        const void *pVertexData, uint32_t dwVertexStride,
                                        const char *cBlockName = nullptr) = 0;

    // DX9Render: Video Section
    virtual void PlayToTexture() = 0;
    virtual CVideoTexture *GetVideoTexture(const char *sVideoName) = 0;
    virtual void ReleaseVideoTexture(CVideoTexture *pVTexture) = 0;

    // DX9Render: Vertex/Index Buffers Section
    virtual int32_t CreateVertexBuffer(int32_t type, size_t nverts, uint32_t usage,
                                       uint32_t dwPool = D3DPOOL_DEFAULT) = 0;
    virtual int32_t CreateIndexBuffer(size_t ntrgs, uint32_t dwUsage = D3DUSAGE_WRITEONLY) = 0;

    virtual IDirect3DVertexBuffer9 *GetVertexBuffer(int32_t id) = 0;
    virtual int32_t GetVertexBufferFVF(int32_t id) = 0;
    virtual void *LockVertexBuffer(int32_t id, uint32_t dwFlags = 0) = 0;
    virtual void UnLockVertexBuffer(int32_t id) = 0;
    virtual int32_t GetVertexBufferSize(int32_t id) = 0;
    virtual void *LockIndexBuffer(int32_t id, uint32_t dwFlags = 0) = 0;
    virtual void UnLockIndexBuffer(int32_t id) = 0;
    virtual void ReleaseVertexBuffer(int32_t id) = 0;
    virtual void ReleaseIndexBuffer(int32_t id) = 0;

    // DX9Render: Render/Texture States Section
    virtual uint32_t SetRenderState(uint32_t State, uint32_t Value) = 0;
    virtual uint32_t GetRenderState(uint32_t State, uint32_t *pValue) = 0;
    virtual uint32_t GetSamplerState(uint32_t Sampler, D3DSAMPLERSTATETYPE Type, uint32_t *pValue) = 0;
    virtual uint32_t SetSamplerState(uint32_t Sampler, D3DSAMPLERSTATETYPE Type, uint32_t Value) = 0;
    virtual uint32_t SetTextureStageState(uint32_t Stage, uint32_t Type, uint32_t Value) = 0;
    virtual uint32_t GetTextureStageState(uint32_t Stage, uint32_t Type, uint32_t *pValue) = 0;

    // aspect ratio section
    virtual float GetHeightDeformator() = 0;
    virtual POINT GetScreenSize() = 0;
    // ===============================================================================================
    // --------------------===================== D3D SECTION =====================--------------------
    // ===============================================================================================

    // D3D Device/Viewport Section
    virtual HRESULT GetViewport(D3DVIEWPORT9 *pViewport) = 0;
    virtual HRESULT SetViewport(const D3DVIEWPORT9 *pViewport) = 0;
    virtual HRESULT GetDeviceCaps(D3DCAPS9 *pCaps) = 0;

    // D3D
    virtual HRESULT SetStreamSource(UINT StreamNumber, void *pStreamData, UINT Stride) = 0;
    virtual HRESULT SetIndices(void *pIndexData) = 0;
    virtual HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) = 0;

    virtual HRESULT Release(IUnknown *pSurface) = 0;

    // Vertex/Index Buffers Section
    virtual HRESULT CreateVertexBuffer(UINT Length, uint32_t Usage, uint32_t FVF, D3DPOOL Pool,
                                       IDirect3DVertexBuffer9 **ppVertexBuffer) = 0;
    virtual HRESULT VBLock(IDirect3DVertexBuffer9 *pVB, UINT OffsetToLock, UINT SizeToLock, uint8_t **ppbData,
                           uint32_t Flags) = 0;
    virtual void VBUnlock(IDirect3DVertexBuffer9 *pVB) = 0;

    // D3D Textures/Surfaces Section
    virtual HRESULT GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface) = 0;
    virtual HRESULT GetCubeMapSurface(IDirect3DCubeTexture9 *ppCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level,
                                      IDirect3DSurface9 **ppCubeMapSurface) = 0;
    virtual HRESULT CreateTexture(UINT Width, UINT Height, UINT Levels, uint32_t Usage, D3DFORMAT Format, D3DPOOL Pool,
                                  IDirect3DTexture9 **ppTexture) = 0;
    virtual HRESULT CreateCubeTexture(UINT EdgeLength, UINT Levels, uint32_t Usage, D3DFORMAT Format, D3DPOOL Pool,
                                      IDirect3DCubeTexture9 **ppCubeTexture) = 0;
    virtual HRESULT CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format,
                                                IDirect3DSurface9 **ppSurface) = 0;
    virtual HRESULT CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format,
                                              D3DMULTISAMPLE_TYPE MultiSample, IDirect3DSurface9 **ppSurface) = 0;
    virtual HRESULT SetTexture(uint32_t Stage, IDirect3DBaseTexture9 *pTexture) = 0;
    virtual HRESULT GetLevelDesc(IDirect3DTexture9 *ppTexture, UINT Level, D3DSURFACE_DESC *pDesc) = 0;
    virtual HRESULT GetLevelDesc(IDirect3DCubeTexture9 *ppCubeTexture, UINT Level, D3DSURFACE_DESC *pDesc) = 0;
    virtual HRESULT LockRect(IDirect3DCubeTexture9 *ppCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level,
                             D3DLOCKED_RECT *pLockedRect, const RECT *pRect, uint32_t Flags) = 0;
    virtual HRESULT LockRect(IDirect3DTexture9 *ppTexture, UINT Level, D3DLOCKED_RECT *pLockedRect, const RECT *pRect,
                             uint32_t Flags) = 0;
    virtual HRESULT UnlockRect(IDirect3DCubeTexture9 *pCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level) = 0;
    virtual HRESULT UnlockRect(IDirect3DTexture9 *pTexture, UINT Level) = 0;
    virtual HRESULT GetSurfaceLevel(IDirect3DTexture9 *ppTexture, UINT Level, IDirect3DSurface9 **ppSurfaceLevel) = 0;
    virtual HRESULT UpdateSurface(IDirect3DSurface9 *pSourceSurface, const RECT *pSourceRectsArray, UINT cRects,
                                  IDirect3DSurface9 *pDestinationSurface, const POINT *pDestPointsArray) = 0;
    virtual HRESULT StretchRect(IDirect3DSurface9 *pSourceSurface, const RECT *pSourceRect,
                                IDirect3DSurface9 *pDestSurface, const RECT *pDestRect,
                                D3DTEXTUREFILTERTYPE Filter) = 0;
    virtual HRESULT GetRenderTargetData(IDirect3DSurface9 *pRenderTarget, IDirect3DSurface9 *pDestSurface) = 0;

    // D3D Pixel/Vertex Shaders Section
    virtual HRESULT CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements,
                                            IDirect3DVertexDeclaration9 **ppDecl) = 0;
    virtual HRESULT SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl) = 0;
    virtual HRESULT CreatePixelShader(const uint32_t *pFunction, IDirect3DPixelShader9 **ppShader) = 0;
    virtual HRESULT CreateVertexShader(const uint32_t *pFunction, IDirect3DVertexShader9 **ppShader) = 0;
    virtual HRESULT DeletePixelShader(IDirect3DPixelShader9 *pShader) = 0;
    virtual HRESULT DeleteVertexShader(IDirect3DVertexShader9 *pShader) = 0;
    virtual HRESULT SetVertexShader(IDirect3DVertexShader9 *pShader) = 0;
    virtual HRESULT SetPixelShader(IDirect3DPixelShader9 *pShader) = 0;
    /*virtual HRESULT SetFVFConstant(DWORD Register, const void * pConstantData, DWORD  ConstantCount ) = 0;*/
    virtual HRESULT SetVertexShaderConstantF(UINT StartRegister, const float *pConstantData, UINT Vector4iCount) = 0;
    virtual HRESULT SetPixelShaderConstantF(UINT StartRegister, const float *pConstantData, UINT Vector4iCount) = 0;
    virtual HRESULT SetFVF(uint32_t handle) = 0;
    virtual HRESULT GetVertexShader(IDirect3DVertexShader9 **ppShader) = 0;
    virtual HRESULT GetPixelShader(IDirect3DPixelShader9 **ppShader) = 0;
#ifdef _WIN32 // Effects
    virtual ID3DXEffect *GetEffectPointer(const char *techniqueName) = 0;
#endif

    // D3D Render Target/Begin/End/Clear
    virtual HRESULT GetRenderTarget(IDirect3DSurface9 **ppRenderTarget) = 0;
    virtual HRESULT SetRenderTarget(IDirect3DSurface9 *pRenderTarget, IDirect3DSurface9 *pNewZStencil) = 0;
    virtual HRESULT Clear(uint32_t Count, const D3DRECT *pRects, uint32_t Flags, D3DCOLOR Color, float Z,
                          uint32_t Stencil) = 0;
    virtual HRESULT BeginScene() = 0;
    virtual HRESULT EndScene() = 0;

    virtual HRESULT ImageBlt(const char *pName, RECT *pDstRect = nullptr, RECT *pSrcRect = nullptr) = 0;
    virtual HRESULT ImageBlt(int32_t nTextureId, RECT *pDstRect = nullptr, RECT *pSrcRect = nullptr) = 0;

    virtual void SetProgressImage(const char *image) = 0;
    virtual void SetProgressBackImage(const char *image) = 0;
    virtual void SetTipsImage(const char *image) = 0;
    virtual void StartProgressView() = 0;
    virtual void ProgressView() = 0;
    virtual void EndProgressView() = 0;

    virtual bool IsInsideScene() = 0;
    virtual char *GetTipsImage() = 0;
    virtual void SetColorParameters(float fGamma, float fBrightness, float fContrast) = 0;

    virtual void DrawSphere(const CVECTOR &vPos, float fRadius, uint32_t dwColor) = 0;
    virtual void DrawEllipsoid(const CVECTOR &vPos, float a, float b, float c, float ay, uint32_t dwColor) = 0;

    virtual void GetNearFarPlane(float &fNear, float &fFar) = 0;
    virtual void SetNearFarPlane(float fNear, float fFar) = 0;

    virtual void SetLoadTextureEnable(bool bEnable = true) = 0;
    virtual IDirect3DBaseTexture9 *GetBaseTexture(int32_t iTexture) = 0;

    virtual bool PushRenderTarget() = 0;
    virtual bool PopRenderTarget() = 0;
    virtual bool SetRenderTarget(IDirect3DCubeTexture9 *pCubeTex, uint32_t dwFaceType, uint32_t dwLevel,
                                 IDirect3DSurface9 *pNewZStencil) = 0;
    virtual void SetView(const CMatrix &mView) = 0;
    virtual void SetWorld(const CMatrix &mView) = 0;
    virtual void SetProjection(const CMatrix &mView) = 0;
    virtual const CMatrix &GetView() = 0;
    virtual const CMatrix &GetWorld() = 0;
    virtual const CMatrix &GetProjection() = 0;

    virtual IDirect3DVolumeTexture9 *CreateVolumeTexture(uint32_t Width, uint32_t Height, uint32_t Depth,
                                                         uint32_t Levels, uint32_t Usage, D3DFORMAT Format,
                                                         D3DPOOL Pool) = 0;

    virtual void MakePostProcess() = 0;
    virtual void SetGLOWParams(float _fBlurBrushSize, int32_t _GlowIntensity, int32_t _GlowPasses) = 0;

    virtual IDirect3DBaseTexture9 *GetTextureFromID(int32_t nTextureID) = 0;

    virtual bool GetRenderTargetAsTexture(IDirect3DTexture9 **tex) = 0;
};
