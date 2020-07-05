//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	WdmDateLabel
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "WdmDateLabel.h"
#include "WorldMap.h"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmDateLabel::WdmDateLabel()
{
	txSkyClock = -1;
	strcpy(month[0], "january");
	strcpy(month[1], "february");
	strcpy(month[2], "march");
	strcpy(month[3], "april");
	strcpy(month[4], "may");
	strcpy(month[5], "june");
	strcpy(month[6], "july");
	strcpy(month[7], "august");
	strcpy(month[8], "september");
	strcpy(month[9], "october");
	strcpy(month[10], "november");
	strcpy(month[11], "december");
	//Init
	txTextBar = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\textbar.tga");
	txSkyClock = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\sky.tga");
	txSkyClockFrame = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\sky_frame.tga");
}

WdmDateLabel::~WdmDateLabel()
{
	dateFont = -1;
	if(txTextBar >= 0) wdmObjects->rs->TextureRelease(txTextBar);
	if(txSkyClock >= 0) wdmObjects->rs->TextureRelease(txSkyClock);
	if(txSkyClockFrame >= 0) wdmObjects->rs->TextureRelease(txSkyClockFrame);
}

//============================================================================================

//Считать имя фонта
void WdmDateLabel::SetAttributes(ATTRIBUTES * apnt)
{
	if(!apnt) return;
	ATTRIBUTES * ap = apnt->FindAClass(apnt, "date");
	if(ap)
	{
		//Font
		char * s = ap->GetAttribute("font");
		if(s && s[0]) dateFont = wdmObjects->wm->GetRS()->LoadFont(s);
		ATTRIBUTES * a = ap->FindAClass(ap, "monthnames");
		if(a)
		{
			char buf[4];
			buf[0] = 'm'; buf[3] = 0;
			for(long i = 1; i <= 12; i++)
			{
				if(i < 10) buf[1] = '0'; else buf[1] = '1';
				buf[2] = char('0' + (i % 10));
				s = a->GetAttribute(buf);
				if(s && s[0])
				{
					long sl = strlen(s) + 1;
					if(sl > 32) sl = 32;
					memcpy(month[i - 1], s, sl);
					month[i - 1][31] = 0;
				}
			}
		}
	}
}

//============================================================================================

//Отрисовка
void WdmDateLabel::LRender(VDX8RENDER * rs)
{
	float w, h;
	wdmObjects->GetVPSize(w, h);
	//Буфер для рисования плашек
	Vertex buf[(3*2)*2];
	//Плашка с надписью Map
	rs->TextureSet(0, txTextBar);
	FillRectCoord(buf, (w - 1024)*0.5f, 3, 1024, 64.0f);
	FillRectUV(buf, 0.0f, 0.0f, 1.0f, 1.0f);
	FillRectColor(buf, 0xdfffffff);
	//DrawRects(buf, 1, "WdmDrawMapBlend");

	
	//Подложка с небом
	float kDef = rs->GetHeightDeformator();
	float cw = 128.0f;
	float ch = 64.0f*kDef;
	float cx = 8;//w - cw - 8;
	float cy = h - ch - 8;


	rs->TextureSet(0, txSkyClock);
	float u = wdmObjects->wm->hour/24.0f;
	FillRectCoord(buf, cx, cy, cw, ch);
	FillRectUV(buf, u, 0.0f, cw/512.0f, 1.0f);
	FillRectColor(buf, 0xffffffff);
	DrawRects(buf, 1, "WdmInterfaceDrawBlend");
	rs->TextureSet(0, txSkyClockFrame);
	FillRectUV(buf, 0.0f, 0.0f, 1.0f, 1.0f);
	DrawRects(buf, 1, "WdmInterfaceDrawBlend");
	//Пишем дату
	long font = dateFont >= 0 ? dateFont : FONT_DEFAULT;
	long fh = rs->CharHeight(font);
	float fy = ch - fh*2;
	float space = float(fh);
	if(fy >= 0.0f)
	{
		fy *= 0.3333333333333f;
		space += fy;
	}else fy *= 0.5f;
	rs->Print(font, 0xffffffff, long(cx + 8), long(cy + fy), "%i %s", wdmObjects->wm->day, month[wdmObjects->wm->mon - 1]);
	//rs->Print(font, 0xffffffff, long(cx + 8), long(cy + fy), "%i %s", wdmObjects->wm->hour, wdmObjects->wm->day);
	rs->Print(font, 0xffffffff, long(cx + 8), long(cy + fy + space), "%i", wdmObjects->wm->year);
	//Print(rs, font, 0xffffffff, cx, cx + cw, cy + fy, "%i %s", 24, month[0]);
	//Print(rs, font, 0xffffffff, cx, cx + cw, cy + fy + space, "%i", 2002);

			
	
}