#include "ai_ship.h"

AIShipRotateController::AIShipRotateController(AIShip *pShip)
{
    fRotate = 0.0f;
    dwRotateNum = 0;
    fRotateSmooth = 0.0f;
    fRotateTime = 0.0f;
    fRotateMode = 0.0f;
    fGlobalMultiply = 1.0f;

    SetAIShip(pShip);
}

AIShipRotateController::~AIShipRotateController()
{
}

bool AIShipRotateController::Init()
{
    return true;
}

void AIShipRotateController::Execute(float fDeltaTime)
{
    if (GetAIShip()->isMainCharacter())
        return;
    fRotateTime += fDeltaTime;
    dwRotateNum++;
    if (fRotate < -1.0f)
        fRotate = -1.0f;
    if (fRotate > 1.0f)
        fRotate = 1.0f;
    fRotateSmooth += fRotate;
    if (fRotateTime >= 1.0f)
    {
        fRotateMode = fRotateSmooth / static_cast<float>(dwRotateNum);
        fRotateTime = 0.0f;
        fRotateSmooth = 0.0f;
        dwRotateNum = 0;
    }

    fRotate = 0.0f;
    GetAIShip()->GetShipBasePointer()->SetRotate(fGlobalMultiply * fRotateMode);

    fGlobalMultiply = 1.0f;
}

void AIShipRotateController::Realize(float fDeltaTime)
{
    // AIHelper::pRS->Print(0,0,"%f",__fRotate);
}

void AIShipRotateController::AddRotate(float _fRotate)
{
    fRotate += _fRotate;
}

void AIShipRotateController::SetGlobalMultiply(float _fGlobalMultiply)
{
    if (_fGlobalMultiply < fGlobalMultiply)
        fGlobalMultiply = _fGlobalMultiply;
}

void AIShipRotateController::Save(CSaveLoad *pSL) const
{
    pSL->SaveDword(dwRotateNum);
    pSL->SaveFloat(fRotateMode);
    pSL->SaveFloat(fRotateTime);
    pSL->SaveFloat(fRotateSmooth);
    pSL->SaveFloat(fRotate);
    pSL->SaveFloat(fGlobalMultiply);
}

void AIShipRotateController::Load(CSaveLoad *pSL)
{
    dwRotateNum = pSL->LoadDword();
    fRotateMode = pSL->LoadFloat();
    fRotateTime = pSL->LoadFloat();
    fRotateSmooth = pSL->LoadFloat();
    fRotate = pSL->LoadFloat();
    fGlobalMultiply = pSL->LoadFloat();
}
