#ifndef _BATTLE_MANSIGN_H
#define _BATTLE_MANSIGN_H

#include "..\defines.h"
#include "..\..\common_h\templates\string.h"
#include "..\..\common_h\templates\array.h"

#define MAX_MAN_QUANTITY	4

class BIManCommandList;

class BIManSign
{
public:
	BIManSign( ENTITY_ID& BIEntityID, VDX8RENDER* pRS );
	~BIManSign();

	void Draw();
	void Init( ATTRIBUTES* pRoot, ATTRIBUTES* pA );

	long AddTexture( const char* pcTextureName, long nCols, long nRows );

	void Recollect();
	void SetUpdate() {m_bMakeUpdate=true;}
	bool IsActive();
	void SetActive(bool bActive);
	void MakeControl();
	void ExecuteCommand( long command );

protected:
	void Release();
	long CalculateManQuantity();
	void UpdateBuffers( long nShipQ );
	void FillIndexBuffer();
	void FillVertexBuffer();
	long WriteSquareToVBuff( BI_COLOR_VERTEX* pv, FRECT& uv, dword color, BIFPOINT& center, FPOINT& size );
	long WriteSquareToVBuffWithProgress( BI_COLOR_VERTEX* pv, FRECT& uv, dword color, BIFPOINT& center, FPOINT& size, float fClampUp, float fClampDown, float fClampLeft, float fClampRight );
	void UpdateCommandList();

	long GetCurrentCommandTopLine();
	long GetCurrentCommandCharacterIndex();
	long GetCurrentCommandMode();

	float GetProgressManHP(long nIdx);
	float GetProgressManEnergy(long nIdx);
	float GetProgressGunChargeMax(long nIdx);
	float GetProgressGunCharge(long nIdx);

	float GetGunProgressByIndex(long nIdx);

	void CheckDataChange();

	__forceinline bool LongACompare(ATTRIBUTES* pA, const char* attrName, long & nCompareVal);
	__forceinline bool FloatACompare(ATTRIBUTES* pA, const char* attrName, float & fCompareVal);
	__forceinline bool StringACompare(ATTRIBUTES* pA, const char* attrName, string & sCompareVal);
	__forceinline bool FRectACompare(ATTRIBUTES* pA, const char* attrName, FRECT & rCompareVal);
	__forceinline bool BoolACompare(ATTRIBUTES* pA, const char* attrName, bool & bCompareVal);
	__forceinline dword GetColorByFactor(dword dwLowColor,dword dwHighColor, float fFactor);

	VDX8RENDER* m_pRS;
	ATTRIBUTES* m_pARoot;
	BIManCommandList* m_pCommandList;
	ENTITY_ID m_idHostEntity;
	long m_nCommandMode;

	long m_nVBufID;
	long m_nIBufID;
	long m_nSquareQ;

	long m_nMaxSquareQ;

	long m_nBackTextureID;
	long m_nBackSquareQ;
	dword m_dwBackColor;
	FRECT m_rBackUV;
	BIFPOINT m_pntBackOffset;
	FPOINT m_pntBackIconSize;

	bool m_bIsAlarmOn;
	long m_nAlarmSquareQ;
	long m_nAlarmTextureID;
	dword m_dwAlarmHighColor;
	dword m_dwAlarmLowColor;
	FRECT m_rAlarmUV;
	BIFPOINT m_pntAlarmOffset;
	FPOINT m_pntAlarmIconSize;
	bool m_bAlarmUpDirection;
	float m_fAlarmTime;
	float m_fAlarmUpSpeed;
	float m_fAlarmDownSpeed;

	long m_nManStateTextureID;
	long m_nManStateSquareQ;
	dword m_dwManStateColor;
	FRECT m_rManHPUV;
	BIFPOINT m_pntManHPOffset;
	FPOINT m_pntManHPIconSize;
	FRECT m_rManEnergyUV;
	BIFPOINT m_pntManEnergyOffset;
	FPOINT m_pntManEnergyIconSize;

	long m_nGunChargeTextureID;
	long m_nGunChargeSquareQ;
	dword m_dwGunChargeColor;
	dword m_dwGunChargeBackColor;
	FRECT m_rGunChargeUV;
	BIFPOINT m_pntGunChargeOffset;
	FPOINT m_pntGunChargeIconSize;

	array<float> m_aChargeProgress;

	FRECT m_rManPicUV;
	BIFPOINT m_pntManPicOffset;
	FPOINT m_pntManPicIconSize;
	DWORD m_dwManFaceColor;

	struct ManDescr
	{
		FPOINT pntPos; // center
		string sTexture;
		long nTexture;
		FRECT rUV;

		long nSlotIndex;
		long nCharacterIndex;
		float fHealth;
		float fEnergy;
		bool bAlarm;
		long nShootMax;
		long nShootCurrent;
	} m_Man[MAX_MAN_QUANTITY];
	long m_nManQuantity;
	long m_nCurrentManIndex;
	long m_nCommandListVerticalOffset;

	bool m_bMakeUpdate;
	bool m_bMakeVertexFill;
	bool m_bActive;
};

__forceinline bool BIManSign::LongACompare(ATTRIBUTES* pA, const char* attrName, long & nCompareVal)
{
	long tmp = nCompareVal;
	nCompareVal = pA->GetAttributeAsDword(attrName);
	return (nCompareVal != tmp);
}

__forceinline bool BIManSign::FloatACompare(ATTRIBUTES* pA, const char* attrName, float & fCompareVal)
{
	float tmp = fCompareVal;
	fCompareVal = pA->GetAttributeAsFloat(attrName);
	return (fCompareVal != tmp);
}

__forceinline bool BIManSign::StringACompare(ATTRIBUTES* pA, const char* attrName, string & sCompareVal)
{
	char* pVal = pA->GetAttribute(attrName);
	if( sCompareVal == pVal ) return false;
	sCompareVal = pVal;
	return true;
}

__forceinline bool BIManSign::FRectACompare(ATTRIBUTES* pA, const char* attrName, FRECT & rCompareVal)
{
	char* pVal = pA->GetAttribute(attrName);
	if( !pVal ) return false;
	FRECT rTmp;
	rTmp.left = rCompareVal.left;	rTmp.top = rCompareVal.top;
	rTmp.right = rCompareVal.right;	rTmp.bottom = rCompareVal.bottom;
	sscanf( pVal, "%f,%f,%f,%f", &rCompareVal.left, &rCompareVal.top, &rCompareVal.right, &rCompareVal.bottom );
	if( rCompareVal.left == rTmp.left &&
		rCompareVal.top == rTmp.top &&
		rCompareVal.right == rTmp.right &&
		rCompareVal.bottom == rTmp.bottom ) return false;
	return true;
}

__forceinline bool BIManSign::BoolACompare(ATTRIBUTES* pA, const char* attrName, bool & bCompareVal)
{
	bool tmp = bCompareVal;
	bCompareVal = pA->GetAttributeAsDword(attrName,bCompareVal?1:0)!=0;
	return (bCompareVal != tmp);
}

__forceinline dword BIManSign::GetColorByFactor(dword dwLowColor,dword dwHighColor, float fFactor)
{
	long asrc = (dwLowColor>>24) & 0xFF;
	long rsrc = (dwLowColor>>16) & 0xFF;
	long gsrc = (dwLowColor>>8) & 0xFF;
	long bsrc = dwLowColor & 0xFF;
	//
	long adst = (dwHighColor>>24) & 0xFF;
	long rdst = (dwHighColor>>16) & 0xFF;
	long gdst = (dwHighColor>>8) & 0xFF;
	long bdst = dwHighColor & 0xFF;
	//
	asrc += ((long)((adst-asrc)*fFactor)) & 0xFF;
	rsrc += ((long)((rdst-rsrc)*fFactor)) & 0xFF;
	gsrc += ((long)((gdst-gsrc)*fFactor)) & 0xFF;
	bsrc += ((long)((bdst-bsrc)*fFactor)) & 0xFF;
	//
	return ARGB(asrc,rsrc,gsrc,bsrc);
}

#endif
