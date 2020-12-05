#ifndef __DX8RENDER_H__
#define __DX8RENDER_H__

//#include <windows.h>
#include "vmodule_api.h"
#include <D3DX9.h>

//#include "vmodule_api.h"
#include "Matrix.h"
#include "types3d.h"
#ifdef _XBOX
#include <xgraphics.h>
#endif

#include "VideoTexture.h"

#define FONT_DEFAULT 0

// define print text alignments
#define ALIGN_LEFT 0
#define ALIGN_RIGHT 1
#define ALIGN_CENTER 2

#define INVALID_BUFFER_ID 0xFFFFFFFF
#define INVALID_LIGHT_ID 0xFFFFFFFF
#define INVALID_TEXTURE_ID 0xFFFFFFFF
#define INVALID_RS_ID 0xFFFFFFFF

class CVideoTexture;
typedef dword RS_ID;

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
    dword dwColor;
    dword dwSubTexture;
};

struct RS_LINE
{
    CVECTOR vPos;
    dword dwColor;
};

struct RS_SPRITE
{
    CVECTOR vPos;
    float rhw;
    dword dwColor;
    float tu, tv;
};

struct RS_LINE2D
{
    CVECTOR vPos;
    float rhw;
    dword dwColor;
};

class IBaseTexture
{
};

class ITexture
{
};

class ICubeTexture
{
};

class VDX8RENDER : public SERVICE
{
  public:
    // DX8Render: Construct/Destruct
    virtual ~VDX8RENDER(){};

    // DX8Render: Init/Release
    virtual bool InitDevice(bool windowed, HWND hwnd, long width, long height) = 0;
    virtual bool ReleaseDevice() = 0;

    // DX8Render: Animation
    virtual void RenderAnimation(long ib, void *src, long numVrts, long minv, long numv, long startidx, long numtrg,
                                 bool isUpdateVB) = 0;

    // DX8Render: Return d3d9 device
    // virtual void * GetDevice() = 0;			// (very bad function/don't use!!!)
    virtual void *GetD3DDevice() = 0; // (very bad function/don't use!!!)

    // DX8Render: Render Target/Begin/End/Clear
    virtual bool DX8Clear(long type) = 0; // D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
    virtual bool DX8BeginScene() = 0;
    virtual bool DX8EndScene() = 0;

    // DX8Render: Materials/Lights Section
    virtual bool SetLight(dword dwIndex, const D3DLIGHT9 *pLight) = 0;
    virtual bool LightEnable(dword dwIndex, bool bOn) = 0;
    virtual bool SetMaterial(D3DMATERIAL9 &material) = 0;
    virtual bool GetLightEnable(DWORD dwIndex, BOOL *pEnable) = 0;
    virtual bool GetLight(DWORD dwIndex, D3DLIGHT9 *pLight) = 0;

    // DX8Render: Screenshot Section
    virtual void SaveShoot() = 0;

    // DX8Render: Clip Planes Section
    virtual HRESULT SetClipPlane(DWORD Index, CONST float *pPlane) = 0;
    virtual PLANE *GetPlanes() = 0;

    // DX8Render: Camera Section
    virtual void SetTransform(long type, D3DMATRIX *mtx) = 0;
    virtual void GetTransform(long type, D3DMATRIX *mtx) = 0;

    virtual bool SetCamera(CVECTOR *pos, CVECTOR *ang, float perspective) = 0;
    virtual bool SetCamera(CVECTOR *pos, CVECTOR *ang) = 0;
    virtual bool SetCamera(CVECTOR lookFrom, CVECTOR lookTo, CVECTOR up) = 0;
    virtual bool SetPerspective(float perspective, float fAspectRatio = -1.0f) = 0;
    virtual void GetCamera(CVECTOR &pos, CVECTOR &ang, float &perspective) = 0;

    virtual bool SetCurrentMatrix(D3DMATRIX *mtx) = 0;

    // DX8Render: Textures Section
    virtual long TextureCreate(const char *fname) = 0;
    virtual bool TextureSet(long stage, long texid) = 0;
    virtual bool TextureRelease(long texid) = 0;

    // DX8Render: Fonts Section
    virtual long _cdecl Print(long x, long y, char *format, ...) = 0;
    virtual long _cdecl Print(long nFontNum, DWORD color, long x, long y, char *format, ...) = 0;
    virtual long _cdecl ExtPrint(long nFontNum, DWORD foreColor, DWORD backColor, int wAlignment, bool bShadow,
                                 float fScale, long scrWidth, long scrHeight, long x, long y, char *format, ...) = 0;
    virtual long StringWidth(const char *string, long nFontNum = 0, float fScale = 1.f, long scrWidth = 0) = 0;
    virtual long CharWidth(utf8::u8_char ch, long nFontNum = 0, float fScale = 1.f, long scrWidth = 0) = 0;
    virtual long CharHeight(long fontID) = 0;
    virtual long LoadFont(char *fontName) = 0; // возвращает номер\идентификатор шрифта или -1 в случае ошибки
    virtual bool UnloadFont(char *fontName) = 0; // возвращает истину если шрифт остался в использовании
    virtual bool UnloadFont(long fontID) = 0; // возвращает истину если шрифт остался в использовании
    virtual bool SetCurFont(char *fontName) = 0; // возвращает истину если установили данный шрифт
    virtual bool SetCurFont(long fontID) = 0; // возвращает истину если установили данный шрифт
    virtual long GetCurFont() = 0;
    virtual char *GetFontIniFileName() = 0;
    virtual bool SetFontIniFileName(char *iniName) = 0;

    // DX8Render: Techniques Section
    virtual bool TechniqueSetParamsAndStart(const char *cBlockName, dword _dwNumParams = 0, void *pParams = null) = 0;
    virtual bool _cdecl TechniqueExecuteStart(const char *cBlockName, dword _dwNumParams = 0, ...) = 0;
    virtual bool TechniqueExecuteNext() = 0;

    // DX8Render: Draw Section
    virtual void _cdecl DrawRects(RS_RECT *pRSR, dword dwRectsNum, const char *cBlockName = 0, dword dwSubTexturesX = 1,
                                  dword dwSubTexturesY = 1, float fScaleX = 1.0f, float fScaleY = 1.0f,
                                  dword dwNumParams = 0, ...) = 0;
    virtual void _cdecl DrawSprites(RS_SPRITE *pRSS, dword dwSpritesNum, const char *cBlockName = 0,
                                    dword dwNumParams = 0, ...) = 0;
    virtual void _cdecl DrawLines(RS_LINE *pRSL, dword dwLinesNum, const char *cBlockName = 0, dword dwNumParams = 0,
                                  ...) = 0;
    virtual void _cdecl DrawVector(const CVECTOR &v1, const CVECTOR &v2, dword dwColor,
                                   const char *pTechniqueName = "DX8Vector", dword dwNumParams = 0, ...) = 0;
    virtual void _cdecl DrawLines2D(RS_LINE2D *pRSL2D, dword dwLinesNum, const char *cBlockName = 0,
                                    dword dwNumParams = 0, ...) = 0;

    virtual void _cdecl DrawBuffer(long vbuff, long stride, long ibuff, long minv, long numv, long startidx,
                                   long numtrg, const char *cBlockName = 0, dword dwNumParams = 0, ...) = 0;
    virtual void _cdecl DrawIndexedPrimitiveNoVShader(D3DPRIMITIVETYPE dwPrimitiveType, long iVBuff, long iStride,
                                                      long iIBuff, long iMinV, long iNumV, long iStartIdx, long iNumTrg,
                                                      const char *cBlockName = 0, dword dwNumParams = 0, ...) = 0;
    virtual void _cdecl DrawPrimitive(D3DPRIMITIVETYPE dwPrimitiveType, long iVBuff, long iStride, long iStartV,
                                      long iNumPT, const char *cBlockName = 0, dword dwNumParams = 0, ...) = 0;
    virtual void _cdecl DrawPrimitiveUP(D3DPRIMITIVETYPE dwPrimitiveType, dword dwVertexBufferFormat, dword dwNumPT,
                                        void *pVerts, dword dwStride, const char *cBlockName = 0, dword dwNumParams = 0,
                                        ...) = 0;
    virtual void _cdecl DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE dwPrimitiveType, dword dwMinIndex, dword dwNumVertices,
                                               dword dwPrimitiveCount, const void *pIndexData,
                                               D3DFORMAT IndexDataFormat, const void *pVertexData, dword dwVertexStride,
                                               const char *cBlockName = 0, dword dwNumParams = 0, ...) = 0;

    // DX8Render: Video Section
    virtual void PlayToTexture() = 0;
    virtual CVideoTexture *GetVideoTexture(char *sVideoName) = 0;
    virtual void ReleaseVideoTexture(CVideoTexture *pVTexture) = 0;

    // DX8Render: Vertex/Index Buffers Section
    virtual long CreateVertexBuffer(long type, long nverts, dword usage) = 0;
    virtual long CreateIndexBuffer(long ntrgs, dword dwUsage = D3DUSAGE_WRITEONLY) = 0;

    virtual long CreateVertexBufferManaged(long type, long nverts, dword usage) = 0;
    virtual long CreateIndexBufferManaged(long ntrgs, dword dwUsage = D3DUSAGE_WRITEONLY) = 0;

    virtual IDirect3DVertexBuffer9 *GetVertexBuffer(long id) = 0;
    virtual long GetVertexBufferFVF(long id) = 0;
    virtual void *LockVertexBuffer(long id, dword dwFlags = 0) = 0;
    virtual void UnLockVertexBuffer(long id) = 0;
    virtual long GetVertexBufferSize(long id) = 0;
    virtual void *LockIndexBuffer(long id, dword dwFlags = 0) = 0;
    virtual void UnLockIndexBuffer(long id) = 0;
    virtual void ReleaseVertexBuffer(long id) = 0;
    virtual void ReleaseIndexBuffer(long id) = 0;

    // DX8Render: Render/Texture States Section
    virtual dword SetRenderState(dword State, dword Value) = 0;
    virtual dword SetRenderStateFloat(dword State, double Value) = 0;
    virtual dword GetRenderState(dword State, dword *pValue) = 0;
    virtual dword SetTextureStageState(dword Stage, dword Type, dword Value) = 0;
    virtual dword GetTextureStageState(dword Stage, dword Type, dword *pValue) = 0;

    // 	DX8Render: Render/Sampler States Section
    virtual dword SetSamplerState(dword Stage, dword Type, dword Value) = 0;
    virtual dword GetSamplerState(dword Stage, dword Type, dword *pValue) = 0;

    // aspect ratio section
    virtual float GetHeightDeformator() = 0;

    // ===============================================================================================
    // --------------------===================== D3D SECTION =====================--------------------
    // ===============================================================================================

    // D3D Device/Viewport Section
    virtual HRESULT GetViewport(D3DVIEWPORT9 *pViewport) = 0;
    virtual HRESULT SetViewport(const D3DVIEWPORT9 *pViewport) = 0;
    virtual HRESULT GetDeviceCaps(D3DCAPS9 *pCaps) = 0;

    // D3D
    virtual HRESULT SetStreamSource(UINT StreamNumber, void *pStreamData, UINT Stride) = 0;
    virtual HRESULT SetIndices(void *pIndexData, UINT BaseVertexIndex) = 0;
    virtual HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) = 0;
#ifndef _XBOX
    virtual HRESULT Release(IUnknown *pSurface) = 0;
#else
    virtual HRESULT Release(IDirect3DResource9 *pSurface) = 0;
#endif

    // Vertex/Index Buffers Section
    virtual HRESULT CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool,
                                       IDirect3DVertexBuffer9 **ppVertexBuffer) = 0;
    virtual HRESULT VBLock(IDirect3DVertexBuffer9 *pVB, UINT OffsetToLock, UINT SizeToLock, BYTE **ppbData,
                           DWORD Flags) = 0;
    virtual void VBUnlock(IDirect3DVertexBuffer9 *pVB) = 0;

    // D3D Textures/Surfaces Section
    virtual HRESULT GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface) = 0;
    virtual HRESULT GetCubeMapSurface(IDirect3DCubeTexture9 *ppCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level,
                                      IDirect3DSurface9 **ppCubeMapSurface) = 0;
    virtual HRESULT CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                  IDirect3DTexture9 **ppTexture) = 0;
    virtual HRESULT CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                      IDirect3DCubeTexture9 **ppCubeTexture) = 0;
    virtual HRESULT CreateImageSurface(UINT Width, UINT Height, D3DFORMAT Format, IDirect3DSurface9 **ppSurface) = 0;
    virtual HRESULT CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format,
                                              D3DMULTISAMPLE_TYPE MultiSample, IDirect3DSurface9 **ppSurface) = 0;
    virtual HRESULT SetTexture(DWORD Stage, IDirect3DBaseTexture9 *pTexture) = 0;
    virtual HRESULT GetLevelDesc(IDirect3DTexture9 *ppTexture, UINT Level, D3DSURFACE_DESC *pDesc) = 0;
    virtual HRESULT GetLevelDesc(IDirect3DCubeTexture9 *ppCubeTexture, UINT Level, D3DSURFACE_DESC *pDesc) = 0;
    virtual HRESULT LockRect(IDirect3DCubeTexture9 *ppCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level,
                             D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags) = 0;
    virtual HRESULT LockRect(IDirect3DTexture9 *ppTexture, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect,
                             DWORD Flags) = 0;
    virtual HRESULT UnlockRect(IDirect3DCubeTexture9 *pCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level) = 0;
    virtual HRESULT UnlockRect(IDirect3DTexture9 *pTexture, UINT Level) = 0;
    virtual HRESULT GetSurfaceLevel(IDirect3DTexture9 *ppTexture, UINT Level, IDirect3DSurface9 **ppSurfaceLevel) = 0;
    virtual HRESULT CopyRects(IDirect3DSurface9 *pSourceSurface, CONST RECT *pSourceRectsArray, UINT cRects,
                              IDirect3DSurface9 *pDestinationSurface, CONST RECT *pDestRectsArray) = 0;
    virtual HRESULT GetRenderTargetData(IDirect3DSurface9 *pSourceSurface, IDirect3DSurface9 *pDestinationSurface) = 0;
    virtual HRESULT UpdateSurface(IDirect3DSurface9 *pSourceSurface, CONST RECT *pSourceRectsArray,
                                  IDirect3DSurface9 *pDestinationSurface, CONST POINT *pDestPoint) = 0;
    virtual HRESULT CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample,
                                       DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9 **ppSurface) = 0;
    virtual HRESULT StretchRect(IDirect3DSurface9 *pSourceSurface, const RECT *pSourceRect,
                                IDirect3DSurface9 *pDestSurface, const RECT *pDestRect,
                                D3DTEXTUREFILTERTYPE Filter) = 0;

    // D3D Pixel/Vertex Shaders Section
    virtual HRESULT CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements,
                                            IDirect3DVertexDeclaration9 **ppDecl) = 0;
    virtual HRESULT SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl) = 0;
    virtual HRESULT SetVertexShaderConstant(DWORD Register, CONST void *pConstantData, DWORD ConstantCount) = 0;
    virtual HRESULT SetPixelShaderConstant(DWORD Register, CONST void *pConstantData, DWORD ConstantCount) = 0;
    virtual HRESULT GetVertexShader(IDirect3DVertexShader9 **ppShader) = 0;
    virtual HRESULT GetPixelShader(IDirect3DPixelShader9 **ppShader) = 0;
    virtual HRESULT CreateVertexShader(CONST DWORD *pFunction, IDirect3DVertexShader9 **ppShader) = 0;
    virtual HRESULT CreatePixelShader(CONST DWORD *pFunction, IDirect3DPixelShader9 **ppShader) = 0;
    virtual HRESULT DeleteVertexShader(IDirect3DVertexShader9 *pShader) = 0;
    virtual HRESULT DeletePixelShader(IDirect3DPixelShader9 *pShader) = 0;
    virtual HRESULT SetVertexShader(IDirect3DVertexShader9 *pShader) = 0;
    virtual HRESULT SetPixelShader(IDirect3DPixelShader9 *pShader) = 0;
    virtual HRESULT SetFVF(DWORD handle) = 0;

    // D3D Render Target/Begin/End/Clear
    virtual HRESULT GetRenderTarget(IDirect3DSurface9 **ppRenderTarget) = 0;
    virtual HRESULT SetRenderTarget(IDirect3DSurface9 *pRenderTarget, IDirect3DSurface9 *pNewZStencil) = 0;
    virtual HRESULT Clear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) = 0;
    virtual HRESULT BeginScene() = 0;
    virtual HRESULT EndScene() = 0;

    virtual HRESULT ImageBlt(const char *pName, RECT *pDstRect = 0, RECT *pSrcRect = 0) = 0;
    virtual HRESULT ImageBlt(long nTextureId, RECT *pDstRect = 0, RECT *pSrcRect = 0) = 0;

    virtual void SetProgressImage(const char *image) = 0;
    virtual void SetProgressBackImage(const char *image) = 0;
    virtual void SetTipsImage(const char *image) = 0;
    virtual void StartProgressView() = 0;
    virtual void ProgressView() = 0;
    virtual void EndProgressView() = 0;

    virtual bool IsInsideScene() = 0;
    virtual char *GetTipsImage() = 0;
    virtual void SetColorParameters(float fGamma, float fBrightness, float fContrast) = 0;

    virtual void DrawSphere(const CVECTOR &vPos, float fRadius, dword dwColor) = 0;

    virtual void GetNearFarPlane(float &fNear, float &fFar) = 0;
    virtual void SetNearFarPlane(float fNear, float fFar) = 0;

    virtual void SetLoadTextureEnable(bool bEnable = true) = 0;
    virtual IDirect3DBaseTexture9 *GetBaseTexture(long iTexture) = 0;

    virtual IDirect3DBaseTexture9 *CreateTextureFromFileInMemory(const char *pFile, dword dwSize) = 0;

    virtual bool PushRenderTarget() = 0;
    virtual bool PopRenderTarget() = 0;
    virtual bool SetRenderTarget(IDirect3DCubeTexture9 *pCubeTex, dword dwFaceType, dword dwLevel,
                                 IDirect3DSurface9 *pNewZStencil) = 0;
    virtual void SetView(const CMatrix &mView) = 0;
    virtual void SetWorld(const CMatrix &mView) = 0;
    virtual void SetProjection(const CMatrix &mView) = 0;
    virtual const CMatrix &GetView() = 0;
    virtual const CMatrix &GetWorld() = 0;
    virtual const CMatrix &GetProjection() = 0;

    virtual IDirect3DVolumeTexture9 *CreateVolumeTexture(dword Width, dword Height, dword Depth, dword Levels,
                                                         dword Usage, D3DFORMAT Format, D3DPOOL Pool) = 0;

    virtual void MakePostProcess() = 0;
    virtual void SetGLOWParams(float _fBlurBrushSize, long _GlowIntensity, long _GlowPasses) = 0;

    virtual IDirect3DBaseTexture9 *GetTextureFromID(long nTextureID) = 0;
};

#endif