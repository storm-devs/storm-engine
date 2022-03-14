#include <fstream>
#include <thread>

#include <SDL2/SDL.h>
#include <mimalloc-new-delete.h>
#include <mimalloc.h>
#include <spdlog/spdlog.h>

#include "lifecycle_diagnostics_service.hpp"
#include "logging.hpp"
#include "compiler.h"
#include "os_window.hpp"
#include "steam_api_impl.hpp"
#include "file_service.h"
#include "s_debug.h"
#include "v_sound_service.h"
#include "storm/fs.h"
#include "watermark.hpp"

#include "bx/bx.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "sprite_renderer.h"
#include "common.h"
#include "dx9render.h"

VFILE_SERVICE *fio = nullptr;
S_DEBUG *CDebug = nullptr;
Core &core = core_internal;
VDX9RENDER *renderService;

namespace
{

constexpr char defaultLoggerName[] = "system";
bool isRunning = false;
bool bActive = true;

storm::diag::LifecycleDiagnosticsService lifecycleDiagnostics;

void RunFrame()
{
    if (!core_internal.Run())
    {
        isRunning = false;
    }

    lifecycleDiagnostics.notifyAfterRun();
}

#ifdef _WIN32
void RunFrameWithOverflowCheck()
{
    __try
    {
        RunFrame();
    }
    __except ([](unsigned code, struct _EXCEPTION_POINTERS *ep) {
        return code == EXCEPTION_STACK_OVERFLOW;
    }(GetExceptionCode(), GetExceptionInformation()))
    {
        _resetstkoflw();
        throw std::runtime_error("Stack overflow");
    }
}
#else
#define RunFrameWithOverflowCheck RunFrame
#endif


void mimalloc_fun(const char *msg, void *arg)
{
    static std::filesystem::path mimalloc_log_path;
    if (mimalloc_log_path.empty())
    {
        mimalloc_log_path = fs::GetLogsPath() / "mimalloc.log";
        std::error_code ec;
        remove(mimalloc_log_path, ec);
    }

    FILE *mimalloc_log =
#ifdef _MSC_VER
        _wfopen(mimalloc_log_path.c_str(), L"a+b");
#else
        fopen(mimalloc_log_path.c_str(), "a+b");
#endif
    if (mimalloc_log != nullptr)
    {
        fputs(msg, mimalloc_log);
        fclose(mimalloc_log);
    }
}

} // namespace

void HandleWindowEvent(const storm::OSWindow::Event &event)
{
    if (event == storm::OSWindow::Closed)
    {
        isRunning = false;
        if (core_internal.initialized())
        {
            core_internal.Event("DestroyWindow");
        }
    }
    else if (event == storm::OSWindow::FocusGained)
    {
        bActive = true;
        if (core_internal.initialized())
        {
            core_internal.AppState(bActive);
            if (const auto soundService = static_cast<VSoundService *>(core.GetService("SoundService")))
            {
                soundService->SetActiveWithFade(true);
            }
        }
    }
    else if (event == storm::OSWindow::FocusLost)
    {
        bActive = false;
        if (core_internal.initialized())
        {
            core_internal.AppState(bActive);
            if (const auto soundService = static_cast<VSoundService *>(core.GetService("SoundService")))
            {
                soundService->SetActiveWithFade(false);
            }
        }
    }
}

//int main(int argc, char **argv)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    // Prevent multiple instances
    if (!CreateEventA(nullptr, false, false, "Global\\FBBD2286-A9F1-4303-B60C-743C3D7AA7BE") ||
        GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBoxA(nullptr, "Another instance is already running!", "Error", MB_ICONERROR);
        return EXIT_SUCCESS;
    }
    mi_register_output(mimalloc_fun, nullptr);
    mi_option_set(mi_option_show_errors, 1);
    mi_option_set(mi_option_show_stats, 1);
    mi_option_set(mi_option_eager_commit, 1);
    mi_option_set(mi_option_eager_region_commit, 1);
    mi_option_set(mi_option_large_os_pages, 1);
    mi_option_set(mi_option_page_reset, 0);
    mi_option_set(mi_option_segment_reset, 0);
    mi_option_set(mi_option_reserve_huge_os_pages, 1);
    mi_option_set(mi_option_segment_cache, 16);
#ifdef _DEBUG
    mi_option_set(mi_option_verbose, 4);
#endif

    SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

    // Init FS
    FILE_SERVICE File_Service;
    fio = &File_Service;

    // Init diagnostics
    const auto lifecycleDiagnosticsGuard =
#ifdef STORM_ENABLE_CRASH_REPORTS
        lifecycleDiagnostics.initialize(true);
#else
        lifecycleDiagnostics.initialize(false);
#endif
    if (!lifecycleDiagnosticsGuard)
    {
        MessageBoxA(nullptr, "Unable to initialize lifecycle service!", "Warning", MB_ICONWARNING);
    }
    else
    {
        lifecycleDiagnostics.setCrashInfoCollector([]() { core_internal.collectCrashInfo(); });
    }

    // Init stash
    create_directories(fs::GetSaveDataPath());

    // Init logging
    spdlog::set_default_logger(storm::logging::getOrCreateLogger(defaultLoggerName));
    spdlog::info("Logging system initialized. Running on {}", STORM_BUILD_WATERMARK_STRING);
    spdlog::info("mimalloc-redirect status: {}", mi_is_redirected());

    // Init core
    core_internal.Init();

    // Init script debugger
    S_DEBUG debug;
    debug.Init();
    CDebug = &debug;

    // Read config
    auto ini = fio->OpenIniFile(fs::ENGINE_INI_FILE_NAME);

    uint32_t dwMaxFPS = 0;
    bool bSteam = false;
    int width = 1024, height = 768;
    bool fullscreen = false;

    if (ini)
    {
        dwMaxFPS = static_cast<uint32_t>(ini->GetInt(nullptr, "max_fps", 0));
        auto bDebugWindow = ini->GetInt(nullptr, "DebugWindow", 0) == 1;
        auto bAcceleration = ini->GetInt(nullptr, "Acceleration", 0) == 1;
        if (ini->GetInt(nullptr, "logs", 1) == 0) // disable logging
        {
            spdlog::set_level(spdlog::level::off);
        }
        width = ini->GetInt(nullptr, "screen_x", 1024);
        height = ini->GetInt(nullptr, "screen_y", 768);
        fullscreen = ini->GetInt(nullptr, "full_screen", false) ? true : false;
        bSteam = ini->GetInt(nullptr, "Steam", 1) != 0;
    }

    // initialize SteamApi through evaluating its singleton
    try
    {
        steamapi::SteamApi::getInstance(!bSteam);
    }
    catch (const std::exception &e)
    {
        spdlog::critical(e.what());
        return EXIT_FAILURE;
    }

    // Collect information about the window from SDL
    std::shared_ptr<storm::OSWindow> window = storm::OSWindow::Create(width, height, fullscreen);
    window->SetTitle("Sea Dogs");
    core_internal.Set_Hwnd(static_cast<HWND>(window->OSHandle()));
    window->Subscribe(HandleWindowEvent);
    window->Show();

    // and give the pointer to the window to pd
    bgfx::PlatformData pd;
    // TODO for platforms this goes inside OSWindow->OSHandle()
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    pd.ndt = wmi.info.x11.display;
    pd.nwh = (void *)(uintptr_t)wmi.info.x11.window;
#elif BX_PLATFORM_OSX
    pd.ndt = NULL;
    pd.nwh = wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
    pd.ndt = NULL;
    pd.nwh = static_cast<HWND>(window->OSHandle());
#elif BX_PLATFORM_STEAMLINK
    pd.ndt = wmi.info.vivante.display;
    pd.nwh = wmi.info.vivante.window;
#endif // BX_PLATFORM_
    pd.context = NULL;
    pd.backBuffer = NULL;
    pd.backBufferDS = NULL;

    // Tell bgfx about the platform and window
    bgfx::setPlatformData(pd);

    // Render an empty frame
    bgfx::renderFrame();

    // Init bgfx
    bgfx::Init init;
    init.type = bgfx::RendererType::Direct3D11;
    init.vendorId = BGFX_PCI_ID_NONE;
    init.resolution.width = width;
    init.resolution.height = height;
    init.resolution.reset = BGFX_RESET_NONE;

    bgfx::init(init);

    const bgfx::Caps *caps = bgfx::getCaps();
    // bool swapChainSupported = 0 != (caps->supported & BGFX_CAPS_SWAP_CHAIN);

    // Enable m_debug text.
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    // Set view 0 clear state.
    // bgfx::setViewClear(0, BGFX_CLEAR_NONE, 0x303030ff, 1.0f, 0);

    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
    bgfx::setViewRect(1, 0, 0, uint16_t(width), uint16_t(height));
    // bgfx::setViewRect(2, 0, 0, uint16_t(width), uint16_t(height));

    // Init core
    core_internal.InitBase();

    // Message loop
    auto dwOldTime = GetTickCount();

    isRunning = true;
    while (isRunning)
    {
        SDL_PumpEvents();
        SDL_FlushEvents(0, SDL_LASTEVENT);

        if (bActive)
        {
            if (dwMaxFPS)
            {
                const auto dwMS = 1000u / dwMaxFPS;
                const auto dwNewTime = GetTickCount();
                if (dwNewTime - dwOldTime < dwMS)
                    continue;
                dwOldTime = dwNewTime;
            }

            RunFrameWithOverflowCheck();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        if (core.Controls->GetDebugAsyncKeyState(VK_F1) && core.Controls->GetDebugAsyncKeyState(VK_SHIFT))
        {
            mi_stats_print_out(mimalloc_fun, nullptr);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    // Release
    core_internal.Event("ExitApplication");
    core_internal.CleanUp();
    core_internal.ReleaseBase();
    ClipCursor(nullptr);
    SDL_Quit();

    return EXIT_SUCCESS;
}
