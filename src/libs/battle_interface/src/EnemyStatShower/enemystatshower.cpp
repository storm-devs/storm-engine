#include "enemystatshower.h"
#include "core.h"
#include "model.h"
#include "shared/messages.h"

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
    auto *pvdat = static_cast<VDATA *>(core.GetScriptVariable("g_TmpModelVariable"));
    if (pvdat)
    {
        core.Send_Message(chrEID, "le", MSG_CHARACTER_GETMODEL, pvdat);
        eid = pvdat->GetEntityID();
    }
    return eid;
}
