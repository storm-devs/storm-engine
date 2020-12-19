#include "SEA_OPERATOR.h"
#include "../../Shared/messages.h"
#include "EntityManager.h"
#include "defines.h"
#include "rands.h"

char dbgs[1024];
static const float MIN_TIME_DELTA = 0.05f;
CVECTOR lastCP;

//--------------------------------------------------------------------
SEA_OPERATOR::SEA_OPERATOR()
    : enabled(false), active(false), sea(nullptr), renderer(nullptr), myShip(nullptr), enemyShip(nullptr), idleTime(0),
      sinceLastActionTime(0)
{
}

//--------------------------------------------------------------------
SEA_OPERATOR::~SEA_OPERATOR()
{
}

//--------------------------------------------------------------------
bool SEA_OPERATOR::Init()
{
    EntityManager::AddToLayer(REALIZE, GetId(), 1);
    EntityManager::AddToLayer(EXECUTE, GetId(), 0);
    // api->SystemMessages(GetId(),true);

    renderer = static_cast<VDX9RENDER *>(api->CreateService("dx9render"));

    return true;
}

//--------------------------------------------------------------------
uint32_t SEA_OPERATOR::AttributeChanged(ATTRIBUTES *_newAttr)
{
    if (*_newAttr == "FirstInit")
        FirstInit();

    return 0;
}

//--------------------------------------------------------------------
uint64_t SEA_OPERATOR::ProcessMessage(MESSAGE &message)
{
    if (!enabled)
        return 0;

    const auto code = message.Long();
    uint32_t outValue = 0;

    switch (code)
    {
    case MSG_SEA_OPERATOR_FIRE: {
        if (!IsTimeToActivate(false))
            break;
        const auto firedShip = message.EntityID();
        if (myShip != static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(firedShip)))
            break;

        char bortName[256];
        message.String(255, static_cast<char *>(bortName));
        CVECTOR direction, destination;
        direction.x = message.Float();
        direction.y = message.Float();
        direction.z = message.Float();
        destination.x = message.Float();
        destination.y = message.Float();
        destination.z = message.Float();
        ballTracked = false;
        HandleShipFire(firedShip, bortName, direction, destination);
    }
    break;
    case MSG_SEA_OPERATOR_BALL_UPDATE: {
        auto *const attr = message.AttributePointer();
        if (myShip->GetACharacter() != attr)
            break;

        const auto ballAlive = message.Long() != 0;
        if (ballAlive && !ballTracked)
        {
            StartNewAction();
            ballTracked = true;
        }

        if (!ballAlive /*&& ballTracked*/)
        {
            StartNewAction();
            // active = false;
            // actionBuffer.Pop();
            // ballTracked = false;
        }

        ballPosition.x = message.Float();
        ballPosition.y = message.Float();
        ballPosition.z = message.Float();
        lastBallPosition.x = message.Float();
        lastBallPosition.y = message.Float();
        lastBallPosition.z = message.Float();

        if (!ballAlive)
            finalBallPosition = ballPosition;
    }
    break;
    case MSG_SEA_OPERATOR_SHIP_HIT: {
        if (!IsTimeToActivate())
            break;

        ballPosition.x = message.Float();
        ballPosition.y = message.Float();
        ballPosition.z = message.Float();

        auto *const shipAttr = message.AttributePointer();
        if (shipAttr == myShip->GetACharacter())
            HandleShipHit();
    }
    break;
    }

    return 0;
}

//--------------------------------------------------------------------
void SEA_OPERATOR::Realize(uint32_t dTime)
{
    // renderer->Print(0, 90, "lastControlTime: %d", api->Controls->LastControlTime());

    if (!enabled)
        return;

    static CVECTOR upVector(0.0f, 1.0f, 0.0f);
    if (active)
    {
        renderer->SetCamera(cameraPos, cameraTargetPos, upVector);
        renderer->Print(0, 30, "cam: [%3.1f;%3.1f;%3.1f], tgt: [%3.1f;%3.1f;%3.1f]", cameraPos.x, cameraPos.y,
                        cameraPos.z, cameraTargetPos.x, cameraTargetPos.y, cameraTargetPos.z);
        renderer->Print(0, 60, dbgs);
    }
}

//--------------------------------------------------------------------
void SEA_OPERATOR::Execute(uint32_t _dTime)
{
    if (!enabled)
        return;

    if (((rand() % IDLE_ACTION_SEED) == 1) && IsTimeToActivate())
        HandleShipIdle();

    if (!active)
        sinceLastActionTime += _dTime;
    ProcessActions(_dTime);
}

void SEA_OPERATOR::FirstInit()
{
    sea = static_cast<SEA_BASE *>(EntityManager::GetEntityPointer(EntityManager::GetEntityId("sea")));

    const auto &entities = EntityManager::GetEntityIdVector("ship");
    for (auto ent : entities)
    {
        SetIfMyShip(ent);
    }

    enabled = this->AttributesPointer->GetAttributeAsDword("Enabled") != 0;
    idleTime = this->AttributesPointer->GetAttributeAsDword("IdleTime");

    if (!myShip)
        enabled = false;
}

//--------------------------------------------------------------------
void SEA_OPERATOR::ProcessActions(uint32_t _dTime)
{
    if (active)
    {
        auto *currentAction = actionBuffer.TopElement();
        if ((currentAction->actionTime != -1) && (currentAction->timePassed >= currentAction->actionTime))
        {
            StartNewAction();
            return;
        }
        (this->*(currentAction->actionMethod))(currentAction);
        currentAction->timePassed += _dTime;
    }
    else
        StartNewAction();
}

//--------------------------------------------------------------------
void SEA_OPERATOR::StartNewAction()
{
    auto *currentAction = actionBuffer.TopElement();
    if (active && !currentAction)
    {
        active = false;
        api->SetTimeScale(1.0f);
        sinceLastActionTime = 0;
        return;
    }

    if (active)
        actionBuffer.Pop();
    currentAction = actionBuffer.TopElement();
    if (!currentAction)
    {
        if (active)
        {
            api->SetTimeScale(1.0f);
            sinceLastActionTime = 0;
        }
        active = false;
        return;
    }

    active = true;
    currentAction->timePassed = 0;
    api->SetTimeScale(currentAction->timeK);
}

//--------------------------------------------------------------------
void SEA_OPERATOR::SetIfMyShip(entid_t _shipID)
{
    auto *ship = static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(_shipID));
    if (!ship)
        return;
    auto *attr = ship->GetACharacter();
    if (attr->GetAttribute("MainCharacter"))
        myShip = ship;
}

//--------------------------------------------------------------------
void SEA_OPERATOR::HandleShipHit()
{
    tAction action;
    action.timeK = 0.3f;
    action.actionTime = 5000;
    action.actionMethod = &SEA_OPERATOR::ShowBallAtMyShip;
    actionBuffer.Push(action);
}

//--------------------------------------------------------------------
void SEA_OPERATOR::HandleShipIdle()
{
    tAction action;

    action.timeK = 1.0f;
    action.actionTime = 15000;

    const auto startAngle = randCentered(PId2);
    const auto startDistance = randUpper(2.0f * myShip->GetCurrentSpeed() * action.actionTime / 1e3f);
    action.direction = myShip->GetPos();
    action.direction.z += startDistance * sinf(startAngle + myShip->GetAng().y);
    action.direction.x += startDistance * cosf(startAngle + myShip->GetAng().y);

    action.actionMethod = &SEA_OPERATOR::ShowMyShipFromPoint;
    actionBuffer.Push(action);
}

//--------------------------------------------------------------------
void SEA_OPERATOR::HandleShipFire(entid_t _shipID, char *_bortName, const CVECTOR &_destination,
                                  const CVECTOR &_direction)
{
    auto bort = BORT_FRONT;
    auto *ship = static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(_shipID));

    if (!strcmp(_bortName, "cannonf"))
        bort = BORT_FRONT;
    else if (!strcmp(_bortName, "cannonb"))
        bort = BORT_REAR;
    else if (!strcmp(_bortName, "cannonl"))
        bort = BORT_LEFT;
    else if (!strcmp(_bortName, "cannonr"))
        bort = BORT_RIGHT;

    tAction action;
    // tTrack *track;
    auto shipPosition = ship->GetPos();
    const auto shipDirection = CVECTOR(sinf(ship->GetAng().y), 0.0f, cosf(ship->GetAng().y));
    auto shipDirectionPerp = CVECTOR(shipDirection.z, 0.0f, -1.0f * shipDirection.x);
    float chosenK;

    srand(GetTickCount());
    if (rand() & 0x1)
        chosenK = -1.0f;
    else
        chosenK = 1.0f;
    auto addY = 0.5f * ship->GetBoxsize().y;

    action.timeK = 0.15f;
    action.timePassed = 0;
    action.actionTime = -1;
    action.attackerShip = ship;
    action.direction = _direction;
    action.destination = _destination;
    action.actionMethod = &SEA_OPERATOR::ShowAttackerBort;
    actionBuffer.Push(action);

    action.timeK = 0.7f;
    action.actionTime = -1;
    action.actionMethod = &SEA_OPERATOR::ShowFromBall;
    actionBuffer.Push(action);

    action.timeK = 0.15f;
    action.actionTime = 5000;
    action.actionMethod = &SEA_OPERATOR::ShowAroundPoint;
    actionBuffer.Push(action);
}

//--------------------------------------------------------------------
void SEA_OPERATOR::ShowAttackerBort(tAction *_action)
{
    const auto shipPosition = _action->attackerShip->GetPos();
    const auto addY = 0.5f * _action->attackerShip->GetBoxsize().y;
    const auto timeDistance = static_cast<float>(_action->timePassed) / 7e2f;
    const auto shipDirection =
        CVECTOR(sinf(_action->attackerShip->GetAng().y), 0.0f, cosf(_action->attackerShip->GetAng().y));
    auto shipDirectionPerp = CVECTOR(shipDirection.z, 0.0f, -1.0f * shipDirection.x);
    const auto directionPerp = CVECTOR(_action->direction.z, 0.0f, -1.0f * _action->direction.x);

    cameraTargetPos = shipPosition;
    cameraTargetPos.y += addY;

    cameraPos = shipPosition + _action->direction * (0.5f * _action->attackerShip->GetBoxsize().z) +
                directionPerp * (0.5f * _action->attackerShip->GetBoxsize().z - timeDistance);
    // cameraPos.y += addY;
    cameraPos.y = 1.0f + sea->WaveXZ(cameraPos.x, cameraPos.z);
}

//--------------------------------------------------------------------
void SEA_OPERATOR::ShowFromBall(tAction *_action)
{
    /*
    float timeDistance = ((float) _action->timePassed) / 200.0f;
    cameraTargetPos = _action->destination;
    cameraPos = lastBallPosition - _action->direction*timeDistance;
    cameraPos.x += 0.2f * sinf(timeDistance);
    cameraPos.y += 2.0f + 0.2f * sinf(timeDistance*1.3f);
    cameraPos.z += 0.2f * sinf(timeDistance*1.9f);
    float timeScale = ~(_action->destination - ballPosition)/3e3f;
    if (timeScale < MIN_TIME_DELTA)
      timeScale = MIN_TIME_DELTA;
    if (timeScale > 1.0f)
      timeScale = 1.0f;
    api->SetTimeScale(timeScale);
    */

    const auto timeDistance = static_cast<float>(_action->timePassed) / 60.0f;
    const auto newCameraPos = 0.8f * ballPosition + 0.2f * _action->destination;
    cameraTargetPos = ballPosition;
    cameraPos = newCameraPos;
    cameraTargetPos.x += 0.1f * sinf(timeDistance);
    cameraTargetPos.y += 0.1f * sinf(timeDistance * 1.3f);
    cameraTargetPos.z += 0.1f * sinf(timeDistance * 1.9f);
    lastCP = cameraPos;
    api->SetTimeScale(0.3f);
}

//--------------------------------------------------------------------
void SEA_OPERATOR::ShowAroundPoint(tAction *_action)
{
    const auto deltaVector = _action->destination - _action->attackerShip->GetPos();
    const auto aroundAngle = atan2f(deltaVector.z, deltaVector.x);
    const auto timeK = (static_cast<float>(_action->timePassed) / _action->actionTime);
    auto angle = aroundAngle - 0.1f * (PId2 + PI * timeK);
    const auto timeScale = MIN_TIME_DELTA + (1.0f - MIN_TIME_DELTA) * powf(timeK, 0.37f);
    auto radius = 15.0f;

    // cameraTargetPos = finalBallPosition;
    if (~(cameraPos - cameraTargetPos) < 40.0f)
        cameraPos =
            lastCP + (20.0f + static_cast<float>(_action->timePassed) / 1000.0f) * !(lastCP - finalBallPosition);
    /*
    //cameraPos = finalBallPosition;
    if (_action->hitIntoShip == -1)
    {
      if (fabs(finalBallPosition.y - sea->WaveXZ(finalBallPosition.x,finalBallPosition.z)) < 0.2)
        _action->hitIntoShip = 0;
      else
        _action->hitIntoShip = 1;
    }

    if (_action->hitIntoShip)
    { //hit into water
      radius = 15.0f;
      //cameraPos.y = 6.0f + sea->WaveXZ(cameraPos.x, cameraPos.z);
    }
    else
    { //hit into ship
      radius = 65.0f;
      //cameraPos.y = 1.0f + sea->WaveXZ(cameraPos.x, cameraPos.z);
    }
    //cameraPos.x += radius*cosf(angle);
    //cameraPos.z += radius*sinf(angle);

    //CVECTOR quake(randCentered(0.8f),randCentered(0.8f),randCentered(0.8f));
    CVECTOR quake(randCentered(0.1f),randCentered(0.6f),randCentered(0.1f));
    //cameraPos += quake;
    //cameraTargetPos += quake;
  */
    api->SetTimeScale(timeScale);
}

//--------------------------------------------------------------------
void SEA_OPERATOR::ShowBallAtMyShip(tAction *_action)
{
    cameraTargetPos = myShip->GetPos();
    const auto timeK = (static_cast<float>(_action->timePassed) / _action->actionTime);
    cameraPos = ballPosition + (0.5f + 0.5f * timeK) * myShip->GetBoxsize().z * !(ballPosition - myShip->GetPos());
    const auto minY = 1.0f + sea->WaveXZ(cameraPos.x, cameraPos.z);
    const auto timeScale = MIN_TIME_DELTA + (1.0f - MIN_TIME_DELTA) * powf(timeK, 0.37f);
    if (cameraPos.y < minY)
        cameraPos.y = minY;

    api->SetTimeScale(timeScale);
}

//--------------------------------------------------------------------
void SEA_OPERATOR::ShowMyShipFromPoint(tAction *_action)
{
    cameraTargetPos = myShip->GetPos();
    // float timeK = ((float) _action->timePassed / _action->actionTime);
    cameraPos = _action->direction;
    const auto minY = 1.0f + sea->WaveXZ(cameraPos.x, cameraPos.z);
    // float timeScale = MIN_TIME_DELTA + (1.0f - MIN_TIME_DELTA)*powf(timeK, 0.37f);
    if (cameraPos.y < minY)
        cameraPos.y = minY;

    // api->SetTimeScale(timeScale);
}

//--------------------------------------------------------------------
bool SEA_OPERATOR::IsTimeToActivate(bool _testControls /* =true */)
{
    if (active)
        return false;

    if (_testControls)
    {
        const uint32_t lastControlTime = api->Controls->LastControlTime();
        if (lastControlTime < idleTime)
            return false;
    }

    if (sinceLastActionTime < TIME_BETWEEN_ACTIONS)
        return false;

    return true;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
