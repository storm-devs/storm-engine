#include "sea.h"

extern float fNewX;

#define BUMP_DIR	"resource\\textures\\weather\\sea\\bump\\"

void SaveTga32(char * sname, dword Width, dword Height, char * pBuffer)
{
	TGA_H	tga_head;
	ZERO(tga_head);

	tga_head.type = 2;
	tga_head.width = (WORD)Width;
	tga_head.height = (WORD)Height;
	tga_head.bpp = 32;
	tga_head.attr8 = 8;

	HANDLE hFile = fio->_CreateFile(sname, GENERIC_WRITE, FILE_SHARE_READ, OPEN_ALWAYS);
	if (INVALID_HANDLE_VALUE == hFile) return;
	fio->_WriteFile(hFile, &tga_head, sizeof(tga_head), 0);
	fio->_WriteFile(hFile, pBuffer, Width * Height * 4, 0);
	fio->_CloseHandle(hFile);
}

bool NetSea::CalculateBumpMapsGF2MX()
{
	char		str[1024];
	long		iTexIndex;
	HANDLE		hFile;

	// delete old bumps
		for (dword i=0; i<aGF2BumpMaps.Size(); i++) pRS->TextureRelease(aGF2BumpMaps[i]);
		aGF2BumpMaps.DelAll();

	// load new bumps
		do
		{
			sprintf(str, "resource\\textures\\%sSeaBump%.4d.tga.tx", (const char*)SeaData.GF2MX.sBumpDir, aGF2BumpMaps.Size());
			hFile = fio->_CreateFile(str, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
			if (INVALID_HANDLE_VALUE != hFile)
			{
				fio->_CloseHandle(hFile);
				sprintf(str, "%sSeaBump%.4d.tga", (const char*)SeaData.GF2MX.sBumpDir, aGF2BumpMaps.Size());
				iTexIndex = pRS->TextureCreate(str);
				if (iTexIndex != -1) aGF2BumpMaps.Add(iTexIndex);
			}
		} while (INVALID_HANDLE_VALUE != hFile);
	
	return true;
}

bool NetSea::CalculateBumpMapsGF3()
{
	dword				x, y, x1, y1;
	D3DSURFACE_DESC		d3dsd;
	D3DLOCKED_RECT		d3dlr; 
	TGA_H				header;
	char				str[256];
	bool				bLoadBump = true;
	dword				dwNumBumps = 0;
	HANDLE				hBumpFile, hFile;
	HRESULT				hr;

	CVECTOR	* pVV = (CVECTOR *)NEW CVECTOR[SQR(SeaData.dwBumpSize)];
	dword	* pMassive = NEW dword[SQR(SeaData.dwBumpSize)];

	// delete old bumps 
		for (dword i=0; i<aBumpMaps.Size(); i++) pRS->Release(aBumpMaps[i]);
		aBumpMaps.DelAll();

	dword dwTexelSize = 2;

	sprintf(str, "%sbump.zap",(const char*)SeaData.GF3.sBumpDir);
	hBumpFile = fio->_CreateFile(str, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
	if (INVALID_HANDLE_VALUE == hBumpFile)
	{
		// try to load
		hBumpFile = fio->_CreateFile(str, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS);
		bLoadBump = false;

		fio->_WriteFile(hBumpFile, &dwNumBumps, sizeof(dwNumBumps), 0);
		fio->_WriteFile(hBumpFile, &SeaData.dwBumpSize, sizeof(SeaData.dwBumpSize), 0);
		fio->_WriteFile(hBumpFile, &SeaData.dwBumpSize, sizeof(SeaData.dwBumpSize), 0);
		fio->_WriteFile(hBumpFile, &SeaData.dwBumpMipLevels, sizeof(SeaData.dwBumpMipLevels), 0);
	}
	else
	{
		fio->_ReadFile(hBumpFile, &dwNumBumps, sizeof(dwNumBumps), 0);
		fio->_ReadFile(hBumpFile, &SeaData.dwBumpSize, sizeof(SeaData.dwBumpSize), 0);
		fio->_ReadFile(hBumpFile, &SeaData.dwBumpSize, sizeof(SeaData.dwBumpSize), 0);
		fio->_ReadFile(hBumpFile, &SeaData.dwBumpMipLevels, sizeof(SeaData.dwBumpMipLevels), 0);
	}

	// create 
		char * pDst = (char*)NEW char[SeaData.dwBumpSize * SeaData.dwBumpSize * dwTexelSize];

	while (true)
	{
		dword dwIdx = aBumpMaps.Size();

		if (!bLoadBump)
		{
			sprintf(str,"%sSeaBump%.4d.tga",(const char*)SeaData.GF3.sBumpDir,dwIdx);
			hFile = fio->_CreateFile(str, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
			if (INVALID_HANDLE_VALUE == hFile) break;
		}
		else if (dwIdx == dwNumBumps) break;

   		IDirect3DTexture8 * * pBumpMap = &aBumpMaps[aBumpMaps.Add()];

		hr = pRS->CreateTexture( SeaData.dwBumpSize, SeaData.dwBumpSize, SeaData.dwBumpMipLevels, 0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, pBumpMap);

		if (!bLoadBump)
		{
			dwNumBumps++;

			fio->_ReadFile(hFile, &header, sizeof(header), 0);
			fio->_ReadFile(hFile, pMassive, sizeof(dword) * header.width * header.height, 0);
			fio->_CloseHandle(hFile);

			float	fLeft, fRight, fTop, fBottom;
			//float	fR,fL,fD,fU,fLU,fLD,fRU,fRD;
			// generate bump from tga file
			for (y=0; y<SeaData.dwBumpSize; y++)
				for (x=0; x<SeaData.dwBumpSize; x++)
				{
#define GET_MASSIVE(dx,dy)	float(pMassive[((x+dx)&(SeaData.dwBumpSize-1)) + ((y+dy)&(SeaData.dwBumpSize-1)) * SeaData.dwBumpSize]&0xFF) / 255.0f
				
					fLeft	= GET_MASSIVE(-1,0); 
					fRight	= GET_MASSIVE(1,0); 
					fTop	= GET_MASSIVE(0,-1); 
					fBottom = GET_MASSIVE(0,1); 

					float dd = SeaData.fBumpScale;
					CVECTOR res, d0, d1, d2, d3;
					d0 = CVECTOR(-1.f, dd * fLeft, 0.f);
					d1 = CVECTOR(0.f, dd * fTop, -1.f);
					d2 = CVECTOR(1.f, dd * fRight, 0.f);
					d3 = CVECTOR(0.f, dd * fBottom, 1.f);

					res = (d0^d1) + (d2^d3); 

					pVV[x + y * SeaData.dwBumpSize] = !res;
				}
		}

		// generate mip levels for random bump 
		for (dword lev=0; lev<SeaData.dwBumpMipLevels; lev++)
		{
			(*pBumpMap)->GetLevelDesc(lev, &d3dsd);
			(*pBumpMap)->LockRect(lev, &d3dlr, 0, 0);

			if (!bLoadBump)
			{
				word * pDstT = (word*)pDst;
				for(y=0; y<d3dsd.Height; y++)
				{
					for(x=0; x<d3dsd.Width; x++)
					{
						CVECTOR vTmp = 0.0f;
						long dwMult = 1<<(lev);
						for (y1 = y * dwMult; y1 < (y + 1) * dwMult; y1++)
							for (x1 = x * dwMult; x1 < (x + 1) * dwMult; x1++)
								vTmp += pVV[(x1 & (SeaData.dwBumpSize-1)) + (y1 & (SeaData.dwBumpSize-1)) * SeaData.dwBumpSize];
					
						vTmp *= (1.0f / float(dwMult * dwMult));
					
						int red		= int((vTmp.x * 0.5f + 0.5f) * 255.0f);		// FIX-ME no ftol
						int green	= int((vTmp.y * 0.5f + 0.5f) * 255.0f);		// FIX-ME no ftol
						int blue	= int((vTmp.z * 0.5f + 0.5f) * 255.0f);		// FIX-ME no ftol
						*pDstT++ = ARGB1555(1, blue, green, red);
						//int xxx = int((float(red)* 0.299f + float(green) * 0.587f + float(blue) * 0.114f));
						//*pDstT++ = ARGB1555(1, xxx, xxx, xxx);
					}
				}
				fio->_WriteFile(hBumpFile, pDst, d3dsd.Width * d3dsd.Height * dwTexelSize, 0);
			}
			else
				fio->_ReadFile(hBumpFile, pDst, d3dsd.Height * d3dsd.Width * dwTexelSize, 0);

			/*if (lev == 0)
			{
				string sFile;
				sFile.Format("%snmap.%.3d.tga", (char*)SeaData.sBumpDir, dwIdx);
				SaveTga32((char*)sFile, SeaData.dwBumpSize, SeaData.dwBumpSize, (char*)pDst);
			}*/
#ifdef _XBOX
			// Swizzle texture
			XGSwizzleRect(pDst, d3dlr.Pitch, 0, d3dlr.pBits, d3dsd.Width, d3dsd.Height, 0, dwTexelSize);
#else
			// simple copy 
			BYTE * pDstTemp = (BYTE *)d3dlr.pBits;
			for(y=0; y<d3dsd.Height; y++)
			{
				memcpy(pDstTemp, &pDst[y * d3dsd.Width * dwTexelSize], d3dsd.Width * dwTexelSize);
				pDstTemp += (dword)d3dlr.Pitch;
			}
#endif
			// write pDst and d3dlr.pBits
			/*SaveTga32("resource\\islands\\test1.tga",128,128,(char*)pDst);
			SaveTga32("resource\\islands\\test2.tga",128,128,(char*)d3dlr.pBits);*/

			(*pBumpMap)->UnlockRect(lev);
		}
	}

	if (!bLoadBump)
	{
		fio->_SetFilePointer(hBumpFile, 0, 0, FILE_BEGIN);
		fio->_WriteFile(hBumpFile, &dwNumBumps, sizeof(dwNumBumps), 0);
		fio->_WriteFile(hBumpFile, &SeaData.dwBumpSize, sizeof(SeaData.dwBumpSize), 0);
		fio->_WriteFile(hBumpFile, &SeaData.dwBumpSize, sizeof(SeaData.dwBumpSize), 0);
		fio->_WriteFile(hBumpFile, &SeaData.dwBumpMipLevels, sizeof(SeaData.dwBumpMipLevels), 0);
	}

	fio->_CloseHandle(hBumpFile);

	DELETE(pMassive);
	DELETE(pVV);
	DELETE(pDst);

	return true;
}

#define SMALL_FLOAT 1e-12f

void NetSea::CalcSTVectors()
{
	long i;

	for (i=0; i<iVStart; i++)
	{
		pVertex[i].S = pNormals[i] ^ CVECTOR(1.0f, 0.0f, 0.0f);
		pVertex[i].T = pNormals[i] ^ pVertex[i].S;
	}
	return;

	for (i=0; i<iTStart; i++)
	{
		CVECTOR du, dv;
		CVECTOR edge01;
		CVECTOR edge02;
		CVECTOR cp;

		sealxvertex_t & v0 = pVertex[*pTrgsStart++];
		sealxvertex_t & v1 = pVertex[*pTrgsStart++];
		sealxvertex_t & v2 = pVertex[*pTrgsStart++];

		// x, s, t
		edge01 = CVECTOR(v1.pos.x - v0.pos.x, v1.Texture.x - v0.Texture.x, v1.Texture.y - v0.Texture.y);
		edge02 = CVECTOR(v2.pos.x - v0.pos.x, v2.Texture.x - v0.Texture.x, v2.Texture.y - v0.Texture.y);

		cp = edge01 ^ edge02;
		if (fabsf(cp.x) > SMALL_FLOAT)
		{
			v0.T.x += cp.y / cp.x;
			v0.S.x += cp.z / cp.x;

			v1.T.x += cp.y / cp.x;
			v1.S.x += cp.z / cp.x;
			
			v2.T.x += cp.y / cp.x;
			v2.S.x += cp.z / cp.x;
		}

		// y, s, t
		edge01 = CVECTOR(v1.pos.y - v0.pos.y, v1.Texture.x - v0.Texture.x, v1.Texture.y - v0.Texture.y);
		edge02 = CVECTOR(v2.pos.y - v0.pos.y, v2.Texture.x - v0.Texture.x, v2.Texture.y - v0.Texture.y);

		cp = edge01 ^ edge02;
		if (fabsf(cp.x) > SMALL_FLOAT)
		{
			v0.T.y += cp.y / cp.x;
			v0.S.y += cp.z / cp.x;

			v1.T.y += cp.y / cp.x;
			v1.S.y += cp.z / cp.x;
			
			v2.T.y += cp.y / cp.x;
			v2.S.y += cp.z / cp.x;
		}

		// z, s, t
		edge01 = CVECTOR(v1.pos.z - v0.pos.z, v1.Texture.x - v0.Texture.x, v1.Texture.y - v0.Texture.y);
		edge02 = CVECTOR(v2.pos.z - v0.pos.z, v2.Texture.x - v0.Texture.x, v2.Texture.y - v0.Texture.y);

		cp = edge01 ^ edge02;
		if (fabsf(cp.x) > SMALL_FLOAT)
		{
			v0.T.z += cp.y / cp.x;
			v0.S.z += cp.z / cp.x;

			v1.T.z += cp.y / cp.x;
			v1.S.z += cp.z / cp.x;
			
			v2.T.z += cp.y / cp.x;
			v2.S.z += cp.z / cp.x;
		}
	}
}

void NetSea::SetBumpTransformConstants()
{
	float	_fFov;
	CVECTOR _vPos, _vAng;
	
	pRS->GetCamera(_vPos, _vAng, _fFov);

	pRS->SetVertexShaderConstant(CV_BUMPSCALE, &CVECTOR4(0.0f, 0.0f, 0.0f, 1.0f/*SeaData.fBumpScale*/), 1);
	pRS->SetVertexShaderConstant(CV_ZERO, &CVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1);
	pRS->SetVertexShaderConstant(CV_ONE, &CVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), 1);
	pRS->SetVertexShaderConstant(CV_VCAM, &CVECTOR4(vCam.x, vCam.y, vCam.z, 0.0f), 1);

	// FOG - (fog_density,start_fog_distance,multiply=255,ignore)
	pRS->SetVertexShaderConstant(CV_FOG, &CVECTOR4(SeaData.fFogSeaDensity, SeaData.fFogStartDistance, 255.0f, 255.0f), 1);
	
	pRS->SetVertexShaderConstant(CV_EYE_WORLD, &CVECTOR4(vCam.x, vCam.y, vCam.z, 0.0f), 1);

	D3DXMATRIX matTemp, matViewTemp;
	D3DXMATRIX matWorldViewProj;
	D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldViewI;
	D3DXMATRIX matWorldI;

	D3DXMATRIX matWorld; pRS->GetTransform(D3DTS_WORLD, &matWorld);
	D3DXMatrixIdentity(&matWorld);
	D3DXMATRIX matProj; pRS->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMATRIX matView; pRS->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixMultiply(&matTemp, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matTemp, &matProj);
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	D3DXMatrixInverse(&matWorldI, NULL, &matWorld);
	
	// Create a 3x3 inverse of the worldview for the normal transformation (we transpose it as we load it into
	// the constant store)
	D3DXMatrixInverse(&matWorldViewI, NULL, &matWorldView);
		
	// Projection to clip space
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	pRS->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	// transformation to view space
	D3DXMatrixTranspose(&matWorldView, &matWorldView);
	pRS->SetVertexShaderConstant(CV_WORLDVIEW_0, &matWorldView(0, 0), 4);
	D3DXMatrixTranspose(&matWorldView, &matWorldView);

	D3DXMatrixTranspose(&matWorld, &matWorld);
	pRS->SetVertexShaderConstant(CV_WORLD_0, &matWorld(0, 0), 1);
	pRS->SetVertexShaderConstant(CV_WORLD_1, &matWorld(1, 0), 1);
	pRS->SetVertexShaderConstant(CV_WORLD_2, &matWorld(2, 0), 1);
	D3DXMatrixTranspose(&matWorld, &matWorld);

	// If you are rotating the cubemap, you need to apply that transform here too
	D3DXMATRIX matRot, matBasisTransform = matWorld;
	D3DXMatrixIdentity(&matRot);

	D3DXMatrixRotationZ(&matTemp, PId2);
	D3DXMatrixMultiply(&matRot, &matRot, &matTemp);
	D3DXMatrixMultiply(&matBasisTransform, &matBasisTransform, &matRot);

	D3DXMatrixTranspose(&matBasisTransform, &matBasisTransform);
	pRS->SetVertexShaderConstant(CV_BASISTRANSFORM_0, &matBasisTransform(0, 0), 1);
	pRS->SetVertexShaderConstant(CV_BASISTRANSFORM_1, &matBasisTransform(1, 0), 1);
	pRS->SetVertexShaderConstant(CV_BASISTRANSFORM_2, &matBasisTransform(2, 0), 1);

	#define SMALL_DELTA_Z			0.00018f
	pRS->SetVertexShaderConstant(CV_SMALL_DELTA, &CVECTOR4(0.0f,0.0f,SMALL_DELTA_Z,0.0f), 1);
	//D3DXMatrixTranspose(&matBasisTransform, &matBasisTransform);
}
