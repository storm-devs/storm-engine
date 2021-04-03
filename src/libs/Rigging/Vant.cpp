#include "Vant.h"
#include "../../shared/sail_msg.h"
#include "Entity.h"
#include "core.h"
#include "defines.h"
#include "ship_base.h"
#include "vfile_service.h"

static const char *RIGGING_INI_FILE = "resource\\ini\\rigging.ini";

VANT_BASE::VANT_BASE()
{
    bUse = false;
    RenderService = nullptr;
    TextureName = nullptr;
    texl = -1;
    bRunFirstTime = true;
    bYesDeleted = false;
    wVantLast = 0;
    gdata = nullptr;
    groupQuantity = 0;
    vlist = nullptr;
    vantQuantity = 0;
    vBuf = iBuf = -1;
    nVert = nIndx = 0;
    VantId = 0;
}

VANT_BASE::~VANT_BASE()
{
    TEXTURE_RELEASE(RenderService, texl);
    STORM_DELETE(TextureName);
    while (groupQuantity > 0)
    {
        groupQuantity--;
        STORM_DELETE(gdata[groupQuantity].vantIdx);
    }
    STORM_DELETE(gdata);
    while (vantQuantity > 0)
    {
        vantQuantity--;
        STORM_DELETE(vlist[vantQuantity]);
    }
    STORM_DELETE(vlist);
    VERTEX_BUFFER_RELEASE(RenderService, vBuf);
    INDEX_BUFFER_RELEASE(RenderService, iBuf);
    nVert = nIndx = 0;
}

bool VANT_BASE::Init()
{
    // GUARD(VANT::VANT())
    SetDevice();
    // UNGUARD
    return true;
}

void VANT_BASE::SetDevice()
{
    // get render service
    RenderService = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!RenderService)
    {
        throw std::exception("No service: dx9render");
    }

    LoadIni();

    if (texl == -1)
        texl = RenderService->TextureCreate(TextureName);
}

bool VANT_BASE::CreateState(ENTITY_STATE_GEN *state_gen)
{
    return true;
}

bool VANT_BASE::LoadState(ENTITY_STATE *state)
{
    return true;
}

void VANT_BASE::Execute(uint32_t Delta_Time)
{
    if (bRunFirstTime)
        FirstRun();
    if (bYesDeleted)
        DoSTORM_DELETE();

    if (bUse)
    {
        // ====================================================
        // If the ini-file has been changed, read the info from it
        if (fio->_FileOrDirectoryExists(RIGGING_INI_FILE))
        {
            auto ft_new = fio->_GetLastWriteTime(RIGGING_INI_FILE);
            if (ft_old != ft_new)
            {
                LoadIni();
            }
        }

        doMove();
    }
}

void VANT_BASE::Realize(uint32_t Delta_Time)
{
    if (bUse)
    {
        // _asm rdtsc _asm mov rtm,eax

        RenderService->TextureSet(0, texl);
        uint32_t ambient;
        RenderService->GetRenderState(D3DRS_AMBIENT, &ambient);
        RenderService->SetRenderState(D3DRS_TEXTUREFACTOR, ambient);
        const auto bDraw = RenderService->TechniqueExecuteStart("ShipVant");
        if (!bDraw)
            return;

        // draw nature vants
        CVECTOR cp, ca;
        float pr;
        RenderService->GetCamera(cp, ca, pr);
        pr = tanf(pr * .5f);
        for (auto gn = 0; gn < groupQuantity; gn++)
            if (gdata[gn].nIndx && nVert && (~(gdata[gn].pMatWorld->Pos() - cp)) * pr < fVantMaxDist)
            {
                static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(gdata[gn].shipEI))->SetLightAndFog(true);
                static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(gdata[gn].shipEI))->SetLights();

                RenderService->SetTransform(D3DTS_WORLD, (D3DXMATRIX *)gdata[gn].pMatWorld);
                RenderService->DrawBuffer(vBuf, sizeof(VANTVERTEX), iBuf, 0, nVert, gdata[gn].sIndx, gdata[gn].nIndx);

                static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(gdata[gn].shipEI))->UnSetLights();
                static_cast<SHIP_BASE *>(EntityManager::GetEntityPointer(gdata[gn].shipEI))->RestoreLightAndFog();
                //_asm rdtsc  _asm sub eax,rtm _asm mov rtm,eax
            }
        while (RenderService->TechniqueExecuteNext())
        {
        }
        // RenderService->Print(0,200,"Vants vert=%d, tr=%d, time=%d",nVert,nIndx,rtm);
    }
}

uint64_t VANT_BASE::ProcessMessage(MESSAGE &message)
{
    const auto code = message.Long();

    switch (code)
    {
    case MSG_VANT_INIT: {
        const auto oldvantQuantity = vantQuantity;
        if (gdata == nullptr)
        {
            if ((gdata = new GROUPDATA[1]) == nullptr)
                throw std::exception("Not memory allocation");
            groupQuantity = 1;
        }
        else
        {
            auto *const oldgdata = gdata;
            if ((gdata = new GROUPDATA[groupQuantity + 1]) == nullptr)
                throw std::exception("Not memory allocation");
            memcpy(gdata, oldgdata, sizeof(GROUPDATA) * groupQuantity);
            delete oldgdata;
            groupQuantity++;
        }
        ZERO(gdata[groupQuantity - 1]);
        gdata[groupQuantity - 1].shipEI = message.EntityID();
        gdata[groupQuantity - 1].model_id = message.EntityID();
        MODEL *mdl;
        mdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(gdata[groupQuantity - 1].model_id));
        if (mdl == nullptr)
            throw std::exception("Bad Vant INIT");

        gdata[groupQuantity - 1].pMatWorld = &mdl->mtx;
        NODE *nod;
        GEOS::INFO gi;
        GEOS::LABEL gl;
        int i, j;
        for (j = 0; j < 1000; j++)
        {
            nod = mdl->GetNode(j);
            if (!nod)
                break;

            nod->geo->GetInfo(gi);
            for (i = 0; i < gi.nlabels; i++)
            {
                nod->geo->GetLabel(i, gl);
                if (VantId == 0)
                {
                    if (!strncmp(gl.name, "vant", 4))
                        AddLabel(gl, nod);
                }
                if (VantId == 1)
                {
                    if (!strncmp(gl.name, "vanx", 4))
                        AddLabel(gl, nod);
                }
                if (VantId == 2)
                {
                    if (!strncmp(gl.name, "vanz", 4))
                        AddLabel(gl, nod);
                }
            }
        }

        if (vantQuantity == oldvantQuantity) // there were no shrouds - delete the whole group
        {
            if (groupQuantity == 1)
            {
                delete gdata;
                gdata = nullptr;
                groupQuantity = 0;
            }
            else
            {
                groupQuantity--;
                auto *const oldgdata = gdata;
                gdata = new GROUPDATA[groupQuantity];
                if (gdata == nullptr)
                    gdata = oldgdata;
                else
                {
                    memcpy(gdata, oldgdata, sizeof(GROUPDATA) * groupQuantity);
                    delete oldgdata;
                }
            }
            return 0;
        }

        bRunFirstTime = true;
        SetAdd(oldvantQuantity);

        nIndx *= 3;
        //            if(gdata[groupQuantity-1].vantIdx) delete gdata[groupQuantity-1].vantIdx;
        gdata[groupQuantity - 1].sIndx = nIndx;
        gdata[groupQuantity - 1].nIndx = 0;
        gdata[groupQuantity - 1].sVert = nVert;
        gdata[groupQuantity - 1].nVert = 0;

        gdata[groupQuantity - 1].vantQuantity = vantQuantity - oldvantQuantity;
        gdata[groupQuantity - 1].vantIdx = new int[vantQuantity - oldvantQuantity];
        if (gdata[groupQuantity - 1].vantIdx == nullptr)
        {
            throw std::exception("allocate memory error");
        }

        auto idx = 0;
        for (int vn = oldvantQuantity; vn < vantQuantity; vn++)
        {
            gdata[groupQuantity - 1].vantIdx[idx++] = vn;
            vlist[vn]->sv = nVert;
            vlist[vn]->st = nIndx;
            gdata[groupQuantity - 1].nIndx += vlist[vn]->nt;
            gdata[groupQuantity - 1].nVert += vlist[vn]->nv;
            nVert += vlist[vn]->nv;
            nIndx += vlist[vn]->nt * 3;
        }

        nIndx /= 3;
    }
    break;

    case MSG_VANT_DEL_GROUP: {
        const entid_t tmp_id = message.EntityID();
        for (int i = 0; i < groupQuantity; i++)
            if (gdata[i].model_id == tmp_id)
            {
                gdata[i].bDeleted = true;
                bYesDeleted = true;
                break;
            }
    }
    break;

    case MSG_VANT_DEL_MAST: {
        const entid_t tmp_id = message.EntityID();
        NODE *mastNode = (NODE *)message.Pointer();
        if (mastNode == nullptr)
            break;
        for (int i = 0; i < groupQuantity; i++)
            if (gdata[i].model_id == tmp_id)
            {
                for (int j = 0; j < gdata[i].vantQuantity; j++)
                    if (&mastNode->glob_mtx == vlist[gdata[i].vantIdx[j]]->pUpMatWorld)
                    {
                        vlist[gdata[i].vantIdx[j]]->bDeleted = true;
                        bYesDeleted = true;
                    }
                break;
            }
    }
    break;
    }

    return 0;
}

void VANT_BASE::SetIndex() const
{
    int i, j;
    int ti, vi;

    uint16_t *pt = static_cast<uint16_t *>(RenderService->LockIndexBuffer(iBuf));
    if (pt)
    {
        for (int vn = 0; vn < vantQuantity; vn++)
        {
            ti = vlist[vn]->st;
            vi = vlist[vn]->sv;

            // set center treangle & square
            pt[ti++] = vi;
            pt[ti++] = vi + 2;
            pt[ti++] = vi + 1;
            pt[ti++] = vi + 3;
            pt[ti++] = vi + 4;
            pt[ti++] = vi + 5;
            pt[ti++] = vi + 4;
            pt[ti++] = vi + 6;
            pt[ti++] = vi + 5;
            vi += 7;

            // set balk treangles
            pt[ti++] = vi;
            pt[ti++] = vi + 1;
            pt[ti++] = vi + 2;
            pt[ti++] = vi + 3;
            pt[ti++] = vi + 4;
            pt[ti++] = vi + 5;
            pt[ti++] = vi;
            pt[ti++] = vi + 3;
            pt[ti++] = vi + 4;
            pt[ti++] = vi;
            pt[ti++] = vi + 4;
            pt[ti++] = vi + 1;
            pt[ti++] = vi;
            pt[ti++] = vi + 2;
            pt[ti++] = vi + 3;
            pt[ti++] = vi + 3;
            pt[ti++] = vi + 2;
            pt[ti++] = vi + 5;
            pt[ti++] = vi + 2;
            pt[ti++] = vi + 4;
            pt[ti++] = vi + 5;
            pt[ti++] = vi + 1;
            pt[ti++] = vi + 4;
            pt[ti++] = vi + 2;
            vi += 6;

            int dIdx = vi + (VANT_EDGE + 1) * ROPE_QUANT;
            // set ropes treangles
            for (i = 0; i < ROPE_QUANT; i++)
            {
                for (j = 0; j < VANT_EDGE; j++)
                {
                    pt[ti] = pt[ti + 3] = vi + j;
                    pt[ti + 2] = dIdx + j;
                    //                    if(j<(VANT_EDGE-1))
                    //                    {
                    pt[ti + 4] = vi + j + 1;
                    pt[ti + 1] = pt[ti + 5] = dIdx + j + 1;
                    //                    }
                    /*                    else
                                        {
                                            pt[ti+4]=vi;
                                            pt[ti+1]=pt[ti+5]=dIdx;
                                        }*/
                    ti += 6;
                }
                dIdx += VANT_EDGE + 1;
                vi += VANT_EDGE + 1;
            }
        }

        RenderService->UnLockIndexBuffer(iBuf);
    }
}

void VANT_BASE::SetVertexes() const
{
    int j, i;
    uint32_t iv;
    CVECTOR uPos, lPos, rPos;

    auto *pv = static_cast<VANTVERTEX *>(RenderService->LockVertexBuffer(vBuf));
    if (pv)
    {
        for (int vn = 0; vn < vantQuantity; vn++)
        {
            if (gdata[vlist[vn]->HostGroup].bDeleted)
                continue;
            iv = vlist[vn]->sv;

            gdata[vlist[vn]->HostGroup].pMatWorld->MulToInv((*vlist[vn]->pUpMatWorld) * vlist[vn]->pUp, uPos);
            gdata[vlist[vn]->HostGroup].pMatWorld->MulToInv((*vlist[vn]->pDownMatWorld) * vlist[vn]->pLeft, lPos);
            gdata[vlist[vn]->HostGroup].pMatWorld->MulToInv((*vlist[vn]->pDownMatWorld) * vlist[vn]->pRight, rPos);

            // Set last parameters
            vlist[vn]->pLeftStart = vlist[vn]->pLeftOld = lPos;
            vlist[vn]->pUpStart = vlist[vn]->pUpOld = uPos;

            CVECTOR horzDirect = !(rPos - lPos);
            CVECTOR vertDirect = !((rPos + lPos) * .5f - uPos);
            // Set angles point
            pv[iv].pos = uPos;
            pv[iv + 3].pos = pv[iv + 1].pos =
                uPos - horzDirect * (upWidth * .5f) + vertDirect * upHeight * (1.f - fBalkHeight);
            pv[iv + 4].pos = pv[iv + 2].pos =
                uPos + horzDirect * (upWidth * .5f) + vertDirect * upHeight * (1.f - fBalkHeight);
            pv[iv + 5].pos = lPos;
            pv[iv + 6].pos = rPos;
            //
            pv[iv].tu = (treangXl + treangXr) * .5f;
            pv[iv].tv = treangYu;
            pv[iv + 1].tu = treangXl;
            pv[iv + 1].tv = treangYd;
            pv[iv + 2].tu = treangXr;
            pv[iv + 2].tv = treangYd;
            //
            const float fh = sqrtf(~((rPos + lPos) * .5f - uPos));
            auto ftmp = static_cast<float>(static_cast<int>(fh / hRopeHeight + .5f));
            pv[iv + 3].tu = ropeXl;
            pv[iv + 3].tv = 0.f;
            pv[iv + 4].tu = ropeXr;
            pv[iv + 4].tv = 0.f;
            pv[iv + 5].tu = ropeXl;
            pv[iv + 5].tv = ftmp;
            pv[iv + 6].tu = ropeXr;
            pv[iv + 6].tv = ftmp;
            iv += 7;

            // set beam points
            CVECTOR tvec = uPos - horzDirect * (upWidth * .5f) + vertDirect * upHeight;
            pv[iv].pos = tvec - vertDirect * upHeight * fBalkHeight;
            pv[iv + 1].pos = tvec + vlist[vn]->pos[0] * fBalkWidth;
            pv[iv + 2].pos = tvec + vlist[vn]->pos[VANT_EDGE / 2] * fBalkWidth;
            tvec += horzDirect * upWidth;
            pv[iv + 3].pos = tvec - vertDirect * upHeight * fBalkHeight;
            pv[iv + 4].pos = tvec + vlist[vn]->pos[0] * fBalkWidth;
            pv[iv + 5].pos = tvec + vlist[vn]->pos[VANT_EDGE / 2] * fBalkWidth;
            //
            pv[iv].tu = pv[iv + 1].tu = pv[iv + 2].tu = treangXl;
            pv[iv + 3].tu = pv[iv + 4].tu = pv[iv + 5].tu = treangXr;
            pv[iv].tv = pv[iv + 3].tv = balkYu;
            pv[iv + 1].tv = pv[iv + 2].tv = pv[iv + 4].tv = pv[iv + 5].tv = balkYd;
            iv += 6;

            // Set up ropes points
            CVECTOR sp = uPos - horzDirect * (.5f * upWidth) + vertDirect * upHeight;
            CVECTOR dp = horzDirect * (upWidth / static_cast<float>(ROPE_QUANT - 1));
            const float dtmp = (vRopeXr - vRopeXl) / static_cast<float>(VANT_EDGE);
            for (i = 0; i < ROPE_QUANT; i++)
            {
                for (j = 0; j <= VANT_EDGE; j++)
                {
                    if (j == VANT_EDGE)
                        pv[iv + j].pos = sp + vlist[vn]->pos[0];
                    else
                        pv[iv + j].pos = sp + vlist[vn]->pos[j];
                    pv[iv + j].tu = vRopeXl + dtmp * static_cast<float>(j);
                    pv[iv + j].tv = 0.f;
                }
                iv += VANT_EDGE + 1;
                sp += dp;
            }

            // Set down ropes points
            sp = lPos;
            dp = (rPos - lPos) / static_cast<float>(ROPE_QUANT - 1);
            ftmp = fh / vRopeHeight;
            for (i = 0; i < ROPE_QUANT; i++)
            {
                for (j = 0; j <= VANT_EDGE; j++)
                {
                    if (j == VANT_EDGE)
                        pv[iv + j].pos = sp + vlist[vn]->pos[0];
                    else
                        pv[iv + j].pos = sp + vlist[vn]->pos[j];
                    pv[iv + j].tu = vRopeXl + dtmp * static_cast<float>(j);
                    pv[iv + j].tv = ftmp;
                }
                iv += VANT_EDGE + 1;
                sp += dp;
            }
        }

        RenderService->UnLockVertexBuffer(vBuf);
    }
}

void VANT_BASE::AddLabel(GEOS::LABEL &lbl, NODE *nod)
{
    VANTDATA *vd;
    int vantNum;

    if (nod == nullptr)
        return;

    vantNum = atoi(&lbl.name[4]);

    int vn;
    for (vn = 0; vn < vantQuantity; vn++)
        if ((vlist[vn]->HostGroup == groupQuantity - 1) && (vlist[vn]->vantNum == vantNum))
        {
            vd = vlist[vn];
            break;
        }
    if (vn == vantQuantity)
    {
        // create a new guy
        vd = new VANTDATA;
        if (vd == nullptr)
            throw std::exception("Not memory allocate");
        PZERO(vd, sizeof(VANTDATA));
        vd->bDeleted = false;
        vd->vantNum = vantNum;
        vd->pUpMatWorld = vd->pDownMatWorld = nullptr;
        vd->HostGroup = groupQuantity - 1;

        if (vantQuantity == 0)
        {
            vlist = new VANTDATA *[1];
            vantQuantity = 1;
        }
        else
        {
            VANTDATA **oldvlist = vlist;
            vlist = new VANTDATA *[vantQuantity + 1];
            if (vlist == nullptr)
                throw std::exception("Not memory allocate");
            memcpy(vlist, oldvlist, sizeof(VANTDATA *) * vantQuantity);
            delete oldvlist;
            vantQuantity++;
        }

        vlist[vantQuantity - 1] = vd;
    }

    switch (lbl.name[5])
    {
    case 'u': // up edge of vant
        vd->pUp = CVECTOR(lbl.m[3][0], lbl.m[3][1], lbl.m[3][2]) - gdata[groupQuantity - 1].pMatWorld->Pos();
        // + nod->glob_mtx.Pos();
        vd->pUpMatWorld = &nod->glob_mtx; // get host matrix
        break;
    case 'l': // left edge of vant
        vd->pLeft = CVECTOR(lbl.m[3][0], lbl.m[3][1], lbl.m[3][2]) - gdata[groupQuantity - 1].pMatWorld->Pos();
        // + nod->glob_mtx.Pos();
        if (vd->pDownMatWorld == nullptr)
            vd->pDownMatWorld = &nod->glob_mtx; // get host matrix
        else if (vd->pDownMatWorld != &nod->glob_mtx)
            vd->pDownMatWorld->MulToInv(nod->glob_mtx * vd->pLeft, vd->pLeft);
        break;
    case 'r': // right edge of vant
        vd->pRight = CVECTOR(lbl.m[3][0], lbl.m[3][1], lbl.m[3][2]) - gdata[groupQuantity - 1].pMatWorld->Pos();
        // + nod->glob_mtx.Pos();
        if (vd->pDownMatWorld == nullptr)
            vd->pDownMatWorld = &nod->glob_mtx; // get host matrix
        else if (vd->pDownMatWorld != &nod->glob_mtx)
            vd->pDownMatWorld->MulToInv(nod->glob_mtx * vd->pLeft, vd->pLeft);
        break;
    }
}

void VANT_BASE::SetAll()
{
    // set vertex and index buffers
    for (int vn = 0; vn < vantQuantity; vn++)
    {
        while (true)
        {
            if (!gdata[vlist[vn]->HostGroup].bDeleted)
                if (vlist[vn]->pUpMatWorld && vlist[vn]->pDownMatWorld)
                    break;

            delete vlist[vn];
            vantQuantity--;
            if (vantQuantity > 0)
            {
                VANTDATA **oldvlist = vlist;
                vlist = new VANTDATA *[vantQuantity];
                if (vlist)
                {
                    if (vn > 0)
                        memcpy(vlist, oldvlist, sizeof(VANTDATA *) * vn);
                    if (vn < vantQuantity)
                        memcpy(&vlist[vn], &oldvlist[vn + 1], sizeof(VANTDATA *) * (vantQuantity - vn));
                    delete oldvlist;
                }
                else
                    vlist = oldvlist;
            }
            else
            {
                delete vlist;
                vlist = nullptr;
            }
            if (vn == vantQuantity)
                break;
        }
        if (vn == vantQuantity)
            break;

        // Set normals with length equal the rope width
        CMatrix tmat;
        tmat.BuildViewMatrix(.5f * (vlist[vn]->pLeft + vlist[vn]->pRight), vlist[vn]->pUp, CVECTOR(0.f, 0.f, 1.f));
        float ca, sa;
        for (int i = 0; i < VANT_EDGE; i++)
        {
            ca = cosf(static_cast<float>(i) / static_cast<float>(VANT_EDGE) * 2.f * PI);
            sa = sinf(static_cast<float>(i) / static_cast<float>(VANT_EDGE) * 2.f * PI);
            // tmat.MulToInvNorm(CVECTOR(ROPE_WIDTH/2.f*ca,ROPE_WIDTH/2.f*sa,0.f),vlist[vn]->pos[i]);
            vlist[vn]->pos[i] = CVECTOR(ROPE_WIDTH / 2.f * ca, 0.f, ROPE_WIDTH / 2.f * sa);
        }

        vlist[vn]->nv = (VANT_EDGE + 1) * ROPE_QUANT * 2 + 7 + 6;
        vlist[vn]->nt = VANT_EDGE * ROPE_QUANT * 2 + 3 + 8;
    }
}

void VANT::LoadIni()
{
    // GUARD(VANT::LoadIni());
    char section[256];
    char param[256];

    if (fio->_FileOrDirectoryExists(RIGGING_INI_FILE))
    {
        ft_old = fio->_GetLastWriteTime(RIGGING_INI_FILE);
    }
    auto ini = fio->OpenIniFile("resource\\ini\\rigging.ini");
    if (!ini)
    {
        throw std::exception("rigging.ini file not found!");
    }

    sprintf_s(section, "VANTS");

    // texture name
    ini->ReadString(section, "TextureName", param, sizeof(param) - 1, "vant.tga");
    if (texl != -1)
    {
        if (strcmp(TextureName, param))
            if (RenderService)
            {
                delete TextureName;
                const auto len = strlen(param) + 1;
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
    // rope thickness
    ROPE_WIDTH = ini->GetFloat(section, "fWidth", 0.1f);
    // number of ropes
    ROPE_QUANT = static_cast<int>(ini->GetLong(section, "fRopeQuant", 5));
    if (ROPE_QUANT < 2)
        ROPE_QUANT = 2;
    // xBeg horizontal rope texture coordinates
    ropeXl = ini->GetFloat(section, "fHRopeXbeg", 0.5f);
    ropeXr = ini->GetFloat(section, "fHRopeXend", 1.f);
    // triangle texture coordinates
    treangXl = ini->GetFloat(section, "fTreangXbeg", 0.f);
    treangXr = ini->GetFloat(section, "fTreangXend", 0.5f);
    treangYu = ini->GetFloat(section, "fTreangYbeg", 0.f);
    treangYd = ini->GetFloat(section, "fTreangYend", 1.f);
    // beam texture coordinates
    balkYu = ini->GetFloat(section, "fBalkYbeg", 0.6f);
    balkYd = ini->GetFloat(section, "fBalkYend", 1.f);
    // vertical rope texture coordinates
    vRopeXl = ini->GetFloat(section, "fVRopeXbeg", 0.f);
    vRopeXr = ini->GetFloat(section, "fVRopeXend", 0.1f);
    // upper triangle width
    upWidth = ini->GetFloat(section, "fTreangWidth", 1.f);
    // the height of the upper triangle
    upHeight = ini->GetFloat(section, "fTreangHeight", 1.f);
    // vertical rope height
    vRopeHeight = ini->GetFloat(section, "fVRopeHeight", 1.f);
    // horizontal rope height
    hRopeHeight = ini->GetFloat(section, "fHRopeHeight", 1.f);
    // beam height relative to triangle height
    fBalkHeight = ini->GetFloat(section, "fBalkHeight", 0.1f);
    fBalkWidth = ini->GetFloat(section, "fBalkWidth", 1.2f);
    // the square of the distance from which the cables are not visible
    fVantMaxDist = ini->GetFloat(section, "fVantMaxDist", 10000.f);
    // Guy motion sampling step
    ZERO_CMP_VAL = ini->GetFloat(section, "fDiscrValue", 0.01f);
    // the maximum change in the guy position at which the guy stops being displayed
    MAXFALL_CMP_VAL = ini->GetFloat(section, "fDisapearValue", 5.f);

    VantId = 0;
    // UNGUARD
}

void VANTL::LoadIni()
{
    // GUARD(VANT::LoadIni());
    char section[256];
    char param[256];

    if (fio->_FileOrDirectoryExists(RIGGING_INI_FILE))
    {
        ft_old = fio->_GetLastWriteTime(RIGGING_INI_FILE);
    }
    auto ini = fio->OpenIniFile("resource\\ini\\rigging.ini");
    if (!ini)
    {
        throw std::exception("rigging.ini file not found!");
    }

    sprintf(section, "VANTS_L");

    // texture name
    ini->ReadString(section, "TextureName", param, sizeof(param) - 1, "vant.tga");
    if (texl != -1)
    {
        if (strcmp(TextureName, param))
            if (RenderService)
            {
                delete TextureName;
                const auto len = strlen(param) + 1;
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
    // rope thickness
    ROPE_WIDTH = ini->GetFloat(section, "fWidth", 0.1f);
    // number of ropes
    ROPE_QUANT = (int)ini->GetLong(section, "fRopeQuant", 5);
    if (ROPE_QUANT < 2)
        ROPE_QUANT = 2;
    // xBeg horizontal rope texture coordinates
    ropeXl = ini->GetFloat(section, "fHRopeXbeg", 0.5f);
    ropeXr = ini->GetFloat(section, "fHRopeXend", 1.f);
    // triangle texture coordinates
    treangXl = ini->GetFloat(section, "fTreangXbeg", 0.f);
    treangXr = ini->GetFloat(section, "fTreangXend", 0.5f);
    treangYu = ini->GetFloat(section, "fTreangYbeg", 0.f);
    treangYd = ini->GetFloat(section, "fTreangYend", 1.f);
    // beam texture coordinates
    balkYu = ini->GetFloat(section, "fBalkYbeg", 0.6f);
    balkYd = ini->GetFloat(section, "fBalkYend", 1.f);
    // vertical rope texture coordinates
    vRopeXl = ini->GetFloat(section, "fVRopeXbeg", 0.f);
    vRopeXr = ini->GetFloat(section, "fVRopeXend", 0.1f);
    // upper triangle width
    upWidth = ini->GetFloat(section, "fTreangWidth", 1.f);
    // the height of the upper triangle
    upHeight = ini->GetFloat(section, "fTreangHeight", 1.f);
    // vertical rope height
    vRopeHeight = ini->GetFloat(section, "fVRopeHeight", 1.f);
    // horizontal rope height
    hRopeHeight = ini->GetFloat(section, "fHRopeHeight", 1.f);
    // beam height relative to triangle height
    fBalkHeight = ini->GetFloat(section, "fBalkHeight", 0.1f);
    fBalkWidth = ini->GetFloat(section, "fBalkWidth", 1.2f);
    // the square of the distance from which the cables are not visible
    fVantMaxDist = ini->GetFloat(section, "fVantMaxDist", 10000.f);
    // Guy motion sampling step
    ZERO_CMP_VAL = ini->GetFloat(section, "fDiscrValue", 0.01f);
    // the maximum change in the guy position at which the guy stops being displayed
    MAXFALL_CMP_VAL = ini->GetFloat(section, "fDisapearValue", 5.f);

    VantId = 1;
    // UNGUARD
}

void VANTZ::LoadIni()
{
    // GUARD(VANT::LoadIni());
    char section[256];
    char param[256];

    if (fio->_FileOrDirectoryExists(RIGGING_INI_FILE))
    {
        ft_old = fio->_GetLastWriteTime(RIGGING_INI_FILE);
    }
    auto ini = fio->OpenIniFile("resource\\ini\\rigging.ini");
    if (!ini)
    {
        throw std::exception("rigging.ini file not found!");
    }

    sprintf(section, "VANTS_Z");

    // texture name
    ini->ReadString(section, "TextureName", param, sizeof(param) - 1, "vant.tga");
    if (texl != -1)
    {
        if (strcmp(TextureName, param))
            if (RenderService)
            {
                delete TextureName;
                const auto len = strlen(param) + 1;
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
    // rope thickness
    ROPE_WIDTH = ini->GetFloat(section, "fWidth", 0.1f);
    // number of ropes
    ROPE_QUANT = (int)ini->GetLong(section, "fRopeQuant", 5);
    if (ROPE_QUANT < 2)
        ROPE_QUANT = 2;
    // xBeg horizontal rope texture coordinates
    ropeXl = ini->GetFloat(section, "fHRopeXbeg", 0.5f);
    ropeXr = ini->GetFloat(section, "fHRopeXend", 1.f);
    // triangle texture coordinates
    treangXl = ini->GetFloat(section, "fTreangXbeg", 0.f);
    treangXr = ini->GetFloat(section, "fTreangXend", 0.5f);
    treangYu = ini->GetFloat(section, "fTreangYbeg", 0.f);
    treangYd = ini->GetFloat(section, "fTreangYend", 1.f);
    // beam texture coordinates
    balkYu = ini->GetFloat(section, "fBalkYbeg", 0.6f);
    balkYd = ini->GetFloat(section, "fBalkYend", 1.f);
    // vertical rope texture coordinates
    vRopeXl = ini->GetFloat(section, "fVRopeXbeg", 0.f);
    vRopeXr = ini->GetFloat(section, "fVRopeXend", 0.1f);
    // upper triangle width
    upWidth = ini->GetFloat(section, "fTreangWidth", 1.f);
    // the height of the upper triangle
    upHeight = ini->GetFloat(section, "fTreangHeight", 1.f);
    // vertical rope height
    vRopeHeight = ini->GetFloat(section, "fVRopeHeight", 1.f);
    // horizontal rope height
    hRopeHeight = ini->GetFloat(section, "fHRopeHeight", 1.f);
    // beam height relative to triangle height
    fBalkHeight = ini->GetFloat(section, "fBalkHeight", 0.1f);
    fBalkWidth = ini->GetFloat(section, "fBalkWidth", 1.2f);
    // the square of the distance from which the cables are not visible
    fVantMaxDist = ini->GetFloat(section, "fVantMaxDist", 10000.f);
    // Guy motion sampling step
    ZERO_CMP_VAL = ini->GetFloat(section, "fDiscrValue", 0.01f);
    // the maximum change in the guy position at which the guy stops being displayed
    MAXFALL_CMP_VAL = ini->GetFloat(section, "fDisapearValue", 5.f);

    VantId = 2;
    // UNGUARD
}

void VANT_BASE::doMove()
{
    int j, i;
    uint32_t iv;
    CVECTOR uPos, lPos, rPos;

    auto *pv = static_cast<VANTVERTEX *>(RenderService->LockVertexBuffer(vBuf));
    if (pv)
    {
        for (int vn = 0; vn < vantQuantity; vn++)
        {
            if (gdata[vlist[vn]->HostGroup].bDeleted || vlist[vn]->bDeleted)
            {
                bYesDeleted = true;
                continue;
            }
            CVECTOR vtmp, htmp;
            gdata[vlist[vn]->HostGroup].pMatWorld->MulToInv(*vlist[vn]->pUpMatWorld * vlist[vn]->pUp, uPos);
            gdata[vlist[vn]->HostGroup].pMatWorld->MulToInv(*vlist[vn]->pDownMatWorld * vlist[vn]->pLeft, lPos);
            gdata[vlist[vn]->HostGroup].pMatWorld->MulToInv(*vlist[vn]->pDownMatWorld * vlist[vn]->pRight, rPos);

            if (!VectCmp(lPos, vlist[vn]->pLeftStart, MAXFALL_CMP_VAL) ||
                !VectCmp(uPos, vlist[vn]->pUpStart, MAXFALL_CMP_VAL))
            {
                vlist[vn]->bDeleted = true; // set the sign of guy removal
                bYesDeleted = true;
            }

            if (!VectCmp(lPos, vlist[vn]->pLeftOld, ZERO_CMP_VAL) || !VectCmp(uPos, vlist[vn]->pUpOld, ZERO_CMP_VAL))
            {
                // Set last parameters
                vlist[vn]->pLeftOld = lPos;
                vlist[vn]->pUpOld = uPos;

                CVECTOR horzDirect = !(rPos - lPos);
                CVECTOR vertDirect = !((rPos + lPos) * .5f - uPos);

                iv = vlist[vn]->sv;

                // Set angles point
                pv[iv].pos = uPos;
                htmp = horzDirect * (upWidth * .5f);
                vtmp = vertDirect * upHeight * (1.f - fBalkHeight);
                pv[iv + 3].pos = pv[iv + 1].pos = uPos - htmp + vtmp;
                pv[iv + 4].pos = pv[iv + 2].pos = uPos + htmp + vtmp;
                pv[iv + 5].pos = lPos;
                pv[iv + 6].pos = rPos;
                iv += 7;

                // set beam points
                CVECTOR tvec = uPos - htmp + vertDirect * upHeight;
                pv[iv].pos = uPos - htmp + vtmp;
                pv[iv + 1].pos = tvec + vlist[vn]->pos[0] * fBalkWidth;
                pv[iv + 2].pos = tvec + vlist[vn]->pos[VANT_EDGE / 2] * fBalkWidth;
                tvec += horzDirect * upWidth;
                pv[iv + 3].pos = uPos + htmp + vtmp;
                pv[iv + 4].pos = tvec + vlist[vn]->pos[0] * fBalkWidth;
                pv[iv + 5].pos = tvec + vlist[vn]->pos[VANT_EDGE / 2] * fBalkWidth;
                iv += 6;

                // Set up ropes points
                CVECTOR sp = uPos - horzDirect * (.5f * upWidth) + vertDirect * upHeight;
                CVECTOR dp = horzDirect * (upWidth / static_cast<float>(ROPE_QUANT - 1));
                for (i = 0; i < ROPE_QUANT; i++)
                {
                    for (j = 0; j <= VANT_EDGE; j++)
                    {
                        if (j == VANT_EDGE)
                            pv[iv + j].pos = sp + vlist[vn]->pos[0];
                        else
                            pv[iv + j].pos = sp + vlist[vn]->pos[j];
                    }
                    iv += VANT_EDGE + 1;
                    sp += dp;
                }

                // Set down ropes points
                sp = lPos;
                dp = (rPos - lPos) / static_cast<float>(ROPE_QUANT - 1);
                for (i = 0; i < ROPE_QUANT; i++)
                {
                    for (j = 0; j <= VANT_EDGE; j++)
                    {
                        if (j == VANT_EDGE)
                            pv[iv + j].pos = sp + vlist[vn]->pos[0];
                        else
                            pv[iv + j].pos = sp + vlist[vn]->pos[j];
                    }
                    iv += VANT_EDGE + 1;
                    sp += dp;
                }
            }
        }

        RenderService->UnLockVertexBuffer(vBuf);
    }
}

bool VANT_BASE::VectCmp(CVECTOR v1, CVECTOR v2, float minCmpVal) // return true if equal
{
    const CVECTOR dv = v1 - v2;

    if (dv.x > minCmpVal || dv.x < -minCmpVal || dv.y > minCmpVal || dv.y < -minCmpVal || dv.z > minCmpVal ||
        dv.z < -minCmpVal)
        return false;
    return true;
}

void VANT_BASE::FirstRun()
{
    if (nVert > 0 && nIndx > 0)
    {
        VERTEX_BUFFER_RELEASE(RenderService, vBuf);
        INDEX_BUFFER_RELEASE(RenderService, iBuf);
        vBuf = RenderService->CreateVertexBuffer(VANTVERTEX_FORMAT, nVert * sizeof(VANTVERTEX), D3DUSAGE_WRITEONLY);
        iBuf = RenderService->CreateIndexBuffer(nIndx * 6);
        SetVertexes();
        SetIndex();
    }

    bUse = (vBuf != -1 && iBuf != -1);

    bRunFirstTime = false;
    wVantLast = vantQuantity;
}

void VANT_BASE::SetAdd(int firstNum)
{
    // set vertex and index buffers
    for (int vn = firstNum; vn < vantQuantity; vn++)
    {
        while (vlist[vn]->pUpMatWorld == nullptr || vlist[vn]->pDownMatWorld == nullptr)
        {
            delete vlist[vn];
            vantQuantity--;
            if (vantQuantity > 0)
            {
                VANTDATA **oldvlist = vlist;
                vlist = new VANTDATA *[vantQuantity];
                if (vlist == nullptr)
                    vlist = oldvlist;
                if (vn > 0)
                    memcpy(vlist, oldvlist, sizeof(VANTDATA *) * vn);
                if (vn < vantQuantity)
                    memcpy(&vlist[vn], &oldvlist[vn + 1], sizeof(VANTDATA *) * (vantQuantity - vn));
                if (vlist != oldvlist)
                    delete oldvlist;
            }
            else
            {
                delete vlist;
                vlist = nullptr;
            }
            if (vn == vantQuantity)
                break;
        }
        if (vn == vantQuantity)
            break;

        // Set normals with length equal the rope width
        CMatrix tmat;
        tmat.BuildViewMatrix(.5f * (vlist[vn]->pLeft + vlist[vn]->pRight), vlist[vn]->pUp, CVECTOR(0.f, 0.f, 1.f));
        float ca, sa;
        for (int i = 0; i < VANT_EDGE; i++)
        {
            ca = cosf(static_cast<float>(i) / static_cast<float>(VANT_EDGE) * 2.f * PI);
            sa = sinf(static_cast<float>(i) / static_cast<float>(VANT_EDGE) * 2.f * PI);
            tmat.MulToInvNorm(CVECTOR(ROPE_WIDTH / 2.f * ca, ROPE_WIDTH / 2.f * sa, 0.f), vlist[vn]->pos[i]);
        }

        vlist[vn]->nv = (VANT_EDGE + 1) * ROPE_QUANT * 2 + 7 + 6;
        vlist[vn]->nt = VANT_EDGE * ROPE_QUANT * 2 + 3 + 8;
    }
}

void VANT_BASE::DoSTORM_DELETE()
{
    // for all deleted groups, delete the guys that belong them
    int ngn = 0;
    int nvn = 0;
    nVert = 0;
    nIndx = 0;
    for (int gn = 0; gn < groupQuantity; gn++)
    {
        int gs = 0; // group size
        gdata[gn].sVert = nVert;
        gdata[gn].sIndx = nIndx;
        gdata[gn].nVert = 0;
        gdata[gn].nIndx = 0;
        for (int idx = 0; idx < gdata[gn].vantQuantity; idx++)
        {
            const int vn = gdata[gn].vantIdx[idx];
            if (vlist[vn]->bDeleted || gdata[gn].bDeleted)
            {
                delete vlist[vn];
                continue;
            }

            vlist[vn]->HostGroup = ngn;
            vlist[vn]->sv = nVert;
            vlist[vn]->st = nIndx;
            gdata[gn].nVert += vlist[vn]->nv;
            gdata[gn].nIndx += vlist[vn]->nt;
            nVert += vlist[vn]->nv;
            nIndx += vlist[vn]->nt * 3;

            gdata[gn].vantIdx[gs++] = nvn;

            // put the guy on the right places in the list
            vlist[nvn++] = vlist[vn];
        }

        // if the group is empty, then delete it
        if (gs == 0)
        {
            delete gdata[gn].vantIdx;
            continue;
        }

        gdata[gn].vantQuantity = gs;
        // put the group in the right place in the list
        if (ngn < gn)
            memcpy(&gdata[ngn], &gdata[gn], sizeof(GROUPDATA));
        ngn++;
    }

    nIndx /= 3;
    // if there are no more guys, then remove them all
    if (ngn == 0 || nvn == 0)
    {
        vantQuantity = groupQuantity = 0;
        VERTEX_BUFFER_RELEASE(RenderService, vBuf);
        INDEX_BUFFER_RELEASE(RenderService, iBuf);
        delete vlist;
        vlist = nullptr;
        delete gdata;
        gdata = nullptr;
    }
    else if (nvn != vantQuantity || ngn != groupQuantity)
    {
        vantQuantity = nvn;
        groupQuantity = ngn;
        VERTEX_BUFFER_RELEASE(RenderService, vBuf);
        INDEX_BUFFER_RELEASE(RenderService, iBuf);
        vBuf = RenderService->CreateVertexBuffer(VANTVERTEX_FORMAT, nVert * sizeof(VANTVERTEX), D3DUSAGE_WRITEONLY);
        iBuf = RenderService->CreateIndexBuffer(nIndx * 6);
        SetVertexes();
        SetIndex();
    }

    bYesDeleted = false;
    wVantLast = vantQuantity;
    bUse = vantQuantity > 0;
}
