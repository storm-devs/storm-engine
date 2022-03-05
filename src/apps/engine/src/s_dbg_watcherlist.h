#pragma once

#include "core_impl.h"
#include "tm_list.h"

class WATCHER_LIST : public TM_LIST
{
    HMENU hMenu;
    std::unique_ptr<INIFILE> ini;

  public:
    WATCHER_LIST(HWND hwnd, HINSTANCE hinst);
    ~WATCHER_LIST();
    void ProcessMessage(uint64_t, uint64_t, uint64_t);
    void ItemChanged(int32_t Item_index, int32_t Subitem_index);
    void Refresh();
};
