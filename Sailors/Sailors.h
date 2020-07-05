
#ifndef _Sailors_h_
#define _Sailors_h_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\collide.h"
#include "..\common_h\model.h"
#include "..\common_h\geos.h"
#include "..\common_h\messages.h"
#include "..\common_h\exs.h"
#include "..\common_h\rands.h"
#include "..\common_h\ship_base.h"
#include "..\common_h\templates\array.h"
#include "..\common_h\templates\string.h"

#include "..\common_h\sea_base.h"


const float RUN_SPEED=  0.30f;
const float MOVE_SPEED= 0.15f;
const float CLIMB_SPEED= 0.065f;

const float JUMP_SPEED_X_MASTFALL= 3.0f;
const float JUMP_SPEED_Y_EXPLOSION= 30.0f;

#include "SailorsWayPoints.h"
#include "..\ENGINE\PROGRAM\SEA_AI\seaPeople.h"

#define CREW2VISIBLE(n)			5.0f*logf(n) //???
#define SHIP_BLACK_PEARL		164


enum ManMode{MAN_JUMP, MAN_SWIM, MAN_OFF ,MAN_WALK, MAN_RUN, MAN_STAY, MAN_CANNONRELOAD, MAN_TURNLEFT, MAN_TURNRIGHT, MAN_CLIMB_UP,  MAN_CLIMB_DOWN};
enum ManMoveTo{ MOVE_TO_POINT, MOVE_TO_CANNON, MOVE_TO_TOP};



struct ShipState{

	DWORD mode;
	bool dead;
	SEA_BASE * sea;

	ShipState(){
		mode= SHIP_SAIL;	
		dead= false;
	};
};


class ShipMan
{
public:
	ENTITY_ID modelID;
	MODEL * model;

	CVECTOR pos, ang; //Текущее положение
	CVECTOR ptTo, angTo, dir; 

	CVECTOR spos; //Обход друг друга
//	float sang;   //Обход друг друга

	float dieTime;
	bool  inWater;
	float jumpSpeedX, jumpSpeedY;

	Path path; //Текущий путь

	ManMode mode, lastMode; //Режим
	int newWayPoint,lastWayPoint,targetWayPoint,lastTargetPoint;

	ManMoveTo moveTo; //Тип текущей точки

	float manSpeed;
	float rotSpeed;

	ShipMan();

	void SetPos(MODEL *ship, SHIP_BASE *ship_base, dword &dltTime, ShipState &shipState);
	void FindNextPoint(SailorsPoints &sailorsPoints, ShipState &shipState);
	int  FindRandomPoint(SailorsPoints &sailorsPoints, ShipState &shipState);
	int  FindRandomPointWithoutType(SailorsPoints &sailorsPoints);
	void ApplyTargetPoint(CVECTOR pt, bool randomWalk);

	void UpdatePos(dword &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState);

	void SetAnimation(dword dltTime, ShipState &shipState);


	bool MoveToPosition(dword &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState);
	bool RotateToAngle(dword &dltTime, SailorsPoints &sailorsPoints);
	bool Stay(dword &dltTime, SailorsPoints &sailorsPoints);
	bool Turn(dword &dltTime, SailorsPoints &sailorsPoints);
	bool Swim(dword &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState);
	bool Jump(dword &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState);

	void NewAction(SailorsPoints &sailorsPoints, ShipState &shipState, dword &dltTime);
	void Free();
	int  GetNearestEmptyCannon(SailorsPoints &sailorsPoints);
};

class ShipWalk
{
public:
	ShipWalk() : shipMan(_FL_) {}

	SHIP_BASE *ship;
	MODEL *shipModel;
	int crewCount; //Количество человек

	bool bHide;

	ENTITY_ID shipID;

	SailorsPoints sailorsPoints; //Точки

	void ReloadCannons(int bort);
	ShipState shipState; //Состояние корабля

	array <ShipMan> shipMan;
	void CreateNewMan(SailorsPoints &sailorsPoints);
	void DeleteMan(int Index);

	void Init(ENTITY_ID &_shipID, int editorMode, char *shipType);
	void CheckPosition(dword &dltTime);
	void SetMastBroken(int iMastIndex);
	void OnHullHit(CVECTOR &v);
	void Reset();

	void Free();
};


class Sailors : public ENTITY
{
public:

	VDX8RENDER *rs;

	array <ShipWalk> shipWalk;
	bool editorMode;
	int shipsCount;
	bool disabled;

	Sailors();
	virtual ~Sailors();

	virtual bool Init();
	virtual void Realize(dword dltTime);

	virtual dword _cdecl ProcessMessage(MESSAGE &message);
	virtual dword AttributeChanged(ATTRIBUTES *_newAttr);
	int IsOnDeck;

	void DeleteShip(int i);
};

#endif