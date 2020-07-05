#ifndef __BATTLE_NAVIGATOR_H_
#define __BATTLE_NAVIGATOR_H_

#include "..\defines.h"
#include "..\..\common_h\Weather_base.h"

#define RADIAL_QUANTITY					36
#define BI_DEFAULT_COLOR				0xFF0010C0
#define BI_ONESIDE_SIZE					6
#define BI_SIDE_QUANTITY				4
#define FIRERANGE_QUANTITY				(BI_ONESIDE_SIZE+1)*BI_SIDE_QUANTITY
#define MAX_ENEMY_SHIP_QUANTITY			20

class BATTLE_NAVIGATOR
{
	VDX8RENDER * rs;
	ENTITY* m_pOwnerEI;

public:
	BATTLE_NAVIGATOR();
	~BATTLE_NAVIGATOR();

	void	Draw();
	void	Update();
	void	Init(VDX8RENDER *RenderService,ENTITY* pOwnerEI);
	void	SetIsland();
	void	DecrementScale() {m_fCurScale-=m_fScaleStep; if(m_fCurScale<m_fMinScale) m_fCurScale=m_fMinScale;}
	void	IncrementScale() {m_fCurScale+=m_fScaleStep; if(m_fCurScale>m_fMaxScale) m_fCurScale=m_fMaxScale;}
	void	SetEnoughBalls(bool notEnoughBallFlag) {m_bNotEnoughBallFlag=notEnoughBallFlag;}

protected:
	void	CalculateTextureRect(FRECT & texRect, long num, long hq, long vq);
	long	SetCircleVertexPos(BI_ONETEXTURE_VERTEX *v, float x,float y, float rad, float angle=0);
	long	SetCircleVertexTex(BI_ONETEXTURE_VERTEX *v, float x=.5f,float y=.5f, float rad=.5f, float angle=0);
	long	SetRectangleVertexPos(BI_ONETEXTURE_VERTEX *v, float x,float y, float width,float height, float angle=0);
	long	SetRectangleVertexTex(BI_ONETEXTURE_VERTEX *v, float x=.5f,float y=.5f, float width=1.f,float height=1.f, float angle=0);
	long	SetRectangleSegVertexPos(BI_ONETEXTURE_VERTEX *v, float x,float y, float width,float height, float begAngle, float endAngle);
	long	SetRectangleSegVertexTex(BI_ONETEXTURE_VERTEX *v, float x,float y, float width,float height, float begAngle, float endAngle);
	float	CalculateCrossX(int side, float w, float h, float angl);
	float	CalculateCrossY(int side, float w, float h, float angl);

	void	SetMainCharacterData();
	void	SetAnotherShip();
	void	ReleaseAll();
	void	UpdateMiniMap();
	void	UpdateFireRangeBuffer();
	void	FillOneSideFireRange(BI_NOTEXTURE_VERTEX * pv,ATTRIBUTES * pShip,ATTRIBUTES * pChar,char * pstr);
	void	UpdateCurrentCharge();

	void	UpdateWindParam();

protected:
	DWORD	m_dwBackGradColor1;
	DWORD	m_dwBackGradColor2;
	// параметры видимого горизонта
	float	m_fMapRadius;	// радиус миникарты на экране
	float	m_fTextureRad;	// радиус миникарты в текстуре
	float	m_fWorldRad;	// радиус миникарты в мировых координатах
	float	m_fCurScale;	// масштаб миникарты
	float	m_fDefaultScale;// масштаб карты по умолчанию
	float	m_fMinScale;	// минимальный масштаб карты
	float	m_fMaxScale;	// максимальный масштаб карты
	float	m_fScaleStep;	// шаг изменения масштаба
	float	m_fShipShowRad;	// радиус показываемого корабля

	// параметры навигационного окна
	long	m_NavigationWidth;
	long	m_NavigationHeight;
	long	m_XNavigator;
	long	m_YNavigator;

	// координаты нашего корабля
	float	m_fXPos;
	float	m_fYPos;
	float	m_fAngle;
	float	m_fShipSpeed;
	float	m_fMaxShipSpeed;
	float	m_fShipSpeedScale;

	// координаты острова
	bool	m_bYesIsland;
	float	m_fXIsland;
	float	m_fYIsland;
	float   m_fIslandWidth;
	float   m_fIslandHeight;
	float	m_fIslandRadius;

	// ветер
	float	m_fWindAngle;
	float	m_fWindStrength;
	float	m_fWindMaxStrength;
	long	m_windWidth;
	long	m_windHeight;

	// параметры заряда пушек
	DWORD	m_dwChargeCannon;
	DWORD	m_dwReadyCannon;
	DWORD	m_dwDamagedCannon;

	float	m_fBegAnglLeftCharge;
	float	m_fCurAnglLeftCharge;
	float	m_fCurAnglLeftDamage;
	float	m_fEndAnglLeftCharge;

	float	m_fBegAnglRightCharge;
	float	m_fCurAnglRightCharge;
	float	m_fCurAnglRightDamage;
	float	m_fEndAnglRightCharge;

	float	m_fBegAnglForwardCharge;
	float	m_fCurAnglForwardCharge;
	float	m_fCurAnglForwardDamage;
	float	m_fEndAnglForwardCharge;

	float	m_fBegAnglBackCharge;
	float	m_fCurAnglBackCharge;
	float	m_fCurAnglBackDamage;
	float	m_fEndAnglBackCharge;

	// параметры скорости корабля и ветра
	float	m_fBegAnglShipSpeed;
	float	m_fEndAnglShipSpeed;
	float	m_fCurAnglShipSpeed;
	float	m_fBegAnglWindSpeed;
	float	m_fEndAnglWindSpeed;
	float	m_fCurAnglWindSpeed;

	// текстуры
	long	m_idCompasTex;	// компас
	long	m_idSpeedTex;	// скорость корабля и ветер
	long	m_idCannonTex;	// заряд пушек
	long	m_idEmptyTex;	// пустое место для скорости и заряда
	long	m_idIslandTexture; // остров
	long	m_idWindTex;	// ветер
	long	m_idChargeTexture; // текущий тип заряда пушек
	long	m_idWindTexture; // скорость ветра
	long	m_idSailTexture; // положение парусов/скорость корабля
	IDirect3DTexture8 * m_pIslandTexture;

	DWORD	m_dwSeaColor;	// цвет моря на миникарте
	DWORD	m_dwFireZoneColor; // цвет зоны огня на миникарте
	DWORD	m_dwEnemyShipColor; // цвет вражеских кораблей
	DWORD	m_dwFrendShipColor; // цвет своих кораблей
	DWORD	m_dwNeutralShipColor; // цвет своих кораблей
	DWORD	m_dwDeadShipColor;	// цвет тонущего корабля

	// буфера
	long	m_idEmptyVBuf;
	long	m_idMapVBuf;
	//--------------------
	long	m_idFireZoneVBuf; // бабочка линии огня пушек
	//--------------------
	long	m_idCannonVBuf;
	long	m_nvCannonCharge;
	long	m_nvCannonReady;
	long	m_nvCannonDamage;
	//--------------------
	long	m_idSpeedVBuf;
	long	m_nvSpeed;
	//--------------------
	long	m_idShipsVBuf;
	long	m_nvShips;
	//--------------------
	long	m_idGradBackVBuf;
	//--------------------
	long	m_idCurChargeVBuf;

	// шрифты и надписи
	long	m_speedFont;
	long	m_ySpeedShow;
	long	m_xShipSpeed;
	long	m_xWindSpeed;

	WEATHER_BASE *m_wb;
	ATTRIBUTES* m_pAWeather;

	// иконка текущего типа заряда пушек
	POINT	m_ChargeGreed;
	POINT	m_ChargePos;
	POINT	m_ChargeSize;
	long	m_curCharge;
	// иконка ветра
	long	m_curWindPic;
	POINT	m_WindGreed;
	POINT	m_WindPos;
	POINT	m_WindSize;
	// иконка положения парусов
	long	m_curSailState;
	POINT	m_SailGreed;
	POINT	m_SailPos;
	POINT	m_SailSize;

	float	m_fFontScale;

	bool	m_bNotEnoughBallFlag;

	float	m_fAspectRatio;
};

#endif
