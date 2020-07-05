#include "gdi_display.h"
#include "common_h.h"
#include "resource.h"
#include "system_log.h"
#include "..\..\common_h\memop.h"

#define DEFAULT_DISPLAY_STACK_SIZE	12

#define HEADER_TEXT		"Storm Engine 2.8"
#define FONT_NAME		"Courier New"
#define FONT_SIZE		20
#define LEFT_OFFSET		10
#define TOP_OFFSET		5

GDI_DISPLAY::GDI_DISPLAY()
{
	hBitmap = 0;
	hFont = 0;
	width = 0;
	height = 0;
	memset(buffer,0,BUFFER_SIZE);
	ms_PTR = 0;	
	MS_Count = 0;
	Log_File = true;
	hwnd = 0;
}

GDI_DISPLAY::~GDI_DISPLAY()
{
	if(hBitmap) DeleteObject(hBitmap);
	if(hFont)	DeleteObject(hFont);
	if(ms_PTR) delete ms_PTR;
}

void GDI_DISPLAY::Release()
{
	if(ms_PTR) delete ms_PTR;
	ms_PTR = null;
}

void GDI_DISPLAY::Switch(bool on)
{
	
	long xs,ys;
	gdi_off = !on; 
	if(on) 
	{
		xs = GetSystemMetrics(SM_CXSCREEN);
		ys = GetSystemMetrics(SM_CYSCREEN);
		MoveWindow(hwnd,(xs - width)/2,(ys - height)/2,width,height,false);
		//InvalidateRect(0,0,false);
		InvalidateRect(hwnd,0,true);
		//ShowWindow(hwnd,SW_SHOWNORMAL);
	}
};

void GDI_DISPLAY::Init(HINSTANCE hInstance, HWND _hwnd, long w, long h)
{
	long xs,ys;
	width = w;
	height = h;
	hwnd = _hwnd;
	xs = GetSystemMetrics(SM_CXSCREEN);
	ys = GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(hwnd,(xs - w)/2,(ys - h)/2,w,h,false);
	hBitmap =  LoadBitmap(hInstance,MAKEINTRESOURCE(storm_logo_bmp));
	InvalidateRect(hwnd,0,true);
	ShowWindow(hwnd,SW_SHOWNORMAL);
	
	hFont = CreateFont(FONT_SIZE,0,0,0,FW_BOLD,1,false,false,DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,VARIABLE_PITCH,FONT_NAME);

	Set_Message_Stack(DEFAULT_DISPLAY_STACK_SIZE);
}

void GDI_DISPLAY::On_Paint(HWND hwnd)
{
	HDC dc,dcmem;
	PAINTSTRUCT PS;
	BOOL Res;
	BITMAP bm;
	HGDIOBJ hBitmap_old;
	HGDIOBJ hFont_old;
	POINT ptSize,ptOrg;

	if(gdi_off) return;

	long dis = 2;
	

	if(hBitmap == 0) return;

	dc = BeginPaint(hwnd,&PS);
	dcmem = CreateCompatibleDC(dc);
	
	hBitmap_old = SelectObject(dcmem,hBitmap);
	hFont_old = SelectObject(dc,hFont);

	SetMapMode(dcmem,GetMapMode(dc));

	GetObject(hBitmap,sizeof(BITMAP),(LPVOID)&bm);
	ptSize.x = bm.bmWidth;
	ptSize.y = bm.bmHeight;
	DPtoLP(dc,&ptSize,1);
	ptOrg.x = 0;
	ptOrg.y = 0;
	DPtoLP(dcmem,&ptOrg,1);
	
	Res = BitBlt(dc,0,0,width,height,dcmem,0,0,SRCCOPY);
	
	
	SetBkMode(dc,TRANSPARENT);
	
	SetTextColor(dc,RGB(0,0,0));
		TextOut(dc,LEFT_OFFSET + dis,TOP_OFFSET + dis,HEADER_TEXT,strlen(HEADER_TEXT));
	SetTextColor(dc,RGB(255,255,222));
		TextOut(dc,LEFT_OFFSET,TOP_OFFSET,HEADER_TEXT,strlen(HEADER_TEXT));
	
	RECT r;	
	GetClientRect(hwnd,&r);
	
	SetTextColor(dc,RGB(0,0,0));
		TextOut(dc,LEFT_OFFSET + dis,r.bottom - TOP_OFFSET - FONT_SIZE + dis,buffer,strlen(buffer)); 
	SetTextColor(dc,RGB(255,255,222));
		TextOut(dc,LEFT_OFFSET,r.bottom - TOP_OFFSET - FONT_SIZE,buffer,strlen(buffer)); 
	
	long n;
	for(n=0;n<MS_Count;n++)
	{
		SetTextColor(dc,RGB(0,0,0));
		TextOut(dc,2 * LEFT_OFFSET + dis,TOP_OFFSET + FONT_SIZE*(n + 2) + dis,
			ms_PTR + n*BUFFER_SIZE,strlen(ms_PTR + n*BUFFER_SIZE)); 
		SetTextColor(dc,RGB(255,255,222));
		TextOut(dc,2 * LEFT_OFFSET,TOP_OFFSET + FONT_SIZE*(n + 2),
			ms_PTR + n*BUFFER_SIZE,strlen(ms_PTR + n*BUFFER_SIZE)); 
		
	}

	SelectObject(dc,hFont_old);
	SelectObject(dcmem,hBitmap_old);
	DeleteDC(dcmem);
	EndPaint(hwnd,&PS);
	

}

	
void GDI_DISPLAY::Set_Text(char * text_PTR,...)
{
	if(gdi_off) return;
	if(text_PTR == 0) return;
	va_list args;
	va_start(args,text_PTR);
	vsprintf(buffer,text_PTR,args);
	va_end(args);
	RECT r;	
	GetClientRect(hwnd,&r);
 	r.bottom -= TOP_OFFSET; r.top = r.bottom - FONT_SIZE;
	InvalidateRect(hwnd,&r,false);
	UpdateWindow(hwnd);
}

bool GDI_DISPLAY::Set_Message_Stack(long lines)
{
	if(ms_PTR) delete ms_PTR;

	ms_PTR = (char *)NEW char[lines*BUFFER_SIZE];
	if(ms_PTR == null) return false;
	memset(ms_PTR,0,lines*BUFFER_SIZE);
	MS_Lines = lines;
	return true;
}

void GDI_DISPLAY::Print(char * text_PTR,...)
{
	if(text_PTR == 0) return;
	va_list args;
	char buffer[256];
	va_start(args,text_PTR);
	vsprintf(buffer,text_PTR,args);
	va_end(args);

	char * copy_PTR;
	long size;
	
	if(ms_PTR == null) Set_Message_Stack(DEFAULT_DISPLAY_STACK_SIZE);
	if(ms_PTR == null) return;

	if(MS_Count >= MS_Lines)
	{
		MS_Count = MS_Lines;
		memcpy(ms_PTR,ms_PTR + BUFFER_SIZE,(MS_Lines - 1)*BUFFER_SIZE);
		copy_PTR = ms_PTR + (MS_Lines - 1)*BUFFER_SIZE;
	} else copy_PTR = ms_PTR + MS_Count*BUFFER_SIZE;

	size = strlen(buffer);
	memset(copy_PTR,0,BUFFER_SIZE);
	if(size >= BUFFER_SIZE) memcpy(copy_PTR,buffer,(BUFFER_SIZE - 1));
	else memcpy(copy_PTR,buffer,size);
	MS_Count++;
	if(MS_Count >= MS_Lines) MS_Count = MS_Lines;

	if(Log_File) trace(buffer);

	if(gdi_off) return;

	RECT r;	long dis;
	GetWindowRect(hwnd,&r);
	dis = r.left; OffsetRect(&r,-dis,0);
	dis = r.top; OffsetRect(&r,0,-dis);
	r.top += 2*FONT_SIZE + TOP_OFFSET;
	r.bottom = r.top + MS_Lines*FONT_SIZE;

	InvalidateRect(hwnd,&r,false);
	UpdateWindow(hwnd);

	

}

void GDI_DISPLAY::Print_Add(char * text_PTR,...)
{
	if(text_PTR == 0) return;
	va_list args;
	char buffer[256];
	va_start(args,text_PTR);
	vsprintf(buffer,text_PTR,args);
	va_end(args);
	
	if(ms_PTR == null) Set_Message_Stack(DEFAULT_DISPLAY_STACK_SIZE);
	if(ms_PTR == null) return;

	if(MS_Count > MS_Lines) return;
	strcat(ms_PTR + (MS_Count - 1)*BUFFER_SIZE,buffer);

	if(Log_File) trace(buffer);

	if(gdi_off) return;

	RECT r;	long dis;
	GetWindowRect(hwnd,&r);
	dis = r.left; OffsetRect(&r,-dis,0);
	dis = r.top; OffsetRect(&r,0,-dis);
	r.top += (1 + MS_Count)*FONT_SIZE + TOP_OFFSET;
	r.bottom = r.top + FONT_SIZE;

	InvalidateRect(hwnd,&r,false);
	UpdateWindow(hwnd);
}

