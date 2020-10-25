#include "..\..\common_h\exs.h"
#include "core.h"
#include "gdi_display.h"
#include "s_debug.h"
#include "system_log.h"
#include <windows.h>

#include <crtdbg.h>
#include <dbghelp.h>
#include <stdio.h>
#include <tchar.h>

#define def_width 600
#define def_height 400

#include "common_h.h"
#include "steam_api.h"

#pragma comment(lib, "dbghelp.lib")

HWND hMain;
HINSTANCE hInst;

char AClass[] = "Corsairs:Cuique suum!";

MEMORY_SERVICE Memory_Service;
FILE_SERVICE File_Service;
GDI_DISPLAY gdi_display;
CORE Core;
VAPI *_CORE_API;
VMA *_pModuleClassRoot = 0;
VAPI *api = 0;
VFILE_SERVICE *fio = 0;

bool bBackspaceExit = false;
bool bDebugWindow = false;
bool bAcceleration = false;
bool bActive = true;
bool bSteam = true;
bool bNetActive = false;
bool System_Hold = false;
bool Error_Flag = false;
char Last_Error[512];
CONTROL_STACK Control_Stack;
CONTROL_BLOCK Control_Block;
SYSTEM_API System_Api;
VSYSTEM_API *_VSYSTEM_API;

S_DEBUG CDebug;

CODESOURCE CodeSource;

dword Exceptions_Mask;
char ENGINE_INI_FILE_NAME[256] = "engine.ini";
extern bool bTraceFilesOff;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ProcessKeys(HWND hwnd, int code, int Press);
void EmergencyExit();
int Alert(const char *lpCaption, const char *lpText);
void CreateMiniDump(EXCEPTION_POINTERS *pep);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    HWND hFndWnd = FindWindow(AClass, AClass);
    if (hFndWnd != NULL)
        return 0;

    Core.hInstance = hInstance;
    _CORE_API = &Core;
    api = &Core;
    fio = &File_Service;
    _CORE_API->fio = &File_Service;
    _VSYSTEM_API = &System_Api;

    if (szCmdLine)
    {
        if (szCmdLine[0])
        {
            strcpy(ENGINE_INI_FILE_NAME, szCmdLine);
        }
    }
    INIFILE *ini;
    bool bValue = false;
    char sMemProfileFileName[MAX_PATH] = "";
    ini = File_Service.OpenIniFile(ENGINE_INI_FILE_NAME);
    bool bFirstLaunch = true;
    dword dwMaxFPS = 0;
    if (ini)
    {
        if (ini->GetLong("stats", "memory_stats", 0) != 0)
            bValue = true;
        else
            bValue = false;
        ini->ReadString(0, "mem_profile", sMemProfileFileName, sizeof(sMemProfileFileName), "");

        dwMaxFPS = (dword)ini->GetLong(0, "max_fps", 0);

        // eddy. уберем читы
        // bDebugWindow = ini->GetLong(0, "DebugWindow", 0) == 1;
        // bAcceleration = ini->GetLong(0, "Acceleration", 0) == 1;
        // bBackspaceExit = ini->GetLong(0, "BackSpaceExit", 0) == 1;

        bTraceFilesOff = ini->GetLong(0, "tracefilesoff", 0) == 1;
        if (ini->GetLong(0, "Steam", 1) != 0)
        {
            bSteam = true;
        }
        else
        {
            bSteam = false;
        }

        bFirstLaunch = ini->GetLong(0, "firstlaunch", 1) != 0;
        if (bFirstLaunch)
        {
            ini->WriteLong(0, "firstlaunch", 0);
        }

        delete ini;
    }
    Memory_Service.CollectInfo(bValue);
    Memory_Service.ProcessMemProfile(sMemProfileFileName);

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
            _CORE_API->InitAchievements();
            _CORE_API->InitSteamDLC();
        }
    }

    if (bFirstLaunch)
    {
        char InstallLocationExe[MAX_PATH];
        _snprintf(InstallLocationExe, sizeof(InstallLocationExe), "config.exe");

        PROCESS_INFORMATION pi;
        STARTUPINFO si;
        memset(&si, 0, sizeof(si));
        si.cb = sizeof(si);

        BOOL bProcess = CreateProcess(InstallLocationExe, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
        if (bProcess == TRUE)
        {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return 0;
        }
    }

    _CORE_API->SetExceptions(_X_NO_MEM | _X_NO_FILE_READ);

    Control_Stack.Init();

    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclass;
    hInst = hInstance;

    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = sizeof(WORD);
    wndclass.hInstance = hInst;
    wndclass.hIcon = LoadIcon(hInstance, "IDI_ICON1");
    wndclass.hCursor = LoadCursor(hInstance, "NULL_CURSOR"); // LoadCursor(NULL,IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = AClass;
    wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&wndclass);

    hwnd = CreateWindow(AClass, AClass, WS_POPUP, 0, 0, def_width, def_height, NULL, NULL, hInstance, NULL);
    hMain = hwnd;

    gdi_display.Init(hInstance, hwnd, def_width, def_height);

    Core.InitBase();

    // SetProcessAffinityMask(GetCurrentProcess(), 0x3);

    bool Run_result;

    dword dwOldTime = GetTickCount();
#ifndef EX_OFF
    try
    {
#endif
        while (true)
        {
            if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                if (WM_QUIT == msg.message)
                {
                    break;
                }
                else
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            else
            {
#ifndef EX_OFF
                try
#else
            __try
#endif
                {
                    if (bActive || bNetActive)
                    {
                        if (dwMaxFPS)
                        {
                            dword dwMS = 1000 / dwMaxFPS;
                            dword dwNewTime = GetTickCount();
                            if (dwNewTime - dwOldTime < dwMS)
                                continue;
                            dwOldTime = dwNewTime;
                        }
                        if (!System_Hold)
                            Run_result = Core.Run();
                        if (!Run_result)
                        {
                            Core.CleanUp();
                            gdi_display.Switch(true);
                            if (System_Api.Exceptions || System_Api.ExceptionsNF)
                            {
                                gdi_display.Print("One or more exception(s) occuried on Run");
                                gdi_display.Print("See log file for details");
                                Sleep(ERROR_MESSAGE_DELAY);
                            }
                            if (Core.Memory_Leak_flag)
                            {
                                gdi_display.Print("Memory leak detected");
                                gdi_display.Print("See log file for details");
                                Sleep(ERROR_MESSAGE_DELAY);
                            }
                            System_Hold = true;
                            SendMessage(hwnd, WM_CLOSE, 0, 0L);
                        }
                    }
                    else
                    {
                        Sleep(50);
                    }
                }
#ifdef EX_OFF
                __except (CreateMiniDump(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER)
                {
                }
#else
            catch (_EXS xobj)
            {
                trace("");
                switch (xobj.xtype)
                {
                case FATAL:
                    EmergencyExit();
                    break;
                case NON_FATAL:
                    Core.TraceCurrent();
                    System_Api.SetXNF();
                    gdi_display.Print("EXCEPTION( non fatal ) : %s", xobj.string);
                    break;
                }
                trace("");
            }
            catch (...)
            {
                EmergencyExit();
            }
#endif
            }
        }
#ifndef EX_OFF
    }
    catch (...)
    {
        trace("FATAL ERROR: %d", 0);
        ExitProcess(0xFFBADBAD);
    }
#endif

    if (Memory_Service.bCollectInfo)
    {
        Memory_Service.MemStat.Report();
        ini = File_Service.OpenIniFile(ENGINE_INI_FILE_NAME);
        if (ini)
        {
            if (ini->GetLong("stats", "update_mem_profile", 0) == 1)
            {
                Memory_Service.MemStat.UpdateMemoryProfile("memory.mp");
            }
            delete ini;
        }
    }
    if (bSteam)
    {
        // Shutdown the SteamAPI
        SteamAPI_Shutdown();
        _CORE_API->DeleteAchievements();
        _CORE_API->DeleteSteamDLC();
    }
    Core.ReleaseBase();

    ClipCursor(0);
    trace("System exit and cleanup :");
    trace("Mem state: User memory: %d  MSSystem: %d  Blocks: %d", Memory_Service.Allocated_memory_user,
          Memory_Service.Allocated_memory_system, Memory_Service.Blocks);

    Memory_Service.GlobalFree();

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    ENTITY_ID eidNET;
    WORD wActive;
#ifndef EX_OFF
    try
    {
#endif

        switch (iMsg)
        {
        case WM_CREATE:
            Core.Set_Hwnd(hwnd);
            return 0;
        case WM_PAINT:
            gdi_display.On_Paint(hwnd);
            break;

        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
            if (wParam == VK_F6)
            {
                return 0;
            }

            break;
        case WM_ACTIVATE:
            wActive = LOWORD(wParam);
            bActive = (wActive == WA_CLICKACTIVE || wActive == WA_ACTIVE);
            Core.AppState(bActive);
            break;

        case WM_ACTIVATEAPP:
            break;

        case WM_KEYDOWN:
            if (bDebugWindow)
                ProcessKeys(hwnd, (int)wParam, 0);

        case WM_KEYUP:
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case MM_MCINOTIFY:
        case WM_LBUTTONDBLCLK:
        case WM_CHAR:
        case WM_MOUSEMOVE:
            if (Core.Controls)
                Core.Controls->EngineMessage(iMsg, wParam, lParam);
            break;
        case 0x20A:
            Core.Event("evMouseWeel", "l", (short)HIWORD(wParam));
            if (Core.Controls)
                Core.Controls->EngineMessage(iMsg, wParam, lParam);
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            gdi_display.Print("DestroyWindow");
            Core.Event("DestroyWindow", null);
            Core.Event("ExitApplication", null);
            CDebug.Release();
            Core.CleanUp();
            gdi_display.Release();
            Control_Stack.Release();
            File_Service.Close();
            CDebug.CloseDebugWindow();
            InvalidateRect(0, 0, 0);
            PostQuitMessage(0);
            break;
        case SD_SERVERMESSAGE:
        case SD_CLIENTMESSAGE:
            if (Core.FindClass(&eidNET, "Net", 0))
            {
                Core.Send_Message(eidNET, "luu", iMsg, wParam, lParam);
            }
            break;
        }

#ifndef EX_OFF
    }
    catch (...)
    {
        gdi_display.Print("ERROR in WndProc: System halted");
        System_Hold = true;
    }
#endif

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void ProcessKeys(HWND hwnd, int code, int Press)
{
    switch (code)
    {
    case VK_BACK:
        if (bBackspaceExit)
        {
            Core.Exit();
            System_Hold = false;
        }
        break;
    case VK_F5:
        if (!CDebug.IsDebug())
            CDebug.OpenDebugWindow(hInst);
        else
        {
            ShowWindow(CDebug.GetWindowHandle(), SW_NORMAL);
            SetFocus(CDebug.SourceView->hOwn);
        }
        break;
    default:
        break;
    }
}

void *_cdecl operator new(size_t size)
{
    return Memory_Service.Allocate(size);
}

void *_cdecl operator new(size_t size, char *pFileName, DWORD nLine)
{
    CodeSource.pFileName = pFileName;
    CodeSource.line = nLine;
    return Memory_Service.Allocate(size);
}

void _cdecl operator delete(void *block_ptr)
{
    Memory_Service.Free(block_ptr);
}

void _cdecl operator delete(void *block_ptr, char *pFileName, DWORD nLine)
{
    Memory_Service.Free(block_ptr);
}

void *_cdecl resize(void *block_ptr, size_t size)
{
    return Memory_Service.Reallocate(block_ptr, size);
}

void *_cdecl resize(void *block_ptr, size_t size, char *pFileName, DWORD nLine)
{
    CodeSource.pFileName = pFileName;
    CodeSource.line = nLine;
    return Memory_Service.Reallocate(block_ptr, size);
}

void EmergencyExit()
{
    System_Api.SetX();
    gdi_display.Switch(true);
    Core.TraceCurrent();
    gdi_display.Print("FATAL EXCEPTION : System halted");
    System_Hold = true;
    Sleep(ERROR_MESSAGE_DELAY);
    ExitProcess(0xFFBADBAD);
}

int Alert(const char *lpCaption, const char *lpText)
{
    return ::MessageBox(NULL, lpText, lpCaption, MB_OK);
}

void CreateMiniDump(EXCEPTION_POINTERS *pep)
{
    // Open the file
    HANDLE hFile = CreateFile(_T("CCS_dump.dmp"), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
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
