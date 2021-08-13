#pragma once

#include "core.h"
#include <CommCtrl.h>
#include <Windows.h>

#include <string>
#include <unordered_map>
#include <vector>

LRESULT CALLBACK SourceViewWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

struct CURSOR_POS
{
    uint32_t line;
    uint32_t collumn;
    uint32_t x_pos;
};

class SOURCE_VIEW
{
    friend LRESULT CALLBACK SourceViewWndProc(HWND, UINT, WPARAM, LPARAM);
    HINSTANCE hInst;
    HWND hMain;
    RECT Pos;
    HFONT hFont;
    POINT pntDragPos;
    std::string sCopyPasteBuffer;
    bool bDrag;
    uint32_t nFontHeight;
    char *pSourceFile;
    std::vector<uint32_t> pLineOffset;
    uint32_t nLinesNum;
    uint32_t nSourceFileSize;
    int32_t nTopLine;
    int32_t nClientLinesSize;
    uint32_t nActiveLine;
    uint32_t nControlLine;
    char SourceFileName[MAX_PATH];
    char ProgramDirectory[MAX_PATH];
    uint32_t nStartSelection, nEndSelection;
    CURSOR_POS Cursor;

    void DoStep(long iCount);

  public:
    std::vector<std::string> aStrings;
    HWND hOwn;
    RECT CopyPasteRect;
    std::vector<bool> pBookmarks;
    std::string sFindStr;
    std::unordered_map<std::string, uint32_t> htBookmarks;

    SOURCE_VIEW(HWND _hMain, HINSTANCE _hInst);
    ~SOURCE_VIEW();
    void ProcessMessage(uint32_t, uint32_t, uint32_t);
    void SetPosition(RECT _Pos);
    void SetFont(HFONT);
    void OnPaint();
    bool OpenSourceFile(const char *_filename);
    void LineUpDown(bool down, uint32_t nlines = 1);
    void UpdateGDIControls();
    void SetProgramDirectory(const char *dir_name);
    void SetActiveLine(uint32_t line);
    void StartSelection(uint32_t x_pos);
    void MoveSelection(uint32_t x_pos);
    void EndSelection(uint32_t x_pos);
    void DetCursorPos(uint32_t x_pos, uint32_t y_pos);
    void InvalidateLineSection(uint32_t line, uint32_t r1, uint32_t r2);

    void ToogleBookmark();
    void ClearAllBookmarks();
    void GoNextBookmark();
    void FindModal();
    void FindNext();
    void VarChangeModal();

    static char cDelimTable[256];

    void SetCharacterMap(char *pMap, const char *pStr);
    bool SetVariableOnChange(const char *pStr, bool bSet);
    const char *GetToken(const char *pStr, std::string &sResult);
};
