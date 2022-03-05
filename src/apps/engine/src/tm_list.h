#pragma once

#include <windows.h>
#include <cstdint>

#define BM_BIND_LEFT 1
#define BM_BIND_TOP 2
#define BM_BIND_RIGHT 4
#define BM_BIND_BOTTOM 8

class TM_LIST
{
    HINSTANCE hInst;
    HWND hMain;
    HWND hOwn;
    HWND hEdit;
    RECT Pos;
    uint32_t Columns_Num;
    uint32_t Items_Num;
    uint32_t Bind_Mask;
    int32_t edit_item;
    int32_t edit_subitem;
    char CharID[MAX_PATH];
    char SearchName[MAX_PATH];
    uint32_t EditMask;
    HFONT hFont;

  public:
    TM_LIST();
    virtual ~TM_LIST();

    HWND GetWindowHandle()
    {
        return hOwn;
    };

    HWND GetMainWindowHandle()
    {
        return hMain;
    };

    HINSTANCE GetInstance()
    {
        return hInst;
    };
    void Initialize(HWND hwnd, HINSTANCE hinst, uint32_t style, uint32_t style_ex);
    void Initialize(HWND hwnd, HINSTANCE hinst, uint32_t style, uint32_t style_ex, uint32_t icon_rid);
    void SetPosition(RECT pos);

    RECT GetPosition()
    {
        return Pos;
    };
    void UpdatePosition();
    void AddColumn(const char *name, int32_t length);
    void AddItem(const char *name);
    void SetItemText(int32_t Item_index, int32_t Subitem_index, const char *text);
    void GetItemText(int32_t Item_index, int32_t Subitem_index, const char *text, int32_t max_size);
    void SetItemImage(int32_t Item_index, int32_t Subitem_index, int32_t image_code);

    void SetBindMask(uint32_t bind_mask)
    {
        Bind_Mask = bind_mask;
    };
    int32_t GetItemsCount();
    char *GetSelectedName();
    void ProcessMessageBase(uint64_t, uint64_t, uint64_t);
    void SetCharID(const char *text);
    char *GetCharID();
    void SelectItem(const char *name);

    void SetEditMask(uint32_t mask)
    {
        EditMask = mask;
    }

    virtual void ProcessMessage(uint32_t, uint32_t, uint32_t){};
    virtual void ItemChanged(int32_t Item_index, int32_t Subitem_index) = 0;

    virtual void PostProcess(){};

    virtual void SetFont(HFONT _hfont)
    {
        hFont = _hfont;
    };

    void StartEditSelectedItem();
};
