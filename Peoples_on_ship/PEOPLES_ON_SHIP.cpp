#include <stdio.h>
#include "..\common_h\messages.h"
#include "..\common_h\exs.h"
#include "..\common_h\rands.h"
#include "..\common_h\types3d.h"
#include "..\ENGINE\PROGRAM\SEA_AI\script_defines.h"
#include "PEOPLES_ON_SHIP.h"
#include "..\engine\program\sea_ai\seaPeople.h"

#define SHIP_BLACK_PEARL		164

INTERFACE_FUNCTION
CREATE_CLASS(PEOPLE_ON_SHIP)

//--------------------------------------------------------------------
PEOPLE_ON_SHIP::PEOPLE_ON_SHIP()
	:shipsCount(0)
	,manSpeed(0.0f)
	,cameraOnDeck(false)
	,hidePeopleOnDeck(true)
{
	GUARD(PEOPLE_ON_SHIP::PEOPLE_ON_SHIP)
	UNGUARD
}

//--------------------------------------------------------------------
PEOPLE_ON_SHIP::~PEOPLE_ON_SHIP()
{
	GUARD(PEOPLE_ON_SHIP::~PEOPLE_ON_SHIP)

	for (int shipN=0; shipN < shipsCount; shipN++)
	{
		for (int manN=0; manN < shipWalk[shipN].crewCount; manN++)
			_CORE_API->DeleteEntity(shipWalk[shipN].crew[manN].modelID);
	}

	UNGUARD
}

//--------------------------------------------------------------------
bool PEOPLE_ON_SHIP::Init()
{
	GUARD(PEOPLE_ON_SHIP::Init)

	renderer = (VDX8RENDER *) _CORE_API->CreateService("dx8render");

	return true;
	UNGUARD
} 


//--------------------------------------------------------------------
dword PEOPLE_ON_SHIP::AttributeChanged(ATTRIBUTES *_newAttr)
{
	if (*_newAttr == "IsOnDeck")
	{
		cameraOnDeck = this->AttributesPointer->GetAttributeAsDword("IsOnDeck") != 0;
		for (int shipN=0; shipN < shipsCount; shipN++)
		{
			if (shipWalk[shipN].ship->GetACharacter()->GetAttribute("MainCharacter"))
				shipWalk[shipN].myShip = true;
			else
				shipWalk[shipN].myShip = false;
		}
	}
	else
	if (*_newAttr == "HidePeopleOnDeck")
	{
		hidePeopleOnDeck = this->AttributesPointer->GetAttributeAsDword("HidePeopleOnDeck") != 0;
	}
	else
	if (*_newAttr == "isNight")
	{
		isNight = this->AttributesPointer->GetAttributeAsDword("isNight") != 0;
	}

	return 0;
}

//--------------------------------------------------------------------
bool SphereVisible (const PLANE &_plane, const CVECTOR &_center, float _r)
{
	CVECTOR planeV = CVECTOR(_plane.Nx, _plane.Ny, _plane.Nz);
	return ((planeV | _center) - _plane.D) > -_r;
}

//--------------------------------------------------------------------
void PEOPLE_ON_SHIP::Execute(DWORD _dTime)
{
	GUARD(PEOPLE_ON_SHIP::Execute)

	// <<debug>>
	/*
	if (GetAsyncKeyState('Z'))
		SetShipState(&shipWalk[0], SHIP_WAR);
	if (GetAsyncKeyState('X'))
		SetShipState(&shipWalk[0], SHIP_SAIL);
	*/

	// crew movement & actions
	for (int shipN=0; shipN < shipsCount; shipN++)
	{
		if (!shipWalk[shipN].enabled)
			continue;

		// ship state
		ATTRIBUTES *shipAttr = shipWalk[shipN].ship->GetACharacter();
		ATTRIBUTES *shipModeAttr = shipAttr->FindAClass(shipAttr, "ship.POS.mode");
		if (shipModeAttr)
		{
			int shipMode = shipModeAttr->GetAttributeAsDword();
			SetShipState(&shipWalk[shipN], shipMode);
		}
		else
			SetShipState(&shipWalk[shipN], SHIP_WAR);
		// crew
		for (int manN=0; manN < shipWalk[shipN].showCount; manN++)
		{
			switch (shipWalk[shipN].crew[manN].state)
			{
			case MAN_WALK:
				if (!ProcessManWalk(&shipWalk[shipN], &shipWalk[shipN].crew[manN], _dTime))
					ChooseNewAction(&shipWalk[shipN], &shipWalk[shipN].crew[manN]);
				break;
			case MAN_CRAWL_WALK_VANT:
			case MAN_CRAWL_VANT_TOP:
			case MAN_CRAWL_TOP_MARS:
			case MAN_CRAWL_MARS:	
			case MAN_CRAWL_MARS_TOP:
			case MAN_CRAWL_TOP_VANT:
			case MAN_CRAWL_VANT_WALK:
				if (!ProcessManCrawl(&shipWalk[shipN], &shipWalk[shipN].crew[manN], _dTime))
					ChooseNewAction(&shipWalk[shipN], &shipWalk[shipN].crew[manN]);
				break;
			case MAN_TURN_LEFT:
			case MAN_TURN_RIGHT:
				if (!ProcessManTurn(&shipWalk[shipN], &shipWalk[shipN].crew[manN], _dTime))
					ChooseNewAction(&shipWalk[shipN], &shipWalk[shipN].crew[manN]);
				break;
			case MAN_RUN:
				if (!ProcessManWalk(&shipWalk[shipN], &shipWalk[shipN].crew[manN], _dTime))
					ChooseNewAction(&shipWalk[shipN], &shipWalk[shipN].crew[manN]);
				break;
			case MAN_STAND1:
			case MAN_STAND2:
			case MAN_STAND3:
			case MAN_STAND4:
			case MAN_RELOAD:
				if (!ProcessManStand(&shipWalk[shipN], &shipWalk[shipN].crew[manN], _dTime))
					ChooseNewAction(&shipWalk[shipN], &shipWalk[shipN].crew[manN]);
				break;
			}
		}
	}

	UNGUARD
}

//--------------------------------------------------------------------
void PEOPLE_ON_SHIP::Realize(DWORD _dTime)
{
	GUARD(PEOPLE_ON_SHIP::Realize)

	CVECTOR shipPos, shipAng;
	CMatrix m,m2;
	CVECTOR manCenter(0.0f, 1.0f, 0.0f), manPos;

	int peopleCount = 0;

	CVECTOR ang;
	float persp;
	renderer->GetCamera(cameraPosition, ang, persp);

	// update frustrum
	PLANE *p;
	p = renderer->GetPlanes();
	clipPlanes[0] = p[0];
	clipPlanes[1] = p[1];
	clipPlanes[2] = p[2];
	clipPlanes[3] = p[3];

	for (int shipN=0; shipN < shipsCount; shipN++)
	{
		if (!shipWalk[shipN].enabled)
			continue;

		if (hidePeopleOnDeck)
		{
			if ((cameraOnDeck) && (shipWalk[shipN].myShip))
				continue;
		}

		shipWalk[shipN].ship->SetLights();

		for (int manN=0; manN < shipWalk[shipN].showCount; manN++)
		{
			if (!shipWalk[shipN].crew[manN].model)
				continue;

			// clip by planes
			m.BuildPosition(shipWalk[shipN].crew[manN].pos.x,shipWalk[shipN].crew[manN].pos.y, shipWalk[shipN].crew[manN].pos.z);
			m2.BuildRotateY(shipWalk[shipN].crew[manN].ang);
			shipWalk[shipN].crew[manN].model->mtx = m2 * m * shipWalk[shipN].shipModel->mtx;
			manPos = shipWalk[shipN].crew[manN].model->mtx * manCenter;

			if (tanf(persp * 0.5f) * ~(manPos - cameraPosition) > (MAX_VIEW_DISTANCE*MAX_VIEW_DISTANCE))
			{
				SetManVisible(&shipWalk[shipN].crew[manN], false);
				continue;
			}
			if (!SphereVisible(clipPlanes[0], manPos, 1.0f))
			{
				SetManVisible(&shipWalk[shipN].crew[manN], false);
				continue;
			}
			if (!SphereVisible(clipPlanes[1], manPos, 1.0f))
			{
				SetManVisible(&shipWalk[shipN].crew[manN], false);
				continue;
			}
			if (!SphereVisible(clipPlanes[2], manPos, 1.0f))
			{
				SetManVisible(&shipWalk[shipN].crew[manN], false);
				continue;
			}
			if (!SphereVisible(clipPlanes[3], manPos, 1.0f))
			{
				SetManVisible(&shipWalk[shipN].crew[manN], false);
				continue;
			}

			SetManVisible(&shipWalk[shipN].crew[manN], true);

			shipWalk[shipN].crew[manN].model->Realize(_dTime);
			++peopleCount;
		}

		shipWalk[shipN].ship->UnSetLights();
	}

	char counterName[16];
	sprintf(counterName, "people: %d", peopleCount);

	//long lw = renderer->StringWidth(counterName)/2;
	//renderer->Print(long(0), long(0), counterName);

	//renderer->SetTransform(D3DTS_WORLD, CMatrix());
	UNGUARD
}

//--------------------------------------------------------------------
dword PEOPLE_ON_SHIP::ProcessMessage(MESSAGE &message)
{
	GUARD(PEOPLE_ON_SHIP::ProcessMessage)

	long code = message.Long();
	dword outValue = 0;
	int vCount;
	ENTITY_ID shipID;
	VDATA *vArray, *gArray, *tArray;

	switch (code)
	{
	case AI_MESSAGE_ADD_SHIP:
		shipID = message.EntityID();
		vCount = message.Long();
		vArray = message.ScriptVariablePointer();
		gArray = message.ScriptVariablePointer();
		tArray = message.ScriptVariablePointer();

		AddShipWalk(shipID, vCount, vArray, gArray, tArray);
		break;
	case MSG_SHIP_DELETE:
		{
			ATTRIBUTES *attrs = message.AttributePointer();
			if (attrs)
				for (int ship = 0; ship < shipsCount; ship++)
				{
					if (attrs == shipWalk[ship].ship->GetACharacter())
					{
						shipWalk[ship].enabled = false;
						return outValue;
					}
				}
		}
		break;
	}

	return outValue;   
	UNGUARD
}

//--------------------------------------------------------------------
void PEOPLE_ON_SHIP::AddShipWalk(ENTITY_ID &_shipID, int vCount, VDATA *vArray, VDATA *gArray, VDATA *tArray)
{
	GUARD(PEOPLE_ON_SHIP::AddShipWalk)

	// attaching to ship entity & model
	shipWalk[shipsCount].ship = (SHIP_BASE *) _CORE_API->GetEntityPointer(&_shipID);
	shipWalk[shipsCount].shipModel = shipWalk[shipsCount].ship->GetModel();
	shipWalk[shipsCount].vertsCount = vCount;

	// getting graph vertices
	float x,y,z;
	long   vType;
	for (int i = 0; i<vCount; i++)
	{
		vArray->Get(x, i*3);
		vArray->Get(y, i*3+1);
		vArray->Get(z, i*3+2);
		tArray->Get(vType, i);
		shipWalk[shipsCount].verts[i].x = x;
		shipWalk[shipsCount].verts[i].y = y;
		shipWalk[shipsCount].verts[i].z = z;
		shipWalk[shipsCount].vertTypes[i] = (byte) vType;
		shipWalk[shipsCount].vertBusy[i] = 0;
	}

	// getting links between graph vertices
	int nElements = gArray->GetElementsNum();
	long gCouple = 0;

	for (i=0; i<nElements; i++)
	{
		gArray->Get(gCouple, i);
		long v1, v2;
		v1 = (gCouple >> 8) & 0xFF;
		v2 = gCouple & 0xFF;
		if ((gCouple > 0) && (v1 < vCount) && (v2 < vCount))
			shipWalk[shipsCount].graph.AddPair(v1, 
											   v2,
											   shipWalk[shipsCount].vertTypes[gCouple >> 8],
											   shipWalk[shipsCount].vertTypes[gCouple & 0xFF]);
	}

	// visible crew count
	ATTRIBUTES *attr = shipWalk[shipsCount].ship->GetACharacter();
	ATTRIBUTES *crewAttr = attr->FindAClass(attr, "ship.crew");
	shipWalk[shipsCount].crewCount = (int) ceilf(CREW2VISIBLE((float) crewAttr->GetAttributeAsDword("quantity", 1)));
	if (shipWalk[shipsCount].crewCount > MAX_PEOPLE)
		shipWalk[shipsCount].crewCount = MAX_PEOPLE;

	// initialize crew
	for (i=0; i<shipWalk[shipsCount].crewCount; i++)
		InitShipMan(shipsCount, i);

	// initialize ship states
	shipWalk[shipsCount].state = -1;
	shipWalk[shipsCount].showCount = 0;
	shipWalk[shipsCount].enabled = true;
	SetShipState(&shipWalk[shipsCount], SHIP_SAIL);


	++shipsCount;

	UNGUARD
}

//--------------------------------------------------------------------
float PEOPLE_ON_SHIP::Vector2Angle(const CVECTOR &_v)
{
	return (float) atan2(_v.x, _v.z);
} 

//--------------------------------------------------------------------
void PEOPLE_ON_SHIP::InitShipMan(int _shipN, int _manN)
{
	ENTITY_ID manID;
	_CORE_API->CreateEntity(&shipWalk[_shipN].crew[_manN].modelID,"MODELR");

	// load appropriate model
	switch (rand() % 6) // 6 different character types
	{
	case 0:
		_CORE_API->Send_Message(shipWalk[_shipN].crew[_manN].modelID,"ls",MSG_MODEL_LOAD_GEO, "Lowcharacters\\Lo_Man_1");
		break;
	case 1:
		_CORE_API->Send_Message(shipWalk[_shipN].crew[_manN].modelID,"ls",MSG_MODEL_LOAD_GEO, "Lowcharacters\\Lo_Man_2");
		break;
	case 2:
		_CORE_API->Send_Message(shipWalk[_shipN].crew[_manN].modelID,"ls",MSG_MODEL_LOAD_GEO, "Lowcharacters\\Lo_Man_3");
		break;
	case 3:
		_CORE_API->Send_Message(shipWalk[_shipN].crew[_manN].modelID,"ls",MSG_MODEL_LOAD_GEO, "Lowcharacters\\Lo_Man_Kamzol_1");
		break;
	case 4:
		_CORE_API->Send_Message(shipWalk[_shipN].crew[_manN].modelID,"ls",MSG_MODEL_LOAD_GEO, "Lowcharacters\\Lo_Man_Kamzol_2");
		break;
	case 5:
		_CORE_API->Send_Message(shipWalk[_shipN].crew[_manN].modelID,"ls",MSG_MODEL_LOAD_GEO, "Lowcharacters\\Lo_Man_Kamzol_3");
		break;
	}

	_CORE_API->Send_Message(shipWalk[_shipN].crew[_manN].modelID,"ls",MSG_MODEL_LOAD_ANI, "Lo_Man");
	MODEL *manModel = (MODEL*)_CORE_API->GetEntityPointer(&shipWalk[_shipN].crew[_manN].modelID);

	// start animation
	manModel->GetAnimation()->Player(0).SetAction("walk");
	//manModel->GetAnimation()->Player(0).Play();
	manModel->GetAnimation()->Player(0).SetPosition(rand(1.0f));

	// detect man speed
	if (manSpeed == 0.0f)
	{
		const char *inData;
		inData = manModel->GetAnimation()->Player(0).GetData("Walk speed");
		if (inData)
			sscanf(inData, "%f", &manSpeed);
		inData = manModel->GetAnimation()->Player(0).GetData("Turn speed");
		if (inData)
			sscanf(inData, "%f", &manTurnSpeed);

		manSpeed /= 1e3f; // m/s -> m/ms
		manTurnSpeed /= 1e3f;
		approachDist = 1e4f*manSpeed;
	}

	//set unique properties
	shipWalk[_shipN].crew[_manN].model = manModel;
	shipWalk[_shipN].crew[_manN].visible = false;
	shipWalk[_shipN].crew[_manN].sourceI = -1; 
}

//--------------------------------------------------------------------
bool PEOPLE_ON_SHIP::ProcessManWalk(tShipWalk *_shipWalk, tShipMan *_man, dword _dTime)
{
	float dNow = (float) 
				 SQR(_man->pos.x - _shipWalk->verts[_man->destI].x)
				+SQR(_man->pos.y - _shipWalk->verts[_man->destI].y)
				+SQR(_man->pos.z - _shipWalk->verts[_man->destI].z);
	float dirVect = _man->speed*_dTime;
	float dFuture = (float)
					SQR(_man->pos.x+dirVect*_man->dir.x - _shipWalk->verts[_man->destI].x)
				   +SQR(_man->pos.y+dirVect*_man->dir.y - _shipWalk->verts[_man->destI].y)
				   +SQR(_man->pos.z+dirVect*_man->dir.z - _shipWalk->verts[_man->destI].z);
	if (dFuture < dNow)
	{
		_man->pos += dirVect*_man->dir;
		return true;
	}
	else
	{
		//_man->pos = _shipWalk->verts[_man->destI];
		_man->sourceI = _man->destI;
		return false;
	}
}

//--------------------------------------------------------------------
bool PEOPLE_ON_SHIP::ProcessManCrawl(tShipWalk *_shipWalk, tShipMan *_man, dword _dTime)
{
	switch (_man->state)
	{
	case MAN_CRAWL_WALK_VANT:
	case MAN_CRAWL_VANT_TOP:
	case MAN_CRAWL_TOP_MARS:
	case MAN_CRAWL_MARS_TOP:
	case MAN_CRAWL_TOP_VANT:
	case MAN_CRAWL_VANT_WALK:
		return ProcessManWalk(_shipWalk, _man, _dTime);
		break;
	case MAN_CRAWL_MARS:
		return ProcessManStand(_shipWalk, _man, _dTime);
		break;
	}

	return false;
}

//--------------------------------------------------------------------
bool PEOPLE_ON_SHIP::ProcessManTurn(tShipWalk *_shipWalk, tShipMan *_man, dword _dTime)
{
	if (fabs(_man->ang - _man->newAngle) >= MAN_MIN_TURN)
	{
		_man->ang += _man->speed*_dTime;
		if (_man->ang > PI)
			_man->ang = -(PIm2 - _man->ang);
		if (_man->ang < -PI)
			_man->ang = PIm2 + _man->ang;
		return true;
	}
	else
		return false;
}

//--------------------------------------------------------------------
bool PEOPLE_ON_SHIP::ProcessManStand(tShipWalk *_shipWalk, tShipMan *_man, dword _dTime)
{
	bool b = _man->model->GetAnimation()->Player(0).IsPlaying();
	if (!b && (_man->state == MAN_RELOAD))
	{
		_shipWalk->vertBusy[_man->sourceI] = 0;
	}
	return b;
}

//--------------------------------------------------------------------
int getRandomByProbs(int _state, bool _allowCrawl)
{
	if (!_allowCrawl)
	{
		int rN = rand() % 100;
		int n = 0;
		for (int i=0; i<MAN_STATE_COUNT; i++) 
		{
			n += (int) (100.0f * probs[_state][i]);
			if (rN < n)
				return i;
		}
		return i;
	}
	else
	{
		int rN = rand() % 100;
		if (rN < ((int) (100.0f * probs[_state][(int) MAN_CRAWL_WALK_VANT])))
			return MAN_CRAWL_WALK_VANT;
		else
			return MAN_RUN;

	}
}

//--------------------------------------------------------------------
void PEOPLE_ON_SHIP::ChooseNewAction(tShipWalk *_shipWalk, tShipMan *_man)
{
	// choose new action
	if ((_man->state == MAN_WALK) || (_man->state == MAN_RUN))
	{
		_man->state = MAN_TURN_LEFT;
	}
	else if (_man->state == MAN_CRAWL_WALK_VANT)
		_man->state = MAN_CRAWL_VANT_TOP;
	else if (_man->state == MAN_CRAWL_VANT_TOP)
		_man->state = MAN_CRAWL_TOP_MARS;
	else if (_man->state == MAN_CRAWL_TOP_MARS)
		_man->state = MAN_CRAWL_MARS;
	else if (_man->state == MAN_CRAWL_MARS)
		_man->state = MAN_CRAWL_MARS_TOP;
	else if (_man->state == MAN_CRAWL_MARS_TOP)
		_man->state = MAN_CRAWL_TOP_VANT;
	else if (_man->state == MAN_CRAWL_TOP_VANT)
		_man->state = MAN_CRAWL_VANT_WALK;
	else
	{
		tManState oldState = _man->state;
		int dstI = _shipWalk->graph.FindRandomLinkedAnyType(_man->sourceI);
		if (_man->state != MAN_RELOAD) 
			_man->state = (tManState) getRandomByProbs(_shipWalk->state,
													   _shipWalk->vertTypes[dstI] == LOCATOR_VANT);

		//handle cannon reload
		if (   (_man->state == MAN_RELOAD) 
			 &&(_shipWalk->vertTypes[_man->sourceI] == LOCATOR_CANNON)
			 &&(!_shipWalk->vertBusy[_man->sourceI]))
		{
			_shipWalk->vertBusy[_man->sourceI] = (dword) _man;
			//_man->state = oldState;
			if (_man->pos.x < 0.0f) //left cannons row
			{
				_man->ang = -PId2;
				_man->dir = CVECTOR(-1.0f, 0.0f, 0.0f);
			}
			else //right row
			{
 				_man->ang = PId2;
				_man->dir = CVECTOR(1.0f, 0.0f, 0.0f);
			}
		}
		else
		if (_man->state == MAN_CRAWL_WALK_VANT)
		{
			//_man->state = MAN_CRAWL_TOP;
			//_man->destI = dstI;
		}
		else
		{
			while ((int) _man->state > (int) MAN_STAND4)
				_man->state = (tManState) getRandomByProbs(_shipWalk->state, false);
		}
	}

	// handle new-chosen action start
	switch (_man->state)
	{
	case MAN_TURN_LEFT:
		_man->sourceI = _man->destI;

		_man->destI = _shipWalk->graph.FindRandomLinkedWithoutType(_man->sourceI, LOCATOR_VANT);

		_man->newAngle = Vector2Angle(!(_shipWalk->verts[_man->destI] - _man->pos));
		if (_man->newAngle > PI)
			_man->newAngle = -(PIm2 - _man->newAngle);
		if (_man->newAngle < -PI)
			_man->newAngle = PIm2 + _man->newAngle;
		if ((_man->newAngle - _man->ang) < 0.0f)
		{
			_man->speed = -manTurnSpeed;
			_man->state = MAN_TURN_RIGHT;
			_man->model->GetAnimation()->Player(0).SetAction("turn_left");
			//_man->model->GetAnimation()->Player(0).Play();
		}
		else
		{
			_man->speed = manTurnSpeed;
			_man->state = MAN_TURN_LEFT;
			_man->model->GetAnimation()->Player(0).SetAction("turn_right");
			//_man->model->GetAnimation()->Player(0).Play();
		}
		break;
	case MAN_WALK:
		if (_man->destI == -1)
			_man->visible = false;
		_man->dir = !(_shipWalk->verts[_man->destI] - _man->pos);
		_man->ang = Vector2Angle(_man->dir);
		_man->speed = manSpeed + randCentered(manSpeed * MAN_SPEED_DEVIATION_K);
		_man->model->GetAnimation()->Player(0).SetAction("walk");
		//_man->model->GetAnimation()->Player(0).Play();
		break;
	case MAN_RUN:
		if (_man->destI == -1)
			_man->visible = false;
		_man->dir = !(_shipWalk->verts[_man->destI] - _man->pos);
		_man->ang = Vector2Angle(_man->dir);
		_man->speed = 2.0f*(manSpeed + randCentered(manSpeed * MAN_SPEED_DEVIATION_K));
		_man->model->GetAnimation()->Player(0).SetAction("run");
		//_man->model->GetAnimation()->Player(0).Play();
		break;
	case MAN_STAND1:
		_man->model->GetAnimation()->Player(0).SetAction("action1");
		//_man->model->GetAnimation()->Player(0).Play();
		break;
	case MAN_STAND2:
		_man->model->GetAnimation()->Player(0).SetAction("action2");
		//_man->model->GetAnimation()->Player(0).Play();
		break;
	case MAN_STAND3:
		_man->model->GetAnimation()->Player(0).SetAction("action3");
		//_man->model->GetAnimation()->Player(0).Play();
		break;
	case MAN_STAND4:
		_man->model->GetAnimation()->Player(0).SetAction("action4");
		//_man->model->GetAnimation()->Player(0).Play();
		break;
	case MAN_RELOAD:
		_man->model->GetAnimation()->Player(0).SetAction("reload");
		//_man->model->GetAnimation()->Player(0).Play();
		break;
	case MAN_CRAWL_WALK_VANT:
		_man->destI = _shipWalk->graph.FindRandomLinkedWithType(_man->sourceI,LOCATOR_VANT);
		if (_man->destI == -1)
			_man->visible = false;
		_man->dir = !(_shipWalk->verts[_man->destI] - _man->pos);
		_man->ang = Vector2Angle(_man->dir);
		_man->speed = 2.0f*(manSpeed + randCentered(manSpeed * MAN_SPEED_DEVIATION_K));
		_man->model->GetAnimation()->Player(0).SetAction("run");
		//_man->model->GetAnimation()->Player(0).Play();
		break;
	case MAN_CRAWL_VANT_TOP:
		_man->destI = _shipWalk->graph.FindRandomLinkedWithType(_man->sourceI,LOCATOR_TOP);
		if (_man->destI == -1)
			_man->visible = false;
		_man->dir = !(_shipWalk->verts[_man->destI] - _man->pos);
		if (_man->pos.x < 0.0f)
			_man->ang = PId2;
		else
			_man->ang = PI + PId2;
		_man->speed = 0.5f*(manSpeed + randCentered(manSpeed * MAN_SPEED_DEVIATION_K));
		_man->model->GetAnimation()->Player(0).SetAction("crawl");
		//_man->model->GetAnimation()->Player(0).Play();
		break;
	case MAN_CRAWL_TOP_MARS:
		_man->destI = _shipWalk->graph.FindRandomLinkedWithType(_man->sourceI,LOCATOR_MARS);
		if (_man->destI == -1)
			_man->visible = false;
		_man->dir = !(_shipWalk->verts[_man->destI] - _man->pos);
		_man->ang = Vector2Angle(_man->dir);
		_man->model->GetAnimation()->Player(0).SetAction("crawl");
		//_man->model->GetAnimation()->Player(0).Play();
		break;
	case MAN_CRAWL_MARS:
		_man->model->GetAnimation()->Player(0).SetAction("action1");
		//_man->model->GetAnimation()->Player(0).Play();
		break;
	case MAN_CRAWL_MARS_TOP:
		_man->destI = _shipWalk->graph.FindRandomLinkedWithType(_man->sourceI,LOCATOR_TOP);
		if (_man->destI == -1)
			_man->visible = false;
		_man->dir = !(_shipWalk->verts[_man->destI] - _man->pos);
		_man->ang = PI + PId2 * ((int) (Vector2Angle(_man->dir) / PId2));
		_man->model->GetAnimation()->Player(0).SetAction("crawl_down");
		//_man->model->GetAnimation()->Player(0).Play();
		break;
	case MAN_CRAWL_TOP_VANT:
		_man->destI = _shipWalk->graph.FindRandomLinkedWithType(_man->sourceI,LOCATOR_VANT);
		if (_man->destI == -1)
			_man->visible = false;
		_man->dir = !(_shipWalk->verts[_man->destI] - _man->pos);
		_man->ang = PI + Vector2Angle(_man->dir);
		_man->model->GetAnimation()->Player(0).SetAction("crawl_down");
		//_man->model->GetAnimation()->Player(0).Play();
		break;
	case MAN_CRAWL_VANT_WALK:
		_man->destI = _shipWalk->graph.FindRandomLinkedWithType(_man->sourceI,LOCATOR_WALK);
		if (_man->destI == -1)
			_man->visible = false;
		if (_man->destI == _man->sourceI)
			_man->destI = _shipWalk->graph.FindRandomLinkedWithType(_man->sourceI,LOCATOR_CANNON);
		_man->dir = !(_shipWalk->verts[_man->destI] - _man->pos);
		_man->ang = Vector2Angle(_man->dir);
		_man->speed = 2.0f*(manSpeed + randCentered(manSpeed * MAN_SPEED_DEVIATION_K));
		_man->model->GetAnimation()->Player(0).SetAction("run");
		//_man->model->GetAnimation()->Player(0).Play();
		break;
	}
	_man->visible = false; //to start new animation playing if visible
}

//--------------------------------------------------------------------
void PEOPLE_ON_SHIP::SetShipState(tShipWalk *_shipWalk, int _newState)
{
	if (_shipWalk->state == _newState)
		return;

	// new show crew count
	_shipWalk->state = _newState;
	int oldShowCount = _shipWalk->showCount;
	int newShowCount = (int) (_shipWalk->crewCount * crewPart[_newState]);

	if (newShowCount > oldShowCount)
	{
		_shipWalk->showCount = newShowCount;

		// search doors
		for (int vertIndex1=0; vertIndex1<_shipWalk->vertsCount; vertIndex1++)
			if (_shipWalk->vertTypes[vertIndex1] == LOCATOR_SPAWN)
				break;
		for (int vertIndex2=_shipWalk->vertsCount; vertIndex2>=0; vertIndex2--)
			if (_shipWalk->vertTypes[vertIndex2] == LOCATOR_SPAWN)
				break;

		int vertIndex;

		for (int i=oldShowCount; i<_shipWalk->showCount; i++)
		{
			if (rand() & 0x1) //choose one of two found doors
				vertIndex = vertIndex1;
			else
				vertIndex = vertIndex2;

			if (_shipWalk->crew[i].sourceI == -1)
				vertIndex = _shipWalk->graph.FindRandomWithType(LOCATOR_WALK);
			_shipWalk->crew[i].sourceI = vertIndex;
			_shipWalk->crew[i].destI = _shipWalk->graph.FindRandomLinkedAnyType(vertIndex);
			_shipWalk->crew[i].pos = _shipWalk->verts[vertIndex];
			_shipWalk->crew[i].dir = !(_shipWalk->verts[_shipWalk->crew[i].destI] - _shipWalk->crew[i].pos);
			_shipWalk->crew[i].ang = Vector2Angle(_shipWalk->crew[i].dir);
			_shipWalk->crew[i].speed = manSpeed + randCentered(manSpeed * MAN_SPEED_DEVIATION_K);
			_shipWalk->crew[i].state = MAN_RUN;
			_shipWalk->crew[i].destI = vertIndex;
			ChooseNewAction(_shipWalk, &_shipWalk->crew[i]);
		}

		//others shall come out from door
		for (i=_shipWalk->showCount; i < MAX_PEOPLE; i++)
			_shipWalk->crew[i].sourceI = vertIndex;
	}
}

//--------------------------------------------------------------------
void PEOPLE_ON_SHIP::SetManVisible (tShipMan *_man, bool _visible)
{
	if (_man->visible == _visible)
		return;

	if (!_visible)
	{
		_man->model->GetAnimation()->Player(0).Stop();
	}
	else
	{
		_man->model->GetAnimation()->Player(0).Play();
	}

	_man->visible = _visible;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
