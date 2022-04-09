#include "tm_list.h"

#include "defines.h"
#include "s_debug.h"

#include <windows.h>

#define MAX_STR_SIZE 1024
extern S_DEBUG * CDebug;

static char TM_LIST_Buffer[MAX_STR_SIZE];

TM_LIST::TM_LIST()
{
    hInst = nullptr;
    hMain = nullptr;
    hOwn = nullptr;
    hEdit = nullptr;
    PZERO(&Pos, sizeof(Pos));
    Columns_Num = 0;
    Items_Num = 0;
    Bind_Mask = 0;
    edit_item = -1;
    edit_subitem = -1;
    PZERO(CharID, sizeof(CharID));
    EditMask = 0;
    hFont = nullptr;
}

TM_LIST::~TM_LIST()
{
    if (hOwn)
        DestroyWindow(hOwn);
    if (hEdit)
        DestroyWindow(hEdit);
    hEdit = nullptr;
}

void TM_LIST::Initialize(HWND hwnd, HINSTANCE hinst, uint32_t style, uint32_t style_ex)
{
    hInst = hinst;
    hMain = hwnd;

    INITCOMMONCONTROLSEX icc;
    PZERO(&icc, sizeof(icc));
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icc);

    hOwn = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, TEXT(""), WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS, 0, 0,
                          CW_USEDEFAULT, CW_USEDEFAULT, hMain, nullptr, hInst, nullptr);
    if (hOwn == nullptr)
        throw std::runtime_error("cant create list view");
    // ListView_SetExtendedListViewStyle(hOwn,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES);
    ListView_SetExtendedListViewStyle(hOwn, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    ShowWindow(hOwn, SW_SHOWNORMAL);
    SetFocus(hOwn);
    // ListView_SetBkColor(hOwn,0xff);
    // ListView_SetTextColor(hOwn,0xff);
}

void TM_LIST::SetPosition(RECT pos)
{
    Pos = pos;
    if (hOwn)
        MoveWindow(hOwn, Pos.left, Pos.top, Pos.right - Pos.left, Pos.bottom - Pos.top, true);
}

void TM_LIST::Initialize(HWND hwnd, HINSTANCE hinst, uint32_t style, uint32_t style_ex, uint32_t icon_rid)
{
    Initialize(hwnd, hinst, style, style_ex);

    /*    HICON hiconItem;        // icon for list view items

        HIMAGELIST himlSmall;   // image list for other views

        himlSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON),
            GetSystemMetrics(SM_CYSMICON), ILC_COLOR8, 1, 1);
         // Add an icon to each image list.
        hiconItem = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
        ImageList_AddIcon(himlSmall, hiconItem);

      DeleteObject(hiconItem);

      hiconItem = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONCONNECTED));
        ImageList_AddIcon(himlSmall, hiconItem);
      DeleteObject(hiconItem);

      hiconItem = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONHAVEDESC));
        ImageList_AddIcon(himlSmall, hiconItem);
      DeleteObject(hiconItem);




        // Assign the image lists to the list view control.
        ListView_SetImageList(hOwn, himlSmall, LVSIL_SMALL);

      DeleteObject(himlSmall);
    */

    ShowWindow(hOwn, SW_SHOWNORMAL);
}

void TM_LIST::AddColumn(const char *name, int32_t length)
{
    LVCOLUMN lvc;

    wchar_t string[MAX_STR_SIZE];
    PZERO(&lvc, sizeof(lvc));

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; // | LVCF_ORDER ;
    lvc.fmt = LVCFMT_LEFT;
    lvc.cx = length;
    lvc.pszText = string;
    // lvc.iOrder = Columns_Num;
    if (name)
    {
        std::wstring NameW = utf8::ConvertUtf8ToWide(name);
        wcscpy(string, NameW.c_str());
    }
    else
        wcscpy(string, L"");
    lvc.iSubItem = Columns_Num;
    if (ListView_InsertColumn(hOwn, Columns_Num, &lvc) == -1)
        throw std::runtime_error("cant add column");
    Columns_Num++;
}

void TM_LIST::AddItem(const char *name)
{
    LVITEM lvi;
    wchar_t string[MAX_STR_SIZE];
    lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
    lvi.state = 0;
    lvi.stateMask = 0;
    lvi.pszText = string;
    lvi.iImage = 0;
    lvi.iItem = Items_Num;
    lvi.iSubItem = 0;
    lvi.lParam = 0;
    if (name)
    {
        std::wstring NameW = utf8::ConvertUtf8ToWide(name);
        wcscpy(string, NameW.c_str());
    }
    else
        wcscpy(string, L"");
    ListView_InsertItem(hOwn, &lvi);
    Items_Num++;
}

void TM_LIST::SetItemText(int32_t Item_index, int32_t Subitem_index, const char *text)
{
    if (text)
    {
        std::wstring TextW = utf8::ConvertUtf8ToWide(text);
        ListView_SetItemText(hOwn, Item_index, Subitem_index, const_cast<wchar_t *>(TextW.c_str()));
    }
}

void TM_LIST::GetItemText(int32_t Item_index, int32_t Subitem_index, const char *text, int32_t max_size)
{
    if (text)
    {
        std::wstring TextW = utf8::ConvertUtf8ToWide(text);
        ListView_GetItemText(hOwn, Item_index, Subitem_index, const_cast<wchar_t *>(TextW.c_str()), max_size);
    }
}

// if (text) for (int32_t i=0, j=0; i<max_size && text[i]; i++) if (text[i] != '\n') text[j++] = text[i];

void TM_LIST::SetItemImage(int32_t Item_index, int32_t Subitem_index, int32_t image_code)
{
    LVITEM lvi;
    if (image_code < 0)
        return;
    PZERO(&lvi, sizeof(lvi));
    lvi.mask = LVIF_IMAGE;
    lvi.iItem = Item_index;
    lvi.iSubItem = Subitem_index;
    lvi.iImage = image_code;
    ListView_SetItem(hOwn, &lvi);
}

void TM_LIST::UpdatePosition()
{
    if (!hOwn)
        return;

    RECT r;
    GetClientRect(hMain, &r);
    const uint32_t width = r.right - r.left;
    const uint32_t height = r.bottom - r.top;
    // MoveWindow(hOwn,Pos.left,Pos.top,width - Pos.left,height - Pos.top,true);

    if (Bind_Mask & BM_BIND_LEFT)
        Pos.left = 0;
    if (Bind_Mask & BM_BIND_TOP)
        Pos.top = 0;
    if (Bind_Mask & BM_BIND_RIGHT)
        Pos.right = width;
    if (Bind_Mask & BM_BIND_BOTTOM)
        Pos.bottom = height;

    MoveWindow(hOwn, Pos.left, Pos.top, Pos.right - Pos.left, Pos.bottom - Pos.top, true);
}

void TM_LIST::SelectItem(const char *name)
{
    if (!name)
        return;
    const auto items = GetItemsCount();
    for (int32_t n = 0; n < items; n++)
    {
        GetItemText(n, 0, SearchName, sizeof(SearchName));
        if (storm::iEquals(SearchName, name))
        {
            ListView_SetItemState(GetWindowHandle(), n, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            return;
        }
    }
    ListView_SetItemState(GetWindowHandle(), 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

void TM_LIST::ProcessMessageBase(uint64_t iMsg, uint64_t wParam, uint64_t lParam)
{
    LPNMHDR pnmh;
    // POINT point;
    // HMENU hSubMenu;
    LPNMLISTVIEW lpnmlv;
    RECT item_rect;
    uint32_t NotifyCode;
    char TextEditBuffer[MAX_STR_SIZE]{};
    uint16_t vKey;

    RECT EditPos;

    switch (iMsg)
    {
    case WM_SYSKEYDOWN:
        if (wParam == VK_F10)
        {
            CDebug->SetTraceMode(TMODE_MAKESTEP_OVER);
        }
        break;
    case WM_KEYDOWN:
        if (wParam == VK_F4)
            StartEditSelectedItem();
        if (wParam == VK_F10)
        {
            CDebug->SetTraceMode(TMODE_MAKESTEP_OVER);
        }
        if (wParam == VK_F11)
        {
            CDebug->SetTraceMode(TMODE_MAKESTEP);
        }
        if (wParam == VK_F5)
        {
            CDebug->SetTraceMode(TMODE_CONTINUE);
            ShowWindow(CDebug->hMain, SW_MINIMIZE);
        }
        break;
    case WM_COMMAND:
        NotifyCode = HIWORD(wParam);
        switch (NotifyCode)
        {
        case EN_KILLFOCUS:
            if (hEdit == (HWND)lParam)
            {
                if (hEdit)
                    DestroyWindow(hEdit);
                hEdit = nullptr;
            }
            break;
        case EN_UPDATE:

            if (hEdit == (HWND)lParam)
            {
                const uint32_t lines = SendMessage(hEdit, EM_GETLINECOUNT, 0, 0);
                if (lines > 1)
                {
                    if (edit_item >= 0 && edit_subitem >= 0)
                    {
                        std::string sTmpBuffer;

                        for (int32_t i = 0; i < static_cast<int32_t>(lines); i++)
                        {
                            PZERO(&TextEditBuffer[0], MAX_STR_SIZE);
                            *(uint16_t *)TextEditBuffer = MAX_STR_SIZE - 2;
                            const int32_t chars =
                                SendMessage(hEdit, EM_GETLINE, i, (LPARAM) static_cast<LPCSTR>(TextEditBuffer));
                            TextEditBuffer[chars] = 0;
                            if (chars)
                                sTmpBuffer += TextEditBuffer;
                        }

                        SetItemText(edit_item, edit_subitem, (char *)sTmpBuffer.c_str());
                        ItemChanged(edit_item, edit_subitem);
                        edit_item = -1;
                        edit_subitem = -1;
                    }
                    if (hEdit)
                        DestroyWindow(hEdit);
                    hEdit = nullptr;
                }
            }
            break;
        }
        break;
    case WM_NOTIFY:
        pnmh = (LPNMHDR)lParam;
        if (!pnmh)
            break;
        if (pnmh->hwndFrom != GetWindowHandle())
            break;
        switch (pnmh->code)
        {
        case LVN_KEYDOWN:
            vKey = ((LPNMLVKEYDOWN)lParam)->wVKey;
            if (vKey == VK_F4)
                StartEditSelectedItem();
            if (vKey == 'O' && GetAsyncKeyState(VK_CONTROL) < 0)
                CDebug->OpenNewFile();
            if (vKey == VK_F10)
            {
                CDebug->SetTraceMode(TMODE_MAKESTEP_OVER);
            }
            if (vKey == VK_F11)
            {
                CDebug->SetTraceMode(TMODE_MAKESTEP);
            }
            if (vKey == VK_F5)
            {
                CDebug->SetTraceMode(TMODE_CONTINUE);
                ShowWindow(CDebug->hMain, SW_MINIMIZE);
            }
            break;
        case NM_DBLCLK:
            lpnmlv = (LPNMLISTVIEW)lParam;
            if (lpnmlv)
            {
                if (lpnmlv->iItem >= 0)
                {
                    edit_item = lpnmlv->iItem;
                    edit_subitem = lpnmlv->iSubItem;
                    uint32_t bitres = 0x1 << edit_subitem;
                    bitres = EditMask & bitres;
                    if (bitres == 0)
                        break;

                    GetClientRect(GetWindowHandle(), &EditPos);
                    ListView_GetItemRect(GetWindowHandle(), lpnmlv->iItem, &item_rect, LVIR_SELECTBOUNDS);

                    EditPos.top = item_rect.top;
                    EditPos.bottom = EditPos.top + item_rect.bottom - item_rect.top - 1;
                    uint32_t subitem_spacing = 0;
                    for (int32_t n = 0; n < lpnmlv->iSubItem; n++)
                    {
                        subitem_spacing += ListView_GetColumnWidth(GetWindowHandle(), n);
                    }
                    EditPos.left += subitem_spacing;
                    subitem_spacing = ListView_GetColumnWidth(GetWindowHandle(), lpnmlv->iSubItem);
                    EditPos.right = EditPos.left + subitem_spacing - 1;
                    const auto list_rect = GetPosition();
                    OffsetRect(&EditPos, list_rect.left + 2, list_rect.top + 2);
                    if (EditPos.right + 2 > list_rect.right)
                        EditPos.right = list_rect.right - 2;
                    GetItemText(lpnmlv->iItem, lpnmlv->iSubItem, TextEditBuffer, sizeof(TextEditBuffer));
                    if (hEdit)
                        DestroyWindow(hEdit);
                    hEdit = nullptr;

                    hEdit = CreateWindowEx(
                        WS_EX_TOPMOST, TEXT("EDIT"), TEXT(""),
                        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL,
                        // hEdit = CreateWindowEx(WS_EX_TOPMOST,"STATIC","",WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL |
                        // ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL ,
                        EditPos.left, EditPos.top, EditPos.right - EditPos.left, EditPos.bottom - EditPos.top,
                        GetMainWindowHandle(), nullptr, GetInstance(), nullptr);
                    if (hFont)
                    {
                        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, 0);
                    }
                    MoveWindow(hEdit, EditPos.left, EditPos.top, EditPos.right - EditPos.left,
                               EditPos.bottom - EditPos.top, true);
                    SetFocus(hEdit);
                    std::wstring TextEditBufferW = utf8::ConvertUtf8ToWide(TextEditBuffer);
                    SetWindowText(hEdit, TextEditBufferW.c_str());
                }
            }
            break;
        }
        break;
    }
}

void TM_LIST::StartEditSelectedItem()
{
    NMLISTVIEW nmlv;
    auto *nmhdr = (NMHDR *)&nmlv;

    nmhdr->code = NM_DBLCLK;
    nmhdr->hwndFrom = GetWindowHandle();

    const auto iSelected = ListView_GetSelectionMark(GetWindowHandle());

    if (iSelected >= 0)
    {
        nmlv.iItem = iSelected;
        nmlv.iSubItem = 0;

        ProcessMessageBase(WM_NOTIFY, 0, (uintptr_t)&nmlv);
    }
}

int32_t TM_LIST::GetItemsCount()
{
    return ListView_GetItemCount(hOwn);
}

void TM_LIST::SetCharID(const char *text)
{
    if (text)
        strcpy_s(CharID, text);
}

char *TM_LIST::GetCharID()
{
    return CharID;
}
