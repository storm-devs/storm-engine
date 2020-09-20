#ifndef _FLAG_H_
#define _FLAG_H_

#include "..\common_h\dx8render.h"
#include "..\common_h\geos.h"
#include "..\common_h\matrix.h"
#include "..\common_h\model.h"
#include "..\common_h\sd2_h\VAI_ObjBase.h"

#define FLAGLXVERTEX_FORMAT (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

struct FLAGLXVERTEX
{
    CVECTOR pos;
    float tu, tv;
};

class FLAG : public ENTITY
{
  public:
    int ENGLISH_FLAG_TEX; // england
    int SPANISH_FLAG_TEX; // spain
    int FRENCH_FLAG_TEX;  // french
    int DUTCH_FLAG_TEX;   // dutch
    int PIRATE_FLAG_TEX;  // pirate

    FLAG();
    ~FLAG();

    bool bUse;
    bool bFirstRun;
    char *TextureName;
    char TextureSectionName[256];

    void SetDevice();
    bool Init();

    void Execute(dword Delta_Time);
    void Realize(dword Delta_Time);

    bool CreateState(ENTITY_STATE_GEN *state_gen);
    bool LoadState(ENTITY_STATE *state);

    dword _cdecl ProcessMessage(MESSAGE &message);

  protected:
    // ----- параметры из INI-файла -----------
    float FLAGVECTORLEN; // длина сегмента флага
    float ALFA_DEPEND;   // скорость изменения угла колебания
    float BETA_DEPEND;   // скорость изменения угла вращения
    float ALFA_RAND; // максимальное значение случайного изменения угла Alpha
    float BETA_RAND; // максимальное значение случайного изменения угла Beta
    float fWindAm;   // Амплитуда колебания флага
    float fRotAm;    // Амплитуда вращения флага
    float DOWNVAL;   // значение декремента по Y
    float fAlfaMax;  // максимальный угол (для конца флага)
    float fAlfaStep;
    float fBetaMax; // максимальный угол (для конца флага)
    float fBetaStep;
    int FlagTextureQuantity;
    int FlagTextureQuantityRow;
    float fWindMaxValue;
    int MinSegmentQuantity;
    // ------------------------------------------------------

    struct FLAGDATA
    {
        WORD vectQuant; // number of segmentes(square) into flags

        bool triangle;
        bool isSpecialFlag;
        bool isShip;

        CVECTOR spos;      // start flags position
        CVECTOR dv;        // delta flags start position to along
        CVECTOR dhv, ddhv; // delta from position to flag top&bottom and delta for this value

        CVECTOR curpos, dHV;
        int lineNum;
        int windIdx, curIdx;
        bool bDecreaze;

        DWORD sv, nv; // start vertex into buffer and quantity vertex
        DWORD st, nt; // start treangle into buffer and quantity vertex

        int flagNum; // number of flag
        int grNum;   // number of flag group
        NODE *nod;
        CMatrix *pMatWorld;

        int texNumC;
        int texNumR;

        FLAGDATA *next;

        float Alfa;
        float Beta;

        int HostGroup;
        bool bDeleted;
    };
    int flagQuantity;
    FLAGDATA **flist;

    struct GROUPDATA
    {
        int nation;
        ENTITY_ID model_id;
        bool bDeleted;
        bool isShip;
        ENTITY_ID ship_id;
        ATTRIBUTES *char_attributes;
    };
    int groupQuantity;
    GROUPDATA *gdata;

    VDX8RENDER *RenderService;

    long texl;
    struct WIND
    {
        float base; // амплитуда колебаний ветра от 0 до 1.0
        struct
        {
            float x, y, z;
        } ang;
    };
    WIND globalWind;
    FILETIME ft_old;

    void FirstRun();
    void SetTextureCoordinate();
    void SetTreangle();
    void DoMove(FLAGDATA *pr, float delta_time);
    void AddLabel(GEOS::LABEL &gl, NODE *nod, bool isSpecialFlag, bool isShip);
    void SetAll();
    void LoadIni();
    void GroupDelete(ENTITY_ID m_id);
    void DoDelete();
    void SetAdd(int flagNum);
    void MoveOtherHost(ENTITY_ID newm_id, long flagNum, ENTITY_ID oldm_id);

    FLAGLXVERTEX *vertBuf;
    WORD *indxBuf;

    D3DXMATRIX rootMatrix;

    long vBuf, iBuf;
    DWORD nVert, nIndx;
    bool bYesDeleted;
    int wFlagLast;
};
#endif