#include "Flag.h"
#include "../../shared/sail_msg.h"
#include "Entity.h"
#include "Weather_Base.h"
#include "core.h"
#include "defines.h"
#include "ship_base.h"
#include "vfile_service.h"

static const char *RIGGING_INI_FILE = "resource\\ini\\rigging.ini";

FLAG::FLAG()
{
    bUse = false;
    RenderService = nullptr;
    TextureName = nullptr;
    bFirstRun = true;
    texl = -1;
    flist = nullptr;
    flagQuantity = 0;
    gdata = nullptr;
    groupQuantity = 0;
    bYesDeleted = false;
    wFlagLast = 0;
    vBuf = iBuf = -1;
    nVert = nIndx = 0;
}

FLAG::~FLAG()
{
    TEXTURE_RELEASE(RenderService, texl);
    STORM_DELETE(TextureName);
    STORM_DELETE(gdata);
    VERTEX_BUFFER_RELEASE(RenderService, vBuf);
    INDEX_BUFFER_RELEASE(RenderService, iBuf);

    while (flagQuantity > 0)
    {
        flagQuantity--;
        STORM_DELETE(flist[flagQuantity]);
    }
    STORM_DELETE(flist);
}

bool FLAG::Init()
{
    // GUARD(FLAG::FLAG())
    SetDevice();
    // UNGUARD
    return true;
}

void FLAG::SetDevice()
{
    // get render service
    RenderService = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!RenderService)
    {
        throw std::exception("No service: dx9render");
    }
    globalWind.ang.x = 0.f;
    globalWind.ang.y = 0.f;
    globalWind.ang.z = 1.f;
    globalWind.base = 1.f;
    LoadIni();
    texl = RenderService->TextureCreate(TextureName);
}

bool FLAG::CreateState(ENTITY_STATE_GEN *state_gen)
{
    return true;
}

bool FLAG::LoadState(ENTITY_STATE *state)
{
    return true;
}

void FLAG::Execute(uint32_t Delta_Time)
{
    if (bFirstRun)
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

        // get the wind value
        if (const auto ei = EntityManager::GetEntityId("weather"))
        {
            auto *wb = static_cast<WEATHER_BASE *>(EntityManager::GetEntityPointer(ei));
            globalWind.ang.x = wb->GetFloat(whf_wind_angle);
            globalWind.ang.z = cosf(globalWind.ang.x);
            globalWind.ang.x = sinf(globalWind.ang.x);
            globalWind.base = wb->GetFloat(whf_wind_speed) / fWindMaxValue;
        }

        // calculation of the shape of the flag
        vertBuf = static_cast<FLAGLXVERTEX *>(RenderService->LockVertexBuffer(vBuf));
        if (vertBuf)
        {
            const auto dt = static_cast<float>(Delta_Time) * 0.02f;
            for (auto fn = 0; fn < flagQuantity; fn++)
                DoMove(flist[fn], dt);
            RenderService->UnLockVertexBuffer(vBuf);
        }
    }
}

void FLAG::Realize(uint32_t Delta_Time)
{
    if (bUse)
    {
        //_asm rdtsc _asm mov rtm,eax

        RenderService->TextureSet(0, texl);
        uint32_t ambient;
        RenderService->GetRenderState(D3DRS_AMBIENT, &ambient);
        RenderService->SetRenderState(D3DRS_TEXTUREFACTOR, ambient);
        RenderService->SetTransform(D3DTS_WORLD, &rootMatrix);

        // draw nature flag
        if (nVert != 0 && nIndx != 0)
            RenderService->DrawBuffer(vBuf, sizeof(FLAGLXVERTEX), iBuf, 0, nVert, 0, nIndx, "ShipFlag");
        //_asm rdtsc  _asm sub eax,rtm _asm mov rtm,eax
        // Print info
        // RenderService->Print(0,220,"Flags tics= %d",rtm);
    }
}

uint64_t FLAG::ProcessMessage(MESSAGE &message)
{
    const auto code = message.Long();

    switch (code)
    {
        //**********************
        // Start Initialisation
        //----------------------
    case MSG_FLAG_INIT: {
        const auto eidModel = message.EntityID();
        const auto nNation = message.Long();
        entid_t eidShip = message.EntityID();

        MODEL *host_mdl;
        host_mdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(eidModel));
        if (host_mdl == nullptr)
        {
            core.Trace("Missing INIT message to FLAG: bad MODEL");
        }

        if (groupQuantity == 0)
        {
            gdata = new GROUPDATA[1];
            if (gdata == nullptr)
                throw std::exception("Not memory allocation");

            groupQuantity = 1;
        }
        else
        {
            auto *const oldgdata = gdata;
            gdata = new GROUPDATA[groupQuantity + 1];
            if (gdata == nullptr)
                throw std::exception("Not memory allocation");
            memcpy(gdata, oldgdata, sizeof(GROUPDATA) * groupQuantity);
            delete oldgdata;
            groupQuantity++;
        }
        gdata[groupQuantity - 1].model_id = eidModel;
        gdata[groupQuantity - 1].nation = nNation;
        gdata[groupQuantity - 1].bDeleted = false;
        gdata[groupQuantity - 1].ship_id = eidShip;
        gdata[groupQuantity - 1].isShip = true;
        gdata[groupQuantity - 1].char_attributes =
            ((VAI_OBJBASE *)EntityManager::GetEntityPointer(gdata[groupQuantity - 1].ship_id))->GetACharacter();

        NODE *nod;
        GEOS::INFO gi;
        GEOS::LABEL gl;
        int i, j;
        for (j = 0; true; j++)
        {
            nod = host_mdl->GetNode(j);
            if (nod == nullptr || nod->geo == nullptr)
                break;

            nod->geo->GetInfo(gi);
            for (i = 0; i < gi.nlabels; i++)
            {
                nod->geo->GetLabel(i, gl);
                if (!strncmp(gl.group_name, "sflag", 5)) // special flag
                {
                    AddLabel(gl, nod, 1, 1);
                }
                else
                {
                    if (!strncmp(gl.group_name, "flag", 4)) // ordinary flag
                        AddLabel(gl, nod, 0, 1);
                }
            }
        }
        bFirstRun = true;
    }
    break;

    case MSG_FLAG_INIT_TOWN: {
        entid_t eidModel = message.EntityID();
        long nNation = message.Long();

        MODEL *host_mdl;
        host_mdl = (MODEL *)EntityManager::GetEntityPointer(eidModel);
        if (host_mdl == nullptr)
        {
            core.Trace("Missing INIT message to FLAG: bad MODEL");
        }

        if (groupQuantity == 0)
        {
            gdata = new GROUPDATA[1];
            if (gdata == nullptr)
                throw std::exception("Not memory allocation");
            groupQuantity = 1;
        }
        else
        {
            GROUPDATA *oldgdata = gdata;
            gdata = new GROUPDATA[groupQuantity + 1];
            if (gdata == 0)
                _THROW("Not memory allocation");
            memcpy(gdata, oldgdata, sizeof(GROUPDATA) * groupQuantity);
            delete oldgdata;
            groupQuantity++;
        }
        gdata[groupQuantity - 1].model_id = eidModel;
        gdata[groupQuantity - 1].nation = nNation;
        gdata[groupQuantity - 1].bDeleted = false;
        gdata[groupQuantity - 1].isShip = false;

        NODE *nod;
        GEOS::INFO gi;
        GEOS::LABEL gl;
        int i, j;
        for (j = 0; true; j++)
        {
            nod = host_mdl->GetNode(j);
            if (nod == nullptr || nod->geo == nullptr)
                break;

            nod->geo->GetInfo(gi);
            for (i = 0; i < gi.nlabels; i++)
            {
                nod->geo->GetLabel(i, gl);
                if (!strncmp(gl.group_name, "sflag", 5)) // special flag
                {
                    AddLabel(gl, nod, 1, 0);
                }
                else
                {
                    if (!strncmp(gl.group_name, "flag", 4)) // ordinary flag
                        AddLabel(gl, nod, 0, 0);
                }
            }
        }
        bFirstRun = true;
    }
    break;

    case MSG_FLAG_DEL_GROUP:
        GroupSTORM_DELETE(message.EntityID());
        break;

    case MSG_FLAG_TO_NEWHOST: // format "lili" (msg_code,oldmodel_id,groupNum,newmodel_id)
    {
        const auto oldModelID = message.EntityID();
        const auto flagGroupNum = message.Long();
        const auto newModelID = message.EntityID();
        MoveOtherHost(newModelID, flagGroupNum, oldModelID);
    }
    break;
    }
    return 0;
}

void FLAG::SetTextureCoordinate() const
{
    if (bUse)
    {
        int i;
        long sIdx;
        float stu, addtu, dtu;
        float stv, addtv, dtv;

        auto *pv = static_cast<FLAGLXVERTEX *>(RenderService->LockVertexBuffer(vBuf));
        if (pv)
        {
            for (auto fn = 0; fn < flagQuantity; fn++)
            {
                if (flist[fn] == nullptr)
                    continue;
                sIdx = flist[fn]->sv;
                addtu = 1.f / static_cast<float>(FlagTextureQuantity);
                addtv = 1.f / (float)FlagTextureQuantityRow;
                stu = addtu * flist[fn]->texNumC;
                stv = addtv * flist[fn]->texNumR;
                if (flist[fn]->triangle)
                {
                    dtu = addtu / static_cast<float>(flist[fn]->vectQuant + 1);
                    dtv = .5f / static_cast<float>(flist[fn]->vectQuant + 1);
                }
                else
                {
                    dtu = addtu / static_cast<float>(flist[fn]->vectQuant);
                    dtv = 0.f;
                }
                for (i = 0; i <= flist[fn]->vectQuant; i++)
                {
                    pv[sIdx].tu = stu;
                    pv[sIdx++].tv = stv;
                    pv[sIdx].tu = stu;
                    pv[sIdx++].tv = stv + addtv;

                    stu += dtu;
                    stv += dtv;
                }
                if (flist[fn]->triangle)
                {
                    pv[sIdx].tv = stv;
                    pv[sIdx].tu = stu;
                }
            }
            RenderService->UnLockVertexBuffer(vBuf);
        }
    }
}

void FLAG::DoMove(FLAGDATA *pr, float delta_time) const
{
    if (pr == nullptr)
        return;
    CVECTOR cPos;
    cPos = *pr->pMatWorld * pr->spos;

    auto xMul = globalWind.ang.x;
    auto zMul = globalWind.ang.z;

    if ((pr->Alfa +=
         (ALFA_DEPEND + ALFA_RAND * static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * delta_time) > PIm2)
        pr->Alfa = 0.f;
    if ((pr->Beta +=
         (BETA_DEPEND + BETA_RAND * static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * delta_time) > PIm2)
        pr->Beta = 0.f;
    auto Alfa = -pr->Alfa;
    auto Beta = -pr->Beta;
    auto sAlfa = sinf(Alfa);
    auto cAlfa = cosf(Alfa);
    auto sBeta = sinf(Beta);
    auto cBeta = cosf(Beta);

    CVECTOR dhVect;
    dhVect = *pr->pMatWorld * pr->dhv - pr->pMatWorld->Pos();

    long sIdx = pr->sv;
    CVECTOR dopVect;
    CVECTOR incrVect;
    incrVect.x = FLAGVECTORLEN * xMul;
    incrVect.z = FLAGVECTORLEN * zMul;
    incrVect.y = -DOWNVAL;

    auto oldSa = sAlfa;
    auto oldCa = cAlfa;
    auto oldSb = sBeta;
    auto oldCb = cBeta;
    auto hIncr = pr->dhv.y;

    auto tmp = fAlfaStep + fAlfaMax / static_cast<float>(pr->vectQuant);
    auto sa = sinf(tmp);
    auto ca = cosf(tmp);
    tmp = fBetaStep + fBetaMax / static_cast<float>(pr->vectQuant);
    auto sb = sinf(tmp);
    auto cb = cosf(tmp);

    for (auto i = 0; i <= pr->vectQuant; i++)
    {
        tmp = fWindAm * (sAlfa - oldSa);
        dopVect.y = fWindAm * (cAlfa - oldCa);
        dopVect.x = tmp * zMul;
        dopVect.z = tmp * xMul;

        vertBuf[sIdx].pos = cPos + dopVect + dhVect;
        sIdx++;
        vertBuf[sIdx].pos = cPos + dopVect - dhVect;
        sIdx++;

        // calculate the new angle (sines \ cosines)
        tmp = sAlfa;
        sAlfa = sAlfa * ca + sa * cAlfa;
        cAlfa = cAlfa * ca - tmp * sa;

        // calculate the new angle (sines \ cosines)
        tmp = sBeta;
        sBeta = sBeta * cb + sb * cBeta;
        cBeta = cBeta * cb - tmp * sb;

        hIncr -= pr->ddhv.y;
        tmp = hIncr * (sBeta - oldSb) * fRotAm;
        if (tmp > hIncr || tmp < -hIncr)
        {
            dhVect.y = 0.f;
            dhVect.x = hIncr * zMul;
            dhVect.z = hIncr * xMul;
        }
        else
        {
            auto fT = hIncr * hIncr - tmp * tmp;
            if (fT < 0.f)
                fT = -fT;
            dhVect.y = sqrtf(hIncr * hIncr - tmp * tmp);
            dhVect.x = tmp * zMul;
            dhVect.z = tmp * xMul;
        }
        dhVect = *pr->pMatWorld * dhVect - pr->pMatWorld->Pos();

        cPos += incrVect;
    }

    if (pr->triangle)
    {
        dopVect.x = fWindAm * (sAlfa - oldSa);
        dopVect.z = dopVect.x * xMul;
        dopVect.x *= zMul;
        dopVect.y = -fWindAm * (cAlfa - oldCa);
        vertBuf[sIdx].pos = cPos + dopVect;
    }
}

void FLAG::AddLabel(GEOS::LABEL &gl, NODE *nod, bool isSpecialFlag, bool isShip)
{
    FLAGDATA *fd;
    int grNum;

    // for fail parameters do not set of data
    if (nod == nullptr)
        return;

    if (isSpecialFlag)
        grNum = atoi(&gl.group_name[5]);
    else
        grNum = atoi(&gl.group_name[4]);

    int fn;
    for (fn = 0; fn < flagQuantity; fn++)
        if (flist[fn] != nullptr && flist[fn]->HostGroup == groupQuantity - 1 && flist[fn]->grNum == grNum &&
            flist[fn]->nod == nod)
        {
            fd = flist[fn];
            break;
        }
    if (fn == flagQuantity)
    {
        // create new flag
        fd = new FLAGDATA;
        if (fd == nullptr)
            throw std::exception("Not memory allocation");
        PZERO(fd, sizeof(FLAGDATA));
        fd->triangle = true; // this is Vimpel
        fd->isSpecialFlag = isSpecialFlag;
        fd->isShip = isShip;
        fd->pMatWorld = &nod->glob_mtx;
        fd->nod = nod;
        fd->grNum = grNum;
        fd->Alfa = 0.f;
        fd->Beta = 0.f;
        fd->HostGroup = groupQuantity - 1;
        fd->bDeleted = false;

        if (flagQuantity == 0)
        {
            flist = new FLAGDATA *[1];
            if (flist == nullptr)
                throw std::exception("Not memory allocation");
            flagQuantity = 1;
        }
        else
        {
            auto *const oldflist = flist;
            flist = new FLAGDATA *[flagQuantity + 1];
            if (flist == nullptr)
                throw std::exception("Not memory allocation");
            memcpy(flist, oldflist, sizeof(FLAGDATA *) * flagQuantity);
            delete oldflist;
            flagQuantity++;
        }
        flist[flagQuantity - 1] = fd;
    }

    CVECTOR cv;
    cv.x = gl.m[3][0] + nod->loc_mtx.matrix[3];
    cv.y = gl.m[3][1] + nod->loc_mtx.matrix[7];
    cv.z = gl.m[3][2] + nod->loc_mtx.matrix[11];
    /*cv.x = gl.m[3][0];
    cv.y = gl.m[3][1];
    cv.z = gl.m[3][2];*/
    // set flag point
    switch (gl.name[1])
    {
    case '2': // bottom begin of flag
        fd->dhv = cv;
        break;
    case '3': // top end of flag (or center of the flag end for triangle flag)
        fd->ddhv = cv;
        break;
    case '4': // bottom ebd of flag
        fd->dv = cv;
        fd->triangle = false; // this not Vimpel - that Flag
        break;
    default: // case 1: top begin of flag
        fd->spos = cv;
    }
}

void FLAG::SetAll()
{
    // set vertex and index buffers
    nVert = nIndx = 0;
    SetAdd(0);
}

void FLAG::SetTreangle() const
{
    int i, idx;

    auto *pt = static_cast<uint16_t *>(RenderService->LockIndexBuffer(iBuf));
    if (pt)
    {
        for (auto fn = 0; fn < flagQuantity; fn++)
        {
            if (flist[fn] == nullptr)
                continue;
            idx = flist[fn]->st;
            for (i = 0; i < static_cast<int>(flist[fn]->nt); i++)
            {
                pt[idx++] = static_cast<uint16_t>(i + flist[fn]->sv);
                pt[idx++] = static_cast<uint16_t>(i + flist[fn]->sv + 1);
                pt[idx++] = static_cast<uint16_t>(i + flist[fn]->sv + 2);
            }
        }

        RenderService->UnLockIndexBuffer(iBuf);
    }
}

void FLAG::LoadIni()
{
    // GUARD(FLAG::LoadIni());
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

    sprintf(section, "FLAGS");

    auto texChange = false;
    int tmp;
    // load texture parameters
    ini->ReadString(section, "TextureName", param, sizeof(param) - 1, "flagall.tga");
    if (TextureName != nullptr)
    {
        if (strcmp(TextureName, param))
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

    FlagTextureQuantity = (int)ini->GetLong(section, "TextureCountColumn", 4);
    FlagTextureQuantityRow = (int)ini->GetLong(section, "TextureCountRow", 8);

    SetTextureCoordinate();

    // flag segment length
    FLAGVECTORLEN = ini->GetFloat(section, "fSegLen", 0.2f);

    // Wind influence parameters
    // vertical wind speed
    ALFA_DEPEND = ini->GetFloat(section, "alfa_depend", 0.1f);
    // horizontal wind speed
    BETA_DEPEND = ini->GetFloat(section, "beta_depend", 0.06f);
    // the maximum value of the random change in the angle Alpha
    ALFA_RAND = ini->GetFloat(section, "alpha_rand", 0.1f);
    // the maximum value of the random change in the angle Beta
    BETA_RAND = ini->GetFloat(section, "beta_rand", 0.1f);
    // maximum angle (for the end of the flag)
    fAlfaMax = ini->GetFloat(section, "fAlfaMax", 4.71f);
    fAlfaStep = ini->GetFloat(section, "fAlfaStep", .2f);
    // maximum angle (for the end of the flag)
    fBetaMax = ini->GetFloat(section, "fBetaMax", 7.85f);
    fBetaStep = ini->GetFloat(section, "fBetaStep", .3f);
    // Parameters of forming the shape of the flag and its fluctuation
    // amplitude of the flag guideline
    fWindAm = ini->GetFloat(section, "fWindAm", 0.2f);
    // amplitude of oscillation (rotation) of the diameter of the flag
    fRotAm = ini->GetFloat(section, "fRotAm", 0.2f);
    // the value of lowering the flag at each step from the mast
    DOWNVAL = ini->GetFloat(section, "fDownVal", 0.05f);
    // maximum wind value
    fWindMaxValue = ini->GetFloat(section, "fWindMaxValue", 12.f);
    // minimum number of segments in the flag
    MinSegmentQuantity = ini->GetLong(section, "MinSegQuantity", 4);
    // UNGUARD
}

void FLAG::FirstRun()
{
    if (wFlagLast)
        SetAdd(wFlagLast);
    else
    {
        SetAll();

        ZERO(rootMatrix);
        rootMatrix._11 = rootMatrix._22 = rootMatrix._33 = rootMatrix._44 = 1.f;
    }

    if (nVert)
    {
        bUse = true;
        iBuf = RenderService->CreateIndexBuffer(nIndx * 2);
        SetTreangle();
        vBuf = RenderService->CreateVertexBuffer(FLAGLXVERTEX_FORMAT, nVert * sizeof(FLAGLXVERTEX), D3DUSAGE_WRITEONLY);
        SetTextureCoordinate();
        nIndx /= 3;
    }

    bFirstRun = false;
    wFlagLast = flagQuantity;
}

void FLAG::GroupSTORM_DELETE(entid_t m_id)
{
    // find a group corresponding to the resulting model
    for (auto gn = 0; gn < groupQuantity; gn++)
        if (gdata[gn].model_id == m_id)
        {
            gdata[gn].bDeleted = true; // mark it as deleted
            bYesDeleted = true;
            break;
        }
}

void FLAG::DoSTORM_DELETE()
{
    // go through all deleted groups and remove the flags they contain
    auto ngn = 0;
    nVert = 0;
    nIndx = 0;
    for (auto gn = 0; gn < groupQuantity; gn++)
    {
        auto gs = 0; // number of unremoved flags in the group
        for (auto fn = 0; fn < flagQuantity; fn++)
            if (flist[fn] != nullptr && flist[fn]->HostGroup == gn)
            {
                // if the flag is marked as deleted, then physically delete
                if (gdata[gn].bDeleted || flist[fn]->bDeleted || flist[fn]->nv <= 0 || flist[fn]->nt <= 0)
                {
                    flist[fn]->bDeleted = true;
                    continue;
                }

                flist[fn]->HostGroup = ngn;
                flist[fn]->sv = nVert;
                flist[fn]->st = nIndx;
                nVert += flist[fn]->nv;
                nIndx += flist[fn]->nt * 3;
                gs++;
            }
        // if all flags are removed from the group, then delete the group
        if (gs == 0)
            continue;

        // write the group to its position
        if (ngn < gn)
            memcpy(&gdata[ngn], &gdata[gn], sizeof(GROUPDATA));
        ngn++;
    }

    auto nfn = 0;
    for (auto fn = 0; fn < flagQuantity; fn++)
    {
        if (flist[fn] == nullptr)
            continue;
        if (flist[fn]->bDeleted)
        {
            delete flist[fn];
            flist[fn] = nullptr;
            continue;
        }

        flist[nfn++] = flist[fn];
    }

    nIndx /= 3;
    if (nfn == 0 || ngn == 0)
    {
        VERTEX_BUFFER_RELEASE(RenderService, vBuf);
        INDEX_BUFFER_RELEASE(RenderService, iBuf);
        flagQuantity = groupQuantity = 0;
        delete flist;
        flist = nullptr;
        delete gdata;
        gdata = nullptr;
    }
    else if (nfn != flagQuantity || ngn != groupQuantity)
    {
        VERTEX_BUFFER_RELEASE(RenderService, vBuf);
        INDEX_BUFFER_RELEASE(RenderService, iBuf);
        vBuf = RenderService->CreateVertexBuffer(FLAGLXVERTEX_FORMAT, nVert * sizeof(FLAGLXVERTEX), D3DUSAGE_WRITEONLY);
        iBuf = RenderService->CreateIndexBuffer(nIndx * 6);

        flagQuantity = nfn;
        groupQuantity = ngn;

        auto *const oldflist = flist;
        flist = new FLAGDATA *[flagQuantity];
        if (flist)
        {
            memcpy(flist, oldflist, sizeof(FLAGDATA *) * flagQuantity);
            delete oldflist;
        }
        else
            flist = oldflist;

        auto *const oldgdata = gdata;
        gdata = new GROUPDATA[groupQuantity];
        if (gdata)
        {
            memcpy(gdata, oldgdata, sizeof(GROUPDATA) * groupQuantity);
            delete oldgdata;
        }
        else
            gdata = oldgdata;

        SetTreangle();
        SetTextureCoordinate();
    }

    wFlagLast = flagQuantity;
    bYesDeleted = false;
    bUse = flagQuantity > 0;
}

void FLAG::SetAdd(int flagNum)
{
    VDATA *pvdat;
    // set vertex and index buffers
    nIndx *= 3;
    for (auto fn = flagNum; fn < flagQuantity; fn++)
    {
        if (flist[fn] == nullptr)
            continue;
        CVECTOR bmpos, empos;
        float len;

        auto p0 = flist[fn]->spos;
        auto p1 = flist[fn]->dv;
        auto p2 = flist[fn]->dhv;
        auto p3 = flist[fn]->ddhv;
        flist[fn]->spos = bmpos = (p0 + p1) * .5f; // the middle of the starting edge of the flag
        // calculate the midpoint of the final edge of the flag
        if (flist[fn]->triangle)
            empos = p2;
        else
            empos = (p2 + p3) * .5f;
        flist[fn]->dhv = p0 - bmpos;
        len = sqrtf(~(empos - bmpos)); // flag length
        if (len < FLAGVECTORLEN)
        {
            delete flist[fn];
            if (flagQuantity == 1)
            {
                delete flist;
                flist = nullptr;
                flagQuantity = 0;
            }
            else
            {
                auto *const oldflist = flist;
                flagQuantity--;
                flist = new FLAGDATA *[flagQuantity];
                if (flist == nullptr)
                    flist = oldflist;
                if (fn > 0)
                    memcpy(flist, oldflist, sizeof(FLAGDATA *) * fn);
                if (fn < flagQuantity)
                    memcpy(&flist[fn], &oldflist[fn + 1], sizeof(FLAGDATA *) * (flagQuantity - fn));
                if (flist != oldflist)
                    delete oldflist;
            }
        }
        else
        {
            long curTexNumC = 0;
            long curTexNumR = 0;
            // set texture number
            if (flist[fn]->isShip) // ship
            {
                pvdat = core.Event("GetRiggingData", "sllla", "GetShipFlagTexNum", flist[fn]->triangle,
                                   gdata[flist[fn]->HostGroup].nation, flist[fn]->isSpecialFlag,
                                   gdata[flist[fn]->HostGroup].char_attributes);
            }
            else
            {
                pvdat = core.Event("GetRiggingData", "slll", "GetTownFlagTexNum", flist[fn]->triangle,
                                   gdata[flist[fn]->HostGroup].nation, flist[fn]->isSpecialFlag);
            }
            if (pvdat == nullptr)
            {
                flist[fn]->texNumC = 0;
                flist[fn]->texNumR = 0;
            }
            else
            {
                pvdat->Get(curTexNumC, 0);
                pvdat->Get(curTexNumR, 1);
                flist[fn]->texNumC = curTexNumC;
                flist[fn]->texNumR = curTexNumR;
            }

            flist[fn]->vectQuant = (int)(len / FLAGVECTORLEN); // number of flag segments
            if (flist[fn]->vectQuant < MinSegmentQuantity)
                flist[fn]->vectQuant = MinSegmentQuantity;
            // compute flag increment
            flist[fn]->dv = (empos - bmpos) / static_cast<float>(flist[fn]->vectQuant);
            if (flist[fn]->triangle)
                flist[fn]->ddhv = (p2 - p0 - empos + bmpos) / static_cast<float>(flist[fn]->vectQuant);
            else
                flist[fn]->ddhv = (p2 - p0 - empos + bmpos) / static_cast<float>(flist[fn]->vectQuant + 1);

            flist[fn]->sv = nVert;
            flist[fn]->st = nIndx;
            flist[fn]->vectQuant; //~!~
            if (flist[fn]->triangle)
            {
                nVert += (flist[fn]->nv = flist[fn]->vectQuant * 2 + 3);
                nIndx += (flist[fn]->nt = flist[fn]->vectQuant * 2 + 1) * 3;
            }
            else
            {
                nVert += (flist[fn]->nv = flist[fn]->vectQuant * 2 + 2);
                nIndx += (flist[fn]->nt = flist[fn]->vectQuant * 2) * 3;
            }
        }
    }
    // remove old buffers
    VERTEX_BUFFER_RELEASE(RenderService, vBuf);
    INDEX_BUFFER_RELEASE(RenderService, iBuf);
}

void FLAG::MoveOtherHost(entid_t newm_id, long flagNum, entid_t oldm_id)
{
    // find the old group
    int oldgn;
    for (oldgn = 0; oldgn < groupQuantity; oldgn++)
        if (gdata[oldgn].model_id == oldm_id)
            break;
    // if there is no such group, then an empty return
    if (oldgn == groupQuantity)
        return;

    // find a new group
    int newgn;
    for (newgn = 0; newgn < groupQuantity; newgn++)
        if (gdata[newgn].model_id == newm_id)
            break;
    // if there is no new group, then create it
    if (newgn == groupQuantity)
    {
        auto *const oldgdata = gdata;
        gdata = new GROUPDATA[groupQuantity + 1];
        if (gdata == nullptr)
            throw std::exception("Not memory allocation");
        memcpy(gdata, oldgdata, sizeof(GROUPDATA) * groupQuantity);
        delete oldgdata;
        groupQuantity++;

        // fill in the parameters of the new group
        gdata[newgn].bDeleted = false;
        gdata[newgn].model_id = newm_id;
        gdata[newgn].nation = gdata[oldgn].nation;
    }

    // find the flag we need
    int fn;
    for (fn = 0; fn < flagQuantity; fn++)
        if (flist[fn] != nullptr && flist[fn]->grNum == flagNum && flist[fn]->HostGroup == oldgn)
            break;
    // reassign its owner to the new owner
    if (fn < flagQuantity)
        flist[fn]->HostGroup = newgn;
}
