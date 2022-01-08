#include "s_dbg_watcherlist.h"
#include "s_debug.h"

extern S_DEBUG * CDebug;

WATCHER_LIST::WATCHER_LIST(HWND hwnd, HINSTANCE hinst)
{
   SetEditMask(0xffffffff);

    hMenu = nullptr;
    Initialize(hwnd, hinst, 0, 0, 0);
    SetBindMask(BM_BIND_RIGHT | BM_BIND_BOTTOM);

    const int32_t xs = GetSystemMetrics(SM_CXSCREEN);

    AddColumn("Name", (3 * xs) / 5 - 20);
    AddColumn("Value", xs - (3 * xs) / 5 - 20);

    for (int32_t n = 0; n < 11; n++)
    {
        AddItem("");
    }

    char buffer[256];

    ini = fio->OpenIniFile("project.df");
    if (ini)
    {
        int32_t n = 0;
        sprintf_s(buffer, "E%d", n);
        if (ini->ReadString(nullptr, buffer, buffer, sizeof(buffer), ""))
        {
            SetItemText(n, 0, buffer);
            SetItemText(n, 1, CDebug->ProcessExpression(buffer));
            n++;

            sprintf_s(buffer, "E%d", n);
            while (ini->ReadStringNext(nullptr, buffer, buffer, sizeof(buffer)))
            {
                if (n < 11)
                {
                    SetItemText(n, 0, buffer);
                    SetItemText(n, 1, CDebug->ProcessExpression(buffer));
                }
                n++;
                sprintf_s(buffer, "E%d", n);
            }
        }
        else
        {
            // try read 11 elements
            for (auto n = 0; n < 11; n++)
            {
                sprintf_s(buffer, "E%d", n);
                if (ini->ReadString(nullptr, buffer, buffer, sizeof(buffer), ""))
                {
                    SetItemText(n, 0, buffer);
                    SetItemText(n, 1, CDebug->ProcessExpression(buffer));
                }
            }
        }
    }
    else
    {
        ini = fio->CreateIniFile("project.df", false);
    }

    ListView_SetItemState(GetWindowHandle(), 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

WATCHER_LIST::~WATCHER_LIST()
{
    if (hMenu)
        DestroyMenu(hMenu);
    hMenu = nullptr;
}

void WATCHER_LIST::ItemChanged(int32_t Item_index, int32_t Subitem_index)
{
    char buffer[MAX_PATH]{};
    char buffer2[MAX_PATH]{};
    char keyname[32];
    //    char name[MAX_PATH];
    // GetItemText(Item_index,0,name,sizeof(name));
    GetItemText(Item_index, Subitem_index, buffer, sizeof(buffer));
    switch (Subitem_index)
    {
    case 0:
        SetItemText(Item_index, 1, CDebug->ProcessExpression(buffer));
        sprintf_s(keyname, "E%d", Item_index);
        ini->WriteString(nullptr, keyname, buffer);
        break;
    case 1:
        GetItemText(Item_index, 0, buffer, sizeof(buffer));
        GetItemText(Item_index, 1, buffer2, sizeof(buffer2));
        CDebug->SetOnDebugExpression(buffer, buffer2);
        SetItemText(Item_index, 1, CDebug->ProcessExpression(buffer));

        break;
    }
}

void WATCHER_LIST::Refresh()
{
    char String[MAX_PATH]{};
    for (int32_t n = 0; n < GetItemsCount(); n++)
    {
        GetItemText(n, 0, String, sizeof(String));
        SetItemText(n, 1, CDebug->ProcessExpression(String));
    }
}

void WATCHER_LIST::ProcessMessage(uint64_t iMsg, uint64_t wParam, uint64_t lParam)
{
    LPNMHDR pnmh;
    LPNMLISTVIEW lpnmlv;

    ProcessMessageBase(iMsg, wParam, lParam);

    switch (iMsg)
    {
    case WM_NOTIFY:
        pnmh = (LPNMHDR)lParam;
        if (!pnmh)
            break;
        if (pnmh->hwndFrom != GetWindowHandle())
            break;
        switch (pnmh->code)
        {
        case NM_RCLICK:
            lpnmlv = (LPNMLISTVIEW)lParam;
            if (lpnmlv)
                if (lpnmlv->iItem >= 0)
                {
                    SetItemText(lpnmlv->iItem, 0, "");
                    ItemChanged(lpnmlv->iItem, 0);
                }
            break;
        case NM_DBLCLK:
            lpnmlv = (LPNMLISTVIEW)lParam;
            if (lpnmlv)
            {
                if (lpnmlv->iItem >= 0)
                {
                    int32_t item = lpnmlv->iItem;
                    int32_t subitem = lpnmlv->iSubItem;
                }
                else
                {
                    AddItem("");
                }
            }
            break;
        }

        break;
    }

    /*LPNMHDR pnmh;
    POINT point;
    HMENU hSubMenu;
    LPNMLISTVIEW lpnmlv;
    int32_t item,subitem;

    switch(iMsg)
    {
      case WM_NOTIFY:
        pnmh = (LPNMHDR) lParam;
        if(!pnmh) break;
        if(pnmh->hwndFrom != GetWindowHandle()) break;
        switch(pnmh->code)
        {
          case NM_RCLICK:
            if(hMenu) DestroyMenu(hMenu); hMenu = 0;
            hMenu = LoadMenu(GetInstance(),MAKEINTRESOURCE(IDR_MENUTASKLIST));
            hSubMenu = GetSubMenu(hMenu,0);
            GetCursorPos(&point);
            TrackPopupMenu(hSubMenu,TPM_LEFTALIGN,point.x,point.y,0,GetMainWindowHandle(),0);
          break;
          case NM_DBLCLK:
            lpnmlv = (LPNMLISTVIEW) lParam;
            if(lpnmlv)
            {
              if(lpnmlv->iItem >= 0 )
              {
                item = lpnmlv->iItem;
                subitem = lpnmlv->iSubItem;

              }
            }

          break;
        }
      break;
    }*/
}
