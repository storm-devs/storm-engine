#ifndef _MAST_H_
#define _MAST_H_

#include "..\common_h\dx8render.h"
#include "..\common_h\defines.h"
#include "..\common_h\model.h"
#include "..\common_h\collide.h"

#define SR_MOVE         1
#define SR_STOPROTATE   2
#define SR_YROTATE      4

class MAST : public ENTITY
{
	struct MountData {
		NODE * pNode;
		ENTITY_ID shipEI;
		ENTITY_ID modelEI;
	} m_mount_param;
    bool            bUse;
    bool            bFallUnderWater;
    int             wMoveCounter;
	VDX8RENDER		*RenderService;
	COLLIDE			*pCollide;
    bool            bModel;
    ENTITY_ID       model_id,oldmodel_id;
    ENTITY_ID       ship_id;
    FILETIME        ft_old;
	NODE*			m_pMastNode;
public:
	 MAST();
	~MAST();
	void            SetDevice();
	bool            Init();
	void            Move();
    void            Execute(dword Delta_Time);
	void            Realize(dword Delta_Time);
	bool            CreateState(ENTITY_STATE_GEN * state_gen);
	bool            LoadState(ENTITY_STATE * state);
	dword _cdecl    ProcessMessage(MESSAGE & message);
protected:
    void            AllRelease();
    void _cdecl     Mount( ENTITY_ID modelEI, ENTITY_ID shipEI, NODE* mastNodePointer );
    void            LoadIni();
    void            doMove(dword DeltaTime);
    int             GetSlide(ENTITY_ID &mod, CVECTOR &pbeg, CVECTOR &pend, CVECTOR &dp, CVECTOR &lrey, CVECTOR &rrey, float &angl);
    struct MASTMOVE
    {
        CVECTOR bp,ep; // координаты мачты
        CVECTOR brey,erey; // координаты реи

        CVECTOR ang,dang,sdang;
        CVECTOR mov,dmov,sdmov;
    };
    MASTMOVE mm;
};

#endif