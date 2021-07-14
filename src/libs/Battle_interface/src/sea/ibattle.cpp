#include <stdio.h>

#include "ibattle.h"

#include "core.h"

#include "ShipPointer.h"
#include "battle_shipsign.h"
#include "island_descr.h"

#include "../land/IBoardingStatus.h"
#include "../land/battle_land.h"

#include "../ActivePerkShower.h"
#include "../ItemEntity/itementity.h"
#include "../LogAndAction.h"
#include "../WorldMapInterface/interface.h"
#include "../spyglass/spyglass.h"
#include "../timer/Timer.h"

#include "shared/battle_interface/msg_control.h"

#include "../interfacemanager/InterfaceManager.h"

#include "ShipInfoImages.h"

CREATE_CLASS(BATTLE_INTERFACE)

CREATE_CLASS(ILogAndActions)

CREATE_CLASS(IBoardingStatus)

CREATE_CLASS(BATTLE_LAND_INTERFACE)

CREATE_CLASS(ISPYGLASS)

CREATE_CLASS(SHIPPOINTER)

CREATE_CLASS(ActivePerkShower)

CREATE_CLASS(BITimer)

CREATE_CLASS(ItemEntity)

CREATE_CLASS(WM_INTERFACE)

CREATE_CLASS(BI_InterfaceManager)

SHIP_DESCRIBE_LIST g_ShipList;
ISLAND_DESCRIBER g_IslandDescr;

#define MINIMAP_ZOOM_IN "MiniMapZoomIn"
#define MINIMAP_ZOOM_OUT "MiniMapZoomOut"

BATTLE_INTERFACE::BATTLE_INTERFACE()
{
    g_IslandDescr.ReleaseAll();
    g_ShipList.ReleaseAll();
    // m_pMessageIcons = null;
    m_pShipIcon = nullptr;
    rs = nullptr;
    m_bShowCommandMenu = true;
    m_bShowBattleNavigator = true;
    m_bYesShowAll = false;
    m_bVisible = true;
    m_fBlinkSpeed = 0.f;
    m_fCurBlinkTime = 0.f;
    m_bNeedIslandSet = true;
    m_bMyShipView = false;
    m_bShowBattleBorder = false;
    m_pShipInfoImages = nullptr;
}

BATTLE_INTERFACE::~BATTLE_INTERFACE()
{
    // STORM_DELETE(m_pMessageIcons);
    STORM_DELETE(m_pShipIcon);
    m_TextArray.clear();
    m_LinesInfo.Release();
    m_ImagesInfo.Release();
    STORM_DELETE(m_pShipInfoImages);
}

bool BATTLE_INTERFACE::Init()
{
    BIUtils::idBattleInterface = GetId();

    if ((rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"))) == nullptr)
    {
        throw std::runtime_error("Can`t create render service");
    }

    LoadIniFile();

    return true;
}

void BATTLE_INTERFACE::Execute(uint32_t delta_time)
{
    m_fCurBlinkTime += delta_time * m_fBlinkSpeed;
    if (m_fCurBlinkTime > PIm2)
        m_fCurBlinkTime -= PIm2;
    BIUtils::g_dwBlinkColor = static_cast<uint32_t>(255 * fabsf(sinf(m_fCurBlinkTime))) & 0xFF;
    CheckSeaState();
    if (m_bYesShowAll && m_bVisible)
    {
        CONTROL_STATE cs;

        if (m_bShowBattleNavigator)
        {
            core.Controls->GetControlState(MINIMAP_ZOOM_IN, cs);
            if (cs.state == CST_ACTIVE)
                BattleNavigator.DecrementScale();
            core.Controls->GetControlState(MINIMAP_ZOOM_OUT, cs);
            if (cs.state == CST_ACTIVE)
                BattleNavigator.IncrementScale();
        }

        if (m_bShowCommandMenu && m_pShipIcon)
        {
            if (!m_pShipIcon->IsActive())
            {
                core.Controls->GetControlState(BI_COMMANDS_ACTIVATE_SEA, cs);
                if (cs.state == CST_ACTIVATED)
                {
                    m_pShipIcon->SetActive(true);
                }
            }
            else
            {
                m_pShipIcon->MakeControl();
            }
        }
        if (m_bShowBattleNavigator)
            BattleNavigator.Update();

        /*if(m_pMessageIcons)
        {
          if( !m_pShipIcon || !m_pShipIcon->IsActive() )
          {
            if(!m_bMyShipView) core.Event("blieGetMsgIconRoot");
            m_bMyShipView = true;
          }
          else
          {
            if(m_bMyShipView) m_pMessageIcons->SetShowMsg(false);
            m_bMyShipView = false;
          }
        }*/
    }
    // if(m_pMessageIcons) m_pMessageIcons->Update(delta_time);
}

void BATTLE_INTERFACE::Realize(uint32_t delta_time)
{
    if (core.Controls->GetDebugAsyncKeyState('K') < 0)
        return;
    if (m_bNeedIslandSet)
    {
        core.Trace("Island Set");
        BattleNavigator.SetIsland();
        m_bNeedIslandSet = false;
    }

    if (m_bYesShowAll)
    {
        if (m_bVisible)
        {
            rs->MakePostProcess();

            if (m_bShowBattleNavigator && m_pShipInfoImages)
                m_pShipInfoImages->Draw();

            if (m_bShowBattleNavigator)
                m_ImagesInfo.Draw();

            if (m_bShowCommandMenu)
                m_pShipIcon->Draw();
            if (m_bShowBattleNavigator)
                BattleNavigator.Draw();
            // if( m_pMessageIcons ) m_pMessageIcons->Draw();

            if (m_bShowBattleNavigator)
            {
                BIUtils::PrintTextInfoArray(m_TextArray);
                m_LinesInfo.Draw();
                if (m_bShowBattleBorder)
                    m_BattleBorder.Draw();
            }
        }
    }
    else
    {
        m_bYesShowAll = true;
    }
}

void BATTLE_INTERFACE::LoadIniFile()
{
    m_fBlinkSpeed = .003f;
    if (AttributesPointer != nullptr)
        m_fBlinkSpeed = AttributesPointer->GetAttributeAsFloat("blindSpeed", m_fBlinkSpeed);
    m_fCurBlinkTime = 0;

    BattleNavigator.Init(rs, this);

    ATTRIBUTES *pA = nullptr;
    if (AttributesPointer != nullptr)
    {
        m_bShowCommandMenu = AttributesPointer->GetAttributeAsDword("ShowCommands", 1) != 0;
        m_bShowBattleNavigator = AttributesPointer->GetAttributeAsDword("ShowNavigator", 1) != 0;
        pA = AttributesPointer->FindAClass(AttributesPointer, "MessageIcons");

        BIUtils::FillTextInfoArray(rs, AttributesPointer->GetAttributeClass("TextInfo"), m_TextArray);
        m_LinesInfo.Init(rs, AttributesPointer->GetAttributeClass("LineInfo"));

        m_BattleBorder.Init(rs, AttributesPointer->GetAttributeClass("battleborder"));

        m_ImagesInfo.Init(rs, AttributesPointer->GetAttributeClass("imageslist"));
    }

    /*STORM_DELETE( m_pMessageIcons );
    m_pMessageIcons = new MESSAGE_ICONS;
    if(m_pMessageIcons==NULL) {
      throw std::runtime_error("allocate memory error");
    }
    if(m_pMessageIcons)    m_pMessageIcons->InitData(GetId(),rs,pA);*/

    STORM_DELETE(m_pShipIcon);
    m_pShipIcon = new BIShipIcon(GetId(), rs);
    Assert(m_pShipIcon);
    m_pShipIcon->Init(AttributesPointer,
                      AttributesPointer ? AttributesPointer->GetAttributeClass("ShipIcon") : nullptr);

    m_pShipInfoImages =
        new ShipInfoImages(rs, AttributesPointer ? AttributesPointer->GetAttributeClass("ShipInfoImages") : nullptr);
    if (m_pShipInfoImages)
    {
        m_pShipInfoImages->SetVisible(
            AttributesPointer ? (AttributesPointer->GetAttributeAsDword("ShifInfoVisible", 0) != 0) : false);
    }
}

uint32_t BATTLE_INTERFACE::AttributeChanged(ATTRIBUTES *pAttr)
{
    return 0;
}

uint64_t BATTLE_INTERFACE::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case BI_MSG_SET_ISLAND:
        m_bNeedIslandSet = true;
        g_IslandDescr.SetIsland(message.AttributePointer());
        m_bMyShipView = false;
        break;
    case BI_IN_CREATE_SHIP: // "laall"
    {
        const auto chIdx = message.Long();
        auto *const pChAttr = message.AttributePointer();
        auto *const pShipAttr = message.AttributePointer();
        const auto bMyShip = (message.Long() != 0L);
        const auto relation = message.Long();
        const uint32_t dwShipColor = message.GetCurrentFormatType() ? message.Long() : 0;
        g_ShipList.Add(AttributesPointer ? AttributesPointer->GetAttributeAsDword("MainChrIndex", -1) : -1, chIdx,
                       pChAttr, pShipAttr, bMyShip, relation, dwShipColor);
        if (m_pShipIcon)
            m_pShipIcon->SetUpdate();
    }
    break;
    case BI_IN_DELETE_SHIP: // "ll"
        g_ShipList.Release(message.Long());
        if (m_pShipIcon)
            m_pShipIcon->SetUpdate();
        break;
    case BI_IN_DEAD_SHIP: // "l"
        g_ShipList.ShipSink(message.Long());
        if (m_pShipIcon)
            m_pShipIcon->SetUpdate();
        break;
    case BI_IN_SET_COMMAND_MODE: // "lllll"
    {
        auto comMode = message.Long();
        auto startTextureNumber = message.Long();
        auto startPictureNumber = message.Long();
        auto characterNum = message.Long();
        auto commandState = message.Long();
    }
    break;
    case BI_SET_VISIBLE: // "ll"
        m_bVisible = (0 != message.Long());
        break;
    case BI_SET_ISLAND_LOCATORS: // "la"
        g_IslandDescr.SetIsland(message.AttributePointer());
        break;
    case BI_MSG_NOT_ENOUGH_BALLS_FLAG: // "l"
        BattleNavigator.SetEnoughBalls(message.Long() != 0);
        break;
    case BI_MSG_REFRESH:
        g_ShipList.Refresh();
        g_IslandDescr.Refresh();
        if (m_pShipIcon)
            m_pShipIcon->SetUpdate();
        break;

    case BI_MSG_SET_MSG_ICONS:
        /*if(m_pMessageIcons)
        {
          EnableMessageIcons(message.ScriptVariablePointer());
        }*/
        break;

    case BI_MSG_ADD_NEWTEXTURE: {
        const std::string &param = message.String();
        const int hQ = message.Long();
        const int vQ = message.Long();
        return m_pShipIcon->AddTexture(param.c_str(), hQ, vQ);
    }
    break;

    case MSG_BATTLE_LAND_MAKE_COMMAND: {
        const std::string &param = message.String();
        if (_stricmp(param.c_str(), "cancel") == 0)
        {
            if (m_pShipIcon)
                m_pShipIcon->ExecuteCommand(BIShipIcon::Command_cancel);
        }
    }
    break;
    }
    return 0;
}

void BATTLE_INTERFACE::CheckSeaState()
{
    auto *main_sd = g_ShipList.GetMainCharacterShip();
    if (main_sd == nullptr)
        return;

    long nReloadTargetIndex = -1;
    auto sqrRadius = core.Entity_GetAttributeAsFloat(BIUtils::idBattleInterface, "boardRadius", 0.f);
    sqrRadius *= sqrRadius;
    auto minReloadRadius = sqrRadius;
    auto sqrFreeDistance = core.Entity_GetAttributeAsFloat(BIUtils::idBattleInterface, "freeDistance", 500.f);
    sqrFreeDistance *= sqrFreeDistance;

    auto bSailTo = false, bLandTroops = false, bMap = true;
    auto bAttack = false, bDefend = false, bReload = false;

    for (auto *ps = g_ShipList.GetShipRoot(); ps != nullptr; ps = ps->next)
    {
        if (ps == main_sd)
            continue;
        bSailTo = true;
        if (ps->isMyShip)
        {
            bDefend = true;
            float curRad;
            if ((curRad = ~(ps->pShip->GetPos() - main_sd->pShip->GetPos())) < sqrRadius)
            {
                bReload = true;
                if (curRad < minReloadRadius)
                {
                    minReloadRadius = curRad;
                    nReloadTargetIndex = ps->characterIndex;
                }
            }
        }
        else
        {
            if (ps->relation == BI_RELATION_ENEMY)
            {
                if (!ps->isDead)
                {
                    bAttack = true;
                    float fdist;
                    if ((fdist = ~(ps->pShip->GetPos() - main_sd->pShip->GetPos())) < sqrFreeDistance)
                        bMap = false;
                }
            }
        }
    }

    if (g_IslandDescr.GetFirstLocator() != nullptr)
        bSailTo = true;

    if (g_IslandDescr.GetFirstEnemyFort() != nullptr)
        bAttack = true;

    core.Event(BI_EVENT_SET_SEA_STATE, "lllllll", bSailTo, bLandTroops, bMap, bAttack, bDefend, bReload,
               nReloadTargetIndex);
    m_bShowBattleBorder = !bMap;
}

void BATTLE_INTERFACE::EnableMessageIcons(VDATA *pvdat)
{
    /*if(!m_pMessageIcons) return;
    m_pMessageIcons->SetShowMsg(false);
    if(!pvdat) return;
    m_pMessageIcons->SetShowMsg(true);

    ATTRIBUTES * pAttr[4];
    long pLeft[4];

    int nCommandos = 0;

    for(int i=0; i<4; i++)
    {
      pAttr[i] = null;
      if(i<nCommandos)    pAttr[i] = pvdat->GetAClass(i);
    }

    m_pMessageIcons->StartData( pAttr, pLeft );*/
}
