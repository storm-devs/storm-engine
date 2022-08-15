#include "battle_man_command.h"
#include "core.h"
#include "shared/battle_interface/msg_control.h"
#include "vma.hpp"

BIManCommandList::BIManCommandList(entid_t eid, ATTRIBUTES *pA, VDX9RENDER *rs) : BICommandList(eid, pA, rs)
{
    Init();
}

BIManCommandList::~BIManCommandList()
{
    Release();
}

void BIManCommandList::FillIcons()
{
    int32_t nIconsQuantity = 0;

    if (m_nCurrentCommandMode & BI_COMMODE_COMMAND_SELECT)
        nIconsQuantity += CommandAdding();

    if (m_nCurrentCommandMode & BI_COMMODE_USER_ICONS)
        nIconsQuantity += UserIconsAdding();

    if (m_nCurrentCommandMode & BI_COMMODE_ABILITY_ICONS)
        nIconsQuantity += AbilityAdding();

    nIconsQuantity += AddCancelIcon();

    /*if( nIconsQuantity == 0 )
      AddCancelIcon();*/
}

void BIManCommandList::Init()
{
    BICommandList::Init();

    m_nIconShowMaxQuantity = 5;
    // boal -->
    auto *pA = m_pARoot;
    if (pA)
        pA = pA->GetAttributeClass("CommandList");
    if (pA)
    {
        m_nIconShowMaxQuantity = pA->GetAttributeAsDword("CommandMaxIconQuantity", m_nIconShowMaxQuantity);
    }
    // boal <--
}

void BIManCommandList::Release()
{
}

int32_t BIManCommandList::CommandAdding()
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
        const int32_t texNum = pA->GetAttributeAsDword("texNum", -1);
        const char *eventName = pA->GetAttribute("event");
        retVal +=
            AddToIconList(texNum, pictureNum, selPictureNum, -1, -1, eventName, -1, nullptr, pA->GetAttribute("note"));
    }

    return retVal;
}

int32_t BIManCommandList::UserIconsAdding()
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
        const char *eventName = pA->GetAttribute("event");
        retVal += AddToIconList(textureNum, pictureNum, selPictureNum, -1, -1, eventName, i + 1,
                                pA->GetAttribute("name"), pA->GetAttribute("note"));
    }

    return retVal;
}

int32_t BIManCommandList::AbilityAdding()
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
        const char *eventName = pA->GetAttribute("event");
        retVal += AddToIconList(textureNum, pictureNum, selPictureNum, -1, -1, eventName, -1, nullptr,
                                pA->GetAttribute("note"));
    }

    return retVal;
}

int32_t BIManCommandList::AddCancelIcon()
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
