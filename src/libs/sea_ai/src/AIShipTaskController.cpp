#include "AIFort.h"
#include "AIShip.h"

AIShipTaskController::AIShipTaskController(AIShip *pShip)
{
    fZapasDistance = 80.0f + FRAND(100.0f);
    SetAIShip(pShip);
}

AIShipTaskController::~AIShipTaskController()
{
}

bool AIShipTaskController::CheckCurrentTaskComplete()
{
    // if we are dead, task is ended
    if (GetAIShip()->isDead())
        return true;

    switch (GetCurrentTask()->dwTaskType)
    {
    case AITASK_ABORDAGE:
    case AITASK_ATTACK:
    case AITASK_DEFEND:
        if (!GetCurrentTaskAIObj() || GetCurrentTaskAIObj()->isDead())
            return true;
        break;
    case AITASK_MOVE:
    case AITASK_RUNAWAY:
        break;
    case AITASK_FOLLOW:
        if (!GetCurrentTaskAIObj() || GetCurrentTaskAIObj()->isDead())
            return true;
        break;
    case AITASK_DRIFT:
        break;
    }
    return false;
}

void AIShipTaskController::SetDestinationPoint(CVECTOR vDestPnt)
{
    GetAIShip()->GetMoveController()->Move(vDestPnt);
}

void AIShipTaskController::DoAttackRotate()
{
    const auto vFirePos = GetCurrentTaskAIObj()->GetPos();
    const auto vOurDir = CVECTOR(sinf(GetAIShip()->GetAng().y), 0.0f, cosf(GetAIShip()->GetAng().y));

    auto *pCC = GetAIShip()->GetCannonController();

    const auto dwBort = pCC->GetBestFireBortOnlyDistance(vFirePos, 20.0f + FRAND(50.0f));

    if (INVALID_BORT_INDEX != dwBort)
    {
        const auto vBortDir = pCC->GetBortDirection(dwBort);
        const auto fRotate = ((vBortDir | vOurDir) > 0.0f) ? -1.2f : 1.2f;
        GetAIShip()->GetRotateController()->AddRotate(fRotate);
        GetAIShip()->GetSpeedController()->AddSpeed(0.5f);
    }
}

void AIShipTaskController::FindRunAwayPoint()
{
    CVECTOR vRAPoint = 0.0f;
    uint32_t iNumPoints = 0;

    const auto fShipK = 1.0f;  // impact factor from ships
    const auto fFortK = 1.0f;  // fort impact factor
    const auto fWindK = 10.0f; // impact factor for wind

    // check ships
    for (uint32_t i = 0; i < AIShip::AIShips.size(); i++)
        if (GetAIShip() != AIShip::AIShips[i])
        {
            if (!Helper.isEnemy(AIShip::AIShips[i]->GetACharacter(), GetAIShip()->GetACharacter()))
                continue;
            auto vDir = AIShip::AIShips[i]->GetPos() - GetAIShip()->GetPos();
            const auto fDistance = sqrtf(~vDir);
            vRAPoint += ((!vDir) * fShipK * Clamp(1.0f - fDistance / 1000.0f));
            iNumPoints++;
        }

    // check forts
    if (AIFort::pAIFort)
    {
        for (uint32_t k = 0; k < AIFort::pAIFort->GetNumForts(); k++)
        {
            auto *const pFort = AIFort::pAIFort->GetFort(k);
            if (!Helper.isEnemy(pFort->GetACharacter(), GetAIShip()->GetACharacter()))
                continue;
            if (!pFort->isNormalMode())
                continue;

            auto vDir = pFort->GetPos() - GetAIShip()->GetPos();
            const auto fDistance = sqrtf(~vDir);
            if (fDistance > 2000.0f)
                continue;
            vRAPoint += ((!vDir) * fFortK * Clamp(1.0f - fDistance / 2000.0f));
            iNumPoints++;
        }
    }
    // normalize the direction - get a vector where the main threat is
    if (iNumPoints)
        vRAPoint = vRAPoint / static_cast<float>(iNumPoints);

    if ((~vRAPoint) && iNumPoints)
    {
        // Get wind direction
        auto fWindAngle = -PI;
        auto *pAWind = GetAIShip()->GetACharacter()->FindAClass(GetAIShip()->GetACharacter(), "SeaAI.WindAngle");
        if (pAWind)
            fWindAngle = pAWind->GetAttributeAsFloat();
        const auto vWindDir = CVECTOR(cosf(fWindAngle), 0.0f, sinf(fWindAngle));
        // core.Trace("fWindAngle = %.3f", fWindAngle);

        vRAPoint = !((-vRAPoint) + (fWindK * vWindDir));
        // set destination point
        SetDestinationPoint(GetAIShip()->GetPos() + 10000.0f * vRAPoint);
    }
    else
    {
        // If 1 is returned from the script, then use the script point
        auto *pV = core.Event(SHIP_GET_RUNAWAY_POINT, "aff", GetAIShip()->GetACharacter(), vRAPoint.x, vRAPoint.z);
        auto *pARAP = GetAIShip()->GetACharacter()->FindAClass(GetAIShip()->GetACharacter(), "SeaAI.RunAwayPnt");
        vRAPoint.y = 0.0f;
        vRAPoint.x = pARAP->GetAttributeAsFloat("x", 0.0f);
        vRAPoint.z = pARAP->GetAttributeAsFloat("z", 0.0f);
        SetDestinationPoint(vRAPoint);
    }

    /*if (core.Controls->GetDebugAsyncKeyState('X') < 0)
    {
      GetAIShip()->GetShipPointer()->Render().DrawVector(GetAIShip()->GetPos(), GetAIShip()->GetPos() + 20.0f *
    vRAPoint, 0xFFFFFFFF); GetAIShip()->GetShipPointer()->Render().DrawSphere(GetAIShip()->GetPos() + 50.0f *
    vRAPoint, 4.0f, 0xFFFFFFFF);
    }*/
}

void AIShipTaskController::DoTask(float fDeltaTime)
{
    auto *const pTask = GetCurrentTask();
    auto *pTaskAIObj = GetCurrentTaskAIObj();

    switch (pTask->dwTaskType)
    {
    case AITASK_ABORDAGE:
        if (pTaskAIObj)
            SetDestinationPoint(pTaskAIObj->GetPos() + pTaskAIObj->GetAbordagePoint(GetAIShip()));
        break;
    case AITASK_ATTACK:
        if (pTaskAIObj)
            SetDestinationPoint(pTaskAIObj->GetPos() + pTaskAIObj->GetAttackPoint(GetAIShip()));
        GetAIShip()->GetSpeedController()->AddSpeed(0.5f);
        DoAttackRotate();
        break;
    case AITASK_DEFEND:
        if (pTaskAIObj)
            SetDestinationPoint(pTaskAIObj->GetPos() + pTaskAIObj->GetFollowPoint(GetAIShip()));
        break;
    case AITASK_MOVE:
        SetDestinationPoint(pTask->vTaskPnt);
        break;
    case AITASK_RUNAWAY:
        FindRunAwayPoint();
        break;
    case AITASK_BRANDER:
        if (pTaskAIObj)
            SetDestinationPoint(pTaskAIObj->GetPos());
        break;
    case AITASK_FOLLOW:
        if (pTaskAIObj)
            SetDestinationPoint(pTaskAIObj->GetPos() + pTaskAIObj->GetFollowPoint(GetAIShip()));
        break;
    case AITASK_DRIFT:
        GetAIShip()->GetSpeedController()->SetGlobalMultiply(0.0f);
        GetAIShip()->GetRotateController()->SetGlobalMultiply(0.0f);
        break;
    }
}

void AIShipTaskController::Execute(float fDeltaTime)
{
    if (fDeltaTime <= 0.0f)
        return;

    auto bTaskCompleted = CheckCurrentTaskComplete();
    if (bTaskCompleted)
    {
        if (Secondary.isActive())
            Secondary.SetActive(false);
        bTaskCompleted = CheckCurrentTaskComplete();
        if (bTaskCompleted)
        {
            // find new primary task
        }
    }
    else
    {
        DoTask(fDeltaTime);
    }
}

void AIShipTaskController::SetNewTask(uint32_t dwPriority, uint32_t _dwNewTaskType, CVECTOR &vPnt)
{
    auto *pTask = GetTask(dwPriority);

    pTask->SetActive(true);
    pTask->dwTaskType = _dwNewTaskType;
    pTask->pATaskCharacter = nullptr;
    // pTask->pTaskAIObj = null;
    pTask->vTaskPnt = vPnt;
}

void AIShipTaskController::SetNewTask(uint32_t dwPriority, uint32_t _dwNewTaskType, ATTRIBUTES *_pATaskCharacter)
{
    auto *pTask = GetTask(dwPriority);

    pTask->SetActive(true);
    pTask->pATaskCharacter = _pATaskCharacter;
    pTask->dwTaskType = _dwNewTaskType;
}

bool AIShipTaskController::isAttack(ATTRIBUTES *pAOtherCharacter)
{
    auto *const pTask = GetCurrentTask();
    if (pTask && pTask->pATaskCharacter == pAOtherCharacter) //~!~
    {
        switch (pTask->dwTaskType)
        {
        case AITASK_ABORDAGE:
        case AITASK_ATTACK:
            return true;
            break;
        }
    }
    return false;
}

void AIShipTaskController::Save(CSaveLoad *pSL)
{
    pSL->SaveFloat(fZapasDistance);
    Primary.Save(pSL);
    Secondary.Save(pSL);
}

void AIShipTaskController::Load(CSaveLoad *pSL)
{
    fZapasDistance = pSL->LoadFloat();

    Primary.Load(pSL);
    Secondary.Load(pSL);
}
