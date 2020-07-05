#ifndef _SPYGLASS_H_
#define _SPYGLASS_H_

#include "..\..\common_h\dx8render.h"
#include "..\..\common_h\defines.h"
#include "..\..\common_h\templates\string.h"
#include "..\..\common_h\templates\array.h"

class BIImageRender;
class BIImage;
class VAI_OBJBASE;

class ISPYGLASS : public ENTITY
{
	struct ImageParam
	{
		BIImage* pImage;
		string sTextureName;
		dword dwColor;
		FRECT rUV;
		RECT rPos;

		ImageParam() {pImage=0;};
		~ImageParam() {Release();}
		void Release();
		void LoadFromAttr(BIImageRender* pImgRender, ATTRIBUTES* pA,const char* pcDefName,long nDefLeftPos,long nDefTopPos,long nDefRightPos,long nDefBottomPos,long nPrior);
		void ChangeIcon(BIImageRender* pImgRender, const char* pcTextureName, FRECT& frUV);
	};

	struct TextParam
	{
		VDX8RENDER* rs;
		long nFontID;
		POINT pos;
		float fScale;
		dword dwColor;
		string sText;
		long nAlign;

		TextParam() {rs=0; nFontID=-1;}
		~TextParam() {Release();}
		void Release() {if(rs && nFontID>=0) rs->UnloadFont(nFontID); nFontID=-1;}
		void LoadFromAttr(VDX8RENDER* rs,ATTRIBUTES* pA,const char* pcDefText,long nDefXPos,long nDefYPos);
		void Print();
	};

	struct SpyGlassCameraParameters
	{
		float fSpyGlassPerspective;
		float fActivateTime;
		float fUpdateTime;

		float fOldPerspective;

		bool bIsActive;
		bool bIsGrow;

		float fCurUpdatingTime;
		float fCurActivateTime;
	};

public:
	ISPYGLASS();
	~ISPYGLASS();
	bool Init();
	void Execute(dword delta_time);
	void Realize(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE & message);

protected:
	VDX8RENDER *rs;
	BIImageRender* m_pImgRender;

	// telescope lens
	ImageParam m_Lens;

	// ship sign
	ImageParam m_ShipImage; // ship back
	ImageParam m_ShipBack; // ship back
	ImageParam m_ShipHP; // ship HP
	ImageParam m_ShipSP; // ship SP
	ImageParam m_Class; // ship class

	// captain sign
	ImageParam m_CaptainBack; // captain back image
	ImageParam m_CaptainFace; // captain face image
	TextParam m_TextCaptainName; // captain name
	// captain skills
	ImageParam m_ImgCaptainFencing;
	TextParam m_TextCaptainFencing;
	ImageParam m_ImgCaptainCannon;
	TextParam m_TextCaptainCannon;
	ImageParam m_ImgCaptainAccuracy;
	TextParam m_TextCaptainAccuracy;
	ImageParam m_ImgCaptainNavigation;
	TextParam m_TextCaptainNavigation;
	ImageParam m_ImgCaptainBoarding;
	TextParam m_TextCaptainBoarding;

	// nation flag
	ImageParam m_Nation;
	// cannon icon
	ImageParam m_Cannon;
	// sail icon (ship speed)
	ImageParam m_Sail;
	// cannon charge icon
	ImageParam m_Charge;
	
	// BOAL distance charge icon
	ImageParam m_SailTo;

	TextParam m_txtShipType;
	TextParam m_txtShipName;
	TextParam m_txtCannons;
	TextParam m_txtShipSpeed;
	TextParam m_txtShipCrew;
	TextParam m_txtSailTo;   // boal

	bool m_bIsOn;
	bool m_bIsPresentShipInfo;
	long m_nInfoCharacterIndex;

	SpyGlassCameraParameters m_Camera;
	array<FRECT> m_aNationUV;
	array<FRECT> m_aChargeUV;
	array<FRECT> m_aSailUV;

	VAI_OBJBASE* m_pFortObj;

	float m_fInfoKeepDelay;
	float m_fMaxInfoKeepDelay;

protected:
	void Release();
	ATTRIBUTES* GetAttr(const char* pcAttrName);
	void TurnOnTelescope(bool bTurnOn);
	void SetShipInfo(long nCharIndex);
	void FindNewTargetShip();
	void TurnOnSpyGlassCamera();
	void ChangeTelescopeType(const char* pcTextureName, float fZoomScale, float fActivateTime, float fUpdateTime);
	void UpdateCamera();
	void ChangeTargetData(const char* pcShipName, const char* pcShipType, float fRelativeHP,float fRelativeSP,
		long nShipCrew, float fShipSpeed, float fSailTo, long nCurCannons,long nMaxCannons,
		long nCharge, long nNation, long nSailState, long nFace,
		long nFencing, long nCannon, long nAccuracy, long nNavigation, long nBoarding,
		const char* pcCaptainName,const char* pcFaceTexture,long nShipClass);
	void FillUVArrayFromAttributes( array<FRECT> & m_aUV, ATTRIBUTES* pA );
	VAI_OBJBASE* GetFort();
};

#endif
