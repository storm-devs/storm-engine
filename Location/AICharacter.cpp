//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	AICharacter
//--------------------------------------------------------------------------------------------
//	
//============================================================================================


#include "AICharacter.h"
#include "Location.h"


#define FALURE_GOTO		2.0f


//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

AICharacter::AICharacter()
{
	currentNode = -1;
	likeKSpd = 0.9f + rand()*0.2f/RAND_MAX;
	collisionValue = 0.0f;

	bMusketer = false;
	bMusketerNoMove = false;
}

AICharacter::~AICharacter()
{

}

//============================================================================================
//Character
//============================================================================================

//Перемещаем персонажа в желаемую позицию
void AICharacter::Move(float dltTime)
{
	//Обнулим силы
	force = 0.0f;
	goForce = 0.0f;
	separation = 0.0f;
	alignment = 0.0f;
	around = 0.0f;
	//Процесируем команду
	switch(command.cmd)
	{
	case aicmd_none:
		break;
	case aicmd_stay:
		break;
	case aicmd_gotopoint:
		CmdProcessGotoPoint(dltTime);
		break;
	case aicmd_escape:
		CmdProcessEscape(dltTime);
		break;
	};
	Character::Move(dltTime);
}

//Провести дополнительные расчёты
void AICharacter::Calculate(float dltTime)
{
	CalcRepulsionForces();
	CVECTOR slideForce;
	location->GetPtcData().FindForce(currentNode, slideForce);
	//Ограничим вектора
	float l = ~separation;
	if(l > 1.0f) separation *= 1.0f/sqrtf(l);
	l = ~alignment;
	if(l > 1.0f) alignment *= 1.0f/sqrtf(l);
	l = ~around;
	if(l > 1.0f) around *= 1.0f/sqrtf(l);
	separation *= IsFight() ? 0.5f : 1.1f;
	alignment *= 0.1f;
	around *= 0.1f;
	goForce *= 1.2f;
	slideForce *= 0.2f;
	//Суммируем силы
	force += separation;
	force += alignment;
	force += around;
	force += goForce;
	force += slideForce;
	//if(isSlide) force += slideDir*0.3f;
	//Внесём шумность для помощи принятия решений
	force.x += rand()*0.0000001f/RAND_MAX;
	force.z += rand()*0.0000001f/RAND_MAX;

	if(location->IsDebugView()) 
	{
		location->DrawLine(curPos + CVECTOR(0, 0.01f, 0), 0xff0000ff, curPos + goForce + CVECTOR(0, 0.01f, 0), 0xff0000ff, false);
		location->DrawLine(curPos + CVECTOR(0, 0.02f, 0), 0xffff0000, curPos + separation + CVECTOR(0, 0.02f, 0), 0xffff0000, false);
		location->DrawLine(curPos + CVECTOR(0, 0.03f, 0), 0xff00ff00, curPos + alignment + CVECTOR(0, 0.03f, 0), 0xff00ff00, false);
		location->DrawLine(curPos + CVECTOR(0, 0.04f, 0), 0xffffff00, curPos + around + CVECTOR(0, 0.04f, 0), 0xffffff00, false);	
		location->DrawLine(curPos + CVECTOR(0, 0.1f, 0), 0xffffffff, curPos + force + CVECTOR(0, 0.1f, 0), 0xffffffff, false);
		location->DrawLine(curPos + CVECTOR(0, 0.1f, 0), 0xff00ffff, curPos + slideForce + CVECTOR(0, 0.1f, 0), 0xffffffff, false);
	}

	Character::Calculate(dltTime);
}

//Обновить позицию персонажа
void AICharacter::Update(float dltTime)
{
	switch(command.cmd)
	{
	case aicmd_none:
		break;
	case aicmd_stay:
		StopMove();
		break;
	case aicmd_gotopoint:
		CmdUpdateGotoPoint(dltTime);
		break;
	case aicmd_escape:
		CmdUpdateEscape(dltTime);
		break;
	};
	Character::Update(dltTime);
	if(isCollision)
	{
		collisionValue += dltTime*1.0f;
		if(collisionValue > 1.0f)
		{
			collisionValue = 0.8f;
			CollisionThreshold();
		}
	}else{
		collisionValue -= dltTime*0.1f;
		if(collisionValue < 0.0f) collisionValue = 0.0f;
	}
}

//Отметить перемещение персонажа
void AICharacter::CharacterTeleport()
{
	currentNode = FindNodeIndex(curPos);
	if(currentNode < 0) api->Trace("Warning: NPCharacter <%s>-> trace node not found",characterID);
}

//============================================================================================
//AICharacter
//============================================================================================

//Ничего не делать
bool AICharacter::CmdNone()
{
	command.cmd = aicmd_none;
	command.exch = null;
	return true;
}

//Стоять
bool AICharacter::CmdStay()
{
	command.cmd = aicmd_stay;
	command.exch = null;
	StopMove();
	return true;
}

//Идти в точку
bool AICharacter::CmdGotoPoint(float x, float y, float z, float rad, long node, bool isCheckBusyPos)
{
	if (bMusketer && bMusketerNoMove) return true;
	if(currentNode < 0) return false;
	if(node < 0) node = FindNodeIndex(CVECTOR(x, y, z), &y);
	if(node < 0) return false;
	command.cmd = aicmd_gotopoint;
	command.pnt = CVECTOR(x, y, z);
	command.node = node;
	command.tpnt = curPos;
	command.tnode = currentNode;
	command.radius = rad;
	command.waitTime = 0.0f;
	command.isWait = false;
	command.isBusy = isCheckBusyPos;
	command.cnt = 0;
	command.exch = null;
	StartMove();
	return true;
}

//Уходить от точки
bool AICharacter::CmdEscape(float x, float y, float z, float rad)
{
	if (bMusketer && bMusketerNoMove) return true;
	command.cmd = aicmd_escape;
	command.pnt = CVECTOR(x, y, z);
	command.node = -1;
	command.radius = rad;
	command.waitTime = 0.0f;
	command.isWait = false;
	command.exch = null;
	StartMove();
	return true;
}

//Установить персонажа с которым нерасталкиваемся
void AICharacter::SetExCharacter(AICharacter * chr)
{
	command.exch = chr;
}

//============================================================================================
//Инкапсуляция
//============================================================================================

//Идти в точку
void AICharacter::CmdProcessGotoPoint(float dltTime)
{
	if(command.isWait) return;
	//Найдём направление пути	
	command.tpnt = curPos;
	command.tnode = currentNode;
	if(!FindDirectional())
	{
		command.cmd = aicmd_none;
		StopMove();
		FailureCommand();
		return;
	}
	goForce.x = command.tpnt.x - curPos.x;
	goForce.y = 0.0f;
	goForce.z = command.tpnt.z - curPos.z;
	float fl = sqrtf(~goForce);
	float sn = sinf(ay);
	float cs = cosf(ay);
	if(fl <= 0.0f)
	{
		goForce.x = command.pnt.x - curPos.x;
		goForce.z = command.pnt.z - curPos.z;
		float fl = sqrtf(~goForce);
	}
	if(fl > 0.0f)
	{	
		goForce *= 1.0f/fl;
	}else{
		goForce.x = sn;
		goForce.z = cs;
	}
	fl = (fl - 0.5f)/(2.0f - 0.5f);
	if(fl < 0.0f) fl = 0.0f;
	if(fl > 1.0f) fl = 1.0f;
	if(goForce.x*sn + goForce.z*cs >= 0.9f - fl*0.1f)
	{
		StartMove();
	}else{
		if((rand() & 1023) == 5)
		{
			command.waitTime = 1.2f;
			command.isWait = true;
			Turn(goForce.x, goForce.z);
			StopMove();
			return;
		}
		StopMove();
	}	
	//Если дошли, то надо остановиться
	float dx = command.pnt.x - curPos.x;
	float dz = command.pnt.z - curPos.z;
	float d = dx*dx + dz*dz;
	if(location->IsDebugView()) 
	{
		float dist = sqrtf(d)*10.0f;
		if(dist > 255.0f) dist = 255.0f;
		dword color = long(dist);
		color |= (255 - color) << 16;
		color |= 0xff00ff00;
		location->DrawLine(command.pnt + CVECTOR(0, 0.01f, 0), color, command.pnt + CVECTOR(0, 3.01f, 0), color, false);
	}
	//Притормазим перед локатором
	if(d < 1.5f*1.5f) SetRunMode(false);
	if(d < command.radius*command.radius)
	{
		d = fabsf(command.pnt.y - curPos.y);
		if(d < height)
		{
			command.cmd = aicmd_none;
			StopMove();
			EndGotoCommand();
		}
	}else
	if(command.isBusy && d < FALURE_GOTO*FALURE_GOTO)
	{
		Assert(d > 0.0f);
		//if(sinf(ay)*dx + cosf(ay)*dz < 0)
		{
			if(!location->supervisor.CheckPosition(command.pnt.x, command.pnt.y, command.pnt.z, this))
			{
				if(!command.isWait)
				{
					if(command.cnt > dword(1 + (rand() & 3)))
					{
						command.cmd = aicmd_none;
						StopMove();
						FailureCommand();
					}else{
						_CORE_API->Event("Location_CharacterBusyPos", "ifff", GetID(), command.pnt.x, command.pnt.y, command.pnt.z);
						command.cnt++;
						command.isWait = true;
						command.waitTime = 2.0f + rand()*2.0f/RAND_MAX;
					}
				}
			}
		}
	}
	if(location->IsDebugView()) location->DrawLine(command.tpnt, 0xffff0000, command.tpnt + CVECTOR(0.0f, 3.0f, 0.0f), 0xff00ff00, false);
}

void AICharacter::CmdUpdateGotoPoint(float dltTime)
{
	if(!command.isWait)
	{
		StartMove();
		//if(location->IsDebugView()) location->DrawLine(curPos, 0xffff0000, curPos + force*2.0f, 0xffff0000, false);
		Turn(force.x, force.z);
		float l = force.x*force.x + force.z*force.z;
		if(l < 0.7f) kSpd = likeKSpd*l/0.7f;	else kSpd = likeKSpd;
		//Если скользим по краю - остановимся и подождём
		if(command.waitTime <= 0.0f)
		{
			/*
			if(isSlide && (rand() & 31) == 5)
			{
				StopMove();
				command.isWait = true;
				command.waitTime = 1.0f + rand()*1.0f/rand();
				Turn(goForce.x, goForce.z);
			}
			*/
		}else command.waitTime -= dltTime;
	}else{
		if(command.waitTime <= 0.0f)
		{
			if(location->supervisor.CheckPosition(command.pnt.x, command.pnt.y, command.pnt.z, this))
			{
				//Продолжаем путь
				StartMove();
				command.isWait = false;
				command.waitTime = 1.0f + rand()*1.0f/rand();
			}else{
				if(command.cnt < 3)
				{
					command.cnt++;
					command.isWait = true;
					command.waitTime = 2.0f + rand()*2.0f/RAND_MAX;
				}else{
					command.cmd = aicmd_none;
					StopMove();
					FailureCommand();
				}
			}
		}else{
			//Ожидаем
			command.waitTime -= dltTime;
			StopMove();
		}
	}
}

//Уходить от точки
void AICharacter::CmdProcessEscape(float dltTime)
{
	//Направление удаления
	goForce.x = curPos.x - command.pnt.x;
	goForce.y = 0.0f;
	goForce.z = curPos.z - command.pnt.z;
	float l = goForce.x*goForce.x + goForce.z*goForce.z;
	if(l >= command.radius*command.radius)
	{
		goForce = 0.0f;
		command.cmd = aicmd_none;
		StopMove();
		EndEscapeCommand();
		return;
	}
	if(l > 0.0f)
	{
		l = 1.0f/sqrtf(l);
		goForce.x *= l;
		goForce.z *= l;
	}else{
		goForce.x = 0.0f;
		goForce.z = 1.0f;
	}
	goForce *= 3.0f;
}

void AICharacter::CmdUpdateEscape(float dltTime)
{
	if(location->IsDebugView()) location->DrawLine(curPos, 0xffff0000, curPos + force*2.0f, 0xffff0000, false);
	Turn(force.x, force.z);
	float l = force.x*force.x + force.z*force.z;
	if(l < 0.7f) kSpd = likeKSpd*l/0.7f;	else kSpd = likeKSpd;
}

//--------------------------------------------------------------------------------------------


//Найти индекс нода для данной координаты
long AICharacter::FindNodeIndex(const CVECTOR & pos, float * hy)
{
	float yy;
	long node = location->GetPtcData().FindNode(pos, yy);
	if(hy) *hy = yy;
	return node;
}

//Найти направление куда идти (ориентация на местности)
bool AICharacter::FindDirectional()
{
	if(command.tnode < 0 || command.node < 0) return false;
	if(!location->GetPtcData().FindPathDir(command.tnode, CVECTOR(command.tpnt), command.node, command.pnt, command.tnode, command.tpnt)) return false;
	if(location->IsDebugView())
	{
		for(long i = 0; i < location->GetPtcData().numSteps; i++)
		{
			location->DrawLine(location->GetPtcData().stepPos[i] + CVECTOR(0, 0.01f, 0), 0xffff00ff, location->GetPtcData().stepPos[i] + CVECTOR(0, 2.01f, 0), 0xff2f80ff, false);
		}
	}
	return true;
}

//Найти расталкивающие силы
void AICharacter::CalcRepulsionForces()
{
	if(numColCharacter <= 0) return;
	float k;
	float kn = 1.0f/numColCharacter;
	for(long i = 0; i < numColCharacter; i++)
	{
		Supervisor::CharacterInfo & ci = location->supervisor.colchr[startColCharacter + i];
		if(ci.d == 0.0f) continue;
		AICharacter * c = (AICharacter *)ci.c;
		if(command.exch == c || c->command.exch == this) continue;
		float dx = c->curPos.x - curPos.x;
		float dz = c->curPos.z - curPos.z;
		float kd = 1.0f/ci.d;
		float kr = kn*(ci.maxD - ci.d)/ci.maxD;
		//Сила расталкивания
		float sx = dx*kr*kd*kd;
		float sz = dz*kr*kd*kd;
		separation.x -= sx;
		separation.z -= sz;
		c->separation.x += sx;
		c->separation.z += sz;
		//Сила выравнивания направлений
		k = 1.0f - fabsf(goForce.x*c->goForce.x + goForce.z*c->goForce.z);
		CVECTOR af = (goForce + c->goForce)*(kn*kr*kd*k);
		if(af.x*dx + af.z*dz < 0.0f)
		{
			alignment += af;
			c->alignment -= af;
		}else{
			alignment -= af;
			c->alignment += af;
		}
		//Сила для обхода
		float kcs = goForce.x*dx + goForce.z*dz;
		if(kcs > 0.0f)
		{
			k = kn*kr*kd*kcs;
			if(goForce.x*dz - goForce.z*dx < 0.0f) k = -k;
			around.x += k*goForce.z;
			around.z += k*-goForce.x;
			c->around.x -= k*goForce.z;
			c->around.z -= k*-goForce.x;
		}		
	}
}

//Вычислить точку образуемую пересечением и лежащую на ребре
bool AICharacter::FindIntersection(const CVECTOR & s, const CVECTOR & e, const CVECTOR & cur, const CVECTOR & to, CVECTOR & res)
{
	float deX = e.x - s.x;
	float deZ = e.z - s.z;
	float dX = to.x - cur.x;
	float dZ = to.z - cur.z;
	//Плоскасть проходящая через отрезок перемещения
	float nx = dZ;
	float nz = -dX;
	float nl = nx*nx + nz*nz;
	if(nl == 0.0f)
	{
		res = to;
		return false;
	}
	nl = sqrtf(nl);
	nx /= nl; nz /= nl;
	float d = cur.x*nx + cur.z*nz;
	//Расстояния вершин ребра до плоскости
	float ds = nx*s.x + nz*s.z - d;
	float de = nx*e.x + nz*e.z - d;
	//Решим что делать
	if(ds != de)
	{
		//Пересекаем плоскость
		float k = ds/(ds - de);
		if(k < 0.0f) k = 0.0f;
		if(k > 1.0f) k = 1.0f;
		res = s + (e - s)*k;		
		//Проверим сторну пересечения
		if(dX*(res.x - cur.x) + dZ*(res.z - cur.z) < 0.0f)
		{
			//Находимся не стой стороны, надо двигаться к краю
			if((deZ)*(dZ) - (-deX)*(dX) < 0.0f) res = s; else res = e;
		}
	}else{
		//Путь паралелен ребру
		if((deZ)*(dZ) - (-deX)*(dX) < 0.0f) res = s; else res = e;
		return false;
	}
	return true;
}

//Вычислить угол из вектора направления
float AICharacter::Angle(double vx, double vz, float defAy)
{
	//Вычисляем угол
	double l = vx*vx + vz*vz;
	if(l <= 0.0) return defAy;
	vz = acos(vz/sqrt(l));
	if(vx < 0) vz = -vz;
	return float(vz);
}

const char * AICharacter::GetCommandName(AICommand cmd)
{
	switch(cmd)
	{
	case aicmd_none:
		return "none";
	case aicmd_stay:
		return "stay";
	case aicmd_gotopoint:
		return "gotopoint";
	case aicmd_escape:
		return "escape";
	}
	return "unknow";
}

