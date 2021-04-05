//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Lighter
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "Lighter.h"

#include "core.h"

#include "Entity.h"
// ============================================================================================
// Construction, destruction
// ============================================================================================

Lighter::Lighter()
{
    rs = nullptr;
    initCounter = 10;
    isInited = false;
    waitChange = 0.0f;
}

Lighter::~Lighter()
{
}

// Initialization
bool Lighter::Init()
{
    // Checking if ini file exists
    auto ini = fio->OpenIniFile("resource\\ini\\loclighter.ini");
    if (!ini)
        return false;
    const auto isLoading = ini->GetLong(nullptr, "loading", 0);
    autoTrace = ini->GetLong(nullptr, "autotrace", 0) != 0;
    autoSmooth = ini->GetLong(nullptr, "autosmooth", 0) != 0;
    window.isSmallSlider = ini->GetLong(nullptr, "smallslider", 0) != 0;
    geometry.useColor = ini->GetLong(nullptr, "usecolor", 0) != 0;
    if (!isLoading)
        return false;
    // DX9 render
    rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!rs)
        throw std::exception("No service: dx9render");
    //
    EntityManager::SetLayerType(LIGHTER_EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::AddToLayer(LIGHTER_EXECUTE, GetId(), 1000);
    EntityManager::SetLayerType(LIGHTER_REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::AddToLayer(LIGHTER_REALIZE, GetId(), 1000);
    //
    lightProcessor.SetParams(&geometry, &window, &lights, &octTree, rs);
    // window system
    if (!window.Init(rs))
        return false;

    return true;
}

// Execution
void Lighter::Execute(uint32_t delta_time)
{
    const auto dltTime = delta_time * 0.001f;
    if (window.isSaveLight)
    {
        window.isSaveLight = false;
        if (geometry.Save())
        {
            window.isSuccessful = 1.0f;
        }
        else
        {
            window.isFailed = 10.0f;
        }
    }
    lightProcessor.Process();
    if (window.isNeedInit)
    {
        window.isNeedInit = false;
        window.Reset(true);
        PreparingData();
    }
    if (waitChange <= 0.0f)
    {
        if (GetAsyncKeyState(VK_NUMPAD0) < 0)
        {
            waitChange = 0.5f;
            if (isInited)
            {
                window.Reset(!window.isVisible);
            }
            else
            {
                window.isNeedInit = true;
                isInited = true;
            }
        }
    }
    else
        waitChange -= dltTime;
}

void Lighter::PreparingData()
{
    // Lighting
    // Scattered
    auto amb = 0xff404040;
    rs->GetRenderState(D3DRS_AMBIENT, &amb);
    CVECTOR clr;
    clr.x = ((amb >> 16) & 0xff) / 255.0f;
    clr.y = ((amb >> 8) & 0xff) / 255.0f;
    clr.z = ((amb >> 0) & 0xff) / 255.0f;
    auto mx = clr.x > clr.y ? clr.x : clr.y;
    mx = mx > clr.z ? mx : clr.z;
    if (mx > 0.0f)
        clr *= 1.0f / mx;
    else
        clr = 1.0f;
    lights.AddAmbient(clr);
    // The sun
    if (rs)
    {
        auto isLight = FALSE;
        rs->GetLightEnable(0, &isLight);
        D3DLIGHT9 lit;
        if (isLight && rs->GetLight(0, &lit))
        {
            CVECTOR clr, dir = !CVECTOR(1.0f, 1.0f, 1.0f);
            clr.x = lit.Diffuse.r;
            clr.y = lit.Diffuse.g;
            clr.z = lit.Diffuse.b;
            if (lit.Type == D3DLIGHT_DIRECTIONAL)
            {
                dir.x = -lit.Direction.x;
                dir.y = -lit.Direction.y;
                dir.z = -lit.Direction.z;
            }
            auto mx = dir.x > dir.y ? dir.x : dir.y;
            mx = mx > dir.z ? mx : dir.z;
            if (mx > 0.0f)
                dir *= 1.0f / mx;
            else
                dir = 1.0f;
            lights.AddWeaterLights(clr, dir);
        }
    }
    lights.PostInit();
    // Geometry
    if (!geometry.Process(rs, lights.Num()))
    {
        window.isFailedInit = true;
        return;
    }
    octTree.Init(&geometry);
    // Lighting
    lightProcessor.UpdateLightsParam();
    // Interface
    window.InitList(lights);
    window.isTraceShadows = autoTrace;
    window.isSmoothShadows = autoSmooth;
}

void Lighter::Realize(uint32_t delta_time)
{
    if (GetAsyncKeyState(VK_DECIMAL) < 0)
    {
        window.isNoPrepared = !isInited;
        geometry.DrawNormals(rs);
    }
    else
        window.isNoPrepared = false;
    window.Draw(delta_time * 0.001f);
}

// Messages
uint64_t Lighter::ProcessMessage(MESSAGE &message)
{
    char command[32];
    message.String(31, command);
    command[31] = 0;
    if (_stricmp(command, "AddModel") == 0)
    {
        // Adding the model
        MsgAddModel(message);
        return true;
    }
    if (_stricmp(command, "ModelsPath") == 0)
    {
        // Adding the model
        MsgModelsPath(message);
        return true;
    }
    if (_stricmp(command, "LightPath") == 0)
    {
        // Adding the model
        MsgLightPath(message);
        return true;
    }
    if (_stricmp(command, "AddLight") == 0)
    {
        // Adding the model
        MsgAddLight(message);
        return true;
    }
    return false;
}

void Lighter::MsgAddModel(MESSAGE &message)
{
    char name[512];
    message.String(511, name);
    name[511] = 0;
    if (!name[0])
    {
        core.Trace("Location lighter: no model name, skip it!");
        return;
    }
    const auto model = message.EntityID();
    geometry.AddObject(name, model);
}

void Lighter::MsgModelsPath(MESSAGE &message)
{
    char name[512];
    message.String(511, name);
    name[511] = 0;
    geometry.SetModelsPath(name);
}

void Lighter::MsgLightPath(MESSAGE &message)
{
    char name[512];
    message.String(511, name);
    name[511] = 0;
    geometry.SetLightPath(name);
}

void Lighter::MsgAddLight(MESSAGE &message)
{
    CVECTOR pos, clr;
    // Position
    pos.x = message.Float();
    pos.y = message.Float();
    pos.z = message.Float();
    // Colour
    clr.x = message.Float();
    clr.y = message.Float();
    clr.z = message.Float();
    // Attenuation
    const auto att0 = message.Float();
    const auto att1 = message.Float();
    const auto att2 = message.Float();
    // Distance
    const auto range = message.Float();
    // Group name
    char group[512];
    message.String(511, group);
    group[511] = 0;
    // Add source
    lights.AddPointLight(clr, pos, att0, att1, att2, range, group);
}
