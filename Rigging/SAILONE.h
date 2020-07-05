// SAILONE.h: interface for the SAILONE class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAILONE_H__CC023C85_54DA_11D5_BCC5_00002121783F__INCLUDED_)
#define AFX_SAILONE_H__CC023C85_54DA_11D5_BCC5_00002121783F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\\common_h\\vmodule_api.h"
#include "..\\common_h\\dx8render.h"
#include "..\\common_h\\matrix.h"
#include "..\\common_h\\sail_base.h"

extern float g_fSailHoleDepend;
extern float GetSailSpeed(int holeQ,int holeMax,float maxSpeed,float fSailHoleDepend = g_fSailHoleDepend);

// used other classes
class NODE;

#define SAIL_COL_MAX 13
#define SAIL_ROW_MAX 17
#define SAILVERTEX_FORMAT		(D3DFVF_XYZ| D3DFVF_NORMAL | D3DFVF_TEX3 | D3DFVF_TEXTUREFORMAT2)

struct SAILVERTEX
{
	CVECTOR pos, norm;
	float tu1, tv1;
	float tu2, tv2;
	float tu3, tv3;
};

// параметры для триангуляции паруса
#define SAIL_FAR_IDX 7
const struct {WORD row,col; float len;} farGrid[SAIL_FAR_IDX]=
{
    {17,13,10.f},   // 12x16
    {13,13,20.f},
    {13,10,30.f},
    {13,7, 50.f},
    {9, 7, 200.f},  // 6x8 
    {5, 7, 250.f},  // 6x4
    {5, 4, 600.f},  // 3x4
//    {3, 4, 10000.f} // 3x2
};

struct WIND
{
    float base; // амплитуда колебаний ветра от 0 до 1.0
    struct {float x,y,z;} ang;
};

struct SPHERE
{
    CVECTOR c;
    CVECTOR rc; // центр в мировых координатах
    float r;
};

#define TEXTUREMAXCOUNTER 3
// таблица текстур
struct TEXTURESLIST {
    int  texCount;
    int  uses[TEXTUREMAXCOUNTER];
    long ntex[TEXTUREMAXCOUNTER];
    long htex[TEXTUREMAXCOUNTER];
    char* nname[TEXTUREMAXCOUNTER];
    char* hname[TEXTUREMAXCOUNTER];
};

struct SAILGEOMETRY {
    union {
        struct {float x,y,z;} v[9];
        struct
        {
            CVECTOR dVv,ddVv,dddVv; // Vectors for vertical flex of sail
            CVECTOR dVh,ddVh;       // Vectors for horizontal flaex of sail
            CVECTOR normL,dnormL;   // Vectors for leftside sail normals
            CVECTOR normR,dnormR;   // Vectors for rightside sail normals
            CVECTOR dopV; // Vector for rope tie;
        };
    };
};

#define SAILROLLSTEPS 40 // steps counter of sail rolling
struct SAILROLLING {
    SAILGEOMETRY oldgeo; // final value of geometry parameters
    float delta;
    bool rollup; // парус сворачивается (ложь-разворачивается)
};

struct ROTATEROPEDSAIL
{
    int ropenum;
    float r1,r2; // радиус до конца веревки и начала другой точки
    CVECTOR b; // координата x,y,z для расчета натяжения веревкой по Z
    int tiePoint;
};
struct SAILTIEROPE {
    bool pnttie[4];
    CVECTOR* pPos[4];
    ROTATEROPEDSAIL* rrs[2];
};

enum SAIL_TYPE {
    SAIL_TREANGLE,
    SAIL_SQUARE,
    SAIL_TRAPECIDAL,
    SAIL_GERBED,
    SAIL_ROLLING
};

struct SAILSTATE {
    DWORD holeCount;
    bool hole[12];
    WORD shi,sni,Nh,Nn; // Стартовый индекс и число индексов в дырявом\нормальном отражении паруса

	SAIL_TYPE eSailType;

    // точки привязки паруса
	CVECTOR hardPoints[4];

	bool bYesLimitPoint;
	CVECTOR LimitPoint;

    // номер текстуры
    WORD texNum;

    //----Параметры составляющие форму паруса----
    float fDeepZ; // абсолютная выпуклость паруса по вертикали
    float fDeepH; // абсолютная выпуклость паруса по горизонтали
    float fDeepVz; // прогиб нижнего края паруса по нормали
    float fDeepVh; // прогиб нижнего края паруса по высоте

    DWORD sVert,nVert;
    DWORD sIndx,nIndx;
    DWORD sholeIndx,nnormIndx,nholeIndx;

    // шар ограничивающий парус
    SPHERE boundSphere;
    CVECTOR boxCenter;
    CVECTOR boxSize;

    bool rollingSail,turningSail;

    float maxAngle,curAngle;
};

struct SAILTIME {
    DWORD idx,vert,noholedraw,holedraw;
};

class SAIL;
struct SAILLIST;
class SAILONE : public SAILONE_BASE
{
    friend SAIL;
public:
	VDX8RENDER * RenderService;
	SAILONE();
	~SAILONE();
	bool SetSail();
	void releaseAll();
    SAILSTATE ss;
	SAILTIME tm;

    void FillIndex(WORD *pt); // заполнение массива треугольников
	void ClearVertex(SAILVERTEX *pv,DWORD maxIdx);
    void goWave(SAILVERTEX *pv,DWORD Delta_Time);
	void FillVertex(SAILVERTEX *pv); // заполнение массива вертексов
	void SetTexGrid(SAILVERTEX *pv); // установка координат в текстуре
    void SetGeometry(); // установка параметров для создания геометрии паруса
    void SetRolling(bool bRoll); // установить сворачивание-разворачивание паруса
    void DoRollingStep(DWORD Delta_Time); // выполняем шаг сворачивания-разворачивания паруса
    void TurnSail(float fTurnStep); // повернуть парус вокруг оси OY
    void CalculateMirrorSailIndex();
    bool GetGrid(CVECTOR &pos,float perspect);
    float Trace(const CVECTOR &src,const CVECTOR &dst,bool bCannonTrace);
    float SSailTrace(CVECTOR &src,CVECTOR &dst,bool bCannonTrace);
    float TSailTrace(CVECTOR &src,CVECTOR &dst,bool bCannonTrace);
    float CheckSailSquar(int i,CVECTOR &va,CVECTOR &vb,CVECTOR &vc,CVECTOR &vsrc, CVECTOR &vdst, bool bCannonTrace);
	void SetAllHole(DWORD holeData);
    // число вертексов по вертикали и горизонтали
	DWORD m_dwRow, m_dwCol;
    float sailWidth,sailHeight;

private:
    SAILGEOMETRY sgeo;
	void GoVWave(SAILVERTEX* pv);
	void GoTWave(SAILVERTEX* pv);
    void DoSRollSail(SAILVERTEX* pv);
    void DoTRollSail(SAILVERTEX* pv);
    void DoSFreeSail(SAILVERTEX* pv);
    void DoTFreeSail(SAILVERTEX* pv);

	void SetTurnLimits();

	DWORD GetHoleDword() {DWORD v=0,t=1; for(int i=0;i<12;i++,t<<=1)if(ss.hole[i])v|=t; return v;}
	DWORD GetMaxHoleCount() { return ss.eSailType==SAIL_TREANGLE?10:12;}

	float GetDistanceFromPointTo3Point(const CVECTOR& v, const CVECTOR& vB1, const CVECTOR& vB2, const CVECTOR& vB3);

    // Параметры воздействия ветра
    //------------------------------
    int VertIdx;  // индекс в массиве векторов ветра
    int HorzIdx;
    bool WindUp; // подъем паруса вверх от ветра
    float SumWind,MaxSumWind;
    bool bFreeSail; // свободно развивающиеся паруса - при падении или отрыве

    CVECTOR SailPnt[20];

    //------------------------------------
    // Сворачивание.разворачивание паруса
    int rollType; // тип сворачивания паруса
    SAILROLLING *sroll; // структура на момент свертывания/развертывания паруса
    bool bRolling; // парус свернут в рулет на рее

    SAIL* pp;
    //------------------------------------
    // параметры определения положения в списке парусов
    int groupNum;   // номер группы парусов в данном узле
    int mastNum;    // номер мачты
    NODE *hostNode; // хозяин узла
    CMatrix *pMatWorld; // матрица этого узла

    SAILTIEROPE sailtrope;
    float oldWindAngl;

    // физика влияния на скорость корабля
    float maxSpeed; // Максимальная скорость даваемая парусом
    float curSpeed; // Текущая скорость

    // ветер
    CVECTOR sailWind;
    void CalculateSailWind();

    int wind_incr,wind_add;

    int HostNum; // номер группы парусов (корабля)
	bool bDeleted;

	// Параметры ограничивающие поворот паруса
	CVECTOR	m_vMastTrace[2];
	float m_fMaxAngle;
	float m_fMinAngle;

	long surfaceTex;
	bool m_bIsGerald;
	long m_nGeraldTex;
	IDirect3DTexture8* m_pGeraldTex;

	float m_fHorzGeraldScale;
	float m_fVertGeraldScale;
};

#endif // !defined(AFX_SAILONE_H__CC023C85_54DA_11D5_BCC5_00002121783F__INCLUDED_)
