//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	LocationEffects
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _LocationEffects_H_
#define _LocationEffects_H_


#include "..\common_h\vmodule_api.h"
#include "..\common_h\matrix.h"

class VDX8RENDER;

#define LFX_SPLASHES_SECT	16

class LocationEffects : public ENTITY  
{

#pragma pack(push, 1)

	struct Vertex
	{
		CVECTOR pos;
		dword color;
		float u, v;
	};

#pragma pack(pop)

	struct Particle
	{
		CVECTOR pos;
		float angle;
		float size;
		float alpha;
	};

	struct ParticleEx : public Particle
	{
		dword color;
		float frame;
	};

	struct ParticleSplash : public Particle
	{
		CVECTOR dir;
		float dAng;
	};

	struct ChrSplash
	{		
		float time;
		float kTime;
		CVECTOR pos;
		ParticleSplash prt[64];
	};

	struct ParticleFly : public ParticleEx
	{
		float ax, ay;
		float kx, ky;
		float a, k;
	};

	struct LampFlys
	{
		CVECTOR pos;
		float radius;
		long start;
		long num;
	};

	struct ParticleSG : public Particle
	{
		CVECTOR spd;		
		float dang;		
		float time;
		float ktime;
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	LocationEffects();
	virtual ~LocationEffects();


	//Инициализация
	bool Init();
	//Исполнение
	void Execute(dword delta_time);
	void Realize(dword delta_time);
	//Сообщения
	dword _cdecl ProcessMessage(MESSAGE & message);


//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	void DrawParticles(void * prts, long num, long size, long texture, const char * tech, bool isEx = false, long numU = 0);


private:
	VDX8RENDER * rs;


	//---------------------------------------------------
	//Брызги от персонажа
	//---------------------------------------------------
	void CreateSplash(const CVECTOR & pos, float power);
	void ProcessedChrSplash(float dltTime);

	ChrSplash chrSplash[4];
	long chrSplashRefCounter;
	long splashesTxt;

	//---------------------------------------------------
	//Мухи у фанарей
	//---------------------------------------------------

	void AddLampFlys(CVECTOR & pos);
	void ProcessedFlys(float dltTime);

	LampFlys * flys;
	long numFlys;
	long maxFlys;
	ParticleFly * fly;
	long numFly;
	long flyTex;

	//---------------------------------------------------
	//Партиклы шотгана
	//---------------------------------------------------
	void SGInited();
	void SGRelease();
	void SGEnvPrt(const CVECTOR & pos, const CVECTOR & ndir);
	void SGBldPrt(const CVECTOR & pos, const CVECTOR & ndir);
	void SGFirePrt(const CVECTOR & pos, const CVECTOR & ndir);
	void ProcessedShotgun(float dltTime);

	ParticleSG smoke[64];
	bool isShgInited;
	long numSmoke;
	long texSmoke;
	ParticleSG flinders[256];
	long numFlinders;
	long texFlinders;
	ParticleSG blood[256];
	long numBlood;
	long texBlood;
	long texHor;

	Vertex buffer[256*6];
};

#endif

