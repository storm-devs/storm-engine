#include "blast.h"
#include "..\common_h\messages.h"

#define ANGLESPEED_MUL	0.2f

BLAST::BLAST()
{
	rs = 0;
	gs = 0;
	Item = 0;
	AngleDeviation = 1.57f;
	ItemsNum = 0;
	pSea = 0;
}

BLAST::~BLAST()
{
	dword n;
	if(Item)
	{
		for(n=0;n<ItemsNum;n++)
		{
			if(!Item[n].bDouble) if(Item[n].geo) delete Item[n].geo;
		}
		ItemsNum = 0;
		delete Item; 
		Item = 0;
	}
}

bool BLAST::Init() 
{	
	gs = (VGEOMETRY *)_CORE_API->CreateService("geometry");
	if(!gs) return false;
	rs = (VDX8RENDER*)_CORE_API->CreateService("dx8render");
	if(!rs) return false;

	long RandomNum;
//	long n;
	INIFILE * ini;
	ini = _CORE_API->fio->OpenIniFile("resource\\ini\\particles\\particles.ini");
	if(!ini) {_CORE_API->Trace("not found: resource\\ini\\particles\\particles.ini"); return false;}

	RandomNum = ini->GetLong("geo","randomnum",0);

	char name[MAX_PATH];
	if(ini->ReadString("geo","file",name,sizeof(name),"")) 
	{
		AddGeometry(name,RandomNum*rand()/RAND_MAX + 1);
	}
	while(ini->ReadStringNext("geo","file",name,sizeof(name)))
	{
		AddGeometry(name,RandomNum*rand()/RAND_MAX + 1);
	}

	delete ini;

	api->FindClass(&Splash,"BallSplash",0);

	return true;
}

void BLAST::AddGeometry(char * name, long num)
{
	long n;
	GEOS * gp;
	//n = ItemsNum;
	//ItemsNum++;
	//Item = (GEOPARTICLE*)RESIZE(Item,sizeof(GEOPARTICLE)*ItemsNum);
	//Item[n].geo = 0;
	//Item[n].geo = gs->CreateGeometry(name,0,0);
	
	Item = (GEOPARTICLE*)RESIZE(Item,sizeof(GEOPARTICLE)*(ItemsNum+num));
	gp = gs->CreateGeometry(name,0,0);
	for(n=0;n<num;n++)
	{
		if(n == 0) Item[n + ItemsNum].bDouble = false;
		else Item[n + ItemsNum].bDouble = true;
		Item[n + ItemsNum].geo = gp;
	}
	ItemsNum += num;
}

void BLAST::SetBlastCenter(CVECTOR pos, CVECTOR ang)
{
	dword n;
	CMatrix m;
	if(Item)
	{
		for(n=0;n<ItemsNum;n++)
		{
			Item[n].bEffect = false;
			Item[n].pos = pos;
			Item[n].ang = ang;
			m.BuildMatrix(ang);
			Item[n].dir = m.Vz();


			Item[n].ang.x += AngleDeviation*rand()/RAND_MAX - AngleDeviation/2.0f;
			Item[n].ang.y += AngleDeviation*rand()/RAND_MAX - AngleDeviation/2.0f;
			Item[n].ang.z += AngleDeviation*rand()/RAND_MAX - AngleDeviation/2.0f;

			Item[n].ang_speed.x = ANGLESPEED_MUL*(AngleDeviation*rand()/RAND_MAX - AngleDeviation/2.0f);
			Item[n].ang_speed.y = ANGLESPEED_MUL*(AngleDeviation*rand()/RAND_MAX - AngleDeviation/2.0f);
			Item[n].ang_speed.z = ANGLESPEED_MUL*(AngleDeviation*rand()/RAND_MAX - AngleDeviation/2.0f);

			Item[n].dir.x += AngleDeviation*rand()/RAND_MAX - AngleDeviation/2.0f;
			Item[n].dir.y += AngleDeviation*rand()/RAND_MAX - AngleDeviation/2.0f;
			Item[n].dir.z += AngleDeviation*rand()/RAND_MAX - AngleDeviation/2.0f;
			Item[n].dir = !Item[n].dir;
			Item[n].speed = rand()*0.025f/RAND_MAX + 0.005;

		}
	}
}

void BLAST::ProcessTime(dword DT)
{
	dword n;
	float a;
	bool bStop;
	float Delta_Time;
	float res;
	
	
	if(!api->ValidateEntity(&sea_eid))
	{
		api->FindClass(&sea_eid,"sea",0);
		pSea = (CANNON_TRACE_BASE *)api->GetEntityPointer(&sea_eid);
	}

	Delta_Time = DT;//*0.1;
	bStop = true;
	if(Item)
	{
		for(n=0;n<ItemsNum;n++)
		{
			//if(Item[n].speed > 0) Item[n].speed -= 0.001f*Delta_Time;
			//else Item[n].speed = 0.0;
			//Item[n].pos += Item[n].speed*Item[n].dir*Delta_Time;
			CVECTOR a;
			a = Item[n].pos;
			Item[n].pos.x += (Item[n].dir.x*Item[n].speed*Delta_Time);
			Item[n].pos.y += (Item[n].dir.y*Item[n].speed*Delta_Time);
			Item[n].pos.z += (Item[n].dir.z*Item[n].speed*Delta_Time);

			

			//Item[n].pos.y -= Delta_Time*0.0005;
			Item[n].dir.y -= Delta_Time*0.0008;
			Item[n].pos.y += Item[n].dir.y*0.01*Delta_Time;
			

			Item[n].ang.x += Item[n].ang_speed.x*Delta_Time*0.05;
			Item[n].ang.y += Item[n].ang_speed.y*Delta_Time*0.05;
			Item[n].ang.z += Item[n].ang_speed.z*Delta_Time*0.05;

			if(Item[n].pos.y > 0) bStop = false;

			if(!Item[n].bEffect)
			{
				if(pSea)
				{
					res = pSea->Cannon_Trace(-1,a,Item[n].pos);
					if(res <= 1) Item[n].bEffect = true;
				} 
				else
				{
					if(Item[n].pos.y < 0)
					{
						Item[n].bEffect = true;
						//api->Send_Message(Splash,"lfff",MSG_BALLSPLASH_ADD,Item[n].pos.x,Item[n].pos.y,Item[n].pos.z);
					}
				}
			}

			/*
			if(Item[n].pos.y < 0 && !Item[n].bEffect)
			{
				Item[n].bEffect = true;
				//api->Send_Message(Splash,"lfff",MSG_BALLSPLASH_ADD,Item[n].pos.x,Item[n].pos.y,Item[n].pos.z);

			}*/
		}
	}
	if(bStop) _CORE_API->DeleteEntity(GetID());
}

dword _cdecl BLAST::ProcessMessage(MESSAGE & message)
{
	long code;
	CVECTOR ang;
	CVECTOR pos;
	code = message.Long();
	switch(code)
	{
		case LM_SETPOINT:
			ang.x = ang.y = ang.z = 0;
			ang.x = -1.57f;
			pos.x = message.Float();
			pos.y = message.Float();
			pos.z = message.Float();
			SetBlastCenter(pos,ang);
		break;
		case LM_SETPOINTANDANGLES:
			pos.x = message.Float();
			pos.y = message.Float();
			pos.z = message.Float();
			ang.x = message.Float();
			ang.y = message.Float();
			ang.z = message.Float();
			SetBlastCenter(pos,ang);
		break;
	}
	return 0;
}

dword BLAST::AttributeChanged(ATTRIBUTES * pA)
{

	return 0;
}

void BLAST::Realize(dword Delta_Time)
{
	dword n;
	
	
	if(!Item) return;

	ProcessTime(Delta_Time);
	
	gs->SetTechnique("");
	for(n=0;n<ItemsNum;n++)
	{
		if(Item[n].geo)	
		{
			Center.BuildMatrix(Item[n].ang,Item[n].pos);
			rs->SetTransform(D3DTS_WORLD, Center);
			Item[n].geo->Draw((GEOS::PLANE *)rs->GetPlanes(), 0, null);
		}
	}
}