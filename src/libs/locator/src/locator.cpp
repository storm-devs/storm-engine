#include "locator.h"
#include "core.h"
#include "entity.h"
#include "shared/messages.h"

CREATE_CLASS(LOCATOR)

CREATE_CLASS(BLAST)

LOCATOR::LOCATOR()
    : rs(nullptr), ParticlesID(0)
{
    gs = nullptr;
    geo = nullptr;
    groupID = -1;
    stringIndex = -1;
}

LOCATOR::~LOCATOR()
{
    delete geo;
    geo = nullptr;
}

bool LOCATOR::Init()
{
    rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    gs = static_cast<VGEOMETRY *>(core.GetService("geometry"));
    if (!gs)
        return false;

    return true;
}

bool LOCATOR::VerifyParticles()
{
    ParticlesID = EntityManager::GetEntityId("particles");
    if (!ParticlesID)
        ParticlesID = EntityManager::CreateEntity("particles");

    return static_cast<bool>(ParticlesID);
}

void LOCATOR::LocateForI_L2(ATTRIBUTES *pA, GEOS *g, GEOS::LABEL &label)
{
    char name[16];
    GEOS::LABEL label2;

    const auto groupID = g->FindName(label.name);
    if (groupID < 0)
    {
        core.Trace("?void LOCATOR::LocateForI_L2(...)");
        return;
    }

    pA = pA->CreateSubAClass(pA, "ships");

    int32_t n = 0;
    for (int32_t stringIndex = 0; (stringIndex = g->FindLabelG(stringIndex, groupID)) >= 0; stringIndex++)
    {
        g->GetLabel(stringIndex, label2);
        sprintf_s(name, "l%d", n);
        auto *pAA = pA->CreateSubAClass(pA, name);
        pAA->SetAttributeUseFloat("x", label2.m[3][0]);
        pAA->SetAttributeUseFloat("y", label2.m[3][1]);
        pAA->SetAttributeUseFloat("z", label2.m[3][2]);
        pAA->SetAttributeUseFloat("ay", atan2f(label2.m[2][0], label2.m[2][2]));
        n++;
    }
}

void LOCATOR::LocateForI(VDATA *pData)
{
    ATTRIBUTES *pA;
    ATTRIBUTES *pAA;
    GEOS *g;
    GEOS::LABEL label;
    int32_t i, n;

    if (pData == nullptr)
    {
        core.Trace("?void LOCATOR::LocateForI(VDATA * pData)");
        return;
    }
    pA = pData->GetAClass();
    if (pA == nullptr)
    {
        core.Trace("?void LOCATOR::LocateForI(VDATA * pData)");
        return;
    }
    if (!pA->GetAttribute("locators"))
    {
        core.Trace("?void LOCATOR::LocateForI(VDATA * pData)");
        return;
    }
    char sFileLocators[256];
    auto *const pAFilesPath = pA->FindAClass(pA, "filespath.models");
    sprintf_s(sFileLocators, "%s\\%s", (pAFilesPath) ? pAFilesPath->GetThisAttr() : "", pA->GetAttribute("locators"));
    rs->SetLoadTextureEnable(false);
    g = gs->CreateGeometry(sFileLocators, "", 0);
    rs->SetLoadTextureEnable(true);
    if (!g)
    {
        core.Trace("?void LOCATOR::LocateForI(VDATA * pData)");
        return;
    }

    auto groupID = g->FindName("reload");
    if (groupID >= 0)
    {
        for (int32_t i = 0; (i = g->FindLabelG(i, groupID)) >= 0; i++)
        {
            g->GetLabel(i, label);
            pAA = pA->FindAClass(pA, "reload");
            if (pAA)
                for (n = 0; n < static_cast<int32_t>(pAA->GetAttributesNum()); n++)
                {
                    if (pAA->GetAttributeClass(n))
                    {
                        if (!pAA->GetAttributeClass(n)->GetAttribute("name"))
                        {
                            core.Trace("LOCATOR: no name");
                            continue;
                        }
                        if (storm::iEquals(pAA->GetAttributeClass(n)->GetAttribute("name"), label.name))
                        {
                            pAA->GetAttributeClass(n)->SetAttributeUseFloat("x", label.m[3][0]);
                            pAA->GetAttributeClass(n)->SetAttributeUseFloat("y", label.m[3][1]);
                            pAA->GetAttributeClass(n)->SetAttributeUseFloat("z", label.m[3][2]);
                            pAA->GetAttributeClass(n)->SetAttributeUseFloat("ay", atan2f(label.m[2][0], label.m[2][2]));
                            LocateForI_L2(pAA->GetAttributeClass(n), g, label);
                        }
                    }
                }
        }
    }

    // check for unfind reloads
    pAA = pA->FindAClass(pA, "reload");
    if (pAA)
        for (n = 0; n < static_cast<int32_t>(pAA->GetAttributesNum()); n++)
        {
            auto *pARC = pAA->GetAttributeClass(n);
            if (!pARC->FindAClass(pARC, "x"))
            {
                core.Trace("LOCATOR: Can't find locator with name: %s, geo: %s", pARC->GetAttribute("name"),
                           pA->GetAttribute("locators"));
            }
        }

    groupID = g->FindName("quest_ships");
    if (groupID >= 0)
    {
        pAA = pA->FindAClass(pA, "Quest_ships");
        if (!pAA)
            pAA = pA->CreateAttribute("Quest_ships", "");
        if (pAA)
            LocateForI_Locators(pAA, g, groupID, _XYZ_ | _AY_);
    }

    groupID = g->FindName("net_deathmatch");
    if (groupID >= 0)
    {
        pAA = pA->CreateAttribute("net_deathmatch", "");
        if (pAA)
            LocateForI_Locators(pAA, g, groupID, _XYZ_ | _AY_);
    }

    groupID = g->FindName("net_team");
    if (groupID >= 0)
    {
        pAA = pA->CreateAttribute("net_team", "");
        if (pAA)
            LocateForI_Locators(pAA, g, groupID, _XYZ_ | _AY_);
    }

    groupID = g->FindName("net_convoy");
    if (groupID >= 0)
    {
        pAA = pA->CreateAttribute("net_convoy", "");
        if (pAA)
            LocateForI_Locators(pAA, g, groupID, _XYZ_ | _AY_);
    }

    groupID = g->FindName("net_fort");
    if (groupID >= 0)
    {
        pAA = pA->CreateAttribute("net_fort", "");
        if (pAA)
            LocateForI_Locators(pAA, g, groupID, _XYZ_ | _AY_);
    }

    groupID = g->FindName("fire");
    if (groupID >= 0)
    {
        pAA = pA->FindAClass(pA, "fire");
        if (!pAA)
            pAA = pA->CreateAttribute("fire", "");
        if (pAA)
            LocateForI_Locators(pAA, g, groupID, _XYZ_);
    }

    auto *pGA = pA->FindAClass(pA, "LoadGroup");
    if (pGA)
        for (n = 0; n < static_cast<int32_t>(pGA->GetAttributesNum()); n++)
        {
            auto *const pARC = pGA->GetAttributeClass(n);
            const char *pLoadGroupName = pARC->GetThisAttr();
            if (!pLoadGroupName)
                continue;

            groupID = g->FindName(pLoadGroupName);
            if (groupID < 0)
                continue;

            pAA = pA->FindAClass(pA, pLoadGroupName);
            if (!pAA)
                pAA = pA->CreateAttribute(pLoadGroupName, "");
            if (pAA)
                LocateForI_Locators(pAA, g, groupID, _XYZ_ | _AY_);
        }

    delete g;
}

void LOCATOR::LocateForI_Locators(ATTRIBUTES *pA, GEOS *geo, int32_t iGroupID, uint32_t dwFlags)
{
    int32_t i;
    GEOS::LABEL label;
    ATTRIBUTES *pAA;

    for (i = 0; (i = geo->FindLabelG(i, iGroupID)) >= 0; i++)
    {
        geo->GetLabel(i, label);
        pAA = pA->CreateSubAClass(pA, label.name);
        if (dwFlags & _X_)
            pAA->SetAttributeUseFloat("x", label.m[3][0]);
        if (dwFlags & _Y_)
            pAA->SetAttributeUseFloat("y", label.m[3][1]);
        if (dwFlags & _Z_)
            pAA->SetAttributeUseFloat("z", label.m[3][2]);
        if (dwFlags & _AY_)
            pAA->SetAttributeUseFloat("ay", atan2f(label.m[2][0], label.m[2][2]));
    }
}

uint64_t LOCATOR::ProcessMessage(MESSAGE &message)
{
    int32_t message_code;
    GEOS::LABEL label;
    ATTRIBUTES *pA;

    message_code = message.Long();
    switch (message_code)
    {
    case LM_LOCATE_I:
        LocateForI(message.ScriptVariablePointer());
        break;
    case LM_LOCATE_FIRST: {
        const std::string &buffer = message.String();
        pA = message.AttributePointer();
        groupID = geo->FindName(buffer.c_str());
        if (groupID >= 0)
        {
            VerifyParticles();
            stringIndex = geo->FindLabelG(0, groupID);
            if (stringIndex < 0)
                break;
            geo->GetLabel(stringIndex, label);

            if (pA)
            {
                pA->SetAttributeUseFloat("x", label.m[3][0]);
                pA->SetAttributeUseFloat("y", label.m[3][1]);
                pA->SetAttributeUseFloat("z", label.m[3][2]);
                pA->SetAttributeUseFloat("ay", atan2f(label.m[2][0], label.m[2][2]));
                pA->SetAttributeUseFloat("vx", label.m[2][0]);
                pA->SetAttributeUseFloat("vy", label.m[2][1]);
                pA->SetAttributeUseFloat("vz", label.m[2][2]);
            }
            stringIndex++;
            return 1;
        }
        return 0;
    }
    case LM_LOCATE_NEXT:
        pA = message.AttributePointer();
        if (groupID >= 0)
        {
            VerifyParticles();
            stringIndex = geo->FindLabelG(stringIndex, groupID);
            if (stringIndex < 0)
                return 0;
            geo->GetLabel(stringIndex, label);
            if (pA)
            {
                pA->SetAttributeUseFloat("x", label.m[3][0]);
                pA->SetAttributeUseFloat("y", label.m[3][1]);
                pA->SetAttributeUseFloat("z", label.m[3][2]);
                pA->SetAttributeUseFloat("ay", atan2f(label.m[2][0], label.m[2][2]));
                pA->SetAttributeUseFloat("vx", label.m[2][0]);
                pA->SetAttributeUseFloat("vy", label.m[2][1]);
                pA->SetAttributeUseFloat("vz", label.m[2][2]);
            }
            stringIndex++;
            return 1;
        }
        return 0;

    case LM_SET_GEOMETRY: {
        const std::string &name = message.String();
        delete geo;
        geo = nullptr;
        rs->SetLoadTextureEnable(false);
        geo = gs->CreateGeometry(name.c_str(), "", 0);
        rs->SetLoadTextureEnable(true);
        break;
        /*case LM_LOCATE:
          groupID = geo->FindName("smoke");
          if(groupID >= 0)
          {
            VerifyParticles();
            for(stringIndex = 0; (stringIndex = geo->FindLabelG(stringIndex, groupID)) >= 0; stringIndex++)
            {
              geo->GetLabel(stringIndex, label);
              core.Send_Message(ParticlesID,"lsffffffl",
                PS_CREATE,"smoke",label.m[3][0],label.m[3][1],label.m[3][2],-1.57,0,0,0);
            }
          }

          groupID = geo->FindName("fire");
          if(groupID >= 0)
          {
            if(!core.FindClass(&ParticlesID,"particles",0))
            {
              if(!EntityManager::CreateEntity(&ParticlesID,"particles")) return 0;
            }
            for(stringIndex = 0; (stringIndex = geo->FindLabelG(stringIndex, groupID)) >= 0; stringIndex++)
            {
              geo->GetLabel(stringIndex, label);
              core.Send_Message(ParticlesID,"lsffffffl",
                PS_CREATE,"fire",label.m[3][0],label.m[3][1],label.m[3][2],-1.57,0,0,0);
            }
          }

          groupID = geo->FindName("water");
          if(groupID >= 0)
          {
            if(!core.FindClass(&ParticlesID,"particles",0))
            {
              if(!EntityManager::CreateEntity(&ParticlesID,"particles")) return 0;
            }
            for(stringIndex = 0; (stringIndex = geo->FindLabelG(stringIndex, groupID)) >= 0; stringIndex++)
            {

              geo->GetLabel(stringIndex, label);
              core.Send_Message(ParticlesID,"lsffffffl",
                PS_CREATEX,"waterfall",label.m[3][0],label.m[3][1],label.m[3][2],label.m[2][0],label.m[2][1],-label.m[2][2],0);
            }
          }
        break;*/
    }
    }
    return 0;
}

uint32_t LOCATOR::AttributeChanged(ATTRIBUTES *pA)
{
    return 0;
}
