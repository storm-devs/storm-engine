#ifndef _BATTLE_SIGN_H
#define _BATTLE_SIGN_H

#include "defines.h"
#include "..\common_h\templates\string.h"

#define MAX_SIGN_QUANTITY	8

class BISignIcon
{
public:
	enum CommandType
	{
		Command_confirm,
		Command_left,
		Command_right,
		Command_up,
		Command_down,
		Command_cancel,

		Command_ForceDword = 0xffffffff
	};

	BISignIcon( ENTITY_ID& BIEntityID, VDX8RENDER* pRS );
	virtual ~BISignIcon();

	virtual void Draw();
	virtual void Init( ATTRIBUTES* pRoot, ATTRIBUTES* pA );

	void Recollect();
	void SetUpdate() {m_bMakeUpdate=true;}
	bool IsActive() {return m_bActive;}
	void SetActive(bool bActive);

	void MakeControl();
	virtual void ExecuteCommand( CommandType command ) = 0;

	long GetLineY(long n) {return (long)m_Sign[n].pntPos.x;}

protected:
	virtual long CalculateSignQuantity();
	virtual void UpdateChildrens() = 0;

	void Release();
	void UpdateBuffers( long nQ );
	void FillIndexBuffer();
	void FillVertexBuffer();
	long WriteSquareToVBuff( BI_COLOR_VERTEX* pv, FRECT& uv, dword color, BIFPOINT& center, FPOINT& size );
	long WriteSquareToVBuffWithProgress( BI_COLOR_VERTEX* pv, FRECT& uv, dword color, BIFPOINT& center, FPOINT& size, float fClampUp, float fClampDown, float fClampLeft, float fClampRight );

	VDX8RENDER* m_pRS;

	long m_nVBufID;
	long m_nIBufID;

	long m_nBackTextureID;
	long m_nBackSquareQ;
	dword m_dwBackColor;
	FRECT m_rBackUV;
	BIFPOINT m_pntBackOffset;
	FPOINT m_pntBackIconSize;

	long m_nSignStateTextureID;
	long m_nSignStateSquareQ;
	dword m_dwSignStateColor;
	FRECT m_rSignStateLeftUV;
	BIFPOINT m_pntSignStateLeftOffset;
	FPOINT m_pntSignStateLeftIconSize;
	FRECT m_rSignStateRightUV;
	BIFPOINT m_pntSignStateRightOffset;
	FPOINT m_pntSignStateRightIconSize;

	long m_nSignStarTextureID;
	long m_nSignStarSquareQ;
	dword m_dwSignStarColor;
	FRECT m_rSignStarUV;
	BIFPOINT m_pntSignStarOffset;
	FPOINT m_pntSignStarIconSize;

	long m_nSignFaceTextureID;
	long m_nSignFaceSquareQ;
	dword m_dwSignFaceColor;
	BIFPOINT m_pntSignFaceOffset;
	FPOINT m_pntSignFaceIconSize;

	long m_idSignTextFont;
	dword m_dwSignTextFontColor;
	float m_fSignTextFontScale;
	POINT m_SignTextFontOffset;

	bool m_bMakeUpdate;
	bool m_bActive;

	ATTRIBUTES* m_pARoot;
	ATTRIBUTES* m_pAData;
	ENTITY_ID m_idHostEntity;

	struct SignDescr
	{
		bool bUse;
		FPOINT pntPos; // center
		float fLeftState;
		float fRightState;
		float fStarProgress;
		FRECT rFaceUV;
		string sText;
	} m_Sign[MAX_SIGN_QUANTITY];
	long m_nMaxSquareQ;
	long m_nSignQ;
};

#endif
