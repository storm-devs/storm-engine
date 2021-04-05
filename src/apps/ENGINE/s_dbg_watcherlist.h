#pragma once

#include "core.h"
#include "tm_list.h"

class WATCHER_LIST : public TM_LIST
{
    uint32_t ShowMask;
    HMENU hMenu;
    std::unique_ptr<INIFILE> ini;

  public:
    WATCHER_LIST(HWND hwnd, HINSTANCE hinst);
    ~WATCHER_LIST();
    void ProcessMessage(uint64_t, uint64_t, uint64_t);
    void SetShowMask(uint32_t mask)
    {
        ShowMask = mask;
    };
    void ItemChanged(long Item_index, long Subitem_index);
    void Refresh();
};
