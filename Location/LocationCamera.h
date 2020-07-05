//============================================================================================
//	Spirenkov Maxim
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	LocationCamera
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _LocationCamera_H_
#define _LocationCamera_H_


#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\matrix.h"
#include "..\common_h\templates\array.h"
#include "..\common_h\templates\string.h"
#include "PathTracks.h"

#include "CameraFollow.h"

class MODEL;
class Character;
class Location;

class LocationCamera : public ENTITY  
{
	friend CameraFollow;

	enum CameraWorkMode
	{
		cwm_none = 0,
		cwm_follow,		//Камера преследования человека
		cwm_topos,		//Переместить камеру в заданную позицию
		cwm_free,		//Камера свободно летающая (отладочная)
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	LocationCamera();
	virtual ~LocationCamera();

	//Инициализация
	bool Init();
	//Исполнение
	void Execute(dword delta_time);
	void Realize(dword delta_time);
	//Сообщения
	dword _cdecl ProcessMessage(MESSAGE & message);
	//Изменение атрибута
	dword AttributeChanged(ATTRIBUTES * apnt);

	void LockFPMode(bool isLock);
	float GetAx();

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	//Подготовить данные
	bool Set();
	//Изменить режим работы камеры
	void SetMode(CameraWorkMode wm);
	//Исполнение камеры следующей за персонажем
	void ExecuteFollow(float dltTime);
	//Исполнение камеры вида из глаз
	void ExecuteLook(float dltTime);
	//Исполнение камеры наблюдающей из точки
	void ExecuteTopos(float dltTime);
	//Исполнение свободно летающей камеры
	void ExecuteFree(float dltTime);

	//Перемещение камеры из заданной позиции в новую
	bool MoveFollow(CVECTOR & pos, const CVECTOR & cpos, const CVECTOR & to);
	
	//Протрейсит луч через локацию
	float Trace(const CVECTOR & src, const CVECTOR & dst);
	bool GetCollideTriangle(TRIANGLE & trg);
	void Clip(PLANE * p, long numPlanes, CVECTOR & cnt, float rad, bool (* fnc)(const CVECTOR * vtx, long num));

	

private:
	VDX8RENDER * rs;
	//Море
	ENTITY_ID sea;
	//Параметры камеры
	float ax;					//Угол наклона камеры
	float lAx;
	float vAx;					//Текущая скорость изменения
	float axmin, axmax;			//Угол наклона камеры
	float teleport_dist;		//Максимальная дистанция при которой камера телепортируется
	float lookHeight;			//Значение куда смотреть по высоте (относительное)
	float radius;				//Радиус удалённости от персонажа
	float rotInertia;			//Инерция вращения
	float radInertia;			//Инерция приближения удаления

	bool isSpecialMode;
	bool isLockFPMode;
	bool isViewExecute;

	ENTITY_ID loc;
	Location * location;

	//Режим работы
	CameraWorkMode wmode;
	bool isSleep;
	bool lockAx;

	float dAx, dAy;

	CVECTOR camPos, lookTo;		//Текущие параметры камеры	
	CVECTOR oldPos, oldLookTo;	//Последняя позиция камеры предыдущего режима	
	float kMorph;				//Текущий коэфициент морфинга

	//Модельки
	ENTITY_ID chr;				//Персонаж
	ENTITY_ID patchMdl;			//Патч для камеры

	CameraFollow cf;			//Камера следующая за персонажем

	float obstructTime;			//Время загрождения персонажа
	CVECTOR obstruct;			//После

	
	float freeAx, freeAy;		//Углы свободной камеры

	bool isLookMode;
	bool isTeleport;

	//Камера наблюдающая за персонажем из точки
	CVECTOR fromLook;

	//Временные значения на момент исполнения
	Character * character;		//Указатель на персонажа
	CVECTOR pos;				//Положение персонажа
	float chay;					//Направление персонажа
	float lheight;				//Высота куда смотреть
	float height;				//Высота персонажа
	float chradius;				//Радиус персонажа

	//динамическое изменение перспективы (пьянство)
	struct {
		bool isOn;
		float fMinFov;
		float fMaxFov;
		float fCurFov;
		bool bFogUp;
		float fFogChangeSpeed;
		float fFogTimeCur;
		float fFogTimeMax;
		float fCurAngle;
		float fMaxAngle;
		float fAngleSpeed;
		bool bAngleUp;
	} dynamic_fog;
	void TurnOnDynamicFov(float fSpeed,float fTime, float fRelationMin, float fRelationMax, float fAngSpeed, float fAngMax);
	void ProcessDynamicFov(float fDeltaTime, const CVECTOR &vFrom, const CVECTOR &vTo, CVECTOR &vUp );
	void StoreRestoreDynamicFov(bool bStore);

	// track mode section
	bool m_bTrackMode;
	float m_fTrackCurTime;
	float m_fTrackMaxTime;
	string m_sCurTrackName;
	float m_fPauseTime;

	struct CameraTrackPause
	{
		float trackTime;
		float pauseTime;
	};
	array<CameraTrackPause> m_aTrackPauses;
	long m_nCurPauseIndex;

	bool LoadCameraTrack(const char* pcTrackFile,float fTrackTime);
	void TurnOffTrackCamera();
	void ProcessTrackCamera();
	float TrackPauseProcess();

	PathTracks m_track;
};

inline void LocationCamera::LockFPMode(bool isLock)
{
	isLockFPMode = isLock;
}

inline float LocationCamera::GetAx()
{
	return ax;
}

#endif

