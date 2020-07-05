//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	WdmEventWindow
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "WdmEventWindow.h"
#include "WorldMap.h"


#define WDM_EW_WIDTHDIVHEIGHT	400.0f/200.0f	//Желаемое отношение сторон окна
#define WDM_EW_SCRSPACE			10.0f			//Отступы окна от экрана
#define WDM_EW_WSPACE			10.0f			//Отступы текста от окна по горизонтали
#define WDM_EW_UPSPACE			10.0f			//Отступы текста от верха окна
#define WDM_EW_DOWNSPACE		60.0f			//Отступ низа окна от текста
#define WDM_EW_MINW				100.0f			//Минимальная ширина окна
#define WDM_EW_BSW				30				//Ширина не скалируемого поля у кнопки
#define WDM_EW_BTSW				0.2f			//Ширина не скалируемого поля у кнопки на текстуре
#define WDM_EW_BUTTONH			36				//Высота кнопки

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmEventWindow::WdmEventWindow()
{
	headerFont = FONT_DEFAULT;
	headerWidth = 0;
	textFont = FONT_DEFAULT;
	textureBkg = -1;
	textureButton = -1;
	buttonFont = FONT_DEFAULT;
	text = null;
	token = null;
	numTokens = 0;
	maxTokens = 0;	
	textureBkg = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\eventbkg.tga");
	textureButton = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\button.tga");
	buttonPosY = 0;
	SetAnswers("Yes", 0);
	//Читаем атрибуты
	if(wdmObjects->wm->AttributesPointer)
	{
		ATTRIBUTES * at = wdmObjects->wm->AttributesPointer->FindAClass(wdmObjects->wm->AttributesPointer, "eventWindow");
		if(at) at = at->FindAClass(at, "font");
		if(at)
		{
			char * s = at->GetAttribute("header");
			if(s) headerFont = wdmObjects->rs->LoadFont(s);
			if(headerFont < 0) headerFont = FONT_DEFAULT;
			s = at->GetAttribute("text");
			if(s) textFont = wdmObjects->rs->LoadFont(s);
			if(textFont < 0) textFont = FONT_DEFAULT;
			s = at->GetAttribute("button");
			if(s) buttonFont = wdmObjects->rs->LoadFont(s);
			if(buttonFont < 0) buttonFont = FONT_DEFAULT;
		}
	}
}	

WdmEventWindow::~WdmEventWindow()
{
	if(text) delete text;
	if(token) delete token;
	if(textureBkg >= 0) wdmObjects->rs->TextureRelease(textureBkg);
	if(textureButton >= 0) wdmObjects->rs->TextureRelease(textureButton);
	if(headerFont != FONT_DEFAULT) wdmObjects->rs->UnloadFont(headerFont);
	if(textFont != FONT_DEFAULT) wdmObjects->rs->UnloadFont(headerFont);
	if(buttonFont != FONT_DEFAULT) wdmObjects->rs->UnloadFont(headerFont);
}

void WdmEventWindow::ActivateEvent(const char * etext, const char * _yes, const char * _no)
{
	SetAnswers(_yes, _no);
	if(text) delete text;
	if(etext)
	{
		long l = strlen(etext) + 1;
		text = NEW char[l];
		memcpy(text, etext, l);
	}else{
		text = NEW char[1];
		text[0] = 0;
	}
	SplitText();
	wdmObjects->isPause = true;
}

void WdmEventWindow::SplitText()
{
	if(!text[0]) return;
	VDX8RENDER * rs = wdmObjects->wm->GetRS();
	numTokens = 0;	
	long sw = 0;
	long maxsw = 0;
	//Ищем и выделяем заголовок
	for(char * ht = text; *ht; ht++)
		if(*ht == '#') break;
	if(*ht)
	{
		*ht = 0;
		headerWidth = rs->StringWidth(text, headerFont); 
		ht++;
	}else{
		headerWidth = 0;
		ht = text;
	}
	//Ищем токены
	for(char * tk = null, * t = ht; true; t++)
	{
		if(*t != ' ' && *t)
		{
			if(tk == null) tk = t;
		}else{
			bool isExit = !*t;
			if(tk)
			{
				*t = 0;
				//Добавляем токен
				if(numTokens >= maxTokens)
				{
					maxTokens += 64;
					token = (Token *)RESIZE(token, maxTokens*sizeof(Token));
				}
				token[numTokens].w = rs->StringWidth(tk, textFont);
				token[numTokens].string = tk;
				sw += token[numTokens].w;
				if(numTokens > 0)
				{
					if(maxsw < token[numTokens].w + token[numTokens - 1].w) maxsw = token[numTokens].w + token[numTokens - 1].w;
				}else{
					if(maxsw < token[numTokens].w) maxsw = token[numTokens].w;
				}
				tk = null;
				numTokens++;
			}
			if(isExit) break;
		}
	}
	long spaceW = rs->CharWidth(' ', textFont);
	maxsw += 2*spaceW;
	sw += numTokens*spaceW;
	long fontH = rs->CharHeight(textFont);
	//Предпологаемая ширина окна
	float w = sqrtf(float(sw*fontH))*WDM_EW_WIDTHDIVHEIGHT;
	if(w < float(maxsw)) w = float(maxsw);
	if(w < WDM_EW_MINW) w = WDM_EW_MINW;
	long butW = yesWidth > noWidth ? yesWidth : noWidth;
	if(yesWidth && noWidth) butW *= 2;
	if(w < butW + 5 + 2*WDM_EW_WSPACE) w = butW + 5 + 2*WDM_EW_WSPACE;
	float scrw, scrh;
	wdmObjects->GetVPSize(scrw, scrh);
	scrw -= 2*WDM_EW_SCRSPACE;
	scrh -= 2*WDM_EW_SCRSPACE;
	w += WDM_EW_WSPACE*2.0f;	
	if(w > scrw) w = scrw;
	winX = (scrw - w)*0.5f;
	winW = w;
	//Расчитываем координаты окна
	w -= 2*WDM_EW_WSPACE;
	float y = headerWidth ? rs->CharHeight(headerFont) + 2 : 0.0f;
	for(long i = 0; i < numTokens;)
	{
		//Смотрим сколько поместится на строке
		long strW = -spaceW;
		for(long j = i; j < numTokens && strW <= w; j++)
		{
			token[j].y = long(y);
			strW += token[j].w + spaceW;
		}
		//Количество токенов
		j -= i;
		//Если надо выкидываем последний
		if(strW > w && j > 1)
		{
			j--;
			strW -= token[i + j].w + spaceW;
		}
		//Расстояние между строками
		float sp = (w - (strW - spaceW*(j - 1)));
		if(j > 1) sp /= j - 1;
		if(i + j >= numTokens) sp = float(spaceW);
		//Раставляем координаты
		float x = winX + WDM_EW_WSPACE;
		for(long k = 0; k < j; k++)
		{
			token[i + k].x = long(x);
			x += token[i + k].w + sp;
		}
		y += fontH;
		i += j;
	}
	winH = y + WDM_EW_UPSPACE + WDM_EW_DOWNSPACE;
	buttonPosY = long(y);
	winY = (scrh - winH)*0.5f;
	y = winY + WDM_EW_UPSPACE;	
	for(i = 0; i < numTokens; i++) token[i].y += long(y);
}

//Установить варианты ответов
void WdmEventWindow::SetAnswers(const char * _yes, const char * _no)
{
	selButton = 0;
	if(_yes && _yes[0])
	{
		yesWidth = strlen(_yes) + 1;
		if(yesWidth > sizeof(yes)) yesWidth = sizeof(yes);
		memcpy(yes, _yes, yesWidth);
		yes[sizeof(yes) - 1] = 0;
		yesWidth = wdmObjects->rs->StringWidth(yes, buttonFont) + 2*WDM_EW_BSW;
	}else{
		yesWidth = 0;
		yes[0] = 0;
	}
	if(_no && _no[0])
	{
		noWidth = strlen(_no) + 1;
		if(noWidth > sizeof(no)) noWidth = sizeof(no);
		memcpy(no, _no, noWidth);
		no[sizeof(no) - 1] = 0;
		noWidth = wdmObjects->rs->StringWidth(no, buttonFont) + 2*WDM_EW_BSW;
	}else{
		noWidth = 0;
		no[0] = 0;
	}
	if(!yesWidth && !noWidth)
	{
		strcpy(yes, "Ok");
		yesWidth = wdmObjects->rs->StringWidth(yes, buttonFont);
		if(yesWidth < 8) yesWidth = 8;
		yesWidth += 2*WDM_EW_BSW;
	}
}

//Расчёты
void WdmEventWindow::Update(float dltTime)
{
	static long wait = 0;
	if(wait) wait--;

	CONTROL_STATE cs,cs1;
	_CORE_API->Controls->GetControlState("WMapILeft",cs);
	_CORE_API->Controls->GetControlState("WMapIRight",cs1);

	if(!text || !text[0]) return;
	//if(GetAsyncKeyState(VK_LEFT) < 0 || GetAsyncKeyState(VK_RIGHT) < 0)
	if(cs.state == CST_ACTIVATED || cs1.state == CST_ACTIVATED)
	{
		if(!wait)
		{
			wait = 10;
			selButton++;
			selButton &= 1;
			if(!yesWidth || !noWidth) selButton = 0;
			_CORE_API->Event("WorldMap_EventWindowChangeSelectoin", "l", selButton);
		}
	}
	else
	{
		_CORE_API->Controls->GetControlState("WMapAction",cs);
		//if(GetAsyncKeyState(VK_SPACE) < 0)
		if(cs.state == CST_ACTIVATED)
		{
			selButton &= 1;
			if(!yesWidth || !noWidth) selButton = 0;
			_CORE_API->Event("WorldMap_EventWindowSelect", "l", selButton);
			wdmObjects->isPause = false;
			text[0] = 0;
		}
	}
}

//Отрисовка
void WdmEventWindow::LRender(VDX8RENDER * rs)
{
	if(!text || !text[0]) return;
	Vertex buf[(3*2)*3];
	//Плашка с надписью
	rs->TextureSet(0, textureBkg);
	FillRectCoord(buf, winX - 32, winY - 2, winW + 70, winH + 22);
	FillRectUV(buf, 0.0f, 0.0f, 1.0f, 1.0f - 1.0f/64.0f);
	FillRectColor(buf, 0xdfffffff);
	DrawRects(buf, 1, "WdmDrawMapBlend");
	//Кнопки
	long bsize = yesWidth > noWidth ? yesWidth : noWidth;
	long bw, btab;
	if(yesWidth && noWidth)
	{
		bw = bsize*2;
		btab = 10;
	}else{
		bw = bsize;
		btab = 0;
	}
	Assert(bw);
	float byesx = winX + (winW - (bw + btab))*0.5f;
	float bnox = byesx + bsize + btab;
	float by = winY + buttonPosY + WDM_EW_UPSPACE + 15.0f;
	float btY = by + (WDM_EW_BUTTONH - rs->CharHeight(buttonFont))*0.5f;
	bool isYesActive = !noWidth || ((selButton & 1) == 0);
	bool isNoActive = !yesWidth || ((selButton & 1) != 0);
	if(yesWidth)
	{
		memset(buf, 0, sizeof(buf));
		
		FillSRectCoord(buf, byesx, by, float(bsize), WDM_EW_BUTTONH, WDM_EW_BSW);
		FillSRectUV(buf, 0.0f, isYesActive ? 0.5f : 0.0f, 1.0f, 0.5f, WDM_EW_BTSW);
		FillSRectColor(buf);
		rs->TextureSet(0, textureButton);
		DrawRects(buf, 3);
		Print(buttonFont, 0xffffffff, byesx, float(byesx + bsize), btY, yes);
	}
	if(noWidth)
	{
		FillSRectCoord(buf, bnox, by, float(bsize), WDM_EW_BUTTONH, WDM_EW_BSW);
		FillSRectUV(buf, 0.0f, isNoActive ? 0.5f : 0.0f, 1.0f, 0.5f, WDM_EW_BTSW);
		FillSRectColor(buf);
		rs->TextureSet(0, textureButton);
		DrawRects(buf, 3);
		Print(buttonFont, 0xffffffff, bnox, float(bnox + bsize), btY, no);
	}
	//Текст
	if(headerWidth) rs->Print(headerFont, 0xffffffff, long(winX + (winW - headerWidth)*0.5f), long(winY + WDM_EW_UPSPACE), text);
	for(long i = 0; i < numTokens; i++) 
		rs->Print(textFont, 0xffffffff, token[i].x, token[i].y, token[i].string);
	
	//Подгонка размеров окна
	//rs->Print(textFont, 0xffffffff, long(winX), long(winY), "HHHHHHH");
	//rs->Print(textFont, 0xffffffff, long(winX + winW), long(winY), "HHHHHHH");
	//rs->Print(textFont, 0xffffffff, long(winX), long(winY + winH), "HHHHHHHHHHHHHHHHHHHHHH");
}
