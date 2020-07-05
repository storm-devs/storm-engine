#include <stdio.h>
#include "ISLAND.h"
#include "Foam.h"
#include "..\engine\program\sea_ai\Script_Defines.h"
#include "..\common_h\weather_base.h"

INTERFACE_FUNCTION
CREATE_CLASS(ISLAND)
CREATE_CLASS(CoastFoam)

#define TGA_DATA_CHUNK		0xC001F00D

#define	HMAP_EMPTY			0
#define	HMAP_START			2.0f
#define HMAP_NUMBERS		(255.0f - HMAP_START)
#define HMAP_MAXHEIGHT		-20.0f

#define SEA_BED_NODE_NAME	"seabed"

#define DMAP_SIZE		2048

ISLAND::ISLAND() : aForts(_FL_), aSpheres(_FL_)
{
	bForeignModels = false;
	pRS = null;
	pGS = null;
	pDepthMap = null;
	pShadowMap = null;
	pIslandTraceWalker = null;
	bDrawReflections = false;

	fCurrentImmersion = 0.0f;

	ZERO(AIFortEID);
}

ISLAND::~ISLAND()
{
	Uninit();

	DELETE(pIslandTraceWalker);
}

void ISLAND::Uninit()
{
	for (dword i=0; i<aSpheres.Size(); i++) api->DeleteEntity(aSpheres[i]);
	aSpheres.DelAll();
	DELETE(pDepthMap);
	DELETE(pShadowMap);

	if (!bForeignModels) 
	{
		api->DeleteEntity(model_id);
		api->DeleteEntity(seabed_id);
	}
}

bool ISLAND::Init()
{
	api->LayerAdd("system_messages", GetID(), 1);
	SetDevice();

	// calc optimization
	for (dword i=0; i<256; i++)
	{
		fDepthHeight[i] = float(HMAP_MAXHEIGHT / HMAP_NUMBERS * (float(i) - HMAP_START) );
		if (i == HMAP_EMPTY) fDepthHeight[i] = HMAP_MAXHEIGHT;
	}

	return true;
}
 
void ISLAND::SetDevice()
{
	api->LayerCreate("island_trace", true, false);

	pCollide = (COLLIDE*)api->CreateService("COLL"); Assert(pCollide);
	pRS = (VDX8RENDER*)api->CreateService("dx8render"); Assert(pRS);
	pGS = (VGEOMETRY *)api->CreateService("geometry"); Assert(pGS);
	pIslandTraceWalker = api->LayerGetWalker("island_trace"); Assert(pIslandTraceWalker);
}

bool bView = false;

void ISLAND::Realize(dword Delta_Time)
{
	dword	dwAmbient, dwAmbientOld;

#ifndef _XBOX
	// update foam
	/*if (GetAsyncKeyState('Z') < 0 && GetAsyncKeyState('G') < 0) 
	{
		DoZapSuperGenerator();
	}
	if (GetAsyncKeyState('U') < 0)
	{
		CreateHeightMap(cFoamDir, cModelsID);
		//CreateShadowMap(cModelsDir, cModelsID);
	}       */
#endif

	if (bForeignModels) return;

	MODEL * pModel = (MODEL*)api->GetEntityPointer(&model_id); Assert(pModel);

	dword bFogEnable;
	dword bLighting;
	pRS->GetRenderState(D3DRS_FOGENABLE, &bFogEnable);
	pRS->GetRenderState(D3DRS_AMBIENT, &dwAmbientOld);
	pRS->GetRenderState(D3DRS_LIGHTING, &bLighting);

	pRS->SetRenderState(D3DRS_LIGHTING, false);
	dwAmbient = dwAmbientOld & 0xFF;
	
	CVECTOR vCamPos, vCamAng;
	float fOldNear, fOldFar, fPerspective;
	pRS->GetCamera(vCamPos, vCamAng, fPerspective);
	pRS->GetNearFarPlane(fOldNear, fOldFar);

	float fRadius = sqrtf(Sqr(vBoxSize.x / 2.0f) + Sqr(vBoxSize.z / 2.0f));
	float fCamDistance = sqrtf(~(vCamPos - vBoxCenter));
	float fMaxDistance = fCamDistance + fRadius;

	fCurrentImmersion = 0.0f;
	if (fCamDistance > fImmersionDistance)
		fCurrentImmersion = (fCamDistance / fImmersionDistance - 1.0f) * fImmersionDepth;

	CMatrix mTemp;
	mTemp.BuildPosition(0.0f, -fCurrentImmersion, 0.0f);
	pModel->mtx = mIslandOld * mTemp;

	float fOldFogDensity;
	float fIslandFogDensity; 

	fIslandFogDensity = AttributesPointer->GetAttributeAsFloat("FogDensity", 0.0f);

	if (aForts.Size() && !AIFortEID.pointer)
	{
		api->FindClass(&AIFortEID, "AIFort", 0);
	}

	pRS->GetRenderState(D3DRS_FOGDENSITY, (dword*)&fOldFogDensity);
	pRS->SetRenderState(D3DRS_FOGDENSITY, F2DW(fIslandFogDensity));
	long j;
	for (j=long(fMaxDistance / fOldFar); j>=0; j--)
	{
		if (j != 0) pRS->SetRenderState(D3DRS_ZWRITEENABLE, false);
		pRS->SetNearFarPlane((j==0) ? fOldNear : fOldFar * float(j), fOldFar * float(j + 1));
		pModel->Realize(Delta_Time);
		pRS->SetRenderState(D3DRS_LIGHTING, true);
		D3DLIGHT8 lt, ltold; ZERO(lt);
		lt.Type = D3DLIGHT_POINT;
		lt.Diffuse.a = 0.0f;	lt.Diffuse.r = 1.0f;	lt.Diffuse.g = 1.0f;	lt.Diffuse.b = 1.0;
		lt.Ambient.r  = 1.0f;	lt.Ambient.g  = 1.0f;	lt.Ambient.b  = 1.0f;
		lt.Specular.r = 1.0f;	lt.Specular.g = 1.0f;	lt.Specular.b = 1.0f;
		lt.Position.x = 0.0f;	lt.Position.y = 0.0f;	lt.Position.z = 0.0f; 
		lt.Attenuation0 = 1.0f;
		lt.Range = 1e9f;
		pRS->GetLight(0, &ltold);
		pRS->SetLight(0, &lt);
		for (dword k=0; k<aForts.Size(); k++)
		{
			CMatrix mOld = ((MODEL*)aForts[k].pointer)->mtx;
			((MODEL*)aForts[k].pointer)->mtx = mOld * mTemp;

			api->Send_Message(AIFortEID, "li", AI_MESSAGE_FORT_SET_LIGHTS, aForts[k]);
			((ENTITY*)aForts[k].pointer)->Realize(Delta_Time);
			api->Send_Message(AIFortEID, "li", AI_MESSAGE_FORT_UNSET_LIGHTS, aForts[k]);

			((MODEL*)aForts[k].pointer)->mtx = mOld;
		}
		pRS->SetLight(0, &ltold);
		pRS->SetRenderState(D3DRS_LIGHTING, false);
		pRS->SetRenderState(D3DRS_ZWRITEENABLE, true);
	}
	pRS->SetRenderState(D3DRS_FOGDENSITY, F2DW(fOldFogDensity));

	pRS->SetNearFarPlane(fOldNear , fOldFar / 2.0f);
	if (!bDrawReflections)
	{
		pRS->SetRenderState(D3DRS_FOGENABLE, false);
		//pRS->SetRenderState(D3DRS_AMBIENT, RGB(dwAmbient/4,dwAmbient/4,dwAmbient/4));

		MODEL * pSeaBed = (MODEL*)api->GetEntityPointer(&seabed_id);
		if (pSeaBed) pSeaBed->Realize(Delta_Time);
	}

	pRS->SetNearFarPlane(fOldNear, fOldFar);

	pRS->SetRenderState(D3DRS_LIGHTING, false);
	pRS->SetRenderState(D3DRS_FOGENABLE, bFogEnable);
	pRS->SetRenderState(D3DRS_AMBIENT, dwAmbientOld);
	pRS->SetRenderState(D3DRS_LIGHTING, bLighting);

	dword i;
	for (i=0; i<aSpheres.Size(); i++)
	{
		MODEL * pModel = (MODEL*)_CORE_API->GetEntityPointer(&aSpheres[i]);
		CVECTOR vPos = AIPath.GetPointPos(i);
		if (pModel)
			pModel->mtx.BuildPosition(vPos.x,5.0f,vPos.z);
	}

#ifndef _XBOX
	if (api->Controls->GetDebugAsyncKeyState('O') < 0) bView ^= 1;
	if (bView)
	{
		array<RS_LINE>	aLines(_FL_);
		for (i=0;i<AIPath.GetNumEdges();i++)
		{
			AIFlowGraph::edge_t * pE = AIPath.GetEdge(i);
			RS_LINE *pRL = &aLines[aLines.Add()];
			pRL->dwColor = 0xFFFFFF;
			pRL->vPos = AIPath.GetPointPos(pE->dw1);
			pRL = &aLines[aLines.Add()];
			pRL->dwColor = 0xFFFFFF;
			pRL->vPos = AIPath.GetPointPos(pE->dw2);
		}
		CMatrix m;
		pRS->SetTransform(D3DTS_WORLD,m);
		pRS->DrawLines(&aLines[0], aLines.Size() / 2, "AILine");
	}
#endif
}

bool ISLAND::GetDepth(FRECT * pRect, float * fMinH, float * fMaxH)
{
	return false;
}

dword _cdecl ISLAND::ProcessMessage(MESSAGE & message)
{
	ENTITY_ID	eID;
	char		str[256], idstr[256];
	switch (message.Long())
	{
		case MSG_ISLAND_ADD_FORT:
			aForts.Add(message.EntityID());
		break;
		case MSG_LOCATION_ADD_MODEL:
			eID = message.EntityID();
			message.String(sizeof(idstr)-1, idstr);
			message.String(sizeof(str)-1, str);
			AddLocationModel(eID, idstr, str);
		break;
		case MSG_ISLAND_LOAD_GEO:							// from sea
			message.String(sizeof(cFoamDir)-1, cFoamDir);
			message.String(sizeof(cModelsDir)-1, cModelsDir);
			message.String(sizeof(cModelsID)-1, cModelsID);
			Mount(cModelsID, cModelsDir, 0);
			CreateHeightMap(cFoamDir, cModelsID);
			//CreateShadowMap(cModelsDir, cModelsID);
		break;
		case MSG_ISLAND_START:								// from location
			CreateHeightMap(cModelsDir, cModelsID);
			//CreateShadowMap(cModelsDir, cModelsID);
		break;
		case MSG_SEA_REFLECTION_DRAW:
			bDrawReflections = true;
			Realize(0);
			bDrawReflections = false;
		break;
		case MSG_MODEL_SET_MAX_VIEW_DIST:
			api->Send_Message(model_id, "lf", MSG_MODEL_SET_MAX_VIEW_DIST, message.Float());
		break;
	}
	return 1;
}

inline float ISLAND::GetShadowTemp(long iX, long iZ)
{
	if (iX >= 0 && iX < DMAP_SIZE && iZ >= 0 && iZ < DMAP_SIZE) 
		return float(mzShadow.Get(iX,iZ)) / 255.0f;
	return 1.0f;
}

bool ISLAND::GetShadow(float x, float z, float * fRes)
{
	long  iX, iZ;
	float fX = (x - vBoxCenter.x) / fShadowMapStep; FTOL(iX,fX); 
	float fZ = (z - vBoxCenter.z) / fShadowMapStep; FTOL(iZ,fZ); 

	*fRes = GetShadowTemp(iX + mzShadow.GetSizeX() / 2, iZ + mzShadow.GetSizeX() / 2);
	
	return true;
}

void ISLAND::AddLocationModel(ENTITY_ID & eid, char * pIDStr, char * pDir)
{
	Assert(pDir && pIDStr);
	bForeignModels = true;
	strcpy(cModelsDir, pDir);
	strcpy(cModelsID, pIDStr);
	api->LayerAdd("island_trace", eid, 10);
}

inline float ISLAND::GetDepthNoCheck(DWORD iX, DWORD iZ)
{
	//if (!mzDepth.isLoaded()) return HMAP_MAXHEIGHT;

	/*BYTE byHeight = mzDepth.Get(iX,iZ);
	if (byHeight == HMAP_EMPTY) return HMAP_MAXHEIGHT;
	float fHeight = float(HMAP_MAXHEIGHT / HMAP_NUMBERS * (float(byHeight) - HMAP_START) );
	return fHeight;*/

	return fDepthHeight[mzDepth.Get(iX,iZ)];
}

inline float ISLAND::GetDepthCheck(DWORD iX, DWORD iZ)
{
	//if (iX>=iDMapSize) iX = iDMapSize - 1;
	//if (iZ>=iDMapSize) iZ = iDMapSize - 1;
	return GetDepthNoCheck(iX, iZ);
}

bool ISLAND::Check2DBoxDepth(CVECTOR vPos, CVECTOR vSize, float fAngY, float fMinDepth)
{
	//if (!mzDepth.isLoaded()) return false;

	float fCos = cosf(fAngY), fSin = sinf(fAngY);
	for (float z = -vSize.z / 2.0f; z < vSize.z / 2.0f; z += fStepDZ)
		for (float x = -vSize.x / 2.0f; x < vSize.x / 2.0f; x += fStepDX)
		{
			float fDepth, xx = x, zz = z;
			RotateAroundY(xx,zz,fCos,fSin);
			GetDepthFast(vPos.x + xx,vPos.z + zz,&fDepth);
			if (fDepth > fMinDepth) return true;
		}
	return false;
}

bool ISLAND::GetDepthFast(float x, float z, float * fRes)
{
	//if (!mzDepth.isLoaded()) { if (fRes) *fRes = -50.0f; return false; }
	x -= vBoxCenter.x;
	z -= vBoxCenter.z;
	if (fabsf(x) >= vRealBoxSize.x || fabsf(z) >= vRealBoxSize.z) 
	//if (x < -vRealBoxSize.x || z < -vRealBoxSize.z || x > vRealBoxSize.x || z > vRealBoxSize.z) 
	{
		*fRes = -50.0f;
		return false;
	}
	
	long iX, iZ;
	float fX = (x * fStep1divDX) + float(iDMapSize >> 1); FTOL(iX, fX); 
	float fZ = (z * fStep1divDZ) + float(iDMapSize >> 1); FTOL(iZ, fZ);
	
	*fRes = GetDepthNoCheck(iX, iZ);
	return true;
}

bool ISLAND::GetDepth(float x, float z, float * fRes)
{
	//if (!mzDepth.isLoaded()) {if (fRes) *fRes = -50.0f;	return false; }
	x -= vBoxCenter.x;
	z -= vBoxCenter.z;
	if (fabsf(x) >= vRealBoxSize.x || fabsf(z) >= vRealBoxSize.z) 
	//if (x < -vRealBoxSize.x || z < -vRealBoxSize.z || x > vRealBoxSize.x || z > vRealBoxSize.z) 
	{
		*fRes = -50.0f;
		return false;
	}
	
	long iX, iZ;
	float fX = (x * fStep1divDX) + float(iDMapSize >> 1); FTOL(iX, fX); 
	float fZ = (z * fStep1divDZ) + float(iDMapSize >> 1); FTOL(iZ, fZ);

	*fRes = GetDepthNoCheck(iX, iZ);

	return true;
}

bool ISLAND::ActivateCamomileTrace(CVECTOR & vSrc)
{
	float	fRadius = 100.0f;
	long	iNumPetal = 8;
	long	iNumInner = 0;

	for (long i=0;i<iNumPetal;i++)
	{
		TRIANGLE	trg;
		CVECTOR		vDst,vCross;
		float		fAng,fCos,fSin,fRes;
		
		fAng = float(i) / float(iNumPetal) * PIm2;
		fCos = cosf(fAng); fSin = sinf(fAng);

		vDst = vSrc + CVECTOR(fCos * fRadius, 0.0f, fSin * fRadius);
		fRes = Trace(vSrc, vDst);
		if (fRes > 1.0f) continue;
		MODEL * pEnt = (MODEL*)api->GetEntityPointer(&pCollide->GetObjectID()); Assert(pEnt);
		pEnt->GetCollideTriangle(trg);
		vCross = !((trg.vrt[1] - trg.vrt[0]) ^ (trg.vrt[2] - trg.vrt[0]));
		fRes = vCross | (!(vDst - vSrc));
		if (fRes > 0.0f) iNumInner++;
		if (iNumInner>1) return true;
	}

	return false;
}

void ISLAND::CalcBoxParameters(CVECTOR & _vBoxCenter, CVECTOR & _vBoxSize)
{
	GEOS::INFO	ginfo;
	ENTITY_ID	* pEID;
	float		x1 = 1e+8f, x2 = -1e+8f, z1 = 1e+8f, z2 = -1e+8f;
	
	pEID = pIslandTraceWalker->GetID();
	if (pEID) do
	{
		MODEL * pM = (MODEL*)api->GetEntityPointer(pEID); Assert(pM);
		dword i = 0;
		while (true)
		{
			NODE * pN = pM->GetNode(i); i++;
			if (!pN) break;
			pN->geo->GetInfo(ginfo);
			CVECTOR vGlobPos = pN->glob_mtx.Pos();
			CVECTOR vBC = vGlobPos + CVECTOR(ginfo.boxcenter.x, 0.0f, ginfo.boxcenter.z);
			CVECTOR vBS = CVECTOR(ginfo.boxsize.x, 0.0f, ginfo.boxsize.z) / 2.0f;
			if (vBC.x - vBS.x < x1) x1 = vBC.x - vBS.x;
			if (vBC.x + vBS.x > x2) x2 = vBC.x + vBS.x;
			if (vBC.z - vBS.z < z1) z1 = vBC.z - vBS.z;
			if (vBC.z + vBS.z > z2) z2 = vBC.z + vBS.z;
		}

	} while (pEID = pIslandTraceWalker->GetIDNext());
	_vBoxCenter	= CVECTOR((x1 + x2) / 2.0f, 0.0f, (z1 + z2) / 2.0f);
	_vBoxSize	= CVECTOR(x2 - x1, 0.0f, z2 - z1);
}

void ISLAND::CreateDirectories(char * pDir)
{
#ifndef _XBOX
	char sCurDir[256], sTemp[256];

	fio->_GetCurrentDirectory(sizeof(sCurDir), sCurDir);
	if (strlen(sCurDir) && sCurDir[strlen(sCurDir) - 1] != '\\') strcat(sCurDir, "\\");

	char * pLast, * pStr;

	pLast = pStr = pDir;
	while (true)
	{
		pStr = strchr(pStr, '\\');
		if (!pStr) break;
		strncpy(sTemp, pLast, pStr - pLast); sTemp[pStr - pLast] = '\0';
		pLast = ++pStr;
		strcat(sCurDir, sTemp); strcat(sCurDir, "\\");
		BOOL bOk = fio->_CreateDirectory(sCurDir, null);
	}
#endif
}

bool ISLAND::CreateShadowMap(char * pDir, char * pName)
{
	string	sDir;
	char	fname[256];
	
	ENTITY_ID		ent;
	WEATHER_BASE	* pWeather;
	if (!api->FindClass(&ent, "Weather",0)) _THROW("No found WEATHER entity!");
	pWeather = (WEATHER_BASE*)api->GetEntityPointer(&ent); Assert(pWeather);

	sDir.Format("resource\\foam\\%s\\%s\\", pDir, AttributesPointer->GetAttribute("LightingPath")); sDir.CheckPath();
	sprintf(fname, "%s%s.tga", (const char*)sDir, pName);

	CreateDirectories((char*)sDir.GetBuffer());

	fShadowMapSize = 2.0f * Max(vRealBoxSize.x, vRealBoxSize.z) + 1024.0f;
	fShadowMapStep = fShadowMapSize / DMAP_SIZE;

	if (mzShadow.Load(string(fname) + ".zap")) return true;
	
	// try to load tga file
	HANDLE hFile = fio->_CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		TGA_H	tga_head;

		fio->_ReadFile(hFile, &tga_head, sizeof(tga_head), 0);
		dword dwSize = tga_head.width;
		pShadowMap = NEW byte[dwSize * dwSize];
		fio->_ReadFile(hFile, pShadowMap, dwSize * dwSize, 0);
		fio->_CloseHandle(hFile);

		mzShadow.DoZip(pShadowMap, dwSize);
		mzShadow.Save(string(fname) + ".zap");
		DELETE(pShadowMap);
		return true;
	}

	pShadowMap = NEW byte[DMAP_SIZE * DMAP_SIZE];

	float	fX, fZ;
	dword	x, z;
	CVECTOR vSun;
	pWeather->GetVector(whv_sun_pos, &vSun); // CVECTOR(15000.0f, 2000.0f, 15000.0f);
	vSun = 100000.0f * !vSun;

	for (fZ = -fShadowMapSize / 2.0f, z = 0; fZ < fShadowMapSize / 2.0f; fZ += fShadowMapStep, z++)
		for (fX = -fShadowMapSize / 2.0f, x = 0; fX < fShadowMapSize / 2.0f; fX += fShadowMapStep, x++)
		{
			//dword x = dword((fX + fShadowMapSize / 2.0f) / fShadowMapStep);
			//dword z = dword((fZ + fShadowMapSize / 2.0f) / fShadowMapStep);
			//if (x == 774) _asm int 3
			CVECTOR vSrc = CVECTOR(fX, 2.0f, fZ) + vBoxCenter;
			CVECTOR vDst = vSrc + vSun;
			float fRes = Trace(vSrc, vDst);
			pShadowMap[x + z * DMAP_SIZE] = 255;
			if (fRes <= 1.0f)
			{
				float fLen = sqrtf(~(vSun - vSrc)) * fRes;
				float fValue = Bring2Range(50.0f, 254.0f, 0.0f, 500.0f, fLen);
				pShadowMap[x + z * DMAP_SIZE] = byte(fValue);
			}
			if (GetDepthFast(vSrc.x, vSrc.z, &fRes))
			{
				if (fRes >= -0.001f) pShadowMap[x + z * DMAP_SIZE] = 0;
			}
		}

	SaveTga8(fname, pShadowMap, DMAP_SIZE, DMAP_SIZE);

	Blur8(&pShadowMap, DMAP_SIZE);
	Blur8(&pShadowMap, DMAP_SIZE);
	Blur8(&pShadowMap, DMAP_SIZE);
	Blur8(&pShadowMap, DMAP_SIZE);

	mzShadow.DoZip(pShadowMap, DMAP_SIZE);
	mzShadow.Save(string(fname) + ".zap");

	DELETE(pShadowMap);

	return true;
}

void ISLAND::Blur8(byte * * pBuffer, dword dwSize)
{
	dword	x, z; 
	byte	* pNewBuffer = NEW byte[dwSize * dwSize];
	dword	dwMask = dwSize - 1;
	// do blur for shadow map
	for (z = 0; z < dwSize; z++)
		for (x = 0; x < dwSize; x++)
		{
			dword dwRes = (*pBuffer)[z * dwSize + x];
			dwRes += (*pBuffer)[((z + 0) & dwMask) * dwSize + ((x + 1) & dwMask)];
			dwRes += (*pBuffer)[((z + 1) & dwMask) * dwSize + ((x + 1) & dwMask)];
			dwRes += (*pBuffer)[((z + 1) & dwMask) * dwSize + ((x + 0) & dwMask)];
			dwRes += (*pBuffer)[((z + 1) & dwMask) * dwSize + ((x - 1) & dwMask)];
			dwRes += (*pBuffer)[((z + 0) & dwMask) * dwSize + ((x - 1) & dwMask)];
			dwRes += (*pBuffer)[((z - 1) & dwMask) * dwSize + ((x - 1) & dwMask)];
			dwRes += (*pBuffer)[((z - 1) & dwMask) * dwSize + ((x + 0) & dwMask)];
			dwRes += (*pBuffer)[((z - 1) & dwMask) * dwSize + ((x + 1) & dwMask)];
			dwRes /= 9;

			pNewBuffer[z * dwSize + x] = byte(dwRes); 
		}

	DELETE(*pBuffer);
	*pBuffer = pNewBuffer;
}

bool ISLAND::CreateHeightMap(char * pDir, char * pName)
{
	TGA_H	tga_head;
	string	sDir; 
	char	fname[256], iname[256], str_tmp[256];
	HANDLE	hFile;

	sDir.Format("resource\\\\foam\\\\%s\\\\", pDir); sDir.CheckPath();
	sprintf(fname, "%s%s.tga", (const char*)sDir, pName);
	sprintf(iname, "%s%s.ini", (const char*)sDir, pName);

	CreateDirectories((char*)sDir.GetBuffer());

	// calc center and size
	CalcBoxParameters(vBoxCenter, vRealBoxSize);
	vBoxSize = vRealBoxSize + CVECTOR(50.0f, 0.0f, 50.0f);

	rIsland.x1 = vBoxCenter.x - vBoxSize.x / 2.0f;	rIsland.y1 = vBoxCenter.z - vBoxSize.z / 2.0f;
	rIsland.x2 = vBoxCenter.x + vBoxSize.x / 2.0f;	rIsland.y2 = vBoxCenter.z + vBoxSize.z / 2.0f;

	bool bLoad = mzDepth.Load(string(fname) + ".zap");

	if (!bLoad)
	{
		hFile = fio->_CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			fio->_ReadFile(hFile, &tga_head, sizeof(tga_head), 0);
			iDMapSize = tga_head.width;
			pDepthMap = NEW byte[iDMapSize * iDMapSize];
			fio->_ReadFile(hFile, pDepthMap, iDMapSize * iDMapSize, 0);
			fio->_CloseHandle(hFile);

			mzDepth.DoZip(pDepthMap, iDMapSize);
			mzDepth.Save(string(fname) + ".zap");
			DELETE(pDepthMap);
			bLoad = true;
		}
	}

	if (bLoad)
	{
		iDMapSize = mzDepth.GetSizeX();
		for (iDMapSizeShift=0; iDMapSizeShift<30; iDMapSizeShift++) if (dword(1<<iDMapSizeShift) == iDMapSize) break;
		fStepDX = vBoxSize.x / float(iDMapSize);
		fStepDZ = vBoxSize.z / float(iDMapSize);

		fStep1divDX = 1.0f / fStepDX;
		fStep1divDZ = 1.0f / fStepDZ;

		vBoxSize /= 2.0f;
		vRealBoxSize /= 2.0f;
		
		INIFILE * pI = fio->OpenIniFile(iname);	Assert(pI);

		CVECTOR vTmpBoxCenter, vTmpBoxSize;
		pI->ReadString("Main", "vBoxCenter", str_tmp, sizeof(str_tmp)-1, "1.0,1.0,1.0");
		sscanf(str_tmp, "%f,%f,%f", &vTmpBoxCenter.x, &vTmpBoxCenter.y, &vTmpBoxCenter.z);
		pI->ReadString("Main", "vBoxSize", str_tmp, sizeof(str_tmp)-1, "1.0,1.0,1.0");
		sscanf(str_tmp, "%f,%f,%f", &vTmpBoxSize.x, &vTmpBoxSize.y, &vTmpBoxSize.z);
		if (~(vTmpBoxCenter - vBoxCenter) > 0.1f) 
		{
			api->Trace("Island: vBoxCenter not equal, foam error: %s, distance = %.3f", iname, sqrtf(~(vTmpBoxCenter - vBoxCenter)));
			api->Trace("vBoxCenter = %f,%f,%f", vBoxCenter.x, vBoxCenter.y, vBoxCenter.z);
		}
		if (~(vTmpBoxSize - vBoxSize) > 0.1f) 
		{
			api->Trace("Island: vBoxSize not equal, foam error: %s", iname);
			api->Trace("vBoxSize = %f,%f,%f", vBoxSize.x, vBoxSize.y, vBoxSize.z);
		}

		AIPath.Load(pI);
		AIPath.BuildTable();
		delete pI;

		return true;
	}

	api->Trace("WARN: FOAM: Can't find foam: %s", fname);

#ifdef _XBOX
	iDMapSizeShift = 11;
	iDMapSize = (1<<iDMapSizeShift);

	fStepDX = vBoxSize.x / float(iDMapSize);
	fStepDZ = vBoxSize.z / float(iDMapSize);

	fStep1divDX = 1.0f / fStepDX;
	fStep1divDZ = 1.0f / fStepDZ;

	pDepthMap = NEW byte[iDMapSize * iDMapSize];
	PZERO(pDepthMap, iDMapSize * iDMapSize);

	vBoxSize /= 2.0f;
	vRealBoxSize /= 2.0f;

	mzDepth.DoZip(pDepthMap, iDMapSize);
	DELETE(pDepthMap);

	return true;
#endif

	long iTestSize = long(vBoxSize.x / 1.5f);
	// fixed maximum depth map to 1024 size!!!!!!!
	iDMapSizeShift = 11;
	//for (iDMapSizeShift=0;iDMapSizeShift<10;iDMapSizeShift++) if ((1<<iDMapSizeShift) >= iTestSize) break;
	iDMapSize = (1<<iDMapSizeShift);

	fStepDX = vBoxSize.x / float(iDMapSize);
	fStepDZ = vBoxSize.z / float(iDMapSize);

	fStep1divDX = 1.0f / fStepDX;
	fStep1divDZ = 1.0f / fStepDZ;

	pDepthMap = NEW byte[iDMapSize * iDMapSize];

	float	fEarthPercent = 0.0f;
	float	fX,fZ;

	for (fZ=0; fZ<float(iDMapSize); fZ+=1.0f)
	{
		if ((long(fZ) & 127) == 127) api->Trace("Z = %.0f", fZ);
		for (fX=0; fX<float(iDMapSize); fX+=1.0f)
		{
			long iIdx = long(fX) + long(fZ) * iDMapSize;
			pDepthMap[iIdx] = 255;
			float fXX = (fX - float(iDMapSize)/2.0f) * fStepDX;
			float fZZ = (fZ - float(iDMapSize)/2.0f) * fStepDZ;
			CVECTOR vSrc(fXX, 0.0f, fZZ), vDst(fXX, -500.0f, fZZ + 0.001f);
			vSrc += vBoxCenter; vDst += vBoxCenter;
			float fRes = Trace(vSrc, vDst);
			Assert(_isnan(fRes) == false);
			if (fRes<=1.0f)	// island ocean floor exist
			{
				float fHeight = sqrtf(~(fRes * (vDst - vSrc)));
				if (fHeight>-HMAP_MAXHEIGHT) 
				{
					fHeight = -HMAP_MAXHEIGHT;
				}
				// Activate camomile trace!
				if (ActivateCamomileTrace(vSrc))
					pDepthMap[iIdx] = BYTE(HMAP_START);
				else
					pDepthMap[iIdx] = (unsigned char)(HMAP_START + float(HMAP_NUMBERS) * fHeight / -HMAP_MAXHEIGHT);
			}
			else	// check for up direction
			{
				vSrc = CVECTOR(fXX, 0.0f, fZZ);
				vDst = CVECTOR(fXX, 1500.0f, fZZ + 0.001f);
				vSrc += vBoxCenter; vDst += vBoxCenter;
				float fRes = Trace(vSrc,vDst);
				if (fRes<=1.0f || ActivateCamomileTrace(vSrc))
				{
					pDepthMap[iIdx] = BYTE(HMAP_START);
				}
			}
			if (pDepthMap[iIdx] == BYTE(HMAP_START)) fEarthPercent += 1.0f;
		}
	}

	vBoxSize /= 2.0f;
	vRealBoxSize /= 2.0f;

	SaveTga8(fname, pDepthMap, iDMapSize, iDMapSize);

	mzDepth.DoZip(pDepthMap, iDMapSize);
	mzDepth.Save(string(fname) + ".zap");
	DELETE(pDepthMap);

	INIFILE * pI = fio->OpenIniFile(iname); 
	if (!pI)
	{
		pI = fio->CreateIniFile(iname,false);
		Assert(pI);
	}
	char str[512];
	pI->WriteString("Main", "DepthFile", fname);
	sprintf(str, "%f,%f,%f", vBoxCenter.x, vBoxCenter.y, vBoxCenter.z);
	pI->WriteString("Main", "vBoxCenter", str);
	sprintf(str, "%f,%f,%f", vBoxSize.x, vBoxSize.y, vBoxSize.z);
	pI->WriteString("Main", "vBoxSize", str);
	delete pI;

	return true;
}

bool ISLAND::SaveTga8(char * fname, byte * pBuffer, dword dwSizeX, dword dwSizeY)
{
	TGA_H tga_head;
	
	ZERO(tga_head);

	tga_head.type = 3;
	tga_head.width = (WORD)dwSizeX;
	tga_head.height = (WORD)dwSizeY;
	tga_head.bpp = 8;
	tga_head.attr8 = 8;

	HANDLE hFile = fio->_CreateFile(fname, GENERIC_WRITE, FILE_SHARE_READ, OPEN_ALWAYS);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		api->Trace("Island: Can't create island file! %s", fname);
		return false;
	}
	api->fio->_WriteFile(hFile, &tga_head, sizeof(tga_head), 0);
	fio->_WriteFile(hFile, pBuffer, dwSizeX * dwSizeY, 0);
	fio->_CloseHandle(hFile);
	
	return true;
}

bool ISLAND::Mount(char * fname, char * fdir, ENTITY_ID * eID)
{
	ENTITY_ID	lighter_id;
	string		sRealFileName;
	string		sModelPath, sLightPath;

	Uninit();

	SetName(fname);

	sRealFileName.Format("%s\\%s", fdir, fname); sRealFileName.CheckPath();
	
	api->CreateEntity(&model_id, "MODELR");
	api->Send_Message(model_id, "ls", MSG_MODEL_SET_LIGHT_PATH, AttributesPointer->GetAttribute("LightingPath"));
	api->Send_Message(model_id, "ls", MSG_MODEL_LOAD_GEO, (char*)sRealFileName.GetBuffer());

	// extract subobject(sea_bed) to another model
	MODEL * pModel = (MODEL*)api->GetEntityPointer(&model_id);
	NODE * pNode = pModel->FindNode(SEA_BED_NODE_NAME);
	if (pNode)
		seabed_id = pNode->Unlink2Model();
	else 
		api->Trace("Island: island %s has no sea bed, check me!", fname);

	api->LayerAdd("island_trace", model_id, 10);
	api->LayerAdd("island_trace", seabed_id, 10);
	api->LayerAdd("rain_drops", model_id, 100);

	MODEL * pSeaBedModel = (MODEL*)api->GetEntityPointer(&seabed_id);

	mIslandOld = pModel->mtx;
	if (pSeaBedModel) mSeaBedOld = pSeaBedModel->mtx;

	api->FindClass(&lighter_id, "lighter", 0);
	/*sModelPath.Format("islands\\%s\\", fname); sModelPath.CheckPath();
	api->Send_Message(lighter_id, "ss", "ModelsPath", (char*)sModelPath);
	sLightPath.Format("%s", AttributesPointer->GetAttribute("LightingPath")); sLightPath.CheckPath();
	api->Send_Message(lighter_id, "ss", "LightPath", (char*)sLightPath);*/
	
	api->Send_Message(lighter_id, "ssi", "AddModel", fname, model_id);
	string sSeaBedName = string(fname) + "_seabed";
	api->Send_Message(lighter_id, "ssi", "AddModel", (char*)sSeaBedName.GetBuffer(), seabed_id);

	fImmersionDistance = AttributesPointer->GetAttributeAsFloat("ImmersionDistance", 3000.0f);
	fImmersionDepth = AttributesPointer->GetAttributeAsFloat("ImmersionDepth", 25.0f);

	//CreateHeightMap(fname);

	// load geometry for reflection and add it to reflection layer

	/*for (dword i=0;i<AIPath.GetNumPoints();i++)
	{
		ENTITY_ID eid;
		api->CreateEntity(&eid,"MODELR");
		api->Send_Message(eid,"ls",MSG_MODEL_LOAD_GEO,"mirror");
		api->LayerAdd("sea_realize",eid,10000);
		aSpheres.Add(eid);
	}*/

	//AIFlowGraph::Path * pPath = FindPath(CVECTOR(-10000.0f,0.0f,-10000.0f),CVECTOR(10000.0f,0.0f,10000.0f));
	//DELETE(pPath);

	return true;
}

float ISLAND::Cannon_Trace(long iBallOwner, const CVECTOR & vSrc, const CVECTOR & vDst)
{
	float fRes = Trace(vSrc,vDst);
	if (fRes<=1.0f)
	{
		CVECTOR vTemp = vSrc + fRes * (vDst - vSrc);
		api->Event(BALL_ISLAND_HIT,"lfff",iBallOwner,vTemp.x,vTemp.y,vTemp.z);
	}
	return fRes; 
}

float ISLAND::Trace(const CVECTOR & vSrc, const CVECTOR & vDst)
{
	return pCollide->Trace(*pIslandTraceWalker, vSrc, vDst, null, 0);
}

// Path section
bool ISLAND::GetMovePoint(CVECTOR & vSrc, CVECTOR & vDst, CVECTOR & vRes)
{
	// check for one side 
	dword	i,j;
	vRes = vDst;
	vSrc.y = vDst.y = 0.1f;

	if ((vSrc.x <= rIsland.x1 && vDst.x <= rIsland.x1) || (vSrc.x >= rIsland.x2 && vDst.x >= rIsland.x2) || 
		(vSrc.z <= rIsland.y1 && vDst.z <= rIsland.y1) || (vSrc.z >= rIsland.y2 && vDst.z >= rIsland.y2))
			return false;

	// one simple trace vSrc - vDst
	if (Trace(vSrc,vDst)>=1.0f) return false;

	CVECTOR vDir = !(vDst - vSrc);

	array<AIFlowGraph::npoint_t>	* PointsSrc, * PointsDst;

	PointsSrc = AIPath.GetNearestPoints(vSrc);
	PointsDst = AIPath.GetNearestPoints(vDst);

	dword dwSizeSrc = ((*PointsSrc).Size() > 8) ? 8 : (*PointsSrc).Size();
	dword dwSizeDst = ((*PointsDst).Size() > 8) ? 8 : (*PointsDst).Size();

	for (i=0;i<dwSizeDst;i++)
		(*PointsDst)[i].fTemp = Trace(vDst,AIPath.GetPointPos((*PointsDst)[i].dwPnt));

	float	fMaxDistance = 1e9f;
	dword	dwI = INVALID_ARRAY_INDEX,dwJ;
	for (i=0; i<dwSizeSrc; i++)
	{
		if (Trace(vSrc,AIPath.GetPointPos((*PointsSrc)[i].dwPnt)) < 1.0f) continue;
		float fDist1 = sqrtf(~(vSrc - AIPath.GetPointPos((*PointsSrc)[i].dwPnt)));
		if (fDist1 < 80.0f) continue;
		for (j=0; j<dwSizeDst; j++) if ((*PointsDst)[j].fTemp > 1.0f)
		{
			//if (Trace(vDst,AIPath.GetPointPos((*PointsDst)[j].dwPnt)) < 1.0f) continue;
			float fDist2 = sqrtf(~(vDst - AIPath.GetPointPos((*PointsDst)[j].dwPnt)));
			float fDistance = AIPath.GetPathDistance((*PointsSrc)[i].dwPnt,(*PointsDst)[j].dwPnt);
			float fTotalDist = fDistance + fDist1 + fDist2;
			if (fTotalDist < fMaxDistance && fTotalDist > 0.0f)
			{
				fMaxDistance = fTotalDist;
				dwI = i;
				dwJ = j;
			}
		}
	}

	if (INVALID_ARRAY_INDEX != dwI) vRes = AIPath.GetPointPos((*PointsSrc)[dwI].dwPnt);

	DELETE(PointsSrc); DELETE(PointsDst);

	return true;
}

/*AIFlowGraph::VectorPath	* ISLAND::FindPath(CVECTOR & vSrc, CVECTOR & vDst)
{
	AIFlowGraph::VectorPath		* pVPath;

	// check for free path

	// check for one side 
	if ((vSrc.x <= rIsland.x1 && vDst.x <= rIsland.x1) || (vSrc.x >= rIsland.x2 && vDst.x >= rIsland.x2) || 
		(vSrc.z <= rIsland.y1 && vDst.z <= rIsland.y1) || (vSrc.z >= rIsland.y2 && vDst.z >= rIsland.y2))
	{
		return null;
	}
	// search to near points
	array<AIFlowGraph::npoint_t> & Points = AIPath.GetNearestPoints(vSrc);
	dword dwPnt1 = Points[0].dwPnt;
	Points = AIPath.GetNearestPoints(vDst);
	dword dwPnt2 = Points[0].dwPnt;
	
	pVPath = AIPath.GetVectorPath(dwPnt1,dwPnt2);
	if (!pVPath) return null;

	return pVPath;
}
*/