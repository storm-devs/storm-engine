#include "Rope.h"
#include "Entity.h"
#include "core.h"
#include "defines.h"
#include "shared/sail_msg.h"
#include "ship_base.h"

extern void sailPrint(VDX9RENDER *rs, const CVECTOR &pos3D, float rad, long line, const char *format, ...);

ROPE::ROPE()
{
    bUse = false;
    bYesDeleted = false;
    wRopeLast = 0;
    RenderService = nullptr;
    gdata = nullptr;
    groupQuantity = 0;
    rlist = nullptr;
    ropeQuantity = 0;

    TextureName = nullptr;
    texl = -1;
    bFirstRun = true;

    ZERO(mat);
    mat.Diffuse.r = 1.f;
    mat.Diffuse.g = 1.f;
    mat.Diffuse.b = 1.f;
    mat.Ambient.r = 1.f;
    mat.Ambient.g = 1.f;
    mat.Ambient.b = 1.f;

    vBuf = iBuf = -1;
    nVert = nIndx = 0;
}

ROPE::~ROPE()
{
    // clearing and deleting the rope list
    if (rlist)
    {
        for (auto i = 0; i < ropeQuantity; i++)
            STORM_DELETE(rlist[i]);
        STORM_DELETE(rlist);
        ropeQuantity = 0;
    }
    // clearing and deleting the group list
    if (gdata)
    {
        for (auto i = 0; i < groupQuantity; i++)
            STORM_DELETE(gdata[i].ropeIdx);
        STORM_DELETE(gdata);
        groupQuantity = 0;
    }
    // removing textures
    TEXTURE_RELEASE(RenderService, texl);
    STORM_DELETE(TextureName);

    VERTEX_BUFFER_RELEASE(RenderService, vBuf);
    INDEX_BUFFER_RELEASE(RenderService, iBuf);
    nVert = nIndx = 0;
}

bool ROPE::Init()
{
    // GUARD(ROPE::ROPE())
    SetDevice();
    // UNGUARD
    return true;
}

void ROPE::SetDevice()
{
    // get render service
    RenderService = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!RenderService)
    {
        throw std::runtime_error("No service: dx9render");
    }

    LoadIni();

    texl = RenderService->TextureCreate(TextureName);
}

bool ROPE::CreateState(ENTITY_STATE_GEN *state_gen)
{
    return true;
}

bool ROPE::LoadState(ENTITY_STATE *state)
{
    return true;
}

FILETIME ft_old;

void ROPE::Execute(uint32_t Delta_Time)
{
    uint64_t rtm;
    RDTSC_B(rtm);

    if (bFirstRun)
        FirstRun();
    if (bYesDeleted)
        DoSTORM_DELETE();

    if (bUse)
    {
        vertBuf = static_cast<ROPEVERTEX *>(RenderService->LockVertexBuffer(vBuf));
        if (vertBuf)
        {
            const auto dtime = static_cast<float>(Delta_Time) * .02f;
            for (auto i = 0; i < ropeQuantity; i++)
            {
                if (rlist[i]->bUse && !gdata[rlist[i]->HostGroup].bDeleted)
                    SetVertexes(rlist[i], dtime);
                // DoMove(rlist[i]);
                else if (rlist[i]->len != 0.f) // set all vertex to point(0,0,0)
                {
                    const auto nulVect = CVECTOR(0.f, 0.f, 0.f);
                    for (auto idx = rlist[i]->sv; idx < rlist[i]->sv + rlist[i]->nv; idx++)
                        vertBuf[idx].pos = nulVect;
                }
            }

            RenderService->UnLockVertexBuffer(vBuf);
        }
    }

    RDTSC_E(rtm);
    execute_tm = rtm;
}

void ROPE::Realize(uint32_t Delta_Time)
{
    if (bUse)
    {
        uint64_t rtm;

        RDTSC_B(rtm);
        if (Delta_Time == 0)
        {
            // draw mirror rope
        }
        else
        {
            // draw nature rope
            CVECTOR cp, ca;
            float pr;
            RenderService->GetCamera(cp, ca, pr);
            pr = tanf(pr * .5f);

            const auto bDraw = RenderService->TechniqueExecuteStart("ShipRope");

            if (bDraw)
            {
                for (auto i = 0; i < groupQuantity; i++)
                    if (!gdata[i].bDeleted && gdata[i].nt != 0 && nVert != 0)
                        if ((~(gdata[i].pMatWorld->Pos() - cp)) * pr < fMaxRopeDist)
                        // if the distance to the ship is not more than the maximum
                        {
                            static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(gdata[i].shipEI))
                                ->SetLightAndFog(true);
                            RenderService->SetTransform(D3DTS_WORLD, (D3DXMATRIX *)gdata[i].pMatWorld);

                            RenderService->TextureSet(0, texl);
                            RenderService->SetMaterial(mat);
                            RenderService->DrawBuffer(vBuf, sizeof(ROPEVERTEX), iBuf, 0, nVert, gdata[i].st,
                                                      gdata[i].nt);
                            static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(gdata[i].shipEI))
                                ->RestoreLightAndFog();
                        }
                while (RenderService->TechniqueExecuteNext())
                {
                }
            }

            RDTSC_E(rtm);
            realize_tm = rtm;
            // RenderService->Print(0,10,"exec=%d, real=%d",execute_tm,realize_tm);
            /*if(core.Controls->GetDebugAsyncKeyState('M')<0)
              for(int i=0; i<ropeQuantity; i++)
              {
                sailPrint(RenderService,*(rlist[i]->bMatWorld) *
              rlist[i]->pBeg,25.f,0,"B%d_%s%d",rlist[i]->ropeNum,rlist[i]->btie?"tie":"",rlist[i]->bgnum);
                sailPrint(RenderService,*(rlist[i]->eMatWorld) *
              rlist[i]->pEnd,25.f,0,"E%d_%s%d",rlist[i]->ropeNum,rlist[i]->etie?"tie":"",rlist[i]->bgnum);
              }*/
        }
    }
}

uint64_t ROPE::ProcessMessage(MESSAGE &message)
{
    const auto code = message.Long();
    entid_t tmp_id;

    switch (code)
    {
    case MSG_ROPE_INIT: {
        const auto tmp_shipEI = message.EntityID();
        const auto tmp_modelEI = message.EntityID();
        auto *mdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(tmp_modelEI));
        if (mdl == nullptr)
        {
            core.Trace("WARNING!!! Missing INIT message to ROPE - bad ship model");
            return 0;
        }

        const auto wFirstRope = ropeQuantity;
        if (gdata != nullptr)
        {
            auto *const oldgdata = gdata;
            gdata = new GROUPDATA[groupQuantity + 1];
            if (gdata == nullptr)
            {
                throw std::runtime_error("allocate memory error");
            }
            memcpy(gdata, oldgdata, sizeof(GROUPDATA) * groupQuantity);
            delete oldgdata;
            groupQuantity++;
        }
        else
        {
            gdata = new GROUPDATA[1];
            groupQuantity = 1;
            if (gdata == nullptr)
            {
                throw std::runtime_error("allocate memory error");
            }
        }

        ZERO(gdata[groupQuantity - 1]);
        gdata[groupQuantity - 1].shipEI = tmp_shipEI;
        gdata[groupQuantity - 1].modelEI = tmp_modelEI;
        gdata[groupQuantity - 1].pMatWorld = &mdl->mtx;

        NODE *nod;
        GEOS::INFO gi;
        GEOS::LABEL gl;
        int i, j;
        for (j = 0; true; j++)
        {
            nod = mdl->GetNode(j);
            if (!nod)
                break;

            nod->geo->GetInfo(gi);
            for (i = 0; i < gi.nlabels; i++)
            {
                nod->geo->GetLabel(i, gl);
                if (!strncmp(gl.name, "rope", 4))
                    AddLabel(gl, nod, false);
                if (!strncmp(gl.name, "fal", 3))
                    AddLabel(gl, nod, true);
                if (!strncmp(gl.name, "stay", 4)) // ugeen 15.08.16
                    AddLabel(gl, nod, true);
            }
        }

        // there were no new ropes
        if (wFirstRope == ropeQuantity)
        {
            groupQuantity--;
            if (groupQuantity <= 0)
                STORM_DELETE(gdata);
            return 0;
        }

        if (!bFirstRun)
        {
            nIndx *= 3;
            bFirstRun = true;
        }
        SetAdd(wFirstRope);

        gdata[groupQuantity - 1].nt = 0;
        gdata[groupQuantity - 1].nv = 0;
        gdata[groupQuantity - 1].st = nIndx;
        gdata[groupQuantity - 1].sv = nVert;
        gdata[groupQuantity - 1].ropeQuantity = 0;
        for (int rn = wFirstRope; rn < ropeQuantity; rn++)
        {
            if (rlist[rn]->HostGroup == groupQuantity - 1)
            {
                gdata[groupQuantity - 1].ropeQuantity++;
                gdata[groupQuantity - 1].nv += rlist[rn]->nv;
                gdata[groupQuantity - 1].nt += rlist[rn]->nt;
                rlist[rn]->sv = nVert;
                rlist[rn]->st = nIndx;
                nVert += rlist[rn]->nv;
                nIndx += rlist[rn]->nt * 3;
            }
        }
        gdata[groupQuantity - 1].ropeIdx = new int[gdata[groupQuantity - 1].ropeQuantity];
        if (gdata[groupQuantity - 1].ropeIdx == nullptr)
        {
            throw std::runtime_error("allocate memory error");
        }
        int idx = 0;
        for (int rn = wFirstRope; rn < ropeQuantity; rn++)
            if (rlist[rn]->HostGroup == groupQuantity - 1)
                gdata[groupQuantity - 1].ropeIdx[idx++] = rn;
    }
    break;

        // removing rope from display and calculation
    case MSG_ROPE_DELETE: {
        tmp_id = message.EntityID();
        int rope_number;
        rope_number = message.Long();
        for (int i = 0; i < ropeQuantity; i++)
        {
            if (rlist[i]->ropeNum == rope_number && gdata[rlist[i]->HostGroup].modelEI == tmp_id)
            {
                rlist[i]->bDeleted = true;
                bYesDeleted = true;
                // rlist[i]->bUse=false;
                /*entid_t sailEI;
                if(core.FindClass(&sailEI,"sail",0))
                    if(rlist[i]->btie || rlist[i]->etie) // untie the sail from the rope
                        core.Send_Message(sailEI,"ll",MSG_SAIL_ROPE_UNTIE,rope_number);*/
                break;
            }
        }
    }
    break;

    case MSG_ROPE_DEL_GROUP: {
        tmp_id = message.EntityID();
        for (int gn = 0; gn < groupQuantity; gn++)
            if (gdata[gn].modelEI == tmp_id)
            {
                gdata[gn].bDeleted = true;
                bYesDeleted = true;
                break;
            }
    }
    break;
    }

    return 0;
}

void ROPE::SetIndex() const
{
    int i, j;
    int ti, vi;

    auto *pt = static_cast<uint16_t *>(RenderService->LockIndexBuffer(iBuf));
    if (pt)
    {
        for (int rn = 0; rn < ropeQuantity; rn++)
        {
            ti = rlist[rn]->st;
            vi = rlist[rn]->sv;

            // set begin edge point triangles
            for (j = 0; j < ROPE_EDGE; j++)
            {
                pt[ti] = vi;
                pt[ti + 1] = vi + 1 + j;
                if (j < ROPE_EDGE - 1)
                    pt[ti + 2] = vi + 2 + j;
                else
                    pt[ti + 2] = vi + 1;
                ti += 3;
            }
            vi++;

            // set medium triangles
            for (i = 0; i < rlist[rn]->segquant; i++)
            {
                for (j = 0; j < ROPE_EDGE; j++)
                {
                    pt[ti] = vi + j;
                    pt[ti + 1] = pt[ti + 4] = vi + j + ROPE_EDGE;
                    if (j < ROPE_EDGE - 1)
                    {
                        pt[ti + 2] = pt[ti + 3] = vi + j + 1;
                        pt[ti + 5] = vi + j + 1 + ROPE_EDGE;
                    }
                    else
                    {
                        pt[ti + 2] = pt[ti + 3] = vi;
                        pt[ti + 5] = vi + ROPE_EDGE;
                    }
                    ti += 6;
                }
                vi += ROPE_EDGE;
            }

            // set end edge point triangles
            for (j = 0; j < ROPE_EDGE; j++)
            {
                pt[ti] = vi + j;
                pt[ti + 1] = vi + 1;
                if (j < ROPE_EDGE - 1)
                    pt[ti + 2] = vi + j + 1;
                else
                    pt[ti + 2] = vi;
                ti += 3;
            }
        }

        RenderService->UnLockIndexBuffer(iBuf);
    }
}

void ROPE::SetVertexes()
{
    vertBuf = static_cast<ROPEVERTEX *>(RenderService->LockVertexBuffer(vBuf));
    if (vertBuf)
    {
        for (int rn = 0; rn < ropeQuantity; rn++)
            if (rlist[rn]->bUse && !rlist[rn]->bDeleted && !gdata[rlist[rn]->HostGroup].bDeleted)
            {
                SetVertexes(rlist[rn], 0.f);
                SetTextureGrid(rlist[rn]);
            }

        RenderService->UnLockVertexBuffer(vBuf);
    }
}

void ROPE::SetVertexes(ROPEDATA *pr, float dtime) const
{
    // set rope shape parameters
    float deepVal;
    pr->segnum = 0;
    deepVal = pr->ropeWave * (MIN_DEEP_MUL + VAR_DEEP_MUL * sinf(pr->angDeep));
    pr->vDeep.x = deepVal * sinf(VAR_ROTATE_ANGL * sinf(pr->angRot));
    pr->vDeep.y = deepVal;
    pr->vDeep.z = 0.;
    pr->angDeep += DEEP_CHANGE_SPEED * dtime;
    if (pr->angDeep > 2.f * PI)
        pr->angDeep = 0.f;
    pr->angRot += ROTATE_SPEED * dtime;
    if (pr->angRot > 2.f * PI)
        pr->angRot = 0.f;

    int vertnum = pr->sv;

    CVECTOR cvb, cve;
    // Get begin point into Ship coordinate
    gdata[pr->HostGroup].pMatWorld->MulToInv(*pr->bMatWorld * pr->pBeg, cvb);
    // Get end point into Ship coordinate
    gdata[pr->HostGroup].pMatWorld->MulToInv(*pr->eMatWorld * pr->pEnd, cve);

    // Set the first and last points of the rope
    vertBuf[vertnum].pos = cvb;
    vertBuf[vertnum + pr->nv - 1].pos = cve;

    cvb += pr->cv;

    vertnum++;

    const CVECTOR dtV = (cve - cvb - pr->cv) / static_cast<float>(pr->segquant);
    const CVECTOR deepV = pr->vDeep * (4.f / static_cast<float>(pr->segquant * pr->segquant));
    for (int segn = 0; segn <= pr->segquant; segn++)
    {
        CVECTOR cv = cvb;
        if (pr->bMakeWave)
            cv += deepV * static_cast<float>(segn * (pr->segquant - segn));

        // set new coordinates in the current section
        for (int i = 0; i < ROPE_EDGE; i++)
            vertBuf[vertnum + i].pos = cv + pr->pos[i];

        cvb += dtV;
        vertnum += ROPE_EDGE;
    }
}

/*
void ROPE::DoMove(ROPEDATA *pr)
{
    if(pr->bUse)
    {
        int vertnum=pr->sv;
        CVECTOR cvb,cve,cv;
        // Get begin point into Ship coordinate
        gdata[pr->HostGroup].pMatWorld->MulToInv(*pr->bMatWorld*pr->pBeg,cvb);
        // Get end point into Ship coordinate
        gdata[pr->HostGroup].pMatWorld->MulToInv(*pr->eMatWorld*pr->pEnd,cve);

        // Set the first and last points of the rope
        vertBuf[vertnum].pos=cvb;
        vertBuf[vertnum+pr->nv-1].pos=cve;

        cvb += pr->cv;

        CVECTOR chV = pr->pOld - pr->pBeg;
        if( chV.x>MaxCh || chV.x<-MaxCh ||
            chV.y>MaxCh || chV.y<-MaxCh ||
            chV.z>MaxCh || chV.z<-MaxCh )
        {
            vertnum++;
            CVECTOR dc = (cve - cvb - pr->cv)/(float)pr->segquant;
            for(int j=0; j<=pr->segquant; j++)
            {
                // set new coordinates in the current section
                for( int i=0; i<ROPE_EDGE; i++ )
                    vertBuf[vertnum++].pos=cvb+pr->pos[i];
                cvb+=dc;
            }

            pr->pOld=pr->pBeg;
            pr->segnum=pr->segquant;
        }
        else
        {
            // base number of vertices of the current section
            vertnum+=pr->segnum*ROPE_EDGE+1;

            cvb += (cve - cvb - pr->cv)*((float)pr->segnum/(float)pr->segquant);
            cvb += pr->vDeep*((float)(pr->segnum*(pr->segquant - pr->segnum))*4.f/(float)(pr->segquant*pr->segquant));

            // set new coordinates in the current section
            for( int i=0; i<ROPE_EDGE; i++ )
                vertBuf[vertnum+i].pos=cvb+pr->pos[i];

            if( ++(pr->segnum)>pr->segquant )
            {
                float deepVal;
                pr->segnum=0;
                deepVal=pr->ropeWave*(1.f+.7f*sinf(pr->angDeep));
                pr->vDeep.x = deepVal*sinf(.2f*PI*sinf(pr->angRot));
                pr->vDeep.y = deepVal;
                pr->vDeep.z = 0.;
                pr->angDeep += .1f*PI; if( pr->angDeep>2.f*PI ) pr->angDeep=0.f;
                pr->angRot += .03f*PI; if( pr->angRot>2.f*PI ) pr->angRot=0.f;
            }
        }
    }
}
*/
void ROPE::AddLabel(GEOS::LABEL &lbl, NODE *nod, bool bDontSage)
{
    ROPEDATA *rd;
    int ropeNum, grNum;
    int rn;

    if (nod == nullptr)
        return;

    if (bDontSage)
    {
        if (!strncmp(lbl.name, "fal", 3))
        {
            ropeNum = atoi(&lbl.name[4]); // fal
        }
        if (!strncmp(lbl.name, "stay", 4))
        {
            ropeNum = atoi(&lbl.name[5]); // stay
        }
    }
    else
    {
        if (!strncmp(lbl.name, "rope", 4))
        {
            ropeNum = atoi(&lbl.name[5]); // rope
        }
    }
    if (ropeNum == 0)
    {
        NODE *pTmpRootNod = nod;
        while (pTmpRootNod->parent)
            pTmpRootNod = pTmpRootNod->parent;
        core.Trace("Warning! Found rope with number 0: (model = %s) (label = %s)", pTmpRootNod->GetName(), lbl.name);
    }
    if (bDontSage)
        ropeNum += 1000;

    for (rn = 0; rn < ropeQuantity; rn++)
        if (rlist[rn]->ropeNum == ropeNum && rlist[rn]->HostGroup == groupQuantity - 1)
            break;
    if (rn == ropeQuantity) // add a new rope
    {
        // change the list of ropes
        if (rlist == nullptr)
        {
            rlist = new ROPEDATA *[1];
            ropeQuantity = 1;
        }
        else
        {
            ROPEDATA **oldrlist = rlist;
            rlist = new ROPEDATA *[ropeQuantity + 1];
            memcpy(rlist, oldrlist, sizeof(ROPEDATA *) * ropeQuantity);
            delete oldrlist;
            ropeQuantity++;
        }
        rd = rlist[ropeQuantity - 1] = new ROPEDATA;
        PZERO(rd, sizeof(ROPEDATA));
        rd->ropeNum = ropeNum;
        rd->HostGroup = groupQuantity - 1;
        rd->btie = rd->etie = false;
        rd->bMakeWave = !bDontSage;
    }
    else
        rd = rlist[rn];

    if (!strncmp(lbl.group_name, "sail", 4))
        grNum = atoi(&lbl.group_name[5]);
    else
        grNum = -1;

    switch (bDontSage ? lbl.name[3] : lbl.name[4])
    {
    case 'b': // begining tip of rope
        rd->pBeg.x = lbl.m[3][0];
        rd->pBeg.y = lbl.m[3][1];
        rd->pBeg.z = lbl.m[3][2];
        rd->bMatWorld = &nod->glob_mtx; // get host matrix
        rd->bgnum = grNum;
        // binding to sail
        if (grNum >= 0) // begin tip of rope - tie with sail
        {
            rd->pBeg.x += nod->glob_mtx.matrix[3];
            rd->pBeg.y += nod->glob_mtx.matrix[7];
            rd->pBeg.z += nod->glob_mtx.matrix[11];
            rd->btie = true;
        }
        break;
    case 'e': // ending tip of rope
        rd->pEnd.x = lbl.m[3][0];
        rd->pEnd.y = lbl.m[3][1];
        rd->pEnd.z = lbl.m[3][2];
        rd->eMatWorld = &nod->glob_mtx; // get host matrix
        rd->egnum = grNum;
        // binding to sail
        if (grNum >= 0) // begin tip of rope - tie with sail
        {
            rd->pEnd.x += nod->glob_mtx.matrix[3];
            rd->pEnd.y += nod->glob_mtx.matrix[7];
            rd->pEnd.z += nod->glob_mtx.matrix[11];
            rd->etie = true;
        }
        break;
    }

    // now getting all two points
    CVECTOR ce, cb;
    if (rd->bMatWorld != nullptr && rd->eMatWorld != nullptr)
    {
        cb = *rd->bMatWorld * rd->pBeg;
        ce = *rd->eMatWorld * rd->pEnd;
        rd->len = sqrtf(~(ce - cb));

        if (rd->len < .1f || rd->len > 200.f)
        {
            core.Trace("Bad rope length = %f for rope num %d", rd->len, rd->ropeNum);
            delete rd;
            if (ropeQuantity == 1)
            {
                delete rlist;
                rlist = nullptr;
            }
            else
            {
                ROPEDATA **oldrlist = rlist;
                rlist = new ROPEDATA *[ropeQuantity - 1];
                if (rn > 0)
                    memcpy(rlist, oldrlist, sizeof(ROPEDATA *) * rn);
                if (rn < ropeQuantity - 1)
                    memcpy(&rlist[rn], &oldrlist[rn + 1], sizeof(ROPEDATA *) * (ropeQuantity - 1 - rn));
                delete oldrlist;
            }
            ropeQuantity--;
        }
        else
        {
            rd->segquant = static_cast<uint16_t>(rd->len / ROPE_SEG_LENGTH) + 1;
            rd->cv = (!(ce - cb)) * ROPE_END_LENGTH; // vector for rope edge length
            // Set normals with length equal the rope width
            CVECTOR norm;
            float cvert, chorz, svert, shorz;
            norm = ce - cb;
            cvert = norm.z / rd->len;
            svert = norm.y / rd->len;
            const float ftmp = sqrtf(norm.x * norm.x + norm.z * norm.z);
            if (ftmp >= 0.0001f)
            {
                chorz = norm.z / ftmp;
                shorz = norm.x / ftmp;

                float ca, sa;
                for (int i = 0; i < ROPE_EDGE; i++)
                {
                    ca = cosf((float)i / (float)ROPE_EDGE * 2.f * PI);
                    sa = sinf((float)i / (float)ROPE_EDGE * 2.f * PI);
                    // vert & horz
                    if (!strncmp(lbl.name, "stay", 4))
                    {
                        rd->pos[i].x = STAY_WIDTH / 2.f * (sa * chorz + ca * svert * shorz);
                        rd->pos[i].y = STAY_WIDTH / 2.f * ca * cvert;
                        rd->pos[i].z = STAY_WIDTH / 2.f * (ca * svert * chorz - sa * shorz);
                    }
                    else
                    {
                        rd->pos[i].x = ROPE_WIDTH / 2.f * (sa * chorz + ca * svert * shorz);
                        rd->pos[i].y = ROPE_WIDTH / 2.f * ca * cvert;
                        rd->pos[i].z = ROPE_WIDTH / 2.f * (ca * svert * chorz - sa * shorz);
                    }
                }
            }
            else
            {
                for (int i = 0; i < ROPE_EDGE; i++)
                {
                    if (!strncmp(lbl.name, "stay", 4))
                    {
                        // vert & horz
                        rd->pos[i].x = STAY_WIDTH / 2.f * sinf((float)i / (float)ROPE_EDGE * 2.f * PI);
                        rd->pos[i].y = 0.f;
                        rd->pos[i].z = STAY_WIDTH / 2.f * cosf((float)i / (float)ROPE_EDGE * 2.f * PI);
                    }
                    else
                    {
                        // vert & horz
                        rd->pos[i].x = ROPE_WIDTH / 2.f * sinf((float)i / (float)ROPE_EDGE * 2.f * PI);
                        rd->pos[i].y = 0.f;
                        rd->pos[i].z = ROPE_WIDTH / 2.f * cosf((float)i / (float)ROPE_EDGE * 2.f * PI);
                    }
                }
            }

            rd->angDeep = 0.f;
            rd->angRot = 0.f;
            rd->vDeep = 0.f;

            if (const auto sailEI = EntityManager::GetEntityId("sail"))
            {
                auto *mdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(gdata[rd->HostGroup].modelEI));
                if (mdl == nullptr)
                    rd->btie = rd->etie = false;
                else
                    for (int i = 0; i < 10000; i++)
                    {
                        NODE *nd = mdl->GetNode(i);
                        if (nd == nullptr)
                            break;

                        if (rd->btie && rd->bMatWorld == &nd->glob_mtx)
                            core.Send_Message(sailEI, "lplpl", MSG_SAIL_ROPE_TIE, nd, rd->bgnum, &rd->pBeg,
                                              rd->ropeNum);
                        if (rd->etie && rd->eMatWorld == &nd->glob_mtx)
                            core.Send_Message(sailEI, "lplpl", MSG_SAIL_ROPE_TIE, nd, rd->egnum, &rd->pEnd,
                                              -rd->ropeNum);
                    }
            }
            else
            {
                rd->btie = rd->etie = false;
            }
        }
    }
}

// get the end point of the rope in the coordinates of the start point
void ROPE::GetEndPoint(CVECTOR *cv, int ropenum, entid_t mdl_id)
{
    int rn;

    if (cv == nullptr) // bad pointer to vector
        return;

    // find out which end of the rope we need to get
    bool bGetEnd = false;
    if (ropenum < 0)
        ropenum = -ropenum;
    else
        bGetEnd = true;

    // find a rope with a given number
    for (rn = 0; rn < ropeQuantity; rn++)
        if (rlist[rn]->ropeNum == ropenum && gdata[rlist[rn]->HostGroup].modelEI == mdl_id)
            break;

    if (rn == ropeQuantity) // there is no such rope
        return;

    if (bGetEnd)
        rlist[rn]->bMatWorld->MulToInv(*rlist[rn]->eMatWorld * rlist[rn]->pEnd, *cv);
    else
        rlist[rn]->eMatWorld->MulToInv(*rlist[rn]->bMatWorld * rlist[rn]->pBeg, *cv);
}

void ROPE::LoadIni()
{
    // GUARD(ROPE::LoadIni());
    char section[256];
    char param[256];

    auto ini = fio->OpenIniFile("resource\\ini\\rigging.ini");
    if (!ini)
        throw std::runtime_error("rigging.ini file not found!");

    sprintf_s(section, "ROPES");

    // texture name
    ini->ReadString(section, "TextureName", param, sizeof(param) - 1, "sail_rope.tga");
    if (texl != -1)
    {
        if (strcmp(TextureName, param))
            if (RenderService)
            {
                const auto len = strlen(param) + 1;
                delete TextureName;
                TextureName = new char[len];
                memcpy(TextureName, param, len);
                RenderService->TextureRelease(texl);
                texl = RenderService->TextureCreate(TextureName);
            }
    }
    else
    {
        const auto len = strlen(param) + 1;
        TextureName = new char[len];
        memcpy(TextureName, param, len);
    }
    // length of one rope segment
    ROPE_SEG_LENGTH = ini->GetFloat(section, "fSEG_LENGTH", 2.f);
    // rope thickness
    ROPE_WIDTH = ini->GetFloat(section, "fWIDTH", 0.025f);
    // rope (head) thickness
    STAY_WIDTH = ini->GetFloat(section, "fSTAY_WIDTH", 0.12f);
    // the length of the rope (triangle) at the point of connection with the sail in relation to the total length
    ROPE_END_LENGTH = ini->GetFloat(section, "fEND_LENGTH", 0.05f);
    // amplitude of rope vibration in absolute values
    ROPE_WAVE = ini->GetFloat(section, "fWAVE", 0.1f);
    // maximum coordinate change at which the entire rope is recalculated
    MaxCh = ini->GetFloat(section, "fMaxCh", .1f);
    // the length of the rope on which the texture applies
    ROPE_TEX_LEN = ini->GetFloat(section, "fTexHeight", .5f);
    // maximum distance from which the ropes are visible
    fMaxRopeDist = ini->GetFloat(section, "fMaxDist", 5000.f);
    // the rate of change in the depth of the rope deflection
    DEEP_CHANGE_SPEED = ini->GetFloat(section, "fDeepChangeSpeed", 0.15f);
    // rope swing speed
    ROTATE_SPEED = ini->GetFloat(section, "fRotateSpeed", 0.08f);
    // the minimum value of the coefficient of use of the deflection of the rope
    MIN_DEEP_MUL = ini->GetFloat(section, "fMinDeepMul", 1.f);
    // limit of variation of the coefficient of use of rope deflection
    VAR_DEEP_MUL = ini->GetFloat(section, "fVarDeepMul", .7f);
    // rope swing angle
    VAR_ROTATE_ANGL = ini->GetFloat(section, "fRotateAng", .1f);

    // UNGUARD
}

void ROPE::FirstRun()
{
    /*    SetAdd(wRopeLast);
        if(wRopeLast>0)
            nIndx*=3;
        else
        {
            nVert=0; nIndx=0;
        }

        int gn;
        if(wRopeLast<ropeQuantity)
            gn=rlist[wRopeLast]->HostGroup;
        else
            gn=groupQuantity;

        for(; gn<groupQuantity; gn++)
        {
            gdata[gn].nt=0;
            gdata[gn].nv=0;
            gdata[gn].st=nIndx;
            gdata[gn].sv=nVert;
            gdata[gn].ropeQuantity=0;
            for(int rn=wRopeLast; rn<ropeQuantity; rn++)
            {
                if(rlist[rn]->HostGroup==gn)
                {
                    gdata[gn].ropeQuantity++;
                    gdata[gn].nv+=rlist[rn]->nv;
                    gdata[gn].nt+=rlist[rn]->nt;
                    rlist[rn]->sv=nVert;
                    rlist[rn]->st=nIndx;
                    nVert+=rlist[rn]->nv;
                    nIndx+=rlist[rn]->nt*3;
                }
            }
            gdata[gn].ropeIdx = new int[gdata[gn].ropeQuantity];
            int idx=0;
            for(rn=wRopeLast; rn<ropeQuantity; rn++)
                if(rlist[rn]->HostGroup==gn)
                    gdata[gn].ropeIdx[idx++]=rn;
        }*/

    if (nVert && nIndx)
    {
        if (ropeQuantity > wRopeLast)
        {
            if (wRopeLast)
            {
                VERTEX_BUFFER_RELEASE(RenderService, vBuf);
                INDEX_BUFFER_RELEASE(RenderService, iBuf);
            }
            vBuf = RenderService->CreateVertexBuffer(ROPEVERTEX_FORMAT, nVert * sizeof(ROPEVERTEX), D3DUSAGE_WRITEONLY);
            iBuf = RenderService->CreateIndexBuffer(nIndx * 2);

            if (vBuf >= 0 && iBuf >= 0)
            {
                SetVertexes();
                SetIndex();
            }
            else
                core.Trace("Can`t create index or vertex buffer (index = %d, vertex = %d)", nIndx, nVert);
        }
        nIndx /= 3; // translate index quantity to triangle quantity
        bUse = true;
    }

    bFirstRun = false;
    wRopeLast = ropeQuantity;
}

void ROPE::SetTextureGrid(ROPEDATA *pv) const
{
    int iv = pv->sv;

    const float tvMax = sqrtf(~(*pv->eMatWorld * pv->pEnd - *pv->bMatWorld * pv->pBeg));

    vertBuf[iv].tu = 0.f;
    vertBuf[iv].tv = 0.f;

    vertBuf[iv + pv->nv - 1].tu = 1.f;
    vertBuf[iv + pv->nv - 1].tv = tvMax;

    // Set begin segment point and delta point
    iv++;
    float tv = ROPE_END_LENGTH / ROPE_TEX_LEN;
    const float dtv = (tvMax / ROPE_TEX_LEN - tv) / static_cast<float>(pv->segquant);
    for (int j = 0; j <= pv->segquant; j++)
    {
        for (int i = 0; i < ROPE_EDGE; i++)
        {
            if (i <= ROPE_EDGE / 2)
                vertBuf[iv + i].tu = static_cast<float>(i) / static_cast<float>((ROPE_EDGE / 2));
            else
                vertBuf[iv + i].tu =
                    1.f - static_cast<float>(i - ROPE_EDGE / 2) / static_cast<float>((ROPE_EDGE + 1) / 2);
            vertBuf[iv + i].tv = tv;
        }
        iv += ROPE_EDGE;
        tv += dtv;
    }
}

void ROPE::SetAdd(int firstNum)
{
    // set vertex and index buffers
    for (int rn = firstNum; rn < ropeQuantity; rn++)
    {
        // remove bad ropes
        while (rlist[rn]->bMatWorld == nullptr || rlist[rn]->eMatWorld == nullptr)
        {
            const long gn = rlist[rn]->HostGroup;
            const char *pcModlName = nullptr;
            auto *pMdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(gdata[gn].modelEI));
            if (pMdl && pMdl->GetNode(0))
                pcModlName = pMdl->GetNode(0)->GetName();

            core.Trace("Bad rope data for rope: (model=%s) (rope num = %d) (begin group=%d, end group=%d)", pcModlName,
                       rlist[rn]->ropeNum, rlist[rn]->bgnum, rlist[rn]->egnum);
            core.Trace("Begin pointer = %d? end pointer = %d", rlist[rn]->bMatWorld, rlist[rn]->eMatWorld);
            // throw std::runtime_error("Rope error: Not label");
            delete rlist[rn];
            ropeQuantity--;
            if (ropeQuantity)
            {
                ROPEDATA **oldrlist = rlist;
                rlist = new ROPEDATA *[ropeQuantity];
                if (rlist == nullptr)
                    rlist = oldrlist;
                if (rn > 0)
                    memcpy(rlist, oldrlist, sizeof(ROPEDATA *) * rn);
                if (rn < ropeQuantity)
                    memcpy(&rlist[rn], &oldrlist[rn + 1], sizeof(ROPEDATA *) * (ropeQuantity - rn));
                if (oldrlist != rlist)
                    delete oldrlist;
            }
            else
            {
                delete rlist;
                rlist = nullptr;
            }
            if (rn == ropeQuantity)
                break;
        }
        if (rn == ropeQuantity)
            break;

        if (rlist != nullptr)
        {
            rlist[rn]->bUse = true;
            rlist[rn]->nv = (rlist[rn]->segquant + 1) * ROPE_EDGE + 2;
            rlist[rn]->nt = (rlist[rn]->segquant + 1) * ROPE_EDGE * 2;
            rlist[rn]->segnum = 0;
            if (rlist[rn]->segquant <= 2)
                rlist[rn]->ropeWave = -ROPE_WAVE * .2f;
            else
                rlist[rn]->ropeWave = -ROPE_WAVE;
        }
    }

    if (rlist != nullptr)
    {
        for (int rn = firstNum; rn < ropeQuantity; rn++)
        {
            if (rlist[rn]->segquant > 100 || rlist[rn]->segquant < 0)
                rlist[rn]->segquant = rlist[rn]->segquant; //~!~
        }
    }
}

void ROPE::DoSTORM_DELETE()
{
    const uint32_t oldnVert = nVert;

    // walk through deleted groups and mark all ropes belonging to them for deletion
    for (int gn = 0; gn < groupQuantity; gn++)
        if (gdata[gn].bDeleted)
            for (int idx = 0; idx < gdata[gn].ropeQuantity; idx++)
                rlist[gdata[gn].ropeIdx[idx]]->bDeleted = true;

    // remove deleted ropes in the ropes list
    int nrn = 0, rn;
    for (rn = 0; rn < ropeQuantity; rn++)
    {
        if (rlist[rn]->bDeleted)
        {
            // remove rope
            delete rlist[rn];
            continue;
        }

        if (nrn < rn)
            rlist[nrn] = rlist[rn];
        nrn++;
    }
    ropeQuantity = nrn;

    // redo the list of groups
    int ngn = 0, gn;
    nIndx = 0;
    nVert = 0;
    for (gn = 0; gn < groupQuantity; gn++)
    {
        // if the group is deleted, then skip it
        if (gdata[gn].bDeleted)
        {
            delete gdata[gn].ropeIdx;
            continue;
        }

        // correct indexes for vertex and index buffers
        gdata[gn].st = nIndx;
        gdata[gn].sv = nVert;
        gdata[gn].nt = 0;
        gdata[gn].nv = 0;

        // for all ropes belonging to this group, correct the owner's number
        int rq = 0;
        for (rn = 0; rn < ropeQuantity; rn++)
            if (rlist[rn]->HostGroup == gn)
            {
                rlist[rn]->HostGroup = ngn;
                gdata[gn].ropeIdx[rq++] = rn;
                gdata[gn].nt += rlist[rn]->nt;
                gdata[gn].nv += rlist[rn]->nv;
                rlist[rn]->sv = nVert;
                rlist[rn]->st = nIndx;
                nVert += rlist[rn]->nv;
                nIndx += rlist[rn]->nt * 3;
            }

        // if the group has become empty, then delete it physically
        if (rq == 0)
        {
            delete gdata[gn].ropeIdx;
            continue;
        }

        // if the list of ropes in the owner has changed, then change it
        if (rq < gdata[gn].ropeQuantity)
        {
            int *oldropeIdx = gdata[gn].ropeIdx;
            gdata[gn].ropeIdx = new int[rq];
            if (gdata[gn].ropeIdx == nullptr)
                gdata[gn].ropeIdx = oldropeIdx;

            gdata[gn].ropeQuantity = rq;
            memcpy(gdata[gn].ropeIdx, oldropeIdx, sizeof(int) * rq);
            if (gdata[gn].ropeIdx != oldropeIdx)
                delete oldropeIdx;
        }

        if (ngn < gn)
            memcpy(&gdata[ngn], &gdata[gn], sizeof(GROUPDATA));
        ngn++;
    }
    groupQuantity = ngn;

    // change the vertex size and buffer index
    nIndx /= 3;
    if (oldnVert != nVert)
    {
        VERTEX_BUFFER_RELEASE(RenderService, vBuf);
        INDEX_BUFFER_RELEASE(RenderService, iBuf);

        if (nVert > 0)
            vBuf = RenderService->CreateVertexBuffer(ROPEVERTEX_FORMAT, nVert * sizeof(ROPEVERTEX), D3DUSAGE_WRITEONLY);
        if (nIndx > 0)
            iBuf = RenderService->CreateIndexBuffer(nIndx * 6);

        if (nVert > 0 && nIndx > 0)
        {
            SetVertexes();
            SetIndex();
        }
    }

    wRopeLast = ropeQuantity;
    bYesDeleted = false;
    bUse = ropeQuantity > 0;
}

bool ROPE::IsAbsentRope(entid_t mdl_id, int ropenum)
{
    bool retVal = true;

    // find the required group
    int gn;
    for (gn = 0; gn < groupQuantity; gn++)
        if (gdata[gn].modelEI == mdl_id)
            break;

    // go through all the ropes and find the rope we need
    int rn = ropeQuantity;
    if (gn < groupQuantity)
        for (rn = 0; rn < ropeQuantity; rn++)
            if (rlist[rn]->ropeNum == ropenum && rlist[rn]->HostGroup == gn)
                break;

    if (rn < ropeQuantity)
        if ((!rlist[rn]->bDeleted) && (!gdata[gn].bDeleted))
            retVal = false;

    return retVal;
}

void ROPE::DoDeleteUntie(entid_t mdl_id, NODE *rnod, int gNum)
{
    int gn;
    for (gn = 0; gn < groupQuantity; gn++)
        if (gdata[gn].modelEI == mdl_id)
            break;

    if (gn < groupQuantity)
    {
        for (int ri = 0; ri < gdata[gn].ropeQuantity; ri++)
        {
            const int rn = gdata[gn].ropeIdx[ri];
            if (((rlist[rn]->bMatWorld == &rnod->glob_mtx) && (rlist[rn]->bgnum == gNum)) ||
                ((rlist[rn]->eMatWorld == &rnod->glob_mtx) && (rlist[rn]->egnum == gNum)))
            {
                rlist[rn]->bDeleted = true;
                bYesDeleted = true;
            }
        }
    }
}
