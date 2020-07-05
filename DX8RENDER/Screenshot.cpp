#include <io.h>
#include <stdio.h>
#include <windows.h>
#include <gdiplus.h>
#include <iostream>
#include <sstream>

#include "sdevice.h"
#include "..\common_h\tga.h"

#pragma comment (lib, "Gdiplus.lib")

void DX8RENDER::PrepareCapture()
{
	hDesktopDC = GetDC(api->GetAppHWND());
	hCaptureDC = CreateCompatibleDC(hDesktopDC);
	hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, screen_size.x, screen_size.y);

	HBITMAP OldBmp = (HBITMAP)SelectObject(hCaptureDC, hCaptureBitmap); 
	lpbi = (LPBITMAPINFO)(NEW char[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)]);
	ZeroMemory(lpbi, sizeof(BITMAPINFO));
	lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	SelectObject(hCaptureDC, OldBmp); 
	GetDIBits(hCaptureDC, hCaptureBitmap, 0, screen_size.y, NULL, lpbi, DIB_RGB_COLORS); 

	bPreparedCapture = true;
}

void DX8RENDER::SaveCaptureBuffers()
{
	dword Written;
	char cFileName[256];
	long fi = 0;

	for (fi=iCaptureFrameIndex; fi<iCaptureFrameIndex + 10000; fi++)
	{
		sprintf(cFileName, "k3cap_%04d.tga", fi);
		if (_access(cFileName, 0) == -1) break;
	}

	for (dword i=0; i<dwCaptureBuffersReady; i++)
	{
		TGA_H TgaHead = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32};
		TgaHead.width = (WORD)screen_size.x; 
		TgaHead.height = (WORD)screen_size.y;
		sprintf(cFileName, "k3cap_%04d.tga", fi + i);
		HANDLE hFile = fio->_CreateFile(cFileName, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS);
		WriteFile(hFile, &TgaHead, sizeof(TGA_H), &Written, 0);
		WriteFile(hFile, aCaptureBuffers[i], screen_size.x * screen_size.y * sizeof(dword), &Written, NULL); 
		CloseHandle(hFile);
	}

	iCaptureFrameIndex = fi + dwCaptureBuffersReady + 1;

	_flushall();
	dwCaptureBuffersReady = 0;
}

bool DX8RENDER::MakeCapture()
{
	if (aCaptureBuffers.IsEmpty()) return false;

	if (!bPreparedCapture)
		PrepareCapture();

	if (dwCaptureBuffersReady >= aCaptureBuffers.Size())
	{
		Beep(1000, 150);
		SaveCaptureBuffers();
		Beep(5000, 150);
	}

	HBITMAP OldBmp = (HBITMAP)SelectObject(hCaptureDC, hCaptureBitmap); 
	BitBlt(hCaptureDC, 0, 0, screen_size.x, screen_size.y, hDesktopDC, 0, 0, SRCCOPY); 
	SelectObject(hCaptureDC, OldBmp); 
	GetDIBits(hCaptureDC, hCaptureBitmap, 0, screen_size.y, aCaptureBuffers[dwCaptureBuffersReady], lpbi, DIB_RGB_COLORS); 
	dwCaptureBuffersReady++;
	return true;
}

int DX8RENDER::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num  = 0;          // number of image encoders
    UINT  size = 0;          // size of the image encoder array in bytes

    Gdiplus::GetImageEncodersSize(&num, &size);
    if(size == 0)
        return -1;  // Failure

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if(pImageCodecInfo == NULL)
        return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for(UINT j = 0; j < num; ++j)
    {
        if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

using namespace std;

void DX8RENDER::GetPngScreenshot()
{
	char file_name[256];
	long i = 0;
	
    hDesktopDC = GetDC(api->GetAppHWND());
	hCaptureDC = CreateCompatibleDC(hDesktopDC);
	hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, screen_size.x, screen_size.y);
	HGDIOBJ old_obj = SelectObject(hCaptureDC, hCaptureBitmap);
	BitBlt(hCaptureDC, 0, 0, screen_size.x, screen_size.y, hDesktopDC, 0, 0, SRCCOPY);

	Gdiplus::Bitmap bitmap(hCaptureBitmap, NULL);
    CLSID clsid;

    GetEncoderClsid(L"image/png", &clsid);

	for(i = 0; i < 10000; i++)
	{
		wsprintf(file_name, "CCS_%04d.png", i);
		if(_access(file_name, 0) == -1) break;
	}

	wstring szPath (file_name, file_name + strlen(file_name));
        
    bitmap.Save(szPath.c_str(), &clsid);

	SelectObject(hCaptureDC, old_obj); 
}

void DX8RENDER::GetPng()
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    GetPngScreenshot();

    Gdiplus::GdiplusShutdown(gdiplusToken);
}


