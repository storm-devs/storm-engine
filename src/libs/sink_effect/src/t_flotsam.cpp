#include "t_flotsam.h"
#include "entity.h"
#include "core.h"
#include "math_inlines.h"
#include "rands.h"
#include "shared/messages.h"

int TFlotsam::modelsInitialized = 0;
MODEL *TFlotsam::models[sink_effect::FLOTSAM_MODELS_COUNT];
char TFlotsam::modelNames[sink_effect::FLOTSAM_MODELS_COUNT][128];
entid_t TFlotsam::modelIDs[sink_effect::FLOTSAM_MODELS_COUNT];

//--------------------------------------------------------------------
TFlotsam::TFlotsam() : sea(nullptr), enabled(false)
{
}

//--------------------------------------------------------------------
TFlotsam::~TFlotsam()
{
    --modelsInitialized;

    if (!modelsInitialized)
        for (auto i = 0; i < sink_effect::FLOTSAM_MODELS_COUNT; i++)
            core.EraseEntity(modelIDs[i]);
}

//--------------------------------------------------------------------
void TFlotsam::Start(float _x, float _z, float _radius)
{
    enabled = true;
    floatTime = 0;
    x = _x + randCentered(_radius);
    z = _z + randCentered(_radius);
    y = sea->WaveXZ(x, z) - sink_effect::FLOTSAM_START_DELTAY;
    vX = randCentered(1.0f) * sink_effect::FLOTSAM_H_SPEED;
    vY = randUpper(1.0f) * sink_effect::FLOTSAM_V_SPEED;
    vZ = randCentered(1.0f) * sink_effect::FLOTSAM_H_SPEED;
    angY = rand(PIm2);
    turnY = randCentered(1.0f) * sink_effect::FLOTSAM_TURN_SPEED;
    maxFloatTime = static_cast<uint32_t>(randUpper(sink_effect::FLOTSAM_FLOAT_TIME));
    state = FLOTSAM_RISE;
    // model = models[rand() % FLOTSAM_MODELS_COUNT];
    ModelID = modelIDs[rand() % sink_effect::FLOTSAM_MODELS_COUNT];
}

//--------------------------------------------------------------------
void TFlotsam::Initialize(SEA_BASE *_sea)
{
    if (!modelsInitialized)
    {
        // modelsInitialized = true;
        strcpy_s(modelNames[0], "particles\\palka01");
        strcpy_s(modelNames[1], "particles\\palka02");
        strcpy_s(modelNames[2], "particles\\palka03");
        strcpy_s(modelNames[3], "particles\\palka04");

        for (auto i = 0; i < sink_effect::FLOTSAM_MODELS_COUNT; i++)
        {
            modelIDs[i] = core.CreateEntity("MODELR");
            core.Send_Message(modelIDs[i], "ls", MSG_MODEL_LOAD_GEO, modelNames[i]);
            models[i] = static_cast<MODEL *>(core.GetEntityPointer(modelIDs[i]));
        }
    }

    ++modelsInitialized;
    sea = _sea;
}

//--------------------------------------------------------------------
void TFlotsam::Process(uint32_t _dTime)
{
    if (!enabled)
        return;

    const auto dTime = _dTime / 1000.0f; // msec -> sec

    switch (state)
    {
    case FLOTSAM_RISE:
        x += dTime * vX;
        z += dTime * vZ;
        y += dTime * vY;
        if (y > (sea->WaveXZ(x, z) + sink_effect::FLOTSAM_DY))
            state = FLOTSAM_FLOAT;
        break;
    case FLOTSAM_FLOAT:
        x += dTime * vX;
        z += dTime * vZ;
        floatTime += _dTime;
        angY = static_cast<float>(fmod(angY + dTime * turnY, PIm2));
        if (floatTime > maxFloatTime)
            state = FLOTSAM_SINK;
        break;
    case FLOTSAM_SINK:
        x += dTime * vX;
        z += dTime * vZ;
        y -= dTime * vY;
        if (y <= sea->WaveXZ(x, z) - sink_effect::FLOTSAM_START_DELTAY)
            enabled = false;
        break;
    }
}

//--------------------------------------------------------------------
void TFlotsam::Realize(uint32_t _dTime)
{
    if (!enabled)
        return;

    CVECTOR pos(x, 0, z);
    if (state == FLOTSAM_FLOAT)
        pos.y = sea->WaveXZ(x, z) + sink_effect::FLOTSAM_DY;
    else
        pos.y = y;
    const CVECTOR ang(0.0f, angY, 0.0f);
    MODEL *model;
    model = static_cast<MODEL *>(core.GetEntityPointer(ModelID));
    if (model)
    {
        model->mtx.BuildMatrix(ang, pos);
        model->ProcessStage(Entity::Stage::realize, _dTime);
    }
}

//--------------------------------------------------------------------
bool TFlotsam::Enabled()
{
    return enabled;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
