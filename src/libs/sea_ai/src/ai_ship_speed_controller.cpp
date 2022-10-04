#include "ai_ship.h"

AIShipSpeedController::AIShipSpeedController(AIShip *pShip)
{
    fSpeed = 0.0f;
    dwSpeedNum = 0;
    fSpeedSmooth = 0.0f;
    fSpeedTime = 0.0f;
    fGlobalMultiply = 1.0f;
    fTopSpeed = 1.0f;
    SetAIShip(pShip);
}

AIShipSpeedController::~AIShipSpeedController()
{
}

bool AIShipSpeedController::Init()
{
    return true;
}

void AIShipSpeedController::Execute(float fDeltaTime)
{
    if (GetAIShip()->isMainCharacter())
        return;

    fSpeedTime += fDeltaTime;
    dwSpeedNum++;
    fSpeedSmooth += fSpeed;
    if (fSpeedTime >= 2.0f)
    {
        fSpeedSmooth = Clamp(fSpeedSmooth / static_cast<float>(dwSpeedNum));
        if (fSpeedSmooth > fTopSpeed)
            fSpeedSmooth = fTopSpeed;
        GetAIShip()->GetShipBasePointer()->SetSailState(fGlobalMultiply * fSpeed);
        fSpeedTime = 0.0f;
        fSpeedSmooth = 0.0f;
        dwSpeedNum = 0;
    }

    fGlobalMultiply = 1.0f;
    fSpeed = 0.0f;
    fTopSpeed = 1.0f;
}

void AIShipSpeedController::Realize(float fDeltaTime)
{
}

void AIShipSpeedController::TopSpeed(float _fSpeed)
{
    fTopSpeed = _fSpeed;
}

void AIShipSpeedController::MulSpeed(float _fSpeed)
{
    fSpeed *= _fSpeed;
}

void AIShipSpeedController::AddSpeed(float _fSpeed)
{
    fSpeed += _fSpeed;
}

void AIShipSpeedController::SetGlobalMultiply(float _fGlobalMultiply)
{
    if (_fGlobalMultiply < fGlobalMultiply)
        fGlobalMultiply = _fGlobalMultiply;
}

void AIShipSpeedController::Save(CSaveLoad *pSL) const
{
    pSL->SaveDword(dwSpeedNum);
    pSL->SaveFloat(fSpeedSmooth);
    pSL->SaveFloat(fSpeed);
    pSL->SaveFloat(fSpeedTime);
    pSL->SaveFloat(fTopSpeed);
    pSL->SaveFloat(fGlobalMultiply);
}

void AIShipSpeedController::Load(CSaveLoad *pSL)
{
    dwSpeedNum = pSL->LoadDword();
    fSpeedSmooth = pSL->LoadFloat();
    fSpeed = pSL->LoadFloat();
    fSpeedTime = pSL->LoadFloat();
    fTopSpeed = pSL->LoadFloat();
    fGlobalMultiply = pSL->LoadFloat();
}
