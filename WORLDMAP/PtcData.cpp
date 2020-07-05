//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	PtcData
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "PtcData.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\matrix.h"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

PtcData::PtcData()
{
	srand(GetTickCount());
	data = null;	
	triangle = null;
	numTriangles = 0;	
	vertex = null;
	numVerteces = 0;	
	normal = null;
	numNormals = 0;	
	min = max = 0.0f;	
	map = null;
	l = w = 0;
	ls = ws = 0.0f;	
	indeces = null;
	numIndeces = 0;	
	table = null;
	lineSize = 0;
	ctriangle = null;
	numClTriangles = 0;
	maxClTriangles = 0;
	dbgTriangles = null;
	dbgEdges = null;
	numSteps = 0;
	materials = null;
	middle = 0.0f;
}

PtcData::~PtcData()
{
	if(data) delete data;
	if(ctriangle) delete ctriangle;
	if(dbgTriangles) delete dbgTriangles;
	if(dbgEdges) delete dbgEdges;
}

bool PtcData::Load(const char * path)
{
	Assert(data == null);
	char * buf = null;
	dword size = 0;
	middle = 0.0f;
	//Загружаем данные
	if(!api->fio->LoadFile(path, &buf, &size))
	{
		api->Trace("Ptc(\"%s\") -> file not found", path);
		return false;
	}
	//Проверяем файл на корректность
	if(!buf || size < sizeof(PtcHeader))
	{
		api->Trace("Ptc(\"%s\") -> invalide file size", path);
		if(buf) delete buf;
		return false;
	}
	PtcHeader & hdr = *(PtcHeader *)buf;
	if(hdr.id != PTC_ID)
	{
		api->Trace("Ptc(\"%s\") -> invalide file ID", path);
		delete buf;
		return false;
	}
	if(hdr.ver != PTC_VERSION && hdr.ver != PTC_PREVERSION1)
	{
		api->Trace("Ptc(\"%s\") -> invalide file version", path);
		delete buf;
		return false;
	}
	dword tsize = sizeof(PtcHeader);
	tsize += hdr.numTriangles*sizeof(PtcTriangle);
	tsize += hdr.numVerteces*sizeof(PtcVertex);
	tsize += hdr.numNormals*sizeof(PtcNormal);
	tsize += hdr.mapL*hdr.mapW*sizeof(PtcMap);
	tsize += hdr.numIndeces*sizeof(word);
	tsize += hdr.lineSize*hdr.numTriangles*sizeof(byte);
	if(hdr.ver == PTC_VERSION) tsize += sizeof(PtcMaterials);
	if(tsize != size)
	{
		api->Trace("Ptc(\"%s\") -> invalide file size", path);
		delete buf;
		return false;
	}
	if(hdr.numTriangles < 1 || 
		hdr.numVerteces < 3 ||
		hdr.numNormals < 1 ||
		hdr.mapL < 1 ||
		hdr.mapW < 1 ||
		hdr.numIndeces < 1 ||
		hdr.lineSize < 1 ||
		hdr.minX >= hdr.maxX ||
		hdr.minY > hdr.maxY ||
		hdr.minZ >= hdr.maxZ)
	{
		api->Trace("Ptc(\"%s\") -> invalide file header", path);
		delete buf;
		return false;
	}
	//Формируем структуры данных
	data = buf;
	SFLB_PotectionLoad();
	return true;
}

//Функция защиты
#ifndef _XBOX
void __declspec(noinline) __cdecl PtcData::SFLB_PotectionLoad()
#else
void PtcData::SFLB_PotectionLoad()
#endif
{
	//Данные
	char * buf = (char *)data;
	PtcHeader & hdr = *(PtcHeader *)buf;
	//Треугольники
	dword tsize = sizeof(PtcHeader);
	triangle = (PtcTriangle *)(buf + tsize);
	numTriangles = hdr.numTriangles;
	//Вершины
	tsize += hdr.numTriangles*sizeof(PtcTriangle);
	vertex = (PtcVertex *)(buf + tsize);
	numVerteces = hdr.numVerteces;
	//Нормали
	tsize += hdr.numVerteces*sizeof(PtcVertex);
	normal = (PtcNormal *)(buf + tsize);
	numNormals = hdr.numNormals;
	//Карта для коллижена
	tsize += hdr.numNormals*sizeof(PtcNormal);
	map = (PtcMap *)(buf + tsize);
	l = hdr.mapL;
	w = hdr.mapW;
	min.x = hdr.minX; min.y = hdr.minY; min.z = hdr.minZ;
	max.x = hdr.maxX; max.y = hdr.maxY; max.z = hdr.maxZ;
	ls = (max.z - min.z)/l;
	ws = (max.x - min.x)/w;
	tsize += hdr.mapL*hdr.mapW*sizeof(PtcMap);
	indeces = (word *)(buf + tsize);
	//Данные для нахождения путей
	tsize += hdr.numIndeces*sizeof(word);
	table = (byte *)(buf + tsize);
	lineSize = hdr.lineSize;
	//Материалы
	if(hdr.ver == PTC_VERSION) materials = (PtcMaterials *)(table + lineSize*numTriangles);	
	//Ищим среднюю точку
	middle = 0.0f;
	for(long i = 0; i < numVerteces; i++)
	{
		middle.x += vertex[i].x;
		middle.y += vertex[i].y;
		middle.z += vertex[i].z;
	}
	if(numVerteces) middle *= 1.0f/numVerteces;
}

//Определить текущию позицию
long PtcData::FindNode(const CVECTOR & pos, float & y)
{
	//Позиция на карте
	long mapX = long((pos.x - min.x)/ws);
	long mapZ = long((pos.z - min.z)/ls);
	if(mapX < 0 || mapX >= w) return -1;
	if(mapZ < 0 || mapZ >= l) return -1;
	//Перебираем треугольники
	PtcMap & m = map[mapZ*w + mapX];
	long node = -1;
	float h = 0.0f;
	float dist = -1.0f;
	for(long i = 0; i < m.size; i++)
	{
		PtcTriangle & trg = triangle[indeces[m.start + i]];
		//Проверяем поподание в треугольник
		for(long j = 0; j < 3; j++)
		{
			//Вершины ребра
			CVECTOR & vs = *(CVECTOR *)&vertex[trg.i[j]];
			CVECTOR & ve = *(CVECTOR *)&vertex[trg.i[j == 2 ? 0 : j + 1]];
			//Нормаль ребра
			float nx = -(ve.z - vs.z);
			float nz = (ve.x - vs.x);
			//Если за пределами треугольника прервать
			if(pos.x*nx + pos.z*nz > vs.x*nx + vs.z*nz) break;
		}
		if(j == 3)
		{
			//Проверим высоту
			float d = FindHeight(indeces[m.start + i], pos.x, pos.z);
			if(dist >= 0.0f)
			{
				if(dist > fabsf(pos.y - d))
				{
					dist = fabsf(pos.y - d);
					h = d;
					node = indeces[m.start + i];
				}
			}else{
				dist = fabsf(pos.y - d);
				h = d;
				node = indeces[m.start + i];
			}
		}		
	}
	y = h;
	return node;
}

//Двирает pos в to, возвращает новый нод
long PtcData::Move(long curNode, const CVECTOR & to, CVECTOR & pos, long depth)
{
	isSlide = false;
	isBearing = false;
	if(curNode < 0) return -1;
	Assert(curNode < numTriangles);
	//Направление перемещения в 2D
	CVECTOR dir(to.x - pos.x, 0.0f, to.z - pos.z);
	CVECTOR ps = pos;
	long firstNode = curNode;
	//Длинна пути в 2D
	float dst = ~dir;
	if(dst == 0.0f || depth > 32)
	{
		if(to.y != pos.y)
		{
			float hy = FindHeight(curNode, pos.x, pos.z);
			pos.y = to.y;
			if(pos.y < hy) pos.y = hy;
		}
		return curNode;
	}
	dst = sqrtf(dst);
	dir *= 1.0f/dst;
	//Вертикальная плоскость, проходящая через отрезок перемещения
	CVECTOR nd(dir.z, 0.0f, -dir.x);
	float d = (nd | pos);
	//Трейс пути
	long fromNode = -2;	//Откуда пришли
	for(long loopCounter = 0; loopCounter < 256; loopCounter++)
	{
		//Проверить нахождения точки прибытия на текущем треугольнике
		word * trg = triangle[curNode].i;
		for(long j = 0; j < 3; j++)
		{
			//Вершины ребра
			CVECTOR & vs = *(CVECTOR *)&vertex[trg[j]];
			CVECTOR & ve = *(CVECTOR *)&vertex[trg[j == 2 ? 0 : j + 1]];
			//Нормаль ребра
			float nx = -(ve.z - vs.z);
			float nz = (ve.x - vs.x);
			float nl = sqrtf(nx*nx + nz*nz);
			
			if(!nl) continue;
			nx /= nl; nz /= nl;

			//Если за пределами треугольника прервать
			if(to.x*nx + to.z*nz > vs.x*nx + vs.z*nz) break;
		}
		if(j == 3)
		{
			//Можно переместиться
			pos = to;
			//Проверим высоту
			float h = FindHeight(curNode, pos.x, pos.z);
			if(pos.y < h)
			{
				pos.y = h;
				isBearing = true;
			}
			return curNode;
		}
		//Найти ребро через которое идём дальше
		short * nb = triangle[curNode].nb;
		long curEdge = -1;
		CVECTOR pnt;
		for(j = 0; j < 3; j++)
		{
			//Пропустим направление откуда пришли
			if(nb[j] == fromNode) continue;
			//Вершины ребра
			CVECTOR & vs = *(CVECTOR *)&vertex[trg[j]];
			CVECTOR & ve = *(CVECTOR *)&vertex[trg[j == 2 ? 0 : j + 1]];
			//Дистанции до прямой перемещения
			float d1 = (nd | vs) - d;
			float d2 = (nd | ve) - d;
			if(d1*d2 > 0.0f) continue;
			if(d1 - d2 == 0.0f) continue;
			//Пересерает линию
			double k = d1/double(d1 - d2);
			if(k < 0.0f) k = 0.0f;
			if(k > 1.0f) k = 1.0f;
			//Точка пересечения
			CVECTOR p;
			//k += 0.000000001;
			p.x = float(vs.x + (ve.x - vs.x)*k);
			p.y = float(vs.y + (ve.y - vs.y)*k);
			p.z = float(vs.z + (ve.z - vs.z)*k);
			//Проверка попадания на путь
			if((d1 = (dir | (p - ps))) < -0.00001f) continue;
			if((d2 = (dir | (to - p))) < -0.00001f) continue;
			//Нашли пересечение
			if(nb[j] >= 0)
			{
				curEdge = j;
				pnt = p;
				break;
			}else{
				curEdge = j;
				pnt = p;
				if(d1 != 0.0f && d2 != 0.0f) break;
			}
		}
		if(curEdge == -1)
		{
			isSlide = true;
			return firstNode;

			Assert(false);
			for(j = 0; j < 3; j++)
			{
				//Пропустим направление откуда пришли
				if(nb[j] == fromNode) continue;
				//Вершины ребра
				CVECTOR & vs = *(CVECTOR *)&vertex[trg[j]];
				CVECTOR & ve = *(CVECTOR *)&vertex[trg[j == 2 ? 0 : j + 1]];
				//Дистанции до прямой перемещения
				float d1 = (nd | vs) - d;
				float d2 = (nd | ve) - d;
				if(d1*d2 > 0.0f) continue;
				if(d1 - d2 == 0.0f) continue;
				//Пересерает линию
				double k = d1/double(d1 - d2);
				if(k < 0.0f) k = 0.0f;
				if(k > 1.0f) k = 1.0f;
				//Точка пересечения
				CVECTOR p;
				//k += 0.000000001;
				p.x = float(vs.x + (ve.x - vs.x)*k);
				p.y = float(vs.y + (ve.y - vs.y)*k);
				p.z = float(vs.z + (ve.z - vs.z)*k);
				//Проверка попадания на путь
				d1 = dir | (p - ps); 
				d2 = dir | (to - p);
				if(d1 < -0.00001f) continue;
				if(d2 < -0.00001f) continue;
				//Нашли пересечение
				if(nb[j] >= 0)
				{
					curEdge = j;
					pnt = p;
					break;
				}else{
					curEdge = j;
					pnt = p;
					if(d1 != 0.0f && d2 != 0.0f) break;
				}
			}
		}

		Assert(curEdge >= 0);

		if(nb[curEdge] >= 0)
		{
			//Переходим на следующий нод и продолжаем движение
			//pos = pnt;
			fromNode = curNode;
			curNode = nb[curEdge];
		}else{
			//Ребро вдоль которого надо скользить
			CVECTOR & vs = *(CVECTOR *)&vertex[trg[curEdge]];
			CVECTOR & ve = *(CVECTOR *)&vertex[trg[curEdge == 2 ? 0 : curEdge + 1]];
			//Нормаль ребра
			float nx = -(ve.z - vs.z);
			float nz = (ve.x - vs.x);
			//Нормализуем
			float d = nx*nx + nz*nz;
			if(d == 0.0f)
			{
				//Аномальная ситуация
				api->Trace("Patch have some problem -> triangle edge by zero length");
				//Просто залипаем
				pos = pnt;
				return curNode;
			}
			d = 1.0f/sqrtf(d); nx *= d; nz *= d;
			//Вектор выхода за пределы треугольника
			CVECTOR sld = to - pnt; sld.y = 0.0f;
			//Дистанция скольжения
			d = nx*sld.x + nz*sld.z;
			if(d > 0.0f) d += 0.001f;
			if(d < 0.0f) d -= 0.001f;
			//Скользим
			CVECTOR newPos(to.x - nx*d, to.y, to.z - nz*d);
			if(newPos.x == to.x && newPos.z == to.z)
			{
				isSlide = true;
				return firstNode;
				Assert(false);
			}
			newPos = pos + (newPos - pos)*0.99f;
			curNode = Move(firstNode, newPos, pos, depth + 1);
			isSlide = true;
			if(d >= 0.0f)
			{
				slideDir.x = nx;
				slideDir.y = 0.0f;
				slideDir.z = nz;
			}else{
				slideDir.x = -nx;
				slideDir.y = 0.0f;
				slideDir.z = -nz;
			}
			return curNode;
		}
	}
	if(loopCounter >= 256)
	{
		if(to.y != pos.y)
		{
			float hy = FindHeight(curNode, pos.x, pos.z);
			pos.y = to.y;
			if(pos.y < hy) pos.y = hy;
		}
		return curNode;
	}
	return -1;
}

//Получить нормаль к ноду
void PtcData::GetNodeNormal(long curNode, CVECTOR & n)
{
	if(curNode >= 0)
	{
		Assert(curNode < numTriangles);
		PtcNormal & nrm = normal[triangle[curNode].n];
		n.x = nrm.x; n.y = nrm.y; n.z = nrm.z;
	}else{
		n = 0.0f;
	}
}

//Найти направление пути
bool PtcData::FindPathDir(long curNode, const CVECTOR & cur, long toNode, const CVECTOR & to, long & node, CVECTOR & toPos)
{
	numSteps = 0;
	if(curNode < 0 || toNode < 0) return false;
	if(FindPathDir(0, curNode, cur, toNode, to, node, toPos)) return true;
	toPos = to;
	return false;
}

//Найти направление пути
bool PtcData::FindPathDir(long step, long curNode, const CVECTOR & cur, long toNode, const CVECTOR & to, long & node, CVECTOR & pos)
{
	if(step > numSteps) numSteps = step;
	Assert(curNode >= 0);
	Assert(toNode >= 0);
	//Если дошли, то вернём почку назначения
	if(curNode == toNode || step >= PTCDATA_MAXSTEPS)
	{
		pos = to;
		node = toNode;
		return true;
	}
	//Определим в каком направлении двигаться (ребро)
	Assert(curNode < numTriangles);
	Assert(toNode < numTriangles);
	byte * line = table + curNode*lineSize;
	byte v = (line[toNode >> 2] >> ((toNode & 3)*2)) & 3;
	if(v == 3) return false;
	//Ребро
	CVECTOR & vs = *(CVECTOR *)&vertex[triangle[curNode].i[v]];
	CVECTOR & ve = *(CVECTOR *)&vertex[triangle[curNode].i[v + 1 < 3 ? v + 1 : 0]];
	//Точка на ребре
	CVECTOR p = FindEdgePoint(vs, ve, cur, to);
	//Проанализируем дальнейший путь
	long nb = triangle[curNode].nb[v];
	if(nb < 0) return false;
	if(!FindPathDir(step + 1, nb, p, toNode, to, node, pos)) return false;
	//Уточнёная точка на ребре
	pos = FindEdgePoint(vs, ve, cur, pos);
	stepPos[step] = pos;
	node = nb;
	return true;
}

//Найти точку на ребре
CVECTOR PtcData::FindEdgePoint(const CVECTOR & vs, const CVECTOR & ve, const CVECTOR & cur, const CVECTOR & to)
{
	CVECTOR pos;
	//Нормаль к ребру
	CVECTOR edge = ve - vs; edge.y = 0.0f;
	CVECTOR nrm(edge.z, 0.0f, -edge.x);
	float nl = ~nrm;
	if(nl > 0.0f)
	{
		//Плоскость
		nrm *= 1.0f/sqrtf(nl);
		float dist = nrm | vs;
		//Положение точки назначения
		float dTo = (to | nrm) - dist;
		if(dTo <= 0.0f)
		{
			//Точка назначения за ребром
			float dCur = (cur | nrm) - dist;
			if(dCur - dTo == 0.0f) return (vs + ve)*0.5f;			
			float k = dCur/(dCur - dTo);
			pos = cur + (to - cur)*k;
			//Ограничиваем точку перемещения
			CVECTOR p = vs + (ve - vs)*0.001f;
			k = (ve - p) | (pos - p);
			if(k <= 0.0f) pos = p;
			p = vs + (ve - vs)*0.999f;
			k = (vs - p) | (pos - p);
			if(k <= 0.0f) pos = p;
		}else{
			//Точка назначения перед ребром
			if(((to - cur) | edge) >= 0.0f)
			{
				//Выбираем конечную точку ребра
				pos = vs + (ve - vs)*0.99f;
			}else{
				//Выбираем начальную точку ребра
				pos = vs + (ve - vs)*0.01f;
			}
		}
	}else{
		pos = (vs + ve)*0.5f;
	}
	return pos;	
}

//Найти пересечение с патчём
float PtcData::Trace(const CVECTOR & s, const CVECTOR & d)
{
	//Область описывающая отрезок
	float k = 2.0f;
	CVECTOR pmin = s;
	if(pmin.x > d.x) pmin.x = d.x;
	if(pmin.y > d.y) pmin.y = d.y;
	if(pmin.z > d.z) pmin.z = d.z;
	CVECTOR pmax = s;
	if(pmax.x < d.x) pmax.x = d.x;
	if(pmax.y < d.y) pmax.y = d.y;
	if(pmax.z < d.z) pmax.z = d.z;
	//Координаты на карте
	long mnX = long((pmin.x - min.x)/ws);
	long mnZ = long((pmin.z - min.z)/ls);
	long mxX = long((pmax.x - min.x)/ws);
	long mxZ = long((pmax.z - min.z)/ls);
	//Проходимся по всем треугольникам в зоне
	for(long zi = mnZ; zi <= mxZ; zi++)
	{
		if(zi < 0 || zi >= l) continue;
		for(long xi = mnX; xi <= mxX; xi++)
		{
			if(xi < 0 || xi >= w) continue;
			PtcMap & m = map[zi*w + xi];
			word * ids = indeces + m.start;
			for(long i = 0; i < m.size; i++)
			{
				float kn = Trace(triangle[ids[i]], s, d);
				if(kn < k) k = kn;
			}
		}
	}
	return k;
}

//Проверить пересечение треугольника с отрезком
float PtcData::Trace(PtcTriangle & trg, const CVECTOR & s, const CVECTOR & d)
{
	//Нормаль к треугольнику
	CVECTOR & n = *(CVECTOR *)&normal[trg.n];
	//Дистанция до треугольника
	float dst = n | *(CVECTOR *)&vertex[trg.i[0]];
	//Дистанции отрезка до плоскости
	float d1 = (n | s) - dst;
	float d2 = (n | d) - dst;
	//Если не пересекаем плоскость, то непопали
	if(d1*d2 > 0.0f) return 2.0f;
	//Точка пересечения луча с плоскостью
	float k;
	if(d1 - d2 != 0.0f) k = d1/(d1 - d2); else k = 0.0f;
	if(k < 0.0f) k = 0.0f;
	if(k > 1.0f) k = 1.0f;
	CVECTOR pnt = s + (d - s)*k;
	//Вершины
	CVECTOR & v0 = *(CVECTOR *)&vertex[trg.i[0]];
	CVECTOR & v1 = *(CVECTOR *)&vertex[trg.i[1]];
	CVECTOR & v2 = *(CVECTOR *)&vertex[trg.i[2]];
	//Проверим нахождение точки в треугольнике
	CVECTOR en;
	en = (v1 - v0) ^ n;
	if((en | pnt) - (en | v0) > 0.0f) return 2.0f;
	en = (v2 - v1) ^ n;
	if((en | pnt) - (en | v1) > 0.0f) return 2.0f;
	en = (v0 - v2) ^ n;
	if((en | pnt) - (en | v2) > 0.0f) return 2.0f;
	//Попали
	return k;
}

//Найти силу отталкивающую от краёв
void PtcData::FindForce(long curNode, const CVECTOR & pos, float dist, CVECTOR & force)
{
	force = 0.0f;
	if(curNode < 0 || curNode >= numTriangles) return;
	short * nb = triangle[curNode].nb;
	for(long i = 0; i < 3; i++)
	{
		if(nb[i] >= 0) continue;
		//Нормаль к ребру
		long s = triangle[curNode].i[i];
		long e = triangle[curNode].i[i < 2 ? i + 1 : 0];
		//Вершины ребра
		CVECTOR & vs = *(CVECTOR *)&vertex[s];
		CVECTOR & ve = *(CVECTOR *)&vertex[e];
		//Нормаль ребра
		float nx = (ve.z - vs.z);
		float nz = -(ve.x - vs.x);
		float nl = sqrtf(nx*nx + nz*nz);
		if(nl < 1e-10f) continue;
		nx /= nl; nz /= nl;
		//Дистанция до ребра
		float d = pos.x*nx + pos.z*nz - vs.x*nx - vs.z*nz;
		if(d >= dist) continue;
		//Сила отталкивания
		if(d < 0.5f) d = 0.5f;
		d = 1.0f/d;
		force.x += nx*d;
		force.z += nz*d;
	}
}

//Получить материал нода
const char * PtcData::GetMaterial(long curNode)
{
	if(!materials) return null;
	if(curNode < 0 || curNode >= numTriangles) return null;
	long mtl = triangle[curNode].mtl;
	if(mtl >= 15 || mtl < 0 || mtl >= materials->numMaterials) return null;
	return materials->material[mtl];
}

//Получить треугольники пересекающии данный квадрат
PtcData::Triangle * PtcData::GetTriangles(float x, float z, float sx, float sz, long & num)
{
	num = 0;
	numClTriangles = 0;
	float minX = x - sx*0.5f;
	float minZ = z - sz*0.5f;
	float maxX = x + sx*0.5f;
	float maxZ = z + sz*0.5f;
	long mnX = long((minX - min.x)/ws);
	long mnZ = long((minZ - min.z)/ls);
	long mxX = long((maxX - min.x)/ws);
	long mxZ = long((maxZ - min.z)/ls);
	for(long zi = mnZ; zi <= mxZ; zi++)
	{
		if(zi < 0 || zi >= l) continue;
		for(long xi = mnX; xi <= mxX; xi++)
		{
			if(xi < 0 || xi >= w) continue;
			PtcMap & m = map[zi*w + xi];
			word * ids = indeces + m.start;
			for(long i = 0; i < m.size; i++) AddClTriangle(ids[i]);
		}
	}
	num = numClTriangles;
	return ctriangle;
}

//Добавить треугольник в буфер
inline void PtcData::AddClTriangle(long i)
{
	for(long j = 0; j < numClTriangles; j++)
		if(ctriangle[j].index == i) return;
	if(numClTriangles >= maxClTriangles)
	{
		maxClTriangles += 64;
		ctriangle = (Triangle *)RESIZE(ctriangle, maxClTriangles*sizeof(Triangle));
	}
	Triangle & ct = ctriangle[numClTriangles++];
	word * idx = triangle[i].i;
	ct.index = i;
	ct.v[0].x = vertex[idx[0]].x;
	ct.v[0].y = vertex[idx[0]].y;
	ct.v[0].z = vertex[idx[0]].z;
	ct.v[1].x = vertex[idx[1]].x;
	ct.v[1].y = vertex[idx[1]].y;
	ct.v[1].z = vertex[idx[1]].z;
	ct.v[2].x = vertex[idx[2]].x;
	ct.v[2].y = vertex[idx[2]].y;
	ct.v[2].z = vertex[idx[2]].z;
	ct.n.x = normal[triangle[i].n].x;
	ct.n.y = normal[triangle[i].n].y;
	ct.n.z = normal[triangle[i].n].z;
}

//Вычислить высоту точки на плоскосте треугольника
inline float PtcData::FindHeight(long trgID, float x, float z)
{
	Assert(trgID >= 0 && trgID < numTriangles);
	CVECTOR & n = *(CVECTOR *)&normal[triangle[trgID].n];
	float d = n | *(CVECTOR *)&vertex[triangle[trgID].i[0]];
	d = float((d - n.x*x - n.z*z)/double(n.y) + 0.0001);
	return d;
}

//Отладочная отрисовка
void PtcData::DebugDraw(VDX8RENDER * rs, float dltTime)
{
	if(numTriangles <= 0) return;
	if(!dbgTriangles)
	{
		dbgTriangles = NEW DbgVertex[numTriangles*3];
		for(long i = 0; i < numTriangles; i++)
		{
			dbgTriangles[i*3 + 0].x = vertex[triangle[i].i[0]].x;
			dbgTriangles[i*3 + 0].y = vertex[triangle[i].i[0]].y;
			dbgTriangles[i*3 + 0].z = vertex[triangle[i].i[0]].z;
			dbgTriangles[i*3 + 0].c = 0xff00008f;
			dbgTriangles[i*3 + 1].x = vertex[triangle[i].i[1]].x;
			dbgTriangles[i*3 + 1].y = vertex[triangle[i].i[1]].y;
			dbgTriangles[i*3 + 1].z = vertex[triangle[i].i[1]].z;
			dbgTriangles[i*3 + 1].c = dbgTriangles[i*3 + 0].c;
			dbgTriangles[i*3 + 2].x = vertex[triangle[i].i[2]].x;
			dbgTriangles[i*3 + 2].y = vertex[triangle[i].i[2]].y;
			dbgTriangles[i*3 + 2].z = vertex[triangle[i].i[2]].z;			
			dbgTriangles[i*3 + 2].c = dbgTriangles[i*3 + 0].c;
		}
	}
	if(!dbgEdges)
	{
		dbgEdges = NEW DbgVertex[numTriangles*3*2];
		for(long i = 0; i < numTriangles; i++)
		{
			for(long j = 0; j < 3; j++)
			{
				float y = 0.05f;
				if(triangle[i].nb[j] >= 0 && triangle[i].nb[j] < i) y += 0.01f;
				dbgEdges[i*6 + j*2 + 0].x = vertex[triangle[i].i[j]].x;
				dbgEdges[i*6 + j*2 + 0].y = vertex[triangle[i].i[j]].y + 0.001f;
				dbgEdges[i*6 + j*2 + 0].z = vertex[triangle[i].i[j]].z;
				dbgEdges[i*6 + j*2 + 0].c = triangle[i].nb[j] >= 0 ? 0xff008f00 : 0xff8f0000;
				dbgEdges[i*6 + j*2 + 1].x = vertex[triangle[i].i[j + 1 < 3 ? j + 1 : 0]].x;
				dbgEdges[i*6 + j*2 + 1].y = vertex[triangle[i].i[j + 1 < 3 ? j + 1 : 0]].y + 0.001f;
				dbgEdges[i*6 + j*2 + 1].z = vertex[triangle[i].i[j + 1 < 3 ? j + 1 : 0]].z;
				dbgEdges[i*6 + j*2 + 1].c = dbgEdges[i*6 + j*2 + 0].c;
			}
		}
	}
	char * tech = "DbgPatchViewZ";
	rs->SetTransform(D3DTS_WORLD, CMatrix());
	rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, numTriangles, dbgTriangles, sizeof(DbgVertex), tech);
	rs->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, numTriangles*3, dbgEdges, sizeof(DbgVertex), tech);
}
