#include "Cannon.h"
#include "..\..\Server.h"

NetCannon::NetCannon()
{
	bLoad = false;
	bEmpty = true;
	bFired = false;
	fDamaged = 0.0f;
	bRecharged = true;
	bReady2Fire = false;
	fTime2Action = 0.0f;
	bCanRecharge = true;
	fTotalTime2Action = 0.0f;
}

NetCannon::~NetCannon()
{
}

void NetCannon::Execute(float fDeltaTime)
{
	if (fTime2Action>=0.0f) 
	{
		if (isRecharged() && (!bCanRecharge || isEmpty()))  
			; 
		else
			fTime2Action -= fDeltaTime;
	}

	if (fTime2Action<=0.0f && isRecharged())
	{
		bRecharged = false;
		if (!isEmpty()) bReady2Fire = true;
		return;
	}

	if (fTime2Action<=0.0f && isFired() && !isDamaged() && !isEmpty())
	{
		RealFire();
		return;
	}
}

float NetCannon::CalcHeightFireAngle(float _fSpeedV0, CVECTOR & vOur, CVECTOR & vEnemy)
{
	CVECTOR vTemp = vOur - vEnemy; vTemp.y = 0.0f;
	float fDistance = sqrtf(~vTemp);
	float g = NetServer::fGravity;
	float a, b, c, d;
	float fHeight = (vEnemy.y - vOur.y);
	a = SQR(g) / 4.0f;
	b = (fHeight * g - SQR(_fSpeedV0));
	c = SQR(fHeight) + SQR(fDistance);
	d = b*b - 4 * a * c;
	if (d<0) return 0.0f;

	float x1, x2, t1, t2;

	x1 = (-b + sqrtf(d)) / (2 * a);
	x2 = (-b - sqrtf(d)) / (2 * a);

	t1 = sqrtf(x1);
	t2 = sqrtf(x2);

	float fTemp = fDistance / (_fSpeedV0 * t2);
	float fAlpha = acosf(Clamp(fTemp));

	float sy0 = _fSpeedV0 * t2 * sinf(fAlpha) - g * SQR(t2) / 2.0f;
	float sy1 = _fSpeedV0 * t2 * sinf(-fAlpha) - g * SQR(t2) / 2.0f;
	return (fabsf(sy0 - fHeight) < fabsf(sy1 - fHeight)) ? fAlpha : -fAlpha;
}

float NetCannon::GetDirY()
{
	CMatrix mRot;
	VAI_OBJBASE * pAIObj = GetAIObjPointer();

	(*pAIObj->GetMatrix()).Get3X3(mRot);

	CVECTOR vDirTemp = mRot * vDir;
	return NormalizeAngle(atan2f(vDirTemp.x, vDirTemp.z));
}

CVECTOR	NetCannon::GetDir()
{
	CMatrix mRot;
	VAI_OBJBASE * pAIObj = GetAIObjPointer();
	(*pAIObj->GetMatrix()).Get3X3(mRot);
	return mRot * vDir;
}

CVECTOR	NetCannon::GetPos()
{
	return *GetAIObjPointer()->GetMatrix() * vPos;
}

void NetCannon::RealFire()
{
	CMatrix mRot;
	// calculate real world cannon position and direction,
	// calculate fire height and azimuth angle, and call script
	VAI_OBJBASE * pAIObj = GetAIObjPointer();
	CVECTOR vPosTemp = *pAIObj->GetMatrix() * vPos;
	(*pAIObj->GetMatrix()).Get3X3(mRot);

	CVECTOR vEnemyDir = !(vEnemyPos - vPosTemp);
	float fFireDirection = NormalizeAngle(atan2f(vEnemyDir.x, vEnemyDir.z));
	float fFireHeightAngle = CalcHeightFireAngle(fSpeedV0, vPosTemp, vEnemyPos);

	CVECTOR vDirTemp = mRot * vDir;
	float fDirY = NormalizeAngle(atan2f(vDirTemp.x, vDirTemp.z));
	api->Event("NetServer_OnCannonFire", "afffffff", GetAIObjPointer()->GetACharacter(), vPosTemp.x, vPosTemp.y, vPosTemp.z, fSpeedV0, fFireDirection, fFireHeightAngle, fDirY);

	pAIObj->Fire(*pAIObj->GetMatrix() * (vPos + vDir));

	bFired = false;
	bEmpty = true;
	Recharge();
}

bool NetCannon::Fire(float _fSpeedV0, CVECTOR & _vFirePos)
{
	if (isFired() || isRecharged() || isDamaged() || isEmpty()) return false;

	VAI_OBJBASE * pAIObj = GetAIObjPointer();
	CVECTOR vPosTemp = *pAIObj->GetMatrix() * vPos;
	CMatrix mRot;
	(*pAIObj->GetMatrix()).Get3X3(mRot);

	CVECTOR vFireDir = !(_vFirePos - vPosTemp);
	CVECTOR vDirTemp = mRot * vDir;
	float fDot = (vFireDir | vDirTemp);
	if (fDot < 0.3f) return false;

	bFired = true;
	bReady2Fire = false;

	vEnemyPos = _vFirePos;
	fSpeedV0 = _fSpeedV0;

	ATTRIBUTES * pAFireTime = GetACharacter()->FindAClass(GetACharacter(), "Ship.Cannons.FireTime"); Assert(pAFireTime);
	fTotalTime2Action = fTime2Action = FRAND(pAFireTime->GetAttributeAsFloat());
	// calculate in script timeout before real fire
	//VDATA * pVData = api->Event(CANNON_GET_FIRE_TIME, "a", pAHolder->GetACharacter()); Assert(pVData);
	//fTotalTime2Action = fTime2Action = pVData->GetFloat();
	return true;
}

void NetCannon::Unload()
{
	// call script
	if (!isDamaged() && !isEmpty())
	{
		ATTRIBUTES * pACharge = GetACharacter()->FindAClass(GetACharacter(), "Ship.Cannons.Charge"); Assert(pACharge);
		ATTRIBUTES * pAGoods = GetACharacter()->FindAClass(GetACharacter(), "Ship.Goods"); Assert(pAGoods);
		ATTRIBUTES * pAGood = pAGoods->GetAttributeClass(pACharge->GetAttributeAsDword("Type")); Assert(pAGood);
		pAGood->SetAttributeUseDword(null, pAGood->GetAttributeAsDword() + 1);
		//api->Event(CANNON_UNLOAD, "a", GetACharacter());
	}
	bReady2Fire = false;
	bRecharged = false;
	bEmpty = true;
	bFired = false;
}

void NetCannon::Load()
{
	ATTRIBUTES * pACharge = GetACharacter()->FindAClass(GetACharacter(), "Ship.Cannons.Charge"); Assert(pACharge);
	ATTRIBUTES * pAGoods = GetACharacter()->FindAClass(GetACharacter(), "Ship.Goods"); Assert(pAGoods);
	ATTRIBUTES * pAGood = pAGoods->GetAttributeClass(pACharge->GetAttributeAsDword("Type")); Assert(pAGood);
	dword dwGoodNum = pAGood->GetAttributeAsDword();
	if (dwGoodNum)
	{
		pAGood->SetAttributeUseDword(null, dwGoodNum - 1);
		bEmpty = false;
	}
	else 
		bEmpty = true;

	//VDATA * pVData = api->Event(CANNON_LOAD, "a", GetACharacter()); Assert(pVData);
	//bEmpty = pVData->GetLong() == 0;
}

void NetCannon::Recharge()
{
	if (isFired() || isDamaged()) return;
	if (!isEmpty()) Unload();
	if (bCanRecharge) Load();
	bReady2Fire = false;
	bRecharged = true;

	ATTRIBUTES * pARechargeTime = GetACharacter()->FindAClass(GetACharacter(), "Ship.Cannons.ReloadTime"); Assert(pARechargeTime);
	float fMin = pARechargeTime->GetAttributeAsFloat("Min", 0.1f);
	float fMax = pARechargeTime->GetAttributeAsFloat("Max", 1.0f);
	fTotalTime2Action = fTime2Action = fMin + FRAND(fMax - fMin);
	// calculate in script recharge time, and possibility of recharge
	//VDATA * pVData = api->Event(CANNON_GET_RECHARGE_TIME, "a", pAHolder->GetACharacter()); Assert(pVData);
	//fTotalTime2Action = fTime2Action = pVData->GetFloat();
}

void NetCannon::QuickRecharge(bool bHaveEnoughBalls)
{
	Recharge();
	fTime2Action = (bHaveEnoughBalls) ? 0.0f : fTotalTime2Action;
}

float NetCannon::GetRechargePercent()
{
	return (isRecharged()) ? 1.0f - fTime2Action / fTotalTime2Action : 1.0f;
}

float NetCannon::CalcMaxFireDistance(float fFireHeight, float fSpeedV0, float fAngle)
{
	float a = -NetServer::fGravity / 2.0f;
	float b = fSpeedV0 * sinf(fAngle);
	float c = fFireHeight;
	float d = b * b - 4.0f * a * c;
	float t1 = (-b - sqrtf(d)) / (2.0f * a);
	//float t2 = (-b + sqrtf(d)) / (2.0f * a);

	float fDistance = fSpeedV0 * t1 * cosf(fAngle);
	//float s2 = fSpeedV0 * t2 * cosf(fAngle);

	return fDistance;
}

void NetCannon::Init(VAI_OBJBASE * pAIObj, GEOS::LABEL & lbl)
{
	this->pAIObj = pAIObj;

	CMatrix m;
	memcpy(&m[0][0], &lbl.m[0][0], sizeof(m));

	vPos = m.Pos();
	vDir = CVECTOR(m.Vz().x, 0.0f, m.Vz().z);
	//vDir = atan2f(m.Vz().x,m.Vz().z);
}
