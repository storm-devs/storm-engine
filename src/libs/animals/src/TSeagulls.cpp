#include "TSeagulls.h"
#include "Entity.h"
#include "core.h"
#include "defines.h"
#include "model.h"
#include "rands.h"
#include "shared/messages.h"
#include "vfile_service.h"
//#pragma warning (disable : 4244)

//--------------------------------------------------------------------
TSeagulls::TSeagulls() : enabled(true), count(0), frightened(false)
{
    seagulls.resize(SEAGULL_COUNT);
}

//--------------------------------------------------------------------
TSeagulls::~TSeagulls()
{
    EntityManager::EraseEntity(seagullModel);
}

//--------------------------------------------------------------------
void TSeagulls::LoadSettings()
{
    auto ini = fio->OpenIniFile(ANIMALS_INI_FILENAME);
    if (!ini)
    {
        countAdd = SEAGULL_ADD_COUNT;
        maxRadius = SEAGULL_MAX_RADIUS;
        maxAngleSpeed = SEAGULL_MAX_SPEED;
        maxHeight = SEAGULL_MAX_HEIGHT;
        maxCircleTime = SEAGULL_MAX_CIRCLE_TIME;
        farChoiceChance = SEAGULL_LONG_DISTANCE_CHANCE;
        maxDistance = SEAGULL_DISTANCE;
        relaxTime = SEAGULL_RELAX_TIME;
        screamTime = SEAGULL_SCREAM_TIME;
        strcpy_s(screamFilename, ANIMALS_SEAGULLS_SCREAM_FILENAME);

        return;
    }

    maxRadius = ini->GetFloat(ANIMALS_SEAGULLS_SECTION, "radius", SEAGULL_MAX_RADIUS);
    maxAngleSpeed = ini->GetFloat(ANIMALS_SEAGULLS_SECTION, "angle speed", SEAGULL_MAX_SPEED);
    maxDistance = ini->GetFloat(ANIMALS_SEAGULLS_SECTION, "distance", SEAGULL_DISTANCE);
    maxHeight = ini->GetFloat(ANIMALS_SEAGULLS_SECTION, "height", SEAGULL_MAX_HEIGHT);
    maxCircleTime = ini->GetLong(ANIMALS_SEAGULLS_SECTION, "circle time", SEAGULL_MAX_CIRCLE_TIME);
    farChoiceChance = ini->GetLong(ANIMALS_SEAGULLS_SECTION, "far choice", SEAGULL_LONG_DISTANCE_CHANCE);
    relaxTime = ini->GetLong(ANIMALS_SEAGULLS_SECTION, "relax time", SEAGULL_RELAX_TIME);
    screamTime = ini->GetLong(ANIMALS_SEAGULLS_SECTION, "scream time", SEAGULL_SCREAM_TIME);
    countAdd = ini->GetLong(ANIMALS_SEAGULLS_SECTION, "add count", SEAGULL_ADD_COUNT);
    ini->ReadString(ANIMALS_SEAGULLS_SECTION, "scream file", screamFilename, 256, ANIMALS_SEAGULLS_SCREAM_FILENAME);
}

//--------------------------------------------------------------------
void TSeagulls::Init()
{
    startY = 0.0f;
    LoadSettings();

    renderService = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    soundService = static_cast<VSoundService *>(core.CreateService("SoundService"));

    if (!renderService)
    {
        throw std::runtime_error("!Seagulls: No service: dx9render");
    }        
    // if(!soundService)
    //    throw std::runtime_error("!Seagulls: No service: sound");

    seagullModel = EntityManager::CreateEntity("MODELR");
    core.Send_Message(seagullModel, "ls", MSG_MODEL_LOAD_GEO, ANIMALS_SEAGULL_FILENAME);
}

//--------------------------------------------------------------------
uint64_t TSeagulls::ProcessMessage(const long _code, MESSAGE &message)
{
    switch (_code)
    {
    case MSG_ANIMALS_SEAGULLS_SHOW:
        enabled = true;
        break;

    case MSG_ANIMALS_SEAGULLS_HIDE:
        enabled = false;
        count = 0;
        break;

    case MSG_ANIMALS_SEAGULLS_FRIGHTEN:
        Frighten();
        break;

    case MSG_ANIMALS_SEAGULLS_ADD:
        Add(message.Float(), message.Float(), message.Float());
        break;
    }

    return 0;
}

//--------------------------------------------------------------------
void TSeagulls::Execute(const uint32_t _dTime)
{
    if (!enabled)
    {
        return;
    }      

    // <relax>
    if (frightened)
    {
        frightenTime -= _dTime;
        if (frightenTime <= 0)
        {
            frightened = false;
            screamTime <<= 1;
            for (auto& seagull : seagulls)
            {
                seagull.va /= 2.0f;
                // seagulls[i].circleTimePassed += _dTime;
            }

        }
    }

    // <all_movements>
    for (auto& seagull : seagulls)
    {
        // Scream
        if (seagull.screamTime > 0)
        {
            seagull.screamTime -= _dTime;
        }            
        else
        {
            seagull.screamTime = (rand() % (screamTime >> 1)) + (screamTime >> 1);
            CVECTOR pos(static_cast<float>(seagull.center.x + sin(seagull.a) * seagull.radius),
                        static_cast<float>(seagull.center.z + cos(seagull.a) * seagull.radius),
                        static_cast<float>(seagull.height));
            // if(soundService) soundService->SoundPlay(screamFilename, PCM_3D, VOLUME_FX, false, false, true, 0, &pos);
        }

        // <angle_inc>
        seagull.a = fmodf(seagull.a + (seagull.va / 1000000.0f) * _dTime, 2 * PI);

        // <new_circle>
        if (seagull.circleTimePassed < seagull.circleTime)
        {
            seagull.circleTimePassed += _dTime;
        }            
        else
        {
            const auto oldR = seagull.radius;
            seagull.radius = SEAGULL_MIN_RADIUS + rand(maxRadius);
            seagull.circleTimePassed = 0;
            seagull.circleTime = static_cast<long>(rand(static_cast<float>(maxCircleTime)));
            if ((seagull.circleTime) < (maxCircleTime / 20))
            {
                seagull.circleTime = maxCircleTime / 20;
            }               
            const auto sinA = sinf(seagull.a);
            const auto cosA = cosf(seagull.a);
            const auto newX1 = seagull.center.x + sinA * (seagull.radius + oldR);
            const auto newZ1 = seagull.center.z + cosA * (seagull.radius + oldR);
            const auto newX2 = seagull.center.x + sinA * (oldR - seagull.radius);
            const auto newZ2 = seagull.center.z + cosA * (oldR - seagull.radius);
            const auto distance1 = fabsf(cameraPos.x - newX1) + fabsf(cameraPos.z - newZ1);
            const auto distance2 = fabsf(cameraPos.x - newX2) + fabsf(cameraPos.z - newZ2);

            seagull.va *= oldR / seagull.radius;
            const auto deltaVa = randCentered(maxAngleSpeed / 5.0f);

            if (((seagull.va + deltaVa) * seagull.va) < 0)
            {
                seagull.va -= deltaVa;
            }
            else
            {
                seagull.va += deltaVa;
            }
                
            const auto minRadius = maxRadius * maxAngleSpeed / seagull.radius;
            if (fabs(seagull.va) < (minRadius / 2.0f))
            {
                if (seagull.va > 0.0f)
                {
                    seagull.va = minRadius / 2.0f;
                }
                else
                {
                    seagull.va = -minRadius / 2.0f;
                }
            }

            // seagulls[i].height += randCentered(maxHeight / 50.0f);
            if ((distance1 < distance2) && ((rand() % farChoiceChance) == 1))
            {
                seagull.center.x = newX1;
                seagull.center.z = newZ1;
                seagull.va = -seagull.va;
                seagull.a = fmodf(seagull.a + PI, 2 * PI);
            }
            else
            {
                seagull.center.x = newX2;
                seagull.center.z = newZ2;
            }
        }
    }
}

//--------------------------------------------------------------------
void TSeagulls::Realize(const uint32_t _dTime)
{
    if (!enabled)
    {
        return;
    }

    float persp;
    renderService->GetCamera(cameraPos, cameraAng, persp);
    if (!count)
    {
        Add(cameraPos.x, cameraPos.y, cameraPos.z);
    }

    auto* seagullPtr = static_cast<MODEL*>(EntityManager::GetEntityPointer(seagullModel));
    if (!seagullPtr)
    {
        return;
    }

    for (auto& seagull : seagulls)
    {
        CVECTOR ang, pos;
        ang.x = 0.0f;
        ang.z = 0.0f;
        if (seagull.va > 0.0f)
        {
            ang.y = seagull.a + PI / 2;
        }
        else
        {       
            ang.y = seagull.a - PI / 2;
        }
        pos.x = seagull.center.x + sinf(seagull.a) * seagull.radius;
        pos.z = seagull.center.z + cosf(seagull.a) * seagull.radius;
        pos.y = seagull.height;
        seagullPtr->mtx = CMatrix(ang, pos);
        seagullPtr->ProcessStage(Entity::Stage::realize, _dTime);
    }
}

//--------------------------------------------------------------------
void TSeagulls::Frighten()
{
    if (frightened)
    {
        return;
    }

    frightened = true;
    frightenTime = relaxTime;

    for (auto& seagull : seagulls)
    {
        seagull.va *= 2.0f;
        seagull.screamTime >>= 2;
    }
    screamTime >>= 1;
}

//--------------------------------------------------------------------
void TSeagulls::Add(const float _x, const float _y, const float _z)
{
    float op;

    for (auto& seagull : seagulls)
    {
        seagull.center = CVECTOR(randCentered(maxDistance) + _x, rand(maxHeight) + SEAGULL_MIN_HEIGHT,
                                 randCentered(maxDistance) + _z);
        seagull.radius = randUpper(maxRadius);
        op = randUpper(maxAngleSpeed);
        if (rand() & 0x1)
        {
            seagull.va = op;
        }
        else
        {
            seagull.va = -op;
        }
        seagull.height = startY + seagull.center.y;
        seagull.a = rand(2 * PI);
        seagull.circleTime = rand() % maxCircleTime;
        seagull.screamTime = (rand() % (screamTime >> 3)) << 3;
    }
}

//--------------------------------------------------------------------
