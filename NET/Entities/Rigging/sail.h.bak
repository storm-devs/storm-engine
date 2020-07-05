#ifndef _SAIL_H_
#define _SAIL_H_

#include "..\..\common.h"
#include "SAILONE.h"

struct SAILGROUP {
    DWORD nVert,nIndx;
    long vertBuf,indxBuf;
};

class NetSail : public SAIL_BASE
{
    // параметры загружаемые из INI-файла //
    //--------------------------------------
//float sailDownStep;
float SsailWindDepend;
float TsailWindDepend;
float FLEXSPEED;
float MAXSUMWIND;
float WINDANGL_DISCRETE;
float MAXTURNANGL;
float TURNSTEPANGL;
float ROLLINGSPEED;
int WINDVECTOR_TINCR;
int WINDVECTOR_TADD;
int WINDVECTOR_SINCR;
int WINDVECTOR_SADD;
int WINDVECTOR_QUANTITY;
float ts_min;
float ts_xdep;
float ts_zdep;
float fs_min;
float fs_xdep;
float fs_zdep;
float ss_min;
float ss_xdep;
float ss_zdep;
int texQuantity;
float texNumCommon;
float texNumEnglish;
float texNumTreangle;
float ROLL_Z_VAL;
float ROLL_Z_DELTA;
float ROLL_Y_VAL;
float ROLL_Y_DELTA;
float SSailRollForm[SAIL_COL_MAX];
float TSailRollForm[5];
float TR_FORM_MUL;
float fWindAdding;
float fTHoleFlexDepend;
float fSHoleFlexDepend;
float FALL_SSAIL_ADD_MIN;
float FALL_SSAIL_ADD_RAND;
float FALL_TSAIL_ADD_MIN;
float FALL_TSAIL_ADD_RAND;

long GROUP_UPDATE_TIME;

WORD SailQuantity;
WORD SailCurNum;

float m_fMinSpeedVal; // минимальная скорость даваемая парусами

// описание ветра
WIND globalWind;
float *WindVect;
    //------------------------------------------

    friend SAILONE;
    bool bUse;
	VDX8RENDER * RenderService;
    D3DMATERIAL8 mat;
    FILETIME ft_old;
    long texl;
	long m_nEmptyGerbTex;

public:
	 NetSail();
	~NetSail();
    // Entity func
	bool Init();
	void Realize(dword Delta_Time);
	void Execute(dword Delta_Time);
	bool CreateState(ENTITY_STATE_GEN * state_gen);
	bool LoadState(ENTITY_STATE * state);
	dword _cdecl ProcessMessage(MESSAGE & message);
	void SetDevice();
    // Collision func
    int LastTraceGroup;
    float Trace(const CVECTOR &src,const CVECTOR &dst);
	const char *GetCollideMaterialName();
	bool GetCollideTriangle(TRIANGLE &triangle);
	bool Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius, ADD_POLYGON_FUNC addpoly);
    float Cannon_Trace(long iBallOwner, const CVECTOR &src,const CVECTOR &dst);
    ENTITY_ID GetShipID() {return gdata[LastTraceGroup].shipEI;}
	SAILONE_BASE * FindSailForCharacter(int chrIdx,char* nodeName,int grNum);
	dword AttributeChanged(ATTRIBUTES * pAttr);

	int GetSailStateForCharacter(int chrIdx);

    SAILGROUP sg;

private:
    SAILTIME tm;
    // список всех парусов
    int sailQuantity;
    SAILONE* *slist;
    // список всех групп парусов
    int groupQuantity;
    struct GROUPDATA
    {
        bool bDeleted;
		bool bYesShip;
        ENTITY_ID shipEI;
        ENTITY_ID modelEI;
        int   sailQuantity;
        int*  sailIdx;
        // ограничительный гробик
        CVECTOR boxCenter,boxSize;
        float boxRadius;
        // Влияние на скорость корабля
        float shipSpeed;
        float maxSpeed;
        float speed_c,speed_m; // реальная и максимальная скорость даваемая парусами
		int   maxHole,curHole;
		int maxSP;
        // спуск\подъем парусов
        int   curSailSet;
        bool  bFinalSailUp;
        bool  bFinalSailDo;
        float fSpeedMul;
		float fRollingSpeed;
		// цвет паруса
		dword dwSailsColor;
    };
    GROUPDATA* gdata;
    void FirstRun();

	bool	GetSailGrid();
    void	AddSailLabel(GEOS::LABEL& lbl, NODE* nod, bool bSailUp);
    void	SetAllSails(int groupNum);
    void	SetAllSails();
    void	SetAddSails(int firstSail);
    void	LoadSailIni();
    void	DoSailToNewHost(ENTITY_ID newMdlEI, ENTITY_ID hewHostEI, int grNum, NODE *nod, ENTITY_ID oldmodelEI);
    void	DoNoRopeSailToNewHost(ENTITY_ID newModel, ENTITY_ID newHost, ENTITY_ID oldHost);
    void	DeleteSailGroup();
	int		FindGroupForCharacter(int chrIdx);
	int		GetCharacterForGroup(int grNum);
	SAILONE * FindSailFromData(int gn,char* nodeName,char* grName);
	void	SetSailTextures(long grNum, VDATA* pvd);
	void	DoRandomsSailsDmg(int chrIdx, int gn, float fDmg);

	// обработка скриптовых запросов
	dword _cdecl ScriptProcessing(char * name, MESSAGE & message);

	void MakeHoleFromServer(long wClientID, long iSailIDCode, long iSailState);

    bool bFirstRun;
    int  wFirstIndx;
    bool bDeleteState;
    bool bCannonTrace;

	long m_nMastCreatedCharacter;
	char* m_sMastName;

	long m_nLastUpdate;
};

#endif