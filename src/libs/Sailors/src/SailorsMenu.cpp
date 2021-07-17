#include "SailorsMenu.h"

void Menu::Draw(VDX9RENDER *rs, SailorsPoints &sailorsPoints) const
{
    for (auto i = 0; i < count; i++)
        if (i != selected && i != blocked)
            rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(100, 100, 100), 10, 20 + 20 * i, "%s", line[i].c_str());

    rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(255, 200, 100), 10, 20 + 20 * selected, "%s", line[selected].c_str());

    if (blocked >= 0)
        rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(255, 255, 255), 10, 20 + 20 * blocked, "%s", line[blocked].c_str());

    if (selected == 1 && blocked == 1)
    {
        rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 100, "%s", "'ins', 'del', arrow keys to edit");
        rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 120, "%s", "'SPACE' or 'SPACE+ SHIFT' to select");

        if (sailorsPoints.points.selected >= 0 && sailorsPoints.points.selected < sailorsPoints.points.count)
        {
            switch (sailorsPoints.points.point[sailorsPoints.points.selected].pointType)
            {
            case (PT_TYPE_NORMAL):
                rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 160, "%s", "type: PT_TYPE_NORMAL('F9')");
                break;

            case (PT_TYPE_CANNON_L):
                rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 160, "%s",
                          "type: PT_TYPE_CANNON_L('F9')");
                break;
            case (PT_TYPE_CANNON_R):
                rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 160, "%s",
                          "type: PT_TYPE_CANNON_R('F9')");
                break;
            case (PT_TYPE_CANNON_F):
                rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 160, "%s",
                          "type: PT_TYPE_CANNON_F('F9')");
                break;
            case (PT_TYPE_CANNON_B):
                rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 160, "%s",
                          "type: PT_TYPE_CANNON_B('F9')");
                break;

            case (PT_TYPE_MAST_1):
                rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 160, "%s", "type: PT_TYPE_MAST_1('F9')");
                break;

            case (PT_TYPE_MAST_2):
                rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 160, "%s", "type: PT_TYPE_MAST_2('F9')");
                break;

            case (PT_TYPE_MAST_3):
                rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 160, "%s", "type: PT_TYPE_MAST_3('F9')");
                break;

            case (PT_TYPE_MAST_4):
                rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 160, "%s", "type: PT_TYPE_MAST_4('F9')");
                break;

            case (PT_TYPE_MAST_5):
                rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 160, "%s", "type: PT_TYPE_MAST_5('F9')");
                break;

            case (PT_TYPE_NOT_TARGETPOINT):
                rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 160, "%s",
                          "type: PT_TYPE_NOT_TARGETPOINT('F9')");
                break;
            }
        }
        else
            rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 160, "%s", "type: ('F9')");

        if (sailrs->shipWalk[0].sailorsPoints.points.count &&
            sailrs->shipWalk[0].sailorsPoints.points.count > sailorsPoints.points.selected)
            if (sailrs->shipWalk[0].sailorsPoints.points.point[sailorsPoints.points.selected].buisy)
                rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 180, "%s", "buisy= true");
            else
                rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 180, "%s", "buisy= false");
    }

    if (selected == 2 && blocked == 2)
    {
        rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 100, "%s", "'ins', 'del' to edit");
        rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 120, "%s", "'SPACE' or 'SPACE+ SHIFT' to select");

        if (sailorsPoints.links.selected >= 0 && sailorsPoints.links.selected < sailorsPoints.links.count)
        {
            std::string s1 = "point 1= ";
            s1 += static_cast<long>(sailorsPoints.links.link[sailorsPoints.links.selected].first);
            s1 += "('F2')";

            rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 140, "%s", s1.c_str());

            s1 = "point 2= ";
            s1 += static_cast<long>(sailorsPoints.links.link[sailorsPoints.links.selected].next);
            s1 += "('F3')";

            rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 160, "%s", s1.c_str());

            sailorsPoints.DrawLinks(rs);
        }
        else
        {
            rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 140, "%s", "point 1=-1 ('F2')");
            rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 160, "%s", "point 2=-1 ('F3')");
            sailorsPoints.DrawLinks(rs);
        }
    }

    auto count = 0;
    auto reloaded = 0;

    for (auto i = 0; i < sailrs->shipWalk[0].sailorsPoints.points.count; i++)
        if (sailrs->shipWalk[0].sailorsPoints.points.point[i].pointType == PT_TYPE_CANNON_L ||
            sailrs->shipWalk[0].sailorsPoints.points.point[i].pointType == PT_TYPE_CANNON_R ||
            sailrs->shipWalk[0].sailorsPoints.points.point[i].pointType == PT_TYPE_CANNON_F ||
            sailrs->shipWalk[0].sailorsPoints.points.point[i].pointType == PT_TYPE_CANNON_B)
        {
            if (sailrs->shipWalk[0].sailorsPoints.points.point[i].cannonReloaded)
                reloaded++;

            count++;
        }

    std::string s1 = "F10 - Reload cannons(";
    s1 += static_cast<long>(reloaded);
    s1 += "/";
    s1 += static_cast<long>(count);
    s1 += ")";

    rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 470, "%s", s1.c_str());

    // rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200,200,200) ,10, 500,"%s" , "F1 - Send points to Sailors");
    rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 520, "%s", "F5 - Save to file");
    rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 540, "%s", "F6 - load from file");
    rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 560, "%s", "F4 - sail/war/storm mode");

    rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 540 + 100, "%s", "'a','s','w','d'");
    rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(200, 200, 200), 10, 560 + 100, "%s", "move camera");
};

//--------------------------------------------------------------------------------------------------------------

void Menu::Update(SailorsPoints &sailorsPoints)
{
    line[0] = "people count <";
    line[0] += static_cast<long>(sailrs->shipWalk[0].crewCount);
    line[0] += ">";

    line[1] = "selected point <";
    line[1] += static_cast<long>(sailorsPoints.points.selected);
    line[1] += ">(";
    line[1] += static_cast<long>(sailorsPoints.points.count);
    line[1] += ")";

    line[2] = "selected path <";
    line[2] += static_cast<long>(sailorsPoints.links.selected);
    line[2] += ">(";
    line[2] += static_cast<long>(sailorsPoints.links.count);
    line[2] += ")";
};

//--------------------------------------------------------------------------------------------------------------

void Menu::ChangeControl1(int key, SailorsPoints &sailorsPoints) // Change people count
{
    if (key == VK_RIGHT)
        sailrs->shipWalk[0].CreateNewMan(sailrs->shipWalk[0].sailorsPoints);

    if (key == VK_LEFT)
    {
        if (!sailrs->shipWalk[0].crewCount)
        {
            for (auto i = 0; i < 50; i++)
                sailrs->shipWalk[0].CreateNewMan(sailrs->shipWalk[0].sailorsPoints);
            Update(sailorsPoints);
            return;
        }
        // free occupied points
        if (sailrs->shipWalk[0].shipMan[0].mode == MAN_CLIMB_UP)
            sailrs->shipWalk[0].sailorsPoints.points.point[sailrs->shipWalk[0].shipMan[0].targetWayPoint].buisy = false;

        if (sailrs->shipWalk[0].shipMan[0].mode == MAN_CLIMB_DOWN)
            sailrs->shipWalk[0].sailorsPoints.points.point[sailrs->shipWalk[0].shipMan[0].lastWayPoint].buisy = false;

        sailrs->shipWalk[0].sailorsPoints.points.point[sailrs->shipWalk[0].shipMan[0].targetWayPoint].buisy = false;

        sailrs->shipWalk[0].DeleteMan(0);
    }

    Update(sailorsPoints);
};

//--------------------------------------------------------------------------------------------------------------

void Menu::ChangeControl2(int key, SailorsPoints &sailorsPoints) // Change points
{
    if (shiftKey && sailorsPoints.points.selected >= 0 && sailorsPoints.points.selected < sailorsPoints.points.count)
    {
        switch (key)
        {
        case VK_UP:
            sailorsPoints.points.point[sailorsPoints.points.selected].y += 0.025f * dltTime / 10.0f;
            return;

        case VK_DOWN:
            sailorsPoints.points.point[sailorsPoints.points.selected].y -= 0.025f * dltTime / 10.0f;
            return;

        case VK_LEFT:
        case VK_RIGHT:
            return;
        }
    }

    if (sailorsPoints.points.selected >= 0 && sailorsPoints.points.selected < sailorsPoints.points.count)
    {
        switch (key)
        {
        case VK_UP:
            sailorsPoints.points.point[sailorsPoints.points.selected].x -= 0.025f * sin(cameraAng.y) * dltTime / 10.0f;
            sailorsPoints.points.point[sailorsPoints.points.selected].z -= 0.025f * cos(cameraAng.y) * dltTime / 10.0f;
            break;

        case VK_DOWN:
            sailorsPoints.points.point[sailorsPoints.points.selected].x += 0.025f * sin(cameraAng.y) * dltTime / 10.0f;
            sailorsPoints.points.point[sailorsPoints.points.selected].z += 0.025f * cos(cameraAng.y) * dltTime / 10.0f;
            break;

        case VK_LEFT:
            sailorsPoints.points.point[sailorsPoints.points.selected].x +=
                0.025f * sin(cameraAng.y + PI / 2) * dltTime / 10.0f;
            sailorsPoints.points.point[sailorsPoints.points.selected].z +=
                0.025f * cos(cameraAng.y + PI / 2) * dltTime / 10.0f;
            break;

        case VK_RIGHT:
            sailorsPoints.points.point[sailorsPoints.points.selected].x -=
                0.025f * sin(cameraAng.y + PI / 2) * dltTime / 10.0f;
            sailorsPoints.points.point[sailorsPoints.points.selected].z -=
                0.025f * cos(cameraAng.y + PI / 2) * dltTime / 10.0f;
            break;
        }
    }

    if (key == VK_SPACE)
    {
        if (shiftKey)
        {
            if (sailorsPoints.points.selected)
                sailorsPoints.points.selected--;
            else if (sailorsPoints.points.count)
                sailorsPoints.points.selected = sailorsPoints.points.count - 1;
            else
                sailorsPoints.points.selected = -1;
        }
        else
        {
            if (sailorsPoints.points.selected < sailorsPoints.points.count - 1)
                sailorsPoints.points.selected++;
            else if (sailorsPoints.points.count)
                sailorsPoints.points.selected = 0;
            else
                sailorsPoints.points.selected = -1;
        }

        if (sailorsPoints.points.selected < 0)
            sailorsPoints.points.selected = 0;
        Update(sailorsPoints);
    }

    if (key == VK_F9)
    {
        if (sailorsPoints.points.selected >= 0 && sailorsPoints.points.selected < sailorsPoints.points.count)
        {
            switch (sailorsPoints.points.point[sailorsPoints.points.selected].pointType)
            {
            case PT_TYPE_NORMAL:
                sailorsPoints.points.point[sailorsPoints.points.selected].pointType = PT_TYPE_CANNON_L;
                break;
            case PT_TYPE_CANNON_L:
                sailorsPoints.points.point[sailorsPoints.points.selected].pointType = PT_TYPE_CANNON_R;
                break;
            case PT_TYPE_CANNON_R:
                sailorsPoints.points.point[sailorsPoints.points.selected].pointType = PT_TYPE_CANNON_F;
                break;
            case PT_TYPE_CANNON_F:
                sailorsPoints.points.point[sailorsPoints.points.selected].pointType = PT_TYPE_CANNON_B;
                break;
            case PT_TYPE_CANNON_B:
                sailorsPoints.points.point[sailorsPoints.points.selected].pointType = PT_TYPE_MAST_1;
                break;

            case PT_TYPE_MAST_1:
                sailorsPoints.points.point[sailorsPoints.points.selected].pointType = PT_TYPE_MAST_2;
                break;

            case PT_TYPE_MAST_2:
                sailorsPoints.points.point[sailorsPoints.points.selected].pointType = PT_TYPE_MAST_3;
                break;

            case PT_TYPE_MAST_3:
                sailorsPoints.points.point[sailorsPoints.points.selected].pointType = PT_TYPE_MAST_4;
                break;

            case PT_TYPE_MAST_4:
                sailorsPoints.points.point[sailorsPoints.points.selected].pointType = PT_TYPE_MAST_5;
                break;

            case PT_TYPE_MAST_5:
                sailorsPoints.points.point[sailorsPoints.points.selected].pointType = PT_TYPE_NOT_TARGETPOINT;
                break;

            case PT_TYPE_NOT_TARGETPOINT:
                sailorsPoints.points.point[sailorsPoints.points.selected].pointType = PT_TYPE_NORMAL;
                break;
            }
        }
        Update(sailorsPoints);
    }

    if (key == VK_INSERT && sailorsPoints.points.count < MAX_POINTS)
    {
        sailorsPoints.points.Add();

        sailorsPoints.points.point[sailorsPoints.points.count - 1].x += rand() / static_cast<float>(RAND_MAX);
        sailorsPoints.points.point[sailorsPoints.points.count - 1].z += rand() / static_cast<float>(RAND_MAX);

        sailorsPoints.points.selected = sailorsPoints.points.count - 1;

        Update(sailorsPoints);
    }

    if (key == VK_DELETE)
    {
        for (auto i = 0; i < sailorsPoints.links.count; i++)
        {
            if (sailorsPoints.links.link[i].first > sailorsPoints.points.selected)
                sailorsPoints.links.link[i].first--;

            if (sailorsPoints.links.link[i].next > sailorsPoints.points.selected)
                sailorsPoints.links.link[i].next--;
        }

        sailorsPoints.points.Delete(sailorsPoints.points.selected);
        Update(sailorsPoints);
    }
};

//--------------------------------------------------------------------------------------------------------------

void Menu::ChangeControl3(int key, SailorsPoints &sailorsPoints) // Change links
{
    if (key == VK_INSERT)
    {
        sailorsPoints.links.Add();

        if (sailorsPoints.links.selected >= 0 && sailorsPoints.links.selected < sailorsPoints.links.count)
        {
            sailorsPoints.links.link[sailorsPoints.links.count - 1] =
                sailorsPoints.links.link[sailorsPoints.links.selected];
        }
        else
        {
            sailorsPoints.links.link[sailorsPoints.links.count - 1].first = 0;
            sailorsPoints.links.link[sailorsPoints.links.count - 1].next = 0;
        }

        sailorsPoints.links.selected = sailorsPoints.links.count - 1;
        Update(sailorsPoints);
    }

    if (key == VK_DELETE)
    {
        sailorsPoints.links.Delete(sailorsPoints.links.selected);
        Update(sailorsPoints);
    }

    /*
    if (key== VK_F11)
    if(sailorsPoints.links.selected>= 0 && sailorsPoints.links.selected< sailorsPoints.links.count)
    {
      sailorsPoints.links.link[sailorsPoints.links.selected].disabled=
      !sailorsPoints.links.link[sailorsPoints.links.selected].disabled;
      sailorsPoints.UpdateLinks();
    };
  */

    if (key == VK_SPACE)
    {
        if (shiftKey)
        {
            if (sailorsPoints.links.selected)
                sailorsPoints.links.selected--;
            else if (sailorsPoints.links.count)
                sailorsPoints.links.selected = sailorsPoints.links.count - 1;
            else
                sailorsPoints.links.selected = -1;
        }
        else
        {
            if (sailorsPoints.links.selected < sailorsPoints.links.count - 1)
                sailorsPoints.links.selected++;
            else if (sailorsPoints.links.count)
                sailorsPoints.links.selected = 0;
            else
                sailorsPoints.links.selected = -1;
        }

        if (sailorsPoints.links.selected < 0)
            sailorsPoints.links.selected = 0;
    }

    if (key == VK_F2)
    {
        const auto selLnk = sailorsPoints.links.selected;

        if (selLnk < sailorsPoints.links.count && selLnk >= 0)
        {
            if (shiftKey)
                sailorsPoints.links.link[selLnk].first--;
            else
                sailorsPoints.links.link[selLnk].first++;

            if (sailorsPoints.links.link[selLnk].first >= sailorsPoints.points.count)
                sailorsPoints.links.link[selLnk].first = 0;

            if (sailorsPoints.links.link[selLnk].first < 0)
                sailorsPoints.links.link[selLnk].first = sailorsPoints.points.count - 1;
        }
    }

    if (key == VK_F3)
    {
        const auto selLnk = sailorsPoints.links.selected;

        if (selLnk < sailorsPoints.links.count && selLnk >= 0)
        {
            if (shiftKey)
                sailorsPoints.links.link[selLnk].next--;
            else
                sailorsPoints.links.link[selLnk].next++;

            if (sailorsPoints.links.link[selLnk].next >= sailorsPoints.points.count)
                sailorsPoints.links.link[selLnk].next = 0;

            if (sailorsPoints.links.link[selLnk].next < 0)
                sailorsPoints.links.link[selLnk].next = sailorsPoints.points.count - 1;
        }
    }

    sailrs->shipWalk[0].sailorsPoints.UpdateLinks();
    Update(sailorsPoints);
};

//--------------------------------------------------------------------------------------------------------------

void Menu::ChangeControl(int key, SailorsPoints &sailorsPoints)
{
    if (key == VK_F10)
    {
        sailrs->shipWalk[0].ReloadCannons(0);
    }

    if (key == VK_F4)
    {
        if (sailrs->shipWalk[0].shipState.mode == SHIP_SAIL)
            sailrs->shipWalk[0].shipState.mode = SHIP_WAR;
        else if (sailrs->shipWalk[0].shipState.mode == SHIP_WAR)
            sailrs->shipWalk[0].shipState.mode = SHIP_STORM;
        else
            sailrs->shipWalk[0].shipState.mode = SHIP_SAIL;
    }

    if (key == VK_F5)
    {
        sailorsPoints.WriteToFile(_fileName_save);
    }

    if (key == VK_F6)
    {
        sailorsPoints.ReadFromFile(_fileName_load);
        Update(sailorsPoints); // update menu
    }

    if (key == VK_F1)
    {
        /*
      // Refresh Sailors points

      for (int i= sailrs->shipWalk[0].sailorsPoints.points.count;
         i< sailorsPoints.points.count; i++)
         sailrs->shipWalk[0].sailorsPoints.points.Add();

      for (int i= sailorsPoints.points.count;
         i< sailrs->shipWalk[0].sailorsPoints.points.count; i++)
         sailrs->shipWalk[0].sailorsPoints.points.STORM_DELETE(sailrs->shipWalk[0].sailorsPoints.points.count- 1);



      for (int i= 0; i< sailorsPoints.points.count; i++){

        sailrs->shipWalk[0].sailorsPoints.points.point[i].x= sailorsPoints.points.point[i].x;
        sailrs->shipWalk[0].sailorsPoints.points.point[i].y= sailorsPoints.points.point[i].y;
        sailrs->shipWalk[0].sailorsPoints.points.point[i].z= sailorsPoints.points.point[i].z;
        sailrs->shipWalk[0].sailorsPoints.points.point[i].pointType= sailorsPoints.points.point[i].pointType;

      };

        // Refresh Sailors links

      sailrs->shipWalk[0].sailorsPoints.links.count=
      sailorsPoints.links.count;

      sailrs->shipWalk[0].sailorsPoints.links.link.clear();


      for (int i= 0; i< sailorsPoints.links.count; i++){

        sailrs->shipWalk[0].sailorsPoints.links.link.Add();
        sailrs->shipWalk[0].sailorsPoints.links.link[i]=
        sailorsPoints.links.link[i];
      };
      sailrs->shipWalk[0].sailorsPoints.UpdateLinks();*/
    }

    if (blocked != selected)
        return;

    switch (selected)
    {
    case 0:

        ChangeControl1(key, sailorsPoints);
        break;
    case 1:

        ChangeControl2(key, sailorsPoints);
        break;
    case 2:

        ChangeControl3(key, sailorsPoints);
        break;
    }
};

//--------------------------------------------------------------------------------------------------------------

void Menu::OnKeyPress(SailorsPoints &sailorsPoints)
{
    key = 0;
    shiftKey = 0;

    if (GetAsyncKeyState(VK_UP) < 0)
        key = VK_UP;
    else if (GetAsyncKeyState(VK_DOWN) < 0)
        key = VK_DOWN;
    else if (GetAsyncKeyState(VK_LEFT) < 0)
        key = VK_LEFT;
    else if (GetAsyncKeyState(VK_RIGHT) < 0)
        key = VK_RIGHT;
    else if (GetAsyncKeyState(VK_RETURN) < 0)
        key = VK_RETURN;
    else if (GetAsyncKeyState(VK_INSERT) < 0)
        key = VK_INSERT;
    else if (GetAsyncKeyState(VK_DELETE) < 0)
        key = VK_DELETE;
    else if (GetAsyncKeyState(VK_SPACE) < 0)
        key = VK_SPACE;
    else if (GetAsyncKeyState(VK_F1) < 0)
        key = VK_F1;
    else if (GetAsyncKeyState(VK_F2) < 0)
        key = VK_F2;
    else if (GetAsyncKeyState(VK_F3) < 0)
        key = VK_F3;
    else if (GetAsyncKeyState(VK_F5) < 0)
        key = VK_F5;
    else if (GetAsyncKeyState(VK_F6) < 0)
        key = VK_F6;
    else if (GetAsyncKeyState(VK_F9) < 0)
        key = VK_F9;
    else if (GetAsyncKeyState(VK_F4) < 0)
        key = VK_F4;
    else if (GetAsyncKeyState(VK_F10) < 0)
        key = VK_F10;
    else if (GetAsyncKeyState(VK_F11) < 0)
        key = VK_F11;

    if (GetAsyncKeyState(VK_LSHIFT) < 0 || GetAsyncKeyState(VK_RSHIFT) < 0)
        shiftKey = 1;
    else
        shiftKey = 0;

    if (!key)
    {
        keyPressed = false;
        return;
    }
    if (keyPressed && !(blocked == 1 && (key == VK_UP || key == VK_DOWN || key == VK_LEFT || key == VK_RIGHT)))
        return;

    switch (key)
    {
    case VK_UP:
        if (selected && blocked == -1)
            selected--;
        ChangeControl(key, sailorsPoints);
        break;

    case VK_DOWN:
        if (selected < count - 1 && blocked == -1)
            selected++;
        ChangeControl(key, sailorsPoints);
        break;

    case VK_RETURN:

        if (blocked == -1)
            blocked = selected;
        else
            blocked = -1;

        if (blocked == 0 && selected == 0)
        {
            sailrs->disabled = false;
        }
        else
            sailrs->disabled = true;

        break;

    case VK_INSERT:
    case VK_DELETE:
    case VK_LEFT:
    case VK_RIGHT:
    case VK_SPACE:
    case VK_F1:
    case VK_F2:
    case VK_F3:
    case VK_F5:
    case VK_F6:
    case VK_F9:
    case VK_F4:
    case VK_F10:
    case VK_F11:

        ChangeControl(key, sailorsPoints);
        break;
    }

    keyPressed = true;
};

//--------------------------------------------------------------------------------------------------------------
