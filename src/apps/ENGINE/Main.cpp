#include "ExecutableLifecycleService.hpp"
#include "SteamApi.hpp"
#include "compiler.h"
#include "file_service.h"
#include "fs.h"
#include "s_debug.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

VFILE_SERVICE *fio = nullptr;
CORE core;
S_DEBUG CDebug;

namespace
{

bool isHold = false;
bool bActive = false;

} // namespace

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    // Prevent multiple instances
    if (!CreateEventA(nullptr, false, false, "Global\\FBBD2286-A9F1-4303-B60C-743C3D7AA7BE") ||
        GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBoxA(nullptr, "Another instance is already running!", "Error", MB_ICONERROR);
        return 0;
    }

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
    }

    // evaluate SteamApi singleton
    steamapi::SteamApi::getInstance(!bSteam);

    /* Register and show window */
    const auto *const windowName = L"Sea Dogs";

    WNDCLASSEX wndclass;
    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = sizeof(uint16_t);
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, TEXT("IDI_ICON1"));
    wndclass.hCursor = LoadCursor(hInstance, TEXT("NULL_CURSOR")); // LoadCursor(NULL,IDC_ARROW);
    wndclass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wndclass.lpszMenuName = nullptr;
    wndclass.lpszClassName = windowName;
    wndclass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    RegisterClassEx(&wndclass);

    auto *const hwnd = CreateWindow(windowName, windowName, WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWNORMAL);

    /* Init stuff */
    core.InitBase();

    /* Message loop */
    auto dwOldTime = GetTickCount();
    MSG msg;

    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
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
                    SendMessage(hwnd, WM_CLOSE, 0, 0L);
                }
            }
            else
            {
                Sleep(50);
            }
        }
    }

    /* Release */
    core.ReleaseBase();
    ClipCursor(nullptr);

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
    case WM_CREATE:
        core.Set_Hwnd(hwnd);
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        core.Event("DestroyWindow", nullptr);
        core.Event("ExitApplication", nullptr);
        CDebug.Release();
        core.CleanUp();
        CDebug.CloseDebugWindow();

        InvalidateRect(nullptr, nullptr, 0);
        PostQuitMessage(0);
        break;

    case WM_ACTIVATE:
        bActive = LOWORD(wParam) == WA_CLICKACTIVE || LOWORD(wParam) == WA_ACTIVE;
        core.AppState(bActive);
        break;

    case WM_KEYDOWN:
        if (wParam == VK_F5) // && bDebugWindow
        {
            if (!CDebug.IsDebug())
                CDebug.OpenDebugWindow(core.hInstance);
            else
            {
                ShowWindow(CDebug.GetWindowHandle(), SW_NORMAL);
                SetFocus(CDebug.SourceView->hOwn);
            }
        }
    case WM_KEYUP:
    case WM_RBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        // case MM_MCINOTIFY:
    case WM_LBUTTONDBLCLK:
    case WM_CHAR:
    case WM_MOUSEMOVE:
        if (core.Controls)
            core.Controls->EngineMessage(iMsg, wParam, lParam);
        break;

    case WM_MOUSEWHEEL:
        core.Event("evMouseWeel", "l", static_cast<short>(HIWORD(wParam)));
        if (core.Controls)
            core.Controls->EngineMessage(iMsg, wParam, lParam);
        break;

    default:;
    }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
