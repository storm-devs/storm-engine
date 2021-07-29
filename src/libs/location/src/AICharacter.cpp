//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    AICharacter
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "AICharacter.h"

#include "core.h"

#include "Location.h"

#define FALURE_GOTO 2.0f

// ============================================================================================
// Construction, destruction
// ============================================================================================

AICharacter::AICharacter()
    : command(), path{}, force(), goForce(), separation(), alignment(), around()
{
    currentNode = -1;
    likeKSpd = 0.9f + rand() * 0.2f / RAND_MAX;
    collisionValue = 0.0f;

    bMusketer = false;
    bMusketerNoMove = false;
}

AICharacter::~AICharacter()
{
}

//============================================================================================
// Character
//============================================================================================

// Move the character to the desired position
void AICharacter::Move(float dltTime)
{
    // zero the forces
    force = 0.0f;
    goForce = 0.0f;
    separation = 0.0f;
    alignment = 0.0f;
    around = 0.0f;
    // process the command
    switch (command.cmd)
    {
    case aicmd_none:
        break;
    case aicmd_stay:
        break;
    case aicmd_gotopoint:
        CmdProcessGotoPoint(dltTime);
        break;
    case aicmd_escape:
        CmdProcessEscape(dltTime);
        break;
    }
    Character::Move(dltTime);
}

// Carry out additional calculations
void AICharacter::Calculate(float dltTime)
{
    auto *const location = GetLocation();
    CalcRepulsionForces();
    CVECTOR slideForce;
    location->GetPtcData().FindForce(currentNode, slideForce);
    // restrict the vector
    auto l = ~separation;
    if (l > 1.0f)
        separation *= 1.0f / sqrtf(l);
    l = ~alignment;
    if (l > 1.0f)
        alignment *= 1.0f / sqrtf(l);
    l = ~around;
    if (l > 1.0f)
        around *= 1.0f / sqrtf(l);
    separation *= IsFight() ? 0.5f : 1.1f;
    alignment *= 0.1f;
    around *= 0.1f;
    goForce *= 1.2f;
    slideForce *= 0.2f;
    // Summing up the forces
    force += separation;
    force += alignment;
    force += around;
    force += goForce;
    force += slideForce;
    // if(isSlide) force += slideDir*0.3f;
    // Bringing in noise to aid decision making
    force.x += rand() * 0.0000001f / RAND_MAX;
    force.z += rand() * 0.0000001f / RAND_MAX;

    if (location->IsDebugView())
    {
        location->DrawLine(curPos + CVECTOR(0, 0.01f, 0), 0xff0000ff, curPos + goForce + CVECTOR(0, 0.01f, 0),
                           0xff0000ff, false);
        location->DrawLine(curPos + CVECTOR(0, 0.02f, 0), 0xffff0000, curPos + separation + CVECTOR(0, 0.02f, 0),
                           0xffff0000, false);
        location->DrawLine(curPos + CVECTOR(0, 0.03f, 0), 0xff00ff00, curPos + alignment + CVECTOR(0, 0.03f, 0),
                           0xff00ff00, false);
        location->DrawLine(curPos + CVECTOR(0, 0.04f, 0), 0xffffff00, curPos + around + CVECTOR(0, 0.04f, 0),
                           0xffffff00, false);
        location->DrawLine(curPos + CVECTOR(0, 0.1f, 0), 0xffffffff, curPos + force + CVECTOR(0, 0.1f, 0), 0xffffffff,
                           false);
        location->DrawLine(curPos + CVECTOR(0, 0.1f, 0), 0xff00ffff, curPos + slideForce + CVECTOR(0, 0.1f, 0),
                           0xffffffff, false);
    }

    Character::Calculate(dltTime);
}

// Update character position
void AICharacter::Update(float dltTime)
{
    switch (command.cmd)
    {
    case aicmd_none:
        break;
    case aicmd_stay:
        StopMove();
        break;
    case aicmd_gotopoint:
        CmdUpdateGotoPoint(dltTime);
        break;
    case aicmd_escape:
        CmdUpdateEscape(dltTime);
        break;
    }
    Character::Update(dltTime);
    if (isCollision)
    {
        collisionValue += dltTime * 1.0f;
        if (collisionValue > 1.0f)
        {
            collisionValue = 0.8f;
            CollisionThreshold();
        }
    }
    else
    {
        collisionValue -= dltTime * 0.1f;
        if (collisionValue < 0.0f)
            collisionValue = 0.0f;
    }
}

// Mark the movement of the character
void AICharacter::CharacterTeleport()
{
    currentNode = FindNodeIndex(curPos);
    if (currentNode < 0)
        core.Trace("Warning: NPCharacter <%s>-> trace node not found", characterID);
}

//============================================================================================
// AICharacter
//============================================================================================

// Nothing to do
bool AICharacter::CmdNone()
{
    command.cmd = aicmd_none;
    command.exch = nullptr;
    return true;
}

// stay
bool AICharacter::CmdStay()
{
    command.cmd = aicmd_stay;
    command.exch = nullptr;
    StopMove();
    return true;
}

// Go to the point
bool AICharacter::CmdGotoPoint(float x, float y, float z, float rad, long node, bool isCheckBusyPos)
{
    if (bMusketer && bMusketerNoMove)
        return true;
    if (currentNode < 0)
        return false;
    if (node < 0)
        node = FindNodeIndex(CVECTOR(x, y, z), &y);
    if (node < 0)
        return false;
    command.cmd = aicmd_gotopoint;
    command.pnt = CVECTOR(x, y, z);
    command.node = node;
    command.tpnt = curPos;
    command.tnode = currentNode;
    command.radius = rad;
    command.waitTime = 0.0f;
    command.isWait = false;
    command.isBusy = isCheckBusyPos;
    command.cnt = 0;
    command.exch = nullptr;
    StartMove();
    return true;
}

// Move away from the point
bool AICharacter::CmdEscape(float x, float y, float z, float rad)
{
    if (bMusketer && bMusketerNoMove)
        return true;
    command.cmd = aicmd_escape;
    command.pnt = CVECTOR(x, y, z);
    command.node = -1;
    command.radius = rad;
    command.waitTime = 0.0f;
    command.isWait = false;
    command.exch = nullptr;
    StartMove();
    return true;
}

// Set the character we don't collide with
void AICharacter::SetExCharacter(AICharacter *chr)
{
    command.exch = chr;
}

// ============================================================================================
// Encapsulation
// ============================================================================================

// Go to the point
void AICharacter::CmdProcessGotoPoint(float dltTime)
{
    if (command.isWait)
        return;
    // Find the direction of the path
    command.tpnt = curPos;
    command.tnode = currentNode;
    if (!FindDirectional())
    {
        command.cmd = aicmd_none;
        StopMove();
        FailureCommand();
        return;
    }
    goForce.x = command.tpnt.x - curPos.x;
    goForce.y = 0.0f;
    goForce.z = command.tpnt.z - curPos.z;
    auto fl = sqrtf(~goForce);
    const auto sn = sinf(ay);
    const auto cs = cosf(ay);
    if (fl <= 0.0f)
    {
        goForce.x = command.pnt.x - curPos.x;
        goForce.z = command.pnt.z - curPos.z;
        fl = sqrtf(~goForce);
    }
    if (fl > 0.0f)
    {
        goForce *= 1.0f / fl;
    }
    else
    {
        goForce.x = sn;
        goForce.z = cs;
    }
    fl = (fl - 0.5f) / (2.0f - 0.5f);
    if (fl < 0.0f)
        fl = 0.0f;
    if (fl > 1.0f)
        fl = 1.0f;
    if (goForce.x * sn + goForce.z * cs >= 0.9f - fl * 0.1f)
    {
        StartMove();
    }
    else
    {
        if ((rand() & 1023) == 5)
        {
            command.waitTime = 1.2f;
            command.isWait = true;
            Turn(goForce.x, goForce.z);
            StopMove();
            return;
        }
        StopMove();
    }
    // If reached destination, then stop
    const auto dx = command.pnt.x - curPos.x;
    const auto dz = command.pnt.z - curPos.z;
    auto d = dx * dx + dz * dz;
    auto *const location = GetLocation();
    if (location->IsDebugView())
    {
        auto dist = sqrtf(d) * 10.0f;
        if (dist > 255.0f)
            dist = 255.0f;
        uint32_t color = static_cast<long>(dist);
        color |= (255 - color) << 16;
        color |= 0xff00ff00;
        location->DrawLine(command.pnt + CVECTOR(0, 0.01f, 0), color, command.pnt + CVECTOR(0, 3.01f, 0), color, false);
    }
    // Slow down in front of the locator
    if (d < 1.5f * 1.5f)
        SetRunMode(false);
    if (d < command.radius * command.radius)
    {
        d = fabsf(command.pnt.y - curPos.y);
        if (d < height)
        {
            command.cmd = aicmd_none;
            StopMove();
            EndGotoCommand();
        }
    }
    else if (command.isBusy && d < FALURE_GOTO * FALURE_GOTO)
    {
        Assert(d > 0.0f);
        // if(sinf(ay)*dx + cosf(ay)*dz < 0
        {
            if (!location->supervisor.CheckPosition(command.pnt.x, command.pnt.y, command.pnt.z, this))
            {
                if (!command.isWait)
                {
                    if (command.cnt > static_cast<uint32_t>(1 + (rand() & 3)))
                    {
                        command.cmd = aicmd_none;
                        StopMove();
                        FailureCommand();
                    }
                    else
                    {
                        core.Event("Location_CharacterBusyPos", "ifff", GetId(), command.pnt.x, command.pnt.y,
                                   command.pnt.z);
                        command.cnt++;
                        command.isWait = true;
                        command.waitTime = 2.0f + rand() * 2.0f / RAND_MAX;
                    }
                }
            }
        }
    }
    if (location->IsDebugView())
        location->DrawLine(command.tpnt, 0xffff0000, command.tpnt + CVECTOR(0.0f, 3.0f, 0.0f), 0xff00ff00, false);
}

void AICharacter::CmdUpdateGotoPoint(float dltTime)
{
    if (!command.isWait)
    {
        StartMove();
        // if(location->IsDebugView()) location->DrawLine(curPos, 0xffff0000, curPos + force*2.0f, 0xffff0000, false);
        Turn(force.x, force.z);
        const auto l = force.x * force.x + force.z * force.z;
        if (l < 0.7f)
            kSpd = likeKSpd * l / 0.7f;
        else
            kSpd = likeKSpd;
        // If slide along the edge, stop and wait
        if (command.waitTime <= 0.0f)
        {
            /*
            if(isSlide && (rand() & 31) == 5)
            {
              StopMove();
              command.isWait = true;
              command.waitTime = 1.0f + rand()*1.0f/rand();
              Turn(goForce.x, goForce.z);
            }
            */
        }
        else
            command.waitTime -= dltTime;
    }
    else
    {
        if (command.waitTime <= 0.0f)
        {
            auto *const location = GetLocation();
            if (location->supervisor.CheckPosition(command.pnt.x, command.pnt.y, command.pnt.z, this))
            {
                // continue our way
                StartMove();
                command.isWait = false;
                command.waitTime = 1.0f + rand() * 1.0f / (rand() + 1.0f);
            }
            else
            {
                if (command.cnt < 3)
                {
                    command.cnt++;
                    command.isWait = true;
                    command.waitTime = 2.0f + rand() * 2.0f / RAND_MAX;
                }
                else
                {
                    command.cmd = aicmd_none;
                    StopMove();
                    FailureCommand();
                }
            }
        }
        else
        {
            // Expect
            command.waitTime -= dltTime;
            StopMove();
        }
    }
}

// Move away from the point
void AICharacter::CmdProcessEscape(float dltTime)
{
    // Removal direction
    goForce.x = curPos.x - command.pnt.x;
    goForce.y = 0.0f;
    goForce.z = curPos.z - command.pnt.z;
    auto l = goForce.x * goForce.x + goForce.z * goForce.z;
    if (l >= command.radius * command.radius)
    {
        goForce = 0.0f;
        command.cmd = aicmd_none;
        StopMove();
        EndEscapeCommand();
        return;
    }
    if (l > 0.0f)
    {
        l = 1.0f / sqrtf(l);
        goForce.x *= l;
        goForce.z *= l;
    }
    else
    {
        goForce.x = 0.0f;
        goForce.z = 1.0f;
    }
    goForce *= 3.0f;
}

void AICharacter::CmdUpdateEscape(float dltTime)
{
    auto *const location = GetLocation();
    if (location->IsDebugView())
        location->DrawLine(curPos, 0xffff0000, curPos + force * 2.0f, 0xffff0000, false);
    Turn(force.x, force.z);
    const auto l = force.x * force.x + force.z * force.z;
    if (l < 0.7f)
        kSpd = likeKSpd * l / 0.7f;
    else
        kSpd = likeKSpd;
}

//--------------------------------------------------------------------------------------------

// Find the node index for a given coordinate
long AICharacter::FindNodeIndex(const CVECTOR &pos, float *hy)
{
    auto *const location = GetLocation();
    float yy;
    const auto node = location->GetPtcData().FindNode(pos, yy);
    if (hy)
        *hy = yy;
    return node;
}

// Find direction where to go (orientation on the terrain)
bool AICharacter::FindDirectional()
{
    if (command.tnode < 0 || command.node < 0)
        return false;
    auto *const location = GetLocation();
    if (!location->GetPtcData().FindPathDir(command.tnode, CVECTOR(command.tpnt), command.node, command.pnt,
                                            command.tnode, command.tpnt))
        return false;
    if (location->IsDebugView())
    {
        for (long i = 0; i < location->GetPtcData().numSteps; i++)
        {
            location->DrawLine(location->GetPtcData().stepPos[i] + CVECTOR(0, 0.01f, 0), 0xffff00ff,
                               location->GetPtcData().stepPos[i] + CVECTOR(0, 2.01f, 0), 0xff2f80ff, false);
        }
    }
    return true;
}

// Find the pushing forces
void AICharacter::CalcRepulsionForces()
{
    if (numColCharacter <= 0)
        return;
    float k;
    const auto kn = 1.0f / numColCharacter;
    for (long i = 0; i < numColCharacter; i++)
    {
        auto *const location = GetLocation();
        auto &ci = location->supervisor.colchr[startColCharacter + i];
        if (ci.d == 0.0f)
            continue;
        auto *c = static_cast<AICharacter *>(ci.c);
        if (command.exch == c || c->command.exch == this)
            continue;
        const auto dx = c->curPos.x - curPos.x;
        const auto dz = c->curPos.z - curPos.z;
        const auto kd = 1.0f / ci.d;
        const auto kr = kn * (ci.maxD - ci.d) / ci.maxD;
        // The power of pushing
        const auto sx = dx * kr * kd * kd;
        const auto sz = dz * kr * kd * kd;
        separation.x -= sx;
        separation.z -= sz;
        c->separation.x += sx;
        c->separation.z += sz;
        // Strength of directions alignment
        k = 1.0f - fabsf(goForce.x * c->goForce.x + goForce.z * c->goForce.z);
        const auto af = (goForce + c->goForce) * (kn * kr * kd * k);
        if (af.x * dx + af.z * dz < 0.0f)
        {
            alignment += af;
            c->alignment -= af;
        }
        else
        {
            alignment -= af;
            c->alignment += af;
        }
        // Force to bypass
        const auto kcs = goForce.x * dx + goForce.z * dz;
        if (kcs > 0.0f)
        {
            k = kn * kr * kd * kcs;
            if (goForce.x * dz - goForce.z * dx < 0.0f)
                k = -k;
            around.x += k * goForce.z;
            around.z += k * -goForce.x;
            c->around.x -= k * goForce.z;
            c->around.z -= k * -goForce.x;
        }
    }
}

// Calculate the point formed by the intersection and lying on the edge
bool AICharacter::FindIntersection(const CVECTOR &s, const CVECTOR &e, const CVECTOR &cur, const CVECTOR &to,
                                   CVECTOR &res)
{
    const auto deX = e.x - s.x;
    const auto deZ = e.z - s.z;
    const auto dX = to.x - cur.x;
    const auto dZ = to.z - cur.z;
    // Plane passing through the move segment
    auto nx = dZ;
    auto nz = -dX;
    auto nl = nx * nx + nz * nz;
    if (nl == 0.0f)
    {
        res = to;
        return false;
    }
    nl = sqrtf(nl);
    nx /= nl;
    nz /= nl;
    const auto d = cur.x * nx + cur.z * nz;
    // Distances of edge vertices to plane
    const auto ds = nx * s.x + nz * s.z - d;
    const auto de = nx * e.x + nz * e.z - d;
    // decide what to do
    if (ds != de)
    {
        // Crossing the plane
        auto k = ds / (ds - de);
        if (k < 0.0f)
            k = 0.0f;
        if (k > 1.0f)
            k = 1.0f;
        res = s + (e - s) * k;
        // Check the side of the intersection
        if (dX * (res.x - cur.x) + dZ * (res.z - cur.z) < 0.0f)
        {
            // not standing by the side, must move to the edge
            if ((deZ) * (dZ) - (-deX) * (dX) < 0.0f)
                res = s;
            else
                res = e;
        }
    }
    else
    {
        // The path is parallel to the edge
        if ((deZ) * (dZ) - (-deX) * (dX) < 0.0f)
            res = s;
        else
            res = e;
        return false;
    }
    return true;
}

// Calculate angle from direction vector
float AICharacter::Angle(double vx, double vz, float defAy)
{
    // Calculate the angle
    const auto l = vx * vx + vz * vz;
    if (l <= 0.0)
        return defAy;
    vz = acos(vz / sqrt(l));
    if (vx < 0)
        vz = -vz;
    return static_cast<float>(vz);
}

const char *AICharacter::GetCommandName(AICommand cmd)
{
    switch (cmd)
    {
    case aicmd_none:
        return "none";
    case aicmd_stay:
        return "stay";
    case aicmd_gotopoint:
        return "gotopoint";
    case aicmd_escape:
        return "escape";
    }
    return "unknow";
}
