#include "ship.h"

#define MAST_IDENTIFY		"mast"
#define MAST_FIRST			1

BOOL NetShip::BuildContour(CVECTOR * vContour,long & iNumVContour)
{
	iNumVContour = 0;

	MODEL * pEnt = GetModel(); Assert(pEnt);

	CVECTOR	vSrc, vDst, vP, vP1, vP2;
	float	fY, fRight, fLeft, fUp, fDown, fRes,
			fZMax, fZMin, fZStep, fZMinStep;

	bool bDefaultContour = false;
	bool bRes = api->ValidateEntity(&model_id); Assert(bRes);

	CMatrix		mTemp;
	pEnt->mtx = mTemp;
	pEnt->Update();

	fY = 0.0f;	// must loaded from ini file???

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
		api->Trace("SHIP: Up trace error, ship %s", GetAShip()->GetAttribute("Name"));
		bDefaultContour = true;
#ifndef _XBOX
		Beep(1000, 200);
#endif
	}
	//Assert(fRes<=1.0f);

	// down point trace
	vSrc = CVECTOR(0.01f,fY,fDown);		vDst = CVECTOR(0.0f,fY,0.0f);
	fRes = pEnt->Trace(vSrc,vDst);
	if (fRes <= 1.0f)
		vP2 = vSrc + fRes * (vDst - vSrc);
	else
	{
		api->Trace("SHIP: Down trace error, ship %s", GetAShip()->GetAttribute("Name"));
		bDefaultContour = true;
#ifndef _XBOX
		Beep(1000, 200);
#endif
	}
	//Assert(fRes<=1.0f);
	
	//bDefaultContour = true;
	if (!bDefaultContour)
	{
		// calculate integer number of steps for z-coord
		fZMax = vP1.z;
		fZMin = vP2.z;
		fZMinStep = 2.0f;
		fZStep = (fZMax - fZMin) / float(long((fZMax - fZMin) / fZMinStep));
		long iNumSteps = long((fZMax - fZMin) / fZStep);

		// trace left and right sides of ship
		for (long i=1; i<iNumSteps-1; i++)
		{
			// left trace
			float fZ = fZMax - float(i) * fZStep;

			vSrc = CVECTOR(fLeft,fY,fZ);	vDst = CVECTOR(0.0f,fY,fZ);
			//api->SetEntityScanLayer("balls_trace");
			fRes = Collide().Trace(model_id,vSrc,vDst);
			Assert(fRes <= 1.0f);
			vP = vSrc + fRes * (vDst - vSrc);
			if (fRes<=1.0f)
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
	else	// setup default contour - wrong situation
	{
		float fDZ = State.vBoxSize.z / 2.0f;
		float fDX = State.vBoxSize.x / 2.0f;
		vContour[0] = CVECTOR(0.0f, 0.0f, fDZ);
		vContour[1] = CVECTOR(fDX * 0.8f, 0.0f, fDZ * 0.6f);
		vContour[2] = CVECTOR(fDX * 1.0f, 0.0f, 0.0f);
		vContour[3] = CVECTOR(fDX * 0.8f, 0.0f, -fDZ * 0.6f);
		vContour[4] = CVECTOR(fDX * 0.6f, 0.0f, -fDZ * 1.0f);
		iNumVContour = 4;
	}

	// mirror copy
	for (long i=0; i<iNumVContour; i++)
	{
		vContour[1+i+iNumVContour] = vContour[1+(iNumVContour-1)-i];
		vContour[1+i+iNumVContour].x = -vContour[1+i+iNumVContour].x;
	}
	iNumVContour = iNumVContour * 2 + 1;

	if (!bDefaultContour)
	{
		// build keel contour
		long iDZ = MAX_KEEL_POINTS / 2;
		float fDZ = iDZ + 1.0f;
		for (i=0; i<MAX_KEEL_POINTS; i++)
		{
			float fZ;
			if (i==iDZ) fZ = 0.0f;
			if (i<iDZ) fZ = fZMax / fDZ * float(iDZ - i);
			if (i>iDZ) fZ = fZMin / fDZ * float(i - iDZ);

			vSrc = CVECTOR(0.0f,-100.0f,fZ);	vDst = CVECTOR(0.001f,10.0f,fZ);
			fRes = Collide().Trace(model_id,vSrc,vDst);
			Assert(fRes<=1.0f);
			vKeelContour[i] = vSrc + fRes * (vDst - vSrc);
		}

		bKeelContour = true;
	}
	
	return true;
}

bool NetShip::BuildMasts()
{
	char str[256];

	MODEL * pEnt = GetModel(); Assert(pEnt);

	// build mast list
	long iNum,iIdx = 0;
	while (true)
	{
		NODE* pNode = (NODE*)pEnt->GetNode(iIdx);
		if (!pNode) break;
		const char *cNodeName = pNode->GetName();
		if (strnicmp(cNodeName,MAST_IDENTIFY,strlen(MAST_IDENTIFY)) == 0)
		{
			CVECTOR vBSize,vBCenter,vUp,vDown,vTemp;

			ATTRIBUTES * pAMasts = AP()->FindAClass(AP(),"Ship.Masts");
			if (!pAMasts)
				pAMasts = AP()->CreateSubAClass(AP(), "Ship.Masts");

			sscanf((const char*)&cNodeName[strlen(MAST_IDENTIFY)],"%d",&iNum);
			pMasts = (mast_t*)RESIZE(pMasts,sizeof(mast_t) * (iNumMasts+1));

			mast_t * pM = &pMasts[iNumMasts];
			pM->iMastNum = iNum;
			pM->bBroken = false;
			pM->fDamage = 0.0f;
			pM->pNode = pNode;

			GEOS::INFO ginfo;
			pNode->geo->GetInfo(ginfo);
			
			vBSize		= CVECTOR(ginfo.boxsize.x,ginfo.boxsize.y,ginfo.boxsize.z);
			vBCenter	= CVECTOR(ginfo.boxcenter.x,ginfo.boxcenter.y,ginfo.boxcenter.z);
			
			vUp		= pNode->glob_mtx * (vBCenter + vBSize / 2.0f);
			vDown	= pNode->glob_mtx * (vBCenter - vBSize / 2.0f);

			vTemp	= (vUp + vDown) / 2.0f;

			if (iNum==MAST_FIRST)	// x aligned mast
			{
				pM->vSrc = CVECTOR(vTemp.x,vDown.y,vDown.z);
				pM->vDst = CVECTOR(vTemp.x,vUp.y,vUp.z);
			}
			else
			{
				pM->vSrc = CVECTOR(vTemp.x,vDown.y,vTemp.z);
				pM->vDst = CVECTOR(vTemp.x,vUp.y,vTemp.z);
			}

			sprintf(str,"%s",pNode->GetName());
			ATTRIBUTES * pAMast = pAMasts->FindAClass(pAMasts,str);
			if (pAMast && pAMast->GetAttributeAsFloat()>=1.0f)
			{
				pM->fDamage = 1.0f;
				pM->bBroken = true;
				ENTITY_ID ent;
				api->CreateEntity(&ent,"mast");
				api->Send_Message(ent,"lpii", MSG_MAST_SETGEOMETRY, pNode, GetID(), GetModelEID());
				api->DeleteEntity(ent);
				//iIdx--;
			}
			else
				pAMasts->SetAttributeUseFloat(str,0.0f);

			iNumMasts++;
		}
		iIdx++;
	}
	return true;
}
