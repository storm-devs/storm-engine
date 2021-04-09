#pragma once

#include "SAILONE.h"
#include "dx9render.h"
#include "geos.h"
#include "model.h"
#include "sail_base.h"
#include "vmodule_api.h"

#include <filesystem>

class VDATA;

struct SAILGROUP
{
    uint32_t nVert, nIndx;
    long vertBuf, indxBuf;
};

class SAIL : public SAIL_BASE
{
    // parameters loaded from INI file //
    // --------------------------------------
    // float sailDownStep;
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

    uint16_t SailQuantity;
    uint16_t SailCurNum;

    float m_fMinSpeedVal; // minimum sail speed

    // wind description
    WIND globalWind;
    float *WindVect;
    //------------------------------------------

    friend SAILONE;
    bool bUse;
    VDX9RENDER *RenderService;
    D3DMATERIAL9 mat;
    std::filesystem::file_time_type ft_old;
    long texl;
    long m_nEmptyGerbTex;

  public:
    SAIL();
    ~SAIL();
    // Entity func
    bool Init() override;
    void Realize(uint32_t Delta_Time);
    void Execute(uint32_t Delta_Time);
    bool CreateState(ENTITY_STATE_GEN *state_gen);
    bool LoadState(ENTITY_STATE *state);
    uint64_t ProcessMessage(MESSAGE &message) override;
    void SetDevice();
    // Collision func
    int LastTraceGroup;
    float Trace(const CVECTOR &src, const CVECTOR &dst) override;
    const char *GetCollideMaterialName() override;
    bool GetCollideTriangle(TRIANGLE &triangle) override;
    bool Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius,
              ADD_POLYGON_FUNC addpoly) override;
    float Cannon_Trace(long iBallOwner, const CVECTOR &src, const CVECTOR &dst) override;
    entid_t GetShipID() override
    {
        return gdata[LastTraceGroup].shipEI;
    }
    SAILONE_BASE *FindSailForCharacter(int chrIdx, char *nodeName, int grNum) override;
    uint32_t AttributeChanged(ATTRIBUTES *pAttr) override;

    void LostRender();
    void RestoreRender();

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
        case Stage::lost_render:
            LostRender();
            break;
        case Stage::restore_render:
            RestoreRender();
            break;
        }
    }

    int GetSailStateForCharacter(int chrIdx) const;

    SAILGROUP sg;

  private:
    SAILTIME tm;
    // list of all sails
    int sailQuantity;
    SAILONE **slist;
    // list of all sail groups
    int groupQuantity;

    struct GROUPDATA
    {
        bool bDeleted;
        bool bYesShip;
        entid_t shipEI;
        entid_t modelEI;
        int sailQuantity;
        int *sailIdx;
        // restrictive box
        CVECTOR boxCenter, boxSize;
        float boxRadius;
        // Effect on ship speed
        float shipSpeed;
        float maxSpeed;
        float speed_c, speed_m; // real and maximum speed given by sails
        int maxHole, curHole;
        int maxSP;
        // lowering \ raising sails
        int curSailSet;
        bool bFinalSailUp;
        bool bFinalSailDo;
        bool bFinalSailDoOld;
        float fSpeedMul;
        float fRollingSpeed;
        // sail color
        uint32_t dwSailsColor;
    };

    GROUPDATA *gdata;
    void FirstRun();

    bool GetSailGrid();
    void AddSailLabel(GEOS::LABEL &lbl, NODE *nod, bool bSailUp);
    void SetAllSails(int groupNum);
    void SetAllSails();
    void SetAddSails(int firstSail);
    void LoadSailIni();
    void DoSailToNewHost(entid_t newMdlEI, entid_t hewHostEI, int grNum, NODE *nod, entid_t oldmodelEI);
    void DoNoRopeSailToNewHost(entid_t newModel, entid_t newHost, entid_t oldHost);
    void DeleteSailGroup();
    int FindGroupForCharacter(int chrIdx) const;
    int GetCharacterForGroup(int grNum) const;
    SAILONE *FindSailFromData(int gn, const char *nodeName, const char *grName) const;
    void SetSailTextures(long grNum, VDATA *pvd) const;
    void DoRandomsSailsDmg(int chrIdx, int gn, float fDmg);
    void GetSailStatus(int chrIdx, int gn);

    // processing script requests
    uint32_t ScriptProcessing(const char *name, MESSAGE &message);

    bool bFirstRun;
    int wFirstIndx;
    bool bDeleteState;
    bool bCannonTrace;

    long m_nMastCreatedCharacter;
    char *m_sMastName;

    long m_nLastUpdate;
};
