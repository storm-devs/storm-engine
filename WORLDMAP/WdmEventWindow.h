//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	WdmEventWindow
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _WdmEventWindow_H_
#define _WdmEventWindow_H_

#include "WdmInterfaceObject.h"

class WdmEventWindow : public WdmInterfaceObject  
{
	struct Token
	{
		long x, y;
		long w;
		char * string;
	};


//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	WdmEventWindow();
	virtual ~WdmEventWindow();

	//Активировать окошко
	void ActivateEvent(const char * etext, const char * _yes, const char * _no);

	//Расчёты
	virtual void Update(float dltTime);
	//Отрисовка
	virtual void LRender(VDX8RENDER * rs);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	void SplitText();
	//Установить варианты ответов
	void SetAnswers(const char * _yes, const char * _no);

private:
	long headerFont;
	long headerWidth;
	long textFont;
	long textureBkg;
	long textureButton;
	long buttonFont;
	char * text;
	Token * token;
	long numTokens;
	long maxTokens;
	float winX, winY, winW, winH;
	long buttonPosY;
	char yes[64], no[64];
	long yesWidth, noWidth;
	long selButton;
};

#endif

