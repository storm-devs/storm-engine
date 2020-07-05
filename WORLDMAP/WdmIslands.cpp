//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmIslands
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "WdmIslands.h"
#include "WorldMap.h"
#include "WdmCamera.h"
#include "WdmShip.h"

//============================================================================================
//Данные для коллижена
//============================================================================================

CVECTOR WdmIslands::centPos;
long WdmIslands::numEdges;
CVECTOR WdmIslands::curPos;
bool WdmIslands::checkMode;
CMatrix WdmIslands::curMatrix;


//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmIslands::WdmIslands() : labelSort(_FL_, 1024), 
	islands(_FL_), 
	labels(_FL_),
	fonts(_FL_),
	merchants(_FL_),
	quests(_FL_)
{
	icons.texture = -1;
	LabelsRelease();
	wdmObjects->islands = this;
	//Грузим базовую модель
	baseModel = (WdmRenderModel *)wdmObjects->wm->CreateModel(NEW WdmRenderModel(), "islands\\islands", false, false, false, 1);
	if(!baseModel || !baseModel->geo) return;
	//Информация о геометрии
	GEOS::INFO ginfo;
	GEOS::LABEL label;
	baseModel->geo->GetInfo(ginfo);
	//Находим размеры мира и перебираем локаторы
	CVECTOR vmin, vmax, center = 0.0f, vmn, vmx;
	bool isMin = false, isMax = false;
	for(long i = 0; i < ginfo.nlabels; i++)
	{
		baseModel->geo->GetLabel(i, label);
		if(!label.group_name || !label.group_name[0]) continue;
		if(stricmp(label.group_name, "locators") == 0)
		{
			if(stricmp(label.name, "min") == 0)
			{
				vmn = ((CMatrix *)label.m)->Pos();
				isMin = true;
			}
			if(stricmp(label.name, "max") == 0)
			{
				vmx = ((CMatrix *)label.m)->Pos();
				isMax = true;
			}
		}else
		if(stricmp(label.group_name, "merchant") == 0)
		{
			merchants.Add(((CMatrix *)label.m)->Pos());
		}else
		if(label.name && label.name[0] && stricmp(label.group_name, "quests") == 0)
		{
			Quest & q = quests[quests.Add()];
			q.pos = ((CMatrix *)label.m)->Pos();
			q.name = label.name;
		}
	}
	if(isMin && isMax)
	{
		vmin.x = min(vmn.x, vmx.x);
		vmin.y = 0.0f;
		vmin.z = min(vmn.z, vmx.z);
		vmax.x = max(vmn.x, vmx.x);
		vmax.y = 0.0f;
		vmax.z = max(vmn.z, vmx.z);
		center = (vmin + vmax)*-0.5f;
		vmin += center;
		vmax += center;
		wdmObjects->SetWorldSize(vmax.x*2.0f, vmax.z*2.0f);
	}
	//Зачитываем острова
	string name;
	for(i = 0; i < ginfo.nlabels; i++)
	{
		//Получаем информацию
		baseModel->geo->GetLabel(i, label);
		if(!label.group_name || !label.group_name[0]) continue;
		if(!label.name || !label.name[0]) continue;
		if(stricmp(label.group_name, "islands") != 0) continue;		
		//Пропускаем, если добавленна
		for(long j = 0; j < islands; j++)
		{
			if(islands[j].modelName == label.name) break;
		}
		if(j < islands) continue;
		name = "islands\\";
		name += label.name;
		//Загружаем
		WdmRenderModel * model = (WdmRenderModel *)wdmObjects->wm->CreateModel(NEW WdmRenderModel(), name, false, false, true, 2);
		if(model)
		{
			//Общее
			Islands & isl = islands[islands.Add()];
			isl.model = model;
			model->mtx = *((CMatrix *)label.m);
			model->mtx.Pos() += center;
			isl.toLocal = model->mtx;
			isl.toLocal.Transposition();
			isl.modelName = label.name;
			isl.worldPosition = model->mtx.Pos();
			//Моделька, описывающая область острова
			name += "_area";
			isl.area = (WdmRenderModel *)wdmObjects->wm->CreateModel(NEW WdmRenderModel(), name, false, false, false, 3);
			if(!isl.area)
			{
				api->Trace("World map: can't load model of island's area: %s", name.GetBuffer());
			}
			//Пальмы
			name = "islands\\";
			name += label.name;
			name += "_palms";
			isl.palms = (WdmRenderModel *)wdmObjects->wm->CreateModel(NEW WdmRenderModel(), name, false, false, true, 4, 800);
			if(isl.palms)
			{
				static const char * techName = "WdmModelDrawStdAlphaTest";
				isl.palms->SetTech(techName, techName);
			}
			//Пена
			name = "islands\\";
			name += label.name;
			name += "_waves";
			isl.waves = (WdmIslandWaves *)wdmObjects->wm->CreateModel(NEW WdmIslandWaves(), name, false, false, true, 4);
		}else{
			api->Trace("World map: can't load model of island: %s", name.GetBuffer());
		}
	}
	//Загрузка патча
	patch = new PtcData();
	if(!patch->Load("RESOURCE\\MODELS\\WorldMap\\islands\\islands_patch.ptc"))
	{
		delete patch;
		patch = null;
	}
}

WdmIslands::~WdmIslands()
{
	if(patch)
	{
		delete patch;
		patch = null;
	}
	wdmObjects->islands = null;
	for(long i = 0; i < islands; i++)
	{
		wdmObjects->wm->DeleteObject(islands[i].model);
		islands[i].model = null;
	}
	if(baseModel) wdmObjects->wm->DeleteObject(baseModel);
	islands.DelAll();
	LabelsRelease();
}


//Проверить на возможное столкновение
bool WdmIslands::CollisionTest(CMatrix & objMtx, float length, float width, bool heighTest)
{
	const float maxHeightInTest = 0.5f;
	//Радиус прямоугольника на плоскости
	float boxRadius = sqrtf(length*length + width*width);
	if(boxRadius < 0.0000001f) return false;
	//Радиус ящика
	float checkRadius = boxRadius + maxHeightInTest;
	if(!heighTest) checkRadius += 1000000.0f;
	//Параметры тестирования
	numEdges = 0;
	centPos = 0.0f;
	checkMode = !heighTest;
	//Проходимся по всем островам
	for(long i = 0; i < islands; i++)
	{
		//Остров с которым работаем
		Islands & island = islands[i];
		//Матрица преобразования в локальную систему
		CMatrix locMtx(objMtx, island.toLocal);
		
		//Проверяем поподание в круг острова
		float dist2 = ~(locMtx.Pos() - island.model->center);
		float maxDist = boxRadius + island.model->radius;
		if(dist2 >= maxDist*maxDist) continue;
		//Будем тестировать на уровне треугольников
		GEOS::PLANE p[5];		
		//Вектор направления
		p[0].nrm.x = locMtx.Vz().x;
		p[0].nrm.y = 0.0f;
		p[0].nrm.z = locMtx.Vz().z;
		float radius = p[0].nrm.x*p[0].nrm.x + p[0].nrm.z*p[0].nrm.z;
		//Проскочим кривую модель
		if(radius < 0.000001f) continue;
		//Нормаль направления
		radius = 1.0f/sqrtf(radius);
		p[0].nrm.x *= radius;
		p[0].nrm.z *= radius;
		//Остальные нормали
		p[1].nrm.x = -p[0].nrm.x; p[1].nrm.y = 0.0f; p[1].nrm.z = -p[0].nrm.z;
		p[2].nrm.x = -p[0].nrm.z; p[2].nrm.y = 0.0f; p[2].nrm.z = p[0].nrm.x;
		p[3].nrm.x = p[0].nrm.z; p[3].nrm.y = 0.0f; p[3].nrm.z = -p[0].nrm.x;
		p[4].nrm.x = 0.0f; p[4].nrm.y = 1.0f; p[4].nrm.z = 0.0f;
		//Дистанции до плоскостей
		p[0].d = (p[0].nrm.x*length + locMtx.Pos().x)*p[0].nrm.x + (p[0].nrm.z*length + locMtx.Pos().z)*p[0].nrm.z;
		p[1].d = (p[1].nrm.x*length + locMtx.Pos().x)*p[1].nrm.x + (p[1].nrm.z*length + locMtx.Pos().z)*p[1].nrm.z;
		p[2].d = (p[2].nrm.x*width + locMtx.Pos().x)*p[2].nrm.x + (p[2].nrm.z*width + locMtx.Pos().z)*p[2].nrm.z;
		p[3].d = (p[3].nrm.x*width + locMtx.Pos().x)*p[3].nrm.x + (p[3].nrm.z*width + locMtx.Pos().z)*p[3].nrm.z;
		p[4].d = maxHeightInTest;
		//Позиция сферы
		GEOS::VERTEX v; v.x = locMtx.Pos().x; v.y = locMtx.Pos().y; v.z = locMtx.Pos().z;
		curPos = locMtx.Pos();
		//Матрица преобразования в мировые координаты
		curMatrix = island.model->mtx;		
		//Ищем попадание полигонов
		island.model->geo->Clip(p, heighTest ? 5 : 4, v, checkRadius, AddEdges);
	}
	if(numEdges)
	{
		centPos *= 1.0f/numEdges;
		return true;
	}
	return false;
}

bool _cdecl WdmIslands::AddEdges(const GEOS::VERTEX * vrt, long numVrt)
{
	//Пропустим кривые данные
	if(numVrt < 3) return true;
	if(WdmIslands::checkMode)
	{
		centPos = curMatrix*CVECTOR(vrt[0].x, 0.0f, vrt[0].z);
		numEdges = 1;
		return false;
	}
	//Попытаемся определить место положение точки относительно плоскости
	CVECTOR v1(vrt[1].x - vrt[0].x, vrt[1].y - vrt[0].y, vrt[1].z - vrt[0].z);
	CVECTOR v2(vrt[2].x - vrt[1].x, vrt[2].y - vrt[1].y, vrt[2].z - vrt[1].z);
	CVECTOR v = v1 ^ v2;
	float d = (v | WdmIslands::curPos) - (v | CVECTOR(vrt[0].x, vrt[0].y, vrt[0].z));
	if(d < 0) return true;
	//Добавляем рёбра
	numEdges += numVrt;
	for(long i = 0; i < numVrt; i++)
	{
		centPos += curMatrix*CVECTOR(vrt[i].x, 0.0f, vrt[i].z);
	}	
	//Продолжим
	return true;
}

//Проверить наличие в данном месте треугольников
bool WdmIslands::ObstacleTest(float x, float z, float radius)
{
	if(radius <= 0.0f) return false;
	CVECTOR wPos(x, 0.0f, z);
	//Проходимся по всем островам
	for(long i = 0; i < islands; i++)
	{
		//Остров с которым работаем
		Islands & island = islands[i];
		//Преобразуем в локальную систему острова
		CVECTOR pos = island.toLocal*wPos;
		GEOS::PLANE p[4];
		p[0].nrm.x = 0.0f; p[0].nrm.y = 0.0f; p[0].nrm.z = 1.0f; p[0].d = pos.z + radius;
		p[1].nrm.x = 0.0f; p[1].nrm.y = 0.0f; p[1].nrm.z = -1.0f; p[1].d = -(pos.z - radius);
		p[2].nrm.x = 1.0f; p[2].nrm.y = 0.0f; p[2].nrm.z = 0.0f; p[2].d = pos.x + radius;
		p[3].nrm.x = -1.0f; p[3].nrm.y = 0.0f; p[3].nrm.z = 0.0f; p[3].d = -(pos.x - radius);
		GEOS::VERTEX vrt;
		vrt.x = pos.x;	
		vrt.y = 0.0f;
		vrt.z = pos.z;
		if(island.model->geo->Clip(p, 4, vrt, 50.0f, 0)) return true;
	}
	return false;
}


//Зачитать данные об островах
void WdmIslands::SetIslandsData(ATTRIBUTES * apnt, bool isChange)
{
	CVECTOR pos;
	if(!isChange)
	{
		LabelsRelease();
		//Зачитываем параметры картинок
		LabelsReadIconParams(apnt);
	}
	if(!apnt) return;
	//Получаем описание меток
	apnt = apnt->FindAClass(apnt, "labels");
	if(!apnt) return;
	//Количество меток
	dword numAttr = apnt->GetAttributesNum();
	//Перебираем все метки
	for(dword i = 0; i < numAttr; i++)
	{
		//Получаем доступ к описанию метки
		ATTRIBUTES * a = apnt->GetAttributeClass(i);
		if(!a) continue;
		//Получаем параметры метки
		char * id = a->GetAttribute("id");
		char * locator = a->GetAttribute("locator");
		char * text = a->GetAttribute("text");
		long icon = (long)a->GetAttributeAsDword("icon", (dword)-1);
		char * font = a->GetAttribute("font");
		float pivotX = -0.5f;
		float pivotY = -0.5f;
		float heightView = a->GetAttributeAsFloat("heightView", 250.0);
		dword weight = a->GetAttributeAsDword("weight", 0);
		//Проверяем на достаточность		
		if(!id || !text || !locator || !locator[0])
		{
			api->Trace("World map: label \"%s\" will be skipping...", apnt->GetAttributeName(i));			
			continue;
		}
		//Ищим метку среди существующих
		dword hash = wdmObjects->CalcHash(id);
		long index = LabelsFind(id, hash);
		if(index < 0)
		{
			if(!LabelsFindLocator(locator, pos))
			{
				api->Trace("World map: locator \"%s\" in label \"%s\" not found...", locator, apnt->GetAttributeName(i));
				continue;
			}
			//Добавляем новую метку
			long iEntry = hash & (sizeof(labelsEntry)/sizeof(labelsEntry[0]) - 1);
			index = labels.Add();
			labels[index].id = id;
			labels[index].idHash = hash;
			labels[index].next = labelsEntry[iEntry];
			labelsEntry[iEntry] = index;
			labels[index].locatorName = locator;
			labels[index].alpha = 255.0f;
			
		}else{
			if(stricmp(labels[index].locatorName, locator) != 0)
			{
				if(LabelsFindLocator(locator, pos))
				{
					labels[index].locatorName = locator;
				}
			}
		}
		//Текст
		labels[index].text = text ? text : "";
		//Шрифт
		labels[index].font = LabelsAddFont(font);
		//Картинка
		labels[index].icon = icon < (long)icons.num ? icon : icons.num - 1;
		//Позиция
		labels[index].pos = pos;
		//Высота гашения
		labels[index].heightView = heightView;
		labels[index].weight = weight;
		//Размеры строки
		float textWidth = (float)wdmObjects->rs->StringWidth((char *)labels[index].text.GetBuffer(), labels[index].font);
		float textHeight = (float)wdmObjects->rs->CharHeight(labels[index].font);
		//Размеры метки
		float labelWidth =textWidth + (labels[index].icon >= 0 ? icons.w + 4.0f: 0.0f);
		float labelHeight = labels[index].icon >= 0 ? max(textHeight, icons.h) : textHeight;
		//Позиция метки с учётом смещения
		float labelX = pivotX*labelWidth;
		float labelY = pivotY*labelHeight;
		//Смещённый прямоугольник
		labels[index].dl = labelX;
		labels[index].dt = labelY;
		labels[index].dr = labelX + labelWidth;
		labels[index].db = labelY + labelHeight;		
		if(labels[index].icon >= 0)
		{
			//Позиция картинки
			labels[index].iconX = 0.0f;
			labels[index].iconY = (labelHeight - icons.h)*0.5f;
			//Позиция текста
			labels[index].textX = icons.h + 4.0f;
			labels[index].textY = (labelHeight - textHeight)*0.5f;
		}else{
			//Позиция картинки
			labels[index].iconX = 0.0f;
			labels[index].iconY = 0.0f;
			//Позиция текста
			labels[index].textX = 0.0f;
			labels[index].textY = 0.0f;
		}
	}	
}

void WdmIslands::LabelsReadIconParams(ATTRIBUTES * apnt)
{
	if(!apnt) return;
	apnt = apnt->FindAClass(apnt, "icon");
	if(!apnt) return;
	icons.w = (float)apnt->GetAttributeAsDword("width", 32);
	icons.h = (float)apnt->GetAttributeAsDword("height", 32);
	icons.num = apnt->GetAttributeAsDword("num", 8);
	icons.frames = apnt->GetAttributeAsDword("frames", 1);
	icons.fps = apnt->GetAttributeAsFloat("fps", 0.0f);
	icons.frame = 0.0f;
	icons.f[0] = 0.0f;
	icons.f[1] = 0.0f;
	icons.blend = 0;
	char * texName = apnt->GetAttribute("texture");
	if(!texName) texName = "";
	string name = "WorldMap\\Interfaces\\";
	name += texName;
	icons.texture = wdmObjects->rs->TextureCreate(name);
	//Размеры uv
	float tw = icons.w*icons.frames;
	float th = icons.h*icons.num;
	if(tw)
	{
		icons.u = icons.w/tw;
	}else{
		icons.u = 0.0f;
	}
	if(th)
	{
		icons.v = icons.h/th;
	}else{
		icons.v = 0.0f;
	}
}

long WdmIslands::LabelsFind(const char * id, dword hash)
{
	if(!id) id = "";
	long i = hash & (sizeof(labelsEntry)/sizeof(labelsEntry[0]) - 1);
	for(i = labelsEntry[i]; i >= 0; i = labels[i].next)
	{
		if(labels[i].idHash == hash)
		{
			if(stricmp(labels[i].id, id) == 0) return i;
		}
	}
	return -1;
}

bool WdmIslands::LabelsFindLocator(const char * name, CVECTOR & pos)
{
	if(!baseModel || !name || !name[0]) return false;
	//Информация о геометрии
	GEOS::INFO ginfo;
	GEOS::LABEL label;
	baseModel->geo->GetInfo(ginfo);
	for(long i = 0; i < ginfo.nlabels; i++)
	{
		baseModel->geo->GetLabel(i, label);
		if(!label.group_name || !label.group_name[0]) continue;
		if(stricmp(label.group_name, "labels") != 0) continue;
		//if(stricmp(label.group_name, "geometry") != 0) continue;
		if(stricmp(label.name, name) == 0)
		{
			pos = ((CMatrix *)label.m)->Pos();
			return true;
		}
	}
	return false;
}

long WdmIslands::LabelsAddFont(const char * name)
{
	if(!name) name = "";
	for(long i = 0; i < fonts; i++)
	{
		if(stricmp(fonts[i].name, name) == 0)
		{
			return fonts[i].id;
		}
	}
	dword index;
	Font & font = fonts[index = fonts.Add()];
	font.name = name;
	font.id = wdmObjects->rs->LoadFont((char *)name);
	if(font.id < 0)
	{
		font.id = FONT_DEFAULT;
	}
	return long(index);
}


void WdmIslands::LabelsRelease()
{
	//Удаляем все метки
	labels.DelAll();
	for(long i = 0; i < sizeof(labelsEntry)/sizeof(labelsEntry[0]); i++)
	{
		labelsEntry[i] = -1;
	}	
	//Удаляем все шрифты
	for(i = 0; i < fonts; i++)
	{
		if(fonts[i].id != FONT_DEFAULT)
		{
			wdmObjects->rs->UnloadFont(fonts[i].id);
		}
	}
	//Удаляем все картинки
	if(icons.texture >= 0)
	{
		wdmObjects->rs->TextureRelease(icons.texture);
	}
	memset(&icons, sizeof(icons), 0);
	icons.texture = -1;
}

void WdmIslands::Update(float dltTime)
{
	if(wdmObjects->playerShip)
	{
		CVECTOR pos;
		wdmObjects->playerShip->GetPosition(pos.x, pos.z, pos.y);
		pos.y = 0.0f;
		for(long i = 0; i < islands; i++)
		{
			if(IsShipInArea(i, pos))
			{
				if(wdmObjects->wm->AttributesPointer)
				{
					wdmObjects->wm->AttributesPointer->SetAttribute("island", (char *)islands[i].modelName.GetBuffer());
					ATTRIBUTES * a = wdmObjects->wm->AttributesPointer->FindAClass(wdmObjects->wm->AttributesPointer, "island");
					if(a)
					{
						a->SetAttributeUseFloat("x", islands[i].worldPosition.x);
						a->SetAttributeUseFloat("z", islands[i].worldPosition.z);
					}
					wdmObjects->curIsland = (char *)islands[i].modelName.GetBuffer();
				}
				return;
			}
		}
	}
	wdmObjects->curIsland = null;
	wdmObjects->wm->AttributesPointer->SetAttribute("island", "");
	ATTRIBUTES * a = wdmObjects->wm->AttributesPointer->FindAClass(wdmObjects->wm->AttributesPointer, "island");
	if(a)
	{
		a->SetAttributeUseFloat("x", 0.0f);
		a->SetAttributeUseFloat("z", 0.0f);
	}

}

void WdmIslands::LRender(VDX8RENDER * rs)
{
	//Рисуем патч если надо
	if(wdmObjects->isDebug)
	{
		if(patch && api->Controls->GetDebugAsyncKeyState('1') < 0)
		{
			patch->DebugDraw(rs, api->GetDeltaTime()*0.001f);
		}
	}
	//Обновим состояние картинок
	icons.frame += api->GetDeltaTime()*0.001f*icons.fps;
	icons.frame = (icons.frame/icons.frames - long(icons.frame/icons.frames))*icons.frames;
	if(icons.frame < 0.0f)
	{
		icons.frame += icons.frames;
	}
	icons.f[0] = (float)long(icons.frame);
	icons.blend = (long)((icons.frame - icons.f[0])*255.0f);
	if(icons.blend < 0) icons.blend = 0;
	if(icons.blend > 255) icons.blend = 255;
	icons.blend |= (icons.blend << 24) | (icons.blend << 16) | (icons.blend << 8);
	if(icons.f[0] < 0) icons.f[0] = icons.frames - 1.0f;
	if(icons.f[0] >= icons.frames) icons.f[0] = 0.0f;
	icons.f[1] = icons.f[0] + 1;	
	if(icons.f[1] < 0) icons.f[1] = icons.frames - 1.0f;
	if(icons.f[1] >= icons.frames) icons.f[1] = 0.0f;
	icons.f[0] *= icons.u;
	icons.f[1] *= icons.u;
	//Получим текущую матрицу преобразования
	static CMatrix mtx, view, prj;
	rs->GetTransform(D3DTS_VIEW, view);
	rs->GetTransform(D3DTS_PROJECTION, prj);
	mtx.EqMultiply(view, prj);
	//Получим текущие размеры vp
	static D3DVIEWPORT8 vp;
	rs->GetViewport(&vp);
	float w = (float)vp.Width;
	float h = (float)vp.Height;
	//Получим высоту камеры
	float cameraHeight = wdmObjects->camera->realHeight;
	float dAlpha = api->GetDeltaTime()*(0.001f*1.5f*255.0f);
	//Проецируем на экран
	labelSort.Empty();
	MTX_PRJ_VECTOR prjVertex;
	for(long i = 0; i < labels; i++)
	{
		//Метка
		Label & label = labels[i];
		//Прозрачность метки
		if(label.heightView > cameraHeight)
		{
			label.alpha += dAlpha;
			if(label.alpha > 255.0f) label.alpha = 255.0f;
		}else{
			label.alpha -= dAlpha;
			if(label.alpha < 0.0f) label.alpha = 0.0f;
		}
		//Если метку не видно, то ничего не делаем
		if(label.alpha < 1.0f) continue;
		//Считаем точку проекции
		//rs->DrawSphere(label.pos, 1.0f, 0xff00ff00);
		mtx.Projection(&label.pos, &prjVertex, 1, w*0.5f, h*0.5f, 0, 0);
		//Прямоугольник на экране
		label.l = prjVertex.x + label.dl;//label.dl;
		label.t = prjVertex.y + 0;//label.dt;
		label.r = prjVertex.x + label.dr - label.dl;
		label.b = prjVertex.y + label.db - label.dt;
		//Если метку не видно, то удаляем запись
		if(label.l >= w || label.t >= h || label.r < 0.0f || label.b  < 0.0f)
		{
			continue;
		}
		labelSort.Add(i);
	}
	//Размещаем метки, чтобы не пересекались по порядку
	//!!!
	//Рисуем иконоки и пишем текст
	for(i = 0; i < labelSort; i++)
	{
		//Метка
		Label & label = labels[labelSort[i]];
		//Пишем текст
		dword color = (long(label.alpha) << 24) | 0xffffff;
		rs->Print(label.font, color, long(label.l + label.textX), long(label.t + label.textY), (char *)label.text.GetBuffer());
		//Рисуем картинку
		if(label.icon < 0) continue;
		rs->TextureSet(0, icons.texture);
		rs->TextureSet(1, icons.texture);
		rs->SetRenderState(D3DRS_TEXTUREFACTOR, (dword)icons.blend);
		//Заполняем вершины
		static struct
		{
			float x, y, z, rhw;
			dword c;
			float tu1, tv1;
			float tu2, tv2;
		} drawbuf[4];
		//0
		drawbuf[0].x = label.l + label.iconX;
		drawbuf[0].y = label.t + label.iconY;
		drawbuf[0].z = 0.0f;
		drawbuf[0].rhw = 0.5f;
		drawbuf[0].c = color;
		drawbuf[0].tu1 = icons.f[0];
		drawbuf[0].tv1 = label.icon*icons.v;
		drawbuf[0].tu2 = icons.f[1];
		drawbuf[0].tv2 = drawbuf[0].tv1;
		//1
		drawbuf[1].x = drawbuf[0].x;
		drawbuf[1].y = drawbuf[0].y + icons.h;
		drawbuf[1].z = 0.0f;
		drawbuf[1].rhw = 0.5f;
		drawbuf[1].c = color;
		drawbuf[1].tu1 = drawbuf[0].tu1;
		drawbuf[1].tv1 = drawbuf[0].tv1 + icons.v;
		drawbuf[1].tu2 = drawbuf[0].tu2;
		drawbuf[1].tv2 = drawbuf[1].tv1;
		//2
		drawbuf[2].x = drawbuf[0].x + icons.w;
		drawbuf[2].y = drawbuf[0].y;
		drawbuf[2].z = 0.0f;
		drawbuf[2].rhw = 0.5f;
		drawbuf[2].c = color;
		drawbuf[2].tu1 = drawbuf[0].tu1 + icons.u;
		drawbuf[2].tv1 = drawbuf[0].tv1;
		drawbuf[2].tu2 = drawbuf[0].tu2 + icons.u;
		drawbuf[2].tv2 = drawbuf[2].tv1;
		//3
		drawbuf[3].x = drawbuf[0].x + icons.w;
		drawbuf[3].y = drawbuf[0].y + icons.h;
		drawbuf[3].z = 0.0f;
		drawbuf[3].rhw = 0.5f;
		drawbuf[3].c = color;
		drawbuf[3].tu1 = drawbuf[2].tu1;
		drawbuf[3].tv1 = drawbuf[0].tv1 + icons.v;
		drawbuf[3].tu2 = drawbuf[2].tu2;
		drawbuf[3].tv2 = drawbuf[3].tv1;
		//Рисуем полученое
		rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2, 2, drawbuf, sizeof(drawbuf[0]), "WdmDrawLabelIcon");
	}
}

//Найти направление для прибытия в заданную точку назначения из текущей
void WdmIslands::FindDirection(const CVECTOR & position, const CVECTOR & destination, CVECTOR & direction)
{
	//Если нет патча то едем по прямой
	if(!patch)
	{
		direction = Norm2D(destination - position);
		return;
	}
	//Пытаемся определить нод на котором стоим и нод к которому надо плыть
	float y = 0.0f;
	long from = patch->FindNode(position, y);
	y = 0.0f;
	long to = patch->FindNode(destination, y);
	if(from < 0 || to < 0)
	{
		direction = Norm2D(destination - position);
		return;
	}
	//Получаем направление
	if(!patch->FindPathDir(from, position, to, destination, to, direction))
	{
		direction = Norm2D(destination - position);
		return;
	}
	direction = Norm2D(direction - position);
}

//Найти силу отталкивания
void WdmIslands::FindReaction(const CVECTOR & position, CVECTOR & reaction)
{
	if(patch)
	{
		float y = 0.0f;
		long n = patch->FindNode(position, y);
		if(n >= 0)
		{
			patch->FindForce(n, position, 20.0f, reaction);
		}else{
			reaction = 0.0f;
		}
	}else{
		reaction = 0.0f;
	}
	
}

//Найти случайную точку для мерчанта
bool WdmIslands::GetRandomMerchantPoint(CVECTOR & p)
{
	if(merchants <= 0)
	{
		p = 0.0f;
		return false;
	}
	p = merchants[rand() % merchants];
	return true;
}

//Получить координаты квестового локатора
bool WdmIslands::GetQuestLocator(const char * locName, CVECTOR & p)
{
	if(!locName || !locName[0])
	{
		p = 0.0f;
		return false;
	}
	for(long i = 0; i < quests; i++)
	{
		if(quests[i].name == locName)
		{
			p = quests[i].pos;
			return true;
		}
	}
	p = 0.0f;
	return false;
}

//Проверить попадание кораблика в зону острова
bool WdmIslands::CheckIslandArea(const char * islandName, float x, float z)
{
	for(long i = 0; i < islands; i++)
	{
		if(!islands[i].area) continue;
		if(islands[i].modelName == islandName)
		{
			return IsShipInArea(i, CVECTOR(x, 0.0f, z));
		}
	}
	return false;
}

//Получить ближайшую точку к зоне острова
void WdmIslands::GetNearPointToArea(const char * islandName, float & x, float & z)
{
	//Ищим область
	for(long i = 0; i < islands; i++)
	{
		if(!islands[i].area) continue;
		if(islands[i].modelName == islandName)
		{
			break;
		}
	}
	if(i >= islands)
	{
		return;
	}
	//Перебираем рёбра выбирая ближайшую точку
	curPos.x = x;
	curPos.y = 0.0f;
	curPos.z = z;
	checkMode = false;
	GEOS::VERTEX v;
	v.x = 0.0f; v.y = 0.0f; v.z = 0.0f;
	islands[i].area->geo->Clip(null, 0, v, wdmObjects->worldSizeX*wdmObjects->worldSizeX + wdmObjects->worldSizeZ*wdmObjects->worldSizeZ, FindNearPoint);
	if(checkMode)
	{
		x = centPos.x;
		z = centPos.z;
	}
}

bool _cdecl WdmIslands::FindNearPoint(const GEOS::VERTEX * vrt, long numVrt)
{
	//Пропустим кривые данные
	for(long i = 0; i < numVrt; i++)
	{
		float dx = vrt[i].x - curPos.x;
		float dz = vrt[i].z - curPos.z;
		float dst = dx*dx + dz*dz;
		if(!checkMode || dst < centPos.y)
		{
			checkMode = true;
			centPos.x = vrt[i].x;
			centPos.z = vrt[i].z;
			centPos.y = dst;
		}		
	}
	return true;
}

bool WdmIslands::IsShipInArea(long islIndex, CVECTOR & pos)
{
	static const float testRadius = 0.01f;
	//Нет ареи нет пересечения
	if(!islands[islIndex].area) return false;
	//Проверяем соприкасаемость радиусов
	float dist2 = ~(pos - islands[islIndex].area->center);
	float rad = testRadius + islands[islIndex].area->radius;
	if(dist2 > rad*rad) return false;
	//Вертикальные плоскости
	GEOS::PLANE p[4];
	p[0].nrm.x = 0.0f; p[0].nrm.y = 0.0f; p[0].nrm.z = 1.0f; p[0].d = pos.z + testRadius;
	p[1].nrm.x = 0.0f; p[1].nrm.y = 0.0f; p[1].nrm.z = -1.0f; p[1].d = -(pos.z - testRadius);
	p[2].nrm.x = 1.0f; p[2].nrm.y = 0.0f; p[2].nrm.z = 0.0f; p[2].d = pos.x + testRadius;
	p[3].nrm.x = -1.0f; p[3].nrm.y = 0.0f; p[3].nrm.z = 0.0f; p[3].d = -(pos.x - testRadius);
	GEOS::VERTEX vrt;
	vrt.x = pos.x;
	vrt.y = 0.0f;
	vrt.z = pos.z;
	return islands[islIndex].area->geo->Clip(p, 4, vrt, 1000.0f, 0);
}




WdmIslandWaves::WdmIslandWaves()
{
	phase = rand()*(1.0f/RAND_MAX);
}

void WdmIslandWaves::Update(float dltTime)
{
	phase += dltTime*0.1f;
	while(phase > 1.0f) phase -= 1.0f;
}

void WdmIslandWaves::LRender(VDX8RENDER * rs)
{
	const float fullPeriod = 0.8f;
	float k = phase*(1.0f/fullPeriod);
	if(k <= 1.0f)
	{
		Render(rs, k);		
	}
	if(phase >= 0.5f)
	{
		k = phase - 0.5f;
	}else{
		k = phase + 0.5f;
	}
	k *= (1.0f/fullPeriod);
	if(k <= 1.0f)
	{
		Render(rs, k);		
	}
}

void WdmIslandWaves::Render(VDX8RENDER * rs, float k)
{
	//Ставим матрицу для текстуры
	//k = 0.5f + 0.5f*sinf(k*2.0f*PI);
	CMatrix mtx;
	mtx.m[1][1] = 0.3f + 1.0f*sinf(k*PI);
	mtx.m[2][1] = 0.0f;//0.2f*sinf((k - 0.25f)*2.0f*PI) - 0.5f;
	rs->SetTransform(D3DTS_TEXTURE0, mtx);
	//Ставим общую прозрачность
	float a = sinf(k*PI)*(1.0f - k*0.5f)*1.25f;
	//Рисуем модель
	SetTech("WdmIslandWaves", null);
	rs->SetRenderState(D3DRS_TEXTUREFACTOR, (long(a*a*a*a*255.0f) << 24) | 0xffffff);
	WdmRenderModel::LRender(rs);
}




