#include "shared/mast_msg.h"
#include "ship.h"
#include "string_compare.hpp"

#define MAST_IDENTIFY "mast"
#define MAST_FIRST 1

BOOL SHIP::BuildContour(CVECTOR *vContour, int32_t &iNumVContour)
{
    iNumVContour = 0;

    auto *pEnt = GetModel();
    Assert(pEnt);

    CVECTOR vSrc, vDst, vP, vP1, vP2;
    float fY, fRight, fLeft, fUp, fDown, fRes, fZMax, fZMin, fZStep, fZMinStep;

    auto bDefaultContour = false;
    bool bRes = core.GetEntityPointer(model_id);
    Assert(bRes);

    CMatrix mTemp;
    pEnt->mtx = mTemp;
    pEnt->Update();

    fY = 0.0f; // must loaded from ini file???

    fRight = State.vBoxSize.x + 5.0f;
    fLeft = -fRight;
    fUp = State.vBoxSize.z + 5.0f;
    fDown = -fUp;

    // up point trace
    vSrc = CVECTOR(0.05f, fY, fUp);
    vDst = CVECTOR(0.0f, fY, 0.0f);
    fRes = pEnt->Trace(vSrc, vDst);
    if (fRes <= 1.0f)
    {
        vP1 = vSrc + fRes * (vDst - vSrc);
        vP1.y = 0.0f;
        vContour[0] = vP1;
    }
    else
    {
        core.Trace("SHIP: Up trace error, ship %s", static_cast<const char*>(GetAShip()->GetAttribute("Name")));
        bDefaultContour = true;
#ifdef _WIN32 // Beep
        Beep(1000, 200);
#endif
    }
    // Assert(fRes<=1.0f);

    // down point trace
    vSrc = CVECTOR(0.01f, fY, fDown);
    vDst = CVECTOR(0.0f, fY, 0.0f);
    fRes = pEnt->Trace(vSrc, vDst);
    if (fRes <= 1.0f)
        vP2 = vSrc + fRes * (vDst - vSrc);
    else
    {
        core.Trace("SHIP: Down trace error, ship %s", static_cast<const char*>(GetAShip()->GetAttribute("Name")));
        bDefaultContour = true;
#ifdef _WIN32 // Beep
        Beep(1000, 200);
#endif
    }
    // Assert(fRes<=1.0f);

    // bDefaultContour = true;
    if (!bDefaultContour)
    {
        // calculate integer number of steps for z-coord
        fZMax = vP1.z;
        fZMin = vP2.z;
        fZMinStep = 2.0f;
        fZStep = (fZMax - fZMin) / static_cast<float>(static_cast<int32_t>((fZMax - fZMin) / fZMinStep));
        auto iNumSteps = static_cast<int32_t>((fZMax - fZMin) / fZStep);

        // trace left and right sides of ship
        for (int32_t i = 1; i < iNumSteps - 1; i++)
        {
            // left trace
            auto fZ = fZMax - static_cast<float>(i) * fZStep;

            vSrc = CVECTOR(fLeft, fY, fZ);
            vDst = CVECTOR(0.0f, fY, fZ);
            // core.SetEntityScanLayer("balls_trace");
            fRes = pCollide->Trace(model_id, vSrc, vDst);
            Assert(fRes <= 1.0f);
            vP = vSrc + fRes * (vDst - vSrc);
            if (fRes <= 1.0f)
            {
                vP.y = 0.0f;
                vP.x *= 1.1f;
                vContour[1 + iNumVContour] = vP;
                iNumVContour++;
            }
            // right trace
        }
        // try to find far point of ship back
        vContour[1 + iNumVContour - 1].z = vP2.z;
    }
    else // setup default contour - wrong situation
    {
        auto fDZ = State.vBoxSize.z / 2.0f;
        auto fDX = State.vBoxSize.x / 2.0f;
        vContour[0] = CVECTOR(0.0f, 0.0f, fDZ);
        vContour[1] = CVECTOR(fDX * 0.8f, 0.0f, fDZ * 0.6f);
        vContour[2] = CVECTOR(fDX * 1.0f, 0.0f, 0.0f);
        vContour[3] = CVECTOR(fDX * 0.8f, 0.0f, -fDZ * 0.6f);
        vContour[4] = CVECTOR(fDX * 0.6f, 0.0f, -fDZ * 1.0f);
        iNumVContour = 4;
    }

    // mirror copy
    for (int32_t i = 0; i < iNumVContour; i++)
    {
        vContour[1 + i + iNumVContour] = vContour[1 + (iNumVContour - 1) - i];
        vContour[1 + i + iNumVContour].x = -vContour[1 + i + iNumVContour].x;
    }
    iNumVContour = iNumVContour * 2 + 1;

    if (!bDefaultContour)
    {
        // build keel contour
        int32_t iDZ = MAX_KEEL_POINTS / 2;
        auto fDZ = iDZ + 1.0f;
        for (int32_t i = 0; i < MAX_KEEL_POINTS; i++)
        {
            float fZ;
            if (i == iDZ)
                fZ = 0.0f;
            if (i < iDZ)
                fZ = fZMax / fDZ * static_cast<float>(iDZ - i);
            if (i > iDZ)
                fZ = fZMin / fDZ * static_cast<float>(i - iDZ);

            vSrc = CVECTOR(0.0f, -100.0f, fZ);
            vDst = CVECTOR(0.001f, 10.0f, fZ);
            fRes = pCollide->Trace(model_id, vSrc, vDst);
            Assert(fRes <= 1.0f);
            vKeelContour[i] = vSrc + fRes * (vDst - vSrc);
        }

        bKeelContour = true;
    }

    return true;
}

bool SHIP::BuildMasts()
{
    char str[256];

    auto *pEnt = GetModel();
    Assert(pEnt);

    // build mast list
    int32_t iNum, iIdx = 0;
    while (true)
    {
        auto *pNode = static_cast<NODE *>(pEnt->GetNode(iIdx));

        if (!pNode)
        {
            if (iNumMasts)
            {
                auto *pAQMasts = GetACharacter()->FindAClass(GetACharacter(), "Ship.MastsQty");
                if (!pAQMasts)
                    pAQMasts = GetACharacter()->CreateSubAClass(GetACharacter(), "Ship.MastsQty");
                pAQMasts->SetAttributeUseDword(nullptr, iNumMasts);
            }
            break;
        }
        const auto *const cNodeName = pNode->GetName();

        if (storm::iEquals(cNodeName, MAST_IDENTIFY, std::size(MAST_IDENTIFY) - 1))
        {
            CVECTOR vBSize, vBCenter, vUp, vDown, vTemp;

            auto *pAMasts = GetACharacter()->FindAClass(GetACharacter(), "Ship.Masts");
            if (!pAMasts)
                pAMasts = GetACharacter()->CreateSubAClass(GetACharacter(), "Ship.Masts");

            sscanf(static_cast<const char *>(&cNodeName[std::size(MAST_IDENTIFY) - 1]), "%d", &iNum);
            pMasts.resize(iNumMasts + 1);

            auto *pM = &pMasts[iNumMasts];
            pM->iMastNum = iNum;
            pM->bBroken = false;
            pM->fDamage = 0.0f;
            pM->pNode = pNode;

            GEOS::INFO ginfo;
            pNode->geo->GetInfo(ginfo);

            vBSize = CVECTOR(ginfo.boxsize.x, ginfo.boxsize.y, ginfo.boxsize.z);
            vBCenter = CVECTOR(ginfo.boxcenter.x, ginfo.boxcenter.y, ginfo.boxcenter.z);

            vUp = pNode->glob_mtx * (vBCenter + vBSize / 2.0f);
            vDown = pNode->glob_mtx * (vBCenter - vBSize / 2.0f);

            vTemp = (vUp + vDown) / 2.0f;

            if (iNum == MAST_FIRST) // x aligned mast
            {
                pM->vSrc = CVECTOR(vTemp.x, vDown.y, vDown.z);
                pM->vDst = CVECTOR(vTemp.x, vUp.y, vUp.z);
            }
            else
            {
                pM->vSrc = CVECTOR(vTemp.x, vDown.y, vTemp.z);
                pM->vDst = CVECTOR(vTemp.x, vUp.y, vTemp.z);
            }

            sprintf_s(str, "%s", pNode->GetName());
            auto *pAMast = pAMasts->FindAClass(pAMasts, str);
            if (pAMast && pAMast->GetAttributeAsFloat() >= 1.0f)
            {
                pM->fDamage = 1.0f;
                pM->bBroken = true;
                entid_t ent;
                ent = core.CreateEntity("mast");
                core.Send_Message(ent, "lpii", MSG_MAST_SETGEOMETRY, pNode, GetId(), GetModelEID());
                core.EraseEntity(ent);
                // iIdx--;
            }
            else
                pAMasts->SetAttributeUseFloat(str, 0.0f);

            iNumMasts++;
        }
        iIdx++;
    }
    return true;
}

bool SHIP::BuildHulls()
{
    char str[256];

    auto *pEnt = GetModel();
    Assert(pEnt);

    // build hull list
    int32_t iNum, iIdx = 0;
    while (true)
    {
        auto *pNode = (NODE *)pEnt->GetNode(iIdx);
        if (!pNode)
            break;
        const auto *const cNodeName = pNode->GetName();
        if (storm::iEquals(cNodeName, HULL_IDENTIFY, std::size(HULL_IDENTIFY) - 1))
        {
            CVECTOR vBSize, vBCenter, vUp, vDown, vTemp;

            auto *pAHulls = GetACharacter()->FindAClass(GetACharacter(), "Ship.Hulls");
            if (!pAHulls)
                pAHulls = GetACharacter()->CreateSubAClass(GetACharacter(), "Ship.Hulls");

            sscanf((const char *)&cNodeName[std::size(HULL_IDENTIFY) - 1], "%d", &iNum);
            pHulls.resize(iNumHulls + 1);

            hull_t *pM = &pHulls[iNumHulls];
            pM->iHullNum = iNum;
            pM->bBroken = false;
            pM->fDamage = 0.0f;
            pM->pNode = pNode;

            GEOS::INFO ginfo;
            pNode->geo->GetInfo(ginfo);

            vBSize = CVECTOR(ginfo.boxsize.x, ginfo.boxsize.y, ginfo.boxsize.z);
            vBCenter = CVECTOR(ginfo.boxcenter.x, ginfo.boxcenter.y, ginfo.boxcenter.z);

            vUp = pNode->glob_mtx * (vBCenter + vBSize / 2.0f);
            vDown = pNode->glob_mtx * (vBCenter - vBSize / 2.0f);

            vTemp = (vUp + vDown) / 2.0f;

            pM->vSrc = CVECTOR(vTemp.x, vDown.y, vTemp.z);
            pM->vDst = CVECTOR(vTemp.x, vUp.y, vTemp.z);

            sprintf_s(str, "%s", pNode->GetName());
            auto *pAHull = pAHulls->FindAClass(pAHulls, str);
            if (pAHull && pAHull->GetAttributeAsFloat() >= 1.0f)
            {
                pM->fDamage = 1.0f;
                pM->bBroken = true;
                entid_t ent;
                ent = core.CreateEntity("hull");
                core.Send_Message(ent, "lpii", MSG_HULL_SETGEOMETRY, pNode, GetId(), GetModelEID());
                core.EraseEntity(ent);
                // iIdx--;
            }
            else
                pAHulls->SetAttributeUseFloat(str, 0.0f);

            iNumHulls++;
        }
        iIdx++;
    }
    return true;
}
