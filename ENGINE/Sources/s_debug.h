#ifndef _S_DEBUG_H_
#define _S_DEBUG_H_

#ifndef _XBOX
/*#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif*/

#define PROJECT_NAME	"project.df"

#include "..\..\COMMON_H\memop.h"
#include "..\..\COMMON_H\d_types.h"
#include "s_dbg_watcherlist.h"
#include "s_dbg_sourceview.h"
#include "s_dbg_breaktable.h"

#define LST_NORMAL		0
#define LST_BREAKPOINT	1
#define LST_CONTROL		2


#define TMODE_CONTINUE		0
#define TMODE_MAKESTEP		1
#define TMODE_MAKESTEP_OVER	2
#define TMODE_MAKESTEP_OUT	3
#define TMODE_WAIT			4
#define TMODE_CLOSE			5


LRESULT CALLBACK DebugWndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK SourceViewWndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam);
DWORD WINAPI BackgroundThreadProc(LPVOID lpParameter);

enum DBG_DISPLAY_MODE
{
	MODE_SOURCE_VIEW,
	MODE_ATTRIBUTES_VIEW,
};

class COMPILER;
class S_DEBUG
{

friend LRESULT CALLBACK DebugWndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam);
friend LRESULT CALLBACK SourceViewWndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam);
friend DWORD WINAPI BackgroundThreadProc(LPVOID lpParameter);
friend COMPILER;
	HFONT hFont;
	HINSTANCE hInst;
	char sLastFileName[MAX_PATH];
	char ProgramDirectory[MAX_PATH];
	char BreakFileName[MAX_PATH];
	DWORD BreakLineCode;
	char * pExpResBuffer;
	RECT WatcherListRect;
	RECT SourceViewRect;
	bool bTrace;
	DWORD nTraceMode;
	DWORD nTraceLine;
	HANDLE hDebugThread;
	DWORD DebugThreadID;
	DWORD MainThreadID;
	DBG_DISPLAY_MODE nDisplayMode;
	DWORD nRecentFilesNum;
	DWORD nRecentFilesIndex;
	DWORD nRFMOffset;
public:
	HWND hMain;
	BREAKPOINTS_TABLE Breaks;
	SOURCE_VIEW * SourceView;
	WATCHER_LIST * WatcherList;

	 S_DEBUG();
	~S_DEBUG();
	HWND GetWindowHandle(){return hMain;};
	bool OpenDebugWindow(HINSTANCE hInstance);
	bool OpenDebugWindow_NT(HINSTANCE hInstance);
	void OpenNewFile();
	void CloseDebugWindow();
	bool IsDebug();
	char * ProcessExpression(char * pExpression);
	bool SetOnDebugExpression(char * pLValue, char * pRValue);
	DWORD GetLineStatus(const char * _pFileName, DWORD _linecode);
	bool BrowseFile(char * buffer, const char * filter);
	void Release();
	void SetProgramDirectory(char * dir_name);
	void SetTrace(bool on);
	bool IsTrace();
	DWORD GetTraceMode();
	void SetTraceMode(DWORD tmode);
	void BreakOn(char * filename,DWORD line);
	void SetTraceLine(DWORD _l){nTraceLine = _l;};
	void SetDbgDisplayMode(DBG_DISPLAY_MODE mode);
	bool BrowseFileWP(char * buffer, const char * filter);

	bool ProcessRegistry_Open();
	void Add2RecentFiles(char * pFileName);
	long GetRecentFileALine(char * pFileName);
	void SaveRecentFileALine(char * pFileName, long nLine);
};
#endif
#endif