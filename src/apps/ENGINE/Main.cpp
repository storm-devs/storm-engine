#include "ExecutableLifecycleService.hpp"
#include "SteamApi.hpp"
#include "compiler.h"
#include "file_service.h"
#include "fs.h"
#include "s_debug.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <OSWindow.hpp>
#include <SDL.h>

#include "bx/bx.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

#include "sprite_renderer.h"

#include "common.h"

#include "dx9render.h"
#include <SDL2/SDL_syswm.h>


VFILE_SERVICE *fio = nullptr;
CORE core;
S_DEBUG CDebug;

VDX9RENDER *renderService;

std::shared_ptr<TextureResource> texture;
std::shared_ptr<TextureResource> texture2;

long PortraitID;


using namespace storm;

namespace
{

bool isHold = false;
bool isRunning = false;
bool bActive = true;

} // namespace

void HandleWindowEvent(const OSWindow::Event &event)
{
    if (event == OSWindow::Closed)
    {
        isRunning = false;
        core.Event("DestroyWindow", nullptr);
    }
    else if (event == OSWindow::FocusGained)
    {
        bActive = true;
        core.AppState(bActive);
    }
    else if (event == OSWindow::FocusLost)
    {
        bActive = false;
        core.AppState(bActive);
    }
}


int main(int argc, char **argv)
{
    // Prevent multiple instances
    if (!CreateEventA(nullptr, false, false, "Global\\FBBD2286-A9F1-4303-B60C-743C3D7AA7BE") ||
        GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBoxA(nullptr, "Another instance is already running!", "Error", MB_ICONERROR);
        return 0;
    }

    SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

    // Init FS
    FILE_SERVICE File_Service;
    fio = &File_Service;

    // Init stash
    create_directories(fs::GetLogsPath());
    create_directories(fs::GetSaveDataPath());

    // Init logging
    auto systemLogPath = fs::GetLogsPath() / u8"system.log";
    auto systemLog = spdlog::basic_logger_st("system", systemLogPath.string(), true);
    systemLog->flush_on(spdlog::level::critical);
    set_default_logger(systemLog);

    auto compileLogPath = fs::GetLogsPath() / COMPILER_LOG_FILENAME;
    auto compileLog = spdlog::basic_logger_st("compile", compileLogPath.string(), true);

    auto errorLogPath = fs::GetLogsPath() / COMPILER_ERRORLOG_FILENAME;
    auto errorLog = spdlog::basic_logger_st("error", errorLogPath.string(), true);

    std::set_terminate(spdlog::shutdown);
    std::atexit(spdlog::shutdown);

    // Init executable lifecycle manager
    storm::except::ExecutableLifecycleService lifecycleService;
    lifecycleService.addAttachment(systemLogPath);
    lifecycleService.addAttachment(compileLogPath);
    lifecycleService.addAttachment(errorLogPath);
    if (!lifecycleService.initialize(true))
    {
        spdlog::error("Unable to initialize crash handler");
    }

    // Init core
    core.Init();
    core.tracelog = systemLog;
    core.Compiler->tracelog = compileLog;
    core.Compiler->errorlog = errorLog;
    core.Compiler->warninglog = errorLog;

    // Init script debugger
    CDebug.Init();

    /* Read config */
    uint32_t dwMaxFPS = 0;
    auto ini = fio->OpenIniFile(fs::ENGINE_INI_FILE_NAME);
    bool bSteam = false;
    int width = 1024, height = 768;
    bool fullscreen = false;

    if (ini)
    {
        dwMaxFPS = static_cast<uint32_t>(ini->GetLong(nullptr, "max_fps", 0));
        auto bDebugWindow = ini->GetLong(nullptr, "DebugWindow", 0) == 1;
        auto bAcceleration = ini->GetLong(nullptr, "Acceleration", 0) == 1;
        if (!ini->GetLong(nullptr, "logs", 0))
        {
            core.tracelog->set_level(spdlog::level::off);
        }
        if (ini->GetLong(nullptr, "Steam", 1) != 0)
        {
            bSteam = true;
        }
        else
        {
            bSteam = false;
        }
        width = ini->GetLong(nullptr, "screen_x", 1024);
        height = ini->GetLong(nullptr, "screen_y", 768);
        fullscreen = ini->GetLong(nullptr, "full_screen", false) ? true : false;
    }

    // evaluate SteamApi singleton
    steamapi::SteamApi::getInstance(!bSteam);

    // Collect information about the window from SDL
    std::shared_ptr<storm::OSWindow> window = storm::OSWindow::Create(width, height, fullscreen);
    window->SetTitle("Sea Dogs");
    core.Set_Hwnd(static_cast<HWND>(window->OSHandle()));
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
    init.type = bgfx::RendererType::Direct3D9;
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
    //bgfx::setViewRect(1, 0, 0, uint16_t(width), uint16_t(height));
    //bgfx::setViewRect(2, 0, 0, uint16_t(width), uint16_t(height));

    
    /* Init stuff */
    core.InitBase();

    core.Run(); // to allow service initialization, remove/comment after tests

    renderService = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));

    /* Message loop */
    auto dwOldTime = GetTickCount64();

    isRunning = renderService != nullptr;
    while (isRunning)
    {
        SDL_PumpEvents();
        SDL_FlushEvents(0, SDL_LASTEVENT);

        if (bActive)
        {
            if (dwMaxFPS)
            {
                const auto dwMS = 1000u / dwMaxFPS;
                const auto dwNewTime = GetTickCount64();
                if (dwNewTime - dwOldTime < dwMS)
                    continue;
                dwOldTime = dwNewTime;
            }

            const auto runResult = core.Run();

            if (!isHold && !runResult)
            {
                isHold = true;
                isRunning = false;
            }
        }
        else
        {
            Sleep(50);
        }
    }
    core.Event("ExitApplication", nullptr);
    CDebug.Release();
    core.CleanUp();
    CDebug.CloseDebugWindow();

    /* Release */
    core.ReleaseBase();
    ClipCursor(nullptr);

    // Shutdown bgfx.
    bgfx::shutdown();


    SDL_Quit();

    return 0;
}