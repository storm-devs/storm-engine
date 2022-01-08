#pragma once

#include <windows.h>
#include <vector>

class SDBG_STATICLIST
{
    std::vector<HWND> hOwn;
    HWND hMain;
    int32_t nLines;

  public:
    SDBG_STATICLIST()
    {
        hMain = nullptr;
        nLines = 0;
    };

    ~SDBG_STATICLIST(){};

    bool Init(int32_t lines, RECT rParent, RECT rChild, HINSTANCE hInstance, HWND _hmain)
    {
        if (lines == 0)
            return false;

        hMain = _hmain;

        hOwn.resize(lines);
        int32_t height = (rChild.bottom - rChild.top) / lines;
        int32_t line_y = rChild.top;

        for (int32_t n = 0; n < lines; n++)
        {
            hOwn[n] = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE | WS_BORDER, rChild.left, line_y,
                                   rChild.right - rChild.left, height, hMain, NULL, hInstance, NULL);
            if (hOwn[n] == nullptr)
            {
                lines = n;
                return false;
            }
            line_y += height - 1;
        }
        nLines = lines;
        return true;
    };

    void SetText(int32_t n, const char *pText)
    {
        if (n >= nLines)
            return;
        SetWindowText(hOwn[n], pText);
        ShowWindow(hOwn[n], SW_NORMAL);
    }
};
