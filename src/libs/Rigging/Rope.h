#ifndef _ROPE_H_
#define _ROPE_H_

#include "Entity.h"
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

#define ROPE_EDGE 5 // число граней в сечении веревки

class ROPE : public ROPE_BASE
{
    // параметры получаемые из INI-файла //
    //-------------------------------------
    float ROPE_SEG_LENGTH; // длина одного сегмента веревки
    float ROPE_WIDTH;      // толщина веревки
    float STAY_WIDTH;      // толщина веревки (штага)
    float ROPE_END_LENGTH; // длина веревки (треугольника) в точке соединения с парусом относительно общей длинны
    float ROPE_WAVE; // амплитуда колебания веревки в абсолютных значениях
    float MaxCh; // максимальное изменение координаты при котором происходит перерасчет всей веревки
    float ROPE_TEX_LEN; // длина веревки на которую ложится текстура
    float fMaxRopeDist; // максимальное растояние с кототрого видны веревки
    float DEEP_CHANGE_SPEED; // скорость изменения глубины прогиба веревки
    float ROTATE_SPEED;      // скорость качания веревки
    float MIN_DEEP_MUL; // минимальное значение коэффициента использования прогиба веревки
    float VAR_DEEP_MUL; // предел изменения коэффициента использования прогиба веревки
    float VAR_ROTATE_ANGL; // амплитуда угла качания веревки
    //-------------------------------------

    D3DMATERIAL9 mat;

    bool bUse;
    bool bFirstRun;
    bool bYesDeleted;
    int wRopeLast;

    char *TextureName;
    long texl;

    VDX9RENDER *RenderService;

  public:
    ROPE();
    ~ROPE();
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

    // список веревок
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

        float ropeWave; // амплитуда колебания веревки
        bool bMakeWave;
    };

    int ropeQuantity;
    ROPEDATA **rlist;

    // список групп (кораблей) веревок
    struct GROUPDATA
    {
        bool bDeleted;
        int ropeQuantity;
        int *ropeIdx;
        entid_t shipEI;
        entid_t modelEI;
        CMatrix *pMatWorld;
        long sv, nv;
        long st, nt;
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

    long vBuf, iBuf;
    uint32_t nVert, nIndx;

    uint64_t execute_tm;
    uint64_t realize_tm;
};

/*API_MODULE_START("rope")
    CREATE_CLASS(0,ROPE)
API_MODULE_END
*/

#endif
