//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	WdmLocations
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "WdmLocations.h"
#include "WorldMap.h"
#include "WdmCamera.h"

//============================================================================================

//Прозрачность в зависимости от высоты
float WdmLocations::heightAlpha = 1.0f;

//============================================================================================

WdmLocations::Location::Location()
{
	model = null;
	isVisible = false;
	isDebugMove = false;
}

WdmLocations::Location::~Location()
{
	if(model) model->killMe = true;
}

bool WdmLocations::Location::SetModel(char * modelName)
{
	if(model) model->killMe = false;
	model = (WdmRenderModel *)wdmObjects.wm->CreateModel(NEW WdmRenderModel(), modelName, false, false);
	if(model == null) return false;
	model->alpha = alpha = isVisible ? 1.0f : 0.0f;
	return true;
}

void WdmLocations::Location::SetVisible(bool isVis)
{
	isVisible = isVis;
	alpha = isVisible ? 1.0f : 0.0f;
	if(model) model->alpha = alpha;
}

bool WdmLocations::Location::Update(float dltTime)
{
	if(!model) return false;
	if(isVisible)
	{
		alpha += dltTime*1.0f;
		if(alpha > 1.0f)
		{
			alpha = 1.0f;
			return false;
		}
		return true;
	}else alpha = 0.0f;
	return false;	
}

//============================================================================================


WdmLocations::Label::Label()
{
	text = null;
	isVisible = false;
	isIsland = false;
	t[0].icon = -1;
	t[0].alpha = 0.0f;
	t[1] = t[0];
	alpha = 0.0f;
	x = y = 0.0f;
}

WdmLocations::Label::~Label()
{
	if(text) delete text;
}

void WdmLocations::Label::SetText(const char * t)
{
	Assert(text != t);
	if(text) delete text;
	if(!t && !t[0])
	{
		text = NEW char[1];
		text[0] = 0;
		return;
	}
	long slen = strlen(t) + 1;
	text = NEW char[slen];
	memcpy(text, t, slen);
}

void WdmLocations::Label::SetIcon(long icon)
{
	if(t[0].icon != icon)
	{
		t[1] = t[0];
		t[0].icon = icon;
		t[0].alpha = isVisible ? 0.0f : 1.0f;
		if(!isVisible) t[1].alpha = 0.0f;
	}
}

void WdmLocations::Label::SetPivot(float px, float py)
{
	x = px;
	y = py;
}

void WdmLocations::Label::SetVisible(bool isVis)
{
	isVisible = isVis;
	alpha = isVisible ? 1.0f : 0.0f;
	t[0].alpha = alpha;
	t[1].icon = -1;
	t[1].alpha = 0.0f;
}

bool WdmLocations::Label::IsHave()
{
	return (t[0].icon >= 0 || text);
}

bool WdmLocations::Label::IsVisible()
{
	return IsHave() && Alpha() >= 1.0;
}

float WdmLocations::Label::Alpha()
{
	if(isIsland) return alpha*255.0f;
	return alpha*heightAlpha*255.0f;
}

bool WdmLocations::Label::Update(float dltTime, WdmLocations * loc)
{	
	bool isChanged = false;
	//Изменяем параметры основной плашки
	if(isVisible)
	{
		alpha += dltTime*1.0f;
		if(alpha < 1.0f) isChanged = true; else alpha = 1.0f;
		t[0].alpha += dltTime*1.0f;
		if(t[0].alpha < 1.0f) isChanged = true; else t[0].alpha = 1.0f;
	}else{
		alpha -= dltTime*1.0f;
		if(alpha > 0.0f) isChanged = true; else alpha = 0.0f;
		t[0].alpha -= dltTime*1.0f;
		if(t[0].alpha > 0.0f) isChanged = true; else t[0].alpha = 0.0f;
	}
	//Изменяем параметры дополнительной плашки
	t[1].alpha -= dltTime*1.0f;
	if(t[1].alpha > 0.0f) isChanged = true; else t[1].alpha = 0.0f;
	//Пересчитаем позиции иконки и текста
	VDX8RENDER * rs = wdmObjects.wm->GetRS();
	//Размеры иконки
	float iw = loc->iconWidth;
	float ih = loc->iconHeight;
	if(t[0].icon < 0 && t[1].icon < 0)
	{
		iw = 0.0f; ih = 0.0f;
	}
	//Размеры строки
	float strw = float(rs->StringWidth(text, font));
	float strh = float(rs->CharHeight(font));
	//Отступ между иконкой и текстом
	const float tab = 3.0f;
	//Общая длинна и высота
	w = iw + tab + strw;
	h = ih > strh ? ih : strh;
	//Позиция иконки
	ix = 0.0f;
	iy = (h - ih)*0.5f;
	//Позиция строки
	sx = w - strw;
	sy = (h - strh)*0.5f;
	//Вернём результат, были ли изменения
	return isChanged;
}

//============================================================================================

WdmLocations::IslandInfo::IslandInfo()
{
	name = null;
	inside = false;
	x = 0.0f; y = 0.0f; z = 0.0f;
	rx = rz = 0.0f;
	radius2 = 0.0f;
	isNeedUpdate = true;
	numLabels = 0;
}

WdmLocations::IslandInfo::~IslandInfo()
{
	if(name) delete name;
};

void WdmLocations::IslandInfo::SetName(char * n)
{
	if(name) delete name;
	name = null;
	if(n && n[0])
	{
		name = NEW char[strlen(n) + 1];
		strcpy(name, n);
	}
}

void WdmLocations::IslandInfo::Update(float dltTime, WdmLocations * loc)
{
	if(isNeedUpdate)
	{
		isNeedUpdate = false;
		for(long i = 0; i < WDM_LOCATIONS_MAXLOC; i++)
		{
			isNeedUpdate |= locations[i].Update(dltTime);
		}
		for(i = 0; i <= WDM_LOCATIONS_MAXLOC; i++)
		{
			isNeedUpdate |= label[i].Update(dltTime, loc);
		}
	}
	for(long i = 0; i < WDM_LOCATIONS_MAXLOC; i++)
	{
		//if(locations[i].model) locations[i].model->alpha = locations[i].alpha*heightAlpha;
		if(locations[i].model) locations[i].model->alpha = locations[i].alpha;
	}
}

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmLocations::WdmLocations()
{
	wdmObjects.locations = this;
	islandsInfo = null;
	iconTexture = -1;
	labelFontIslands = -1;
	labelFontLocations = -1;
	heightViewLabel = 150.0f;
	iconWidth = 32;
	iconHeight = 32;
	iconNum = 1;
	iconFrames = 1;
	curFrame = 0.0f;
	iconFps = 8;
	strcpy(iconTextureName, "icons.tga");
	isEnablePause = false;
	curDebugIsland = -1;
	curDebugLocation = -1;
}

WdmLocations::~WdmLocations()
{
	Release();
	wdmObjects.locations = null;
	if(iconTexture >= 0) wdmObjects.rs->TextureRelease(iconTexture);
}

//Зачитать данные об островах и локациях
void WdmLocations::SetIslandsData(ATTRIBUTES * apnt)
{
	VDX8RENDER * rs = wdmObjects.wm->GetRS();
	//Общее
	char * f = apnt->GetAttribute("fontIslands");
	if(!f) f = "normal";
	labelFontIslands = rs->LoadFont(f);
	f = apnt->GetAttribute("fontLocations");
	if(!f) f = "normal";
	labelFontLocations = rs->LoadFont(f);
	labelPivotX = apnt->GetAttributeAsFloat("labelPivotX", 0.5f);
	labelPivotY = apnt->GetAttributeAsFloat("labelPivotY", 0.5f);
	heightViewLabel = apnt->GetAttributeAsFloat("heightViewLabel", heightViewLabel);
	//Параметры иконок
	ATTRIBUTES * ap = apnt->FindAClass(apnt, "icon");
	if(ap)
	{
		iconWidth = ap->GetAttributeAsFloat("width", iconWidth);
		iconHeight = ap->GetAttributeAsFloat("height", iconHeight);
		iconNum = ap->GetAttributeAsDword("num", iconNum);
		iconFrames = ap->GetAttributeAsDword("frames", iconFrames);
		iconFps = ap->GetAttributeAsFloat("fps", iconFps);
		if(iconFps < 0) iconFps = 0;
		f = ap->GetAttribute("texture");
		if(f) strcpy(iconTextureName, f);
	}
	//Грузим текстуру
	char itPath[128];
	strcpy(itPath, "\\WorldMap\\Interfaces\\");
	strcat(itPath, f);
	iconTexture = rs->TextureCreate(itPath);
	//Найти раздел с островами
	ap = apnt->FindAClass(apnt, "islands");
	if(!ap) return;
	//Удаляем старое
	Release();
	//Зачитываем значения атрибутов
	numIslands = ap->GetAttributesNum();
	if(numIslands <= 0) return;
	islandsInfo = NEW IslandInfo[numIslands];
	for(long i = 0; i < numIslands; i++)
	{
		islandsInfo[i].isNeedUpdate = true;
		//Остров
		ATTRIBUTES * aIsland = ap->GetAttributeClass(i);
		islandsInfo[i].SetName(aIsland->GetThisName());
		ATTRIBUTES * a = aIsland->FindAClass(aIsland, "position");
		if(a)
		{
			islandsInfo[i].x = a->GetAttributeAsFloat("x", 0);
			islandsInfo[i].y = a->GetAttributeAsFloat("y", 0);
			islandsInfo[i].z = a->GetAttributeAsFloat("z", 0);
			islandsInfo[i].rx = a->GetAttributeAsFloat("rx", 0);
			islandsInfo[i].rz = a->GetAttributeAsFloat("rz", 0);
			islandsInfo[i].radius2 = aIsland->GetAttributeAsFloat("radius", 0);
			islandsInfo[i].radius2 *= islandsInfo[i].radius2;
			islandsInfo[i].kRadius = aIsland->GetAttributeAsFloat("kradius", 0.8f);
			if(islandsInfo[i].kRadius < 0.2f) islandsInfo[i].kRadius = 0.2f;
			if(islandsInfo[i].kRadius > 0.99f) islandsInfo[i].kRadius = 0.99f;
		}
		InitLabel(islandsInfo[i].label[0], aIsland->FindAClass(aIsland, "label"), true, true);
		//Локации на острове
		a = aIsland->FindAClass(aIsland, "locations");
		if(a)
		{
			for(long j = 0; j < long(a->GetAttributesNum()) && j < WDM_LOCATIONS_MAXLOC; j++)
			{
				ATTRIBUTES * loc = a->GetAttributeClass(j);
				//Позиция города
				ATTRIBUTES * pos = loc->FindAClass(loc, "position");
				if(pos)
				{
					float x = pos->GetAttributeAsFloat("x", 0);
					float y = pos->GetAttributeAsFloat("y", 0);
					float z = pos->GetAttributeAsFloat("z", 0);
					float ay = pos->GetAttributeAsFloat("ay", 0);
					char * str = loc->GetAttribute("visible");
					bool isVisible = str ? (stricmp(str, "true") == 0) : false;
					str = loc->GetAttribute("move");
					bool isDebugMove = str ? (stricmp(str, "true") == 0) : false;
					//Создаём локацию
					if(islandsInfo[i].locations[j].SetModel(loc->GetAttribute("modelName"))
						|| islandsInfo[i].locations[j].SetModel("def_location"))
					{
						islandsInfo[i].locations[j].model->mtx.BuildMatrix(0.0f, ay, 0.0f, x, y, z);
						islandsInfo[i].locations[j].isVisible = isVisible;
						islandsInfo[i].locations[j].isDebugMove = isDebugMove;
						islandsInfo[i].locations[j].ay = ay;
						//Инициализируем метку
						InitLabel(islandsInfo[i].label[j + 1], loc->FindAClass(loc, "label"), false, true);
					}
				}
			}
		}
		UpdateLabelPriority(islandsInfo[i]);
	}
	Update(0);
}

//Изменение значения атрибутов видимости
void WdmLocations::AttributeChanged(ATTRIBUTES * apnt)
{
	char * s;
	//Общее
	s = apnt->GetAttribute("debug");
	wdmObjects.isDebug = s && (stricmp(s, "true") == 0);
	//Найти раздел с островами
	ATTRIBUTES * ap = apnt->FindAClass(apnt, "islands");
	if(!ap) return;
	//Зачитываем значения атрибутов
	long n = ap->GetAttributesNum();
	if(n > numIslands) n = numIslands;
	if(n <= 0) return;
	for(long i = 0; i < n; i++)
	{
		islandsInfo[i].isNeedUpdate = true;
		//Остров
		ATTRIBUTES * aIsland = ap->GetAttributeClass(i);
		InitLabel(islandsInfo[i].label[0], aIsland->FindAClass(aIsland, "label"), true);
		ATTRIBUTES * a = aIsland->FindAClass(aIsland, "locations");
		if(a)
		{
			for(long j = 0; j < long(a->GetAttributesNum()) && j < WDM_LOCATIONS_MAXLOC; j++)
			{
				ATTRIBUTES * loc = a->GetAttributeClass(j);
				//Позиция города
				ATTRIBUTES * pos = loc->FindAClass(loc, "position");
				if(pos)
				{
					float x = pos->GetAttributeAsFloat("x", 0);
					float y = pos->GetAttributeAsFloat("y", 0);
					float z = pos->GetAttributeAsFloat("z", 0);
					float ay = pos->GetAttributeAsFloat("ay", 0);
					char * str = loc->GetAttribute("visible");
					bool isVisible = str ? (stricmp(str, "true") == 0) : false;
					str = loc->GetAttribute("move");
					bool isDebugMove = str ? (stricmp(str, "true") == 0) : false;
					islandsInfo[i].locations[j].model->mtx.BuildMatrix(0.0f, ay, 0.0f, x, y, z);
					islandsInfo[i].locations[j].isVisible = isVisible;
					islandsInfo[i].locations[j].isDebugMove = isDebugMove;
					islandsInfo[i].locations[j].ay = ay;
					InitLabel(islandsInfo[i].label[j + 1], loc->FindAClass(loc, "label"), false);
				}
			}
		}
		UpdateLabelPriority(islandsInfo[i]);
	}
}

void WdmLocations::Release()
{
	if(islandsInfo) delete [] islandsInfo;
	islandsInfo = null;
	numIslands = 0;

}

void WdmLocations::InitLabel(Label & label, ATTRIBUTES * apnt, bool isIsl, bool isSet)
{
	if(!apnt) return;	
	char * text = apnt->GetAttribute("text");
	long icon = apnt->GetAttributeAsDword("icon", -1);
	label.SetText(text);
	label.SetIcon(icon);
	label.SetPivot(labelPivotX, labelPivotY);
	label.isIsland = isIsl;
	if(isIsl)
	{
		label.font = labelFontIslands >= 0 ? labelFontIslands : FONT_DEFAULT;
	}else{
		label.font = labelFontLocations >= 0 ? labelFontLocations : FONT_DEFAULT;
	}
	char * vf = apnt->GetAttribute("visible");	
	if(isSet) label.SetVisible(vf ? stricmp(vf, "true") == 0 : false);
		else label.isVisible = vf ? stricmp(vf, "true") == 0 : false;	
}

void WdmLocations::UpdateLabelPriority(IslandInfo & ii)
{
	ii.numLabels = 0;
	//Если размер острова крив не учитываем его
	if(ii.radius2 < 0.0001f) return;
	//Считаем радиусы до локаций от центра острова
	float rad[WDM_LOCATIONS_MAXLOC];	
	for(long i = 0; i < WDM_LOCATIONS_MAXLOC; i++)
	{
		if(ii.locations[i].model)
		{
			float dx = ii.locations[i].model->mtx.Pos().x - ii.x;
			float dz = ii.locations[i].model->mtx.Pos().z - ii.z;
			rad[i] = dx*dx + dz*dz;
		}else rad[i] = -1.0f;
	}
	//Добавляем в список метки
	//Острова
	if(ii.label[0].IsHave()) ii.labelPrt[ii.numLabels++] = 0;
	//Локаций
	for(i = 0; i < WDM_LOCATIONS_MAXLOC; i++)
	{
		float maxRad;
		long k = -1;
		for(long j = 0; j < WDM_LOCATIONS_MAXLOC; j++)
		{
			if(rad[j] >= 0.0f)
			{
				if(k >= 0)
				{
					if(rad[j] < maxRad)
					{
						maxRad = rad[j];
						k = j;
					}
				}else{
					maxRad = rad[j];
					k = j;
				}
			}
		}
		if(k >= 0)
		{	
			//Пропишем индекс метки, если метка нормальная
			if(ii.label[k + 1].IsHave()) ii.labelPrt[ii.numLabels++] = k + 1;
			rad[k] = -1;
		}else break;
	}	
}


//Расчёты
void WdmLocations::Update(float dltTime)
{
	if(!wdmObjects.playerShip) return;
	//Позиция корабля игрока
	float sx = ((WdmRenderModel *)wdmObjects.playerShip)->mtx.Pos().x;
	float sz = ((WdmRenderModel *)wdmObjects.playerShip)->mtx.Pos().z;
	//Смотрим попадание корабля персонажа в зону островов
	for(long i = 0; i < numIslands; i++)
	{
		float dx = islandsInfo[i].x - sx;
		float dz = islandsInfo[i].z - sz;
		float r = dx*dx + dz*dz;
		if(r < islandsInfo[i].radius2)
		{
			if(!islandsInfo[i].inside)
			{
				islandsInfo[i].inside = true;
				_CORE_API->Event("WorldMap_inside_islandzone", "s", islandsInfo[i].name);
				wdmObjects.curIsland = islandsInfo[i].name;
			}
		}else{
			if(islandsInfo[i].inside)
			{
				islandsInfo[i].inside = false;
				_CORE_API->Event("WorldMap_outside_islandzone", "s", islandsInfo[i].name);
				wdmObjects.curIsland = null;
			}
		}
		islandsInfo[i].Update(dltTime, this);
#ifndef _XBOX
		if(wdmObjects.isDebug)
		{
			float dlt = dltTime*5.0f;
			if(GetAsyncKeyState(VK_CONTROL) < 0) dlt *= 5.0f;
			if(GetAsyncKeyState(VK_SHIFT) < 0) dlt *= 20.0f;
			for(long j = 0; j < WDM_LOCATIONS_MAXLOC; j++)
			{
				if(!islandsInfo[i].locations[j].model) continue;
				if(!islandsInfo[i].locations[j].isDebugMove) continue;
				if(GetAsyncKeyState(VK_UP) < 0)
				{
					islandsInfo[i].locations[j].model->mtx.Pos() += CVECTOR(0.0f, 0.0f, dlt);
				}else
				if(GetAsyncKeyState(VK_DOWN) < 0)
				{
					islandsInfo[i].locations[j].model->mtx.Pos() -= CVECTOR(0.0f, 0.0f, dlt);
				}else
				if(GetAsyncKeyState(VK_LEFT) < 0)
				{
					islandsInfo[i].locations[j].model->mtx.Pos() -= CVECTOR(dlt, 0.0f, 0.0f);
				}else
				if(GetAsyncKeyState(VK_RIGHT) < 0)
				{
					islandsInfo[i].locations[j].model->mtx.Pos() += CVECTOR(dlt, 0.0f, 0.0f);
				}else
				if(GetAsyncKeyState(VK_HOME) < 0)
				{
					islandsInfo[i].locations[j].model->mtx.Pos() += CVECTOR(0.0f, dlt, 0.0f);
				}else
				if(GetAsyncKeyState(VK_END) < 0)
				{
					islandsInfo[i].locations[j].model->mtx.Pos() -= CVECTOR(0.0f, dlt, 0.0f);
				}else
				if(GetAsyncKeyState(VK_PRIOR) < 0)
				{
					islandsInfo[i].locations[j].ay += dlt*0.1f;
					islandsInfo[i].locations[j].model->mtx.BuildMatrix(CVECTOR(0.0f, islandsInfo[i].locations[j].ay, 0.0f), CVECTOR(islandsInfo[i].locations[j].model->mtx.Pos()));
				}else
				if(GetAsyncKeyState(VK_NEXT) < 0)
				{
					islandsInfo[i].locations[j].ay -= dlt*0.1f;
					islandsInfo[i].locations[j].model->mtx.BuildMatrix(CVECTOR(0.0f, islandsInfo[i].locations[j].ay, 0.0f), CVECTOR(islandsInfo[i].locations[j].model->mtx.Pos()));
				}
				if(GetAsyncKeyState(VK_RETURN) < 0)
				{
					api->Trace("i:%i l:%i  (%f, %f, %f) ay:%f", i, j, 
						islandsInfo[i].locations[j].model->mtx.Pos().x, 
						islandsInfo[i].locations[j].model->mtx.Pos().y, 
						islandsInfo[i].locations[j].model->mtx.Pos().z,
						islandsInfo[i].locations[j].ay);
				}
			}
		}
#endif
	}
	curFrame += iconFps*dltTime;
	while(curFrame >= iconFrames) curFrame -= iconFrames;// ;)
	//if(wdmObjects.camera->pos.y < heightViewLabel)
	if(wdmObjects.camera->realHeight < heightViewLabel)
	{
		heightAlpha += 1.5f*dltTime;
		if(heightAlpha > 1.0f) heightAlpha = 1.0f;
	}else{
		heightAlpha -= 1.5f*dltTime;
		if(heightAlpha < 0.0f) heightAlpha = 0.0f;
	}
}

//Отрисовка
void WdmLocations::LRender(VDX8RENDER * rs)
{
	//Если отладочный режим, то рисуем отладочную информацию
	if(islandsInfo && wdmObjects.isDebug)
	{
		for(long i = 0; i < numIslands; i++)
		{
			wdmObjects.DrawCircle(CVECTOR(islandsInfo[i].x, 0.1f, islandsInfo[i].z), sqrtf(islandsInfo[i].radius2), 0x4fff0000);
			wdmObjects.DrawCircle(CVECTOR(islandsInfo[i].x, 0.1f, islandsInfo[i].z), sqrtf(islandsInfo[i].radius2)*islandsInfo[i].kRadius, 0x4f00ff00);
		}
	}
	//Рисуем метки
	DrawLabels(rs);
}

void WdmLocations::DrawLabels(VDX8RENDER * rs)
{
	//Получим текущую матрицу преобразования
	static CMatrix mtx, view, prj;
	rs->GetTransform(D3DTS_VIEW, view);
	rs->GetTransform(D3DTS_PROJECTION, prj);
	mtx.EqMultiply(view, prj);
	//Получим текущие размеры vp
	static D3DVIEWPORT8 vp;
	rs->GetViewport(&vp);
	float w = vp.Width*0.5f;
	float h = vp.Height*0.5f;
	//Буфера для проецирования
	static CVECTOR lvrt[WDM_LOCATIONS_MAXLOC + 1];
	static struct Vrt : public MTX_PRJ_VECTOR
	{
		long label;
	} vrt[WDM_LOCATIONS_MAXLOC + 1];
	//Буфер для раздвирания меток
	//Прямоугольник, описывающий 
	static struct
	{
		float l, t, r, b;
	} lbr[WDM_LOCATIONS_MAXLOC + 1];
	//Рисуем метки
	for(long i = 0; i < numIslands; i++)
	{
		IslandInfo & island = islandsInfo[i];
		//Пропустим кривые острова
		if(island.radius2 < 0.0001f) continue;
		if(island.numLabels <= 0) continue;
		//Находим экранные координаты меток
		for(long j = 0; j < island.numLabels; j++)
		{
			long k = island.labelPrt[j];
			if(k > 0)
			{
				//Метка локации
				lvrt[j] = island.locations[k - 1].model->mtx.Pos();
			}else{
				//Метка острова
				lvrt[j] = CVECTOR(island.x, 0.0f, island.z);
			}
			vrt[j].label = k;			
		}
		mtx.Projection(lvrt, vrt, island.numLabels, w, h, sizeof(CVECTOR), sizeof(vrt[0]));
		//Размещаем метки, чтобы не пересекались по приоритету
		long numRects = 0;
		if(island.label[vrt[0].label].IsVisible())
		{
			lbr[0].l = vrt[0].x - island.label[vrt[0].label].x*island.label[vrt[0].label].w;
			lbr[0].t = vrt[0].y - island.label[vrt[0].label].y*island.label[vrt[0].label].h;
			lbr[0].r = lbr[0].l + island.label[vrt[0].label].w;
			lbr[0].b = lbr[0].t + island.label[vrt[0].label].h;
			numRects++;
		}
		for(j = 1; j < island.numLabels; j++)
		{
			if(!island.label[vrt[j].label].IsVisible()) continue;
			//Прямоугольник текущей метки
			float l = vrt[j].x - island.label[vrt[j].label].x*island.label[vrt[j].label].w;
			float t = vrt[j].y - island.label[vrt[j].label].y*island.label[vrt[j].label].h;
			float r = l + island.label[vrt[j].label].w;
			float b = t + island.label[vrt[j].label].h;
			//Направление перемещение
			long dx = 0;
			long dy = 0;
			//Ищем пересечения с добавленными
			for(long k = 0; k < numRects; k++)
			{
				//Пропустим если нет пересечения
				if(l >= lbr[k].r) continue;
				if(r <= lbr[k].l) continue;
				if(t >= lbr[k].b) continue;
				if(b <= lbr[k].t) continue;
				//Определим направлеине перемещения
				float ddx = 0.0f;
				float ddy = 0.0f;
				if(dx == 0) dx = ((lbr[k].r + lbr[k].l) < (r + l)) ? 1 : -1;
				if(dy == 0) dy = ((lbr[k].b + lbr[k].t) < (b + t)) ? 1 : -1;
				if(dx > 0) ddx = lbr[k].r - l; else ddx = lbr[k].l - r;
				if(dy > 0) ddy = lbr[k].b - t; else ddy = lbr[k].t - b;
				if(fabsf(ddx) < fabsf(ddy))
				{
					l += ddx;
					r += ddx;
					vrt[j].x += ddx;
				}else{
					t += ddy;
					b += ddy;
					vrt[j].y += ddy;
				}
			}
			//Заносим в список прямоугольник
			lbr[numRects].l = l;
			lbr[numRects].t = t;
			lbr[numRects].r = r;
			lbr[numRects].b = b;
			numRects++;
		}
		//Рисуем иконоки и пишем текст
		float ktu = 0.0f;
		float ktv = 0.0f;
		for(j = 0; j < island.numLabels; j++)
		{
			Label & label = island.label[vrt[island.numLabels - j - 1].label];
			if(!label.IsVisible()) continue;
			float x = vrt[island.numLabels - j - 1].x - label.x*label.w;
			float y = vrt[island.numLabels - j - 1].y - label.y*label.h;
			//Иконки
			DrawIcon(rs, x, y, label);
			dword color = (dword(label.Alpha()) << 24) | 0xffffff;
			//Текст
			rs->Print(label.font, color, long(x + label.sx), long(y + label.sy), label.text);
		}
	}
	//RenderState
	//rs->SetRenderState(D3DRS_ZENABLE, TRUE);
	//rs->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//TextureStageState
	//rs->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
}

//Найти реакцию от островов для корабля
void WdmLocations::FindReaction(float x, float z, float & rx, float & rz)
{
	rx = 0.0f;
	rz = 0.0f;
	for(long i = 0; i < numIslands; i++)
	{
		IslandInfo & island = islandsInfo[i];
		float dx = x - island.x;
		float dz = z - island.z;
		float r = dx*dx + dz*dz - island.radius2*islandsInfo[i].kRadius*islandsInfo[i].kRadius;
		if(r < 0.01f) r = 0.01f;
		r = 1.0f/powf(r, 0.8f);
		rx += dx*r;
		rz += dz*r;		
	}
	float rl = rx*rx + rz*rz;
	if(rl > 1.0f)
	{
		rl = 1.0f/sqrtf(rl);
		rx *= rl;
		rz *= rl;
	}
}

//Найти ближайший остров
bool WdmLocations::FindGoodIsland(float shipX, float shipZ, float & x, float & z, float & r, bool testInZone, bool retPivot)
{
	//Ищем самый удалённый остров
	if(numIslands <= 0) return false;
	float dst;
	for(long i = 0, j = -1; i < numIslands; i++)
	{
		float dx = islandsInfo[i].x - shipX;
		float dz = islandsInfo[i].z - shipZ;
		float d = dx*dx + dz*dz;
		if(testInZone && d >= islandsInfo[i].radius2) continue;
		if(j < 0)
		{
			j = i;
			dst = d;
		}else{
			if(dst < d)
			{
				dst = d;
				j = i;
			}
		}
	}
	if(j < 0) return false;
	if(!retPivot)
	{
		x = islandsInfo[j].x;
		z = islandsInfo[j].z;
	}else{
		x = islandsInfo[j].rx;
		z = islandsInfo[j].rz;
	}
	r = sqrtf(islandsInfo[j].radius2);
	return true;
}

void WdmLocations::DrawIcon(VDX8RENDER * rs, float x, float y, Label & label)
{
	//Буфер для рисования треугольников
	static struct
	{
		float x, y, z, rhw;
		dword color;
		float tu, tv;
	} drawbuf[6];
	dword color;
	if(label.t[0].icon < 0 && label.t[1].icon < 0) return;
	//Настраиваем режим рендеринга
	//Texture
	rs->TextureSet(0, iconTexture);
	//Текстурные координаты
	float ktu = 1.0f/iconFrames;
	float ktv = 1.0f/iconNum;
	//Заполняем треугольники
	//1
	drawbuf[0].x = x + label.ix;
	drawbuf[0].y = y + label.iy;
	drawbuf[0].z = 0.0f; drawbuf[0].rhw = 0.5f;
	drawbuf[0].tu = long(curFrame)*ktu;
	//2
	drawbuf[1].x = x + iconWidth;
	drawbuf[1].y = y;
	drawbuf[1].z = 0.0f; drawbuf[1].rhw = 0.5f;
	drawbuf[1].tu = long(curFrame + 1)*ktu - 1.0f/256.0f;
	//3
	drawbuf[2].x = x;
	drawbuf[2].y = y + iconHeight;
	drawbuf[2].z = 0.0f; drawbuf[2].rhw = 0.5f;
	drawbuf[2].tu = long(curFrame)*ktu;
	
	//4
	drawbuf[3].x = x + iconWidth;
	drawbuf[3].y = y + iconHeight;
	drawbuf[3].z = 0.0f; drawbuf[3].rhw = 0.5f;
	drawbuf[3].tu = long(curFrame + 1)*ktu - 1.0f/256.0f;
	//Прозрачность
	Assert(label.alpha >= 0.0f && label.alpha <= 1.0f);
	Assert(label.t[0].alpha >= 0.0f && label.t[0].alpha <= 1.0f);
	Assert(label.t[1].alpha >= 0.0f && label.t[1].alpha <= 1.0f);
	//Рисуем содержимое массива
	if(label.t[1].icon >= 0 && label.t[1].alpha > 0.0f)
	{
		color = (dword(label.Alpha()*label.t[1].alpha) << 24) | 0xffffff;
		if(color & 0xff000000)
		{
			drawbuf[0].color = color;
			drawbuf[0].tv = label.t[1].icon*ktv;
			drawbuf[1].color = color;
			drawbuf[1].tv = label.t[1].icon*ktv;
			drawbuf[2].color = color;
			drawbuf[2].tv = (label.t[1].icon + 1)*ktv;
			drawbuf[3].color = color;
			drawbuf[3].tv = (label.t[1].icon + 1)*ktv;
			drawbuf[4] = drawbuf[2];				
			drawbuf[5] = drawbuf[1];
			rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, 2, drawbuf, sizeof(drawbuf[0]), "WdmDrawLabelIcon");
		}
	}
	color = (dword(label.Alpha()*label.t[0].alpha) << 24) | 0xffffff;
	if(color & 0xff000000)
	{
		drawbuf[0].color = color;		
		drawbuf[0].tv = label.t[0].icon*ktv;
		drawbuf[1].color = color;		
		drawbuf[1].tv = label.t[0].icon*ktv;
		drawbuf[2].color = color;		
		drawbuf[2].tv = (label.t[0].icon + 1)*ktv;
		drawbuf[3].color = color;		
		drawbuf[3].tv = (label.t[0].icon + 1)*ktv;
		drawbuf[4] = drawbuf[2];				
		drawbuf[5] = drawbuf[1];
		rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, 2, drawbuf, sizeof(drawbuf[0]), "WdmDrawLabelIcon");
	}
}
