#pragma once

#include "core.h"
#include "tm_list.h"

class WATCHER_LIST : public TM_LIST
{
    HMENU hMenu;
    std::unique_ptr<INIFILE> ini;

  public:
    WATCHER_LIST(HWND hwnd, HINSTANCE hinst);
    ~WATCHER_LIST();
    void ProcessMessage(uint64_t, uint64_t, uint64_t);
    void ItemChanged(long Item_index, long Subitem_index);
    void Refresh();
};
