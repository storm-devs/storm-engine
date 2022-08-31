//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Sharks
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "sharks.h"
#include "entity.h"
#include "core.h"
#include "geometry.h"
#include "string_compare.hpp"
#include "shared/messages.h"

#include <SDL_timer.h>

CREATE_CLASS(Sharks)

//============================================================================================
// Fish
//============================================================================================

#define SHARK_PI 3.14159265f
#define SHARK_REPULSION 80.0f // Coefficient of shark repulsion
#define SHARK_REPULSION_DIST 90.0f // Distance of shark repulsion
#define SHARK_MAX_RSPEED 8.1f // Maximum speed of movement of the follow point
#define SHARK_PULL_DIST 100.0f // Pull distance
#define SHARK_REPPUL_DIST 10.0f // Repulsion distance
#define SHARK_MAX_Y 0.01f // Upper swimming limit
#define SHARK_MIN_Y -20.0f // Lower swimming limit
#define SHARK_MAX_TURN 0.2f // Maximum turn speed
#define SHARK_KROW_TURN 2.0f // Tilt factor when turning max = turn * SHARK_MAX_TURN
#define SHARK_IMSPD_ACC 0.1f // Dive acceleration
#define SHARK_MAX_IMSPD 1.0f // Maximum dive speed
#define SHARK_KAX_IMSPD 0.3f // Angle versus dive speed
#define SHARK_MAX_SPEED 8.0f // Shark top speed
#define SHARK_MIN_SPEED 5.0f // Minimum shark speed

//============================================================================================
// Shark
//============================================================================================

uint16_t Sharks::Shark::indeces[] = {
    0, 1, 2, 0, 2, 3, 1, 4, 2, 2, 4, 5, 2, 5, 6, 2, 6, 3, 4, 7, 8, 4, 8, 5, 5, 8, 6, 6, 8, 9,
};

Sharks::Shark::Shark() : fforce(), spos(), angs(), vBase(0), model(0)
{
    pos = 0.0f;
    vel = 0.0f;
    force = 0.0f;
    yDir = 0.0f;
    dirTime = 0.0f;
    turn = 0.0f;
    imspd = 0.0f;
    speed = (SHARK_MAX_SPEED + SHARK_MIN_SPEED) * 0.5f;
    aniTime = 0.0f;
    jumpTime = 10.0f + rand() * 30.0f / RAND_MAX;
    accs = 0.007f + rand() * 0.008f / RAND_MAX;
    rForce = 0.0f;
    rTime = 0.0f;
    shipY = 0.0f;
    speedUp = false;
}

Sharks::Shark::~Shark()
{
    core.EraseEntity(model);
}

bool Sharks::Shark::Init(float vp_x, float vp_z, bool isLoadModel)
{
    // Position
    const auto radius = SHARK_PULL_DIST * (0.4f + rand() * 1.6f / RAND_MAX);
    const auto ang = SHARK_PI * rand() * (2.0f / RAND_MAX);
    pos.x = vp_x + radius * cosf(ang);
    pos.z = vp_z + radius * sinf(ang);
    pos.y = SHARK_MIN_Y + (SHARK_MAX_Y - SHARK_MIN_Y) * rand() * 1.0f / RAND_MAX;
    spos = pos;
    angs.y = SHARK_PI * rand() * (2.0f / RAND_MAX);
    if (!isLoadModel)
        return true;
    // Loading the model
    if (!(model = core.CreateEntity("modelr")))
        return false;
    // Path to textures
    auto *gs = static_cast<VGEOMETRY *>(core.GetService("geometry"));
    if (!gs)
    {
        core.Trace("Can't create geometry service!");
        return false;
    }
    gs->SetTexturePath("Animals\\");
    if (!core.Send_Message(model, "ls", MSG_MODEL_LOAD_GEO, "Animals\\shark"))
    {
        gs->SetTexturePath("");
        core.Trace("Shark model 'shark' not loaded");
        core.EraseEntity(model);
        return false;
    }
    gs->SetTexturePath("");
    if (!core.Send_Message(model, "ls", MSG_MODEL_LOAD_ANI, "shark"))
    {
        core.Trace("Shark animation 'shark' not loaded");
        core.EraseEntity(model);
        return false;
    }
    // Set the default animation
    auto *mdl = static_cast<MODEL *>(core.GetEntityPointer(model));
    if (!mdl || !mdl->GetAnimation())
        return false;
    mdl->GetAnimation()->SetEvent(ae_end, 0, this);
    mdl->GetAnimation()->Player(0).SetAction("stand");
    mdl->GetAnimation()->Player(0).Play();
    auto *node = mdl->GetNode(0);
    if (!node)
        return false;
    node->SetTechnique("Shark");
    return true;
}

inline void Sharks::Shark::Reset(float cam_x, float cam_z)
{
    force = 0.0f;
    fforce = 0.0f;
    // If further away from the camera than allowed, then teleport
    const auto dx = cam_x - pos.x;
    const auto dz = cam_z - pos.z;
    const auto max = SHARK_PULL_DIST * 4.0f;
    if (dx * dx + dz * dz > max * max)
    {
        Init(cam_x, cam_z, false);
        pos.y = SHARK_MIN_Y * 4.0f;
    }
    shipY = SHARK_MAX_Y;
}

inline void Sharks::Shark::Repulsion(Shark &shr)
{
    auto v = pos - shr.pos;
    auto k = ~v;
    if (k > 0.0f && k < SHARK_REPULSION_DIST * SHARK_REPULSION_DIST)
    {
        v *= SHARK_REPULSION / k;
        force += v;
        shr.force -= v;
    }
    v = spos - shr.spos;
    k = ~v;
    if (k > 0.0f && k < 15.0f * 15.0f)
    {
        v *= 5.0f / k;
        fforce += v;
        shr.fforce -= v;
    }
}

inline void Sharks::Shark::ShipApply(float x, float z, float r2)
{
    const auto dx = x - spos.x;
    const auto dz = z - spos.z;
    const auto d = dx * dx + dz * dz;
    auto fy = 0.0f;
    if (d < r2)
    {
        const auto k = sqrtf(d / r2);
        fy = -(1.0f - k) * 100.0f;
        shipY = SHARK_MIN_Y + (SHARK_MAX_Y - SHARK_MIN_Y) * k;
    }
    if (dx * vel.x + dz * vel.z < 0.0f)
        fy *= 0.5f;
    force.y += fy;
    fforce.y += fy;
}

inline void Sharks::Shark::Coordination(float cam_x, float cam_z, float dltTime, SEA_BASE *sb, ISLAND_BASE *ib)
{
    // get a model
    auto *mdl = static_cast<MODEL *>(core.GetEntityPointer(model));
    if (!mdl)
        return;
    // Repulsion force
    auto l = ~force;
    if (l > 1600.0f)
        force *= 40.0f / sqrtf(l);
    // Random force
    rTime -= dltTime;
    if (rTime < 0.0f)
    {
        const auto radius = rand() * 10.0f / RAND_MAX;
        const auto ang = SHARK_PI * rand() * (2.0f / RAND_MAX);
        rForce.x = radius * sinf(ang);
        rForce.z = radius * cosf(ang);
        rTime = 1.0f + rand() * 10.0f;
    }
    force += rForce;
    // Direction to camera
    auto vx = cam_x - pos.x;
    auto vz = cam_z - pos.z;
    const auto k = vx * vx + vz * vz;
    // If closer than, then repulse the point
    if (k < SHARK_REPPUL_DIST * SHARK_REPPUL_DIST)
    {
        force.x -= vx * 1.0f / SHARK_REPPUL_DIST;
        force.z -= vz * 1.0f / SHARK_REPPUL_DIST;
    }
    else
    {
        // Pulling towards the camera
        vx *= 0.1f;
        vz *= 0.1f;
        l = sqrtf(k) * 0.1f;
        if (l > 20.0f * 20.0f)
        {
            l = 20.0f / l;
            vx *= l;
            vz *= l;
        }
        force.x += vx;
        force.z += vz;
    }
    // Moving vertically
    force.y += 0.4f * (yDir - pos.y);
    // Noise
    force.x += (0.5f - (rand() & 15) * 1.0f / 15.0f) * 0.001f;
    force.y += (0.5f - (rand() & 15) * 1.0f / 15.0f) * 0.001f;
    force.z += (0.5f - (rand() & 15) * 1.0f / 15.0f) * 0.001f;
    // Move speed
    vel += accs * force * dltTime;
    l = ~vel;
    if (l > SHARK_MAX_RSPEED * SHARK_MAX_RSPEED)
        vel *= SHARK_MAX_RSPEED / sqrtf(l);
    // Position
    pos += vel * dltTime;
    if (pos.y > 0.1f)
        pos.y = 0.1f;
    if (pos.y < -20.0f)
        pos.y = -20.0f;
    // Swimming altitude trend
    dirTime -= dltTime;
    if (dirTime <= 0.0f)
    {
        const auto p = rand() * 1.0f / RAND_MAX;
        yDir = SHARK_MIN_Y + (SHARK_MAX_Y - SHARK_MIN_Y) * (1.0f - p * p);
        dirTime = 5.0f + 10.0f * rand() * 1.0f / RAND_MAX;
    }
    // Fish
    // Force of gravity
    auto fff = pos - spos;
    l = ~fff;
    if (l > 1.0f)
        fff *= 1.0f / sqrtf(l);
    // The power of repulsion
    l = ~fforce;
    if (l > 100.0f)
        fforce *= 10.0f / sqrtf(l);
    fforce += fff;
    // Determine the collision relative to the island
    if (ib)
    {
        IslandCollision(ib, 32, 5.0f, 100.0f);
        IslandCollision(ib, 16, 30.0f, 50.0f);
        IslandCollision(ib, 8, 50.0f, 20.0f);
    }
    // Determine the direction of the fish relative to the follow point
    const auto sx = sinf(angs.y);
    const auto sz = cosf(angs.y);
    vx = fforce.x;
    vz = fforce.z;
    l = vx * vx + vz * vz;
    if (l > 0.0f)
    {
        l = 1.0f / sqrtf(l);
        vx *= l;
        vz *= l;
    }
    else
    {
        vx = sx;
        vz = sz;
    }
    const auto sn = sx * vz - sz * vx;
    const auto cs = sx * vx + sz * vz;
    turn -= 0.4f * sn * dltTime;
    if (turn > SHARK_MAX_TURN)
        turn = SHARK_MAX_TURN;
    if (turn < -SHARK_MAX_TURN)
        turn = -SHARK_MAX_TURN;
    angs.y += turn * dltTime;
    // Tilt angle when turning
    angs.z = -turn * SHARK_KROW_TURN;
    // Speed
    speed += 0.3f * cs * dltTime;
    speedUp = cs > 0.0f;
    if (speed > SHARK_MAX_SPEED)
        speed = SHARK_MAX_SPEED;
    if (speed < SHARK_MIN_SPEED)
        speed = SHARK_MIN_SPEED;
    // Depth change
    if (fforce.y < 0.0f)
    {
        if (spos.y > SHARK_MIN_Y)
        {
            imspd -= SHARK_IMSPD_ACC * dltTime;
            if (imspd < -SHARK_MAX_IMSPD)
                imspd = -SHARK_MAX_IMSPD;
        }
        else
        {
            imspd += SHARK_IMSPD_ACC * dltTime;
        }
    }
    else
    {
        imspd += SHARK_IMSPD_ACC * dltTime;
        if (imspd > SHARK_MAX_IMSPD)
            imspd = SHARK_MAX_IMSPD;
    }
    // Dive angle
    angs.x = -imspd * SHARK_KAX_IMSPD;
    // New position
    Assert(speed >= SHARK_MIN_SPEED);
    spos.x += speed * sinf(angs.y) * dltTime;
    spos.y += imspd * dltTime;
    spos.z += speed * cosf(angs.y) * dltTime;
    auto rpos = spos;
    if (sb)
    {
        CVECTOR n;
        const auto seaY = sb->WaveXZ(spos.x, spos.z, &n);
        if (rpos.y > seaY)
            rpos.y = seaY;
    }
    if (rpos.y > shipY)
        rpos.y = shipY;
    // Tilt limit by depth
    if (rpos.y > -3.0f)
    {
        l = rpos.y;
        if (l > 0.0f)
            l = 0.0f;
        l = 1.0f - (3.0f + l) / 3.0f;
        angs.x *= l * l;
    }
    // Animation
    aniTime -= dltTime;
    jumpTime -= dltTime;
    // Track
    vBase += 0.2f * speed * dltTime;
    if (vBase > 1.0f)
        vBase -= 1.0f;
    // Matrix update
    mdl->mtx.BuildMatrix(angs, rpos);
    /*
    if(core.Controls->GetAsyncKeyState('Z') >= 0)
    {
      if(core.Controls->GetAsyncKeyState('X') < 0) rpos.y += 30.0f;
      mdl->mtx.BuildMatrix(angs, rpos);
    }else{
      mdl->mtx.BuildMatrix(angs, pos + CVECTOR(0.0f, 30.0f, 0.0f));
    }
    //*/
}

inline void Sharks::Shark::IslandCollision(ISLAND_BASE *ib, int32_t numPnt, float rad, float frc)
{
    const auto step = 2.0f * SHARK_PI / numPnt;
    auto vx = 0.0f;
    auto vz = 0.0f;
    auto *mdl = static_cast<MODEL *>(core.GetEntityPointer(ib->GetSeabedEID()));
    if (!mdl)
        return;
    for (int32_t i = 0; i < numPnt; i++)
    {
        const auto x = sinf(i * step);
        const auto z = cosf(i * step);
        const auto xp = spos.x + rad * x;
        const auto zp = spos.z + rad * z;
        auto h = mdl->Trace(CVECTOR(xp, 100.0f, zp), CVECTOR(xp, -50.0f, zp));
        if (h < 1.0f)
        {
            h -= 100.0f / 150.0f;
            if (h > 0.0f)
            {
                h *= 150.0f / 50.0f;
            }
            else
                h = 0.0f;
            vx -= x * (1.0f - h);
            vz -= z * (1.0f - h);
        }
    }
    rad = vx * vx + vz * vz;
    if (rad > 1.0f)
    {
        rad = 1.0f / sqrtf(rad);
        vx *= rad;
        vz *= rad;
    }
    fforce.x += vx * frc;
    fforce.z += vz * frc;
}

void Sharks::Shark::Event(Animation *animation, int32_t index, int32_t eventID, AnimationEvent event)
{
    if (aniTime > 0.0f)
        return;
    static const auto *actStand = "stand";
    static const auto *actSwim = "Shark_Swim";
    static const auto *actJump = "Shark_Jump";
    const auto *const act = animation->Player(0).GetAction();
    const int32_t rnd = rand();
    animation->Player(0).Stop();
    if (angs.x > 0.0f && (rnd & 1) || speedUp)
    {
        animation->Player(0).SetAction(actSwim);
        aniTime = 3.0f + rand() * 3.0f / RAND_MAX;
    }
    if (storm::iEquals(act, actSwim) || (rnd & 1))
    {
        animation->Player(0).SetAction(actStand);
        aniTime = 3.0f + rand() * 3.0f / RAND_MAX;
    }
    else
    {
        if (jumpTime <= 0.0f && spos.y > -0.1f && ~(rnd & 0xf0)) //~!~
        {
            // jump out of the water
            animation->Player(0).SetAction(actJump);
            jumpTime = 60.0f + rand() * 100.0f / RAND_MAX;
            aniTime = -0.1f;
        }
        else
        {
            animation->Player(0).SetAction(actSwim);
            aniTime = 2.0f + rand() * 3.0f / RAND_MAX;
        }
    }
    animation->Player(0).Play();
}

int32_t Sharks::Shark::GenerateTrack(uint16_t *inds, Vertex *vrt, uint16_t base, SEA_BASE *sb)
{
    // get a model
    auto *mdl = static_cast<MODEL *>(core.GetEntityPointer(model));
    if (!mdl)
        return 0;
    auto k = mdl->mtx.Pos().y;
    if (k <= -1.2f)
        return 0;
    if (k > 0.0f)
        k = 0.0f;
    k = (1.2f + k) / 1.2f;
    // Fin parameters
    const auto length = 2.0f * 3.0f * k;
    const auto width = 1.0f * 1.5f * k;
    // Indexes
    Assert(sizeof(indeces) / sizeof(uint16_t) == 30);
    for (int32_t i = 0; i < 30; i++)
        inds[i] = indeces[i] + base;
    // Vertices
    const CVECTOR s(0.0f, 0.0f, 0.75f);
    vrt[0].pos = s;
    vrt[7].pos = s + CVECTOR(-width, 0.0f, -length);
    vrt[9].pos = s + CVECTOR(width, 0.0f, -length);
    vrt[8].pos = (vrt[7].pos + vrt[9].pos) * 0.5f;
    vrt[1].pos = s + (vrt[7].pos - s) * 0.333f;
    vrt[2].pos = s + (vrt[8].pos - s) * 0.333f;
    vrt[3].pos = s + (vrt[9].pos - s) * 0.333f;
    vrt[4].pos = s + (vrt[7].pos - s) * 0.666f;
    vrt[5].pos = s + (vrt[8].pos - s) * 0.666f;
    vrt[6].pos = s + (vrt[9].pos - s) * 0.666f;
    vrt[0].u = 0.5f;
    vrt[0].v = 0.0f;
    vrt[1].u = 0.0f;
    vrt[1].v = 0.333f;
    vrt[2].u = 0.5f;
    vrt[2].v = 0.333f;
    vrt[3].u = 1.0f;
    vrt[3].v = 0.333f;
    vrt[4].u = 0.0f;
    vrt[4].v = 0.666f;
    vrt[5].u = 0.5f;
    vrt[5].v = 0.666f;
    vrt[6].u = 1.0f;
    vrt[6].v = 0.666f;
    vrt[7].u = 0.0f;
    vrt[7].v = 1.0f;
    vrt[8].u = 0.5f;
    vrt[8].v = 1.0f;
    vrt[9].u = 1.0f;
    vrt[9].v = 1.0f;
    for (int32_t i = 0; i < 10; i++)
    {
        vrt[i].pos = mdl->mtx * CVECTOR(vrt[i].pos);
        vrt[i].pos.y = sb->WaveXZ(vrt[i].pos.x, vrt[i].pos.z) + 0.001f;
        vrt[i].color = 0x4fffffff;
        vrt[i].v -= vBase;
    }
    vrt[7].color = 0;
    vrt[8].color = 0;
    vrt[9].color = 0;
    return 10;
}

// ============================================================================================
// Construction, destruction
// ============================================================================================

Sharks::Sharks() : sea(0), island(0), indeces{}, vrt{}
{
    rs = nullptr;
    camPos = 0.0f;
    numShakes = 3 + (SDL_GetTicks() & 3);
    trackTx = -1;
    periscope.time = -1.0;
    waitPTime = -1.0f;
}

Sharks::~Sharks()
{
    core.EraseEntity(periscope.model);
    if (rs)
        rs->TextureRelease(trackTx);
}

// Initialization
bool Sharks::Init()
{
    rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    if (!rs)
        throw std::runtime_error("No service: dx9render");
    for (int32_t i = 0; i < numShakes; i++)
        if (!shark[i].Init(0.0f, 0.0f))
            return false;
    // Execution layers
    const int32_t emdl = AttributesPointer->GetAttributeAsDword("executeModels", 77);
    const int32_t rmdl = AttributesPointer->GetAttributeAsDword("realizeModels", 77);
    const int32_t eprt = AttributesPointer->GetAttributeAsDword("executeParticles", 77);
    const int32_t rprt = AttributesPointer->GetAttributeAsDword("realizeParticles", 100000);
    // Set the execution layers
    core.AddToLayer(SEA_EXECUTE, GetId(), eprt);
    core.AddToLayer(SEA_REALIZE, GetId(), rprt);
    for (int32_t i = 0; i < numShakes; i++)
    {
        core.AddToLayer(SEA_EXECUTE, shark[i].model, emdl);
        core.AddToLayer(SEA_REALIZE, shark[i].model, rmdl);
    }
    // Load the texture
    trackTx = rs->TextureCreate("Animals\\SharkTrack.tga");
    // Analyzing the possibility of creating a periscope
    auto *v = static_cast<VDATA *>(core.GetScriptVariable("Environment"));
    if (v)
    {
        auto *root = v->GetAClass();
        if (root)
        {
            const auto time = root->GetAttributeAsFloat("time");
            if (time > 9.0f && time < 20.0f)
            {
                root = root->FindAClass(root, "date");
                if (root)
                {
                    const auto year = root->GetAttributeAsFloat("year");
                    if (year >= 1633.0f)
                    {
                        const auto month = root->GetAttributeAsDword("month");
                        if (month & 1)
                        {
                            const auto day = root->GetAttributeAsDword("day");
                            if (day == 7)
                            {
                                if ((SDL_GetTicks() & 7) == 5)
                                {
                                    waitPTime = 60.0f + rand() * 500.0f / RAND_MAX;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

// Execution
void Sharks::Execute(uint32_t delta_time)
{
    CVECTOR a;
    if (delta_time & 1)
        rand();
    rs->GetCamera(camPos, a, a.x);
    const auto dltTime = delta_time * 0.001f;
    const auto num = numShakes;
    // Reset the states
    for (int32_t i = 0; i < num; i++)
        shark[i].Reset(camPos.x, camPos.z);
    // calculate the forces
    for (int32_t i = 0; i < num - 1; i++)
        for (auto j = i + 1; j < num; j++)
            shark[i].Repulsion(shark[j]);
    // take into account ships

    auto &&entities = core.GetEntityIds("ship");
    for (auto ent : entities)
    {
        // Object pointer
        auto *ship = static_cast<VAI_OBJBASE *>(core.GetEntityPointer(ent));
        if (!ship)
            break;
        // Ship position
        const auto shipPos = ship->GetMatrix()->Pos();
        // Ship size
        const auto s = ship->GetBoxsize();
        const auto rd2 = (s.x * s.x + s.z * s.z) * 3.0f;
        // Tell sharks about ships
        for (int32_t i = 0; i < num; i++)
            shark[i].ShipApply(shipPos.x, shipPos.z, rd2);
    }
    // The sea
    auto *sb = static_cast<SEA_BASE *>(core.GetEntityPointer(sea));
    if (!sb)
    {
        sea = core.GetEntityId("sea");
        sb = static_cast<SEA_BASE *>(core.GetEntityPointer(sea));
        if (!sb)
            return;
    }
    auto *ib = static_cast<ISLAND_BASE *>(core.GetEntityPointer(island));
    if (!ib)
    {
        island = core.GetEntityId("island");
        ib = static_cast<ISLAND_BASE *>(core.GetEntityPointer(island));
        if (!ib)
            return;
    }
    // Calculating new positions
    for (int32_t i = 0; i < num; i++)
        shark[i].Coordination(camPos.x, camPos.z, dltTime, sb, ib);
    // Processing the periscope
    if (!ib)
    {
        if (periscope.time >= 0.0f)
        {
            periscope.time -= dltTime;
            if (periscope.time < 10.0f)
            {
                periscope.pos.y -= dltTime;
            }
            else
            {
                periscope.pos.y += 2.0f * dltTime;
                if (periscope.pos.y > 0.0f)
                    periscope.pos.y = 0.0f;
            }
            auto *mdl = static_cast<MODEL *>(core.GetEntityPointer(periscope.model));
            if (mdl)
            {
                mdl->mtx.BuildMatrix(CVECTOR(0.0f, periscope.ay, 0.0f), periscope.pos + CVECTOR(0.0f, 1.0f, 0.0f));
                periscope.pos.x += mdl->mtx.Vz().x * dltTime * 5.0f;
                periscope.pos.z += mdl->mtx.Vz().z * dltTime * 5.0f;
            }
            if (periscope.time < 0.0f)
            {
                periscope.time = -1.0f;
                core.EraseEntity(periscope.model);
            }
        }
        else
        {
            if (waitPTime > 0.0f)
            {
                waitPTime -= dltTime;
                if (waitPTime <= 0.0f)
                {
                    periscope.pos = 0.0f;
                    periscope.ay = rand() * (6.14f / RAND_MAX);
                    if (LoadPeriscopeModel())
                    {
                        periscope.time = 30.0f + rand() * (20.0f / RAND_MAX);
                        CVECTOR dummy;
                        rs->GetCamera(periscope.pos, dummy, periscope.pos.y);
                    }
                    periscope.pos.y = -10.0f;
                    waitPTime = -1.0f;
                }
            }
        }
    }
}

bool Sharks::LoadPeriscopeModel()
{
    if (!(periscope.model = core.CreateEntity("modelr")))
        return false;
    auto *gs = static_cast<VGEOMETRY *>(core.GetService("geometry"));
    if (!gs)
        return false;
    gs->SetTexturePath("Animals\\");
    if (!core.Send_Message(periscope.model, "ls", MSG_MODEL_LOAD_GEO, "Animals\\periscope"))
    {
        gs->SetTexturePath("");
        core.EraseEntity(periscope.model);
        return false;
    }
    gs->SetTexturePath("");
    auto *mdl = static_cast<MODEL *>(core.GetEntityPointer(periscope.model));
    if (!mdl)
    {
        core.EraseEntity(periscope.model);
        return false;
    }
    core.AddToLayer(SEA_REALIZE, periscope.model, 10);
    return true;
}

void Sharks::Realize(uint32_t delta_time)
{
    auto *sb = static_cast<SEA_BASE *>(core.GetEntityPointer(sea));
    if (!sb)
        return;
    int32_t num = 0;
    for (int32_t i = 0; i < numShakes; i++)
    {
        num += shark[i].GenerateTrack(indeces + num * 3, vrt + num, static_cast<uint16_t>(num), sb);
    }
    if (num)
    {
        rs->TextureSet(0, trackTx);
        rs->SetTransform(D3DTS_WORLD, CMatrix());
        rs->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
        rs->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, num, num, indeces, D3DFMT_INDEX16, vrt, sizeof(Vertex),
                                   "SharkTrack");
    }

    /*
    ISLAND_BASE * ib = (ISLAND_BASE *)core.GetEntityPointer(island);
    if(!ib) return;
    float maxRad = 0.0f;
    int32_t s = 30;
    for(int32_t i = 0; i < numShakes; i++)
    {
      float r = sqrtf(~(shark[i].spos - shark[i].pos));
      if(r > maxRad) maxRad = r;
      if(ib)
      {
        float h = -1000.0f;
        ib->GetDepth(shark[i].spos.x, shark[i].spos.z, &h);
        rs->Print(10, s, "Height = %f", h);
        s += 20;
      }
    }
    rs->Print(10, 10, "MaxRad = %f", maxRad);
    */
}
