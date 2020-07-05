//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Tornado
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "Tornado.h"

//============================================================================================


INTERFACE_FUNCTION
CREATE_CLASS(Tornado)

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

Tornado::Tornado() : particles(pillar), noiseCloud(pillar), debris(pillar)
{
	ib = -1;
	vb = -1;
	rs = null;
	eventCounter = 0.0f;
	liveTime = 60.0f;
	galhpa = 1.0f;
	soundService = null;
	sID = SOUND_INVALID_ID;
}

Tornado::~Tornado()
{
	if(rs)
	{
		if(ib >= 0) rs->ReleaseIndexBuffer(ib);
		if(vb >= 0) rs->ReleaseVertexBuffer(vb);
		if(noiseCloud.texture >= 0) rs->TextureRelease(noiseCloud.texture);
		if(particles.txtPillarPrts >= 0) rs->TextureRelease(particles.txtPillarPrts);
		if(particles.txtGroundPrts >= 0) rs->TextureRelease(particles.txtGroundPrts);
	}
	if(soundService && sID != SOUND_INVALID_ID) soundService->SoundRelease(sID);
}

//============================================================================================

//Инициализация
bool Tornado::Init()
{
	_CORE_API->LayerCreate("execute", true, false);
	_CORE_API->LayerSetExecute("execute", true);	
	_CORE_API->LayerCreate("realize", true, false);
	_CORE_API->LayerSetRealize("realize", true);
	_CORE_API->LayerAdd("execute", GetID(), 70000);
	_CORE_API->LayerAdd("realize", GetID(), 70000);

	//DX8 render
	rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!rs) _THROW("No service: dx8render");

	//Создаём буфера для столба
	ib = rs->CreateIndexBuffer(pillar.GetNumTriangles()*3*sizeof(word));
	if(ib < 0) return false;
	vb = rs->CreateVertexBuffer(D3DFVF_XYZ | D3DFVF_DIFFUSE, pillar.GetNumVerteces()*sizeof(Pillar::Vertex), D3DUSAGE_WRITEONLY);
	if(vb < 0) return false;
	word * ibpnt = (word *)rs->LockIndexBuffer(ib);
	if(!ibpnt) return false;
	pillar.FillIndexBuffer(ibpnt);
	rs->UnLockIndexBuffer(ib);
	noiseCloud.texture = rs->TextureCreate("Tornado\\trncloud.tga");
	particles.txtPillarPrts = rs->TextureCreate("Tornado\\pillarprts.tga");
	particles.txtGroundPrts = rs->TextureCreate("Tornado\\groundprts.tga");
	particles.SetSea();
	particles.Update(0.0f);
	debris.Init();
	//Создаём звук
	soundService = (VSoundService *)api->CreateService("SoundService");
	if(soundService)
	{
		sID = soundService->SoundPlay("tornado", PCM_3D, VOLUME_FX, false, true, false, 0, &CVECTOR(pillar.GetX(0.0f), 0.0f, pillar.GetZ(0.0f)));
	}
	return true;
}

//Исполнение
void Tornado::Execute(dword delta_time)
{	
	float dltTime = delta_time*0.001f*1.0f;
	pillar.Update(dltTime);
	particles.Update(dltTime);
	noiseCloud.Update(dltTime);
	debris.Update(dltTime);
	eventCounter += dltTime;
	if(eventCounter > 1.0f) _CORE_API->Event("TornadoDamage", "fff", eventCounter, pillar.GetX(0.0f), pillar.GetZ(0.0f));
	if(liveTime < 0.0f)
	{
		SetAlpha(galhpa);
		galhpa -= dltTime*0.2f;
		if(galhpa < 0.0f)
		{
			galhpa = 0.0f;
			_CORE_API->Event("TornadoDelete", null);
			_CORE_API->DeleteEntity(GetID());
		}
	}else liveTime -= dltTime;	
	if(soundService && sID != SOUND_INVALID_ID)
	{
		soundService->SoundSet3DParam(sID, SM_POSITION, &CVECTOR(pillar.GetX(0.0f), 0.0f, pillar.GetZ(0.0f)));
	}
}

void Tornado::Realize(dword delta_time)
{
	liveTime = 1000.0f;
	//Обломки
	debris.Draw(rs);
	//Единичная мировая матрица
	rs->SetTransform(D3DTS_WORLD, CMatrix());
	//Облака
	noiseCloud.Draw(rs);
	//Столб
	Pillar::Vertex * vrt = (Pillar::Vertex *)rs->LockVertexBuffer(vb);
	if(!vrt) return;
	rs->TextureSet(0, -1);
	pillar.FillVertexBuffer(vrt);
	rs->UnLockVertexBuffer(vb);
	rs->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	rs->DrawIndexedPrimitiveNoVShader(D3DPT_TRIANGLELIST, vb, sizeof(Pillar::Vertex), ib, 0, pillar.GetNumVerteces(), 0, pillar.GetNumTriangles(), "TornadoPillar");
	//Системы частиц
	particles.Draw(rs);
}

dword _cdecl Tornado::ProcessMessage(MESSAGE & message)
{
	liveTime = message.Float();
	if(liveTime < 5.0f) liveTime = 5.0f;
	if(liveTime > 3600.0f) liveTime = 3600.0f;
	return 0;
}

void Tornado::SetAlpha(float a)
{
	if(a < 0.0f) a= 0.0f;
	if(a > 1.0f) a= 1.0f;
	galhpa = a;
	pillar.SetGlobalAlpha(a);
	particles.SetGlobalAlpha(a);
	noiseCloud.SetGlobalAlpha(a);
	debris.SetGlobalAlpha(a);
}



