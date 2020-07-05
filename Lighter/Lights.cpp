//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Lights
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "Lights.h"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

Lights::Lights()
{
	maxLights = 256;
	numLights = 3;
	light = (Light *)RESIZE(null, maxLights*sizeof(Light));
	for(long i = 0; i < maxLights; i++) SetDefLightParam(i);	
	light[0].type = Light::t_amb;
	light[0].color = CVECTOR(0.2f, 0.2f, 0.2f);
	light[0].group = null;
	light[0].isOn = false;
	light[1].type = Light::t_sun;
	light[1].color = CVECTOR(0.9f, 0.9f, 0.6f);
	light[1].p = !CVECTOR(1.0f, 1.0f, 1.0f);
	light[1].group = null;
	light[1].isOn = false;
	light[2].type = Light::t_sky;
	light[2].color = CVECTOR(0.3f, 0.5f, 1.0f);
	light[2].group = null;
	light[2].isOn = false;
}

Lights::~Lights()
{
	if(light)
	{
		for(long i = 0; i < numLights; i++) if(light[i].group) delete light[i].group;
		delete light;
	}
}

void Lights::AddAmbient(const CVECTOR & color)
{
	light[0].color = color;
	light[0].isOn = true;
}

void Lights::AddWeaterLights(const CVECTOR & color, const CVECTOR & dir)
{
	light[1].color = color;
	if(~dir > 0.0f) light[1].p = !dir;
	light[1].isOn = true;
	light[2].isOn = true;
}

void Lights::AddPointLight(const CVECTOR & color, const CVECTOR & pos, float att0, float att1, float att2, float range, const char * group)
{
	if(numLights > maxLights)
	{
		maxLights += 32;
		light = (Light *)RESIZE(light, maxLights*sizeof(Light));
	}
	SetDefLightParam(numLights);
	light[numLights].color = color;
	light[numLights].p = pos;
	light[numLights].att0 = att0;
	light[numLights].att1 = att1;
	light[numLights].att2 = att2;
	light[numLights].range = range;
	if(group && group[0])
	{
		light[numLights].group = NEW char[strlen(group) + 1];
		strcpy(light[numLights].group, group);
	}else light[numLights].group = null;
	light[numLights].type = Light::t_point;
	light[numLights].isOn = true;
	numLights++;
}

void Lights::SetDefLightParam(long i)
{
	light[i].cosine = 1.0f;
	light[i].shadow = 0.8f;
	light[i].bright = 0.5f;
	light[i].contr = 0.5f;
	light[i].gamma = 0.5f;
	light[i].isOn = false;
	light[i].isMark = false;
}

void Lights::PostInit()
{
	//Соберём все существующие группы
	char ** grp = NEW char * [numLights + 1];
	long numGrp = 0;
	for(long i = 0; i < numLights; i++)
	{
		if(!light[i].group) continue;
		for(long j = 0; j < numGrp; j++)
			if(stricmp(grp[j], light[i].group) == 0) break;
		if(j == numGrp) grp[numGrp++] = light[i].group;
	}
	//Добавляем груповые источники освещения
	if(numLights + numGrp > maxLights)
	{
		maxLights += numGrp + 4;
		light = (Light *)RESIZE(light, maxLights*sizeof(Light));
	}
	long num = numLights;
	for(i = 0; i < numGrp; i++)
	{
		memset(&light[numLights], 0, sizeof(light[numLights]));
		light[numLights].group = NEW char[strlen(grp[i]) + 1];
		strcpy(light[numLights].group, grp[i]);
		light[numLights].type = Light::t_group;
		light[numLights].isOn = true;
		//Собираем параметры
		float nrm = 0.0f;
		for(long j = 0; j < numLights; j++)
		{
			if(light[j].group && stricmp(light[j].group, grp[i]) == 0)
			{
				nrm += 1.0f;
				light[numLights].color += light[j].color;
				light[numLights].p += light[j].p;
				light[numLights].att0 += light[j].att0;
				light[numLights].att1 += light[j].att1;
				light[numLights].att2 += light[j].att2;
				light[numLights].cosine += light[j].cosine;
				light[numLights].shadow += light[j].shadow;
				light[numLights].bright += light[j].bright;
				light[numLights].contr += light[j].contr;
				light[numLights].gamma += light[j].gamma;
				light[numLights].curgm += light[j].curgm;
				light[numLights].curct += light[j].curct;
				light[numLights].range += light[j].range;
			}
		}
		Assert(nrm > 0.0f);
		nrm = 1.0f/nrm;
		light[numLights].color *= nrm;
		light[numLights].p *= nrm;
		light[numLights].att0 *= nrm;
		light[numLights].att1 *= nrm;
		light[numLights].att2 *= nrm;
		light[numLights].cosine *= nrm;
		light[numLights].shadow *= nrm;
		light[numLights].bright *= nrm;
		light[numLights].contr *= nrm;
		light[numLights].gamma *= nrm;
		light[numLights].curgm *= nrm;
		light[numLights].curct *= nrm;
		light[numLights].range *= nrm;
		numLights++;
	}
	delete grp;
}

void Lights::UpdateLights(long lit)
{
	long i;
	if(lit >= 0)
	{
		for(i = 0; i < numLights; i++) light[i].isMark = false;
	}else{
		for(i = 0; i < numLights; i++) light[i].isMark = true;
	}
	for(i = 0; i < numLights; i++)
	{
		
		if(light[i].type == Light::t_group)
		{
			for(long j = 0; j < numLights; j++)
			{
				if(light[j].type == Light::t_point && light[j].group)
				{
					if(stricmp(light[j].group, light[i].group) == 0)
					{
						light[j].color = light[i].color;
						light[j].cosine = light[i].cosine;
						light[j].shadow = light[i].shadow;
						light[j].bright = light[i].bright;
						light[j].contr = light[i].contr;
						light[j].gamma = light[i].gamma;
						light[j].curgm = light[i].curgm;
						light[j].curct = light[i].curct;
						light[j].att0 = light[i].att0;
						light[j].att1 = light[i].att1;
						light[j].att2 = light[i].att2;
						light[j].range = light[i].range;
						light[j].isOn = light[i].isOn;
						light[j].isMark |= (i == lit);
					}
				}
			}
		}else{
			if(i == lit) light[i].isMark = true;			
		}
	}
}
