#ifndef _VANT_H_
#define _VANT_H_

#include "Matrix.h"
#include "dx9render.h"
#include "geos.h"
#include "vmodule_api.h"

class NODE;

#define VANT_EDGE 5 // число граней в сечении веревки

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
    virtual ~VANT_BASE();
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
    // параметры получаемые из INI-файла //
    //-------------------------------------
    int ROPE_QUANT;        // количество веревок
    float ROPE_WIDTH;      // толщина веревки
    float upWidth;         // ширина верхнего треугольника
    float upHeight;        // высота верхнего треугольника
    float treangXl;        // координаты текстуры треугольника
    float treangXr;        //
    float treangYu;        //
    float treangYd;        //
    float balkYu;          // координаты текстуры балки
    float balkYd;          //
    float ropeXl;          // координаты текстуры вертикальной веревки
    float ropeXr;          //
    float vRopeXl;         // координаты текстуры горизонтальной веревки xBeg
    float vRopeXr;         //
    float vRopeHeight;     // высота вертикальной веревки
    float hRopeHeight;     // высота горизонтальной веревки
    float fBalkHeight;     // высота балки относительно высоты треугольника
    float fBalkWidth;      //
    float fVantMaxDist;    // квадрат расстояния с которого не видны ванты
    float ZERO_CMP_VAL;    // шаг дискретизации движения ванта
    float MAXFALL_CMP_VAL; // максимальное изменение положения ванта при котором вант перестает отображаться
    //-------------------------------------
    FILETIME ft_old;

    bool bUse;
    bool bRunFirstTime;
    bool bYesDeleted;
    int wVantLast;
    char *TextureName;
    long texl;
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
        long sVert, nVert;
        long sIndx, nIndx;

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

    long vBuf, iBuf;
    uint32_t nVert, nIndx;
};

class VANT : public VANT_BASE
{
  public:
    VANT(){};
    virtual ~VANT(){};

    virtual void LoadIni();
};

class VANTL : public VANT_BASE
{
  public:
    VANTL(){};
    virtual ~VANTL(){};

    virtual void LoadIni();
};

class VANTZ : public VANT_BASE
{
  public:
    VANTZ(){};
    virtual ~VANTZ(){};

    virtual void LoadIni();
};

#endif
