#include "s_dbg_sourceview.h"
#include "core.h"
#include "data.h"
#include "defines.h"
#include "resource.h"
#include "s_debug.h"
#include <algorithm>

extern S_DEBUG CDebug;

#define X_OFFSET 0 // 16

#define WRGB(r, g, b) ((COLORREF)(((uint8_t)(r) | ((uint16_t)((uint8_t)(g)) << 8)) | (((uint32_t)(uint8_t)(b)) << 16)))

const wchar_t *SVClass = L"Source View";

extern int FONT_HEIGHT;
bool SelectionInProgress = false;

void SOURCE_VIEW::DoStep(long iCount)
{
    if (nActiveLine == 0xFFFFFFFF)
    {
        SetActiveLine(nTopLine);
        return;
    }

    int iNewActiveLine = nActiveLine + iCount;
    if (iNewActiveLine < 0)
        iNewActiveLine = 0;
    if (iNewActiveLine >= nLinesNum)
        iNewActiveLine = static_cast<int>(nLinesNum - 1);

    if (iNewActiveLine < nTopLine)
        LineUpDown(false, nTopLine - iNewActiveLine);

    if (iNewActiveLine >= nTopLine + nClientLinesSize)
        LineUpDown(true, 1 + iNewActiveLine - (nTopLine + nClientLinesSize));

    SetActiveLine(iNewActiveLine);
}

LRESULT CALLBACK SourceViewWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    int fwKeys, zDelta;
    int xPos, yPos;
    uint32_t aline;
    RECT SelectionRect;

    if (CDebug.SourceView)
        switch (iMsg)
        {
        case WM_SYSKEYDOWN:
            switch (static_cast<int>(wParam))
            {
            case VK_F10:
                CDebug.SetTraceMode(TMODE_MAKESTEP_OVER);
                break;
            }
            return 0;
        case WM_KEYDOWN:
            switch (static_cast<int>(wParam))
            {
            case VK_HOME:
                if (GetAsyncKeyState(VK_CONTROL) < 0)
                    CDebug.SourceView->SetActiveLine(0);
                break;
            case VK_END:
                if (GetAsyncKeyState(VK_CONTROL) < 0)
                    CDebug.SourceView->SetActiveLine(CDebug.SourceView->nLinesNum - 1);
                break;
            case 'G':
                break;
            case 'F':
                if (GetAsyncKeyState(VK_CONTROL) < 0)
                    CDebug.SourceView->FindModal();
                break;
            case 'O':
                if (GetAsyncKeyState(VK_CONTROL) < 0)
                    CDebug.OpenNewFile();
                break;
            case VK_F2:
                if (GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState(VK_SHIFT) < 0)
                {
                    CDebug.SourceView->ClearAllBookmarks();
                    break;
                }
                if (GetAsyncKeyState(VK_CONTROL) < 0)
                {
                    CDebug.SourceView->ToogleBookmark();
                    break;
                }
                CDebug.SourceView->GoNextBookmark();
                break;
            case VK_F3:
                CDebug.SourceView->FindNext();
                break;
            case VK_UP:
                CDebug.SourceView->DoStep(-1);
                break;
            case VK_DOWN:
                CDebug.SourceView->DoStep(1);
                break;
            case VK_PRIOR:
                CDebug.SourceView->DoStep(-CDebug.SourceView->nClientLinesSize);
                break;
            case VK_NEXT:
                CDebug.SourceView->DoStep(CDebug.SourceView->nClientLinesSize);
                break;
            case VK_F4:
                if (!CDebug.SourceView->bDrag)
                    CDebug.WatcherList->StartEditSelectedItem();
                break;
            case VK_F9:
                CDebug.Breaks.FlipBreakPoint(CDebug.SourceView->SourceFileName, CDebug.SourceView->nActiveLine);
                SelectionRect = CDebug.SourceView->Pos;
                SelectionRect.top =
                    (CDebug.SourceView->nActiveLine - CDebug.SourceView->nTopLine) * CDebug.SourceView->nFontHeight;
                SelectionRect.bottom = SelectionRect.top + CDebug.SourceView->nFontHeight;
                InvalidateRect(hwnd, &SelectionRect, true);
                break;
            case VK_F10:
                CDebug.SetTraceMode(TMODE_MAKESTEP_OVER);
                break;
            case VK_F11:
                CDebug.SetTraceMode(TMODE_MAKESTEP);
                break;
            case VK_F5:
                CDebug.SetTraceMode(TMODE_CONTINUE);
                ShowWindow(CDebug.hMain, SW_MINIMIZE);
                // SetFocus(Core.App_Hwnd);
                break;
            case 'C':
                if (CDebug.SourceView->nEndSelection == CDebug.SourceView->nStartSelection)
                    break;
                if (OpenClipboard(nullptr)) // hwnd))
                {
                    EmptyClipboard();
                    long dwBytes;

                    dwBytes = abs(static_cast<long>(CDebug.SourceView->nEndSelection) -
                                  static_cast<long>(CDebug.SourceView->nStartSelection)) +
                              1;

                    auto *hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, dwBytes);

                    auto *lptstrCopy = static_cast<char *>(GlobalLock(hMem));
                    if (CDebug.SourceView->nEndSelection > CDebug.SourceView->nStartSelection)
                        memcpy(lptstrCopy,
                               CDebug.SourceView->pSourceFile +
                                   CDebug.SourceView->pLineOffset[CDebug.SourceView->nActiveLine] +
                                   CDebug.SourceView->nStartSelection,
                               dwBytes - 1);
                    else
                        memcpy(lptstrCopy,
                               CDebug.SourceView->pSourceFile +
                                   CDebug.SourceView->pLineOffset[CDebug.SourceView->nActiveLine] +
                                   CDebug.SourceView->nEndSelection,
                               dwBytes - 1);
                    lptstrCopy[dwBytes - 1] = 0;
                    GlobalUnlock(hMem);

                    // memcpy(hMem,CDebug.SourceView->pSourceFile +
                    // CDebug.SourceView->pLineOffset[CDebug.SourceView->nActiveLine] +
                    // CDebug.SourceView->nStartSelection,dwBytes-1);
                    //((char *)hMem)[dwBytes-1] = 0;

                    SetClipboardData(CF_TEXT, hMem);
                    CloseClipboard();
                }
                break;
            }
            break;

        case WM_MOUSEMOVE:
            if (CDebug.SourceView->bDrag)
            {
                POINT pnt;
                GetCursorPos(&pnt);

                auto bPointerInWatchWindow = WindowFromPoint(pnt) == CDebug.WatcherList->GetWindowHandle();
                SetCursor(LoadCursor(CDebug.SourceView->hInst, (bPointerInWatchWindow)
                                                                   ? MAKEINTRESOURCE(IDC_DRAGPOINTER)
                                                                   : MAKEINTRESOURCE(IDC_DRAGNODROP)));

                if (bPointerInWatchWindow)
                {
                    ListView_SetItemState(CDebug.WatcherList->GetWindowHandle(), -1, 0, LVIS_SELECTED);
                    long iItemCount = ListView_GetItemCount(CDebug.WatcherList->GetWindowHandle());
                    ScreenToClient(CDebug.WatcherList->GetWindowHandle(), &pnt);
                    for (long i = 0; i < iItemCount; i++)
                    {
                        RECT rect;
                        auto bRes = ListView_GetItemRect(CDebug.WatcherList->GetWindowHandle(), i, &rect, LVIR_BOUNDS);
                        if (PtInRect(&rect, pnt))
                        {
                            ListView_SetItemState(CDebug.WatcherList->GetWindowHandle(), i, LVIS_SELECTED,
                                                  LVIS_SELECTED);
                            break;
                        }
                    }
                }

                return 0;
            }
            // SetFocus(CDebug.SourceView->hOwn);
            if (!SelectionInProgress)
                break;
            xPos = LOWORD(lParam);
            yPos = HIWORD(lParam);
            CDebug.SourceView->DetCursorPos(xPos, yPos);

            // CDebug.SourceView->InvalidateLineSection(CDebug.SourceView->nActiveLine,CDebug.SourceView->nStartSelection,CDebug.SourceView->nEndSelection);

            CDebug.SourceView->nEndSelection = CDebug.SourceView->Cursor.collumn;

            // CDebug.SourceView->InvalidateLineSection(CDebug.SourceView->nActiveLine,CDebug.SourceView->nStartSelection,CDebug.SourceView->nEndSelection);

            // CDebug.SourceView->MoveSelection(xPos);
            /*SelectionRect = CDebug.SourceView->Pos;
            SelectionRect.top = (CDebug.SourceView->nActiveLine - CDebug.SourceView->nTopLine) *
            CDebug.SourceView->nFontHeight; SelectionRect.bottom = SelectionRect.top + CDebug.SourceView->nFontHeight;
            InvalidateRect(hwnd,&SelectionRect,true);*/

            break;
        case WM_LBUTTONUP:
            xPos = LOWORD(lParam);
            yPos = HIWORD(lParam);

            if (CDebug.SourceView->bDrag)
            {
                CDebug.SourceView->bDrag = false;
                ReleaseCapture();
                SetCursor(LoadCursor(nullptr, IDC_ARROW));

                if (abs(CDebug.SourceView->pntDragPos.x - xPos) < 3 && abs(CDebug.SourceView->pntDragPos.y - yPos) < 3)
                {
                    CDebug.SourceView->nStartSelection = -1;
                    CDebug.SourceView->nEndSelection = -1;

                    InvalidateRect(hwnd, &CDebug.SourceView->CopyPasteRect, true);
                }

                POINT pnt;
                GetCursorPos(&pnt);

                if (CDebug.SourceView->sCopyPasteBuffer.size() &&
                    WindowFromPoint(pnt) == CDebug.WatcherList->GetWindowHandle())
                {
                    long iItemCount = ListView_GetItemCount(CDebug.WatcherList->GetWindowHandle());
                    for (long i = 0; i < iItemCount; i++)
                    {
                        if (ListView_GetItemState(CDebug.WatcherList->GetWindowHandle(), i, LVIS_SELECTED) &
                            LVIS_SELECTED)
                        {
                            CDebug.WatcherList->SetItemText(i, 0, (char *)CDebug.SourceView->sCopyPasteBuffer.c_str());
                            CDebug.WatcherList->ItemChanged(i, 0);
                            CDebug.SourceView->sCopyPasteBuffer.clear();
                            break;
                        }
                    }
                }

                break;
            }

            // CDebug.SourceView->MoveSelection(xPos);
            SelectionInProgress = false;
            CDebug.SourceView->DetCursorPos(xPos, yPos);

            break;
        case WM_LBUTTONDOWN:
            xPos = LOWORD(lParam);
            yPos = HIWORD(lParam);

            {
                auto &r = CDebug.SourceView->CopyPasteRect;
                auto bCursorInSelection = r.top < yPos && r.bottom > yPos && r.left < xPos && r.right > xPos;
                if (bCursorInSelection)
                {
                    CDebug.SourceView->pntDragPos.x = xPos;
                    CDebug.SourceView->pntDragPos.y = yPos;
                    SetCapture(CDebug.SourceView->hOwn);
                    SetFocus(CDebug.SourceView->hOwn);
                    CDebug.SourceView->bDrag = true;
                    SetCursor(LoadCursor(CDebug.SourceView->hInst, MAKEINTRESOURCE(IDC_DRAGNODROP)));
                    break;
                }
            }

            SelectionInProgress = true;
            SetFocus(hwnd);
            CDebug.SourceView->Cursor.collumn = 0;
            CDebug.SourceView->Cursor.x_pos = 0;

            if (CDebug.SourceView->Cursor.line >= CDebug.SourceView->nTopLine &&
                CDebug.SourceView->Cursor.line <= CDebug.SourceView->nTopLine + CDebug.SourceView->nClientLinesSize)
            {
                SelectionRect = CDebug.SourceView->Pos;
                SelectionRect.top =
                    (CDebug.SourceView->Cursor.line - CDebug.SourceView->nTopLine) * CDebug.SourceView->nFontHeight;
                SelectionRect.bottom = SelectionRect.top + CDebug.SourceView->nFontHeight;
                InvalidateRect(hwnd, &SelectionRect, true);
            }

            if (CDebug.SourceView->nLinesNum == 0)
            {
                CDebug.SourceView->nActiveLine = 0xffffffff;
                break;
            }
            aline = CDebug.SourceView->nTopLine + yPos / CDebug.SourceView->nFontHeight;
            if (aline >= CDebug.SourceView->nLinesNum)
                CDebug.SourceView->nActiveLine = CDebug.SourceView->nLinesNum - 1;
            if (aline != CDebug.SourceView->nActiveLine)
            {
                SelectionRect = CDebug.SourceView->Pos;
                SelectionRect.top =
                    (CDebug.SourceView->nActiveLine - CDebug.SourceView->nTopLine) * CDebug.SourceView->nFontHeight;
                SelectionRect.bottom = SelectionRect.top + CDebug.SourceView->nFontHeight;
                InvalidateRect(hwnd, &SelectionRect, true);

                CDebug.SourceView->nActiveLine = aline;

                SelectionRect = CDebug.SourceView->Pos;
                SelectionRect.top =
                    (CDebug.SourceView->nActiveLine - CDebug.SourceView->nTopLine) * CDebug.SourceView->nFontHeight;
                SelectionRect.bottom = SelectionRect.top + CDebug.SourceView->nFontHeight;
                InvalidateRect(hwnd, &SelectionRect, true);
            }
            // CDebug.SourceView->StartSelection(xPos);
            CDebug.SourceView->DetCursorPos(xPos, yPos);
            CDebug.SourceView->nStartSelection = CDebug.SourceView->Cursor.collumn;
            CDebug.SourceView->nEndSelection = CDebug.SourceView->Cursor.collumn;
            CDebug.SourceView->Cursor.line = CDebug.SourceView->nActiveLine;
            break;
        case WM_PAINT:
            CDebug.SourceView->OnPaint();
            // SetFocus(CDebug.SourceView->hOwn);
            break;
        case WM_SIZE:
            break;
            // case WM_MOUSEWHEEL:
        case 0x020A:
            fwKeys = LOWORD(wParam);                     // key flags
            zDelta = static_cast<short>(HIWORD(wParam)); // wheel rotation
            // xPos = (short) LOWORD(lParam);    // horizontal position of pointer
            // yPos = (short) HIWORD(lParam);    // vertical position of pointer
            if (zDelta > 0)
                CDebug.SourceView->LineUpDown(false, 3);
            else
                CDebug.SourceView->LineUpDown(true, 3);
            return 0;
        case WM_VSCROLL:
            auto nScrollCode = static_cast<int>(LOWORD(wParam)); // scroll bar value
            int nPos = static_cast<short>(HIWORD(wParam));       // scroll box position
            auto *hwndScrollBar = (HWND)lParam;                  // handle to scroll bar

            switch (nScrollCode)
            {
            case SB_PAGEUP:
            case SB_PAGEDOWN:
                CDebug.SourceView->nTopLine +=
                    (CDebug.SourceView->nClientLinesSize - 1) * ((nScrollCode == SB_PAGEDOWN) ? 1 : -1);
                if (static_cast<long>(CDebug.SourceView->nTopLine) < 0)
                    CDebug.SourceView->nTopLine = 0;
                if (CDebug.SourceView->nTopLine >= CDebug.SourceView->nLinesNum - CDebug.SourceView->nClientLinesSize)
                    CDebug.SourceView->nTopLine = CDebug.SourceView->nLinesNum - CDebug.SourceView->nClientLinesSize;
                CDebug.SourceView->UpdateGDIControls();
                InvalidateRect(hwnd, nullptr, true);
                break;
            case SB_LINEDOWN:
                CDebug.SourceView->LineUpDown(true);
                break;
            case SB_LINEUP:
                CDebug.SourceView->LineUpDown(false);
                break;
            case SB_THUMBTRACK:
                if (CDebug.SourceView->nTopLine == static_cast<uint32_t>(nPos))
                    break;
            case SB_THUMBPOSITION:
                CDebug.SourceView->nTopLine = nPos;
                if (CDebug.SourceView->nTopLine < 0)
                    CDebug.SourceView->nTopLine = 0; //~!~
                if (CDebug.SourceView->nTopLine >= CDebug.SourceView->nLinesNum - CDebug.SourceView->nClientLinesSize)
                    CDebug.SourceView->nTopLine = CDebug.SourceView->nLinesNum - CDebug.SourceView->nClientLinesSize;
                CDebug.SourceView->UpdateGDIControls();
                InvalidateRect(hwnd, nullptr, true);
                break;
            }

            break;
        }
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

char SOURCE_VIEW::cDelimTable[256];

void SOURCE_VIEW::SetCharacterMap(char *pMap, const char *pStr)
{
    PZERO(pMap, 256);
    const uint32_t dwLen = strlen(pStr);
    for (uint32_t i = 0; i < dwLen; i++)
        pMap[pStr[i]] = true;
}

SOURCE_VIEW::SOURCE_VIEW(HWND _hMain, HINSTANCE _hInst)
{
    SetCharacterMap(cDelimTable, "`~!@#$%^&*()-=+[],.?><\"\\/|{};': ");

    bDrag = false;
    ProgramDirectory[0] = 0;
    SourceFileName[0] = 0;
    nActiveLine = 0xffffffff;
    nControlLine = 0xffffffff;
    nTopLine = 0;
    nLinesNum = 0;
    nSourceFileSize = 0;
    pSourceFile = nullptr;
    hFont = nullptr;
    hInst = _hInst;
    hMain = _hMain;
    Pos.left = Pos.top = 0;
    Pos.right = Pos.bottom = 20;
    Pos.top = 200;
    Pos.right = 900;
    Pos.bottom = 600;
    nFontHeight = FONT_HEIGHT;
    nClientLinesSize = 0;
    nStartSelection = 0;
    nEndSelection = 0;
    Cursor.collumn = 0;
    Cursor.line = 0;
    Cursor.x_pos = 0;

    CopyPasteRect.left = 0;
    CopyPasteRect.right = 0;
    CopyPasteRect.top = 0;
    CopyPasteRect.bottom = 0;

    WNDCLASSEX wndclass;

    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wndclass.lpfnWndProc = SourceViewWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = sizeof(uint16_t);
    wndclass.hInstance = _hInst;
    wndclass.hIcon = nullptr;
    wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndclass.hbrBackground = static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH));
    wndclass.lpszMenuName = nullptr;
    wndclass.lpszClassName = SVClass;
    wndclass.hIconSm = nullptr; // LoadIcon(NULL,IDI_APPLICATION);
    RegisterClassEx(&wndclass);

    hOwn = CreateWindowEx(WS_EX_CLIENTEDGE, SVClass, SVClass, WS_CHILD | WS_VISIBLE | WS_VSCROLL, 0, 0,
                          Pos.right - Pos.left, Pos.bottom - Pos.top, hMain, nullptr, hInst, nullptr);
    if (hOwn == nullptr)
        throw "cant create source view";

    SetFocus(hOwn);
    // OpenSourceFile("program\\ps.c");
    // OpenSourceFile("program\\seadogs.c");

    auto pI = fio->OpenIniFile(PROJECT_NAME);
    if (pI)
    {
        char buffer[1024];
        if (pI->ReadString("bookmarks", "BM", buffer, sizeof(buffer), ""))
            do
            {
                htBookmarks[buffer] = static_cast<uint32_t>(0);
            } while (pI->ReadStringNext("bookmarks", "BM", buffer, sizeof(buffer)));
    }
}

SOURCE_VIEW::~SOURCE_VIEW()
{
    delete pSourceFile;

    auto pI = fio->OpenIniFile(PROJECT_NAME);
    if (pI)
    {
        pI->DeleteSection("bookmarks");
        /*hable<uint32_t>::iterator htIter(htBookmarks);

        int n = 0;
        for (htIter.Begin(); !htIter.IsDone(); htIter.Next())
        {
          const char * pName = htIter.GetName();
          pI->AddString("bookmarks", "BM", (char*)pName);
        }*/
        for (const auto &bookmark : htBookmarks)
            pI->AddString("bookmarks", "BM", (char *)bookmark.first.c_str());
    }
}

void SOURCE_VIEW::SetPosition(RECT _Pos)
{
    Pos = _Pos;
    if (hOwn)
        MoveWindow(hOwn, Pos.left, Pos.top, Pos.right - Pos.left, Pos.bottom - Pos.top, true);

    nClientLinesSize = (Pos.bottom - Pos.top) / nFontHeight;
    UpdateGDIControls();
}

void SOURCE_VIEW::SetFont(HFONT _hFont)
{
    hFont = _hFont;
}

void SOURCE_VIEW::ProcessMessage(uint32_t iMsg, uint32_t wParam, uint32_t lParam)
{
    /*switch(iMsg)
    {
      case WM_PAINT: OnPaint(); break;
    }*/
}

bool SOURCE_VIEW::OpenSourceFile(const char *_filename)
{
    ShowWindow(hMain, SW_NORMAL);

    if (_stricmp(SourceFileName, _filename) == 0)
    {
        return true;
    }

    if (SourceFileName[0] != 0)
    {
        CDebug.SaveRecentFileALine(SourceFileName, nActiveLine);
    }

    char DirectoryName[MAX_PATH];
    fio->_GetCurrentDirectory(sizeof(DirectoryName), DirectoryName);

    strcat_s(DirectoryName, "\\");
    strcat_s(DirectoryName, ProgramDirectory);
    strcat_s(DirectoryName, "\\");
    strcat_s(DirectoryName, _filename);

    auto fileS = fio->_CreateFile(DirectoryName, std::ios::binary | std::ios::in);
    if (!fileS.is_open())
    {
        return false;
    }
    const uint32_t nDataSize = fio->_GetFileSize(DirectoryName);

    nTopLine = 0;
    delete pSourceFile;
    nSourceFileSize = 0;
    nLinesNum = 0;
    nActiveLine = 0xffffffff;

    pSourceFile = new char[nDataSize + 1];
    const auto readSuccess = fio->_ReadFile(fileS, pSourceFile, nDataSize);
    fio->_CloseFile(fileS);
    if (!readSuccess)
    {
        delete pSourceFile;
        pSourceFile = nullptr;
        return false;
    }
    pSourceFile[nDataSize] = 0;
    nSourceFileSize = nDataSize;

    nLinesNum = 1;
    pLineOffset.resize(nLinesNum);
    pLineOffset[0] = 0;

    for (uint32_t n = 0; n < nDataSize; n++)
    {
        if (pSourceFile[n] == 0xd)
        {
            if (pSourceFile[n + 1] == 0xa)
                n++;
            nLinesNum++;
            pLineOffset.resize(nLinesNum);
            pLineOffset[nLinesNum - 1] = n + 1;
        }
    }
    if (nLinesNum > 0)
        nActiveLine = 0;
    pBookmarks.resize(nLinesNum);
    UpdateGDIControls();
    InvalidateRect(hOwn, nullptr, true);
    strcpy_s(SourceFileName, _filename);
    SetFocus(hOwn);

    // set bookmarks]
    const std::string sSourceFileName = SourceFileName;
    for (int n = 0; n < nLinesNum; n++)
    {
        std::string sTmp = sSourceFileName + "," + std::to_string(n);
        // if (htBookmarks.Find(sTmp, dwTmpFind))
        if (htBookmarks.count(sTmp) > 0)
            pBookmarks[n] = true;
    }

    return true;
}

void SOURCE_VIEW::UpdateGDIControls()
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = nLinesNum - 1;
    si.nPage = nClientLinesSize;
    si.nPos = nTopLine;
    SetScrollInfo(hOwn, SB_VERT, &si, true);
}

void SOURCE_VIEW::OnPaint()
{
    PAINTSTRUCT PS;
    uint32_t nFrom, nTo;

    const std::string sSourceFileName = SourceFileName;

    const HDC dc = BeginPaint(hOwn, &PS);
    const HGDIOBJ hFont_old = SelectObject(dc, hFont);

    const HBRUSH hControlBrush = CreateSolidBrush(WRGB(255, 255, 0));
    const HBRUSH hBreakBrush = CreateSolidBrush(WRGB(255, 104, 104));
    const HBRUSH hBookmarkBrush = CreateSolidBrush(WRGB(199, 243, 196));

    SetBkMode(dc, TRANSPARENT);
    // SetBkMode(dc,OPAQUE);

    SetTextColor(dc, WRGB(0, 0, 0));
    SetBkColor(dc, WRGB(0, 0, 0));

    if (nStartSelection == nEndSelection)
    {
        CopyPasteRect.left = 0;
        CopyPasteRect.right = 0;
        CopyPasteRect.top = 0;
        CopyPasteRect.bottom = 0;

        sCopyPasteBuffer.clear();
    }

    uint32_t nTextLen;
    if (pSourceFile)
    {
        uint32_t x = X_OFFSET;
        uint32_t y = 0;
        // for(n=nTopLine;n<nLinesNum;n++)
        for (uint32_t n = nTopLine; n < nTopLine + nClientLinesSize + 1; n++)
        {
            if (n >= nLinesNum)
                break;
            if (n == nLinesNum - 1)
                nTextLen = nSourceFileSize - pLineOffset[n];
            else
                nTextLen = pLineOffset[n + 1] - pLineOffset[n];

            if (n != nLinesNum - 1)
            {
                if (nTextLen < 2)
                    nTextLen = 0;
                else
                    nTextLen = nTextLen - 2;
            }

            {
                const uint32_t nLineStatus = CDebug.GetLineStatus(SourceFileName, n);
                RECT SelectionRect;
                SelectionRect = Pos;
                SelectionRect.top = y;
                SelectionRect.bottom = SelectionRect.top + nFontHeight;
                if (nLineStatus == LST_BREAKPOINT || nLineStatus == LST_CONTROL)
                {
                    if (nLineStatus == LST_BREAKPOINT)
                        FillRect(dc, &SelectionRect, hBreakBrush);
                    else
                    {
                        FillRect(dc, &SelectionRect, hControlBrush);
                        nControlLine = n;
                    }
                }
                // else if (htBookmarks.Find(sSourceFileName + "," + n, dwTmpFind)) //;CDebug.SourceView->pBookmarks[n])
                else if (htBookmarks.count(sSourceFileName + "," + std::to_string(n)) > 0)
                    FillRect(dc, &SelectionRect, hBookmarkBrush);

                if (n == nActiveLine)
                {
                    MoveToEx(dc, Pos.left, y + nFontHeight - 1, static_cast<LPPOINT>(nullptr));
                    LineTo(dc, Pos.right, y + nFontHeight - 1);
                }

                std::wstring TextW = utf8::ConvertUtf8ToWide(pSourceFile + pLineOffset[n]);
                if (nStartSelection == nEndSelection || n != Cursor.line)
                {
                    TabbedTextOut(dc, x, y, TextW.c_str(), nTextLen, 0, nullptr, 0);
                }
                else
                {
                    if (nStartSelection < nEndSelection)
                    {
                        nFrom = nStartSelection;
                        nTo = nEndSelection;
                    }
                    else
                    {
                        nFrom = nEndSelection;
                        nTo = nStartSelection;
                    }

                    long w;

                    uint32_t nPosted = 0;

                    if (nFrom > 0)
                    {
                        w = LOWORD(TabbedTextOut(dc, x, y, TextW.c_str(), nFrom, 0, nullptr, 0));
                        x += w;
                    }

                    SetBkMode(dc, OPAQUE);
                    SetTextColor(dc, WRGB(255, 255, 255));

                    CopyPasteRect.left = x;
                    w = LOWORD(TabbedTextOut(dc, x, y, TextW.c_str() + nFrom, nTo - nFrom, 0, nullptr, 0));
                    x += w;
                    CopyPasteRect.right = CopyPasteRect.left + w;
                    CopyPasteRect.top = y;
                    CopyPasteRect.bottom = y + nFontHeight;

                    char *str = new char[nTo - nFrom + 1];
                    strncpy_s(str, nTo - nFrom + 1, pSourceFile + pLineOffset[n] + nFrom, nTo - nFrom);
                    str[nTo - nFrom] = 0;
                    sCopyPasteBuffer = str;
                    delete[] str;

                    SetBkMode(dc, TRANSPARENT);

                    SetTextColor(dc, WRGB(0, 0, 0));
                    TabbedTextOut(dc, x, y, TextW.c_str() + nTo, nTextLen - nTo, 0, nullptr, 0);

                    x = 16;
                }

                if (nLineStatus == LST_BREAKPOINT || nLineStatus == LST_CONTROL)
                    SetTextColor(dc, WRGB(0, 0, 0));
            }
            y += nFontHeight;
        }
    }
    else
        TextOut(dc, 0, 0, TEXT("Source View"), wcslen(TEXT("Source View")));

    DeleteObject(hBreakBrush);
    DeleteObject(hControlBrush);
    SelectObject(dc, hFont_old);
    EndPaint(hOwn, &PS);
}

void SOURCE_VIEW::LineUpDown(bool down, uint32_t _nlines)
{
    //    RECT ClientRect;
    if (down)
    {
        if ((nTopLine + nClientLinesSize) < (nLinesNum))
        {
            if (nTopLine + nClientLinesSize + _nlines < nLinesNum)
                nTopLine += _nlines;
            else
                nTopLine = nLinesNum - nClientLinesSize;

            // nTopLine += _nlines;
            UpdateGDIControls();
            InvalidateRect(hOwn, nullptr, true);
        }
    }
    else
    {
        if (nTopLine > 0)
        {
            if (nTopLine < _nlines)
                nTopLine = 0;
            else
                nTopLine -= _nlines;
            UpdateGDIControls();
            InvalidateRect(hOwn, nullptr, true);
        }
    }
}

void SOURCE_VIEW::SetProgramDirectory(const char *dir_name)
{
    if (dir_name)
        strcpy_s(ProgramDirectory, dir_name);
}

void SOURCE_VIEW::SetActiveLine(uint32_t line)
{
    RECT SelectionRect;

    nStartSelection = 0;
    nEndSelection = 0;

    SelectionRect = Pos;
    SelectionRect.top = (nActiveLine - nTopLine) * nFontHeight;
    SelectionRect.bottom = SelectionRect.top + nFontHeight;
    InvalidateRect(hOwn, &SelectionRect, true);

    nActiveLine = line;
    if (nActiveLine >= nLinesNum)
        nActiveLine = 0;

    if (nActiveLine < nTopLine || nActiveLine >= nTopLine + nClientLinesSize)
    {
        long newtopline = static_cast<long>(nActiveLine) - static_cast<long>(nClientLinesSize) / 2;
        if (newtopline < 0)
            newtopline = 0;
        if (newtopline + nClientLinesSize >= nLinesNum)
            newtopline = nLinesNum - nClientLinesSize;
        nTopLine = newtopline;
        InvalidateRect(hOwn, nullptr, true);
        UpdateGDIControls();
    }

    SelectionRect = Pos;
    SelectionRect.top = (nActiveLine - nTopLine) * nFontHeight;
    SelectionRect.bottom = SelectionRect.top + nFontHeight;
    InvalidateRect(hOwn, &SelectionRect, true);

    SelectionRect = Pos;
    SelectionRect.top = (nControlLine - nTopLine) * nFontHeight;
    SelectionRect.bottom = SelectionRect.top + nFontHeight;
    InvalidateRect(hOwn, &SelectionRect, true);

    SetFocus(hOwn);
}

void SOURCE_VIEW::StartSelection(uint32_t x_pos)
{
    RECT RR;

    nStartSelection = 0;
    nEndSelection = 0;

    if (nActiveLine >= nLinesNum)
        return;

    const HDC dc = GetDC(hOwn);
    const HGDIOBJ hFont_old = SelectObject(dc, hFont);
    SetBkMode(dc, TRANSPARENT);

    char *pLine = pSourceFile + pLineOffset[nActiveLine];
    std::wstring LineW = utf8::ConvertUtf8ToWide(pLine);

    uint32_t nLen = 0;
    uint32_t nSymNum = 0;

    while (!(pLine[nSymNum] == 0xd || pLine[nSymNum] == 0xa || pLine[nSymNum] == 0))
    {
        // nLen = LOWORD(GetTabbedTextExtent(dc,pLine,nSymNum+1,0,0));

        BeginPath(dc);
        TabbedTextOut(dc, X_OFFSET, 0, LineW.c_str(), nSymNum + 1, 0, nullptr, 0);
        EndPath(dc);
        const HRGN r = PathToRegion(dc);
        if (r)
        {
            GetRgnBox(r, &RR);
            nLen = RR.right;
        }
        else
            nLen = 0;

        if (nLen /* + X_OFFSET*/ >= x_pos)
        {
            nStartSelection = nSymNum;
            nEndSelection = nStartSelection;
            break;
        }
        nSymNum++;
    }
    SelectObject(dc, hFont_old);
    ReleaseDC(hOwn, dc);
}

void SOURCE_VIEW::MoveSelection(uint32_t x_pos)
{
    RECT RR;

    if (nActiveLine >= nLinesNum)
        return;

    const HDC dc = GetDC(hOwn);
    const HGDIOBJ hFont_old = SelectObject(dc, hFont);
    SetBkMode(dc, TRANSPARENT);

    char *pLine = pSourceFile + pLineOffset[nActiveLine];
    std::wstring LineW = utf8::ConvertUtf8ToWide(pLine);

    uint32_t nLen = 0;
    uint32_t nSymNum = 0;

    while (!(pLine[nSymNum] == 0xd || pLine[nSymNum] == 0xa || pLine[nSymNum] == 0))
    {
        // nLen = LOWORD(GetTabbedTextExtent(dc,pLine,nSymNum+1,0,0));

        BeginPath(dc);
        TabbedTextOut(dc, X_OFFSET, 0, LineW.c_str(), nSymNum + 1, 0, nullptr, 0);
        EndPath(dc);
        const HRGN r = PathToRegion(dc);
        if (r)
        {
            GetRgnBox(r, &RR);
            nLen = RR.right;
        }
        else
            nLen = 0;

        if (nLen /*+ X_OFFSET*/ >= x_pos)
        {
            nEndSelection = nSymNum;
            break;
        }
        nSymNum++;
    }
    SelectObject(dc, hFont_old);
    ReleaseDC(hOwn, dc);
}

void SOURCE_VIEW::InvalidateLineSection(uint32_t line, uint32_t r1, uint32_t r2)
{
    uint32_t from, to;
    //    long x_diff;

    if (line >= nLinesNum)
        return;
    if (r1 == r2)
        return;
    if (r1 > r2)
    {
        from = r2;
        to = r1;
    }
    else
    {
        from = r1;
        to = r2;
    }

    const HDC dc = GetDC(hOwn);
    const HGDIOBJ hFont_old = SelectObject(dc, hFont);
    SetBkMode(dc, TRANSPARENT);

    char *pLine = pSourceFile + pLineOffset[line]; // + old_pos.collumn;
    std::wstring LineW = utf8::ConvertUtf8ToWide(pLine);

    uint32_t nSymNum = 0;
    uint32_t nLen = 0;

    RECT SelectionRect = Pos;
    SelectionRect.top = (line - nTopLine) * nFontHeight;
    SelectionRect.bottom = SelectionRect.top + nFontHeight;
    SelectionRect.left = LOWORD(GetTabbedTextExtent(dc, LineW.c_str(), r1 + 1, 0, nullptr));
    SelectionRect.right = LOWORD(GetTabbedTextExtent(dc, LineW.c_str(), r2 + 2, 0, nullptr));

    InvalidateRect(hOwn, &SelectionRect, true);

    // SetBkMode(dc,OPAQUE);
    // TabbedTextOut(dc,0,0,pSourceFile + pLineOffset[nActiveLine],Cursor.collumn + 1,0,0,0);

    SelectObject(dc, hFont_old);
    ReleaseDC(hOwn, dc);
}

// work only for active line yet
void SOURCE_VIEW::DetCursorPos(uint32_t x_pos, uint32_t y_pos)
{
    //    long x_diff;

    if (nActiveLine >= nLinesNum)
        return;

    CURSOR_POS old_pos = Cursor;

    const HDC dc = GetDC(hOwn);
    const HGDIOBJ hFont_old = SelectObject(dc, hFont);
    SetBkMode(dc, TRANSPARENT);

    char *pLine = pSourceFile + pLineOffset[nActiveLine]; // + old_pos.collumn;
    std::wstring LineW = utf8::ConvertUtf8ToWide(pLine);

    uint32_t nSymNum = 0;
    uint32_t nLen = 0;

    while (!(pLine[nSymNum] == 0xd || pLine[nSymNum] == 0xa || pLine[nSymNum] == 0))
    {
        nLen = LOWORD(GetTabbedTextExtent(dc, LineW.c_str(), nSymNum + 1, 0, nullptr));
        if (nLen + X_OFFSET /*+ old_pos.x_pos*/ >= x_pos)
        {
            // nEndSelection = nSymNum;
            Cursor.collumn = /*old_pos.collumn + */ nSymNum;
            Cursor.x_pos = nLen /* + old_pos.x_pos*/;
            break;
        }
        Cursor.collumn = /*old_pos.collumn + */ nSymNum;
        Cursor.x_pos = nLen /* + old_pos.x_pos*/;
        nSymNum++;
        if (pLine[nSymNum] == 0xd || pLine[nSymNum] == 0xa)
        {
            Cursor.collumn = /*old_pos.collumn + */ nSymNum;
            Cursor.x_pos = nLen;
            break;
        }
    }

    RECT SelectionRect = Pos;
    SelectionRect.top = (nActiveLine - nTopLine) * nFontHeight;
    SelectionRect.bottom = SelectionRect.top + nFontHeight;
    InvalidateRect(hOwn, &SelectionRect, true);
    /*if(Cursor.x_pos != old_pos.x_pos)
    {
      if(Cursor.x_pos > old_pos.x_pos)
      {
        SelectionRect.left = old_pos.x_pos;
        SelectionRect.right = Cursor.x_pos;
      }
      else
      {
        SelectionRect.left = Cursor.x_pos;
        SelectionRect.right = old_pos.x_pos;
      }
      InvalidateRect(hOwn,&SelectionRect,true);
    }*/

    // SetBkMode(dc,OPAQUE);
    // TabbedTextOut(dc,0,0,pSourceFile + pLineOffset[nActiveLine],Cursor.collumn + 1,0,0,0);

    SelectObject(dc, hFont_old);
    ReleaseDC(hOwn, dc);
}

void SOURCE_VIEW::ToogleBookmark()
{
    if (nActiveLine < nLinesNum)
    {
        pBookmarks[nActiveLine] = !pBookmarks[nActiveLine];
        const std::string sFilename = std::string(SourceFileName) + "," + std::to_string(nActiveLine);
        if (pBookmarks[nActiveLine])
            htBookmarks[sFilename] = nActiveLine;
        else
            htBookmarks.erase(sFilename);
        InvalidateRect(hMain, nullptr, true);
    }
}

void SOURCE_VIEW::ClearAllBookmarks()
{
    for (long i = 0; i < nLinesNum; i++)
        pBookmarks[i] = false;
    htBookmarks.clear();
    InvalidateRect(hMain, nullptr, true);
}

void SOURCE_VIEW::GoNextBookmark()
{
    const int nStartLine =
        (nActiveLine < nTopLine || nActiveLine > nTopLine + nClientLinesSize) ? nTopLine : nActiveLine;
    for (long i = 1; i < nLinesNum; i++)
    {
        const int iLine = (nStartLine + i) % nLinesNum;
        if (pBookmarks[iLine])
        {
            SetActiveLine(iLine);
            return;
        }
    }
}

const char *SOURCE_VIEW::GetToken(const char *pStr, std::string &sResult)
{
    char cToken[1024];
    uint32_t dwTokenSize = 0;
    cToken[dwTokenSize] = 0;

    if (!pStr || !pStr[0])
    {
        sResult = "";
        return nullptr;
    }

    while (*pStr)
    {
        const char cSym = *pStr;

        if (cDelimTable[cSym])
        {
            if (dwTokenSize == 0)
            {
                cToken[dwTokenSize++] = cSym;
                pStr++;
                break;
            }
            break;
        }
        cToken[dwTokenSize++] = cSym;
        pStr++;
    }
    cToken[dwTokenSize] = 0;
    sResult = cToken;

    return (*pStr) ? pStr : nullptr;
}

bool SOURCE_VIEW::SetVariableOnChange(const char *pString, bool bSet)
{
    long iDigit;
    std::string sVarName, sToken, sDigit;
    const char *pStr = (char *)pString;
    VDATA *pObject = nullptr;

    pStr = GetToken(pStr, sVarName);
    if (!pStr)
    {
        pObject = static_cast<VDATA *>(core.GetScriptVariable(sVarName.c_str(), nullptr));
        if (!pObject)
            return false;
        // set VOC to alone variable
        return true;
    }
    pStr = GetToken(pStr, sToken);
    if (sToken == "[")
    {
        pStr = GetToken(pStr, sDigit);
        pStr = GetToken(pStr, sToken);
        if (sToken != "]")
        {
            return false;
        }
        auto *pV = static_cast<VDATA *>(core.GetScriptVariable(sVarName.c_str(), nullptr));
        if (!pV)
            return false;
        sscanf(sDigit.c_str(), "%d", &iDigit);
        pObject = pV->GetArrayElement(iDigit);
        pStr = GetToken(pStr, sToken);
    }
    else if (sToken == ".")
    {
        pObject = static_cast<VDATA *>(core.GetScriptVariable(sVarName.c_str(), nullptr));
    }
    else
        return false;

    if (!pObject)
        return false;
    if (sToken != ".")
    {
        // set on object
        return true;
    }

    ATTRIBUTES *pA = pObject->GetAClass();
    if (!pA)
        return false;
    ATTRIBUTES *pALast = pA->FindAClass(pA, pStr);
    if (!pALast)
        return false;

    pALast->SetBreak(bSet);

    return true;
}

INT_PTR CALLBACK VarChangeDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY: {
        HWND hwndList = GetDlgItem(hwndDlg, IDC_XLIST);
        long iNum = ListView_GetItemCount(hwndList);
        CDebug.SourceView->aStrings.clear();
        for (long i = 0; i < iNum; i++)
        {
            wchar_t str[1024];
            str[0] = 0;
            ListView_GetItemText(hwndList, i, 0, str, sizeof(str));
            std::string utf8 = utf8::ConvertWideToUtf8(str);
            std::string sValue = utf8.c_str();
            CDebug.SourceView->aStrings.push_back(sValue);
        }
    }
    break;
    case WM_INITDIALOG: {
        HWND hwndList = GetDlgItem(hwndDlg, IDC_XLIST);

        LVCOLUMN col;
        PZERO(&col, sizeof(col));
        col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
        col.fmt = LVCFMT_LEFT;
        col.cx = 378;
        // ~!~ TODO
        col.pszText = const_cast<LPWSTR>(TEXT("Variable name"));
        col.cchTextMax = wcslen(TEXT("Variable name"));
        ListView_InsertColumn(hwndList, 0, &col);
        uint32_t dwOld = ListView_GetExtendedListViewStyle(hwndList);
        ListView_SetExtendedListViewStyle(hwndList, dwOld | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

        for (long i = 0; i < CDebug.SourceView->aStrings.size(); i++)
        {
            LVITEM item;
            PZERO(&item, sizeof(item));
            item.mask = LVIF_TEXT;
            item.iItem = 0;
            item.iSubItem = 0;
            std::wstring StrW = utf8::ConvertUtf8ToWide(CDebug.SourceView->aStrings[i].c_str());
            item.pszText = const_cast<wchar_t *>(StrW.c_str());
            item.cchTextMax = CDebug.SourceView->aStrings[i].size();
            ListView_InsertItem(hwndList, &item);
        }
    }
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hwndDlg, LOWORD(wParam));
            return TRUE;
        }

        if (LOWORD(wParam) == ID_XADD)
        {
            HWND hwndEdit = GetDlgItem(hwndDlg, IDC_XEDIT);
            HWND hwndList = GetDlgItem(hwndDlg, IDC_XLIST);

            wchar_t StrW[1024];
            StrW[0] = 0;
            GetWindowText(hwndEdit, StrW, sizeof(StrW));
            if (!StrW[0])
                return false;

            std::string Str = utf8::ConvertWideToUtf8(StrW);
            if (!CDebug.SourceView->SetVariableOnChange(Str.c_str(), true))
            {
                MessageBox(hwndDlg, TEXT("Не найдена переменная"), TEXT("Ошибка"), MB_OK);
                return false;
            }

            LVITEM item;
            PZERO(&item, sizeof(item));
            item.mask = LVIF_TEXT;
            item.iItem = 0;
            item.iSubItem = 0;
            item.pszText = StrW;
            item.cchTextMax = wcslen(StrW);
            ListView_InsertItem(hwndList, &item);

            return false;
        }

        if (LOWORD(wParam) == ID_XDELETE)
        {
            wchar_t StrW[1024];
            StrW[0] = 0;

            HWND hwndList = GetDlgItem(hwndDlg, IDC_XLIST);
            long iSel = ListView_GetSelectionMark(hwndList);
            if (iSel == -1)
                return false;
            ListView_GetItemText(hwndList, iSel, 0, StrW, sizeof(StrW));
            std::string Str = utf8::ConvertWideToUtf8(StrW);
            CDebug.SourceView->SetVariableOnChange(Str.c_str(), false);
            ListView_DeleteItem(hwndList, iSel);
            return false;
        }
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK FindDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hwndDlg, LOWORD(wParam));
            return TRUE;
        }
        if (LOWORD(wParam) == IDC_EDIT1)
        {
            wchar_t StrW[1024];
            StrW[0] = 0;
            GetWindowText((HWND)lParam, StrW, sizeof(StrW));
            std::string Str = utf8::ConvertWideToUtf8(StrW);
            CDebug.SourceView->sFindStr = Str.c_str();
        }
        break;
    }
    return FALSE;

    return true;
}

void SOURCE_VIEW::VarChangeModal()
{
    DialogBox(hInst, MAKEINTRESOURCE(IDD_VARCHANGE), CDebug.GetWindowHandle(), VarChangeDialogProc);
}

void SOURCE_VIEW::FindModal()
{
    if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DBGCTRL_F), CDebug.GetWindowHandle(), FindDialogProc) == IDOK)
    {
        FindNext();
    }
}

void SOURCE_VIEW::FindNext()
{
    if (sFindStr.empty() || !nLinesNum)
        return;

    // sFindStr.Lower();
    std::transform(sFindStr.begin(), sFindStr.end(), sFindStr.begin(), tolower);

    const int nStartLine =
        (nActiveLine < nTopLine || nActiveLine > nTopLine + nClientLinesSize) ? nTopLine : nActiveLine;
    for (long i = 1; i < nLinesNum; i++)
    {
        const int iLine = (nStartLine + i) % nLinesNum;
        char str[2048];

        int nTextLen;
        if (iLine == nLinesNum - 1)
            nTextLen = nSourceFileSize - pLineOffset[iLine];
        else
            nTextLen = pLineOffset[iLine + 1] - pLineOffset[iLine];
        memcpy(str, pSourceFile + pLineOffset[iLine], nTextLen);
        str[nTextLen] = 0;
        std::string sTestStr = str;
        std::transform(sTestStr.begin(), sTestStr.end(), sTestStr.begin(), tolower);
        if (sTestStr.find(sFindStr) != std::string::npos)
        {
            SetActiveLine(iLine);
            break;
        }
    }
}
