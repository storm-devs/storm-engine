#ifndef BASE_EMITTER
#define BASE_EMITTER

#include "../../ICommon/IEmitter.h"
#include "../../ICommon/Types.h"
#include "../DataSource/DataSource.h"
#include "../ParticleSystem/particlesystem.h"
#include "math3D.h"

class DataGraph;

class BaseEmitter : public IEmitter
{
    struct structParticleType
    {
        bool Visible;         //Видим или нет
        ParticleType Type;    // Тип партикла
        float Remain;         // Сколько осталось незапущенных с прошлого кадра
        uint32_t ActiveCount; // Количество активных партиклов данного типа
        uint32_t MaxParticlesCount; // Максимальное кол-во партиклов этого типа
        DataGraph *EmissionRate; // График задающий скорость испускания партиклов

        FieldList *pFields;

        structParticleType()
        {
            ActiveCount = 0;
            Remain = 0.0f;
            EmissionRate = nullptr;
            pFields = nullptr;
            Type = UNKNOWN_PARTICLE;
        }
    };

    std::vector<structParticleType> ParticleTypes;

    DataSource::EmitterDesc *pEmitter;
    FieldList *pFields;
    bool IsAttachedFlag;

    Vector Position;
    EmitterType Type;
    float LifeTime;
    float ElapsedTime;
    DataGraph *EmissionDirX;
    DataGraph *EmissionDirY;
    DataGraph *EmissionDirZ;

    bool Looped;
    bool Stoped;
    bool Visible;

    uint32_t Unique_GUID;

    Matrix matWorldTransform;
    bool OldMatrixNotInitialized;
    Matrix matWorldTransformOld;
    Matrix matWorldTransformNew;

    void BlendMatrix(Matrix &result, const Matrix &mat1, const Matrix &mat2, float BlendK);

    void IncreaseTime(float DeltaTime);

  protected:
    std::string Name;
    ParticleSystem *pMaster;

  public:
    // Конструктор / деструктор
    BaseEmitter(ParticleSystem *pSystem);
    virtual ~BaseEmitter();

    //Получить позицию для рождения новых партиклов
    virtual Vector GetNewParticlePosition(float DeltaTime) = 0;

    //Родить новые партиклы
    void BornParticles(float DeltaTime) override;
    //Исполнить
    void Execute(float DeltaTime) override;

    //Присоединиться к источнику данных
    virtual void AttachToDataSource(DataSource::EmitterDesc *pEmitter);

    virtual void CreateBillBoardParticle(FieldList &Fields);
    virtual void CreateModelParticle(FieldList &Fields);

    ParticleSystem *GetMaster() const;
    ParticleManager *GetManager() const;
    void GetEmissionDirection(Matrix &matWorld) const;

    virtual void SetGUID(uint32_t GUID)
    {
        Unique_GUID = GUID;
    }

    virtual uint32_t GetGUID()
    {
        return Unique_GUID;
    }

    void Restart() override;

    uint32_t GetParticleCount() override;
    bool IsStoped() override;

    void SetTransform(const Matrix &matWorld) override;
    void Teleport(const Matrix &matWorld) override;

    const char *GetName() override;

    //Если флаг в true емиттер не будет самостоятельно испускать партиклы
    //так, как он привязан
    void SetAttachedFlag(bool Flag) override;
    bool IsAttached() override;

    float GetTime() override;
    void SetTime(float Time) override;

    uint32_t GetParticleTypesCount() override;
    FieldList *GetParticleTypeDataByIndex(uint32_t Index) override;
    ParticleType GetParticleTypeByIndex(uint32_t Index) override;

    FieldList *GetData() override;

    bool SetEnable(bool bVisible) override;
    bool GetEnable() override;

    //-1 если не нашли, иначе индекс
    int GetParticleTypeIndex(FieldList *pFields) override;
    bool SetParticleTypeEnable(bool bVisible, uint32_t Index) override;
    bool GetParticleTypeEnable(uint32_t Index) override;

    void Editor_UpdateCachedData() override;

    void SetName(const char *Name) override;

    void Stop() override;
};

#endif
