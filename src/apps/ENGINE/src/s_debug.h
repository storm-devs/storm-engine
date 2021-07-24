#pragma once

#define PROJECT_NAME "project.df"

#include "s_dbg_breaktable.h"
#include "s_dbg_sourceview.h"
#include "s_dbg_watcherlist.h"

#define LST_NORMAL 0
#define LST_BREAKPOINT 1
#define LST_CONTROL 2

#define TMODE_CONTINUE 0
#define TMODE_MAKESTEP 1
#define TMODE_MAKESTEP_OVER 2
#define TMODE_MAKESTEP_OUT 3
#define TMODE_WAIT 4
#define TMODE_CLOSE 5

LRESULT CALLBACK DebugWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SourceViewWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI BackgroundThreadProc(LPVOID lpParameter);

enum DBG_DISPLAY_MODE
{
    MODE_SOURCE_VIEW,
    MODE_ATTRIBUTES_VIEW,
};

class COMPILER;

class S_DEBUG
{
    friend LRESULT CALLBACK DebugWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
    friend LRESULT CALLBACK SourceViewWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
    friend DWORD WINAPI BackgroundThreadProc(LPVOID lpParameter);
    friend COMPILER;
    HFONT hFont;
    HINSTANCE hInst;
    char sLastFileName[MAX_PATH];
    char ProgramDirectory[MAX_PATH];
    char BreakFileName[MAX_PATH];
    uint32_t BreakLineCode;
    char *pExpResBuffer;
    RECT WatcherListRect;
    RECT SourceViewRect;
    bool bTrace;
    uint32_t nTraceMode;
    uint32_t nTraceLine;
    HANDLE hDebugThread;
    uint32_t DebugThreadID;
    uint32_t MainThreadID;
    DBG_DISPLAY_MODE nDisplayMode;
    uint32_t nRecentFilesNum;
    uint32_t nRecentFilesIndex;
    uint32_t nRFMOffset;

  public:
    HWND hMain;
    BREAKPOINTS_TABLE Breaks;
    SOURCE_VIEW *SourceView;
    WATCHER_LIST *WatcherList;

    S_DEBUG() = default;
    ~S_DEBUG();

    HWND GetWindowHandle()
    {
        return hMain;
    };

    void Init();

    bool OpenDebugWindow(HINSTANCE hInstance);
    bool OpenDebugWindow_NT(HINSTANCE hInstance);
    void OpenNewFile();
    void CloseDebugWindow();
    bool IsDebug();
    const char *ProcessExpression(const char *pExpression);
    bool SetOnDebugExpression(const char *pLValue, const char *pRValue);
    uint32_t GetLineStatus(const char *_pFileName, uint32_t linecode);
    bool BrowseFile(char *buffer, const char *filter);
    void Release();
    void SetProgramDirectory(const char *dir_name);
    void SetTrace(bool on);
    bool IsTrace();
    uint32_t GetTraceMode();
    void SetTraceMode(uint32_t tmode);
    void BreakOn(const char *filename, uint32_t line);

    void SetTraceLine(uint32_t l)
    {
        nTraceLine = l;
    };
    void SetDbgDisplayMode(DBG_DISPLAY_MODE mode);
    bool BrowseFileWP(char *buffer, const char *filter);

    bool ProcessRegistry_Open();
    void Add2RecentFiles(const char *pFileName);
    long GetRecentFileALine(const char *pFileName);
    void SaveRecentFileALine(const char *pFileName, long nLine);
};
