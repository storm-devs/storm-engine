#include "enemystatshower.h"
#include "../../../shared/messages.h"
#include "model.h"

EnemyStatShower::EnemyStatShower()
{
    SetBeginData();
}

EnemyStatShower::~EnemyStatShower()
{
    Release();
}

bool EnemyStatShower::Init()
{
    if (!ReadAndCreate())
        return false;
    return true;
}

void EnemyStatShower::Realize(uint32_t delta_time)
{
}

uint64_t EnemyStatShower::ProcessMessage(MESSAGE &message)
{
    const auto nMsgCod = message.Long();
    switch (nMsgCod)
    {
    case 0: // add character for shower
    {
        // entid_t mdlEID = GetModelEIDFromCharacterEID( message.EntityID() );
    }
    break;
    }
    return -1;
}

bool EnemyStatShower::ReadAndCreate()
{
    // BIUtils::ReadVectorFormAttr(AttributesPointer,"pos",m_mtxpos.Pos(),CVECTOR(0.f));
    return true;
}

void EnemyStatShower::SetBeginData()
{
}

void EnemyStatShower::Release()
{
}

entid_t EnemyStatShower::GetModelEIDFromCharacterEID(entid_t chrEID)
{
    entid_t eid;
    auto *pvdat = static_cast<VDATA *>(api->GetScriptVariable("g_TmpModelVariable"));
    if (pvdat)
    {
        api->Send_Message(chrEID, "le", MSG_CHARACTER_GETMODEL, pvdat);
        eid = pvdat->GetEntityID();
    }
    return eid;
}
