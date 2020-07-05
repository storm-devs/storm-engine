#include "AIHelper.h"
#include "AIShip.h"
#include "AIFort.h"

AIHelper		Helper;

VDX8RENDER		* AIHelper::pRS = null;
ISLAND_BASE		* AIHelper::pIsland = null;
COLLIDE			* AIHelper::pCollide = null;
ATTRIBUTES		* AIHelper::pASeaCameras = null;

float			AIHelper::fGravity = 9.81f;

AIHelper::AIHelper() : aCharacters(_FL_), aMainCharacters(_FL_)
{
	pRelations = null;
}

AIHelper::~AIHelper()
{
	Uninit();
}

bool AIHelper::Uninit()
{
	//pRS = null;
	pIsland = null;
	//pCollide = null;
	pASeaCameras = null;
	aCharacters.DelAll();
	aMainCharacters.DelAll();
	dwRelationSize = 0;
	DELETE(pRelations);
	return true;
}

bool AIHelper::SetDevice()
{
	pRS = (VDX8RENDER *)api->CreateService("dx8render"); Assert(pRS);
	pCollide = (COLLIDE*)api->CreateService("COLL"); Assert(pCollide);

	return true;
}

bool AIHelper::Init()
{
	pIsland = null;

	ENTITY_ID eidIsland;
	if (api->FindClass(&eidIsland, "island", 0))
		pIsland = (ISLAND_BASE*)api->GetEntityPointer(&eidIsland);

	return true;
}

ATTRIBUTES * AIHelper::GetMainCharacter(ATTRIBUTES * pACharacter)
{
	dword dwIdx = aCharacters.Find(pACharacter);
	if (dwIdx != INVALID_ARRAY_INDEX)
		return aMainCharacters[dwIdx];
	return null;	
}

void AIHelper::AddCharacter(ATTRIBUTES * pACharacter, ATTRIBUTES * pAMainCharacter)
{
	dword dwIdx = aCharacters.Find(pACharacter);
	if (dwIdx != INVALID_ARRAY_INDEX)
	{
		aMainCharacters[dwIdx] = pAMainCharacter;
		return;
	}
	aCharacters.Add(pACharacter);
	aMainCharacters.Add(pAMainCharacter);
}

void AIHelper::CalculateRelations()
{
	dword x, y;

	DELETE(pRelations);
	dwRelationSize = aCharacters.Size();
	pRelations = NEW dword[SQR(dwRelationSize)];
	for (y=0;y<dwRelationSize;y++)
		for (x=0;x<dwRelationSize;x++) if (x != y)
		{
			VDATA * pData = api->Event(GET_RELATION_EVENT,"ll",GetIndex(aMainCharacters[y]),GetIndex(aMainCharacters[x])); Assert(pData);
			*GetRelation(y,x) = dword(pData->GetLong());
		}
		else
		{
			*GetRelation(y,x) = RELATION_FRIEND;
		}
}

dword AIHelper::FindIndex(ATTRIBUTES * pACharacter) const
{
	return aCharacters.Find(pACharacter);
}

dword * AIHelper::GetRelation(dword x, dword y) const
{
	Assert(x < dwRelationSize && y < dwRelationSize);
	return &pRelations[x + y * dwRelationSize];
}

dword AIHelper::GetRelationSafe(ATTRIBUTES * pA1, ATTRIBUTES * pA2) const
{
	Assert(pA1 && pA2);
	if (dwRelationSize == 0) return RELATION_NEUTRAL;
	dword dw1 = FindIndex(pA1); if (dw1 == INVALID_ARRAY_INDEX) return RELATION_NEUTRAL;
	dword dw2 = FindIndex(pA2); if (dw2 == INVALID_ARRAY_INDEX) return RELATION_NEUTRAL;
	return *GetRelation(dw1,dw2);
}

dword AIHelper::GetRelation(ATTRIBUTES * pA1, ATTRIBUTES * pA2) const
{
	Assert(pA1 && pA2);
	dword dw1 = FindIndex(pA1); Assert(dw1 != INVALID_ARRAY_INDEX);
	dword dw2 = FindIndex(pA2); Assert(dw2 != INVALID_ARRAY_INDEX);
	return *GetRelation(dw1,dw2);
}

bool AIHelper::isFriend(ATTRIBUTES * pA1, ATTRIBUTES * pA2) const
{
	return GetRelation(pA1,pA2) == RELATION_FRIEND;
}

bool AIHelper::isEnemy(ATTRIBUTES * pA1, ATTRIBUTES * pA2) const
{
	return GetRelation(pA1,pA2) == RELATION_ENEMY;
}

bool AIHelper::isNeutral(ATTRIBUTES * pA1, ATTRIBUTES * pA2) const
{
	return GetRelation(pA1,pA2) == RELATION_NEUTRAL;
}

VAI_INNEROBJ * AIHelper::FindAIInnerObj(ATTRIBUTES * pACharacter)
{
	VAI_INNEROBJ * pObj = null;
	if (AIFort::pAIFort) pObj = AIFort::pAIFort->FindFort(pACharacter);
	if (!pObj) pObj = AIShip::FindShip(pACharacter);
	return pObj;
}

void AIHelper::Print(float x, float y, float fScale, char * pFormat, ...) 
{
	char cBuffer[2048];

	va_list args;
	va_start(args,pFormat);
	_vsnprintf(cBuffer, sizeof(cBuffer), pFormat, args);
	va_end(args);
 
	pRS->ExtPrint(FONT_DEFAULT, 0xFFFFFFFF, 0x00000000, ALIGN_CENTER, 0, fScale, 0, 0, long(x), long(y), cBuffer);
}

void AIHelper::Print3D(CVECTOR vPos, float dy, float fScale, char * pFormat, ...) 
{
	CMatrix			mtx, view, prj;
	char			Buff_4k[2048];
	D3DVIEWPORT8	vp;
	MTX_PRJ_VECTOR	vrt;

	pRS->GetTransform(D3DTS_VIEW, view);
	pRS->GetTransform(D3DTS_PROJECTION, prj);
	mtx.EqMultiply(view, prj);
	view.Transposition();

	pRS->GetViewport(&vp);
	float fWidth = vp.Width * 0.5f;
	float fHeight = vp.Height * 0.5f;

	mtx.Projection(&vPos, &vrt, 1, fWidth, fHeight, sizeof(CVECTOR), sizeof(MTX_PRJ_VECTOR));
	vPos = CVECTOR(vrt.x, vrt.y, vrt.z);

	va_list args;
	va_start(args,pFormat);
	_vsnprintf(Buff_4k, sizeof(Buff_4k), pFormat, args);
	va_end(args);

	pRS->ExtPrint(FONT_DEFAULT, 0xFFFFFFFF, 0x00000000, ALIGN_CENTER, 0, fScale, 0, 0, (long)vPos.x, long(vPos.y + dy), Buff_4k);
}

void AIHelper::Save(CSaveLoad * pSL)
{
	pSL->SaveFloat(fGravity);
	pSL->SaveDword(dwRelationSize);
	pSL->SaveBuffer((const char *)pRelations, dwRelationSize * dwRelationSize * sizeof(dword));

	pSL->SaveAPointer("seacameras", pASeaCameras);

	pSL->SaveDword(aCharacters.Size());
	for (dword i=0; i<aCharacters.Size(); i++) pSL->SaveAPointer("character", aCharacters[i]);

	pSL->SaveDword(aMainCharacters.Size());
	for (i=0; i<aMainCharacters.Size(); i++) pSL->SaveAPointer("character", aMainCharacters[i]);
}

void AIHelper::Load(CSaveLoad * pSL)
{
	fGravity = pSL->LoadFloat();
	dwRelationSize = pSL->LoadDword();
	pSL->LoadBuffer((char**)&pRelations);

	pASeaCameras = pSL->LoadAPointer("seacameras");

	dword dwNum = pSL->LoadDword();
	for (dword i=0; i<dwNum; i++) aCharacters.Add(pSL->LoadAPointer("character"));

	dwNum = pSL->LoadDword();
	for (i=0; i<dwNum; i++) aMainCharacters.Add(pSL->LoadAPointer("character"));
}
