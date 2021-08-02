#include "TFishSchools.h"
#include "Entity.h"
#include "core.h"
#include "defines.h"
#include "rands.h"
#include "shared/messages.h"
#include "vfile_service.h"

//--------------------------------------------------------------------
TFishSchools::TFishSchools() : enabled(false)
{
    fishSchools.resize(FISHSCHOOL_COUNT);
}

//--------------------------------------------------------------------
TFishSchools::~TFishSchools()
{
    EntityManager::EraseEntity(fishSchoolModel);
}

//--------------------------------------------------------------------
void TFishSchools::LoadSettings()
{
    auto ini = fio->OpenIniFile(ANIMALS_INI_FILENAME);
    if (!ini)
    {
        return;
    }
       
    fishSchoolsCount = ini->GetLong(ANIMALS_FISHSCHOOLS_SECTION, "count", FISHSCHOOL_COUNT);
    maxDistance = ini->GetFloat(ANIMALS_FISHSCHOOLS_SECTION, "distance", FISHSCHOOL_DISTANCE);
}

//--------------------------------------------------------------------
void TFishSchools::Init()
{
    LoadSettings();

    renderService = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!renderService)
    {
        throw std::runtime_error("!FishSchools: No service 'dx9render'");
    }       

    sea = static_cast<SEA_BASE*>(EntityManager::GetEntityPointer(EntityManager::GetEntityId("sea")));
    if (!sea)
    {
        enabled = false;
        return;
    }

    for (auto& fishSchool : fishSchools)
    {
        fishSchool = std::make_unique<TFishSchool>();
        fishSchool->Initialize(CVECTOR(0.0f, 0.0f, 0.0f), maxDistance);
        fishSchool->depth = 0.05f;
        fishSchool->time = 0;
        fishSchool->timeDivider = randUpper(100.0f);
        fishSchool->amplitude = rand(0.01f);
        AddDeflector(fishSchool.get());
    }

    AddAttractor(cameraObject.get());

    fishSchoolModel = EntityManager::CreateEntity("MODELR");
    core.Send_Message(fishSchoolModel, "ls", MSG_MODEL_LOAD_GEO, ANIMALS_FISHSCHOOL_FILENAME);
}

//--------------------------------------------------------------------
uint64_t TFishSchools::ProcessMessage(const long _code, MESSAGE &message)
{
    switch (_code)
    {
    case MSG_ANIMALS_FISHSCHOOLS_SHOW:
        enabled = true;
        break;

    case MSG_ANIMALS_FISHSCHOOLS_HIDE:
        enabled = false;
        break;
    }

    return 0;
}

//--------------------------------------------------------------------
void TFishSchools::Execute(const uint32_t _dTime)
{
    if (!enabled)
    {
        return;
    }

    CVECTOR pos, ang, fishPos;
    float persp;
    renderService->GetCamera(pos, ang, persp);

    cameraObject->SetXYZ(pos);
    const auto speedK = static_cast<float>(_dTime) / 1000.0f;

    for (auto& fishSchool : fishSchools)
    {
        // respawn near camera if needed
        fishPos = fishSchool->GetXYZ();
        if ((fabs(fishPos.x - pos.x) + fabs(fishPos.x - pos.x)) > RESPAWN_DISTANCE)
        {
            fishSchool->Initialize(pos, maxDistance);
        }           

        // recalculate all fishes
        fishSchool->Calculate(attractors, deflectors, speedK);
        fishSchool->time += _dTime;
    }
}

//--------------------------------------------------------------------
void TFishSchools::Realize(const uint32_t _dTime)
{
    if (!enabled)
    {
        return;
    }        
    /*
      CVECTOR cameraPos, cameraAng;
      float   cameraPersp;
      renderService->GetCamera(cameraPos, cameraAng, cameraPersp);
    */
    sea = static_cast<SEA_BASE*>(EntityManager::GetEntityPointer(EntityManager::GetEntityId("sea")));
    if (!sea)
    {
        enabled = false;
        return;
    }

    auto *fishSchoolPtr = static_cast<MODEL*>(EntityManager::GetEntityPointer(fishSchoolModel));
    if (!fishSchoolPtr)
    {
        return;
    }        

    for (auto& fishSchool : fishSchools)
    {
        static const auto OSC_AMPLITUDE = 0.1f;
        const auto fishSchoolAngle = fishSchool->GetAngle();
        const auto fishSchoolPos = fishSchool->GetXYZ();
        const auto fishSchoolTime = fishSchool->time / fishSchool->timeDivider;
        CVECTOR ang(0.0f,
                    PId2 - fishSchoolAngle - (fishSchool->amplitude * (PId2 / 10.0f) / 0.1f) * cosf(fishSchoolTime),
                    0.0f);
        CVECTOR pos;
        pos.x = fishSchoolPos.x + fishSchool->amplitude * cosf(fishSchoolAngle + PId2) * sinf(fishSchoolTime);
        pos.z = fishSchoolPos.z + fishSchool->amplitude * sinf(fishSchoolAngle + PId2) * sinf(fishSchoolTime);
        pos.y = sea->WaveXZ(pos.x, pos.z) - fishSchool->depth;
        fishSchoolPtr->mtx.BuildMatrix(ang, pos);
        fishSchoolPtr->ProcessStage(Entity::Stage::realize, _dTime);
    }
}

//--------------------------------------------------------------------
