#include "Sea.h"

void NetSea::EnvMap_GetSideMatrix(D3DCUBEMAP_FACES Face, CMatrix & mView)
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
            vUp = CVECTOR(0.0f, 0.0f,-1.0f);
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Y:
            vLookTo = CVECTOR(0.0f,-1.0f, 0.0f);
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

bool NetSea::SunRoad_Render()
{
	CMatrix mOldView = Render().GetView();
	CMatrix mOldProjection = Render().GetProjection();

	Render().EndScene();
	Render().PushRenderTarget();

	Render().SetProjection( CMatrix().BuildProjectionMatrix(PI / 2.0f, 256.0f, 256.0f, 1.0f, 4000.0f) );

	dword dwSkyCode = api->Class_Name2Code("sky");
	dword dwShipCode = api->Class_Name2Code("ship");
	dword dwSailCode = api->Class_Name2Code("sail");

	bool bLayerFrozen = (api->LayerGetFlags("sea_reflection") & LRFLAG_FROZEN) != 0;

	dword Colors[6] = {0xd934c8, 0x2FFF1F, 0x0000FF, 0xFF00, 0xb28e11, 0x0};
	for (dword i=0; i<6; i++) 
	{
		if (i == D3DCUBEMAP_FACE_NEGATIVE_Y) continue;

		Render().BeginScene();
		bool bCool = Render().SetRenderTarget(pSunRoadMap, i, 0, pZStencil);
		bCool = D3D_OK != Render().Clear(0, null, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0x0, 1.0f, 0);

		CMatrix mView;
		EnvMap_GetSideMatrix((D3DCUBEMAP_FACES)i, mView);

		mView.m[3][0] = -(mView.m[0][0] * vCamPos.x - mView.m[1][0] * vCamPos.y + mView.m[2][0] * vCamPos.z);
		mView.m[3][1] = -(mView.m[0][1] * vCamPos.x - mView.m[1][1] * vCamPos.y + mView.m[2][1] * vCamPos.z);
		mView.m[3][2] = -(mView.m[0][2] * vCamPos.x - mView.m[1][2] * vCamPos.y + mView.m[2][2] * vCamPos.z);

		Render().SetView(mView);

		ENTITY_ID ent_id;
		// Render scene here.
		if (api->SetEntityScanLayer("sea_sunroad") && api->GetEntity(&ent_id))
			do 
			{
				dword dwCCode = ent_id.class_code;
				if (!bLayerFrozen || (dwCCode != dwShipCode && dwCCode != dwSailCode)) 
				{
					api->Send_Message(ent_id, "ll", MSG_SEA_SUNROAD_DRAW, long(bSimpleSea));
				}
			} while (api->GetEntityNext(&ent_id));

		Render().EndScene();
	}

	Render().PopRenderTarget();
	Render().SetView(mOldView);
	Render().SetProjection(mOldProjection);

	Render().BeginScene();
	mOldView = Render().GetView();

	return true;
}

bool NetSea::EnvMap_Render()
{
	CMatrix mOldView = Render().GetView();
	CMatrix mOldProjection = Render().GetProjection();
	Render().EndScene();
	Render().PushRenderTarget();

	Render().SetProjection( CMatrix().BuildProjectionMatrix(PI / 2.0f, 256.0f, 256.0f, 1.0f, 4000.0f) );

	dword dwSkyCode = api->Class_Name2Code("sky");
	dword dwShipCode = api->Class_Name2Code("ship");
	dword dwSailCode = api->Class_Name2Code("sail");

	bool bLayerFrozen = (api->LayerGetFlags("sea_reflection") & LRFLAG_FROZEN) != 0;

	dword Colors[6] = {0xd934c8, 0x2FFF1F, 0x0000FF, 0xFF00, 0xb28e11, 0x0};
	for (dword i=0; i<6; i++) 
	{
		if (i == D3DCUBEMAP_FACE_NEGATIVE_Y) continue;
		//if (i != D3DCUBEMAP_FACE_NEGATIVE_Z) continue;

		Render().BeginScene();
		bool bCool = Render().SetRenderTarget(pEnvMap, i, 0, pZStencil);
		bCool = D3D_OK != Render().Clear(0, null, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, Colors[i]/*0x0*/, 1.0f, 0);

		CMatrix mView;
		EnvMap_GetSideMatrix((D3DCUBEMAP_FACES)i, mView);

		mView.m[3][0] = -(mView.m[0][0] * vCamPos.x - mView.m[1][0] * vCamPos.y + mView.m[2][0] * vCamPos.z);
		mView.m[3][1] = -(mView.m[0][1] * vCamPos.x - mView.m[1][1] * vCamPos.y + mView.m[2][1] * vCamPos.z);
		mView.m[3][2] = -(mView.m[0][2] * vCamPos.x - mView.m[1][2] * vCamPos.y + mView.m[2][2] * vCamPos.z);
		
		//api->Trace("sea: %.3f, %.3f, %.3f", mView.m[3][0], mView.m[3][1], mView.m[3][2]);
		Render().SetView(mView);

		ENTITY_ID ent_id;
		ENTITY *pSky = 0;
		// Render scene here.
		if (api->SetEntityScanLayer("sea_reflection") && api->GetEntity(&ent_id))
			do 
			{
				dword dwCCode = ent_id.class_code;
				//if (bUnderWater && dwCCode == dwShipCode) continue;
				if (!bLayerFrozen || (dwCCode != dwShipCode && dwCCode != dwSailCode)) 
				{
					api->Send_Message(ent_id, "ll", MSG_SEA_REFLECTION_DRAW, long(bSimpleSea));
				}
			} while (api->GetEntityNext(&ent_id));

		Render().EndScene();
	}

	Render().PopRenderTarget();
	Render().SetView(mOldView);
	Render().SetProjection(mOldProjection);
	
	Render().BeginScene();

	return true;
}

bool NetSea::SunRoad_Render2()
{
	CMatrix mOldView = Render().GetView();
	CMatrix mOldProjection = Render().GetProjection();

	Render().EndScene();
	Render().PushRenderTarget();

	//Render().SetProjection( CMatrix().BuildProjectionMatrix(PI / 2.0f, 256.0f, 256.0f, 1.0f, 4000.0f) );

	float PlaneHeight = 0.5f;

	D3DXPLANE plane;
	D3DXPlaneFromPointNormal( &plane, &D3DXVECTOR3(0, PlaneHeight, 0), &D3DXVECTOR3(0, 1, 0) );

	D3DXMATRIX matReflect;
	D3DXMatrixReflect( &matReflect, &plane );

	CMatrix mView = Render().GetView();

	CMatrix mViewNew = mView;

	CMatrix Invertor;
	memcpy (Invertor.m, matReflect.m, sizeof(float)* 16);
	mViewNew = Invertor * mViewNew;

	Render().SetView(mViewNew);

	CMatrix _mWorld = CMatrix();
	CMatrix _mView = Render().GetView();
	CMatrix _mProj = Render().GetProjection();
	CMatrix _mWorldView = _mWorld * _mView;
	CMatrix _mWorldViewProj = _mWorldView * _mProj;

	D3DXMATRIX mInv;
	memcpy (mInv.m, _mWorldViewProj.m, sizeof(float)* 16);
	D3DXMatrixInverse(&mInv, NULL, &mInv);
	D3DXMatrixTranspose(&mInv, &mInv);
	D3DXPlaneTransform(&plane, &plane, &mInv);
	Render().SetClipPlane (0, (FLOAT *)&plane);

	dword dwSkyCode = api->Class_Name2Code("sky");
	dword dwShipCode = api->Class_Name2Code("ship");
	dword dwSailCode = api->Class_Name2Code("sail");

	bool bLayerFrozen = (api->LayerGetFlags("sea_reflection") & LRFLAG_FROZEN) != 0;

	dword Colors[6] = {0xd934c8, 0x2FFF1F, 0x0000FF, 0xFF00, 0xb28e11, 0x0};
	//for (dword i=0; i<6; i++) 
	{
		//if (i == D3DCUBEMAP_FACE_NEGATIVE_Y) continue;

		IDirect3DSurface8 * pReflectionSurface;
		pReflectionSunroad->GetSurfaceLevel(0, &pReflectionSurface);

		Render().BeginScene();
		bool bCool = Render().SetRenderTarget(pReflectionSurface, pReflectionSurfaceDepth);
		bCool = D3D_OK != Render().Clear(0, null, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0x0, 1.0f, 0);

		//CMatrix mView;
		//EnvMap_GetSideMatrix((D3DCUBEMAP_FACES)i, mView);

		//mView.m[3][0] = -(mView.m[0][0] * vCamPos.x - mView.m[1][0] * vCamPos.y + mView.m[2][0] * vCamPos.z);
		//mView.m[3][1] = -(mView.m[0][1] * vCamPos.x - mView.m[1][1] * vCamPos.y + mView.m[2][1] * vCamPos.z);
		//mView.m[3][2] = -(mView.m[0][2] * vCamPos.x - mView.m[1][2] * vCamPos.y + mView.m[2][2] * vCamPos.z);

		//Render().SetView(mView);

		ENTITY_ID ent_id;
		// Render scene here.
		if (api->SetEntityScanLayer("sea_sunroad") && api->GetEntity(&ent_id))
			do 
			{
				dword dwCCode = ent_id.class_code;
				if (!bLayerFrozen || (dwCCode != dwShipCode && dwCCode != dwSailCode)) 
				{
					api->Send_Message(ent_id, "ll", MSG_SEA_SUNROAD_DRAW, long(bSimpleSea));
				}
			} while (api->GetEntityNext(&ent_id));

		Render().EndScene();
		pReflectionSurface->Release();
	}

	Render().PopRenderTarget();
	Render().SetView(mOldView);
	Render().SetProjection(mOldProjection);

	Render().BeginScene();
	mOldView = Render().GetView();

	return true;
}

bool NetSea::EnvMap_Render2()
{
	CMatrix mOldView = Render().GetView();
	CMatrix mOldProjection = Render().GetProjection();
	Render().EndScene();
	Render().PushRenderTarget();

	//Render().SetProjection( CMatrix().BuildProjectionMatrix(PI / 2.0f, 256.0f, 256.0f, 1.0f, 4000.0f) );

	float PlaneHeight = 0.5f;

	D3DXPLANE plane;
	D3DXPlaneFromPointNormal( &plane, &D3DXVECTOR3(0, PlaneHeight, 0), &D3DXVECTOR3(0, 1, 0) );

	D3DXMATRIX matReflect;
	D3DXMatrixReflect( &matReflect, &plane );

	CMatrix mView = Render().GetView();

	CMatrix mViewNew = mView;

	CMatrix Invertor;
	memcpy (Invertor.m, matReflect.m, sizeof(float)* 16);
	mViewNew = Invertor * mViewNew;

	Render().SetView(mViewNew);

	CMatrix _mWorld = CMatrix();
	CMatrix _mView = Render().GetView();
	CMatrix _mProj = Render().GetProjection();
	CMatrix _mWorldView = _mWorld * _mView;
	CMatrix _mWorldViewProj = _mWorldView * _mProj;

	D3DXMATRIX mInv;
	memcpy (mInv.m, _mWorldViewProj.m, sizeof(float)* 16);
	D3DXMatrixInverse(&mInv, NULL, &mInv);
	D3DXMatrixTranspose(&mInv, &mInv);
	D3DXPlaneTransform(&plane, &plane, &mInv);
	Render().SetClipPlane (0, (FLOAT *)&plane);
	//Render().SetEffect("FlatSeaReverseCull");
	//Event("SeaReflection");



	dword dwSkyCode = api->Class_Name2Code("sky");
	dword dwShipCode = api->Class_Name2Code("ship");
	dword dwSailCode = api->Class_Name2Code("sail");

	bool bLayerFrozen = (api->LayerGetFlags("sea_reflection") & LRFLAG_FROZEN) != 0;

	dword Colors[6] = {0xd934c8, 0x2FFF1F, 0x0000FF, 0xFF00, 0xb28e11, 0x0};
	//for (dword i=0; i<6; i++) 
	{
		//if (i == D3DCUBEMAP_FACE_NEGATIVE_Y) continue;
		//if (i != D3DCUBEMAP_FACE_NEGATIVE_Z) continue;

		IDirect3DSurface8 * pReflectionSurface;
		pReflection->GetSurfaceLevel(0, &pReflectionSurface);

		Render().BeginScene();
		//bool bCool = Render().SetRenderTarget(pEnvMap, i, 0, pZStencil);
		bool bCool = Render().SetRenderTarget(pReflectionSurface, pReflectionSurfaceDepth);
		bCool = D3D_OK != Render().Clear(0, null, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0x0, 1.0f, 0);

		//CMatrix mView;
		//EnvMap_GetSideMatrix((D3DCUBEMAP_FACES)i, mView);

		//mView.m[3][0] = -(mView.m[0][0] * vCamPos.x - mView.m[1][0] * vCamPos.y + mView.m[2][0] * vCamPos.z);
		//mView.m[3][1] = -(mView.m[0][1] * vCamPos.x - mView.m[1][1] * vCamPos.y + mView.m[2][1] * vCamPos.z);
		//mView.m[3][2] = -(mView.m[0][2] * vCamPos.x - mView.m[1][2] * vCamPos.y + mView.m[2][2] * vCamPos.z);
		
		//api->Trace("sea: %.3f, %.3f, %.3f", mView.m[3][0], mView.m[3][1], mView.m[3][2]);
		//Render().SetView(mView);

		ENTITY_ID ent_id;
		ENTITY *pSky = 0;
		// Render scene here.
		if (api->SetEntityScanLayer("sea_reflection") && api->GetEntity(&ent_id))
			do 
			{
				dword dwCCode = ent_id.class_code;
				//if (bUnderWater && dwCCode == dwShipCode) continue;
				if (!bLayerFrozen || (dwCCode != dwShipCode && dwCCode != dwSailCode)) 
				{
					api->Send_Message(ent_id, "ll", MSG_SEA_REFLECTION_DRAW, long(bSimpleSea));
				}
			} while (api->GetEntityNext(&ent_id));

		Render().EndScene();
		pReflectionSurface->Release();
	}

	Render().PopRenderTarget();
	Render().SetView(mOldView);
	Render().SetProjection(mOldProjection);
	
	Render().BeginScene();

	CMatrix mTex;
	mTex.BuildScale(CVECTOR (0.5f, -0.5f, 0.5f));
	mTex.pos = CVECTOR (0.5f, 0.5f, 0.5f);

	CMatrix mProj = Render().GetProjection();
	mProj = mProj * mTex;

	CMatrix mCurWorld = Render().GetWorld();
	CMatrix mCurView = Render().GetView();

	mTexProjection = (mCurWorld * mCurView) * mProj;

	return true;
}