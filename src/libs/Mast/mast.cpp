#include "MAST.h"
#include "../../shared/mast_msg.h"
#include "../../shared/sail_msg.h"
#include "Island_Base.h"
#include "ship_base.h"

#define DELTA_TIME(x) ((x)*0.001f)
#define DELTA_TIME_ROTATE(x) ((x)*0.01f)

static const char *MAST_INI_FILE = "resource\\ini\\mast.ini";

float MAST_MOVE_STEP = 0.2f;
float MAST_FALL_STEP = .05f;
float MAST_MAX_FALL_SPEED = .5f;
float MAX_FALL_ANGLE = PI;
float YROTATE_STEP = 0.1f;
float TRACE_HEIGHT = 40.f;
float TRACE_ADDING = .01f;
float TRACE_SLIDING = 0.1f;
float MAST_WIDTH = 0.15f;
float MINZ_COMPARE = 1.f;
float MAX_CHANGE_LENGTH = .8f;
float MAX_SLIDING_LENGHT = 1.2f;
float DESTRUCT_MAST_DEEP = 20.f;
float DEEP_FALL_STEP = 3.f;
int MAX_MOVE_CICLES = 50;
int MIN_MOV_COUNTER = 5;
// angles of rotation (fall) of the mast
float MIN_X_DANG = -0.01f;
float VAR_X_DANG = 0.02f;
float MIN_Z_DANG = 0.07f;
float VAR_Z_DANG = 0.03f;
float MIN_SIGNIFICANT = 0.1f;

MAST::MAST()
{
    RenderService = nullptr;
    wMoveCounter = 0;
    bUse = false;
    m_pMastNode = nullptr;

    m_mount_param.pNode = nullptr;
}

MAST::~MAST()
{
    AllRelease();
}

bool MAST::Init()
{
    // GUARD(MAST::Init())

    SetDevice();

    // UNGUARD
    return true;
}

void MAST::SetDevice()
{
    // GUARD(MAST::SetDevice())

    RenderService = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!RenderService)
        throw std::exception("No service: dx9render");

    pCollide = static_cast<COLLIDE *>(core.CreateService("COLL"));
    if (!pCollide)
        throw std::exception("No service: collide");

    LoadIni();

    // UNGUARD
}

bool MAST::CreateState(ENTITY_STATE_GEN *state_gen)
{
    // GUARD(bool MAST::CreateState(ENTITY_STATE_GEN * state_gen))

    return true;
    // UNGUARD
}

bool MAST::LoadState(ENTITY_STATE *state)
{
    // GUARD(bool MAST::LoadState(ENTITY_STATE * state))

    SetDevice();

    // UNGUARD
    return true;
}

void MAST::Execute(uint32_t Delta_Time)
{
    // GUARD(void MAST::Execute(uint32_t Delta_Time))

    if (bUse)
    {
        // ====================================================
        // If the ini-file has been changed, read the info from it
        if (fio->_FileOrDirectoryExists(MAST_INI_FILE))
        {
            auto ft_new = fio->_GetLastWriteTime(MAST_INI_FILE);
            if (ft_old != ft_new)
            {
                LoadIni();
            }
        }
        doMove(Delta_Time);
        auto *mdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(model_id));
        if (mdl)
            mdl->Update();
    }
    else
    {
        EntityManager::EraseEntity(GetId());
    }
    // UNGUARD
}

#define D3DLXLINEVERTEX_FORMAT (D3DFVF_DIFFUSE | D3DFVF_XYZ | D3DFVF_TEX0)

void MAST::Realize(uint32_t Delta_Time)
{
    // GUARD(void MAST::Realize(uint32_t Delta_Time))

    if (m_mount_param.pNode)
    {
        Mount(m_mount_param.modelEI, m_mount_param.shipEI, m_mount_param.pNode);
        m_mount_param.pNode = nullptr;
    }

    MODEL *mdl;
    if ((mdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(model_id))) != nullptr)
    {
        RenderService->SetRenderState(D3DRS_LIGHTING, true);
        mdl->ProcessStage(Stage::realize, Delta_Time);
        RenderService->SetRenderState(D3DRS_LIGHTING, false);

        /*        CVECTOR bp=mdl->mtx*mm.bp;
                CVECTOR ep=mdl->mtx*mm.ep;
                CVECTOR brey=mdl->mtx*mm.brey;
                CVECTOR erey=mdl->mtx*mm.erey;

                CMatrix mtx;
                mtx.SetIdentity();
                RenderService->SetTransform(D3DTS_WORLD,(D3DXMATRIX*)&mtx);

                struct LINEVERTEX
                {
                    CVECTOR pos;
                    float   col;
                };
                LINEVERTEX pVerts[4];
                pVerts[0].pos=bp; pVerts[0].col=0xffffff;
                pVerts[1].pos=ep; pVerts[1].col=0xffffff;
                pVerts[2].pos=brey; pVerts[2].col=0xffffff;
                pVerts[3].pos=erey; pVerts[3].col=0xffffff;
                RenderService->DrawPrimitiveUP(D3DPT_LINELIST, D3DLXLINEVERTEX_FORMAT, 2, pVerts, sizeof(LINEVERTEX));*/
    }

    // UNGUARD
}

uint64_t MAST::ProcessMessage(MESSAGE &message)
{
    // GUARD(uint32_t MAST::ProcessMessage(MESSAGE message))

    switch (message.Long())
    {
    case MSG_MAST_SETGEOMETRY: {
        if (!m_mount_param.pNode)
        {
            m_mount_param.pNode = (NODE *)message.Pointer(); // new root node pointer
            m_mount_param.shipEI = message.EntityID();       // ship entity
            m_mount_param.modelEI = message.EntityID();      // old model entity
        }
    }
    break;
    }

    // UNGUARD
    return 0;
}

#define ADD_MINIMUM .01f

void MAST::Mount(entid_t modelEI, entid_t shipEI, NODE *mastNodePointer)
{
    m_pMastNode = mastNodePointer;
    if (mastNodePointer == nullptr)
        return;
    auto *oldmdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(modelEI));
    if (oldmdl == nullptr)
        return; // do not bring down anything if there is no old model
    oldmodel_id = modelEI;
    ship_id = shipEI;

    const auto ropeEI = EntityManager::GetEntityId("rope");
    const auto sailEI = EntityManager::GetEntityId("sail");
    const auto flagEI = EntityManager::GetEntityId("flag");
    const auto vantEI = EntityManager::GetEntityId("vant");

    // find the attributes
    VAI_OBJBASE *pVAI = nullptr;
    pVAI = static_cast<VAI_OBJBASE *>(EntityManager::GetEntityPointer(shipEI));
    ATTRIBUTES *pA = nullptr;
    if (pVAI != nullptr)
        pA = pVAI->GetACharacter();

    ATTRIBUTES *pAMasts = nullptr;
    if (pA != nullptr)
        pAMasts = pA->FindAClass(pA, "Ship.Masts");
    float fMastDamage = 0.f;
    if (pAMasts != nullptr)
        fMastDamage = pAMasts->GetAttributeAsFloat((char *)mastNodePointer->GetName(), 0.f);
    long chrIdx = -1;
    if (pA != nullptr)
        chrIdx = pA->GetAttributeAsDword("index", -1);
    core.Event("EventMastFall", "lsl", chrIdx, mastNodePointer->GetName(), fMastDamage < 1.f);
    if (fMastDamage < 1.f)
    {
        if (sailEI)
            core.Send_Message(sailEI, "lls", MSG_SAIL_MAST_PROCESSING, chrIdx, mastNodePointer->GetName());
    }

    if (mastNodePointer != nullptr) //~!~
    {
        int i, j;

        // create a new model
        bModel = true;
        model_id = mastNodePointer->Unlink2Model();

        // go through all the ropes of this mast and turn them off
        if (vantEI)
            core.Send_Message(vantEI, "lip", MSG_VANT_DEL_MAST, modelEI, mastNodePointer);
        MODEL *mdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(model_id));
        if (mdl != nullptr)
            for (i = 0; i < 10000; i++)
            {
                NODE *nod = mdl->GetNode(i);
                if (nod == nullptr || nod->geo == nullptr)
                    break;
                GEOS::INFO gi;
                nod->geo->GetInfo(gi);
                for (j = 0; j < gi.nlabels; j++)
                {
                    GEOS::LABEL gl;
                    nod->geo->GetLabel(j, gl);
                    if (!strncmp(gl.name, "rope", 4))
                    {
                        if (ropeEI)
                            core.Send_Message(ropeEI, "lil", MSG_ROPE_DELETE, modelEI, atoi(&gl.name[5]));
                    }
                    if (!strncmp(gl.name, "fal", 3))
                    {
                        if (ropeEI)
                            core.Send_Message(ropeEI, "lil", MSG_ROPE_DELETE, modelEI, atoi(&gl.name[4]) + 1000);
                    }
                    else if (!strncmp(gl.name, "sail", 4))
                    {
                        if (sailEI)
                            core.Send_Message(sailEI, "liplii", MSG_SAIL_TO_NEWHOST, modelEI, nod,
                                              atoi(&gl.group_name[5]), GetId(), model_id);
                    }
                    else if (!strncmp(gl.group_name, "flag", 4))
                    {
                        if (flagEI)
                            core.Send_Message(flagEI, "lili", MSG_FLAG_TO_NEWHOST, modelEI, atoi(&gl.group_name[4]),
                                              model_id);
                    }
                }
                // also bring down the sails associated with this mast
                if (sailEI)
                {
                    core.Send_Message(sailEI, "liii", MSG_SAIL_CHECK, shipEI, GetId(), model_id);
                    core.Send_Message(sailEI, "li", MSG_SAIL_FREE_GROUP, GetId());
                }
            }
        if (sailEI)
            core.Send_Message(sailEI, "ll", MSG_SAIL_MAST_PROCESSING, -1);

        // set the initial parameters of the mast movement
        SHIP_BASE *sb;
        sb = static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(shipEI));
        if (sb)
        {
            mm.ang = sb->State.vAng;

            mm.dmov.y = 0.f;
            float k = KNOTS2METERS(sb->State.vSpeed.z);
            mm.dmov.z = k * cosf(sb->State.vAng.y);
            mm.dmov.x = k * sinf(sb->State.vAng.y);
            k = KNOTS2METERS(sb->State.vSpeed.x);
            mm.dmov.z += k * cosf(sb->State.vAng.y + PId2);
            mm.dmov.x += k * sinf(sb->State.vAng.y + PId2);

            mm.sdmov = mm.dmov;
        }
        else
        {
            mm.ang = CVECTOR(0.f, 0.f, 0.f);
            mm.dmov = mm.sdmov = CVECTOR(0.f, 0.f, 0.f);
        }
        mm.mov = mastNodePointer->glob_mtx.Pos();
        mm.dang = CVECTOR(MIN_X_DANG + VAR_X_DANG * static_cast<float>(rand()) / static_cast<float>(RAND_MAX), 0.f,
                          MIN_Z_DANG + VAR_Z_DANG * static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
        // find the nearest ship
        float minDist = 10000.f;
        SHIP_BASE *minDstShip;

        const auto &ships = EntityManager::GetEntityIdVector("ship");
        for (auto ship : ships)
        {
            if (ship == ship_id)
                continue;

            auto *sb = static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(ship));
            const float tmpDist = ~(sb->State.vPos - mm.mov);
            if (tmpDist < minDist)
            {
                minDist = tmpDist;
                minDstShip = sb;
            }
        }
        if (minDist < 4000.f) // if the nearest ship is close to us, then bring down the mast in the opposite direction
        {
            CVECTOR vect;
            mastNodePointer->glob_mtx.MulToInvNorm(minDstShip->State.vPos - mm.mov, vect);
            if (vect.x < 0.f)
                mm.dang.z = -mm.dang.z;
        }
        else if (rand() & 1)
            mm.dang.z = -mm.dang.z;

        if (mm.ang.x > MIN_SIGNIFICANT)
            if (mm.dang.x < 0.f)
                mm.dang.x = -mm.dang.x;
        if (mm.ang.x < -MIN_SIGNIFICANT)
            if (mm.dang.x > 0.f)
                mm.dang.x = -mm.dang.x;
        if (mm.ang.z > MIN_SIGNIFICANT)
            if (mm.dang.z < 0.f)
                mm.dang.z = -mm.dang.z;
        if (mm.ang.z < -MIN_SIGNIFICANT)
            if (mm.dang.z > 0.f)
                mm.dang.z = -mm.dang.z;

        mm.sdang = mm.dang;

        GEOS::INFO gi;
        // set the top and bottom points of the mast
        mastNodePointer->geo->GetInfo(gi);
        mm.ep = mm.bp = *(CVECTOR *)&gi.boxcenter;
        mm.bp.y -= gi.boxsize.y * .5f;
        mm.ep.y += gi.boxsize.y * .5f;
        if (gi.boxsize.z > MINZ_COMPARE)
        {
            mm.bp.z -= gi.boxsize.z * .5f;
            mm.ep.z += gi.boxsize.z * .5f;
        }
        // check the starting point of the mast, and if it is planted in the ship, then
        // trim it to the point of contact with the ship
        CVECTOR bv = mastNodePointer->glob_mtx * mm.bp;
        const CVECTOR ev = mastNodePointer->glob_mtx * mm.ep;
        // zero the local matrix
        mastNodePointer->loc_mtx.SetIdentity();
        const float tmpTrace = oldmdl->Trace(ev, bv);
        if (tmpTrace <= 1.f)
        {
            bv = ev + (bv - ev) * tmpTrace;
            mastNodePointer->glob_mtx.MulToInv(bv, mm.bp);
        }

        // set the left and right points of the yard
        for (i = 0; i < mastNodePointer->nnext; i++)
            if (!strncmp(mastNodePointer->next[i]->GetName(), "rey", 3))
            {
                mastNodePointer->next[i]->geo->GetInfo(gi);
                mm.brey = mm.erey = *(CVECTOR *)&gi.boxcenter;
                mm.brey.x -= gi.boxsize.x * .5f;
                mm.erey.x += gi.boxsize.x * .5f;
                if (gi.boxsize.z > MINZ_COMPARE)
                {
                    mm.brey.z -= gi.boxsize.z * .5f;
                    mm.erey.z += gi.boxsize.z * .5f;
                }
                break;
            }
        if (i == mastNodePointer->nnext)
            mm.brey = mm.erey = CVECTOR(0.f, 0.f, 0.f);
        else
        {
            mm.brey = mastNodePointer->next[i]->loc_mtx * mm.brey;
            mm.erey = mastNodePointer->next[i]->loc_mtx * mm.erey;
        }

        bUse = true;
        bFallUnderWater = false;
    }
}

void MAST::LoadIni()
{
    // GUARD(MAST::LoadIni());
    char section[256];

    if (fio->_FileOrDirectoryExists(MAST_INI_FILE))
    {
        ft_old = fio->_GetLastWriteTime(MAST_INI_FILE);
    }
    auto ini = fio->OpenIniFile(MAST_INI_FILE);
    if (!ini)
    {
        throw std::exception("mast.ini file not found!");
    }

    sprintf_s(section, "MAST");

    /*=========================================================================
     =============================================================================
    ======= LOADING PARAMETERS ======================================== ======
    ==============================================================================*/
    // step of movement of the mast when lowering one end into the water
    MAST_MOVE_STEP = ini->GetFloat(section, "water_slide_step", MAST_MOVE_STEP);
    // acceleration of the step of free fall of the entire mast down
    MAST_FALL_STEP = ini->GetFloat(section, "downfall_acceleration", MAST_FALL_STEP);
    // maximum step of the mast falling down
    MAST_MAX_FALL_SPEED = ini->GetFloat(section, "downfall_maxspeed", MAST_MAX_FALL_SPEED);
    // maximum mast swing angle
    MAX_FALL_ANGLE = ini->GetFloat(section, "fMaxAngle", MAX_FALL_ANGLE);
    // step of rotation of the mast around the Y-axis (when the yard collides with an object)
    YROTATE_STEP = ini->GetFloat(section, "fYRotateStep", YROTATE_STEP);
    // the height of the tracing ray on collision
    TRACE_HEIGHT = ini->GetFloat(section, "fTraceHeight", TRACE_HEIGHT);
    // extra addition in height to the point of collision to avoid repeated collisions
    TRACE_ADDING = ini->GetFloat(section, "fTraceHeightAdding", TRACE_ADDING);
    // horizontal mast shift
    TRACE_SLIDING = ini->GetFloat(section, "fTraceSliding", TRACE_SLIDING);
    // maximum coordinate change to switch the mast position shift algorithm
    MAX_CHANGE_LENGTH = ini->GetFloat(section, "fMaxPosChange", MAX_CHANGE_LENGTH);
    // maximum coordinate change when the mast slides
    MAX_SLIDING_LENGHT = ini->GetFloat(section, "fMaxSlideLenght", MAX_SLIDING_LENGHT);
    // the collision line goes below the thickness of the mast
    MAST_WIDTH = ini->GetFloat(section, "fMastWidth", MAST_WIDTH);
    // the minimum Z-value in the NODE size to account for at the endpoints of the tracing lines
    MINZ_COMPARE = ini->GetFloat(section, "fMinZCompare", MINZ_COMPARE);
    // the depth at which the mast turns off
    DESTRUCT_MAST_DEEP = ini->GetFloat(section, "fMaxDeep", DESTRUCT_MAST_DEEP);
    // the step of lowering the mast in depth
    DEEP_FALL_STEP = ini->GetFloat(section, "fDeepStep", DEEP_FALL_STEP);
    // number of motion frames after which collision with objects is disabled
    MAX_MOVE_CICLES = ini->GetLong(section, "maxMoveCicles", MAX_MOVE_CICLES);
    // the number of motion frames after which the fall of the mast is switched on
    MIN_MOV_COUNTER = ini->GetLong(section, "minMoveCicles", MIN_MOV_COUNTER);
    // minimum angle of rotation of the mast in X
    MIN_X_DANG = ini->GetFloat(section, "fMinXdang", MIN_X_DANG);
    // limit of change of the angle of rotation of the mast in X
    VAR_X_DANG = ini->GetFloat(section, "fVarXdang", VAR_X_DANG);
    // minimum angle of rotation of the mast in Z
    MIN_Z_DANG = ini->GetFloat(section, "fMinZdang", MIN_Z_DANG);
    // limit of variation of the angle of rotation of the mast in Z
    VAR_Z_DANG = ini->GetFloat(section, "fVarZdang", VAR_Z_DANG);
    // the angle of the ship position starting from which the mast falls along this angle
    MIN_SIGNIFICANT = ini->GetFloat(section, "fAngSignificant", MIN_SIGNIFICANT);
    // UNGUARD
}

void MAST::doMove(uint32_t DeltaTime)
{
    if (wMoveCounter <= MAX_MOVE_CICLES)
        wMoveCounter++;

    float dtime = DELTA_TIME(static_cast<float>(DeltaTime));
    float rtime = DELTA_TIME_ROTATE(static_cast<float>(DeltaTime));

    auto *mdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(model_id)); // geometry model of the mast
    if (mdl != nullptr)
    {
        if (bFallUnderWater) // if the mast is already sinking
        {
            // reached the depth where we destroy the mast
            if (mdl->mtx.Pos().y < -DESTRUCT_MAST_DEEP)
                bUse = false;
            // move the mast lower into the depths of the sea
            else
                mdl->mtx.Move(0.f, -DEEP_FALL_STEP * dtime, 0.f);
        }
        else // the mast does not sink ... until it falls
        {
            mm.mov += mm.dmov * dtime;
            // If the mast has not tilted horizontally yet, then continue tilting
            if (mm.ang.z < MAX_FALL_ANGLE && mm.ang.z > -MAX_FALL_ANGLE && mm.ang.x < MAX_FALL_ANGLE &&
                mm.ang.x > -MAX_FALL_ANGLE)
                mm.ang += mm.dang * rtime;

            // calculate the global matrix for the new mast position
            CMatrix mtx;
            mtx.BuildMatrix(mm.ang);
            mtx.SetPosition(mm.mov);
            mtx = mdl->GetNode(0)->loc_mtx * mtx;

            CVECTOR bp;                                          // mast top end coordinate
            CVECTOR ep;                                          // coordinate of the lower end of the mast
            CVECTOR lp;                                          // coordinate of the left point of the yard (total)
            CVECTOR rp;                                          // coordinate of the right point of the yard (total)
            bool bNextClass = (wMoveCounter <= MAX_MOVE_CICLES); // continue the collision a certain number of times
            bool bStopRotate = false; // by default do not stop the rotation of the mast when it falls

            bp = mtx * mm.bp;
            ep = mtx * mm.ep;
            lp = mtx * mm.brey;
            rp = mtx * mm.erey;
            // change mast fall speed
            if (bp.y > 0.f && ep.y > 0.f && wMoveCounter > MIN_MOV_COUNTER)
                mm.dmov.y -= MAST_FALL_STEP;
            // falling faster
            if (mm.dmov.y < -MAST_MAX_FALL_SPEED)
                mm.dmov.y = -MAST_MAX_FALL_SPEED;
            // maximum fall rate limitation
            // trace with an approximate consideration of the diameter of the mast
            bp.y -= MAST_WIDTH;
            ep.y -= MAST_WIDTH;
            lp.y -= MAST_WIDTH;
            rp.y -= MAST_WIDTH;
            while (bNextClass)
            {
                bNextClass = false;
                // collision with the island
                entid_t findEI = EntityManager::GetEntityId("ISLAND");
                if (findEI && EntityManager::GetEntityPointer(findEI) != nullptr)
                {
                    auto modEI = static_cast<ISLAND_BASE *>(EntityManager::GetEntityPointer(findEI))->GetModelEID();

                    CVECTOR dp;
                    int tmp;
                    float yAng;
                    if ((tmp = GetSlide(modEI, bp, ep, dp, lp, rp, yAng)) != 0)
                    {
                        if (tmp & SR_STOPROTATE)
                            bStopRotate = true;
                        if (tmp & SR_YROTATE)
                            mdl->GetNode(0)->loc_mtx.RotateY(yAng);
                        if (tmp & SR_MOVE)
                        {
                            mm.mov += dp;
                            mm.dmov += dp;
                            mtx.SetPosition(mm.mov);
                            bp += dp;
                            ep += dp;
                            lp += dp;
                            rp += dp;
                            bNextClass = true;
                            break;
                        }
                    }
                }
                // collision with the ship
                const auto &ships = EntityManager::GetEntityIdVector("ship");
                for (auto ship : ships)
                {
                    if (EntityManager::GetEntityPointer(ship) == nullptr)
                        continue;
                    auto modEI = static_cast<VAI_OBJBASE *>(EntityManager::GetEntityPointer(ship))->GetModelEID();
                    CVECTOR dp;
                    int tmp;
                    float yAng;
                    if ((tmp = GetSlide(modEI, bp, ep, dp, lp, rp, yAng)) != 0)
                    {
                        if (tmp & SR_STOPROTATE)
                            bStopRotate = true;
                        if (tmp & SR_YROTATE)
                            mdl->GetNode(0)->loc_mtx.RotateY(yAng);
                        if (tmp & SR_MOVE)
                        {
                            mm.mov += dp;
                            mm.dmov += dp;
                            mtx.SetPosition(mm.mov);
                            bp += dp;
                            ep += dp;
                            lp += dp;
                            rp += dp;
                            bNextClass = true;
                            break;
                        }
                    }
                }
            }

            if (bp.y <= -MAST_WIDTH || ep.y <= -MAST_WIDTH)
            {
                mm.dang = 0.2f * mm.sdang;
                CVECTOR dm;
                if (bp.y <= 0.f)
                    dm = !(bp - ep);
                else
                    dm = !(ep - bp);
                mm.dmov = dm * MAST_MOVE_STEP;
            }
            else
                mm.dang = mm.sdang;

            if (bStopRotate)
                mm.dang = CVECTOR(0.f, 0.f, 0.f);

            if (bp.y < 0.f && ep.y < 0.f)
                bFallUnderWater = true;
            else
                mdl->mtx = mtx;
        }
    }
}

int MAST::GetSlide(entid_t mod, CVECTOR &pbeg, CVECTOR &pend, CVECTOR &dp, CVECTOR &lrey, CVECTOR &rrey, float &angl)
{
    int retVal = 0;

    // rhea collision
    const CVECTOR vl = lrey;
    const CVECTOR vr = rrey;
    const CVECTOR vcentr = (vl + vr) * .5f;
    float ang = 0.f;
    const float lf = pCollide->Trace(mod, vl, vcentr);
    const float rf = pCollide->Trace(mod, vr, vcentr);

    if ((lf <= 1.f && rf > 1.f) || (lf > 1.f && rf <= 1.f))
    {
        if (lf > 1.f)
            angl = -YROTATE_STEP;
        else
            angl = YROTATE_STEP;
        retVal |= SR_YROTATE;
    }

    // mast collision
    float tmp;
    float hVal = 0.f;
    float sVal = 0.f;
    CVECTOR vb = pbeg;
    CVECTOR ve = pend;
    dp = CVECTOR(0.f, 0.f, 0.f);
    if ((tmp = pCollide->Trace(mod, ve, vb)) <= 1.f)
    {
        retVal |= SR_MOVE;
        if (tmp < 0.5f)
            retVal |= SR_STOPROTATE;
        do
        {
            if (hVal > MAX_CHANGE_LENGTH)
            {
                if (sVal > MAX_SLIDING_LENGHT)
                    return (retVal & (~SR_MOVE));
                dp.y -= hVal;
                vb.y -= hVal;
                ve.y -= hVal;
                hVal = 0;
                auto *pmdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(mod));
                if (pmdl)
                {
                    NODE *pnod = pmdl->GetCollideNode();
                    if (pnod)
                    {
                        CVECTOR cv = vb + (ve - vb) * tmp - pnod->glob_mtx.Pos();
                        cv.y = 0.f;
                        if (cv.x == 0.f && cv.z == 0.f)
                            cv.y = TRACE_SLIDING;
                        else
                            cv = TRACE_SLIDING * (!cv);

                        sVal += TRACE_SLIDING;
                        dp += cv;
                        vb += cv;
                        ve += cv;
                    }
                    else
                        return 0;
                }
                else
                    return 0;
            }
            else
            {
                hVal += TRACE_ADDING;
                dp.y += TRACE_ADDING;
                vb.y += TRACE_ADDING;
                ve.y += TRACE_ADDING;
            }
        } while ((tmp = pCollide->Trace(mod, ve, vb)) <= 1.f);
    }
    else
        return retVal;

    return retVal;
}

void MAST::AllRelease()
{
    if (m_mount_param.pNode)
    {
        Mount(m_mount_param.modelEI, m_mount_param.shipEI, m_mount_param.pNode);
        m_mount_param.pNode = nullptr;
    }

    // delete sail group
    core.Send_Message(EntityManager::GetEntityId("sail"), "li", MSG_SAIL_DEL_GROUP, GetId());

    // remove flag group
    core.Send_Message(EntityManager::GetEntityId("flag"), "li", MSG_FLAG_DEL_GROUP, model_id);

    // announce deleting
    core.Send_Message(ship_id, "lp", MSG_MAST_DELGEOMETRY, m_pMastNode);

    // delete model
    EntityManager::EraseEntity(model_id);
    m_pMastNode = nullptr;
}
