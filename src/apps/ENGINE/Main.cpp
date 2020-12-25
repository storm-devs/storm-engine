#include <spdlog/spdlog.h>

#include "externs.h"
#include "fs.h"
#include "s_debug.h"
#include "steam_api.h"
#include <crtdbg.h>
#include <dbghelp.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <stdio.h>
#include <tchar.h>

S_DEBUG CDebug;
bool isHold = false;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int Alert(const char *lpCaption, const char *lpText);
void CreateMiniDump(EXCEPTION_POINTERS *pep);
bool _loopMain();

bool _loopMain()
{
    bool runResult = false;

    //__try
    {
        runResult = core.Run();
    }
    //__except( CreateMiniDump( GetExceptionInformation() ), EXCEPTION_EXECUTE_HANDLER )
    {
    }

    return runResult;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    /* Prevent multiple instances */
    if (!CreateEventA(nullptr, false, false, "Global\\FBBD2286-A9F1-4303-B60C-743C3D7AA7BE") ||
        GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBoxA(nullptr, "Another instance is already running!", "Error", MB_ICONERROR);
        return 0;
    }

    /* Init stash */
    create_directories(fs::GetLogsPath());
    create_directories(fs::GetSaveDataPath());

    /* Init system log */
    const auto log_path = fs::GetLogsPath() / "system.log";
    const auto system_log = spdlog::basic_logger_mt("system", log_path.string(), true);
    set_default_logger(system_log);
    system_log->set_level(spdlog::level::trace);

    fio = &File_Service;
    //_VSYSTEM_API = &System_Api;

    /* Read config */
    uint32_t dwMaxFPS = 0;
    auto *ini = File_Service.OpenIniFile(ENGINE_INI_FILE_NAME);
    if (ini)
    {
        dwMaxFPS = static_cast<uint32_t>(ini->GetLong(nullptr, "max_fps", 0));
        auto bDebugWindow = ini->GetLong(nullptr, "DebugWindow", 0) == 1;
        auto bAcceleration = ini->GetLong(nullptr, "Acceleration", 0) == 1;
        if (!ini->GetLong(nullptr, "logs", 0))
        {
            system_log->set_level(spdlog::level::off);
        }
        if (ini->GetLong(nullptr, "Steam", 1) != 0)
        {
            bSteam = true;
        }
        else
        {
            bSteam = false;
        }

        delete ini;
    }

    if (bSteam)
    {
        if (SteamAPI_RestartAppIfNecessary(223330))
        {
            return EXIT_FAILURE;
        }

        if (!SteamAPI_Init())
        {
            Alert("Fatal Error", "Steam must be running to play this game - SteamAPI_Init() failed!\n");
            return EXIT_FAILURE;
        }
        else
        {
            core.InitAchievements();
            core.InitSteamDLC();
        }
    }

    /* Register and show window */
    const auto *const windowName = L"Thunderstorm";

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

                bool runResult = _loopMain();

                //				if (!isHold && !core.Run())
                if (!isHold && !runResult)
                {
                    core.CleanUp();
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
    if (bSteam)
    {
        // Shutdown the SteamAPI
        SteamAPI_Shutdown();
        core.DeleteAchievements();
        core.DeleteSteamDLC();
    }

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
        File_Service.Close();
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

void CreateMiniDump(EXCEPTION_POINTERS *pep)
{
    // Open the file
    HANDLE hFile = CreateFile(_T("engine_dump.dmp"), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, NULL);

    if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
    {
        // Create the minidump
        MINIDUMP_EXCEPTION_INFORMATION mdei;

        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = pep;
        mdei.ClientPointers = FALSE;

        MINIDUMP_TYPE mdt =
            (MINIDUMP_TYPE)(MiniDumpWithFullMemory | MiniDumpWithFullMemoryInfo | MiniDumpWithHandleData |
                            MiniDumpWithThreadInfo | MiniDumpWithUnloadedModules);

        BOOL rv =
            MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, mdt, (pep != 0) ? &mdei : 0, 0, 0);

        if (!rv)
            _tprintf(_T("MiniDumpWriteDump failed. Error: %u \n"), GetLastError());
        else
            _tprintf(_T("Minidump created.\n"));

        // Close the file
        CloseHandle(hFile);
    }
    else
    {
        _tprintf(_T("CreateFile failed. Error: %u \n"), GetLastError());
    }
}

int Alert(const char *lpCaption, const char *lpText)
{
    std::wstring CaptionW = utf8::ConvertUtf8ToWide(lpCaption);
    std::wstring TextW = utf8::ConvertUtf8ToWide(lpText);
    return ::MessageBox(NULL, TextW.c_str(), CaptionW.c_str(), MB_OK);
}
