#ifndef _XBOX

//#include <COMMCTRL.H>
#include "s_dbg_watcherlist.h"
#include "s_debug.h"

extern S_DEBUG CDebug;
extern CORE Core;

WATCHER_LIST::WATCHER_LIST(HWND hwnd, HINSTANCE hinst)
{
	long n;
	
	SetEditMask(0xffffffff);

	hMenu = 0;
	Initialize(hwnd,hinst,0,0,0);
	SetBindMask(BM_BIND_RIGHT|BM_BIND_BOTTOM);
	
	long xs;
	xs = GetSystemMetrics(SM_CXSCREEN);

	AddColumn("Name",(3*xs)/5 - 20);
	AddColumn("Value",xs - (3*xs)/5 - 20);
	
	for(n=0; n<11; n++)
	{
		AddItem("");
	}

	
	char buffer[256];

	ini = Core.fio->OpenIniFile("project.df");
	n = 0;
	if(ini)
	{
		wsprintf(buffer,"E%d",n);
		if (ini->ReadString(0, buffer, buffer, sizeof(buffer), ""))
		{
			if (n < 11)
			{
				SetItemText(n, 0, buffer);
				SetItemText(n, 1, CDebug.ProcessExpression(buffer)); 
			}
			n++;
			
			wsprintf(buffer,"E%d",n);
			while(ini->ReadStringNext(0,buffer,buffer,sizeof(buffer)))
			{
				if (n < 11)
				{
					SetItemText(n,0,buffer);
					SetItemText(n,1,CDebug.ProcessExpression(buffer)); 
				}
				n++;
				wsprintf(buffer,"E%d",n);
			}
		}
		else
		{
			// try read 11 elements
			for (int n=0; n<11; n++)
			{
				sprintf(buffer, "E%d", n);
				if (ini->ReadString(0, buffer, buffer, sizeof(buffer), ""))
				{
					SetItemText(n, 0, buffer);
					SetItemText(n, 1, CDebug.ProcessExpression(buffer));
				}
			}
		}
	}
	else
	{
		ini = Core.fio->CreateIniFile("project.df",false);
	}



	ListView_SetItemState(GetWindowHandle(),0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
}

WATCHER_LIST::~WATCHER_LIST()
{
	if(hMenu) DestroyMenu(hMenu); hMenu = 0;
	if(ini) delete ini;
}

void WATCHER_LIST::ItemChanged(long Item_index, long Subitem_index)
{
	char buffer[MAX_PATH];
	char buffer2[MAX_PATH];
	char keyname[32];
//	char name[MAX_PATH];
	//GetItemText(Item_index,0,name,sizeof(name));
	GetItemText(Item_index,Subitem_index,buffer,sizeof(buffer));
	switch(Subitem_index)
	{
	case 0:
		SetItemText(Item_index,1,CDebug.ProcessExpression(buffer));
		wsprintf(keyname,"E%d",Item_index);
		ini->WriteString(0,keyname,buffer);
	break;
	case 1:
		GetItemText(Item_index,0,buffer,sizeof(buffer));
		GetItemText(Item_index,1,buffer2,sizeof(buffer2));
		CDebug.SetOnDebugExpression(buffer,buffer2);
		SetItemText(Item_index,1,CDebug.ProcessExpression(buffer));

	break;
	}
}

void WATCHER_LIST::Refresh()
{
	char String[MAX_PATH];
	long n;
	for(n=0;n<GetItemsCount();n++)
	{
		GetItemText(n,0,String,sizeof(String));
		SetItemText(n,1,CDebug.ProcessExpression(String));
	}
}

void WATCHER_LIST::ProcessMessage(DWORD iMsg, DWORD wParam, DWORD lParam)
{
	LPNMHDR pnmh;
	LPNMLISTVIEW lpnmlv;
	long item,subitem;

	ProcessMessageBase(iMsg,wParam,lParam);

	switch(iMsg)
	{
		case WM_NOTIFY:
			pnmh = (LPNMHDR) lParam; 
			if(!pnmh) break;
			if(pnmh->hwndFrom != GetWindowHandle()) break;
			switch(pnmh->code)
			{
				case NM_RCLICK:
					lpnmlv = (LPNMLISTVIEW) lParam;
					if(lpnmlv)
						if(lpnmlv->iItem >= 0 ) 
						{
							SetItemText(lpnmlv->iItem, 0, "");
							ItemChanged(lpnmlv->iItem, 0);
						}
				break;
				case NM_DBLCLK:
					lpnmlv = (LPNMLISTVIEW) lParam;
					if(lpnmlv)
					{
						if(lpnmlv->iItem >= 0 ) 
						{
							item = lpnmlv->iItem;
							subitem = lpnmlv->iSubItem;
							
						} else
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
	long item,subitem;

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

#endif