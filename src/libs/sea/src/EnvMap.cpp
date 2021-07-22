#include "core.h"

#include "Entity.h"
#include "sea.h"
#include "shared/messages.h"

#include "shared/layers.h"

void SEA::EnvMap_GetSideMatrix(D3DCUBEMAP_FACES Face, CMatrix &mView)
{
    CVECTOR vLookTo, vUp;

    switch (Face)
    {
    case D3DCUBEMAP_FACE_POSITIVE_X:
        vLookTo = CVECTOR(1.0f, 0.0f, 0.0f);
        vUp = CVECTOR(0.0f, 1.0f, 0.0f);
        break;
    case D3DCUBEMAP_FACE_NEGATIVE_X:
        vLookTo = CVECTOR(-1.0f, 0.0f, 0.0f);
        vUp = CVECTOR(0.0f, 1.0f, 0.0f);
        break;
    case D3DCUBEMAP_FACE_POSITIVE_Y:
        vLookTo = CVECTOR(0.0f, 1.0f, 0.0f);
        vUp = CVECTOR(0.0f, 0.0f, -1.0f);
        break;
    case D3DCUBEMAP_FACE_NEGATIVE_Y:
        vLookTo = CVECTOR(0.0f, -1.0f, 0.0f);
        vUp = CVECTOR(0.0f, 0.0f, 1.0f);
        break;
    case D3DCUBEMAP_FACE_POSITIVE_Z:
        vLookTo = CVECTOR(0.0f, 0.0f, 1.0f);
        vUp = CVECTOR(0.0f, 1.0f, 0.0f);
        break;
    case D3DCUBEMAP_FACE_NEGATIVE_Z:
        vLookTo = CVECTOR(0.0f, 0.0f, -1.0f);
        vUp = CVECTOR(0.0f, 1.0f, 0.0f);
        break;
    }

    mView.BuildViewMatrix(0.0f, vLookTo, vUp);
}

bool SEA::SunRoad_Render2()
{
    auto mOldView = rs->GetView();
    auto mOldProjection = rs->GetProjection();

    rs->EndScene();
    rs->PushRenderTarget();

    // rs->SetProjection( CMatrix().BuildProjectionMatrix(PI / 2.0f, 256.0f, 256.0f, 1.0f, 4000.0f) );

    auto PlaneHeight = 0.5f;

    D3DXPLANE plane;
    const D3DXVECTOR3 point(0, PlaneHeight, 0), normal(0, 1, 0);
    D3DXPlaneFromPointNormal(&plane, &point, &normal);

    D3DXMATRIX matReflect;
    D3DXMatrixReflect(&matReflect, &plane);

    auto mView = rs->GetView();

    auto mViewNew = mView;

    CMatrix Invertor;
    memcpy(Invertor.m, matReflect.m, sizeof(float) * 16);
    mViewNew = Invertor * mViewNew;

    rs->SetView(mViewNew);

    auto _mWorld = CMatrix();
    auto _mView = rs->GetView();
    auto _mProj = rs->GetProjection();
    auto _mWorldView = _mWorld * _mView;
    auto _mWorldViewProj = _mWorldView * _mProj;

    D3DXMATRIX mInv;
    memcpy(mInv.m, _mWorldViewProj.m, sizeof(float) * 16);
    D3DXMatrixInverse(&mInv, nullptr, &mInv);
    D3DXMatrixTranspose(&mInv, &mInv);
    D3DXPlaneTransform(&plane, &plane, &mInv);
    rs->SetClipPlane(0, (FLOAT *)&plane);

    uint32_t Colors[6] = {0xd934c8, 0x2FFF1F, 0x0000FF, 0xFF00, 0xb28e11, 0x0};
    // for (uint32_t i=0; i<6; i++)
    {
        // if (i == D3DCUBEMAP_FACE_NEGATIVE_Y) continue;
        // if (i != D3DCUBEMAP_FACE_NEGATIVE_Z) continue;

        IDirect3DSurface9 *pReflectionSurface;
        pReflectionSunroad->GetSurfaceLevel(0, &pReflectionSurface);

        rs->BeginScene();
        bool bCool = rs->SetRenderTarget(pReflectionSurface, pZStencil);
        bCool = D3D_OK != rs->Clear(0, nullptr, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0x0, 1.0f, 0);

        // CMatrix mView;
        // EnvMap_GetSideMatrix((D3DCUBEMAP_FACES)i, mView);

        // mView.m[3][0] = -(mView.m[0][0] * vCamPos.x - mView.m[1][0] * vCamPos.y + mView.m[2][0] * vCamPos.z);
        // mView.m[3][1] = -(mView.m[0][1] * vCamPos.x - mView.m[1][1] * vCamPos.y + mView.m[2][1] * vCamPos.z);
        // mView.m[3][2] = -(mView.m[0][2] * vCamPos.x - mView.m[1][2] * vCamPos.y + mView.m[2][2] * vCamPos.z);

        // rs->SetView(mView);

        // Render scene here.
        // uint32_t dwSkyCode = MakeHashValue("sky");

        if (!EntityManager::IsLayerFrozen(SEA_REFLECTION2))
        {
            const auto its = EntityManager::GetEntityIdIterators(SEA_SUNROAD);
            for (auto it = its.first; it != its.second; ++it)
            {
                const auto ent_id = it->second;
                const auto hash = EntityManager::GetClassCode(ent_id);
                if (hash != dwShipCode && hash != dwSailCode && hash != dwIslandCode)
                {
                    core.Send_Message(ent_id, "ll", MSG_SEA_SUNROAD_DRAW, static_cast<long>(bSimpleSea));
                }
            }
        }

        rs->EndScene();
        pReflectionSurface->Release();
    }

    rs->PopRenderTarget();
    rs->SetView(mOldView);
    rs->SetProjection(mOldProjection);

    rs->BeginScene();
    mOldView = rs->GetView();

    return true;
}

bool SEA::EnvMap_Render2()
{
    auto mOldView = rs->GetView();
    auto mOldProjection = rs->GetProjection();
    rs->EndScene();
    rs->PushRenderTarget();

    // rs->SetProjection( CMatrix().BuildProjectionMatrix(PI / 2.0f, 256.0f, 256.0f, 1.0f, 4000.0f) );

    auto PlaneHeight = 0.5f;

    D3DXPLANE plane;
    const D3DXVECTOR3 point(0, PlaneHeight, 0), normal(0, 1, 0);
    D3DXPlaneFromPointNormal(&plane, &point, &normal);

    D3DXMATRIX matReflect;
    D3DXMatrixReflect(&matReflect, &plane);

    auto mView = rs->GetView();

    CMatrix mViewNew = mView;

    CMatrix Invertor;
    memcpy(Invertor.m, matReflect.m, sizeof(float) * 16);
    mViewNew = Invertor * mViewNew;

    rs->SetView(mViewNew);

    auto _mWorld = CMatrix();
    CMatrix _mView = rs->GetView();
    CMatrix _mProj = rs->GetProjection();
    CMatrix _mWorldView = _mWorld * _mView;
    CMatrix _mWorldViewProj = _mWorldView * _mProj;

    D3DXMATRIX mInv;
    memcpy(mInv.m, _mWorldViewProj.m, sizeof(float) * 16);
    D3DXMatrixInverse(&mInv, nullptr, &mInv);
    D3DXMatrixTranspose(&mInv, &mInv);
    D3DXPlaneTransform(&plane, &plane, &mInv);
    rs->SetClipPlane(0, (FLOAT *)&plane);
    // rs->SetEffect("FlatSeaReverseCull");
    // Event("SeaReflection");

    uint32_t Colors[6] = {0xd934c8, 0x2FFF1F, 0x0000FF, 0xFF00, 0xb28e11, 0x0};
    // for (uint32_t i=0; i<6; i++)
    {
        // if (i == D3DCUBEMAP_FACE_NEGATIVE_Y) continue;
        // if (i != D3DCUBEMAP_FACE_NEGATIVE_Z) continue;

        IDirect3DSurface9 *pReflectionSurface;
        pReflection->GetSurfaceLevel(0, &pReflectionSurface);

        rs->BeginScene();
        // bool bCool = rs->SetRenderTarget(pEnvMap, i, 0, pZStencil);
        // bool bCool = rs->SetRenderTarget(pEnvMap, i, 0, pZStencil);
        bool bCool = rs->SetRenderTarget(pReflectionSurface, pReflectionSurfaceDepth);
        bCool = D3D_OK != rs->Clear(0, nullptr, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0x0, 1.0f, 0);

        // CMatrix mView;
        // EnvMap_GetSideMatrix((D3DCUBEMAP_FACES)i, mView);

        // mView.m[3][0] = -(mView.m[0][0] * vCamPos.x - mView.m[1][0] * vCamPos.y + mView.m[2][0] * vCamPos.z);
        // mView.m[3][1] = -(mView.m[0][1] * vCamPos.x - mView.m[1][1] * vCamPos.y + mView.m[2][1] * vCamPos.z);
        // mView.m[3][2] = -(mView.m[0][2] * vCamPos.x - mView.m[1][2] * vCamPos.y + mView.m[2][2] * vCamPos.z);

        // core.Trace("sea: %.3f, %.3f, %.3f", mView.m[3][0], mView.m[3][1], mView.m[3][2]);
        // rs->SetView(mView);

        // Render scene here.
        auto its = EntityManager::GetEntityIdIterators(SEA_REFLECTION);
        for (auto it = its.first; it != its.second; ++it)
        {
            core.Send_Message(it->second, "ll", MSG_SEA_REFLECTION_DRAW, static_cast<long>(bSimpleSea));
        }

        its = EntityManager::GetEntityIdIterators(SEA_REFLECTION2);
        for (auto it = its.first; it != its.second; ++it)
        {
            core.Send_Message(it->second, "ll", MSG_SEA_REFLECTION_DRAW, static_cast<long>(bSimpleSea));
        }

        rs->EndScene();
        pReflectionSurface->Release();
    }

    rs->PopRenderTarget();
    rs->SetView(mOldView);
    rs->SetProjection(mOldProjection);

    rs->BeginScene();

    CMatrix mTex;
    mTex.BuildScale(CVECTOR(0.5f, -0.5f, 0.5f));
    mTex.pos = CVECTOR(0.5f, 0.5f, 0.5f);

    CMatrix mProj = rs->GetProjection();
    mProj = mProj * mTex;

    CMatrix mCurWorld = rs->GetWorld();
    CMatrix mCurView = rs->GetView();

    mTexProjection = (mCurWorld * mCurView) * mProj;

    return true;
}

bool SEA::SunRoad_Render()
{
    CMatrix mOldView = rs->GetView();
    CMatrix mOldProjection = rs->GetProjection();

    rs->EndScene();
    rs->PushRenderTarget();

    rs->SetProjection(CMatrix().BuildProjectionMatrix(PI / 2.0f, 256.0f, 256.0f, 1.0f, 4000.0f));

    uint32_t Colors[6] = {0xd934c8, 0x2FFF1F, 0x0000FF, 0xFF00, 0xb28e11, 0x0};
    for (uint32_t i = 0; i < 6; i++)
    {
        if (!bUnderSea && i == D3DCUBEMAP_FACE_NEGATIVE_Y)
            continue;

        rs->BeginScene();
        bool bCool = rs->SetRenderTarget(pSunRoadMap, i, 0, pZStencil);
        bCool = D3D_OK != rs->Clear(0, nullptr, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0x0, 1.0f, 0);

        CMatrix mView;
        EnvMap_GetSideMatrix(static_cast<D3DCUBEMAP_FACES>(i), mView);

        mView.m[3][0] = -(mView.m[0][0] * vCamPos.x - mView.m[1][0] * vCamPos.y + mView.m[2][0] * vCamPos.z);
        mView.m[3][1] = -(mView.m[0][1] * vCamPos.x - mView.m[1][1] * vCamPos.y + mView.m[2][1] * vCamPos.z);
        mView.m[3][2] = -(mView.m[0][2] * vCamPos.x - mView.m[1][2] * vCamPos.y + mView.m[2][2] * vCamPos.z);

        rs->SetView(mView);

        // Render scene here.
        if (!EntityManager::IsLayerFrozen(SEA_REFLECTION2))
        {
            const auto its = EntityManager::GetEntityIdIterators(SEA_SUNROAD);
            for (auto it = its.first; it != its.second; ++it)
            {
                const auto ent_id = it->second;
                const auto hash = EntityManager::GetClassCode(ent_id);
                if (hash != dwShipCode && hash != dwSailCode && hash != dwIslandCode)
                {
                    core.Send_Message(ent_id, "ll", MSG_SEA_SUNROAD_DRAW, static_cast<long>(bSimpleSea));
                }
            }
        }

        rs->EndScene();
    }

    rs->PopRenderTarget();
    rs->SetView(mOldView);
    rs->SetProjection(mOldProjection);

    rs->BeginScene();
    mOldView = rs->GetView();

    return true;
}

bool SEA::EnvMap_Render()
{
    CMatrix mOldView = rs->GetView();
    CMatrix mOldProjection = rs->GetProjection();
    rs->EndScene();
    rs->PushRenderTarget();

    rs->SetProjection(CMatrix().BuildProjectionMatrix(PI / 2.0f, 256.0f, 256.0f, 1.0f, 4000.0f));

    uint32_t Colors[6] = {0xd934c8, 0x2FFF1F, 0x0000FF, 0xFF00, 0xb28e11, 0x0};
    for (uint32_t i = 0; i < 6; i++)
    {
        if (!bUnderSea && i == D3DCUBEMAP_FACE_NEGATIVE_Y)
            continue;
        // if (i != D3DCUBEMAP_FACE_NEGATIVE_Z) continue;

        rs->BeginScene();
        bool bCool = rs->SetRenderTarget(pEnvMap, i, 0, pZStencil);
        bCool = D3D_OK != rs->Clear(0, nullptr, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, Colors[i] /*0x0*/, 1.0f, 0);

        CMatrix mView;
        EnvMap_GetSideMatrix(static_cast<D3DCUBEMAP_FACES>(i), mView);

        mView.m[3][0] = -(mView.m[0][0] * vCamPos.x - mView.m[1][0] * vCamPos.y + mView.m[2][0] * vCamPos.z);
        mView.m[3][1] = -(mView.m[0][1] * vCamPos.x - mView.m[1][1] * vCamPos.y + mView.m[2][1] * vCamPos.z);
        mView.m[3][2] = -(mView.m[0][2] * vCamPos.x - mView.m[1][2] * vCamPos.y + mView.m[2][2] * vCamPos.z);

        // core.Trace("sea: %.3f, %.3f, %.3f", mView.m[3][0], mView.m[3][1], mView.m[3][2]);
        rs->SetView(mView);

        // Render scene here.
        auto its = EntityManager::GetEntityIdIterators(SEA_REFLECTION);
        for (auto it = its.first; it != its.second; ++it)
        {
            core.Send_Message(it->second, "ll", MSG_SEA_REFLECTION_DRAW, static_cast<long>(bSimpleSea));
        }

        its = EntityManager::GetEntityIdIterators(SEA_REFLECTION2);
        for (auto it = its.first; it != its.second; ++it)
        {
            core.Send_Message(it->second, "ll", MSG_SEA_REFLECTION_DRAW, static_cast<long>(bSimpleSea));
        }

        rs->EndScene();
    }

    rs->PopRenderTarget();
    rs->SetView(mOldView);
    rs->SetProjection(mOldProjection);

    rs->BeginScene();

    return true;
}
