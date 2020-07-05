#ifndef _VANT_H_
#define _VANT_H_

#include "..\..\common.h"

class NODE;

#define VANT_EDGE 5 // число граней в сечении веревки

#define VANTVERTEX_FORMAT		(D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2)

struct VANTVERTEX
{
	CVECTOR pos;
	float tu, tv;
};

class NetVant : public ENTITY
{
    // параметры получаемые из INI-файла //
    //-------------------------------------
	int ROPE_QUANT;// количество веревок
	float ROPE_WIDTH;// толщина веревки
	float upWidth;// ширина верхнего треугольника
	float upHeight;// высота верхнего треугольника
	float treangXl;// координаты текстуры треугольника
	float treangXr;//
	float treangYu;//
	float treangYd;//
	float balkYu;// координаты текстуры балки
	float balkYd;//
	float ropeXl;// координаты текстуры вертикальной веревки
	float ropeXr;//
	float vRopeXl;// координаты текстуры горизонтальной веревки xBeg
	float vRopeXr;//
	float vRopeHeight;// высота вертикальной веревки
	float hRopeHeight;// высота горизонтальной веревки
	float fBalkHeight;// высота балки относительно высоты треугольника
	float fBalkWidth;//
	float fVantMaxDist;// квадрат расстояния с которого не видны ванты
	float ZERO_CMP_VAL;// шаг дискретизации движения ванта
	float MAXFALL_CMP_VAL;// максимальное изменение положения ванта при котором вант перестает отображаться
    //-------------------------------------
    FILETIME ft_old;

    bool bUse;
    bool bRunFirstTime;
    bool bYesDeleted;
    int  wVantLast;
	VDX8RENDER  *RenderService;
    char* TextureName;
    long texl;

public:
	 NetVant();
	~NetVant();
	void SetDevice();
	bool Init();
	void Realize(dword Delta_Time);
	void Execute(dword Delta_Time);
	bool CreateState(ENTITY_STATE_GEN * state_gen);
	bool LoadState(ENTITY_STATE * state);
    dword _cdecl ProcessMessage(MESSAGE & message);

private:
    struct VANTDATA
    {
        bool bDeleted;
        CVECTOR pUp,pLeft,pRight;
        CMatrix *pUpMatWorld,*pDownMatWorld;
        DWORD sv,nv, st,nt;

        int vantNum;
        CVECTOR pos[VANT_EDGE];

        CVECTOR pUpOld,pLeftOld, pUpStart,pLeftStart;
        int HostGroup;
    };
    int vantQuantity;
    VANTDATA **vlist;

    struct GROUPDATA
    {
        bool bDeleted;
        int vantQuantity;
        int *vantIdx;
        long sVert,nVert;
        long sIndx,nIndx;

        CMatrix *pMatWorld;
        ENTITY_ID model_id;
		ENTITY_ID shipEI;
    };
    int groupQuantity;
    GROUPDATA *gdata;

    void SetVertexes();
    void SetIndex();
    void AddLabel(GEOS::LABEL &lbl, NODE* nod);
    void SetAll();
    void SetAdd(int firstNum);
    void LoadIni();
    void doMove();
    bool VectCmp(CVECTOR v1,CVECTOR v2,float minCmpVal);
    void FirstRun();
    void DoDelete();

    VANTVERTEX *vertBuf;

    long vBuf,iBuf;
    DWORD nVert,nIndx;
};



#endif