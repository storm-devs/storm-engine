#ifndef _XI_BACK_SCENE_H
#define _XI_BACK_SCENE_H

#include "..\..\common_h\dx8render.h"
#include "..\..\common_h\templates\array.h"
#include "..\..\common_h\templates\string.h"
#include "..\..\common_h\matrix.h"
#include "..\defines.h"

class MODEL;
class NODE;
class CMatrix;

class InterfaceBackScene : public ENTITY
{
	struct LightParam
	{
		bool bUse;

		long indexLight;
		D3DLIGHT9 lightSource;
		D3DLIGHT9 lightOldSource;

		dword dwFlareColor;
		float fMinFlareColor;
		float fMaxFlareColor;

		D3DCOLORVALUE colorMin,colorMax;
		float fColorTimer;
		float fColorPeriod;
		float fAddPeriod;
		float fAddPeriodMax;

		float fRangeMin,fRangeMax;
		float fRangeTimer;
		float fRangePeriod;

		MODEL* pModel;
		ENTITY_ID eiModel;

		CVECTOR vLightPos;
		NODE* pLightSrcNode;

		LightParam() {bUse=false; pModel=0; pLightSrcNode=0; fColorTimer = 0.0f; }
		~LightParam();
		void UpdateParams(float fTime);
	};

public:
	InterfaceBackScene();
	~InterfaceBackScene();

	bool Init();
	void Execute(dword Delta_Time);
	void Realize(dword Delta_Time);
	dword _cdecl ProcessMessage(MESSAGE & message);

protected:
	VDX8RENDER* m_pRS;

	ENTITY_ID m_eiModel;
	MODEL* m_pModel;
	ENTITY_ID m_eiLocators;
	MODEL* m_pLocators;

	FXYPOINT m_pntOldMouse;
	CVECTOR m_vCamPos;
	CVECTOR m_vCamAng;
	float m_fCamPerspective;

	array<LightParam*> m_aLights;

	struct MenuDescr
	{
		bool bSelectable;
		ENTITY_ID eiActive;
		MODEL* pActive;
		ENTITY_ID eiPassive;
		MODEL* pPassive;
		string sEventName;

		MenuDescr() {pActive=0; pPassive=0; bSelectable=false;}
		~MenuDescr();
		void Set( CMatrix* pMtx, const char* pcActiveName, const char* pcPassiveName, const char* pcEvent, const char* pcPathName, const char* pcTechniqueName );
	};
	array<MenuDescr*> m_aMenuDescr;
	long m_nSelectMenuIndex;

	void LoadModel( const char* pcModelName );
	void SetCameraPosition( const char* pcLocatorName );
	void SetShipPosition( const char* pcLocName, ATTRIBUTES* pAChar );

	bool FindLocator( const char* pcLocName, CMatrix* pMtx, CVECTOR* pPos, float* pYAng );
	void SetLocatorPosition( MODEL* pModel, const char* pcLocName, CVECTOR & pos, NODE* &pNodPtr );

	void ReleaseMenuList();
	void CreateMenuList( long nStartIndex, ATTRIBUTES* pAMenu );
	void ChooseNextMenu();
	void ChoosePrevMenu();
	void SetNewMenu(long nNewSelectIndex);
	void SetMenuSelectableState( long nMenuIndex, bool bSelectable );
	void ExecuteMenu( long nMenuIndex );

	long CheckMousePos(float fX, float fY);

	void InitLight( ATTRIBUTES* pAParam );
	void SetLight();
	void RestoreLight();
	void FlareShow(long idx);

	void InitAniModel( ATTRIBUTES* pAParam );
	void InitStaticModel( ATTRIBUTES* pAParam );

	struct AniModelDescr
	{
		ENTITY_ID ei;
		MODEL* pModel;

		bool bUseTFactor;
		DWORD dwTFactor;

		AniModelDescr() {pModel=0;bUseTFactor=false;}
		~AniModelDescr() {api->DeleteEntity(ei); pModel=0;}
	};
	array<AniModelDescr*> m_apAniModel;

	// муха - перелетная птица!
	struct Particle
	{
		CVECTOR pos;
		float angle;
		float size;
		float alpha;
	};
	struct ParticleEx : public Particle
	{
		dword color;
		float frame;
	};
	struct ParticleFly : public ParticleEx
	{
		float ax, ay;
		float kx, ky;
		float a, k;
	};
	struct LampFlys
	{
		CVECTOR pos;
		float radius;
		long start;
		long num;
	};
	struct Vertex
	{
		CVECTOR pos;
		dword color;
		float u, v;
	};
	LampFlys * flys;
	long numFlys;
	long maxFlys;
	ParticleFly * fly;
	long numFly;
	long flyTex;
	Vertex buffer[256*6];

	CVECTOR m_vFlarePos;
	float m_fFlareSize;
	long m_nFlareTexture;

	void AddLampFlys(CVECTOR & pos);
	void ProcessedFlys(float dltTime);
	void DrawParticles(void * prts, long num, long size, long texture, const char * tech, bool isEx = false, long numU = 0);
};

#endif
