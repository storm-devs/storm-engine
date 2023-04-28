#include <thread>

#include <SDL2/SDL.h>
#include <mimalloc-new-delete.h>
#include <mimalloc.h>
#include <spdlog/spdlog.h>

#include "core_private.h"
#include "lifecycle_diagnostics_service.hpp"
#include "logging.hpp"
#include "os_window.hpp"
#include "steam_api.hpp"
#include "v_sound_service.h"
#include "fs.h"
#include "watermark.hpp"

namespace
{

CorePrivate *core_private;

constexpr char defaultLoggerName[] = "system";
bool isRunning = false;
bool bActive = true;
bool bSoundInBackground = false;

storm::diag::LifecycleDiagnosticsService lifecycleDiagnostics;

void RunFrame()
{
    if (!core_private->Run())
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
        if (core_private->initialized())
        {
            core_private->Event("DestroyWindow");
        }
    }
    else if (event == storm::OSWindow::FocusGained)
    {
        bActive = true;
        if (core_private->initialized())
        {
            core_private->AppState(bActive);
            if (const auto soundService = static_cast<VSoundService *>(core.GetService("SoundService"));
                soundService && !bSoundInBackground)
            {
                soundService->SetActiveWithFade(true);
            }
        }
    }
    else if (event == storm::OSWindow::FocusLost)
    {
        bActive = false;
        if (core_private->initialized())
        {
            core_private->AppState(bActive);
            if (const auto soundService = static_cast<VSoundService *>(core.GetService("SoundService"));
                soundService && !bSoundInBackground)
            {
                soundService->SetActiveWithFade(false);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    // Prevent multiple instances
#ifdef _WIN32 // CreateEventA
    if (!CreateEventA(nullptr, false, false, "Global\\FBBD2286-A9F1-4303-B60C-743C3D7AA7BE") ||
        GetLastError() == ERROR_ALREADY_EXISTS)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Another instance is already running!", nullptr);
        return EXIT_SUCCESS;
    }
#endif
    mi_register_output(mimalloc_fun, nullptr);
    mi_option_set(mi_option_show_errors, 1);
    mi_option_set(mi_option_show_stats, 0);
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

    // Init diagnostics
    const auto lifecycleDiagnosticsGuard =
#ifdef STORM_ENABLE_CRASH_REPORTS
        lifecycleDiagnostics.initialize(true);
#else
        lifecycleDiagnostics.initialize(false);
#endif
    if (!lifecycleDiagnosticsGuard)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", "Unable to initialize lifecycle service!", nullptr);
    }
    else
    {
        lifecycleDiagnostics.setCrashInfoCollector([]() { core_private->collectCrashInfo(); });
    }

    // Init stash
    create_directories(fs::GetSaveDataPath());

    // Init logging
    spdlog::set_default_logger(storm::logging::getOrCreateLogger(defaultLoggerName));
    spdlog::info("Logging system initialized. Running on {}", STORM_BUILD_WATERMARK_STRING);
    spdlog::info("mimalloc-redirect status: {}", mi_is_redirected());

    // Init core
    core_private = static_cast<CorePrivate *>(&core);
    core_private->Init();

    // Read config
    auto ini = fio->OpenIniFile(fs::ENGINE_INI_FILE_NAME);

    uint32_t dwMaxFPS = 0;
    bool bSteam = false;
    int width = 1024, height = 768;
    int preferred_display = 0;
    bool fullscreen = false;
    bool show_borders = false;
    bool run_in_background = false;

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
        preferred_display = ini->GetInt(nullptr, "display", 0);
        fullscreen = ini->GetInt(nullptr, "full_screen", false);
        show_borders = ini->GetInt(nullptr, "window_borders", false);
        run_in_background = ini->GetInt(nullptr, "run_in_background ", false);
        bSoundInBackground = (!run_in_background) || ini->GetInt(nullptr, "sound_in_background ", true);
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

    std::shared_ptr<storm::OSWindow> window =
        storm::OSWindow::Create(width, height, preferred_display, fullscreen, show_borders);
    window->SetTitle("Sea Dogs");
    window->Subscribe(HandleWindowEvent);
    window->Show();
    core_private->SetWindow(window);

    // Init core
    core_private->InitBase();

    // Message loop
    auto dwOldTime = SDL_GetTicks();

    isRunning = true;
    while (isRunning)
    {
        SDL_PumpEvents();
        SDL_FlushEvents(0, SDL_LASTEVENT);

        if (bActive || run_in_background)
        {
            if (dwMaxFPS)
            {
                const auto dwMS = 1000u / dwMaxFPS;
                const auto dwNewTime = SDL_GetTicks();
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
    core_private->Event("ExitApplication");
    core_private->CleanUp();
    core_private->ReleaseBase();
#ifdef _WIN32 // FIX_LINUX Cursor
    ClipCursor(nullptr);
#endif
    SDL_Quit();

    return EXIT_SUCCESS;
}
