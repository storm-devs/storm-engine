#pragma once

#include "dx9render.h"
#include "geos.h"
#include "matrix.h"
#include "sail_base.h"

class NODE;

#define ROPEVERTEX_FORMAT (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

struct ROPEVERTEX
{
    CVECTOR pos;
    float tu, tv;
};

#define ROPE_EDGE 5 // number of edges in the rope section

class ROPE : public ROPE_BASE
{
    // parameters obtained from INI file //
    // -------------------------------------
    float ROPE_SEG_LENGTH; // length of one rope segment
    float ROPE_WIDTH;      // rope thickness
    float STAY_WIDTH;      // rope (head) thickness
    float ROPE_END_LENGTH; // the length of the rope (triangle) at the point of connection with the sail in relation to
    // the total length
    float ROPE_WAVE;         // amplitude of rope vibration in absolute values
    float MaxCh;             // maximum coordinate change at which the entire rope is recalculated
    float ROPE_TEX_LEN;      // the length of the rope on which the texture applies
    float fMaxRopeDist;      // maximum distance from which the ropes are visible
    float DEEP_CHANGE_SPEED; // the rate of change in the depth of rope deflection
    float ROTATE_SPEED;      // rope swing speed
    float MIN_DEEP_MUL;      // the minimum value of the coefficient of use of the deflection of the rope
    float VAR_DEEP_MUL;      // limit of variation of the coefficient of use of rope deflection
    float VAR_ROTATE_ANGL;   // rope swing angle
    // -------------------------------------

    D3DMATERIAL9 mat;

    bool bUse;
    bool bFirstRun;
    bool bYesDeleted;
    int wRopeLast;

    char *TextureName;
    int32_t texl;

    VDX9RENDER *RenderService;

  public:
    ROPE();
    ~ROPE() override;
    // Entity Function
    void SetDevice();
    bool Init() override;
    void Realize(uint32_t Delta_Time);
    void Execute(uint32_t Delta_Time);

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
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    bool CreateState(ENTITY_STATE_GEN *state_gen);
    bool LoadState(ENTITY_STATE *state);
    uint64_t ProcessMessage(MESSAGE &message) override;
    // Service Function
    void GetEndPoint(CVECTOR *cv, int ropenum, entid_t mdl_id) override;
    bool IsAbsentRope(entid_t mdl_id, int ropenum) override;
    void DoDeleteUntie(entid_t mdl_id, NODE *rnod, int gNum) override;

  private:
    ROPEVERTEX *vertBuf;

    // ropes list
    struct ROPEDATA
    {
        bool bUse;
        bool bDeleted;
        int HostGroup;

        CVECTOR pBeg, pEnd, pOld;

        float len;
        CVECTOR vDeep;
        float angDeep, angRot;

        bool btie, etie;
        int bgnum, egnum;
        CMatrix *bMatWorld, *eMatWorld;

        uint32_t sv, nv, st, nt;

        int ropeNum;
        CVECTOR pos[ROPE_EDGE];
        CVECTOR cv;
        uint16_t segnum;
        uint16_t segquant;

        float ropeWave; // rope swing
        bool bMakeWave;
    };

    int ropeQuantity;
    ROPEDATA **rlist;

    // list of groups (ships) of ropes
    struct GROUPDATA
    {
        bool bDeleted;
        int ropeQuantity;
        int *ropeIdx;
        entid_t shipEI;
        entid_t modelEI;
        CMatrix *pMatWorld;
        int32_t sv, nv;
        int32_t st, nt;
    };

    int groupQuantity;
    GROUPDATA *gdata;

    void SetVertexes();
    void SetVertexes(ROPEDATA *pr, float dtime) const;
    void SetTextureGrid(ROPEDATA *pr) const;
    void SetIndex() const;
    void DoSTORM_DELETE();
    void AddLabel(GEOS::LABEL &lbl, NODE *nod, bool bDontSage);
    void SetAdd(int firstNum);
    void LoadIni();
    void FirstRun();

    int32_t vBuf, iBuf;
    uint32_t nVert, nIndx;

    uint64_t execute_tm;
    uint64_t realize_tm;
};
