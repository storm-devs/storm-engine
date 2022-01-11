#include <thread>

#include "lifecycle_diagnostics_service.hpp"
#include "logging.hpp"

#include "steam_api_impl.hpp"
#include "compiler.h"
#include "file_service.h"
#include "s_debug.h"
#include "v_sound_service.h"
#include "storm/fs.h"
#include "watermark.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <os_window.hpp>
#include <SDL2/SDL.h>

VFILE_SERVICE *fio = nullptr;
S_DEBUG *CDebug = nullptr;
Core &core = core_internal;

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

} // namespace

void HandleWindowEvent(const storm::OSWindow::Event &event)
{
    if (event == storm::OSWindow::Closed)
    {
        isRunning = false;
        core_internal.Event("DestroyWindow", nullptr);
    }
    else if (event == storm::OSWindow::FocusGained)
    {
        bActive = true;
        core_internal.AppState(bActive);
        if (const auto soundService = static_cast<VSoundService*>(core.GetService("SoundService")))
        {
            soundService->SetActiveWithFade(true);
        }
    }
    else if (event == storm::OSWindow::FocusLost)
    {
        bActive = false;
        core_internal.AppState(bActive);
        if (const auto soundService = static_cast<VSoundService *>(core.GetService("SoundService")))
        {
            soundService->SetActiveWithFade(false);
        }
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    // Prevent multiple instances
    if (!CreateEventA(nullptr, false, false, "Global\\FBBD2286-A9F1-4303-B60C-743C3D7AA7BE") ||
        GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBoxA(nullptr, "Another instance is already running!", "Error", MB_ICONERROR);
        return EXIT_SUCCESS;
    }

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

    std::shared_ptr<storm::OSWindow> window = storm::OSWindow::Create(width, height, fullscreen);
    window->SetTitle("Sea Dogs");
    core_internal.Set_Hwnd(static_cast<HWND>(window->OSHandle()));
    window->Subscribe(HandleWindowEvent);
    window->Show();

    // Init core
    core_internal.InitBase();

    // Message loop
    auto dwOldTime = std::chrono::system_clock::now();

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
                const auto dwNewTime = std::chrono::system_clock::now();
                const std::chrono::duration<double, std::milli> passedTime = dwNewTime - dwOldTime;
                if (static_cast<uint32_t>(passedTime.count()) < dwMS)
                    continue;
                dwOldTime = dwNewTime;
            }

            RunFrameWithOverflowCheck();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    // Release
    core_internal.Event("ExitApplication", nullptr);
    core_internal.CleanUp();
    core_internal.ReleaseBase();
    ClipCursor(nullptr);
    SDL_Quit();

    return EXIT_SUCCESS;
}
