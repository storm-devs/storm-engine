//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	LocationEffects
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "LocationEffects.h"
#include "..\common_h\dx8render.h"


#define LFX_SPLASHES_NUM	(sizeof(chrSplash)/sizeof(ChrSplash))
#define LFX_SPLASHES_P_NUM	64// fix boal for intel cpp (sizeof(LocationEffects::ChrSplash::prt)/sizeof(ParticleSplash))
#define LFX_SPLASHES_SRAD	0.14f

#define LFX_PI				3.141592654f

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================


LocationEffects::LocationEffects()
{
	//Всплеск
	chrSplashRefCounter = 0;
	for(long i = 0; i < LFX_SPLASHES_NUM; i++) chrSplash[i].time = -1.0f;
	splashesTxt = -1;
	//Мухи
	flys = null;
	numFlys = 0;
	maxFlys = 0;
	fly = null;
	numFly = 0;
	flyTex = -1;
	//Шотган
	numSmoke = 0;
	numFlinders = 0;
	numBlood = 0;
	texSmoke = -1;
	texFlinders = -1;
	texBlood = -1;
	texHor = -1;
	isShgInited = false;
}

LocationEffects::~LocationEffects()
{
	if(rs)
	{
		if(splashesTxt >= 0) rs->TextureRelease(splashesTxt);
		if(texSmoke >= 0) rs->TextureRelease(texSmoke);
		if(texFlinders >= 0) rs->TextureRelease(texFlinders);
		if(texBlood >= 0) rs->TextureRelease(texBlood);
		if(texHor >= 0) rs->TextureRelease(texHor);
		if(flyTex >= 0) rs->TextureRelease(flyTex);
	}
	if(flys) delete flys;
	if(fly) delete fly;
}

//Инициализация
bool LocationEffects::Init()
{
	//DX8 render
	rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!rs) _THROW("No service: dx8render");

	_CORE_API->LayerCreate("execute", true, false);
	_CORE_API->LayerSetFlags("execute", LRFLAG_EXECUTE);
	_CORE_API->LayerAdd("execute", GetID(), 10);

	_CORE_API->LayerCreate("realize", true, false);
	_CORE_API->LayerSetFlags("realize", LRFLAG_REALIZE);
	_CORE_API->LayerAdd("realize", GetID(), 1000000);

	splashesTxt = rs->TextureCreate("LocEfx\\chrsplprt.tga");
	flyTex = rs->TextureCreate("LocEfx\\firefly.tga");

	return true;
}

//Исполнение
void LocationEffects::Execute(dword delta_time)
{

}

void LocationEffects::Realize(dword delta_time)
{
	float dltTime = delta_time*0.001f;
	ProcessedFlys(dltTime);
	ProcessedChrSplash(dltTime);
	ProcessedShotgun(dltTime);
}

//Сообщения
dword _cdecl LocationEffects::ProcessMessage(MESSAGE & message)
{
	char buf[32];
	message.String(32, buf);
	buf[31] = 0;
	CVECTOR pos, dir;
	if(stricmp(buf, "Splashes") == 0)
	{
		pos.x = message.Float();
		pos.y = message.Float();
		pos.z = message.Float();
		CreateSplash(pos, message.Float());
	}else
	if(stricmp(buf, "SGFireParticles") == 0)
	{
		pos.x = message.Float();
		pos.y = message.Float();
		pos.z = message.Float();
		dir.x = message.Float();
		dir.y = message.Float();
		dir.z = message.Float();
		SGFirePrt(pos, dir);
	}else
	if(stricmp(buf, "SGBloodParticles") == 0)
	{
		pos.x = message.Float();
		pos.y = message.Float();
		pos.z = message.Float();
		dir.x = message.Float();
		dir.y = message.Float();
		dir.z = message.Float();
		SGBldPrt(pos, dir);
	}else
	if(stricmp(buf, "SGEnvParticles") == 0)
	{
		pos.x = message.Float();
		pos.y = message.Float();
		pos.z = message.Float();
		dir.x = message.Float();
		dir.y = message.Float();
		dir.z = message.Float();
		SGEnvPrt(pos, dir);
	}else
	if(stricmp(buf, "SGInited") == 0)
	{
		SGInited();
	}else
	if(stricmp(buf, "SGRelease") == 0)
	{
		SGRelease();
	}else
	if(stricmp(buf, "AddFly") == 0)
	{
		pos.x = message.Float();
		pos.y = message.Float();
		pos.z = message.Float();
		AddLampFlys(pos);
	}else
	if(stricmp(buf, "DelFlys") == 0)
	{
		numFlys = 0;
		numFly = 0;
	}
	return 0;
}

inline void LocationEffects::DrawParticles(void * prts, long num, long size, long texture, const char * tech, bool isEx, long numU)
{
	if( num<=0 ) return;
	CMatrix camMtx;
	rs->GetTransform(D3DTS_VIEW, camMtx);
	rs->SetTransform(D3DTS_VIEW,CMatrix());
	rs->SetTransform(D3DTS_WORLD,CMatrix());
	rs->TextureSet(0, texture);
	for(long i = 0, n = 0; i < num; i++)
	{
		Particle * parts = (Particle *)prts;
		prts = (char *)prts + size;
		CVECTOR pos = camMtx*parts->pos;
		float size = parts->size*0.5f;
		float sn = sinf(parts->angle);
		float cs = cosf(parts->angle);
		long color = (long(parts->alpha) << 24);
		if(!isEx) color |= 0x00ffffff; else color |= 0x00ffffff & ((ParticleEx *)parts)->color;
		float u1 = 0.0f;
		float u2 = 1.0f;
		if(isEx && numU)
		{
			u2 = 1.0f/float(numU);
			u1 = long(((ParticleEx *)parts)->frame)*u2;
			u2 += u1;
		}
		buffer[n*6 + 0].pos = pos + CVECTOR(size*(-cs + sn), size*(sn + cs), 0.0f);
		buffer[n*6 + 0].color = color;
		buffer[n*6 + 0].u = u1;
		buffer[n*6 + 0].v = 0.0f;
		buffer[n*6 + 1].pos = pos + CVECTOR(size*(-cs - sn), size*(sn - cs), 0.0f);
		buffer[n*6 + 1].color = color;
		buffer[n*6 + 1].u = u1;
		buffer[n*6 + 1].v = 1.0f;
		buffer[n*6 + 2].pos = pos + CVECTOR(size*(cs + sn), size*(-sn + cs), 0.0f);
		buffer[n*6 + 2].color = color;
		buffer[n*6 + 2].u = u2;
		buffer[n*6 + 2].v = 0.0f;
		buffer[n*6 + 3].pos = buffer[n*6 + 2].pos;
		buffer[n*6 + 3].color = color;
		buffer[n*6 + 3].u = u2;
		buffer[n*6 + 3].v = 0.0f;		
		buffer[n*6 + 4].pos = buffer[n*6 + 1].pos;
		buffer[n*6 + 4].color = color;
		buffer[n*6 + 4].u = u1;
		buffer[n*6 + 4].v = 1.0f;
		buffer[n*6 + 5].pos = pos + CVECTOR(size*(cs - sn), size*(-sn - cs), 0.0f);
		buffer[n*6 + 5].color = color;
		buffer[n*6 + 5].u = u2;
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
	rs->SetTransform(D3DTS_VIEW, camMtx);
}

void LocationEffects::CreateSplash(const CVECTOR & pos, float power)
{
	//Выберим освободившийся блок
	for(long i = 0; i < LFX_SPLASHES_NUM; i++) if(chrSplash[i].time < 0.0f) break;
	if(i >= LFX_SPLASHES_NUM) return;
	if(power < 0.0f) power = 0.0f;
	if(power > 1.0f) power = 1.0f;
	//Инициализируем
	ChrSplash & spl = chrSplash[i];
	spl.time = 0.0f;
	spl.kTime = 1.4f;
	for(i = 0; i < LFX_SPLASHES_P_NUM; i++)
	{	
		float ang = rand()*(LFX_PI*2.0f/RAND_MAX);
		float r = rand()*(2.0f*LFX_SPLASHES_SRAD/float(RAND_MAX));
		float s = 1.0f + rand()*(3.0f/float(RAND_MAX));
		spl.prt[i].dir = CVECTOR(0.3f*sinf(ang), s, 0.3f*cosf(ang));
		spl.prt[i].pos = pos + CVECTOR(r*sinf(ang), 0.0f, r*cosf(ang));
		spl.prt[i].angle = 0.0f;
		spl.prt[i].alpha = 0.0f;
		spl.prt[i].dAng = ((rand() & 7)*(1.0f/7.0f) - 0.5f)*0.2f;
		spl.prt[i].size = 0.5f + rand()*(0.5f/float(RAND_MAX));
	}
	if(chrSplashRefCounter < 0) chrSplashRefCounter = 0;
	chrSplashRefCounter++;
}

void LocationEffects::ProcessedChrSplash(float dltTime)
{
	dltTime *= 0.9f;
	if(chrSplashRefCounter <= 0) return;
	for(long i = 0; i < LFX_SPLASHES_NUM; i++)
	{
		ChrSplash & spl = chrSplash[i];
		//Смотрим время жизни
		if(spl.time < 0.0f) continue;
		if(spl.time >= 1.0f)
		{
			spl.time = -1.0f;
			chrSplashRefCounter--;
			continue;
		}
		//Вычисляем альфазначения
		float aprt = spl.time;
		if(aprt < 0.1f) aprt *= 10.0f; else if(aprt > 0.5f) aprt = 1.0f - (aprt - 0.5f)/(1.0f - 0.5f); else aprt = 1.0f;
		aprt = aprt*255.0f*0.2f;
		//Партиклы
		for(long j = 0; j < LFX_SPLASHES_P_NUM; j++)
		{
			spl.prt[j].pos += spl.prt[j].dir*dltTime;
			spl.prt[j].alpha = aprt;
			spl.prt[j].dir.y -= 9.8f*dltTime;
			spl.prt[i].angle += spl.prt[i].dAng*dltTime;
		}
		//Увеличиваем время жизни
		spl.time += dltTime*spl.kTime;
		//Рисуем
		DrawParticles(spl.prt, LFX_SPLASHES_P_NUM, sizeof(ParticleSplash), splashesTxt, "LocCharacterSplashes");
	}
}

//---------------------------------------------------
//Мухи у фанарей
//---------------------------------------------------

void LocationEffects::AddLampFlys(CVECTOR & pos)
{
	//Занимаем массив
	if(numFlys >= maxFlys)
	{
		maxFlys += 8;
		flys = (LampFlys *)RESIZE(flys, maxFlys*sizeof(LampFlys));
	}
	//Заполняем параметры
	//Общие
	flys[numFlys].pos = pos;
	flys[numFlys].radius = 0.6f;
	flys[numFlys].start = numFly;
	flys[numFlys].num = 1 + (rand() & 7);	
	numFly += flys[numFlys].num;
	fly = (ParticleFly *)RESIZE(fly, numFly*sizeof(ParticleFly));
	//Каждой мухи
	for(long i = 0; i < flys[numFlys].num; i++)
	{
		ParticleFly & f = fly[flys[numFlys].start + i];
		f.ax = rand()*2.0f*LFX_PI/RAND_MAX;
		f.ay = rand()*2.0f*LFX_PI/RAND_MAX;
		f.kx = 0.8f + rand()*(0.4f/RAND_MAX);
		if(rand() & 1) f.kx = -f.kx;
		f.ky = 0.8f + rand()*(0.4f/RAND_MAX);
		if(rand() & 1) f.ky = -f.ky;
		f.a = rand()*2.0f*LFX_PI/RAND_MAX;
		f.k = 0.8f + rand()*(0.4f/RAND_MAX);
		if(rand() & 1) f.k = -f.k;
		f.angle = 0.0f;
		f.size = 0.03f;
		f.alpha = 1.0f;
		f.frame = float(rand() & 3);
	}
	numFlys++;
}

void LocationEffects::ProcessedFlys(float dltTime)
{
	CMatrix view;
	rs->GetTransform(D3DTS_VIEW, view);
	view.Transposition();
	CVECTOR cam = view.Pos();
	float dax = dltTime*1.3f;
	float day = dltTime*1.4f;
	float da = dltTime*5.6f;
	//Расчитываем
	for(long i = 0; i < numFlys; i++)
	{
		//Коэфициент видимости
		CVECTOR dir = cam - flys[i].pos;
		float k = ~dir;
		if(k > 400.0f) continue;
		k = sqrtf(k);
		if(k > 0.0f) dir *= 1.0f/k;
		k = k/20.0f;
		k = 3.0f*(1.0f - k);
		if(k > 1.0f) k = 1.0f;		
		//Обновляем мух
		ParticleFly * fl = fly + flys[i].start;
		for(long j = 0; j < flys[i].num; j++)
		{
			ParticleFly & f = fl[j];
			//Углы
			f.ax += dax*f.kx; f.ay += day*f.ky; f.a += da*f.k;
			//Радиус
			float r = 1.0f + 0.5f*sinf(f.a) + 0.2f*cosf(f.a*f.k*2.1f);
			r *= flys[i].radius;
			//Позиция
			f.pos.x = flys[i].pos.x + r*sinf(f.ax)*sinf(f.ay);
			f.pos.y = flys[i].pos.y + r*cosf(f.ax)*cosf(f.ay);
			f.pos.z = flys[i].pos.z + r*sinf(f.ax)*cosf(f.ay);
			//Прозрачность
			f.alpha = k*255.0f;
			//Цвет
			CVECTOR tmp = f.pos - flys[i].pos;
			float dst = sqrtf(~tmp);
			if(dst > 0.0f) tmp *= 1.0f/dst;
			float cs = -(tmp | dir) + 0.4f;
			if(dst > flys[i].radius)
			{
				dst = 3.0f*(dst - flys[i].radius)/flys[i].radius;
				if(dst > 1.0f) dst = 1.0f;
				cs *= 1.0f - dst;
			}
			if(cs < 0.0f) cs = 0.0f;
			if(cs > 1.0f) cs = 1.0f;
			f.color = long(cs*255.0f);
			f.color |= (f.color << 16) | (f.color << 8);
			//Кадр
			f.frame += dltTime*f.k*25.0f;
			if(f.frame >= 4.0f) f.frame -= 4.0f;
			//Угл
			f.angle += dltTime*f.k*3.0f;
		}
	}
	//Рисуем
	DrawParticles(fly, numFly, sizeof(ParticleFly), flyTex, "LocFly", true, 4);
}

//---------------------------------------------------------------------------------

void LocationEffects::SGInited()
{
	if(!isShgInited)
	{
		texSmoke = rs->TextureCreate("LocEfx\\sgsmoke.tga");
		texFlinders = rs->TextureCreate("LocEfx\\sgflinders.tga");
		texBlood = rs->TextureCreate("LocEfx\\sgblood.tga");
		texHor = rs->TextureCreate("LocEfx\\sghor.tga");
		isShgInited = true;
	}
}

void LocationEffects::SGRelease()
{
	if(texSmoke >= 0) rs->TextureRelease(texSmoke);
	if(texFlinders >= 0) rs->TextureRelease(texFlinders);
	if(texBlood >= 0) rs->TextureRelease(texBlood);
	if(texHor >= 0) rs->TextureRelease(texHor);	
	isShgInited = false;
	texSmoke = -1;
	texFlinders = -1;
	texBlood = -1;
	texHor = -1;
}

void LocationEffects::SGEnvPrt(const CVECTOR & pos, const CVECTOR & ndir)
{
	SGInited();
	long max = sizeof(flinders)/sizeof(flinders[0]);
	long num = 4 + (rand() & 3);
	for(long i = 0; i < num && numFlinders < max; i++)
	{
		flinders[numFlinders].pos = pos;
		flinders[numFlinders].spd = ndir;
		float r = rand()*0.7f/RAND_MAX;
		float a = rand()*6.283185307f/(RAND_MAX + 1);
		flinders[numFlinders].spd.y += r*sinf(a);
		r *= cosf(a);
		flinders[numFlinders].spd.x += r*ndir.z;
		flinders[numFlinders].spd.z -= r*ndir.x;
		flinders[numFlinders].spd *= 1.0f + (rand() & 15)*2.0f/15.0f;
		flinders[numFlinders].angle = a;
		flinders[numFlinders].dang = 0.3f - 0.6f*rand()/RAND_MAX;
		if(flinders[numFlinders].dang >= 0.0f)
		{
			flinders[numFlinders].dang += 0.1f;
		}else{
			flinders[numFlinders].dang -= 0.1f;
		}
		flinders[numFlinders].size = 0.1f;
		flinders[numFlinders].alpha = 255.0f;
		flinders[numFlinders].time = 0.0f;
		flinders[numFlinders].ktime = 2.0f;
		numFlinders++;
	}
}


void LocationEffects::SGBldPrt(const CVECTOR & pos, const CVECTOR & ndir)
{
	SGInited();
	long max = sizeof(blood)/sizeof(blood[0]);
	long num = 16 + (rand() & 7);
	for(long i = 0; i < num && numBlood < max; i++)
	{
		blood[numBlood].pos = pos;
		blood[numBlood].spd = ndir;
		float r = rand()*0.7f/RAND_MAX;
		float a = rand()*6.283185307f/(RAND_MAX + 1);
		blood[numBlood].spd.y += r*sinf(a);
		r *= cosf(a);
		blood[numBlood].spd.x += r*ndir.z;
		blood[numBlood].spd.z -= r*ndir.x;
		blood[numBlood].spd *= 1.0f + (rand() & 15)*2.0f/15.0f;
		blood[numBlood].angle = 0.0f;
		blood[numBlood].dang = 0.3f - 0.6f*rand()/RAND_MAX;
		if(blood[numBlood].dang >= 0.0f)
		{
			blood[numBlood].dang += 0.1f;
		}else{
			blood[numBlood].dang -= 0.1f;
		}
		blood[numBlood].size = 0.3f;
		blood[numBlood].alpha = 1.0f;
		blood[numBlood].time = 0.0f;
		blood[numBlood].ktime = 1.0f;
		numBlood++;
	}
}

void LocationEffects::SGFirePrt(const CVECTOR & pos, const CVECTOR & ndir)
{
	SGInited();
	long max = sizeof(smoke)/sizeof(smoke[0]);
	long num = 5 + (rand() & 3);
	for(long i = 0; i < num && numSmoke < max; i++)
	{
		smoke[numSmoke].pos = pos;
		smoke[numSmoke].spd = ndir;
		float r = rand()*0.03f/RAND_MAX;
		float a = rand()*6.283185307f/(RAND_MAX + 1);
		smoke[numSmoke].spd.y += r*sinf(a);
		r *= cosf(a);
		smoke[numSmoke].spd.x += r*ndir.z;
		smoke[numSmoke].spd.z -= r*ndir.x;
		smoke[numSmoke].spd *= 2.0f + (rand() & 15)*20.0f/15.0f;
		smoke[numSmoke].angle = a;
		smoke[numSmoke].dang = 0.3f - 0.6f*rand()/RAND_MAX;
		if(smoke[numSmoke].dang >= 0.0f)
		{
			smoke[numSmoke].dang += 0.1f;
		}else{
			smoke[numSmoke].dang -= 0.1f;
		}
		smoke[numSmoke].size = 0.001f;
		blood[numBlood].alpha = 0.0f;
		smoke[numSmoke].time = 0.0f;
		smoke[numSmoke].ktime = 1.0f + (rand() & 15)*0.2f/15.0f;
		numSmoke++;
	}
}

void LocationEffects::ProcessedShotgun(float dltTime)
{
	if(!isShgInited) return;
	CVECTOR winDir = 0.0f;
	VDATA * param = _CORE_API->Event("EWhr_GetWindAngle", 0);
	if(param)
	{
		float ang;
		if(!param->Get(ang)) ang = 0.0f;
		winDir.x = sinf(ang);
		winDir.z = cosf(ang);
	}
	param = _CORE_API->Event("EWhr_GetWindSpeed", 0);
	if(param)
	{
		float spd;
		if(!param->Get(spd)) spd = 0.0f;
		if(spd < 0.0f) spd = 0.0f;
		if(spd > 20.0f) spd = 20.0f;
		winDir *= powf(0.001f + spd/20.0f, 10.0f)*10.0f;
	}else{
		winDir *= 0.05f;
	}
	for(long i = 0, j = 0; i < numSmoke; i++)
	{
		smoke[i].pos += (smoke[i].spd + winDir)*dltTime;
		float k = dltTime*10.0f;
		if(k > 1.0f) k = 1.0f;
		smoke[i].spd -= smoke[i].spd*k;
		smoke[i].angle += smoke[i].dang*dltTime;
		if(smoke[i].size < 0.5f)
		{
			smoke[i].size += dltTime*10.0f;
		}else{
			if(smoke[i].size < 2.0f)
			{
				smoke[i].size += dltTime*1.0f;
			}else smoke[i].size = 2.0f;
		}
		if(smoke[i].time < 0.1f)
		{
			smoke[i].alpha = smoke[i].time/0.1f;
		}else{
			smoke[i].alpha = 1.0f - (smoke[i].time - 0.1f)/(1.0f - 0.1f);
		}
		smoke[i].alpha *= 0.4f*255.0f;
		smoke[i].time += smoke[i].ktime*dltTime;
		if(smoke[i].time < 1.0f)
		{
			if(i != j) smoke[j] = smoke[i];
			j++;
		};
	}
	numSmoke = j;
	for(i = 0, j = 0; i < numBlood; i++)
	{
		blood[i].pos += blood[i].spd*dltTime;
		float k = dltTime*5.0f;
		if(k > 1.0f) k = 1.0f;
		blood[i].spd.x -= blood[i].spd.x*k;
		blood[i].spd.z -= blood[i].spd.z*k;
		blood[i].spd.y -= dltTime*8.8f;
		if(blood[i].time > 0.9f) blood[i].alpha = 1.0f - (blood[i].time - 0.9f)*10.0f;
		blood[i].alpha = 0.4f*255.0f;
		blood[i].time += blood[i].ktime*dltTime;
		if(blood[i].time < 1.0f)
		{
			if(i != j) blood[j] = blood[i];
			j++;
		};
	}
	numBlood = j;
	for(i = 0, j = 0; i < numFlinders; i++)
	{
		flinders[i].pos += flinders[i].spd*dltTime;
		float k = dltTime*5.0f;
		if(k > 1.0f) k = 1.0f;
		flinders[i].spd.x -= flinders[i].spd.x*k;
		flinders[i].spd.z -= flinders[i].spd.z*k;
		flinders[i].spd.y -= dltTime*8.8f;
		if(flinders[i].time > 0.9f) flinders[i].alpha = 1.0f - (flinders[i].time - 0.9f)*10.0f;
		flinders[i].time += flinders[i].ktime*dltTime;
		if(flinders[i].time < 1.0f)
		{
			if(i != j) flinders[j] = flinders[i];
			j++;
		};
	}
	numFlinders = j;
	//
	if(numFlinders)
	{
		DrawParticles(flinders, numFlinders, sizeof(flinders[0]), texFlinders, "ShootParticles");
	}
	if(numBlood)
	{
		DrawParticles(blood, numBlood, sizeof(blood[0]), texBlood, "ShootParticles");
	}
	if(numSmoke)
	{
		DrawParticles(smoke, numSmoke, sizeof(smoke[0]), texSmoke, "ShootParticles");
	}
	CMatrix mtx;
	rs->GetTransform(D3DTS_VIEW, mtx);	
	mtx.Transposition();
	Particle prt;
	prt.pos = mtx.Pos() + mtx.Vz()*2.0f;
	prt.angle = 0.0f;
	prt.size = 0.1f;
	prt.alpha = 255.0f;
 	DrawParticles(&prt, 1, sizeof(prt), texHor, "ShootParticlesNoZ");
}



