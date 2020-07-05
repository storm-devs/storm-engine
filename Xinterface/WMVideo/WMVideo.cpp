#include "WMVideo.h"
#include "..\\..\\engine\\program\\interface\\messages.h"

INTERFACE_FUNCTION
CREATE_CLASS(WMVideoPlay)

#define VIDEO_DIRECTORY		"d:\\Resource\\XVideos"

extern float g_fWMVAspectRatioK;

WMVideoPlay::WMVideoPlay()
{
	rs = null;
	pBase = null;
}

WMVideoPlay::~WMVideoPlay()
{
	if(pBase!=null)	pBase->Release();
	pBase=null;
}

bool WMVideoPlay::Init()
{
	GUARD(WMVideoPlay::Init())

	rs = (VDX8RENDER *)api->CreateService("dx8render");
	if(!rs)	{_THROW("No service: dx8render")}

	g_fWMVAspectRatioK = rs->GetHeightDeformator();

	UNGUARD
	return true;
}
 
void WMVideoPlay::Execute(dword Delta_Time)
{
}

void WMVideoPlay::Realize(dword Delta_Time)
{
	if(rs) rs->Clear(0L, NULL, D3DCLEAR_TARGET, 0x0, 1.0f, 0L);
	if(!pBase || !pBase->FrameUpdate())	api->Event("ievntEndVideo");
}

dword _cdecl WMVideoPlay::ProcessMessage(MESSAGE & message)
{
	switch(message.Long())
	{
	case 40123:
		if(pBase)
		{
			float fLTex = message.Float();
			float fTTex = message.Float();
			float fRTex = message.Float();
			float fBTex = message.Float();

			float fLPos = message.Float();
			float fTPos = message.Float();
			float fRPos = message.Float();
			float fBPos = message.Float();

			pBase->SetShowParameters(fLTex,fTTex,fRTex,fBTex, fLPos,fTPos,fRPos,fBPos);
		}
	break;

	case MSG_SET_VIDEO_PLAY:
		char param[256];
		char vidName[256];
		message.String(sizeof(param)-1,param);
		sprintf(vidName,"%s\\%s",VIDEO_DIRECTORY,param);
		if(pBase!=null)
		{
			delete pBase;
			pBase = null;
		}
		pBase = NEW WMVBase;
		if(pBase!=null && pBase->Initialize((IDirect3DDevice8*)rs->GetD3DDevice(),vidName))
		{
			api->LayerCreate("vRealize",true,false);
			api->LayerSetRealize("vRealize",true);
			api->LayerAdd("vRealize",GetID(),-1);
		}
		else
		{
			api->Trace("ERROR!!! Can`t launch video %s",vidName);
			if(pBase!=null)
			{
				pBase->Release();
				pBase = null;
			}
			api->Event("ievntEndVideo");
		}
	break;
	}
	return 0;
}
