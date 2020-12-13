#include <io.h>

#include "sdevice.h"
#include "tga.h"

void DX9RENDER::PrepareCapture()
{
    hDesktopDC = GetDC(api->GetAppHWND());
    hCaptureDC = CreateCompatibleDC(hDesktopDC);
    hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, screen_size.x, screen_size.y);

    auto *const OldBmp = static_cast<HBITMAP>(SelectObject(hCaptureDC, hCaptureBitmap));
    lpbi = (LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)]);
    PZERO(lpbi, sizeof(BITMAPINFO));
    lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    SelectObject(hCaptureDC, OldBmp);
    GetDIBits(hCaptureDC, hCaptureBitmap, 0, screen_size.y, nullptr, lpbi, DIB_RGB_COLORS);

    bPreparedCapture = true;
}

void DX9RENDER::SaveCaptureBuffers()
{
    uint32_t Written;
    char cFileName[256];

    long fi;
    for (fi = iCaptureFrameIndex; fi < iCaptureFrameIndex + 10000; fi++)
    {
        sprintf_s(cFileName, "k3cap_%04d.tga", fi);
        if (_access(cFileName, 0) == -1)
            break;
    }

    for (uint32_t i = 0; i < dwCaptureBuffersReady; i++)
    {
        TGA_H TgaHead = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32};
        TgaHead.width = static_cast<uint16_t>(screen_size.x);
        TgaHead.height = static_cast<uint16_t>(screen_size.y);
        sprintf_s(cFileName, "k3cap_%04d.tga", fi + i);
        auto *const hFile = fio->_CreateFile(cFileName, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS);
        WriteFile(hFile, &TgaHead, sizeof(TGA_H), (LPDWORD)&Written, nullptr);
        WriteFile(hFile, aCaptureBuffers[i], screen_size.x * screen_size.y * sizeof(uint32_t), (LPDWORD)&Written,
                  nullptr);
        CloseHandle(hFile);
    }

    iCaptureFrameIndex = fi + dwCaptureBuffersReady + 1;

    _flushall();
    dwCaptureBuffersReady = 0;
}

bool DX9RENDER::MakeCapture()
{
    if (aCaptureBuffers.empty())
        return false;

    if (!bPreparedCapture)
        PrepareCapture();

    if (dwCaptureBuffersReady >= aCaptureBuffers.size())
    {
        Beep(1000, 150);
        SaveCaptureBuffers();
        Beep(5000, 150);
    }

    auto *const OldBmp = static_cast<HBITMAP>(SelectObject(hCaptureDC, hCaptureBitmap));
    BitBlt(hCaptureDC, 0, 0, screen_size.x, screen_size.y, hDesktopDC, 0, 0, SRCCOPY);
    SelectObject(hCaptureDC, OldBmp);
    GetDIBits(hCaptureDC, hCaptureBitmap, 0, screen_size.y, aCaptureBuffers[dwCaptureBuffersReady], lpbi,
              DIB_RGB_COLORS);
    dwCaptureBuffersReady++;
    return true;
}
