#include "sea.h"

void NetSea::CubeMapGetSideMatrix(long iSide, void *matView)
{
    D3DXVECTOR3 vEnvEyePt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vLookatPt, vUpVec;

    switch( iSide )
    {
        case D3DCUBEMAP_FACE_POSITIVE_X:
            vLookatPt = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
            vUpVec   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_X:
            vLookatPt = D3DXVECTOR3(-1.0f, 0.0f, 0.0f );
            vUpVec   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Y:
            vLookatPt = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            vUpVec   = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Y:
            vLookatPt = D3DXVECTOR3( 0.0f,-1.0f, 0.0f );
            vUpVec   = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Z:
            vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            vUpVec   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Z:
            vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
            vUpVec   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
    }
    D3DXMatrixLookAtLH((D3DXMATRIX*)matView, &vEnvEyePt, &vLookatPt, &vUpVec);
}

void NetSea::CubeMapSaveMatricesAndSetProjection()
{
	HRESULT		hr;
	D3DXMATRIX	matProj;
    pRS->GetTransform(D3DTS_VIEW,       &matViewSave);
    pRS->GetTransform(D3DTS_PROJECTION, &matProjSave);

    hr = pRS->GetRenderTarget( &pMainBackBuffer );
    hr = pRS->GetDepthStencilSurface(&pMainZBuffer);

    // Use 90-degree field of view in the projection.    D3DMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/2, 1.0f, 1.0f, 4000.0f);
    pRS->SetTransform(D3DTS_PROJECTION, &matProj);
}

void NetSea::CubeMapRestoreMatrices()
{
	// Change the render target back to the main back buffer.
    HRESULT	hr = pRS->SetRenderTarget(pMainBackBuffer, pMainZBuffer);
    pMainBackBuffer->Release();    pMainZBuffer->Release();

    // Restore the original transformation matrices.
    pRS->SetTransform(D3DTS_VIEW,       &matViewSave);
    pRS->SetTransform(D3DTS_PROJECTION, &matProjSave);
}

IDirect3DSurface8* NetSea::CubeGetFace(IDirect3DCubeTexture8 * _pCubeMap, long idx)
{
    IDirect3DSurface8 * pFace;
    HRESULT hr = pRS->GetCubeMapSurface(_pCubeMap, (D3DCUBEMAP_FACES)idx, 0, &pFace);
	return pFace;
}

void NetSea::SetRenderTarget(IDirect3DSurface8 * pFace, IDirect3DSurface8 * _pCubeFaceZBuffer)
{
    // Get pointer to surface in order to render to it.
    HRESULT	hr = pRS->SetRenderTarget(pFace, _pCubeFaceZBuffer);
}

// Render scene to reflection cubemap surface
void NetSea::CubeMapCalculateReflectionDynamic()
{
	D3DVIEWPORT8 NewViewport;
	HRESULT hr;
	dword Colors[6] = {0xd934c8, 0x2FFF1F, 0x0000FF, 0xFF00, 0xb28e11, 0x0};
	D3DXMATRIX matView, matCamera;

	pRS->GetTransform(D3DTS_VIEW, &matCamera);
	matCamera._41 = 0.0f;	matCamera._42 = 0.0f;	matCamera._43 = 0.0f;

	CubeMapSaveMatricesAndSetProjection(); 

	dword dwSkyCode = api->Class_Name2Code("sky");
	dword dwShipCode = api->Class_Name2Code("ship");
	dword dwSailCode = api->Class_Name2Code("sail");

	bool bLayerFrozen = (api->LayerGetFlags("sea_reflection") & LRFLAG_FROZEN) != 0;
		
    // Loop through the six faces of the cube map.
	//m_iCubeMapSide = 0;
    for (long i=0; i<6; i++) //if (i==m_iCubeMapSide)
    {
		if (i == D3DCUBEMAP_FACE_NEGATIVE_Y) continue;
		/*if (m_iCubeMapSide == D3DCUBEMAP_FACE_NEGATIVE_Y) 
		{
			m_iCubeMapSide = (m_iCubeMapSide+1)%6;
			continue;
		}*/

		CubeMapGetSideMatrix(i/*m_iCubeMapSide*/,&matView);
		// Standard view that will be overridden below.
		if (Techniques[dwCurTechnique].dwCode == G2MX_SEA) D3DXMatrixMultiply( &matView, &matCamera, &matView );
		matView._41 = -(matView._11*vCam.x - matView._21*vCam.y + matView._31*vCam.z);
		matView._42 = -(matView._12*vCam.x - matView._22*vCam.y + matView._32*vCam.z);
		matView._43 = -(matView._13*vCam.x - matView._23*vCam.y + matView._33*vCam.z);

		IDirect3DSurface8 * pFace = CubeGetFace(pRenderTargetReflectionCubeMap, i);
		SetRenderTarget(pFace, pCubeFaceZBuffer);
		pFace->Release();

		NewViewport.X = 0; NewViewport.Width = SeaData.dwCubeMapSize;
		NewViewport.Y = 0; NewViewport.Height = SeaData.dwCubeMapSize;
		NewViewport.MinZ = 0.0f; NewViewport.MaxZ = 1.0f;
		pRS->SetViewport(&NewViewport);

		hr = pRS->Clear(0L, NULL, D3DCLEAR_ZBUFFER, Colors[i], 1.0f, 0L);
        hr = pRS->BeginScene();
        pRS->SetTransform( D3DTS_VIEW, &matView );

		ENTITY_ID ent_id;
		ENTITY *pSky = 0;
        // Render scene here.
		if (hr==D3D_OK && api->SetEntityScanLayer("sea_reflection") && api->GetEntity(&ent_id))
		do 
		{
			dword dwCCode = ent_id.class_code;
			if (bUnderWater && dwCCode == dwShipCode) continue;
			if (!bLayerFrozen || (dwCCode != dwShipCode && dwCCode != dwSailCode)) 
			{
				api->Send_Message(ent_id, "l", MSG_SEA_REFLECTION_DRAW);
			}
		} while (api->GetEntityNext(&ent_id));
        hr = pRS->EndScene();
    }
	CubeMapRestoreMatrices();
}

// Render scene to sunroad cubemap surface
void NetSea::CubeMapCalculateSunroadDynamic()
{
	D3DVIEWPORT8 NewViewport;
	HRESULT hr;
	dword Colors[6] = {0xd934c8, 0x2FFF1F, 0x0000FF, 0xFF00, 0xb28e11, 0x0};
	D3DXMATRIX matView, matCamera;

	pRS->GetTransform(D3DTS_VIEW, &matCamera);
	matCamera._41 = 0.0f;	matCamera._42 = 0.0f;	matCamera._43 = 0.0f;

	CubeMapSaveMatricesAndSetProjection(); 

	dword dwSkyCode = api->Class_Name2Code("sky");
	dword dwShipCode = api->Class_Name2Code("ship");
	dword dwSailCode = api->Class_Name2Code("sail");

	bool bLayerFrozen = (api->LayerGetFlags("sea_reflection") & LRFLAG_FROZEN) != 0;

	// Loop through the six faces of the cube map.
	//m_iCubeMapSide = 0;
	for (long i=0; i<6; i++) //if (i==m_iCubeMapSide)
	{
		if (i == D3DCUBEMAP_FACE_NEGATIVE_Y) continue;

		CubeMapGetSideMatrix(i, &matView);
		// Standard view that will be overridden below.
		if (Techniques[dwCurTechnique].dwCode == G2MX_SEA) D3DXMatrixMultiply( &matView, &matCamera, &matView );
		matView._41 = -(matView._11*vCam.x - matView._21*vCam.y + matView._31*vCam.z);
		matView._42 = -(matView._12*vCam.x - matView._22*vCam.y + matView._32*vCam.z);
		matView._43 = -(matView._13*vCam.x - matView._23*vCam.y + matView._33*vCam.z);

		IDirect3DSurface8 * pFace = CubeGetFace(pRenderTargetSunroadCubeMap, i);
		SetRenderTarget(pFace, pCubeFaceZBuffer);
		pFace->Release();

		NewViewport.X = 0; NewViewport.Width = SeaData.dwCubeMapSize;
		NewViewport.Y = 0; NewViewport.Height = SeaData.dwCubeMapSize;
		NewViewport.MinZ = 0.0f; NewViewport.MaxZ = 1.0f;
		pRS->SetViewport(&NewViewport);

		hr = pRS->Clear(0L, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0x0/*Colors[i]*/, 1.0f, 0L);
		hr = pRS->BeginScene();
		pRS->SetTransform( D3DTS_VIEW, &matView );

		ENTITY_ID ent_id;
		// Render scene here.
		if (hr==D3D_OK && api->SetEntityScanLayer("sea_sunroad") && api->GetEntity(&ent_id))
			do 
			{
				dword dwCCode = ent_id.class_code;
				if (!bLayerFrozen || (dwCCode != dwShipCode && dwCCode != dwSailCode)) 
				{
					api->Send_Message(ent_id, "l", MSG_SEA_SUNROAD_DRAW);
				}
			} while (api->GetEntityNext(&ent_id));
			hr = pRS->EndScene();
	}
	CubeMapRestoreMatrices();
}

void NetSea::CubeMapExecute(dword DeltaTime)
{
	if (pRenderTargetReflectionCubeMap) CubeMapCalculateReflectionDynamic();
	if (pRenderTargetSunroadCubeMap) CubeMapCalculateSunroadDynamic();
}

