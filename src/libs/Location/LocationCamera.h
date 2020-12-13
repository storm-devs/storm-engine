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

#include "Matrix.h"
#include "PathTracks.h"
#include "dx9render.h"
#include "vmodule_api.h"
#include <string>
#include <vector>

#include "CameraFollow.h"

class MODEL;
class Character;
class Location;

class LocationCamera : public Entity
{
    friend CameraFollow;

    enum CameraWorkMode
    {
        cwm_none = 0,
        cwm_follow,
        //Камера преследования человека
        cwm_topos,
        //Переместить камеру в заданную позицию
        cwm_free,
        //Камера свободно летающая (отладочная)
    };

    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    LocationCamera();
    virtual ~LocationCamera();

    //Инициализация
    bool Init() override;
    //Исполнение
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
    //Сообщения
    uint64_t ProcessMessage(MESSAGE &message) override;
    //Изменение атрибута
    uint32_t AttributeChanged(ATTRIBUTES *apnt) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    void LockFPMode(bool isLock);
    float GetAx() const;

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
    bool MoveFollow(CVECTOR &pos, const CVECTOR &cpos, const CVECTOR &to);

    //Протрейсит луч через локацию
    float Trace(const CVECTOR &src, const CVECTOR &dst) const;
    bool GetCollideTriangle(TRIANGLE &trg) const;
    void Clip(PLANE *p, long numPlanes, CVECTOR &cnt, float rad, bool (*fnc)(const CVECTOR *vtx, long num)) const;

  private:
    VDX9RENDER *rs;
    //Море
    entid_t sea;
    //Параметры камеры
    float ax; //Угол наклона камеры
    float lAx;
    float vAx;          //Текущая скорость изменения
    float axmin, axmax; //Угол наклона камеры
    float teleport_dist; //Максимальная дистанция при которой камера телепортируется
    float lookHeight; //Значение куда смотреть по высоте (относительное)
    float radius;     //Радиус удалённости от персонажа
    float rotInertia; //Инерция вращения
    float radInertia; //Инерция приближения удаления

    bool isSpecialMode;
    bool isLockFPMode;
    bool isViewExecute;

    entid_t loc;
    Location *location;

    //Режим работы
    CameraWorkMode wmode;
    bool isSleep;
    bool lockAx;

    float dAx, dAy;

    CVECTOR camPos, lookTo;    //Текущие параметры камеры
    CVECTOR oldPos, oldLookTo; //Последняя позиция камеры предыдущего режима
    float kMorph;              //Текущий коэфициент морфинга

    //Модельки
    entid_t chr;      //Персонаж
    entid_t patchMdl; //Патч для камеры

    CameraFollow cf; //Камера следующая за персонажем

    float obstructTime; //Время загрождения персонажа
    CVECTOR obstruct;   //После

    float freeAx, freeAy; //Углы свободной камеры

    bool isLookMode;
    bool isTeleport;

    //Камера наблюдающая за персонажем из точки
    CVECTOR fromLook;

    //Временные значения на момент исполнения
    Character *character; //Указатель на персонажа
    CVECTOR pos;          //Положение персонажа
    float chay;           //Направление персонажа
    float lheight;        //Высота куда смотреть
    float height;         //Высота персонажа
    float chradius;       //Радиус персонажа

    //динамическое изменение перспективы (пьянство)
    //~!~ rearrange this!
    struct
    {
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

    void TurnOnDynamicFov(float fSpeed, float fTime, float fRelationMin, float fRelationMax, float fAngSpeed,
                          float fAngMax);
    void ProcessDynamicFov(float fDeltaTime, const CVECTOR &vFrom, const CVECTOR &vTo, CVECTOR &vUp);
    void StoreRestoreDynamicFov(bool bStore);

    // track mode section
    bool m_bTrackMode;
    float m_fTrackCurTime;
    float m_fTrackMaxTime;
    std::string m_sCurTrackName;
    float m_fPauseTime;

    struct CameraTrackPause
    {
        float trackTime;
        float pauseTime;
    };

    std::vector<CameraTrackPause> m_aTrackPauses;
    long m_nCurPauseIndex;

    bool LoadCameraTrack(const char *pcTrackFile, float fTrackTime);
    void TurnOffTrackCamera();
    void ProcessTrackCamera();
    float TrackPauseProcess();

    PathTracks m_track;
};

inline void LocationCamera::LockFPMode(bool isLock)
{
    isLockFPMode = isLock;
}

inline float LocationCamera::GetAx() const
{
    return ax;
}

#endif
