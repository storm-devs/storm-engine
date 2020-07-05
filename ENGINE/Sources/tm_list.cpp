#include "tm_list.h"
#include "Core.h"
#include "..\..\common_h\templates.h"
#include "s_debug.h"


#define MAX_STR_SIZE 1024
extern CORE Core;
extern S_DEBUG CDebug;

static char TM_LIST_Buffer[MAX_STR_SIZE];

TM_LIST::TM_LIST()
{
	hInst = null;
	hMain = null;
	hOwn = null;
	hEdit = null;
	ZeroMemory(&Pos,sizeof(Pos));
	Columns_Num = 0;
	Items_Num = 0;
	Bind_Mask = 0;
	edit_item = -1;
	edit_subitem = -1;
	ZeroMemory(CharID,sizeof(CharID));
	EditMask = 0;
	hFont = 0;
}

TM_LIST::~TM_LIST()
{
	if(hOwn) DestroyWindow(hOwn);
	if(hEdit) DestroyWindow(hEdit); hEdit = 0;
}

void TM_LIST::Initialize(HWND hwnd, HINSTANCE hinst, dword style, dword style_ex)
{
	hInst = hinst;
	hMain = hwnd;

	INITCOMMONCONTROLSEX icc;
	ZeroMemory(&icc,sizeof(icc));
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icc);

	hOwn = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW,"", WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS,
		0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hMain, NULL, hInst, NULL); 
    if(hOwn == NULL) throw "cant create list view"; 
	//ListView_SetExtendedListViewStyle(hOwn,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES);
	ListView_SetExtendedListViewStyle(hOwn,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	ShowWindow(hOwn,SW_SHOWNORMAL);
	SetFocus(hOwn);
	//ListView_SetBkColor(hOwn,0xff);
	//ListView_SetTextColor(hOwn,0xff);

}

void TM_LIST::SetPosition(RECT pos)
{
	Pos = pos;
	if(hOwn) MoveWindow(hOwn,Pos.left,Pos.top,Pos.right - Pos.left,Pos.bottom - Pos.top,true);
}

void TM_LIST::Initialize(HWND hwnd, HINSTANCE hinst, dword style, dword style_ex, dword icon_rid)
{
	Initialize(hwnd,hinst,style,style_ex);
	


/*	HICON hiconItem;        // icon for list view items 

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

	ShowWindow(hOwn,SW_SHOWNORMAL);
}

void TM_LIST::AddColumn(char * name, long length)
{
	LVCOLUMN lvc; 

	char string[MAX_STR_SIZE];
	ZeroMemory(&lvc,sizeof(lvc));
	
   	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;// | LVCF_ORDER ; 
    lvc.fmt = LVCFMT_LEFT; 
    lvc.cx = length; 
    lvc.pszText = string;
	//lvc.iOrder = Columns_Num;
	if(name) strcpy(string,name); else strcpy(string,"");
	lvc.iSubItem = Columns_Num; 
    if(ListView_InsertColumn(hOwn,Columns_Num,&lvc) == -1) throw "cant add column"; 
	Columns_Num++;
}


void TM_LIST::AddItem(char * name)
{
    LVITEM lvi;
	char string[MAX_STR_SIZE];
    lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE; 
    lvi.state = 0; 
    lvi.stateMask = 0; 
    lvi.pszText = string;
    lvi.iImage = 0;
	lvi.iItem = Items_Num; 
	lvi.iSubItem = 0; 
	lvi.lParam = 0;
	if(name) strcpy(string,name); else strcpy(string,"");
	ListView_InsertItem(hOwn, &lvi); 
	Items_Num++;
}

void TM_LIST::SetItemText(long Item_index, long Subitem_index, char * text)
{
	if(text) ListView_SetItemText(hOwn,Item_index,Subitem_index,text);	
}

void TM_LIST::GetItemText(long Item_index, long Subitem_index, char * text, long max_size)
{
	if(text) ListView_GetItemText(hOwn,Item_index,Subitem_index,text,max_size); 		
}

//if (text) for (long i=0, j=0; i<max_size && text[i]; i++) if (text[i] != '\n') text[j++] = text[i];

void TM_LIST::SetItemImage(long Item_index, long Subitem_index, long image_code)
{
	LVITEM lvi;
	if(image_code < 0) return;
	ZeroMemory(&lvi,sizeof(lvi));
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = Item_index;
	lvi.iSubItem = Subitem_index;
	lvi.iImage = image_code;
	ListView_SetItem(hOwn,&lvi);
}

void TM_LIST::UpdatePosition()
{
	if(!hOwn) return;

	dword width,height;
	RECT r;
	GetClientRect(hMain,&r);
	width = r.right - r.left;
	height = r.bottom - r.top;
	//MoveWindow(hOwn,Pos.left,Pos.top,width - Pos.left,height - Pos.top,true);

	if(Bind_Mask & BM_BIND_LEFT) Pos.left = 0;
	if(Bind_Mask & BM_BIND_TOP) Pos.top = 0;
	if(Bind_Mask & BM_BIND_RIGHT) Pos.right = width;
	if(Bind_Mask & BM_BIND_BOTTOM) Pos.bottom = height;

	MoveWindow(hOwn,Pos.left,Pos.top,Pos.right - Pos.left, Pos.bottom - Pos.top,true);
}

char * TM_LIST::GetSelectedName()
{
	long index;
	index  = ListView_GetSelectionMark(hOwn);
	if(index < 0) return 0;
	ListView_GetItemText(hOwn,index,0,TM_LIST_Buffer,sizeof(TM_LIST_Buffer));
	return TM_LIST_Buffer;
}

void TM_LIST::SelectItem(char * name)
{
	long n;
	long items;
	if(!name) return;
	items = GetItemsCount();
	for(n=0;n<items;n++)
	{
		GetItemText(n,0,SearchName,sizeof(SearchName));
		if(stricmp(SearchName,name)==0)
		{
			ListView_SetItemState(GetWindowHandle(),n,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
			return;
		}
	}
	ListView_SetItemState(GetWindowHandle(),0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
}

void TM_LIST::ProcessMessageBase(DWORD iMsg, DWORD wParam, DWORD lParam)
{
	LPNMHDR pnmh;
	//POINT point;
	//HMENU hSubMenu;
	LPNMLISTVIEW lpnmlv;
	//DWORD item_spacing;
	DWORD subitem_spacing;
	RECT list_rect;
	RECT item_rect;
	long n,chars;
	DWORD NotifyCode;
	dword lines;
	char TextEditBuffer[MAX_STR_SIZE];
	dword bitres;
	word vKey;
	
	RECT EditPos;

	switch(iMsg)
	{
		case WM_SYSKEYDOWN:
			if (wParam == VK_F10) 
			{
				CDebug.SetTraceMode(TMODE_MAKESTEP_OVER);
			}
		break;
		case WM_KEYDOWN:
			if (wParam == VK_F4) StartEditSelectedItem();
			if (wParam == VK_F10) 
			{
				CDebug.SetTraceMode(TMODE_MAKESTEP_OVER);
			}
			if (wParam == VK_F11) 
			{
				CDebug.SetTraceMode(TMODE_MAKESTEP);
			}
			if (wParam == VK_F5)
			{
				CDebug.SetTraceMode(TMODE_CONTINUE);
				ShowWindow(CDebug.hMain, SW_MINIMIZE);
			}
		break;
		case WM_COMMAND:
			NotifyCode = HIWORD(wParam);
			switch(NotifyCode)
			{
				case EN_KILLFOCUS:
					if(hEdit == (HWND)lParam)
					{
						if(hEdit) DestroyWindow(hEdit); hEdit = 0;
					}
				break;
				case EN_UPDATE:
					
					if(hEdit == (HWND)lParam)
					{
						lines = SendMessage(hEdit,EM_GETLINECOUNT,0,0);
						if(lines > 1)
						{
							if(edit_item >= 0 && edit_subitem >= 0)
							{
								string sTmpBuffer;

								for (long i=0; i<(long)lines; i++)
								{
									ZeroMemory(&TextEditBuffer[0], MAX_STR_SIZE);
									*(word*)TextEditBuffer = MAX_STR_SIZE - 2;
									chars = SendMessage(hEdit,EM_GETLINE,i,(LPARAM)(LPCSTR)TextEditBuffer);
									TextEditBuffer[chars] = 0;
									if (chars) sTmpBuffer += TextEditBuffer;
								}

								SetItemText(edit_item,edit_subitem,(char*)sTmpBuffer.GetBuffer());
								ItemChanged(edit_item,edit_subitem);
								edit_item = -1;
								edit_subitem = -1;
							}
							if(hEdit) DestroyWindow(hEdit); hEdit = 0;
						}
					}
				break;
			}
		break;
		case WM_NOTIFY:
			pnmh = (LPNMHDR) lParam; 
			if(!pnmh) break;
			if(pnmh->hwndFrom != GetWindowHandle()) break;
			switch(pnmh->code)
			{
				case LVN_KEYDOWN:
					vKey = ((LPNMLVKEYDOWN)lParam)->wVKey;
					if (vKey == VK_F4) StartEditSelectedItem();
					if (vKey == 'O' &&  GetAsyncKeyState(VK_CONTROL) < 0) CDebug.OpenNewFile();
					if (vKey == VK_F10) 
					{
						CDebug.SetTraceMode(TMODE_MAKESTEP_OVER);
					}
					if (vKey == VK_F11) 
					{
						CDebug.SetTraceMode(TMODE_MAKESTEP);
					}
					if (vKey == VK_F5)
					{
						CDebug.SetTraceMode(TMODE_CONTINUE);
						ShowWindow(CDebug.hMain, SW_MINIMIZE);
					}
				break;
				case NM_DBLCLK:
					lpnmlv = (LPNMLISTVIEW) lParam;
					if(lpnmlv)
					{
						if(lpnmlv->iItem >= 0 ) 
						{
							edit_item = lpnmlv->iItem;
							edit_subitem = lpnmlv->iSubItem;
							bitres = 0x1 << edit_subitem;
							bitres = EditMask & bitres;
							if(bitres == 0) break;

							GetClientRect(GetWindowHandle(),&EditPos);
							ListView_GetItemRect(GetWindowHandle(),lpnmlv->iItem,&item_rect,LVIR_SELECTBOUNDS );
							
							EditPos.top = item_rect.top;
							EditPos.bottom = EditPos.top + item_rect.bottom - item_rect.top - 1;
							subitem_spacing = 0;
							for(n=0;n<lpnmlv->iSubItem;n++)
							{
								subitem_spacing += ListView_GetColumnWidth(GetWindowHandle(),n);
							}
							EditPos.left += subitem_spacing;
							subitem_spacing = ListView_GetColumnWidth(GetWindowHandle(),lpnmlv->iSubItem);
							EditPos.right = EditPos.left + subitem_spacing - 1;
							list_rect = GetPosition();
							OffsetRect(&EditPos,list_rect.left + 2,list_rect.top + 2);
							if(EditPos.right + 2 > list_rect.right) EditPos.right = list_rect.right - 2;
							GetItemText(lpnmlv->iItem,lpnmlv->iSubItem,TextEditBuffer,sizeof(TextEditBuffer));
							if(hEdit) DestroyWindow(hEdit); hEdit = 0;
							 
								hEdit = CreateWindowEx(WS_EX_TOPMOST,"EDIT","",WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL ,
								//hEdit = CreateWindowEx(WS_EX_TOPMOST,"STATIC","",WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL ,
								EditPos.left,
								EditPos.top,
								EditPos.right-EditPos.left,
								EditPos.bottom-EditPos.top,
								GetMainWindowHandle(),
								NULL,GetInstance(),NULL);
							if(hFont)
							{
								SendMessage(hEdit,WM_SETFONT,(WPARAM)hFont,0);
							}
							MoveWindow(hEdit,EditPos.left,EditPos.top,EditPos.right-EditPos.left,EditPos.bottom-EditPos.top,true);
							SetFocus(hEdit);
							SetWindowText(hEdit,TextEditBuffer);
						
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
	NMHDR * nmhdr = (NMHDR *)&nmlv;

	nmhdr->code = NM_DBLCLK;
	nmhdr->hwndFrom = GetWindowHandle();

	int iSelected = ListView_GetSelectionMark(GetWindowHandle());

	if (iSelected >= 0)
	{
		nmlv.iItem = iSelected;
		nmlv.iSubItem = 0;

		ProcessMessageBase(WM_NOTIFY, (WPARAM)null, (LPARAM)&nmlv);
	}
}

long TM_LIST::GetItemsCount()
{
	return ListView_GetItemCount(hOwn);
}

void TM_LIST::SetCharID(char * text)
{
	if(text) strcpy(CharID,text);
}

char * TM_LIST::GetCharID()
{
	return CharID;
}