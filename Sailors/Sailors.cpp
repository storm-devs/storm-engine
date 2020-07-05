 
//------------------------------------------------------------------------------------
//	Sailors, SailorsWayPoints, SailorsEditor, SailorsMenu, ShipWalk.c, SailorsMain
//	BattleInterface.c, ShipDead.c, SeaPeople.h
//------------------------------------------------------------------------------------
// micuss, 2004
//------------------------------------------------------------------------------------
#include <stdio.h>
#include "sailors.h"

#include "..\common_h\messages.h"
#include "..\common_h\exs.h"
#include "..\common_h\rands.h"
#include "..\common_h\types3d.h"
#include "..\ENGINE\PROGRAM\SEA_AI\script_defines.h"
#include "..\engine\program\sea_ai\seaPeople.h"
#include "..\common_h\ship_msg.h"
#include "..\Ship\ship.h"

#define GUARD_SAILORS(block)	{ static const TCHAR block_name[] = TEXT(#block); try {
#define UNGUARD_SAILORS			} catch(...) { api->Trace("ERROR in Sailors.dll : block '%s'", (char*)block_name); throw _EXS(FATAL,"unknown"); }}

//------------------------------------------------------------------------------------
ShipMan::ShipMan()
{
	GUARD_SAILORS(ShipMan::ShipMan())

	pos= CVECTOR(0.0f,11.0f,30.0f);
	ang. x= ang.y = ang.z = 0;
    angTo.x = angTo.y = angTo.z= 0;
	spos.x = spos.y = spos.z= 0;
	manSpeed = 0.1f;
	rotSpeed = 0.5f;
	mode =     MAN_WALK;
	lastMode = MAN_OFF;
	newWayPoint = lastWayPoint = 0;
	dieTime= 0;
	inWater = false;
	jumpSpeedX = jumpSpeedY= 0;

	UNGUARD_SAILORS
};
//------------------------------------------------------------------------------------
void ShipMan::Free()
{ 
	GUARD_SAILORS(ShipMan::Free())

	api->DeleteEntity(this->modelID);

	UNGUARD_SAILORS
};
//-----Построение матрицы с учетом текущего состояния---------------------------------
void ShipMan::SetPos(MODEL *ship, SHIP_BASE *ship_base, dword &dltTime, ShipState &shipState)
{
	GUARD_SAILORS(ShipMan::SetPos())

	if(ship_base && (shipState.dead || jumpSpeedY))
	{
		//Если попал в воду - отвязать от корабля и плыть
		if(!inWater && model->mtx.Pos().y< shipState.sea->WaveXZ(model->mtx.Pos().x, model->mtx.Pos().z)- 1.4f) 
		{
			inWater= true;

			model->GetAnimation()->Player(0).SetAction("crawl");
			model->GetAnimation()->Player(0).SetPosition( rand()/float(RAND_MAX) );
			model->GetAnimation()->Player(0).SetSpeed( model->GetAnimation()->Player(0).GetSpeed() / 2 );
			model->GetAnimation()->Player(0).Play();
		
			pos = model->mtx.Pos();

			ang.y+= ship_base->GetAng().y;
			ang.x = ang.z = 0;
			angTo.y = float( rand() ) * PI * 2.0f / float(RAND_MAX);

			rotSpeed = MOVE_SPEED;
			mode = MAN_SWIM;
		}
	}

	if (!inWater)
	{	
		CMatrix mPos, mRot, m1,m2;
		
		mPos.BuildMatrix(CVECTOR(ang.x,ang.y, ang.z), pos + spos/*CVECTOR(pos.x+ spos.x, pos.y+ spos.y, pos.z+ spos.z)*/);

		if (ship_base)
		{
			model->mtx = mPos * (*ship_base->GetMatrix()); 
		} else 
		{
			model->mtx = mRot*mPos;		
		}
		
	} else {

		CMatrix mpos, mrot;
		mrot.BuildMatrix(this->ang);
		mpos.BuildPosition(this->pos.x, this->pos.y, this->pos.z);
		model->mtx = mrot * mpos;
	};

	UNGUARD_SAILORS
};
//------------------------------------------------------------------------------------
bool ShipMan::RotateToAngle(dword &dltTime, SailorsPoints &sailorsPoints)
{
	GUARD_SAILORS(ShipMan::RotateToAngle())

	if (abs(angTo.y- ang.y))
	{ 
		if (abs(ang.y- angTo.y)< PI) 
		{
			if (angTo.y> ang.y) 
				ang.y+= rotSpeed*dltTime/100.0f; else
				ang.y-= rotSpeed*dltTime/100.0f;

		} else 
		{
			if (angTo.y> ang.y) 
				ang.y-= rotSpeed*dltTime/100.0f; else		
				ang.y+= rotSpeed*dltTime/100.0f;
		}

		if (ang.y>= PI*2) ang.y-= PI*2;
		if (ang.y<  0)    ang.y+= PI*2;

		if (abs(float(angTo.y- ang.y))< rotSpeed*dltTime/100.0f) ang = angTo;

		return false;
	};

	UNGUARD_SAILORS
	return true;
};
//-----Найти новую точку-цель---------------------------------------------------------
int  ShipMan::FindRandomPoint(SailorsPoints &sailorsPoints, ShipState &shipState)
{
	GUARD_SAILORS(ShipMan::FindRandomPoint())

	int ran;
	//Если боевой режим или перезарядка то ищем свободные пушки
	for (int m= 0; m< sailorsPoints.points.count; m++)
		for (int i= 0; i< sailorsPoints.points.count; i++){

		ran= int(rand()*(sailorsPoints.points.count - 1)/float(RAND_MAX));

		if (sailorsPoints.points.point[ran].IsCannon())

			if (ran!= targetWayPoint && !sailorsPoints.points.point[ran].buisy &&
				(!sailorsPoints.points.point[ran].cannonReloaded || shipState.mode== SHIP_WAR)){
				
  				sailorsPoints.points.point[ran].buisy= true;
				moveTo= MOVE_TO_CANNON;
				return ran;
		}
	}

	//Ищем свободные мачты
		if (shipState.dead || rand()*30/float(RAND_MAX)<= 1)

		for (int m= 0; m< sailorsPoints.points.count; m++)
		for (int i= 0; i< sailorsPoints.points.count; i++){

		ran= int(rand()*(sailorsPoints.points.count - 1)/float(RAND_MAX));

		if (!sailorsPoints.points.point[ran].disabled)

		if (sailorsPoints.points.point[ran].IsMast())

			if (ran!= targetWayPoint && (shipState.dead || !sailorsPoints.points.point[ran].buisy)){
				
  				sailorsPoints.points.point[ran].buisy= true;
				moveTo= MOVE_TO_TOP;
				return ran;
		}
	}


	//Ищем простые незанятые точки
		for (int m= 0; m< sailorsPoints.points.count; m++)
		for (int i= 0; i< sailorsPoints.points.count; i++){

		ran= int(rand()*(sailorsPoints.points.count - 1)/float(RAND_MAX));

			if (ran!= targetWayPoint && !sailorsPoints.points.point[ran].buisy &&
				sailorsPoints.points.point[ran].pointType== PT_TYPE_NORMAL){
				
				moveTo= MOVE_TO_POINT;
				return ran;
		}
	}
	UNGUARD_SAILORS
	return newWayPoint;
};
//-----Найти новую точку-цель без типа------------------------------------------------
int  ShipMan::FindRandomPointWithoutType(SailorsPoints &sailorsPoints) //Найти любую простую точку
{
	GUARD_SAILORS(ShipMan::FindRandomPointWithoutType())

	int ran;
	for (int m= 0; m< sailorsPoints.points.count; m++)
	for (int i= 0; i< sailorsPoints.points.count; i++)

		ran= int(rand()*(sailorsPoints.points.count - 1)/float(RAND_MAX));
		if (sailorsPoints.points.point[ran].pointType== PT_TYPE_NORMAL)
			return ran;

	return newWayPoint;

	UNGUARD_SAILORS
};
//-----Найти след. точку в пути / новый путь------------------------------------------
void ShipMan::FindNextPoint(SailorsPoints &sailorsPoints, ShipState &shipState)
{
	GUARD_SAILORS(ShipMan::FindNextPoint())

	//Найти ближайшую незаряженную пушку
	if (moveTo!= MOVE_TO_CANNON)
	{
			int cannon= GetNearestEmptyCannon(sailorsPoints);
			if (cannon>= 0) 
			{
				path.length= 0;
				path.min= -1;

				targetWayPoint= cannon;
  				sailorsPoints.points.point[targetWayPoint].buisy= true;
				moveTo= MOVE_TO_CANNON;
				mode= MAN_RUN;
				path= sailorsPoints.findPath(path,newWayPoint, targetWayPoint);

				if (path.min!= -1)
				{
					path.currentPointPosition= 0;
					newWayPoint= path.point[path.currentPointPosition];
				}

				return;
			}
	}

		//Если путь пройден или его нет- найти новый
		if (!path.length || path.currentPointPosition== path.length- 1 || path.min== -1){

			path.length= 0;
			path.min= -1;

			lastTargetPoint= targetWayPoint;

			targetWayPoint= FindRandomPoint(sailorsPoints, shipState);

			path= sailorsPoints.findPath(path,newWayPoint, targetWayPoint);

			if (path.min!= -1)
			{
				path.currentPointPosition= 0;
				lastWayPoint= newWayPoint;

				if (path.length> 1)
				newWayPoint= path.point[path.currentPointPosition+ 1]; else
				newWayPoint= path.point[path.currentPointPosition];

				ApplyTargetPoint(CVECTOR(sailorsPoints.points.point[newWayPoint].x,sailorsPoints.points.point[newWayPoint].y,sailorsPoints.points.point[newWayPoint].z),
								 (sailorsPoints.points.point[newWayPoint].pointType== PT_TYPE_NORMAL));
			}

		//Выбрать след. точку в пути
		} else 
		if (mode == MAN_RUN || mode == MAN_WALK || mode == MAN_CLIMB_UP || mode == MAN_CLIMB_DOWN)
		{
			path.currentPointPosition++;
			lastWayPoint = newWayPoint;

			newWayPoint= path.point[path.currentPointPosition];

			if (sailorsPoints.points.point[targetWayPoint].disabled)
			{
				path.length= 0;
				FindNextPoint(sailorsPoints, shipState);
				return;
			}

			//Если начался бой/перезарядка то перейти на бег
			if ((shipState.mode == SHIP_WAR || moveTo == MOVE_TO_CANNON) && mode == MAN_WALK)
				mode= MAN_RUN;

			ApplyTargetPoint(CVECTOR(sailorsPoints.points.point[newWayPoint].x,sailorsPoints.points.point[newWayPoint].y,sailorsPoints.points.point[newWayPoint].z),
				             (sailorsPoints.points.point[newWayPoint].pointType == PT_TYPE_NORMAL));
		}
	UNGUARD_SAILORS
};
//------------------------------------------------------------------------------------
void ShipMan::ApplyTargetPoint(CVECTOR pt, bool randomWalk)
{	
	GUARD_SAILORS(ShipMan::ApplyTargetPoint())

	if (randomWalk){ //разброс - 0.50

		ptTo.x= pt.x+ (rand()/float(RAND_MAX)- rand()/float(RAND_MAX))*0.50f;
		ptTo.y= pt.y;
		ptTo.z= pt.z+ (rand()/float(RAND_MAX)- rand()/float(RAND_MAX))*0.50f;

	} else 
	{
		ptTo= pt;
	}

		dir = !(ptTo - pos);

		if (mode!= MAN_CLIMB_UP)
		{
			angTo.y=  Vector2Angle(dir);
		}
	UNGUARD_SAILORS
};
//------------------------------------------------------------------------------------
bool ShipMan::MoveToPosition(dword &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState){
	
	GUARD_SAILORS(ShipMan::MoveToPosition())

	float dNow = (float) 
				 SQR(pos.x - ptTo.x)
				+SQR(pos.y - ptTo.y)
				+SQR(pos.z - ptTo.z);

	float dFuture = (float)
					SQR(pos.x+manSpeed*dir.x - ptTo.x)
				   +SQR(pos.y+manSpeed*dir.y - ptTo.y)
				   +SQR(pos.z+manSpeed*dir.z - ptTo.z);

	if (shipState.dead && mode == MAN_CLIMB_UP && dNow < sailorsPoints.points.point[newWayPoint].climbPosition * 10+ 1)
	{ 
			model->GetAnimation()->Player(0).Pause();
			sailorsPoints.points.point[newWayPoint].climbPosition++;
			mode= MAN_OFF;
			return false;
	};


	if (dFuture < dNow)
	{
		if (RotateToAngle(dltTime, sailorsPoints) || mode == MAN_RUN || (mode == MAN_WALK && abs(angTo.y- ang.y) < PI/8))
		{
			pos.x+= manSpeed*dir.x*dltTime/100.0f;
			pos.y+= manSpeed*dir.y*dltTime/100.0f;
			pos.z+= manSpeed*dir.z*dltTime/100.0f;
		}

		return false;
	}

	pos = ptTo;
	ang.y = angTo.y;

	return true;

	UNGUARD_SAILORS
};
//-----Найти ближайшую пушку----------------------------------------------------------
int ShipMan::GetNearestEmptyCannon(SailorsPoints &sailorsPoints)
{
	GUARD_SAILORS(ShipMan::GetNearestEmptyCannon())	

	float minDest= 999;
	float dest;
	int   minIndex= -1;

	for (int i = 0; i < sailorsPoints.points.count; i++)
		if (sailorsPoints.points.point[i].IsCannon() &&
			sailorsPoints.points.point[i].buisy == false &&
			sailorsPoints.points.point[i].cannonReloaded == false && 
			sailorsPoints.points.point[i].disabled == false)
	{

		dest= Dest(CVECTOR(sailorsPoints.points.point[i].x,
					       sailorsPoints.points.point[i].y,
						   sailorsPoints.points.point[i].z), pos);

		if (dest< minDest)
		{
			minDest= dest;
			minIndex= i;
		}
	}
	
	return minIndex;

	UNGUARD_SAILORS
};
//------------------------------------------------------------------------------------
bool ShipMan :: Swim(dword &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState)
{
	GUARD_SAILORS(ShipMan::Swim())

	if (dieTime< 9)
	{
		pos.y+=  ((shipState.sea->WaveXZ(pos.x, pos.z)- 1.4f) - pos.y)*dltTime/200.0f;
		pos.x+= sin(ang.y)*manSpeed*dltTime/500.0f;
		pos.z+= cos(ang.y)*manSpeed*dltTime/500.0f;

		RotateToAngle(dltTime, sailorsPoints);

	} else //Силы на исходе - утопить
		pos.y-= dltTime/1000.0f;

        dieTime+= 1.5f/float(dltTime);

		if (dieTime>= 10) return true;
		return false;

	UNGUARD_SAILORS
};
//------------------------------------------------------------------------------------
bool ShipMan::Stay(dword &dltTime, SailorsPoints &sailorsPoints)
{
	GUARD_SAILORS(ShipMan::Stay())	

	return (!model->GetAnimation()->Player(0).IsPlaying());

	UNGUARD_SAILORS
};
//------------------------------------------------------------------------------------
bool ShipMan::Turn(dword &dltTime, SailorsPoints &sailorsPoints)
{
	GUARD_SAILORS(ShipMan::Turn())	

	return RotateToAngle(dltTime, sailorsPoints);

	UNGUARD_SAILORS
};
//------------------------------------------------------------------------------------
bool ShipMan::Jump(dword &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState)
{
	GUARD_SAILORS(ShipMan::Jump())	
		pos.y-=  jumpSpeedY*dltTime/1500.0f;

	    pos.x+= sin(ang.y + PI)*jumpSpeedX*dltTime/350.0f;
		pos.z+= cos(ang.y + PI)*jumpSpeedX*dltTime/350.0f;

		jumpSpeedY+= float(dltTime)/10.0f;
		jumpSpeedX-= jumpSpeedX*float(dltTime)/750.0f;

		if (shipState.sea && pos.y<= shipState.sea->WaveXZ(pos.x, pos.z)- 1.4f) //Утопить поглубже
			pos.y-= jumpSpeedY*float(dltTime)/1000.0f;

		return false;		

	UNGUARD_SAILORS
};
//-----Обновить анимацию и скорость---------------------------------------------------
void ShipMan::SetAnimation(dword dltTime, ShipState &shipState){

	GUARD_SAILORS(ShipMan::SetAnimation())	

	if (mode== lastMode && model->GetAnimation()->Player(0).IsPlaying()) return;
	float ran;

	switch(mode){

		case MAN_WALK:
				model->GetAnimation()->Player(0).SetAction("walk");
				model->GetAnimation()->Player(0).SetPosition(rand()/float(RAND_MAX));
				model->GetAnimation()->Player(0).Play();

				manSpeed= MOVE_SPEED+ rand()*MOVE_SPEED/float(RAND_MAX)/4.0f- 
									  rand()*MOVE_SPEED/float(RAND_MAX)/4.0f;

				rotSpeed= MOVE_SPEED*3.0f;
				break;
		case MAN_RUN:
				model->GetAnimation()->Player(0).SetAction("run");
				model->GetAnimation()->Player(0).SetPosition(rand()/float(RAND_MAX));
				model->GetAnimation()->Player(0).Play();

				manSpeed= RUN_SPEED;
				rotSpeed= MOVE_SPEED*5.0f;
				break;
		case MAN_STAY:

				ran= rand()/float(RAND_MAX);

				if(ran< 0.25f)
					model->GetAnimation()->Player(0).SetAction("action1"); else
				if(ran< 0.50f)
					model->GetAnimation()->Player(0).SetAction("action2"); else
				if(ran< 0.75f)
					model->GetAnimation()->Player(0).SetAction("action3"); else
				if(ran< 1.00f)
					model->GetAnimation()->Player(0).SetAction("action4");


				model->GetAnimation()->Player(0).SetPosition(rand()/float(RAND_MAX));
				model->GetAnimation()->Player(0).Play();
				break;
		case MAN_TURNLEFT:
				model->GetAnimation()->Player(0).SetAction("turn_left");
				model->GetAnimation()->Player(0).SetPosition(0);
				model->GetAnimation()->Player(0).Play();
				break;
		case MAN_TURNRIGHT:
				model->GetAnimation()->Player(0).SetAction("turn_right");
				model->GetAnimation()->Player(0).SetPosition(0);
				model->GetAnimation()->Player(0).Play();
				break;
		case MAN_CLIMB_UP:

				manSpeed= CLIMB_SPEED;
				rotSpeed= MOVE_SPEED*10.0f;

				model->GetAnimation()->Player(0).SetAction("crawl");
				model->GetAnimation()->Player(0).SetPosition(0);
				model->GetAnimation()->Player(0).Play();

				if (shipState.dead || shipState.mode == SHIP_WAR || shipState.mode == SHIP_STORM){
					manSpeed*=2;
					model->GetAnimation()->Player(0).SetSpeed(
					model->GetAnimation()->Player(0).GetSpeed()*2);
				}

				break;
		case MAN_CLIMB_DOWN:

				manSpeed= CLIMB_SPEED;
				rotSpeed= MOVE_SPEED*10.0f;

				model->GetAnimation()->Player(0).SetAction("crawl_down");
				model->GetAnimation()->Player(0).SetPosition(0);
				model->GetAnimation()->Player(0).Play();

				if (shipState.dead || shipState.mode == SHIP_WAR || shipState.mode == SHIP_STORM){
					manSpeed*=2;
					model->GetAnimation()->Player(0).SetSpeed(
					model->GetAnimation()->Player(0).GetSpeed()*2);
				}
				break;
		case MAN_CANNONRELOAD:

				model->GetAnimation()->Player(0).SetAction("reload");
				model->GetAnimation()->Player(0).SetPosition(0);
				model->GetAnimation()->Player(0).Play();
				break;
	}

	lastMode = mode;

	UNGUARD_SAILORS
};
//-----Выбрать новое действие---------------------------------------------------------
void ShipMan::NewAction(SailorsPoints &sailorsPoints, ShipState &shipState,dword &dltTime)
{
	GUARD_SAILORS(ShipMan::NewAction())	
	if (!sailorsPoints.links.count) return;

	//Если путь пройден  выбрать новое действие
	if (mode!= MAN_CLIMB_UP && path.currentPointPosition== path.length- 1 || mode== MAN_STAY){

		if (mode== MAN_CANNONRELOAD && (shipState.mode== SHIP_SAIL || shipState.mode== SHIP_STORM || shipState.mode== SHIP_WAR))
		{ //Уйти от пушки

			sailorsPoints.points.point[targetWayPoint].buisy = false;
			sailorsPoints.points.point[targetWayPoint].cannonReloaded = true;

			FindNextPoint(sailorsPoints, shipState);
			if (shipState.mode== SHIP_WAR) mode= MAN_RUN; else mode= MAN_WALK;
			return;
		};
	
		if (targetWayPoint== newWayPoint && moveTo== MOVE_TO_CANNON)
		{ //Работать у пушки

			mode = MAN_CANNONRELOAD;

			ang.y= angTo.y= Vector2Angle(!(CVECTOR(sailorsPoints.points.point[targetWayPoint].x,
										           sailorsPoints.points.point[targetWayPoint].y,
												   sailorsPoints.points.point[targetWayPoint].z) -

										   CVECTOR(sailorsPoints.points.point[lastWayPoint].x,
												   sailorsPoints.points.point[lastWayPoint].y,
												   sailorsPoints.points.point[lastWayPoint].z)));
			return;
		};

		if (mode== MAN_CLIMB_DOWN)
		{ //Слезть и освободить точку

			mode= MAN_WALK;
			sailorsPoints.points.point[lastTargetPoint].buisy= false;
			return;
		};

		switch(shipState.mode)
		{
			case SHIP_SAIL:
								float ran;
								ran= rand()/float(RAND_MAX);

								if(mode!= MAN_STAY && ran< 0.1f) mode= MAN_RUN;  else 
								if(ran< 0.6f || mode== MAN_STAY) mode= MAN_WALK;  
								else mode= MAN_STAY;
								break;
			case SHIP_WAR:
			case SHIP_STORM:
								mode= MAN_RUN;
								break;
		}	
	}

	//найти/продолжить путь
	FindNextPoint(sailorsPoints, shipState);

	if (sailorsPoints.points.point[newWayPoint].IsMast())
	{
		mode = MAN_CLIMB_UP; 
	} else					
	if (sailorsPoints.points.point[lastWayPoint].IsMast())
	{
		mode = MAN_CLIMB_DOWN; 
	} else
	if (mode == MAN_CLIMB_DOWN) 
	{
		mode= MAN_WALK;

		if (lastTargetPoint>= 0 && lastTargetPoint< sailorsPoints.points.count)
		sailorsPoints.points.point[lastTargetPoint].buisy= false;
	}

	UNGUARD_SAILORS
};
//-----Основная ф-ия-----------------------------------------------------
void ShipMan::UpdatePos(dword &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState)
{
	GUARD_SAILORS(ShipMan::UpdatePos())	

	switch(mode){

		case MAN_WALK: 
		case MAN_RUN:
		case MAN_CLIMB_UP: 
		case MAN_CLIMB_DOWN: 
		
						if (MoveToPosition(dltTime, sailorsPoints, shipState)) NewAction(sailorsPoints,shipState,dltTime);
						break;

		case MAN_STAY:
		case MAN_CANNONRELOAD:
						if (Stay(dltTime, sailorsPoints)) NewAction(sailorsPoints,shipState,dltTime);
						break;

		case MAN_TURNLEFT:
		case MAN_TURNRIGHT:

						if (Turn(dltTime, sailorsPoints)) NewAction(sailorsPoints,shipState,dltTime);
						break;

		case MAN_SWIM:
						Swim(dltTime, sailorsPoints, shipState);
						break;
		case MAN_JUMP:
						Jump(dltTime, sailorsPoints, shipState);
						break;
	}

	SetAnimation(dltTime, shipState);

	UNGUARD_SAILORS
};


//------------------------------------------------------------------------------------
void ShipWalk::Free()
{	
	GUARD_SAILORS(ShipWalk::Free())	

	for (int i = 0; i< crewCount; i++)
	{
		shipMan[i].Free();
	}

	UNGUARD_SAILORS
};
//-----Обнулить флаги зарядки пушек---------------------------------------------------
void ShipWalk::ReloadCannons(int bort)
{
	GUARD_SAILORS(ShipWalk::ReloadCannons())

	for (int i= 0; i< sailorsPoints.points.count; i++)
		if ((sailorsPoints.points.point[i].pointType== PT_TYPE_CANNON_L && (bort== 0 || bort== 1)) ||
			(sailorsPoints.points.point[i].pointType== PT_TYPE_CANNON_R && (bort== 0 || bort== 2)) ||
			(sailorsPoints.points.point[i].pointType== PT_TYPE_CANNON_F && (bort== 0 || bort== 3)) ||
			(sailorsPoints.points.point[i].pointType== PT_TYPE_CANNON_B && (bort== 0 || bort== 4)))
			sailorsPoints.points.point[i].cannonReloaded= false;

	UNGUARD_SAILORS
};
//------------------------------------------------------------------------------------
void ShipWalk::CreateNewMan(SailorsPoints &sailorsPoints)
{
	GUARD_SAILORS(ShipWalk::CreateNewMan())	

	if (crewCount >= 50 || !sailorsPoints.points.count) return;

	ENTITY_ID manID;
	shipMan.Add(); 

	int current = shipMan.Len()- 1;

	api->CreateEntity(&shipMan[current].modelID,"MODELR");

	switch (rand() % 6) // 6 different character types
	{
	case 0:
		_CORE_API->Send_Message(shipMan[current].modelID,"ls",MSG_MODEL_LOAD_GEO, "Lowcharacters\\Lo_Man_1");
		break;
	case 1:
		_CORE_API->Send_Message(shipMan[current].modelID,"ls",MSG_MODEL_LOAD_GEO, "Lowcharacters\\Lo_Man_2");
		break;
	case 2:
		_CORE_API->Send_Message(shipMan[current].modelID,"ls",MSG_MODEL_LOAD_GEO, "Lowcharacters\\Lo_Man_3");
		break;
	case 3:
		_CORE_API->Send_Message(shipMan[current].modelID,"ls",MSG_MODEL_LOAD_GEO, "Lowcharacters\\Lo_Man_Kamzol_1");
		break;
	case 4:
		_CORE_API->Send_Message(shipMan[current].modelID,"ls",MSG_MODEL_LOAD_GEO, "Lowcharacters\\Lo_Man_Kamzol_2");
		break;
	case 5:
		_CORE_API->Send_Message(shipMan[current].modelID,"ls",MSG_MODEL_LOAD_GEO, "Lowcharacters\\Lo_Man_Kamzol_3");
		break;
	}	
	
	api->Send_Message(shipMan[current].modelID,"ls",MSG_MODEL_LOAD_ANI, "Lo_Man");

	shipMan[current].model = (MODEL*)_CORE_API->GetEntityPointer(&shipMan[current].modelID);	
	shipMan[current].SetAnimation(0, shipState);
	
	if (sailorsPoints.points.count)
	{
	    shipMan[current].newWayPoint = shipMan[current].FindRandomPointWithoutType(sailorsPoints);

		shipMan[current].pos.x = sailorsPoints.points.point[shipMan[current].newWayPoint].x;
		shipMan[current].pos.y = sailorsPoints.points.point[shipMan[current].newWayPoint].y;
		shipMan[current].pos.z = sailorsPoints.points.point[shipMan[current].newWayPoint].z;
		
		DWORD dltTime = 0;
		shipMan[current].NewAction(sailorsPoints,shipState,dltTime);
	}
	crewCount++;

	UNGUARD_SAILORS
};
//------------------------------------------------------------------------------------
void ShipWalk::DeleteMan(int Index)
{
	GUARD_SAILORS(ShipWalk::DeleteMan())

	if (Index>= 0 && Index< crewCount)
	{
	    shipMan[Index].Free();
		shipMan.DelIndex(Index);
		crewCount--;
	}

	UNGUARD_SAILORS
};
//------------------------------------------------------------------------------------
void ShipWalk::Init(ENTITY_ID &_shipID, int editorMode, char *shipType)
{
	GUARD_SAILORS(ShipWalk::Init())	

	crewCount = 0;
	bHide = false;
	shipID = _shipID;

	ENTITY_ID seaID;
	api->FindClass(&seaID, "sea", 0);
	shipState.sea = (SEA_BASE*) _CORE_API->GetEntityPointer(&seaID);

	if (!editorMode)
	{ //Режим игры - создан корабль

		ship = (SHIP_BASE *)api->GetEntityPointer(&_shipID);
		shipModel = ship->GetModel();

		//Загрузить точки
		string fileName= "RESOURCE\\MODELS\\Ships\\SAILORSPOINTS\\";
		fileName+= shipType;
		fileName+= ".ini";

		GUARD_SAILORS(sailorsPoints.ReadFromFile)
		sailorsPoints.ReadFromFile(fileName);
		UNGUARD_SAILORS

		if (sailorsPoints.points.count <= 0 || sailorsPoints.links.count <= 0) return;

		//Найти сломанные мачты
		ATTRIBUTES *attr = ship->GetACharacter();
		ATTRIBUTES *mastsAttr = attr->FindAClass(attr, "Ship.Masts");

		int iNumMasts = mastsAttr->GetAttributesNum();

		for (int i= 0; i< iNumMasts; i++)
		{
			if (mastsAttr->GetAttributeClass(i)->GetAttributeAsFloat())
			SetMastBroken(((iNumMasts - 1) - i) + 1); //??? Мачты расположены наоборот???
		}

		// people count
		//ATTRIBUTES *att = ship->GetACharacter();
		ATTRIBUTES *paShip = ship->GetAShip();
		int peopleCount = 5;
		if( paShip ) {
			peopleCount = paShip->GetAttributeAsDword("lowpolycrew",peopleCount);
		}
		//ATTRIBUTES *crewAttr = att->FindAClass(attr, "ship.crew");
		//int peopleCount = (int) ceilf(CREW2VISIBLE((float) crewAttr->GetAttributeAsDword("quantity", 1)));		
	
		if (peopleCount > 100) peopleCount = 100;
		if (peopleCount < 0) peopleCount = 0;

		for (int i= 0; i< peopleCount; i++)
			CreateNewMan(sailorsPoints);


	} else //Режим редактора
	{
		ship = NULL;
	}

	UNGUARD_SAILORS
};
//-----Выключение точек сломанной мачты-----------------------------------------------
void ShipWalk::SetMastBroken(int iMastIndex)
{
	GUARD_SAILORS(ShipWalk::SetMastBroken())	

	for (int i= 0; i< sailorsPoints.points.count; i++){

		if (sailorsPoints.points.point[i].pointType == PT_TYPE_MAST_1 && iMastIndex== 1)
			sailorsPoints.points.point[i].disabled= true;

		if (sailorsPoints.points.point[i].pointType == PT_TYPE_MAST_2 && iMastIndex== 2)
			sailorsPoints.points.point[i].disabled= true;
							
		if (sailorsPoints.points.point[i].pointType == PT_TYPE_MAST_3 && iMastIndex== 3)
			sailorsPoints.points.point[i].disabled= true;
							
		if (sailorsPoints.points.point[i].pointType == PT_TYPE_MAST_4 && iMastIndex== 4)
			sailorsPoints.points.point[i].disabled= true;
							
		if (sailorsPoints.points.point[i].pointType == PT_TYPE_MAST_5 && iMastIndex== 5)
			sailorsPoints.points.point[i].disabled= true;
	}		

	for (int i= 0; i< crewCount; i++)
	if (sailorsPoints.points.point[shipMan[i].newWayPoint].disabled ||
		sailorsPoints.points.point[shipMan[i].lastWayPoint].disabled)
		{
				shipMan[i].mode = MAN_JUMP;
				shipMan[i].jumpSpeedX = JUMP_SPEED_X_MASTFALL;
		}	
	UNGUARD_SAILORS
};
//-----Попадание ядер в корабль-------------------------------------------------------
void ShipWalk::OnHullHit(CVECTOR &v)
{
	GUARD_SAILORS(ShipWalk::OnHullHit())	
	for (int i= 0; i< crewCount; i++)
		if (10*rand()/RAND_MAX< 3 && shipMan[i].mode!= MAN_JUMP && shipMan[i].mode!= MAN_SWIM)
		{

			CVECTOR pos;
			if (ship) pos= shipModel->mtx*shipMan[i].pos; else pos= shipMan[i].pos;

			if (sqrt((pos.x- v.x)*(pos.x- v.x)+ (pos.z- v.z)*(pos.z- v.z))< 1)
			{
				//Освободить точки
				if (shipMan[i].mode== MAN_CLIMB_DOWN)
				{
					sailorsPoints.points.point[shipMan[i].lastTargetPoint].buisy= false; 
				}
				else 
				{
					sailorsPoints.points.point[shipMan[i].targetWayPoint].buisy= false;
					sailorsPoints.points.point[shipMan[i].newWayPoint].buisy= false;
				};

				shipMan[i].mode= MAN_JUMP;
				shipMan[i].jumpSpeedY= -(JUMP_SPEED_Y_EXPLOSION +rand()*JUMP_SPEED_Y_EXPLOSION/float(RAND_MAX));
				shipMan[i].jumpSpeedX= -shipMan[i].jumpSpeedY/10;
			}
		}

	UNGUARD_SAILORS
};
//-----Обход друг друга---------------------------------------------------------------
void ShipWalk::CheckPosition(dword &dltTime)
{

	GUARD_SAILORS(ShipWalk::CheckPosition())

	for (int m= 0; m< crewCount; m++)
	{
		shipMan[m].spos.x -= shipMan[m].spos.x / 100.0f * float(dltTime) / 10.0f;
		shipMan[m].spos.z -= shipMan[m].spos.z / 100.0f * float(dltTime) / 10.0f;
	}

	float d;

	for (int m= 0; m< crewCount; m++)
	for (int i= 0; i< crewCount; i++)

	if (i!= m && Dest(shipMan[m].pos, shipMan[i].pos, 1) &&
	(shipMan[m].mode== MAN_WALK || shipMan[m].mode== MAN_RUN) &&
	shipMan[m].path.currentPointPosition >= 0 && shipMan[m].path.currentPointPosition < shipMan[m].path.length &&
	shipMan[i].path.currentPointPosition >= 0 && shipMan[i].path.currentPointPosition < shipMan[i].path.length)
	{
		d = Dest(shipMan[m].pos, shipMan[i].pos);

		if (d < 1.0f)
		{
			//если Идут в разные стороны
			if (shipMan[m].path.point[shipMan[m].path.currentPointPosition]!=
				shipMan[i].path.point[shipMan[i].path.currentPointPosition])
			{
					shipMan[m].spos.x+= 0.2f*(+shipMan[m].dir.z*(1-d)- shipMan[m].spos.x)/15.0f*float(dltTime)/20.0f;
					shipMan[m].spos.z+= 0.2f*(-shipMan[m].dir.x*(1-d)- shipMan[m].spos.z)/15.0f*float(dltTime)/20.0f;
			} else 
			{ 
				if (m< i){
					shipMan[m].spos.x+= 0.2f*(+shipMan[m].dir.z*(1-d)- shipMan[m].spos.x)/15.0f*float(dltTime)/20.0f;
					shipMan[m].spos.z+= 0.2f*(-shipMan[m].dir.x*(1-d)- shipMan[m].spos.z)/15.0f*float(dltTime)/20.0f;
				} else {
					shipMan[m].spos.x+= 0.2f*(-shipMan[m].dir.z*(1-d)- shipMan[m].spos.x)/15.0f*float(dltTime)/20.0f;
					shipMan[m].spos.z+= 0.2f*(+shipMan[m].dir.x*(1-d)- shipMan[m].spos.z)/15.0f*float(dltTime)/20.0f;
				}
			}

			break;
		}
	}

	UNGUARD_SAILORS
};
//------------------------------------------------------------------------------------
void ShipWalk::Reset()
{

	return;
/*
	for(int i= 0; i< sailorsPoints.points.count; i++)
	{
		sailorsPoints.points.point[i].buisy= false;
		sailorsPoints.points.point[i].cannonReloaded= true;
	};

	bool result= true;

	while (result)
	{
		result= false;

		for(int i= 0; i< crewCount; i++){

			if (shipMan[i].mode == MAN_SWIM || shipMan[i].mode == MAN_JUMP)
			{
				DeleteMan(i);
				result= true;
				break;
			};

		};
	};


	for(int i= 0; i< crewCount; i++)
	{
		shipMan[i].newWayPoint= shipMan[i].FindRandomPointWithoutType(sailorsPoints);

		shipMan[i].pos.x= sailorsPoints.points.point[shipMan[i].newWayPoint].x;
		shipMan[i].pos.y= sailorsPoints.points.point[shipMan[i].newWayPoint].y;
		shipMan[i].pos.z= sailorsPoints.points.point[shipMan[i].newWayPoint].z;

		shipMan[i].mode= MAN_WALK;

		DWORD dltTime= 0;
		shipMan[i].NewAction(sailorsPoints,shipState,dltTime);
	};*/
};
//------------------------------------------------------------------------------------
Sailors::Sailors():shipWalk(_FL_)
{
	GUARD_SAILORS(Sailors::Sailors():shipWalk())

	shipsCount= 0;
	editorMode= false;
	disabled= false;
	IsOnDeck= 0;

	UNGUARD_SAILORS
};
//------------------------------------------------------------------------------------
Sailors::~Sailors()
{
	GUARD_SAILORS(Sailors::~Sailors())	

	for (int i= 0; i< shipsCount; i++)
	{
		shipWalk[i].Free();
	}
	UNGUARD_SAILORS
}
//------------------------------------------------------------------------------------
bool Sailors::Init()
{
	GUARD_SAILORS(Sailors :: Init())	

	rs = (VDX8RENDER *) _CORE_API->CreateService("dx8render");

	api->LayerCreate("sea_realize", true, false);
	api->LayerSetFlags("sea_realize", LRFLAG_REALIZE);
	api->LayerAdd("sea_realize", GetID(), 65530);

	UNGUARD_SAILORS

	return true;
}
//------------------------------------------------------------------------------------
void Sailors::Realize(dword dltTime)
{
	GUARD_SAILORS(Sailors :: Realize())	

	if (dltTime> 500) 
	{
		dltTime= 500;
	}

	if (disabled) return;

	rs->SetRenderState(D3DRS_LIGHTING,true);



#ifdef SAILORS_DEBUG
	if (api->Controls->GetDebugAsyncKeyState(VK_F7)< 0)
	{
		for (int i= 0; i<shipsCount; i++)
		{ 
			shipWalk[i].SetMastBroken(3);
			shipWalk[i].SetMastBroken(2);
			shipWalk[i].SetMastBroken(1);
		}
	}
#endif

	for (int m= 0; m< shipsCount; m++)
	{
		//Если корабль и все люди умерли то удалить обьект
		if (shipWalk[m].shipState.dead && shipWalk[m].crewCount <= 0)
		{	
			DeleteShip(m);
			rs->SetRenderState(D3DRS_LIGHTING,false);
			return;
		}

		//Обновление и рисование
		for (int i= 0; i< shipWalk[m].shipMan.Len(); i++)
		{	
			shipWalk[m].shipMan[i].UpdatePos(dltTime, shipWalk[m].sailorsPoints, shipWalk[m].shipState);
			shipWalk[m].shipMan[i].SetPos(shipWalk[m].shipModel,shipWalk[m].ship,dltTime, shipWalk[m].shipState);	

			if(!shipWalk[m].bHide)
			shipWalk[m].shipMan[i].model->Realize(dltTime);
		}

		for (int i= 0; i< shipWalk[m].shipMan.Len(); i++)
		{	//Если умер то удалить
			if (shipWalk[m].shipMan[i].dieTime> 10 || shipWalk[m].shipMan[i].pos.y< -100)
			{
				shipWalk[m].DeleteMan(i);
				rs->SetRenderState(D3DRS_LIGHTING,false);
				return;
			}
		}

		//Установка состояния корабля
		if (!shipWalk[m].shipState.dead)  
		{
			shipWalk[m].CheckPosition(dltTime);
	
			if (shipWalk[m].ship && !shipWalk[m].shipState.dead && !editorMode){
				///shipState
				ATTRIBUTES *shipAttr = shipWalk[m].ship->GetACharacter();
				ATTRIBUTES *shipModeAttr = shipAttr->FindAClass(shipAttr, "ship.POS.mode");

				if (shipModeAttr)
				{ 
					shipWalk[m].shipState.mode= shipModeAttr->GetAttributeAsDword();
				}
			}
		}
	}

	rs->SetRenderState(D3DRS_LIGHTING,false);
	UNGUARD_SAILORS
}
//------------------------------------------------------------------------------------
void Sailors::DeleteShip(int i)
{
	GUARD_SAILORS(Sailors::DeleteShip)	

	shipWalk[i].Free();
	shipWalk.DelIndex(i);
	shipsCount--;

	UNGUARD_SAILORS
}
//------------------------------------------------------------------------------------
dword Sailors :: ProcessMessage(MESSAGE &message)
{
	GUARD_SAILORS(Sailors::ProcessMessage())	

	long code = message.Long();
	dword outValue = 0;
	ENTITY_ID shipID;
	char c[20];

	switch (code)
	{
	// Добавить людей на корабль
	case AI_MESSAGE_ADD_SHIP:

		shipID = message.EntityID();
		message.String(sizeof(c), c);


 		shipWalk.Add();
		shipWalk[shipsCount].Init(shipID, editorMode, &c[0]);
		shipsCount++;

		if (!editorMode)
		if (!shipWalk[shipsCount- 1].sailorsPoints.points.count || !shipWalk[shipsCount- 1].sailorsPoints.links.count){
			
			DeleteShip(shipsCount- 1);
			api->Trace("Sailors: sailors ship %s deleted",&c[0]);
			return 0;
		}
		break;

	// Перезарядка бортов
	case AI_MESSAGE_CANNON_RELOAD:	

		shipID = message.EntityID();
		char bortName[256];
		message.String(256, (char *) bortName);

	    for (int i= 0; i< shipsCount; i++)
		if (shipID == shipWalk[i].shipID){


			if (!strcmp(bortName, "cannonl"))
			{
					shipWalk[i].ReloadCannons(1);
					return outValue;
			}

			if (!strcmp(bortName, "cannonr"))
			{
					shipWalk[i].ReloadCannons(2);
					return outValue;
			}

			if (!strcmp(bortName, "cannonf"))
			{
					shipWalk[i].ReloadCannons(3);
					return outValue;
			}

			if (!strcmp(bortName, "cannonb"))
			{
					shipWalk[i].ReloadCannons(4);
					return outValue;
			}

			shipWalk[i].ReloadCannons(0);
			break;
		};

		break;

	// Падение мачты
	case MSG_PEOPLES_ON_SHIP_MASTFALL:
		{
			ATTRIBUTES *attrs = message.AttributePointer();
			if (!attrs) return 0;

				for (int m = 0; m < shipsCount; m++)
					if (attrs == shipWalk[m].ship->GetACharacter())
					{
						shipWalk[m].SetMastBroken(message.Long());
						return outValue;
					}
		
		break;
		}

	// Попадание ядра в корабль
	case MSG_PEOPLES_ON_SHIP_HULLHIT:
		{

			ATTRIBUTES *attrs = message.AttributePointer();
			if (!attrs) return 0;

				for (int m = 0; m < shipsCount; m++)
					if (attrs == shipWalk[m].ship->GetACharacter())
					{
						float x= message.Float();
						float y= message.Float();
						float z= message.Float();

						shipWalk[m].OnHullHit(CVECTOR(x,y,z));
						return outValue;
					}
		
		break;
		};

	// Удаление корабля
	case MSG_SHIP_DELETE:
		{
			ATTRIBUTES *attrs = message.AttributePointer();
			if (attrs)
				for (int m = 0; m < shipsCount; m++)
					if (attrs == shipWalk[m].ship->GetACharacter())
					{

						shipWalk[m].shipState.dead= true;
						shipWalk[m].shipState.mode= SHIP_STORM;

						return outValue;
					}
		break;
		}
	}

	return outValue; 

	UNGUARD_SAILORS
};

//------------------------------------------------------------------------------------
dword Sailors::AttributeChanged(ATTRIBUTES *_newAttr)
{
	GUARD_SAILORS(Sailors::AttributeChanged())	

	// Убрать людей с палубы
	if (*_newAttr == "IsOnDeck")
	{
		IsOnDeck = this->AttributesPointer->GetAttributeAsDword("IsOnDeck") != 0;

		if (IsOnDeck)
		{
			for (int i= 0; i< shipsCount; i++)
			{
				if (shipWalk[i].ship->GetACharacter()->GetAttribute("MainCharacter"))
				{
					shipWalk[i].bHide = true;
					shipWalk[i].Reset();
					break;
				}
			}

		} else 
		{
			for (int i= 0; i< shipsCount; i++)
			{
				shipWalk[i].bHide = false;
			}
		}
		return 0;
	}

	UNGUARD_SAILORS
	
	return 0;
};
//-----end----------------------------------------------------------------------------