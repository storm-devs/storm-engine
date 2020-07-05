#ifndef _DIALOG_H_
#define _DIALOG_H_

#include "..\common_h\matrix.h"
#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"

#include "..\common_h\templates\string.h"
#include "..\common_h\templates\array.h"

#define MAX_LINES				 5
#define SCROLL_LINE_TIME		 100
#define TILED_LINE_HEIGHT		 26
#define SBL 6
#define TICK_SOUND				 "interface\\ok.wav"

#define XI_TEX_FVF	(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2)
struct XI_TEX_VERTEX
{
	CVECTOR pos;
	float rhw;
	DWORD color;
	float u,v;
};

#define BUTTON_STATE_UPENABLE	1
#define BUTTON_STATE_DOWNENABLE	2
#define BUTTON_STATE_UPLIGHT	4
#define BUTTON_STATE_DOWNLIGHT	8

class VSoundService;

class DIALOG : public ENTITY
{
	static VDX8RENDER * RenderService;
public:
	 DIALOG();
	~DIALOG();
	
	bool Init();
	void Realize(dword Delta_Time);
	dword AttributeChanged(ATTRIBUTES * pA);
	dword _cdecl ProcessMessage(MESSAGE & message);

	static void AddToStringArrayLimitedByWidth(const char* pcSrcText, long nFontID,float fScale,long nLimitWidth, array<string> & asOutTextList, array<long>* panPageIndices, long nPageSize);

private:
	void EmergencyExit();

	// Nikita data
	string m_sTalkPersName;

	struct TextDescribe
	{
		VDX8RENDER* rs;
		POINT offset;
		long nWindowWidth;
		long nFontID;
		dword dwColor;
		dword dwSelColor;
		float fScale;
		long nLineInterval;
		array<string> asText;
		long nStartIndex;
		long nShowQuantity;
		long nSelectLine;

		TextDescribe() : asText(_FL) {rs=0; nFontID=-1;}
		virtual ~TextDescribe() {Release();}
		virtual void Release() {if( rs && nFontID>=0 ) rs->UnloadFont(nFontID); nFontID = -1; asText.DelAll();}
	};

	bool m_bDlgChanged;
	void UpdateDlgTexts();
	void UpdateDlgViewport();

	struct DlgTextDescribe : public TextDescribe
	{
		float fScrollTime;
		array<long> anPageEndIndex;

		DlgTextDescribe() : TextDescribe(),anPageEndIndex(_FL) {}
		virtual ~DlgTextDescribe() {Release();}
		virtual void Release() {TextDescribe::Release();}
		void __declspec(dllexport) __cdecl ChangeText(const char* pcText);
		void __declspec(dllexport) __cdecl Init(VDX8RENDER* pRS, D3DVIEWPORT8& vp, INIFILE* pIni);
		long GetShowHeight();
		void Show(long nY);
		bool IsLastPage();
		void PrevPage();
		void NextPage();
	};
	DlgTextDescribe m_DlgText;

	struct DlgLinkDescribe : public TextDescribe
	{
		array<long> anLineEndIndex;

		long nEditLine;
		long nEditVarIndex;
		long nEditCharIndex;
		float fCursorCurrentTime,fCursorVisibleTime, fCursorInvisibleTime;
		DIALOG* pDlg;

		DlgLinkDescribe() : TextDescribe(),anLineEndIndex(_FL) {pDlg=0;}
		virtual ~DlgLinkDescribe() {Release();}
		virtual void Release() {TextDescribe::Release();}
		void __declspec(dllexport) __cdecl ChangeText(ATTRIBUTES* pALinks);
		void __declspec(dllexport) __cdecl Init(VDX8RENDER* pRS, D3DVIEWPORT8& vp, INIFILE* pIni);
		long GetShowHeight();
		void Show(long nY);
		void __declspec(dllexport) __cdecl ShowEditMode(long nX, long nY, long nTextIdx);
		void SetDlg(DIALOG* _pDlg) {pDlg=_pDlg;}
	};
	DlgLinkDescribe m_DlgLinks;

	struct BackParameters
	{
		long m_idBackTex;

		FRECT m_frLeftTopUV;
		FRECT m_frRightTopUV;
		FRECT m_frLeftBottomUV;
		FRECT m_frRightBottomUV;
		FRECT m_frLeftUV;
		FRECT m_frRightUV;
		FRECT m_frTopUV;
		FRECT m_frBottomUV;
		FRECT m_frCenterUV;
		FRECT m_frDividerUV;

		FRECT m_frBorderExt;
		FRECT m_frBorderInt;
		FRECT frBorderRect;

		FRECT frCharacterNameRectLeftUV;
		FRECT frCharacterNameRectRightUV;
		FRECT frCharacterNameRectCenterUV;
		FPOINT fpCharacterNameOffset;
		float fCharacterNameRectHeight;
		float fCharacterNameRectLeftWidth;
		float fCharacterNameRectCenterWidth;
		float fCharacterNameRectRightWidth;

		bool bShowDivider;
		float nDividerHeight;
		float nDividerOffsetX;
		long nDividerOffsetY;
	};
	BackParameters m_BackParams;
	long m_idVBufBack;
	long m_idIBufBack;
	long m_nVQntBack;
	long m_nIQntBack;

	struct ButtonParameters
	{
		long m_idTexture;

		FRECT frUpNormalButtonUV;
		FRECT frDownNormalButtonUV;
		FRECT frUpLightButtonUV;
		FRECT frDownLightButtonUV;

		FPOINT fpButtonSize;

		float fRightOffset;
		float fTopOffset;
		float fBottomOffset;
	};
	ButtonParameters m_ButtonParams;
	long m_idVBufButton;
	long m_idIBufButton;
	long m_nVQntButton;
	long m_nIQntButton;
	dword m_dwButtonState;

	long m_nCharNameTextFont;
	dword m_dwCharNameTextColor;
	float m_fCharNameTextScale;
	FPOINT m_fpCharNameTextOffset;

	long m_nScrBaseWidth;
	long m_nScrBaseHeight;
	static FRECT m_frScreenData;

	static float GetScrX( float fX ) {return fX*m_frScreenData.right + m_frScreenData.left;}
	static float GetScrY( float fY ) {return fY*m_frScreenData.bottom + m_frScreenData.top;}
	static float GetScrWidth( float fX ) {return fX*m_frScreenData.right;}
	static float GetScrHeight( float fY ) {return fY*m_frScreenData.bottom;}

	void __declspec(dllexport) __cdecl CreateBack();
	void FillBack();
	void FillDivider();
	void DrawBack();

	void CreateButtons();
	void FillButtons();
	void DrawButtons();

	void LoadFromIni();

	static void GetRectFromIni( INIFILE* ini, const char* pcSection, const char* pcKey, FRECT& frect );
	static void GetPointFromIni( INIFILE* ini, const char* pcSection, const char* pcKey, FPOINT& fpoint );

	VSoundService *snd;
	ENTITY_ID charId, persId;
	ENTITY_ID charMdl, persMdl;
	D3DVIEWPORT8 textViewport;

	long curSnd;
	char soundName[256];
	char charDefSnd[256];

	bool forceEmergencyClose;
	char selectedLinkName[1024];

	int  unfadeTime;

	long play;
	bool start;

	bool bEditMode;
};

#endif