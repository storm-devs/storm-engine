#include "Blood.h"
#include "..\common_h\Geometry.h"
#include "..\common_h\matrix.h"
#include "Location.h"
#include "Character.h"

#define BLOOD_RADIUS		1.f		// размер пятна крови
#define BLOOD_LIVE_TIME		5.5f	// сек
#define BLOOD_BLENDOUT_TIME	5.f		// сек
#define BLOOD_RANDOM_DIST	0.8f	// разброс крови вокруг в метрах

Blood::ClipTriangle Blood::clipT[MAX_CLIPPING_TRIANGLES];
long Blood::nClipTQ;
CVECTOR Blood::normal;

Blood::Blood() :
	aBlood(_FL),
	aModels(_FL)
{
	texID = -1;
	nStartT = 0;
	nUsedTQ = 0;
}

Blood::~Blood()
{
	if( texID!=-1 ) pRS->TextureRelease(texID); texID = -1;
}


//Инициализация
bool Blood::Init()
{
	pRS = (VDX8RENDER *)api->CreateService("dx8render");
	Assert(pRS);

	pCol = (COLLIDE *)api->CreateService("coll");
	Assert(pCol);

	texID = pRS->TextureCreate("blood.tga");

	return true;
}


//Работа
void Blood::Execute(dword delta_time)
{
	if( nUsedTQ < ON_LIVETIME_BLOOD_TRIANGLES ) return;

	float fDeltaTime = delta_time * 0.001f;
	for(long n=0; n<aBlood; n++)
	{
		aBlood[n].fLiveTime -= fDeltaTime;
		if( aBlood[n].fLiveTime <= 0.f )
		{
			if( aBlood[n].nStartIdx == nStartT )
			{ // удаляем кровь
				nStartT += aBlood[n].nIdxQ;
				nUsedTQ -= aBlood[n].nIdxQ;
				if( nStartT >= MAX_BLOOD_TRIANGLES )
					nStartT -= MAX_BLOOD_TRIANGLES;
				aBlood.DelIndex(n);
				n--;
				continue;
			}
			aBlood[n].fLiveTime = 0.f;
		}
		if( aBlood[n].fLiveTime < BLOOD_BLENDOUT_TIME )
		{
			dword dwCol = (dword)(255.f/BLOOD_BLENDOUT_TIME * aBlood[n].fLiveTime);
			if( dwCol > 255 ) dwCol = 255;
			dwCol = (dwCol<<24) | 0xFFFFFF;
			for(long i=0,m=aBlood[n].nStartIdx; i<aBlood[n].nIdxQ; i++,m++)
			{
				if( m >= MAX_BLOOD_TRIANGLES ) m -= MAX_BLOOD_TRIANGLES;
				pvBloodT[m].v[0].dwCol = dwCol;
				pvBloodT[m].v[1].dwCol = dwCol;
				pvBloodT[m].v[2].dwCol = dwCol;
			}
		} else break;
	}
}

void Blood::Realize(dword delta_time)
{
	if( nUsedTQ>0 )
	{
		dword dwOldTF,dwAmbient;

		pRS->GetRenderState(D3DRS_TEXTUREFACTOR,&dwOldTF);
		pRS->GetRenderState(D3DRS_AMBIENT,&dwAmbient);
		pRS->SetRenderState(D3DRS_TEXTUREFACTOR,dwAmbient);

		pRS->TextureSet(0,texID);
		CMatrix mtx;
		mtx.SetIdentity();
		pRS->SetWorld(mtx);

		if( nStartT + nUsedTQ <= MAX_BLOOD_TRIANGLES )
		{
			pRS->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, nUsedTQ, (void*)&pvBloodT[nStartT], sizeof(BloodVertex), "blood");
		} else {
			pRS->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, MAX_BLOOD_TRIANGLES-nStartT, (void*)&pvBloodT[nStartT], sizeof(BloodVertex), "blood");
			pRS->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, nStartT + nUsedTQ - MAX_BLOOD_TRIANGLES, (void*)pvBloodT, sizeof(BloodVertex), "blood");
		}

		pRS->SetRenderState(D3DRS_TEXTUREFACTOR,dwOldTF);
	}
}

dword _cdecl Blood::ProcessMessage(MESSAGE &message)
{
	ENTITY_ID eid;
	CVECTOR cv;

	switch( message.Long() )
	{
	case 1: // Add model
		eid = message.EntityID();
		aModels.Add(eid);
	break;
	case 2: // add blood
		cv.x = message.Float();
		cv.y = message.Float();
		cv.z = message.Float();
		AddBlood(cv);
	break;
	}

	return 0;
}

bool Blood::AddClipPoligon(const CVECTOR * v, long nv)
{
	if(nClipTQ >= MAX_CLIPPING_TRIANGLES) return false;
	if(nv < 3) return true;
	for(long i = 2; i < nv; i++)
	{
		clipT[nClipTQ].v[0] = v[0];
		clipT[nClipTQ].v[1] = v[i-1];
		clipT[nClipTQ].v[2] = v[i];
		nClipTQ++;
		if(nClipTQ >= MAX_CLIPPING_TRIANGLES) return false;
	}
	return true;
}

void Blood::AddBlood(const CVECTOR& pos)
{
	nClipTQ = 0;
	normal = CVECTOR(0.f,1.f,0.f);

	CVECTOR cpos = pos;
	VIDWALKER *walker = api->LayerGetWalker("blood");
	if( !walker ) return;

	CVECTOR src = pos; src.y += 1.f;
	CVECTOR dst = pos; dst.y -= 10.f;
	float fTrace = pCol->Trace(*walker,src,dst,0,0);
	if( fTrace <=1.f )
		cpos.y = src.y + (dst.y-src.y)*fTrace;

	long nThisBloodQ = CheckBloodQuantityInRadius(cpos,BLOOD_RADIUS,4);
	if( nThisBloodQ>=4 ) { // уже много крови в этом месте
		delete walker;
		return;
	}
	if( nThisBloodQ>0 ) {
		// разброс крови случайно, если уже есть тут кровь
		cpos.x += FRAND(BLOOD_RANDOM_DIST*2.f) - BLOOD_RANDOM_DIST;
		cpos.z += FRAND(BLOOD_RANDOM_DIST*2.f) - BLOOD_RANDOM_DIST;

		src = cpos; src.y += 1.5f;
		dst = cpos; dst.y -= 10.f;
		fTrace = pCol->Trace(*walker,src,dst,0,0);
		if( fTrace <=1.f )
			cpos.y = src.y + (dst.y-src.y)*fTrace;
	}

	//Описываюищй ящик
	static PLANE p[6];
	p[0].Nx = 0.0f; p[0].Ny = 1.0f; p[0].Nz = 0.0f; p[0].D = (cpos.y + BLOOD_RADIUS);
	p[1].Nx = 0.0f; p[1].Ny = -1.0f; p[1].Nz = 0.0f; p[1].D = -(cpos.y - BLOOD_RADIUS);
	p[2].Nx = 0.0f; p[2].Ny = 0.0f; p[2].Nz = 1.0f; p[2].D = (cpos.z + BLOOD_RADIUS);
	p[3].Nx = 0.0f; p[3].Ny = 0.0f; p[3].Nz = -1.0f; p[3].D = -(cpos.z - BLOOD_RADIUS);
	p[4].Nx = 1.0f; p[4].Ny = 0.0f; p[4].Nz = 0.0f; p[4].D = (cpos.x + BLOOD_RADIUS);
	p[5].Nx = -1.0f; p[5].Ny = 0.0f; p[5].Nz = 0.0f; p[5].D = -(cpos.x - BLOOD_RADIUS);

	// бегаем по лееру
	for(ENTITY_ID *pEID=walker->GetID(); pEID; pEID=walker->GetIDNext())
	{
		MODEL * m = (MODEL *)api->GetEntityPointer(pEID);
		if(!m) continue;
		NODE * root = m->GetNode(0);
		m->Clip(p, 6, cpos, BLOOD_RADIUS, AddClipPoligon);
	}
	delete walker;

	// бегаем по массиву моделек
	for(long n=0; n<aModels; n++)
	{
		MODEL * m = (MODEL *)api->GetEntityPointer(&aModels[n]);
		if(!m) continue;
		NODE * root = m->GetNode(0);
		m->Clip(p, 6, cpos, BLOOD_RADIUS, AddClipPoligon);
	}

	if( nClipTQ<=0 ) return;

	BuildBloodDataByCollision(cpos);
}

void Blood::BuildBloodDataByCollision(const CVECTOR& cpos)
{
	if( nClipTQ<=0 ) return;

	BloodInfo curBlood;
	curBlood.fLiveTime = BLOOD_LIVE_TIME;
	curBlood.nIdxQ = nClipTQ;
	curBlood.cpos = cpos;

	// ищем свободное место для треугольников
	if( nClipTQ + nUsedTQ <= MAX_BLOOD_TRIANGLES )
	{
		curBlood.nStartIdx = nStartT + nUsedTQ;
		if( curBlood.nStartIdx >= MAX_BLOOD_TRIANGLES )
			curBlood.nStartIdx -= MAX_BLOOD_TRIANGLES;
	} else
	{
		api->Trace("Blood::BuildData() : can`t blood add - insufficient buffer space");
		return;
	}
	nUsedTQ += nClipTQ;
	aBlood.Add(curBlood);

	// заполняем буффер
	float fU0,fV0;
	switch(rand()%8)
	{
	case 0: fU0=0.f; fV0=0.f; break;
	case 1: fU0=0.25f; fV0=0.f; break;
	case 2: fU0=0.5f; fV0=0.f; break;
	case 3: fU0=0.75f; fV0=0.f; break;
	case 4: fU0=0.f; fV0=0.5f; break;
	case 5: fU0=0.25f; fV0=0.5f; break;
	case 6: fU0=0.5f; fV0=0.5f; break;
	case 7: fU0=0.75f; fV0=0.5f; break;
	}
	for(long i=0, n=curBlood.nStartIdx; i<nClipTQ; i++,n++)
	{
		if( n>=MAX_BLOOD_TRIANGLES ) n -= MAX_BLOOD_TRIANGLES;
		SetVertexByPos(pvBloodT[n].v[0], clipT[i].v[0], cpos, fU0,fV0);
		SetVertexByPos(pvBloodT[n].v[1], clipT[i].v[1], cpos, fU0,fV0);
		SetVertexByPos(pvBloodT[n].v[2], clipT[i].v[2], cpos, fU0,fV0);
	}
}

void Blood::SetVertexByPos(BloodVertex& v, const CVECTOR& pos, const CVECTOR& vc, float fU0, float fV0)
{
	v.pos = pos;
	v.pos.y += 0.01f;
	v.dwCol = 0xFFFFFFFF;

	v.u = 0.125f + (pos.x - vc.x) * .125f / BLOOD_RADIUS;
	if( v.u<0.f ) v.u = 0.f;
	if( v.u>0.25f ) v.u = 0.25f;
	v.u += fU0;

	v.v = 0.25f + (pos.z - vc.z) * .25f / BLOOD_RADIUS;
	if( v.v<0.f ) v.v = 0.f;
	if( v.v>0.5f ) v.v = 0.5f;
	v.v += fV0;
}

long Blood::CheckBloodQuantityInRadius(const CVECTOR& cpos, float fDist, long nLimitQ)
{
	float fDistPow2 = fDist*fDist;
	long nCurQ = 0;
	for(long n=0; n<aBlood; n++)
	{
		if( (~(aBlood[n].cpos - cpos)) > fDistPow2 ) continue;
		nCurQ++;
		if( nCurQ>=nLimitQ ) return nCurQ;
	}
	return nCurQ;
}
