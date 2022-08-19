#include "mdl_processor.h"

#include "core.h"

#include "../../i_common/i_emitter.h"
#include "../../i_common/names.h"
#include "../data_source/data_graph.h"
#include "../particle_system/particle_system.h"
#include "geos.h"
#include "math_inlines.h"
#include "physic.h"
#include "string_compare.hpp"

// how many models there can be
#define MAX_MODELS 8192

ModelProcessor::ModelProcessor(ParticleManager *pManager)
    : Parser()
{
    Particles.reserve(MAX_MODELS);
    pMasterManager = pManager;
    pMemArray = new MemArrayItem[MAX_MODELS];

    for (uint32_t n = 0; n < MAX_MODELS; n++)
    {
        pMemArray[n].Free = true;
    }

    pRS = static_cast<VDX9RENDER *>(core.GetService("DX9Render"));
    Assert(pRS);
}

ModelProcessor::~ModelProcessor()
{
    delete pMemArray;
}

MDL_ParticleData *ModelProcessor::AllocParticle() const
{
    for (uint32_t n = 0; n < MAX_MODELS; n++)
    {
        if (pMemArray[n].Free)
        {
            pMemArray[n].Free = false;
            return &pMemArray[n].pData;
        }
    }

    return nullptr;
}

void ModelProcessor::FreeParticle(MDL_ParticleData *pItem) const
{
    for (uint32_t n = 0; n < MAX_MODELS; n++)
    {
        if (&pMemArray[n].pData == pItem)
        {
            pMemArray[n].Free = true;
            return;
        }
    }
}

void ModelProcessor::AddParticle(ParticleSystem *pSystem, const Vector &velocity_dir, const Vector &pos,
                                 const Matrix &matWorld, float EmitterTime, float EmitterLifeTime, FieldList *pFields,
                                 uint32_t *pActiveCount, uint32_t dwGUID)
{
    auto *pData = AllocParticle();

    // works if the number of particles > MAX_BILLBOARDS, there shouldn't be that many :))))
    if (!pData)
    {
        *(pActiveCount) = (*(pActiveCount)-1);
        return;
    }

    const auto *const GeomNames = pFields->GetString(PARTICLE_GEOM_NAMES);
    const auto *const pGeomName = Parser.GetRandomName(GeomNames);
    pData->pScene = pMasterManager->GetModel(pGeomName);

    if (!pData->pScene)
    {
        // core.Trace("Cant create particle. Reason geometry '%s', '%s' not found !!!", GeomNames, pGeomName);
        *(pActiveCount) = (*(pActiveCount)-1);
        FreeParticle(pData);
        return;
    }

    pData->Graph_TrackX = pFields->FindGraph(PARTICLE_TRACK_X);
    pData->Graph_TrackY = pFields->FindGraph(PARTICLE_TRACK_Y);
    pData->Graph_TrackZ = pFields->FindGraph(PARTICLE_TRACK_Z);

    Vector PositionOffset;
    PositionOffset.x = pData->Graph_TrackX->GetRandomValue(0.0f, 100.0f);
    PositionOffset.y = pData->Graph_TrackY->GetRandomValue(0.0f, 100.0f);
    PositionOffset.z = pData->Graph_TrackZ->GetRandomValue(0.0f, 100.0f);

    pData->EmitterGUID = dwGUID;
    pData->ActiveCount = pActiveCount;
    pData->RenderPos = (pos + PositionOffset) * matWorld;
    pData->Velocity = matWorld.MulNormal(velocity_dir);
    pData->ElapsedTime = 0.0f;
    pData->matWorld = matWorld;

    pData->Angle = Vector(0.0f);
    pData->RenderAngle = Vector(0.0f);
    pData->ExternalForce = Vector(0.0f, 0.0f, 0.0f);
    pData->PhysPos = pData->RenderPos;

    pData->OldRenderPos = pData->RenderPos;
    pData->OldRenderAngle = pData->RenderAngle;

    pData->LifeTime = pFields->GetRandomGraphVal(PARTICLE_LIFE_TIME, EmitterTime, EmitterLifeTime);
    pData->Mass = pFields->GetRandomGraphVal(PARTICLE_MASS, EmitterTime, EmitterLifeTime);
    pData->Spin.x = pFields->GetRandomGraphVal(PARTICLE_SPIN_X, EmitterTime, EmitterLifeTime);
    pData->Spin.y = pFields->GetRandomGraphVal(PARTICLE_SPIN_Y, EmitterTime, EmitterLifeTime);
    pData->Spin.z = pFields->GetRandomGraphVal(PARTICLE_SPIN_Z, EmitterTime, EmitterLifeTime);
    pData->Spin = pData->Spin * MUL_DEGTORAD;
    // core.Trace("spin %3.2f, %3.2f, %3.2f [%3.2f, %3.2f]", pData->Spin.x, pData->Spin.y, pData->Spin.z, EmitterTime,
    // EmitterLifeTime);

    const auto VelocityPower = pFields->GetRandomGraphVal(PARTICLE_VELOCITY_POWER, EmitterTime, EmitterLifeTime);
    pData->Velocity = pData->Velocity * VelocityPower;
    pData->UMass = fabsf(pData->Mass);

    pData->Graph_SpinDragX = pFields->FindGraph(PARTICLE_SPIN_DRAGX);
    pData->Graph_SpinDragY = pFields->FindGraph(PARTICLE_SPIN_DRAGY);
    pData->Graph_SpinDragZ = pFields->FindGraph(PARTICLE_SPIN_DRAGZ);
    pData->Graph_Drag = pFields->FindGraph(PARTICLE_DRAG);
    pData->Graph_PhysBlend = pFields->FindGraph(PARTICLE_PHYSIC_BLEND);
    pData->graph_GravK = pFields->FindGraph(PARTICLE_GRAVITATION_K);

    pData->DragK = FRAND(1.0f);
    pData->SpinDragK_X = FRAND(1.0f);
    pData->SpinDragK_Y = FRAND(1.0f);
    pData->SpinDragK_Z = FRAND(1.0f);
    pData->GravKK = FRAND(1.0f);

    pData->KPhysBlend = FRAND(1.0f);
    pData->KTrackX = FRAND(1.0f);
    pData->KTrackY = FRAND(1.0f);
    pData->KTrackZ = FRAND(1.0f);

    const auto *const pEmitterName = pFields->GetString(ATTACHEDEMITTER_NAME);
    if (storm::iEquals(pEmitterName, "none"))
    {
        pData->AttachedEmitter = nullptr;
    }
    else
    {
        pData->AttachedEmitter = pSystem->FindEmitter(pEmitterName);
        if (pData->AttachedEmitter)
            pData->AttachedEmitter->SetAttachedFlag(true);
    }

    Particles.push_back(pData);
}

// Calculates physics, tracks, etc.
void ModelProcessor::Process(float DeltaTime)
{
    // DWORD t;
    // RDTSC_B (t);

    for (uint32_t n = 0; n < Particles.size(); n++)
    {
        Particles[n]->ElapsedTime += DeltaTime;

        const auto Time = Particles[n]->ElapsedTime;
        const auto LifeTime = Particles[n]->LifeTime;

        //        _mm_prefetch ((const char *)Particles[n+1], _MM_HINT_T0);

        // kill the dead ones ...
        if (Time > LifeTime)
        {
            *(Particles[n]->ActiveCount) = (*(Particles[n]->ActiveCount) - 1);
            FreeParticle(Particles[n]);
            // Particles.ExtractNoShift(n);
            Particles[n] = Particles.back();
            Particles.pop_back();
            n--;
            continue;
        }

        auto Drag = Particles[n]->Graph_Drag->GetValue(Time, LifeTime, Particles[n]->DragK);
        Drag = 1.0f - (Drag * 0.01f);
        if (Drag < 0.0f)
            Drag = 0.0f;
        if (Drag > 1.0f)
            Drag = 1.0f;

        const auto GravK = Particles[n]->graph_GravK->GetValue(Time, LifeTime, Particles[n]->GravKK);

        AddGravityForce(Particles[n]->ExternalForce, Particles[n]->Mass, GravK);
        SolvePhysic(Particles[n]->PhysPos, Particles[n]->Velocity, Particles[n]->ExternalForce, Particles[n]->UMass,
                    Drag, DeltaTime);
        Particles[n]->ExternalForce = Vector(0.0f);

        // FIX ME !!!
        Vector SpinDrag;
        SpinDrag.x = Particles[n]->Graph_SpinDragX->GetValue(Time, LifeTime, Particles[n]->SpinDragK_X);
        SpinDrag.x = 1.0f - (SpinDrag.x * 0.01f);
        if (SpinDrag.x < 0.0f)
            SpinDrag.x = 0.0f;
        if (SpinDrag.x > 1.0f)
            SpinDrag.x = 1.0f;

        SpinDrag.y = Particles[n]->Graph_SpinDragX->GetValue(Time, LifeTime, Particles[n]->SpinDragK_Y);
        SpinDrag.y = 1.0f - (SpinDrag.y * 0.01f);
        if (SpinDrag.y < 0.0f)
            SpinDrag.y = 0.0f;
        if (SpinDrag.y > 1.0f)
            SpinDrag.y = 1.0f;

        SpinDrag.z = Particles[n]->Graph_SpinDragX->GetValue(Time, LifeTime, Particles[n]->SpinDragK_Z);
        SpinDrag.z = 1.0f - (SpinDrag.z * 0.01f);
        if (SpinDrag.z < 0.0f)
            SpinDrag.z = 0.0f;
        if (SpinDrag.z > 1.0f)
            SpinDrag.z = 1.0f;

        Particles[n]->Angle += (Particles[n]->Spin * SpinDrag) * DeltaTime;

        Vector TrackPos;
        TrackPos.x = Particles[n]->Graph_TrackX->GetValue(Time, LifeTime, Particles[n]->KTrackX);
        TrackPos.y = Particles[n]->Graph_TrackY->GetValue(Time, LifeTime, Particles[n]->KTrackY);
        TrackPos.z = Particles[n]->Graph_TrackZ->GetValue(Time, LifeTime, Particles[n]->KTrackZ);
        TrackPos = TrackPos * Particles[n]->matWorld;

        // FIX ME !!!
        auto BlendPhys = Particles[n]->Graph_PhysBlend->GetValue(Time, LifeTime, Particles[n]->KPhysBlend);
        BlendPhys = 1.0f - (BlendPhys * DeltaTime);
        if (BlendPhys < 0.0f)
            BlendPhys = 0.0f;
        if (BlendPhys > 1.0f)
            BlendPhys = 1.0f;

        // Save old positions
        Particles[n]->OldRenderPos = Particles[n]->RenderPos;
        Particles[n]->OldRenderAngle = Particles[n]->RenderAngle;

        Particles[n]->RenderPos.Lerp(TrackPos, Particles[n]->PhysPos, BlendPhys);
        Particles[n]->PhysPos = Particles[n]->RenderPos;

        // Particles[n]->RenderPos    = Particles[n]->PhysPos;
        Particles[n]->RenderAngle = Particles[n]->Angle;
    }

    // waiting for the particles that are attached to our particle ...

    for (uint32_t n = 0; n < Particles.size(); n++)
    {
        if (Particles[n]->AttachedEmitter)
        {
            // core.Trace("%d, %3.2f, %3.2f, %3.2f", n, Particles[n]->RenderPos.x, Particles[n]->RenderPos.y,
            // Particles[n]->RenderPos.z); Particles[n]->AttachedEmitter->SaveTime();
            Particles[n]->AttachedEmitter->Teleport(Matrix(Particles[n]->OldRenderAngle, Particles[n]->OldRenderPos));
            Particles[n]->AttachedEmitter->SetTransform(Matrix(Particles[n]->RenderAngle, Particles[n]->RenderPos));
            Particles[n]->AttachedEmitter->BornParticles(DeltaTime);

            // if (n < Particles.size()-1)  Particles[n]->AttachedEmitter->RestoreTime();
        }
    }

    // RDTSC_E (t);
    // core.Trace("Time - %d", t);
}

uint32_t ModelProcessor::GetCount() const
{
    return Particles.size();
}

void ModelProcessor::DeleteWithGUID(uint32_t dwGUID, uint32_t GUIDRange)
{
    for (uint32_t j = 0; j < Particles.size(); j++)
    {
        if (Particles[j]->EmitterGUID >= dwGUID && Particles[j]->EmitterGUID < dwGUID + GUIDSTEP)
        {
            *(Particles[j]->ActiveCount) = (*(Particles[j]->ActiveCount) - 1);
            FreeParticle(Particles[j]);
            // Particles.ExtractNoShift(j);
            Particles[j] = Particles.back();
            Particles.pop_back();
            j--;
        }
    }
}

// Draws all the particles ...
void ModelProcessor::Draw()
{
    for (uint32_t j = 0; j < Particles.size(); j++)
    {
        const auto pR = Particles[j];

        pMasterManager->Render()->SetTransform(D3DTS_WORLD, Matrix(pR->RenderAngle, pR->RenderPos));
        pR->pScene->Draw(nullptr, 0, nullptr);
    }

    // core.Trace ("PSYS 2.0 : Draw %d model particles", Particles.size());
}

void ModelProcessor::Clear()
{
    for (uint32_t j = 0; j < Particles.size(); j++)
    {
        *(Particles[j]->ActiveCount) = (*(Particles[j]->ActiveCount) - 1);
        FreeParticle(Particles[j]);
    }
    Particles.clear();
}
