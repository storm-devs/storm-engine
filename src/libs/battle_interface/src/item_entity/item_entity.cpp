#include "item_entity.h"
#include "../utils.h"
#include "entity.h"
#include "core.h"
#include "message.h"
#include "model.h"
#include "shared/messages.h"

ItemEntity::ItemEntity()
{
    SetBeginData();
}

ItemEntity::~ItemEntity()
{
    Release();
}

bool ItemEntity::Init()
{
    if (!ReadAndCreate())
        return false;
    return true;
}

void ItemEntity::Realize(uint32_t delta_time)
{
    if (m_bVisible && m_pModel)
        if (m_bTieToLocator)
            DrawIntoLocator();
        else
            m_pModel->ProcessStage(Stage::realize, delta_time);
}

uint64_t ItemEntity::ProcessMessage(MESSAGE &message)
{
    const auto nMsgCod = message.Long();
    switch (nMsgCod)
    {
    case 0: // Tie item to moving locator
    {
        DeleteParticle();
        const auto mdlEID = GetModelEIDFromCharacterEID(message.EntityID());
        const std::string &locName = message.String();
        m_bTieToLocator = TieToLocator(mdlEID, locName.c_str());
    }
    break;

    case 1: // Untie item from moving locator and return to his place
        UnTieFromLocator();
        CreateParticle();
        break;

    case 2: // Turn off item show
        if (m_bVisible)
        {
            UnTieFromLocator();
            m_bVisible = false;
            DeleteParticle();
        }
        break;

    case 3: // turn on item show
        m_bVisible = true;
        CreateParticle();
        break;

    case 4: // replace item to other position
        m_mtxpos.Pos().x = message.Float();
        m_mtxpos.Pos().y = message.Float();
        m_mtxpos.Pos().z = message.Float();
        m_mtxpos.Vx().x = message.Float();
        m_mtxpos.Vx().y = message.Float();
        m_mtxpos.Vx().z = message.Float();
        m_mtxpos.Vy().x = message.Float();
        m_mtxpos.Vy().y = message.Float();
        m_mtxpos.Vy().z = message.Float();
        m_mtxpos.Vz().x = message.Float();
        m_mtxpos.Vz().y = message.Float();
        m_mtxpos.Vz().z = message.Float();
        SetModelToPosition(m_mtxpos);
        DeleteParticle();
        CreateParticle();
        break;

    case 5: {
        DeleteParticle();
        const auto mdlID = GetModelEIDFromCharacterEID(message.EntityID());
        const auto mdlToTieID = GetModelEIDFromCharacterEID(message.EntityID());
        const std::string &sLocName = message.String();
        const std::string &sStartEvntName = message.String();
        const std::string &sEndEvntName = message.String();
        SetEventListener(mdlID, mdlToTieID, sLocName.c_str(), sStartEvntName.c_str(), sEndEvntName.c_str());
    }
    break;
    }
    return -1;
}

bool ItemEntity::ReadAndCreate()
{
    BIUtils::ReadVectorFormAttr(AttributesPointer, "pos", m_mtxpos.Pos(), CVECTOR(0.f));
    BIUtils::ReadVectorFormAttr(AttributesPointer, "pos.vx", m_mtxpos.Vx(), CVECTOR(0.f));
    BIUtils::ReadVectorFormAttr(AttributesPointer, "pos.vy", m_mtxpos.Vy(), CVECTOR(0.f));
    BIUtils::ReadVectorFormAttr(AttributesPointer, "pos.vz", m_mtxpos.Vz(), CVECTOR(0.f));
    auto *const pcModelName = BIUtils::GetStringFromAttr(AttributesPointer, "model", "");
    auto *const pcTechnique = BIUtils::GetStringFromAttr(AttributesPointer, "technique", "");
    if (pcModelName)
    {
        if (m_eidModel = EntityManager::CreateEntity("modelr"))
        {
            core.Send_Message(m_eidModel, "ls", MSG_MODEL_LOAD_GEO, pcModelName);
            m_pModel = static_cast<MODEL *>(EntityManager::GetEntityPointer(m_eidModel));
            SetModelToPosition(m_mtxpos);
            SetTechnique(pcTechnique);
        }
    }
    m_bVisible = m_pModel != nullptr;
    if (m_bVisible)
        CreateParticle();
    return true;
}

void ItemEntity::SetBeginData()
{
    m_bVisible = false;
    m_pModel = nullptr;
    m_bTieToLocator = false;
    m_pParticle = nullptr;
}

void ItemEntity::Release()
{
    if (m_pModel)
    {
        EntityManager::EraseEntity(m_eidModel);
        m_pModel = nullptr;
    }
    DeleteParticle();
}

void ItemEntity::SetModelToPosition(const CMatrix &mtx) const
{
    if (m_pModel)
    {
        core.Send_Message(m_eidModel, "lffffffffffff", MSG_MODEL_SET_POSITION, m_mtxpos.Pos().x, m_mtxpos.Pos().y,
                          m_mtxpos.Pos().z, m_mtxpos.Vx().x, m_mtxpos.Vx().y, m_mtxpos.Vx().z, m_mtxpos.Vy().x,
                          m_mtxpos.Vy().y, m_mtxpos.Vy().z, m_mtxpos.Vz().x, m_mtxpos.Vz().y, m_mtxpos.Vz().z);
    }
}

void ItemEntity::SetTechnique(const char *pcTechnique) const
{
    if (m_pModel)
    {
        auto *pRootNod = m_pModel->GetNode(0);
        if (pRootNod)
        {
            pRootNod->SetTechnique(pcTechnique);
        }
    }
}

bool ItemEntity::TieToLocator(entid_t mdlEID, const char *pcLocName)
{
    m_eidTieModel = mdlEID;
    m_sTieLocName = pcLocName;
    auto *pMdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(mdlEID));
    if (pMdl)
    {
        m_pMdlNode = pMdl->GetNode(0);
        if (m_pMdlNode)
            return true;
    }
    return false;
}

void ItemEntity::UnTieFromLocator()
{
    if (m_bTieToLocator)
    {
        m_bTieToLocator = false;
        SetModelToPosition(m_mtxpos);
    }
}

void ItemEntity::EndEventProcess()
{
    UnTieFromLocator();
    if (!AttributesPointer)
        return;
    if (AttributesPointer->GetAttributeAsDword("hide_after_using", 0) != 0)
    {
        m_bVisible = false;
        AttributesPointer->SetAttributeUseDword("hide", 1);
    }
}

void ItemEntity::DrawIntoLocator()
{
    auto *pMdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(m_eidTieModel));
    if (!pMdl)
    {
        UnTieFromLocator();
        return;
    }

    CMatrix perMtx;
    int32_t sti;
    if (m_pModel)
    {
        sti = -1;
        auto idLoc = m_pMdlNode->geo->FindName(m_sTieLocName.c_str());

        if ((sti = m_pMdlNode->geo->FindLabelN(sti + 1, idLoc)) > -1)
        {
            auto *ani = pMdl->GetAnimation();
            auto *bones = &ani->GetAnimationMatrix(0);

            GEOS::LABEL lb;
            m_pMdlNode->geo->GetLabel(sti, lb);
            CMatrix mt;
            mt.Vx() = CVECTOR(lb.m[0][0], lb.m[0][1], lb.m[0][2]);
            mt.Vy() = CVECTOR(lb.m[1][0], lb.m[1][1], lb.m[1][2]);
            mt.Vz() = CVECTOR(lb.m[2][0], lb.m[2][1], lb.m[2][2]);
            mt.Pos() = CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);

            auto mbn = mt * bones[lb.bones[0]];
            mbn.Pos().x *= -1.0f;
            mbn.Vx().x *= -1.0f;
            mbn.Vy().x *= -1.0f;
            mbn.Vz().x *= -1.0f;

            CMatrix scl;
            scl.Vx().x = -1.0f;
            scl.Vy().y = 1.0f;
            scl.Vz().z = 1.0f;
            mbn.EqMultiply(scl, CMatrix(mbn));

            perMtx = mbn * pMdl->mtx;
        }

        m_pModel->mtx = perMtx;
        m_pModel->ProcessStage(Stage::realize, 0);
    }
}

entid_t ItemEntity::GetModelEIDFromCharacterEID(entid_t chrEID)
{
    if (auto *data = static_cast<VDATA *>(core.GetScriptVariable("g_TmpModelVariable")))
    {
        core.Send_Message(chrEID, "le", MSG_CHARACTER_GETMODEL, data);
        return data->GetEntityID();
    }

    return invalid_entity;
}

void ItemEntity::SetEventListener(entid_t mdlEID, entid_t mdlToTieEID, const char *pcLocName, const char *pcStartEvent,
                                  const char *pcEndEvent)
{
    auto *pMdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(mdlEID));
    if (!pMdl)
        return;
    auto *a = pMdl->GetAnimation();
    if (a)
    {
        m_eventListener.item = this;
        m_eventListener.m_bStartWaiting = true;
        m_eventListener.m_eidListenedModel = mdlEID;
        m_eventListener.m_eidToTieModel = mdlToTieEID;
        m_eventListener.m_sToTieLocator = pcLocName;
        m_eventListener.m_sStartEvent = pcStartEvent;
        m_eventListener.m_sEndEvent = pcEndEvent;
        a->SetEventListener(&m_eventListener);
        a->SetEvent(ae_end, 0, &m_eventListener);
    }
}

void ItemEntity::EventListener::Event(Animation *animation, int32_t playerIndex, const char *eventName)
{
    if (m_bStartWaiting && m_sStartEvent == eventName)
    {
        m_bStartWaiting = false;
        item->m_bTieToLocator = item->TieToLocator(m_eidToTieModel, m_sToTieLocator.c_str());
        return;
    }
    if (!m_bStartWaiting && m_sEndEvent == eventName)
    {
        auto *pMdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(m_eidListenedModel));
        if (pMdl)
        {
            auto *a = pMdl->GetAnimation();
            if (a)
                a->SetEventListener(nullptr);
        }
        item->EndEventProcess();
    }
}

bool ItemEntity::CreateParticle()
{
    DeleteParticle();

    if (m_bVisible)
    {
        auto *const pcParticleName = BIUtils::GetStringFromAttr(AttributesPointer, "particle", "");
        if (pcParticleName && pcParticleName[0])
        {
            const auto eidParticle = EntityManager::GetEntityId("particles");
            if (eidParticle)
            {
                const auto vPos = m_mtxpos.Pos();
                m_pParticle =
                    (VPARTICLE_SYSTEM *)core.Send_Message(eidParticle, "lsffffffl", PS_CREATE_RIC, pcParticleName,
                                                          vPos.x, vPos.y, vPos.z, 0.0f, 1.0f, 0.0f, 0);
                return true;
            }
        }
    }
    return false;
}

void ItemEntity::DeleteParticle()
{
    if (m_pParticle)
    {
        const auto eidParticle = EntityManager::GetEntityId("particles");
        if (eidParticle)
        {
            if (core.Send_Message(eidParticle, "lp", PS_VALIDATE_PARTICLE, m_pParticle))
                m_pParticle->Pause(true);
        }
        m_pParticle = nullptr;
    }
}
