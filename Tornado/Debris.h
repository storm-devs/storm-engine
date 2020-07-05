//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Debris
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _Debris_H_
#define _Debris_H_

#include "..\common_h\matrix.h"
#include "..\common_h\model.h"
#include "..\SoundService\VSoundService.h"
#include "Pillar.h"

class Debris  
{
	struct ModelInfo
	{
		float a, r;
		float y, ay;
		float alpha;
		float scale;
		float maxSpeed;
		CVECTOR ang;
		MODEL * mdl;
	};

	struct Model
	{
		MODEL * mdl;
		float prt;
		float maxSpeed;
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	Debris(Pillar & _pillar);
	virtual ~Debris();

	void Init();

	void Update(float dltTime);
	void Draw(VDX8RENDER * rs);

	void SetGlobalAlpha(float a);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	void AddModel(const char * modelNamem, float prt, float spd);
	void NormalazedModels();
	MODEL * SelectModel(float & maxSpd);
	bool IsShip();

private:
	VSoundService * soundService;
	float lastPlayTime;
	Pillar & pillar;
	float galpha;
	Model mdl[16];		//Загруженные модельки
	long numModels;		//Количество моделей

	ModelInfo fly[64];	//Летящии модельки
	long flyCounter;	//Количество летящих моделек

	dword shipcode;
};

#endif

