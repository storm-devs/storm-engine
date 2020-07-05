#ifndef _XBOX

#include <shlobj.h>
#include "s_debug.h"
#include "core.h"
#include "resource.h"

LRESULT CALLBACK DebugWndProc(HWND,UINT,WPARAM,LPARAM);

//#define PROJECT_NAME	"project.df"
int FONT_HEIGHT = 15;
const char DClass[] = "SEDebug";

#define DBGWIN_WIDTH	900
#define DBGWIN_HEIGHT	600

extern S_DEBUG CDebug;
extern CORE Core;
char filefilter[256] = 
{
	"Script file\0 *.c\0Any file\0*.*\0\0"
};
DWORD WINAPI BackgroundThreadProc(LPVOID lpParameter)
{
	if(CDebug.hMain == 0) CDebug.OpenDebugWindow_NT(CDebug.hInst);

	MSG msg;
	while(true)
	{
		if(PeekMessage(&msg,NULL,0U,0U,PM_REMOVE))
		{
			if (WM_QUIT == msg.message) 
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if(CDebug.GetTraceMode() == TMODE_CLOSE) break;
			}
			
		}
	}
	CDebug.CloseDebugWindow();
	DWORD dwExitCode;
	GetExitCodeThread(CDebug.hDebugThread,&dwExitCode);
 	ExitThread(dwExitCode);
	CloseHandle(CDebug.hDebugThread);
	CDebug.hDebugThread = 0;
	
 	return 0;
}

LRESULT CALLBACK DebugWndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
//	WORD wActive;
//	bool bActive;
	INIFILE * ini;
	char buffer[MAX_PATH];
	char wintext[MAX_PATH];
	BROWSEINFO bi;
	DWORD n;

	if(CDebug.WatcherList) 
	{
		CDebug.WatcherList->ProcessMessage(iMsg,wParam,lParam);
	}
	if(CDebug.SourceView)
	{
		CDebug.SourceView->ProcessMessage(iMsg,wParam,lParam);
	}
	
	switch(iMsg)
	{
		case WM_COMMAND:

			
			HMENU hMenu;
			hMenu = GetMenu(CDebug.hMain);
			if(hMenu)
			{
				HMENU hFileSubMenu;
				MENUITEMINFO mii;
				hFileSubMenu = GetSubMenu(hMenu,0);
			
				for(n=0;n<(DWORD)GetMenuItemCount(hFileSubMenu);n++)
				{
					if(GetMenuItemID(hFileSubMenu,n) == LOWORD(wParam))
					{
						if(n<CDebug.nRFMOffset || n>=(CDebug.nRFMOffset + CDebug.nRecentFilesNum)) continue;

						ZeroMemory(&mii,sizeof(mii));
						mii.cbSize = sizeof(mii);
						mii.fMask = MIIM_TYPE ;
						mii.fType = MFT_STRING;
						mii.dwTypeData = buffer;
						mii.cch = sizeof(buffer);
						
						GetMenuItemInfo(hFileSubMenu,n,true,&mii);

						strcpy(CDebug.sLastFileName,buffer);
						CDebug.SourceView->OpenSourceFile(buffer);
						sprintf(wintext,"SDebug - %s",buffer);
						SetWindowText(hwnd,wintext);
						CDebug.Add2RecentFiles(buffer);
						CDebug.SourceView->SetActiveLine(CDebug.GetRecentFileALine(buffer));
						break;
					}
				}
			}

			switch(LOWORD(wParam))
			{
				case MENU_OPENFILE:
					CDebug.OpenNewFile();
				break;
				case MENU_EXITDEBUG:
					PostMessage(hwnd,WM_CLOSE,0,0);
				return 0;
				case ID_FILE_CLOSEPROGRAM:
					Core.Exit();
					PostMessage(hwnd,WM_CLOSE,0,0);
				break;
				case ID_OPTIONS_BREAKONVARIABLECHANGE:
					CDebug.SourceView->VarChangeModal();
				break;
				case ID_VIEW_SOURCEVIEWER:
					CDebug.SetDbgDisplayMode(MODE_SOURCE_VIEW);
					//CheckMenuItem(?,
				break;
				case ID_VIEW_ATTRIBUTEVIEWER:
					CDebug.SetDbgDisplayMode(MODE_ATTRIBUTES_VIEW);
				break;
				case ID_OPTIONS_BREAKONERROR:
					ini = Core.fio->OpenIniFile(PROJECT_NAME);
					if(!ini) break;
					if(ini->GetLong("options","break_on_error",0) == 1)
					{
						CheckMenuItem((HMENU)GetMenu(hwnd),LOWORD(wParam),MF_UNCHECKED);
						ini->WriteLong("options","break_on_error",0);
						Core.Compiler.bBreakOnError = false;
					} 
					else 
					{
						CheckMenuItem((HMENU)GetMenu(hwnd),LOWORD(wParam),MF_CHECKED);
						ini->WriteLong("options","break_on_error",1);
						Core.Compiler.bBreakOnError = true;
					}
					delete ini;
				break;
				case ID_FORMAT_DIALOG:
					if(CDebug.BrowseFileWP(buffer,filefilter))
					{
						Core.Compiler.FormatDialog(buffer);
					}
				break;
				case ID_FORMAT_ALLDIALOGS:
					ZeroMemory(&bi,sizeof(bi));
					bi.hwndOwner = hwnd; 
					bi.pidlRoot = 0; 
					bi.pszDisplayName = buffer; 
					bi.lpszTitle = "Select Dialog Folder"; 
					bi.lpfn = 0; 
					bi.lParam = 0; 
					bi.iImage = 0; 
					if(SHGetPathFromIDList(SHBrowseForFolder(&bi),buffer))
					{
						Core.Compiler.FormatAllDialog(buffer);
					}
				break;
			}
		break;
		case WM_SIZE:
			if(CDebug.WatcherList)
			{
				CDebug.WatcherListRect.right = LOWORD(lParam);
				CDebug.WatcherList->SetPosition(CDebug.WatcherListRect);
			}
			if(CDebug.SourceView)
			{
				CDebug.SourceViewRect.right = LOWORD(lParam);
				CDebug.SourceViewRect.bottom = HIWORD(lParam);
				CDebug.SourceView->SetPosition(CDebug.SourceViewRect);
			}
		break;
		case WM_ACTIVATE: 
			/*wActive = LOWORD(wParam);           
			bActive = (wActive == WA_CLICKACTIVE || wActive == WA_ACTIVE);
			if(bActive)
			{
//				ShowCursor(true);
//				ShowCursor(true);
			}
			else
			{
//				ShowCursor(false);
//				ShowCursor(false);
			}*/
		break;
		case WM_CLOSE:
			CDebug.CloseDebugWindow();
		return 0;
		case WM_DESTROY:
			//CursorONOFF(false);
		break;
		case WM_CREATE:
			//CursorONOFF(true);

			if(CDebug.WatcherList)
			{
				GetClientRect(hwnd,&CDebug.WatcherListRect);
				CDebug.WatcherListRect.bottom = 199;
				//CDebug.WatcherListRect.right = LOWORD(lParam);
				//CDebug.WatcherListRect.bottom = HIWORD(lParam)/2;
				CDebug.WatcherList->SetPosition(CDebug.WatcherListRect);
			}

			if(CDebug.SourceView)
			{
				GetClientRect(hwnd,&CDebug.SourceViewRect);
				CDebug.SourceViewRect.top = 200;
				CDebug.SourceView->SetPosition(CDebug.SourceViewRect);
			}

			ini = Core.fio->OpenIniFile(PROJECT_NAME);
			if(ini)
			{
				if(ini->GetLong("options","break_on_error",0) == 1)
				{
					CheckMenuItem((HMENU)GetMenu(hwnd),ID_OPTIONS_BREAKONERROR,MF_CHECKED);
					Core.Compiler.bBreakOnError = true;
				}
				else
				{
					CheckMenuItem((HMENU)GetMenu(hwnd),ID_OPTIONS_BREAKONERROR,MF_UNCHECKED);
					Core.Compiler.bBreakOnError = false;
				} 
				delete ini;
			}

		break;

		case WM_SYSKEYDOWN: return 1;
		case WM_SYSKEYUP: return 1;
	
		/*case WM_PAINT:
			gdi_display.On_Paint(hwnd);
		break;

		case WM_ACTIVATE: 
			wActive = LOWORD(wParam);           
			bActive = (wActive == WA_CLICKACTIVE || wActive == WA_ACTIVE);
			Core.AppState(bActive);
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
			if(bActive)	Core.ProcessSystemMessage(iMsg,wParam,lParam);
		break;
		/*case WM_CLOSE:
			DestroyWindow(hwnd);
		return 0;
		case WM_DESTROY:
			//InvalidateRect(0,0,0);
			//PostQuitMessage(0);
		break;*/
		
	}
	return DefWindowProc(hwnd,iMsg,wParam,lParam);
}

void S_DEBUG::BreakOn(char * filename,DWORD line)
{
	strcpy(BreakFileName,filename);
	BreakLineCode = line;
	ShowWindow(hMain,SW_NORMAL);
	if(WatcherList)
	{
		WatcherList->Refresh();
		InvalidateRect(WatcherList->GetWindowHandle(),0,true);

	}
	if(SourceView)
	{
		SourceView->OpenSourceFile(BreakFileName);
		//SourceView->SetActiveLine(BreakLineCode);
		char wintext[MAX_PATH];
		sprintf(wintext,"SDebug - %s",filename);
		SetWindowText(hMain,wintext);
		SourceView->SetActiveLine(BreakLineCode);
		
	}
}

S_DEBUG::S_DEBUG()
{
	nDisplayMode = MODE_SOURCE_VIEW;
	MainThreadID = GetCurrentThreadId();
	BreakFileName[0] = 0;
	BreakLineCode = 0;
	hDebugThread = 0;
	DebugThreadID = 0;
	ProgramDirectory[0] = 0;
	sLastFileName[0] = 0;
	hInst = 0;
	hMain = 0;
	WatcherList = 0;
	SourceView = 0;
	hFont = CreateFont(FONT_HEIGHT,0,0,0,
		//FW_BOLD,
		FW_MEDIUM,
		false,false,false,DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,VARIABLE_PITCH,
		"Courier New");
		//"arial");
	pExpResBuffer = 0;
	bTrace = false;
	Breaks.ReadProject(PROJECT_NAME);
}

S_DEBUG::~S_DEBUG()
{
	CloseDebugWindow();
	if(hFont) DeleteObject(hFont);
	//if(pExpResBuffer) delete pExpResBuffer;

	if(CDebug.hDebugThread)
	{
		//DWORD dwExitCode;
		//GetExitCodeThread(CDebug.hDebugThread,&dwExitCode);
 		//ExitThread(dwExitCode);
		//CloseHandle(CDebug.hDebugThread);
		//SetTraceMode(TMODE_CLOSE);
	}
	SetTraceMode(TMODE_CLOSE);
}

void S_DEBUG::Release()
{
	Breaks.Release();
	if(WatcherList) delete WatcherList; WatcherList = 0;
	if(SourceView) delete SourceView; SourceView = 0;
	if(pExpResBuffer) delete pExpResBuffer; pExpResBuffer = 0;
}

bool S_DEBUG::OpenDebugWindow(HINSTANCE hInstance)
{
	hInst =	hInstance;
	SetTraceMode(TMODE_CONTINUE);
	hDebugThread = CreateThread(0,0,BackgroundThreadProc,0,0,&DebugThreadID);
	return true;
}

bool S_DEBUG::OpenDebugWindow_NT(HINSTANCE hInstance)
{
	if(hMain != 0) return true;
	//hInst =	hInstance;

	WNDCLASSEX wndclass;
	
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
	wndclass.lpfnWndProc = DebugWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = sizeof(WORD);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wndclass.lpszMenuName = "DebugMenu";//NULL;
	wndclass.lpszClassName = DClass;
	wndclass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);
	RegisterClassEx(&wndclass);

	long xs,ys;
	xs = GetSystemMetrics(SM_CXSCREEN);
	ys = GetSystemMetrics(SM_CYSCREEN);
	//MoveWindow(hMain,(xs - DBGWIN_WIDTH)/2,(ys - DBGWIN_HEIGHT)/2,DBGWIN_WIDTH,DBGWIN_HEIGHT,false);

	hMain = CreateWindow(DClass,DClass,
		//WS_POPUP|WS_SYSMENU|WS_CAPTION|WS_OVERLAPPED|WS_VISIBLE,
		WS_POPUP|WS_OVERLAPPEDWINDOW|WS_VISIBLE,
		0,0,xs,ys,
		//(xs - DBGWIN_WIDTH)/2,(ys - DBGWIN_HEIGHT)/2,DBGWIN_WIDTH,DBGWIN_HEIGHT,
		NULL,NULL,hInstance,NULL);
	
	ShowWindow(hMain,SW_NORMAL);

	MoveWindow(hMain,0,0,xs,ys - 32,false);
	
	if(hMain == 0) return false;

	ProcessRegistry_Open();

/*	char sb[MAX_PATH];
	HMENU hMenu;
	hMenu = GetMenu(hMain);
	if(hMenu)
	{
		HMENU hFileSubMenu;
		MENUITEMINFO mii;
		hFileSubMenu = GetSubMenu(hMenu,0);
		
		strcpy(sb,"c:\\projects\\drive_v2\\engine\\programs\\seadogs.c");
		ZeroMemory(&mii,sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE ;
		mii.fType = MFT_STRING;
		mii.dwTypeData = sb;
		mii.cch = strlen(sb) + 1;
		InsertMenuItem(hFileSubMenu,MENU_EXITDEBUG,false,&mii);


		ZeroMemory(&mii,sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_SEPARATOR;
		InsertMenuItem(hFileSubMenu,MENU_EXITDEBUG,false,&mii);
		
	}
*/	



	

	InvalidateRect(0,0,0);
	WatcherListRect.left = 0;
	WatcherListRect.top = 0;
	WatcherListRect.right = DBGWIN_WIDTH;
	WatcherListRect.bottom = 199;//DBGWIN_HEIGHT;
	SourceViewRect.left = 0;
	SourceViewRect.top = 200;
	SourceViewRect.bottom = DBGWIN_HEIGHT;
	SourceViewRect.right = DBGWIN_WIDTH;

	if(WatcherList) delete WatcherList; WatcherList = 0;

	WatcherList = NEW WATCHER_LIST(hMain,hInstance);
	if(WatcherList)
	{
		SendMessage(WatcherList->GetWindowHandle(),WM_SETFONT,(WPARAM)hFont,0);
		WatcherList->SetFont(hFont);
	}
	if(SourceView) delete SourceView; SourceView = 0;

	SourceView = NEW SOURCE_VIEW(hMain,hInstance);
	if(SourceView)
	{
		SourceView->SetFont(hFont);
		SourceView->SetProgramDirectory(ProgramDirectory);
	}


	if(WatcherList)
	{
		GetClientRect(hMain,&WatcherListRect);
		WatcherListRect.bottom = 199;
		WatcherList->SetPosition(WatcherListRect);
	}

	if(SourceView)
	{
		GetClientRect(hMain,&SourceViewRect);
		SourceViewRect.top = 200;
		//SourceViewRect.bottom -= FONT_HEIGHT;
		SourceView->SetPosition(SourceViewRect);
		if(BreakFileName[0] != 0) 
		{
			SourceView->OpenSourceFile(BreakFileName);
			SourceView->SetActiveLine(BreakLineCode);
		}
	}
	if(sLastFileName[0] != 0) SourceView->OpenSourceFile(sLastFileName);
	
	return true;
}

void S_DEBUG::CloseDebugWindow()
{
	if(hMain) DestroyWindow(hMain);	
	hMain = 0;
	if(WatcherList) delete WatcherList; WatcherList = 0;
	if(SourceView) delete SourceView; SourceView = 0;
	SetTraceMode(TMODE_CLOSE);
	BreakFileName[0] = 0;
}

bool S_DEBUG::IsDebug()
{
	if(hMain) return true;
	return false;

}

bool S_DEBUG::SetOnDebugExpression(char * pLValue, char * pRValue)
{
	DATA Result;
//	char * pC;
	Result.SetVCompiler(&Core.Compiler);
	if(Core.Compiler.SetOnDebugExpression(pLValue,pRValue,Result)) return true;
	return false;
}

char * S_DEBUG::ProcessExpression(char * pExpression)
{
	if (!pExpression || !strlen(pExpression)) return "";
	DATA Result;
	char * pC;
	Result.SetVCompiler(&Core.Compiler);
	if(Core.Compiler.ProcessDebugExpression(pExpression,Result))
	{
		Result.Convert(VAR_STRING);
		if(Result.Get(pC))
		{
			pExpResBuffer = (char *)RESIZE(pExpResBuffer,strlen(pC) + 1);
			strcpy(pExpResBuffer,pC);
			return pExpResBuffer;
		}
	}
	return "Invalid Expression";
}

DWORD S_DEBUG::GetLineStatus(const char * _pFileName, DWORD _linecode)
{
	//nDebugTraceLineCode
	if(Core.Compiler.pRun_fi && Core.Compiler.pRun_fi->decl_file_name)
	if(stricmp(Core.Compiler.pRun_fi->decl_file_name,_pFileName)==0)
	{
		if(_linecode == Core.Compiler.nDebugTraceLineCode) return LST_CONTROL;
	}
	if(Breaks.Find(_pFileName,_linecode)) return LST_BREAKPOINT;
	return LST_NORMAL;
	//if(_linecode == 100) return LST_BREAKPOINT;
	//return LST_NORMAL;
}


bool S_DEBUG::BrowseFile(char * buffer, const char * filter)
{
	char DirectoryName[MAX_PATH];
	Core.fio->_GetCurrentDirectory(sizeof(DirectoryName),DirectoryName);
	char file_name[MAX_PATH];
	OPENFILENAME ofn;
	BOOL bRes;
	file_name[0] = 0;
	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hInstance = hInst;
	ofn.hwndOwner = hMain;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.lpstrFile = file_name;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = filter;
	ofn.lpstrTitle = "Open script source file";
	bRes = GetOpenFileName(&ofn);
	Core.fio->_SetCurrentDirectory(DirectoryName);
	if(bRes) 
	{
		strcat(DirectoryName,"\\");
		strcat(DirectoryName,ProgramDirectory);
		strcat(DirectoryName,"\\");
		strcpy(buffer,file_name + strlen(DirectoryName)); 
		//strcpy(buffer,file_name);
		return true;
	}
	return false;
}

bool S_DEBUG::BrowseFileWP(char * buffer, const char * filter)
{
	char DirectoryName[MAX_PATH];
	Core.fio->_GetCurrentDirectory(sizeof(DirectoryName),DirectoryName);
	char file_name[MAX_PATH];
	OPENFILENAME ofn;
	BOOL bRes;
	file_name[0] = 0;
	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hInstance = hInst;
	ofn.hwndOwner = hMain;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.lpstrFile = file_name;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = filter;
	ofn.lpstrTitle = "Open script source file";
	bRes = GetOpenFileName(&ofn);
	Core.fio->_SetCurrentDirectory(DirectoryName);
	if(bRes) 
	{
		strcpy(buffer,file_name);
		return true;
	}
	return false;
}

void S_DEBUG::SetProgramDirectory(char * dir_name)
{
	if(dir_name)
	{
		strcpy(ProgramDirectory,dir_name);
		if(SourceView) SourceView->SetProgramDirectory(dir_name);
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

DWORD S_DEBUG::GetTraceMode()
{
	return nTraceMode;
}

void S_DEBUG::SetTraceMode(DWORD tmode)
{
//	DWORD dwExitCode;
	nTraceMode = tmode;
//	DWORD nRes;
	switch(nTraceMode)
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
	if(nDisplayMode == mode) return;
	switch(mode)
	{
		case MODE_SOURCE_VIEW:
			

		break;
		case MODE_ATTRIBUTES_VIEW:

		break;
	}
}

#define RECENT_FILES_MAX		8
#define ID_RECENTFILE_OFFSET	5000

void S_DEBUG::Add2RecentFiles(char * pFileName)
{
	HKEY hKey;
	char buffer[MAX_PATH];
	char kn[MAX_PATH];
	bool bAdd;
	DWORD dwSize;
	HMENU hMenu;
	HMENU hFileSubMenu;
	MENUITEMINFO mii;


	if(pFileName == 0) return;


	if(nRecentFilesNum >= RECENT_FILES_MAX) bAdd = false;
	else bAdd = true;

	if(nRecentFilesIndex >= RECENT_FILES_MAX)
	{
		nRecentFilesIndex = 0;
	}
	RegOpenKeyEx(HKEY_CURRENT_USER,"SDIIDEBUGGER",0,KEY_ALL_ACCESS,&hKey);
	if(!hKey) if(RegCreateKey(HKEY_CURRENT_USER,"SDIIDEBUGGER",&hKey) != ERROR_SUCCESS) return;

	for(DWORD n=0;n<nRecentFilesNum;n++)
	{
		wsprintf(kn,"file%d",n);
		dwSize = sizeof(buffer);
		buffer[0] = 0;
		if(RegQueryValueEx(hKey,kn,0,0,(unsigned char *)buffer,&dwSize) == ERROR_SUCCESS)
		{
			if(stricmp(buffer,pFileName)==0) 
			{
				// already in recent files list
				RegCloseKey(hKey);
				return;
			}
		}
	}

	wsprintf(kn,"file%d",nRecentFilesIndex);
	RegSetValueEx(hKey,kn,0,REG_SZ,(const unsigned char *)pFileName,strlen(pFileName)+1);

	if(!bAdd)
	{
		hMenu = GetMenu(hMain);
		if(hMenu)
		{
			
			hFileSubMenu = GetSubMenu(hMenu,0);
			ZeroMemory(&mii,sizeof(mii));
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_TYPE|MIIM_ID ;
			mii.fType = MFT_STRING;
			mii.dwTypeData = pFileName;
			mii.cch = strlen(pFileName) + 1;
			mii.wID = ID_RECENTFILE_OFFSET + nRecentFilesIndex;
			SetMenuItemInfo(hFileSubMenu,nRFMOffset + nRecentFilesIndex,true,&mii);
 		}
	}
	
	nRecentFilesIndex++;
	//RegSetValueEx(hKey,"Recent Files Index",0,REG_DWORD,(const unsigned char *)&nRecentFilesIndex,sizeof(DWORD));	
	
	if(bAdd && (nRecentFilesNum < RECENT_FILES_MAX))
	{
		nRecentFilesNum++;
		RegSetValueEx(hKey,"Recent Files Num",0,REG_DWORD,(const unsigned char *)&nRecentFilesNum,sizeof(DWORD));	
		
		hMenu = GetMenu(hMain);
		if(hMenu)
		{
			
			hFileSubMenu = GetSubMenu(hMenu,0);
			ZeroMemory(&mii,sizeof(mii));
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_TYPE|MIIM_ID ;
			mii.fType = MFT_STRING;
			mii.dwTypeData = pFileName;
			mii.cch = strlen(pFileName) + 1;
			mii.wID = ID_RECENTFILE_OFFSET + nRecentFilesNum - 1;
			InsertMenuItem(hFileSubMenu,nRFMOffset,true,&mii);
		}
		if(nRecentFilesNum == 1)
		{
			ZeroMemory(&mii,sizeof(mii));
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_TYPE;
			mii.fType = MFT_SEPARATOR;
			InsertMenuItem(hFileSubMenu,MENU_EXITDEBUG,false,&mii);
		}
	}
		
	RegCloseKey(hKey);
}


bool S_DEBUG::ProcessRegistry_Open()
{
	HKEY hKey;
	char buffer[MAX_PATH];
	char kn[MAX_PATH];
	DWORD dwSize;
	long nRes;
	DWORD n;

	nRecentFilesNum = 0;

	RegOpenKeyEx(HKEY_CURRENT_USER,"SDIIDEBUGGER",0,KEY_ALL_ACCESS,&hKey);
	if(!hKey) if(RegCreateKey(HKEY_CURRENT_USER,"SDIIDEBUGGER",&hKey) != ERROR_SUCCESS) return false;

	dwSize = sizeof(DWORD);
	nRes = RegQueryValueEx(hKey,"Recent Files Num",0,0,(unsigned char *)&nRecentFilesNum,&dwSize);
	if(nRes != ERROR_SUCCESS)
	{
		// write default value
		RegSetValueEx(hKey,"Recent Files Num",0,REG_DWORD,(const unsigned char *)&nRecentFilesNum,sizeof(DWORD));	
	}

	if(nRecentFilesNum < RECENT_FILES_MAX) nRecentFilesIndex = nRecentFilesNum;
	else nRecentFilesIndex = 0;
	

/*	dwSize = sizeof(DWORD);
	nRes = RegQueryValueEx(hKey,"Recent Files Index",0,0,(unsigned char *)&nRecentFilesIndex,&dwSize);
	if(nRes != ERROR_SUCCESS)
	{
		// write default value
		RegSetValueEx(hKey,"Recent Files Index",0,REG_DWORD,(const unsigned char *)&nRecentFilesIndex,sizeof(DWORD));	
	}
*/	

//	char sb[MAX_PATH];
	HMENU hMenu;
	hMenu = GetMenu(hMain);
	if(hMenu)
	{
		HMENU hFileSubMenu;
		MENUITEMINFO mii;
		hFileSubMenu = GetSubMenu(hMenu,0);
	
		for(n=0;n<(DWORD)GetMenuItemCount(hFileSubMenu);n++)
		{
			if(GetMenuItemID(hFileSubMenu,n) == MENU_EXITDEBUG)
			{
				nRFMOffset = n;
				break;
			}
		}

		
		for(n=0;n<nRecentFilesNum;n++)
		{
			wsprintf(kn,"file%d",n);
			dwSize = sizeof(buffer);
			buffer[0] = 0;
			RegQueryValueEx(hKey,kn,0,0,(unsigned char *)buffer,&dwSize);
		
			ZeroMemory(&mii,sizeof(mii));
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_TYPE|MIIM_ID;
			mii.fType = MFT_STRING;
			mii.dwTypeData = buffer;
			mii.cch = strlen(buffer) + 1;
			mii.wID = ID_RECENTFILE_OFFSET + n;
			//InsertMenuItem(hFileSubMenu,MENU_EXITDEBUG,false,&mii);
			InsertMenuItem(hFileSubMenu,nRFMOffset,true,&mii);
		}

		if(nRecentFilesNum)
		{
			ZeroMemory(&mii,sizeof(mii));
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_TYPE;
			mii.fType = MFT_SEPARATOR;
			InsertMenuItem(hFileSubMenu,MENU_EXITDEBUG,false,&mii);
		}
		
	}

	RegCloseKey(hKey);
	return true;
}



long S_DEBUG::GetRecentFileALine(char * pFileName)
{
	HKEY hKey;
	char buffer[MAX_PATH];
	char kn[MAX_PATH];

	DWORD dwSize;
	DWORD dwLine;


	if(pFileName == 0) return 0;

	RegOpenKeyEx(HKEY_CURRENT_USER,"SDIIDEBUGGER",0,KEY_ALL_ACCESS,&hKey);
	if(!hKey) return 0;

	for(DWORD n=0;n<nRecentFilesNum;n++)
	{
		wsprintf(kn,"file%d",n);
		dwSize = sizeof(buffer);
		buffer[0] = 0;
		if(RegQueryValueEx(hKey,kn,0,0,(unsigned char *)buffer,&dwSize) == ERROR_SUCCESS)
		{
			if(stricmp(buffer,pFileName)==0) 
			{
				wsprintf(kn,"line%d",n);

				dwSize = sizeof(DWORD);
				if(RegQueryValueEx(hKey,kn,0,0,(unsigned char *)&dwLine,&dwSize) == ERROR_SUCCESS)
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

void S_DEBUG::SaveRecentFileALine(char * pFileName, long nLine)
{
	HKEY hKey;
	char buffer[MAX_PATH];
	char kn[MAX_PATH];
	DWORD dwSize;

	if(pFileName == 0) return;

	RegOpenKeyEx(HKEY_CURRENT_USER,"SDIIDEBUGGER",0,KEY_ALL_ACCESS,&hKey);
	if(!hKey) if(RegCreateKey(HKEY_CURRENT_USER,"SDIIDEBUGGER",&hKey) != ERROR_SUCCESS) return;

	for(DWORD n=0;n<nRecentFilesNum;n++)
	{
		wsprintf(kn,"file%d",n);
		dwSize = sizeof(buffer);
		buffer[0] = 0;
		if(RegQueryValueEx(hKey,kn,0,0,(unsigned char *)buffer,&dwSize) == ERROR_SUCCESS)
		{
			if(stricmp(buffer,pFileName)==0) 
			{
				wsprintf(kn,"line%d",n);
				RegSetValueEx(hKey,kn,0,REG_DWORD,(const unsigned char *)&nLine,sizeof(DWORD));	
				RegCloseKey(hKey);
				return;
			}
		}
	}
	RegCloseKey(hKey);
	return;
}

void S_DEBUG::OpenNewFile()
{
	char buffer[1024], wintext[1024];
	if(CDebug.BrowseFile(buffer,filefilter))
	{
		strcpy(CDebug.sLastFileName,buffer);
		CDebug.SourceView->OpenSourceFile(buffer);
		sprintf(wintext,"SDebug - %s",buffer);
		SetWindowText(CDebug.GetWindowHandle(), wintext);
		CDebug.Add2RecentFiles(buffer);
	}
}

#endif