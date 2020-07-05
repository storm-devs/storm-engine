//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	LightProcessor
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "LightProcessor.h"


#define LIGHTPRC_TRACE_NUM		1000
#define LIGHTPRC_SMOOTH_NUM		1000
#define LIGHTPRC_BLUR_NUM		500


//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

LightProcessor::LightProcessor()
{
	geometry = null;
	window = null;
	shadowTriangle = -1;
	smoothVertex = -1;
	blurVertex = -1;
}

LightProcessor::~LightProcessor()
{

}

void LightProcessor::SetParams(LGeometry * g, Window * win, Lights * lit, OctTree * ot, VDX8RENDER * _rs)
{
	geometry = g;
	window = win;
	lights = lit;
	rs = _rs;
	octtree = ot;
}

void LightProcessor::Process()
{
	if(shadowTriangle >= 0)
	{
		if(geometry->numTrg)
		{
			CalcShadows();
			window->tracePrc = shadowTriangle/float(geometry->numTrg);
		}else shadowTriangle = -1;		
		if(shadowTriangle == -1)
		{
			//Нормализуем результат
			Vertex * vrt = geometry->vrt;
			long numVrt = geometry->numVrt;
			long numLights = lights->Num();
			for(long i = 0; i < numVrt; i++)
			{
				for(long j = 0; j < numLights; j++)
				{
					if(vrt[i].shadow[j].nrm > 0.0)
					{
						vrt[i].shadow[j].v /= vrt[i].shadow[j].nrm;
					}else{
						vrt[i].shadow[j].v = 1.0;
					}
					vrt[i].shadow[j].sm = vrt[i].shadow[j].v;
				}
			}
			//Указываем что закончили
			window->isLockCtrl = false;
			window->tracePrc = 1.0f;
			CalcLights();
		}
		return;
	}
	if(smoothVertex >= 0)
	{
		SmoothShadows();
		if(smoothVertex == -1)
		{
			//Указываем что закончили
			window->isLockCtrl = false;
			window->smoothPrc = 1.0f;
			CalcLights();
		}else{
			if(geometry->numVrt) window->smoothPrc = smoothVertex/float(geometry->numVrt);
		}
	}
	if(blurVertex >= 0)
	{
		BlurLight();
		if(blurVertex == -1)
		{
			//Указываем что закончили
			window->isLockCtrl = false;
			window->blurPrc = 1.0f;
			CalcLights();
		}else{
			if(geometry->numVrt) window->blurPrc = blurVertex/float(geometry->numVrt);
		}
	}
	if(window->isTraceShadows)
	{
		//Подготавливаем
		shadowTriangle = 0;
		window->isTraceShadows = false;
		window->isLockCtrl = true;
		window->tracePrc = 0.0f;
		//Сбрасываем состояние цветов
		Vertex * vrt = geometry->vrt;
		long numVrt = geometry->numVrt;
		long numLights = lights->Num();
		for(long i = 0; i < numVrt; i++)
		{
			for(long j = 0; j < numLights; j++)
			{
				vrt[i].shadow[j].v = 0.0f;
				vrt[i].shadow[j].nrm = 0.0f;
				vrt[i].shadow[j].sm = 0.0f;
			}
		}
	}
	if(window->isSmoothShadows)
	{
		smoothVertex = 0;
		window->isSmoothShadows = false;
		window->isLockCtrl = true;
		window->smoothPrc = 0.0f;
	}
	if(window->isBlurLight)
	{
		CalcLights();
		blurVertex = 0;
		window->isBlurLight = false;
		window->isLockCtrl = true;
		window->blurPrc = 0.0f;
	}
	if(window->isResetBlurLight)
	{
		Vertex * vrt = geometry->vrt;
		long numVrt = geometry->numVrt;
		for(long i = 0; i < numVrt; i++) vrt[i].bc = 0.0f;
		window->isResetBlurLight = false;
		CalcLights();
	}
	if(window->isUpdateLight)
	{		
		CalcLights(window->updateLight, window->updateLightCos, window->updateLightAtt, window->updateLightSdw);
		window->updateLight = -1;
		window->updateLightCos = true;
		window->updateLightAtt = true;
		window->updateLightSdw = true;
		window->isUpdateLight = false;
	}
}


void LightProcessor::UpdateLightsParam()
{
	Vertex * vrt = geometry->vrt;
	long numVrt = geometry->numVrt;
	long numLights = lights->Num();
	Lights & ls = *lights;
	float cs, att;
	float dst;
	CVECTOR nrm;
	for(long i = 0; i < numVrt; i++)
	{
		Vertex & v = vrt[i];
		for(long j = 0; j < numLights; j++)
		{
			Light & lt = ls[j];
			Shadow & swh = v.shadow[j];
			switch(lt.type)
			{
			case Light::t_amb:
				swh.cs = 1.0f;
				swh.att = 1.0f;
				break;
			case Light::t_sun:
				cs = lt.p | v.n;
				if(cs < 0.0f) cs = 0.0f;
				swh.cs = cs;
				swh.att = 1.0f;
				break;
			case Light::t_sky:
				cs = v.n.y;
				if(cs < 0.0f) cs = 0.0f;
				swh.cs = v.n.y;
				swh.att = 1.0f;
				break;
			case Light::t_point:
				//Косинус угла
				nrm = lt.p - v.p;
				dst = sqrtf(~nrm);
				if(dst > 0.0f)
				{
					nrm *= 1.0f/dst;
					cs = nrm | v.n;
				}else cs = 1.0f;
				if(cs < 0.0f) cs = 0.0f;
				swh.cs = cs;
				//Коэфициент затухания
				if(dst < lt.range)
				{
					att = lt.att0 + dst*lt.att1 + dst*dst*lt.att2;
					if(att > 0.0f) att = 1.0f/att; else att = 0.0f;
				}else att = 0.0f;
				swh.cs = cs;
				swh.att = att;
				swh.dst = dst;
				break;
			}
			swh.csatt = float(swh.cs*swh.att);
			swh.shw = 1.0f;
		}
	}	
}

//Расчитать затенения
void LightProcessor::CalcShadows()
{
	//Вычисляем затенения
	for(long i = 0; i < LIGHTPRC_TRACE_NUM && shadowTriangle < geometry->numTrg; i++, shadowTriangle++) ApplyTriangleShadows(geometry->trg[shadowTriangle]);
	if(shadowTriangle == geometry->numTrg) shadowTriangle = -1;
}

//Распределить затенение с треугольника на вершины
void LightProcessor::ApplyTriangleShadows(Triangle & t)
{
	Lights & ls = *lights;
	long num = ls.Num();
	Vertex * vrt = geometry->vrt;
	for(long i = 0; i < num; i++)
	{
		//Нужно ли трейсить
		if(ls[i].type == Light::t_none || ls[i].type == Light::t_amb) continue;
		//Коэфициент нормирования
		vrt[t.i[0]].shadow[i].nrm += t.sq;
		vrt[t.i[1]].shadow[i].nrm += t.sq;
		vrt[t.i[2]].shadow[i].nrm += t.sq;
		//Точка откуда трейсить
		CVECTOR pnt = (vrt[t.i[0]].p + vrt[t.i[1]].p + vrt[t.i[2]].p)/3.0f;
		pnt += t.n*0.001f;
		//Определяем затенённость
		switch(ls[i].type)
		{
		case Light::t_sun:
			//Освещение от солнца
			if((ls[i].p | t.n) >= 0.0f)
			{
				if(geometry->Trace(pnt, pnt + ls[i].p*geometry->radius) > 1.0f)
				{
					vrt[t.i[0]].shadow[i].v += t.sq;
					vrt[t.i[1]].shadow[i].v += t.sq;
					vrt[t.i[2]].shadow[i].v += t.sq;
				}
			}
			break;
		case Light::t_sky:
			//Освещение от неба
			if(t.n.y >= 0.0f)
			{
				float sky = 0.0;
				float rad = geometry->radius;
				float rdx = geometry->radius*0.2f;
				if(geometry->Trace(pnt, pnt + CVECTOR(0.0f, rad, 0.0f)) > 1.0f) sky += 1.0f/5.0f;
				if(geometry->Trace(pnt, pnt + CVECTOR(rdx, rad, 0.0f)) > 1.0f) sky += 1.0f/5.0f;
				if(geometry->Trace(pnt, pnt + CVECTOR(-rdx, rad, 0.0f)) > 1.0f) sky += 1.0f/5.0f;
				if(geometry->Trace(pnt, pnt + CVECTOR(0.0f, rad, rdx)) > 1.0f) sky += 1.0f/5.0f;
				if(geometry->Trace(pnt, pnt + CVECTOR(0.0f, rad, -rdx)) > 1.0f) sky += 1.0f/5.0f;
				sky *= t.sq;
				vrt[t.i[0]].shadow[i].v += sky;
				vrt[t.i[1]].shadow[i].v += sky;
				vrt[t.i[2]].shadow[i].v += sky;
			}
			break;
		case Light::t_point:
			//Освещение от солнца
			if(((ls[i].p - pnt) | t.n) >= 0.0f)
			{
				if(geometry->Trace(pnt, ls[i].p) > 1.0f)
				{
					vrt[t.i[0]].shadow[i].v += t.sq;
					vrt[t.i[1]].shadow[i].v += t.sq;
					vrt[t.i[2]].shadow[i].v += t.sq;
				}
			}
			break;
		default:
			vrt[t.i[0]].shadow[i].v += t.sq;
			vrt[t.i[1]].shadow[i].v += t.sq;
			vrt[t.i[2]].shadow[i].v += t.sq;
		}
	}
}



//Сгладить затенённость
void LightProcessor::SmoothShadows()
{
	bool lookNorm = window->smoothNorm;
	float smoothRad = window->smoothRad;
	float kSmoothRad = 1.0f/smoothRad;
	double smoothRad2 = smoothRad*smoothRad;
	Lights & ls = *lights;
	long num = ls.Num();
	Vertex * vrt = geometry->vrt;
	for(long i = 0; i < LIGHTPRC_SMOOTH_NUM && smoothVertex < geometry->numVrt; i++, smoothVertex++)
	{
		Vertex & v = vrt[smoothVertex];
		//Ищем окружающие вершины
		octtree->FindVerts(v.p, smoothRad);
		OctFndVerts * verts = octtree->verts;
		long numVerts = octtree->numVerts;
		
		if(false)
		{
			float r2 = smoothRad*smoothRad;
			static Vertex * ov[16384];
			long numV = 0;
			long num = geometry->numVrt;
			for(long n = 0; n < num; n++)
			{
				float r = ~(vrt[n].p - v.p);
				if(r < r2)
				{
					Assert(numV < sizeof(ov)/sizeof(OctFndVerts));
					ov[numV++] = vrt + n;
				}
			}
			if(numVerts != numV) api->Trace("numVerts(%i) != numV(%i)", numVerts, numV);
			for(n = 0; n < numVerts; n++)
			{
				for(long k = 0; k < numV; k++)
					if(verts[n].v == ov[k]) break;
				if(k >= numV) api->Trace("k >= numV");
			}
		}
//		Assert(numVerts > 0);
		//Проходимся по всем источникам
		for(long n = 0; n < num; n++)
		{
			//Обнуляем
			double sm = 0.0;
			double kNorm = 0.0f;
			//По всем вершинам
			for(long j = 0; j < numVerts; j++)
			{
				if(lookNorm && (v.n | verts[j].v->n) <= 0.6f) continue;
				double k = sqrt(verts[j].r2)*kSmoothRad;
				if(k < 0.0) k = 0.0;
				if(k > 1.0) k = 1.0;
				k = 1.0 - k;
				sm += verts[j].v->shadow[n].v*k;
				kNorm += k;
			}
			if(kNorm > 0.0) sm /= kNorm; else sm = v.shadow[n].v;
			v.shadow[n].sm = sm;
		}
	}
	if(smoothVertex >= geometry->numVrt) smoothVertex = -1;
}


//Сгладить освещение
void LightProcessor::BlurLight()
{
	bool isTrace = window->isTraceBlur;
	float blurRad = window->blurRad;
	float kBlurRad = 1.0f/blurRad;
	float pw = window->blurAtt;
	float kCos = window->blurCos;
	float kCos1 = 1.0f - window->blurCos;
	double blurRad2 = blurRad*blurRad;
	Vertex * vrt = geometry->vrt;
	long numVrt = geometry->numVrt;
	for(long i = 0; i < LIGHTPRC_BLUR_NUM && blurVertex < numVrt; i++, blurVertex++)
	{
		Vertex & v = vrt[blurVertex];
		//Ищем окружающие вершины
		octtree->FindVerts(v.p, blurRad);
		OctFndVerts * verts = octtree->verts;
		long numVerts = octtree->numVerts;

		continue;

		long step = (numVerts + 8) >> 4;
		if(step < 1) step = 1;
		step = 1;
		double r = 0.0, g = 0.0, b = 0.0, sum = 0.0;
		//По всем вершинам
		for(long j = 0; j < numVerts; j += step)
		{
			Vertex & vs = *verts[j].v;
			if(vs.c.x + vs.c.y + vs.c.z <= 0.0f) continue;
			CVECTOR n = vs.p - v.p;
			double css = -(n | vs.n);
			if(css <= 0.0) continue;
			double csd = n | v.n;
			if(csd <= 0.0) continue;
			double dst = sqrt(~n);
			if(dst <= 0.0) continue;
			double k = dst*kBlurRad;
			dst = 1.0f/dst;
			n *= float(dst*0.001);
			if(isTrace && geometry->Trace(v.p + n, vs.p - n) <= 1.0f) continue;
			css *= dst; csd *= dst;			
			if(css > 1.0) css = 1.0;
			if(csd > 1.0) csd = 1.0;
			if(k <= 0.0) continue;
			if(k > 1.0) k = 1.0;
			k = powf(1.0 - k, pw)*(css*csd*kCos + kCos1);
			r += vs.c.x*k;
			g += vs.c.y*k;
			b += vs.c.z*k;
			sum += 1.0f;
		}
		if(sum > 0.0){ sum = 1.0/sum; r *= sum; g *= sum; b *= sum; }
		double max = r > g ? r : g;
		if(max < b) max = b;
		max = 1.0;
		if(max > 0.0){ max = 1.0/max; r *= max; g *= max; b *= max; }
		v.bc.x = float(r);
		v.bc.y = float(g);
		v.bc.z = float(b);
	}
	if(blurVertex >= geometry->numVrt) blurVertex = -1;
}


//Расчитать освещение
void LightProcessor::CalcLights(long lit, bool isCos, bool isAtt, bool isSdw)
{
	lights->UpdateLights(lit);
	Lights & ls = *lights;
	long num = ls.Num();
	Vertex * vrt = geometry->vrt;
	float kBlur = window->kBlur;
	CVECTOR c, nrm;
	for(long i = 0; i < num; i++)
	{
		if(!ls[i].isOn) continue;
		if(ls[i].type != Light::t_sun && ls[i].type != Light::t_sky && ls[i].type != Light::t_point) continue;
		ls[i].curgm = ls[i].gamma >= 0.5f ? 1.0f + (ls[i].gamma - 0.5f)*12.0f : ls[i].gamma + 0.5f;
		ls[i].curgm = 1.0f/ls[i].curgm;
		ls[i].curct = ls[i].contr >= 0.5f ? 1.0f + (ls[i].contr - 0.5f)*20.0f : 0.05f + ls[i].contr*2.0f*0.95f;
	}
	for(long n = 0; n < geometry->numVrt; n++)
	{
		Vertex & v = vrt[n];
		if(!v.shadow) continue;
		c = v.bc*(kBlur*kBlur*2.0f);		
		float sw;
		double vl;
		for(long i = 0; i < num; i++)
		{
			Light & lt = ls[i];
			if(!lt.isOn) continue;
			Shadow & shw = v.shadow[i];
			if(!lt.isMark){ c += shw.c; continue; }
			switch(lt.type)
			{
			case Light::t_amb:
				shw.c = lt.color;
				c += shw.c;
				break;
			case Light::t_sun:
				//Косинус угла
				if(isCos)
				{
					shw.csatt = lt.cosine*shw.cs + (1.0f - lt.cosine);
				}
				//Коэфициент затенения
				if(isSdw)
				{
					vl = (v.shadow[i].sm - 0.5)*lt.curct + 0.5;
					if(vl < 0.0f) vl = 0.0f;
					if(vl > 1.0f) vl = 1.0f;
					sw = float(pow(vl, double(lt.curgm))) + (lt.bright - 0.5f)*1.8f;
					if(sw < 0.0f) sw = 0.0f;
					if(sw > 1.0f) sw = 1.0f;
					shw.shw = lt.shadow*sw + (1.0f - lt.shadow);
				}
				//Результирующий цвет
				shw.c = lt.color*(shw.csatt*shw.shw);
				c += shw.c;
				break;
			case Light::t_sky:
				//Косинус угла
				if(isCos)
				{
					shw.csatt = lt.cosine*shw.cs + (1.0f - lt.cosine);
				}
				//Коэфициент затенения
				if(isSdw)
				{
					vl = (v.shadow[i].sm - 0.5)*lt.curct + 0.5;
					if(vl < 0.0f) vl = 0.0f;
					if(vl > 1.0f) vl = 1.0f;
					sw = float(pow(vl, double(lt.curgm))) + (lt.bright - 0.5f)*1.8f;
					if(sw < 0.0f) sw = 0.0f;
					if(sw > 1.0f) sw = 1.0f;
					shw.shw = lt.shadow*sw + (1.0f - lt.shadow);
				}
				//Результирующий цвет
				shw.c = lt.color*(shw.csatt*shw.shw);
				c += shw.c;
				break;
			case Light::t_point:
				//Коэфициент затухания
				if(isAtt)
				{
					if(shw.dst < lt.range)
					{
						shw.att = lt.att0 + shw.dst*lt.att1 + shw.dst*shw.dst*lt.att2;
						if(shw.att > 0.0f) shw.att = 1.0f/shw.att; else shw.att = 0.0f;
					}else shw.att = 0.0f;
				}
				//Косинус угла
				if(isCos || isAtt)
				{
					shw.csatt = (lt.cosine*shw.cs + (1.0f - lt.cosine))*shw.att;
				}
				//Коэфициент затенения
				if(isSdw)
				{
					vl = (v.shadow[i].sm - 0.5)*lt.curct + 0.5;
					if(vl < 0.0f) vl = 0.0f;
					if(vl > 1.0f) vl = 1.0f;
					sw = float(pow(vl, double(lt.curgm))) + (lt.bright - 0.5f)*1.8f;
					if(sw < 0.0f) sw = 0.0f;
					if(sw > 1.0f) sw = 1.0f;
					shw.shw = lt.shadow*sw + (1.0f - lt.shadow);
				}
				//Результирующий цвет
				shw.c = lt.color*(shw.csatt*shw.shw);
				c += shw.c;
				break;
			}
		}
		v.c = c;
	}
	geometry->UpdateColors(rs);
}
