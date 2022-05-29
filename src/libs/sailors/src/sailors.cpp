//------------------------------------------------------------------------------------
//    Sailors, SailorsWayPoints, SailorsEditor, SailorsMenu, ShipWalk.c, SailorsMain
//    BattleInterface.c, ShipDead.c, sea_people.h
//------------------------------------------------------------------------------------
// micuss, 2004
//------------------------------------------------------------------------------------
#include "sailors.h"

#include "shared/messages.h"
#include "shared/sea_ai/script_defines.h"

CREATE_CLASS(Sailors)

//------------------------------------------------------------------------------------
ShipMan::ShipMan()
    : modelID(0), ptTo(), dir(), targetWayPoint(0), lastTargetPoint(0), moveTo()
{
    pos = CVECTOR(0.0f, 11.0f, 30.0f);
    ang.x = ang.y = ang.z = 0;
    angTo.x = angTo.y = angTo.z = 0;
    spos.x = spos.y = spos.z = 0;
    manSpeed = 0.1f;
    rotSpeed = 0.5f;
    mode = MAN_WALK;
    lastMode = MAN_OFF;
    newWayPoint = lastWayPoint = 0;
    dieTime = 0;
    inWater = false;
    jumpSpeedX = jumpSpeedY = 0;
}
;
//------------------------------------------------------------------------------------
void ShipMan::Free() const
{
    core.EraseEntity(this->modelID);
};
// ----- Building a matrix taking into account the current state ---------------------------------
void ShipMan::SetPos(MODEL *ship, SHIP_BASE *ship_base, uint32_t &dltTime, ShipState &shipState)
{
    if (auto model = static_cast<MODEL *>(core.GetEntityPointer(modelID)))
    {
        if (ship_base && (shipState.dead || jumpSpeedY))
        {
            // If got in the water - detach from the ship and swim
            if (!inWater && model->mtx.Pos().y < shipState.sea->WaveXZ(model->mtx.Pos().x, model->mtx.Pos().z) - 1.4f)
            {
                inWater = true;

                model->GetAnimation()->Player(0).SetAction("crawl");
                model->GetAnimation()->Player(0).SetPosition(rand() / static_cast<float>(RAND_MAX));
                model->GetAnimation()->Player(0).SetSpeed(model->GetAnimation()->Player(0).GetSpeed() / 2);
                model->GetAnimation()->Player(0).Play();

                pos = model->mtx.Pos();

                ang.y += ship_base->GetAng().y;
                ang.x = ang.z = 0;
                angTo.y = static_cast<float>(rand()) * PI * 2.0f / static_cast<float>(RAND_MAX);

                rotSpeed = MOVE_SPEED;
                mode = MAN_SWIM;
            }
        }

        if (!inWater)
        {
            CMatrix mPos, mRot, m1, m2;

            mPos.BuildMatrix(CVECTOR(ang.x, ang.y, ang.z),
                             pos + spos /*CVECTOR(pos.x+ spos.x, pos.y+ spos.y, pos.z+ spos.z)*/);

            if (ship_base)
            {
                model->mtx = mPos * (*ship_base->GetMatrix());
            }
            else
            {
                model->mtx = mRot * mPos;
            }
        }
        else
        {
            CMatrix mpos, mrot;
            mrot.BuildMatrix(this->ang);
            mpos.BuildPosition(this->pos.x, this->pos.y, this->pos.z);
            model->mtx = mrot * mpos;
        }
    }
};
//------------------------------------------------------------------------------------
bool ShipMan::RotateToAngle(uint32_t &dltTime, SailorsPoints &sailorsPoints)
{
    if (fabs(angTo.y - ang.y))
    {
        if (fabs(ang.y - angTo.y) < PI)
        {
            if (angTo.y > ang.y)
                ang.y += rotSpeed * dltTime / 100.0f;
            else
                ang.y -= rotSpeed * dltTime / 100.0f;
        }
        else
        {
            if (angTo.y > ang.y)
                ang.y -= rotSpeed * dltTime / 100.0f;
            else
                ang.y += rotSpeed * dltTime / 100.0f;
        }

        if (ang.y >= PI * 2)
            ang.y -= PI * 2;
        if (ang.y < 0)
            ang.y += PI * 2;

        if (fabs(static_cast<float>(angTo.y - ang.y)) < rotSpeed * dltTime / 100.0f)
            ang = angTo;

        return false;
    }
    return true;
};
// ----- Find New Target ---------------------------------------- -----------------
int ShipMan::FindRandomPoint(SailorsPoints &sailorsPoints, ShipState &shipState)
{
    int ran;
    // If combat mode or reload, then look for free guns
    for (auto m = 0; m < sailorsPoints.points.count; m++)
        for (auto i = 0; i < sailorsPoints.points.count; i++)
        {
            ran = static_cast<int>(rand() * (sailorsPoints.points.count - 1) / static_cast<float>(RAND_MAX));

            if (sailorsPoints.points.point[ran].IsCannon())

                if (ran != targetWayPoint && !sailorsPoints.points.point[ran].buisy &&
                    (!sailorsPoints.points.point[ran].cannonReloaded || shipState.mode == SHIP_WAR))
                {
                    sailorsPoints.points.point[ran].buisy = true;
                    moveTo = MOVE_TO_CANNON;
                    return ran;
                }
        }

    // Looking for free masts
    if (shipState.dead || rand() * 30 / static_cast<float>(RAND_MAX) <= 1)

        for (auto m = 0; m < sailorsPoints.points.count; m++)
            for (auto i = 0; i < sailorsPoints.points.count; i++)
            {
                ran = static_cast<int>(rand() * (sailorsPoints.points.count - 1) / static_cast<float>(RAND_MAX));

                if (!sailorsPoints.points.point[ran].disabled)

                    if (sailorsPoints.points.point[ran].IsMast())

                        if (ran != targetWayPoint && (shipState.dead || !sailorsPoints.points.point[ran].buisy))
                        {
                            sailorsPoints.points.point[ran].buisy = true;
                            moveTo = MOVE_TO_TOP;
                            return ran;
                        }
            }

    // Looking for simple unoccupied points
    for (auto m = 0; m < sailorsPoints.points.count; m++)
    {
        for (auto i = 0; i < sailorsPoints.points.count; i++)
        {
            ran = static_cast<int>(rand() * (sailorsPoints.points.count - 1) / static_cast<float>(RAND_MAX));

            if (ran != targetWayPoint && !sailorsPoints.points.point[ran].buisy &&
                sailorsPoints.points.point[ran].pointType == PT_TYPE_NORMAL)
            {
                moveTo = MOVE_TO_POINT;
                return ran;
            }
        }
    }

    return newWayPoint;
};
// ----- Find new target point without type -------------------------------------- ----------
int ShipMan::FindRandomPointWithoutType(SailorsPoints &sailorsPoints) const
// Find any simple point
{
    int ran;
    for (auto m = 0; m < sailorsPoints.points.count; m++)
        for (auto i = 0; i < sailorsPoints.points.count; i++)

            ran = static_cast<int>(rand() * (sailorsPoints.points.count - 1) / static_cast<float>(RAND_MAX));
    if (sailorsPoints.points.point[ran].pointType == PT_TYPE_NORMAL)
        return ran;

    return newWayPoint;
};
// ----- Find the next waypoint / new route ------------------------------------------
void ShipMan::FindNextPoint(SailorsPoints &sailorsPoints, ShipState &shipState)
{
    // Find the nearest unloaded cannon
    if (moveTo != MOVE_TO_CANNON)
    {
        const auto cannon = GetNearestEmptyCannon(sailorsPoints);
        if (cannon >= 0)
        {
            path.length = 0;
            path.min = -1;

            targetWayPoint = cannon;
            sailorsPoints.points.point[targetWayPoint].buisy = true;
            moveTo = MOVE_TO_CANNON;
            mode = MAN_RUN;
            path = sailorsPoints.findPath(path, newWayPoint, targetWayPoint);

            if (path.min != -1)
            {
                path.currentPointPosition = 0;
                newWayPoint = path.point[path.currentPointPosition];
            }

            return;
        }
    }

    // If the path has been passed or if no path, then find a new one
    if (!path.length || path.currentPointPosition == path.length - 1 || path.min == -1)
    {
        path.length = 0;
        path.min = -1;

        lastTargetPoint = targetWayPoint;

        targetWayPoint = FindRandomPoint(sailorsPoints, shipState);

        path = sailorsPoints.findPath(path, newWayPoint, targetWayPoint);

        if (path.min != -1)
        {
            path.currentPointPosition = 0;
            lastWayPoint = newWayPoint;

            if (path.length > 1)
                newWayPoint = path.point[path.currentPointPosition + 1];
            else
                newWayPoint = path.point[path.currentPointPosition];

            ApplyTargetPoint(CVECTOR(sailorsPoints.points.point[newWayPoint].x,
                                     sailorsPoints.points.point[newWayPoint].y,
                                     sailorsPoints.points.point[newWayPoint].z),
                             (sailorsPoints.points.point[newWayPoint].pointType == PT_TYPE_NORMAL));
        }

        // Select the next waypoint
    }
    else if (mode == MAN_RUN || mode == MAN_WALK || mode == MAN_CLIMB_UP || mode == MAN_CLIMB_DOWN)
    {
        path.currentPointPosition++;
        lastWayPoint = newWayPoint;

        newWayPoint = path.point[path.currentPointPosition];

        if (sailorsPoints.points.point[targetWayPoint].disabled)
        {
            path.length = 0;
            FindNextPoint(sailorsPoints, shipState);
            return;
        }

        // If a fight / reload has started, then start running
        if ((shipState.mode == SHIP_WAR || moveTo == MOVE_TO_CANNON) && mode == MAN_WALK)
            mode = MAN_RUN;

        ApplyTargetPoint(CVECTOR(sailorsPoints.points.point[newWayPoint].x, sailorsPoints.points.point[newWayPoint].y,
                                 sailorsPoints.points.point[newWayPoint].z),
                         (sailorsPoints.points.point[newWayPoint].pointType == PT_TYPE_NORMAL));
    }
};
//------------------------------------------------------------------------------------
void ShipMan::ApplyTargetPoint(CVECTOR pt, bool randomWalk)
{
    if (randomWalk)
    {
        // spread - 0.50

        ptTo.x = pt.x + (rand() / static_cast<float>(RAND_MAX) - rand() / static_cast<float>(RAND_MAX)) * 0.50f;
        ptTo.y = pt.y;
        ptTo.z = pt.z + (rand() / static_cast<float>(RAND_MAX) - rand() / static_cast<float>(RAND_MAX)) * 0.50f;
    }
    else
    {
        ptTo = pt;
    }

    dir = !(ptTo - pos);

    if (mode != MAN_CLIMB_UP)
    {
        angTo.y = Vector2Angle(dir);
    }
};
//------------------------------------------------------------------------------------
bool ShipMan::MoveToPosition(uint32_t &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState)
{
    if (auto model = static_cast<MODEL *>(core.GetEntityPointer(modelID)))
    {
        const auto dNow = static_cast<float>(SQR(pos.x - ptTo.x)) + SQR(pos.y - ptTo.y) + SQR(pos.z - ptTo.z);

        const auto dFuture = static_cast<float>(SQR(pos.x + manSpeed * dir.x - ptTo.x)) +
                             SQR(pos.y + manSpeed * dir.y - ptTo.y) + SQR(pos.z + manSpeed * dir.z - ptTo.z);

        if (shipState.dead && mode == MAN_CLIMB_UP &&
            dNow < sailorsPoints.points.point[newWayPoint].climbPosition * 10 + 1)
        {
            model->GetAnimation()->Player(0).Pause();
            sailorsPoints.points.point[newWayPoint].climbPosition++;
            mode = MAN_OFF;
            return false;
        }

        if (dFuture < dNow)
        {
            if (RotateToAngle(dltTime, sailorsPoints) || mode == MAN_RUN ||
                (mode == MAN_WALK && fabs(angTo.y - ang.y) < PI / 8))
            {
                pos.x += manSpeed * dir.x * dltTime / 100.0f;
                pos.y += manSpeed * dir.y * dltTime / 100.0f;
                pos.z += manSpeed * dir.z * dltTime / 100.0f;
            }

            return false;
        }

        pos = ptTo;
        ang.y = angTo.y;

        return true;
    }

    return false;
};
// ----- Find the nearest cannon ------------------------------------------ ----------------
int ShipMan::GetNearestEmptyCannon(SailorsPoints &sailorsPoints) const
{
    float minDest = 999;
    float dest;
    auto minIndex = -1;

    for (auto i = 0; i < sailorsPoints.points.count; i++)
        if (sailorsPoints.points.point[i].IsCannon() && sailorsPoints.points.point[i].buisy == false &&
            sailorsPoints.points.point[i].cannonReloaded == false && sailorsPoints.points.point[i].disabled == false)
        {
            dest = Dest(CVECTOR(sailorsPoints.points.point[i].x, sailorsPoints.points.point[i].y,
                                sailorsPoints.points.point[i].z),
                        pos);

            if (dest < minDest)
            {
                minDest = dest;
                minIndex = i;
            }
        }

    return minIndex;

};
//------------------------------------------------------------------------------------
bool ShipMan::Swim(uint32_t &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState)
{
    if (dieTime < 9)
    {
        pos.y += ((shipState.sea->WaveXZ(pos.x, pos.z) - 1.4f) - pos.y) * dltTime / 200.0f;
        pos.x += sin(ang.y) * manSpeed * dltTime / 500.0f;
        pos.z += cos(ang.y) * manSpeed * dltTime / 500.0f;

        RotateToAngle(dltTime, sailorsPoints);
    }
    else // The strength is running out - drown
        pos.y -= dltTime / 1000.0f;

    dieTime += 1.5f / static_cast<float>(dltTime);

    if (dieTime >= 10)
        return true;
    return false;

};
//------------------------------------------------------------------------------------
bool ShipMan::Stay(uint32_t &dltTime, SailorsPoints &sailorsPoints) const
{
    if (auto model = static_cast<MODEL *>(core.GetEntityPointer(modelID)))
    {
        return (!model->GetAnimation()->Player(0).IsPlaying());
    }

    return false;
};
//------------------------------------------------------------------------------------
bool ShipMan::Turn(uint32_t &dltTime, SailorsPoints &sailorsPoints)
{
    return RotateToAngle(dltTime, sailorsPoints);
};
//------------------------------------------------------------------------------------
bool ShipMan::Jump(uint32_t &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState)
{
    pos.y -= jumpSpeedY * dltTime / 1500.0f;

    pos.x += sin(ang.y + PI) * jumpSpeedX * dltTime / 350.0f;
    pos.z += cos(ang.y + PI) * jumpSpeedX * dltTime / 350.0f;

    jumpSpeedY += static_cast<float>(dltTime) / 10.0f;
    jumpSpeedX -= jumpSpeedX * static_cast<float>(dltTime) / 750.0f;

    if (shipState.sea && pos.y <= shipState.sea->WaveXZ(pos.x, pos.z) - 1.4f) // Drown deeper
        pos.y -= jumpSpeedY * static_cast<float>(dltTime) / 1000.0f;

    return false;
};
// ----- Update animation and speed ----------------------------------------- ----------
void ShipMan::SetAnimation(uint32_t dltTime, ShipState &shipState)
{
    if (auto model = static_cast<MODEL *>(core.GetEntityPointer(modelID)))
    {
        if (auto ani = model->GetAnimation(); mode == lastMode && ani && ani->Player(0).IsPlaying())
            return;
        float ran;

        switch (mode)
        {
        case MAN_WALK:
            model->GetAnimation()->Player(0).SetAction("walk");
            model->GetAnimation()->Player(0).SetPosition(rand() / static_cast<float>(RAND_MAX));
            model->GetAnimation()->Player(0).Play();

            manSpeed = MOVE_SPEED + rand() * MOVE_SPEED / static_cast<float>(RAND_MAX) / 4.0f -
                       rand() * MOVE_SPEED / static_cast<float>(RAND_MAX) / 4.0f;

            rotSpeed = MOVE_SPEED * 3.0f;
            break;
        case MAN_RUN:
            model->GetAnimation()->Player(0).SetAction("run");
            model->GetAnimation()->Player(0).SetPosition(rand() / static_cast<float>(RAND_MAX));
            model->GetAnimation()->Player(0).Play();

            manSpeed = RUN_SPEED;
            rotSpeed = MOVE_SPEED * 5.0f;
            break;
        case MAN_STAY:

            ran = rand() / static_cast<float>(RAND_MAX);

            if (ran < 0.25f)
                model->GetAnimation()->Player(0).SetAction("action1");
            else if (ran < 0.50f)
                model->GetAnimation()->Player(0).SetAction("action2");
            else if (ran < 0.75f)
                model->GetAnimation()->Player(0).SetAction("action3");
            else if (ran < 1.00f)
                model->GetAnimation()->Player(0).SetAction("action4");

            model->GetAnimation()->Player(0).SetPosition(rand() / static_cast<float>(RAND_MAX));
            model->GetAnimation()->Player(0).Play();
            break;
        case MAN_TURNLEFT:
            model->GetAnimation()->Player(0).SetAction("turn_left");
            model->GetAnimation()->Player(0).SetPosition(0);
            model->GetAnimation()->Player(0).Play();
            break;
        case MAN_TURNRIGHT:
            model->GetAnimation()->Player(0).SetAction("turn_right");
            model->GetAnimation()->Player(0).SetPosition(0);
            model->GetAnimation()->Player(0).Play();
            break;
        case MAN_CLIMB_UP:

            manSpeed = CLIMB_SPEED;
            rotSpeed = MOVE_SPEED * 10.0f;

            model->GetAnimation()->Player(0).SetAction("crawl");
            model->GetAnimation()->Player(0).SetPosition(0);
            model->GetAnimation()->Player(0).Play();

            if (shipState.dead || shipState.mode == SHIP_WAR || shipState.mode == SHIP_STORM)
            {
                manSpeed *= 2;
                model->GetAnimation()->Player(0).SetSpeed(model->GetAnimation()->Player(0).GetSpeed() * 2);
            }

            break;
        case MAN_CLIMB_DOWN:

            manSpeed = CLIMB_SPEED;
            rotSpeed = MOVE_SPEED * 10.0f;

            model->GetAnimation()->Player(0).SetAction("crawl_down");
            model->GetAnimation()->Player(0).SetPosition(0);
            model->GetAnimation()->Player(0).Play();

            if (shipState.dead || shipState.mode == SHIP_WAR || shipState.mode == SHIP_STORM)
            {
                manSpeed *= 2;
                model->GetAnimation()->Player(0).SetSpeed(model->GetAnimation()->Player(0).GetSpeed() * 2);
            }
            break;
        case MAN_CANNONRELOAD:

            model->GetAnimation()->Player(0).SetAction("reload");
            model->GetAnimation()->Player(0).SetPosition(0);
            model->GetAnimation()->Player(0).Play();
            break;
        }

        lastMode = mode;
    }
};
// ----- Select a new action ------------------------------------------ ---------------
void ShipMan::NewAction(SailorsPoints &sailorsPoints, ShipState &shipState, uint32_t &dltTime)
{
    if (!sailorsPoints.links.count)
        return;

    // If the path is completed, choose a new action
    if (mode != MAN_CLIMB_UP && path.currentPointPosition == path.length - 1 || mode == MAN_STAY)
    {
        if (mode == MAN_CANNONRELOAD &&
            (shipState.mode == SHIP_SAIL || shipState.mode == SHIP_STORM || shipState.mode == SHIP_WAR))
        {
            // Get away from the cannon

            sailorsPoints.points.point[targetWayPoint].buisy = false;
            sailorsPoints.points.point[targetWayPoint].cannonReloaded = true;

            FindNextPoint(sailorsPoints, shipState);
            if (shipState.mode == SHIP_WAR)
                mode = MAN_RUN;
            else
                mode = MAN_WALK;
            return;
        }

        if (targetWayPoint == newWayPoint && moveTo == MOVE_TO_CANNON)
        {
            // Work at the cannon

            mode = MAN_CANNONRELOAD;

            ang.y = angTo.y = Vector2Angle(
                !(CVECTOR(sailorsPoints.points.point[targetWayPoint].x, sailorsPoints.points.point[targetWayPoint].y,
                          sailorsPoints.points.point[targetWayPoint].z) -

                  CVECTOR(sailorsPoints.points.point[lastWayPoint].x, sailorsPoints.points.point[lastWayPoint].y,
                          sailorsPoints.points.point[lastWayPoint].z)));
            return;
        }

        if (mode == MAN_CLIMB_DOWN)
        {
            // Get off and free the point

            mode = MAN_WALK;
            sailorsPoints.points.point[lastTargetPoint].buisy = false;
            return;
        }

        switch (shipState.mode)
        {
        case SHIP_SAIL:
            float ran;
            ran = rand() / static_cast<float>(RAND_MAX);

            if (mode != MAN_STAY && ran < 0.1f)
                mode = MAN_RUN;
            else if (ran < 0.6f || mode == MAN_STAY)
                mode = MAN_WALK;
            else
                mode = MAN_STAY;
            break;
        case SHIP_WAR:
        case SHIP_STORM:
            mode = MAN_RUN;
            break;
        }
    }

    // find / continue the path
    FindNextPoint(sailorsPoints, shipState);

    if (sailorsPoints.points.point[newWayPoint].IsMast())
    {
        mode = MAN_CLIMB_UP;
    }
    else if (sailorsPoints.points.point[lastWayPoint].IsMast())
    {
        mode = MAN_CLIMB_DOWN;
    }
    else if (mode == MAN_CLIMB_DOWN)
    {
        mode = MAN_WALK;

        if (lastTargetPoint >= 0 && lastTargetPoint < sailorsPoints.points.count)
            sailorsPoints.points.point[lastTargetPoint].buisy = false;
    }
};
// ----- Main function -----------------------------------------------------
void ShipMan::UpdatePos(uint32_t &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState)
{
    switch (mode)
    {
    case MAN_WALK:
    case MAN_RUN:
    case MAN_CLIMB_UP:
    case MAN_CLIMB_DOWN:

        if (MoveToPosition(dltTime, sailorsPoints, shipState))
            NewAction(sailorsPoints, shipState, dltTime);
        break;

    case MAN_STAY:
    case MAN_CANNONRELOAD:
        if (Stay(dltTime, sailorsPoints))
            NewAction(sailorsPoints, shipState, dltTime);
        break;

    case MAN_TURNLEFT:
    case MAN_TURNRIGHT:

        if (Turn(dltTime, sailorsPoints))
            NewAction(sailorsPoints, shipState, dltTime);
        break;

    case MAN_SWIM:
        Swim(dltTime, sailorsPoints, shipState);
        break;
    case MAN_JUMP:
        Jump(dltTime, sailorsPoints, shipState);
        break;
    }

    SetAnimation(dltTime, shipState);
};

//------------------------------------------------------------------------------------
void ShipWalk::Free()
{
    for (auto i = 0; i < crewCount; i++)
    {
        shipMan[i].Free();
    }
};
// ----- Reset cannon charging flags ---------------------------------------------------
void ShipWalk::ReloadCannons(int bort)
{
    for (auto i = 0; i < sailorsPoints.points.count; i++)
        if ((sailorsPoints.points.point[i].pointType == PT_TYPE_CANNON_L && (bort == 0 || bort == 1)) ||
            (sailorsPoints.points.point[i].pointType == PT_TYPE_CANNON_R && (bort == 0 || bort == 2)) ||
            (sailorsPoints.points.point[i].pointType == PT_TYPE_CANNON_F && (bort == 0 || bort == 3)) ||
            (sailorsPoints.points.point[i].pointType == PT_TYPE_CANNON_B && (bort == 0 || bort == 4)))
            sailorsPoints.points.point[i].cannonReloaded = false;

};
//------------------------------------------------------------------------------------
void ShipWalk::CreateNewMan(SailorsPoints &sailorsPoints)
{
    if (crewCount >= 50 || !sailorsPoints.points.count)
        return;

    entid_t manID;
    shipMan.push_back(ShipMan{});

    const int current = shipMan.size() - 1;

    shipMan[current].modelID = core.CreateEntity("MODELR");

    int modelIdx = rand() % std::size(shipManModels_);
    core.Send_Message(shipMan[current].modelID, "ls", MSG_MODEL_LOAD_GEO, shipManModels_[modelIdx].c_str());

    if(!core.Send_Message(shipMan[current].modelID, "ls", MSG_MODEL_LOAD_ANI, "Lo_Man"))
    {
        throw std::runtime_error("cannot load animation 'Lo_Man'");
    }

    shipMan[current].SetAnimation(0, shipState);

    if (sailorsPoints.points.count)
    {
        shipMan[current].newWayPoint = shipMan[current].FindRandomPointWithoutType(sailorsPoints);

        shipMan[current].pos.x = sailorsPoints.points.point[shipMan[current].newWayPoint].x;
        shipMan[current].pos.y = sailorsPoints.points.point[shipMan[current].newWayPoint].y;
        shipMan[current].pos.z = sailorsPoints.points.point[shipMan[current].newWayPoint].z;

        uint32_t dltTime = 0;
        shipMan[current].NewAction(sailorsPoints, shipState, dltTime);
    }
    crewCount++;
};
//------------------------------------------------------------------------------------
void ShipWalk::DeleteMan(int Index)
{
    if (Index >= 0 && Index < crewCount)
    {
        shipMan[Index].Free();
        shipMan.erase(shipMan.begin() + Index);
        crewCount--;
    }
};
//------------------------------------------------------------------------------------
bool ShipWalk::Init(entid_t _shipID, int editorMode, const char *shipType, std::vector<std::string> &&shipManModels)
{
    crewCount = 0;
    bHide = false;
    shipID = _shipID;

    const auto seaID = core.GetEntityId("sea");
    shipState.sea = static_cast<SEA_BASE *>(core.GetEntityPointer(seaID));

    if (!editorMode)
    {
        // Game mode - ship created

        ship = static_cast<SHIP_BASE *>(core.GetEntityPointer(_shipID));
        shipModel = ship->GetModel();

        // Load points
        std::string fileName = "RESOURCE\\MODELS\\Ships\\SAILORSPOINTS\\";
        fileName += shipType;
        fileName += ".ini";

        // GUARD_SAILORS(sailorsPoints.ReadFromFile)
        sailorsPoints.ReadFromFile(fileName);
        // UN//GUARD_SAILORS

        if (sailorsPoints.points.count <= 0 || sailorsPoints.links.count <= 0)
            return false;

        // Find broken masts
        auto *attr = ship->GetACharacter();
        auto *mastsAttr = attr->FindAClass(attr, "Ship.Masts");

        if (mastsAttr == nullptr)
        {
            spdlog::error("Cannot initialize ShipWalk: Ship.Masts is empty");
            return false;
        }

        const int iNumMasts = mastsAttr->GetAttributesNum();

        for (auto i = 0; i < iNumMasts; i++)
        {
            if (mastsAttr->GetAttributeClass(i)->GetAttributeAsFloat())
                SetMastBroken(((iNumMasts - 1) - i) + 1); // ??? The masts are opposite ???
        }

        if (!shipManModels.empty())
        {
            shipManModels_ = std::move(shipManModels);
        }

        // people count
        // ATTRIBUTES *att = ship->GetACharacter();
        auto *paShip = ship->GetAShip();
        auto peopleCount = 5;
        if (paShip)
        {
            peopleCount = paShip->GetAttributeAsDword("lowpolycrew", peopleCount);
        }
        // ATTRIBUTES *crewAttr = att->FindAClass(attr, "ship.crew");
        // int peopleCount = (int) ceilf(CREW2VISIBLE((float) crewAttr->GetAttributeAsDword("quantity", 1)));

        if (peopleCount > 100)
            peopleCount = 100;
        if (peopleCount < 0)
            peopleCount = 0;

        for (auto i = 0; i < peopleCount; i++)
            CreateNewMan(sailorsPoints);
    }
    else // Editor mode
    {
        ship = nullptr;
    }

    return true;
};
// ----- Turn off points of broken mast ----------------------------------------- ------
void ShipWalk::SetMastBroken(int iMastIndex)
{
    for (auto i = 0; i < sailorsPoints.points.count; i++)
    {
        if (sailorsPoints.points.point[i].pointType == PT_TYPE_MAST_1 && iMastIndex == 1)
            sailorsPoints.points.point[i].disabled = true;

        if (sailorsPoints.points.point[i].pointType == PT_TYPE_MAST_2 && iMastIndex == 2)
            sailorsPoints.points.point[i].disabled = true;

        if (sailorsPoints.points.point[i].pointType == PT_TYPE_MAST_3 && iMastIndex == 3)
            sailorsPoints.points.point[i].disabled = true;

        if (sailorsPoints.points.point[i].pointType == PT_TYPE_MAST_4 && iMastIndex == 4)
            sailorsPoints.points.point[i].disabled = true;

        if (sailorsPoints.points.point[i].pointType == PT_TYPE_MAST_5 && iMastIndex == 5)
            sailorsPoints.points.point[i].disabled = true;
    }

    for (auto i = 0; i < crewCount; i++)
        if (sailorsPoints.points.point[shipMan[i].newWayPoint].disabled ||
            sailorsPoints.points.point[shipMan[i].lastWayPoint].disabled)
        {
            shipMan[i].mode = MAN_JUMP;
            shipMan[i].jumpSpeedX = JUMP_SPEED_X_MASTFALL;
        }
};
// ----- Cannonball hitting the ship ----------------------------------------- --------------
void ShipWalk::OnHullHit(const CVECTOR &v)
{
    for (auto i = 0; i < crewCount; i++)
        if (10 * rand() / RAND_MAX < 3 && shipMan[i].mode != MAN_JUMP && shipMan[i].mode != MAN_SWIM)
        {
            CVECTOR pos;
            if (ship)
                pos = shipModel->mtx * shipMan[i].pos;
            else
                pos = shipMan[i].pos;

            if (sqrt((pos.x - v.x) * (pos.x - v.x) + (pos.z - v.z) * (pos.z - v.z)) < 1)
            {
                // Free the points
                if (shipMan[i].mode == MAN_CLIMB_DOWN)
                {
                    sailorsPoints.points.point[shipMan[i].lastTargetPoint].buisy = false;
                }
                else
                {
                    sailorsPoints.points.point[shipMan[i].targetWayPoint].buisy = false;
                    sailorsPoints.points.point[shipMan[i].newWayPoint].buisy = false;
                }

                shipMan[i].mode = MAN_JUMP;
                shipMan[i].jumpSpeedY =
                    -(JUMP_SPEED_Y_EXPLOSION + rand() * JUMP_SPEED_Y_EXPLOSION / static_cast<float>(RAND_MAX));
                shipMan[i].jumpSpeedX = -shipMan[i].jumpSpeedY / 10;
            }
        }
};
// ----- Bypassing Each Other ------------------------------------------ ---------------------
void ShipWalk::CheckPosition(uint32_t &dltTime)
{
    for (auto m = 0; m < crewCount; m++)
    {
        shipMan[m].spos.x -= shipMan[m].spos.x / 100.0f * static_cast<float>(dltTime) / 10.0f;
        shipMan[m].spos.z -= shipMan[m].spos.z / 100.0f * static_cast<float>(dltTime) / 10.0f;
    }

    float d;

    for (auto m = 0; m < crewCount; m++)
        for (auto i = 0; i < crewCount; i++)

            if (i != m && Dest(shipMan[m].pos, shipMan[i].pos, 1) &&
                (shipMan[m].mode == MAN_WALK || shipMan[m].mode == MAN_RUN) &&
                shipMan[m].path.currentPointPosition >= 0 &&
                shipMan[m].path.currentPointPosition < shipMan[m].path.length &&
                shipMan[i].path.currentPointPosition >= 0 &&
                shipMan[i].path.currentPointPosition < shipMan[i].path.length)
            {
                d = Dest(shipMan[m].pos, shipMan[i].pos);

                if (d < 1.0f)
                {
                    // if go in different directions
                    if (shipMan[m].path.point[shipMan[m].path.currentPointPosition] !=
                        shipMan[i].path.point[shipMan[i].path.currentPointPosition])
                    {
                        shipMan[m].spos.x += 0.2f * (+shipMan[m].dir.z * (1 - d) - shipMan[m].spos.x) / 15.0f *
                                             static_cast<float>(dltTime) / 20.0f;
                        shipMan[m].spos.z += 0.2f * (-shipMan[m].dir.x * (1 - d) - shipMan[m].spos.z) / 15.0f *
                                             static_cast<float>(dltTime) / 20.0f;
                    }
                    else
                    {
                        if (m < i)
                        {
                            shipMan[m].spos.x += 0.2f * (+shipMan[m].dir.z * (1 - d) - shipMan[m].spos.x) / 15.0f *
                                                 static_cast<float>(dltTime) / 20.0f;
                            shipMan[m].spos.z += 0.2f * (-shipMan[m].dir.x * (1 - d) - shipMan[m].spos.z) / 15.0f *
                                                 static_cast<float>(dltTime) / 20.0f;
                        }
                        else
                        {
                            shipMan[m].spos.x += 0.2f * (-shipMan[m].dir.z * (1 - d) - shipMan[m].spos.x) / 15.0f *
                                                 static_cast<float>(dltTime) / 20.0f;
                            shipMan[m].spos.z += 0.2f * (+shipMan[m].dir.x * (1 - d) - shipMan[m].spos.z) / 15.0f *
                                                 static_cast<float>(dltTime) / 20.0f;
                        }
                    }

                    break;
                }
            }
};
//------------------------------------------------------------------------------------
void ShipWalk::Reset(){
    /*
    for(int i= 0; i< sailorsPoints.points.count; i++)
    {
      sailorsPoints.points.point[i].buisy= false;
      sailorsPoints.points.point[i].cannonReloaded= true;
    };

    bool result= true;

    while (result)
    {
      result= false;

      for(int i= 0; i< crewCount; i++){

        if (shipMan[i].mode == MAN_SWIM || shipMan[i].mode == MAN_JUMP)
        {
          DeleteMan(i);
          result= true;
          break;
        };

      };
    };


    for(int i= 0; i< crewCount; i++)
    {
      shipMan[i].newWayPoint= shipMan[i].FindRandomPointWithoutType(sailorsPoints);

      shipMan[i].pos.x= sailorsPoints.points.point[shipMan[i].newWayPoint].x;
      shipMan[i].pos.y= sailorsPoints.points.point[shipMan[i].newWayPoint].y;
      shipMan[i].pos.z= sailorsPoints.points.point[shipMan[i].newWayPoint].z;

      shipMan[i].mode= MAN_WALK;

      DWORD dltTime= 0;
      shipMan[i].NewAction(sailorsPoints,shipState,dltTime);
    };*/
};
//------------------------------------------------------------------------------------
Sailors::Sailors()
    : rs(nullptr)
{
    shipsCount = 0;
    editorMode = false;
    disabled = false;
    IsOnDeck = 0;
}
;
//------------------------------------------------------------------------------------
Sailors::~Sailors()
{
    for (auto i = 0; i < shipsCount; i++)
    {
        shipWalk[i].Free();
    }
}

//------------------------------------------------------------------------------------
bool Sailors::Init()
{
    rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));

    core.SetLayerType(SEA_REALIZE, layer_type_t::realize);
    core.AddToLayer(SEA_REALIZE, GetId(), 65530);

    return true;
}

//------------------------------------------------------------------------------------
void Sailors::Realize(uint32_t dltTime)
{
    if (dltTime > 500)
    {
        dltTime = 500;
    }

    if (disabled)
        return;

    rs->SetRenderState(D3DRS_LIGHTING, true);

#ifdef SAILORS_DEBUG
    if (core.Controls->GetDebugAsyncKeyState(VK_F7) < 0)
    {
        for (int i = 0; i < shipsCount; i++)
        {
            shipWalk[i].SetMastBroken(3);
            shipWalk[i].SetMastBroken(2);
            shipWalk[i].SetMastBroken(1);
        }
    }
#endif

    for (auto m = 0; m < shipsCount; m++)
    {
        // If the ship and all people are dead then delete the object
        if (shipWalk[m].shipState.dead && shipWalk[m].crewCount <= 0)
        {
            DeleteShip(m);
            rs->SetRenderState(D3DRS_LIGHTING, false);
            return;
        }

        // Updating and drawing
        for (auto i = 0; i < shipWalk[m].shipMan.size(); i++)
        {
            shipWalk[m].shipMan[i].UpdatePos(dltTime, shipWalk[m].sailorsPoints, shipWalk[m].shipState);
            shipWalk[m].shipMan[i].SetPos(shipWalk[m].shipModel, shipWalk[m].ship, dltTime, shipWalk[m].shipState);

            if (!shipWalk[m].bHide)
            {
                if (auto model = static_cast<MODEL *>(core.GetEntityPointer(shipWalk[m].shipMan[i].modelID)))
                {
                    model->ProcessStage(Entity::Stage::realize, dltTime);
                }
            }
        }

        for (auto i = 0; i < shipWalk[m].shipMan.size(); i++)
        {
            // If died then delete
            if (shipWalk[m].shipMan[i].dieTime > 10 || shipWalk[m].shipMan[i].pos.y < -100)
            {
                shipWalk[m].DeleteMan(i);
                rs->SetRenderState(D3DRS_LIGHTING, false);
                return;
            }
        }

        // Setting ship state
        if (!shipWalk[m].shipState.dead)
        {
            shipWalk[m].CheckPosition(dltTime);

            if (shipWalk[m].ship && !shipWalk[m].shipState.dead && !editorMode)
            {
                /// shipState
                auto *shipAttr = shipWalk[m].ship->GetACharacter();
                auto *shipModeAttr = shipAttr->FindAClass(shipAttr, "ship.POS.mode");

                if (shipModeAttr)
                {
                    shipWalk[m].shipState.mode = shipModeAttr->GetAttributeAsDword();
                }
            }
        }
    }

    rs->SetRenderState(D3DRS_LIGHTING, false);
}

//------------------------------------------------------------------------------------
void Sailors::DeleteShip(int i)
{
    shipWalk[i].Free();
    shipWalk.erase(shipWalk.begin() + i);
    shipsCount--;
}

//------------------------------------------------------------------------------------
uint64_t Sailors::ProcessMessage(MESSAGE &message)
{
    const auto code = message.Long();
    const uint32_t outValue = 0;
    entid_t shipID;

    switch (code)
    {
        // Add people to the ship
    case AI_MESSAGE_ADD_SHIP: {

        shipID = message.EntityID();
        const std::string &c = message.String();
        std::vector<std::string> shipManModels;

        if (message.GetFormat() == "lise")
        {
            auto *pvd = message.ScriptVariablePointer();

            if (pvd != nullptr)
            {
                const auto nq = pvd->GetElementsNum();
                shipManModels.reserve(nq);

                for (auto i = 0; i < nq; i++)
                {
                    const char *pstr;
                    pvd->Get(pstr, i);
                    shipManModels.push_back(pstr);
                }
            }
        }

        shipWalk.emplace_back();
        if (shipWalk[shipsCount].Init(shipID, editorMode, c.c_str(), std::move(shipManModels)))
        {
            shipsCount++;

            if (!editorMode)
                if (!shipWalk[shipsCount - 1].sailorsPoints.points.count ||
                    !shipWalk[shipsCount - 1].sailorsPoints.links.count)
                {
                    DeleteShip(shipsCount - 1);
                    core.Trace("Sailors: sailors ship %s deleted", &c[0]);
                    return 0;
                }
        }
        else
        {
            shipWalk.pop_back();   
        }
        break;
    }

        // Reloading the cannons
    case AI_MESSAGE_CANNON_RELOAD: {
        shipID = message.EntityID();
        const std::string &bortName = message.String();

        for (auto i = 0; i < shipsCount; i++)
            if (shipID == shipWalk[i].shipID)
            {
                if (!strcmp(bortName.c_str(), "cannonl"))
                {
                    shipWalk[i].ReloadCannons(1);
                    return outValue;
                }

                if (!strcmp(bortName.c_str(), "cannonr"))
                {
                    shipWalk[i].ReloadCannons(2);
                    return outValue;
                }

                if (!strcmp(bortName.c_str(), "cannonf"))
                {
                    shipWalk[i].ReloadCannons(3);
                    return outValue;
                }

                if (!strcmp(bortName.c_str(), "cannonb"))
                {
                    shipWalk[i].ReloadCannons(4);
                    return outValue;
                }

                shipWalk[i].ReloadCannons(0);
                break;
            }

        break;
    }
        // Fall of the mast
    case MSG_PEOPLES_ON_SHIP_MASTFALL: {
        auto *const attrs = message.AttributePointer();
        if (!attrs)
            return 0;

        for (auto m = 0; m < shipsCount; m++)
            if (attrs == shipWalk[m].ship->GetACharacter())
            {
                shipWalk[m].SetMastBroken(message.Long());
                return outValue;
            }

        break;
    }

        // Cannonball hit the ship
    case MSG_PEOPLES_ON_SHIP_HULLHIT: {
        auto *const attrs = message.AttributePointer();
        if (!attrs)
            return 0;

        for (auto m = 0; m < shipsCount; m++)
            if (attrs == shipWalk[m].ship->GetACharacter())
            {
                const auto x = message.Float();
                const auto y = message.Float();
                const auto z = message.Float();

                shipWalk[m].OnHullHit(CVECTOR(x, y, z));
                return outValue;
            }

        break;
    }

        // Removing a ship
    case MSG_SHIP_DELETE: {
        auto *const attrs = message.AttributePointer();
        if (attrs)
            for (auto m = 0; m < shipsCount; m++)
                if (attrs == shipWalk[m].ship->GetACharacter())
                {
                    shipWalk[m].shipState.dead = true;
                    shipWalk[m].shipState.mode = SHIP_STORM;

                    return outValue;
                }
        break;
    }
    }

    return outValue;
};

//------------------------------------------------------------------------------------
uint32_t Sailors::AttributeChanged(ATTRIBUTES *_newAttr)
{
    // Remove people from deck
    if (*_newAttr == "IsOnDeck")
    {
        IsOnDeck = this->AttributesPointer->GetAttributeAsDword("IsOnDeck") != 0;

        if (IsOnDeck)
        {
            for (auto i = 0; i < shipsCount; i++)
            {
                if (shipWalk[i].ship->GetACharacter()->GetAttribute("MainCharacter"))
                {
                    shipWalk[i].bHide = true;
                    shipWalk[i].Reset();
                    break;
                }
            }
        }
        else
        {
            for (auto i = 0; i < shipsCount; i++)
            {
                shipWalk[i].bHide = false;
            }
        }
        return 0;
    }

    return 0;
};