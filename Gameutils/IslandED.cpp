#include "IslandED.h"

dword DWR;

IslandED::IslandED() : aPoints(_FL)
{
	bLoaded = false;
	pRaw = null;
	fScale = 0.2f;
	fX = fY = 0.0f;
	bCanDoPoint = true;
	bCanDoGraf = true;
	bTrace = false;
	bPathFirstPnt = false;
	bCanDoPath = true;

	pIni = null;
	pPath = null;

	bCursorVisible = true;
	bFirstExecute = true;
}

IslandED::~IslandED()
{
	Graph.Save(pIni);
	DELETE(pRaw);
	DELETE(pIni);
	RELEASE(pIslandTexture);
	ShowCursor(true);
}

long iTexID;

bool IslandED::Init()
{
	api->LayerAdd("execute",GetID(),1);
	api->LayerAdd("realize",GetID(),1);
	api->LayerAdd("system_messages",GetID(),1);
	api->LayerSetFlags("execute",LRFLAG_EXECUTE);
	api->LayerSetFlags("realize",LRFLAG_REALIZE);

	pRS = (VDX8RENDER*)api->CreateService("DX8RENDER");
	iTexID = pRS->TextureCreate("shore.tga");

	GetWindowRect(api->GetAppHWND(),&SR);
	iLockX = SR.left + (SR.right - SR.left) / 2;
	iLockY = SR.top + (SR.bottom - SR.top) / 2;

	fCenterX = (SR.right - SR.left) / 2.0f;
	fCenterY = (SR.bottom - SR.top) / 2.0f;

	OpenNewFile();

	return true;
}

bool IslandED::CheckIslandPoint(CVECTOR & vPos)
{
	float x1,y1,x2,y2; // island rect
	x1 = vBoxCenter.x - vBoxSize.x;
	x2 = vBoxCenter.x + vBoxSize.x;

	y1 = vBoxCenter.z - vBoxSize.z;
	y2 = vBoxCenter.z + vBoxSize.z;
	if (vPos.x < x1 || vPos.x>x2 || vPos.z<y1 || vPos.z>y2) return false;

	long x,y;

	x = long(float(Header.width) * (vPos.x - x1) / (x2-x1));
	y = long(float(Header.height) * (vPos.z - y1) / (y2-y1));
	BYTE bRes = pRaw[x + y * Header.width];
	return (bRes <= 128);
}

float IslandED::Trace(spoint *p1, spoint *p2)
{
	p1->vPos.y = p2->vPos.y = 0.0f;
	float fLen = sqrtf(~(p1->vPos - p2->vPos));
	CVECTOR vDir = !(p2->vPos - p1->vPos);
	for (float f=0.0f;f<fLen;f+=0.98f)
	{
		CVECTOR vPos = p1->vPos + vDir * f;
		if (CheckIslandPoint(vPos)) return f / fLen;
	}
	return 2.0f;
}

dword IslandED::GetNearestPointIdx(CVECTOR vSrc)
{
	float fMinDistance = 1e8f;
	dword dwIdx = INVALID_POINT_INDEX;
	for (dword i=0;i<aPoints.Size();i++) if (~(aPoints[i].vPos - vSrc) < fMinDistance)
	{
		fMinDistance = ~(aPoints[i].vPos - vSrc);
		dwIdx = i;
	}
	return dwIdx;
}
CVECTOR pVTest;

void IslandED::Execute(dword dwDeltaTime)
{
	dword	i,j;
	POINT pnt;
	GetCursorPos(&pnt);
	float fDX = float(pnt.x - iLockX);
	float fDY = float(pnt.y - iLockY);

	if (bFirstExecute)
	{
		bFirstExecute = false;
		HCURSOR hC = LoadCursor(null, IDC_ARROW);
		SetClassLong(api->GetAppHWND(), GCL_HCURSOR, (long)hC);
	}

	CVECTOR pV;
	pV.y = 0.0f;
	pV.x = ((pnt.x - iLockX) - fX) / fScale;
	pV.z = ((pnt.y - iLockY) - fY) / fScale;
	pVTest = pV;

	bool bNewCursorVisible = true;
	if (GetAsyncKeyState(VK_RBUTTON) < 0)
	{
		bNewCursorVisible = false;
	}

	if (bNewCursorVisible != bCursorVisible)
	{
		bCursorVisible = bNewCursorVisible;
		ShowCursor(bCursorVisible);
		if (bCursorVisible == false)
		{
			SetCursorPos(iLockX,iLockY);
			fDX = fDY = 0.0f;
		}
	}
	if (GetAsyncKeyState(VK_MBUTTON)==0) { bCanDoPath = true; }
	if (GetAsyncKeyState(VK_LBUTTON)==0) { bTrace = false; bCanDoPoint = true; }
	if (GetAsyncKeyState(VK_LBUTTON)<0 && bCursorVisible && GetAsyncKeyState(VK_SHIFT)<0)
	{
		((!bTrace) ? pTrace1 : pTrace2) = pV;
		bTrace = true;
	}

	if (GetAsyncKeyState(VK_LBUTTON)<0 && bCursorVisible && bCanDoPoint && GetAsyncKeyState(VK_SHIFT)==0)
	{
		bCanDoPoint = false;
		if (GetAsyncKeyState(VK_CONTROL)==0)
		{
			spoint *pS = &aPoints[aPoints.Add()];
			pS->vPos = pV;
		}
		else if (aPoints.Size())
		{	
			// search nearest point and delete her
			dword dwIdx = GetNearestPointIdx(pV);
			if (dwIdx != INVALID_POINT_INDEX) aPoints.ExtractNoShift(dwIdx);
		}
	}

	if (GetAsyncKeyState(VK_MBUTTON) < 0 && bCanDoPath)
	{
		if (bPathFirstPnt)
		{
			bPathFirstPnt = false;
			DELETE(pPath);
			pPath = Graph.GetPath(dwFirstPointIdx,GetNearestPointIdx(pV));
		}
		else
		{
			bCanDoPath = false;
			dwFirstPointIdx = GetNearestPointIdx(pV);
			bPathFirstPnt = true;
		}
	}
	if (GetAsyncKeyState(VK_RBUTTON) < 0)
	{
		SetCursorPos(iLockX,iLockY);
		bNewCursorVisible = false;
		if (GetAsyncKeyState(VK_MENU)==0) 
		{
			fX -= fDX;
			fY -= fDY;
		}
		else fScale -= fDY * 0.001f;
		if (fScale<=0.0f) fScale = 0.0001f;
	}
	if (GetAsyncKeyState(VK_LBUTTON)==0) bCanDoGraf = true;
	if (GetAsyncKeyState('G')<0 && bCanDoGraf)
	{
		Graph.ReleaseAll();
		bCanDoGraf = false;
		for (i=0;i<aPoints.Size();i++) aPoints[i].dwIdx = Graph.AddPoint(aPoints[i].vPos);
		for (i=0;i<aPoints.Size();i++)
		{
			for (j=0;j<aPoints.Size();j++) if (i!=j)
			{
				if (Trace(&aPoints[i],&aPoints[j]) > 1.0f)
				{
					Graph.AddEdge2Point(aPoints[i].dwIdx,aPoints[i].dwIdx,aPoints[j].dwIdx);
				}
			}
		}
		Graph.Save(pIni);
		DELETE(pPath);
		RDTSC_B(DWR);
		Graph.BuildTable();
		RDTSC_E(DWR);
		//pPath = Graph.GetPath(0,2);
	}
	if (GetAsyncKeyState('H')<0 && bCanDoGraf)
	{
		Graph.BuildTable();
		//DELETE(pPath);
		//pPath = Graph.GetPath(rand()%Graph.aPoints.Size(),rand()%Graph.aPoints.Size());
	}
}	

dword kk;

CVECTOR IslandED::GetScreenPos(CVECTOR & vRealPos)
{
	CVECTOR v;
	v.x = fCenterX + fX + vRealPos.x * fScale;
	v.z = 1.0f;
	v.y = fCenterY + fY + vRealPos.z * fScale;
	return v;
}

CVECTOR IslandED::GetRealPos(CVECTOR & vScrPos)
{
	CVECTOR v = 0.0f;
	return v;
}

void IslandED::Realize(dword dwDeltaTime)
{
	dword		i;
	RS_SPRITE	rss[8];
	ZERO(rss);

	float x,y,dx,dy;
	x = fCenterX + fX + vBoxCenter.x * fScale;
	y = fCenterY + fY + vBoxCenter.z * fScale;
	dx = vBoxSize.x * fScale;
	dy = vBoxSize.z * fScale;

	FillSpriteVertex(rss[0],x-dx,y-dy,0.0f,0xFFFFFF);
	FillSpriteVertex(rss[1],x+dx,y-dy,0.0f,0xFFFFFF,1.0f);
	FillSpriteVertex(rss[2],x+dx,y+dy,0.0f,0xFFFFFF,1.0f,1.0f);
	FillSpriteVertex(rss[3],x-dx,y+dy,0.0f,0xFFFFFF,0.0f,1.0f);

	if (Graph.aEdges.Size())
	{
		array<RS_LINE2D> Lines(_FL_);
		for (i=0;i<Graph.aEdges.Size();i++)
		{
			CVECTOR		v;
			RS_LINE2D	*pL;
			pL = &Lines[Lines.Add()];
			pL->dwColor = 0x1F4F00; pL->rhw = 0.5f;
			v = Graph.aPoints[Graph.aEdges[i].dw1].vPos;
			pL->vPos = GetScreenPos(v);

			pL = &Lines[Lines.Add()];
			pL->dwColor = 0x1F4F00; pL->rhw = 0.5f;
			v = Graph.aPoints[Graph.aEdges[i].dw2].vPos;
			pL->vPos = GetScreenPos(v);
		}
		if (pPath && pPath->aPoints.Size())
		{
			for (i=0;i<pPath->aPoints.Size()-1;i++)
			{
				CVECTOR		v;
				RS_LINE2D	*pL;
				pL = &Lines[Lines.Add()];
				pL->dwColor = 0xFFFFFF; pL->rhw = 0.5f;
				v = Graph.aPoints[pPath->aPoints[i].dwPnt].vPos;
				pL->vPos = GetScreenPos(v);

				pL = &Lines[Lines.Add()];
				pL->dwColor = 0xFFFFFF; pL->rhw = 0.5f;
				v = Graph.aPoints[pPath->aPoints[i+1].dwPnt].vPos;
				pL->vPos = GetScreenPos(v);
			}
		}

		pRS->DrawLines2D(&Lines[0],Lines.Size()/2,"islanded_points");
	}
	if (GetAsyncKeyState('B')<0)
	{
		dword dx = dword(vBoxSize.x / 32.0f);
		dword dy = dword(vBoxSize.z / 32.0f);
		RS_SPRITE *rsp = new RS_SPRITE[dx * dy * 4];
		float ddx = 16.0f * fScale;
		float ddy = 16.0f * fScale;
		for (dword y = 0; y < dy ; y++)
			for (dword x = 0; x < dx ; x++)
			{
				dword idx = (x + y * dx) * 4;
				CVECTOR v,pV;
				v.x = vBoxCenter.x + vBoxSize.x * 2.0f * float(x) / float(dx) - vBoxSize.x;
				v.y = 0.0f;
				v.z = vBoxCenter.z + vBoxSize.z * 2.0f * float(y) / float(dy) - vBoxSize.z;
				pV = GetScreenPos(v);
				pV.z = pV.y;
				dword dwColor = 0xFFFFFF;
				if (CheckIslandPoint(v)) dwColor = 0x00;
				FillSpriteVertex(rsp[idx + 0],pV.x-ddx,pV.z-ddy,1.0f,dwColor);
				FillSpriteVertex(rsp[idx + 1],pV.x+ddx,pV.z-ddy,1.0f,dwColor);
				FillSpriteVertex(rsp[idx + 2],pV.x+ddx,pV.z+ddy,1.0f,dwColor);
				FillSpriteVertex(rsp[idx + 3],pV.x-ddx,pV.z+ddy,1.0f,dwColor);
			}
		pRS->DrawSprites(&rsp[0],dx * dy,"islanded_points");
		DELETE(rsp);
	}
	if (aPoints.Size())
	{
		RS_SPRITE *rsp = new RS_SPRITE[aPoints.Size() * 4];
		float dx = 24.0f * fScale;
		float dy = 24.0f * fScale;
		for (i=0;i<aPoints.Size();i++)
		{
			CVECTOR pV = aPoints[i].vPos;
			pV = GetScreenPos(aPoints[i].vPos);
			pV.z = pV.y;
			FillSpriteVertex(rsp[i*4 + 0],pV.x-dx,pV.z-dy);
			FillSpriteVertex(rsp[i*4 + 1],pV.x+dx,pV.z-dy);
			FillSpriteVertex(rsp[i*4 + 2],pV.x+dx,pV.z+dy);
			FillSpriteVertex(rsp[i*4 + 3],pV.x-dx,pV.z+dy);
		}
		pRS->DrawSprites(&rsp[0],aPoints.Size(),"islanded_points");
		DELETE(rsp);
	}

	pRS->SetTexture(0,pIslandTexture);
	//pRS->TextureSet(0,iTexID);
	pRS->DrawSprites(&rss[0],1,"spritetest");

	pRS->Print(0,0,"%d, %d",kk,DWR);
	pRS->Print(0,20,"%.4f,%.4f,%.4f",pVTest.x,pVTest.y,pVTest.z);
}

void IslandED::OpenNewFile()
{
	char	sFile[512],sCurDir[512];
	
	DELETE(pRaw);
	bLoaded = false;

	ZERO(sFile[0]);

	fio->_GetCurrentDirectory(512,sCurDir);

	OPENFILENAME	ofn;
	ZERO(ofn);
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = null;
	ofn.lpstrFilter = "Ini Files (*.ini)\0*.ini\0\0";
	ofn.lpstrFile = &sFile[0];
	ofn.nMaxFile = 512;
	BOOL bRes = GetOpenFileName(&ofn);
	fio->_SetCurrentDirectory(sCurDir);
	if (!bRes) return;

	DELETE(pIni);
	pIni = fio->OpenIniFile(sFile);
	if (!pIni) return;
	pIni->ReadString("Main","DepthFile",sFile,sizeof(sFile));
	Graph.Load(pIni);
	aPoints.DelAll();
	for (dword i=0;i<Graph.aPoints.Size();i++)
	{
		aPoints[aPoints.Add()].vPos = Graph.aPoints[i].vPos;
	}
	
	HANDLE hFile = fio->_CreateFile(sFile);
	if (INVALID_HANDLE_VALUE == hFile) return;
	fio->_ReadFile(hFile,&Header,sizeof(Header),0);

	pRaw = new BYTE[Header.width * Header.height];
	fio->_ReadFile(hFile,pRaw,Header.width * Header.height,0);

	HRESULT hr = pRS->CreateTexture(Header.width,Header.height,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&pIslandTexture);
	if (D3D_OK != hr) { DELETE(pRaw); fio->_CloseHandle(hFile); return; }

	D3DLOCKED_RECT lr;
	pIslandTexture->LockRect(0,&lr,0,0);
	DWORD *pSurf = (DWORD*)lr.pBits;
	for (dword y=0;y<Header.height;y++)
		for (dword x=0;x<Header.width;x++)
		{
			DWORD dwColor = DWORD(pRaw[x + y * Header.width]);
			dwColor = (dwColor<=128) ? 0xFF : 0x0;
			pSurf[x + y * (lr.Pitch / 4)] = ARGB(0,0,0,dwColor);
		}
	pIslandTexture->UnlockRect(0);

	fio->_CloseHandle(hFile);

	// and load ini file parameters
	char str[256];
	pIni->ReadString("Main","vBoxCenter",str,255);
	sscanf(str,"%f,%f,%f",&vBoxCenter.x,&vBoxCenter.y,&vBoxCenter.z);
	pIni->ReadString("Main","vBoxSize",str,255);
	sscanf(str,"%f,%f,%f",&vBoxSize.x,&vBoxSize.y,&vBoxSize.z);
	fX = 0.0f;
	fY = 0.0f;

	bLoaded = true;
}