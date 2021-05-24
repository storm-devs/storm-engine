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

VFILE_SERVICE *fio = nullptr;
CORE core;
S_DEBUG CDebug;

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
