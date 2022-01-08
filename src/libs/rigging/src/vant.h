#pragma once

#include "matrix.h"
#include "dx9render.h"
#include "geos.h"
#include "v_module_api.h"

#include <filesystem>

class NODE;

#define VANT_EDGE 5 // number of edges in the rope section

#define VANTVERTEX_FORMAT (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

struct VANTVERTEX
{
    CVECTOR pos;
    float tu, tv;
};

class VANT_BASE : public Entity
{
  public:
    VANT_BASE();
    ~VANT_BASE() override;
    void SetDevice();
    bool Init() override;
    void Realize(uint32_t Delta_Time);
    void Execute(uint32_t Delta_Time);
    bool CreateState(ENTITY_STATE_GEN *state_gen);
    bool LoadState(ENTITY_STATE *state);
    uint64_t ProcessMessage(MESSAGE &message) override;
    virtual void LoadIni() = 0;

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

    VDX9RENDER *RenderService;

  protected:
    // parameters obtained from INI file //
    // -------------------------------------
    int ROPE_QUANT;        // number of ropes
    float ROPE_WIDTH;      // rope thickness
    float upWidth;         // upper triangle width
    float upHeight;        // the height of the upper triangle
    float treangXl;        // triangle texture coordinates
    float treangXr;        //
    float treangYu;        //
    float treangYd;        //
    float balkYu;          // beam texture coordinates
    float balkYd;          //
    float ropeXl;          // vertical rope texture coordinates
    float ropeXr;          //
    float vRopeXl;         // xBeg horizontal rope texture coordinates
    float vRopeXr;         //
    float vRopeHeight;     // vertical rope height
    float hRopeHeight;     // horizontal rope height
    float fBalkHeight;     // beam height relative to triangle height
    float fBalkWidth;      //
    float fVantMaxDist;    // the square of the distance from which the guys are not visible
    float ZERO_CMP_VAL;    // Guy motion sampling step
    float MAXFALL_CMP_VAL; // the maximum change in the guy position at which the guy stops being displayed
    // -------------------------------------
    std::filesystem::file_time_type ft_old;

    bool bUse;
    bool bRunFirstTime;
    bool bYesDeleted;
    int wVantLast;
    char *TextureName;
    int32_t texl;
    int VantId;

    struct VANTDATA
    {
        bool bDeleted;
        CVECTOR pUp, pLeft, pRight;
        CMatrix *pUpMatWorld, *pDownMatWorld;
        uint32_t sv, nv, st, nt;

        int vantNum;
        CVECTOR pos[VANT_EDGE];

        CVECTOR pUpOld, pLeftOld, pUpStart, pLeftStart;
        int HostGroup;
    };

    int vantQuantity;
    VANTDATA **vlist;

    struct GROUPDATA
    {
        bool bDeleted;
        int vantQuantity;
        int *vantIdx;
        int32_t sVert, nVert;
        int32_t sIndx, nIndx;

        CMatrix *pMatWorld;
        entid_t model_id;
        entid_t shipEI;
    };

    int groupQuantity;
    GROUPDATA *gdata;

    void SetVertexes() const;
    void SetIndex() const;
    void AddLabel(GEOS::LABEL &lbl, NODE *nod);
    void SetAll();
    void SetAdd(int firstNum);
    void doMove();
    bool VectCmp(CVECTOR v1, CVECTOR v2, float minCmpVal);
    void FirstRun();
    void DoSTORM_DELETE();

    VANTVERTEX *vertBuf;

    int32_t vBuf, iBuf;
    uint32_t nVert, nIndx;
};

class VANT : public VANT_BASE
{
  public:
    VANT(){};

    ~VANT() override{};

    void LoadIni() override;
};

class VANTL : public VANT_BASE
{
  public:
    VANTL(){};

    ~VANTL() override{};

    void LoadIni() override;
};

class VANTZ : public VANT_BASE
{
  public:
    VANTZ(){};

    ~VANTZ() override{};

    void LoadIni() override;
};
