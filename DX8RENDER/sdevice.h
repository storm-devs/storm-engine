#ifndef __SDEVICE_H__
#define __SDEVICE_H__


#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\defines.h"
#include "..\common_h\VideoTexture.h"
#include "..\common_h\templates.h"
#include "Technique.h"
#include "font.h"

#include <D3DX9.h>
#include <D3DX9core.h>

#define MAX_STEXTURES	4096
#define MAX_BUFFERS		4096
#define MAX_FONTS       256

struct D3DERRORS
{
	HRESULT		err;
	char		* descrition;
};

struct texpaths_t
{
	char str[_MAX_PATH];
};

struct STEXTURE
{
	IDirect3DBaseTexture9	* d3dtex;
	char					* name;
	unsigned long			hash;		
	long					ref;
	dword					dwSize;
	bool					isCubeMap;
	bool					loaded;
};

//-----------buffers-----------
struct VERTEX_BUFFER
{
	dword					dwNumLocks;
	long					type;
	long					size;
	dword					usage;	
	bool					isManaged;
	IDirect3DVertexBuffer9	* buff;
};

struct INDEX_BUFFER
{
	dword					dwNumLocks;
	long					ntrgs;
	dword					usage;
	bool					isManaged;
	IDirect3DIndexBuffer9	* buff;
};

struct FONTENTITY
{
	char			* name;
    unsigned long	hash;
    FONT			* font;
    long			ref;
};

struct VideoTextureEntity
{
	char				* name;
	unsigned long		hash;
    long				ref;
	ENTITY_ID			videoTexture_id;
	CVideoTexture		* VideoTexture;
	VideoTextureEntity	* next;
};


// ----- Sound statistics -----
extern dword dwSoundBuffersCount;
extern dword dwSoundBytes;
extern dword dwSoundBytesCached;

//-----------SDEVICE-----------
class DX8RENDER  : public VDX8RENDER
{

#define RS_RECT_VERTEX_FORMAT	(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
	struct RECT_VERTEX
	{
		CVECTOR pos;
		dword	color;
		float	u, v;
	};


	struct RenderTarget
	{
		IDirect3DSurface9 * pRenderTarget;
		IDirect3DSurface9 * pDepthSurface;
		D3DVIEWPORT9		ViewPort;
	};

	IDirect3DDevice9	* d3d9;
	IDirect3D9			* d3d;
	HWND				hwnd;

	CVECTOR				Pos, Ang;
	float				Fov;

	CTechnique			* pTechnique;

    char				* fontIniFileName;
    long				nFontQuantity;
    FONTENTITY			FontList[MAX_FONTS];
    long				idFontCurrent;

	VideoTextureEntity	* pVTL;

	long				nTextureDegradation;
	float				aspectRatio;
	float				m_fHeightDeformator;
	
	bool				bSafeRendering;
	bool				bShowFps, bShowExInfo;
	bool				bInsideScene;

	PLANE				viewplane[4];

	STEXTURE			Textures[MAX_STEXTURES];
	INDEX_BUFFER		IndexBuffers[MAX_BUFFERS];
	VERTEX_BUFFER		VertexBuffers[MAX_BUFFERS];

	bool				MakeAvi;
	IDirect3DSurface9	* ImageBuffer;

	// VideoCapture section
	HDC					hDesktopDC, hCaptureDC;
	HBITMAP				hCaptureBitmap;
	LPBITMAPINFO		lpbi;
	long				iCaptureFrameIndex;
	bool				bPreparedCapture;
	bool				bVideoCapture;
	float				fFixedFPS;
	array<char*>		aCaptureBuffers;
	dword				dwCaptureBuffersReady;

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


	QuadVertex	PostProcessQuad[4];
	QuadVertex	qv[32 * 32];
	word		qi[31 * 31 * 2 * 3];

	float fSmallWidth;
	float fSmallHeight;
	IDirect3DTexture9* pPostProcessTexture;
	IDirect3DSurface9* pPostProcessSurface;

	IDirect3DTexture9* pSmallPostProcessTexture;
	IDirect3DSurface9* pSmallPostProcessSurface;

	IDirect3DTexture9* pSmallPostProcessTexture2;
	IDirect3DSurface9* pSmallPostProcessSurface2;

	IDirect3DSurface9* pOriginalScreenSurface;
	IDirect3DSurface9* pOriginalDepthSurface;

	D3DVIEWPORT9 OriginalViewPort;

	void CreateRenderQuad (float fWidth, float fHeight, float fSrcWidth, float fSrcHeight, float fMulU = 1.0f, float fMulV = 1.0f);
	
	void ClearPostProcessSurface (IDirect3DSurface9* pSurf);
	void BlurGlowTexture ();
	void CopyGlowToScreen ();
	void CopyPostProcessToScreen();

	void SetPostProcessTextureAsRenderTarget ();
	void SetScreenAsRenderTarget ();

	float fBlurSize;
	int GlowIntensity;
	int iBlurPasses;

	bool bNeedCopyToScreen;
	bool bPostProcessEnabled;
	bool bPostProcessError;
	bool bSeaEffect;
	float fSeaEffectSize;
	float fSeaEffectSpeed;
	dword dwBackColor;

//-------- post process


	// state save/load ex
	POINT			screen_size;
	D3DFORMAT		screen_bpp;
	D3DFORMAT		stencil_format;

	bool			bMakeShoot;
	bool			bWindow;
	bool			bBackBufferCanLock;

	IDirect3DVertexBuffer9	* aniVBuffer;
	long					numAniVerteces;

	IDirect3DVertexBuffer9	* pDropConveyorVBuffer;

	dword			dwNumDrawPrimitive, dwNumLV, dwNumLI;
	float			fG, fB, fC;
	D3DGAMMARAMP	DefaultRamp;

	float			fNearClipPlane, fFarClipPlane;

	bool			bLoadTextureEnabled;

	bool			bTrace;
	long			iSetupPath;
	dword			dwSetupNumber;
	texpaths_t		TexPaths[4];

	bool			bDropVideoConveyor;

	stack<RenderTarget>		stRenderTarget;

	bool TextureLoad(long texid);
	bool ErrorHandler(char *message, HRESULT hres);

	bool MakeCapture();
	void SaveCaptureBuffers();
	void PrepareCapture();
	
	int  GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	void GetPngScreenshot();
	void GetPng();

public:
	static DX8RENDER	* pRS;

	DX8RENDER();
	~DX8RENDER();
	void func();

	// DX8Render: Init/Release
		virtual bool InitDevice(bool windowed, HWND hwnd, long width, long height);
		virtual bool ReleaseDevice();

	// DX8Render: Animation
		virtual void RenderAnimation(long ib, void * src, long numVrts, long minv, long numv,  long startidx, long numtrg, bool isUpdateVB);

	// DX8Render: Return d3d9 device 
		virtual void * GetDevice() { return d3d9; };		
		virtual void * GetD3DDevice() { return d3d9; };		

	// DX8Render: Render Target/Begin/End/Clear
		virtual bool DX8Clear(long type);	//D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
		virtual bool DX8BeginScene();
		virtual bool DX8EndScene();

	// DX8Render: Materials/Lights Section
		virtual bool	SetLight(dword dwIndex, const D3DLIGHT9 * pLight);
		virtual bool	LightEnable(dword dwIndex, bool bOn);
		virtual bool	SetMaterial(D3DMATERIAL9 & material);
		virtual bool	GetLightEnable(DWORD dwIndex, BOOL * pEnable);
		virtual bool	GetLight(DWORD dwIndex, D3DLIGHT9 * pLight);

	// DX8Render: Screenshot Section
		virtual void SaveShoot();

	// DX8Render: Clip Planes Section
		virtual HRESULT SetClipPlane( DWORD Index, CONST float * pPlane );
		virtual PLANE * GetPlanes();

	// DX8Render: Camera Section
		virtual void SetTransform(long type, D3DMATRIX * mtx);
		virtual void GetTransform(long type, D3DMATRIX * mtx);

		virtual bool SetCamera(CVECTOR * pos, CVECTOR * ang, float perspective);
		virtual bool SetCamera(CVECTOR * pos, CVECTOR * ang);
		virtual bool SetCamera(CVECTOR lookFrom, CVECTOR lookTo, CVECTOR up);
		virtual bool SetPerspective(float perspective, float fAspectRatio = -1.0f);
		virtual void GetCamera(CVECTOR & pos, CVECTOR & ang, float & perspective);

		virtual bool SetCurrentMatrix(D3DMATRIX * mtx);

	// DX8Render: Textures Section
		virtual long TextureCreate(const char * fname);
		virtual bool TextureSet(long stage, long texid);
		virtual bool TextureRelease(long texid);

	// DX8Render: Fonts Section
		virtual long _cdecl Print(long x, long y,char * format,...);
		virtual long _cdecl Print(long nFontNum, DWORD color, long x, long y,char * format,...);
		virtual long _cdecl ExtPrint(long nFontNum, DWORD foreColor, DWORD backColor, int wAlignment,
							 bool bShadow, float fScale, long scrWidth, long scrHeight,
							 long x, long y,char * format,...);
		virtual long StringWidth(char * string, long nFontNum=0, float fScale=1.f, long scrWidth=0);
		virtual long CharWidth(char ch, long nFontNum=0, float fScale=1.f, long scrWidth=0);
		virtual long CharHeight(long fontID);
		virtual long LoadFont(char * fontName); // возвращает номер\идентификатор шрифта или -1 в случае ошибки
		virtual bool UnloadFont(char * fontName); // возвращает истину если шрифт остался в использовании
		virtual bool UnloadFont(long fontID); // возвращает истину если шрифт остался в использовании
		virtual bool SetCurFont(char * fontName); // возвращает истину если установили данный шрифт
		virtual bool SetCurFont(long fontID); // возвращает истину если установили данный шрифт
		virtual long GetCurFont();
		virtual char * GetFontIniFileName();
		virtual bool SetFontIniFileName(char * iniName);

	// DX8Render: Techniques Section
		virtual bool TechniqueSetParamsAndStart(const char * cBlockName, dword _dwNumParams = 0, void * pParams = null);
		virtual bool _cdecl TechniqueExecuteStart(const char * cBlockName, dword _dwNumParams = 0, ...);
		virtual bool TechniqueExecuteNext();

	// DX8Render: Draw Section
		virtual void _cdecl DrawRects(RS_RECT * pRSR, dword dwRectsNum, const char * cBlockName = 0, dword dwSubTexturesX = 1, dword dwSubTexturesY = 1, float fScaleX = 1.0f, float fScaleY = 1.0f, dword dwNumParams = 0, ...);
		virtual void _cdecl DrawSprites(RS_SPRITE * pRSS, dword dwSpritesNum, const char * cBlockName = 0, dword dwNumParams = 0, ...);
		virtual void _cdecl DrawLines(RS_LINE * pRSL, dword dwLinesNum, const char * cBlockName = 0, dword dwNumParams = 0, ...);
		virtual void _cdecl DrawLines2D(RS_LINE2D * pRSL2D, dword dwLinesNum, const char * cBlockName = 0, dword dwNumParams = 0, ...);

		virtual void _cdecl DrawBuffer(long vbuff, long stride, long ibuff, long minv, long numv, long startidx, long numtrg, const char *cBlockName = 0, dword dwNumParams = 0, ...);
		virtual void _cdecl DrawIndexedPrimitiveNoVShader(D3DPRIMITIVETYPE dwPrimitiveType, long iVBuff, long iStride, long iIBuff, long iMinV, long iNumV, long iStartIdx, long iNumTrg, const char *cBlockName = 0, dword dwNumParams = 0, ...); 
		virtual void _cdecl DrawPrimitive(D3DPRIMITIVETYPE dwPrimitiveType, long iVBuff, long iStride, long iStartV, long iNumPT, const char *cBlockName = 0, dword dwNumParams = 0, ...);
		virtual void _cdecl DrawPrimitiveUP(D3DPRIMITIVETYPE dwPrimitiveType, dword dwVertexBufferFormat, dword dwNumPT, void *pVerts, dword dwStride, const char *cBlockName = 0, dword dwNumParams = 0, ...);
		virtual void _cdecl DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE dwPrimitiveType, dword dwMinIndex, dword dwNumVertices, dword dwPrimitiveCount, const void *pIndexData, D3DFORMAT IndexDataFormat, const void *pVertexData, dword dwVertexStride, const char *cBlockName = 0, dword dwNumParams = 0, ...);

   	// DX8Render: Video Section
		virtual void				PlayToTexture();
		virtual CVideoTexture *		GetVideoTexture(char * sVideoName);
		virtual void				ReleaseVideoTexture(CVideoTexture * pVTexture);

	// DX8Render: Vertex/Index Buffers Section
		virtual long CreateVertexBuffer(long type, long nverts,dword usage);
		virtual long CreateIndexBuffer(long ntrgs, dword dwUsage = D3DUSAGE_WRITEONLY);
		
		virtual long CreateVertexBufferManaged(long type, long nverts,dword usage);
		virtual long CreateIndexBufferManaged(long ntrgs, dword dwUsage = D3DUSAGE_WRITEONLY);

		virtual IDirect3DVertexBuffer9 * GetVertexBuffer(long id);
		virtual long	GetVertexBufferFVF(long id);
		virtual void *	LockVertexBuffer(long id, dword dwFlags = 0);
		virtual void	UnLockVertexBuffer(long id);
		virtual long	GetVertexBufferSize(long id);
		virtual void *	LockIndexBuffer(long id, dword dwFlags = 0);
		virtual void	UnLockIndexBuffer(long id);
		virtual void	ReleaseVertexBuffer(long id);
		virtual void	ReleaseIndexBuffer(long id);

	// DX8Render: Render/Texture States Section
		virtual dword SetRenderState(dword State, dword Value);
		virtual dword SetRenderStateFloat(dword State, double Value);
		virtual dword GetRenderState(dword State, dword * pValue);
		virtual dword SetTextureStageState(dword Stage, dword Type, dword Value);
		virtual dword GetTextureStageState(dword Stage, dword Type, dword * pValue);

	// 	DX8Render: Render/Sampler States Section
		virtual dword SetSamplerState(dword Stage, dword Type, dword Value);
		virtual dword GetSamplerState(dword Stage, dword Type, dword * pValue);	

	// aspect ratio section
		virtual float GetHeightDeformator() {return m_fHeightDeformator;}

// ===============================================================================================
// --------------------===================== D3D SECTION =====================--------------------
// ===============================================================================================

	// D3D Device/Viewport Section
		virtual HRESULT GetViewport(D3DVIEWPORT9 * pViewport);
		virtual HRESULT SetViewport(const D3DVIEWPORT9 * pViewport);
		virtual HRESULT GetDeviceCaps(D3DCAPS9* pCaps);

	// D3D 
		virtual HRESULT SetStreamSource(UINT StreamNumber, void * pStreamData, UINT Stride);
		virtual HRESULT SetIndices(void * pIndexData, UINT BaseVertexIndex);
		virtual HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
		virtual HRESULT Release(IUnknown *pSurface);

	// Vertex/Index Buffers Section
		virtual HRESULT CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer);
		virtual HRESULT VBLock(IDirect3DVertexBuffer9 * pVB, UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags);
		virtual void VBUnlock(IDirect3DVertexBuffer9 * pVB);

	// D3D Textures/Surfaces Section
		virtual HRESULT GetDepthStencilSurface( IDirect3DSurface9** ppZStencilSurface );
		virtual HRESULT GetCubeMapSurface( IDirect3DCubeTexture9* ppCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface9** ppCubeMapSurface );
		virtual HRESULT CreateTexture( UINT Width, UINT Height, UINT  Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture );
		virtual HRESULT CreateCubeTexture( UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture );
		virtual HRESULT CreateImageSurface( UINT Width, UINT Height, D3DFORMAT Format, IDirect3DSurface9 * * ppSurface);
		virtual HRESULT CreateDepthStencilSurface( UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, IDirect3DSurface9** ppSurface );
		virtual HRESULT SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture );
		virtual HRESULT GetLevelDesc( IDirect3DTexture9* ppTexture, UINT Level, D3DSURFACE_DESC* pDesc );
		virtual HRESULT GetLevelDesc( IDirect3DCubeTexture9* ppCubeTexture, UINT Level, D3DSURFACE_DESC* pDesc );
		virtual HRESULT LockRect( IDirect3DCubeTexture9* ppCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags );
		virtual HRESULT LockRect( IDirect3DTexture9* ppTexture, UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags );
		virtual HRESULT UnlockRect( IDirect3DCubeTexture9 *pCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level );
		virtual HRESULT UnlockRect( IDirect3DTexture9 *pTexture, UINT Level );
		virtual HRESULT GetSurfaceLevel( IDirect3DTexture9* ppTexture, UINT Level, IDirect3DSurface9** ppSurfaceLevel );
		virtual HRESULT CopyRects( IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRectsArray, UINT cRects, IDirect3DSurface9* pDestinationSurface, CONST RECT* pDestRectsArray );
		virtual HRESULT GetRenderTargetData( IDirect3DSurface9* pSourceSurface, IDirect3DSurface9* pDestinationSurface );
		virtual HRESULT UpdateSurface( IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRectsArray, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint );
		virtual HRESULT CreateRenderTarget( UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface );
		virtual HRESULT StretchRect(IDirect3DSurface9  *pSourceSurface, const RECT  *pSourceRect, IDirect3DSurface9  *pDestSurface, const RECT *pDestRect, D3DTEXTUREFILTERTYPE Filter);

	// D3D Pixel/Vertex Shaders Section
		virtual HRESULT CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements, IDirect3DVertexDeclaration9 **ppDecl);
		virtual HRESULT SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl);
		virtual HRESULT SetVertexShaderConstant(DWORD Register, CONST void* pConstantData, DWORD  ConstantCount );
		virtual HRESULT SetPixelShaderConstant( DWORD Register, CONST void* pConstantData, DWORD ConstantCount );
		virtual HRESULT GetVertexShader(IDirect3DVertexShader9 **ppShader);
		virtual HRESULT GetPixelShader(IDirect3DPixelShader9 **ppShader);
		virtual HRESULT CreateVertexShader(CONST DWORD * pFunction, IDirect3DVertexShader9 **ppShader);		
		virtual HRESULT CreatePixelShader(CONST DWORD * pFunction, IDirect3DPixelShader9 **ppShader);
		virtual HRESULT DeleteVertexShader( IDirect3DVertexShader9 *pShader );
		virtual HRESULT DeletePixelShader( IDirect3DPixelShader9 *pShader );
		virtual HRESULT SetVertexShader(IDirect3DVertexShader9 *pShader);
		virtual HRESULT SetPixelShader(IDirect3DPixelShader9 *pShader);
		virtual HRESULT SetFVF(DWORD handle);

	// D3D Render Target/Begin/End/Clear
		virtual HRESULT GetRenderTarget(IDirect3DSurface9** ppRenderTarget);
		virtual HRESULT SetRenderTarget( IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pNewZStencil );
		virtual HRESULT Clear( DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil );
		virtual HRESULT BeginScene();
		virtual HRESULT EndScene();

		virtual HRESULT ImageBlt(const char * pName, RECT * pDstRect, RECT * pSrcRect);
		virtual HRESULT ImageBlt(long nTextureId, RECT * pDstRect, RECT * pSrcRect);


	void MakeScreenShot();
	bool LoadTextureSurface(HANDLE file, IDirect3DSurface9 * suface, dword mipSize, dword width, dword height, bool isSwizzled);
	dword LoadCubmapSide(HANDLE file, IDirect3DCubeTexture9 * tex, D3DCUBEMAP_FACES face, dword numMips, dword mipSize, dword size, bool isSwizzled);

	// core interface
	bool  Init();
	void  RunStart();
	void  RunEnd();
	dword RunSection(){return SECTION_REALIZE;};
	bool  LoadState(ENTITY_STATE * state);
	bool  CreateState(ENTITY_STATE_GEN * state_gen);

	void ProcessScriptPosAng(CVECTOR & vPos, CVECTOR & vAng);
	void FindPlanes(IDirect3DDevice9 * d3dDevice);
	
	virtual void SetProgressImage(const char * image);
	virtual void SetProgressBackImage(const char * image);
	virtual void SetTipsImage(const char * image);
	virtual void StartProgressView();
	virtual void ProgressView();
	virtual void EndProgressView();
	
	void LostRender();
	long RestoreRender();
	void RecoverDevice();
	
	static const dword rectsVBuffer_SizeInRects;
	IDirect3DVertexBuffer9	* rectsVBuffer;

	char * progressImage;
	long progressImageSize;
	long backTexture;
	
	char * progressBackImage;
	long progressBackImageSize;
	long back0Texture;
	long progressTexture;
	char * progressTipsImage;
	long progressTipsImageSize;
	long progressTipsTexture;

	long loadFrame;
	long progressSafeCounter;	
	bool isInPViewProcess;
	dword progressUpdateTime;
	float progressFramesPosX;
	float progressFramesPosY;
	float progressFramesWidth;
	float progressFramesHeight;
	long progressFramesCountX;
	long progressFramesCountY;

	D3DPRESENT_PARAMETERS d3dpp;
	bool bDeviceLost;

	CMatrix mView, mWorld, mProjection;

	CVECTOR vWordRelationPos;
	CVECTOR vViewRelationPos;

	bool bUseLargeBackBuffer;

	virtual bool IsInsideScene() {return bInsideScene;}

	char * GetTipsImage();

	void SetColorParameters(float fGamma, float fBrightness, float fContrast);
	void DrawSphere(const CVECTOR & vPos, float fRadius, dword dwColor);

	void GetNearFarPlane(float & fNear, float & fFar);
	void SetNearFarPlane(float fNear, float fFar);

	virtual void SetLoadTextureEnable(bool bEnable = true);
	bool ResetDevice();

	void MakeDrawVector(RS_LINE * pLines, dword dwNumSubLines, const CMatrix & mMatrix, CVECTOR vUp, CVECTOR v1, CVECTOR v2, float fScale, dword dwColor);
	void DrawVector(const CVECTOR & v1, const CVECTOR & v2, dword dwColor, const char * pTechniqueName, dword dwNumParams = 0, ...);
	IDirect3DBaseTexture9 * GetBaseTexture(long iTexture);

	virtual IDirect3DBaseTexture9 * CreateTextureFromFileInMemory(const char * pFile, dword dwSize);

	virtual bool PushRenderTarget();
	virtual bool PopRenderTarget();
	virtual bool SetRenderTarget(IDirect3DCubeTexture9 * pCubeTex, dword dwFaceType, dword dwLevel, IDirect3DSurface9* pNewZStencil);
	virtual void SetView(const CMatrix & mView);
	virtual void SetWorld(const CMatrix & mView);
	virtual void SetProjection(const CMatrix & mView);
	virtual const CMatrix & GetView();
	virtual const CMatrix & GetWorld();
	virtual const CMatrix & GetProjection();

	virtual IDirect3DVolumeTexture9 * CreateVolumeTexture(dword Width, dword Height, dword Depth, dword Levels, dword Usage, D3DFORMAT Format, D3DPOOL Pool);

	virtual void MakePostProcess();
	virtual void SetGLOWParams (float _fBlurBrushSize, long _GlowIntensity, long _GlowPasses);

	virtual IDirect3DBaseTexture9* GetTextureFromID(long nTextureID);
};

#endif