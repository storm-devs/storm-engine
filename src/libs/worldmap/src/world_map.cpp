//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WorldMap
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "world_map.h"

#include <chrono>

#include "shared/messages.h"
#include "core.h"

#include "wdm_camera_std_ctrl.h"
#include "wdm_clouds.h"
#include "wdm_follow_ship.h"
#include "wdm_icon.h"
#include "wdm_islands.h"
#include "wdm_merchant_ship.h"
#include "wdm_player_ship.h"
#include "wdm_sea.h"
#include "wdm_storm.h"
#include "wdm_warring_ship.h"
#include "wdm_wind_ui.h"
#include "entity.h"
#include "math_inlines.h"
#include "string_compare.hpp"

#ifdef GetObject
#undef GetObject
#endif

CREATE_CLASS(WorldMap)

//============================================================================================

//#define EVENTS_OFF
//#define ENCS_OFF

#define WDM_MAX_STORMS 4

int32_t WorldMap::month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// ============================================================================================
// Construction, destruction
// ============================================================================================

WorldMap::WorldMap() : rs{}, aDate{}
{
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::system_clock;

    Assert(!wdmObjects);
    new WdmObjects();
    firstFreeObject = 0;
    firstObject = -1;
    firstPrObject = -1;
    firstMrObject = -1;
    firstLrObject = -1;
    for (int32_t i = 0; i < WDMAP_MAXOBJECTS; i++)
        object[i].next = i + 1;
    object[WDMAP_MAXOBJECTS - 1].next = -1;
    wdmObjects->wm = this;
    camera = nullptr;
    srand(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    encTime = 0.0f;
    aStorm = nullptr;
    aEncounter = nullptr;
    aInfo = nullptr;
    saveData = nullptr;
    timeScale = 1.0f;
    hour = 11.0f;
    day = 14;
    mon = 6;
    year = 1655;
    encCounter = 0;
}

WorldMap::~WorldMap()
{
    /*for(; firstObject >= 0; firstObject = object[firstObject].next)
    {
      delete object[firstObject].ro;
    }*/
    delete camera;
    WdmRenderObject::DeleteAllObjects();
    wdmObjects->Clear();

    delete wdmObjects;
}

//============================================================================================
// Entity
//============================================================================================

// Initialization
bool WorldMap::Init()
{
    // GUARD(LocationCamera::Init())
    // Layers
    // core.LayerCreate("execute", true, false);
    core.SetLayerType(EXECUTE, layer_type_t::execute);
    // core.LayerCreate("realize", true, false);
    core.SetLayerType(REALIZE, layer_type_t::realize);
    core.AddToLayer(EXECUTE, GetId(), 10000);
    core.AddToLayer(REALIZE, GetId(), 10000);

    // DX9 render
    rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    if (!rs)
        throw std::runtime_error("No service: dx9render");
    rs->SetPerspective((1.57f + 1.0f) / 2);
    wdmObjects->rs = rs;
    // GS
    wdmObjects->gs = static_cast<VGEOMETRY *>(core.GetService("geometry"));
    // Create map objects
    WdmRenderObject *ro;
    // Create islands
    ro = AddObject(new WdmIslands());
    AddObject(ro, -100000);
    AddLObject(ro, 500);
    //
    rs->ProgressView();
    // Create the sea
    auto *sea = new WdmSea();
    AddObject(sea);
    AddPObject(sea, 10);
    AddLObject(sea, -1);
    // create clouds
    AddLObject(AddObject(new WdmClouds()), 10000);
    rs->ProgressView();
    // Create a camera
    camera = new WdmCameraStdCtrl();
    auto camAy = 0.0f;
    auto camH = -1.0f;
    auto camLock = false;
    if (AttributesPointer)
    {
        camAy = AttributesPointer->GetAttributeAsFloat("wdmCameraAY", camAy);
        camH = AttributesPointer->GetAttributeAsFloat("wdmCameraY", camH);
        camLock = AttributesPointer->GetAttributeAsDword("wdmCameraRotLock", false) != 0;
        wdmObjects->SetWindSaveString(AttributesPointer->GetAttribute("WindData"));
        wdmObjects->shipSpeedOppositeWind =
            AttributesPointer->GetAttributeAsFloat("shipSpeedOppositeWind", wdmObjects->shipSpeedOppositeWind);
        wdmObjects->shipSpeedOverWind =
            AttributesPointer->GetAttributeAsFloat("shipSpeedOverWind", wdmObjects->shipSpeedOverWind);
    }
    else
    {
        wdmObjects->SetWindSaveString(nullptr);
    }
    camera->Init(camAy, camH);
    camera->lock = camLock;
    // create the player's ship
    ro = CreateModel(new WdmPlayerShip(), "Ship");
    Assert(ro);
    AddLObject(ro, 100);
    auto psX = 50.0f;
    auto psZ = 0.0f;
    auto psAy = 0.0f;
    auto psRad = 16.0f;
    if (AttributesPointer)
    {
        psX = AttributesPointer->GetAttributeAsFloat("playerShipX", psX);
        psZ = AttributesPointer->GetAttributeAsFloat("playerShipZ", psZ);
        psAy = AttributesPointer->GetAttributeAsFloat("playerShipAY", psAy);
        psRad = AttributesPointer->GetAttributeAsFloat("playerShipActionRadius", psRad);
        wdmObjects->enemyshipViewDistMin =
            AttributesPointer->GetAttributeAsFloat("enemyshipViewDistMin", wdmObjects->enemyshipViewDistMin);
        wdmObjects->enemyshipViewDistMax =
            AttributesPointer->GetAttributeAsFloat("enemyshipViewDistMax", wdmObjects->enemyshipViewDistMax);
        wdmObjects->enemyshipDistKill =
            AttributesPointer->GetAttributeAsFloat("enemyshipDistKill", wdmObjects->enemyshipDistKill);
        wdmObjects->enemyshipBrnDistMin =
            AttributesPointer->GetAttributeAsFloat("enemyshipBrnDistMin", wdmObjects->enemyshipBrnDistMin);
        wdmObjects->enemyshipBrnDistMax =
            AttributesPointer->GetAttributeAsFloat("enemyshipBrnDistMax", wdmObjects->enemyshipBrnDistMax);
        wdmObjects->stormViewDistMin =
            AttributesPointer->GetAttributeAsFloat("stormViewDistMin", wdmObjects->stormViewDistMin);
        wdmObjects->stormViewDistMax =
            AttributesPointer->GetAttributeAsFloat("stormViewDistMax", wdmObjects->stormViewDistMax);
        wdmObjects->stormDistKill = AttributesPointer->GetAttributeAsFloat("stormDistKill", wdmObjects->stormDistKill);
        wdmObjects->stormBrnDistMin =
            AttributesPointer->GetAttributeAsFloat("stormBrnDistMin", wdmObjects->stormBrnDistMin);
        wdmObjects->stormBrnDistMax =
            AttributesPointer->GetAttributeAsFloat("stormBrnDistMax", wdmObjects->stormBrnDistMax);
        wdmObjects->stormZone = AttributesPointer->GetAttributeAsFloat("stormZone", wdmObjects->stormZone);
        const char *s = AttributesPointer->GetAttribute("debug");
        wdmObjects->isDebug = s && (storm::iEquals(s, "true"));
        saveData = AttributesPointer->CreateSubAClass(AttributesPointer, "encounters");
        wdmObjects->resizeRatio = AttributesPointer->GetAttributeAsFloat("resizeRatio", wdmObjects->resizeRatio);
    }
    static_cast<WdmShip *>(ro)->Teleport(psX, psZ, psAy);
    static_cast<WdmPlayerShip *>(ro)->SetActionRadius(psRad);
    rs->ProgressView();
    // Create a location descriptor
    wdmObjects->islands->SetIslandsData(AttributesPointer, false);
    // Script interface attributes
    if (AttributesPointer)
    {
        // Storms interface
        AttributesPointer->CreateSubAClass(AttributesPointer, "storm.num");
        AttributesPointer->CreateSubAClass(AttributesPointer, "storm.cur");
        AttributesPointer->CreateSubAClass(AttributesPointer, "storm.x");
        AttributesPointer->CreateSubAClass(AttributesPointer, "storm.z");
        AttributesPointer->CreateSubAClass(AttributesPointer, "storm.time");
        aStorm = AttributesPointer->FindAClass(AttributesPointer, "storm");
        // Ship encounters inderface
        AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.num");
        AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.cur");
        AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.x");
        AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.z");
        AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.ay");
        AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.time");
        AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.type");
        AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.attack");
        AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.nation");
        AttributesPointer->CreateSubAClass(AttributesPointer, "encounter.id");
        aEncounter = AttributesPointer->FindAClass(AttributesPointer, "encounter");
        // Info
        AttributesPointer->CreateSubAClass(AttributesPointer, "info.playerInStorm");
        AttributesPointer->CreateSubAClass(AttributesPointer, "info.updateinfo");
        aInfo = AttributesPointer->FindAClass(AttributesPointer, "info");
        // Date
        AttributesPointer->CreateSubAClass(AttributesPointer, "date.sec");
        AttributesPointer->CreateSubAClass(AttributesPointer, "date.min");
        AttributesPointer->CreateSubAClass(AttributesPointer, "date.hour");
        AttributesPointer->CreateSubAClass(AttributesPointer, "date.day");
        AttributesPointer->CreateSubAClass(AttributesPointer, "date.month");
        AttributesPointer->CreateSubAClass(AttributesPointer, "date.year");
        aDate = AttributesPointer->FindAClass(AttributesPointer, "date");
    }
    if (aDate)
    {
        const int32_t sec = aDate->GetAttributeAsDword("sec", 1);
        const int32_t min = aDate->GetAttributeAsDword("min", 1);
        hour = static_cast<float>(aDate->GetAttributeAsDword("hour", static_cast<int32_t>(hour)));
        hour += (min + sec / 60.0f) / 60.0f;
        day = aDate->GetAttributeAsDword("day", day);
        mon = aDate->GetAttributeAsDword("month", mon);
        year = aDate->GetAttributeAsDword("year", year);
        timeScale = aDate->GetAttributeAsFloat("hourPerSec", timeScale);
    }
    ResetScriptInterfaces();
    rs->ProgressView();
    // Creating interface elements

    // Date
    auto *windUI = new WdmWindUI();
    windUI->SetAttributes(AttributesPointer);
    AddLObject(AddObject(windUI, 1001), 10100);

    // Compass
    // ro = CreateModel(new WdmWindRose(), "WindRose");
    // AddLObject(ro, 10099);
    // The calendar
    // WdmCounter * cnt = new WdmCounter();
    // if(!cnt->Init()) core.Trace("Counter not created");

    // AddLObject(cnt, 10099);
    // Icon
    AddLObject(AddObject(new WdmIcon(), 1000), 10099);

    // load encounters, if any
    if (saveData)
    {
        const auto num = saveData->GetAttributesNum();
        for (uint32_t i = 0; i < num; i++)
        {
            auto *a = saveData->GetAttributeClass(i);
            if (!a)
                continue;
            const char *type = a->GetAttribute("type");
            const char *modelName = a->GetAttribute("modelName");
            if (!type || !type[0])
            {
                saveData->DeleteAttributeClassX(a);
                continue;
            }
            if (storm::iEquals(type, "Merchant") && modelName && modelName[0])
            {
                if (!CreateMerchantShip(modelName, nullptr, nullptr, 1.0f, -1.0f, a))
                {
                    core.Trace("WoldMap: not loaded merchant encounter.");
                }
                continue;
            }
            if (storm::iEquals(type, "Follow") && modelName && modelName[0])
            {
                if (!CreateFollowShip(modelName, 1.0f, -1.0f, a))
                {
                    core.Trace("WoldMap: not loaded follow encounter.");
                }
                continue;
            }
            if (storm::iEquals(type, "Warring") && modelName && modelName[0])
            {
                const char *attacked = a->GetAttribute("attacked");
                if (attacked)
                {
                    auto *a1 = saveData->FindAClass(saveData, attacked);
                    if (a1)
                    {
                        const char *modelName1 = a1->GetAttribute("modelName");
                        if (modelName1 && modelName1[0])
                        {
                            if (!CreateWarringShips(modelName, modelName1, -1.0f, a, a1))
                            {
                                core.Trace("WoldMap: not loaded warring encounter.");
                            }
                        }
                        else
                        {
                            core.Trace("WoldMap: not loaded warring encounter.");
                            saveData->DeleteAttributeClassX(a);
                            saveData->DeleteAttributeClassX(a1);
                        }
                    }
                }
                continue;
            }
            if (storm::iEquals(type, "Attacked"))
            {
                continue;
            }
            if (storm::iEquals(type, "Storm"))
            {
                const auto isTornado = (a->GetAttributeAsDword("isTornado", 0) != 0);
                if (!CreateStorm(isTornado, -1.0f, a))
                {
                    core.Trace("WoldMap: not loaded storm encounter.");
                }
                continue;
            }
            saveData->DeleteAttributeClassX(a);
        }
    }

    rs->ProgressView();

    // Adjusting the player's ship
    auto *playerShip = static_cast<WdmPlayerShip *>(wdmObjects->playerShip);
    playerShip->PushOutFromIsland();
    auto *const atrData = AttributesPointer->FindAClass(AttributesPointer, "island");
    if (atrData)
    {
        float x, z, ay;
        playerShip->GetPosition(x, z, ay);
        if (!wdmObjects->islands->CheckIslandArea(atrData->GetThisAttr(), x, z))
        {
            wdmObjects->islands->GetNearPointToArea(atrData->GetThisAttr(), x, z);
            playerShip->Teleport(x, z, ay);
        }
    }
    return true;
    // UNGUARD
}

// Execution
void WorldMap::Execute(uint32_t delta_time)
{
}

void WorldMap::Realize(uint32_t delta_time)
{
    if (AttributesPointer && wdmObjects->playerShip)
    {
        CVECTOR wind(0.0f);
        float x, z, ay;
        wdmObjects->playerShip->GetPosition(x, z, ay);
        const auto force = wdmObjects->GetWind(x, z, wind);
        AttributesPointer->SetAttributeUseFloat("WindX", wind.x);
        AttributesPointer->SetAttributeUseFloat("WindZ", wind.z);
        AttributesPointer->SetAttributeUseFloat("WindF", force);
    }
    if (!wdmObjects->isPause)
    {
        CONTROL_STATE cs;
        core.Controls->GetControlState("WMapCancel", cs);
        if (cs.state == CST_ACTIVATED)
        {
            if (wdmObjects->playerShip)
            {
                if (!static_cast<WdmPlayerShip *>(wdmObjects->playerShip)->ExitFromMap())
                    core.Event("ExitFromWorldMap");
            }
            else
                core.Event("ExitFromWorldMap");
        }
    }
    //---------------------------------------------------------
    const auto dltTime = 0.001f * delta_time;
    // Updating the date
    if (hour < 0.0f)
        hour = 0.0f;
    hour += dltTime * timeScale;
    auto days = static_cast<int32_t>(hour / 24.0f);
    hour = (hour / 24.0f - days) * 24.0f;
    const auto dtHour = static_cast<int32_t>(hour);
    const auto dtMin = static_cast<int32_t>((hour - dtHour) * 60.0f);
    const auto dtSec = static_cast<int32_t>(((hour - dtHour) * 60.0f - dtMin) * 60.0f);
    aDate->SetAttributeUseDword("sec", dtSec);
    aDate->SetAttributeUseDword("min", dtMin);
    aDate->SetAttributeUseDword("hour", dtHour);
    if (days)
    {
        for (; days > 0; days--)
        {
            day++;
            if (day > month[mon])
            {
                day -= month[mon++];
                if (mon > 12)
                {
                    mon = 1;
                    year++;
                    aDate->SetAttributeUseDword("year", year);
                }
                aDate->SetAttributeUseDword("month", mon);
            }
            aDate->SetAttributeUseDword("day", day);

#ifndef EVENTS_OFF
            core.Event("WorldMap_UpdateDate", "f", hour);
            wdmObjects->isNextDayUpdate = true;
            core.Event("NextDay");
#endif
        }
    }
    else
    {
#ifndef EVENTS_OFF
        core.Event("WorldMap_UpdateDate", "f", hour);
#endif
    }
    //
    const char *tmp = aDate->GetAttribute("sec");
    if (tmp)
        strcpy_s(wdmObjects->attrSec, tmp);
    tmp = aDate->GetAttribute("min");
    if (tmp)
        strcpy_s(wdmObjects->attrMin, tmp);
    tmp = aDate->GetAttribute("hour");
    if (tmp)
        strcpy_s(wdmObjects->attrHour, tmp);
    tmp = aDate->GetAttribute("day");
    if (tmp)
        strcpy_s(wdmObjects->attrDay, tmp);
    tmp = aDate->GetAttribute("month");
    if (tmp)
        strcpy_s(wdmObjects->attrMonth, tmp);
    tmp = aDate->GetAttribute("year");
    if (tmp)
        strcpy_s(wdmObjects->attrYear, tmp);
    //---------------------------------------------------------
    if (camera && !wdmObjects->isPause)
    {
        camera->Move(dltTime, rs);
        AttributesPointer->SetAttributeUseFloat("wdmCameraY", camera->pos.y);
        AttributesPointer->SetAttributeUseFloat("wdmCameraAY", camera->ang.y);
    }
    auto isKill = false;
    // execute all objects
    for (auto i = firstObject; i >= 0; i = object[i].next)
    {
        if (!object[i].ro->killMe)
            object[i].ro->Update(object[i].ro->isEnablePause && wdmObjects->isPause ? 0.0f : dltTime);
        isKill |= object[i].ro->killMe;
    }
    // Remove objects if necessary
    if (isKill)
    {
        for (auto i = firstObject; i >= 0;)
            if (object[i].ro->killMe)
            {
                DeleteObject(object[i].ro);
                i = firstObject;
            }
            else
                i = object[i].next;
    }
    // Current number of events
    if (aStorm)
        aStorm->SetAttributeUseDword("num", wdmObjects->storms.size());
    if (aEncounter)
        aEncounter->SetAttributeUseDword("num", wdmObjects->ships.size() - (wdmObjects->playerShip != nullptr));
    // Events
    encTime += dltTime;
    if (encTime >= 1.0f && wdmObjects->playerShip && !wdmObjects->isPause)
    {
        auto psx = 0.0f, psz = 0.0f, psay = 0.0f;
        wdmObjects->playerShip->GetPosition(psx, psz, psay);
#ifndef ENCS_OFF
        core.Event("WorldMap_EncounterCreate", "ffff", encTime, psx, psz, psay);
#endif
        encTime = 0.0f;
    }
    rs->SetRenderState(D3DRS_FOGENABLE, FALSE);
    rs->SetRenderState(D3DRS_LIGHTING, FALSE);
    for (auto i = firstPrObject; i >= 0; i = object[i].next)
    {
        if (!object[i].ro->killMe)
            object[i].ro->PRender(rs);
    }
    for (auto i = firstMrObject; i >= 0; i = object[i].next)
    {
        if (!object[i].ro->killMe)
            object[i].ro->MRender(rs);
    }
    for (auto i = firstLrObject; i >= 0; i = object[i].next)
    {
        if (!object[i].ro->killMe)
            object[i].ro->LRender(rs);
    }
    // renew the wind
    wdmObjects->UpdateWind(dltTime);
    wdmObjects->isNextDayUpdate = false;
    // Notify scripts about wind change
    auto wind_dir = CVECTOR();
    auto x = 0.0f;
    auto z = 0.0f;
    auto ay = 0.0f;
    wdmObjects->playerShip->GetPosition(x, z, ay);
    const auto wind_force = wdmObjects->GetWind(x, z, wind_dir);
    const auto ang = atan2(wind_dir.x, wind_dir.z);
    core.Event("WorldMap_GetWindParameters", "ff", wind_force, ang);
    // Checking the update attribute of the encounter
    if (AttributesPointer)
    {
        const char *upd = AttributesPointer->GetAttribute("addQuestEncounters");
        if (upd && upd[0] != 0)
        {
            core.Event("WorldMap_AddQuestEncounters");
        }
    }
}

// Messages
uint64_t WorldMap::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case MSG_WORLDMAP_CREATESTORM: {
        const auto isTornado = message.Long() != 0;
        CreateStorm(isTornado);
    }
    break;
    case MSG_WORLDMAP_CREATEENC_MER: {
        const std::string &sName = message.String();
        const std::string &buf = message.String();
        const std::string &sName2 = message.String();
        const auto kSpeed = message.Float();
        const auto timeOut = message.Float();                                                   // boal
        return CreateMerchantShip(sName.c_str(), buf.c_str(), sName2.c_str(), kSpeed, timeOut); // boal
    }
    break;
        // boal 04/01/06 -->
    case MSG_WORLDMAP_CREATEENC_MER_XZ: {
        const std::string &sName = message.String();
        const auto fx1 = message.Float();
        const auto fz1 = message.Float();
        const auto fx2 = message.Float();
        const auto fz2 = message.Float();
        const auto kSpeed = message.Float();
        const auto timeOut = message.Float();
        return CreateMerchantShipXZ(sName.c_str(), fx1, fz1, fx2, fz2, kSpeed, timeOut);
    }
    break;
        // boal <--
    case MSG_WORLDMAP_CREATEENC_FLW: {
        const std::string &sName = message.String();
        const auto kSpeed = message.Float();
        const auto timeOut = message.Float();
        return CreateFollowShip(sName.c_str(), kSpeed, timeOut);
    }
    break;
    case MSG_WORLDMAP_CREATEENC_WAR: {
        const std::string &sName = message.String();
        const std::string &sName2 = message.String();
        const auto timeOut = message.Float();
        return CreateWarringShips(sName.c_str(), sName2.c_str(), timeOut);
    }
    break;
    case MSG_WORLDMAP_CREATEENC_RELEASE:
        ReleaseEncounters();
        break;
    case MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA:
        if (wdmObjects->playerShip)
        {
            if (!static_cast<WdmPlayerShip *>(wdmObjects->playerShip)->ExitFromMap())
                core.Event("ExitFromWorldMap");
        }
        else
            core.Event("ExitFromWorldMap");
        break;
    case MSG_WORLDMAP_SET_NATION_FLAG:
        wdmObjects->nationFlagIndex = message.Long();
        break;
    case MSG_WORLDMAP_SET_COORDINATES:
        wdmObjects->coordinate = message.String();
        break;
    }
    return 0;
}

// Changing an attribute
uint32_t WorldMap::AttributeChanged(ATTRIBUTES *apnt)
{
    float x, z, ay;
    if (!apnt || !AttributesPointer)
        return 0;
    if (storm::iEquals(apnt->GetThisName(), "deleteUpdate"))
    {
        for (int32_t i = 0; i < wdmObjects->ships.size(); i++)
        {
            if (wdmObjects->ships[i] == wdmObjects->playerShip)
                continue;
            static_cast<WdmEnemyShip *>(wdmObjects->ships[i])->DeleteUpdate();
        }
        for (int32_t i = 0; i < wdmObjects->storms.size(); i++)
        {
            wdmObjects->storms[i]->DeleteUpdate();
        }
    }
    else if (storm::iEquals(apnt->GetThisName(), "playerShipUpdate"))
    {
        if (wdmObjects->playerShip)
        {
            float x, z, ay;
            wdmObjects->playerShip->GetPosition(x, z, ay);
            AttributesPointer->SetAttributeUseFloat("playerShipX", x);
            AttributesPointer->SetAttributeUseFloat("playerShipZ", z);
            AttributesPointer->SetAttributeUseFloat("playerShipAY", ay);
        }
    }
    else if (storm::iEquals(apnt->GetThisName(), "cur"))
    {
        auto *pa = apnt->GetParent();
        if (pa == aStorm)
        {
            const auto cur = static_cast<int32_t>(pa->GetAttributeAsDword("cur"));
            if (cur >= 0 && cur < wdmObjects->storms.size())
            {
                Assert(wdmObjects->storms[cur]);
                wdmObjects->storms[cur]->GetPosition(x, z);
                pa->SetAttributeUseFloat("x", x);
                pa->SetAttributeUseFloat("z", z);
                pa->SetAttributeUseFloat("time", wdmObjects->storms[cur]->GetLiveTime());
            }
            else
            {
                pa->SetAttributeUseDword("cur", -1);
            }
        }
        else if (pa == aEncounter)
        {
            const auto cur = static_cast<int32_t>(pa->GetAttributeAsDword("cur"));
            // Determine the encounter index
            int32_t i = 0;
            for (int32_t enc = 0; i < wdmObjects->ships.size(); i++)
            {
                if (wdmObjects->ships[i] == wdmObjects->playerShip)
                    continue;
                if (enc == cur)
                    break;
                enc++;
            }
            if (i < wdmObjects->ships.size())
            {
                Assert(wdmObjects->ships[i]);
                wdmObjects->ships[i]->GetPosition(x, z, ay);
                pa->SetAttributeUseFloat("x", x);
                pa->SetAttributeUseFloat("z", z);
                pa->SetAttributeUseFloat("ay", ay);
                auto *const es = static_cast<WdmEnemyShip *>(wdmObjects->ships[i]);
                pa->SetAttributeUseFloat("time", es->GetLiveTime());
                char buf[32];
                sprintf_s(buf, "%i", es->type);
                pa->SetAttribute("type", buf);
                pa->SetAttributeUseDword("select", es->isSelect);
                pa->SetAttribute("id", (char *)static_cast<WdmEnemyShip *>(wdmObjects->ships[i])->GetAttributeName());
                // If there is an attacker, get his index
                if (es->attack)
                {
                    Assert(es->attack != es);
                    int32_t i, j = 0;
                    for (i = 0; i < wdmObjects->ships.size(); i++)
                    {
                        if (wdmObjects->ships[i] == wdmObjects->playerShip)
                            continue;
                        if (wdmObjects->ships[i] == es->attack)
                            break;
                        j++;
                    }
                    if (i >= wdmObjects->ships.size())
                        j = -1;
                    pa->SetAttributeUseDword("attack", j);
                }
                else
                {
                    pa->SetAttributeUseDword("attack", -1);
                }
            }
            else
            {
                pa->SetAttributeUseDword("cur", -1);
            }
        }
    }
    else if (storm::iEquals(apnt->GetThisName(), "updateinfo"))
    {
        auto *pa = apnt->GetParent();
        if (pa == aInfo)
        {
            pa->SetAttributeUseDword("playerInStorm", static_cast<int32_t>(wdmObjects->playarInStorm));
        }
    }
    else
    {
        for (auto *pa = apnt; pa; pa = pa->GetParent())
        {
            if (storm::iEquals(pa->GetThisName(), "labels"))
            {
                wdmObjects->islands->SetIslandsData(AttributesPointer, true);
                return 0;
            }
        }
    }
    return 0;
}

// ============================================================================================
// Objects management
// ============================================================================================

// Add object
WdmRenderObject *WorldMap::AddObject(WdmRenderObject *obj, int32_t level)
{
    if (!obj)
        return nullptr;
    const auto i = GetObject(firstObject, level);
    object[i].ro = obj;
    return obj;
}

// Add object to render list before reflection
void WorldMap::AddPObject(WdmRenderObject *obj, int32_t level)
{
    if (!obj)
        return;
    const auto i = GetObject(firstPrObject, level);
    object[i].ro = obj;
}

// Add object to reflection render list
void WorldMap::AddMObject(WdmRenderObject *obj, int32_t level)
{
    if (!obj)
        return;
    const int32_t i = GetObject(firstMrObject, level);
    object[i].ro = obj;
}

// Add object to render list after reflection
void WorldMap::AddLObject(WdmRenderObject *obj, int32_t level)
{
    if (!obj)
        return;
    const int32_t i = GetObject(firstLrObject, level);
    object[i].ro = obj;
}

// Delete object
void WorldMap::DeleteObject(WdmRenderObject *obj)
{
    if (!obj)
        return;
    // go through all the lists, deleting the entry about the object
    for (int32_t i = firstObject, j; i >= 0;)
    {
        j = i;
        i = object[i].next;
        if (object[j].ro == obj)
            FreeObject(firstObject, j);
    }
    for (int32_t i = firstPrObject, j; i >= 0;)
    {
        j = i;
        i = object[i].next;
        if (object[j].ro == obj)
            FreeObject(firstPrObject, j);
    }

    for (int32_t i = firstMrObject, j; i >= 0;)
    {
        j = i;
        i = object[i].next;
        if (object[j].ro == obj)
            FreeObject(firstMrObject, j);
    }
    for (int32_t i = firstLrObject, j; i >= 0;)
    {
        j = i;
        i = object[i].next;
        if (object[j].ro == obj)
            FreeObject(firstLrObject, j);
    }
    delete obj;
}

// ============================================================================================
// Encapsulation
// ============================================================================================

// Objects management

// Include a record about an object in the list with the required level
int32_t WorldMap::GetObject(int32_t &first, int32_t level)
{
    Assert(firstFreeObject >= 0);
    const int32_t i = firstFreeObject;
    firstFreeObject = object[firstFreeObject].next;
    object[i].ro = nullptr;
    object[i].level = level;
    object[i].prev = -1;
    object[i].next = -1;
    if (first >= 0)
    {
        if (level >= object[first].level)
        {
            int32_t j;
            for (j = first; object[j].next >= 0 && level >= object[object[j].next].level; j = object[j].next)
                ;
            object[i].prev = j;
            object[i].next = object[j].next;
            object[j].next = i;
            if (object[i].next >= 0)
                object[object[i].next].prev = i;
        }
        else
        {
            object[i].next = first;
            first = i;
        }
    }
    else
        first = i;
    return i;
}

// Exclude entry from the list
void WorldMap::FreeObject(int32_t &first, int32_t i)
{
    Assert(i >= 0.0f && i < WDMAP_MAXOBJECTS);
    object[i].ro = nullptr;
    object[i].level = 0;
    if (object[i].next >= 0)
        object[object[i].next].prev = object[i].prev;
    if (object[i].prev >= 0)
        object[object[i].prev].next = object[i].next;
    else
        first = object[i].next;
    object[i].prev = -1;
    object[i].next = firstFreeObject;
    firstFreeObject = i;
}

// Utilities

// Initialize the model and add it to the required render lists
WdmRenderObject *WorldMap::CreateModel(WdmRenderModel *rm, const char *modelName, bool pr, bool mr, bool lr,
                                       int32_t objectLevel, int32_t drawLevel)
{
    if (!modelName || !modelName[0])
    {
        delete rm; // boal fix needs to be deleted
        return nullptr;
    }
    if (!rm->Load(modelName))
    {
        delete rm;
        return nullptr;
    }
    AddObject(rm, objectLevel);
    if (pr)
        AddPObject(rm, drawLevel);
    if (mr)
        AddMObject(rm, drawLevel);
    if (lr)
        AddLObject(rm, drawLevel);
    return rm;
}

// Create a storm if possible and set a lifetime
bool WorldMap::CreateStorm(bool isTornado, float time, ATTRIBUTES *save)
{
    if (wdmObjects->storms.size() >= WDM_MAX_STORMS)
        return false;
    auto *s = new WdmStorm();
    AddLObject(s, 800);
    if (!AddObject(s))
        return false;
    if (time > 0.0f)
    {
        if (time < 1.0f)
            time = 1.0f;
        s->SetLiveTime(time);
    }
    if (!save)
        save = GetEncSaveData("Storm", "EncounterID1");
    s->SetSaveAttribute(save);
    s->isTornado = isTornado;
    return true;
}

// Create a merchant's ship
bool WorldMap::CreateMerchantShip(const char *modelName, const char *locNameStart, const char *locNameEnd, float kSpeed,
                                  float time, ATTRIBUTES *save)
{
    if (kSpeed < 0.1f)
        kSpeed = 0.1f;
    WdmShip *ship = new WdmMerchantShip();
    if (ship->killMe)
    {
        delete ship;
        return false;
    }
    if (!CreateModel(ship, modelName))
        return false;
    AddLObject(ship, 100);
    // Looking for a place to sail
    if (!wdmObjects->islands)
    {
        core.Trace("World map: Islands not found");
    }
    CVECTOR gpos;
    if (!locNameEnd || !locNameEnd[0])
    {
        if (wdmObjects->islands)
        {
            if (!wdmObjects->islands->GetRandomMerchantPoint(gpos))
            {
                core.Trace("World map: Locators <Merchants:...> not found");
            }
        }
    }
    else
    {
        if (wdmObjects->islands)
        {
            if (!wdmObjects->islands->GetQuestLocator(locNameEnd, gpos))
            {
                core.Trace("World map: Quest locator <Quests:%s> for merchant not found", locNameEnd);
            }
        }
    }
    static_cast<WdmMerchantShip *>(ship)->Goto(gpos.x, gpos.z, 2.0f);
    // If necessary, change the current position
    if (locNameStart && locNameStart[0])
    {
        if (wdmObjects->islands)
        {
            if (wdmObjects->islands->GetQuestLocator(locNameStart, gpos))
            {
                ship->Teleport(gpos.x, gpos.z, rand() * (PI * 2.0f / RAND_MAX));
            }
            else
            {
                core.Trace("World map: Quest locator <Quests:%s> for merchant not found", locNameStart); // boal fix
            }
        }
    }
    // Speed
    ship->SetMaxSpeed(kSpeed);
    // Lifetime
    if (time >= 0.0f)
    {
        if (time < 3.0f)
            time = 3.0f;
        static_cast<WdmEnemyShip *>(ship)->SetLiveTime(time);
    }
    if (!save)
        save = GetEncSaveData("Merchant", "EncounterID1");
    if (save)
    {
        save->SetAttribute("modelName", (char *)modelName);
    }
    static_cast<WdmEnemyShip *>(ship)->SetSaveAttribute(save);
    return true;
}

// boal Create a merchant's ship in coordinates
bool WorldMap::CreateMerchantShipXZ(const char *modelName, float x1, float z1, float x2, float z2, float kSpeed,
                                    float time, ATTRIBUTES *save)
{
    if (kSpeed < 0.1f)
        kSpeed = 0.1f;
    WdmShip *ship = new WdmMerchantShip();
    if (ship->killMe)
    {
        delete ship;
        return false;
    }
    if (!CreateModel(ship, modelName))
        return false;
    AddLObject(ship, 100);
    // Looking for a place to sail
    if (!wdmObjects->islands)
    {
        core.Trace("World map: Islands not found");
    }

    static_cast<WdmMerchantShip *>(ship)->Goto(x2, z2, 2.0f); // where
    // If necessary, change the current position
    ship->Teleport(x1, z1, rand() * (PI * 2.0f / RAND_MAX)); // from where
    // Speed
    ship->SetMaxSpeed(kSpeed);
    // Lifetime
    if (time >= 0.0f)
    {
        if (time < 3.0f)
            time = 3.0f;
        static_cast<WdmEnemyShip *>(ship)->SetLiveTime(time);
    }
    if (!save)
        save = GetEncSaveData("Merchant", "EncounterID1");
    if (save)
    {
        save->SetAttribute("modelName", (char *)modelName);
    }
    static_cast<WdmEnemyShip *>(ship)->SetSaveAttribute(save);
    return true;
}

// Create a ship that follows us
bool WorldMap::CreateFollowShip(const char *modelName, float kSpeed, float time, ATTRIBUTES *save)
{
    if (kSpeed < 0.1f)
        kSpeed = 0.1f;
    WdmShip *ship = new WdmFollowShip();
    if (ship->killMe)
    {
        delete ship;
        return false;
    }
    if (!CreateModel(ship, modelName))
        return false;
    AddLObject(ship, 100);
    // Speed
    ship->SetMaxSpeed(kSpeed);
    // Lifetime
    if (time >= 0.0f)
    {
        if (time < 1.0f)
            time = 1.0f;
        static_cast<WdmEnemyShip *>(ship)->SetLiveTime(time);
    }
    VDATA *isSkipEnable = core.Event("WorldMap_IsSkipEnable");
    if (isSkipEnable)
    {
        int32_t skipEnable = 0;
        if (isSkipEnable->Get(skipEnable))
        {
            static_cast<WdmEnemyShip *>(ship)->canSkip = skipEnable != 0;
        }
    }
    if (!save)
        save = GetEncSaveData("Follow", "EncounterID1");
    if (save)
    {
        save->SetAttribute("modelName", (char *)modelName);
    }
    static_cast<WdmEnemyShip *>(ship)->SetSaveAttribute(save);
    return true;
}

bool WorldMap::CreateWarringShips(const char *modelName1, const char *modelName2, float time, ATTRIBUTES *save1,
                                  ATTRIBUTES *save2)
{
    static const float pi = 3.14159265359f;
    // Create ships
    auto *ship1 = new WdmWarringShip();
    if (ship1->killMe)
    {
        delete ship1;
        return false;
    }
    if (!CreateModel(ship1, modelName1))
        return false;
    auto *ship2 = new WdmWarringShip();
    if (ship2->killMe)
    {
        delete ship2;
        return false;
    }
    if (!CreateModel(ship2, modelName2))
        return false;
    const float moveRadius = (ship1->modelRadius + ship2->modelRadius) * (0.4f + (rand() & 3) * (0.1f / 3.0f));
    const float fullRadius = 0.6f * (moveRadius + 2.0f * std::max(ship1->modelRadius, ship2->modelRadius));
    // General position
    float x, z;
    if (!WdmEnemyShip::GeneratePosition(fullRadius, 1.5f, x, z))
        return false;
    // General angle
    const float angl = rand() * 2.0f * pi / (RAND_MAX + 1);
    // Center offset
    const float dx = moveRadius * cosf(angl);
    const float dz = -moveRadius * sinf(angl);
    AddLObject(ship1, 100);
    ship1->Teleport(x + dx, z + dz, angl + pi * (rand() & 1));
    AddLObject(ship2, 100);
    ship2->Teleport(x - dx, z - dz, angl + pi * (rand() & 1));
    ship2->SetLiveTime(ship1->GetLiveTime());
    // Setting characteristics
    ship1->attack = ship2;
    ship2->attack = ship1;
    // Lifetime
    if (time >= 0.0f)
    {
        if (time < 1.0f)
            time = 1.0f;
        ship1->SetLiveTime(time);
        ship2->SetLiveTime(time);
    }
    if (!save2)
        save2 = GetEncSaveData("Attacked", "EncounterID1");
    if (!save1)
        save1 = GetEncSaveData("Warring", "EncounterID2");
    if (save1 && save2)
    {
        save1->SetAttribute("attacked", save2->GetThisName());
    }
    if (save1)
    {
        save1->SetAttribute("modelName", (char *)modelName1);
    }
    if (save2)
    {
        save2->SetAttribute("modelName", (char *)modelName2);
    }
    ship1->SetSaveAttribute(save1);
    ship2->SetSaveAttribute(save2);
    return true;
}

// Find coordinates and radius by destination
bool WorldMap::FindIslandPosition(const char *name, float &x, float &z, float &r)
{
    return false;
}

void WorldMap::ResetScriptInterfaces() const
{
    if (aStorm)
    {
        aStorm->SetAttributeUseDword("num", 0);
        aStorm->SetAttributeUseDword("cur", 0);
        aStorm->SetAttributeUseDword("x", 0);
        aStorm->SetAttributeUseDword("z", 0);
        aStorm->SetAttributeUseDword("time", 0);
    }
    if (aEncounter)
    {
        aEncounter->SetAttributeUseDword("num", 0);
        aEncounter->SetAttributeUseDword("cur", 0);
        aEncounter->SetAttributeUseDword("x", 0);
        aEncounter->SetAttributeUseDword("z", 0);
        aEncounter->SetAttributeUseDword("ay", 0);
        aEncounter->SetAttributeUseDword("time", 0);
        aEncounter->SetAttribute("type", "-1");
        aEncounter->SetAttributeUseDword("attack", -1);
        aEncounter->SetAttributeUseDword("nation", -1);
        aEncounter->SetAttribute("id", "");
    }
    if (aInfo)
    {
        aInfo->SetAttributeUseDword("playerInStorm", 0);
    }
}

// Delete all encounters
void WorldMap::ReleaseEncounters()
{
    // leave the encounter parameters intact
    for (int32_t i = 0; i < wdmObjects->ships.size(); i++)
    {
        if (wdmObjects->ships[i] == wdmObjects->playerShip)
            continue;
        static_cast<WdmEnemyShip *>(wdmObjects->ships[i])->SetSaveAttribute(nullptr);
        wdmObjects->ships[i]->killMe = true;
    }
    for (int32_t i = 0; i < wdmObjects->storms.size(); i++)
    {
        wdmObjects->storms[i]->SetSaveAttribute(nullptr);
        wdmObjects->storms[i]->killMe = true;
    }
}

// Create an attribute to save the encounter parameters
ATTRIBUTES *WorldMap::GetEncSaveData(const char *type, const char *retName)
{
    if (!saveData)
        return nullptr;
    // Generating the name of the attribute
    encCounter++;
    char atrName[64];
    int32_t i;
    for (i = 0; i < 1000000; i++, encCounter++)
    {
        sprintf_s(atrName, "enc_%u", encCounter);
        ATTRIBUTES *a = saveData->FindAClass(saveData, atrName);
        if (!a)
            break;
        if (a->FindAClass(a, "needDelete"))
        {
            saveData->DeleteAttributeClassX(a);
            break;
        }
    }
    if (i == 1000000)
        return nullptr;
    // Create a branch
    ATTRIBUTES *a = saveData->CreateSubAClass(saveData, atrName);
    if (!a)
        return nullptr;
    // Set the type
    a->SetAttribute("type", (char *)type);
    // Save the name
    if (AttributesPointer)
    {
        AttributesPointer->SetAttribute((char *)retName, atrName);
    }
    return a;
}
