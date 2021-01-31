#ifndef _PARTICLE_SYSTEM_H_
#define _PARTICLE_SYSTEM_H_

#include "../../ICommon/Types.h"
#include "../../Manager/particlemanager.h"
#include "../DataSource/DataSource.h"
#include "particles/iparticlesystem.h"
#include "vmodule_api.h"

class IEmitter;

class ParticleSystem : public IParticleSystem
{
    struct EmitterDesc
    {
        EmitterType Type;
        IEmitter *pEmitter;
    };

    std::vector<EmitterDesc> Emitters;

    //Автоудаляемая система или обычная
    bool AutoDeleted;

    // Не рождать новые партиклы !!!
    bool EmissionPause;

    bool DeleteIfNeed();

    std::string SystemName;

  protected:
    virtual ~ParticleSystem();

    IEmitter *CreatePointEmitter(DataSource::EmitterDesc *pEmitter);

    void DeleteAllEmitters();

    ParticleManager *pMaster;

    Matrix matWorld;

  public:
    //Создание/удаление
    ParticleSystem(ParticleManager *serv);
    bool Release() override;

    //Отработать всем партиклам
    uint32_t Execute(float DeltaTime) override;

    //Перезапустить партикловую систему
    void Restart(uint32_t RandomSeed) override;

    //Запаузить испускание партиклов
    void PauseEmission(bool bPause) override;

    void Stop() override;
    //Узнать на паузе эмиссия или нет
    bool IsEmissionPaused() override;

    //Установить автоудаляемая система или обычная...
    void AutoDelete(bool Enabled) override;
    //Узнаять автоудаляемая система или нет
    bool IsAutoDeleted() override;
    //Установить матрицу трансформации для системы
    void SetTransform(const Matrix &transform) override;
    void GetTransform(Matrix &_matWorld) override;

    void CreateFromDataSource(DataSource *pDataSource);

    ParticleManager *GetMaster() const;

    IEmitter *FindEmitter(const char *name);
    IEmitter *FindEmitterByData(FieldList *Data);

    void Teleport(const Matrix &transform) override;

    bool IsAlive() override;

    const char *GetName() override;
    void SetName(const char *Name);

  public:
    uint32_t GetEmittersCount() const;
    IEmitter *GetEmitterByIndex(uint32_t Index);
    EmitterType GetEmitterTypeByIndex(uint32_t Index);

    void Editor_UpdateCachedData();
};

#endif
