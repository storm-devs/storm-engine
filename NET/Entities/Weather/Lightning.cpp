#include "lightning.h"

NetLightning::NetLightning() : aLightnings(_FL_)
{
	iLightningTexture = -1;
	iFlashTexture = -1;
	
	pVWSunTrace = null;
}

NetLightning::~NetLightning()
{
	Release();
}

void NetLightning::Release()
{
	pRS->TextureRelease(iLightningTexture);
	pRS->TextureRelease(iFlashTexture);

	DELETE(pVWSunTrace);
}

bool NetLightning::Init()
{
	SetDevice();

	return true;
}

void NetLightning::SetDevice()
{
	pRS = (VDX8RENDER *)_CORE_API->CreateService("dx8render"); Assert(pRS);
	pCollide = (COLLIDE*)_CORE_API->CreateService("COLL"); Assert(pCollide);
}

bool NetLightning::CreateState(ENTITY_STATE_GEN * state_gen)
{
	return true;
}

bool NetLightning::LoadState(ENTITY_STATE * state)
{
	return true;
}

void NetLightning::Execute(dword Delta_Time)
{
	float fDeltaTime = float(Delta_Time) * 0.001f;

	if (!pVWSunTrace) pVWSunTrace = api->LayerGetWalker("sun_trace");
	
	for (dword i=0; i<aLightnings.Size(); i++)
	{
		lightning_t * pL = &aLightnings[i];
		if (pL->fTime > 0.0f)
		{
			pL->fAlpha = (dword(pL->fTime*1000.0f) & pL->dwFlickerTime) ? 1.0f : 0.0f;
			pL->fTime -= fDeltaTime;
			if (pL->fTime <= 0.0f)
			{
				pL->fAlpha = 1.0f;
			}
		}
		if (pL->fTime < 0.0f)
		{
			pL->fAlpha -= float(Delta_Time) * 0.01f;
			if (pL->fAlpha < 0.0f)
			{
				aLightnings.ExtractNoShift(i); i--; continue;
			}
		}
	}
}

void NetLightning::Realize(dword Delta_Time)
{
	dword	i;
	RS_RECT rs_rect;

	if (iLightningTexture >= 0)
	{
		pRS->TextureSet(0, iLightningTexture);
		for (i=0; i<aLightnings.Size(); i++)
		{
			lightning_t * pL = &aLightnings[i];
			RS_RECT		* pR = &rs_rect;

			dword dwAlpha = dword(255.0f * pL->fAlpha);
			pR->dwSubTexture = pL->dwSubTexture;
			pR->dwColor = RGB(dwAlpha, dwAlpha, dwAlpha);
			pR->vPos = pL->vPos;
			pR->fSize = pL->fSize;
			pR->fAngle = 0.0f;
			pRS->DrawRects(pR, 1, (char*)pL->sTechnique, dwSubTexX, dwSubTexY, pL->fScaleX, pL->fScaleY);
		}
	}

	if (iFlashTexture >= 0)
	{
		pRS->TextureSet(0, iFlashTexture);
		for (i=0; i<aLightnings.Size(); i++)
		{
			lightning_t * pL = &aLightnings[i];
			RS_RECT		* pR = &rs_rect;

			dword dwAlpha = dword(255.0f * pL->fAlpha * pL->fPower);
			pR->dwColor = RGB(dwAlpha, dwAlpha, dwAlpha);
			pR->vPos = pL->vPos;
			pR->fSize = pL->Flash.fSize;
			pR->fAngle = 0.0f;
			pRS->DrawRects(pR, 1, (char*)pL->Flash.sTechnique);
		}
	}
}

dword NetLightning::ProcessMessage(MESSAGE & message)
{

	switch (message.Long())
	{
		case MSG_SEA_REFLECTION_DRAW:
			Realize(0);
		break;
		case MSG_WHR_LIGHTNING_ADD:
		{
			// add new lightning
			lightning_t * pL = &aLightnings[aLightnings.Add()];

			pL->dwSubTexture = message.Long();
			message.String(sizeof(pL->sTechnique), pL->sTechnique); 
			pL->fTime = message.Float();
			pL->dwFlickerTime = (dword)message.Long();
			pL->fSize = message.Float();
			pL->fScaleX = message.Float();
			pL->fScaleY = message.Float();
			pL->vPos.x = message.Float();	
			pL->vPos.y = message.Float();	
			pL->vPos.z = message.Float();

			message.String(sizeof(pL->Flash.sTechnique), pL->Flash.sTechnique); 
			pL->Flash.fSize = message.Float();
			pL->Flash.fTime = message.Float();

			CalcFlashPower(pL);
		}
		break;
	}

	return 0;
}

void NetLightning::CalcFlashPower(lightning_t * pL)
{
	CVECTOR vCamPos, vCamAng, vTrace[3];
	float	fFov;

	pRS->GetCamera(vCamPos, vCamAng, fFov);
	
	vTrace[0] = CVECTOR(pL->vPos.x, pL->vPos.y + pL->fSize * 0.9f, pL->vPos.z);
	vTrace[1] = pL->vPos;
	vTrace[2] = CVECTOR(pL->vPos.x, pL->vPos.y - pL->fSize * 0.9f, pL->vPos.z);

	float fPower = 1.0f;

	for (dword i=0; i<3; i++)
	{
		float fRes = pCollide->Trace(*pVWSunTrace, vCamPos, vTrace[i], 0, 0);
		if (fRes <= 1.0f) fPower -= 0.31f;
	}
	pL->fPower = fPower;
}

dword NetLightning::AttributeChanged(ATTRIBUTES * pAttribute)
{
	string sTextureName;

	ATTRIBUTES * pParent = pAttribute->GetParent();

	if (*pAttribute == "Clear")	
	{
		aLightnings.DelAll();
		return 0;
	}

	if (*pParent == "Flash")
	{
		if (*pAttribute == "Texture")	
		{
			if (iFlashTexture >=0 ) pRS->TextureRelease(iFlashTexture);
			iFlashTexture = pRS->TextureCreate(pAttribute->GetThisAttr());
			return 0;
		}
		return 0;
	}

	if (*pAttribute == "Texture")	
	{ 
		if (iLightningTexture >= 0) pRS->TextureRelease(iLightningTexture);
		iLightningTexture = pRS->TextureCreate(pAttribute->GetThisAttr());
		return 0; 
	}

	if (*pAttribute == "SubTexX") { dwSubTexX = pAttribute->GetAttributeAsDword(); return 0; }
	if (*pAttribute == "SubTexY") { dwSubTexY = pAttribute->GetAttributeAsDword(); return 0; }
	if (*pAttribute == "isDone") return 0;
	return 0;
}
