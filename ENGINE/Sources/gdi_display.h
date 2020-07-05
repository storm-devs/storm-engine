#ifndef _GDI_DISPLAY_H_
#define _GDI_DISPLAY_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif

#define BUFFER_SIZE	256 

class GDI_DISPLAY
{
public:

	GDI_DISPLAY();
	~GDI_DISPLAY();
	void Init(HINSTANCE hInstance, HWND _hwnd, long w, long h);
	void On_Paint(HWND hwnd);
	void Set_Text(char * text_PTR,...);
	bool Set_Message_Stack(long lines);
	void Print(char * text_PTR,...);
	void Print_Add(char * text_PTR,...);
	void Release();
	void Switch(bool on);

	HWND	hwnd;
	HBITMAP hBitmap;
	HFONT	hFont;
	long	width,height;
	char	buffer[BUFFER_SIZE];

	char * ms_PTR;
	long MS_Lines;
	long MS_Count;

	bool Log_File;
	bool gdi_off;

	
};
#endif