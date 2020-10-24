#ifndef PARTICLE_STRUCT
#define PARTICLE_STRUCT

#include "../../common_h/math3d.h"

class DataGraph;
class DataColor;
class DataUV;
class IEmitter;
class GEOS;

struct BB_ParticleData
{
    bool SpeedOriented; //Поворачиваються по вектору скорости...

    //Указатель на кол-во партиклов данного типа, при удалении партикла нужно уменьшать !!!
    DWORD *ActiveCount;

    //Физическая позиция
    CVECTOR PhysPos;

    //Угол поворота
    float Angle;

    //Скорость закручивания, радиан в сек.
    float Spin;

    //Направление и "сила" скорости (НЕ Нормализованная)
    CVECTOR Velocity;

    //Внешняя сила (для прикладывания ее к партиклу)
    CVECTOR ExternalForce;

    //Время жизни
    float LifeTime;

    //Сколько прожил времени
    float ElapsedTime;

    //Матрица трансформации, при рождении партикла (для трека)
    CMatrix matWorld;

    //Масса
    float Mass;

    // fabsf(Масса)
    float UMass;

    //===========================================================================
    //Финальная позиция/угол для рендера
    CVECTOR RenderPos;
    float RenderAngle;
    CVECTOR OldRenderPos;
    float OldRenderAngle;

    //===========================================================================
    //Дистанция до камеры
    float CamDistance;
    bool Visible;

    //===========================================================================
    //Графики.........

    DataGraph *Graph_SpinDrag;
    DataGraph *Graph_Drag;
    DataGraph *Graph_Size;
    DataGraph *Graph_Frames;
    DataColor *Graph_Color;
    DataUV *Graph_UV;
    DataGraph *Graph_Transparency;
    DataGraph *Graph_TrackX;
    DataGraph *Graph_TrackY;
    DataGraph *Graph_TrackZ;
    DataGraph *Graph_PhysBlend;
    DataGraph *graph_GravK;
    DataGraph *graph_AddPower;

    //===========================================================================
    //Коэфиценты рандомайзинга
    float DragK;
    float SpinDragK;
    float SizeK;
    float ColorK;
    float AlphaK;
    float FrameK;
    float GravKK;
    float AddPowerK;
    float KTrackX;
    float KTrackY;
    float KTrackZ;
    float KPhysBlend;

    //===========================================================================
    //Указатель на эмиттер который прицеплен к партиклу
    IEmitter *AttachedEmitter;

    //===============================================
    // ID эмиттера которому принадлежит партикл
    DWORD EmitterGUID;
};

struct MDL_ParticleData
{

    //Указатель на кол-во партиклов данного типа, при удалении партикла нужно уменьшать !!!
    DWORD *ActiveCount;

    //Физическая позиция
    CVECTOR PhysPos;

    //Угол поворота
    CVECTOR Angle;

    //Скорость закручивания, радиан в сек.
    CVECTOR Spin;

    //Направление и "сила" скорости (НЕ Нормализованная)
    CVECTOR Velocity;

    //Внешняя сила (для прикладывания ее к партиклу)
    CVECTOR ExternalForce;

    //Время жизни
    float LifeTime;

    //Сколько прожил времени
    float ElapsedTime;

    //Матрица трансформации, при рождении партикла (для трека)
    CMatrix matWorld;

    //Масса
    float Mass;

    // fabsf(Масса)
    float UMass;

    //===========================================================================
    //Финальная позиция/угол для рендера
    CVECTOR RenderPos;
    CVECTOR RenderAngle;
    CVECTOR OldRenderPos;
    CVECTOR OldRenderAngle;

    //===========================================================================
    //Графики.........

    DataGraph *Graph_SpinDragX;
    DataGraph *Graph_SpinDragY;
    DataGraph *Graph_SpinDragZ;
    DataGraph *Graph_Drag;
    DataGraph *Graph_TrackX;
    DataGraph *Graph_TrackY;
    DataGraph *Graph_TrackZ;
    DataGraph *Graph_PhysBlend;
    DataGraph *graph_GravK;

    //===========================================================================
    //Коэфиценты рандомайзинга
    float DragK;
    float SpinDragK_X;
    float SpinDragK_Y;
    float SpinDragK_Z;
    float GravKK;
    float KTrackX;
    float KTrackY;
    float KTrackZ;
    float KPhysBlend;

    //===========================================================================
    //Указатель на модель для рендера
    GEOS *pScene;

    //===========================================================================
    //Указатель на эмиттер который прицеплен к партиклу
    IEmitter *AttachedEmitter;

    //===============================================
    // ID эмиттера которому принадлежит партикл
    DWORD EmitterGUID;
};

#endif