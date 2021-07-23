#include "LifecycleDiagnosticsService.hpp"
#include "logging.hpp"

#include "SteamApi.hpp"
#include "compiler.h"
#include "file_service.h"
#include "s_debug.h"
#include "storm/fs.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <OSWindow.hpp>
#include <SDL.h>

VFILE_SERVICE *fio = nullptr;
CORE core;
S_DEBUG CDebug;

namespace
{

constexpr char defaultLoggerName[] = "system";
bool isHold = false;
bool isRunning = false;
bool bActive = true;

storm::diag::LifecycleDiagnosticsService lifecycleDiagnostics;

} // namespace

void HandleWindowEvent(const storm::OSWindow::Event &event)
{
    if (event == storm::OSWindow::Closed)
    {
        isRunning = false;
        core.Event("DestroyWindow", nullptr);
    }
    else if (event == storm::OSWindow::FocusGained)
    {
        bActive = true;
        core.AppState(bActive);
    }
    else if (event == storm::OSWindow::FocusLost)
    {
        bActive = false;
        core.AppState(bActive);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
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

    // Init diagnostics
    const auto lifecycleDiagnosticsGuard =
#ifdef STORM_ENABLE_CRASH_REPORTS
        lifecycleDiagnostics.initialize(true);
#else
        lifecycleDiagnostics.initialize(false);
#endif
    if (!lifecycleDiagnosticsGuard)
    {
        spdlog::error("Unable to initialize lifecycle service");
    }

    // Init stash
    create_directories(fs::GetSaveDataPath());

    // Init logging
    spdlog::set_default_logger(storm::logging::getOrCreateLogger(defaultLoggerName));

    // Init core
    core.Init();

    // Init script debugger
    CDebug.Init();

    /* Read config */
    auto ini = fio->OpenIniFile(fs::ENGINE_INI_FILE_NAME);

    uint32_t dwMaxFPS = 0;
    bool bSteam = false;
    int width = 1024, height = 768;
    bool fullscreen = false;

    if (ini)
    {
        dwMaxFPS = static_cast<uint32_t>(ini->GetLong(nullptr, "max_fps", 0));
        auto bDebugWindow = ini->GetLong(nullptr, "DebugWindow", 0) == 1;
        auto bAcceleration = ini->GetLong(nullptr, "Acceleration", 0) == 1;
        if (ini->GetLong(nullptr, "logs", 1) == 0) // disable logging
        {
            spdlog::set_level(spdlog::level::off);
        }
        width = ini->GetLong(nullptr, "screen_x", 1024);
        height = ini->GetLong(nullptr, "screen_y", 768);
        fullscreen = ini->GetLong(nullptr, "full_screen", false) ? true : false;
        bSteam = ini->GetLong(nullptr, "Steam", 1) != 0;
    }

    // evaluate SteamApi singleton
    steamapi::SteamApi::getInstance(!bSteam);

    std::shared_ptr<storm::OSWindow> window = storm::OSWindow::Create(width, height, fullscreen);
    window->SetTitle("Sea Dogs");
    core.Set_Hwnd(static_cast<HWND>(window->OSHandle()));
    window->Subscribe(HandleWindowEvent);
    window->Show();

    /* Init stuff */
    core.InitBase();

    /* Message loop */
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
            const auto runResult = core.Run();
            if (!isHold && !runResult)
            {
                isHold = true;
                isRunning = false;
            }

            lifecycleDiagnostics.notifyAfterRun();
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

    SDL_Quit();

    return 0;
}
