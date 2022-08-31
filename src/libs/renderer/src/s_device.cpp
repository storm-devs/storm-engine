#include "s_device.h"

#include "core.h"

#include "entity.h"
#include "math_inlines.h"
#include "s_import_func.h"
#include "texture.h"
#include "v_s_stack.h"
#include "fs.h"
#include "debug-trap.h"
#include "string_compare.hpp"

#include <algorithm>
#include <SDL_timer.h>

#include <fmt/chrono.h>

#ifdef _WIN32
#include <DxErr.h>
#include <corecrt_io.h>
#else
#include <unistd.h>
#endif

CREATE_SERVICE(DX9RENDER)

CREATE_SCRIPTLIBRIARY(DX9RENDER_SCRIPT_LIBRIARY)

#define POST_PROCESS_FVF (D3DFVF_XYZRHW | D3DFVF_TEX4)

#define S_RELEASE(a, b)                                                                                                \
    if (a)                                                                                                             \
    {                                                                                                                  \
        ULONG refc = a->Release();                                                                                     \
        a = NULL;                                                                                                      \
    }

namespace
{
constexpr auto kKeyTakeScreenshot = "TakeScreenshot";

#ifdef _WIN32 // Screenshot
D3DXIMAGE_FILEFORMAT GetScreenshotFormat(const std::string &fmt)
{
    if (fmt == "bmp")
    {
        return D3DXIFF_BMP;
    }
    if (fmt == "jpg")
    {
        return D3DXIFF_JPG;
    }
    if (fmt == "tga")
    {
        return D3DXIFF_TGA;
    }
    if (fmt == "png")
    {
        return D3DXIFF_PNG;
    }
    if (fmt == "dds")
    {
        return D3DXIFF_DDS;
    }
    if (fmt == "ppm")
    {
        return D3DXIFF_PPM;
    }
    if (fmt == "dib")
    {
        return D3DXIFF_DIB;
    }
    if (fmt == "hdr")
    {
        return D3DXIFF_HDR;
    }
    if (fmt == "pfm")
    {
        return D3DXIFF_PFM;
    }

    return D3DXIFF_FORCE_DWORD;
}
#endif

void InvokeEntitiesLostRender()
{
    core.ForEachEntity([](entptr_t entity_ptr) { entity_ptr->ProcessStage(Entity::Stage::lost_render); });
}
void InvokeEntitiesRestoreRender()
{
    core.ForEachEntity([](entptr_t entity_ptr) { entity_ptr->ProcessStage(Entity::Stage::restore_render); });
}

} // namespace

DX9RENDER *DX9RENDER::pRS = nullptr;

class LostDeviceSentinel : public SERVICE
{
    void RunStart() override
    {
        if (auto d3d9 = static_cast<IDirect3DDevice9 *>(DX9RENDER::pRS->GetD3DDevice()))
        {
            switch (d3d9->TestCooperativeLevel())
            {
            case D3DERR_DEVICENOTRESET:
                if (!DX9RENDER::pRS->ResetDevice())
                {
                    core.stopFrameProcessing();
                }
                break;

            case D3DERR_DEVICELOST:
                core.stopFrameProcessing();
                break;
            }
        }

    }
};
CREATE_SERVICE(LostDeviceSentinel)

uint32_t DX9SetTexturePath(VS_STACK *pS)
{
    auto *pString = (VDATA *)pS->Pop();
    auto *pNumber = (VDATA *)pS->Pop();

    const uintptr_t iNumber = pNumber->GetInt();
    auto *const pStr = pString->GetString();

    if (!DX9RENDER::pRS)
    {
        core.GetService("dx9render");
        Assert(DX9RENDER::pRS);
    }

    auto *pVR = (VDATA *)pS->Push();
    if (!pVR || iNumber < 0 || iNumber >= 4)
    {
        pVR->Set(0);
        return IFUNCRESULT_OK;
    }

    DX9RENDER::pRS->TextureCreate((const char *)-1);
    DX9RENDER::pRS->TextureCreate((const char *)iNumber);
    DX9RENDER::pRS->TextureCreate(static_cast<const char *>(pStr));

    pVR->Set(1);

    return IFUNCRESULT_OK;
}

uint32_t RPrint(VS_STACK *pS)
{
    auto *pString = (VDATA *)pS->Pop();
    const int32_t y = ((VDATA *)pS->Pop())->GetInt();
    const int32_t x = ((VDATA *)pS->Pop())->GetInt();

    if (pString->GetString())
        DX9RENDER::pRS->Print(x, y, pString->GetString());
    auto *pVR = (VDATA *)pS->Push();
    pVR->Set(0);
    return IFUNCRESULT_OK;
}

uint32_t SetGlowParams(VS_STACK *pS)
{
    const float fBlurBrushSize = ((VDATA *)pS->Pop())->GetFloat();
    const int32_t Intensivity = ((VDATA *)pS->Pop())->GetInt();
    const int32_t BlurPasses = ((VDATA *)pS->Pop())->GetInt();

    DX9RENDER::pRS->SetGLOWParams(fBlurBrushSize, Intensivity, BlurPasses);

    auto *pVR = (VDATA *)pS->Push();
    pVR->Set(0);
    return IFUNCRESULT_OK;
}

uint32_t slGetTexture(VS_STACK *pS)
{
    auto filename = ((VDATA *)pS->Pop())->GetString();
    int32_t texNum = DX9RENDER::pRS->TextureCreate(filename);

    auto *pVR = (VDATA *)pS->Push();
    pVR->Set(texNum);
    return IFUNCRESULT_OK;
}

uint32_t slReleaseTexture(VS_STACK *pS)
{
    int32_t texNum = ((VDATA *)pS->Pop())->GetInt();

    DX9RENDER::pRS->TextureRelease(texNum);
    return IFUNCRESULT_OK;
}

bool DX9RENDER_SCRIPT_LIBRIARY::Init()
{
    IFUNCINFO sIFuncInfo;

    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncName = "SetTexturePath";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = DX9SetTexturePath;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 3;
    sIFuncInfo.pFuncName = "RPrint";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = RPrint;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 3;
    sIFuncInfo.pFuncName = "SetGlowParams";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = SetGlowParams;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "GetTexture";
    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncAddress = slGetTexture;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncName = "ReleaseTexture";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = slReleaseTexture;
    core.SetScriptFunction(&sIFuncInfo);

    return true;
}

uint64_t _rdtsc;
uint32_t dwTotalSize = 0;
uint32_t dwSplashTime = 0;
bool bSplash = false;
#pragma warning(push)
#pragma warning(disable : 4305 4309)
char sSplashText[] = {'\xbb', '\x9a', '\x89', '\x9a', '\x93', '\x90', '\x8f', '\x9a', '\x8d', '\x8c', '\xdf',
                      '\xbc', '\x90', '\x8f', '\x86', '\xd1', '\xdf', '\xb1', '\x90', '\x8b', '\xdf', '\x99',
                      '\x90', '\x8d', '\xdf', '\xac', '\x9e', '\x93', '\x9a', '\xd1', '\0'};
#pragma warning(pop)
char splashbuffer[256];

#define VIDEODIR "Resource\\Videos\\%s"

struct DX9SphVertex
{
    CVECTOR v;
    uint32_t c;
};

uint32_t DX9sphereNumTrgs;
DX9SphVertex *DX9sphereVertex = nullptr;

void CreateSphere()
{
#define CalcKColor(ind)                                                                                                \
    {                                                                                                                  \
        kColor = light | !CVECTOR(DX9sphereVertex[t * 3 + ind].v.x, DX9sphereVertex[t * 3 + ind].v.y,                  \
                                  DX9sphereVertex[t * 3 + ind].v.z);                                                   \
        if (kColor < 0.0f)                                                                                             \
            kColor = 0.0f;                                                                                             \
    }
#define CLerp(c, min) (uint32_t(c * (kColor * (1.0f - min) + min)))
#define Color1                                                                                                         \
    ((CLerp(255.0f, 0.5f) << 24) | (CLerp(255.0f, 0.7f) << 16) | (CLerp(255.0f, 0.7f) << 8) |                          \
     (CLerp(255.0f, 0.7f) << 0));

    if (DX9sphereVertex)
        return;

    const float myPI = 3.1415926535897932f;
    const int32_t a1 = 32;
    const int32_t a2 = (a1 / 2);

    DX9sphereNumTrgs = a1 * a2 * 2;
    DX9sphereVertex = new DX9SphVertex[DX9sphereNumTrgs * 6];

    const CVECTOR light = !CVECTOR(0.0f, 0.0f, 1.0f);
    float kColor;
    // fill in the vertices
    for (int32_t i = 0, t = 0; i < a2; i++)
    {
        const float r1 = sinf(myPI * i / static_cast<float>(a2));
        const float y1 = cosf(myPI * i / static_cast<float>(a2));
        const float r2 = sinf(myPI * (i + 1) / static_cast<float>(a2));
        const float y2 = cosf(myPI * (i + 1) / static_cast<float>(a2));
        for (int32_t j = 0; j < a1; j++)
        {
            const float x1 = sinf(2.0f * myPI * j / static_cast<float>(a1));
            const float z1 = cosf(2.0f * myPI * j / static_cast<float>(a1));
            const float x2 = sinf(2.0f * myPI * (j + 1) / static_cast<float>(a1));
            const float z2 = cosf(2.0f * myPI * (j + 1) / static_cast<float>(a1));
            // 0
            DX9sphereVertex[t * 3 + 0].v.x = r1 * x1;
            DX9sphereVertex[t * 3 + 0].v.y = y1;
            DX9sphereVertex[t * 3 + 0].v.z = r1 * z1;
            CalcKColor(0);
            DX9sphereVertex[t * 3 + 0].c = Color1;
            // 1
            DX9sphereVertex[t * 3 + 1].v.x = r2 * x1;
            DX9sphereVertex[t * 3 + 1].v.y = y2;
            DX9sphereVertex[t * 3 + 1].v.z = r2 * z1;
            CalcKColor(1);
            DX9sphereVertex[t * 3 + 1].c = Color1;
            // 2
            DX9sphereVertex[t * 3 + 2].v.x = r1 * x2;
            DX9sphereVertex[t * 3 + 2].v.y = y1;
            DX9sphereVertex[t * 3 + 2].v.z = r1 * z2;
            CalcKColor(2);
            DX9sphereVertex[t * 3 + 2].c = Color1;
            // 3 = 2
            DX9sphereVertex[t * 3 + 3] = DX9sphereVertex[t * 3 + 2];
            // 4 = 1
            DX9sphereVertex[t * 3 + 4] = DX9sphereVertex[t * 3 + 1];
            // 5
            DX9sphereVertex[t * 3 + 5].v.x = r2 * x2;
            DX9sphereVertex[t * 3 + 5].v.y = y2;
            DX9sphereVertex[t * 3 + 5].v.z = r2 * z2;
            CalcKColor(5);
            DX9sphereVertex[t * 3 + 5].c = Color1;
            // added 2 triangles
            t += 2;
        }
    }
}

struct SD_TEXTURE_FORMAT
{
    TX_FORMAT txFormat;
    D3DFORMAT d3dFormat;
    bool isSwizzled;
    const char *format;
};

SD_TEXTURE_FORMAT textureFormats[] = {
    {TXF_DXT1, D3DFMT_DXT1, true, "D3DFMT_DXT1"},
    {TXF_DXT3, D3DFMT_DXT3, true, "D3DFMT_DXT3"},
    {TXF_DXT5, D3DFMT_DXT5, true, "D3DFMT_DXT5"},
    {TXF_A8R8G8B8, D3DFMT_A8R8G8B8, false, "D3DFMT_A8R8G8B8"},
    {TXF_X8R8G8B8, D3DFMT_X8R8G8B8, false, "D3DFMT_X8R8G8B8"},
    {TXF_R5G6B5, D3DFMT_R5G6B5, false, "D3DFMT_R5G6B5"},
    {TXF_A4R4G4B4, D3DFMT_A4R4G4B4, false, "D3DFMT_A4R4G4B4"},
    {TXF_A1R5G5B5, D3DFMT_A1R5G5B5, false, "D3DFMT_A1R5G5B5"},
};

uint32_t dwSoundBuffersCount = 0;
uint32_t dwSoundBytes = 0;
uint32_t dwSoundBytesCached = 0;

//################################################################################
//~!~
/*#define STRIPPATH(s)\
    (sizeof(s) > 11 && (s)[sizeof(s)-12] == '/' ? (s) + sizeof(s) - 11 : \
    sizeof(s) > 10 && (s)[sizeof(s)-11] == '/' ? (s) + sizeof(s) - 10 : \
    sizeof(s) > 9 && (s)[sizeof(s)-10] == '/' ? (s) + sizeof(s) - 9 : \
    sizeof(s) > 8 && (s)[sizeof(s)-9] == '/' ? (s) + sizeof(s) - 8 : \
    sizeof(s) > 7 && (s)[sizeof(s)-8] == '/' ? (s) + sizeof(s) - 7 : \
    sizeof(s) > 6 && (s)[sizeof(s)-7] == '/' ? (s) + sizeof(s) - 6 : \
    sizeof(s) > 5 && (s)[sizeof(s)-6] == '/' ? (s) + sizeof(s) - 5 : \
    sizeof(s) > 4 && (s)[sizeof(s)-5] == '/' ? (s) + sizeof(s) - 4 : \
    sizeof(s) > 3 && (s)[sizeof(s)-4] == '/' ? (s) + sizeof(s) - 3 : \
    sizeof(s) > 2 && (s)[sizeof(s)-3] == '/' ? (s) + sizeof(s) - 2 : (s))*/

#define CHECKD3DERR(expr) ErrorHandler(expr, __FILE__, __LINE__, __func__, #expr)

inline bool ErrorHandler(HRESULT hr, const char *file, unsigned line, const char *func, const char *expr)
{
    if (hr != D3D_OK)
    {
#ifdef _WIN32
        core.Trace("[%s:%s:%d] %s: %s (%s)", file, func, line, DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr), expr);
#else
        core.Trace("[%s:%s:%d] (%s)", file, func, line, expr);
#endif
        return true;
    }

    return false;
}

const uint32_t DX9RENDER::rectsVBuffer_SizeInRects = 512;

//################################################################################
DX9RENDER::DX9RENDER()
{
    rectsVBuffer = nullptr;

    bPostProcessEnabled = true;
    bPostProcessError = false;
    iBlurPasses = 4;
    GlowIntensity = 200;
    fBlurSize = 0.8f;

    pPostProcessTexture = nullptr;
    pSmallPostProcessTexture = nullptr;
    pSmallPostProcessTexture2 = nullptr;

    pPostProcessSurface = nullptr;
    pSmallPostProcessSurface = nullptr;
    pSmallPostProcessSurface2 = nullptr;

    pOriginalScreenSurface = nullptr;
    pOriginalDepthSurface = nullptr;

    pRS = this;
    d3d = nullptr;
    d3d9 = nullptr;
    aniVBuffer = nullptr;
    numAniVerteces = 0;
    pVTL = nullptr;
    nFontQuantity = 0;
    idFontCurrent = 0;
    fontIniFileName = nullptr;

    bLoadTextureEnabled = true;

    bSeaEffect = false;
    fSeaEffectSize = 0.0f;
    fSeaEffectSpeed = 0.0f;
    dwBackColor = 0x0;

    bTrace = true;
    iSetupPath = 0;

    bDropVideoConveyor = false;
    pDropConveyorVBuffer = nullptr;

    aspectRatio = -1.0f;

    bMakeShoot = false;
    bShowFps = false;
    bShowExInfo = false;
    bInsideScene = false;

    m_fHeightDeformator = 1.f;

    progressImage = nullptr;
    progressImageSize = 0;
    progressTexture = -1;
    progressBackImage = nullptr;
    progressBackImageSize = 0;
    progressTipsImage = nullptr;
    progressTipsImageSize = 0;
    progressTipsTexture = -1;
    loadFrame = 0;
    backTexture = -1;
    back0Texture = -1;
    progressSafeCounter = 0;
    isInPViewProcess = false;
    progressUpdateTime = 0;
    progressFramesPosX = 0.85f;
    progressFramesPosY = 0.8f;
    progressFramesWidth = 64;
    progressFramesHeight = 64;
    progressFramesCountX = 8;
    progressFramesCountY = 8;

    vViewRelationPos = CVECTOR(0.f, 0.f, 0.f);
    vWordRelationPos = -vViewRelationPos;
    bUseLargeBackBuffer = false;
}

static bool texLog = false;
static float fSin = 0.0f;

bool DX9RENDER::Init()
{
    if (auto *sentinelService = core.GetService("LostDeviceSentinel"); !sentinelService)
    {
        throw std::runtime_error("Cannot create LostDeviceSentinel! Abort");
    }

    char str[256];
    for (int32_t i = 0; i < MAX_STEXTURES; i++)
        Textures[i].ref = NULL;

    d3d = nullptr;
    d3d9 = nullptr;

    create_directories(fs::GetScreenshotsPath());

    auto ini = fio->OpenIniFile(core.EngineIniFileName());
    if (ini)
    {
        // bPostProcessEnabled = ini->GetInt(0, "PostProcess", 0) == 1;
        bPostProcessEnabled = false; //~!~

        // screenshot format and extension
        ini->ReadString(nullptr, "screenshot_format", str, sizeof(str), "jpg");
        screenshotExt = str;
        std::ranges::transform(screenshotExt, screenshotExt.begin(),
                               [](const unsigned char c) { return std::tolower(c); });
#ifdef _WIN32 // Screenshot
        screenshotFormat = GetScreenshotFormat(str);
        if (screenshotFormat == D3DXIFF_FORCE_DWORD)
        {
            screenshotExt = "jpg";
            screenshotFormat = D3DXIFF_JPG;
        }
#endif

        bShowFps = ini->GetInt(nullptr, "show_fps", 0) == 1;
        bShowExInfo = ini->GetInt(nullptr, "show_exinfo", 0) == 1;
        bSafeRendering = ini->GetInt(nullptr, "safe_render", 0) == 0;
        bDropVideoConveyor = ini->GetInt(nullptr, "DropVideoConveyor", 0) != 0;
        texLog = ini->GetInt(nullptr, "texture_log", 0) == 1;
        bUseLargeBackBuffer = ini->GetInt(nullptr, "UseLargeBackBuffer", 0) != 0;

        bWindow = ini->GetInt(nullptr, "full_screen", 1) == 0;

        nTextureDegradation = ini->GetInt(nullptr, "texture_degradation", 0);

        FovMultiplier = ini->GetFloat(nullptr, "fov_multiplier", 1.0f);

        screen_size.x = ini->GetInt(nullptr, "screen_x", 1024);
        screen_size.y = ini->GetInt(nullptr, "screen_y", 768);
        fNearClipPlane = ini->GetFloat(nullptr, "NearClipPlane", 0.1f);
        fFarClipPlane = ini->GetFloat(nullptr, "FarClipPlane", 4000.0f);
        bBackBufferCanLock = ini->GetInt(nullptr, "lockable_back_buffer", 0) != 0;
        ini->ReadString(nullptr, "screen_bpp", str, sizeof(str), "D3DFMT_R5G6B5");
        screen_bpp = D3DFMT_R5G6B5;
        stencil_format = D3DFMT_D16;
        if (storm::iEquals(str, "D3DFMT_A8R8G8B8"))
        {
            screen_bpp = D3DFMT_A8R8G8B8;
            stencil_format = D3DFMT_D24S8;
        }
        if (storm::iEquals(str, "D3DFMT_X8R8G8B8"))
        {
            screen_bpp = D3DFMT_X8R8G8B8;
            stencil_format = D3DFMT_D24S8;
        }
        if (storm::iEquals(str, "D3DFMT_R5G6B5"))
        {
            screen_bpp = D3DFMT_R5G6B5;
            stencil_format = D3DFMT_D16;
        }

        // new renderer settings
        vSyncEnabled = ini->GetInt(nullptr, "vsync", 0);

        msaa = ini->GetInt(nullptr, "msaa", D3DMULTISAMPLE_NONE);
        if (msaa != D3DMULTISAMPLE_NONE)
        {
            if (msaa < D3DMULTISAMPLE_2_SAMPLES || msaa > D3DMULTISAMPLE_16_SAMPLES)
            {
                msaa = D3DMULTISAMPLE_16_SAMPLES;
            }
        }

        videoAdapterIndex = ini->GetInt(nullptr, "adapter", std::numeric_limits<int32_t>::max());

        // stencil_format = D3DFMT_D24S8;
        if (!InitDevice(bWindow, static_cast<HWND>(core.GetWindow()->OSHandle()), screen_size.x, screen_size.y))
            return false;

#ifdef _WIN32 // Effects
        RecompileEffects();
#else
        pTechnique = std::make_unique<CTechnique>(this);
        pTechnique->DecodeFiles();
#endif

        // get start ini file for fonts
        if (!ini->ReadString(nullptr, "startFontIniFile", str, sizeof(str) - 1, ""))
        {
            core.Trace("Not finded 'startFontIniFile' parameter into ENGINE.INI file");
            sprintf_s(str, "resource\\ini\\fonts.ini");
        }
        const auto len = strlen(str) + 1;
        if ((fontIniFileName = new char[len]) == nullptr)
            throw std::runtime_error("allocate memory error");
        strcpy_s(fontIniFileName, len, str);
        // get start font quantity
        if (!ini->ReadString(nullptr, "font", str, sizeof(str) - 1, ""))
        {
            core.Trace("Start font not defined");
            sprintf_s(str, "normal");
        }
        if (LoadFont(str) == -1L)
            core.Trace("can not init start font: %s", str);
        idFontCurrent = 0L;

        // Progress image parameters
        progressFramesPosX = ini->GetFloat("ProgressImage", "RelativePosX", 0.85f);
        progressFramesPosY = ini->GetFloat("ProgressImage", "RelativePosY", 0.8f);
        progressFramesWidth = ini->GetFloat("ProgressImage", "RelativeWidth", 0.0625f);
        if (progressFramesWidth < 0.0f)
            progressFramesWidth = 0.0f;
        if (progressFramesWidth > 10.0f)
            progressFramesWidth = 10.0f;
        progressFramesHeight = ini->GetFloat("ProgressImage", "RelativeHeight", 0.0625f);
        if (progressFramesHeight < 0.0f)
            progressFramesHeight = 0.0f;
        if (progressFramesHeight > 10.0f)
            progressFramesHeight = 10.0f;
        progressFramesCountX = static_cast<int32_t>(ini->GetFloat("ProgressImage", "HorisontalFramesCount", 8));
        if (progressFramesCountX < 1)
            progressFramesCountX = 1;
        if (progressFramesCountX > 64)
            progressFramesCountX = 64;
        progressFramesCountY = static_cast<int32_t>(ini->GetFloat("ProgressImage", "VerticalFramesCount", 8));
        if (progressFramesCountY < 1)
            progressFramesCountY = 1;
        if (progressFramesCountY > 64)
            progressFramesCountY = 64;

        CreateSphere();
        auto *pScriptRender = static_cast<VDATA *>(core.GetScriptVariable("Render"));
        ATTRIBUTES *pARender = pScriptRender->GetAClass();

        pARender->SetAttributeUseDword("full_screen", !bWindow);
        pARender->SetAttributeUseDword("screen_x", screen_size.x);
        pARender->SetAttributeUseDword("screen_y", screen_size.y);
    }

    pDropConveyorVBuffer = nullptr;
    rectsVBuffer = nullptr;
    d3d9->CreateVertexBuffer(2 * sizeof(CVECTOR), D3DUSAGE_WRITEONLY, D3DFVF_XYZ, D3DPOOL_MANAGED,
                             &pDropConveyorVBuffer, nullptr);
    d3d9->CreateVertexBuffer(rectsVBuffer_SizeInRects * 6 * sizeof(RECT_VERTEX), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                             RS_RECT_VERTEX_FORMAT, D3DPOOL_DEFAULT, &rectsVBuffer, nullptr); // D3DPOOL_MANAGED
    if (!pDropConveyorVBuffer || !rectsVBuffer)
    {
        return false;
    }

    uint16_t *pI = &qi[0];
    // setup ibuffer
    for (int32_t y = 0; y < 31; y++)
    {
        for (int32_t x = 0; x < 31; x++)
        {
            *pI++ = static_cast<uint16_t>((y + 0) * 32 + x + 1);
            *pI++ = static_cast<uint16_t>((y + 1) * 32 + x);
            *pI++ = static_cast<uint16_t>((y + 0) * 32 + x);

            *pI++ = static_cast<uint16_t>((y + 0) * 32 + x + 1);
            *pI++ = static_cast<uint16_t>((y + 1) * 32 + x + 1);
            *pI++ = static_cast<uint16_t>((y + 1) * 32 + x);
        }
    }

    const auto ctrl = core.Controls->CreateControl(kKeyTakeScreenshot);
    core.Controls->MapControl(ctrl, VK_F8);

    // UNGUARD
    return true;
}

//################################################################################
DX9RENDER::~DX9RENDER()
{
    // d3d9->SetRenderTarget(NULL, NULL); //~!~

    S_RELEASE(pSmallPostProcessSurface2, 0);
    S_RELEASE(pPostProcessSurface, 1);
    S_RELEASE(pSmallPostProcessSurface, 2);

    S_RELEASE(pOriginalScreenSurface, 3);
    S_RELEASE(pOriginalDepthSurface, 4);

    S_RELEASE(pSmallPostProcessTexture2, 5);
    S_RELEASE(pPostProcessTexture, 6);
    S_RELEASE(pSmallPostProcessTexture, 7);

    S_RELEASE(rectsVBuffer, 8);

    delete progressImage;
    progressImage = nullptr;
    delete progressBackImage;
    progressBackImage = nullptr;
    delete progressTipsImage;
    progressTipsImage = nullptr;

    for (int i = 0; i < nFontQuantity; i++)
    {
        delete FontList[i].font;

        delete FontList[i].name;
    }
    nFontQuantity = 0;
    delete fontIniFileName;

    STORM_DELETE(DX9sphereVertex);
    ReleaseDevice();
}

bool DX9RENDER::InitDevice(bool windowed, HWND _hwnd, int32_t width, int32_t height)
{
    // GUARD(DX9RENDER::InitDevice)

    aniVBuffer = nullptr;
    numAniVerteces = 0;

    screen_size.x = width;
    screen_size.y = height;
    bWindow = windowed;

    hwnd = _hwnd;
    core.Trace("Initializing DirectX 9");
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (d3d == nullptr)
    {
        // MessageBox(hwnd, "Direct3DCreate9 error", "InitDevice::Direct3DCreate9", MB_OK);
        core.Trace("Direct3DCreate9 error : InitDevice::Direct3DCreate9");
        return false;
    }

    d3dpp = {};
    d3dpp.BackBufferWidth = width;
    d3dpp.BackBufferHeight = height;
    d3dpp.BackBufferFormat = screen_bpp;
    d3dpp.BackBufferCount = 1;
    d3dpp.hDeviceWindow = hwnd;
    d3dpp.Windowed = windowed;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = stencil_format;

    if (windowed)
    {
        D3DDISPLAYMODE d3ddm;
        if (FAILED(d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
            throw std::runtime_error("failed to GetAdapterDisplayMode");
        d3dpp.BackBufferFormat = d3ddm.Format;
        if (d3dpp.BackBufferFormat == D3DFMT_R5G6B5)
            d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        else
            d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
        stencil_format = d3dpp.AutoDepthStencilFormat;
        if (!bUseLargeBackBuffer)
            if (d3ddm.Width < static_cast<uint32_t>(width) || d3ddm.Height < static_cast<uint32_t>(height))
            {
                d3dpp.BackBufferWidth = d3ddm.Width;
                d3dpp.BackBufferHeight = d3ddm.Height;
                width = d3ddm.Width;
                height = d3ddm.Height;
            }
    }

    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    for (auto samples = msaa; msaa > D3DMULTISAMPLE_2_SAMPLES; samples--)
    {
        if (SUCCEEDED(d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.BackBufferFormat, false,
                                                      static_cast<D3DMULTISAMPLE_TYPE>(samples), nullptr)))
        {
            d3dpp.MultiSampleType = static_cast<D3DMULTISAMPLE_TYPE>(samples);
            break;
        }
    }

    if (bBackBufferCanLock)
        d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    else
        d3dpp.Flags = 0;
    // d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    // d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
    // if(windowed) d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//FLIP;
    // else d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;

    if (vSyncEnabled)
    {
        d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    }
    else
    {
        d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }

    // Choose desired video adapter
    auto adapters_num = d3d->GetAdapterCount();
    if (videoAdapterIndex > adapters_num - 1)
        videoAdapterIndex = 0U;

    spdlog::info("Querying available DirectX 9 adapters... detected {}:", adapters_num);
    for (UINT i = 0; i != adapters_num; ++i)
    {
        D3DCAPS9 caps;
        d3d->GetDeviceCaps(i, D3DDEVTYPE_HAL, &caps);
        if (caps.DeviceType == D3DDEVTYPE_HAL)
        {
            D3DADAPTER_IDENTIFIER9 id;
            d3d->GetAdapterIdentifier(i, 0, &id);
            spdlog::info("{}: {} ({}) ", i, id.Description, id.DeviceName);
        }
    }
    spdlog::info("Using adapter with index {} (configurable by setting adapter=<index> inside engine.ini)",
             videoAdapterIndex);

    // Create device
    if (CHECKD3DERR(d3d->CreateDevice(videoAdapterIndex, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, &d3d9)))
    {
        if (CHECKD3DERR(d3d->CreateDevice(videoAdapterIndex, D3DDEVTYPE_HAL, hwnd, D3DCREATE_MIXED_VERTEXPROCESSING,
                                          &d3dpp, &d3d9)))
        {
            if (CHECKD3DERR(d3d->CreateDevice(videoAdapterIndex, D3DDEVTYPE_HAL, hwnd,
                                              D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3d9)))
            {
                return false;
            }
        }
    }
#ifdef _WIN32 // Effects
    effects_.setDevice(d3d9);
#endif

    // Create render targets for POST PROCESS effects
    d3d9->GetRenderTarget(0, &pOriginalScreenSurface);
    d3d9->GetDepthStencilSurface(&pOriginalDepthSurface);

    fSmallWidth = 128;
    fSmallHeight = 128;

    if (bPostProcessEnabled)
    {
        d3d9->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                            &pPostProcessTexture, nullptr);
        d3d9->CreateTexture(static_cast<int>(fSmallWidth), static_cast<int>(fSmallHeight), 1, D3DUSAGE_RENDERTARGET,
                            D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pSmallPostProcessTexture, nullptr);
        d3d9->CreateTexture(static_cast<int>(fSmallWidth * 2.0f), static_cast<int>(fSmallHeight * 2.0f), 1,
                            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pSmallPostProcessTexture2,
                            nullptr);
    }

    if (!pPostProcessTexture || !pSmallPostProcessTexture || !pSmallPostProcessTexture2)
    {
        bPostProcessEnabled = false;
        bPostProcessError = true;
    }

    if (!bPostProcessError)
    {
        pPostProcessTexture->GetSurfaceLevel(0, &pPostProcessSurface);
        pSmallPostProcessTexture2->GetSurfaceLevel(0, &pSmallPostProcessSurface2);
        pSmallPostProcessTexture->GetSurfaceLevel(0, &pSmallPostProcessSurface);

        if (!pPostProcessSurface || !pSmallPostProcessSurface2 || !pSmallPostProcessSurface)
        {
            bPostProcessEnabled = false;
            bPostProcessError = true;
        }
    }

    if (!bPostProcessError)
    {
        ClearPostProcessSurface(pPostProcessSurface);
        ClearPostProcessSurface(pSmallPostProcessSurface);
        ClearPostProcessSurface(pSmallPostProcessSurface2);
    }

    ClearPostProcessSurface(pOriginalScreenSurface);

    OriginalViewPort.X = 0;
    OriginalViewPort.Y = 0;
    OriginalViewPort.Width = width;
    OriginalViewPort.Height = height;
    OriginalViewPort.MinZ = 0.0f;
    OriginalViewPort.MaxZ = 1.0f;

    // if(CHECKD3DERR(SetViewport(&vprt))==true)    return false;

    // set it as a render target
    // FIX
    // SetPostProcessTextureAsRenderTarget();
    // set it as a render target

    for (int32_t b = 0; b < MAX_BUFFERS; b++)
    {
        VertexBuffers[b].buff = nullptr;
        IndexBuffers[b].buff = nullptr;
    }

    int32_t num_stages;
    num_stages = 8;

    for (int32_t s = 0; s < num_stages; s++)
    {
        // texture operation
        SetTextureStageState(s, D3DTSS_COLORARG1, D3DTA_CURRENT);
        SetTextureStageState(s, D3DTSS_COLORARG2, D3DTA_TEXTURE);
        SetTextureStageState(s, D3DTSS_COLOROP, D3DTOP_DISABLE);

        // texture coord
        SetTextureStageState(s, D3DTSS_TEXCOORDINDEX, s);

        // texture filtering
        SetSamplerState(s, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        SetSamplerState(s, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        SetSamplerState(s, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    }

    // set base texture and diffuse+specular lighting
    SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADDSIGNED);

    // texture op for lightmaps
    // SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADDSIGNED);

    SetCamera(CVECTOR(0.0f, 0.0f, 0.0f), CVECTOR(0.0f, 0.0f, 0.0f), 1.0f);

    D3DLIGHT9 l{};
    l.Type = D3DLIGHT_POINT;
    l.Range = 100.0f;
    l.Attenuation0 = 1.0f;

    for (int i = 0; i < 8; i++)
    {
        d3d9->SetLight(i, &l);
        d3d9->LightEnable(i, false);
    }

    SetCommonStates();

    screen_size.x = width;
    screen_size.y = height;

    m_fHeightDeformator = (float)(height * 4.0f) / (float)(width * 3.0f);

    // UNGUARD
    return true;
}

//################################################################################
bool DX9RENDER::ReleaseDevice()
{
    if (aniVBuffer)
        aniVBuffer->Release();
    aniVBuffer = nullptr;
    numAniVerteces = 0;
    for (int32_t b = 0; b < MAX_BUFFERS; b++)
    {
        if (VertexBuffers[b].buff)
            VertexBuffers[b].buff->Release();
        VertexBuffers[b].buff = nullptr;
        if (IndexBuffers[b].buff)
            IndexBuffers[b].buff->Release();
        IndexBuffers[b].buff = nullptr;
    }

    bool res = true;
    for (int32_t t = 0; t < MAX_STEXTURES; t++)
        if (Textures[t].ref && Textures[t].loaded && Textures[t].d3dtex)
        {
            if (CHECKD3DERR(Textures[t].d3dtex->Release()) == false)
                res = false;
            Textures[t].ref = NULL;
            delete Textures[t].name;
        }

    if (d3d9 != nullptr && CHECKD3DERR(d3d9->Release()) == false)
        res = false;
    d3d9 = nullptr;
    if (d3d != nullptr && CHECKD3DERR(d3d->Release()) == false)
        res = false;
    d3d = nullptr;
    return res;
}

//################################################################################
bool DX9RENDER::DX9Clear(int32_t type)
{
    if (CHECKD3DERR(d3d9->Clear(0L, NULL, type, dwBackColor, 1.0f, 0L)) == true)
        return false;
    // if(CHECKD3DERR(d3d9->Clear(0L, NULL, type, 0x0, 1.0f, 0L))==true)    return false;
    return true;
}

//################################################################################
bool DX9RENDER::DX9BeginScene()
{
    //~!~
    return false;
}

void DX9RENDER::CreateRenderQuad(float fWidth, float fHeight, float fSrcWidth, float fSrcHeight, float fMulU,
                                 float fMulV)
{
    const float StartX = -0.5f;
    const float StartY = -0.5f;
    fWidth -= 0.5f;
    fHeight -= 0.5f;
    PostProcessQuad[0].vPos = Vector4(StartX, fHeight, 0.0f, 1.0f);
    PostProcessQuad[1].vPos = Vector4(StartX, StartY, 0.0f, 1.0f);
    PostProcessQuad[2].vPos = Vector4(fWidth, fHeight, 0.0f, 1.0f);
    PostProcessQuad[3].vPos = Vector4(fWidth, StartY, 0.0f, 1.0f);

    const float fTexelU = 1.0f / fSrcWidth;
    const float fTexelV = 1.0f / fSrcHeight;

    const float fNearV = fTexelV * 0.5f;
    const float fFarV = 1.0f - (fTexelV * 0.5f);

    const float fNearU = fTexelU * 0.5f;
    const float fFarU = 1.0f - (fTexelU * 0.5f);

    PostProcessQuad[0].v0 = fFarV;
    PostProcessQuad[0].u0 = fNearU;
    PostProcessQuad[1].v0 = fNearV;
    PostProcessQuad[1].u0 = fNearU;
    PostProcessQuad[2].v0 = fFarV;
    PostProcessQuad[2].u0 = fFarU;
    PostProcessQuad[3].v0 = fNearV;
    PostProcessQuad[3].u0 = fFarU;

    for (int i = 0; i < 4; i++)
    {
        float u = PostProcessQuad[i].u0;
        float v = PostProcessQuad[i].v0;

        u = u * fMulU;
        v = v * fMulV;

        PostProcessQuad[i].u0 = u - (fTexelU * fBlurSize);
        PostProcessQuad[i].v0 = v - (fTexelV * fBlurSize);

        PostProcessQuad[i].u1 = u + (fTexelU * fBlurSize);
        PostProcessQuad[i].v1 = v - (fTexelV * fBlurSize);

        PostProcessQuad[i].u2 = u - (fTexelU * fBlurSize);
        PostProcessQuad[i].v2 = v + (fTexelV * fBlurSize);

        PostProcessQuad[i].u3 = u + (fTexelU * fBlurSize);
        PostProcessQuad[i].v3 = v + (fTexelV * fBlurSize);
    }
}

void DX9RENDER::BlurGlowTexture()
{
    // Render everything to a small texture
    CreateRenderQuad(fSmallWidth * 2.0f, fSmallHeight * 2.0f, 1024.0f, 1024.0f);
    SetTexture(0, pPostProcessTexture);
    SetTexture(1, pPostProcessTexture);
    SetTexture(2, pPostProcessTexture);
    SetTexture(3, pPostProcessTexture);
    SetRenderTarget(pSmallPostProcessSurface2, nullptr);
    DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POST_PROCESS_FVF, 2, PostProcessQuad, sizeof(QuadVertex), "PostProcessBlur");

    // pre-blur iBlurPasses times
    for (int i = 0; i < iBlurPasses; i++)
    {
        CreateRenderQuad(fSmallWidth, fSmallHeight, fSmallWidth * 2.0f, fSmallHeight * 2.0f);

        SetTexture(0, pSmallPostProcessTexture2);
        SetTexture(1, pSmallPostProcessTexture2);
        SetTexture(2, pSmallPostProcessTexture2);
        SetTexture(3, pSmallPostProcessTexture2);
        SetRenderTarget(pSmallPostProcessSurface, nullptr);
        DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POST_PROCESS_FVF, 2, PostProcessQuad, sizeof(QuadVertex),
                        "PostProcessBlur");

        CreateRenderQuad(fSmallWidth * 2.0f, fSmallHeight * 2.0f, fSmallWidth, fSmallHeight);

        SetTexture(0, pSmallPostProcessTexture);
        SetTexture(1, pSmallPostProcessTexture);
        SetTexture(2, pSmallPostProcessTexture);
        SetTexture(3, pSmallPostProcessTexture);
        SetRenderTarget(pSmallPostProcessSurface2, nullptr);
        DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POST_PROCESS_FVF, 2, PostProcessQuad, sizeof(QuadVertex),
                        "PostProcessBlur");
    }

    CreateRenderQuad(fSmallWidth, fSmallHeight, fSmallWidth * 2.0f, fSmallHeight * 2.0f);
    SetTexture(0, pSmallPostProcessTexture2);
    SetTexture(1, pSmallPostProcessTexture2);
    SetTexture(2, pSmallPostProcessTexture2);
    SetTexture(3, pSmallPostProcessTexture2);
    SetRenderTarget(pSmallPostProcessSurface, nullptr);
    DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POST_PROCESS_FVF, 2, PostProcessQuad, sizeof(QuadVertex), "PostProcessBlur");
}

void DX9RENDER::CopyGlowToScreen()
{
    const FLOAT sx = static_cast<FLOAT>(screen_size.x);
    const auto sy = static_cast<FLOAT>(screen_size.y);
    // Render to screen
    PostProcessQuad[0].vPos = Vector4(0, sy, 0.0f, 1.0f);
    PostProcessQuad[1].vPos = Vector4(0, 0, 0.0f, 1.0f);
    PostProcessQuad[2].vPos = Vector4(sx, sy, 0.0f, 1.0f);
    PostProcessQuad[3].vPos = Vector4(sx, 0, 0.0f, 1.0f);

    PostProcessQuad[0].v0 = 1.0f;
    PostProcessQuad[0].u0 = 0.0f;
    PostProcessQuad[1].v0 = 0.0f;
    PostProcessQuad[1].u0 = 0.0f;
    PostProcessQuad[2].v0 = 1.0f;
    PostProcessQuad[2].u0 = 1.0f;
    PostProcessQuad[3].v0 = 0.0f;
    PostProcessQuad[3].u0 = 1.0f;

    SetRenderTarget(pOriginalScreenSurface, pOriginalDepthSurface);

    if (GlowIntensity < 0)
        GlowIntensity = 0;
    if (GlowIntensity > 255)
        GlowIntensity = 255;
    const uint8_t bGLOW = static_cast<uint8_t>(GlowIntensity);
    const uint32_t dwTFactor = (bGLOW << 24) | (bGLOW << 16) | (bGLOW << 8) | bGLOW;

    // Draw the GLOW screen
    SetRenderState(D3DRS_TEXTUREFACTOR, dwTFactor);

    SetTexture(0, pSmallPostProcessTexture);
    SetTexture(1, pSmallPostProcessTexture);
    SetTexture(2, pSmallPostProcessTexture);
    SetTexture(3, pSmallPostProcessTexture);

    DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POST_PROCESS_FVF, 2, PostProcessQuad, sizeof(QuadVertex), "PostProcessGlow");
}

void DX9RENDER::CopyPostProcessToScreen()
{
    const FLOAT sx = static_cast<FLOAT>(screen_size.x);
    const auto sy = static_cast<FLOAT>(screen_size.y);
    PostProcessQuad[0].vPos = Vector4(0, sy, 0.0f, 1.0f);
    PostProcessQuad[1].vPos = Vector4(0, 0, 0.0f, 1.0f);
    PostProcessQuad[2].vPos = Vector4(sx, sy, 0.0f, 1.0f);
    PostProcessQuad[3].vPos = Vector4(sx, 0, 0.0f, 1.0f);

    PostProcessQuad[0].v0 = 1.0f;
    PostProcessQuad[0].u0 = 0.0f;
    PostProcessQuad[1].v0 = 0.0f;
    PostProcessQuad[1].u0 = 0.0f;
    PostProcessQuad[2].v0 = 1.0f;
    PostProcessQuad[2].u0 = 1.0f;
    PostProcessQuad[3].v0 = 0.0f;
    PostProcessQuad[3].u0 = 1.0f;

    SetRenderTarget(pOriginalScreenSurface, pOriginalDepthSurface);

    // Draw the original screen
    SetTexture(0, pPostProcessTexture);
    SetTexture(1, pPostProcessTexture);
    SetTexture(2, pPostProcessTexture);
    SetTexture(3, pPostProcessTexture);

    if (bSeaEffect)
    {
        SetFVF(POST_PROCESS_FVF);
        DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 32 * 32, 31 * 31 * 2, qi, D3DFMT_INDEX16, qv, sizeof(QuadVertex),
                               "PostProcess");
    }
    else
    {
        DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POST_PROCESS_FVF, 2, PostProcessQuad, sizeof(QuadVertex), "PostProcess");
    }
}

void DX9RENDER::ClearPostProcessSurface(IDirect3DSurface9 *pSurf)
{
    HRESULT hr = SetRenderTarget(pSurf, nullptr);
    hr = BeginScene();
    hr = d3d9->Clear(0, nullptr, D3DCLEAR_TARGET, 0x0, 0.0f, 0x0);
    hr = EndScene();
}

void DX9RENDER::SetScreenAsRenderTarget()
{
    SetRenderTarget(pOriginalScreenSurface, pOriginalDepthSurface);
    SetViewport(&OriginalViewPort);
}

void DX9RENDER::SetPostProcessTextureAsRenderTarget()
{
    SetRenderTarget(pPostProcessSurface, pOriginalDepthSurface);
    SetViewport(&OriginalViewPort);
}

void DX9RENDER::MakePostProcess()
{
    if (bPostProcessError)
        return;
    if (!bSeaEffect && !bPostProcessEnabled)
        return;
    if (!bNeedCopyToScreen)
        return;

    IDirect3DStateBlock9 *pStateBlock = nullptr;
    d3d9->CreateStateBlock(D3DSBT_ALL, &pStateBlock);
    pStateBlock->Capture();

    bNeedCopyToScreen = false;

    CopyPostProcessToScreen();
    BlurGlowTexture();
    CopyGlowToScreen();

    pStateBlock->Apply();
    pStateBlock->Release();

    SetScreenAsRenderTarget();
    /*
    Print (0, 0, "Blur = %3.2f",    fBlurSize);
    Print (0, 16, "Intensity = %d",    GlowIntensity);
    Print (0, 32, "BlurPasses = %d",    iBlurPasses);
    */
}

//################################################################################
bool DX9RENDER::DX9EndScene()
{
    if (bShowFps)
    {
        Print(screen_size.x - 100, screen_size.y - 50, "FPS %d", core.EngineFps());
    }

    if (bShowExInfo)
    {
        uint32_t dwTotalTexSize = 0;
        uint32_t dwTotalTexNum = 0, dwTotalVB = 0, dwTotalIB = 0, dwTotalVBSize = 0, dwTotalIBSize = 0;

        int32_t t;
        for (t = 0; t < MAX_STEXTURES; t++)
            if (Textures[t].ref > 0)
            {
                dwTotalTexSize += Textures[t].dwSize;
                dwTotalTexNum++;
            }

        for (t = 0; t < MAX_BUFFERS; t++)
        {
            if (VertexBuffers[t].buff)
            {
                dwTotalVBSize += VertexBuffers[t].size;
                dwTotalVB++;
            }
            if (IndexBuffers[t].buff)
            {
                dwTotalIBSize += IndexBuffers[t].size;
                dwTotalIB++;
            }
        }

        CMatrix mView;
        GetTransform(D3DTS_VIEW, mView);
        mView.Transposition();
        Print(80, 50, "Cam: %.3f, %.3f, %.3f", mView.Pos().x, mView.Pos().y, mView.Pos().z);
        Print(80, 70, "t : %d, %.3f Mb", dwTotalTexNum, float(dwTotalTexSize) / (1024.0f * 1024.0f));
        Print(80, 90, "v : %d, %.3f Mb", dwTotalVB, float(dwTotalVBSize) / (1024.0f * 1024.0f));
        Print(80, 110, "i : %d, %.3f Mb", dwTotalIB, float(dwTotalIBSize) / (1024.0f * 1024.0f));
        Print(80, 130, "d : %d, lv: %d, li: %d", dwNumDrawPrimitive, dwNumLV, dwNumLI);
        Print(80, 150, "s : %d, %.3f, %.3f", dwSoundBuffersCount, dwSoundBytes / 1024.f, dwSoundBytesCached / 1024.f);
    }

    // Try to drop video conveyor
    if (bDropVideoConveyor && pDropConveyorVBuffer)
    {
        CVECTOR *pV;
        pDropConveyorVBuffer->Lock(0, 0, (VOID **)&pV, 0);
        for (int32_t i = 0; i < 2; i++)
            pV[i] = CVECTOR(1e6f, 1e6f, 1e6f);
        pDropConveyorVBuffer->Unlock();
        d3d9->SetStreamSource(0, pDropConveyorVBuffer, 0, sizeof(CVECTOR));
        SetFVF(D3DFVF_XYZ);
        DrawPrimitive(D3DPT_LINELIST, 0, 1);
    }

    if (CHECKD3DERR(EndScene()))
        return false;

    if (bMakeShoot)
        MakeScreenShot();

    const HRESULT hRes = d3d9->Present(nullptr, nullptr, nullptr, nullptr);

    if (hRes == D3DERR_DEVICELOST)
    {
        LostRender();
    }

#ifdef _WIN32 // bSafeRendering
    if (bSafeRendering)
    {
        const HDC dc = GetDC(hwnd);
        SetPixel(dc, 0, 0, 0);
        ReleaseDC(hwnd, dc);
    }
#endif

    return true;
}

//################################################################################
static int totSize = 0;

int32_t DX9RENDER::TextureCreate(const char *fname)
{
    // start add texture path
    if ((uintptr_t)fname == -1)
    {
        iSetupPath = 1;
        return -1;
    }

    // continue add texture path process
    if (iSetupPath)
    {
        if (iSetupPath == 1)
        {
            dwSetupNumber = (uintptr_t)(fname);
            iSetupPath = 2;
            return -1;
        }

        TexPaths[dwSetupNumber].str[0] = 0;
        if (fname && fname[0])
            strcpy_s(TexPaths[dwSetupNumber].str, fname);
        iSetupPath = 0;
        return -1;
    }

    if (fname == nullptr)
    {
        core.Trace("Can't create texture with null name");
        return -1L;
    }

    if (!bLoadTextureEnabled)
        return -1;

    for (int32_t i = 3; i >= -1; i--)
    {
        char _fname[256];

        if (i >= 0 && TexPaths[i].str[0] == 0)
            continue;
        if (i >= 0)
        {
            const uint32_t dwLen = strlen(fname);

            int32_t j;
            for (j = dwLen - 1; j >= 0; j--)
                if (fname[j] == '\\')
                    break;

            _fname[0] = 0;
            strncpy_s(_fname, fname, j + 1);
            _fname[j + 1] = 0;
            strcat_s(_fname, TexPaths[i].str);
            strcat_s(_fname, &fname[j + 1]);
            bTrace = false;
        }
        else
        {
            bTrace = true;
            strcpy_s(_fname, fname);
        }

        std::ranges::for_each(_fname, [](char &c) { c = std::toupper(c); });

        const uint32_t hf = MakeHashValue(_fname);

        int32_t t;
        for (t = 0; t < MAX_STEXTURES; t++)
            if (Textures[t].ref != 0)
                if (Textures[t].name)
                    if (Textures[t].hash == hf && storm::iEquals(Textures[t].name, _fname))
                    {
                        Textures[t].ref++;
                        return t;
                    }

        for (t = 0; t < MAX_STEXTURES; t++)
            if (Textures[t].ref == 0)
                break;

        Textures[t].hash = hf;

        const auto len = strlen(_fname) + 1;
        if ((Textures[t].name = new char[len]) == nullptr)
            throw std::runtime_error("allocate memory error");
        strcpy_s(Textures[t].name, len, _fname);
        Textures[t].isCubeMap = false;
        Textures[t].loaded = false;
        Textures[t].ref++;
        if (TextureLoad(t))
            return t;
        Textures[t].ref--;
        STORM_DELETE(Textures[t].name);
    }
    return -1;
}

int32_t DX9RENDER::TextureCreate(UINT width, UINT height, UINT levels, uint32_t usage, D3DFORMAT format, D3DPOOL pool)
{
    IDirect3DTexture9 *texture = nullptr;

    const auto result = CreateTexture(width, height, levels, usage, format, pool, &texture);
    if (CHECKD3DERR(result))
    {
        return -1;
    }

    int32_t t;
    for (t = 0; t < MAX_STEXTURES; t++)
    {
        if (Textures[t].ref == 0)
        {
            break;
        }
    }

    Textures[t].d3dtex = texture;
    Textures[t].name = nullptr;
    Textures[t].hash = 0;
    Textures[t].ref = 1;
    Textures[t].dwSize = width * height * 4; // Assuming 32-bit pixels
    Textures[t].isCubeMap = false;
    Textures[t].loaded = true;

    return t;
}

bool DX9RENDER::TextureIncReference(int32_t texid)
{
    ++Textures[texid].ref;
    return true;
}

bool DX9RENDER::TextureLoad(int32_t t)
{
    using namespace std::literals;

    ProgressView();
    // Form the path to the texture
    char fn[_MAX_FNAME];
    Textures[t].dwSize = 0;
    if (Textures[t].name == nullptr)
    {
        return false;
    }

    auto lTexture = std::string(Textures[t].name);
    std::transform(lTexture.begin(), lTexture.end(), lTexture.begin(), [](unsigned char c) { return std::tolower(c); });
    auto has_resource_prefix = starts_with(lTexture, "resource\\textures\\");
    auto has_tx_postfix = ends_with(lTexture, ".tx");

    sprintf_s(fn, "%s%s%s", has_resource_prefix ? "" : "resource\\textures\\", Textures[t].name, has_tx_postfix ? "" : ".tx");

    for (int32_t s = 0, d = 0; fn[d]; s++)
    {
        if (d > 0 && (fn[d - 1] == PATH_SEP || fn[d - 1] == WRONG_PATH_SEP) &&
            (fn[s] == PATH_SEP || fn[s] == WRONG_PATH_SEP))
        {
            continue;
        }
        fn[d++] = fn[s];
    }
    // Opening the file
    auto fileS = fio->_CreateFile(fn, std::ios::binary | std::ios::in);
    if (!fileS.is_open())
    {
        // try to load without '.tx' (e.g. raw Targa)
        std::filesystem::path path_to_tex{fn};
        path_to_tex.replace_extension();
        if (exists(path_to_tex))
        {
            return TextureLoadUsingD3DX(path_to_tex.string().c_str(), t);
        }
        if (bTrace)
        {
            core.Trace("Can't load texture %s", fn);
        }
        delete Textures[t].name;
        Textures[t].name = nullptr;
        return false;
    }
    // Reading the header
    TX_FILE_HEADER head;
    if (!fio->_ReadFile(fileS, &head, sizeof(head)))
    {
        if (bTrace)
        {
            core.Trace("Can't load texture %s", fn);
        }
        delete Textures[t].name;
        Textures[t].name = nullptr;
        fio->_CloseFile(fileS);
        return false;
    }
    // Analyzing the format
    D3DFORMAT d3dFormat = D3DFMT_UNKNOWN;
    int32_t textureFI;
    for (textureFI = 0; textureFI < sizeof(textureFormats) / sizeof(SD_TEXTURE_FORMAT); textureFI++)
    {
        if (textureFormats[textureFI].txFormat == head.format)
        {
            break;
        }
    }
    if (textureFI == sizeof(textureFormats) / sizeof(SD_TEXTURE_FORMAT) || head.flags & TX_FLAGS_PALLETTE)
    {
        if (bTrace)
        {
            core.Trace("Invalidate texture format %s, not loading it.", fn);
        }
        delete Textures[t].name;
        Textures[t].name = nullptr;
        fio->_CloseFile(fileS);
        return false;
    }
    d3dFormat = textureFormats[textureFI].d3dFormat;
    bool isSwizzled = textureFormats[textureFI].isSwizzled;
    const char *formatTxt = textureFormats[textureFI].format;
    // Skipping mips
    uint32_t seekposition = 0;
    for (int32_t nTD = nTextureDegradation; nTD > 0; nTD--)
    {
        if (head.nmips <= 1 || head.width <= 32 || head.height <= 32)
        {
            break; // degradation limit
        }
        seekposition += head.mip_size;
        head.nmips--;
        head.width /= 2;
        head.height /= 2;
        head.mip_size /= 4;
    }
    // Loading the texture
    if (!(head.flags & TX_FLAGS_CUBEMAP))
    {
        // Loading a regular texture
        // Position in file
        if (seekposition)
        {
            fio->_SetFilePointer(fileS, seekposition, std::ios::cur);
        }
        // create the texture
        IDirect3DTexture9 *tex = nullptr;
        if (CHECKD3DERR(d3d9->CreateTexture(head.width, head.height, head.nmips, 0, d3dFormat, D3DPOOL_MANAGED, &tex,
                                            NULL)) == true ||
            !tex)
        {
            if (bTrace)
            {
                core.Trace(
                    "Texture %s is not created (width: %i, height: %i, num mips: %i, format: %s), not loading it.", fn,
                    head.width, head.height, head.nmips, formatTxt);
            }
            delete Textures[t].name;
            Textures[t].name = nullptr;
            fio->_CloseFile(fileS);
            return false;
        }
        // Filling the levels
        for (int32_t m = 0; m < head.nmips; m++)
        {
            // take into account the size of the mip
            Textures[t].dwSize += head.mip_size;
            // Getting the mip surface
            bool isError = false;
            IDirect3DSurface9 *surface = nullptr;
            if (CHECKD3DERR(tex->GetSurfaceLevel(m, &surface)) == true || !surface)
            {
                isError = true;
            }
            else
            {
                // read the mip
                isError = !LoadTextureSurface(fileS, surface, head.mip_size, head.width, head.height, isSwizzled);
            }
            // Freeing the surface
            if (surface)
            {
                surface->Release();
            }
            // If there was an error, then interrupt the download
            if (isError)
            {
                if (bTrace)
                {
                    core.Trace("Can't loading mip %i, texture %s is not created (width: %i, height: %i, num mips: %i, "
                               "format: %s), not loading it.",
                               m, fn, head.width, head.height, head.nmips, formatTxt);
                }
                delete Textures[t].name;
                Textures[t].name = nullptr;
                fio->_CloseFile(fileS);
                tex->Release();
                return false;
            }
            // recalculate the dimensions for the next mip
            head.width /= 2;
            head.height /= 2;
            head.mip_size /= 4;
        }
        Textures[t].d3dtex = tex;
        Textures[t].isCubeMap = false;
    }
    else
    {
        // Download cubemap
        if (head.width != head.height)
        {
            if (bTrace)
            {
                core.Trace("Cube map texture can't has not squared sides %s, not loading it.", fn);
            }
            delete Textures[t].name;
            Textures[t].name = nullptr;
            fio->_CloseFile(fileS);
            return false;
        }
        // Number of mips
        D3DCAPS9 devcaps;
        if (CHECKD3DERR(d3d9->GetDeviceCaps(&devcaps)))
        {
            if (bTrace)
            {
                core.Trace("Cube map texture %s is not created (size: %i, num mips: %i, format: %s), not loading it.",
                           fn, head.width, head.nmips, formatTxt);
            }
            delete Textures[t].name;
            Textures[t].name = nullptr;
            fio->_CloseFile(fileS);
            return false;
        }
        if (!(devcaps.TextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP))
        {
            head.nmips = 1;
        }
        // create the texture
        IDirect3DCubeTexture9 *tex = nullptr;
        if (CHECKD3DERR(d3d9->CreateCubeTexture(head.width, head.nmips, 0, d3dFormat, D3DPOOL_MANAGED, &tex, NULL)) ==
                true ||
            !tex)
        {
            if (bTrace)
            {
                core.Trace("Cube map texture %s is not created (size: %i, num mips: %i, format: %s), not loading it.",
                           fn, head.width, head.nmips, formatTxt);
            }
            delete Textures[t].name;
            Textures[t].name = nullptr;
            fio->_CloseFile(fileS);
            return false;
        }
        // Loading the sides
        bool isError = false;
        if (seekposition)
        {
            fio->_SetFilePointer(fileS, seekposition, std::ios::cur);
        }
        uint32_t sz =
            LoadCubmapSide(fileS, tex, D3DCUBEMAP_FACE_POSITIVE_Z, head.nmips, head.mip_size, head.width, isSwizzled);
        if (sz)
        {
            Textures[t].dwSize += sz;
            if (seekposition)
            {
                fio->_SetFilePointer(fileS, seekposition, std::ios::cur);
            }
            sz = LoadCubmapSide(fileS, tex, D3DCUBEMAP_FACE_POSITIVE_X, head.nmips, head.mip_size, head.width,
                                isSwizzled);
            if (sz)
            {
                Textures[t].dwSize += sz;
                if (seekposition)
                {
                    fio->_SetFilePointer(fileS, seekposition, std::ios::cur);
                }
                sz = LoadCubmapSide(fileS, tex, D3DCUBEMAP_FACE_NEGATIVE_Z, head.nmips, head.mip_size, head.width,
                                    isSwizzled);
                if (sz)
                {
                    Textures[t].dwSize += sz;
                    if (seekposition)
                    {
                        fio->_SetFilePointer(fileS, seekposition, std::ios::cur);
                    }
                    sz = LoadCubmapSide(fileS, tex, D3DCUBEMAP_FACE_NEGATIVE_X, head.nmips, head.mip_size, head.width,
                                        isSwizzled);
                    if (sz)
                    {
                        Textures[t].dwSize += sz;
                        if (seekposition)
                        {
                            fio->_SetFilePointer(fileS, seekposition, std::ios::cur);
                        }
                        sz = LoadCubmapSide(fileS, tex, D3DCUBEMAP_FACE_POSITIVE_Y, head.nmips, head.mip_size,
                                            head.width, isSwizzled);
                        if (sz)
                        {
                            Textures[t].dwSize += sz;
                            if (seekposition)
                            {
                                fio->_SetFilePointer(fileS, seekposition, std::ios::cur);
                            }
                            sz = LoadCubmapSide(fileS, tex, D3DCUBEMAP_FACE_NEGATIVE_Y, head.nmips, head.mip_size,
                                                head.width, isSwizzled);
                            if (!sz)
                            {
                                isError = true;
                            }
                            Textures[t].dwSize += sz;
                        }
                        else
                        {
                            isError = true;
                        }
                    }
                    else
                    {
                        isError = true;
                    }
                }
                else
                {
                    isError = true;
                }
            }
            else
            {
                isError = true;
            }
        }
        else
        {
            isError = true;
        }

        if (isError)
        {
            if (bTrace)
            {
                core.Trace("Cube map texture %s can't loading (size: %i, num mips: %i, format: %s), not loading it.",
                           fn, head.width, head.nmips, formatTxt);
            }
            delete Textures[t].name;
            Textures[t].name = nullptr;
            fio->_CloseFile(fileS);
            tex->Release();
            return false;
        }
        Textures[t].d3dtex = tex;
        Textures[t].isCubeMap = true;
    }

    //---------------------------------------------------------------
    // print statistics
    //---------------------------------------------------------------
    if (texLog)
    {
        char s[256];
        if (totSize == 0)
        {
            fio->_DeleteFile("texLoad.txt");
        }
        auto fileS2 = fio->_CreateFile("texLoad.txt", std::ios::binary | std::ios::out | std::ios::app);
        totSize += Textures[t].dwSize;
        sprintf_s(s, "%.2f, size: %d, %d * %d, %s\n", totSize / 1024.0f / 1024.0f, Textures[t].dwSize, head.width,
                  head.height, Textures[t].name);
        fio->_WriteFile(fileS2, s, strlen(s));
        fio->_FlushFileBuffers(fileS2);
        fio->_CloseFile(fileS2);
    }
    dwTotalSize += Textures[t].dwSize;
    //---------------------------------------------------------------
    Textures[t].loaded = true;
    // Close the file
    fio->_CloseFile(fileS);
    return true;
}

bool DX9RENDER::TextureLoadUsingD3DX(const char* path, int32_t t)
{
#ifdef _WIN32 // TextureLoadUsingD3DX - used only for loading raw Targa
    // TODO: reimplement the whole thing in a tidy way
    IDirect3DTexture9 *pTex;
    if(CHECKD3DERR(D3DXCreateTextureFromFileA(d3d9, path, &pTex)))
    {
        delete Textures[t].name;
        Textures[t].name = nullptr;
        return false;
    }

    D3DSURFACE_DESC desc;
    pTex->GetLevelDesc(0, &desc);

    Textures[t].hash = 0;
    Textures[t].ref = 1;
    Textures[t].d3dtex = pTex;
    Textures[t].isCubeMap = false;
    Textures[t].dwSize = desc.Height * desc.Width * 4;
    Textures[t].loaded = true;

    return true;
#else
    return false;
#endif
}

IDirect3DBaseTexture9 *DX9RENDER::GetBaseTexture(int32_t iTexture)
{
    return (iTexture >= 0) ? Textures[iTexture].d3dtex : nullptr;
}

uint32_t DX9RENDER::LoadCubmapSide(std::fstream &fileS, IDirect3DCubeTexture9 *tex, D3DCUBEMAP_FACES face,
                                   uint32_t numMips, uint32_t mipSize, uint32_t size, bool isSwizzled)
{
    uint32_t texsize = 0;
    // Filling the levels
    for (uint32_t m = 0; m < numMips; m++)
    {
        // take into account the size of the mip
        texsize += mipSize;
        // Getting the mip surface
        bool isError = false;
        IDirect3DSurface9 *surface = nullptr;
        if (CHECKD3DERR(tex->GetCubeMapSurface(face, m, &surface)) == true || !surface)
        {
            isError = true;
        }
        else
        {
            // read the mip
            isError = !LoadTextureSurface(fileS, surface, mipSize, size, size, isSwizzled);
        }
        // Freeing the surface
        if (surface)
        {
            surface->Release();
        }
        // If there was an error, then interrupt the download
        if (isError)
        {
            if (bTrace)
            {
                core.Trace("Can't loading cubemap mip %i (side: %i), not loading it.", m, face);
            }
            return 0;
        }
        // recalculate the dimensions for the next mip
        size /= 2;
        mipSize /= 4;
    }
    return texsize;
}

bool DX9RENDER::LoadTextureSurface(std::fstream &fileS, IDirect3DSurface9 *suface, uint32_t mipSize, uint32_t width,
                                   uint32_t height, bool isSwizzled)
{
    //------------------------------------------------------------------------------------------
    // PC version
    // ------------------------------------------------------------------------------------------
    // Surface pointer
    D3DLOCKED_RECT lock;
    if (CHECKD3DERR(suface->LockRect(&lock, NULL, 0L)) == true)
    {
        return false;
    }
    // Reading out
    if (!fio->_ReadFile(fileS, lock.pBits, mipSize))
    {
        if (CHECKD3DERR(suface->UnlockRect()) == true)
        {
            return false;
        }
        return false;
    }
    // Surface release
    if (CHECKD3DERR(suface->UnlockRect()) == true)
    {
        return false;
    }
    return true;
    //------------------------------------------------------------------------------------------
}

//################################################################################
bool DX9RENDER::TextureSet(int32_t stage, int32_t texid)
{
    if (texid == -1)
    {
        if (CHECKD3DERR(d3d9->SetTexture(stage, NULL)) == true)
        {
            return false;
        }
        return true;
    }

    /*
    if(Textures[texid].loaded==false)
    {
    int tex2load[MAX_STEXTURES];
    int t2l=0;
    for(int32_t t=0; t<MAX_STEXTURES; t++)
    if(Textures[t].ref>0 && Textures[t].loaded==false)    tex2load[t2l++] = t;
    */
    /*/sort textures
    for(t=0; t<t2l; t++)
    for(int32_t tt=t; tt<t2l; tt++)
    if(_strcmpi(Textures[tex2load[tt]].name, Textures[tex2load[t]].name)<0)
    {
    int ttemp = tex2load[t];
    tex2load[t] = tex2load[tt];
    tex2load[tt] = ttemp;
    }*/
    /*
    for(t=0; t<t2l; t++)
    {
    Textures[tex2load[t]].loaded = true;
    TextureLoad(tex2load[t]);
    }
    //Textures[texid].loaded = true;    TextureLoad(texid);
    }
    */

    if (CHECKD3DERR(d3d9->SetTexture(stage, Textures[texid].d3dtex)) == true)
    {
        return false;
    }
    return true;
}

//################################################################################
bool DX9RENDER::TextureRelease(int32_t texid)
{
    if (texid == -1)
    {
        return true;
    }
    Textures[texid].ref--;
    if (Textures[texid].ref != 0)
    {
        return false;
    }
    if (Textures[texid].name != nullptr)
    {
        if (texLog)
        {
            auto fileS = fio->_CreateFile("texLoad.txt", std::ios::binary | std::ios::in | std::ios::out);
            const int bytes = fio->_GetFileSize("texLoad.txt");
            auto buf = new char[bytes + 1];
            fio->_ReadFile(fileS, buf, bytes);
            buf[bytes] = 0;

            char *str = strstr(buf, Textures[texid].name);
            if (str != nullptr)
            {
                fio->_SetFilePointer(fileS, str - buf, std::ios::beg);
                auto s = "*";
                fio->_WriteFile(fileS, s, 1);
            }
            delete[] buf;
            fio->_FlushFileBuffers(fileS);
            fio->_CloseFile(fileS);

            /*FILE *flstat = fopen("texLoad.txt", "r+b");
            totSize -= Textures[texid].dwSize;
            fseek(flstat, 0, SEEK_END);
            int bytes = ftell(flstat);
            char *buf = new char[bytes+1];
            fseek(flstat, 0, SEEK_SET);
            fread(buf, bytes, sizeof *buf, flstat);
            buf[bytes] = 0;

            char *str = strstr(buf, Textures[texid].name);
            if(str!=0)
            {
            fseek(flstat, str-buf, SEEK_SET);
            const char *s = "*";
            fwrite(s, 1, 1, flstat);
            }
            delete buf;
            fclose(flstat);*/
        }

        delete Textures[texid].name;
        Textures[texid].name = nullptr;
    }
    if (Textures[texid].loaded == false)
    {
        return false;
    }

    if (Textures[texid].d3dtex)
    {
        if (CHECKD3DERR(Textures[texid].d3dtex->Release()) == true)
        {
            return false;
        }
    }
    Textures[texid].d3dtex = nullptr;
    dwTotalSize -= Textures[texid].dwSize;

    return true;
}

//################################################################################
bool DX9RENDER::SetCamera(const CVECTOR &pos, const CVECTOR &ang, float fov)
{
    if (!SetCamera(pos, ang))
        return false;
    if (!SetPerspective(fov, aspectRatio))
        return false;

    ProcessScriptPosAng(pos, ang);
    return true;
}

bool DX9RENDER::SetCamera(const CVECTOR &pos, const CVECTOR &ang)
{
    CMatrix mtx;
    const CVECTOR vOldWordRelationPos = vWordRelationPos;
    /*if (!pos || !ang)
    {
      if (!pos && !ang) return true;
      //if(CHECKD3DERR(GetTransform(D3DTS_VIEW, mtx))==true) return false;
      d3d9->GetTransform(D3DTS_VIEW, mtx);
      if (pos)
      {
        //mtx.SetInversePosition(pos->x, pos->y, pos->z);
        Pos = *pos;
        vWordRelationPos = -Pos;
      }
      else
      {
        //CVECTOR p;
        //mtx.MulToInvNorm(mtx.Pos(), p);
        mtx.BuildMatrix(*ang);
        mtx.Transposition3X3();
        //mtx.SetInversePosition(p.x, p.y, p.z);
        Ang = *ang;
      }
    }
    else*/
    {
        mtx.BuildMatrix(ang);
        mtx.Transposition3X3();
        // mtx.SetInversePosition(pos->x, pos->y, pos->z);
        Pos = pos;
        Ang = ang;
        vWordRelationPos = -Pos;
    }
    // if(CHECKD3DERR(SetTransform(D3DTS_VIEW, mtx))==true)    return false;

    d3d9->SetTransform(D3DTS_VIEW, mtx);
    vViewRelationPos = -(mtx * vWordRelationPos);
    d3d9->GetTransform(D3DTS_WORLD, mtx);
    mtx.Pos() += vWordRelationPos - vOldWordRelationPos;
    d3d9->SetTransform(D3DTS_WORLD, mtx);

    FindPlanes(d3d9);
    ProcessScriptPosAng(pos, ang);
    return true;
}

bool DX9RENDER::SetCamera(CVECTOR lookFrom, CVECTOR lookTo, CVECTOR up)
{
    CMatrix mtx;
    if (!mtx.BuildViewMatrix(lookFrom, lookTo, up))
        return false;
    // if(CHECKD3DERR(SetTransform(D3DTS_VIEW, mtx))==true)    return false;
    SetTransform(D3DTS_VIEW, mtx);
    Pos = lookFrom;

    // calculate the angle of the camera
    // Ang = 0.0f;
    const CVECTOR vNorm = !(lookTo - lookFrom);
    Ang.y = atan2f(vNorm.x, vNorm.z);
    Ang.x = atan2f(-vNorm.y, sqrtf(vNorm.x * vNorm.x + vNorm.z * vNorm.z));
    Ang.z = 0.f;

    FindPlanes(d3d9);
    ProcessScriptPosAng(lookFrom, Ang);
    return true;
}

void DX9RENDER::ProcessScriptPosAng(const CVECTOR &vPos, const CVECTOR &vAng)
{
    core.Event("CameraPosAng", "ffffff", vPos.x, vPos.y, vPos.z, vAng.x, vAng.y, vAng.z);
}

void DX9RENDER::GetNearFarPlane(float &fNear, float &fFar)
{
    fNear = fNearClipPlane;
    fFar = fFarClipPlane;
}

void DX9RENDER::SetNearFarPlane(float fNear, float fFar)
{
    fNearClipPlane = fNear;
    fFarClipPlane = fFar;

    SetPerspective(Fov, aspectRatio);
}

bool DX9RENDER::SetPerspective(float perspective, float fAspectRatio)
{
    perspective *= FovMultiplier;

    const float near_plane = fNearClipPlane; // Distance to near clipping
    const float far_plane = fFarClipPlane;   // Distance to far clipping
    const float fov_horiz = perspective;     // Horizontal field of view  angle, in radians
    if (fAspectRatio < 0)
    {
        fAspectRatio = static_cast<float>(screen_size.y) / screen_size.x;
    }
    aspectRatio = fAspectRatio;
    const float fov_vert = perspective * fAspectRatio; // Vertical field of view  angle, in radians

    const float w = 1.0f / tanf(fov_horiz * 0.5f);
    const float h = 1.0f / tanf(fov_vert * 0.5f);
    const float Q = far_plane / (far_plane - near_plane);

    D3DMATRIX mtx{};

    mtx._11 = w;
    mtx._22 = h;
    mtx._33 = Q;
    mtx._43 = -Q * near_plane;
    mtx._34 = 1.0f;

    if (CHECKD3DERR(d3d9->SetTransform(D3DTS_PROJECTION, &mtx)) == true)
        return false;
    Fov = perspective;
    FindPlanes(d3d9);
    return true;
}

//################################################################################
bool DX9RENDER::SetCurrentMatrix(D3DMATRIX *mtx)
{
    // return CHECKD3DERR(SetTransform(D3DTS_WORLD, mtx)) == D3D_OK;
    SetTransform(D3DTS_WORLD, mtx);
    return true;
}

bool DX9RENDER::SetMaterial(D3DMATERIAL9 &m)
{
    if (CHECKD3DERR(d3d9->SetMaterial(&m)))
        return false;
    return true;
}

bool DX9RENDER::SetLight(uint32_t dwIndex, const D3DLIGHT9 *pLight)
{
    // Set the property information for the first light.
    D3DLIGHT9 tmpLight = *pLight;
    tmpLight.Position.x += vWordRelationPos.x;
    tmpLight.Position.y += vWordRelationPos.y;
    tmpLight.Position.z += vWordRelationPos.z;
    if (CHECKD3DERR(d3d9->SetLight(dwIndex, &tmpLight)))
        return false;
    return true;
}

bool DX9RENDER::LightEnable(uint32_t dwIndex, bool bOn)
{
    if (CHECKD3DERR(d3d9->LightEnable(dwIndex, bOn)))
        return false;
    return true;
}

bool DX9RENDER::GetLightEnable(uint32_t dwIndex, BOOL *pEnable)
{
    if (CHECKD3DERR(d3d9->GetLightEnable(dwIndex, pEnable)))
        return false;
    return true;
}

bool DX9RENDER::GetLight(uint32_t dwIndex, D3DLIGHT9 *pLight)
{
    if (CHECKD3DERR(d3d9->GetLight(dwIndex, pLight)))
        return false;

    pLight->Position.x -= vWordRelationPos.x;
    pLight->Position.y -= vWordRelationPos.y;
    pLight->Position.z -= vWordRelationPos.z;
    return true;
}

//################################################################################

int32_t DX9RENDER::CreateVertexBuffer(int32_t type, size_t size, uint32_t dwUsage)
{
    if (size <= 0)
        return -1; // fix

    int32_t b;
    for (b = 0; b < MAX_BUFFERS; b++)
        if (VertexBuffers[b].buff == nullptr)
            break;

    if (b == MAX_BUFFERS)
        return -1;

    if (CHECKD3DERR(d3d9->CreateVertexBuffer(size, dwUsage, type, D3DPOOL_DEFAULT, &VertexBuffers[b].buff, NULL)))
        return -1;

    VertexBuffers[b].type = type;
    VertexBuffers[b].size = size;
    VertexBuffers[b].dwUsage = dwUsage;

    return b;
}

IDirect3DVertexBuffer9 *DX9RENDER::GetVertexBuffer(int32_t id)
{
    if (id < 0 || id >= MAX_BUFFERS)
        return nullptr;
    return VertexBuffers[id].buff;
}

int32_t DX9RENDER::GetVertexBufferFVF(int32_t id)
{
    if (id < 0 || id >= MAX_BUFFERS)
        return 0;
    return VertexBuffers[id].type;
}

//################################################################################
int32_t DX9RENDER::CreateIndexBuffer(size_t size, uint32_t dwUsage)
{
    int32_t b;
    for (b = 0; b < MAX_BUFFERS; b++)
        if (IndexBuffers[b].buff == nullptr)
            break;

    if (b == MAX_BUFFERS)
        return -1;

    // d3d9->CreateIndexBuffer(size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
    if (CHECKD3DERR(
            d3d9->CreateIndexBuffer(size, dwUsage, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &IndexBuffers[b].buff, NULL)))
        return -1;

    IndexBuffers[b].size = size;
    IndexBuffers[b].dwUsage = dwUsage;

    return b;
}

//################################################################################
void DX9RENDER::DrawBuffer(int32_t vbuff, int32_t stride, int32_t ibuff, int32_t minv, size_t numv, size_t startidx, size_t numtrg,
                           const char *cBlockName)
{
    bool bDraw = true;

    if (vbuff >= 0)
        if (CHECKD3DERR(SetFVF(VertexBuffers[vbuff].type)))
            return;
    // else VertexBuffer already set

    if (CHECKD3DERR(d3d9->SetIndices(IndexBuffers[ibuff].buff)))
        return;

    if (vbuff >= 0)
        if (CHECKD3DERR(d3d9->SetStreamSource(0, VertexBuffers[vbuff].buff, 0, stride)))
            return;
    // else VertexBuffer already set

    if (cBlockName && cBlockName[0])
        bDraw = TechniqueExecuteStart(cBlockName);

    if (bDraw)
        do
        {
            dwNumDrawPrimitive++;
            CHECKD3DERR(d3d9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, minv, 0, numv, startidx, numtrg));
        } while (cBlockName && cBlockName[0] && TechniqueExecuteNext());
}

void DX9RENDER::DrawIndexedPrimitiveNoVShader(D3DPRIMITIVETYPE dwPrimitiveType, int32_t iVBuff, int32_t iStride, int32_t iIBuff,
                                              int32_t iMinV, int32_t iNumV, int32_t iStartIdx, int32_t iNumTrg,
                                              const char *cBlockName)
{
    bool bDraw = true;

    if (CHECKD3DERR(d3d9->SetIndices(IndexBuffers[iIBuff].buff)) == true)
        return;

    if (iVBuff >= 0)
    {
        if (CHECKD3DERR(d3d9->SetStreamSource(0, VertexBuffers[iVBuff].buff, 0, iStride)) == true)
            return;
    }

    if (cBlockName && cBlockName[0])
        bDraw = TechniqueExecuteStart(cBlockName);
    if (bDraw)
        do
        {
            dwNumDrawPrimitive++;
            CHECKD3DERR(d3d9->DrawIndexedPrimitive(dwPrimitiveType, iMinV, 0, iNumV, iStartIdx, iNumTrg));
        } while (cBlockName && TechniqueExecuteNext());
}

void DX9RENDER::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE dwPrimitiveType, uint32_t dwMinIndex, uint32_t dwNumVertices,
                                       uint32_t dwPrimitiveCount, const void *pIndexData, D3DFORMAT IndexDataFormat,
                                       const void *pVertexData, uint32_t dwVertexStride, const char *cBlockName)
{
    bool bDraw = true;
    if (cBlockName && cBlockName[0])
        bDraw = TechniqueExecuteStart(cBlockName);
    if (bDraw)
        do
        {
            dwNumDrawPrimitive++;
            CHECKD3DERR(d3d9->DrawIndexedPrimitiveUP(dwPrimitiveType, dwMinIndex, dwNumVertices, dwPrimitiveCount,
                                                     pIndexData, IndexDataFormat, pVertexData, dwVertexStride));
        } while (cBlockName && TechniqueExecuteNext());
}

void DX9RENDER::DrawPrimitiveUP(D3DPRIMITIVETYPE dwPrimitiveType, uint32_t dwVertexBufferFormat, uint32_t dwNumPT,
                                const void *pVerts, uint32_t dwStride, const char *cBlockName)
{
    bool bDraw = true;

    if (CHECKD3DERR(SetFVF(dwVertexBufferFormat)) == true)
        return;

    if (cBlockName && cBlockName[0])
        bDraw = TechniqueExecuteStart(cBlockName);
    if (bDraw)
        do
        {
            dwNumDrawPrimitive++;
            CHECKD3DERR(d3d9->DrawPrimitiveUP(dwPrimitiveType, dwNumPT, pVerts, dwStride));
        } while (cBlockName && TechniqueExecuteNext());
}

void DX9RENDER::DrawPrimitive(D3DPRIMITIVETYPE dwPrimitiveType, int32_t iVBuff, int32_t iStride, int32_t iStartV, int32_t iNumPT,
                              const char *cBlockName)
{
    bool bDraw = true;

    if (CHECKD3DERR(SetFVF(VertexBuffers[iVBuff].type)) == true)
        return;

    if (CHECKD3DERR(d3d9->SetStreamSource(0, VertexBuffers[iVBuff].buff, 0, iStride)) == true)
        return;

    if (cBlockName && cBlockName[0])
        bDraw = TechniqueExecuteStart(cBlockName);
    if (bDraw)
        do
        {
            dwNumDrawPrimitive++;
            CHECKD3DERR(d3d9->DrawPrimitive(dwPrimitiveType, iStartV, iNumPT));
        } while (cBlockName && TechniqueExecuteNext());
}

//################################################################################
void DX9RENDER::RenderAnimation(int32_t ib, void *src, int32_t numVrts, int32_t minv, int32_t numv, int32_t startidx, int32_t numtrg,
                                bool isUpdateVB)
{
    if (numVrts <= 0 || !src || ib < 0)
        return;
    const uint32_t type = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
    const int32_t size = numVrts * sizeof(FVF_VERTEX);
    if (isUpdateVB || numVrts > numAniVerteces || !aniVBuffer)
    {
        // Create vertex buffer
        if (numVrts > numAniVerteces)
        {
            if (aniVBuffer)
                aniVBuffer->Release();
            aniVBuffer = nullptr;
            numAniVerteces = 0;
            if (CHECKD3DERR(d3d9->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, type, D3DPOOL_MANAGED, &aniVBuffer,
                                                     NULL)) == true)
                return;
            numAniVerteces = numVrts;
        }
        // Copy verteces
        uint8_t *ptr;
        RDTSC_B(_rdtsc);
        if (CHECKD3DERR(aniVBuffer->Lock(0, size, (VOID **)&ptr, 0)) == true)
            return;
        dwNumLV++;
        RDTSC_E(_rdtsc);
        memcpy(ptr, src, size);
        CHECKD3DERR(aniVBuffer->Unlock());
    }
    // Render
    if (CHECKD3DERR(SetFVF(type)) == true)
        return;

    if (CHECKD3DERR(d3d9->SetIndices(IndexBuffers[ib].buff)) == true)
        return;

    if (CHECKD3DERR(d3d9->SetStreamSource(0, aniVBuffer, 0, sizeof(FVF_VERTEX))) == true)
        return;

    CHECKD3DERR(d3d9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, minv, 0, numv, startidx, numtrg));

    dwNumDrawPrimitive++;
}

//################################################################################
void *DX9RENDER::LockVertexBuffer(int32_t id, uint32_t dwFlags)
{
    uint8_t *ptr;
    VertexBuffers[id].dwNumLocks++;
    if (CHECKD3DERR(VertexBuffers[id].buff->Lock(0, VertexBuffers[id].size, (VOID **)&ptr, dwFlags)))
        return nullptr;

    dwNumLV++;
    return ptr;
}

//################################################################################
void DX9RENDER::UnLockVertexBuffer(int32_t id)
{
        VertexBuffers[id].dwNumLocks--;
        CHECKD3DERR(VertexBuffers[id].buff->Unlock());
}

int32_t DX9RENDER::GetVertexBufferSize(int32_t id)
{
    return VertexBuffers[id].size;
}

void *DX9RENDER::LockIndexBuffer(int32_t id, uint32_t dwFlags)
{
    uint8_t *ptr = nullptr;
    IndexBuffers[id].dwNumLocks++;
    if (CHECKD3DERR(IndexBuffers[id].buff->Lock(0, IndexBuffers[id].size, (VOID **)&ptr, dwFlags)))
        return nullptr;

    dwNumLI++;
    return ptr;
}

void DX9RENDER::UnLockIndexBuffer(int32_t id)
{
        IndexBuffers[id].dwNumLocks--;
        CHECKD3DERR(IndexBuffers[id].buff->Unlock());
}

//################################################################################
void DX9RENDER::ReleaseVertexBuffer(int32_t id)
{
    if (VertexBuffers[id].buff == nullptr)
        return;
    CHECKD3DERR(VertexBuffers[id].buff->Release());
    VertexBuffers[id].buff = nullptr;
    VertexBuffers[id].dwNumLocks = 0;
}

//################################################################################
void DX9RENDER::ReleaseIndexBuffer(int32_t id)
{
    if (IndexBuffers[id].buff == nullptr)
        return;
    CHECKD3DERR(IndexBuffers[id].buff->Release());
    IndexBuffers[id].buff = nullptr;
    IndexBuffers[id].dwNumLocks = 0;
}

void DX9RENDER::SetTransform(int32_t type, D3DMATRIX *mtx)
{
    CMatrix m = *(CMatrix *)mtx;
    if (type == D3DTS_VIEW)
    {
        vViewRelationPos.x = -mtx->_41;
        vViewRelationPos.y = -mtx->_42;
        vViewRelationPos.z = -mtx->_43;

        CVECTOR vDeltaWorld = vWordRelationPos;
        m.MulToInvNorm(-vViewRelationPos, vWordRelationPos);
        vDeltaWorld -= vWordRelationPos;
        CMatrix mw;
        d3d9->GetTransform(D3DTS_WORLD, (D3DMATRIX *)&mw);
        mw.Pos() -= vDeltaWorld;
        d3d9->SetTransform(D3DTS_WORLD, mw);

        m.Pos() += vViewRelationPos;
    }
    else if (type == D3DTS_WORLD)
    {
        m.Pos() += vWordRelationPos;
    }

    CHECKD3DERR(d3d9->SetTransform(static_cast<D3DTRANSFORMSTATETYPE>(type), (D3DMATRIX *)&m));
}

void DX9RENDER::GetTransform(int32_t type, D3DMATRIX *mtx)
{
    CHECKD3DERR(d3d9->GetTransform(static_cast<D3DTRANSFORMSTATETYPE>(type), mtx));

    if (type == D3DTS_VIEW)
    {
        mtx->_41 -= vViewRelationPos.x;
        mtx->_42 -= vViewRelationPos.y;
        mtx->_43 -= vViewRelationPos.z;
    }
    else if (type == D3DTS_WORLD)
    {
        mtx->_41 -= vWordRelationPos.x;
        mtx->_42 -= vWordRelationPos.y;
        mtx->_43 -= vWordRelationPos.z;
    }
}

bool DX9RENDER::CreateState(ENTITY_STATE_GEN *state_gen)
{
    // state_gen->SetState("vm",sizeof(screen_size),screen_size,sizeof(bool),&window);
    return true;
}

bool DX9RENDER::LoadState(ENTITY_STATE *state)
{
    // GUARD(DX9RENDER::Init)
    // for(int32_t t=0; t<MAX_STEXTURES; t++)    Textures[t].d3dtex = NULL;
    // d3d = NULL;
    // d3d9 = NULL;
    // state->Struct(sizeof(screen_size),(char *)&screen_size);
    // state->MemoryBlock(sizeof(bool),(char *)&window);
    // InitDevice(window,core.GetAppHWND(),screen_size.x,screen_size.y);
    // UNGUARD
    return true;
}

void DX9RENDER::LostRender()
{
    if (resourcesReleased)
    {
        return;
    }

    InvokeEntitiesLostRender();

    Release(pOriginalScreenSurface);
    Release(pOriginalDepthSurface);
    Release(rectsVBuffer);
    for (int32_t b = 0; b < MAX_BUFFERS; b++)
    {
        if (VertexBuffers[b].buff)
        {
            if (VertexBuffers[b].buff->Release() > 0)
                psnip_trap();
        }
        if (IndexBuffers[b].buff)
        {
            if (IndexBuffers[b].buff->Release() > 0)
                psnip_trap();
        }
    }

    resourcesReleased = true;
}

void DX9RENDER::RestoreRender()
{
    d3d9->GetRenderTarget(0, &pOriginalScreenSurface);
    d3d9->GetDepthStencilSurface(&pOriginalDepthSurface);
    fSmallWidth = 128;
    fSmallHeight = 128;
    if (bPostProcessEnabled)
    {
        d3d9->CreateTexture(screen_size.x, screen_size.y, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                            &pPostProcessTexture, nullptr);
        d3d9->CreateTexture(static_cast<int>(fSmallWidth), static_cast<int>(fSmallHeight), 1, D3DUSAGE_RENDERTARGET,
                            D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pSmallPostProcessTexture, nullptr);
        d3d9->CreateTexture(static_cast<int>(fSmallWidth * 2.0f), static_cast<int>(fSmallHeight * 2.0f), 1,
                            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pSmallPostProcessTexture2,
                            nullptr);
    }
    if (!pPostProcessTexture || !pSmallPostProcessTexture || !pSmallPostProcessTexture2)
    {
        bPostProcessEnabled = false;
        bPostProcessError = true;
    }
    if (!bPostProcessError)
    {
        pPostProcessTexture->GetSurfaceLevel(0, &pPostProcessSurface);
        pSmallPostProcessTexture2->GetSurfaceLevel(0, &pSmallPostProcessSurface2);
        pSmallPostProcessTexture->GetSurfaceLevel(0, &pSmallPostProcessSurface);

        if (!pPostProcessSurface || !pSmallPostProcessSurface2 || !pSmallPostProcessSurface)
        {
            bPostProcessEnabled = false;
            bPostProcessError = true;
        }
    }
    if (!bPostProcessError)
    {
        ClearPostProcessSurface(pPostProcessSurface);
        ClearPostProcessSurface(pSmallPostProcessSurface);
        ClearPostProcessSurface(pSmallPostProcessSurface2);
    }
    ClearPostProcessSurface(pOriginalScreenSurface);
    d3d9->CreateVertexBuffer(rectsVBuffer_SizeInRects * 6 * sizeof(RECT_VERTEX), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                             RS_RECT_VERTEX_FORMAT, D3DPOOL_DEFAULT, &rectsVBuffer, nullptr);
    for (int32_t b = 0; b < MAX_BUFFERS; b++)
    {
        if (VertexBuffers[b].buff)
        {
            CHECKD3DERR(d3d9->CreateVertexBuffer(VertexBuffers[b].size, VertexBuffers[b].dwUsage, VertexBuffers[b].type,
                                                 D3DPOOL_DEFAULT, &VertexBuffers[b].buff, NULL));
        }
        if (IndexBuffers[b].buff)
        {
            CHECKD3DERR(d3d9->CreateIndexBuffer(IndexBuffers[b].size, IndexBuffers[b].dwUsage, D3DFMT_INDEX16,
                                                D3DPOOL_DEFAULT, &IndexBuffers[b].buff, NULL));
        }
    }
    int32_t num_stages;
    num_stages = 8;
    for (int32_t s = 0; s < num_stages; s++)
    {
        // texture operation
        SetTextureStageState(s, D3DTSS_COLORARG1, D3DTA_CURRENT);
        SetTextureStageState(s, D3DTSS_COLORARG2, D3DTA_TEXTURE);
        SetTextureStageState(s, D3DTSS_COLOROP, D3DTOP_DISABLE);

        // texture coord
        SetTextureStageState(s, D3DTSS_TEXCOORDINDEX, s);

        // texture filtering
        SetSamplerState(s, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        SetSamplerState(s, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        SetSamplerState(s, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    }
    // set base texture and diffuse+specular lighting
    SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADDSIGNED);
    D3DLIGHT9 l{};
    l.Type = D3DLIGHT_POINT;
    l.Range = 100.0f;
    l.Attenuation0 = 1.0f;
    for (int i = 0; i < 8; i++)
    {
        CHECKD3DERR(d3d9->SetLight(i, &l));
        CHECKD3DERR(d3d9->LightEnable(i, false));
    }
    SetCommonStates();

#ifdef _WIN32 // Effects
    RecompileEffects();
#else
    pTechnique = std::make_unique<CTechnique>(this);
    pTechnique->DecodeFiles();
#endif

    InvokeEntitiesRestoreRender();

    resourcesReleased = false;
}

void DX9RENDER::RecompileEffects()
{
#ifdef _WIN32 // Effects
    effects_.release();

    std::filesystem::path cur_path = std::filesystem::current_path();
    std::filesystem::current_path(std::filesystem::u8path(fio->_GetExecutableDirectory()));
    for (const auto &p : std::filesystem::recursive_directory_iterator("resource/techniques"))
        if (is_regular_file(p) && p.path().extension() == ".fx")
        {
            auto s = p.path().string(); // hug microsoft
            effects_.compile(s.c_str());
        }
    std::filesystem::current_path(cur_path);
#endif
}

bool DX9RENDER::ResetDevice()
{
    LostRender();

    if (CHECKD3DERR(d3d9->Reset(&d3dpp)))
        return false;

    RestoreRender();

    return true;
}

void DX9RENDER::SetGLOWParams(float _fBlurBrushSize, int32_t _GlowIntensity, int32_t _GlowPasses)
{
    fBlurSize = _fBlurBrushSize;
    GlowIntensity = _GlowIntensity;
    iBlurPasses = _GlowPasses;
}

void DX9RENDER::RunStart()
{
    auto *pScriptRender = static_cast<VDATA *>(core.GetScriptVariable("Render"));
    ATTRIBUTES *pARender = pScriptRender->GetAClass();

    bSeaEffect = pARender->GetAttributeAsDword("SeaEffect", 0) != 0;
    fSeaEffectSize = pARender->GetAttributeAsFloat("SeaEffectSize", 0.0f);
    fSeaEffectSpeed = pARender->GetAttributeAsFloat("SeaEffectSpeed", 0.0f);
    dwBackColor = pARender->GetAttributeAsDword("BackColor", 0);

    if (bSeaEffect)
    {
        fSin += static_cast<float>(core.GetRDeltaTime()) * 0.001f * fSeaEffectSpeed;

        const auto sx = static_cast<float>(screen_size.x);
        const auto sy = static_cast<float>(screen_size.y);

        const auto fDX = static_cast<float>(static_cast<int32_t>(sx * fSeaEffectSize));
        const float fDY = static_cast<float>(static_cast<int32_t>(sy * fSeaEffectSize));

        const float sx2 = sx + fDX * 2.0f;
        const float sy2 = sy + fDY * 2.0f;

        for (int32_t y = 0; y < 32; y++)
        {
            for (int32_t x = 0; x < 32; x++)
            {
                qv[x + y * 32].vPos =
                    Vector4(sx * static_cast<float>(x) / 31.0f, sy * static_cast<float>(y) / 31.0f, 0.0f, 1.0f);

                qv[x + y * 32].u0 = (sx * static_cast<float>(x) / 31.0f +
                                     sinf(fSin + static_cast<float>(x) / 31.0f * PI * 16.0f) * fDX + fDX) /
                                    sx2;
                qv[x + y * 32].v0 = (sy * static_cast<float>(y) / 31.0f +
                                     sinf(fSin + static_cast<float>(y) / 31.0f * PI * 16.0f) * fDY + fDY) /
                                    sy2;
            }
        }
    }

    bNeedCopyToScreen = true;
    //------------------------------------------
    if ((bPostProcessEnabled || bSeaEffect) && (!bPostProcessError))
    {
        SetPostProcessTextureAsRenderTarget();
    }
    else
    {
        SetScreenAsRenderTarget();
    }

    DX9Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | ((stencil_format == D3DFMT_D24S8) ? D3DCLEAR_STENCIL : 0));

    dwNumDrawPrimitive = 0;
    dwNumLV = 0;
    dwNumLI = 0;
    BeginScene();

    //------------------------------------------
    bInsideScene = true;

    // execute default technique for set default render/texture states
    // if (TechniqueExecuteStart("default")) do{} while (TechniqueExecuteNext());

    // boal del_cheat
    if (core.Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0 && core.Controls->GetDebugAsyncKeyState(VK_F11) < 0)
    {
        InvokeEntitiesLostRender();
#ifdef _WIN32 // Effects
        RecompileEffects();
#else
        pTechnique = std::make_unique<CTechnique>(this);
        pTechnique->DecodeFiles();
#endif
        InvokeEntitiesRestoreRender();
    }

    SetRenderState(D3DRS_FILLMODE, (core.Controls->GetDebugAsyncKeyState('F') < 0) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
    // SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID); eddy

    PlayToTexture();

    // make screenshot
    if (CONTROL_STATE cs; core.Controls->GetControlState(kKeyTakeScreenshot, cs) 
                          && cs.state == CST_ACTIVATED)
    {
        SaveShoot();
    }
}

void DX9RENDER::RunEnd()
{
    // MakePostProcess();

    bInsideScene = false;
    DX9EndScene();
    if (progressTexture >= 0)
    {
        const int32_t oldCnt = progressSafeCounter;
        ProgressView();
        progressSafeCounter = oldCnt;
        if (progressSafeCounter >= 50)
        {
            EndProgressView();
        }
        else
            progressSafeCounter++;
    }
}

char Buff_4k[4096];

int32_t DX9RENDER::Print(int32_t x, int32_t y, const char *format, ...)
{
    // GUARD(DX9RENDER::Print)
    if (idFontCurrent < 0 || idFontCurrent >= nFontQuantity)
        return 0;
    if (FontList[idFontCurrent].font == nullptr || FontList[idFontCurrent].ref == 0)
        return 0;

    va_list args;
    va_start(args, format);
    vsnprintf(Buff_4k, sizeof(Buff_4k), format, args);
    va_end(args);

    return FontList[idFontCurrent].font->Print(x, y, Buff_4k);
    // UNGUARD
}

int32_t DX9RENDER::Print(int32_t nFontNum, uint32_t color, int32_t x, int32_t y, const char *format, ...)
{
    // GUARD(DX9RENDER::Print)
    if (nFontNum < 0 || nFontNum >= nFontQuantity)
        return 0;
    if (FontList[nFontNum].font == nullptr || FontList[nFontNum].ref == 0)
        return 0;

    va_list args;
    va_start(args, format);
    vsnprintf(Buff_4k, sizeof(Buff_4k), format, args);
    va_end(args);

    FontList[nFontNum].font->StoreFontParameters();
    FontList[nFontNum].font->SetColor(color);
    const int32_t retVal = FontList[nFontNum].font->Print(x, y, Buff_4k);
    FontList[nFontNum].font->RestoreFontParameters();
    return retVal;
    // UNGUARD
}

int32_t DX9RENDER::StringWidth(const char *string, int32_t nFontNum, float fScale, int32_t scrWidth)
{
    if (nFontNum < 0 || nFontNum >= nFontQuantity)
        return 0;
    FONT *pFont = FontList[nFontNum].font;
    if (FontList[nFontNum].ref == 0 || pFont == nullptr)
        return 0;

    pFont->StoreFontParameters();

    const int32_t xs = screen_size.x;
    if (scrWidth == 0)
        scrWidth = xs;
    if (xs != scrWidth)
        fScale *= static_cast<float>(xs) / scrWidth;
    pFont->SetScale(fScale);

    const int32_t retVal = pFont->GetStringWidth(string);
    pFont->RestoreFontParameters();
    return retVal;
}

int32_t DX9RENDER::CharWidth(utf8::u8_char ch, int32_t nFontNum, float fScale, int32_t scrWidth)
{
    std::string str(ch.b, ch.l);
    return StringWidth(str.c_str(), nFontNum, fScale, scrWidth);
}

int32_t DX9RENDER::CharHeight(int32_t fontID)
{
    if (fontID < 0 || fontID >= nFontQuantity)
        return 0;
    if (FontList[fontID].ref == 0 || FontList[fontID].font == nullptr)
        return 0;

    return FontList[fontID].font->GetHeight();
}

int32_t DX9RENDER::ExtPrint(int32_t nFontNum, uint32_t foreColor, uint32_t backColor, int wAlign, bool bShadow, float fScale,
                         int32_t scrWidth, int32_t scrHeight, int32_t x, int32_t y, const char *format, ...)
{
    // GUARD(DX9RENDER::ExtPrint)

    if (nFontNum < 0 || nFontNum >= nFontQuantity)
        return 0;
    FONT *pFont = FontList[nFontNum].font;
    if (FontList[nFontNum].ref == 0 || pFont == nullptr)
        return 0;

    va_list args;
    va_start(args, format);
    vsnprintf(Buff_4k, sizeof(Buff_4k), format, args);
    va_end(args);

    pFont->StoreFontParameters();

    IDirect3DSurface9 *pRenderTarget;
    GetRenderTarget(&pRenderTarget);
    D3DSURFACE_DESC dscrSurface;
    pRenderTarget->GetDesc(&dscrSurface);
    const int32_t xs = dscrSurface.Width;
    const int32_t ys = dscrSurface.Height;
    pRenderTarget->Release();
    if (scrWidth == 0)
        scrWidth = xs;
    if (scrHeight == 0)
        scrHeight = ys;
    if (xs != scrWidth)
    {
        const float fHorzScale = static_cast<float>(xs) / scrWidth;
        x = static_cast<int32_t>(x * fHorzScale);
        fScale *= fHorzScale;
    }
    if (ys != scrHeight)
        y = static_cast<int32_t>(y * (static_cast<float>(ys) / scrHeight));

    switch (wAlign)
    {
    case PR_ALIGN_CENTER:
        x -= static_cast<int32_t>((pFont->GetStringWidth(Buff_4k) + 1L) / 2L * fScale);
        break;
    case PR_ALIGN_RIGHT:
        x -= static_cast<int32_t>(pFont->GetStringWidth(Buff_4k) * fScale);
        break;
    }

    pFont->SetColor(foreColor);
    pFont->SetShadow(bShadow);
    pFont->SetScale(fScale);
    const int32_t retVal = pFont->Print(x, y, Buff_4k);

    pFont->RestoreFontParameters();
    return retVal;
    // UNGUARD
}

int32_t DX9RENDER::LoadFont(const char *fontName)
{
    if (fontName == nullptr)
        return -1L;
    char sDup[256];
    if (strlen(fontName) < sizeof(sDup) - 1)
        strcpy_s(sDup, fontName);
    else
    {
        strncpy_s(sDup, fontName, sizeof(sDup) - 1);
        sDup[sizeof(sDup) - 1] = 0;
    }

    std::ranges::for_each(sDup, [](char &c) { c = std::toupper(c); });
    fontName = sDup;

    const uint32_t hashVal = MakeHashValue(fontName);

    int32_t i;
    for (i = 0; i < nFontQuantity; i++)
        if (FontList[i].hash == hashVal && storm::iEquals(FontList[i].name, fontName))
        {
            if (FontList[i].ref > 0)
                FontList[i].ref++;
            else
            {
                FontList[i].ref = 1;
                FontList[i].font->RepeatInit();
            }
            return i;
        }
    if (nFontQuantity < MAX_FONTS)
    {
        if ((FontList[i].font = new FONT) == nullptr)
            throw std::runtime_error("allocate memory error");
        if (!FontList[i].font->Init(fontName, fontIniFileName, d3d9, this))
        {
            delete FontList[i].font;
            core.Trace("Can't init font %s", fontName);
            return -1L;
        }
        FontList[i].hash = hashVal;
        FontList[i].ref = 1;
        const auto len = strlen(fontName) + 1;
        if ((FontList[i].name = new char[len]) == nullptr)
            throw std::runtime_error("allocate memory error");
        strcpy_s(FontList[i].name, len, fontName);
        nFontQuantity++;
    }
    else
        throw std::runtime_error("maximal font quantity exceeded");
    return i;
}

bool DX9RENDER::UnloadFont(const char *fontName)
{
    if (fontName == nullptr)
        return false;
    char sDup[256];
    if (strlen(fontName) < sizeof(sDup) - 1)
        strcpy_s(sDup, fontName);
    else
    {
        strncpy_s(sDup, fontName, sizeof(sDup) - 1);
        sDup[sizeof(sDup) - 1] = 0;
    }
    std::ranges::for_each(sDup, [](char &c) { c = std::toupper(c); });
    fontName = sDup;
    const uint32_t hashVal = MakeHashValue(fontName);

    for (int i = 0; i < nFontQuantity; i++)
        if (FontList[i].hash == hashVal && storm::iEquals(FontList[i].name, fontName))
            return UnloadFont(i);
    core.Trace("Font name \"%s\" is not containing", fontName);
    return false;
}

bool DX9RENDER::UnloadFont(int32_t fontID)
{
    if (fontID < 0 || fontID >= nFontQuantity)
        return false;

    if (FontList[fontID].ref > 0)
    {
        FontList[fontID].ref--;
        if (FontList[fontID].ref == 0)
        {
            FontList[fontID].font->TempUnload();
            // if (idFontCurrent == fontID)
            idFontCurrent = FONT_DEFAULT;
        }
        else
            return false;
    }

    return true;
}

bool DX9RENDER::IncRefCounter(int32_t fontID)
{
    if (fontID < 0 || fontID >= nFontQuantity)
        return false;

    FontList[fontID].ref++;

    return true;
}

bool DX9RENDER::SetCurFont(const char *fontName)
{
    if (fontName == nullptr)
        return false;
    char sDup[256];
    if (strlen(fontName) < sizeof(sDup) - 1)
        strcpy_s(sDup, fontName);
    else
    {
        strncpy_s(sDup, fontName, sizeof(sDup) - 1);
        sDup[sizeof(sDup) - 1] = 0;
    }
    std::ranges::for_each(sDup, [](char &c) { c = std::toupper(c); });
    fontName = sDup;
    const uint32_t hashVal = MakeHashValue(fontName);

    for (int i = 0; i < nFontQuantity; i++)
        if (FontList[i].hash == hashVal)
        {
            idFontCurrent = i;
            return true;
        }
    core.Trace("Font name \"%s\" is not containing", fontName);
    return false;
}

bool DX9RENDER::SetCurFont(int32_t fontID)
{
    if (fontID < 0 || fontID >= nFontQuantity)
        return false;
    idFontCurrent = fontID;
    return true;
}

int32_t DX9RENDER::GetCurFont()
{
    if (idFontCurrent >= 0 && idFontCurrent < nFontQuantity)
        return idFontCurrent;
    return -1L;
}

char *DX9RENDER::GetFontIniFileName()
{
    return fontIniFileName;
}

bool DX9RENDER::SetFontIniFileName(const char *iniName)
{
    if (fontIniFileName != nullptr && iniName != nullptr && storm::iEquals(fontIniFileName, iniName))
        return true;

    delete fontIniFileName;
    if (iniName == nullptr)
    {
        fontIniFileName = nullptr;
        return false;
    }
    const auto len = strlen(iniName) + 1;
    if ((fontIniFileName = new char[len]) == nullptr)
        throw std::runtime_error("allocate memory error");
    strcpy_s(fontIniFileName, len, iniName);

    for (int n = 0; n < nFontQuantity; n++)
    {
        delete FontList[n].font;

        if ((FontList[n].font = new FONT) == nullptr)
            throw std::runtime_error("allocate memory error");
        FontList[n].font->Init(FontList[n].name, fontIniFileName, d3d9, this);
        if (FontList[n].ref == 0)
            FontList[n].font->TempUnload();
    }

    return true;
}

void DX9RENDER::SetCommonStates()
{
    SetRenderState(D3DRS_DITHERENABLE, TRUE);
    SetRenderState(D3DRS_ALPHATESTENABLE, true);
    //----------Z---------
    SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

    SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP);

    SetRenderState(D3DRS_LIGHTING, TRUE);
    SetRenderState(D3DRS_LOCALVIEWER, FALSE);
    SetRenderState(D3DRS_AMBIENT, 0x505050);
    SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);

    SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    SetRenderState(D3DRS_COLORVERTEX, TRUE);
    SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    SetRenderState(D3DRS_ALPHAREF, 0xa0);
    SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    // *****************************************************************88
    // texture filtering
    SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
    SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 3);

    // unchanged texture stage states - both for base and detal texture
    SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
    SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
    SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
    SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_CURRENT);
    SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
    SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

    // general
    SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    SetRenderState(D3DRS_ALPHAREF, 0xa0);
    SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    // lighting effects
    SetRenderState(D3DRS_AMBIENT, 0x404040);
    SetRenderState(D3DRS_LIGHTING, FALSE); // TRUE);
    SetRenderState(D3DRS_COLORVERTEX, TRUE);
    SetRenderState(D3DRS_SPECULARENABLE, FALSE); // TRUE);

    // AA
    SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
}

HRESULT DX9RENDER::GetViewport(D3DVIEWPORT9 *pViewport)
{
    return CHECKD3DERR(d3d9->GetViewport(pViewport));
}

HRESULT DX9RENDER::SetViewport(const D3DVIEWPORT9 *pViewport)
{
    return CHECKD3DERR(d3d9->SetViewport(pViewport));
}

uint32_t DX9RENDER::SetRenderState(uint32_t State, uint32_t Value)
{
    return CHECKD3DERR(d3d9->SetRenderState(static_cast<D3DRENDERSTATETYPE>(State), Value));
}

uint32_t DX9RENDER::GetRenderState(uint32_t State, uint32_t *pValue)
{
    return CHECKD3DERR(d3d9->GetRenderState(static_cast<D3DRENDERSTATETYPE>(State), (DWORD *)pValue));
}

uint32_t DX9RENDER::GetSamplerState(uint32_t Sampler, D3DSAMPLERSTATETYPE Type, uint32_t *pValue)
{
    return CHECKD3DERR(d3d9->GetSamplerState(Sampler, Type, (DWORD *)pValue));
}

uint32_t DX9RENDER::SetSamplerState(uint32_t Sampler, D3DSAMPLERSTATETYPE Type, uint32_t Value)
{
    return CHECKD3DERR(d3d9->SetSamplerState(Sampler, Type, Value));
}

uint32_t DX9RENDER::SetTextureStageState(uint32_t Stage, uint32_t Type, uint32_t Value)
{
    return CHECKD3DERR(d3d9->SetTextureStageState(Stage, static_cast<D3DTEXTURESTAGESTATETYPE>(Type), Value));
}

uint32_t DX9RENDER::GetTextureStageState(uint32_t Stage, uint32_t Type, uint32_t *pValue)
{
    return CHECKD3DERR(d3d9->GetTextureStageState(Stage, static_cast<D3DTEXTURESTAGESTATETYPE>(Type), (DWORD *)pValue));
}

void DX9RENDER::GetCamera(CVECTOR &pos, CVECTOR &ang, float &perspective)
{
    pos = Pos;
    ang = Ang;
    perspective = Fov;
}

using TGA_H = struct tagTGA_H
{
    uint8_t byte1; // = 0
    uint8_t byte2; // = 0
    uint8_t type;
    uint8_t byte4_9[9]; // = 0
    uint16_t width;
    uint16_t height;
    uint8_t bpp; // bit per pixel
    uint8_t attr : 4;
    uint8_t rez : 1;
    uint8_t origin : 1;
    uint8_t storage : 2;
};

// WORD Temp[1600*4];

void DX9RENDER::SaveShoot()
{
    bMakeShoot = true;
}

void DX9RENDER::MakeScreenShot()
{
    bMakeShoot = false;

    IDirect3DSurface9 *renderTarget;
    if (FAILED(GetRenderTarget(&renderTarget)))
    {
        core.Trace("Failed to make screenshot");
        return;
    }

    IDirect3DSurface9 *surface;
    if (FAILED(CreateOffscreenPlainSurface(screen_size.x, screen_size.y, D3DFMT_X8R8G8B8, &surface)))
    {
        renderTarget->Release();
        core.Trace("Failed to make screenshot");
        return;
    }

    if (CHECKD3DERR(D3DXLoadSurfaceFromSurface(surface, NULL, NULL, renderTarget, NULL, NULL, D3DX_DEFAULT, 0)))
    {
        surface->Release();
        renderTarget->Release();
        core.Trace("Failed to make screenshot");
        return;
    }

    const auto screenshot_base_filename = fmt::format("{:%Y-%m-%d_%H-%M-%S}", fmt::localtime(std::time(nullptr)));
    auto screenshot_path = fs::GetScreenshotsPath() / screenshot_base_filename;
    screenshot_path.replace_extension(screenshotExt);
    for(size_t i = 0; exists(screenshot_path); ++i)
    {
        screenshot_path.replace_filename(screenshot_base_filename + "_" + std::to_string(i));
        screenshot_path.replace_extension(screenshotExt);
    }
#ifdef _WIN32 // Screenshot
    D3DXSaveSurfaceToFile(screenshot_path.c_str(), screenshotFormat, surface, nullptr, nullptr);
#endif

    surface->Release();
    renderTarget->Release();
}

PLANE *DX9RENDER::GetPlanes()
{
    FindPlanes(d3d9);
    return viewplane;
}

void DX9RENDER::FindPlanes(IDirect3DDevice9 *d3dDevice)
{
    D3DMATRIX m;
    CVECTOR v[4];
    GetTransform(D3DTS_PROJECTION, static_cast<D3DMATRIX *>(&m));
    // left
    v[0].x = m.m[0][0];
    v[0].y = 0.0f;
    v[0].z = 1.0f;
    // right
    v[1].x = -m.m[0][0];
    v[1].y = 0.0f;
    v[1].z = 1.0f;
    // top
    v[2].x = 0.0f;
    v[2].y = -m.m[1][1];
    v[2].z = 1.0f;
    // bottom
    v[3].x = 0.0f;
    v[3].y = m.m[1][1];
    v[3].z = 1.0f;
    v[0] = !v[0];
    v[1] = !v[1];
    v[2] = !v[2];
    v[3] = !v[3];

    GetTransform(D3DTS_VIEW, static_cast<D3DMATRIX *>(&m));
    CVECTOR pos;

    pos.x = -m._41 * m._11 - m._42 * m._12 - m._43 * m._13;
    pos.y = -m._41 * m._21 - m._42 * m._22 - m._43 * m._23;
    pos.z = -m._41 * m._31 - m._42 * m._32 - m._43 * m._33;

    viewplane[0].Nx = v[0].x * m._11 + v[0].y * m._12 + v[0].z * m._13;
    viewplane[0].Ny = v[0].x * m._21 + v[0].y * m._22 + v[0].z * m._23;
    viewplane[0].Nz = v[0].x * m._31 + v[0].y * m._32 + v[0].z * m._33;

    viewplane[1].Nx = v[1].x * m._11 + v[1].y * m._12 + v[1].z * m._13;
    viewplane[1].Ny = v[1].x * m._21 + v[1].y * m._22 + v[1].z * m._23;
    viewplane[1].Nz = v[1].x * m._31 + v[1].y * m._32 + v[1].z * m._33;

    viewplane[2].Nx = v[2].x * m._11 + v[2].y * m._12 + v[2].z * m._13;
    viewplane[2].Ny = v[2].x * m._21 + v[2].y * m._22 + v[2].z * m._23;
    viewplane[2].Nz = v[2].x * m._31 + v[2].y * m._32 + v[2].z * m._33;

    viewplane[3].Nx = v[3].x * m._11 + v[3].y * m._12 + v[3].z * m._13;
    viewplane[3].Ny = v[3].x * m._21 + v[3].y * m._22 + v[3].z * m._23;
    viewplane[3].Nz = v[3].x * m._31 + v[3].y * m._32 + v[3].z * m._33;

    viewplane[0].D = (pos.x * viewplane[0].Nx + pos.y * viewplane[0].Ny + pos.z * viewplane[0].Nz);
    viewplane[1].D = (pos.x * viewplane[1].Nx + pos.y * viewplane[1].Ny + pos.z * viewplane[1].Nz);
    viewplane[2].D = (pos.x * viewplane[2].Nx + pos.y * viewplane[2].Ny + pos.z * viewplane[2].Nz);
    viewplane[3].D = (pos.x * viewplane[3].Nx + pos.y * viewplane[3].Ny + pos.z * viewplane[3].Nz);
}

#ifdef _WIN32 // Effects
bool DX9RENDER::TechniqueExecuteStart(const char *cBlockName)
{
    if (!cBlockName)
        return false;
    return effects_.begin(cBlockName);
}
#else
bool DX9RENDER::TechniqueExecuteStart(const char *cBlockName)
{
    if (!cBlockName)
        return false;
    pTechnique->SetCurrentBlock(cBlockName, 0, nullptr);
    return pTechnique->ExecutePassStart();
}
#endif

bool DX9RENDER::TechniqueExecuteNext()
{
#ifdef _WIN32 // Effects
    return effects_.next();
#else
    return pTechnique->ExecutePassNext();
#endif
}

void DX9RENDER::DrawRects(RS_RECT *pRSR, uint32_t dwRectsNum, const char *cBlockName, uint32_t dwSubTexturesX,
                          uint32_t dwSubTexturesY, float fScaleX, float fScaleY)
{
    if (!pRSR || dwRectsNum == 0 || !rectsVBuffer)
        return;

    bool bDraw = true;

    static CMatrix camMtx, oldWorldMatrix, IMatrix;
    d3d9->GetTransform(D3DTS_VIEW, camMtx);
    d3d9->GetTransform(D3DTS_WORLD, oldWorldMatrix);

    fScaleY *= GetHeightDeformator();

    float du, dv;
    bool bUseSubTextures = false;
    if (dwSubTexturesX > 1)
    {
        bUseSubTextures = true;
        du = 1.0f / dwSubTexturesX;
    }
    else
    {
        dwSubTexturesX = 1;
        du = 1.0f;
    }
    if (dwSubTexturesY > 1)
    {
        bUseSubTextures = true;
        dv = 1.0f / dwSubTexturesY;
    }
    else
    {
        dwSubTexturesY = 1;
        dv = 1.0f;
    }

    d3d9->SetTransform(D3DTS_VIEW, IMatrix);
    d3d9->SetTransform(D3DTS_WORLD, IMatrix);

    for (uint32_t cnt = 0; cnt < dwRectsNum;)
    {
        // Number of rectangles to draw at a time
        uint32_t drawCount = dwRectsNum - cnt;
        if (drawCount > rectsVBuffer_SizeInRects)
            drawCount = rectsVBuffer_SizeInRects;
        // Buffer
        RECT_VERTEX *data = nullptr;
        if (rectsVBuffer->Lock(0, drawCount * 6 * sizeof(RECT_VERTEX), (VOID **)&data, D3DLOCK_DISCARD) != D3D_OK)
            return;
        if (!data)
            return;
        // Filling the buffer
        for (uint32_t i = 0; i < drawCount && cnt < dwRectsNum; i++)
        {
            // Local array of a particle
            RECT_VERTEX *buffer = &data[i * 6];
            RS_RECT &rect = pRSR[cnt++];
            CVECTOR pos = camMtx * (rect.vPos + vWordRelationPos);
            const float sizex = rect.fSize * fScaleX;
            const float sizey = rect.fSize * fScaleY;
            const float sn = sinf(rect.fAngle);
            const float cs = cosf(rect.fAngle);
            const int32_t color = rect.dwColor;
            float u1, v1, u2, v2;
            if (!bUseSubTextures)
            {
                v1 = u1 = 0.0f;
                v2 = u2 = 1.0f;
            }
            else
            {
                u1 = (rect.dwSubTexture % dwSubTexturesX) * du;
                v1 = ((rect.dwSubTexture / dwSubTexturesX) % dwSubTexturesY) * dv;
                u2 = u1 + du;
                v2 = v1 + dv;
            }
            // Filling the particle buffer
            buffer[0].pos = pos + CVECTOR(sizex * (-cs - sn), sizey * (sn - cs), 0.0f);
            buffer[0].color = color;
            buffer[0].u = u1;
            buffer[0].v = v2;
            buffer[1].pos = pos + CVECTOR(sizex * (-cs + sn), sizey * (sn + cs), 0.0f);
            buffer[1].color = color;
            buffer[1].u = u1;
            buffer[1].v = v1;
            buffer[2].pos = pos + CVECTOR(sizex * (cs + sn), sizey * (-sn + cs), 0.0f);
            buffer[2].color = color;
            buffer[2].u = u2;
            buffer[2].v = v1;
            buffer[3].pos = buffer[0].pos;
            buffer[3].color = color;
            buffer[3].u = u1;
            buffer[3].v = v2;
            buffer[4].pos = buffer[2].pos;
            buffer[4].color = color;
            buffer[4].u = u2;
            buffer[4].v = v1;
            buffer[5].pos = pos + CVECTOR(sizex * (cs - sn), sizey * (-sn - cs), 0.0f);
            buffer[5].color = color;
            buffer[5].u = u2;
            buffer[5].v = v2;
        }
        // Draw a buffer
        rectsVBuffer->Unlock();
        CHECKD3DERR(SetFVF(RS_RECT_VERTEX_FORMAT));
        if (cBlockName && cBlockName[0])
            bDraw = TechniqueExecuteStart(cBlockName);
        if (bDraw)
            do
            {
                CHECKD3DERR(SetStreamSource(0, rectsVBuffer, sizeof(RECT_VERTEX)));
                CHECKD3DERR(DrawPrimitive(D3DPT_TRIANGLELIST, 0, drawCount * 2));
            } while (cBlockName && TechniqueExecuteNext());
    }

    d3d9->SetTransform(D3DTS_VIEW, camMtx);
    d3d9->SetTransform(D3DTS_WORLD, oldWorldMatrix);
}

void DX9RENDER::DrawSprites(RS_SPRITE *pRSS, uint32_t dwSpritesNum, const char *cBlockName)
{
    uint32_t i;
#define RS_SPRITE_VERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
    if (dwSpritesNum == 0)
        return;

    auto *pIndices = new uint16_t[dwSpritesNum * 6];

    for (i = 0; i < dwSpritesNum; i++)
    {
        pIndices[i * 6 + 0] = static_cast<uint16_t>(i * 4 + 0);
        pIndices[i * 6 + 1] = static_cast<uint16_t>(i * 4 + 3);
        pIndices[i * 6 + 2] = static_cast<uint16_t>(i * 4 + 2);
        pIndices[i * 6 + 3] = static_cast<uint16_t>(i * 4 + 0);
        pIndices[i * 6 + 4] = static_cast<uint16_t>(i * 4 + 2);
        pIndices[i * 6 + 5] = static_cast<uint16_t>(i * 4 + 1);
    }

    SetFVF(RS_SPRITE_VERTEX_FORMAT);

    bool bDraw = true;
    if (cBlockName && cBlockName[0])
        bDraw = TechniqueExecuteStart(cBlockName);
    if (bDraw)
        do
        {
            DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, dwSpritesNum * 4, dwSpritesNum * 2, pIndices, D3DFMT_INDEX16,
                                   pRSS, sizeof(RS_SPRITE));
        } while (cBlockName && TechniqueExecuteNext());
    delete[] pIndices;
}

void DX9RENDER::DrawLines(RS_LINE *pRSL, uint32_t dwLinesNum, const char *cBlockName)
{
    if (!pRSL || dwLinesNum == 0)
        return;

#define RS_LINE_VERTEX_FORMAT (D3DFVF_XYZ | D3DFVF_DIFFUSE)

    bool bDraw = true;

    if (cBlockName && cBlockName[0])
        bDraw = TechniqueExecuteStart(cBlockName);
    if (bDraw)
        do
        {
            DrawPrimitiveUP(D3DPT_LINELIST, RS_LINE_VERTEX_FORMAT, dwLinesNum, pRSL, sizeof(RS_LINE));
        } while (cBlockName && TechniqueExecuteNext());
}

void DX9RENDER::DrawLines2D(RS_LINE2D *pRSL2D, size_t dwLinesNum, const char *cBlockName)
{
    if (!pRSL2D || dwLinesNum == 0)
        return;

#define RS_LINE2D_VERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

    bool bDraw = true;

    if (cBlockName && cBlockName[0])
        bDraw = TechniqueExecuteStart(cBlockName);
    if (bDraw)
        do
        {
            DrawPrimitiveUP(D3DPT_LINELIST, RS_LINE2D_VERTEX_FORMAT, dwLinesNum, pRSL2D, sizeof(RS_LINE2D));
        } while (cBlockName && TechniqueExecuteNext());
}

//-----------------------
HRESULT DX9RENDER::CreateVertexBuffer(UINT Length, uint32_t Usage, uint32_t FVF, D3DPOOL Pool,
                                      IDirect3DVertexBuffer9 **ppVertexBuffer)
{
    return CHECKD3DERR(d3d9->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, NULL));
}

HRESULT DX9RENDER::VBLock(IDirect3DVertexBuffer9 *pVB, UINT OffsetToLock, UINT SizeToLock, uint8_t **ppbData,
                          uint32_t Flags)
{
    dwNumLV++;
    return CHECKD3DERR(pVB->Lock(OffsetToLock, SizeToLock, (VOID **)ppbData, Flags));
}

void DX9RENDER::VBUnlock(IDirect3DVertexBuffer9 *pVB)
{
    CHECKD3DERR(pVB->Unlock());
}

HRESULT DX9RENDER::SetFVF(uint32_t handle)
{
    return CHECKD3DERR(d3d9->SetVertexShader(NULL)) || CHECKD3DERR(d3d9->SetFVF(handle));
}

HRESULT DX9RENDER::SetStreamSource(UINT StreamNumber, void *pStreamData, UINT Stride)
{
    return CHECKD3DERR(
        d3d9->SetStreamSource(StreamNumber, static_cast<IDirect3DVertexBuffer9 *>(pStreamData), 0, Stride));
}

HRESULT DX9RENDER::SetIndices(void *pIndexData)
{
    return CHECKD3DERR(d3d9->SetIndices(static_cast<IDirect3DIndexBuffer9 *>(pIndexData)));
}

HRESULT DX9RENDER::DrawPrimitive(D3DPRIMITIVETYPE dwPrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
    dwNumDrawPrimitive++;
    return CHECKD3DERR(d3d9->DrawPrimitive(dwPrimitiveType, StartVertex, PrimitiveCount));
}

HRESULT DX9RENDER::Release(IUnknown *pObject)
{
    if (pObject)
    {
        const auto result = pObject->Release();
        pObject = nullptr;
        return result;
    }

    return D3D_OK;
}

HRESULT DX9RENDER::GetRenderTarget(IDirect3DSurface9 **ppRenderTarget)
{
    return CHECKD3DERR(d3d9->GetRenderTarget(0, ppRenderTarget));
}

HRESULT DX9RENDER::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface)
{
    return CHECKD3DERR(d3d9->GetDepthStencilSurface(ppZStencilSurface));
}

HRESULT DX9RENDER::GetCubeMapSurface(IDirect3DCubeTexture9 *ppCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level,
                                     IDirect3DSurface9 **ppCubeMapSurface)
{
    return ppCubeTexture->GetCubeMapSurface(FaceType, Level, ppCubeMapSurface);
}

HRESULT DX9RENDER::SetRenderTarget(IDirect3DSurface9 *pRenderTarget, IDirect3DSurface9 *pNewZStencil)
{
    auto result = !CHECKD3DERR(d3d9->SetDepthStencilSurface(pNewZStencil)) &&
                  !CHECKD3DERR(d3d9->SetRenderTarget(0, pRenderTarget));
    return result ? D3D_OK : S_FALSE;
}

HRESULT DX9RENDER::Clear(uint32_t Count, CONST D3DRECT *pRects, uint32_t Flags, D3DCOLOR Color, float Z,
                         uint32_t Stencil)
{
    return CHECKD3DERR(d3d9->Clear(Count, pRects, Flags, Color, Z, Stencil));
}

static bool isInScene = false;

HRESULT DX9RENDER::BeginScene()
{
    if (!isInScene)
    {
        isInScene = true;
        return CHECKD3DERR(d3d9->BeginScene());
    }

    return D3D_OK;
}

HRESULT DX9RENDER::EndScene()
{
    if (isInScene)
    {
        isInScene = false;
        return CHECKD3DERR(d3d9->EndScene());
    }

    return D3D_OK;
}

HRESULT DX9RENDER::SetClipPlane(uint32_t Index, CONST float *pPlane)
{
    // return d3d9->SetClipPlane( Index, pPlane );
    return D3D_OK;
}

HRESULT DX9RENDER::CreateTexture(UINT Width, UINT Height, UINT Levels, uint32_t Usage, D3DFORMAT Format, D3DPOOL Pool,
                                 IDirect3DTexture9 **ppTexture)
{
    return CHECKD3DERR(d3d9->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, NULL));
}

HRESULT DX9RENDER::CreateCubeTexture(UINT EdgeLength, UINT Levels, uint32_t Usage, D3DFORMAT Format, D3DPOOL Pool,
                                     IDirect3DCubeTexture9 **ppCubeTexture)
{
    return CHECKD3DERR(d3d9->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, NULL));
}

HRESULT DX9RENDER::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, IDirect3DSurface9 **ppSurface)
{
    return CHECKD3DERR(d3d9->CreateOffscreenPlainSurface(Width, Height, Format, D3DPOOL_SYSTEMMEM, ppSurface, NULL));
    //~!~D3DERR_OUTOFVIDEOMEMORY
    // GetAvailableTextureMem
}

HRESULT DX9RENDER::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample,
                                             IDirect3DSurface9 **ppSurface)
{
    return CHECKD3DERR(d3d9->CreateDepthStencilSurface(Width, Height, Format, MultiSample, 0, TRUE, ppSurface, NULL));
}

HRESULT DX9RENDER::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9 *pVertexElements,
                                           IDirect3DVertexDeclaration9 **ppDecl)
{
    return CHECKD3DERR(d3d9->CreateVertexDeclaration(pVertexElements, ppDecl));
}

HRESULT DX9RENDER::SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl)
{
    return CHECKD3DERR(d3d9->SetVertexDeclaration(pDecl));
}

HRESULT DX9RENDER::CreateVertexShader(CONST uint32_t *pFunction, IDirect3DVertexShader9 **ppShader)
{
    return CHECKD3DERR(d3d9->CreateVertexShader((const DWORD *)pFunction, ppShader));
}

HRESULT DX9RENDER::CreatePixelShader(CONST uint32_t *pFunction, IDirect3DPixelShader9 **ppShader)
{
    return CHECKD3DERR(d3d9->CreatePixelShader((const DWORD *)pFunction, ppShader));
}

HRESULT DX9RENDER::DeleteVertexShader(IDirect3DVertexShader9 *pShader)
{
    if (pShader)
    {
        return pShader->Release();
    }
    return D3D_OK;
}

HRESULT DX9RENDER::DeletePixelShader(IDirect3DPixelShader9 *pShader)
{
    if (pShader)
    {
        return pShader->Release();
    }
    return D3D_OK;
}

HRESULT DX9RENDER::GetVertexShader(IDirect3DVertexShader9 **ppShader)
{
    return CHECKD3DERR(d3d9->GetVertexShader(ppShader));
}

HRESULT DX9RENDER::GetPixelShader(IDirect3DPixelShader9 **ppShader)
{
    return CHECKD3DERR(d3d9->GetPixelShader(ppShader));
}

#ifdef _WIN32 // Effects
ID3DXEffect *DX9RENDER::GetEffectPointer(const char *techniqueName)
{
    return effects_.getEffectPointer(techniqueName);
}
#endif

HRESULT DX9RENDER::SetTexture(uint32_t Stage, IDirect3DBaseTexture9 *pTexture)
{
    return CHECKD3DERR(d3d9->SetTexture(Stage, pTexture));
}

HRESULT DX9RENDER::GetLevelDesc(IDirect3DTexture9 *ppTexture, UINT Level, D3DSURFACE_DESC *pDesc)
{
    return CHECKD3DERR(ppTexture->GetLevelDesc(Level, pDesc));
}

HRESULT DX9RENDER::GetLevelDesc(IDirect3DCubeTexture9 *ppCubeTexture, UINT Level, D3DSURFACE_DESC *pDesc)
{
    return CHECKD3DERR(ppCubeTexture->GetLevelDesc(Level, pDesc));
}

HRESULT DX9RENDER::LockRect(IDirect3DCubeTexture9 *ppCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level,
                            D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, uint32_t Flags)
{
    return CHECKD3DERR(ppCubeTexture->LockRect(FaceType, Level, pLockedRect, pRect, Flags));
}

HRESULT DX9RENDER::LockRect(IDirect3DTexture9 *ppTexture, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect,
                            uint32_t Flags)
{
    return CHECKD3DERR(ppTexture->LockRect(Level, pLockedRect, pRect, Flags));
}

HRESULT DX9RENDER::UnlockRect(IDirect3DCubeTexture9 *pCubeTexture, D3DCUBEMAP_FACES FaceType, UINT Level)
{
    return CHECKD3DERR(pCubeTexture->UnlockRect(FaceType, Level));
}

HRESULT DX9RENDER::UnlockRect(IDirect3DTexture9 *pTexture, UINT Level)
{
    return CHECKD3DERR(pTexture->UnlockRect(Level));
}

HRESULT DX9RENDER::GetSurfaceLevel(IDirect3DTexture9 *ppTexture, UINT Level, IDirect3DSurface9 **ppSurfaceLevel)
{
    return CHECKD3DERR(ppTexture->GetSurfaceLevel(Level, ppSurfaceLevel));
}

HRESULT DX9RENDER::UpdateSurface(IDirect3DSurface9 *pSourceSurface, CONST RECT *pSourceRectsArray, UINT cRects,
                                 IDirect3DSurface9 *pDestinationSurface, CONST POINT *pDestPointsArray)
{
    return CHECKD3DERR(D3DXLoadSurfaceFromSurface(pDestinationSurface, nullptr, nullptr, pSourceSurface, nullptr,
                                                  nullptr, D3DX_DEFAULT, 0));
    //return CHECKD3DERR(d3d9->UpdateSurface(pSourceSurface, pSourceRectsArray, pDestinationSurface, pDestPointsArray));
}

HRESULT DX9RENDER::StretchRect(IDirect3DSurface9 *pSourceSurface, const RECT *pSourceRect,
                               IDirect3DSurface9 *pDestSurface, const RECT *pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
    return CHECKD3DERR(d3d9->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter));
}

HRESULT DX9RENDER::GetRenderTargetData(IDirect3DSurface9 *pRenderTarget, IDirect3DSurface9 *pDestSurface)
{
    D3DSURFACE_DESC desc;
    if (CHECKD3DERR(pRenderTarget->GetDesc(&desc)))
        return D3DERR_WRONGTEXTUREFORMAT;

    if (desc.MultiSampleType != D3DMULTISAMPLE_NONE)
    {
        IDirect3DSurface9 *pNonsampledSurface = nullptr;

        if(CHECKD3DERR(d3d9->CreateRenderTarget(desc.Width, desc.Height, desc.Format, D3DMULTISAMPLE_NONE, 0, FALSE,
                                             &pNonsampledSurface, nullptr)))
        {
            return D3DERR_WRONGTEXTUREFORMAT;
        }

        if (CHECKD3DERR(pDestSurface->GetDesc(&desc)))
        {
            return D3DERR_WRONGTEXTUREFORMAT;
        }

        if (CHECKD3DERR(d3d9->StretchRect(pRenderTarget, nullptr, pNonsampledSurface, nullptr, D3DTEXF_NONE)))
        {
            return D3DERR_WRONGTEXTUREFORMAT;
        }

        bool error;
        if (desc.Pool == D3DPOOL_SYSTEMMEM || desc.Pool == D3DPOOL_SCRATCH)
        {
            error = CHECKD3DERR(d3d9->GetRenderTargetData(pNonsampledSurface, pDestSurface));
        }
        else
        {
            error = UpdateSurface(pNonsampledSurface, nullptr, 0, pDestSurface, nullptr);
        }

        pNonsampledSurface->Release();
        return error ? D3DERR_WRONGTEXTUREFORMAT : D3D_OK;
    }

    return CHECKD3DERR(d3d9->GetRenderTargetData(pRenderTarget, pDestSurface));
}

HRESULT DX9RENDER::SetVertexShader(IDirect3DVertexShader9 *pShader)
{
    return CHECKD3DERR(d3d9->SetVertexShader(pShader));
}

HRESULT DX9RENDER::SetPixelShader(IDirect3DPixelShader9 *pShader)
{
    return CHECKD3DERR(d3d9->SetPixelShader(pShader));
}

HRESULT DX9RENDER::SetVertexShaderConstantF(UINT StartRegister, CONST float *pConstantData, UINT Vector4iCount)
{
    return CHECKD3DERR(d3d9->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4iCount));
}

HRESULT DX9RENDER::SetPixelShaderConstantF(UINT StartRegister, CONST float *pConstantData, UINT Vector4iCount)
{
    return CHECKD3DERR(d3d9->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4iCount));
}

HRESULT DX9RENDER::GetDeviceCaps(D3DCAPS9 *pCaps)
{
    return CHECKD3DERR(d3d9->GetDeviceCaps(pCaps));
}

CVideoTexture *DX9RENDER::GetVideoTexture(const char *sVideoName)
{
    if (sVideoName == nullptr)
        return nullptr;
    CVideoTexture *retVal = nullptr;
    VideoTextureEntity *pVTLcur = pVTL;

    // check already loaded
    const uint32_t newHash = MakeHashValue(sVideoName);
    while (pVTLcur != nullptr)
    {
        if (pVTLcur->hash == newHash && storm::iEquals(pVTLcur->name, sVideoName))
        {
            if (core.GetEntityPointer(pVTLcur->videoTexture_id))
            {
                pVTLcur->ref++;
                return pVTLcur->VideoTexture;
            }
            VideoTextureEntity *pVTmp = pVTLcur;
            pVTLcur = pVTLcur->next;
            delete pVTmp->name;
            delete pVTmp;
            continue;
        }
        pVTLcur = pVTLcur->next;
    }

    // create new video texture
    pVTLcur = new VideoTextureEntity;
    if (pVTLcur == nullptr)
        throw std::runtime_error("memory allocate error");
    pVTLcur->next = pVTL;
    pVTLcur->VideoTexture = nullptr;
    pVTLcur->hash = newHash;
    pVTLcur->ref = 1;
    const auto len = strlen(sVideoName) + 1;
    if ((pVTLcur->name = new char[len]) == nullptr)
        throw std::runtime_error("memory allocate error");
    strcpy_s(pVTLcur->name, len, sVideoName);
    const entid_t ei = core.CreateEntity("TextureSequence");
    pVTLcur->VideoTexture = static_cast<CVideoTexture *>(core.GetEntityPointer(ei));
    if (pVTLcur->VideoTexture != nullptr)
    {
        pVTLcur->videoTexture_id = ei;
        if (pVTLcur->VideoTexture->Initialize(this, sVideoName, true) == nullptr)
        {
            delete pVTLcur;
            core.EraseEntity(ei);
        }
        else
        {
            pVTL = pVTLcur;
            retVal = pVTLcur->VideoTexture;
        }
    }
    else
    {
        delete pVTLcur;
        core.EraseEntity(ei);
    }

    return retVal;
}

void DX9RENDER::ReleaseVideoTexture(CVideoTexture *pVTexture)
{
    VideoTextureEntity *cur = pVTL;
    VideoTextureEntity *prev = nullptr;

    if (cur != nullptr)
        do
        {
            if (cur->VideoTexture == pVTexture)
            {
                cur->ref--;
                if (cur->ref > 0)
                    return;
                if (prev == nullptr)
                    pVTL = cur->next;
                else
                    prev->next = cur->next;
                core.EraseEntity(cur->videoTexture_id);
                delete cur->name;
                delete cur;
                break;
            }
            prev = cur;
        } while ((cur = cur->next) != nullptr);
}

void DX9RENDER::PlayToTexture()
{
    VideoTextureEntity *cur = pVTL;
    while (cur != nullptr)
    {
        if (core.GetEntityPointer(pVTL->videoTexture_id))
        {
            cur->VideoTexture->FrameUpdate();
            cur = cur->next;
        }
        else
        {
            core.Trace("ERROR: void DX9RENDER::PlayToTexture()");
            delete cur->name;
            VideoTextureEntity *pcur = cur;
            cur = cur->next;
            if (pVTL == pcur)
                pVTL = cur;
            delete pcur;
        }
    }
}

HRESULT DX9RENDER::ImageBlt(int32_t TextureID, RECT *pDstRect, RECT *pSrcRect)
{
    struct F3DVERTEX
    {
        float x, y, z, rhw;
        float tu, tv;
    };
    RECT dr;
    F3DVERTEX v[6];
    HRESULT hRes;

    if (pDstRect)
    {
        dr = *pDstRect;
    }
    else
    {
        dr.left = 0;
        dr.top = 0;
        dr.right = screen_size.x - 1;
        dr.bottom = screen_size.y - 1;
    }

    for (uint32_t n = 0; n < 6; n++)
    {
        v[n].rhw = 1.0f;
        v[n].z = 0.5f;
    }

    v[0].x = static_cast<float>(dr.left);
    v[0].y = static_cast<float>(dr.top);
    v[0].tu = 0.0f;
    v[0].tv = 0.0f;

    v[1].x = static_cast<float>(dr.left);
    v[1].y = static_cast<float>(dr.bottom);
    v[1].tu = 0.0f;
    v[1].tv = 1.0f;

    v[2].x = static_cast<float>(dr.right);
    v[2].y = static_cast<float>(dr.top);
    v[2].tu = 1.0f;
    v[2].tv = 0.0f;

    v[3].x = static_cast<float>(dr.right);
    v[3].y = static_cast<float>(dr.top);
    v[3].tu = 1.0f;
    v[3].tv = 0.0f;

    v[4].x = static_cast<float>(dr.left);
    v[4].y = static_cast<float>(dr.bottom);
    v[4].tu = 0.0f;
    v[4].tv = 1.0f;

    v[5].x = static_cast<float>(dr.right);
    v[5].y = static_cast<float>(dr.bottom);
    v[5].tu = 1.0f;
    v[5].tv = 1.0f;

    TextureSet(0, TextureID);

    const bool bDraw = TechniqueExecuteStart("texturedialogfon");
    if (bDraw)
        do
        {
            SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2);
            hRes = d3d9->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, &v, sizeof(F3DVERTEX));
            dwNumDrawPrimitive++;
        } while (TechniqueExecuteNext());

    return hRes;
}

HRESULT DX9RENDER::ImageBlt(const char *pName, RECT *pDstRect, RECT *pSrcRect)
{
    int32_t TextureID;
    TextureID = TextureCreate(pName);
    const HRESULT hRes = ImageBlt(TextureID, pDstRect, pSrcRect);
    TextureRelease(TextureID);
    SetProgressImage(pName);
    return hRes;
}

void DX9RENDER::SetProgressImage(const char *image)
{
    if (!image || !image[0])
    {
        if (progressImageSize > 0 && progressImage)
            progressImage[0] = 0;
        return;
    }
    const int32_t s = strlen(image) + 1;
    if (s > progressImageSize)
    {
        progressImageSize = s;
        delete progressImage;
        progressImage = new char[progressImageSize];
    }
    strcpy_s(progressImage, s, image);
}

void DX9RENDER::SetProgressBackImage(const char *image)
{
    if (!image || !image[0])
    {
        if (progressBackImageSize > 0 && progressBackImage)
            progressBackImage[0] = 0;
        return;
    }
    const int32_t s = strlen(image) + 1;
    if (s > progressBackImageSize)
    {
        progressBackImageSize = s;
        delete progressBackImage;
        progressBackImage = new char[progressBackImageSize];
    }
    strcpy_s(progressBackImage, s, image);
}

void DX9RENDER::SetTipsImage(const char *image)
{
    if (!image || !image[0])
    {
        if (progressTipsImageSize > 0 && progressTipsImage)
            progressTipsImage[0] = 0;
        return;
    }
    const int32_t s = strlen(image) + 1;
    if (s > progressTipsImageSize)
    {
        progressTipsImageSize = s;
        delete progressTipsImage;
        progressTipsImage = new char[progressTipsImageSize];
    }
    memcpy(progressTipsImage, image, s);
}

char *DX9RENDER::GetTipsImage()
{
    return progressTipsImage;
}

void DX9RENDER::StartProgressView()
{
    progressSafeCounter = 0;
    if (progressTexture < 0)
    {
        // Loading the texture
        loadFrame = 0;
        isInPViewProcess = true;
        const int32_t t = TextureCreate("Loading\\progress.tga");
        isInPViewProcess = false;
        if (t < 0)
        {
            core.Trace("Progress error!");
            return;
        }
        progressTexture = t;
    }
    else
        return;
    // Loading an unscaled background image
    if (progressBackImage && progressBackImage[0])
    {
        isInPViewProcess = true;
        if (back0Texture >= 0)
            TextureRelease(back0Texture);
        back0Texture = -1;
        back0Texture = TextureCreate(progressBackImage);
        isInPViewProcess = false;
    }
    else
    {
        back0Texture = -1;
    }
    // Loading a scalable background image
    if (progressImage && progressImage[0])
    {
        isInPViewProcess = true;
        if (backTexture >= 0)
            TextureRelease(backTexture);
        backTexture = -1;
        backTexture = TextureCreate(progressImage);
        isInPViewProcess = false;
    }
    else
    {
        backTexture = -1;
    }
    // Loading tips
    if (progressTipsImage && progressTipsImage[0])
    {
        isInPViewProcess = true;
        if (progressTipsTexture >= 0)
            TextureRelease(progressTipsTexture);
        progressTipsTexture = -1;
        progressTipsTexture = TextureCreate(progressTipsImage);
        isInPViewProcess = false;
    }
    progressUpdateTime = SDL_GetTicks() - 1000;
}

void DX9RENDER::ProgressView()
{
    // get the texture
    if (progressTexture < 0)
        return;
    if (isInPViewProcess)
        return;
    // Analyzing time
    const uint32_t time = SDL_GetTicks();
    if (abs(static_cast<int32_t>(progressUpdateTime - time)) < 50)
        return;
    progressUpdateTime = time;
    isInPViewProcess = true;
    progressSafeCounter = 0;
    // Drawing mode
    BeginScene();
    // Filling the vertices of the texture
    struct LoadVertex
    {
        float x, y, z, rhw;
        uint32_t color;
        float u, v;
    } v[4];
    uint32_t i;
    for (i = 0; i < 4; i++)
    {
        v[i].z = 0.5;
        v[i].rhw = 2.0;
        v[i].color = 0xffffffff;
    }
    // Calculate the rectangle in which to draw
    D3DVIEWPORT9 vp;
    GetViewport(&vp);

    v[0].x = 0.0f;
    v[0].y = 0.0f;
    v[1].x = float(vp.Width);
    v[1].y = 0.0f;
    v[2].x = 0.0f;
    v[2].y = float(vp.Height);
    v[3].x = float(vp.Width);
    v[3].y = float(vp.Height);
    v[0].u = 0.0f;
    v[0].v = 0.0f;
    v[1].u = 1.0f;
    v[1].v = 0.0f;
    v[2].u = 0.0f;
    v[2].v = 1.0f;
    v[3].u = 1.0f;
    v[3].v = 1.0f;
    TextureSet(0, back0Texture);
    if (back0Texture < 0)
        for (i = 0; i < 4; i++)
            v[i].color = 0;
    if (back0Texture >= 0)
    {
        DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, 2, v, sizeof(v[0]),
                        "ProgressBackTech");
    }

    float dy = 0.0f;
    float dx = ((float(vp.Width) - (4.0f * float(vp.Height) / 3.0f)) / 2.0f);
    if (dx < 10.0f)
        dx = 0.0f;
    else
    {
        dy = 25.0f;
        dx = ((float(vp.Width) - (4.0f * (float(vp.Height) - 2.0f * dy) / 3.0f)) / 2.0f);
    }

    v[0].x = 0.0f + dx;
    v[0].y = 0.0f + dy;
    v[1].x = float(vp.Width) - dx;
    v[1].y = 0.0f + dy;
    v[2].x = 0.0f + dx;
    v[2].y = float(vp.Height) - dy;
    v[3].x = float(vp.Width) - dx;
    v[3].y = float(vp.Height) - dy;
    v[0].u = 0.0f;
    v[0].v = 0.0f;
    v[1].u = 1.0f;
    v[1].v = 0.0f;
    v[2].u = 0.0f;
    v[2].v = 1.0f;
    v[3].u = 1.0f;
    v[3].v = 1.0f;

    TextureSet(0, backTexture);
    if (backTexture < 0)
    {
        for (i = 0; i < 4; i++)
            v[i].color = 0;
    }
    else
    {
        if (progressTipsTexture >= 0)
        {
            TextureSet(1, progressTipsTexture);
            DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, 2, v, sizeof(v[0]),
                            "ProgressBackTechWithTips");
        }
        else
        {
            DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, 2, v, sizeof(v[0]),
                            "ProgressBackTech");
        }
    }
    if (backTexture < 0)
        for (i = 0; i < 4; i++)
            v[i].color = 0xffffffff;
    // Animated object
    m_fHeightDeformator = ((float)vp.Height * 4.0f) / ((float)vp.Width * 3.0f);
    // core.Trace(" size_x %f", (vp.Width - dx * 2.0f)*progressFramesWidth);
    CVECTOR pos((vp.Width - dx * 2.0f) * progressFramesPosX + dx, (vp.Height - dy * 2.0f) * progressFramesPosY + dy,
                0.0f);
    CVECTOR size((vp.Width - dx * 2.0f) * progressFramesWidth,
                 (vp.Height - dy * 2.0f) * progressFramesHeight * 4.0f / 3.0f, 0.0f);
    v[0].x = pos.x;
    v[0].y = pos.y;
    v[1].x = pos.x + size.x + 0.5f;
    v[1].y = pos.y;
    v[2].x = pos.x;
    v[2].y = pos.y + size.y + 0.5f;
    v[3].x = pos.x + size.x + 0.5f;
    v[3].y = pos.y + size.y + 0.5f;
    v[3].y = pos.y + size.y + 0.5f;
    // Frame grid size
    int32_t sizeX = progressFramesCountX;
    int32_t sizeY = progressFramesCountY;
    // Position of the current frame
    int32_t fx = loadFrame % sizeX;
    int32_t fy = loadFrame / sizeY;
    v[0].u = fx / float(sizeX);
    v[0].v = fy / float(sizeY);
    v[1].u = (fx + 1.0f) / float(sizeX);
    v[1].v = fy / float(sizeY);
    v[2].u = fx / float(sizeX);
    v[2].v = (fy + 1.0f) / float(sizeY);
    v[3].u = (fx + 1.0f) / float(sizeX);
    v[3].v = (fy + 1.0f) / float(sizeY);
    // Draw
    TextureSet(0, progressTexture);
    DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, 2, v, sizeof(v[0]),
                    "ProgressTech");
    EndScene();
    d3d9->Present(nullptr, nullptr, nullptr, nullptr);
    BeginScene();
    // Next frame
    loadFrame++;
    if (loadFrame >= sizeX * sizeY)
        loadFrame = 0;
    // leave
    isInPViewProcess = false;
}

void DX9RENDER::EndProgressView()
{
    if (progressTexture >= 0)
        TextureRelease(progressTexture);
    progressTexture = -1;
    if (backTexture >= 0)
        TextureRelease(backTexture);
    backTexture = -1;
    if (back0Texture >= 0)
        TextureRelease(back0Texture);
    back0Texture = -1;
    if (progressTipsTexture >= 0)
        TextureRelease(progressTipsTexture);
    progressTipsTexture = -1;
    if (progressImage && progressImageSize > 0)
        progressImage[0] = 0;
    if (progressBackImage && progressBackImageSize > 0)
        progressBackImage[0] = 0;
    if (progressTipsImage && progressTipsImageSize > 0)
        progressTipsImage[0] = 0;
}

void DX9RENDER::SetColorParameters(float fGamma, float fBrightness, float fContrast)
{
    uint16_t rgb[256];
    for (uint32_t i = 0; i < 256; i++)
    {
        float fRamp = std::clamp(fContrast * 255.0f * 256.0f * powf(static_cast<float>(i / 255.0f), 1.0f / fGamma) +
                                     fBrightness * 256.0f,
                                 0.0f, 65535.0f);
        rgb[i] = static_cast<uint16_t>(fRamp);
    }
    core.GetWindow()->SetGamma(rgb, rgb, rgb);
}

void DX9RENDER::MakeDrawVector(RS_LINE *pLines, uint32_t dwNumSubLines, const CMatrix &mMatrix, CVECTOR vUp, CVECTOR v1,
                               CVECTOR v2, float fScale, uint32_t dwColor)
{
    uint32_t i;
    uint32_t k;

    // for (i=0; i<dwNumSubLines * 2 + 2; i++) pLines[i].dwColor = dwColor;
    k = dwNumSubLines * 2 + 2; // boal optimization if the for loop runs the calculations every iteration
    for (i = 0; i < k; i++)
    {
        pLines[i].dwColor = dwColor;
    }
    pLines[0].vPos = v1;
    pLines[1].vPos = v1 + (fScale * v2);

    const float fRadius = 0.03f * fScale;
    const float fDist = 0.85f * fScale * sqrtf(~v2);

    for (i = 0; i < dwNumSubLines; i++)
    {
        const float fAng = PIm2 * static_cast<float>(i) / static_cast<float>(dwNumSubLines);

        const float x = fRadius * sinf(fAng);
        const float z = fRadius * cosf(fAng);

        CVECTOR vRes;

        if (fabsf(vUp.x) < 1e-5f)
            vRes = CVECTOR(fDist, x, z);
        if (fabsf(vUp.y) < 1e-5f)
            vRes = CVECTOR(x, fDist, z);
        if (fabsf(vUp.z) < 1e-5f)
            vRes = CVECTOR(x, z, fDist);
        vRes = (CMatrix &)mMatrix * vRes;
        pLines[2 + i * 2 + 0].vPos = vRes;
        pLines[2 + i * 2 + 1].vPos = pLines[1].vPos;
    }
}

void DX9RENDER::DrawVector(const CVECTOR &v1, const CVECTOR &v2, uint32_t dwColor, const char *pTechniqueName)
{
    RS_LINE lines[51 * 2];
    CMatrix mView;

    const float fScale = sqrtf(~(v2 - v1));
    if (!mView.BuildViewMatrix(v1, v2, CVECTOR(0.0f, 1.0f, 0.0f)))
        if (!mView.BuildViewMatrix(v1, v2, CVECTOR(1.0f, 0.0f, 0.0f)))
            return;

    mView.Transposition();

    MakeDrawVector(&lines[0], 50, mView, CVECTOR(1.0f, 1.0f, 0.0f), mView.Pos(), mView.Vz(), fScale, dwColor);

    const CMatrix mWorldSave;
    GetTransform(D3DTS_WORLD, mWorldSave);
    SetTransform(D3DTS_WORLD, CMatrix());
    DrawLines(lines, 51, pTechniqueName);
    SetTransform(D3DTS_WORLD, mWorldSave);
}

void DX9RENDER::DrawSphere(const CVECTOR &vPos, float fRadius, uint32_t dwColor)
{
    CMatrix m;
    m.BuildPosition(vPos.x, vPos.y, vPos.z);
    m.m[0][0] = fRadius;
    m.m[1][1] = fRadius;
    m.m[2][2] = fRadius;

    SetRenderState(D3DRS_TEXTUREFACTOR, dwColor);
    SetTransform(D3DTS_WORLD, m);
    DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, DX9sphereNumTrgs, DX9sphereVertex,
                    sizeof(DX9SphVertex), "DXSphere");
}


void DX9RENDER::DrawEllipsoid(const CVECTOR &vPos, float a, float b, float c, float ay, uint32_t dwColor)
{
    CMatrix trans, scale, rot;
    trans.BuildPosition(vPos.x, vPos.y, vPos.z);
    scale.BuildScale(a, b, c);
    rot.BuildRotateY(ay);

    SetTransform(D3DTS_WORLD, scale * rot * trans);
    SetRenderState(D3DRS_TEXTUREFACTOR, dwColor);
    DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, DX9sphereNumTrgs, DX9sphereVertex,
                    sizeof(DX9SphVertex), "DXEllipsoid");
}

void DX9RENDER::SetLoadTextureEnable(bool bEnable)
{
    bLoadTextureEnabled = bEnable;
}

IDirect3DVolumeTexture9 *DX9RENDER::CreateVolumeTexture(uint32_t Width, uint32_t Height, uint32_t Depth,
                                                        uint32_t Levels, uint32_t Usage, D3DFORMAT Format, D3DPOOL Pool)
{
    IDirect3DVolumeTexture9 *pVolumeTexture = nullptr;
    CHECKD3DERR(d3d9->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, &pVolumeTexture, NULL));
    return pVolumeTexture;
}

bool DX9RENDER::PushRenderTarget()
{
    RenderTarget renderTarget{};
    GetRenderTarget(&renderTarget.pRenderTarget);
    CHECKD3DERR(d3d9->GetDepthStencilSurface(&renderTarget.pDepthSurface));
    GetViewport(&renderTarget.ViewPort);
    stRenderTarget.push(renderTarget);

    return true;
}

bool DX9RENDER::PopRenderTarget()
{
    if (stRenderTarget.empty())
    {
        core.Trace("DX9Error: Try to pop RenderTarget, but RenderTarget stack is empty");
        return false;
    }

    auto top = stRenderTarget.top();
    SetRenderTarget(top.pRenderTarget, top.pDepthSurface);
    SetViewport(&top.ViewPort);
    RELEASE(top.pRenderTarget);
    RELEASE(top.pDepthSurface);
    stRenderTarget.pop();

    return true;
}

bool DX9RENDER::SetRenderTarget(IDirect3DCubeTexture9 *pRenderTarget, uint32_t FaceType, uint32_t dwLevel,
                                IDirect3DSurface9 *pZStencil)
{
    IDirect3DSurface9 *pSurface;
    return !CHECKD3DERR(pRenderTarget->GetCubeMapSurface(static_cast<D3DCUBEMAP_FACES>(FaceType), dwLevel, &pSurface)) &&
           !CHECKD3DERR(SetRenderTarget(pSurface, pZStencil)) && Release(pSurface) == D3D_OK;
}

void DX9RENDER::SetView(const CMatrix &mView)
{
    SetTransform(D3DTS_VIEW, mView);
}

void DX9RENDER::SetWorld(const CMatrix &mWorld)
{
    SetTransform(D3DTS_WORLD, mWorld);
}

void DX9RENDER::SetProjection(const CMatrix &mProjection)
{
    SetTransform(D3DTS_PROJECTION, mProjection);
}

const CMatrix &DX9RENDER::GetView()
{
    GetTransform(D3DTS_VIEW, mView);
    return mView;
}

const CMatrix &DX9RENDER::GetWorld()
{
    GetTransform(D3DTS_WORLD, mWorld);
    return mWorld;
}

const CMatrix &DX9RENDER::GetProjection()
{
    GetTransform(D3DTS_PROJECTION, mProjection);
    return mProjection;
}

IDirect3DBaseTexture9 *DX9RENDER::GetTextureFromID(int32_t nTextureID)
{
    if (nTextureID < 0)
        return nullptr;
    return Textures[nTextureID].d3dtex;
}

bool DX9RENDER::GetRenderTargetAsTexture(IDirect3DTexture9 **tex)
{
    Release(*tex);
    IDirect3DSurface9 *renderTarget;
    D3DSURFACE_DESC desc;

    auto success = false;

    if (GetRenderTarget(&renderTarget) == D3D_OK && renderTarget->GetDesc(&desc) == D3D_OK)
    {
        if (CreateTexture(desc.Width, desc.Height, 0, 0, desc.Format, D3DPOOL_DEFAULT, tex) == D3D_OK)
        {
            IDirect3DSurface9 *pTexSurface;
            if ((*tex)->GetSurfaceLevel(0, &pTexSurface) == D3D_OK)
            {
                if (GetRenderTargetData(renderTarget, pTexSurface) == D3D_OK)
                {
                    success = true;
                }
                pTexSurface->Release();
            }
        }
    }

    return success;
}

