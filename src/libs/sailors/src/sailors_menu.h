#pragma once

#include "Sailors.h"
#include <string>

class Menu
{
  public:
    std::string line[10];
    int selected;
    int blocked;
    int count;
    CVECTOR cameraAng;
    CVECTOR cameraPos;
    uint32_t dltTime;

    bool keyPressed;
    int key, shiftKey;

    Sailors *sailrs;
    std::string _fileName_save;
    std::string _fileName_load;

    Menu() : dltTime(0), key(0), shiftKey(0), sailrs(nullptr)
    {
        count = 3;
        selected = 0;
        blocked = -1;
        keyPressed = false;
    };

    void Draw(VDX9RENDER *rs, SailorsPoints &sailorsPoints) const;
    void OnKeyPress(SailorsPoints &sailorsPoints);
    void ChangeControl(int key, SailorsPoints &sailorsPoints);

    void ChangeControl1(int key, SailorsPoints &sailorsPoints);
    void ChangeControl2(int key, SailorsPoints &sailorsPoints);
    void ChangeControl3(int key, SailorsPoints &sailorsPoints);

    void Update(SailorsPoints &sailorsPoints);
};
