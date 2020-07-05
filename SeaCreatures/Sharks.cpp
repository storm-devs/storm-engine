//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Sharks
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "Sharks.h"
#include "..\common_h\messages.h"
#include "..\common_h\model.h"
#include "..\Common_h\geometry.h"


//============================================================================================
//Fish
//============================================================================================

#define SHARK_PI				3.14159265f
#define SHARK_REPULSION				80.0f	//Коэфициент расталкивыния акул
#define SHARK_REPULSION_DIST		90.0f	//Дистанция расталкивыния акул
#define SHARK_MAX_RSPEED			8.1f	//Максимальная скорость перемещения точки следования
#define SHARK_PULL_DIST				100.0f	//Дистанция притягивания
#define SHARK_REPPUL_DIST			10.0f	//Дистанция отталкивания
#define SHARK_MAX_Y					0.01f	//Верхний потолок плаванья
#define SHARK_MIN_Y					-20.0f	//Нижний потолок плаванья
#define SHARK_MAX_TURN				0.2f	//Максимальная скорость поворота
#define SHARK_KROW_TURN				2.0f	//Коэфициент наклона при повороте max = turn*SHARK_MAX_TURN
#define SHARK_IMSPD_ACC				0.1f	//Ускорение погружения
#define SHARK_MAX_IMSPD				1.0f	//Максимальная скорость погружения
#define SHARK_KAX_IMSPD				0.3f	//Зависимость угла от скорости погружения
#define SHARK_MAX_SPEED				8.0f	//Максимальная скорость акулы
#define SHARK_MIN_SPEED				5.0f	//Минимальная скорость акулы


//============================================================================================
//Shark
//============================================================================================

word Sharks::Shark::indeces[] = {
	0, 1, 2,
	0, 2, 3,
	1, 4, 2,
	2, 4, 5,
	2, 5, 6,
	2, 6, 3,
	4, 7, 8,
	4, 8, 5,
	5, 8, 6,
	6, 8, 9,
};


Sharks::Shark::Shark()
{
	pos = 0.0f;
	vel = 0.0f;
	force = 0.0f;
	yDir = 0.0f;
	dirTime = 0.0f;
	turn = 0.0f;
	imspd = 0.0f;
	speed = (SHARK_MAX_SPEED + SHARK_MIN_SPEED)*0.5f;
	aniTime = 0.0f;
	jumpTime = 10.0f + rand()*30.0f/RAND_MAX;
	accs = 0.007f + rand()*0.008f/RAND_MAX;
	rForce = 0.0f;
	rTime = 0.0f;
	shipY = 0.0f;
	speedUp = false;
}


Sharks::Shark::~Shark()
{
	_CORE_API->DeleteEntity(model);
}

bool Sharks::Shark::Init(float vp_x, float vp_z, bool isLoadModel)
{
	//Позиция
	float radius = SHARK_PULL_DIST*(0.4f + rand()*1.6f/RAND_MAX);
	float ang = SHARK_PI*rand()*(2.0f/RAND_MAX);
	pos.x = vp_x + radius*cosf(ang);
	pos.z = vp_z + radius*sinf(ang);
	pos.y = SHARK_MIN_Y + (SHARK_MAX_Y - SHARK_MIN_Y)*rand()*1.0f/RAND_MAX;
	spos = pos;
	angs.y = SHARK_PI*rand()*(2.0f/RAND_MAX);
	if(!isLoadModel) return true;
	//Загружаем модельку	
	if(!_CORE_API->CreateEntity(&model, "modelr")) return false;
	//Путь для текстур
	VGEOMETRY * gs = (VGEOMETRY *)_CORE_API->CreateService("geometry");
	if(!gs)
	{
		_CORE_API->Trace("Can't create geometry service!");
		return false;
	}
	gs->SetTexturePath("Animals\\");
	if(!_CORE_API->Send_Message(model, "ls", MSG_MODEL_LOAD_GEO, "Animals\\shark"))
	{
		gs->SetTexturePath("");
		_CORE_API->Trace("Shark model 'shark' not loaded");
		_CORE_API->DeleteEntity(model);
		return false;
	}
	gs->SetTexturePath("");
	if(!_CORE_API->Send_Message(model, "ls", MSG_MODEL_LOAD_ANI, "shark"))
	{
		_CORE_API->Trace("Shark animation 'shark' not loaded");
		_CORE_API->DeleteEntity(model);
		return false;
	}
	//Ставим анимацию по умолчанию
	MODEL * mdl = (MODEL *)_CORE_API->GetEntityPointer(&model);
	if(!mdl || !mdl->GetAnimation()) return false;
	mdl->GetAnimation()->SetEvent(ae_end, 0, this);
	mdl->GetAnimation()->Player(0).SetAction("stand");
	mdl->GetAnimation()->Player(0).Play();
	NODE * node = mdl->GetNode(0);
	if(!node) return false;
	node->SetTechnique("Shark");
	return true;
}

inline void Sharks::Shark::Reset(float cam_x, float cam_z)
{
	force = 0.0f;
	fforce = 0.0f;
	//Если дальше от камеры чем можно, то телепортируемся
	float dx = cam_x - pos.x;
	float dz = cam_z - pos.z;
	const float max = SHARK_PULL_DIST*4.0f;
	if(dx*dx + dz*dz > max*max)
	{
		Init(cam_x, cam_z, false);
		pos.y = SHARK_MIN_Y*4.0f;
	}
	shipY = SHARK_MAX_Y;
}

inline void Sharks::Shark::Repulsion(Shark & shr)
{
	CVECTOR v = pos - shr.pos;
	float k = ~v;
	if(k > 0.0f && k < SHARK_REPULSION_DIST*SHARK_REPULSION_DIST)
	{
		v *= SHARK_REPULSION/k;
		force += v;
		shr.force -= v;
	}
	v = spos - shr.spos;
	k = ~v;
	if(k > 0.0f && k < 15.0f*15.0f)
	{
		v *= 5.0f/k;
		fforce += v;
		shr.fforce -= v;
	}
}

inline void Sharks::Shark::ShipApply(float x, float z, float r2)
{
	float dx = x - spos.x;
	float dz = z - spos.z;
	float d = dx*dx + dz*dz;
	float fy = 0.0f;
	if(d < r2)
	{
		float k = sqrtf(d/r2);
		fy = -(1.0f - k)*100.0f;
		shipY = SHARK_MIN_Y + (SHARK_MAX_Y - SHARK_MIN_Y)*k;
	}
	if(dx*vel.x + dz*vel.z < 0.0f) fy *= 0.5f;
	force.y += fy;
	fforce.y += fy;
}

inline void Sharks::Shark::Coordination(float cam_x, float cam_z, float dltTime, SEA_BASE * sb, ISLAND_BASE * ib)
{
	//Получим модельку
	MODEL * mdl = (MODEL *)_CORE_API->GetEntityPointer(&model);
	if(!mdl) return;
	//Сила расталкивания
	float l = ~force;
	if(l > 1600.0f) force *= 40.0f/sqrtf(l);
	//Случайная сила
	rTime -= dltTime;
	if(rTime < 0.0f)
	{
		float radius = rand()*10.0f/RAND_MAX;
		float ang = SHARK_PI*rand()*(2.0f/RAND_MAX);
		rForce.x = radius*sinf(ang);
		rForce.z = radius*cosf(ang);
		rTime = 1.0f + rand()*10.0f;
	}
	force += rForce;
	//Направление на камеру
	float vx = cam_x - pos.x;
	float vz = cam_z - pos.z;
	float k = vx*vx + vz*vz;
	//Если ближе чем, то разгоняем точку
	if(k < SHARK_REPPUL_DIST*SHARK_REPPUL_DIST)
	{
		force.x -= vx*1.0f/SHARK_REPPUL_DIST;
		force.z -= vz*1.0f/SHARK_REPPUL_DIST;
	}else{
		//Притягивание к камере
		vx *= 0.1f; vz *= 0.1f;
		l = sqrtf(k)*0.1f;
		if(l > 20.0f*20.0f){ l = 20.0f/l; vx *= l; vz *= l; }
		force.x += vx;
		force.z += vz;
	}	
	//Перемещение по высоте
	force.y += 0.4f*(yDir - pos.y);
	//Шум
	force.x += (0.5f - (rand() & 15)*1.0f/15.0f)*0.001f;
	force.y += (0.5f - (rand() & 15)*1.0f/15.0f)*0.001f;
	force.z += (0.5f - (rand() & 15)*1.0f/15.0f)*0.001f;
	//Скорость перемещения
	vel += accs*force*dltTime;
	l = ~vel;
	if(l > SHARK_MAX_RSPEED*SHARK_MAX_RSPEED) vel *= SHARK_MAX_RSPEED/sqrtf(l);	
	//Позиция
	pos += vel*dltTime;
	if(pos.y > 0.1f) pos.y = 0.1f;
	if(pos.y < -20.0f) pos.y = -20.0f;
	//Тенденция к высоте плаванья
	dirTime -= dltTime;
	if(dirTime <= 0.0f)
	{		
		float p = rand()*1.0f/RAND_MAX;
		yDir = SHARK_MIN_Y + (SHARK_MAX_Y - SHARK_MIN_Y)*(1.0f - p*p);
		dirTime = 5.0f + 10.0f*rand()*1.0f/RAND_MAX;
	}
	//Рыба
	//Сила притяжения
	CVECTOR fff = pos - spos;
	l = ~fff;
	if(l > 1.0f) fff *= 1.0f/sqrtf(l);
	//Сила расталкивания
	l = ~fforce;
	if(l > 100.0f) fforce *= 10.0f/sqrtf(l);
	fforce += fff;
	//Определим коллижен относительно острова
	if(ib)
	{
		IslandCollision(ib, 32, 5.0f, 100.0f);
		IslandCollision(ib, 16, 30.0f, 50.0f);
		IslandCollision(ib, 8, 50.0f, 20.0f);
	}
	//Определим направление рыбы относительно точки следования
	float sx = sinf(angs.y);
	float sz = cosf(angs.y);
	vx = fforce.x;
	vz = fforce.z;
	l = vx*vx + vz*vz;
	if(l > 0.0f)
	{
		l = 1.0f/sqrtf(l);
		vx *= l; vz *= l;
	}else{
		vx = sx; vz = sz;
	}
	float sn = sx*vz - sz*vx;
	float cs = sx*vx + sz*vz;
	turn -= 0.4f*sn*dltTime;
	if(turn > SHARK_MAX_TURN) turn = SHARK_MAX_TURN;
	if(turn < -SHARK_MAX_TURN) turn = -SHARK_MAX_TURN;
	angs.y += turn*dltTime;
	//Угол наклона при повороте
	angs.z = -turn*SHARK_KROW_TURN;
	//Скорость
	speed += 0.3f*cs*dltTime;
	speedUp = cs > 0.0f;
	if(speed > SHARK_MAX_SPEED) speed = SHARK_MAX_SPEED;
	if(speed < SHARK_MIN_SPEED) speed = SHARK_MIN_SPEED;
	//Изменение глубины
	if(fforce.y < 0.0f)
	{
		if(spos.y > SHARK_MIN_Y)
		{
			imspd -= SHARK_IMSPD_ACC*dltTime;
			if(imspd < -SHARK_MAX_IMSPD) imspd = -SHARK_MAX_IMSPD;
		}else{
			imspd += SHARK_IMSPD_ACC*dltTime;
		}
	}else{
		imspd += SHARK_IMSPD_ACC*dltTime;
		if(imspd > SHARK_MAX_IMSPD) imspd = SHARK_MAX_IMSPD;
	}
	//Угол наклона при погружении
	angs.x = -imspd*SHARK_KAX_IMSPD;
	//Новая позиция
	Assert(speed >= SHARK_MIN_SPEED);
	spos.x += speed*sinf(angs.y)*dltTime;
	spos.y += imspd*dltTime;
	spos.z += speed*cosf(angs.y)*dltTime;
	CVECTOR rpos = spos;
	if(sb)
	{
		CVECTOR n;
		float seaY = sb->WaveXZ(spos.x, spos.z, &n);
		if(rpos.y > seaY) rpos.y = seaY;
	}
	if(rpos.y > shipY) rpos.y = shipY;
	//Ограничение наклона от глубины
	if(rpos.y > -3.0f)
	{
		l = rpos.y;
		if(l > 0.0f) l = 0.0f;
		l = 1.0f - (3.0f + l)/3.0f;
		angs.x *= l*l;
	}
	//Анимация
	aniTime -= dltTime;
	jumpTime -= dltTime;
	//Трек
	vBase += 0.2f*speed*dltTime;
	if(vBase > 1.0f) vBase -= 1.0f;
	//Обновление матрицы
	mdl->mtx.BuildMatrix(angs, rpos);
	/*
	if(GetAsyncKeyState('Z') >= 0)
	{
		if(GetAsyncKeyState('X') < 0) rpos.y += 30.0f;
		mdl->mtx.BuildMatrix(angs, rpos);
	}else{
		mdl->mtx.BuildMatrix(angs, pos + CVECTOR(0.0f, 30.0f, 0.0f));
	}
	//*/
}

inline void Sharks::Shark::IslandCollision(ISLAND_BASE * ib, long numPnt, float rad, float frc)
{
	float step = 2.0f*SHARK_PI/numPnt;
	float vx = 0.0f;
	float vz = 0.0f;
	MODEL * mdl = (MODEL *)api->GetEntityPointer(&ib->GetSeabedEID());
	if(!mdl) return;
	for(long i = 0; i < numPnt; i++)
	{
		float x = sinf(i*step);
		float z = cosf(i*step);
		float xp = spos.x + rad*x;
		float zp = spos.z + rad*z;
		float h = mdl->Trace(CVECTOR(xp, 100.0f, zp), CVECTOR(xp, -50.0f, zp));
		if(h < 1.0f)
		{
			h -= 100.0f/150.0f;
			if(h > 0.0f)
			{
				h *= 150.0f/50.0f;
			}else h = 0.0f;
			h = 0.0f;
			vx -= x*(1.0f - h);
			vz -= z*(1.0f - h);
		}
	}
	rad = vx*vx + vz*vz;
	if(rad > 1.0f)
	{
		rad = 1.0f/sqrtf(rad);
		vx *= rad;
		vz *= rad;
	}
	fforce.x += vx*frc;
	fforce.z += vz*frc;
}

void Sharks::Shark::Event(Animation * animation, long index, long eventID, AnimationEvent event)
{
	if(aniTime > 0.0f) return;
	static const char * actStand = "stand";
	static const char * actSwim = "Shark_Swim";
	static const char * actJump = "Shark_Jump";
	const char * act = animation->Player(0).GetAction();
	long rnd = rand();
	animation->Player(0).Stop();
	if(angs.x > 0.0f && (rnd & 1) || speedUp)
	{
		animation->Player(0).SetAction(actSwim);
		aniTime = 3.0f + rand()*3.0f/RAND_MAX;
	}
	if(stricmp(act, actSwim) == 0 || (rnd & 1))
	{
		animation->Player(0).SetAction(actStand);
		aniTime = 3.0f + rand()*3.0f/RAND_MAX;
	}else{
		if(jumpTime <= 0.0f && spos.y > -0.1f && ~(rnd & 0xf0))
		{
			//Выпрыгиваем из воды
			animation->Player(0).SetAction(actJump);
			jumpTime = 60.0f + rand()*100.0f/RAND_MAX;
			aniTime = -0.1f;
		}else{
			animation->Player(0).SetAction(actSwim);
			aniTime = 2.0f + rand()*3.0f/RAND_MAX;
		}
	}
	animation->Player(0).Play();
}

long Sharks::Shark::GenerateTrack(word * inds, Vertex * vrt, word base, SEA_BASE * sb)
{
	//Получим модельку
	MODEL * mdl = (MODEL *)_CORE_API->GetEntityPointer(&model);
	if(!mdl) return 0;
	float k = mdl->mtx.Pos().y;
	if(k <= -1.2f) return 0;
	if(k > 0.0f) k = 0.0f;
	k = (1.2f + k)/1.2f;
	//Параметры плавника
	float length = 2.0f*3.0f*k;
	float width = 1.0f*1.5f*k;
	//Индексы
	Assert(sizeof(indeces)/sizeof(word) == 30);
	for(long i = 0; i < 30; i++) inds[i] = indeces[i] + base;
	//Вершины
	CVECTOR s(0.0f, 0.0f, 0.75f);
	vrt[0].pos = s;
	vrt[7].pos = s + CVECTOR(-width, 0.0f, -length);
	vrt[9].pos = s + CVECTOR(width, 0.0f, -length);
	vrt[8].pos = (vrt[7].pos + vrt[9].pos)*0.5f;
	vrt[1].pos = s + (vrt[7].pos - s)*0.333f;
	vrt[2].pos = s + (vrt[8].pos - s)*0.333f;
	vrt[3].pos = s + (vrt[9].pos - s)*0.333f;
	vrt[4].pos = s + (vrt[7].pos - s)*0.666f;
	vrt[5].pos = s + (vrt[8].pos - s)*0.666f;
	vrt[6].pos = s + (vrt[9].pos - s)*0.666f;
	vrt[0].u = 0.5f; vrt[0].v = 0.0f;
	vrt[1].u = 0.0f; vrt[1].v = 0.333f;
	vrt[2].u = 0.5f; vrt[2].v = 0.333f;
	vrt[3].u = 1.0f; vrt[3].v = 0.333f;
	vrt[4].u = 0.0f; vrt[4].v = 0.666f;
	vrt[5].u = 0.5f; vrt[5].v = 0.666f;
	vrt[6].u = 1.0f; vrt[6].v = 0.666f;
	vrt[7].u = 0.0f; vrt[7].v = 1.0f;
	vrt[8].u = 0.5f; vrt[8].v = 1.0f;
	vrt[9].u = 1.0f; vrt[9].v = 1.0f;	
	for(i = 0; i < 10; i++)
	{
		vrt[i].pos = mdl->mtx*CVECTOR(vrt[i].pos);
		vrt[i].pos.y = sb->WaveXZ(vrt[i].pos.x, vrt[i].pos.z) + 0.001f;
		vrt[i].color = 0x4fffffff;
		vrt[i].v -= vBase;
	}
	vrt[7].color = 0;
	vrt[8].color = 0;
	vrt[9].color = 0;
	return 10;
}

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

Sharks::Sharks()
{
	rs = null;
	camPos = 0.0f;
	shipcode = _CORE_API->Class_Name2Code("ship");
	numShakes = 3 + (GetTickCount() & 3);
	trackTx = -1;
	periscope.time = -1.0;
	waitPTime = -1.0f;
}

Sharks::~Sharks()
{
	_CORE_API->DeleteEntity(periscope.model);
	if(rs) rs->TextureRelease(trackTx);
}

//Инициализация
bool Sharks::Init()
{
	rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!rs) _THROW("No service: dx8render");
	for(long i = 0; i < numShakes; i++)
		if(!shark[i].Init(0.0f, 0.0f)) return false;
	//Лаера исполнения
	char execute[64];
	char realize[64];	
	const char * attr = AttributesPointer->GetAttribute("execute");
	if(attr && attr[0]) strcpy(execute, attr); else strcpy(execute, "execute");
	attr = AttributesPointer->GetAttribute("realize");
	if(attr && attr[0]) strcpy(realize, attr); else strcpy(realize, "realize");
	//Уровни исполнения
	long emdl = AttributesPointer->GetAttributeAsDword("executeModels", 77);
	long rmdl = AttributesPointer->GetAttributeAsDword("realizeModels", 77);
	long eprt = AttributesPointer->GetAttributeAsDword("executeParticles", 77);
	long rprt = AttributesPointer->GetAttributeAsDword("realizeParticles", 100000);
	//Установим уровни исполнения
	_CORE_API->LayerAdd(execute, GetID(), eprt);
	_CORE_API->LayerAdd(realize, GetID(), rprt);
	for(i = 0; i < numShakes; i++)
	{
		_CORE_API->LayerAdd(execute, shark[i].model, emdl);
		_CORE_API->LayerAdd(realize, shark[i].model, rmdl);		
	}
	//Загрузим текстуру
	trackTx = rs->TextureCreate("Animals\\SharkTrack.tga");
	//Анализируем возможность создания перископа
	VDATA * v = (VDATA *)api->GetScriptVariable("Environment");
	if(v)
	{
		ATTRIBUTES * root = v->GetAClass();
		if(root)
		{
			float time = root->GetAttributeAsFloat("time");
			if(time > 9.0f && time < 20.0f)
			{
				root = root->FindAClass(root, "date");
				if(root)
				{
					float year = root->GetAttributeAsFloat("year");
					if(year >= 1633.0f)
					{
						dword month = root->GetAttributeAsDword("month");
						if(month & 1)
						{
							dword day = root->GetAttributeAsDword("day");
							if(day == 7)
							{
								if((GetTickCount() & 7) == 5)
								{
									waitPTime = 60.0f + rand()*500.0f/RAND_MAX;
								}
							}
						}
					}
				}
			}
		}
	}
	return true;
}

//Исполнение
void Sharks::Execute(dword delta_time)
{
	CVECTOR a;
	if(delta_time & 1) rand();
	rs->GetCamera(camPos, a, a.x);
	float dltTime = delta_time*0.001f;
	const long num = numShakes;
	//Сбросим состояния
	for(long i = 0; i < num; i++) shark[i].Reset(camPos.x, camPos.z);
	//Разчитаем силы
	for(i = 0; i < num - 1; i++)
		for(long j = i + 1; j < num; j++) shark[i].Repulsion(shark[j]);
	//Учитываем корабли
	ENTITY_ID id;
	bool res = _CORE_API->FindClass(&id, null, shipcode);
	for(; res; res = _CORE_API->FindClassNext(&id))
	{
		//Указатель на объект
		VAI_OBJBASE * ship = (VAI_OBJBASE *)_CORE_API->GetEntityPointer(&id);
		if(!ship) break;
		//Позиция корабля
		CVECTOR shipPos = ship->GetMatrix()->Pos();
		//Размер корабля
		CVECTOR s = ship->GetBoxSize();
		float rd2 = (s.x*s.x + s.z*s.z)*3.0f;
		//Говорим акулам о короблях
		for(i = 0; i < num; i++) shark[i].ShipApply(shipPos.x, shipPos.z, rd2);
	}
	//Море
	SEA_BASE * sb = (SEA_BASE *)_CORE_API->GetEntityPointer(&sea);
	if(!sb)
	{
		_CORE_API->FindClass(&sea, "sea", 0);
		if(!sb) return;
	}
	ISLAND_BASE * ib = (ISLAND_BASE *)_CORE_API->GetEntityPointer(&island);
	if(!ib)
	{
		_CORE_API->FindClass(&island, "island", 0);
	}
	//Расчитываем новые позиции
	for(i = 0; i < num; i++) shark[i].Coordination(camPos.x, camPos.z, dltTime, sb, ib);
	//Обрабатываем перископ
	if(!ib)
	{
		if(periscope.time >= 0.0f)
		{
			periscope.time -= dltTime;
			if(periscope.time < 10.0f)
			{
				periscope.pos.y -= dltTime;
			}else{
				periscope.pos.y += 2.0f*dltTime;
				if(periscope.pos.y > 0.0f) periscope.pos.y = 0.0f;
			}
			MODEL * mdl = (MODEL *)_CORE_API->GetEntityPointer(&periscope.model);
			if(mdl)
			{
				mdl->mtx.BuildMatrix(CVECTOR(0.0f, periscope.ay, 0.0f), periscope.pos + CVECTOR(0.0f, 1.0f, 0.0f));
				periscope.pos.x += mdl->mtx.Vz().x*dltTime*5.0f;
				periscope.pos.z += mdl->mtx.Vz().z*dltTime*5.0f;
			}
			if(periscope.time < 0.0f)
			{
				periscope.time = -1.0f;
				api->DeleteEntity(periscope.model);
			}
		}else{			
			if(waitPTime > 0.0f)
			{
				waitPTime -= dltTime;
				if(waitPTime <= 0.0f)
				{
					periscope.pos = 0.0f;
					periscope.ay = rand()*(6.14f/RAND_MAX);
					if(LoadPeriscopeModel())
					{
						periscope.time = 30.0f + rand()*(20.0f/RAND_MAX);
						rs->GetCamera(periscope.pos, CVECTOR(0.0f), periscope.pos.y);
					}
					periscope.pos.y = -10.0f;
					waitPTime = -1.0f;
				}
			}
		}
	}
}

bool Sharks::LoadPeriscopeModel()
{
	if(!api->CreateEntity(&periscope.model, "modelr")) return false;
	VGEOMETRY * gs = (VGEOMETRY *)_CORE_API->CreateService("geometry");
	if(!gs) return false;
	gs->SetTexturePath("Animals\\");
	if(!_CORE_API->Send_Message(periscope.model, "ls", MSG_MODEL_LOAD_GEO, "Animals\\periscope"))
	{
		gs->SetTexturePath("");
		_CORE_API->DeleteEntity(periscope.model);
		return false;
	}
	gs->SetTexturePath("");
	MODEL * mdl = (MODEL *)_CORE_API->GetEntityPointer(&periscope.model);
	if(!mdl)
	{
		_CORE_API->DeleteEntity(periscope.model);
		return false;
	}
	_CORE_API->LayerAdd("sea_realize", periscope.model, 10);
	return true;
}

void Sharks::Realize(dword delta_time)
{
	SEA_BASE * sb = (SEA_BASE *)_CORE_API->GetEntityPointer(&sea);
	if(!sb) return;
	long num = 0;
	for(long i = 0; i < numShakes; i++)
	{
		num += shark[i].GenerateTrack(indeces + num*3, vrt + num, word(num), sb);
	}
	if(num)
	{
		rs->TextureSet(0, trackTx);
		rs->SetTransform(D3DTS_WORLD, CMatrix());
		rs->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		rs->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, num, num, indeces, D3DFMT_INDEX16, vrt, sizeof(Vertex), "SharkTrack");
	}

	/*
	ISLAND_BASE * ib = (ISLAND_BASE *)_CORE_API->GetEntityPointer(&island);
	if(!ib) return;
	float maxRad = 0.0f;
	long s = 30;
	for(long i = 0; i < numShakes; i++)
	{
		float r = sqrtf(~(shark[i].spos - shark[i].pos));
		if(r > maxRad) maxRad = r;
		if(ib)
		{
			float h = -1000.0f;
			ib->GetDepth(shark[i].spos.x, shark[i].spos.z, &h);
			rs->Print(10, s, "Height = %f", h);
			s += 20;			
		}
	}
	rs->Print(10, 10, "MaxRad = %f", maxRad);
	*/
}


