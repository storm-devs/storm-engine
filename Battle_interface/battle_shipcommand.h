#ifndef _BATTLE_SHIPCOMMAND_H_
#define _BATTLE_SHIPCOMMAND_H_

#include "defines.h"
#include "..\common_h\templates\string.h"
#include "..\common_h\templates\array.h"

class BIImageRender;

// nCommandType
#define BISCT_Command	1
#define BISCT_Ability	2
#define BISCT_Ship		3
#define BISCT_Fort		4
#define BISCT_Land		5
#define BISCT_Charge	6
#define BISCT_UserIcon	7
#define BISCT_Cancel	8

#define BISHIPCOMMANDMODE_

class BIShipCommandList
{
public:
	BIShipCommandList( ENTITY_ID& eid, ATTRIBUTES* pA, VDX8RENDER* rs );
	~BIShipCommandList();

	void Draw();
	void Update( long nTopLine, long nCharacterIndex, long nCommandMode );

	long AddTexture( const char* pcTextureName, long nCols, long nRows );

	// commands
	long ExecuteConfirm();
	long ExectuteLeft();
	long ExectuteRight();
	long ExecuteCancel();

protected:
	void Release();
	void Init();
	long FindTextureIndexByName( const char* pcTextureFieName );
	void GetUVForPictureIntoTexture( long nPicIndex, long nTextureIndex, FRECT& rUV );

	long ShipAdding( bool allLabel, bool bMyShip, bool bEnemy, bool bNeutral, bool bFriend );
	long FortAdding(bool allLabel, bool bFriend, bool bNeutral, bool bEnemy );
	long LandAdding(bool allLabel);
	long CommandAdding();
	long ChargeAdding();
	long UserIconsAdding();
	long AbilityAdding();
	long AddCancelIcon();

	long AddToIconList( long nTextureNum, long nNormPictureNum, long nSelPictureNum,
		long nCharacterIndex, const char* pcCommandName, long nTargetIndex, const char* pcLocName,
		const char* pcNoteName );

	long IconAdd( long nPictureNum, long nTextureNum );
	FRECT& GetPictureUV( long nTextureNum, long nPictureNum, FRECT& uv );
	RECT& GetCurrentPos(RECT& rpos);

	void UpdateShowIcon();
	void SetNote( const char* pcNote, long nX, long nY );

	ENTITY_ID m_idHostObj;
	ATTRIBUTES* m_pARoot;
	VDX8RENDER* m_pRS;

	BIImageRender* m_pImgRender;

	struct TextureDescr
	{
		string sFileName;
		long nCols;
		long nRows;
	};
	array<TextureDescr> m_aTexture;
	long m_nChargeTextureNum;
	long m_nCommandTextureNum;

	struct CommandDescr
	{
		long nCommandType;
		string sCommandName;
		string sTextureName;
		long nTexture;

		long nPicIndex;
		FRECT rUV;

		string sNote;
		string sEvent;
	};
	array<CommandDescr> m_aCommand;

	struct UsedCommand
	{
		long nCharIndex;
		string sCommandName;
		long nTargetIndex;
		string sLocName;
		string sNote;

		long nTextureIndex;
		long nSelPictureIndex;
		long nNormPictureIndex;
	};
	array<UsedCommand> m_aUsedCommand;
	long m_nStartUsedCommandIndex;
	long m_nSelectedCommandIndex;
	long m_nIconShowMaxQuantity;
	bool m_bLeftArrow;
	bool m_bRightArrow;

	string	m_sCurrentCommandName;
	long	m_nCurrentCommandCharacterIndex;
	long	m_nCurrentCommandMode;

	POINT m_LeftTopPoint;
	POINT m_IconSize;
	long m_nIconSpace;

	array<long> m_aChargeQuantity;

	long m_NoteFontID;
	dword m_NoteFontColor;
	float m_NoteFontScale;
	POINT m_NotePos;
	POINT m_NoteOffset;
	string m_NoteText;
};

#endif
