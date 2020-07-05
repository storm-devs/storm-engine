#ifndef _BATTLE_SHIPSIGN_H
#define _BATTLE_SHIPSIGN_H

#include "defines.h"
#include "ships_list.h"

#define MAX_SHIP_QUANTITY	4

class BIShipCommandList;

class BIShipIcon
{
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

public:
	BIShipIcon( ENTITY_ID& BIEntityID, VDX8RENDER* pRS );
	~BIShipIcon();

	void Draw();
	void Init( ATTRIBUTES* pRoot, ATTRIBUTES* pA );

	long AddTexture( const char* pcTextureName, long nCols, long nRows );

	void Recollect();
	void SetUpdate() {m_bMakeUpdate=true;}
	bool IsActive() {return m_bActive;}
	void SetActive(bool bActive);
	void MakeControl();
	void ExecuteCommand( CommandType command );

protected:
	void Release();
	long CalculateShipQuantity();
	void UpdateBuffers( long nShipQ );
	void FillIndexBuffer();
	void FillVertexBuffer();
	long WriteSquareToVBuff( BI_COLOR_VERTEX* pv, FRECT& uv, dword color, BIFPOINT& center, FPOINT& size );
	long WriteSquareToVBuffWithProgress( BI_COLOR_VERTEX* pv, FRECT& uv, dword color, BIFPOINT& center, FPOINT& size, float fClampUp, float fClampDown, float fClampLeft, float fClampRight );
	void UpdateCommandList();

	long GetCurrentCommandTopLine();
	long GetCurrentCommandCharacterIndex();
	long GetCurrentCommandMode();

	ATTRIBUTES* GetSailorQuantityAttribute( SHIP_DESCRIBE_LIST::SHIP_DESCR * pSD );
	float GetRelativeShipHP(long nShipNum);
	float GetRelativeShipSP(long nShipNum);

	VDX8RENDER* m_pRS;

	long m_nVBufID;
	long m_nIBufID;

	long m_nMaxSquareQ;

	long m_nBackTextureID;
	long m_nBackSquareQ;
	dword m_dwBackColor;
	FRECT m_rBackUV;
	BIFPOINT m_pntBackOffset;
	FPOINT m_pntBackIconSize;

	long m_nShipStateTextureID;
	long m_nShipStateSquareQ;
	dword m_dwShipStateColor;
	FRECT m_rShipHPUV;
	BIFPOINT m_pntShipHPOffset;
	FPOINT m_pntShipHPIconSize;
	FRECT m_rShipSPUV;
	BIFPOINT m_pntShipSPOffset;
	FPOINT m_pntShipSPIconSize;

	long m_nShipClassTextureID;
	long m_nShipClassSquareQ;
	dword m_dwShipClassColor;
	FRECT m_rShipClassUV;
	BIFPOINT m_pntShipClassOffset;
	FPOINT m_pntShipClassIconSize;

	long m_nShipTextureID;
	long m_nShipSquareQ;
	dword m_dwShipColor;
	//FRECT m_rShipUV;
	BIFPOINT m_pntShipOffset;
	FPOINT m_pntShipIconSize;

	struct ShipDescr
	{
		long nCharacterIndex;
		FPOINT pntPos; // center
		ATTRIBUTES* pASailorQuantity;
		ATTRIBUTES* pAShip;
		long nMaxHP;
		long nMaxSP;
	} m_Ship[MAX_SHIP_QUANTITY];
	long m_nShipQuantity;
	long m_nCurrentShipIndex;
	long m_nCommandListVerticalOffset;

	long m_idSailorFont;
	dword m_dwSailorFontColor;
	float m_fSailorFontScale;
	POINT m_SailorFontOffset;

	bool m_bMakeUpdate;
	bool m_bActive;

	ATTRIBUTES* m_pARoot;

	BIShipCommandList* m_pCommandList;
	ENTITY_ID m_idHostEntity;
	long m_nCommandMode;
};

#endif
