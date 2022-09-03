#include "ship_command.h"
#include "core.h"
#include "shared/battle_interface/msg_control.h"
#include "vma.hpp"

WMShipCommandList::WMShipCommandList(entid_t eid, ATTRIBUTES *pA, VDX9RENDER *rs) : BICommandList(eid, pA, rs)
{
    Init();
}

WMShipCommandList::~WMShipCommandList()
{
    Release();
}

void WMShipCommandList::FillIcons()
{
    int32_t nIconsQuantity = 0;

    if (m_nCurrentCommandMode & BI_COMMODE_COMMAND_SELECT)
        nIconsQuantity += CommandAdding();
}

void WMShipCommandList::Init()
{
    BICommandList::Init();
}

void WMShipCommandList::Release()
{
}

int32_t WMShipCommandList::CommandAdding()
{
    core.Event("WM_SetPossibleCommands", "l", m_nCurrentCommandCharacterIndex);
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
