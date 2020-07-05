//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	TornadoParticles
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "TornadoParticles.h"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

TornadoParticles::TornadoParticles(Pillar & _pillar) : pillar(_pillar)
{
	for(long i = 0; i < sizeof(groundPrt)/sizeof(GroundParticle); i++)
	{
		groundPrt[i].pos = 0.0f;
		groundPrt[i].size = GetRand(25.0f, 0.3f);
		groundPrt[i].alpha = 0.7f;
		groundPrt[i].angle = rand()*2.0f*TRND_PI/RAND_MAX;
		groundPrt[i].k = 0.8f + rand()*0.4f/RAND_MAX;
		groundPrt[i].a = rand()*2.0f*TRND_PI/RAND_MAX;
		groundPrt[i].r = 20.0f + rand()*5.0f/RAND_MAX;
		groundPrt[i].t = rand()*1.0f/RAND_MAX;
		groundPrt[i].dt = 0.0f;
		groundPrt[i].p = 1.5f + 2.5f*rand()/RAND_MAX;
	}
	for(i = 0; i < sizeof(pillarPrt)/sizeof(PillarParticle); i++)
	{
		pillarPrt[i].ang = pillar.RandomPos(pillarPrt[i].pos);		
		pillarPrt[i].sz = pillarPrt[i].size = GetRand(10.0f, 0.3f);
		pillarPrt[i].alpha = 0.7f;
		pillarPrt[i].angle = rand()*2.0f*TRND_PI/RAND_MAX;
		pillarPrt[i].k = 0.8f + rand()*0.4f/RAND_MAX;
	}
	txtGroundPrts = -1;
	txtPillarPrts = -1;
	SetGlobalAlpha(1.0f);
}

TornadoParticles::~TornadoParticles()
{

}

void TornadoParticles::SetSea()
{
	api->FindClass(&seaID, "sea", 0);
}

void TornadoParticles::Update(float dltTime)
{
	//Получим точку уровня моря
	float seaLevel = 0.0f;
	if(txtGroundPrts >= 0 || txtPillarPrts >= 0)
	{
		SEA_BASE * sea = (SEA_BASE *)api->GetEntityPointer(&seaID);
		if(sea)
		{
			seaLevel = sea->WaveXZ(pillar.GetX(0.0f), pillar.GetZ(0.0f));
		}
	}
	//seaLevel -= 0.5f;
	//Партиклы у земли
	for(long i = 0; i < sizeof(groundPrt)/sizeof(GroundParticle); i++)
	{
		float k = pillarPrt[i].k;
		//Время жизни частицы
		groundPrt[i].dt += dltTime*0.3f;
		groundPrt[i].t += k*dltTime*groundPrt[i].dt;
		if(groundPrt[i].t > 1.0f)
		{
			groundPrt[i].t -= long(groundPrt[i].t);
			groundPrt[i].dt = 0.0f;
		}
		float t = groundPrt[i].t;
		if (t == 0) continue; //eddy. вылет по assert нам не нужен
		//Assert(t);
		//Вражение вокруг оси столба
		groundPrt[i].a += k*dltTime*(0.5f + 5.0f*t);
		if(groundPrt[i].a > TRND_PI*2.0f) groundPrt[i].a -= TRND_PI*2.0f;
		//Позиция
		float r = groundPrt[i].r*((1.0f - t)*(1.0f - t)*0.7f + 0.3f);
		groundPrt[i].pos.y = seaLevel + 30.0f*powf(t, groundPrt[i].p);
		groundPrt[i].pos.x = pillar.GetX(groundPrt[i].pos.y) + r*sinf(groundPrt[i].a);
		groundPrt[i].pos.z = pillar.GetZ(groundPrt[i].pos.y) + r*cosf(groundPrt[i].a);
		//Вращение вокруг себя
		groundPrt[i].angle += k*dltTime*(0.1f + 5.0f*t*t);
		if(groundPrt[i].angle > TRND_PI*2.0f) groundPrt[i].angle -= TRND_PI*2.0f;
		//Прозрачность
		groundPrt[i].alpha = 1.0f;
		if(t < 0.2f) groundPrt[i].alpha = t/0.2f;
		if(t > 0.5f) groundPrt[i].alpha = 1.0f - (t - 0.5f)/0.5f;
		k -= 0.2f;
		if(k > 1.0f) k = 1.0f;
		groundPrt[i].alpha *= k;
		//if(groundPrt[i].pos.y > 3.0f) groundPrt[i].alpha *= 1.0f - (groundPrt[i].pos.y - 3.0f)/(30.0f - 3.0f);
	}
	//Партиклы столба
	for(i = 0; i < sizeof(pillarPrt)/sizeof(PillarParticle); i++)
	{
		float kh = pillar.GetKHeight(pillarPrt[i].pos.y);
		float k = pillarPrt[i].k;
		pillarPrt[i].pos.y += k*dltTime*(20.0f + 40.0f*kh*kh);
		if(pillarPrt[i].pos.y >= pillar.GetHeight()) pillarPrt[i].pos.y = 0.0f;
		float x = pillar.GetX(pillarPrt[i].pos.y);
		float z = pillar.GetZ(pillarPrt[i].pos.y);
		float r = pillar.GetRaduis(pillarPrt[i].pos.y) - pillarPrt[i].size*0.25f;
		pillarPrt[i].angle += k*dltTime*3.5f;
		if(pillarPrt[i].angle > TRND_PI*2.0f) pillarPrt[i].angle -= TRND_PI*2.0f;
		pillarPrt[i].ang += k*dltTime*(19.0f - 10.0f*pillar.GetKHeight(pillarPrt[i].pos.y));
		if(pillarPrt[i].ang > TRND_PI*2.0f) pillarPrt[i].ang -= TRND_PI*2.0f;
		pillarPrt[i].pos.x = x + r*sinf(pillarPrt[i].ang);
		pillarPrt[i].pos.z = z + r*cosf(pillarPrt[i].ang);
		pillarPrt[i].size = pillarPrt[i].sz + kh*kh*50.0f;
		kh = pillar.GetKHeight(pillarPrt[i].pos.y);
		if(kh > 0.5f) pillarPrt[i].alpha = 0.2f*(1.0f - (kh - 0.5f)/(1.0f - 0.5f));
		if(kh < 0.2f) pillarPrt[i].alpha = 0.2f*(1.0f - (0.2f - kh)/0.2f);
	}
}

void TornadoParticles::Draw(VDX8RENDER * rs)
{
	rs->GetTransform(D3DTS_VIEW, camMtx);
	rs->SetTransform(D3DTS_VIEW,CMatrix());
	rs->SetTransform(D3DTS_WORLD,CMatrix());
	rs->TextureSet(0, txtPillarPrts);
	DrawParticles(rs, pillarPrt, sizeof(pillarPrt)/sizeof(PillarParticle), sizeof(PillarParticle), -1, "TornadoPillarParticles");
	rs->TextureSet(0, txtGroundPrts);
	DrawParticles(rs, groundPrt, sizeof(groundPrt)/sizeof(GroundParticle), sizeof(GroundParticle), -1, "TornadoGroundParticles");
	rs->SetTransform(D3DTS_VIEW, camMtx);
}

inline void TornadoParticles::DrawParticles(VDX8RENDER * rs, void * prts, long num, long size, long texture, const char * tech)
{
	for(long i = 0, n = 0; i < num; i++)
	{
		Particle * parts = (Particle *)prts;
		prts = (char *)prts + size;
		CVECTOR pos = camMtx*parts->pos;
		float size = parts->size*0.5f;
		float sn = sinf(parts->angle);
		float cs = cosf(parts->angle);
		long color = (long(parts->alpha*galpha) << 24) | 0x00ffffff;
		buffer[n*6 + 0].pos = pos + CVECTOR(size*(-cs + sn), size*(sn + cs), 0.0f);
		buffer[n*6 + 0].color = color;
		buffer[n*6 + 0].u = 0.0f;
		buffer[n*6 + 0].v = 0.0f;
		buffer[n*6 + 1].pos = pos + CVECTOR(size*(-cs - sn), size*(sn - cs), 0.0f);
		buffer[n*6 + 1].color = color;
		buffer[n*6 + 1].u = 0.0f;
		buffer[n*6 + 1].v = 1.0f;
		buffer[n*6 + 2].pos = pos + CVECTOR(size*(cs + sn), size*(-sn + cs), 0.0f);
		buffer[n*6 + 2].color = color;
		buffer[n*6 + 2].u = 1.0f;
		buffer[n*6 + 2].v = 0.0f;
		buffer[n*6 + 3].pos = buffer[n*6 + 2].pos;
		buffer[n*6 + 3].color = color;
		buffer[n*6 + 3].u = 1.0f;
		buffer[n*6 + 3].v = 0.0f;		
		buffer[n*6 + 4].pos = buffer[n*6 + 1].pos;
		buffer[n*6 + 4].color = color;
		buffer[n*6 + 4].u = 0.0f;
		buffer[n*6 + 4].v = 1.0f;
		buffer[n*6 + 5].pos = pos + CVECTOR(size*(cs - sn), size*(-sn - cs), 0.0f);
		buffer[n*6 + 5].color = color;
		buffer[n*6 + 5].u = 1.0f;
		buffer[n*6 + 5].v = 1.0f;
		n++;
		if(n*2 == 256)
		{
			rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, n*2, buffer, sizeof(Vertex), (char *)tech);
			n = 0;
		}
	}
	if(n > 0)
	{
		rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, n*2, buffer, sizeof(Vertex), (char *)tech);
	}
}

void TornadoParticles::SetGlobalAlpha(float a)
{
	if(a < 0.0f) a = 0.0f;
	if(a > 1.0f) a = 1.0f;
	galpha = a*255.0f;
}
