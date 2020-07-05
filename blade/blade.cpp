/******************************************************************************
File:  blade.cpp

Author:   Nick Chirkov

Comments:
model binded to an animated locator
******************************************************************************/
#include "blade.h"
#include "..\common_h\model.h"
#include "..\common_h\geometry.h"
static const char *handName = "Saber_hand";
static const char *beltName = "Saber_belt";
static const char *bloodName = "Saber_blood";
static const char *lightName = "Saber_light";
static const char *bladeStart = "start";
static const char *bladeEnd = "end";

static const char *gunHandName = "gun_hand";
static const char *gunBeltName = "gun_belt";
static const char *gunFire = "gun_fire";

static const char *sabergunHandName = "sabergun_hand";
static const char *sabergunBeltName = "sabergun_belt";
static const char *sabergunFire = "sabersabgun_fire";

INTERFACE_FUNCTION
CREATE_CLASS(BLADE)

BLADE::BLADE_INFO::BLADE_INFO()
{
	locatorName = beltName;
	defLifeTime = 0.15f;
	lifeTime = 0.0f;
	color[0] = 0x80162EBE;
	color[1] = 0x00FF0000;
	time = 0.0f;
	for(long v=0; v<WAY_LENGTH; v++)	vrtTime[v] = -1e20f;
}

BLADE::BLADE_INFO::~BLADE_INFO()
{
	api->DeleteEntity(eid);
}

void BLADE::BLADE_INFO::DrawBlade(VDX8RENDER * rs,unsigned int blendValue,MODEL *mdl,NODE *manNode)
{
	MODEL *obj = (MODEL*)api->GetEntityPointer(&eid);
	if(obj!=0)
	{
		CMatrix perMtx;

		NODE *bladeNode = obj->GetNode(0);
		if((blendValue & 0xff000000) == 0xff000000)
		{
			bladeNode->SetTechnique("EnvAmmoShader");
		}else{
			bladeNode->SetTechnique("AnimationBlend");
		}
		long sti = -1;
		long idBlade;
		idBlade = manNode->geo->FindName(locatorName);

		if((sti = manNode->geo->FindLabelN(sti+1, idBlade))>-1)
		{
			Animation *ani = mdl->GetAnimation();
			CMatrix *bones = &ani->GetAnimationMatrix(0);

			GEOS::LABEL lb;
			manNode->geo->GetLabel(sti, lb);
			CMatrix mt;
			mt.Vx() = CVECTOR(lb.m[0][0], lb.m[0][1], lb.m[0][2]);
			mt.Vy() = CVECTOR(lb.m[1][0], lb.m[1][1], lb.m[1][2]);
			mt.Vz() = CVECTOR(lb.m[2][0], lb.m[2][1], lb.m[2][2]);
			mt.Pos() = CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);

			CMatrix mbn = mt*bones[lb.bones[0]];
			mbn.Pos().x *= -1.0f;
			mbn.Vx().x *= -1.0f;
			mbn.Vy().x *= -1.0f;
			mbn.Vz().x *= -1.0f;

			CMatrix scl;
			scl.Vx().x = -1.0f;
			scl.Vy().y = 1.0f;
			scl.Vz().z = 1.0f;
			mbn.EqMultiply(scl, CMatrix(mbn));

			perMtx = mbn*mdl->mtx;
		}
		obj->mtx = perMtx;
		obj->Realize(0);

		//--------------------------------------------------------------------------
		rs->SetTexture(0, 0);
		rs->SetTransform(D3DTS_WORLD, CMatrix());

		//move to the beginning
		long first = 0;	//end vertex 2 draw
		for(long v=WAY_LENGTH-2; v>-1; v--)
		{
			vrt[v*2+2] = vrt[v*2+0];
			vrt[v*2+3] = vrt[v*2+1];
			vrtTime[v+1] = vrtTime[v+0];

			if(vrtTime[v+1]+lifeTime<=time)	continue;

			first++;

			float blend = (time-vrtTime[v+1])/lifeTime;

			float fcol0 = float((color[0]>>24)&0xFF);
			float fcol1 = float((color[1]>>24)&0xFF);
			unsigned long a = (unsigned long)(fcol0 + blend*(fcol1-fcol0));

			fcol0 = float((color[0]>>16)&0xFF);
			fcol1 = float((color[1]>>16)&0xFF);
			unsigned long r = (unsigned long)(fcol0 + blend*(fcol1-fcol0));

			fcol0 = float((color[0]>>8)&0xFF);
			fcol1 = float((color[1]>>8)&0xFF);
			unsigned long g = (unsigned long)(fcol0 + blend*(fcol1-fcol0));

			fcol0 = float((color[0]>>0)&0xFF);
			fcol1 = float((color[1]>>0)&0xFF);
			unsigned long b = (unsigned long)(fcol0 + blend*(fcol1-fcol0));

			vrt[v*2+2].diffuse = vrt[v*2+3].diffuse = (a<<24) | (r<<16) | (g<<8) | b;
		}

		//search for start
		sti = bladeNode->geo->FindLabelN(0, bladeNode->geo->FindName(bladeStart));
		GEOS::LABEL lb;
		if(sti >= 0)
		{
			bladeNode->geo->GetLabel(sti, lb);
			vrt[0].pos = perMtx*CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);
			vrt[0].diffuse = color[0];
			sti = bladeNode->geo->FindLabelN(0, bladeNode->geo->FindName(bladeEnd));
			if(sti >= 0)
			{
				bladeNode->geo->GetLabel(sti, lb);
				vrt[1].pos = perMtx*CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);
				vrt[1].diffuse = color[0];
				vrtTime[0] = time;

				bool bDraw = rs->TechniqueExecuteStart("blade");
				if(bDraw)
				{
					if(first>0)	rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, FVF, first*2, &vrt[0], sizeof vrt[0]);
				}
				while (rs->TechniqueExecuteNext()) {};
			}else{
				api->Trace("BLADE::Realize -> no find locator \"%s\", model \"%s\"", bladeEnd, bladeNode->GetName());
			}
		}else{
			api->Trace("BLADE::Realize -> no find locator \"%s\", model \"%s\"", bladeStart, bladeNode->GetName());
		}
	}
}

bool BLADE::BLADE_INFO::LoadBladeModel(MESSAGE &message)
{
	api->DeleteEntity(eid);

	//Имя модельки
	char mdlName[200];
	message.String(sizeof(mdlName), mdlName);
	mdlName[sizeof(mdlName) - 1] = 0;
	if(strlen(mdlName) > 0)
	{
		//Путь до модельки
		char path[256];
		strcpy(path, "Ammo\\");
		strcat(path, mdlName);
		//Путь до текстур
		VGEOMETRY * gs = (VGEOMETRY *)_CORE_API->CreateService("geometry");
		if(gs) gs->SetTexturePath("Ammo\\");
		//Создаём модельку
		api->CreateEntity(&eid, "modelr");
		if(!api->Send_Message(eid, "ls", MSG_MODEL_LOAD_GEO, path))
		{
			api->DeleteEntity(eid);
			if(gs) gs->SetTexturePath("");
			return false;
		}
		if(gs) gs->SetTexturePath("");
		//Параметры следа
		defLifeTime = message.Float();
		color[0] = message.Long();
		color[1] = message.Long();
	}else return false;
	return true;
}

BLADE::BLADE()
{
	gunLocName = gunBeltName;
	blendValue = 0xFFFFFFFF;
}

BLADE::~BLADE()
{
	_CORE_API->DeleteEntity(gun);

	for( long i=0; i<ITEMS_INFO_QUANTITY; i++ ) items[i].Release();
}

bool BLADE::Init()
{
	GUARD(BLADE::BLADE())

	col = (COLLIDE *)_CORE_API->CreateService("coll");
	if(col==null)	_THROW("No service: COLLIDE");

	_CORE_API->LayerAdd("realize",GetID(),65550);

	rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!rs)	_THROW("No service: dx8render");
	
	UNGUARD
	return true;
}


//------------------------------------------------------------------------------------
//realize
//------------------------------------------------------------------------------------
void BLADE::Realize(dword Delta_Time)
{
	blade[0].time += 0.001f*(Delta_Time);

	MODEL *mdl = (MODEL*)_CORE_API->GetEntityPointer(&man);
	if(!mdl) return;

	NODE *manNode = mdl->GetNode(0);
	rs->TextureSet(0, -1);
	rs->TextureSet(1, -1);
	rs->TextureSet(2, -1);
	rs->TextureSet(3, -1);

	CMatrix mtx;
	rs->GetTransform(D3DTS_VIEW, mtx);
	mtx.Transposition();
	mtx.Pos() = 0.0f;
	rs->SetTransform(D3DTS_TEXTURE1, mtx);
	rs->SetRenderState(D3DRS_TEXTUREFACTOR, blendValue);


	//------------------------------------------------------
	//draw gun
	CMatrix perMtx;
	long sti;
	MODEL *obj = (MODEL*)_CORE_API->GetEntityPointer(&gun);
	if(obj!=0)
	{
		NODE *gunNode = obj->GetNode(0);
		if((blendValue & 0xff000000) == 0xff000000)
		{
			gunNode->SetTechnique("EnvAmmoShader");
		}else{
			gunNode->SetTechnique("AnimationBlend");
		}
		sti = -1;
		long idGun = manNode->geo->FindName(gunLocName);

		if((sti = manNode->geo->FindLabelN(sti+1, idGun))>-1)
		{
			Animation *ani = mdl->GetAnimation();
			CMatrix *bones = &ani->GetAnimationMatrix(0);

			GEOS::LABEL lb;
			manNode->geo->GetLabel(sti, lb);
			CMatrix mt;
			mt.Vx() = CVECTOR(lb.m[0][0], lb.m[0][1], lb.m[0][2]);
			mt.Vy() = CVECTOR(lb.m[1][0], lb.m[1][1], lb.m[1][2]);
			mt.Vz() = CVECTOR(lb.m[2][0], lb.m[2][1], lb.m[2][2]);
			mt.Pos() = CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);

			CMatrix mbn = mt*bones[lb.bones[0]];
			mbn.Pos().x *= -1.0f;
			mbn.Vx().x *= -1.0f;
			mbn.Vy().x *= -1.0f;
			mbn.Vz().x *= -1.0f;

			CMatrix scl;
			scl.Vx().x = -1.0f;
			scl.Vy().y = 1.0f;
			scl.Vz().z = 1.0f;
			mbn.EqMultiply(scl, CMatrix(mbn));

			perMtx = mbn*mdl->mtx;
		}
		obj->mtx = perMtx;

		obj->Realize(0);
	}

	//------------------------------------------------------
	//draw saber
	blade[0].DrawBlade( rs, blendValue, mdl, manNode );
	blade[1].DrawBlade( rs, blendValue, mdl, manNode );

	//------------------------------------------------------
	//draw tied items
	for(long n=0; n<ITEMS_INFO_QUANTITY; n++)
		if( items[n].nItemIndex!=-1 )
			items[n].DrawItem( rs, blendValue, mdl, manNode );

	mtx.SetIdentity();
	rs->SetTransform(D3DTS_TEXTURE1, mtx);
}

bool BLADE::LoadBladeModel(MESSAGE &message)
{
	long nBladeIdx = message.Long();
	if( nBladeIdx<0 || nBladeIdx>=BLADE_INFO_QUANTITY ) return false;

	man = message.EntityID();

	if( nBladeIdx == 1 )
		blade[nBladeIdx].locatorName = sabergunBeltName;
	else
		blade[nBladeIdx].locatorName = beltName;

	return blade[nBladeIdx].LoadBladeModel(message);
}

bool BLADE::LoadGunModel(MESSAGE &message)
{
	_CORE_API->DeleteEntity(gun);
	man = message.EntityID();
	//Имя модельки
	char mdlName[200];
	message.String(sizeof(mdlName), mdlName);
	mdlName[sizeof(mdlName) - 1] = 0;
	if(strlen(mdlName) > 0)
	{
		//Путь до модельки
		char path[256];
		strcpy(path, "Ammo\\");
		strcat(path, mdlName);
		//Путь до текстур
		VGEOMETRY * gs = (VGEOMETRY *)_CORE_API->CreateService("geometry");
		if(gs) gs->SetTexturePath("Ammo\\");
		//Создаём модельку
		_CORE_API->CreateEntity(&gun, "modelr");
		if(!_CORE_API->Send_Message(gun, "ls", MSG_MODEL_LOAD_GEO, path))
		{
			_CORE_API->DeleteEntity(gun);
			if(gs) gs->SetTexturePath("");
			return false;
		}
		if(gs) gs->SetTexturePath("");
	}else return false;
	return true;
}

void BLADE::GunFire()
{
	MODEL *mdl = (MODEL*)_CORE_API->GetEntityPointer(&man);
	NODE *manNode = mdl->GetNode(0);

	//------------------------------------------------------
	//search gunfire
	CMatrix perMtx;
	long sti;

	MODEL *obj = (MODEL*)_CORE_API->GetEntityPointer(&gun);
	if( obj==0 ) // нет пистолета - посмотрим на саблю-пистолет
		obj = (MODEL*)api->GetEntityPointer(&blade[1].eid);

	if(obj!=0)
	{
		NODE *gunNode = obj->GetNode(0);
		sti = -1;
		long idGun = manNode->geo->FindName(gunLocName);

		if((sti = manNode->geo->FindLabelN(sti+1, idGun))>-1)
		{
			Animation *ani = mdl->GetAnimation();
			CMatrix *bones = &ani->GetAnimationMatrix(0);

			GEOS::LABEL lb;
			manNode->geo->GetLabel(sti, lb);
			CMatrix mt;
			mt.Vx() = CVECTOR(lb.m[0][0], lb.m[0][1], lb.m[0][2]);
			mt.Vy() = CVECTOR(lb.m[1][0], lb.m[1][1], lb.m[1][2]);
			mt.Vz() = CVECTOR(lb.m[2][0], lb.m[2][1], lb.m[2][2]);
			mt.Pos() = CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);

			CMatrix mbn = mt*bones[lb.bones[0]];
			mbn.Pos().x *= -1.0f;
			mbn.Vx().x *= -1.0f;
			mbn.Vy().x *= -1.0f;
			mbn.Vz().x *= -1.0f;
			perMtx = mbn*mdl->mtx;
		}

		//search for start
		sti = gunNode->geo->FindLabelN(0, gunNode->geo->FindName(gunFire));
		if(sti>=0)
		{
			GEOS::LABEL lb;
			gunNode->geo->GetLabel(sti, lb);
			CMatrix resm;
			resm.EqMultiply(perMtx, *(CMatrix*)&lb.m);
			CVECTOR rp = perMtx*CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);

			ENTITY_ID prt;
			api->FindClass(&prt, "particles", 0);
			api->Send_Message(prt, "lsffffffl", PS_CREATEX, "gunfire", rp.x, rp.y, rp.z, 
				resm.Vz().x, resm.Vz().y, resm.Vz().z, 0);
		}
		else _CORE_API->Trace("MSG_BLADE_GUNFIRE Can't find gun_fire locator");
	}
}

dword _cdecl BLADE::ProcessMessage(MESSAGE &message)
{
	long n;

	switch(message.Long())
	{
		case MSG_BLADE_SET:
			return LoadBladeModel(message);
		break;

		case MSG_BLADE_BELT:
			n = message.Long();
			if( n==0 ) {
				blade[n].locatorName = beltName;
				blade[n].lifeTime = 0.0f;
			} else if( n==1 ) {
				blade[n].locatorName = sabergunBeltName;
				blade[n].lifeTime = 0.0f;
			}
			//_CORE_API->Trace("MSG_BLADE_BELT::%s", beltName);
		break;

		case MSG_BLADE_HAND:
			n = message.Long();
			if( n==0 ) {
				blade[n].locatorName = handName;
			} else if( n==1 ) {
				blade[n].locatorName = sabergunHandName;
			}
			//_CORE_API->Trace("MSG_BLADE_HAND::%s", handName);
		break;

		case MSG_BLADE_GUNSET:
			return LoadGunModel(message);
		break;
		case MSG_BLADE_GUNBELT:
			gunLocName = gunBeltName;
			//_CORE_API->Trace("MSG_BLADE_GUNBELT::%s", gunLocName);
		break;
		case MSG_BLADE_GUNHAND:
			gunLocName = gunHandName;
			//_CORE_API->Trace("MSG_BLADE_GUNHAND::%s", gunLocName);
		break;
		case MSG_BLADE_GUNFIRE:
			GunFire();
			//_CORE_API->Trace("MSG_BLADE_GUNFIRE::%s", handName);
		break;

		case MSG_BLADE_TRACE_ON:
			n = message.Long();
			if( n>=0 && n<BLADE_INFO_QUANTITY )
			{
				blade[0].lifeTime = blade[0].defLifeTime;
			}
			//_CORE_API->Trace("MSG_BLADE_TRACE_ON::%f", lifeTime);
		break;

		case MSG_BLADE_TRACE_OFF:
			n = message.Long();
			if( n>=0 && n<BLADE_INFO_QUANTITY )
			{
				blade[0].lifeTime = 0.0f;
			}
			//_CORE_API->Trace("MSG_BLADE_TRACE_OFF");
		break;

		case MSG_BLADE_BLOOD:
			//_CORE_API->Trace("MSG_BLADE_BLOOD");
		break;

		case MSG_BLADE_LIGHT:
			//_CORE_API->Trace("MSG_BLADE_LIGHT");
		break;
		case MSG_BLADE_ALPHA:
			blendValue = message.Long();
		break;

		case 1001:
			man = message.EntityID();
			AddTieItem(message);
		break;
		case 1002:
			DelTieItem(message);
		break;
		case 1003:
			DelAllTieItem();
		break;

	}
	return 0;
}

void BLADE::AddTieItem(MESSAGE &message)
{
	long nItemIdx = message.Long();

	char mdlName[MAX_PATH];
	message.String(sizeof(mdlName), mdlName);

	char locName[128];
	message.String(sizeof(locName), locName);

	long n = FindTieItemByIndex(nItemIdx);
	if( n>=0 ) {
		api->Trace("Warning! BLADE::AddTieItem(%d,%s,%s) already set that item",nItemIdx,mdlName,locName);
	} else {
		for( n=0; n<ITEMS_INFO_QUANTITY; n++ )
			if( items[n].nItemIndex == -1 ) break;
		if( n<ITEMS_INFO_QUANTITY )
		{
			items[n].nItemIndex = nItemIdx;
			items[n].LoadItemModel(mdlName,locName);
		} else
		{
			api->Trace("Warning! BLADE::AddTieItem(%d,%s,%s) very mach items already set",nItemIdx,mdlName,locName);
		}
	}
}

void BLADE::DelTieItem(MESSAGE &message)
{
	long nItemIdx = message.Long();
	long n = FindTieItemByIndex(nItemIdx);
	if(n>=0) items[n].Release();
}

void BLADE::DelAllTieItem()
{
	for( long i=0; i<ITEMS_INFO_QUANTITY; i++ )
		if( items[i].nItemIndex != -1 )
			items[i].Release();
}

long BLADE::FindTieItemByIndex(long n)
{
	if( n<0 ) return -1;
	for( long i=0; i<ITEMS_INFO_QUANTITY; i++ )
		if( items[i].nItemIndex == n )
			return i;
	return -1;
}

void BLADE::TIEITEM_INFO::Release()
{
	if( nItemIndex!=-1 )
	{
		nItemIndex = -1;
		api->DeleteEntity(eid);
		if(locatorName) delete locatorName; locatorName=0;
	}
}
void BLADE::TIEITEM_INFO::DrawItem(VDX8RENDER * rs,unsigned int blendValue,MODEL *mdl,NODE *manNode)
{
	MODEL *obj = (MODEL*)api->GetEntityPointer(&eid);
	if(obj!=0)
	{
		CMatrix perMtx;

		NODE *mdlNode = obj->GetNode(0);
		if((blendValue & 0xff000000) == 0xff000000)
		{
			mdlNode->SetTechnique("EnvAmmoShader");
		}else{
			mdlNode->SetTechnique("AnimationBlend");
		}
		long sti = -1;
		long idLoc;
		idLoc = manNode->geo->FindName(locatorName);

		if((sti = manNode->geo->FindLabelN(sti+1, idLoc))>-1)
		{
			Animation *ani = mdl->GetAnimation();
			CMatrix *bones = &ani->GetAnimationMatrix(0);

			GEOS::LABEL lb;
			manNode->geo->GetLabel(sti, lb);
			CMatrix mt;
			mt.Vx() = CVECTOR(lb.m[0][0], lb.m[0][1], lb.m[0][2]);
			mt.Vy() = CVECTOR(lb.m[1][0], lb.m[1][1], lb.m[1][2]);
			mt.Vz() = CVECTOR(lb.m[2][0], lb.m[2][1], lb.m[2][2]);
			mt.Pos() = CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);

			CMatrix mbn = mt*bones[lb.bones[0]];
			mbn.Pos().x *= -1.0f;
			mbn.Vx().x *= -1.0f;
			mbn.Vy().x *= -1.0f;
			mbn.Vz().x *= -1.0f;

			CMatrix scl;
			scl.Vx().x = -1.0f;
			scl.Vy().y = 1.0f;
			scl.Vz().z = 1.0f;
			mbn.EqMultiply(scl, CMatrix(mbn));

			perMtx = mbn*mdl->mtx;
		}
		obj->mtx = perMtx;
		obj->Realize(0);
	}
}
bool BLADE::TIEITEM_INFO::LoadItemModel(const char* mdlName, const char* locName)
{
	api->DeleteEntity(eid);
	if(locatorName) delete locatorName; locatorName=0;

	if( !locName || !mdlName ) return false;

	locatorName = new char[strlen(locName)+1];
	Assert(locatorName);
	strcpy(locatorName,locName);

	//Путь до модельки
	char path[256];
	strcpy(path, "Ammo\\");
	strcat(path, mdlName);
	//Путь до текстур
	VGEOMETRY * gs = (VGEOMETRY *)api->CreateService("geometry");
	if(gs) gs->SetTexturePath("Ammo\\");
	//Создаём модельку
	api->CreateEntity(&eid, "modelr");
	if(!api->Send_Message(eid, "ls", MSG_MODEL_LOAD_GEO, path))
	{
		api->DeleteEntity(eid);
		if(gs) gs->SetTexturePath("");
		return false;
	}
	if(gs) gs->SetTexturePath("");

	return true;
}
