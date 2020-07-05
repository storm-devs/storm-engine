#include <stdio.h>
#include "battle_navigator.h"
#include "..\..\common_h\sd2_h\VAI_ObjBase.h"
#include "..\..\common_h\ship_base.h"
#include "..\..\common_h\Weather_base.h"
#include "..\..\common_h\Island_Base.h"
#include "ships_list.h"
#include "..\utils.h"

#define NAVBACKGROUND_SIZESCALE	1.3f

#define MAP_TEXTURE_WIDTH		256
#define MAP_TEXTURE_HEIGHT		256
#define BIGMAP_TEXTURE_WIDTH	1024
#define BIGMAP_TEXTURE_HEIGHT	1024

inline bool FloatCompare(float a,float b)
{
	return((a>b-.0001f) && (a<b+.0001f));
}

bool g_bExternMapCenter = false;
bool g_bMapScaling = false;
CVECTOR g_externPos;

void BATTLE_NAVIGATOR::CalculateTextureRect(FRECT & texRect, long num, long hq, long vq)
{
	long yNum = num/hq;
	long xNum = num - yNum*hq;

	texRect.left = 1.f/hq*xNum;
	texRect.right = 1.f/hq*(xNum+1);
	texRect.top = 1.f/vq*yNum;
	texRect.bottom = 1.f/vq*(yNum+1);
}

BATTLE_NAVIGATOR::BATTLE_NAVIGATOR()
{
	m_pOwnerEI = 0;
	m_dwFireZoneColor=0x20FF0050;
	rs=NULL;

	m_fShipSpeedScale = 1.f;

	m_pIslandTexture = NULL;
	m_fCurScale = m_fDefaultScale = 1.f;
	m_bYesIsland = false;

	// текстуры
	m_idCompasTex = -1;
	m_idSpeedTex = -1;
	m_idCannonTex = -1;
	m_idEmptyTex = -1;
	m_idIslandTexture = -1;
	m_idChargeTexture = -1;
	m_idWindTex = -1;
	m_idWindTexture = -1;
	m_idSailTexture = -1;


	// буферы
	m_idEmptyVBuf = -1;
	m_idMapVBuf = -1;
	m_idFireZoneVBuf = -1;
	m_idCannonVBuf = -1;
	m_idSpeedVBuf = -1;
	m_idCurChargeVBuf = -1;

	m_idShipsVBuf = -1;
	m_nvShips = 0;

	m_speedFont = -1;
	m_wb = 0;
	m_pAWeather = 0;

	m_curCharge = -1;
	m_bNotEnoughBallFlag = false;
}

BATTLE_NAVIGATOR::~BATTLE_NAVIGATOR()
{
	ReleaseAll();
}

void BATTLE_NAVIGATOR::Draw()
{
	if(rs==NULL) return;
	int n;

	// set world matrix
	CMatrix matw;
	rs->SetTransform(D3DTS_WORLD,(D3DXMATRIX*)&matw);

	// градиентная подложка
	//rs->DrawPrimitive(D3DPT_TRIANGLEFAN,m_idGradBackVBuf,sizeof(BI_COLORONLY_VERTEX),0,1,"battle_only_color");

	// остров
	rs->SetTextureStageState(0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
	rs->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);
	if(m_idIslandTexture>=0)
		rs->TextureSet(0,m_idIslandTexture);
	if(m_pIslandTexture!=NULL)
		rs->SetTexture(0,m_pIslandTexture);
	if(m_bYesIsland)
		rs->DrawPrimitive(D3DPT_TRIANGLEFAN,m_idMapVBuf,sizeof(BI_ONETEXTURE_VERTEX),0,RADIAL_QUANTITY,"battle_island");
	else
	{
		rs->SetRenderState(D3DRS_TEXTUREFACTOR,m_dwSeaColor);
		rs->DrawPrimitive(D3DPT_TRIANGLEFAN,m_idMapVBuf,sizeof(BI_ONETEXTURE_VERTEX),0,RADIAL_QUANTITY,"battle_only_tfactor");
	}
	rs->SetTextureStageState(0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
	rs->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);

	// корабли
	if(m_nvShips>0)
		rs->DrawPrimitive(D3DPT_TRIANGLELIST,m_idShipsVBuf,sizeof(BI_COLORONLY_VERTEX),0,m_nvShips/3,"battle_only_color");

	// бабочка зоны поражения пушек
	if(m_idFireZoneVBuf!=-1L)
	{
		rs->SetRenderState(D3DRS_TEXTUREFACTOR,m_dwFireZoneColor);
		rs->DrawPrimitive(D3DPT_TRIANGLEFAN,m_idFireZoneVBuf,sizeof(BI_NOTEXTURE_VERTEX),0,FIRERANGE_QUANTITY-2,"battle_only_tfactor");
	}

	// компас и подложка
	if(m_idEmptyVBuf!=-1L)
	{
		// подложка под миникарту
		rs->TextureSet(0,m_idEmptyTex);
		rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idEmptyVBuf,sizeof(BI_ONETEXTURE_VERTEX),0,2,"battle_rectangle");
		// компас
		rs->TextureSet(0,m_idCompasTex);
		rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idEmptyVBuf,sizeof(BI_ONETEXTURE_VERTEX),4,2,"battle_rectangle");
	}

	// показатель заряда пушек
	rs->TextureSet(0,m_idCannonTex);
	n = 0;
	if(m_nvCannonCharge>0)
	{
		rs->SetRenderState(D3DRS_TEXTUREFACTOR,m_dwChargeCannon);
		rs->DrawPrimitive(D3DPT_TRIANGLEFAN,m_idCannonVBuf,sizeof(BI_ONETEXTURE_VERTEX),n,m_nvCannonCharge,"battle_tf_rectangle");
		n += m_nvCannonCharge+2;
	}
	if(m_nvCannonReady>0)
	{
		rs->SetRenderState(D3DRS_TEXTUREFACTOR,m_dwReadyCannon);
		rs->DrawPrimitive(D3DPT_TRIANGLEFAN,m_idCannonVBuf,sizeof(BI_ONETEXTURE_VERTEX),n,m_nvCannonReady,"battle_tf_rectangle");
		n += m_nvCannonReady+2;
	}
	if(m_nvCannonDamage>0)
	{
		rs->SetRenderState(D3DRS_TEXTUREFACTOR,m_dwDamagedCannon);
		rs->DrawPrimitive(D3DPT_TRIANGLEFAN,m_idCannonVBuf,sizeof(BI_ONETEXTURE_VERTEX),n,m_nvCannonDamage,"battle_tf_rectangle");
	}

	// показатель скорости
	/*rs->TextureSet(0,m_idSpeedTex);
	if(m_nvSpeed>0)
		rs->DrawPrimitive(D3DPT_TRIANGLEFAN,m_idSpeedVBuf,sizeof(BI_ONETEXTURE_VERTEX),0,m_nvSpeed,"battle_rectangle");*/

	// показать стрелку направления ветра
	if(m_idEmptyVBuf!=-1L)
	{
		rs->TextureSet(0,m_idWindTex);
		rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idEmptyVBuf,sizeof(BI_ONETEXTURE_VERTEX),8,2,"battle_rectangle");
	}

	// отпечатать скорость ветра и корабля
	rs->ExtPrint(m_speedFont,0xFFFFFFFF,0,ALIGN_CENTER,true,m_fFontScale,0,0,m_xWindSpeed,m_ySpeedShow,"%.1f",m_fWindStrength);
	rs->ExtPrint(m_speedFont,0xFFFFFFFF,0,ALIGN_CENTER,true,m_fFontScale,0,0,m_xShipSpeed,m_ySpeedShow,"%.1f",m_fShipSpeed);

	// показать текущий заряд
	if(m_curCharge>=0)
	{
		rs->TextureSet(0,m_idChargeTexture);
		rs->SetRenderState(D3DRS_TEXTUREFACTOR,(BIUtils::g_dwBlinkColor<<24L)|0xFFFFFF);
		if(m_bNotEnoughBallFlag)
			rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idCurChargeVBuf,sizeof(BI_ONETEXTURE_VERTEX),0,2,"battle_texure_blend_tf");
		else
			rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idCurChargeVBuf,sizeof(BI_ONETEXTURE_VERTEX),0,2,"battle_rectangle");
	}
	// показать иконку ветра
	rs->TextureSet(0,m_idWindTexture);
	rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idCurChargeVBuf,sizeof(BI_ONETEXTURE_VERTEX),4,2,"battle_rectangle");
	// показать иконку положения парусов
	rs->TextureSet(0,m_idSailTexture);
	rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idCurChargeVBuf,sizeof(BI_ONETEXTURE_VERTEX),8,2,"battle_rectangle");
}

void BATTLE_NAVIGATOR::Update()
{
	if(rs==NULL) return;

	UpdateWindParam();
	SetMainCharacterData();
	UpdateCurrentCharge();

	// change compas buffer
	BI_ONETEXTURE_VERTEX * pV = (BI_ONETEXTURE_VERTEX *)rs->LockVertexBuffer(m_idEmptyVBuf);
	if(pV!=NULL)
	{
		SetRectangleVertexPos(&pV[4],(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,-m_fAngle);
		SetRectangleVertexPos(&pV[8],(float)m_XNavigator,(float)m_YNavigator,(float)m_windWidth,(float)m_windHeight,m_fWindAngle-m_fAngle);
		rs->UnLockVertexBuffer(m_idEmptyVBuf);
	}
	// change cannon charge buffer
	pV = (BI_ONETEXTURE_VERTEX *)rs->LockVertexBuffer(m_idCannonVBuf);
	if(pV!=NULL)
	{
		m_nvCannonCharge = m_nvCannonReady = m_nvCannonDamage = 0;

		if(!FloatCompare(m_fCurAnglLeftCharge,m_fCurAnglLeftDamage))
		{
			SetRectangleSegVertexPos(pV,(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,m_fBegAnglLeftCharge,m_fCurAnglLeftCharge);
			m_nvCannonCharge = SetRectangleSegVertexTex(pV,.5f,.5f,1.f,1.f,m_fBegAnglLeftCharge,m_fCurAnglLeftCharge);
		}
		if(!FloatCompare(m_fCurAnglRightCharge,m_fCurAnglRightDamage))
		{
			SetRectangleSegVertexPos(&pV[m_nvCannonCharge],(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,m_fCurAnglRightCharge,m_fBegAnglRightCharge);
			m_nvCannonCharge += SetRectangleSegVertexTex(&pV[m_nvCannonCharge],.5f,.5f,1.f,1.f,m_fCurAnglRightCharge,m_fBegAnglRightCharge);
		}
		if(!FloatCompare(m_fCurAnglForwardCharge,m_fCurAnglForwardDamage))
		{
			SetRectangleSegVertexPos(&pV[m_nvCannonCharge],(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,m_fBegAnglForwardCharge,m_fCurAnglForwardCharge);
			m_nvCannonCharge += SetRectangleSegVertexTex(&pV[m_nvCannonCharge],.5f,.5f,1.f,1.f,m_fBegAnglForwardCharge,m_fCurAnglForwardCharge);
		}
		if(!FloatCompare(m_fCurAnglBackCharge,m_fCurAnglBackDamage))
		{
			SetRectangleSegVertexPos(&pV[m_nvCannonCharge],(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,m_fCurAnglBackCharge,m_fBegAnglBackCharge);
			m_nvCannonCharge += SetRectangleSegVertexTex(&pV[m_nvCannonCharge],.5f,.5f,1.f,1.f,m_fCurAnglBackCharge,m_fBegAnglBackCharge);
		}

		if(FloatCompare(m_fCurAnglLeftCharge,m_fCurAnglLeftDamage))
		{
			SetRectangleSegVertexPos(&pV[m_nvCannonCharge],(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,m_fBegAnglLeftCharge,m_fCurAnglLeftCharge);
			m_nvCannonReady = SetRectangleSegVertexTex(&pV[m_nvCannonCharge],.5f,.5f,1.f,1.f,m_fBegAnglLeftCharge,m_fCurAnglLeftCharge);
		}
		if(FloatCompare(m_fCurAnglRightCharge,m_fCurAnglRightDamage))
		{
			SetRectangleSegVertexPos(&pV[m_nvCannonCharge+m_nvCannonReady],(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,m_fCurAnglRightCharge,m_fBegAnglRightCharge);
			m_nvCannonReady += SetRectangleSegVertexTex(&pV[m_nvCannonCharge+m_nvCannonReady],.5f,.5f,1.f,1.f,m_fCurAnglRightCharge,m_fBegAnglRightCharge);
		}
		if(FloatCompare(m_fCurAnglForwardCharge,m_fCurAnglForwardDamage))
		{
			SetRectangleSegVertexPos(&pV[m_nvCannonCharge+m_nvCannonReady],(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,m_fBegAnglForwardCharge,m_fCurAnglForwardCharge);
			m_nvCannonReady += SetRectangleSegVertexTex(&pV[m_nvCannonCharge+m_nvCannonReady],.5f,.5f,1.f,1.f,m_fBegAnglForwardCharge,m_fCurAnglForwardCharge);
		}
		if(FloatCompare(m_fCurAnglBackCharge,m_fCurAnglBackDamage))
		{
			SetRectangleSegVertexPos(&pV[m_nvCannonCharge+m_nvCannonReady],(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,m_fCurAnglBackCharge,m_fBegAnglBackCharge);
			m_nvCannonReady += SetRectangleSegVertexTex(&pV[m_nvCannonCharge+m_nvCannonReady],.5f,.5f,1.f,1.f,m_fCurAnglBackCharge,m_fBegAnglBackCharge);
		}

		if(!FloatCompare(m_fCurAnglLeftDamage,m_fEndAnglLeftCharge))
		{
			SetRectangleSegVertexPos(&pV[m_nvCannonCharge+m_nvCannonReady],(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,m_fCurAnglLeftDamage,m_fEndAnglLeftCharge);
			m_nvCannonDamage = SetRectangleSegVertexTex(&pV[m_nvCannonCharge+m_nvCannonReady],.5f,.5f,1.f,1.f,m_fCurAnglLeftDamage,m_fEndAnglLeftCharge);
		}
		if(!FloatCompare(m_fCurAnglRightDamage,m_fEndAnglRightCharge))
		{
			SetRectangleSegVertexPos(&pV[m_nvCannonCharge+m_nvCannonReady+m_nvCannonDamage],(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,m_fEndAnglRightCharge,m_fCurAnglRightDamage);
			m_nvCannonDamage += SetRectangleSegVertexTex(&pV[m_nvCannonCharge+m_nvCannonReady+m_nvCannonDamage],.5f,.5f,1.f,1.f,m_fEndAnglRightCharge,m_fCurAnglRightDamage);
		}
		if(!FloatCompare(m_fCurAnglForwardDamage,m_fEndAnglForwardCharge))
		{
			SetRectangleSegVertexPos(&pV[m_nvCannonCharge+m_nvCannonReady+m_nvCannonDamage],(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,m_fCurAnglForwardDamage,m_fEndAnglForwardCharge);
			m_nvCannonDamage += SetRectangleSegVertexTex(&pV[m_nvCannonCharge+m_nvCannonReady+m_nvCannonDamage],.5f,.5f,1.f,1.f,m_fCurAnglForwardDamage,m_fEndAnglForwardCharge);
		}
		if(!FloatCompare(m_fCurAnglBackDamage,m_fEndAnglBackCharge))
		{
			SetRectangleSegVertexPos(&pV[m_nvCannonCharge+m_nvCannonReady+m_nvCannonDamage],(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,m_fEndAnglBackCharge,m_fCurAnglBackDamage);
			m_nvCannonDamage += SetRectangleSegVertexTex(&pV[m_nvCannonCharge+m_nvCannonReady+m_nvCannonDamage],.5f,.5f,1.f,1.f,m_fEndAnglBackCharge,m_fCurAnglBackDamage);
		}

		m_nvCannonCharge -= 2;
		m_nvCannonReady -= 2;
		m_nvCannonDamage -= 2;

		rs->UnLockVertexBuffer(m_idCannonVBuf);
	}
	// change speed buffer
	pV = (BI_ONETEXTURE_VERTEX *)rs->LockVertexBuffer(m_idSpeedVBuf);
	if(pV!=NULL)
	{
		SetRectangleSegVertexPos(pV,(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,m_fBegAnglWindSpeed,m_fCurAnglWindSpeed);
		m_nvSpeed = SetRectangleSegVertexTex(pV,.5f,.5f,1.f,1.f,m_fBegAnglWindSpeed,m_fCurAnglWindSpeed);
		SetRectangleSegVertexPos(&pV[m_nvSpeed],(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth,m_fCurAnglShipSpeed,m_fBegAnglShipSpeed);
		m_nvSpeed += SetRectangleSegVertexTex(&pV[m_nvSpeed],.5f,.5f,1.f,1.f,m_fCurAnglShipSpeed,m_fBegAnglShipSpeed);
		m_nvSpeed -= 2;

		rs->UnLockVertexBuffer(m_idSpeedVBuf);
	}
	// change island buffer
	if(m_bYesIsland) UpdateMiniMap();

	UpdateFireRangeBuffer();

	SetAnotherShip();
}

void BATTLE_NAVIGATOR::UpdateFireRangeBuffer()
{
	BI_NOTEXTURE_VERTEX *pv = (BI_NOTEXTURE_VERTEX *)rs->LockVertexBuffer(m_idFireZoneVBuf);
	if(pv==null) return;

	SHIP_DESCRIBE_LIST::SHIP_DESCR * psd = g_ShipList.GetMainCharacterShip();
	ATTRIBUTES * pAttr = g_ShipList.GetMainCharacterShipAttr();
	if(psd!=null && pAttr!=null)
	{
		FillOneSideFireRange(&pv[0],pAttr,psd->pAttr,"cannonf");
		FillOneSideFireRange(&pv[(BI_ONESIDE_SIZE+1)],pAttr,psd->pAttr,"cannonr");
		FillOneSideFireRange(&pv[2*(BI_ONESIDE_SIZE+1)],pAttr,psd->pAttr,"cannonb");
		FillOneSideFireRange(&pv[3*(BI_ONESIDE_SIZE+1)],pAttr,psd->pAttr,"cannonl");
	}
	rs->UnLockVertexBuffer(m_idFireZoneVBuf);
}

void BATTLE_NAVIGATOR::FillOneSideFireRange(BI_NOTEXTURE_VERTEX * pv,ATTRIBUTES * pShip,ATTRIBUTES * pChar,char * pstr)
{
	if(pv==NULL || pShip==NULL || pChar==NULL || pstr==NULL) return;

	float fDirAng=0.f, fSizeAng=0.f, fFireZone=0.f;
	ATTRIBUTES * pA;
	if((pA=BIUtils::GetAttributesFromPath(pShip,"Cannons","Borts",pstr,0))!=NULL)
	{
		fSizeAng = pA->GetAttributeAsFloat("FireZone",0.f);
		fDirAng = pA->GetAttributeAsFloat("FireDir",0.f);
	}

	if((pA=BIUtils::GetAttributesFromPath(pChar,"Cannons","Borts",pstr,0))!=NULL)
		fFireZone = pA->GetAttributeAsFloat("MaxFireDistance",0.f);
	fFireZone *= m_fMapRadius/(m_fWorldRad*m_fCurScale);
	if(fFireZone>m_fMapRadius) fFireZone = m_fMapRadius;

	float curAng = fDirAng-fSizeAng/2.f;
	float dAng = fSizeAng/(float)(BI_ONESIDE_SIZE-1);
	for(int i=0; i<BI_ONESIDE_SIZE; i++,curAng+=dAng)
	{
		pv[i+1].pos.x = m_XNavigator + sinf(curAng)*fFireZone;
		pv[i+1].pos.y = m_YNavigator - cosf(curAng)*fFireZone*m_fAspectRatio;
	}
}

void BATTLE_NAVIGATOR::Init(VDX8RENDER *RenderService,ENTITY* pOwnerEI)
{
	int i;
	char * tmpstr;

	m_pOwnerEI = pOwnerEI;
	if(RenderService==NULL) return;
	rs = RenderService;

	m_dwChargeCannon = ARGB(255,255,0,0);
	m_dwReadyCannon = ARGB(255,0,255,0);
	m_dwDamagedCannon = ARGB(255,64,64,64);

	// максимальная скорость ветра
	m_fWindMaxStrength = api->Entity_GetAttributeAsFloat(&BIUtils::idBattleInterface,"MaxWind",30.f);
	// и корабля
	m_fMaxShipSpeed = api->Entity_GetAttributeAsFloat(&BIUtils::idBattleInterface,"MaxShipSpeed",20.f);
	//
	m_fShipSpeedScale = api->Entity_GetAttributeAsFloat(&BIUtils::idBattleInterface,"ShipSpeedScaler",1.f);

	ATTRIBUTES * pARoot = api->Entity_GetAttributeClass(&BIUtils::idBattleInterface,"navigation");

	m_fAspectRatio = BIUtils::GetFloatFromAttr(pARoot, "aspectRatio", 1.f);

	// Get navigation window parameters
	m_NavigationWidth = BIUtils::GetLongFromAttr(pARoot, "navigatorWidth", 128);
	m_NavigationHeight = BIUtils::GetLongFromAttr(pARoot, "navigatorHeight", 128);
	m_XNavigator = BIUtils::GetLongFromAttr(pARoot, "rightPos", 640) - m_NavigationWidth/2;
	m_YNavigator = BIUtils::GetLongFromAttr(pARoot, "topPos", 0) + m_NavigationHeight/2;

	// радиус видимости на миникарте
	m_fWorldRad = BIUtils::GetFloatFromAttr(pARoot, "horizontRadius", 400.f);
	m_fMinScale = BIUtils::GetFloatFromAttr(pARoot, "minScale", .01f);
	m_fMaxScale = BIUtils::GetFloatFromAttr(pARoot, "maxScale", 2.f);
	m_fCurScale = m_fDefaultScale = BIUtils::GetFloatFromAttr(pARoot, "curScale", 1.f);
	m_fScaleStep = BIUtils::GetFloatFromAttr(pARoot, "scaleStep", .01f);
	m_fShipShowRad = BIUtils::GetFloatFromAttr(pARoot, "shipShowRadius", 2.f);

	m_windWidth = BIUtils::GetLongFromAttr(pARoot, "windWidth", 20);
	m_windHeight = BIUtils::GetLongFromAttr(pARoot, "windHeight", 158);

	// цвет заряженных пушек
	m_dwReadyCannon = BIUtils::GetLongFromAttr(pARoot, "argbReadyCannonColor", m_dwReadyCannon);
	// цвет заряжающихся пушек
	m_dwChargeCannon = BIUtils::GetLongFromAttr(pARoot, "argbChargeCannonColor", m_dwChargeCannon);
	// цвет поврежденных пушек
	m_dwDamagedCannon = BIUtils::GetLongFromAttr(pARoot, "argbDamageCannonColor", m_dwDamagedCannon);
	// цвет моря
	m_dwSeaColor = BIUtils::GetLongFromAttr(pARoot, "argbSeaColor", ARGB(255,255,255,255));
	// цвет зоны обстрела пушек
	m_dwFireZoneColor = BIUtils::GetLongFromAttr(pARoot, "argbFireZoneColor", ARGB(255,255,255,255));
	// цвет вражеского корабля
	m_dwEnemyShipColor = BIUtils::GetLongFromAttr(pARoot, "argbEnemyShipColor", ARGB(255,255,255,255));
	// цвет своего корабля
	m_dwFrendShipColor = BIUtils::GetLongFromAttr(pARoot, "argbFrendShipColor", ARGB(255,255,255,255));
	// цвет нейтрального корабля
	m_dwNeutralShipColor = BIUtils::GetLongFromAttr(pARoot, "argbNeutralShipColor", ARGB(255,255,255,255));
	// цвет тонущего корабля
	m_dwDeadShipColor = BIUtils::GetLongFromAttr(pARoot, "argbDeadShipColor", ARGB(255,255,255,255));
	// цвет градиента бэка
	m_dwBackGradColor1 = BIUtils::GetLongFromAttr(pARoot, "argbBackMaxColor", ARGB(255,0,0,128));
	m_dwBackGradColor2 = BIUtils::GetLongFromAttr(pARoot, "argbBackMinColor", ARGB(55,0,0,128));

	// get strings parameters
	tmpstr = BIUtils::GetStringFromAttr(pARoot, "speedShowFont", null);
	if(tmpstr==null)	m_speedFont = -1;
	else m_speedFont = rs->LoadFont(tmpstr);
	m_ySpeedShow = m_YNavigator + BIUtils::GetLongFromAttr(pARoot, "speedOutYOffset", -m_NavigationHeight/2);
	m_xShipSpeed = m_XNavigator + BIUtils::GetLongFromAttr(pARoot, "shipSpeedXOffset", 10);
	m_xWindSpeed = m_XNavigator + BIUtils::GetLongFromAttr(pARoot, "windSpeedXOffset", -20);
	m_fFontScale = BIUtils::GetFloatFromAttr(pARoot, "fontScale", 1.f);

	tmpstr = BIUtils::GetStringFromAttr(pARoot, "compasTexture", null);
	if(tmpstr==null)	m_idCompasTex = -1;
	else m_idCompasTex = rs->TextureCreate(tmpstr);

	tmpstr = BIUtils::GetStringFromAttr(pARoot, "speedTexture", null);
	if(tmpstr==null)	m_idSpeedTex = -1;
	else m_idSpeedTex = rs->TextureCreate(tmpstr);

	tmpstr = BIUtils::GetStringFromAttr(pARoot, "cannonsTexture", null);
	if(tmpstr==null)	m_idCannonTex = -1;
	else m_idCannonTex = rs->TextureCreate(tmpstr);

	tmpstr = BIUtils::GetStringFromAttr(pARoot, "emptyTexture", null);
	if(tmpstr==null)	m_idEmptyTex = -1;
	else m_idEmptyTex = rs->TextureCreate(tmpstr);

	tmpstr = BIUtils::GetStringFromAttr(pARoot, "windTexture", null);
	if(tmpstr==null)	m_idWindTex = -1;
	else m_idWindTex = rs->TextureCreate(tmpstr);

	tmpstr = BIUtils::GetStringFromAttr(pARoot, "chargeTexture", null);
	if(tmpstr==null)	m_idChargeTexture = -1;
	else m_idChargeTexture = rs->TextureCreate(tmpstr);

	tmpstr = BIUtils::GetStringFromAttr(pARoot, "sailstateTexture", null);
	if(tmpstr==null)	m_idSailTexture = -1;
	else m_idSailTexture = rs->TextureCreate(tmpstr);

	tmpstr = BIUtils::GetStringFromAttr(pARoot, "windStateTexture", null);
	if(tmpstr==null)	m_idWindTexture = -1;
	else m_idWindTexture = rs->TextureCreate(tmpstr);

	// get cannon charge angles
	m_fBegAnglLeftCharge = (float)BIUtils::GetLongFromAttr(pARoot,"leftChargeBegAngle",0)/180.f*PI;
	m_fEndAnglLeftCharge = (float)BIUtils::GetLongFromAttr(pARoot,"leftChargeEndAngle",0)/180.f*PI;

	m_fBegAnglRightCharge = (float)BIUtils::GetLongFromAttr(pARoot,"rightChargeBegAngle",0)/180.f*PI;
	m_fEndAnglRightCharge = (float)BIUtils::GetLongFromAttr(pARoot,"rightChargeEndAngle",0)/180.f*PI;

	m_fBegAnglForwardCharge = (float)BIUtils::GetLongFromAttr(pARoot,"forwardChargeBegAngle",0)/180.f*PI;
	m_fEndAnglForwardCharge = (float)BIUtils::GetLongFromAttr(pARoot,"forwardChargeEndAngle",0)/180.f*PI;

	m_fBegAnglBackCharge = (float)BIUtils::GetLongFromAttr(pARoot,"backwardChargeBegAngle",0)/180.f*PI;
	m_fEndAnglBackCharge = (float)BIUtils::GetLongFromAttr(pARoot,"backwardChargeEndAngle",0)/180.f*PI;

	// get speed angles
	m_fBegAnglShipSpeed = (float)BIUtils::GetLongFromAttr(pARoot,"shipSpeedBegAngle",0)/180.f*PI;
	m_fCurAnglShipSpeed = m_fEndAnglShipSpeed = (float)BIUtils::GetLongFromAttr(pARoot,"shipSpeedEndAngle",0)/180.f*PI;
	m_fBegAnglWindSpeed = (float)BIUtils::GetLongFromAttr(pARoot,"windSpeedBegAngle",0)/180.f*PI;
	m_fCurAnglWindSpeed = m_fEndAnglWindSpeed = (float)BIUtils::GetLongFromAttr(pARoot,"windSpeedEndAngle",0)/180.f*PI;

	// текущий тип заряда
	m_ChargeGreed.x = 1; m_ChargeGreed.y = 1;
	if( (tmpstr=BIUtils::GetStringFromAttr(pARoot,"chargeTextureGreed",null))!=null )
		sscanf(tmpstr,"%d,%d",&m_ChargeGreed.x,&m_ChargeGreed.y);
	if(m_ChargeGreed.x<1) m_ChargeGreed.x = 1;
	if(m_ChargeGreed.y<1) m_ChargeGreed.y = 1;
	m_ChargePos.x = 160;	m_ChargePos.y = 160;
	if( (tmpstr=BIUtils::GetStringFromAttr(pARoot,"chargePos",null))!=null )
		sscanf(tmpstr,"%d,%d",&m_ChargePos.x,&m_ChargePos.y);
	m_ChargePos.x += m_XNavigator;// - m_NavigationWidth/2;
	m_ChargePos.y += m_YNavigator;// - m_NavigationHeight/2;
	m_ChargeSize.x = 32;	m_ChargeSize.y = 32;
	if( (tmpstr=BIUtils::GetStringFromAttr(pARoot,"chargePictureSize",null))!=null )
		sscanf(tmpstr,"%d,%d",&m_ChargeSize.x,&m_ChargeSize.y);

	// иконка ветра
	m_curSailState = 0;
	m_WindGreed.x = 1; m_WindGreed.y = 1;
	if( (tmpstr=BIUtils::GetStringFromAttr(pARoot,"windTextureGreed",null))!=null )
		sscanf(tmpstr,"%d,%d",&m_WindGreed.x,&m_WindGreed.y);
	if(m_WindGreed.x<1) m_WindGreed.x = 1;
	if(m_WindGreed.y<1) m_WindGreed.y = 1;
	m_WindPos.x = 160;	m_WindPos.y = 160;
	if( (tmpstr=BIUtils::GetStringFromAttr(pARoot,"windPos",null))!=null )
		sscanf(tmpstr,"%d,%d",&m_WindPos.x,&m_WindPos.y);
	m_WindPos.x += m_XNavigator;
	m_WindPos.y += m_YNavigator;
	m_WindSize.x = 32;	m_WindSize.y = 32;
	if( (tmpstr=BIUtils::GetStringFromAttr(pARoot,"windPictureSize",null))!=null )
		sscanf(tmpstr,"%d,%d",&m_WindSize.x,&m_WindSize.y);

	// иконка положения парусов
	m_curSailState = 0;
	m_SailGreed.x = 1; m_SailGreed.y = 1;
	if( (tmpstr=BIUtils::GetStringFromAttr(pARoot,"sailstateTextureGreed",null))!=null )
		sscanf(tmpstr,"%d,%d",&m_SailGreed.x,&m_SailGreed.y);
	if(m_SailGreed.x<1) m_SailGreed.x = 1;
	if(m_SailGreed.y<1) m_SailGreed.y = 1;
	m_SailPos.x = 160;	m_SailPos.y = 160;
	if( (tmpstr=BIUtils::GetStringFromAttr(pARoot,"sailstatePos",null))!=null )
		sscanf(tmpstr,"%d,%d",&m_SailPos.x,&m_SailPos.y);
	m_SailPos.x += m_XNavigator;
	m_SailPos.y += m_YNavigator;
	m_SailSize.x = 32;	m_SailSize.y = 32;
	if( (tmpstr=BIUtils::GetStringFromAttr(pARoot,"sailstatePictureSize",null))!=null )
		sscanf(tmpstr,"%d,%d",&m_SailSize.x,&m_SailSize.y);

	// create buffers
	m_idEmptyVBuf = rs->CreateVertexBuffer(BI_ONETEX_VERTEX_FORMAT,(4+4+4)*sizeof(BI_ONETEXTURE_VERTEX),D3DUSAGE_WRITEONLY);
	m_idCannonVBuf = rs->CreateVertexBuffer(BI_ONETEX_VERTEX_FORMAT,7*4*sizeof(BI_ONETEXTURE_VERTEX),D3DUSAGE_WRITEONLY);
	m_idSpeedVBuf = rs->CreateVertexBuffer(BI_ONETEX_VERTEX_FORMAT,7*2*sizeof(BI_ONETEXTURE_VERTEX),D3DUSAGE_WRITEONLY);
	m_idMapVBuf = rs->CreateVertexBuffer(BI_ONETEX_VERTEX_FORMAT,(RADIAL_QUANTITY+2)*sizeof(BI_ONETEXTURE_VERTEX),D3DUSAGE_WRITEONLY);
	m_idFireZoneVBuf = rs->CreateVertexBuffer(BI_NOTEX_VERTEX_FORMAT,FIRERANGE_QUANTITY*sizeof(BI_NOTEXTURE_VERTEX),D3DUSAGE_WRITEONLY);
	m_idShipsVBuf = rs->CreateVertexBuffer(BI_COLORONLY_VERTEX_FORMAT,MAX_ENEMY_SHIP_QUANTITY*3*sizeof(BI_COLORONLY_VERTEX),D3DUSAGE_WRITEONLY);
	m_idGradBackVBuf = rs->CreateVertexBuffer(BI_COLORONLY_VERTEX_FORMAT,3*sizeof(BI_COLORONLY_VERTEX),D3DUSAGE_WRITEONLY);
	m_idCurChargeVBuf = rs->CreateVertexBuffer(BI_ONETEX_VERTEX_FORMAT,3*4*sizeof(BI_ONETEXTURE_VERTEX),D3DUSAGE_WRITEONLY);
	if(m_idEmptyVBuf==-1 || m_idCannonVBuf==-1 || m_idSpeedVBuf==-1 || m_idMapVBuf==-1 || m_idFireZoneVBuf==-1 || m_idShipsVBuf==-1 || m_idGradBackVBuf==-1 || m_idCurChargeVBuf==-1)
	{
		_THROW("Can`t create vertex\index buffer");
	}
	m_nvShips = 0;

	// fill constant value for vertex buffer
	BI_ONETEXTURE_VERTEX * pV = (BI_ONETEXTURE_VERTEX *)rs->LockVertexBuffer(m_idEmptyVBuf);
	if(pV!=NULL)
	{
		for(i=0; i<12; i++)
		{
			pV[i].w = .5f;
			pV[i].pos.z = 1.f;
		}
		SetRectangleVertexPos(&pV[0],(float)m_XNavigator,(float)m_YNavigator,(float)m_NavigationWidth,(float)m_NavigationWidth);
		SetRectangleVertexTex(&pV[0]);
		SetRectangleVertexTex(&pV[4]);
		SetRectangleVertexTex(&pV[8]);
		rs->UnLockVertexBuffer(m_idEmptyVBuf);
	}
	// cannon charge vertexs
	pV = (BI_ONETEXTURE_VERTEX *)rs->LockVertexBuffer(m_idCannonVBuf);
	if(pV!=NULL)
	{
		for(i=0; i<7*4; i++)
		{
			pV[i].pos.z = 1.f;
			pV[i].w = .5f;
		}
		rs->UnLockVertexBuffer(m_idCannonVBuf);
	}
	// speed show vertexs
	pV = (BI_ONETEXTURE_VERTEX *)rs->LockVertexBuffer(m_idSpeedVBuf);
	if(pV!=NULL)
	{
		for(i=0; i<7*2; i++)
		{
			pV[i].pos.z = 1.f;
			pV[i].w = .5f;
		}
		rs->UnLockVertexBuffer(m_idSpeedVBuf);
	}
	// map vertexes
	m_fMapRadius = BIUtils::GetFloatFromAttr(pARoot,"mapRadius",100.f);
	pV = (BI_ONETEXTURE_VERTEX *)rs->LockVertexBuffer(m_idMapVBuf);
	if(pV!=NULL)
	{
		for(i=0; i<RADIAL_QUANTITY+2; i++)
		{
			pV[i].w = .5f;
			pV[i].pos.z = 1.f;
		}
		SetCircleVertexPos(pV,(float)m_XNavigator,(float)m_YNavigator,m_fMapRadius);
		SetCircleVertexTex(pV);
		rs->UnLockVertexBuffer(m_idMapVBuf);
	}
	// зона поражения пушками
	BI_NOTEXTURE_VERTEX *pv = (BI_NOTEXTURE_VERTEX*)rs->LockVertexBuffer(m_idFireZoneVBuf);
	if(pv!=NULL)
	{
		CVECTOR vCenter = CVECTOR((float)m_XNavigator,(float)m_YNavigator,1.f);
		for(i=0; i<FIRERANGE_QUANTITY; i++)
		{
			pv[i].w = .5f;
			pv[i].pos = vCenter;
		}
		rs->UnLockVertexBuffer(m_idFireZoneVBuf);
	}
	// корабли на карте
	BI_COLORONLY_VERTEX * pcv = (BI_COLORONLY_VERTEX*)rs->LockVertexBuffer(m_idShipsVBuf);
	if(pcv!=NULL)
	{
		for(i=0; i<MAX_ENEMY_SHIP_QUANTITY*3; i++)
		{
			pcv[i].w = .5f;
			pcv[i].pos.z = 1.f;
		}
		rs->UnLockVertexBuffer(m_idShipsVBuf);
	}

	// треугольный градиент для подложки
	pcv = (BI_COLORONLY_VERTEX*)rs->LockVertexBuffer(m_idGradBackVBuf);
	if(pcv!=NULL)
	{
		pcv[0].w = pcv[1].w = pcv[2].w = 0.5f;
		pcv[0].pos.z = pcv[1].pos.z = pcv[2].pos.z = 1.f;
#ifndef _XBOX
		pcv[0].pos.x = (float)m_XNavigator + m_NavigationWidth/2;
		pcv[0].pos.y = (float)m_YNavigator - m_NavigationHeight/2;
		pcv[1].pos.x = (float)m_XNavigator - NAVBACKGROUND_SIZESCALE*m_NavigationWidth/2;
		pcv[1].pos.y = (float)m_YNavigator - m_NavigationHeight/2;
		pcv[2].pos.x = (float)m_XNavigator + m_NavigationWidth/2;
		pcv[2].pos.y = (float)m_YNavigator + NAVBACKGROUND_SIZESCALE*m_NavigationHeight/2;
#else
		pcv[0].pos.x = 720.f;
		pcv[0].pos.y = 0.f;
		pcv[1].pos.x = (float)m_XNavigator - NAVBACKGROUND_SIZESCALE*m_NavigationWidth/2;
		pcv[1].pos.y = 0.f;
		pcv[2].pos.x = 720.f;
		pcv[2].pos.y = (float)m_YNavigator + NAVBACKGROUND_SIZESCALE*m_NavigationHeight/2;
#endif

		pcv[0].col = m_dwBackGradColor1;
		pcv[1].col = pcv[2].col = m_dwBackGradColor2;
		rs->UnLockVertexBuffer(m_idGradBackVBuf);
	}

	// Текущий тип заряда
	pV = (BI_ONETEXTURE_VERTEX *)rs->LockVertexBuffer(m_idCurChargeVBuf);
	if(pV!=NULL)
	{
		for(i=0; i<4*3; i++)
		{
			pV[i].pos.z = 1.f;
			pV[i].w = .5f;
		}
		SetRectangleVertexPos(&pV[0], (float)m_ChargePos.x,(float)m_ChargePos.y,
								(float)m_ChargeSize.x, (float)m_ChargeSize.y);
		SetRectangleVertexPos(&pV[4], (float)m_WindPos.x,(float)m_WindPos.y,
			(float)m_WindSize.x, (float)m_WindSize.y);
		SetRectangleVertexPos(&pV[8], (float)m_SailPos.x,(float)m_SailPos.y,
			(float)m_SailSize.x, (float)m_SailSize.y);
		rs->UnLockVertexBuffer(m_idCurChargeVBuf);
	}
}

long BATTLE_NAVIGATOR::SetRectangleVertexPos(BI_ONETEXTURE_VERTEX *v, float x,float y, float width,float height, float angle)
{
	if(v==NULL) return 0;

	if(angle==0)
	{
		v[2].pos.x = v[3].pos.x = (v[0].pos.x = v[1].pos.x = x-width/2) + width;
		v[1].pos.y = v[3].pos.y = (v[0].pos.y = v[2].pos.y = y-height/2*m_fAspectRatio) + height*m_fAspectRatio;
	}
	else
	{
		float ca = cosf(angle);
		float sa = sinf(angle);
		float wca = width/2*ca;
		float wsa = width/2*sa;
		float hca = height/2*ca;
		float hsa = height/2*sa;
		v[0].pos.x = x + (-wca + hsa);
		v[0].pos.y = y + (-wsa - hca)*m_fAspectRatio;
		v[1].pos.x = x + (-wca - hsa);
		v[1].pos.y = y + (-wsa + hca)*m_fAspectRatio;
		v[2].pos.x = x + (wca + hsa);
		v[2].pos.y = y + (wsa - hca)*m_fAspectRatio;
		v[3].pos.x = x + (wca - hsa);
		v[3].pos.y = y + (wsa + hca)*m_fAspectRatio;
	}

	return 4;
}

long BATTLE_NAVIGATOR::SetRectangleVertexTex(BI_ONETEXTURE_VERTEX *v, float x,float y, float width,float height, float angle)
{
	if(v==NULL) return 0;

	if(angle==0)
	{
		v[2].tu = v[3].tu = (v[0].tu = v[1].tu = x-width/2) + width;
		v[1].tv = v[3].tv = (v[0].tv = v[2].tv = y-height/2) + height;
	}
	else
	{
		float ca = cosf(angle);
		float sa = sinf(angle);
		float wca = width/2*ca;
		float wsa = width/2*sa;
		float hca = height/2*ca;
		float hsa = height/2*sa;
		v[0].tu = x + (-wca + hsa);
		v[0].tv = y + (-wsa - hca);
		v[1].tu = x + (-wca - hsa);
		v[1].tv = y + (-wsa + hca);
		v[2].tu = x + (wca + hsa);
		v[2].tv = y + (wsa - hca);
		v[3].tu = x + (wca - hsa);
		v[3].tv = y + (wsa + hca);
	}

	return 4;
}

long BATTLE_NAVIGATOR::SetCircleVertexPos(BI_ONETEXTURE_VERTEX * v, float x,float y, float rad, float angle)
{
	if(v==NULL) return 0;

	float addAngle=angle;
	int idx=0;
	v[idx].pos.x = x;
	v[idx].pos.y = y;
	idx++;

	for(int i=0; i<RADIAL_QUANTITY+1; i++)
	{
		v[idx].pos.x = x+rad*sinf(addAngle);
		v[idx].pos.y = y+rad*cosf(addAngle)*m_fAspectRatio;
		addAngle += 2.f*PI/RADIAL_QUANTITY;
		idx++;
	}
	v[idx].pos.x = x+rad*sinf(angle);
	v[idx].pos.y = y+rad*cosf(angle)*m_fAspectRatio;

	return RADIAL_QUANTITY+2;
}

long BATTLE_NAVIGATOR::SetCircleVertexTex(BI_ONETEXTURE_VERTEX * v, float x,float y, float rad, float angle)
{
	if(v==NULL) return 0;

	float addAngle=angle;
	int idx=0;
	v[idx].tu = x;
	v[idx].tv = y;
	idx++;

	for(int i=0; i<RADIAL_QUANTITY+1; i++)
	{
		v[idx].tu = x+rad*sinf(addAngle);
		v[idx].tv = y+rad*cosf(addAngle);
		addAngle += 2.f*PI/RADIAL_QUANTITY;
		idx++;
	}
	v[idx].tu = x+rad*sinf(angle);
	v[idx].tv = y+rad*sinf(angle);

	return RADIAL_QUANTITY+2;
}

void BATTLE_NAVIGATOR::SetMainCharacterData()
{
	m_fCurAnglLeftCharge = 0;
	m_fCurAnglRightCharge = 0;
	m_fCurAnglForwardCharge = 0;
	m_fCurAnglBackCharge = m_fBegAnglBackCharge;
	SHIP_DESCRIBE_LIST::SHIP_DESCR * psd = g_ShipList.GetMainCharacterShip();
	if(psd==NULL) return;

	// get map center
	if(g_bExternMapCenter)
	{
		m_fXPos = g_externPos.x;
		m_fYPos = g_externPos.z;
	}
	else
	{
		CVECTOR cPos = psd->pShip->GetPos();
		m_fXPos = cPos.x;
		m_fYPos = cPos.z;
	}
	if(g_bMapScaling)
	{
		g_bMapScaling = false;
		m_fCurScale = m_fDefaultScale;//m_fMaxScale;
	}
	// get ship y angle
	CVECTOR cAng = psd->pShip->GetAng();
	m_fAngle = cAng.y;
	m_fShipSpeed = ((SHIP_BASE*)psd->pShip)->State.vSpeed.z;
	m_fShipSpeed *= m_fShipSpeedScale; // boal приведение скорости тут нужнее
	if(m_fShipSpeed>m_fMaxShipSpeed)
		m_fCurAnglShipSpeed = m_fEndAnglShipSpeed;
	else
		m_fCurAnglShipSpeed = GetBetwinFloat(m_fBegAnglShipSpeed,m_fEndAnglShipSpeed,m_fShipSpeed/m_fMaxShipSpeed);
	//m_fShipSpeed *= m_fShipSpeedScale;

    // получим значение ветра
	UpdateWindParam();
	if(m_fWindStrength>m_fWindMaxStrength)
		m_fCurAnglWindSpeed = m_fEndAnglWindSpeed;
	else
		m_fCurAnglWindSpeed = GetBetwinFloat(m_fBegAnglWindSpeed,m_fEndAnglWindSpeed,m_fWindStrength/m_fWindMaxStrength);

	m_fCurAnglLeftCharge = m_fBegAnglLeftCharge;
	m_fCurAnglRightCharge = m_fBegAnglRightCharge;
	m_fCurAnglForwardCharge = m_fBegAnglForwardCharge;
	m_fCurAnglBackCharge = m_fBegAnglBackCharge;
	ATTRIBUTES * pBortsAttr = BIUtils::GetAttributesFromPath(psd->pAttr,"cannons","borts",0);
	if(pBortsAttr!=NULL)
	{
		ATTRIBUTES * pTmpAttr;
		float fCharge, fDamage;
		// left cannons
		if( (pTmpAttr=pBortsAttr->GetAttributeClass("cannonl"))!=NULL )
		{
			fCharge = pTmpAttr->GetAttributeAsFloat("ChargeRatio",0);
			fDamage = pTmpAttr->GetAttributeAsFloat("DamageRatio",0);
			m_fCurAnglLeftDamage = GetBetwinFloat(m_fEndAnglLeftCharge,m_fBegAnglLeftCharge,fDamage);
			m_fCurAnglLeftCharge = GetBetwinFloat(m_fBegAnglLeftCharge,m_fCurAnglLeftDamage,fCharge);
		}
		// right cannons
		if( (pTmpAttr=pBortsAttr->GetAttributeClass("cannonr"))!=NULL )
		{
			fCharge = pTmpAttr->GetAttributeAsFloat("ChargeRatio",0);
			fDamage = pTmpAttr->GetAttributeAsFloat("DamageRatio",0);
			m_fCurAnglRightDamage = GetBetwinFloat(m_fEndAnglRightCharge,m_fBegAnglRightCharge,fDamage);
			m_fCurAnglRightCharge = GetBetwinFloat(m_fBegAnglRightCharge,m_fCurAnglRightDamage,fCharge);
		}
		// forward cannons
		if( (pTmpAttr=pBortsAttr->GetAttributeClass("cannonf"))!=NULL )
		{
			fCharge = pTmpAttr->GetAttributeAsFloat("ChargeRatio",0);
			fDamage = pTmpAttr->GetAttributeAsFloat("DamageRatio",0);
			m_fCurAnglForwardDamage = GetBetwinFloat(m_fEndAnglForwardCharge,m_fBegAnglForwardCharge,fDamage);
			m_fCurAnglForwardCharge = GetBetwinFloat(m_fBegAnglForwardCharge,m_fCurAnglForwardDamage,fCharge);
		}
		// backward cannons
		if( (pTmpAttr=pBortsAttr->GetAttributeClass("cannonb"))!=NULL )
		{
			fCharge = pTmpAttr->GetAttributeAsFloat("ChargeRatio",0);
			fDamage = pTmpAttr->GetAttributeAsFloat("DamageRatio",0);
			m_fCurAnglBackDamage = GetBetwinFloat(m_fEndAnglBackCharge,m_fBegAnglBackCharge,fDamage);
			m_fCurAnglBackCharge = GetBetwinFloat(m_fBegAnglBackCharge,m_fCurAnglBackDamage,fCharge);
		}
	}
}

void BATTLE_NAVIGATOR::SetAnotherShip()
{
	BI_COLORONLY_VERTEX * pv = (BI_COLORONLY_VERTEX*)rs->LockVertexBuffer(m_idShipsVBuf);
	if(pv==NULL) return;
	SHIP_DESCRIBE_LIST::SHIP_DESCR *pMainCharacter=g_ShipList.GetMainCharacterShip();
	int idx=0;
	float fSqrMapRad = m_fMapRadius*m_fMapRadius;

	m_nvShips=0L;
	// Fill ships buffer
	for(SHIP_DESCRIBE_LIST::SHIP_DESCR *psd=g_ShipList.GetShipRoot(); psd!=NULL; psd=psd->next)
	{
		// не рисовать корабль основного героя
		if(psd == pMainCharacter) continue;
		// определить координаты корабля и если они за пределами карты, то не рисовать корабль
		float fX = (psd->pShip->GetPos().x-m_fXPos)*m_fMapRadius/(m_fWorldRad*m_fCurScale);
		float fY = (psd->pShip->GetPos().z-m_fYPos)*m_fMapRadius/(m_fWorldRad*m_fCurScale);
		if(fX*fX+fY*fY>fSqrMapRad) continue;
		float tmp=fX*cosf(m_fAngle)-fY*sinf(m_fAngle);
		fY=m_YNavigator-(fY*cosf(m_fAngle)+fX*sinf(m_fAngle))*m_fAspectRatio;
		fX=tmp+m_XNavigator;
		// определить цвет отображаемого корабля
		DWORD dwColor = 0xFFFFFFFF;
		if( psd->dwShipColor==0 ) {
			switch(psd->relation)
			{
			case BI_RELATION_ENEMY:
				dwColor = m_dwEnemyShipColor;
				break;
			case BI_RELATION_FRIEND:
				dwColor = m_dwFrendShipColor;
				break;
			case BI_RELATION_NEUTRAL:
				dwColor = m_dwNeutralShipColor;
				break;
			}
		} else dwColor = psd->dwShipColor;
		if(psd->isDead)
			dwColor = m_dwDeadShipColor;
		// определить угол корабля
		float fAngle = psd->pShip->GetAng().y-m_fAngle;
		// заполнить буфер
		pv[idx].col = pv[idx+1].col = pv[idx+2].col = dwColor;
		pv[idx+0].pos.x = fX+m_fShipShowRad*sinf(fAngle);			pv[idx+0].pos.y = fY-m_fShipShowRad*cosf(fAngle)*m_fAspectRatio;
		pv[idx+1].pos.x = fX+m_fShipShowRad*sinf(fAngle-PI*.9f);	pv[idx+1].pos.y = fY-m_fShipShowRad*cosf(fAngle-PI*.8f)*m_fAspectRatio;
		pv[idx+2].pos.x = fX+m_fShipShowRad*sinf(fAngle+PI*.9f);	pv[idx+2].pos.y = fY-m_fShipShowRad*cosf(fAngle+PI*.8f)*m_fAspectRatio;
		idx+=3; m_nvShips+=3;
	}

	rs->UnLockVertexBuffer(m_idShipsVBuf);
}

void BATTLE_NAVIGATOR::ReleaseAll()
{
	m_wb = 0;
	m_pAWeather = 0;
	m_bYesIsland = false;

	// текстуры
	TEXTURE_RELEASE(rs,m_idCompasTex);
	TEXTURE_RELEASE(rs,m_idSpeedTex);
	TEXTURE_RELEASE(rs,m_idCannonTex);
	TEXTURE_RELEASE(rs,m_idEmptyTex);
	TEXTURE_RELEASE(rs,m_idIslandTexture);
	TEXTURE_RELEASE(rs,m_idChargeTexture);
	TEXTURE_RELEASE(rs,m_idWindTex);
	TEXTURE_RELEASE(rs,m_idWindTexture);
	TEXTURE_RELEASE(rs,m_idSailTexture);

	if(m_pIslandTexture!=NULL)
	{
		rs->Release(m_pIslandTexture);
		m_pIslandTexture=NULL;
	}

	VERTEX_BUFFER_RELEASE(rs,m_idEmptyVBuf);
	VERTEX_BUFFER_RELEASE(rs,m_idMapVBuf);
	VERTEX_BUFFER_RELEASE(rs,m_idCannonVBuf);
	VERTEX_BUFFER_RELEASE(rs,m_idSpeedVBuf);
	VERTEX_BUFFER_RELEASE(rs,m_idShipsVBuf);
	VERTEX_BUFFER_RELEASE(rs,m_idFireZoneVBuf);
	VERTEX_BUFFER_RELEASE(rs,m_idGradBackVBuf);
	VERTEX_BUFFER_RELEASE(rs,m_idCurChargeVBuf);

	if(m_speedFont!=-1L)
	{
		rs->UnloadFont(m_speedFont);
		m_speedFont = -1L;
	}

	rs=NULL;
}

float BATTLE_NAVIGATOR::CalculateCrossX(int side, float w, float h, float angl)
{
	switch(side)
	{
	case 0:
		return h/2*tanf(angl);
	case 1:
		return w/2;
	case 2:
		return -h/2*tanf(angl);
	case 3:
		return -w/2;
	}
	return 0.f;
}

float BATTLE_NAVIGATOR::CalculateCrossY(int side, float w, float h, float angl)
{
	switch(side)
	{
	case 0:
		return -h/2;
	case 1:
		return w/2*tanf(angl-PI/2);
	case 2:
		return h/2;
	case 3:
		return -w/2*tanf(angl-PI/2);
	}
	return 0.f;
}

long BATTLE_NAVIGATOR::SetRectangleSegVertexPos(BI_ONETEXTURE_VERTEX *v, float x,float y, float width,float height, float begAngle, float endAngle)
{
	if(v==NULL) return 0;

	while(begAngle<0) begAngle+=2*PI;
	while(begAngle>2*PI) begAngle-=2*PI;
	while(endAngle<0) endAngle+=2*PI;
	while(endAngle>2*PI) endAngle-=2*PI;

	float alpha = atan2f(width/2,height/2);

	int begseg = begAngle<alpha?0:begAngle<(PI-alpha)?1:begAngle<(PI+alpha)?2:begAngle<(2*PI-alpha)?3:0;
	int endseg = endAngle<alpha?0:endAngle<(PI-alpha)?1:endAngle<(PI+alpha)?2:endAngle<(2*PI-alpha)?3:0;

	v[0].pos.x = x;
	v[0].pos.y = y;

	int idx=1;
	int curSide=begseg;
	while(true)
	{
		if(curSide==begseg)
		{
			v[idx].pos.x = x + CalculateCrossX(curSide,width,height,begAngle);
			v[idx].pos.y = y + CalculateCrossY(curSide,width,height,begAngle)*m_fAspectRatio;
			idx++;
		}
		if(curSide==endseg)
		{
			v[idx].pos.x = x + CalculateCrossX(curSide,width,height,endAngle);
			v[idx].pos.y = y + CalculateCrossY(curSide,width,height,endAngle)*m_fAspectRatio;
			idx++;
			break;
		}

		switch(curSide)
		{
		case 0:
			v[idx].pos.x = x + width/2;
			v[idx].pos.y = y - height/2*m_fAspectRatio;
			idx++;
			break;
		case 1:
			v[idx].pos.x = x + width/2;
			v[idx].pos.y = y + height/2*m_fAspectRatio;
			idx++;
			break;
		case 2:
			v[idx].pos.x = x - width/2;
			v[idx].pos.y = y + height/2*m_fAspectRatio;
			idx++;
			break;
		case 3:
			v[idx].pos.x = x - width/2;
			v[idx].pos.y = y - height/2*m_fAspectRatio;
			idx++;
			break;
		}
		curSide++;
		if(curSide>3) curSide=0;
	}

	return idx;
}

long BATTLE_NAVIGATOR::SetRectangleSegVertexTex(BI_ONETEXTURE_VERTEX *v, float x,float y, float width,float height, float begAngle, float endAngle)
{
	if(v==NULL) return 0;

	while(begAngle<0) begAngle+=2*PI;
	while(begAngle>2*PI) begAngle-=2*PI;
	while(endAngle<0) endAngle+=2*PI;
	while(endAngle>2*PI) endAngle-=2*PI;

	float alpha = atan2f(width/2,height/2);

	int begseg = begAngle<alpha?0:begAngle<(PI-alpha)?1:begAngle<(PI+alpha)?2:begAngle<(2*PI-alpha)?3:0;
	int endseg = endAngle<alpha?0:endAngle<(PI-alpha)?1:endAngle<(PI+alpha)?2:endAngle<(2*PI-alpha)?3:0;

	v[0].tu = x;
	v[0].tv = y;

	int idx=1;
	int curSide=begseg;
	while(1)
	{
		if(curSide==begseg)
		{
			v[idx].tu = x + CalculateCrossX(curSide,width,height,begAngle);
			v[idx].tv = y + CalculateCrossY(curSide,width,height,begAngle);
			idx++;
		}
		if(curSide==endseg)
		{
			v[idx].tu = x + CalculateCrossX(curSide,width,height,endAngle);
			v[idx].tv = y + CalculateCrossY(curSide,width,height,endAngle);
			idx++;
			break;
		}

		switch(curSide)
		{
		case 0:
			v[idx].tu = x + width/2;
			v[idx].tv = y - height/2;
			idx++;
			break;
		case 1:
			v[idx].tu = x + width/2;
			v[idx].tv = y + height/2;
			idx++;
			break;
		case 2:
			v[idx].tu = x - width/2;
			v[idx].tv = y + height/2;
			idx++;
			break;
		case 3:
			v[idx].tu = x - width/2;
			v[idx].tv = y - height/2;
			idx++;
			break;
		}
		curSide++;
		if(curSide>3) curSide=0;
	}

	return idx;
}

void BATTLE_NAVIGATOR::SetIsland()
{
	ENTITY_ID ei;
	MODEL * pM=NULL;
	CVECTOR posCenter;
	float islSize = 0;
	if( api->FindClass(&ei,"ISLAND",0) ||
		api->FindClass(&ei,"NetIsland",0) )
	{
		ISLAND_BASE * pIsl = (ISLAND_BASE*)api->GetEntityPointer(&ei);
		if(pIsl!=NULL)
		{
			pM = (MODEL*)api->GetEntityPointer(&pIsl->GetModelEID());
		}
	}
	if(pM!=NULL)
	{
		pM->Update();
		// Get island size
		float xMin=0.f,yMin=0.f,zMin=0.f, xMax=0.f,yMax=0.f,zMax=0.f;
		posCenter = pM->mtx.Pos();
		xMin=xMax=posCenter.x; zMin=zMax=posCenter.z; yMin=yMax=posCenter.y;
		int i=0;
		for(NODE * pN=pM->GetNode(0); pN!=NULL; pN=pM->GetNode(++i))
		{
			GEOS::INFO gi;
			pN->geo->GetInfo(gi);
			posCenter = pN->glob_mtx * *(CVECTOR*)&gi.boxcenter;
			float xmin = posCenter.x-gi.boxsize.x/2;
			float xmax = posCenter.x+gi.boxsize.x/2;
			float zmin = posCenter.z-gi.boxsize.z/2;
			float zmax = posCenter.z+gi.boxsize.z/2;
			if(xmin<xMin) xMin=xmin;
			if(xmax>xMax) xMax=xmax;
			if(zmin<zMin) zMin=zmin;
			if(zmax>zMax) zMax=zmax;
		}
		posCenter.x=(xMin+xMax)/2.f;
		posCenter.z=(zMin+zMax)/2.f;
		posCenter.y=0.f;
		islSize = (xMax-xMin)>(zMax-zMin)?(xMax-xMin):(zMax-zMin);
		islSize *= 1.1f;

		IDirect3DSurface8 *pRenderTarg=NULL, *pOldRenderTarg=NULL;
		if( rs->GetRenderTarget(&pOldRenderTarg)==D3D_OK )
		{
			if( rs->CreateTexture(MAP_TEXTURE_WIDTH,MAP_TEXTURE_HEIGHT,1,D3DUSAGE_RENDERTARGET,D3DFMT_R5G6B5,D3DPOOL_DEFAULT,&m_pIslandTexture) == D3D_OK )
			{
				// SIMPLE OUT
				if( rs->GetSurfaceLevel(m_pIslandTexture,0,&pRenderTarg) == D3D_OK )
				{
					IDirect3DSurface8 * pStencil;
					rs->GetDepthStencilSurface(&pStencil);
					if( rs->SetRenderTarget(pRenderTarg,NULL) == D3D_OK )
					{
						CMatrix matw,matv,oldmatv;
						D3DXMATRIX matp,oldmatp;
						matv.BuildViewMatrix(posCenter+CVECTOR(0.f,islSize/2.f,0.f),posCenter,CVECTOR(0.f,0.f,1.f));
						rs->GetTransform(D3DTS_VIEW,(D3DXMATRIX*)&oldmatv);
					    rs->SetTransform(D3DTS_VIEW,(D3DXMATRIX*)&matv);
						rs->GetTransform(D3DTS_PROJECTION,&oldmatp);
						ZERO(matp);
						matp._11 = matp._22 = 2.f/islSize;
						matp._33 = 1.f/islSize;
						matp._44 = 1.f;
						rs->SetTransform(D3DTS_PROJECTION,(D3DXMATRIX*)&matp);
						// fill fone color
						rs->Clear(0,NULL,D3DCLEAR_TARGET,m_dwSeaColor,1.f,0);
						// show island
						if( rs->TechniqueExecuteStart("battle_island_gettexture") )
						{
							pM->Realize(1);
							while(rs->TechniqueExecuteNext());
						}
						rs->SetRenderTarget(pOldRenderTarg,pStencil);
						rs->SetTransform(D3DTS_VIEW,(D3DXMATRIX*)&oldmatv);
						rs->SetTransform(D3DTS_PROJECTION,&oldmatp);
					}
					pStencil->Release();
					pRenderTarg->Release();
				}
			}
			pOldRenderTarg->Release();
		}
	}

	if(m_pIslandTexture!=NULL) m_bYesIsland = true;
	else m_bYesIsland = false;
	m_fXIsland = posCenter.x;
	m_fYIsland = posCenter.z;
	m_fIslandWidth = m_fIslandHeight = islSize;
	m_fIslandRadius = sqrtf(SQR(m_fIslandWidth)+SQR(m_fIslandHeight));
}

void BATTLE_NAVIGATOR::UpdateMiniMap()
{
	float x,y,radius;

	x = .5f + (m_fXPos-m_fXIsland)/m_fIslandWidth;
	y = .5f - (m_fYPos-m_fYIsland)/m_fIslandHeight;
	radius=m_fWorldRad*m_fCurScale/m_fIslandRadius;

	BI_ONETEXTURE_VERTEX * pV = (BI_ONETEXTURE_VERTEX *)rs->LockVertexBuffer(m_idMapVBuf);
	if(pV!=NULL)
	{
		SetCircleVertexTex(pV,x,y,radius,-m_fAngle);
		rs->UnLockVertexBuffer(m_idMapVBuf);
	}
}

void BATTLE_NAVIGATOR::UpdateCurrentCharge()
{
	VDATA * pVD = api->Event("GetCurrentCharge");
	if(pVD==NULL) return;
	long curCharge = m_curCharge;
	long curSailState = m_curSailState;
	long curWindPic = m_curWindPic;
	pVD->Get(curCharge,0);
	pVD->Get(curSailState,1);
	pVD->Get(curWindPic,2);
	if(curCharge==m_curCharge && curSailState==m_curSailState && curWindPic==m_curWindPic) return;
	m_curCharge = curCharge;
	m_curSailState = curSailState;
	m_curWindPic = curWindPic;
	if(m_curCharge<0) m_curCharge = 0;
	if(m_curSailState<0) m_curSailState = 0;
	if(m_curWindPic<0) m_curWindPic = 0;

	BI_ONETEXTURE_VERTEX * pV = (BI_ONETEXTURE_VERTEX*)rs->LockVertexBuffer(m_idCurChargeVBuf);
	if(pV!=NULL)
	{
		FRECT texRect;
		CalculateTextureRect(texRect,m_curCharge,m_ChargeGreed.x,m_ChargeGreed.y);
		pV[0].tu = texRect.left;		pV[0].tv = texRect.top;
		pV[1].tu = texRect.left;		pV[1].tv = texRect.bottom;
		pV[2].tu = texRect.right;		pV[2].tv = texRect.top;
		pV[3].tu = texRect.right;		pV[3].tv = texRect.bottom;

		CalculateTextureRect(texRect,m_curWindPic,m_WindGreed.x,m_WindGreed.y);
		pV[4].tu = texRect.left;		pV[4].tv = texRect.top;
		pV[5].tu = texRect.left;		pV[5].tv = texRect.bottom;
		pV[6].tu = texRect.right;		pV[6].tv = texRect.top;
		pV[7].tu = texRect.right;		pV[7].tv = texRect.bottom;

		CalculateTextureRect(texRect,m_curSailState,m_SailGreed.x,m_SailGreed.y);
		pV[8].tu = texRect.left;		pV[8].tv = texRect.top;
		pV[9].tu = texRect.left;		pV[9].tv = texRect.bottom;
		pV[10].tu = texRect.right;		pV[10].tv = texRect.top;
		pV[11].tu = texRect.right;		pV[11].tv = texRect.bottom;

		rs->UnLockVertexBuffer(m_idCurChargeVBuf);
	}
}

void BATTLE_NAVIGATOR::UpdateWindParam()
{
	if( !m_wb && !m_pAWeather ) {
		ENTITY_ID ei;
		if( api->FindClass(&ei,"weather",0) )
			m_wb = (WEATHER_BASE*)api->GetEntityPointer(&ei);
		if( api->IsNetActive() && !m_wb && m_pOwnerEI ) {
			VDATA * pSVWeather = (VDATA*)api->GetScriptVariable((m_pOwnerEI->IsServer()) ? "NSWeather" : "NCWeather"); Assert(pSVWeather);
			m_pAWeather = pSVWeather->GetAClass(); Assert(m_pAWeather);
		}
	}

	if( m_wb ) {
		m_fWindAngle = m_wb->GetFloat(whf_wind_angle);
		m_fWindStrength = m_wb->GetFloat(whf_wind_speed);
	}
	if( m_pAWeather ) {
		m_fWindStrength = m_pAWeather->GetAttributeAsFloat("WindSpeed");
		m_fWindAngle = m_pAWeather->GetAttributeAsFloat("WindAngle");
	}
}