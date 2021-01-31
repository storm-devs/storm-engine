#ifndef _PARTICLE_DATA_SOURCE_H_
#define _PARTICLE_DATA_SOURCE_H_

#include "../../icommon/types.h"
#include "fieldlist.h"
#include <vector>

class DataObject;
class DataColor;
class DataBool;
class DataFloat;
class DataGraph;
class DataString;
class DataPosition;
class DataUV;

class MemFile;

class IParticleManager;

//Источник данных для партиклов...
class DataSource
{
  public:
    //Описание партикла (используеться при создании системы)
    struct ParticleDesc
    {
        ParticleType Type;
        FieldList Fields;

        ParticleDesc()
        {
            Type = UNKNOWN_PARTICLE;
        }
    };

    //Описание эмиттера (используеться при создании системы)
    struct EmitterDesc
    {
        EmitterType Type;
        FieldList Fields;
        std::vector<ParticleDesc> Particles;

        EmitterDesc()
        {
            Type = UNKNOWN_EMITTER;
        }
    };

  private:
    std::vector<EmitterDesc> Emitters;

    //Загрузить точечный эмиттер
    void CreatePointEmitter(MemFile *pMemFile);

    //Загрузить BillBoard партикл
    void CreateBillBoardParticle(std::vector<ParticleDesc> &Particles, MemFile *pMemFile);

    //Загрузить Model партикл
    void CreateModelParticle(std::vector<ParticleDesc> &Particles, MemFile *pMemFile);

    int FindEmitter(const char *Name);

  protected:
    virtual ~DataSource();

  public:
    FieldList *CreateEmptyPointEmitter(const char *EmitterName);
    FieldList *CreateBillBoardParticle(const char *ParticleName, const char *EmitterName);
    FieldList *CreateModelParticle(const char *ParticleName, const char *EmitterName);

    void DeletePointEmitter(FieldList *pEmitter);
    void DeleteBillboard(FieldList *pEmitter, FieldList *pParticles);
    void DeleteModel(FieldList *pEmitter, FieldList *pParticles);

    void Destroy();

    //---------- Создание/удаление --------------------
    DataSource(IParticleManager *Master);
    bool Release() const;

    // ========================= Load & Save =======================================
    //Сохранить/восстановить из файла
    virtual void Write(MemFile *pMemFile);
    virtual void Load(MemFile *pMemFile);

    virtual int GetEmitterCount();
    EmitterDesc *GetEmitterDesc(int Index);
};

#endif
