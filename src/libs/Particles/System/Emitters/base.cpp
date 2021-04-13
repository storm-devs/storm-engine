#include "base.h"
#include "../../ICommon/Names.h"
#include "../DataSource/DataGraph.h"
#include "../DataSource/DataString.h"
#include "math3d/Quaternion.h"

#define INTERPOLATION_STEPS 32.0f

// Constructor / destructor
BaseEmitter::BaseEmitter(ParticleSystem *pSystem)
{
    pEmitter = nullptr;
    Visible = true;
    pFields = nullptr;
    IsAttachedFlag = false;
    OldMatrixNotInitialized = true;
    LifeTime = 0.0f;
    pMaster = pSystem;
    ElapsedTime = 0.0f;
    Position = Vector(0.0f);
    EmissionDirX = nullptr;
    EmissionDirY = nullptr;
    EmissionDirZ = nullptr;
    Stoped = false;
}

BaseEmitter::~BaseEmitter()
{
    GetManager()->GetBBProcessor()->DeleteWithGUID(Unique_GUID);
    GetManager()->GetMDLProcessor()->DeleteWithGUID(Unique_GUID);
    Stoped = true;
}

// Emit new particles
void BaseEmitter::BornParticles(float DeltaTime)
{
    const auto SavedTime = ElapsedTime;
    if (!Visible)
        return;
    Matrix matTransform;

    auto MatrixBlend = 0.0f;
    const auto MatrixBlendInc = 1.0f / INTERPOLATION_STEPS;

    for (auto i = 0; i < static_cast<int>(INTERPOLATION_STEPS); i++)
    {
        BlendMatrix(matWorldTransform, matWorldTransformOld, matWorldTransformNew, MatrixBlend);

        const auto TransformPos = Position * matWorldTransform;
        matWorldTransform.pos = TransformPos;
        MatrixBlend += MatrixBlendInc;

        const auto DeltaTimeDiv = DeltaTime / INTERPOLATION_STEPS;
        IncreaseTime(DeltaTimeDiv);

        // If the emitter gets stuck early exit
        if (Stoped)
        {
            ElapsedTime = SavedTime;
            return;
        }

        for (uint32_t n = 0; n < ParticleTypes.size(); n++)
        {
            if (!ParticleTypes[n].Visible)
                continue;

            auto EmissionRate = ParticleTypes[n].EmissionRate->GetRandomValue(ElapsedTime, LifeTime);
            EmissionRate *= DeltaTimeDiv;

            // How many particles remain unemissed from last frame
            auto ParticlesRemain = ParticleTypes[n].Remain;

            ParticlesRemain += EmissionRate;
            ParticleTypes[n].Remain = ParticlesRemain;

            while (ParticleTypes[n].Remain >= 1.0f)
            {
                ParticleTypes[n].Remain -= 1.0f;
                if (ParticleTypes[n].ActiveCount < ParticleTypes[n].MaxParticlesCount)
                {
                    auto ParticlePos = GetNewParticlePosition(DeltaTime);
                    GetEmissionDirection(matTransform);
                    auto VelDir = matTransform.vy;
                    switch (ParticleTypes[n].Type)
                    {
                    case BILLBOARD_PARTICLE:
                        ParticleTypes[n].ActiveCount++;
                        GetManager()->GetBBProcessor()->AddParticle(pMaster, VelDir, ParticlePos, matWorldTransform,
                                                                    ElapsedTime, LifeTime, ParticleTypes[n].pFields,
                                                                    &ParticleTypes[n].ActiveCount, (Unique_GUID + n));
                        break;
                    case MODEL_PARTICLE:
                        ParticleTypes[n].ActiveCount++;
                        GetManager()->GetMDLProcessor()->AddParticle(pMaster, VelDir, ParticlePos, matWorldTransform,
                                                                     ElapsedTime, LifeTime, ParticleTypes[n].pFields,
                                                                     &ParticleTypes[n].ActiveCount, (Unique_GUID + n));
                        // ParticleTypes[n].ActiveCount++;
                        // GetMaster()->GetMaster()->GetMDLProcessor()->AddParticle(ParticleTypes[n].pFields);
                        break;
                    default:
                        throw std::runtime_error("Try to emmit unknown particle type");
                    } // switch
                }     // Active < Max
            }         // While Remain
        }             // For all types
    }                 // INTERPOLATION_STEPS iterations

    matWorldTransformOld = matWorldTransformNew;

    ElapsedTime = SavedTime;
}

// Execute
void BaseEmitter::Execute(float DeltaTime)
{
    if (!Stoped && !IsAttachedFlag)
        BornParticles(DeltaTime);

    IncreaseTime(DeltaTime);

    /*
      for (DWORD n = 0; n < ParticleTypes.size(); n++)
      {
        switch (ParticleTypes[n].Type)
        {
          case BILLBOARD_PARTICLE:
            break;
          case MODEL_PARTICLE:
            break;
        }
      }
    */
}

// Attach to data source
void BaseEmitter::AttachToDataSource(DataSource::EmitterDesc *pEmitter)
{
    this->pEmitter = pEmitter;
    pFields = &pEmitter->Fields;
    Type = pEmitter->Type;

    Editor_UpdateCachedData();

    EmissionDirX = pEmitter->Fields.FindGraph(EMISSION_DIR_X);
    EmissionDirY = pEmitter->Fields.FindGraph(EMISSION_DIR_Y);
    EmissionDirZ = pEmitter->Fields.FindGraph(EMISSION_DIR_Z);

    for (uint32_t n = 0; n < pEmitter->Particles.size(); n++)
    {
        auto *pDesc = &pEmitter->Particles[n];
        switch (pDesc->Type)
        {
        case BILLBOARD_PARTICLE:
            CreateBillBoardParticle(pDesc->Fields);
            break;
        case MODEL_PARTICLE:
            CreateModelParticle(pDesc->Fields);
            break;
        default:
            throw std::runtime_error("Particles: Unknown particle type !!!!");
        }
    }
}

void BaseEmitter::IncreaseTime(float DeltaTime)
{
    ElapsedTime += DeltaTime;
    if (ElapsedTime > LifeTime)
    {
        if (!Looped)
            Stoped = true;
        ElapsedTime -= LifeTime;
    }
}

void BaseEmitter::CreateBillBoardParticle(FieldList &Fields)
{
    //    core.Trace("Create BB Particle\n");

    ParticleTypes.push_back(structParticleType{});
    // structParticleType* NewBillBoard = &ParticleTypes[ParticleTypes.Add()];
    auto *NewBillBoard = &ParticleTypes.back();
    NewBillBoard->Type = BILLBOARD_PARTICLE;
    NewBillBoard->EmissionRate = Fields.FindGraph(EMISSION_RATE);
    NewBillBoard->MaxParticlesCount = Fields.GetFloatAsInt(MAX_PARTICLES_COUNT);
    NewBillBoard->ActiveCount = 0;
    NewBillBoard->Remain = 0.0f;
    NewBillBoard->pFields = &Fields;
    NewBillBoard->Visible = true;

    //    core.Trace("%d", NewBillBoard->MaxParticlesCount);
}

void BaseEmitter::CreateModelParticle(FieldList &Fields)
{
    //    core.Trace("Create MODEL Particle\n");

    ParticleTypes.push_back(structParticleType{});
    // structParticleType* NewModel = &ParticleTypes[ParticleTypes.Add()];
    auto *NewModel = &ParticleTypes.back();
    NewModel->Type = MODEL_PARTICLE;
    NewModel->EmissionRate = Fields.FindGraph(PARTICLE_EMISSION_RATE);
    NewModel->MaxParticlesCount = Fields.GetFloatAsInt(PARTICLE_MAX_COUNT);
    NewModel->ActiveCount = 0;
    NewModel->Remain = 0.0f;
    NewModel->pFields = &Fields;
    NewModel->Visible = true;
}

ParticleSystem *BaseEmitter::GetMaster() const
{
    return pMaster;
}

ParticleManager *BaseEmitter::GetManager() const
{
    return pMaster->GetMaster();
}

void BaseEmitter::GetEmissionDirection(Matrix &matWorld) const
{
    Vector DirAngles;
    DirAngles.x = EmissionDirX->GetRandomValue(ElapsedTime, LifeTime);
    DirAngles.y = EmissionDirY->GetRandomValue(ElapsedTime, LifeTime);
    DirAngles.z = EmissionDirZ->GetRandomValue(ElapsedTime, LifeTime);

    DirAngles *= MUL_DEGTORAD;

    matWorld = Matrix(DirAngles, Vector(0.0f));
}

void BaseEmitter::Restart()
{
    // Removing particles ...
    GetManager()->GetBBProcessor()->DeleteWithGUID(Unique_GUID);
    GetManager()->GetMDLProcessor()->DeleteWithGUID(Unique_GUID);
    Stoped = false;
    ElapsedTime = 0.0f;
}

uint32_t BaseEmitter::GetParticleCount()
{
    uint32_t Count = 0;
    for (uint32_t n = 0; n < ParticleTypes.size(); n++)
    {
        Count += ParticleTypes[n].ActiveCount;
    }

    return Count;
}

bool BaseEmitter::IsStoped()
{
    return Stoped;
}

void BaseEmitter::SetTransform(const Matrix &matWorld)
{
    if (OldMatrixNotInitialized)
    {
        matWorldTransformOld = matWorld;
        OldMatrixNotInitialized = false;
    }

    matWorldTransformNew = matWorld;
    // matWorldTransform = matWorld;
}

void BaseEmitter::Teleport(const Matrix &matWorld)
{
    matWorldTransformOld = matWorld;
    matWorldTransformNew = matWorld;
}

void BaseEmitter::BlendMatrix(Matrix &result, const Matrix &mat1, const Matrix &mat2, float BlendK)
{
    const Quaternion qRot1(mat1);
    const Quaternion qRot2(mat2);
    const auto vPos1 = mat1.pos;
    const auto vPos2 = mat2.pos;

    Quaternion qBlend;
    qBlend.SLerp(qRot1, qRot2, BlendK);

    Vector vBlend;
    vBlend.Lerp(vPos1, vPos2, BlendK);

    qBlend.GetMatrix(result);
    result.pos = vBlend;
}

const char *BaseEmitter::GetName()
{
    return Name.c_str();
}

void BaseEmitter::SetAttachedFlag(bool Flag)
{
    IsAttachedFlag = Flag;
}

bool BaseEmitter::IsAttached()
{
    return IsAttachedFlag;
}

float BaseEmitter::GetTime()
{
    return ElapsedTime;
}

void BaseEmitter::SetTime(float Time)
{
    ElapsedTime = Time;
}

uint32_t BaseEmitter::GetParticleTypesCount()
{
    return ParticleTypes.size();
}

FieldList *BaseEmitter::GetParticleTypeDataByIndex(uint32_t Index)
{
    return ParticleTypes[Index].pFields;
}

ParticleType BaseEmitter::GetParticleTypeByIndex(uint32_t Index)
{
    return ParticleTypes[Index].Type;
}

FieldList *BaseEmitter::GetData()
{
    return pFields;
}

bool BaseEmitter::SetEnable(bool bVisible)
{
    Visible = bVisible;

    if (Visible == false)
    {
        GetManager()->GetBBProcessor()->DeleteWithGUID(Unique_GUID);
        GetManager()->GetMDLProcessor()->DeleteWithGUID(Unique_GUID);
    }

    for (uint32_t n = 0; n < ParticleTypes.size(); n++)
        ParticleTypes[n].Visible = bVisible;

    return Visible;
}

bool BaseEmitter::GetEnable()
{
    return Visible;
}

int BaseEmitter::GetParticleTypeIndex(FieldList *pFields)
{
    for (uint32_t n = 0; n < ParticleTypes.size(); n++)
    {
        if (ParticleTypes[n].pFields == pFields)
            return n;
    }
    return -1;
}

bool BaseEmitter::SetParticleTypeEnable(bool bVisible, uint32_t Index)
{
    ParticleTypes[Index].Visible = bVisible;

    if (bVisible == false)
    {
        GetManager()->GetBBProcessor()->DeleteWithGUID(Unique_GUID + Index, 1);
        GetManager()->GetMDLProcessor()->DeleteWithGUID(Unique_GUID + Index, 1);
    }

    return ParticleTypes[Index].Visible;
}

bool BaseEmitter::GetParticleTypeEnable(uint32_t Index)
{
    return ParticleTypes[Index].Visible;
}

void BaseEmitter::Editor_UpdateCachedData()
{
    Name = pEmitter->Fields.GetString(EMITTER_NAME, "NoName");
    LifeTime = pEmitter->Fields.GetFloat(EMITTER_LIFETIME);
    Assert(LifeTime > 0);
    Position = pEmitter->Fields.GetPosition(EMITTER_POSITION);
    Looped = pEmitter->Fields.GetBool(EMITTER_LOOPING, false);

    for (uint32_t n = 0; n < ParticleTypes.size(); n++)
    {
        ParticleTypes[n].MaxParticlesCount = ParticleTypes[n].pFields->GetFloatAsInt(MAX_PARTICLES_COUNT);
    }
}

void BaseEmitter::SetName(const char *Name)
{
    auto *EmitterName = pEmitter->Fields.FindString(EMITTER_NAME);
    Assert(EmitterName);
    EmitterName->SetValue(Name);
    Editor_UpdateCachedData();
}

void BaseEmitter::Stop()
{
    Stoped = true;
}
