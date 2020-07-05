#ifndef _SDBG_STATICLIST_H_
#define _SDBG_STATICLIST_H_

#include <windows.h>
#include "dat.h"

class SDBG_STATICLIST
{
	CDAT<HWND> hOwn; 
	HWND hMain;
	long nLines;
public:

	 SDBG_STATICLIST(){hMain = 0; nLines = 0;};
	~SDBG_STATICLIST(){};
	bool Init(long lines, RECT rParent, RECT rChild, HINSTANCE hInstance, HWND _hmain)
	{
		long n;
		long height;
		long line_y;

		if(lines == 0) return false;

		hMain = _hmain;
		
		hOwn.SetSize(lines);
		height = (rChild.bottom - rChild.top)/lines;
		line_y = rChild.top;
		
		for(n=0;n<lines;n++)
		{
			hOwn[n] = CreateWindow("STATIC","",WS_CHILD|WS_VISIBLE|WS_BORDER,
				rChild.left,line_y,
				rChild.right-rChild.left,height,hMain,NULL,hInstance,NULL);
			if(hOwn[n] == 0) 
			{
				lines = n;
				return false;
			}
			line_y += height-1;
		}
		nLines = lines;
		return true;
	};
	void SetText(long n, const char * pText)
	{
		if(n >= nLines) return;
		SetWindowText(hOwn[n],pText);
		ShowWindow(hOwn[n],SW_NORMAL);
	}

};

#endif