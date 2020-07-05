//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	LGeometry
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "LGeometry.h"
#include <io.h>


//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

LGeometry::LGeometry()
{
	object = null;
	numObjects = 0;
	maxObjects = 0;
	vrt = null;
	numVrt = 0;
	maxVrt = 0;
	trg = null;
	numTrg = 0;
	maxTrg = 0;
	vbuffer = null;
	numVBuffers = 0;
	maxVBuffers = 0;
	shadows = null;
	drawbuf = null;
	radius = 0.0f;
	modelsPath[0] = 0;
	lightPath[0] = 0;
}

LGeometry::~LGeometry()
{
	if(object)
	{
		for(long i = 0; i < numObjects; i++)
		{
			delete object[i].name;
			delete object[i].nameReal;
		}
		delete object;
	}
	if(vrt) delete vrt;	
	if(trg) delete trg;
	if(vbuffer) delete vbuffer;
	if(shadows) delete shadows;
	if(drawbuf) delete drawbuf;
}

//Установить путь до моделек
void LGeometry::SetModelsPath(const char * mPath)
{
	strcpy(modelsPath, mPath);
}

//Установить путь для текущей погоды
void LGeometry::SetLightPath(const char * lPath)
{
	strcpy(lightPath, lPath);
}

//Добавить объект
void LGeometry::AddObject(const char * name, ENTITY_ID & model)
{
	if(numObjects >= maxObjects)
	{
		maxObjects += 16;
		object = (Object *)RESIZE(object, maxObjects*sizeof(Object));
	}
	object[numObjects].nameReal = NEW char[strlen(name) + strlen(modelsPath) + 8];
	object[numObjects].nameReal[0] = 0;
	strcat(object[numObjects].nameReal, modelsPath);
	strcat(object[numObjects].nameReal, name);
	strcat(object[numObjects].nameReal, ".gm");
	object[numObjects].name = NEW char[strlen(name) + 2048];
	object[numObjects].name[0] = 0;
	strcat(object[numObjects].name, "resource\\models\\");
	strcat(object[numObjects].name, modelsPath);
	strcat(object[numObjects].name, "\\");
	strcat(object[numObjects].name, name);
	strcat(object[numObjects].name, "_");
	strcat(object[numObjects].name, lightPath);
	strcat(object[numObjects].name, ".col");
	char * str = object[numObjects].name;
	for(long s = 0, d = 0; str[d]; s++)
	{
		if(str[s] >= 'a' && str[s] <= 'z') str[s] -= 'a' - 'A';
		if(d > 0 && str[d - 1] == '\\' && str[s] == '\\') continue;
		str[d++] = str[s];
	}
	object[numObjects].model = model;
	object[numObjects].m = (MODEL *)api->GetEntityPointer(&model);
	if(!object[numObjects].m)
	{
		api->Trace("Location lighter: can't get pointer to model %s", name);
	}else numObjects++;
}


//Обработать данные
bool LGeometry::Process(VDX8RENDER * rs, long numLights)
{
	//Подготовка данных для освещения
	for(long i = 0; i < numObjects; i++)
	{
		//Вершины--------------------------------------------------------------------------------
		//Индекс в конечном файле
		long cindex = 0;
		//Проверочка
		if(object[i].m != (MODEL *)api->GetEntityPointer(&object[i].model))
		{
			api->Trace("Location lighter: lost model!!!");
			return false;
		}
		if(object[i].m->GetNode(1))
		{
			api->Trace("Location lighter: incorrent model %s (nodes above 1)", object[i].nameReal);
			return false;
		}
		//Пересчитать матрицы
		object[i].m->Update();
		//Получаем геометрию		
		NODE * node = object[i].m->GetNode(0);
		GEOS * g = node->geo;
		if(!g)
		{
			api->Trace("Location lighter: incorrent model %s (node not include geos)", object[i].nameReal);
			return false;
		}
		//Информация о геометрии
		GEOS::INFO info;
		g->GetInfo(info);
		if(info.nvrtbuffs <= 0)
		{
			api->Trace("Location lighter: incorrent model %s (not vertex buffers), skip it", object[i].nameReal);
			object[i].lBufSize = 0;
			continue;
		}
		if(numVBuffers + info.nvrtbuffs > maxVBuffers)
		{
			maxVBuffers += info.nvrtbuffs + 16;
			vbuffer = (VertexBuffer *)RESIZE(vbuffer, maxVBuffers*sizeof(VertexBuffer));
		}
		for(long vb = 0; vb < info.nvrtbuffs; vb++)
		{
			long vbID = g->GetVertexBuffer(vb);
			if(vbID < 0) continue;
			vbuffer[numVBuffers].vbID = vbID;
			vbuffer[numVBuffers++].start = numVrt;
			//Получаем вершины			
			IDirect3DVertexBuffer8 * vbuf = rs->GetVertexBuffer(vbID);
			D3DVERTEXBUFFER_DESC desc;
			if(!vbuf || vbuf->GetDesc(&desc) != D3D_OK)
			{
				api->Trace("Location lighter: vertex buffer error, model %s, vbID %i", object[i].nameReal, vbID);
				return false;
			}
			//Анализируем тип
			bool isEnabledType = true;
			isEnabledType &= ((desc.FVF & D3DFVF_POSITION_MASK) == D3DFVF_XYZ);
			isEnabledType &= ((desc.FVF & D3DFVF_NORMAL) != 0);
			isEnabledType &= ((desc.FVF & D3DFVF_DIFFUSE) != 0);
			isEnabledType &= ((desc.FVF & D3DFVF_PSIZE) == 0);			
			if(!isEnabledType)
			{
				api->Trace("Location lighter: incorrect fvf of vertex buffer, model %s, vbID %i", object[i].nameReal, vbID);
				return false;
			}
			//Размер вершины
			long stride = 6*sizeof(float) + sizeof(dword);
			stride += ((desc.FVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT)*2*sizeof(float);
			if(desc.FVF & D3DFVF_SPECULAR) stride += sizeof(dword);
			//Количество вершин
			long num = desc.Size/stride;
			if(num <= 0)
			{
				api->Trace("Location lighter: incorrect number of verteces in vertex buffer, model %s, vbID %i", object[i].nameReal, vbID);
				return false;
			}
			//Резервируем место
			if(numVrt + num > maxVrt)
			{
				maxVrt = numVrt + num + 64;
				vrt = (Vertex *)RESIZE(vrt, maxVrt*sizeof(Vertex));
			}
			//Копируем
			byte * pnt = null;
			if(vbuf->Lock(0, desc.Size, &pnt, 0) != D3D_OK)
			{
				api->Trace("Location lighter: vertex buffer no locked, model %s, vbID %i", object[i].nameReal, vbID);
				return false;
			}
			for(long v = 0; v < num; v++)
			{
				CVECTOR * pos = (CVECTOR *)(pnt + v*stride);
				vrt[numVrt].p = *pos;
				CVECTOR * nrm = (CVECTOR *)(pnt + v*stride + 3*sizeof(float));
				vrt[numVrt].n = *nrm;
				dword color = *(dword *)(pnt + v*stride + 6*sizeof(float));
				float l = ~vrt[numVrt].n;
				if(l > 0.0f)
				{
					if(l != 1.0f) vrt[numVrt].n *= 1.0f/sqrtf(l);
				}else{
					api->Trace("Location lighter: model %s, vbID %i, vrt: %i : normal have zero length", object[i].nameReal, vbID, v);
				}
				vrt[numVrt].c = 0.0f;
				vrt[numVrt].bc = 0.0f;
				vrt[numVrt].mc = 1.0f;
				vrt[numVrt].alpha = 0xff000000;
				vrt[numVrt].flags = Vertex::f_zero;
				vrt[numVrt].vbid = vbID;
				vrt[numVrt].shadow = null;
				vrt[numVrt].addr = v*stride + 6*sizeof(float);
				vrt[numVrt].obj = i;
				vrt[numVrt].cindex = cindex++;
				if(useColor)
				{
					vrt[numVrt].mc.x = ((color >> 16) & 255)*(1.0f/255.0f)*2.0f;
					vrt[numVrt].mc.y = ((color >> 8) & 255)*(1.0f/255.0f)*2.0f;
					vrt[numVrt].mc.z = ((color >> 0) & 255)*(1.0f/255.0f)*2.0f;
					vrt[numVrt].alpha = color & 0xff000000;
				}
				numVrt++;
			}
			vbuf->Unlock();
		}
		object[i].lBufSize = cindex;
		//Треугольники--------------------------------------------------------------------------------
		long ibID = g->GetIndexBuffer();
		word * idx = (word *)rs->LockIndexBuffer(ibID);
		if(!idx)
		{
			api->Trace("Location lighter: index buffer no locked, model %s", object[i].nameReal);
			return false;
		}
		GEOS::OBJECT obj;
		for(long n = 0; n < info.nobjects; n++)
		{
			g->GetObj(n, obj);
			//Ищем вертексбуфер
			for(vb = 0; vb < numVBuffers; vb++)
				if(vbuffer[vb].vbID == long(obj.vertex_buff)) break;
			if(vb >= numVBuffers)
			{
				api->Trace("Location lighter: vertex buffer %i not found, model %s", obj.vertex_buff, object[i].nameReal);
				return false;
			}
			vb = vbuffer[vb].start + obj.start_vertex;
			//Читаем треугольники
			word * triangles = idx + obj.striangle*3;
			for(long t = 0; t < obj.ntriangles; t++)
			{
				//Относительные индексы
				long i1 = triangles[t*3 + 0];
				long i2 = triangles[t*3 + 1];
				long i3 = triangles[t*3 + 2];
				if(i1 >= obj.num_vertices || i2 >= obj.num_vertices || i3 >= obj.num_vertices)
				{
					api->Trace("Location lighter: model %s have incorrect vertex index, (obj: %i, trg: %i)", object[i].nameReal, n, t);
					return false;
				}
				//Абсолютные индексы
				i1 += vb; i2 += vb; i3 += vb;
				//Нормаль к треугольнику
				Assert(i1 >= 0 && i1 < numVrt);
				Assert(i2 >= 0 && i2 < numVrt);
				Assert(i3 >= 0 && i3 < numVrt);
				CVECTOR nrm = ((vrt[i2].p - vrt[i1].p) ^ (vrt[i3].p - vrt[i1].p));
				float sq = sqrtf(~nrm);
				//Пропустим пустой треугольник
				if(sq <= 0.0f)
				{
					api->Trace("Location lighter: model %s have zero triangle, (obj: %i, trg: %i)", object[i].nameReal, n, t);
					continue;
				}
				//Добавим треугольник
				if(numTrg >= maxTrg)
				{
					maxTrg += 256;
					trg = (Triangle *)RESIZE(trg, maxTrg*sizeof(Triangle));
				}
				trg[numTrg].n = nrm*(1.0f/sq);
				trg[numTrg].sq = sq;
				trg[numTrg].i[0] = i1;
				trg[numTrg].i[1] = i2;
				trg[numTrg].i[2] = i3;
				for(long nv = 0; nv < 3; nv++)
				{
					Vertex & vr = vrt[trg[numTrg].i[nv]];
					bool isInv = (trg[numTrg].n | vr.n) < 0.0f;
					if(vr.flags & Vertex::f_set)
					{
						if(((vr.flags & Vertex::f_inv) != 0) != isInv)
						{
							api->Trace("Location lighter: model %s have bug normals, (obj: %i, trg: %i)", object[i].nameReal, n, t);
							vr.flags |= Vertex::f_bug;
						}
					}else{
						if(isInv) vr.flags |= Vertex::f_inv;
					}
				}
				numTrg++;
			}
		}
	}
	if(numVrt <= 0) return false;
	shadows = NEW Shadow[numVrt*numLights];
	memset(shadows, 0, numVrt*numLights*sizeof(Shadow));
	min = vrt[0].p;
	max = vrt[0].p;
	long lghtpnt = 0;
	for(i = 0; i < numVrt; i++)
	{
		Vertex & v = vrt[i];
		v.shadow = shadows + lghtpnt;
		lghtpnt += numLights;
		if(v.p.x < min.x) min.x = v.p.x;
		if(v.p.y < min.y) min.y = v.p.y;
		if(v.p.z < min.z) min.z = v.p.z;
		if(v.p.x > max.x) max.x = v.p.x;
		if(v.p.y > max.y) max.y = v.p.y;
		if(v.p.z > max.z) max.z = v.p.z;
		if(v.flags & Vertex::f_inv)
		{
			if((v.flags & Vertex::f_bug) == 0)
			{
				v.n.x = -v.n.x;
				v.n.y = -v.n.y;
				v.n.z = -v.n.z;
			}
		}
	}
	radius = sqrtf(~(max - min));
	return true;
}

//Нарисовать нормали
void LGeometry::DrawNormals(VDX8RENDER * rs)
{
	if(!drawbuf) drawbuf = NEW CVECTOR[1024];
	rs->SetRenderState(D3DRS_TEXTUREFACTOR, 0xff00ff00);
	for(long i = 0, p = 0; i < numVrt; i++)
	{
		drawbuf[p + 0] = vrt[i].p;
		drawbuf[p + 1] = vrt[i].p + vrt[i].n;
		p += 2;
		if(p >= 1024)
		{
			rs->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZ, 512, drawbuf, sizeof(CVECTOR), "DbgDrawLines");
			p = 0;
		}
	}
	if(p > 1)
	{
		rs->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZ, p/2, drawbuf, sizeof(CVECTOR), "DbgDrawLines");
	}
}

//Обновить цвета в буферах
void LGeometry::UpdateColors(VDX8RENDER * rs)
{
	long lockedVB = -1;
	byte * pnt = null;
	for(long i = 0; i < numVrt; i++)
	{
		if(vrt[i].vbid != lockedVB)
		{
			if(lockedVB >= 0) rs->UnLockVertexBuffer(lockedVB);
			lockedVB = -1;
			pnt = (byte *)rs->LockVertexBuffer(vrt[i].vbid);
			if(!pnt)
			{
				api->Trace("Location lighter: no lock vertex buffer for update color");
				continue;
			}
			lockedVB = vrt[i].vbid;
		}
		CVECTOR c = vrt[i].c*vrt[i].mc*255.0f;
		if(c.x < 0.0f) c.x = 0.0f;
		if(c.x > 255.0f) c.x = 255.0f;
		if(c.y < 0.0f) c.y = 0.0f;
		if(c.y > 255.0f) c.y = 255.0f;
		if(c.z < 0.0f) c.z = 0.0f;
		if(c.z > 255.0f) c.z = 255.0f;
		dword & clr = *(dword *)(pnt + vrt[i].addr);
		clr = (dword(c.x) << 16) | (dword(c.y) << 8) | (dword(c.z) << 0) | vrt[i].alpha;
	}
	if(lockedVB >= 0) rs->UnLockVertexBuffer(lockedVB);
}

//Протрейсить луч
float LGeometry::Trace(CVECTOR & src, CVECTOR & dst)
{
	for(long i = 0; i < numObjects; i++)
	{
		float res = object[i].m->Trace(src, dst);
		if(res <= 1.0f) return res;
	}
	return 2.0f;
}

//Сохранить освещение
bool LGeometry::Save()
{
	//Сохраняем текущий путь
	char * oldPath = NEW char[4096];
	::GetCurrentDirectory(4096, oldPath);
	char * dir = NEW char[4096];
	//Сохраняем объекты
	bool result = true;
	long bufSize = 16384;
	dword * buf = NEW dword[bufSize];
	for(long i = 0, pnt = 0; i < numObjects; i++)
	{	
		if(object[i].lBufSize <= 0) continue;
		//Создаём путь
		::SetCurrentDirectory(oldPath);
		bool isCont = false;
		for(long c = 0, p = 0; true; c++, p++)
		{
			dir[p] = object[i].name[c];
			if(dir[p] == '\\')
			{
				dir[p] = 0;
				if(_access(dir, 0) == -1)
				{
					if(!CreateDirectory(dir, null))
					{
						isCont = true;
						break;
					}
				}
				::SetCurrentDirectory(dir);
				p = -1;
				continue;
			}
			if(dir[p] == 0) break;
		}
		if(isCont) continue;
		FILE * fl = fopen(dir, "w+b");
		if(!fl)
		{
			result = false;
			continue;
		}
		long sv = 0;
		for(long j = 0, n = object[i].lBufSize; j < n; j++)
		{
			CVECTOR c = vrt[pnt].c*vrt[pnt].mc*255.0f; pnt++;
			if(c.x < 0.0f) c.x = 0.0f;
			if(c.x > 255.0f) c.x = 255.0f;
			if(c.y < 0.0f) c.y = 0.0f;
			if(c.y > 255.0f) c.y = 255.0f;
			if(c.z < 0.0f) c.z = 0.0f;
			if(c.z > 255.0f) c.z = 255.0f;
			buf[sv++] = (dword(c.x) << 16) | (dword(c.y) << 8) | (dword(c.z) << 0) | 0xff000000;
			if(sv >= bufSize)
			{
				result = (fwrite(buf, sv*sizeof(dword), 1, fl) == 1);
				sv = 0;
			}
		}
		if(sv > 0) result &= (fwrite(buf, sv*sizeof(dword), 1, fl) == 1);
		fclose(fl);
	}
	::SetCurrentDirectory(oldPath);
	delete oldPath;
	delete dir;
	delete buf;
	return result;
}


