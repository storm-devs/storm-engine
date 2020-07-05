#ifndef _TM_LIST_H_
#define _TM_LIST_H_

#ifndef _XBOX

#include <windows.h>
#include <commctrl.h>
#include "resource.h"

#define BM_BIND_LEFT	1
#define BM_BIND_TOP		2
#define BM_BIND_RIGHT	4
#define BM_BIND_BOTTOM	8

typedef unsigned long dword;
#define null 0

class TM_LIST
{
	HINSTANCE hInst;
	HWND hMain;
	HWND hOwn;
	HWND hEdit;
	RECT Pos;
	dword Columns_Num;
	dword Items_Num;
	dword Bind_Mask;
	long  edit_item;
	long  edit_subitem;
	char  CharID[MAX_PATH];
	char  SearchName[MAX_PATH];
	dword EditMask;
	HFONT hFont;
public:
	TM_LIST();
	virtual ~TM_LIST();
	HWND GetWindowHandle() {return hOwn;};
	HWND GetMainWindowHandle() {return hMain;};
	HINSTANCE GetInstance() {return hInst;};
	void Initialize(HWND hwnd, HINSTANCE hinst, dword style, dword style_ex);
	void Initialize(HWND hwnd, HINSTANCE hinst, dword style, dword style_ex, dword icon_rid);
	void SetPosition(RECT pos);
	RECT GetPosition(){return Pos;};
	void UpdatePosition();
	void AddColumn(char * name, long length);
	void AddItem(char * name);
	void SetItemText(long Item_index, long Subitem_index, char * text);
	void GetItemText(long Item_index, long Subitem_index, char * text, long max_size);
	void SetItemImage(long Item_index, long Subitem_index, long image_code);
	void SetBindMask(dword bind_mask) {Bind_Mask = bind_mask;};
	long GetItemsCount();
	char * GetSelectedName();
	void ProcessMessageBase(DWORD,DWORD,DWORD);
	void SetCharID(char * text);
	char * GetCharID();
	void SelectItem(char * name);
	void SetEditMask(dword mask){EditMask = mask;}
	virtual void ProcessMessage(DWORD,DWORD,DWORD){};
	virtual void ItemChanged(long Item_index, long Subitem_index)= 0;
	virtual void PostProcess(){};
	virtual void SetFont(HFONT _hfont){hFont = _hfont;};
	
	void StartEditSelectedItem();

};

#endif

#endif