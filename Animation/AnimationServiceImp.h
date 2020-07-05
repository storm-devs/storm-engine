//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Storm engine v2.00
//--------------------------------------------------------------------------------------------
//	AnimationServiceImp
//--------------------------------------------------------------------------------------------
//	Сервис анимации, позволяющий создавать объекты AnimationManager
//============================================================================================

#ifndef _AnimationServiceImp_H_
#define _AnimationServiceImp_H_

#include "..\COMMON_H\Animation.h"
#include "..\common_h\vmodule_api.h"
#include "AnimationInfo.h"

//============================================================================================

class AnimationImp;

class AnimationServiceImp : public AnimationService
{
//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	AnimationServiceImp();
	virtual ~AnimationServiceImp();
	
	//Место исполнения
	virtual dword RunSection();
	//Функции исполнения
	virtual void RunStart();
	virtual void RunEnd();
	//Создать анимацию для модели, удалять через delete
	virtual Animation * CreateAnimation(const char * animationName);

//--------------------------------------------------------------------------------------------
//Функции для Animation
//--------------------------------------------------------------------------------------------	
	//Удалить анимацию (вызывается из деструктора)
	void DeleteAnimation(AnimationImp * ani);
	//Событие
	void Event(const char * eventName);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	//Загрузить анимацию
	long LoadAnimation(const char * animationName);
	//Загрузить из текущей секции пользовательские данные
	void LoadUserData(INIFILE * ani, const char * sectionName, UserData & data, const char * animationName);
	//Загрузить AN
	bool LoadAN(const char * fname, AnimationInfo * info);

	
	AnimationInfo ** ainfo;
	long numInfos;

	AnimationImp ** animation;
	long numAnimations;

	static char key[1024];
};

//============================================================================================

#endif

