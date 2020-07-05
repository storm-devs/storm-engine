#ifndef _S_WATCHERLIST_H_
#define _S_WATCHERLIST_H_

#ifndef _XBOX

#include "tm_list.h"
#include "core.h"


class WATCHER_LIST : public TM_LIST
{
	dword ShowMask;
	HMENU hMenu;
	INIFILE * ini;
public:
	WATCHER_LIST(HWND hwnd, HINSTANCE hinst);
	~WATCHER_LIST();
	void ProcessMessage(DWORD,DWORD,DWORD);
	void SetShowMask(dword mask) {ShowMask = mask;};
	void ItemChanged(long Item_index, long Subitem_index);
	void Refresh();
};

#endif

#endif