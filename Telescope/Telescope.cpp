#include <stdio.h>
#include <stdlib.h>
#include "..\common_h\messages.h"
#include "..\common_h\exs.h"
#include "..\common_h\CVECTOR.h"
#include "..\common_h\messages.h"
#include "..\common_h\geos.h"
#include "..\common_h\geometry.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\ship_base.h"
#include "..\common_h\model.h"
#include "..\common_h\geos.h"
#include "..\common_h\sea_base.h"
#include "..\engine\program\events.h"

#include "TELESCOPE.h"

INTERFACE_FUNCTION
CREATE_CLASS(TELESCOPE)	

CVECTOR pos, ang, destPos;
CMatrix worldM;
ENTITY_ID cameraID;

dword testTime = 0;
float fdx, fdy;
//--------------------------------------------------------------------
TELESCOPE::TELESCOPE()
	:status(INACTIVE)
	,mainTextureID(0)
	,zoomK(0.0f)
	,changeStatusTime(0)
	,updateTime(0)
	,shipWalker(0)
	,shipsCount(0)
{
	visualizationInfo.shownItems = 0;
}

//--------------------------------------------------------------------
TELESCOPE::~TELESCOPE()
{
	GUARD(TELESCOPE::~TELESCOPE)

	if (mainTextureID)
		renderer->TextureRelease(mainTextureID);

	if (shipWalker)
		delete shipWalker;

	UNGUARD
}

//--------------------------------------------------------------------
bool TELESCOPE::Init()
{
	GUARD(TELESCOPE::Init)

	fdx = 0; fdy = 0;

	renderer = (VDX8RENDER *) _CORE_API->CreateService("dx8render");

	CVECTOR pos, ang;
	renderer->GetCamera(pos, ang, originalPerspective);

	float w,h;
	IDirect3DSurface8 *renderTarget;
	D3DSURFACE_DESC renderDesc;
	renderer->GetRenderTarget(&renderTarget);
	renderTarget->GetDesc(&renderDesc);
	w = (float) renderDesc.Width;
	h = (float) renderDesc.Height;
	renderTarget->Release();

	// main sprite init
	FillSpriteVertex(mainSprite[0], 0, 0, 0.1f, 0xFFFFFFFF, 0.0f, 0.0f);
	FillSpriteVertex(mainSprite[1], 0, h, 0.1f, 0xFFFFFFFF, 0.0f, 1.0f);
	FillSpriteVertex(mainSprite[2], w, h, 0.1f, 0xFFFFFFFF, 1.0f, 1.0f);
	FillSpriteVertex(mainSprite[3], w, 0, 0.1f, 0xFFFFFFFF, 1.0f, 0.0f);

	//ship trace init
	collide = (COLLIDE *)_CORE_API->CreateService("coll");
	shipClassID = _CORE_API->Class_Name2Code("SHIP");

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
dword _cdecl TELESCOPE::ProcessMessage(MESSAGE & message)
{
	GUARD(TELESCOPE::ProcessMessage)

	long code = message.Long();
	dword outValue = 0;

	switch (code)
	{
	case MSG_TELESCOPE_INIT_ARRAYS:
		{
			nationsArray = message.ScriptVariablePointer();
			shipTypesArray = message.ScriptVariablePointer();
			goodsArray = message.ScriptVariablePointer();

			api->FindClass(&cameraID, "DECK_CAMERA", 0);
			shipWalker = _CORE_API->LayerGetWalker("ship_cannon_trace");

			//fort trace init
			ENTITY_ID islandID, islandModelID;
			_CORE_API->FindClass(&islandID, "ISLAND", 0);
			island = (ISLAND_BASE *) _CORE_API->GetEntityPointer(&islandID);
			if (island)
			{
				islandModelID = island->GetModelEID();
				islandModel = (MODEL *) _CORE_API->GetEntityPointer(&islandModelID);
			}

			// fort
			api->FindClass(&fortID, "AIFORT", 0);
			fort = (VAI_OBJBASE *) api->GetEntityPointer(&fortID);

			//find all ships
			tShipInfo *foamInfo = &shipInfo[0];
			ENTITY_ID shipID;
			if (_CORE_API->FindClass(&shipID, "ship", 0))
			{
				do
				{
					foamInfo->ship = (SHIP_BASE*) _CORE_API->GetEntityPointer(&shipID);
					foamInfo->shipModel = foamInfo->ship->GetModel();
					//foamInfo->shipModel->GetNode(0)->geo->GetInfo(foamInfo->hullInfo);
					foamInfo->enabled = true;

					ATTRIBUTES *mainAttributes = foamInfo->ship->GetACharacter();
					if (mainAttributes->GetAttribute("MainCharacter"))
						foamInfo->myShip = true;
					else
						foamInfo->myShip = false;

					foamInfo = &shipInfo[++shipsCount];
				}
				while (_CORE_API->FindClassNext(&shipID));
			}
		}
		break;
		case MSG_SHIP_DELETE:
			{
				ATTRIBUTES *attrs = message.AttributePointer();
				tShipInfo *foamInfo = 0;
				if (attrs)
					for (int ship = 0; ship < shipsCount; ship++)
					{
						if (attrs == shipInfo[ship].ship->GetACharacter())
							shipInfo[ship].enabled = false;

							return outValue;
					}
			}
			break;
	case MSG_TELESCOPE_REQUEST:
		switch (status)
		{
		case INACTIVE:
			SetActive(true);
			break;
		case ACTIVE:
			SetActive(false);
			break;
		}
		break;
	case MSG_TELESCOPE_SET_TYPE:
		{
			tTelescopeConfig config;
			message.String(256, config.texture);
			config.zoom         = message.Float();
			config.nShownItems  = message.Long();
			config.activateTime = message.Long();
			config.updateTiming = message.Long();
			SetType(config/*message.Long()*/);
		}
		break;
	}

	return outValue;
	UNGUARD
}

//--------------------------------------------------------------------
void TELESCOPE::Realize(dword _dTime)
{
	GUARD(TELESCOPE::Realize)

	if (status == INACTIVE)
		return;
/*
	RS_LINE lines[2];
	lines[0].dwColor = 0xFFFFFF;
	lines[0].vPos = pos;
	lines[0].vPos.y -= 1.f;
	lines[1].dwColor = 0xFFFFFF;
	lines[1].vPos = destPos;
	lines[1].vPos.y -= 1.f;

	renderer->DrawLines(lines, 1, "AILINE");
*/
	if (mainTextureID)
		renderer->TextureSet(0, mainTextureID);
	renderer->DrawSprites(&mainSprite[0], 1, "telescope");

	UNGUARD
}

//--------------------------------------------------------------------
void TELESCOPE::Execute(dword _dTime)
{
	GUARD(TELESCOPE::Execute)

	CONTROL_STATE cState;

	_CORE_API->Controls->GetControlState("TelescopeIn", cState);
	if ((cState.state == CST_ACTIVATED) && (status == INACTIVE))
	{	
		_CORE_API->Event("MSG_TELESCOPE_REQUEST", "");
		//api->Trace("TELESCOPE: Making active");
	}

	_CORE_API->Controls->GetControlState("TelescopeOut", cState);
	if ((cState.state == CST_ACTIVATED) && (status == ACTIVE))
	{
		_CORE_API->Event("MSG_TELESCOPE_REQUEST", "");
		//api->Trace("TELESCOPE: Making inactive");
	}

	HandleStatusChange(_dTime);

	if (status == ACTIVE)
	{
		HandleTargetTest(_dTime);
		renderer->SetPerspective(originalPerspective / zoomK);

		if (entityFound)
			idleTime = 0;
		else
			idleTime += _dTime;
	}

	/*
	_CORE_API->Controls->GetControlState("TelescopeAutoAim", cState);
	if ((cState.state == CST_ACTIVE) && (status == ACTIVE))
	{
		HandleAutoAim();
	}
	*/
	CONTROL_STATE csH, csV;
	api->Controls->GetControlState("DeckCamera_Turn_H",csH);
	api->Controls->GetControlState("DeckCamera_Turn_V",csV);
	static float oldX = csH.fValue;
	static float oldY = csV.fValue;
	float dx = csH.fValue - oldX;
	oldX = csH.fValue;
	float dy = csV.fValue - oldY;
	oldY = csV.fValue;

#ifndef _XBOX
	static const float k1 = .5f;
	static const float k2 = .5f;
#else
	static const float k1 = .0f;
	static const float k2 = .0f;
#endif

	fdx = k1*fabsf(dx) + k2*fdx;
	fdy = k1*fabsf(dy) + k2*fdy;

	if (status == ACTIVE && (fdx < .1f) && (fdy < .1f)) 
		HandleAutoAim();

	UNGUARD
}

//--------------------------------------------------------------------
void TELESCOPE::HandleStatusChange(dword _dTime)
{
	float k;
	switch (status)
	{
	case MAKING_ACTIVE:
		k = 1.0f + (zoomK - 1.0f) * (changeTime/((float) changeStatusTime));
		if (k > zoomK) 
			k = zoomK;
		renderer->SetPerspective(originalPerspective / k);
		changeTime += _dTime;
		if (changeTime >= changeStatusTime)
		{
			status = ACTIVE;
			testTime = updateTime;

			//re-search my ship
			for (int i=0; i<shipsCount; i++)
			{
				if (!shipInfo[i].enabled)
					continue;

					ATTRIBUTES *mainAttributes = shipInfo[i].ship->GetACharacter();
					if (mainAttributes->GetAttribute("MainCharacter"))
						shipInfo[i].myShip = true;
					else
						shipInfo[i].myShip = false;
			}
		}
		break;
	case MAKING_INACTIVE:
		k = 1.0f + (zoomK - 1.0f) * (1.0f - changeTime/((float) changeStatusTime));
		if (k < 1.0f) 
			k = 1.0f;
		renderer->SetPerspective(originalPerspective / k);
		changeTime += _dTime;
		if (changeTime >= changeStatusTime)
		{
			status = INACTIVE;
			_CORE_API->Event(TELESCOPE_ACTIVE,"l", 0);
			_CORE_API->Event("BI_VISIBLE", "l", 1);
		}
		break;
	}
}

//--------------------------------------------------------------------
void TELESCOPE::SetActive(bool _active)
{
	if (_active && (status == ACTIVE))
		return;
	if (_active && (status == MAKING_ACTIVE))
		return;
	if (!_active && (status == INACTIVE))
		return;
	if (!_active && (status == MAKING_INACTIVE))
		return;

	CVECTOR pos, ang;
	float persp = 0.0f;
	renderer->GetCamera(pos, ang, persp);

	if (_active)
	{ //make active
		//renderer->SetPerspective(originalPerspective / 5.0f);
		_CORE_API->Event("BI_VISIBLE", "l", 0);
		_CORE_API->Event(TELESCOPE_ACTIVE,"l", 1);
		changeTime = 0;
		status = MAKING_ACTIVE;
	}
	else
	{
		//renderer->SetPerspective(originalPerspective);
		changeTime = 0;
		status = MAKING_INACTIVE;
	}
}

//--------------------------------------------------------------------
void TELESCOPE::HandleTargetTest(dword _dTime)
{
	if (testTime < updateTime)
	{
		testTime += _dTime;
		return;
	}

	entityFound = false;
	testTime = 0;
	static const float TARGET_TEST_DIST = 50000.0f;
	ENTITY_ID foundEntity;

	renderer->GetTransform(D3DTS_VIEW, (D3DMATRIX *) &worldM.matrix);
	worldM.MulToInv(CVECTOR(0.0f, 0.0f, 0.0f), pos);
	worldM.MulToInv(CVECTOR(0.0f, 0.0f, TARGET_TEST_DIST), destPos);


	float ray = collide->Trace(*shipWalker, pos, destPos, 0, 0);
	foundEntity = collide->GetObjectID();
	if ((ray <= 1.0f) && (foundEntity.class_code == shipClassID))
	{ //really a ship?
		SHIP_BASE *shipBase = (SHIP_BASE *) foundEntity.pointer;
		ATTRIBUTES *mainAttributes = shipBase->GetACharacter();
		if (mainAttributes->GetAttribute("MainCharacter"))
		{
			if(idleTime>SHOW_OBJECT_TIME) api->Event("ReleaseTelescopeInfo",NULL);
			return;
		}
/*
		ATTRIBUTES *shipAttr = mainAttributes->FindAClass(mainAttributes, "Ship");
		strcpy(visualizationInfo.strings[0], shipAttr->GetAttribute("Name"));
		ATTRIBUTES *crewAttr = mainAttributes->FindAClass(mainAttributes, "ship.crew");
		ATTRIBUTES *cannonAttr = mainAttributes->FindAClass(mainAttributes, "ship.cannon");
		ATTRIBUTES *cannonsAttr = shipAttr->FindAClass(shipAttr, "cannons");
		ATTRIBUTES *chargeAttr = cannonsAttr->FindAClass(cannonsAttr, "Charge");
		float shipSpeed = fabsf(shipBase->GetCurrentSpeed());
		char *nationName, *typeName, *goodsName, *hpName, *cannonsQName;

		try
		{
			nationsArray->Get("Name", nationName, mainAttributes->GetAttributeAsDword("Nation"));
			shipTypesArray->Get("Name", typeName, shipAttr->GetAttributeAsDword("Type"));
			shipTypesArray->Get("HP", hpName, shipAttr->GetAttributeAsDword("Type"));
			goodsArray->Get("Name", goodsName, chargeAttr->GetAttributeAsDword("Type"));
			shipTypesArray->Get("CannonsQuantity", cannonsQName, shipAttr->GetAttributeAsDword("Type"));
		}
		catch (...)
		{
		}
		for (int i = 0; i<visualizationInfo.shownItems; i++)
			visualizationInfo.strings[i][0] = 0;

		int hpAll = 0;
		int hpPerc= 0;
		int cannonsQ = 0;
		try
		{
			sscanf(hpName, "%d", &hpAll);
			hpPerc = (100 * shipAttr->GetAttributeAsDword("HP")) / hpAll;
			sscanf(cannonsQName, "%d", &cannonsQ);
		}
		catch (...)
		{
		}

		char eshipType[256];
		int eshipHull = 100;
		int eshipSail = 100;
		int eshipCannons = 16;
		sprintf(eshipType,"%s/%d",typeName,1);
*/
		if(mainAttributes)
			api->Event("SetTelescopeInfo","l", mainAttributes->GetAttributeAsDword("index",-1));
/*		api->Event("SetTelescopeInfo","sslllfll", 
			shipAttr->GetAttribute("Name"),
			typeName,
			hpPerc,
			shipAttr->GetAttributeAsDword("SP"), 
			crewAttr->GetAttributeAsDword("quantity"), 
			shipSpeed,
			cannonsQ, 
			chargeAttr->GetAttributeAsDword("Type"));

		sprintf(visualizationInfo.strings[0], "TYPE: %s", typeName);
		sprintf(visualizationInfo.strings[1], "NAME: %s", shipAttr->GetAttribute("Name"));
		sprintf(visualizationInfo.strings[2], "NATION: %s", nationName);
		sprintf(visualizationInfo.strings[3], "DAMAGE: %s/%s", shipAttr->GetAttribute("HP"), hpName);
		sprintf(visualizationInfo.strings[4], "SPEED: %2.2f", shipSpeed);
		sprintf(visualizationInfo.strings[5], "CREW: %s", crewAttr->GetAttribute("quantity"));
		sprintf(visualizationInfo.strings[6], "CANNON: %s", goodsName);
*/
		idleTime = 0;
		entityFound = true;
	}
	else
	{ //not a ship, search through island
		//!!!DEBUG!!!
		//if(idleTime>SHOW_OBJECT_TIME) api->Event("ReleaseTelescopeInfo",NULL);

		//fort
		if (!fort)
		{
			if(idleTime>SHOW_OBJECT_TIME) api->Event("ReleaseTelescopeInfo",NULL);
			return;
		}

		ray = fort->Trace(pos, destPos);
		if (ray <= 1.f)
		{
			ATTRIBUTES * fortAttributes = fort->GetACharacter();
			if(fortAttributes)
				api->Event("SetTelescopeInfo","l", fortAttributes->GetAttributeAsDword("index",-1));
			entityFound = true;
			idleTime = 0;
			strcpy(visualizationInfo.strings[0], "FORT");
		}
		else
		{
			if(idleTime>SHOW_OBJECT_TIME) api->Event("ReleaseTelescopeInfo",NULL);
		}

		/*
		if (!island || !islandModel)
			return;

		float islandRay = islandModel->Trace(pos, destPos);
		if (islandRay <= 1.0f)
		{ //island found
			//NODE *collideNode = islandModel->GetCollideNode();
			//collideNode->
			idleTime = 0;

			for (int i = 0; i<visualizationInfo.shownItems; i++)
				visualizationInfo.strings[i][0] = 0;

			sprintf(visualizationInfo.strings[0], "TYPE: %s", "ISLAND");
			entityFound = true;
		}
		*/
	}
}

//--------------------------------------------------------------------
void TELESCOPE::SetType (const tTelescopeConfig &config)
{
	if (mainTextureID)
		renderer->TextureRelease(mainTextureID);

	zoomK = config.zoom;
	changeStatusTime = config.activateTime;
	updateTime = config.updateTiming;
	visualizationInfo.shownItems = config.nShownItems;

	mainTextureID = renderer->TextureCreate(config.texture);
	for (int i = 0; i<visualizationInfo.shownItems; i++)
		visualizationInfo.strings[i][0] = 0;
}

//--------------------------------------------------------------------
void TELESCOPE::VisualizeInfo()
{
	/*
	for (int i = 0; i<visualizationInfo.shownItems; i++)
		renderer->Print(0, i*30, visualizationInfo.strings[i]);
	*/
}

//--------------------------------------------------------------------

void TELESCOPE::HandleAutoAim()
{
	float maxD   = -1e5;
	int   foundI = -1;
	int   oldFoundI = -1;

	//look for nearest to crosshair ship
	for (int i=0; i<shipsCount; i++)
	{
		if (!shipInfo[i].enabled || shipInfo[i].myShip)
			continue;

		static const CVECTOR origin (0.f, 0.f, 1.f);
		float d =  origin | !(worldM * shipInfo[i].ship->GetPos());
		if (d <= 0.999f)
			continue;
		if (d > maxD)
		{
			oldFoundI = foundI;
			foundI = i;
			maxD = d;
		}
	}

	//if nearest ship found, adjust camera
	if (foundI != -1)
	{
		static int oldFoundI = -1;
		CVECTOR foundPos = shipInfo[foundI].ship->GetPos();
		foundPos.y -= 1.f;
		api->Send_Message(cameraID, "lfff", MSG_DECK_CAMERA_SET_VIEWPOINT, foundPos.x, foundPos.y, foundPos.z);
		if (foundI != oldFoundI)
			testTime = updateTime;
		entityFound = true;
		oldFoundI = foundI;
	}
	/*
	if (!entityFound)
	{
		if (foundI != -1)
		{
			CVECTOR foundPos = shipInfo[foundI].ship->GetPos();
			api->Send_Message(cameraID, "lfff", MSG_DECK_CAMERA_SET_VIEWPOINT, foundPos.x, foundPos.y, foundPos.z);
		}
	}
	else
	{
		if (oldFoundI != -1)
		{
			CVECTOR foundPos = shipInfo[oldFoundI].ship->GetPos();
			api->Send_Message(cameraID, "lfff", MSG_DECK_CAMERA_SET_VIEWPOINT, foundPos.x, foundPos.y, foundPos.z);
		}
		else
			if (foundI != -1)
			{
				CVECTOR foundPos = shipInfo[foundI].ship->GetPos();
				api->Send_Message(cameraID, "lfff", MSG_DECK_CAMERA_SET_VIEWPOINT, foundPos.x, foundPos.y, foundPos.z);
			}
	}
	*/
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
