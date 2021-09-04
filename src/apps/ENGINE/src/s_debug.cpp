#include "s_debug.h"
#include "compiler.h"
#include "core.h"
#include "resource.h"
#include "token.h"
#include <ShlObj.h>

LRESULT CALLBACK DebugWndProc(HWND, UINT, WPARAM, LPARAM);

//#define PROJECT_NAME    "project.df"
int FONT_HEIGHT = 15;
const wchar_t *DClass = L"SEDebug";

#define DBGWIN_WIDTH 900
#define DBGWIN_HEIGHT 600

extern S_DEBUG * CDebug;
char filefilter[256] = {"Script file\0 *.c\0Any file\0*.*\0\0"};

DWORD WINAPI BackgroundThreadProc(LPVOID lpParameter)
{
    if (CDebug->hMain == nullptr)
        CDebug->OpenDebugWindow_NT(CDebug->hInst);

    MSG msg;
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            if (WM_QUIT == msg.message)
            {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (CDebug->GetTraceMode() == TMODE_CLOSE)
                break;
        }
    }
    CDebug->CloseDebugWindow();
    uint32_t dwExitCode;
    GetExitCodeThread(CDebug->hDebugThread, (LPDWORD)&dwExitCode);
    ExitThread(dwExitCode);
    CloseHandle(CDebug->hDebugThread);
    CDebug->hDebugThread = nullptr;

    return 0;
}

LRESULT CALLBACK DebugWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    //    WORD wActive;
    //    bool bActive;
    wchar_t BufferW[MAX_PATH];
    wchar_t WinTextW[MAX_PATH];
    BROWSEINFO bi;

    if (CDebug->WatcherList)
    {
        CDebug->WatcherList->ProcessMessage(iMsg, wParam, lParam);
    }
    if (CDebug->SourceView)
    {
        CDebug->SourceView->ProcessMessage(iMsg, wParam, lParam);
    }

    switch (iMsg)
    {
    case WM_COMMAND:

        HMENU hMenu;
        hMenu = GetMenu(CDebug->hMain);
        if (hMenu)
        {
            MENUITEMINFO mii;
            auto *const hFileSubMenu = GetSubMenu(hMenu, 0);

            for (uint32_t n = 0; n < static_cast<uint32_t>(GetMenuItemCount(hFileSubMenu)); n++)
            {
                if (GetMenuItemID(hFileSubMenu, n) == LOWORD(wParam))
                {
                    if (n < CDebug->nRFMOffset || n >= (CDebug->nRFMOffset + CDebug->nRecentFilesNum))
                        continue;

                    PZERO(&mii, sizeof(mii));
                    mii.cbSize = sizeof(mii);
                    mii.fMask = MIIM_TYPE;
                    mii.fType = MFT_STRING;
                    mii.dwTypeData = BufferW;
                    mii.cch = sizeof(BufferW);

                    GetMenuItemInfo(hFileSubMenu, n, true, &mii);

                    std::string Buffer = utf8::ConvertWideToUtf8(BufferW);
                    strcpy(CDebug->sLastFileName, Buffer.c_str());
                    CDebug->SourceView->OpenSourceFile(Buffer.c_str());
                    wsprintf(WinTextW, L"SDebug - %s", BufferW);
                    SetWindowText(hwnd, WinTextW);
                    CDebug->Add2RecentFiles(Buffer.c_str());
                    CDebug->SourceView->SetActiveLine(CDebug->GetRecentFileALine(Buffer.c_str()));
                    break;
                }
            }
        }

        switch (LOWORD(wParam))
        {
        case MENU_OPENFILE:
            CDebug->OpenNewFile();
            break;
        case MENU_EXITDEBUG:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            return 0;
        case ID_FILE_CLOSEPROGRAM:
            core.Exit();
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case ID_OPTIONS_BREAKONVARIABLECHANGE:
            CDebug->SourceView->VarChangeModal();
            break;
        case ID_VIEW_SOURCEVIEWER:
            CDebug->SetDbgDisplayMode(MODE_SOURCE_VIEW);
            // CheckMenuItem(?,
            break;
        case ID_VIEW_ATTRIBUTEVIEWER:
            CDebug->SetDbgDisplayMode(MODE_ATTRIBUTES_VIEW);
            break;
        case ID_OPTIONS_BREAKONERROR: {
            auto ini = fio->OpenIniFile(PROJECT_NAME);
            if (!ini)
                break;
            if (ini->GetLong("options", "break_on_error", 0) == 1)
            {
                CheckMenuItem(static_cast<HMENU>(GetMenu(hwnd)), LOWORD(wParam), MF_UNCHECKED);
                ini->WriteLong("options", "break_on_error", 0);
                core.Compiler->bBreakOnError = false;
            }
            else
            {
                CheckMenuItem(static_cast<HMENU>(GetMenu(hwnd)), LOWORD(wParam), MF_CHECKED);
                ini->WriteLong("options", "break_on_error", 1);
                core.Compiler->bBreakOnError = true;
            }
            break;
        }
        case ID_FORMAT_DIALOG: {
            char Buffer[MAX_PATH];
            if (CDebug->BrowseFileWP(Buffer, filefilter))
            {
                core.Compiler->FormatDialog(Buffer);
            }
        }
        break;
        case ID_FORMAT_ALLDIALOGS:
            PZERO(&bi, sizeof(bi));
            bi.hwndOwner = hwnd;
            bi.pidlRoot = nullptr;
            bi.pszDisplayName = BufferW;
            bi.lpszTitle = TEXT("Select Dialog Folder");
            bi.lpfn = nullptr;
            bi.lParam = 0;
            bi.iImage = 0;
            if (SHGetPathFromIDList(SHBrowseForFolder(&bi), BufferW))
            {
                std::string Buffer = utf8::ConvertWideToUtf8(BufferW);
                core.Compiler->FormatAllDialog(Buffer.c_str());
            }
            break;
        }
        break;
    case WM_SIZE:
        if (CDebug->WatcherList)
        {
            CDebug->WatcherListRect.right = LOWORD(lParam);
            CDebug->WatcherList->SetPosition(CDebug->WatcherListRect);
        }
        if (CDebug->SourceView)
        {
            CDebug->SourceViewRect.right = LOWORD(lParam);
            CDebug->SourceViewRect.bottom = HIWORD(lParam);
            CDebug->SourceView->SetPosition(CDebug->SourceViewRect);
        }
        break;
    case WM_ACTIVATE:
        /*wActive = LOWORD(wParam);
        bActive = (wActive == WA_CLICKACTIVE || wActive == WA_ACTIVE);
        if(bActive)
        {
    //                ShowCursor(true);
    //                ShowCursor(true);
        }
        else
        {
    //                ShowCursor(false);
    //                ShowCursor(false);
        }*/
        break;
    case WM_CLOSE:
        CDebug->CloseDebugWindow();
        return 0;
    case WM_DESTROY:
        // CursorONOFF(false);
        break;
    case WM_CREATE: {
        // CursorONOFF(true);

        if (CDebug->WatcherList)
        {
            GetClientRect(hwnd, &CDebug->WatcherListRect);
            CDebug->WatcherListRect.bottom = 199;
            // CDebug->WatcherListRect.right = LOWORD(lParam);
            // CDebug->WatcherListRect.bottom = HIWORD(lParam)/2;
            CDebug->WatcherList->SetPosition(CDebug->WatcherListRect);
        }

        if (CDebug->SourceView)
        {
            GetClientRect(hwnd, &CDebug->SourceViewRect);
            CDebug->SourceViewRect.top = 200;
            CDebug->SourceView->SetPosition(CDebug->SourceViewRect);
        }

        auto ini = fio->OpenIniFile(PROJECT_NAME);
        if (ini)
        {
            if (ini->GetLong("options", "break_on_error", 0) == 1)
            {
                CheckMenuItem(static_cast<HMENU>(GetMenu(hwnd)), ID_OPTIONS_BREAKONERROR, MF_CHECKED);
                core.Compiler->bBreakOnError = true;
            }
            else
            {
                CheckMenuItem(static_cast<HMENU>(GetMenu(hwnd)), ID_OPTIONS_BREAKONERROR, MF_UNCHECKED);
                core.Compiler->bBreakOnError = false;
            }
        }

        break;
    }
    case WM_SYSKEYDOWN:
        return 1;
    case WM_SYSKEYUP:
        return 1;

        /*case WM_PAINT:
          gdi_display.On_Paint(hwnd);
        break;

        case WM_ACTIVATE:
          wActive = LOWORD(wParam);
          bActive = (wActive == WA_CLICKACTIVE || wActive == WA_ACTIVE);
          core.AppState(bActive);
        break;

        case WM_KEYDOWN:
          ProcessKeys(hwnd,(int)wParam,0);
        //case WM_ACTIVATE:
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
        case 0x20A:
          if(bActive)    core.ProcessSystemMessage(iMsg,wParam,lParam);
        break;
        /*case WM_CLOSE:
          DestroyWindow(hwnd);
        return 0;
        case WM_DESTROY:
          //InvalidateRect(0,0,0);
          //PostQuitMessage(0);
        break;*/
    }
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void S_DEBUG::BreakOn(const char *filename, uint32_t line)
{
    strcpy_s(BreakFileName, filename);
    BreakLineCode = line;
    ShowWindow(hMain, SW_NORMAL);
    if (WatcherList)
    {
        WatcherList->Refresh();
        InvalidateRect(WatcherList->GetWindowHandle(), nullptr, true);
    }
    if (SourceView)
    {
        SourceView->OpenSourceFile(BreakFileName);
        // SourceView->SetActiveLine(BreakLineCode);
        char wintext[MAX_PATH];
        sprintf_s(wintext, "SDebug - %s", filename);
        std::wstring WinTextW = utf8::ConvertUtf8ToWide(wintext);
        SetWindowText(hMain, WinTextW.c_str());
        SourceView->SetActiveLine(BreakLineCode);
    }
}

S_DEBUG::~S_DEBUG()
{
    CloseDebugWindow();
    if (hFont)
        DeleteObject(hFont);
    // if(pExpResBuffer) delete pExpResBuffer;

    if (CDebug->hDebugThread)
    {
        // DWORD dwExitCode;
        // GetExitCodeThread(CDebug->hDebugThread,&dwExitCode);
        // ExitThread(dwExitCode);
        // CloseHandle(CDebug->hDebugThread);
        // SetTraceMode(TMODE_CLOSE);
    }
    SetTraceMode(TMODE_CLOSE);

    Release();
}

void S_DEBUG::Release()
{
    Breaks.Release();
    delete WatcherList;
    WatcherList = nullptr;
    delete SourceView;
    SourceView = nullptr;
    delete pExpResBuffer;
    pExpResBuffer = nullptr;
}

void S_DEBUG::Init()
{
    nDisplayMode = MODE_SOURCE_VIEW;
    MainThreadID = GetCurrentThreadId();
    BreakFileName[0] = 0;
    BreakLineCode = 0;
    hDebugThread = nullptr;
    DebugThreadID = 0;
    ProgramDirectory[0] = 0;
    sLastFileName[0] = 0;
    hInst = nullptr;
    hMain = nullptr;
    WatcherList = nullptr;
    SourceView = nullptr;
    hFont = CreateFont(FONT_HEIGHT, 0, 0, 0,
                       // FW_BOLD,
                       FW_MEDIUM, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Courier New"));
    //"arial");
    pExpResBuffer = nullptr;
    bTrace = false;
    Breaks.ReadProject(PROJECT_NAME);
}

bool S_DEBUG::OpenDebugWindow(HINSTANCE hInstance)
{
    hInst = hInstance;
    SetTraceMode(TMODE_CONTINUE);
    hDebugThread = CreateThread(nullptr, 0, BackgroundThreadProc, nullptr, 0, (LPDWORD)&DebugThreadID);
    return true;
}

bool S_DEBUG::OpenDebugWindow_NT(HINSTANCE hInstance)
{
    if (hMain != nullptr)
        return true;
    // hInst =    hInstance;

    WNDCLASSEX wndclass;

    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wndclass.lpfnWndProc = DebugWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = sizeof(uint16_t);
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndclass.hbrBackground = static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH));
    wndclass.lpszMenuName = TEXT("DebugMenu"); // NULL;
    wndclass.lpszClassName = DClass;
    wndclass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    RegisterClassEx(&wndclass);

    const long xs = GetSystemMetrics(SM_CXSCREEN);
    const long ys = GetSystemMetrics(SM_CYSCREEN);
    // MoveWindow(hMain,(xs - DBGWIN_WIDTH)/2,(ys - DBGWIN_HEIGHT)/2,DBGWIN_WIDTH,DBGWIN_HEIGHT,false);

    hMain = CreateWindow(DClass, DClass,
                         // WS_POPUP|WS_SYSMENU|WS_CAPTION|WS_OVERLAPPED|WS_VISIBLE,
                         WS_POPUP | WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, xs, ys,
                         //(xs - DBGWIN_WIDTH)/2,(ys - DBGWIN_HEIGHT)/2,DBGWIN_WIDTH,DBGWIN_HEIGHT,
                         NULL, NULL, hInstance, NULL);

    ShowWindow(hMain, SW_NORMAL);

    MoveWindow(hMain, 0, 0, xs, ys - 32, false);

    if (hMain == nullptr)
        return false;

    ProcessRegistry_Open();

    /*    char sb[MAX_PATH];
      HMENU hMenu;
      hMenu = GetMenu(hMain);
      if(hMenu)
      {
        HMENU hFileSubMenu;
        MENUITEMINFO mii;
        hFileSubMenu = GetSubMenu(hMenu,0);

        strcpy_s(sb,"c:\\projects\\drive_v2\\engine\\programs\\seadogs.c");
        PZERO(&mii,sizeof(mii));
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_TYPE ;
        mii.fType = MFT_STRING;
        mii.dwTypeData = sb;
        mii.cch = strlen(sb) + 1;
        InsertMenuItem(hFileSubMenu,MENU_EXITDEBUG,false,&mii);


        PZERO(&mii,sizeof(mii));
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_TYPE;
        mii.fType = MFT_SEPARATOR;
        InsertMenuItem(hFileSubMenu,MENU_EXITDEBUG,false,&mii);

      }
    */

    InvalidateRect(nullptr, nullptr, 0);
    WatcherListRect.left = 0;
    WatcherListRect.top = 0;
    WatcherListRect.right = DBGWIN_WIDTH;
    WatcherListRect.bottom = 199; // DBGWIN_HEIGHT;
    SourceViewRect.left = 0;
    SourceViewRect.top = 200;
    SourceViewRect.bottom = DBGWIN_HEIGHT;
    SourceViewRect.right = DBGWIN_WIDTH;

    delete WatcherList;
    WatcherList = nullptr;

    WatcherList = new WATCHER_LIST(hMain, hInstance);
    if (WatcherList)
    {
        SendMessage(WatcherList->GetWindowHandle(), WM_SETFONT, (WPARAM)hFont, 0);
        WatcherList->SetFont(hFont);
    }
    delete SourceView;
    SourceView = nullptr;

    SourceView = new SOURCE_VIEW(hMain, hInstance);
    if (SourceView)
    {
        SourceView->SetFont(hFont);
        SourceView->SetProgramDirectory(ProgramDirectory);
    }

    if (WatcherList)
    {
        GetClientRect(hMain, &WatcherListRect);
        WatcherListRect.bottom = 199;
        WatcherList->SetPosition(WatcherListRect);
    }

    if (SourceView)
    {
        GetClientRect(hMain, &SourceViewRect);
        SourceViewRect.top = 200;
        // SourceViewRect.bottom -= FONT_HEIGHT;
        SourceView->SetPosition(SourceViewRect);
        if (BreakFileName[0] != 0)
        {
            SourceView->OpenSourceFile(BreakFileName);
            SourceView->SetActiveLine(BreakLineCode);
        }
    }
    if (sLastFileName[0] != 0)
        SourceView->OpenSourceFile(sLastFileName);

    return true;
}

void S_DEBUG::CloseDebugWindow()
{
    if (hMain)
        DestroyWindow(hMain);
    hMain = nullptr;
    delete WatcherList;
    WatcherList = nullptr;
    delete SourceView;
    SourceView = nullptr;
    SetTraceMode(TMODE_CLOSE);
    BreakFileName[0] = 0;
}

bool S_DEBUG::IsDebug()
{
    if (hMain)
        return true;
    return false;
}

bool S_DEBUG::SetOnDebugExpression(const char *pLValue, const char *pRValue)
{
    DATA Result;
    //    char * pC;
    Result.SetVCompiler(core.Compiler);
    if (core.Compiler->SetOnDebugExpression(pLValue, pRValue, Result))
        return true;
    return false;
}

const char *S_DEBUG::ProcessExpression(const char *pExpression)
{
    if (!pExpression || !strlen(pExpression))
        return "";
    DATA Result;
    const char *pC;
    Result.SetVCompiler(core.Compiler);
    if (core.Compiler->ProcessDebugExpression(pExpression, Result))
    {
        Result.Convert(VAR_STRING);
        if (Result.Get(pC))
        {
            const auto len = strlen(pC) + 1;
            delete pExpResBuffer;
            pExpResBuffer = new char[len];
            memcpy(pExpResBuffer, pC, len);
            return pExpResBuffer;
        }
    }
    return "Invalid Expression";
}

uint32_t S_DEBUG::GetLineStatus(const char *_pFileName, uint32_t _linecode)
{
    // nDebugTraceLineCode
    if (core.Compiler->pRun_fi && !core.Compiler->pRun_fi->decl_file_name.empty())
        if (_stricmp(core.Compiler->pRun_fi->decl_file_name.c_str(), _pFileName) == 0)
        {
            if (_linecode == core.Compiler->nDebugTraceLineCode)
                return LST_CONTROL;
        }
    if (Breaks.Find(_pFileName, _linecode))
        return LST_BREAKPOINT;
    return LST_NORMAL;
    // if(_linecode == 100) return LST_BREAKPOINT;
    // return LST_NORMAL;
}

bool S_DEBUG::BrowseFile(char *buffer, const char *filter)
{
    auto DirectoryName = fio->_GetCurrentDirectory();
    wchar_t FilenameW[MAX_PATH];
    OPENFILENAME ofn;
    FilenameW[0] = 0;
    std::wstring FilterW = utf8::ConvertUtf8ToWide(filter);
    PZERO(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hInstance = hInst;
    ofn.hwndOwner = hMain;
    ofn.lpstrFilter = FilterW.c_str();
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_FILEMUSTEXIST;
    ofn.lpstrFile = FilenameW;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = FilterW.c_str();
    ofn.lpstrTitle = TEXT("Open script source file");
    const auto bRes = GetOpenFileName(&ofn);
    fio->_SetCurrentDirectory(DirectoryName.c_str());
    if (bRes)
    {
        std::string Filename = utf8::ConvertWideToUtf8(FilenameW);
        DirectoryName = DirectoryName + "\\" + ProgramDirectory + "\\";
        strcpy_s(buffer, MAX_PATH, Filename.c_str() + strlen(DirectoryName.c_str()));
        //    strcpy_s(buffer,MAX_PATH, file_name + strlen(DirectoryName));
        // strcpy_s(buffer,file_name);
        return true;
    }
    return false;
}

bool S_DEBUG::BrowseFileWP(char *buffer, const char *filter)
{
    auto DirectoryName = fio->_GetCurrentDirectory();
    wchar_t FilenameW[MAX_PATH];
    OPENFILENAME ofn;
    FilenameW[0] = 0;
    std::wstring FilterW = utf8::ConvertUtf8ToWide(filter);
    PZERO(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hInstance = hInst;
    ofn.hwndOwner = hMain;
    ofn.lpstrFilter = FilterW.c_str();
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_FILEMUSTEXIST;
    ofn.lpstrFile = FilenameW;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = FilterW.c_str();
    ofn.lpstrTitle = TEXT("Open script source file");
    const auto bRes = GetOpenFileName(&ofn);
    fio->_SetCurrentDirectory(DirectoryName.c_str());
    if (bRes)
    {
        std::string Filename = utf8::ConvertWideToUtf8(FilenameW);
        strcpy_s(buffer, MAX_PATH, Filename.c_str());
        return true;
    }
    return false;
}

void S_DEBUG::SetProgramDirectory(const char *dir_name)
{
    if (dir_name)
    {
        strcpy_s(ProgramDirectory, dir_name);
        if (SourceView)
            SourceView->SetProgramDirectory(dir_name);
    }
}

void S_DEBUG::SetTrace(bool on)
{
    bTrace = on;
}

bool S_DEBUG::IsTrace()
{
    return bTrace;
}

uint32_t S_DEBUG::GetTraceMode()
{
    return nTraceMode;
}

void S_DEBUG::SetTraceMode(uint32_t tmode)
{
    //    DWORD dwExitCode;
    nTraceMode = tmode;
    //    DWORD nRes;
    switch (nTraceMode)
    {
    case TMODE_WAIT:
        /*if(hDebugThread) break;
        hDebugThread = CreateThread(0,0,BackgroundThreadProc,0,0,&DebugThreadID);
        nRes = AttachThreadInput(DebugThreadID,MainThreadID,true);*/

        break;
    case TMODE_CONTINUE:
        /*nRes = AttachThreadInput(DebugThreadID,MainThreadID,false);

        GetExitCodeThread(hDebugThread,&dwExitCode);
         ExitThread(dwExitCode);
        CloseHandle(hDebugThread);
        hDebugThread = 0;*/
        break;
    }
}

void S_DEBUG::SetDbgDisplayMode(DBG_DISPLAY_MODE mode)
{
    if (nDisplayMode == mode)
        return;
    switch (mode)
    {
    case MODE_SOURCE_VIEW:

        break;
    case MODE_ATTRIBUTES_VIEW:

        break;
    }
}

#define RECENT_FILES_MAX 8
#define ID_RECENTFILE_OFFSET 5000

void S_DEBUG::Add2RecentFiles(const char *pFileName)
{
    HKEY hKey;
    char buffer[MAX_PATH];
    wchar_t knW[MAX_PATH];
    bool bAdd;
    uint32_t dwSize;
    HMENU hMenu;
    HMENU hFileSubMenu;
    MENUITEMINFO mii;

    if (pFileName == nullptr)
        return;

    if (nRecentFilesNum >= RECENT_FILES_MAX)
        bAdd = false;
    else
        bAdd = true;

    if (nRecentFilesIndex >= RECENT_FILES_MAX)
    {
        nRecentFilesIndex = 0;
    }
    RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("SDIIDEBUGGER"), 0, KEY_ALL_ACCESS, &hKey);
    if (!hKey)
        if (RegCreateKey(HKEY_CURRENT_USER, TEXT("SDIIDEBUGGER"), &hKey) != ERROR_SUCCESS)
            return;

    for (uint32_t n = 0; n < nRecentFilesNum; n++)
    {
        wsprintf(knW, L"file%d", n);
        dwSize = sizeof(buffer);
        buffer[0] = 0;
        if (RegQueryValueEx(hKey, knW, nullptr, nullptr, (unsigned char *)buffer, (LPDWORD)&dwSize) == ERROR_SUCCESS)
        {
            if (_stricmp(buffer, pFileName) == 0)
            {
                // already in recent files list
                RegCloseKey(hKey);
                return;
            }
        }
    }

    wsprintf(knW, L"file%d", nRecentFilesIndex);
    RegSetValueEx(hKey, knW, 0, REG_SZ, (const unsigned char *)pFileName, strlen(pFileName) + 1);

    if (!bAdd)
    {
        hMenu = GetMenu(hMain);
        if (hMenu)
        {
            std::wstring FileNameW = utf8::ConvertUtf8ToWide(pFileName);
            hFileSubMenu = GetSubMenu(hMenu, 0);
            PZERO(&mii, sizeof(mii));
            mii.cbSize = sizeof(mii);
            mii.fMask = MIIM_TYPE | MIIM_ID;
            mii.fType = MFT_STRING;
            mii.dwTypeData = const_cast<wchar_t *>(FileNameW.c_str());
            mii.cch = FileNameW.length() + 1;
            mii.wID = ID_RECENTFILE_OFFSET + nRecentFilesIndex;
            SetMenuItemInfo(hFileSubMenu, nRFMOffset + nRecentFilesIndex, true, &mii);
        }
    }

    nRecentFilesIndex++;
    // RegSetValueEx(hKey,"Recent Files Index",0,REG_DWORD,(const unsigned char *)&nRecentFilesIndex,sizeof(DWORD));

    if (bAdd && (nRecentFilesNum < RECENT_FILES_MAX))
    {
        nRecentFilesNum++;
        RegSetValueEx(hKey, TEXT("Recent Files Num"), 0, REG_DWORD, (const unsigned char *)&nRecentFilesNum,
                      sizeof(uint32_t));

        hMenu = GetMenu(hMain);
        if (hMenu)
        {
            std::wstring FileNameW = utf8::ConvertUtf8ToWide(pFileName);
            hFileSubMenu = GetSubMenu(hMenu, 0);
            PZERO(&mii, sizeof(mii));
            mii.cbSize = sizeof(mii);
            mii.fMask = MIIM_TYPE | MIIM_ID;
            mii.fType = MFT_STRING;
            mii.dwTypeData = const_cast<wchar_t *>(FileNameW.c_str());
            mii.cch = FileNameW.length() + 1;
            mii.wID = ID_RECENTFILE_OFFSET + nRecentFilesNum - 1;
            InsertMenuItem(hFileSubMenu, nRFMOffset, true, &mii);
        }
        if (nRecentFilesNum == 1)
        {
            PZERO(&mii, sizeof(mii));
            mii.cbSize = sizeof(mii);
            mii.fMask = MIIM_TYPE;
            mii.fType = MFT_SEPARATOR;
            InsertMenuItem(hFileSubMenu, MENU_EXITDEBUG, false, &mii);
        }
    }

    RegCloseKey(hKey);
}

bool S_DEBUG::ProcessRegistry_Open()
{
    HKEY hKey;
    char buffer[MAX_PATH];
    wchar_t knW[MAX_PATH];
    uint32_t dwSize;
    uint32_t n;

    nRecentFilesNum = 0;

    RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("SDIIDEBUGGER"), 0, KEY_ALL_ACCESS, &hKey);
    if (!hKey)
        if (RegCreateKey(HKEY_CURRENT_USER, TEXT("SDIIDEBUGGER"), &hKey) != ERROR_SUCCESS)
            return false;

    dwSize = sizeof(uint32_t);
    const auto nRes = RegQueryValueEx(hKey, TEXT("Recent Files Num"), nullptr, nullptr,
                                      (unsigned char *)&nRecentFilesNum, (LPDWORD)&dwSize);
    if (nRes != ERROR_SUCCESS)
    {
        // write default value
        RegSetValueEx(hKey, TEXT("Recent Files Num"), 0, REG_DWORD, (const unsigned char *)&nRecentFilesNum,
                      sizeof(uint32_t));
    }

    if (nRecentFilesNum < RECENT_FILES_MAX)
        nRecentFilesIndex = nRecentFilesNum;
    else
        nRecentFilesIndex = 0;

    /*    dwSize = sizeof(DWORD);
      nRes = RegQueryValueEx(hKey,"Recent Files Index",0,0,(unsigned char *)&nRecentFilesIndex,&dwSize);
      if(nRes != ERROR_SUCCESS)
      {
        // write default value
        RegSetValueEx(hKey,"Recent Files Index",0,REG_DWORD,(const unsigned char *)&nRecentFilesIndex,sizeof(DWORD));
      }
    */

    auto *const hMenu = GetMenu(hMain);
    if (hMenu)
    {
        MENUITEMINFO mii;
        const HMENU hFileSubMenu = GetSubMenu(hMenu, 0);

        for (n = 0; n < static_cast<uint32_t>(GetMenuItemCount(hFileSubMenu)); n++)
        {
            if (GetMenuItemID(hFileSubMenu, n) == MENU_EXITDEBUG)
            {
                nRFMOffset = n;
                break;
            }
        }

        for (n = 0; n < nRecentFilesNum; n++)
        {
            wsprintf(knW, L"file%d", n);
            dwSize = sizeof(buffer);
            buffer[0] = 0;
            RegQueryValueEx(hKey, knW, nullptr, nullptr, (unsigned char *)buffer, (LPDWORD)&dwSize);

            std::wstring BufferW = utf8::ConvertUtf8ToWide(buffer);

            PZERO(&mii, sizeof(mii));
            mii.cbSize = sizeof(mii);
            mii.fMask = MIIM_TYPE | MIIM_ID;
            mii.fType = MFT_STRING;
            mii.dwTypeData = const_cast<wchar_t *>(BufferW.c_str());
            mii.cch = BufferW.length() + 1;
            mii.wID = ID_RECENTFILE_OFFSET + n;
            // InsertMenuItem(hFileSubMenu,MENU_EXITDEBUG,false,&mii);
            InsertMenuItem(hFileSubMenu, nRFMOffset, true, &mii);
        }

        if (nRecentFilesNum)
        {
            PZERO(&mii, sizeof(mii));
            mii.cbSize = sizeof(mii);
            mii.fMask = MIIM_TYPE;
            mii.fType = MFT_SEPARATOR;
            InsertMenuItem(hFileSubMenu, MENU_EXITDEBUG, false, &mii);
        }
    }

    RegCloseKey(hKey);
    return true;
}

long S_DEBUG::GetRecentFileALine(const char *pFileName)
{
    HKEY hKey;
    char buffer[MAX_PATH];
    wchar_t knW[MAX_PATH];

    uint32_t dwSize;
    uint32_t dwLine;

    if (pFileName == nullptr)
        return 0;

    RegOpenKeyEx(HKEY_CURRENT_USER, L"SDIIDEBUGGER", 0, KEY_ALL_ACCESS, &hKey);
    if (!hKey)
        return 0;

    for (uint32_t n = 0; n < nRecentFilesNum; n++)
    {
        wsprintf(knW, L"file%d", n);
        dwSize = sizeof(buffer);
        buffer[0] = 0;
        if (RegQueryValueEx(hKey, knW, nullptr, nullptr, (unsigned char *)buffer, (LPDWORD)&dwSize) == ERROR_SUCCESS)
        {
            if (_stricmp(buffer, pFileName) == 0)
            {
                wsprintf(knW, L"line%d", n);

                dwSize = sizeof(uint32_t);
                if (RegQueryValueEx(hKey, knW, nullptr, nullptr, (unsigned char *)&dwLine, (LPDWORD)&dwSize) ==
                    ERROR_SUCCESS)
                {
                    RegCloseKey(hKey);
                    return dwLine;
                }
                RegCloseKey(hKey);
                return 0;
            }
        }
    }
    RegCloseKey(hKey);
    return 0;
}

void S_DEBUG::SaveRecentFileALine(const char *pFileName, long nLine)
{
    HKEY hKey;
    char buffer[MAX_PATH];
    wchar_t knW[MAX_PATH];
    uint32_t dwSize;

    if (pFileName == nullptr)
        return;

    RegOpenKeyEx(HKEY_CURRENT_USER, L"SDIIDEBUGGER", 0, KEY_ALL_ACCESS, &hKey);
    if (!hKey)
        if (RegCreateKey(HKEY_CURRENT_USER, L"SDIIDEBUGGER", &hKey) != ERROR_SUCCESS)
            return;

    for (uint32_t n = 0; n < nRecentFilesNum; n++)
    {
        wsprintf(knW, L"file%d", n);
        dwSize = sizeof(buffer);
        buffer[0] = 0;
        if (RegQueryValueEx(hKey, knW, nullptr, nullptr, (unsigned char *)buffer, (LPDWORD)&dwSize) == ERROR_SUCCESS)
        {
            if (_stricmp(buffer, pFileName) == 0)
            {
                wsprintf(knW, L"line%d", n);
                RegSetValueEx(hKey, knW, 0, REG_DWORD, (const unsigned char *)&nLine, sizeof(uint32_t));
                RegCloseKey(hKey);
                return;
            }
        }
    }
    RegCloseKey(hKey);
}

void S_DEBUG::OpenNewFile()
{
    char buffer[1024], wintext[1024];
    if (CDebug->BrowseFile(buffer, filefilter))
    {
        strcpy_s(CDebug->sLastFileName, buffer);
        CDebug->SourceView->OpenSourceFile(buffer);
        sprintf_s(wintext, "SDebug - %s", buffer);
        std::wstring WinTextW = utf8::ConvertUtf8ToWide(wintext);
        SetWindowText(CDebug->GetWindowHandle(), WinTextW.c_str());
        CDebug->Add2RecentFiles(buffer);
    }
}
