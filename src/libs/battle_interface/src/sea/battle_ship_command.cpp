#include "battle_ship_command.h"

#include "core.h"

#include "island_descr.h"
#include "shared/battle_interface/msg_control.h"
#include "ships_list.h"

BIShipCommandList::BIShipCommandList(entid_t eid, ATTRIBUTES *pA, VDX9RENDER *rs) : BICommandList(eid, pA, rs)
{
    Init();
}

BIShipCommandList::~BIShipCommandList()
{
    Release();
}

void BIShipCommandList::FillIcons()
{
    int32_t nIconsQuantity = 0;

    if (m_nCurrentCommandMode & BI_COMMODE_MY_SHIP_SELECT)
        nIconsQuantity +=
            ShipAdding((m_nCurrentCommandMode & BI_COMMODE_ALLLOCATOR_SELECT) != 0, true, false, false, false);
    if (m_nCurrentCommandMode & BI_COMMODE_NEUTRAL_SHIP_SELECT)
        nIconsQuantity +=
            ShipAdding((m_nCurrentCommandMode & BI_COMMODE_ALLLOCATOR_SELECT) != 0, false, false, true, false);
    if (m_nCurrentCommandMode & BI_COMMODE_FRIEND_SHIP_SELECT)
        nIconsQuantity +=
            ShipAdding((m_nCurrentCommandMode & BI_COMMODE_ALLLOCATOR_SELECT) != 0, false, false, false, true);
    if (m_nCurrentCommandMode & BI_COMMODE_ENEMY_SHIP_SELECT)
        nIconsQuantity +=
            ShipAdding((m_nCurrentCommandMode & BI_COMMODE_ALLLOCATOR_SELECT) != 0, false, true, false, false);

    if (m_nCurrentCommandMode & BI_COMMODE_FRIEND_FORT_SELECT)
        nIconsQuantity += FortAdding((m_nCurrentCommandMode & BI_COMMODE_ALLLOCATOR_SELECT) != 0, true, false, false);
    if (m_nCurrentCommandMode & BI_COMMODE_NEUTRAL_FORT_SELECT)
        nIconsQuantity += FortAdding((m_nCurrentCommandMode & BI_COMMODE_ALLLOCATOR_SELECT) != 0, false, true, false);
    if (m_nCurrentCommandMode & BI_COMMODE_ENEMY_FORT_SELECT)
        nIconsQuantity += FortAdding((m_nCurrentCommandMode & BI_COMMODE_ALLLOCATOR_SELECT) != 0, false, false, true);
    if (m_nCurrentCommandMode & BI_COMMODE_LAND_SELECT)
        nIconsQuantity += LandAdding((m_nCurrentCommandMode & BI_COMMODE_ALLLOCATOR_SELECT) != 0);

    if (m_nCurrentCommandMode & BI_COMMODE_COMMAND_SELECT)
        nIconsQuantity += CommandAdding();

    if (m_nCurrentCommandMode & BI_COMMODE_CANNON_CHARGE)
        nIconsQuantity += ChargeAdding();

    if (m_nCurrentCommandMode & BI_COMMODE_USER_ICONS)
        nIconsQuantity += UserIconsAdding();

    if (m_nCurrentCommandMode & BI_COMMODE_ENEMY_TOWN)
        nIconsQuantity +=
            TownAdding((m_nCurrentCommandMode & BI_COMMODE_ALLLOCATOR_SELECT) != 0, true, true, true, false, false);

    if (m_nCurrentCommandMode & BI_COMMODE_DISEASED_TOWN)
        nIconsQuantity +=
            TownAdding((m_nCurrentCommandMode & BI_COMMODE_ALLLOCATOR_SELECT) != 0, true, false, true, true, true);

    if (m_nCurrentCommandMode & BI_COMMODE_NOTDISEASED_TOWN)
        nIconsQuantity +=
            TownAdding((m_nCurrentCommandMode & BI_COMMODE_ALLLOCATOR_SELECT) != 0, false, true, true, true, true);

    nIconsQuantity += AddCancelIcon();
}

void BIShipCommandList::Init()
{
    BICommandList::Init();

    m_nChargeTextureNum = -1;
    m_nCommandTextureNum = -1;
    m_nIconShowMaxQuantity = 8;

    auto *pA = m_pARoot;
    if (pA)
        pA = pA->GetAttributeClass("CommandTextures");
    if (pA)
    {
        m_nChargeTextureNum = pA->GetAttributeAsDword("ChargeTexNum", m_nChargeTextureNum);
        m_nCommandTextureNum = pA->GetAttributeAsDword("CommandTexNum", m_nCommandTextureNum);
    }

    pA = m_pARoot;
    if (pA)
        pA = pA->GetAttributeClass("CommandList");
    if (pA)
    {
        m_nIconShowMaxQuantity = pA->GetAttributeAsDword("CommandMaxIconQuantity", m_nIconShowMaxQuantity);
    }
}

void BIShipCommandList::Release()
{
}

int32_t BIShipCommandList::ShipAdding(bool allLabel, bool bMyShip, bool bEnemy, bool bNeutral, bool bFriend)
{
    int32_t n;
    int32_t retVal = 0;

    // list of ships
    auto *sd = g_ShipList.GetShipRoot();
    if (sd == nullptr)
        return 0;

    // distance for removing the ship from the list
    auto *pA = GetCurrentCommandAttribute();
    auto sqrRadius = -1.f;
    if (pA)
        sqrRadius = pA->GetAttributeAsFloat("EffectRadius", sqrRadius);
    if (sqrRadius < 0.f)
        allLabel = true;
    sqrRadius *= sqrRadius;

    auto *selShip = g_ShipList.FindShip(m_nCurrentCommandCharacterIndex);
    float selX, selZ;
    if (selShip == nullptr)
    {
        allLabel = true;
        selX = 0;
        selZ = 0;
    }
    else
    {
        selX = selShip->pShip->GetPos().x;
        selZ = selShip->pShip->GetPos().z;
    }

    auto *main_sd = g_ShipList.GetMainCharacterShip();
    if (bMyShip && main_sd != selShip)
    {
        n = AddToIconList(main_sd->textureNum, main_sd->pictureNum, main_sd->selectPictureNum, -1,
                          main_sd->characterIndex, nullptr, -1, nullptr, main_sd->pAttr->GetAttribute("name"));
        if (n > 0)
        {
            retVal += n;
            AddFlagPictureToIcon(main_sd->characterIndex);
        }
    }

    for (; sd != nullptr; sd = sd->next)
    {
        if (sd != selShip && sd != main_sd)
        {
            if (bMyShip && sd->isMyShip || !bMyShip && bEnemy && !sd->isMyShip && sd->relation == BI_RELATION_ENEMY ||
                !bMyShip && bFriend && !sd->isMyShip && sd->relation == BI_RELATION_FRIEND ||
                !bMyShip && bNeutral && !sd->isMyShip && sd->relation == BI_RELATION_NEUTRAL)
            {
                // fits the distance?
                if (!allLabel)
                {
                    const auto cv = sd->pShip->GetPos();
                    if (SQR(selX - cv.x) + SQR(selZ - cv.z) > sqrRadius)
                        continue;
                }
                // check for validity of the ship from the script
                if (!m_sCurrentCommandName.empty())
                {
                    auto *pvdat =
                        core.Event("evntCheckEnableShip", "sl", m_sCurrentCommandName.c_str(), sd->characterIndex);
                    if (pvdat != nullptr && pvdat->GetInt() == 0)
                        continue;
                }
                n = AddToIconList(sd->textureNum, sd->pictureNum, sd->selectPictureNum, -1, sd->characterIndex, nullptr,
                                  -1, nullptr, sd->pAttr->GetAttribute("name"));
                if (n > 0)
                {
                    retVal += n;
                    AddFlagPictureToIcon(sd->characterIndex);
                }
            }
        }
    }

    return retVal;
}

int32_t BIShipCommandList::FortAdding(bool allLabel, bool bFriend, bool bNeutral, bool bEnemy)
{
    auto *pL = g_IslandDescr.GetFirstFort();
    if (pL == nullptr)
        return 0;
    int32_t retVal = 0;

    auto *pA = GetCurrentCommandAttribute();
    auto sqrRadius = pL->r;
    if (pA)
        sqrRadius = pA->GetAttributeAsFloat("EffectRadius", sqrRadius);
    if (sqrRadius < 0.f)
        allLabel = true;
    sqrRadius *= sqrRadius;

    // Determine the coordinates of the ship receiving the commands
    const auto selectedCharacter = m_nCurrentCommandCharacterIndex;
    auto *selShip = g_ShipList.FindShip(selectedCharacter);
    float selX, selZ;
    if (selShip == nullptr)
    {
        allLabel = true;
        selX = 0;
        selZ = 0;
    }
    else
    {
        selX = selShip->pShip->GetPos().x;
        selZ = selShip->pShip->GetPos().z;
    }

    do
    {
        if (bFriend && pL->relation == BI_RELATION_FRIEND || bNeutral && pL->relation == BI_RELATION_NEUTRAL ||
            bEnemy && pL->relation == BI_RELATION_ENEMY)
        {
            if (!allLabel)
                if (SQR(pL->x - selX) + SQR(pL->z - selZ) > sqrRadius)
                    continue;
            auto *pvdat = core.Event("evntCheckEnableLocator", "sa", m_sCurrentCommandName.c_str(), pL->pA);
            if (pvdat != nullptr && pvdat->GetInt() == 0)
                continue;
            char *pLocName = nullptr;
            if (pL->pA != nullptr)
                pLocName = pL->pA->GetAttribute("name");
            retVal += AddToIconList(pL->texIdx, pL->picIdx, pL->selPicIdx, -1, pL->characterIndex, nullptr, -1,
                                    pL->pA->GetAttribute("name"), pL->pchr_note);
        }
    } while ((pL = g_IslandDescr.GetNext()) != nullptr);

    return retVal;

    return 0;
}

int32_t BIShipCommandList::LandAdding(bool allLabel)
{
    auto *pL = g_IslandDescr.GetFirstLand();
    if (pL == nullptr)
        return 0;
    int32_t retVal = 0;

    auto *pA = GetCurrentCommandAttribute();
    auto sqrRadius = pL->r;
    if (pA)
        sqrRadius = pA->GetAttributeAsFloat("EffectRadius", sqrRadius);
    if (sqrRadius < 0.f)
        allLabel = true;
    sqrRadius *= sqrRadius;

    // Determine the coordinates of the ship receiving the commands
    const auto selectedCharacter = m_nCurrentCommandCharacterIndex;
    auto *selShip = g_ShipList.FindShip(selectedCharacter);
    float selX, selZ;
    if (selShip == nullptr)
    {
        allLabel = true;
        selX = 0;
        selZ = 0;
    }
    else
    {
        selX = selShip->pShip->GetPos().x;
        selZ = selShip->pShip->GetPos().z;
    }

    do
    {
        if (!allLabel)
            if (SQR(pL->x - selX) + SQR(pL->z - selZ) > sqrRadius)
                continue;
        auto *pvdat = core.Event("evntCheckEnableLocator", "sa", m_sCurrentCommandName.c_str(), pL->pA);
        if (pvdat != nullptr && pvdat->GetInt() == 0)
            continue;
        char *pLocName = nullptr;
        if (pL->pA != nullptr)
            pLocName = pL->pA->GetAttribute("name");
        retVal += AddToIconList(pL->texIdx, pL->picIdx, pL->selPicIdx, -1, pL->characterIndex, nullptr, -1, pLocName,
                                pL->pchr_note);
    } while ((pL = g_IslandDescr.GetNext()) != nullptr);
    return retVal;
}

int32_t BIShipCommandList::CommandAdding()
{
    core.Event("BI_SetPossibleCommands", "l", m_nCurrentCommandCharacterIndex);
    int32_t retVal = 0;
    auto *pAttr = m_pARoot->GetAttributeClass("Commands");
    if (!pAttr)
        return 0;
    const size_t attrQuant = pAttr->GetAttributesNum();

    for (int32_t i = 0; i < attrQuant; i++)
    {
        auto *pA = pAttr->GetAttributeClass(i);
        if (pA == nullptr)
            continue; // no such attribute
        if (pA->GetAttributeAsDword("enable", 0) == 0)
            continue; // command not available
        const int32_t pictureNum = pA->GetAttributeAsDword("picNum", 0);
        const int32_t selPictureNum = pA->GetAttributeAsDword("selPicNum", 0);
        const int32_t cooldownPictureNum = pA->GetAttributeAsDword("cooldownPicNum", -1);
        const int32_t texNum = pA->GetAttributeAsDword("texNum", m_nCommandTextureNum);
        auto *const eventName = pA->GetAttribute("event");
        retVal += AddToIconList(texNum, pictureNum, selPictureNum, cooldownPictureNum, -1, eventName, -1, nullptr,
                                pA->GetAttribute("note"));
    }

    return retVal;
}

int32_t BIShipCommandList::ChargeAdding()
{
    // Determine the amount of each charge on board
    auto *tmpDat = core.Event("BI_GetChargeQuantity", "l", m_nCurrentCommandCharacterIndex);
    if (tmpDat == nullptr)
        return 0;
    int32_t lIdx = 0; // number of charge types
    tmpDat->Get(lIdx, 0);
    if (lIdx <= 0)
        return 0;

    auto *pAList = m_pARoot->GetAttributeClass("charge");
    int32_t retVal = 0;
    m_aChargeQuantity.clear();
    for (auto i = 0; i < lIdx; i++)
    {
        m_aChargeQuantity.push_back(0);
        tmpDat->Get(m_aChargeQuantity[i], i + 1);

        char param[128];
        sprintf_s(param, sizeof(param), "charge%d", i + 1);
        auto *pA = pAList ? pAList->GetAttributeClass(param) : nullptr;
        const int32_t nNormalPicIndex = pA ? pA->GetAttributeAsDword("picNum", -1) : -1;
        const int32_t nSelectPicIndex = pA ? pA->GetAttributeAsDword("selPicNum", -1) : -1;
        retVal += AddToIconList(m_nChargeTextureNum, nNormalPicIndex, nSelectPicIndex, -1, -1, nullptr, i + 1, nullptr,
                                nullptr);
    }
    return retVal;
}

int32_t BIShipCommandList::UserIconsAdding()
{
    int32_t retVal = 0;
    auto *pAttr = m_pARoot->GetAttributeClass("UserIcons");
    if (!pAttr)
        return 0;
    const size_t attrQuant = pAttr->GetAttributesNum();

    for (int32_t i = 0; i < attrQuant; i++)
    {
        auto *pA = pAttr->GetAttributeClass(i);
        if (pA == nullptr)
            continue; // no such attribute
        if (pA->GetAttributeAsDword("enable", 0) == 0)
            continue; // command not available
        const int32_t pictureNum = pA->GetAttributeAsDword("pic", 0);
        const int32_t selPictureNum = pA->GetAttributeAsDword("selpic", 0);
        const int32_t textureNum = pA->GetAttributeAsDword("tex", -1);
        retVal += AddToIconList(textureNum, pictureNum, selPictureNum, -1, -1, nullptr, i + 1, pA->GetAttribute("name"),
                                pA->GetAttribute("note"));
    }

    return retVal;
}

int32_t BIShipCommandList::AbilityAdding()
{
    core.Event("evntSetUsingAbility", "l", m_nCurrentCommandCharacterIndex);
    int32_t retVal = 0;
    auto *pAttr = m_pARoot->GetAttributeClass("AbilityIcons");
    if (!pAttr)
        return 0;
    const size_t attrQuant = pAttr->GetAttributesNum();

    for (int32_t i = 0; i < attrQuant; i++)
    {
        auto *pA = pAttr->GetAttributeClass(i);
        if (pA == nullptr)
            continue; // no such attribute
        if (pA->GetAttributeAsDword("enable", 0) == 0)
            continue; // command not available
        const int32_t pictureNum = pA->GetAttributeAsDword("picNum", 0);
        const int32_t selPictureNum = pA->GetAttributeAsDword("selPicNum", 0);
        const int32_t textureNum = pA->GetAttributeAsDword("texNum", -1);
        // retVal += AddToIconList( textureNum, pictureNum, selPictureNum, -1, -1, pA->GetThisName(), i, null,
        // pA->GetAttribute("note") );
        const int32_t cooldownPictureNum = pA->GetAttributeAsDword("cooldownPicNum", -1);
        auto *const eventName = pA->GetAttribute("event");
        retVal += AddToIconList(textureNum, pictureNum, selPictureNum, cooldownPictureNum, -1, eventName, -1, nullptr,
                                pA->GetAttribute("note"));
    }

    return retVal;
}

int32_t BIShipCommandList::AddCancelIcon()
{
    auto *pA = m_pARoot->GetAttributeClass("Commands");
    if (pA)
        pA = pA->GetAttributeClass("Cancel");
    if (!pA)
        return 0;
    const int32_t pictureNum = pA->GetAttributeAsDword("picNum", 0);
    const int32_t selPictureNum = pA->GetAttributeAsDword("selPicNum", 0);
    const int32_t textureNum = pA->GetAttributeAsDword("texNum", -1);
    return AddToIconList(textureNum, pictureNum, selPictureNum, -1, -1, pA->GetAttribute("event"), -1, nullptr,
                         pA->GetAttribute("note"));
}

int32_t BIShipCommandList::TownAdding(bool allLabel, bool bDiseased, bool bNotDiseased, bool bEnemy, bool bNeutral,
                                   bool bFriend)
{
    auto *pL = g_IslandDescr.GetFirstLocator();
    if (pL == nullptr)
        return 0;
    int32_t retVal = 0;

    // determine the radius of the command (everything that is not included in it is not shown)
    auto *pA = GetCurrentCommandAttribute();
    auto sqrRadius = pL->r;
    if (pA)
        sqrRadius = pA->GetAttributeAsFloat("EffectRadius", sqrRadius);
    if (sqrRadius < 0.f)
        allLabel = true;
    sqrRadius *= sqrRadius;

    // Determine the coordinates of the ship receiving the commands
    const auto selectedCharacter = m_nCurrentCommandCharacterIndex;
    auto *selShip = g_ShipList.FindShip(selectedCharacter);
    float selX, selZ;
    if (selShip == nullptr)
    {
        allLabel = true;
        selX = 0;
        selZ = 0;
    }
    else
    {
        selX = selShip->pShip->GetPos().x;
        selZ = selShip->pShip->GetPos().z;
    }

    do
    {
        if (pL->locatorType != ISLAND_LOCATOR_TOWN)
            continue; // check by type - must be city
        // check against (enemy, neutral, friend)
        if (pL->relation == BI_RELATION_ENEMY && !bEnemy)
            continue;
        if (pL->relation == BI_RELATION_NEUTRAL && !bNeutral)
            continue;
        if (pL->relation == BI_RELATION_FRIEND && !bFriend)
            continue;
        if (pL->bDiseased && !bDiseased)
            continue;
        if (!pL->bDiseased && !bNotDiseased)
            continue;
        if (!allLabel)
            if (SQR(pL->x - selX) + SQR(pL->z - selZ) > sqrRadius)
                continue;
        auto *pvdat = core.Event("evntCheckEnableLocator", "sa", m_sCurrentCommandName.c_str(), pL->pA);
        if (pvdat != nullptr && pvdat->GetInt() == 0)
            continue;
        char *pLocName = nullptr;
        if (pL->pA != nullptr)
            pLocName = pL->pA->GetAttribute("name");
        retVal += AddToIconList(pL->texIdx, pL->picIdx, pL->selPicIdx, -1, pL->characterIndex, nullptr, -1, pLocName,
                                pL->pchr_note);
    } while ((pL = g_IslandDescr.GetNext()) != nullptr);
    return retVal;
}

void BIShipCommandList::AddFlagPictureToIcon(int32_t nCharIdx)
{
    auto *pvdat = core.Event("evntGetSmallFlagData", "l", nCharIdx);
    if (!pvdat)
        return;
    int32_t nTex, nPic, nBackPic;
    pvdat->Get(nTex, 0);
    pvdat->Get(nPic, 1);
    pvdat->Get(nBackPic, 2);
    AddAdditiveToIconList(nTex, nBackPic, 2.f, 32.f, 32.f);
    AddAdditiveToIconList(nTex, nPic, -32.f, 32.f, 32.f);
}
