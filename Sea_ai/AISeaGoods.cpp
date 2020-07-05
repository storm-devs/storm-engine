#include "AISeaGoods.h"

AISeaGoods::AISeaGoods() : aGoods(_FL_, 12), aShips(_FL_)
{
	pSea = null;
	pGeoService = null;

	bDeleteGoodAnyway = false;

	dtCheckShips.Setup(FRAND(1.0f), 1.0f);
}

AISeaGoods::~AISeaGoods()
{
	for (dword i=0; i<aGoods.Size(); i++)
	{
		if (aGoods[i]->pGeo) pGeoService->DeleteGeometry(aGoods[i]->pGeo);
		aGoods[i]->sModel.DelAll();
		aGoods[i]->aItems.DelAll();
		DELETE(aGoods[i]);
	}
	aGoods.DelAll();
}

bool AISeaGoods::Init()
{
	SetDevice();
	return true;
}

void AISeaGoods::SetDevice()
{
	pGeoService = (VGEOMETRY*)api->CreateService("geometry"); Assert(pGeoService);
}

void AISeaGoods::Execute(dword dwDeltaTime)
{
	ENTITY_ID	EID;

	float fDeltaTime = float(dwDeltaTime) * 0.001f;

	if (!pSea && api->FindClass(&EID, "sea", 0))
		pSea = (SEA_BASE*)api->GetEntityPointer(&EID);
	
	if (!pSea) return;

	for (dword i=0; i<aGoods.Size(); i++)
		for (dword j=0; j<aGoods[i]->aItems.Size(); j++)
		{
			item_t * pI = &aGoods[i]->aItems[j];
			pI->fTime -= fDeltaTime;
			
			pI->vPos.y = pSea->WaveXZ(pI->vPos.x, pI->vPos.z, &pI->vNormal);

			if (pI->fTime <= 0.0f)
			{
				pI->vPos.y -= fabsf(pI->fTime) * 0.05f;
				if (pI->fTime < -20.0f)
				{
					aGoods[i]->aItems.ExtractNoShift(j); j--;
					continue;
				}
			}

		if (dtCheckShips.Update(dwDeltaTime))
		{
			aShips.Empty();

			// enumerate ships
			if (api->FindClass(&EID, "ship", 0)) do
			{
				aShips.Add((SHIP_BASE*)api->GetEntityPointer(&EID));
			} while (api->FindClassNext(&EID));

			// check ships
			for (dword k=0; k<aShips.Size(); k++)
			{
				SHIP_BASE * pS = aShips[k];
				ATTRIBUTES * pACharacter = pS->GetACharacter();
				int iCharacterIndex = GetIndex(pS->GetACharacter());
				float fDistance = sqrtf(~(pS->State.vPos - pI->vPos));
				if (fDistance <= pS->State.vBoxSize.z * fDistanceMultiply)
				{
					VDATA * pVData = api->Event(SHIP_EAT_SWIM_GOOD, "llsl", iCharacterIndex, pI->iCharIndex, pI->sGoodName, pI->iQuantity);
					if (pVData->GetLong() || bDeleteGoodAnyway) { aGoods[i]->aItems.ExtractNoShift(j); j--; break; }
				}
			}
		}
	}
}

void AISeaGoods::Realize(dword dwDeltaTime)
{
	if (!pSea) return;

	AIHelper::pRS->SetRenderState(D3DRS_LIGHTING, true);

	for (dword i=0; i<aGoods.Size(); i++) if (aGoods[i]->pGeo)
		for (dword j=0; j<aGoods[i]->aItems.Size(); j++)
		{
			item_t * pI = &aGoods[i]->aItems[j];
	
			// set world matrix for item
			CMatrix m; m.BuildPosition(pI->vPos.x, pI->vPos.y, pI->vPos.z);
			
			AIHelper::pRS->SetTransform(D3DTS_WORLD, m);
			aGoods[i]->pGeo->Draw((GEOS::PLANE*)AIHelper::pRS->GetPlanes(), 0, null);
		}

	AIHelper::pRS->SetRenderState(D3DRS_LIGHTING, false);
}

dword AISeaGoods::AttributeChanged(ATTRIBUTES * pAttribute)
{
	ATTRIBUTES * pParent = pAttribute->GetParent();

	if (*pAttribute == "Add")
	{
		for (dword i=0; i<aGoods.Size(); i++) if (aGoods[i]->sModel == sTmpModel)
		{
			aGoods[i]->aItems.Add(TmpItem);
			return 0;
		}
		string	sName; 
		goods_t * pG = NEW goods_t; aGoods[aGoods.Add()] = pG;
		pG->sModel = sTmpModel;
		pG->aItems.Add(TmpItem);
		sName.Format("%s\\%s",(const char*)sModelPath,(const char*)sTmpModel);
		pG->pGeo = pGeoService->CreateGeometry(sName, 0, 0);
		return 0;
	}

	if (*pAttribute == "CharIndex") { TmpItem.iCharIndex = pAttribute->GetAttributeAsDword(); return 0; }
	if (*pAttribute == "Time")		{ TmpItem.fTime = pAttribute->GetAttributeAsFloat(); return 0; }
	if (*pAttribute == "Quantity")	{ TmpItem.iQuantity = pAttribute->GetAttributeAsDword(); return 0; }
	if (*pAttribute == "Model")		{ sTmpModel = pAttribute->GetThisAttr(); return 0; }
	if (*pAttribute == "Good")		{ strcpy(TmpItem.sGoodName, pAttribute->GetThisAttr()); return 0; }
	
	if (*pParent == "Pos")
	{
		if (*pAttribute == "x")		{ TmpItem.vPos.x = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "z")		{ TmpItem.vPos.z = pAttribute->GetAttributeAsFloat(); return 0; }
	}

	if (*pAttribute == "ModelsPath")		{ sModelPath = pAttribute->GetThisAttr(); return 0; }
	if (*pAttribute == "DeleteGoodAnyway")	{ bDeleteGoodAnyway = pAttribute->GetAttributeAsDword() == 1; return 0; }
	if (*pAttribute == "DistanceMultiply")	{ fDistanceMultiply = pAttribute->GetAttributeAsFloat(); return 0; }

	return 0;
}
