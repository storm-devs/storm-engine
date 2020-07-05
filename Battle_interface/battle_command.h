#ifndef _BI_COMMANDLIST_H_
#define _BI_COMMANDLIST_H_

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

class BICommandList
{
public:
	BICommandList( ENTITY_ID& eid, ATTRIBUTES* pA, VDX8RENDER* rs );
	virtual ~BICommandList();

	void Draw();
	void Update( long nTopLine, long nCharacterIndex, long nCommandMode );
	virtual void FillIcons() = 0;

	long AddTexture( const char* pcTextureName, long nCols, long nRows );

	// commands
	long ExecuteConfirm();
	long ExecuteLeft();
	long ExecuteRight();
	long ExecuteCancel();

	void SetActive(bool bActive);
	bool GetActive() {return m_bActive;}

	void SetUpDown(bool bUp,bool bDown);

	virtual void Init();

	long AddToIconList( long nTextureNum, long nNormPictureNum, long nSelPictureNum, long nCooldownPictureNum,
		long nCharacterIndex, const char* pcCommandName, long nTargetIndex, const char* pcLocName,
		const char* pcNoteName );
	void AddAdditiveToIconList( long nTextureNum, long nPictureNum, float fDist, float fWidth, float fHeight );

protected:
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
		long nCooldownPictureIndex;

		float fCooldownFactor;

		struct AdditiveIcon {
			long nTex;
			long nPic;
			float fDelta;
			FPOINT fpSize;
		};
		array<AdditiveIcon> aAddPicList;

		UsedCommand() : aAddPicList(_FL_) {};
	};

	bool m_bActive;

	array<UsedCommand> m_aUsedCommand;
	long m_nStartUsedCommandIndex;
	long m_nSelectedCommandIndex;
	long m_nIconShowMaxQuantity;
	bool m_bLeftArrow;
	bool m_bRightArrow;

	POINT m_pntActiveIconOffset;
	POINT m_pntActiveIconSize;
	string m_sActiveIconTexture;
	FRECT m_frActiveIconUV1;
	FRECT m_frActiveIconUV2;
	string m_sActiveIconNote;

	bool m_bUpArrow;
	bool m_bDownArrow;
	string m_sUpDownArrowTexture;
	FRECT m_frUpArrowUV;
	FRECT m_frDownArrowUV;
	POINT m_pntUpDownArrowSize;
	POINT m_pntUpArrowOffset;
	POINT m_pntDownArrowOffset;

	string	m_sCurrentCommandName;
	long	m_nCurrentCommandCharacterIndex;
	long	m_nCurrentCommandMode;

	POINT m_LeftTopPoint;
	POINT m_IconSize;
	long m_nIconSpace;

	long m_NoteFontID;
	dword m_NoteFontColor;
	float m_NoteFontScale;
	POINT m_NotePos;
	POINT m_NoteOffset;
	string m_NoteText;

	struct CoolDownUpdateData
	{
		long nIconNum;
		float fTime;
		float fUpdateTime;
	};
	array<CoolDownUpdateData> m_aCooldownUpdate;

	void Release();

	long IconAdd( long nPictureNum, long nTextureNum, RECT& rpos );
	long ClockIconAdd( long nForePictureNum, long nBackPictureNum, long nTextureNum, RECT& rpos, float fFactor );
	void AdditiveIconAdd( float fX, float fY, array<UsedCommand::AdditiveIcon>& aList );
	FRECT& GetPictureUV( long nTextureNum, long nPictureNum, FRECT& uv );
	RECT& GetCurrentPos(long num,RECT& rpos);
	RECT& GetAddingPos(long num,RECT& rpos);

	void UpdateShowIcon();
	void SetNote( const char* pcNote, long nX, long nY );

	ATTRIBUTES* GetCurrentCommandAttribute();
};

#endif
