#include "core.h"
#include "entity.h"
#include "modelr.h"
#include "shared/messages.h"

#ifdef _WIN32 // FIX_LINUX DirectXMath
#include <DirectXMath.h>
#endif

CREATE_CLASS(MODELR)

IDirect3DVertexBuffer9 *dest_vb;

MODELR::MODELR()
{
    bSetupFog = false;
    LightPath[0] = 0;
    lmPath[0] = 0;
    ani = nullptr;
    memset(aniVerts, 0, sizeof(aniVerts));
    d3dDestVB = nullptr;
    for (int32_t i = 0; i < ANI_MAX_ACTIONS; i++)
        aniPos[i] = -1.0f;
    root = nullptr;
    useBlend = false;
    idxBuff = nullptr;
    nAniVerts = 0;
}

CMatrix *bones;

MODELR::~MODELR()
{
    if (d3dDestVB != nullptr)
        d3dDestVB->Release();
    delete root;
    for (auto i = 0; i < MODEL_ANI_MAXBUFFERS; i++)
        delete aniVerts[i].v;
    delete ani;

    delete idxBuff;
}

bool MODELR::Init()
{
    rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    if (!rs)
        throw std::runtime_error("No service: dx9render");

    GeometyService = static_cast<VGEOMETRY *>(core.GetService("geometry"));
    if (!GeometyService)
        throw std::runtime_error("No service: geometry");

    return true;
}

bool alreadyTransformed;

void *VBTransform(void *vb, int32_t startVrt, int32_t nVerts, int32_t totVerts)
{
    if (alreadyTransformed)
        return dest_vb;
    alreadyTransformed = true;
    if (!totVerts)
        return dest_vb;

    auto *src = static_cast<GEOS::AVERTEX0 *>(vb);

    GEOS::VERTEX0 *dst;
    dest_vb->Lock(0, 0, (void **)&dst, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);

#ifndef _WIN32 // FIX_LINUX DirectXMath
    CMatrix mtx;
#endif
    for (int32_t v = 0; v < totVerts; v++)
    {
        // Vertex
        auto &vrt = src[v];
        auto &dstVrt = dst[v];
        // Matrices
        auto &m1 = bones[vrt.boneid & 0xff];
        auto &m2 = bones[(vrt.boneid >> 8) & 0xff];
        // Inverse blending coefficient
        const auto wNeg = 1.0f - vrt.weight;

#ifdef _WIN32 // FIX_LINUX DirectXMath
#ifdef __AVX__
        // apparently, _mm256_set1_ps seems to be better using msvc with lat=~7+1*2+4+1*5+5+7+1*2==24 vs ~7+1*2+4+1*5+1+3+1*2==32 for _mm256_broadcast_ss
        // TODO: check clang listings
        const auto ymm0 = _mm256_set1_ps(vrt.weight);
        const auto ymm1 = _mm256_set1_ps(wNeg);

        const auto ymm2 = _mm256_add_ps(_mm256_mul_ps(_mm256_load_ps(&m1.matrix[0]), ymm0),
                                        _mm256_mul_ps(_mm256_load_ps(&m2.matrix[0]), ymm1));
        const auto ymm3 = _mm256_add_ps(_mm256_mul_ps(_mm256_load_ps(&m1.matrix[8]), ymm0),
                                        _mm256_mul_ps(_mm256_load_ps(&m2.matrix[8]), ymm1));

        const DirectX::XMMATRIX xmmtx(_mm256_castps256_ps128(ymm2), _mm256_extractf128_ps(ymm2, 1),
                                      _mm256_castps256_ps128(ymm3), _mm256_extractf128_ps(ymm3, 1));
#else
        // pure SSE2
        const auto xmm0 = _mm_set1_ps(vrt.weight);
        const auto xmm1 = _mm_set1_ps(wNeg);

        const auto xmm2 =
            _mm_add_ps(_mm_mul_ps(_mm_load_ps(&m1.matrix[0]), xmm0), _mm_mul_ps(_mm_load_ps(&m2.matrix[0]), xmm1));
        const auto xmm3 =
            _mm_add_ps(_mm_mul_ps(_mm_load_ps(&m1.matrix[4]), xmm0), _mm_mul_ps(_mm_load_ps(&m2.matrix[4]), xmm1));
        const auto xmm4 =
            _mm_add_ps(_mm_mul_ps(_mm_load_ps(&m1.matrix[8]), xmm0), _mm_mul_ps(_mm_load_ps(&m2.matrix[8]), xmm1));
        const auto xmm5 =
            _mm_add_ps(_mm_mul_ps(_mm_load_ps(&m1.matrix[12]), xmm0), _mm_mul_ps(_mm_load_ps(&m2.matrix[12]), xmm1));


        const DirectX::XMMATRIX xmmtx(xmm2, xmm3, xmm4, xmm5);
#endif

        // Position
        XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3 *>(&dstVrt.pos),
            XMVector3Transform(XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3 *>(&vrt.pos)), xmmtx));

        // Normal
        XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3 *>(&dstVrt.nrm),
            XMVector3Transform(XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3 *>(&vrt.nrm)), xmmtx));
#else
        mtx.matrix[0] = -(m1.matrix[0] * vrt.weight + m2.matrix[0] * wNeg);
        mtx.matrix[1] = m1.matrix[1] * vrt.weight + m2.matrix[1] * wNeg;
        mtx.matrix[2] = m1.matrix[2] * vrt.weight + m2.matrix[2] * wNeg;
        mtx.matrix[4] = -(m1.matrix[4] * vrt.weight + m2.matrix[4] * wNeg);
        mtx.matrix[5] = m1.matrix[5] * vrt.weight + m2.matrix[5] * wNeg;
        mtx.matrix[6] = m1.matrix[6] * vrt.weight + m2.matrix[6] * wNeg;
        mtx.matrix[8] = -(m1.matrix[8] * vrt.weight + m2.matrix[8] * wNeg);
        mtx.matrix[9] = m1.matrix[9] * vrt.weight + m2.matrix[9] * wNeg;
        mtx.matrix[10] = m1.matrix[10] * vrt.weight + m2.matrix[10] * wNeg;
        mtx.matrix[12] = -(m1.matrix[12] * vrt.weight + m2.matrix[12] * wNeg);
        mtx.matrix[13] = m1.matrix[13] * vrt.weight + m2.matrix[13] * wNeg;
        mtx.matrix[14] = m1.matrix[14] * vrt.weight + m2.matrix[14] * wNeg;

        // Position
        ((CVECTOR &)dstVrt.pos) = mtx * (CVECTOR &)vrt.pos;

        // Normal
        ((CVECTOR &)dstVrt.nrm) = mtx * (CVECTOR &)vrt.nrm;
#endif // _WIN32 DirectXMath

        // Rest
        dstVrt.color = vrt.color;
        dstVrt.tu = vrt.tu0;
        dstVrt.tv = vrt.tv0;
    }

    dest_vb->Unlock();
    return dest_vb;
}

void SetChildrenTechnique(NODE *_root, const char *_name)
{
    if (!_root || !_name)
        return;
    _root->SetTechnique(_name);
    for (int i = 0; i < _root->next.size(); i++)
        SetChildrenTechnique(_root->next[i], _name);
}

//-----------------------------------------------------------------------------------
// realize
//-----------------------------------------------------------------------------------
GEOS::PLANE ViewPlane[4];

void MODELR::Realize(uint32_t Delta_Time)
{
    // GUARD(MODELR::Realize)
    if (!root)
        return;

    uint32_t dwOldFogEnable;
    float fOldFogDensity;
    if (bSetupFog)
    {
        rs->GetRenderState(D3DRS_FOGENABLE, &dwOldFogEnable);
        rs->GetRenderState(D3DRS_FOGDENSITY, (uint32_t *)&fOldFogDensity);

        rs->SetRenderState(D3DRS_FOGENABLE, (bFogEnable) ? true : false);
        rs->SetRenderState(D3DRS_FOGDENSITY, F2DW(fFogDensity));
    }

    if (renderTuner)
        renderTuner->Set(this, rs);

    if (useBlend)
    {
        if (!passedTime)
            SetChildrenTechnique(root, blendTechnique.c_str());

        passedTime += Delta_Time;
        /*
        if (passedTime > blendTime)
          useBlend = false;
        else
        */
        {
            static uint32_t ambient;
            rs->GetRenderState(D3DRS_AMBIENT, &ambient);
            ambient &= 0x00FFFFFF;
            float timeK = static_cast<float>(passedTime) / blendTime;
            if (timeK > 1.0f)
                timeK = 1.0f;
            float alpha = alpha1 + (alpha2 - alpha1) * timeK;
            ambient |= static_cast<unsigned char>(255.0 * alpha) << 24;
            // ambient |= 0x05 << 24;
            rs->SetRenderState(D3DRS_TEXTUREFACTOR, ambient);
        }
    }

    CMatrix view, proj;
    rs->GetTransform(D3DTS_VIEW, view);
    rs->GetTransform(D3DTS_PROJECTION, proj);
    FindPlanes(view, proj);

    CVECTOR tmp;
    root->Update(mtx, tmp);

    // if have animation - special render
    if (ani)
    {
        // create VB
        if (d3dDestVB == nullptr)
        {
            // calculate total number of vertices
            GEOS::INFO gi;
            root->geo->GetInfo(gi);
            nAniVerts = 0;
            for (int32_t vb = 0; vb < gi.nvrtbuffs; vb++)
            {
                int32_t avb = root->geo->GetVertexBuffer(vb);
                VGEOMETRY::ANIMATION_VB gavb = GeometyService->GetAnimationVBDesc(avb);
                nAniVerts += gavb.nvertices;
            }

            int32_t fvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEXTUREFORMAT2 | D3DFVF_TEX1;
            rs->CreateVertexBuffer(sizeof(GEOS::VERTEX0) * nAniVerts, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, fvf,
                                   D3DPOOL_DEFAULT, &d3dDestVB);
        }
        dest_vb = d3dDestVB;

        alreadyTransformed = true;
        for (int32_t i = 0; i < 2; i++)
        {
            if (ani->Player(i).IsPlaying())
            {
                float ap = ani->Player(i).GetPosition();
                if (aniPos[i] != ap)
                    alreadyTransformed = false;
                aniPos[i] = ap;
            }
            else
            {
                if (aniPos[i] != -1.0f)
                    alreadyTransformed = false;
                aniPos[i] = -1.0f;
            }
        }

        GeometyService->SetVBConvertFunc(VBTransform);

        bones = &ani->GetAnimationMatrix(0);
        root->Draw();
        GeometyService->SetVBConvertFunc(nullptr);
        if (!alreadyTransformed)
        {
            aniPos[0] = -2.0f;
            aniPos[1] = -2.0f;
        }
    }
    else
        root->Draw();

    if (renderTuner)
        renderTuner->Restore(this, rs);

    if (bSetupFog)
    {
        rs->SetRenderState(D3DRS_FOGENABLE, dwOldFogEnable);
        rs->SetRenderState(D3DRS_FOGDENSITY, F2DW(fOldFogDensity));
    }

    // UNGUARD
}

Animation *MODELR::GetAnimation()
{
    return ani;
}

void MODELR::AniRender()
{
}

uint64_t MODELR::ProcessMessage(MESSAGE &message)
{
    std::string str;
    const int32_t code = message.Long();
    CVECTOR tmp;
    switch (code)
    {
    case MSG_SEA_REFLECTION_DRAW:
        Realize(0);
        break;
    case MSG_MODEL_SET_PARENT: {
        /*entid_t ParentID = message.EntityID();
        if (core.ValidateEntity(&ParentID))
        {
          parent = (MODEL*)ParentID.pointer;
        }*/
    }
    break;
    case MSG_MODEL_SET_POSITION: {
        // Pos,vx,vy,vz
        CVECTOR &vx = mtx.Vx();
        CVECTOR &vy = mtx.Vy();
        CVECTOR &vz = mtx.Vz();
        CVECTOR &vpos = mtx.Pos();

        vpos.x = message.Float();
        vpos.y = message.Float();
        vpos.z = message.Float();

        vx.x = message.Float();
        vx.y = message.Float();
        vx.z = message.Float();

        vy.x = message.Float();
        vy.y = message.Float();
        vy.z = message.Float();

        vz.x = message.Float();
        vz.y = message.Float();
        vz.z = message.Float();
    }
    break;
    case MSG_MODEL_BLEND:
        // blendTechnique, time, a1, a2
        useBlend = true;
        passedTime = 0;
        blendTechnique = message.String();
        blendTime = message.Long();
        alpha1 = message.Float();
        alpha2 = message.Float();
        break;
    case MSG_MODEL_LOAD_GEO: // set geometry
        // GUARD(MSG_MODEL_LOAD_GEO)
        str = message.String();
        NODER::gs = GeometyService;
        NODER::rs = rs;
        root = new NODER();
        if (!root->Init(LightPath.c_str(), str.c_str(), "", CMatrix(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f), mtx, nullptr,
                        lmPath.c_str()))
        {
            delete root;
            root = nullptr;
            core.EraseEntity(GetId());
            return 0;
        }
        // CVECTOR tmp;
        root->Update(mtx, tmp);
        return 1;
        // UNGUARD
        break;
    case MSG_MODEL_LOAD_ANI: // set animation
    {
        str = message.String();
        auto asr = static_cast<AnimationService *>(core.GetService("AnimationServiceImp"));
        ani = asr->CreateAnimation(str.c_str());
        if (ani)
            return 1;
        return 0;
    }
    break;
    case MSG_MODEL_SET_FOG:
        bSetupFog = message.Long() != 0;
        bFogEnable = message.Long() != 0;
        fFogDensity = message.Float();
        break;
    case MSG_MODEL_SET_LIGHT_PATH:
        // GUARD(MSG_MODEL_SET_LIGHT_PATH)
        LightPath = message.String();
        // UNGUARD
        break;
    case MSG_MODEL_SET_LIGHT_LMPATH:
        // GUARD(MSG_MODEL_SET_LIGHT_LMPATH)
        lmPath = message.String();
        // UNGUARD
        break;
    case MSG_MODEL_RELEASE:
        // GUARD(MSG_MODEL_RELEASE)
        if (root)
            root->ReleaseGeometry();
        // UNGUARD
        break;
    case MSG_MODEL_RESTORE:
        // GUARD(MSG_MODEL_RESTORE)
        if (root)
            root->RestoreGeometry();
        // UNGUARD
        break;
    case MSG_MODEL_SET_DIRPATH: {
        str = message.String();
        GeometyService->SetTexturePath(str.c_str());
    }
    break;
    case MSG_MODEL_SET_TECHNIQUE: {
        str = message.String();
        if (root)
            SetChildrenTechnique(root, str.c_str());
    }
    break;
    case MSG_MODEL_SET_MAX_VIEW_DIST:
        if (root)
            root->SetMaxViewDist(message.Float());
        break;
    case MSG_MODEL_SUBSTITUTE_GEOMETRY_NODE: {
        auto &&geometry_node = message.String();
        auto &&new_model_name = message.String();

        if (auto *node = FindNode(geometry_node.c_str()))
        {
            node->SubstituteGeometry(new_model_name);
        }
        else
        {
            spdlog::trace("MODELR: Cannot substitute geometry node {}", geometry_node);
        }
        break;
    }

    case MSG_MODEL_PLAY_ACTION: {
        if (!ani)
        {
            return 0;
        }

        auto &player = ani->Player(message.Long());
        player.SetAction(message.String().c_str());
        player.Play();
        break;
    }
    }
    return 1;
}

NODE *MODELR::GetNode(int32_t n)
{
    return root->GetNode(n);
}

NODE *MODELR::FindNode(const char *cNodeName)
{
    return root->FindNode(cNodeName);
}

void MODELR::Update()
{
    CVECTOR tmp;
    static_cast<NODER *>(root)->Update(mtx, tmp);
}

//-------------------------------------------------------------------
// clip functions
//-------------------------------------------------------------------
const PLANE *clip_p;
const CVECTOR *clip_c;
float clip_r;
ADD_POLYGON_FUNC clip_ap;
GEOS::ADD_POLYGON_FUNC clip_geosap;
int32_t clip_nps;

bool AddPolygon(const GEOS::VERTEX *vr, int32_t nv);
//-------------------------------------------------------------------
bool MODELR::Clip(const PLANE *planes, int32_t nplanes, const CVECTOR &center, float radius, ADD_POLYGON_FUNC addpoly)
{
    clip_p = planes;
    clip_nps = nplanes;
    clip_c = &center;
    clip_r = radius;
    clip_ap = addpoly;
    if (clip_ap == nullptr)
        clip_geosap = nullptr;
    else
        clip_geosap = AddPolygon;
    return root->Clip();
}

extern NODE *bestTraceNode;
//-------------------------------------------------------------------
const char *MODELR::GetCollideMaterialName()
{
    GEOS::TRACE_INFO ti;
    if (colideNode->geo->GetCollisionDetails(ti) == false)
        return "";
    GEOS::OBJECT go;
    colideNode->geo->GetObj(ti.obj, go);
    static GEOS::MATERIAL gm;
    colideNode->geo->GetMaterial(go.material, gm);
    return gm.name;
}

//-------------------------------------------------------------------
bool MODELR::GetCollideTriangle(TRIANGLE &triangle)
{
    GEOS::TRACE_INFO ti;
    if (colideNode->geo->GetCollisionDetails(ti) == false)
        return false;
    triangle.vrt[0] = colideNode->glob_mtx * CVECTOR(ti.vrt[0].x, ti.vrt[0].y, ti.vrt[0].z);
    triangle.vrt[1] = colideNode->glob_mtx * CVECTOR(ti.vrt[1].x, ti.vrt[1].y, ti.vrt[1].z);
    triangle.vrt[2] = colideNode->glob_mtx * CVECTOR(ti.vrt[2].x, ti.vrt[2].y, ti.vrt[2].z);
    return true;
}

//-------------------------------------------------------------------
CVECTOR cold[1024 * 1024];

float MODELR::Trace(const CVECTOR &src, const CVECTOR &dst)
{
    // collision with skinned geometry
    if (ani)
    {
        // check for bounding spheres intersection
        CVECTOR lmn = dst - src;
        float dist2ray2 = ~((root->glob_mtx * root->center - src) ^ lmn);
        float dlmn = ~(lmn);
        // hierarchy test
        if (dist2ray2 > dlmn * root->radius * root->radius)
            return 2.0f;
        // if(core.Controls->GetAsyncKeyState(0xC0)>=0)    return 2.0f;
        // if(core.Controls->GetAsyncKeyState(VK_SHIFT)<0 && dist2ray2 > dlmn*root->radius*root->radius)    return 2.0f;

        // get bones
        bones = &ani->GetAnimationMatrix(0);

        CVECTOR _src, _dst;
        root->glob_mtx.MulToInv(src, _src);
        root->glob_mtx.MulToInv(dst, _dst);
        CVECTOR dirvec = _dst - _src;

        float bd = 2.0f;
        GEOS::INFO gi;

        // load indices
        if (idxBuff == nullptr)
        {
            auto *idx = static_cast<unsigned short *>(rs->LockIndexBuffer(root->geo->GetIndexBuffer()));
            if (idx == nullptr)
                return 0.;

            int nt = 0;
            root->geo->GetInfo(gi);
            for (int32_t vb = 0; vb < gi.nvrtbuffs; vb++)
            {
                int32_t avb = root->geo->GetVertexBuffer(vb);
                VGEOMETRY::ANIMATION_VB gavb = GeometyService->GetAnimationVBDesc(avb);
                auto *gsrc = static_cast<GEOS::AVERTEX0 *>(gavb.buff);

                // for all objects that refers to this vertexBuffer
                for (int32_t o = 0; o < gi.nobjects; o++)
                {
                    GEOS::OBJECT go;
                    root->geo->GetObj(o, go);
                    if (go.vertex_buff != static_cast<uint32_t>(avb))
                        continue;

                    nt += go.ntriangles;
                }
            }

            idxBuff = new unsigned short[nt * 3];
            for (int32_t vb = 0; vb < gi.nvrtbuffs; vb++)
            {
                int32_t avb = root->geo->GetVertexBuffer(vb);
                VGEOMETRY::ANIMATION_VB gavb = GeometyService->GetAnimationVBDesc(avb);
                auto *gsrc = static_cast<GEOS::AVERTEX0 *>(gavb.buff);

                // for all objects that refers to this vertexBuffer
                for (int32_t o = 0; o < gi.nobjects; o++)
                {
                    GEOS::OBJECT go;
                    root->geo->GetObj(o, go);
                    if (go.vertex_buff != static_cast<uint32_t>(avb))
                        continue;

                    // for all triangles in object
                    for (int32_t gt = 0; gt < go.ntriangles; gt++)
                    {
                        int32_t t = gt * 3 + go.striangle;
                        // Tomas Moller and Ben Trumbore algorithm
                        idxBuff[t + 0] = idx[t + 0];
                        idxBuff[t + 1] = idx[t + 1];
                        idxBuff[t + 2] = idx[t + 2];
                    }
                }
            }
            rs->UnLockIndexBuffer(root->geo->GetIndexBuffer());
        }

        root->geo->GetInfo(gi);
        for (int32_t vb = 0; vb < gi.nvrtbuffs; vb++)
        {
            int32_t avb = root->geo->GetVertexBuffer(vb);
            VGEOMETRY::ANIMATION_VB gavb = GeometyService->GetAnimationVBDesc(avb);
            auto *gsrc = static_cast<GEOS::AVERTEX0 *>(gavb.buff);

            // transform vertices and trace
            for (int32_t v = 0; v < gavb.nvertices; v++)
            {
                cold[v] = *(CVECTOR *)(&gsrc->pos);
                CMatrix &m1 = bones[gsrc->boneid & 0xff];
                CMatrix &m2 = bones[(gsrc->boneid >> 8) & 0xff];

                CVECTOR pos(*(CVECTOR *)(&gsrc->pos));
                CVECTOR v1 = m1 * pos;
                CVECTOR v2 = m2 * pos;
                cold[v].x = -(gsrc->weight * v1.x + (1.0f - gsrc->weight) * v2.x);
                cold[v].y = gsrc->weight * v1.y + (1.0f - gsrc->weight) * v2.y;
                cold[v].z = gsrc->weight * v1.z + (1.0f - gsrc->weight) * v2.z;
                gsrc++;
            }

            // for all objects that refers to this vertexBuffer
            for (int32_t o = 0; o < gi.nobjects; o++)
            {
                GEOS::OBJECT go;
                root->geo->GetObj(o, go);
                if (go.vertex_buff != static_cast<uint32_t>(avb))
                    continue;

                // for all triangles in object
                for (int32_t gt = 0; gt < go.ntriangles; gt++)
                {
                    int32_t t = gt * 3 + go.striangle;
                    // Tomas Moller and Ben Trumbore algorithm
                    CVECTOR a = cold[idxBuff[t + 1] + go.start_vertex] - cold[idxBuff[t + 0] + go.start_vertex];
                    CVECTOR b = cold[idxBuff[t + 2] + go.start_vertex] - cold[idxBuff[t + 0] + go.start_vertex];
                    CVECTOR pvec = dirvec ^ b;
                    float det = a | pvec;
                    float invdet = 1.0f / det;

                    CVECTOR c = _src - cold[idxBuff[t + 0] + go.start_vertex];
                    float U = (c | pvec) * invdet;
                    if (U < 0.0f || U > 1.0)
                        continue;

                    CVECTOR qvec = c ^ a;
                    float V = (dirvec | qvec) * invdet;
                    if (V < 0.0f || U + V > 1.0)
                        continue;

                    float d = (b | qvec) * invdet;
                    if (d >= 0.0f && d <= 1.0 && d < bd)
                    {
                        bd = d;
                    }
                }
            }
        }

        return bd;
    }
    float d = root->Trace(src, dst);
    colideNode = bestTraceNode;
    return d;
}

//-------------------------------------------------------------------
NODE *MODELR::GetCollideNode()
{
    return colideNode;
}

void MODELR::FindPlanes(const CMatrix &view, const CMatrix &proj)
{
    CVECTOR v[4];
    // left
    v[0].x = proj.m[0][0];
    v[0].y = 0.0f;
    v[0].z = 1.0f;
    // right
    v[1].x = -proj.m[0][0];
    v[1].y = 0.0f;
    v[1].z = 1.0f;
    // top
    v[2].x = 0.0f;
    v[2].y = -proj.m[1][1];
    v[2].z = 1.0f;
    // bottom
    v[3].x = 0.0f;
    v[3].y = proj.m[1][1];
    v[3].z = 1.0f;
    v[0] = !v[0];
    v[1] = !v[1];
    v[2] = !v[2];
    v[3] = !v[3];

    CVECTOR pos;

    pos.x = -view.m[3][0] * view.m[0][0] - view.m[3][1] * view.m[0][1] - view.m[3][2] * view.m[0][2];
    pos.y = -view.m[3][0] * view.m[1][0] - view.m[3][1] * view.m[1][1] - view.m[3][2] * view.m[1][2];
    pos.z = -view.m[3][0] * view.m[2][0] - view.m[3][1] * view.m[2][1] - view.m[3][2] * view.m[2][2];

    ViewPlane[0].nrm.x = v[0].x * view.m[0][0] + v[0].y * view.m[0][1] + v[0].z * view.m[0][2];
    ViewPlane[0].nrm.y = v[0].x * view.m[1][0] + v[0].y * view.m[1][1] + v[0].z * view.m[1][2];
    ViewPlane[0].nrm.z = v[0].x * view.m[2][0] + v[0].y * view.m[2][1] + v[0].z * view.m[2][2];

    ViewPlane[1].nrm.x = v[1].x * view.m[0][0] + v[1].y * view.m[0][1] + v[1].z * view.m[0][2];
    ViewPlane[1].nrm.y = v[1].x * view.m[1][0] + v[1].y * view.m[1][1] + v[1].z * view.m[1][2];
    ViewPlane[1].nrm.z = v[1].x * view.m[2][0] + v[1].y * view.m[2][1] + v[1].z * view.m[2][2];

    ViewPlane[2].nrm.x = v[2].x * view.m[0][0] + v[2].y * view.m[0][1] + v[2].z * view.m[0][2];
    ViewPlane[2].nrm.y = v[2].x * view.m[1][0] + v[2].y * view.m[1][1] + v[2].z * view.m[1][2];
    ViewPlane[2].nrm.z = v[2].x * view.m[2][0] + v[2].y * view.m[2][1] + v[2].z * view.m[2][2];

    ViewPlane[3].nrm.x = v[3].x * view.m[0][0] + v[3].y * view.m[0][1] + v[3].z * view.m[0][2];
    ViewPlane[3].nrm.y = v[3].x * view.m[1][0] + v[3].y * view.m[1][1] + v[3].z * view.m[1][2];
    ViewPlane[3].nrm.z = v[3].x * view.m[2][0] + v[3].y * view.m[2][1] + v[3].z * view.m[2][2];

    ViewPlane[0].nrm.x = -ViewPlane[0].nrm.x;
    ViewPlane[0].nrm.y = -ViewPlane[0].nrm.y;
    ViewPlane[0].nrm.z = -ViewPlane[0].nrm.z;
    ViewPlane[1].nrm.x = -ViewPlane[1].nrm.x;
    ViewPlane[1].nrm.y = -ViewPlane[1].nrm.y;
    ViewPlane[1].nrm.z = -ViewPlane[1].nrm.z;
    ViewPlane[2].nrm.x = -ViewPlane[2].nrm.x;
    ViewPlane[2].nrm.y = -ViewPlane[2].nrm.y;
    ViewPlane[2].nrm.z = -ViewPlane[2].nrm.z;
    ViewPlane[3].nrm.x = -ViewPlane[3].nrm.x;
    ViewPlane[3].nrm.y = -ViewPlane[3].nrm.y;
    ViewPlane[3].nrm.z = -ViewPlane[3].nrm.z;

    ViewPlane[0].d = (pos.x * ViewPlane[0].nrm.x + pos.y * ViewPlane[0].nrm.y + pos.z * ViewPlane[0].nrm.z);
    ViewPlane[1].d = (pos.x * ViewPlane[1].nrm.x + pos.y * ViewPlane[1].nrm.y + pos.z * ViewPlane[1].nrm.z);
    ViewPlane[2].d = (pos.x * ViewPlane[2].nrm.x + pos.y * ViewPlane[2].nrm.y + pos.z * ViewPlane[2].nrm.z);
    ViewPlane[3].d = (pos.x * ViewPlane[3].nrm.x + pos.y * ViewPlane[3].nrm.y + pos.z * ViewPlane[3].nrm.z);
}

void MODELR::LostRender()
{
    root->ReleaseGeometry();
    rs->Release(d3dDestVB);
}

void MODELR::RestoreRender()
{
    root->RestoreGeometry();
    const int32_t fvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEXTUREFORMAT2 | D3DFVF_TEX1;
    if (nAniVerts)
        rs->CreateVertexBuffer(sizeof(GEOS::VERTEX0) * nAniVerts, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, fvf,
                               D3DPOOL_DEFAULT, &d3dDestVB);
}
