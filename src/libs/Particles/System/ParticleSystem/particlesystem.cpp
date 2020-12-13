#include "particlesystem.h"
#include "../Emitters/point.h"

//Глобальный GUID для присваивания эмиттерам
uint32_t EmitterID = 0;

//Создание/удаление
ParticleSystem::ParticleSystem(ParticleManager *serv)
{
    AutoDeleted = false;
    pMaster = serv;
    EmissionPause = false;
}

ParticleSystem::~ParticleSystem()
{
    pMaster->RemoveResource(this);
    DeleteAllEmitters();
}

bool ParticleSystem::Release()
{
    delete this;
    return false;
}

//Отработать всем партиклам
uint32_t ParticleSystem::Execute(float DeltaTime)
{
    if (AutoDeleted)
    {
        if (DeleteIfNeed())
        {
            // api->Trace("AUTO DELETE !!!!\n");
            return 0;
        }
    }

    if (EmissionPause)
        return Emitters.size();

    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        Emitters[n].pEmitter->Execute(DeltaTime);
    }

    return Emitters.size();
}

void ParticleSystem::CreateFromDataSource(DataSource *pDataSource)
{
    const auto EmitterCount = pDataSource->GetEmitterCount();
    if (EmitterCount <= 0)
        return;

    for (auto n = 0; n < EmitterCount; n++)
    {
        auto *const pEmitterDecription = pDataSource->GetEmitterDesc(n);
        IEmitter *pEmitter = nullptr;
        switch (pEmitterDecription->Type)
        {
        case POINT_EMITTER:
            pEmitter = CreatePointEmitter(pEmitterDecription);
            break;
        default:
            throw std::exception("Particles: Unknown emitter type !!!!");
        }

        // Assert (pEmitter);

        // CreateParticles (pEmitter, pEmitterDecription->Particles);
    }
}

IEmitter *ParticleSystem::CreatePointEmitter(DataSource::EmitterDesc *pEmitter)
{
    //	api->Trace("Create 'Point' emitter\n");
    auto *pNewEmitter = new PointEmitter(this);
    pNewEmitter->AttachToDataSource(pEmitter);
    pNewEmitter->SetGUID(EmitterID);

    EmitterDesc NewEmitter;
    NewEmitter.Type = POINT_EMITTER;
    NewEmitter.pEmitter = pNewEmitter;
    Emitters.push_back(NewEmitter);

    EmitterID += GUIDSTEP;

    return pNewEmitter;
}

void ParticleSystem::DeleteAllEmitters()
{
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        delete Emitters[n].pEmitter;
    }

    Emitters.clear();
}

ParticleManager *ParticleSystem::GetMaster() const
{
    return pMaster;
}

//Перезапустить партикловую систему
void ParticleSystem::Restart(uint32_t RandomSeed)
{
    srand(RandomSeed);
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        Emitters[n].pEmitter->Restart();
    }

    PauseEmission(false);
}

//Запаузить испускание партиклов
void ParticleSystem::PauseEmission(bool bPause)
{
    EmissionPause = bPause;
}

//Узнать на паузе эмиссия или нет
bool ParticleSystem::IsEmissionPaused()
{
    return EmissionPause;
}

bool ParticleSystem::DeleteIfNeed()
{
    if (!IsAlive())
    {
        pMaster->DefferedDelete(this);
        return true;
    }
    return false;
}

//Установить автоудаляемая система или обычная...
void ParticleSystem::AutoDelete(bool Enabled)
{
    AutoDeleted = Enabled;
}

//Узнаять автоудаляемая система или нет
bool ParticleSystem::IsAutoDeleted()
{
    return AutoDeleted;
}

//Установить матрицу трансформации для системы
void ParticleSystem::SetTransform(const Matrix &transform)
{
    matWorld = transform;
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        Emitters[n].pEmitter->SetTransform(transform);
    }
}

void ParticleSystem::Teleport(const Matrix &transform)
{
    matWorld = transform;
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        Emitters[n].pEmitter->Teleport(transform);
    }
}

void ParticleSystem::GetTransform(Matrix &_matWorld)
{
    _matWorld = matWorld;
}

IEmitter *ParticleSystem::FindEmitterByData(FieldList *Data)
{
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        if (Emitters[n].pEmitter->GetData() == Data)
            return Emitters[n].pEmitter;
    }

    return nullptr;
}

IEmitter *ParticleSystem::FindEmitter(const char *name)
{
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        if (_stricmp(Emitters[n].pEmitter->GetName(), name) == 0)
            return Emitters[n].pEmitter;
    }

    return nullptr;
}

uint32_t ParticleSystem::GetEmittersCount() const
{
    return Emitters.size();
}

IEmitter *ParticleSystem::GetEmitterByIndex(uint32_t Index)
{
    return Emitters[Index].pEmitter;
}

EmitterType ParticleSystem::GetEmitterTypeByIndex(uint32_t Index)
{
    return Emitters[Index].Type;
}

bool ParticleSystem::IsAlive()
{
    uint32_t pCount = 0;
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        pCount += Emitters[n].pEmitter->GetParticleCount();
        if (!Emitters[n].pEmitter->IsStoped())
            return true;
    }

    if (pCount > 0)
        return true;

    return false;
}

const char *ParticleSystem::GetName()
{
    return SystemName.c_str();
}

void ParticleSystem::SetName(const char *Name)
{
    SystemName = Name;
}

void ParticleSystem::Editor_UpdateCachedData()
{
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        Emitters[n].pEmitter->Editor_UpdateCachedData();
    }
}

void ParticleSystem::Stop()
{
    for (uint32_t n = 0; n < Emitters.size(); n++)
    {
        Emitters[n].pEmitter->Stop();
    }
}
