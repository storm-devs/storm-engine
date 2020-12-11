//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	Lighter
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "Lighter.h"
#include "EntityManager.h"
//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

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

//Инициализация
bool Lighter::Init()
{
    //Проверяем, будем ли работать
    auto *ini = fio->OpenIniFile("resource\\ini\\loclighter.ini");
    if (!ini)
        return false;
    const auto isLoading = ini->GetLong(nullptr, "loading", 0);
    autoTrace = ini->GetLong(nullptr, "autotrace", 0) != 0;
    autoSmooth = ini->GetLong(nullptr, "autosmooth", 0) != 0;
    window.isSmallSlider = ini->GetLong(nullptr, "smallslider", 0) != 0;
    geometry.useColor = ini->GetLong(nullptr, "usecolor", 0) != 0;
    delete ini;
    if (!isLoading)
        return false;
    // DX9 render
    rs = static_cast<VDX9RENDER *>(api->CreateService("dx9render"));
    if (!rs)
        throw std::exception("No service: dx9render");
    //
    EntityManager::SetLayerType(LIGHTER_EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::AddToLayer(LIGHTER_EXECUTE, GetId(), 1000);
    EntityManager::SetLayerType(LIGHTER_REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::AddToLayer(LIGHTER_REALIZE, GetId(), 1000);
    //
    lightProcessor.SetParams(&geometry, &window, &lights, &octTree, rs);
    //оконная система
    if (!window.Init(rs))
        return false;

    return true;
}

//Исполнение
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
    //Освещение
    //Рассеяное
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
    //Солнце
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
    //Геометрия
    if (!geometry.Process(rs, lights.Num()))
    {
        window.isFailedInit = true;
        return;
    }
    octTree.Init(&geometry);
    //Освещалка
    lightProcessor.UpdateLightsParam();
    //Интерфейс
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

//Сообщения
uint64_t Lighter::ProcessMessage(MESSAGE &message)
{
    char command[32];
    message.String(31, command);
    command[31] = 0;
    if (_stricmp(command, "AddModel") == 0)
    {
        //Добавляем модельку
        MsgAddModel(message);
        return true;
    }
    if (_stricmp(command, "ModelsPath") == 0)
    {
        //Добавляем модельку
        MsgModelsPath(message);
        return true;
    }
    if (_stricmp(command, "LightPath") == 0)
    {
        //Добавляем модельку
        MsgLightPath(message);
        return true;
    }
    if (_stricmp(command, "AddLight") == 0)
    {
        //Добавляем модельку
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
        api->Trace("Location lighter: no model name, skip it!");
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
    //Позиция
    pos.x = message.Float();
    pos.y = message.Float();
    pos.z = message.Float();
    //Цвет
    clr.x = message.Float();
    clr.y = message.Float();
    clr.z = message.Float();
    //Затухание
    const auto att0 = message.Float();
    const auto att1 = message.Float();
    const auto att2 = message.Float();
    //Дистанция
    const auto range = message.Float();
    //Имя группы
    char group[512];
    message.String(511, group);
    group[511] = 0;
    //Добавляем источник
    lights.AddPointLight(clr, pos, att0, att1, att2, range, group);
}
