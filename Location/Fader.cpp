//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Fader
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "Fader.h"
#include "..\common_h\messages.h"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

long Fader::numberOfTips = 0;
long Fader::currentTips = -1;

Fader::Fader()
{
	rs = null;
	isWork = false;
	haveFrame = false;
	endFade = false;
	alpha = 0.0f;
	surface = null;
	renderTarget = null;
	textureID = -1;
	tipsID = -1;
	eventStart = false;
	eventEnd = false;
	deleteMe = 0;
}

Fader::~Fader()
{
	if(surface) rs->Release(surface);
	if(renderTarget) renderTarget->Release();
	surface = null;	
	renderTarget = null;
	if(rs)
	{
		if(textureID >= 0) rs->TextureRelease(textureID);
		if(tipsID >= 0) rs->TextureRelease(tipsID);
	}
}

//Инициализация
bool Fader::Init()
{
	GUARD(Fader::Init())
	//Проверим что единственные
	ENTITY_ID eid;
	if(_CORE_API->FindClass(&eid, "Fader", 0))
	{
		if(_CORE_API->GetEntityPointer(&eid) != this || _CORE_API->FindClassNext(&eid))
		{
			if(fadeIn == ((Fader *)_CORE_API->GetEntityPointer(&eid))->fadeIn)
			{
				_CORE_API->Trace("Fader::Init() -> Fader already created, %s", fadeIn ? "fade in phase" : "fade out phase");
			}
			//!!!
			//return false;
		}
	}
	//Layers
	_CORE_API->LayerCreate("fader_realize", true, false);
	_CORE_API->LayerSetRealize("fader_realize", true);
	_CORE_API->LayerAdd("fader_realize", GetID(), -256);
	_CORE_API->LayerCreate("fader_execute", true, false);
	_CORE_API->LayerSetExecute("fader_execute", true);
	_CORE_API->LayerAdd("fader_execute", GetID(), -256);
	//DX8 render
	rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!rs) _THROW("No service: dx8render");
	D3DVIEWPORT8 vp;
	rs->GetViewport(&vp);
	w = float(vp.Width);
	h = float(vp.Height);
	if(w <= 0 || h <= 0) return false;
	if(rs->GetRenderTarget(&renderTarget) != D3D_OK) return false;
	//Зачитаем количество типсов, если надо
	if(!numberOfTips)
	{
		INIFILE * ini = api->fio->OpenIniFile(api->EngineIniFileName());
		if(ini)
		{
			numberOfTips = ini->GetLong(null, "numoftips", -1);
			delete ini;
		}else numberOfTips = -1;
		if(numberOfTips > 9999) numberOfTips = 9999;
	}
	return true;
	UNGUARD
}

//Сообщения
dword _cdecl Fader::ProcessMessage(MESSAGE & message)
{
	char _name[MAX_PATH];
	switch(message.Long())
	{
	case FADER_OUT:	//Запустить затемнение экрана
		alpha = 0.0f;
		fadeSpeed = message.Float();
		if(fadeSpeed > 0.0f) fadeSpeed = 1.0f/fadeSpeed; else fadeSpeed = 0.0f;
		isWork = true;
		haveFrame = false;
		fadeIn = false;
		isStart = true;
		endFade = false;
		isAutodelete = message.Long() != 0;
		eventStart = false;
		eventEnd = false;
		break;
	case FADER_IN:	//Запустить проявление экрана
		alpha = 0.0f;
		fadeSpeed = message.Float();
		if(fadeSpeed < 0.00001f) fadeSpeed = 0.00001f;
		fadeSpeed = 1.0f/fadeSpeed;
		haveFrame = false;
		isWork = true;
		fadeIn = true;
		isStart = true;
		endFade = false;
		isAutodelete = message.Long() != 0;
		haveFrame = false;
		if(surface) surface->Release();
		surface = null;
		eventStart = false;
		eventEnd = false;
		break;
	case FADER_STARTFRAME:
		haveFrame = true;
		break;
	case FADER_PICTURE:
		if(textureID >= 0 && rs) rs->TextureRelease(textureID);
		message.String(sizeof(_name),_name);
		textureID = rs->TextureCreate(_name);
		if(rs)
		{
			rs->SetProgressImage(_name);
			//Текстура подсказки
			if(numberOfTips > 0)
			{
				sprintf(_name, "tips\\tips_%.4u.tga", rand() % numberOfTips);
				tipsID = rs->TextureCreate(_name);
				rs->SetTipsImage(_name);
			}
		}
		break;
	case FADER_PICTURE0:
		if(textureID >= 0 && rs) rs->TextureRelease(textureID);
		message.String(sizeof(_name),_name);
		textureID = rs->TextureCreate(_name);
		if(rs)
		{
			rs->SetProgressImage(_name);
			//Текстура подсказки
			if(numberOfTips > 0)
			{
				//sprintf(_name, "tips\\tips_%.4u.tga", rand() % numberOfTips);
				char * pTipsName = rs->GetTipsImage();
				if(pTipsName)
				{
					tipsID = rs->TextureCreate(pTipsName);
					//rs->SetTipsImage(_name);
				}
			}
		}
		break;
	}
	return 0;
}

//Работа
void Fader::Execute(dword delta_time)
{
	//api->Trace("fader frame");
	if(deleteMe)
	{
		deleteMe++;
		if(deleteMe >= 3) _CORE_API->DeleteEntity(GetID());
	}
	if(eventStart) 
	{
		eventStart = false;
		if(!fadeIn)
		{
			_CORE_API->PostEvent("FaderEvent_StartFade", 0, "li", fadeIn, GetID());
			//_CORE_API->Trace("FaderEvent_StartFade");
		}else{
			_CORE_API->PostEvent("FaderEvent_StartFadeIn", 0, "li", fadeIn, GetID());
		//	_CORE_API->Trace("FaderEvent_StartFadeIn");
		}
	}
	if(eventEnd)
	{
		eventEnd = false;
		deleteMe = isAutodelete;
		if(!fadeIn)
		{
			_CORE_API->PostEvent("FaderEvent_EndFade", 0, "li", fadeIn, GetID());
			//_CORE_API->Trace("FaderEvent_EndFade");
		}else{
			_CORE_API->PostEvent("FaderEvent_EndFadeIn", 0, "li", fadeIn, GetID());
		//	_CORE_API->Trace("FaderEvent_EndFadeIn");
		}
	}
}


void Fader::Realize(dword delta_time)
{
	if(!isWork) return;
	if(isStart) eventStart = true;	
	//Снятие и рисование стартового кадра
	if(!endFade)
	{
		if(haveFrame)
		{
			if(isStart)
			{
				//Надо снять шот
				bool isOk = false;
				D3DSURFACE_DESC desc;
				if(renderTarget->GetDesc(&desc) == D3D_OK)
				{			
					if(rs->CreateImageSurface(desc.Width, desc.Height, desc.Format, &surface) == D3D_OK)
					{
						if(rs->CopyRects(renderTarget, null, 0, surface, null) == D3D_OK)
						{
							isOk = true;
						}
					}
				}
				if(!isOk) _CORE_API->Trace("Screen shot for fader not created!");
			}else{
				//Копируем шот
				if(rs->CopyRects(surface, null, 0, renderTarget, null) != D3D_OK)
				{
					_CORE_API->Trace("Can't copy fader screen shot to render target!");
				}
			}
		}
	}
	//Рисование затеняющего прямоугольника
	static struct
	{
		float x, y, z, rhw;
		dword color;
		float u, v;
	} drawbuf[6];
	if(alpha >= 1.0f)
	{
		alpha = 1.0f;
		if(!endFade)
		{
			endFade = true;
			eventEnd = true;
		}
	}
	dword color = (dword((fadeIn ? (1.0f - alpha) : alpha)*255.0f) << 24);
	if(textureID >= 0)  color |= 0x00ffffff;
	drawbuf[0].x = 0.0f; drawbuf[0].y = 0.0f; drawbuf[0].z = 0.5f; drawbuf[0].rhw = 1.0f; drawbuf[0].color = color; drawbuf[0].u = 0.0f; drawbuf[0].v = 0.0f;
	drawbuf[1].x = w;    drawbuf[1].y = 0.0f; drawbuf[1].z = 0.5f; drawbuf[1].rhw = 1.0f; drawbuf[1].color = color; drawbuf[1].u = 1.0f; drawbuf[1].v = 0.0f;
	drawbuf[2].x = 0.0f; drawbuf[2].y = h; drawbuf[2].z = 0.5f; drawbuf[2].rhw = 1.0f; drawbuf[2].color = color; drawbuf[2].u = 0.0f; drawbuf[2].v = 1.0f;
	drawbuf[3].x = 0.0f; drawbuf[3].y = h; drawbuf[3].z = 0.5f; drawbuf[3].rhw = 1.0f; drawbuf[3].color = color; drawbuf[3].u = 0.0f; drawbuf[3].v = 1.0f;
	drawbuf[4].x = w;    drawbuf[4].y = 0.0f; drawbuf[4].z = 0.5f; drawbuf[4].rhw = 1.0f; drawbuf[4].color = color; drawbuf[4].u = 1.0f; drawbuf[4].v = 0.0f;
	drawbuf[5].x = w;    drawbuf[5].y = h;    drawbuf[5].z = 0.5f; drawbuf[5].rhw = 1.0f; drawbuf[5].color = color; drawbuf[5].u = 1.0f; drawbuf[5].v = 1.0f;
	rs->TextureSet(0, textureID);
	if(tipsID >= 0)
	{
		rs->TextureSet(1, tipsID);
		rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2, 2, drawbuf, sizeof(drawbuf[0]), "FaderWithTips");
	}else{
		rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2, 2, drawbuf, sizeof(drawbuf[0]), "Fader");
	}
	//Увеличение alpha
	if(!endFade)
	{
		if(fadeSpeed > 0.0f) alpha += delta_time*0.001f*fadeSpeed; else alpha = 1.0f;
	}
	isStart = false;
}


