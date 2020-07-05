#ifndef _SOURCE_VIEW_H_
#define _SOURCE_VIEW_H_

#ifndef _XBOX

#include <windows.h>
#include <commctrl.h>
#include "core.h"
#include "..\..\common_h\templates.h"

LRESULT CALLBACK SourceViewWndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam);

struct CURSOR_POS
{
	DWORD line;
	DWORD collumn;
	DWORD x_pos;
};

class SOURCE_VIEW
{
friend LRESULT CALLBACK SourceViewWndProc(HWND,UINT,WPARAM,LPARAM); 
	HINSTANCE hInst;
	HWND hMain;
	RECT Pos;
	HFONT hFont;
	POINT pntDragPos;
	string sCopyPasteBuffer;
	bool bDrag;
	DWORD nFontHeight;
	char * pSourceFile;
	DWORD * pLineOffset;
	DWORD nLinesNum;
	DWORD nSourceFileSize;
	DWORD nTopLine;
	DWORD nClientLinesSize;
	DWORD nActiveLine;
	DWORD nControlLine;
	char SourceFileName[MAX_PATH];
	char ProgramDirectory[MAX_PATH];
	DWORD nStartSelection,nEndSelection;
	CURSOR_POS Cursor;

	void DoStep(long iCount);

public:
	array<string> aStrings;
	HWND hOwn;
	RECT CopyPasteRect;
	bool * pBookmarks;
	string sFindStr;
	htable<dword>	htBookmarks;

	 SOURCE_VIEW(HWND _hMain, HINSTANCE _hInst);
	~SOURCE_VIEW();
	void ProcessMessage(DWORD,DWORD,DWORD);
	void SetPosition(RECT _Pos);
	void SetFont(HFONT);
	void OnPaint();
	bool OpenSourceFile(const char * _filename);
	void LineUpDown(bool down,DWORD _nlines = 1);
	void UpdateGDIControls();
	void SetProgramDirectory(char * dir_name);
	void SetActiveLine(DWORD line);
	void StartSelection(DWORD x_pos);
	void MoveSelection(DWORD x_pos);
	void EndSelection(DWORD x_pos);
	void DetCursorPos(DWORD x_pos, DWORD y_pos);
	void InvalidateLineSection(DWORD line, DWORD r1, DWORD r2);


	void ToogleBookmark();
	void ClearAllBookmarks();
	void GoNextBookmark();
	void FindModal();
	void FindNext();
	void VarChangeModal();

	static char cDelimTable[256];

	void SetCharacterMap(char * pMap, char * pStr);
	bool SetVariableOnChange(const char * pStr, bool bSet);
	char * GetToken(char * pStr, string & sResult);
};

#endif

#endif