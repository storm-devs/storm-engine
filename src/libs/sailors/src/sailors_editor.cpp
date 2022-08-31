#include "sailors_editor.h"

#include "core.h"
#include "shared/messages.h"
#include "shared/sea_ai/script_defines.h"

CREATE_CLASS(SailorsEditor)

SailorsEditor::SailorsEditor()
    : rs(nullptr), sailors(0), shipID(0), pointID(0), model(nullptr)
{
    cameraAng = 0.0f;

    cameraAng.x = PI / 4;
    cameraAng.y = -PI / 3 + PI;

    cameraTo = CVECTOR(0.0f, 5, 0.0f);
    cameraPos = CVECTOR(0.0f, 30.0f, 0.0f);
};

SailorsEditor::~SailorsEditor()
{
    core.EraseEntity(sailors);
    core.EraseEntity(shipID);
};

bool SailorsEditor::Init()
{
    rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));

    sailors = core.CreateEntity("Sailors");

    core.SetLayerType(EXECUTE, layer_type_t::execute);
    core.AddToLayer(EXECUTE, GetId(), 0);

    core.SetLayerType(EDITOR_REALIZE, layer_type_t::realize);
    core.AddToLayer(EDITOR_REALIZE, GetId(), 100000);

    LoadFromIni("SailorsEditor.ini");

    shipID = core.CreateEntity("MODELR");
    core.Send_Message(shipID, "ls", MSG_MODEL_LOAD_GEO, _shipName.c_str());

    core.AddToLayer(EDITOR_REALIZE, shipID, 100000);
    model = static_cast<MODEL *>(core.GetEntityPointer(shipID));

    model->mtx.BuildMatrix(CVECTOR(0.0f), CVECTOR(0.0f, 0.0f, 0.0f));

    auto ctrl = core.Controls->CreateControl("DeltaMouseH");
    core.Controls->MapControl(ctrl, 256);
    ctrl = core.Controls->CreateControl("DeltaMouseV");
    core.Controls->MapControl(ctrl, 257);

    menu.sailrs = static_cast<Sailors *>(core.GetEntityPointer(sailors));

    menu.sailrs->editorMode = true;

    core.Send_Message(sailors, "lis", AI_MESSAGE_ADD_SHIP, shipID, "");

    menu.Update(menu.sailrs->shipWalk[0].sailorsPoints);

    return true;
};

void SailorsEditor::Execute(uint32_t dltTime)
{
    SetCamera(dltTime);
    menu.OnKeyPress(menu.sailrs->shipWalk[0].sailorsPoints);

    if (core.Controls->GetAsyncKeyState(VK_ESCAPE) < 0)
#ifdef _WIN32 // TODO: restore sailors editor and move to tools folder
        ExitProcess(0);
#else
        exit(0);
#endif
};

void SailorsEditor::Realize(uint32_t dltTime)
{
    menu.Draw(rs, menu.sailrs->shipWalk[0].sailorsPoints);

    // rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(100,100,100) ,700, 10,"%f" , 1000.0f/float(dltTime));

    if (menu.blocked == 1 || menu.blocked == 2)
        menu.sailrs->shipWalk[0].sailorsPoints.Draw(rs, (menu.selected == 1 && menu.blocked == 1));

    if (menu.blocked < 1)
        menu.sailrs->shipWalk[0].sailorsPoints.Draw_(rs, false);
    // menu.sailrs->shipWalk[0].sailorsPoints.Draw_(rs, (menu.selected== 1 && menu.blocked== 1));
};

void SailorsEditor::SetCamera(uint32_t &dltTime)
{
    CONTROL_STATE cs;
    core.Controls->GetControlState("DeltaMouseV", cs);
    cameraAng.x += -cs.fValue * 0.001f;
    if (cameraAng.x < 0.01f)
        cameraAng.x = 0.01f;
    if (cameraAng.x > 1.57f)
        cameraAng.x = 1.57f;

    core.Controls->GetControlState("DeltaMouseH", cs);
    cameraAng.y += cs.fValue * 0.001f;

    CVECTOR pos;

    CMatrix mtx(cameraAng);
    pos = mtx * cameraPos;

    float speed = 0;
    if (core.Controls->GetAsyncKeyState(VK_LBUTTON) < 0)
        speed = -0.1f;
    if (core.Controls->GetAsyncKeyState(VK_RBUTTON) < 0)
        speed = 0.1f;

    cameraPos.y += speed * (dltTime / 10.0f);
    rs->SetCamera(cameraTo + pos, cameraTo, CVECTOR(0.0f, 1.0f, 0.0f));

    if (core.Controls->GetAsyncKeyState(0x57) < 0)
    {
        cameraTo.x -= sin(cameraAng.y) * dltTime / 50.0f;
        cameraTo.z -= cos(cameraAng.y) * dltTime / 50.0f;
    }

    if (core.Controls->GetAsyncKeyState(0x53) < 0)
    {
        cameraTo.x += sin(cameraAng.y) * dltTime / 50.0f;
        cameraTo.z += cos(cameraAng.y) * dltTime / 50.0f;
    }

    if (core.Controls->GetAsyncKeyState(0x41) < 0)
    {
        cameraTo.x += sin(cameraAng.y + PI / 2) * dltTime / 50.0f;
        cameraTo.z += cos(cameraAng.y + PI / 2) * dltTime / 50.0f;
    }

    if (core.Controls->GetAsyncKeyState(0x44) < 0)
    {
        cameraTo.x -= sin(cameraAng.y + PI / 2) * dltTime / 50.0f;
        cameraTo.z -= cos(cameraAng.y + PI / 2) * dltTime / 50.0f;
    }

    menu.cameraAng = cameraAng;
    menu.cameraPos = cameraTo;

    menu.dltTime = dltTime;
};

void SailorsEditor::LoadFromIni(std::string fileName)
{
    char param[256];

    auto pIni = fio->OpenIniFile(fileName.c_str());

    if (!pIni)
    {
        core.Trace("Sailors : Can`t open '%s'", fileName.c_str());
        return;
    }

    pIni->ReadString("PATH", "ship", param, sizeof(param) - 1);
    _shipName = param;

    pIni->ReadString("PATH", "filename_save", param, sizeof(param) - 1);
    menu._fileName_save = param;

    pIni->ReadString("PATH", "filename_load", param, sizeof(param) - 1);
    menu._fileName_load = param;
};
