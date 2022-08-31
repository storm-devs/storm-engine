#include "core.h"

#include "entity.h"
#include "math_inlines.h"
#include "sea.h"
#include "shared/messages.h"

#include "shared/layers.h"
#include "math3d/plane.h"

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

    const Vector point(0, PlaneHeight, 0), normal(0, 1, 0);
    auto mPlane = Plane(normal, point);

    CMatrix Invertor;
    Invertor.BuildMirrorMatrix(mPlane.N.x, mPlane.N.y, mPlane.N.z, mPlane.D);

    auto mView = rs->GetView();
    auto mViewNew = mView;

    mViewNew = Invertor * mViewNew;

    rs->SetView(mViewNew);

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

        if (!core.IsLayerFrozen(SEA_REFLECTION2))
        {
            auto &&entities = core.GetEntityIds(SEA_SUNROAD);
            for (auto ent_id : entities)
            {
                const auto hash = core.GetClassCode(ent_id);
                if (hash != dwShipCode && hash != dwSailCode && hash != dwIslandCode)
                {
                    core.Send_Message(ent_id, "ll", MSG_SEA_SUNROAD_DRAW, static_cast<int32_t>(bSimpleSea));
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

    const Vector point(0, PlaneHeight, 0), normal(0, 1, 0);
    auto mPlane = Plane(normal, point);

    CMatrix Invertor;
    Invertor.BuildMirrorMatrix(mPlane.N.x, mPlane.N.y, mPlane.N.z, mPlane.D);

    auto mView = rs->GetView();
    CMatrix mViewNew = mView;

    mViewNew = Invertor * mViewNew;

    rs->SetView(mViewNew);

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
        {
            auto &&entities = core.GetEntityIds(SEA_REFLECTION);
            for (auto ent_id : entities)
            {
                core.Send_Message(ent_id, "ll", MSG_SEA_REFLECTION_DRAW, static_cast<int32_t>(bSimpleSea));
            }
        }
        {
            auto &&entities = core.GetEntityIds(SEA_REFLECTION2);
            for (auto ent_id : entities)
            {
                core.Send_Message(ent_id, "ll", MSG_SEA_REFLECTION_DRAW, static_cast<int32_t>(bSimpleSea));
            }
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
        if (!core.IsLayerFrozen(SEA_REFLECTION2))
        {
            auto &&entities = core.GetEntityIds(SEA_SUNROAD);
            for (auto ent_id : entities)
            {
                const auto hash = core.GetClassCode(ent_id);
                if (hash != dwShipCode && hash != dwSailCode && hash != dwIslandCode)
                {
                    core.Send_Message(ent_id, "ll", MSG_SEA_SUNROAD_DRAW, static_cast<int32_t>(bSimpleSea));
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
        {
            auto &&entities = core.GetEntityIds(SEA_REFLECTION);
            for (auto ent_id : entities)
            {
                core.Send_Message(ent_id, "ll", MSG_SEA_REFLECTION_DRAW, static_cast<int32_t>(bSimpleSea));
            }
        }
        {
            auto &&entities = core.GetEntityIds(SEA_REFLECTION2);
            for (auto ent_id : entities)
            {
                core.Send_Message(ent_id, "ll", MSG_SEA_REFLECTION_DRAW, static_cast<int32_t>(bSimpleSea));
            }
        }

        rs->EndScene();
    }

    rs->PopRenderTarget();
    rs->SetView(mOldView);
    rs->SetProjection(mOldProjection);

    rs->BeginScene();

    return true;
}
