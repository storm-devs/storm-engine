#include "battle_land.h"
#include "defines.h"
#include "..\\common_h\\matrix.h"

inline long GetLongFromA(ATTRIBUTES *pA,char * sAttrName,long defValue=0)
{
	if(pA!=NULL) return pA->GetAttributeAsDword(sAttrName,defValue);
	else return defValue;
}

inline float GetFloatFromA(ATTRIBUTES *pA,char * sAttrName,float defValue=0)
{
	if(pA!=NULL) return pA->GetAttributeAsFloat(sAttrName,defValue);
	else return defValue;
}

inline long GetTextureFromA(VDX8RENDER * rs,ATTRIBUTES * pA,char * sAttrName)
{
	if(rs==NULL || pA==NULL) return -1;
	char * sname = pA->GetAttribute(sAttrName);
	if(sname==NULL || sname[0]==0) return -1;
	return rs->TextureCreate(sname);
}

BATTLE_LAND_INTERFACE::BATTLE_LAND_INTERFACE()
{
	rs = NULL;

	m_nShowCommand = 0;
	m_pCommandMenu = NULL;
	m_pMessageIcons = NULL;

	m_idShadowTexture = -1;
	m_idIconTexture[0] = m_idIconTexture[1] = m_idIconTexture[2] = m_idIconTexture[3] = -1;
	m_idStateTexture = -1;
	m_idGunChargeTexture = -1;
	m_bAlarmUp = true;
	m_nGunCharge = 0;

	m_idVBufCommandos	= -1;
	m_idIBufCommandos	= -1;
	m_idVBufHealth		= -1;
	m_idIBufHealth		= -1;
	m_idIBufGunShoot	= -1;
	m_fTexAOff			= 0.f;

	m_nCommandos = 0;
}

BATTLE_LAND_INTERFACE::~BATTLE_LAND_INTERFACE()
{
	Release();
}

bool BATTLE_LAND_INTERFACE::Init()
{
	if( (rs=(VDX8RENDER *)_CORE_API->CreateService("dx8render")) == NULL )
	{
		_THROW("Can`t create render service");
	}

	SetShowParameters();
	return true;
}

void BATTLE_LAND_INTERFACE::Execute(dword delta_time)
{
	if(m_nCommandos>0)
	{
		// мигание индикатора тревоги
		if(m_bAlarmUp)	m_fCurAlarmBlind += m_fAlarmBlindUpSpeed*delta_time;
		else m_fCurAlarmBlind -= m_fAlarmBlindDownSpeed*delta_time;
		if(m_fCurAlarmBlind<0.f) {m_fCurAlarmBlind = 0.f; m_bAlarmUp = true;}
		if(m_fCurAlarmBlind>1.f) {m_fCurAlarmBlind = 1.f; m_bAlarmUp = false;}

		UpdateAlarm();

		if(m_bEnableCommand)
		{
			if(!m_bShowCommandos)
			{
				if(m_nShowCommand!=0)
				{
					m_nShowCommand = 0;
					m_pCommandMenu->SetMode(0,-1,-1,-1);
					m_pCommandMenu->ResetStoredCommand();
					api->Event("blieGetMsgIconRoot");
				}
				if( AttributesPointer!=null )
					AttributesPointer->SetAttributeUseDword("ComState",0);
				return;
			}

			CONTROL_STATE cs;
			if(m_pCommandMenu->GetComState()>0)
			{
				m_nShowCommand = 1;
				DisableMessageIcons();

				api->Controls->GetControlState(BI_COMMANDS_CONFIRM,cs);
				if(cs.state == CST_ACTIVATED)
					m_pCommandMenu->DoControl(BI_MSG_COMMAND_ACTIVATE,-1);

				api->Controls->GetControlState(BI_COMMANDS_RIGHTSTEP,cs);
				if(cs.state == CST_ACTIVATED)
					m_pCommandMenu->DoControl(BI_MSG_COMMAND_RIGHT,-1);

				api->Controls->GetControlState(BI_COMMANDS_LEFTSTEP,cs);
				if(cs.state == CST_ACTIVATED)
					m_pCommandMenu->DoControl(BI_MSG_COMMAND_LEFT,-1);

				api->Controls->GetControlState(BI_COMMANDS_CANCEL,cs);
				if(cs.state == CST_ACTIVATED)
				{
					switch(m_pCommandMenu->GetComState())
					{
					case 1:
						m_nShowCommand = 0;
						m_pCommandMenu->SetMode(0,-1,-1,-1);
						m_pCommandMenu->ResetStoredCommand();
						api->Event("blieGetMsgIconRoot");
					break;
					case 2:
						m_nShowCommand = 1;
						m_pCommandMenu->SetMode(BI_COMMODE_COMMAND_SELECT,-1,-1,-1,1);
						m_pCommandMenu->ResetStoredCommand();
					break;
					}
				}
			}
			else
			{
				api->Controls->GetControlState(LI_COMMANDS_ACTIVATE,cs);
				if(cs.state == CST_ACTIVATED)
				{
					m_nShowCommand = 1;
					m_pCommandMenu->SetMode(BI_COMMODE_COMMAND_SELECT,-1,-1,-1,1);
					DisableMessageIcons();
				}
				else
				{
					if(m_nShowCommand!=0)
					{
						m_nShowCommand = 0;
						m_pCommandMenu->SetMode(0,-1,-1,-1);
						m_pCommandMenu->ResetStoredCommand();
						api->Event("blieGetMsgIconRoot");
					}
				}
			}

			if(m_nShowCommand>0)	m_pCommandMenu->Update();
		}

		if(m_bShowCommandos && m_nShowCommand==0)
		{
			m_pMessageIcons->Update(delta_time);
			UpdateCommandos();
		}
	}
	if( m_pCommandMenu!=null && AttributesPointer!=null )
		AttributesPointer->SetAttributeUseDword("ComState",m_nShowCommand);
}

void BATTLE_LAND_INTERFACE::Realize(dword delta_time)
{
#ifndef _XBOX
		//if(GetAsyncKeyState('K')<0) return;
#endif
	if(m_nCommandos>0)
	{
		CMatrix mtx;
		rs->SetTransform(D3DTS_WORLD,(D3DXMATRIX*)&mtx);

		if(m_bShowCommandos && m_nShowCommand==0)
		{
			rs->TextureSet(0,m_idShadowTexture);
			rs->DrawBuffer(m_idVBufCommandos, sizeof(BI_COLOR_VERTEX), m_idIBufCommandos, m_nMaxCommandos*4,m_nCommandos*4, 0,m_nCommandos*2, "battle_shadow_Rectangle");
			int iconNum=0;
			for(int i=0; i<4; i++)
			{
				if(iconNum>=m_nCommandos) break;
				if(m_idIconTexture[i]<0) continue;
				rs->TextureSet(0,m_idIconTexture[i]);
				rs->DrawBuffer(m_idVBufCommandos, sizeof(BI_COLOR_VERTEX), m_idIBufCommandos, iconNum*4,4, 0,2, "battle_tex_col_Rectangle");
				iconNum++;
			}
			rs->TextureSet(0,m_idStateTexture);
			rs->DrawBuffer(m_idVBufHealth, sizeof(BI_COLOR_VERTEX), m_idIBufCommandos, 0,m_nCommandos*4, 0,m_nCommandos*2, "battle_colorRectangle");
			rs->DrawBuffer(m_idVBufHealth, sizeof(BI_COLOR_VERTEX), m_idIBufHealth, m_nMaxCommandos*4,m_nCommandos*7, 0,m_nCommandos*4, "battle_colorRectangle");
			if(m_nGunCharge>0)
			{
				rs->TextureSet(0,m_idGunChargeTexture);
				rs->DrawBuffer(m_idVBufHealth, sizeof(BI_COLOR_VERTEX), m_idIBufGunShoot, m_nMaxCommandos*(4+7),m_nGunCharge*4, 0,m_nGunCharge*2, "battle_colorRectangle");
			}
		}
		if(m_bShowCommandos)
		{
			if(m_pCommandMenu!=NULL && m_nShowCommand>0)
				m_pCommandMenu->Draw();
			else if(m_pMessageIcons!=null)	m_pMessageIcons->Draw();
		}
	}
}

dword _cdecl BATTLE_LAND_INTERFACE::ProcessMessage(MESSAGE & message)
{
	switch (message.Long())
	{
	case MSG_BATTLE_LAND_START:
		//m_nShowCommand = 0;
		//m_bShowCommandos = true;
		SetParameters();
		api->Event("blieGetMsgIconRoot");
	break;

	case MSG_BATTLE_LAND_END:
		EndShow();
	break;

	case BI_IN_SET_COMMAND_MODE:
		if(m_bShowCommandos && m_pCommandMenu!=NULL && m_nShowCommand>0)
		{
			long comMode = message.Long();
			long startTextureNumber = message.Long();
			long startPictureNumber = message.Long();
			long characterNum = message.Long();
			if(comMode==0)	m_nShowCommand = 0;
			m_pCommandMenu->SetMode(comMode,startTextureNumber,startPictureNumber,characterNum);
		}
	break;

	case MSG_BATTLE_LAND_SET_SHOW:
		m_bShowCommandos = (message.Long()!=0);
	break;

	case MSG_BATTLE_LAND_SET_MSGICONS:
		EnableMessageIcons(message.ScriptVariablePointer());
	break;

	case BI_MSG_ADD_NEWTEXTURE:
		{
			char param[256];	message.String(sizeof(param)-1,param);
			int hQ = message.Long();
			int vQ = message.Long();
			if(m_pCommandMenu) return m_pCommandMenu->AddTexture(param, hQ, vQ);
			return -1;
		}
	break;

	case MSG_BATTLE_LAND_SET_ICONTEX:
		{
			int texIdx = message.Long();
			char param[256];
			message.String(sizeof(param)-1,param);
			if(texIdx>=0 && texIdx<4)
			{
				int newTexID = -1;
				if(param[0]) newTexID = rs->TextureCreate(param);
				TEXTURE_RELEASE(rs,m_idIconTexture[texIdx]);
				m_idIconTexture[texIdx] = newTexID;
			}
		}
	break;
	}
	return 0;
}

void BATTLE_LAND_INTERFACE::SetParameters()
{
	ATTRIBUTES * pA = NULL;

	PTR_DELETE(m_pCommandMenu);
	m_pCommandMenu = NEW BATTLE_COMMAND;
	if(m_pCommandMenu==NULL) {
		THROW("allocate memory error");
	}
	m_pCommandMenu->Init(GetID(),rs);

	PTR_DELETE(m_pMessageIcons);
	m_pMessageIcons = NEW MESSAGE_ICONS;
	if(m_pMessageIcons==NULL) {
		THROW("allocate memory error");
	}
	if(AttributesPointer!=NULL) pA = AttributesPointer->FindAClass(AttributesPointer,"MessageIcons");
	m_pMessageIcons->InitData(GetID(),rs,pA);

	pA = api->Entity_GetAttributeClass(&GetID(),"data");
	if(pA!=NULL)
		pA = pA->GetAttributeClass("icons");
	BI_COLOR_VERTEX *pVCBuf = (BI_COLOR_VERTEX*)rs->LockVertexBuffer(m_idVBufCommandos);
	if(pVCBuf==NULL)
	{
		THROW("Can`t locate vertex buffer");
	}

	m_nCommandos = pA!=NULL ? pA->GetAttributesNum() : 0;
	float fLeft = (float)m_left;
	for(int i=0;i<m_nCommandos;i++)
	{
		ATTRIBUTES * pATmp = pA->GetAttributeClass(i);
		if(pATmp==NULL) break;
		SetRectangleTexture(&pVCBuf[i*4], 0.f,0.f,1.f,1.f);
		fLeft += m_dist;
	}

	rs->UnLockVertexBuffer(m_idVBufCommandos);
	UpdateCommandos();
	UpdateAlarm();
}

void BATTLE_LAND_INTERFACE::Release()
{
	TEXTURE_RELEASE(rs,m_idShadowTexture);
	for(int i=0; i<4; i++)
	{
		TEXTURE_RELEASE(rs,m_idIconTexture[i]);
	}
	TEXTURE_RELEASE(rs,m_idStateTexture);
	TEXTURE_RELEASE(rs,m_idGunChargeTexture);

	VERTEX_BUFFER_RELEASE(rs,m_idVBufCommandos);
	INDEX_BUFFER_RELEASE(rs,m_idIBufCommandos);
	VERTEX_BUFFER_RELEASE(rs,m_idVBufHealth);
	INDEX_BUFFER_RELEASE(rs,m_idIBufHealth);
	INDEX_BUFFER_RELEASE(rs,m_idIBufGunShoot);

	PTR_DELETE(m_pCommandMenu);
	PTR_DELETE(m_pMessageIcons);

	rs = NULL;
}

void BATTLE_LAND_INTERFACE::EndShow()
{
	m_nCommandos = 0;
}

void BATTLE_LAND_INTERFACE::SetShowParameters()
{
	int i,j,nc,idx;
	ATTRIBUTES * pA = api->Entity_GetAttributeClass(&GetID(),"Parameters");

	long shadowW,shadowH,shadowTOP;
	long lc,tc,wc,hc,dc;
	long lh,th,wh,hh, tgun,hgun;

	// ƒанные дл€ иконок себ€ и офицеров
	shadowW = GetLongFromA(pA,"shadowW",8);
	shadowH = GetLongFromA(pA,"shadowH",8);
	shadowTOP = GetLongFromA(pA,"shadowTOP",8);
	wc = GetLongFromA(pA,"widthCom",64);
	hc = GetLongFromA(pA,"heightCom",64);
	lc = GetLongFromA(pA,"leftPosCom",16);
	tc = GetLongFromA(pA,"topPosCom",408);
	dc = m_dist = wc + GetLongFromA(pA,"distCom",8);

	// данные дл€ иконок здоровь€ и зар€да пистолетов
	wh = m_width = GetLongFromA(pA,"widthHealth",64);
	hh = GetLongFromA(pA,"heightHealth",8);
	lh = m_left = lc + (wc-wh)/2;
	th = tc - hh - GetLongFromA(pA,"distHealth",4);

	hgun = GetLongFromA(pA,"GunShootHeight",16);
	tgun = th - hgun - GetLongFromA(pA,"GunShootSpace",2);

	// ƒанные о количестве выводимой информации
	m_bEnableCommand = true;
	m_bShowCommandos = GetLongFromA(pA,"DoShowCommandos",true)!=0;
	m_nMaxCommandos = GetLongFromA(pA,"CommandosQuantity",4);

	// получим текстуры
	m_idShadowTexture = GetTextureFromA(rs,pA,"shadowTexture");
	m_idIconTexture[0] = GetTextureFromA(rs,pA,"iconTexture0");
	m_idIconTexture[1] = GetTextureFromA(rs,pA,"iconTexture1");
	m_idIconTexture[2] = GetTextureFromA(rs,pA,"iconTexture2");
	m_idIconTexture[3] = GetTextureFromA(rs,pA,"iconTexture3");
	m_idStateTexture = GetTextureFromA(rs,pA,"stateTexture");
	m_idGunChargeTexture = GetTextureFromA(rs,pA,"gunChargeTexture");

	// количество иконок в текстуре по вертикали и горизонтали
	m_nHQ = GetLongFromA(pA,"iconHorzQuantity",1);
	m_nVQ = GetLongFromA(pA,"iconVertQuantity",1);
	if(m_nHQ==0) m_nHQ=1;
	if(m_nVQ==0) m_nVQ=1;

	// ћигание иконки тревоги
	m_argbAlarmLightColor	= GetLongFromA(pA,"AlarmColorLight",0xFFFFFFFF);
	m_argbAlarmDarkColor	= GetLongFromA(pA,"AlarmColorDark",0xFF808080);
	m_argbPoisonLightColor	= GetLongFromA(pA,"PoisonColorLight",0xFFFFFFFF);
	m_argbPoisonDarkColor	= GetLongFromA(pA,"PoisonColorDark",0xFF808080);
	m_fAlarmBlindUpSpeed	= GetFloatFromA(pA,"AlarmBlindUpTime",1.f);
	m_fAlarmBlindDownSpeed	= GetFloatFromA(pA,"AlarmBlindDownTime",1.f);
	if(m_fAlarmBlindUpSpeed<=0.f) m_fAlarmBlindUpSpeed = 1.f;
	else m_fAlarmBlindUpSpeed = .001f/m_fAlarmBlindUpSpeed;
	if(m_fAlarmBlindDownSpeed<=0.f) m_fAlarmBlindDownSpeed = 1.f;
	else m_fAlarmBlindDownSpeed = .001f/m_fAlarmBlindDownSpeed;
	m_fCurAlarmBlind		= 1.f;

	// —оздадим буфера
	m_idVBufCommandos = rs->CreateVertexBuffer(BI_COLOR_VERTEX_FORMAT,m_nMaxCommandos*4*2*sizeof(BI_COLOR_VERTEX),D3DUSAGE_WRITEONLY);
	m_idIBufCommandos = rs->CreateIndexBuffer(m_nMaxCommandos*6*2);
	m_idVBufHealth = rs->CreateVertexBuffer(BI_COLOR_VERTEX_FORMAT,m_nMaxCommandos*(4+7+4*4)*sizeof(BI_COLOR_VERTEX),D3DUSAGE_WRITEONLY);
	m_idIBufHealth = rs->CreateIndexBuffer(m_nMaxCommandos*12*2);
	m_idIBufGunShoot = rs->CreateIndexBuffer(m_nMaxCommandos*4*6 * 2);

	if( m_idVBufCommandos==-1 || m_idIBufCommandos==-1 ||
		m_idVBufHealth==-1 || m_idIBufHealth==-1 ||
		m_idIBufGunShoot==-1 )
	{
		THROW("Can`t create vertex buffer");
	}

	// «аполним индекс буферы стандартными значени€ми
	WORD *pICBuf = (WORD*)rs->LockIndexBuffer(m_idIBufCommandos);
	WORD *pIHBuf = (WORD*)rs->LockIndexBuffer(m_idIBufHealth);
	WORD *pIGunBuf = (WORD*)rs->LockIndexBuffer(m_idIBufGunShoot);
	if(pICBuf==null || pIHBuf==null || pIGunBuf==null)
	{
		THROW("Can`t lock index buffer");
	}
	for(i=0; i<m_nMaxCommandos; i++)
	{
		idx=i*4;
		nc=i*6;
		pICBuf[nc] = idx;
		pICBuf[nc+1] = idx+1;
		pICBuf[nc+2] = idx+2;
		pICBuf[nc+3] = idx+1;
		pICBuf[nc+4] = idx+3;
		pICBuf[nc+5] = idx+2;

		idx=i*7;
		nc=i*12;
		pIHBuf[nc] = idx;
		pIHBuf[nc+1] = idx+1;
		pIHBuf[nc+2] = idx+3;
		pIHBuf[nc+3] = idx+1;
		pIHBuf[nc+4] = idx+4;
		pIHBuf[nc+5] = idx+3;
		pIHBuf[nc+6] = idx+1;
		pIHBuf[nc+7] = idx+2;
		pIHBuf[nc+8] = idx+5;
		pIHBuf[nc+9] = idx+2;
		pIHBuf[nc+10] = idx+6;
		pIHBuf[nc+11] = idx+5;
	}
	for(i=0; i<m_nMaxCommandos*4; i++)
	{
		nc=i*6;
		idx = i*4;
		pIGunBuf[nc+0] = idx;
		pIGunBuf[nc+1] = idx+1;
		pIGunBuf[nc+2] = idx+2;
		pIGunBuf[nc+3] = idx+2;
		pIGunBuf[nc+4] = idx+1;
		pIGunBuf[nc+5] = idx+3;
	}
	rs->UnLockIndexBuffer(m_idIBufCommandos);
	rs->UnLockIndexBuffer(m_idIBufHealth);
	rs->UnLockIndexBuffer(m_idIBufGunShoot);

	// «аполним вертекс буфера константными значени€ми
	BI_COLOR_VERTEX *pVCBuf = (BI_COLOR_VERTEX*)rs->LockVertexBuffer(m_idVBufCommandos);
	BI_COLOR_VERTEX *pVHBuf = (BI_COLOR_VERTEX*)rs->LockVertexBuffer(m_idVBufHealth);
	if(pVCBuf==NULL || pVHBuf==NULL)
	{
		THROW("Can`t lock vertex buffer");
	}
	for(i=0;i<m_nMaxCommandos;i++)
	{
		// подложка с тенью
		idx = (m_nMaxCommandos+i)*4;
		for(j=0;j<4;j++)
		{
			pVCBuf[idx+j].col = 0xFF808080;
			pVCBuf[idx+j].w = .5f;
			pVCBuf[idx+j].pos.z = 1.f;
		}
		SetRectanglePos(&pVCBuf[idx], float(lc + i*dc - shadowW), (float)(th - shadowTOP), float(lc + i*dc + wc + shadowW), float(tc + hc + shadowH));
		SetRectangleTexture(&pVCBuf[idx],0.f,0.f,1.f,1.f);

		// иконки действующих персонажей
		idx = i*4;
		for(j=0;j<4;j++)
		{
			pVCBuf[idx+j].col = 0xFF808080;
			pVCBuf[idx+j].w = .5f;
			pVCBuf[idx+j].pos.z = 1.f;
		}
		SetRectanglePos(&pVCBuf[idx],float(lc + i*dc),(float)tc,float(lc + i*dc + wc),float(tc + hc));

		// подложки под показатели жизни и зар€да
		idx = i*4;
		for(j=0;j<4;j++)
		{
			pVHBuf[idx+j].col = 0xFFFFFFFF;
			pVHBuf[idx+j].pos.z = 1.f;
			pVHBuf[idx+j].w = .5f;
		}
		SetRectanglePos(&pVHBuf[idx],float(lh + i*dc),(float)th,float(lh + i*dc + wh),float(th + hh));
		SetRectangleTexture(&pVHBuf[idx],0.f,.5f,1.f,1.f);

		// показатели жизни и зар€да
		idx = m_nMaxCommandos*4+i*7;
		for(j=0;j<7;j++)
		{
			pVHBuf[idx+j].col = 0xFFFFFFFF;
			pVHBuf[idx+j].pos.z = 1.f;
			pVHBuf[idx+j].w = .5f;
		}
		pVHBuf[idx].pos.x	= pVHBuf[idx+1].pos.x = pVHBuf[idx+2].pos.x	=	float(lh + i*dc);
		pVHBuf[idx].pos.y	= pVHBuf[idx+3].pos.y =							float(th);
		pVHBuf[idx+1].pos.y = pVHBuf[idx+4].pos.y = pVHBuf[idx+5].pos.y =	float(th + hh/2);
		pVHBuf[idx+2].pos.y = pVHBuf[idx+6].pos.y =							float(th + hh);
		pVHBuf[idx].tu = pVHBuf[idx+1].tu = pVHBuf[idx+2].tu =		0.f;
		pVHBuf[idx].tv =	pVHBuf[idx+3].tv =						0.f;
		pVHBuf[idx+1].tv =	pVHBuf[idx+4].tv = pVHBuf[idx+5].tv =	.25f;
		pVHBuf[idx+2].tv =	pVHBuf[idx+6].tv =						.5f;
	}

	// показатели числа оставшихс€ зар€дов
	idx = m_nMaxCommandos*(4+7);
	for(i=0;i<4*m_nMaxCommandos;i++)
	{
		for(j=0; j<4; j++)
		{
			pVHBuf[idx+j].col = 0xFFFFFFFF;
			pVHBuf[idx+j].pos.z = 1.f;
			pVHBuf[idx+j].w = .5f;
		}
		SetRectanglePos(&pVHBuf[idx],	0,(float)tgun,	0,(float)(tgun+hgun));
		SetRectangleTexture(&pVHBuf[idx],	0.f,0.f,	0.5f,1.f);
		idx += 4;
	}

	rs->UnLockVertexBuffer(m_idVBufCommandos);
	rs->UnLockVertexBuffer(m_idVBufHealth);
}

void BATTLE_LAND_INTERFACE::UpdateCommandos()
{
	ATTRIBUTES * pA = api->Entity_GetAttributeClass(&GetID(),"data");
	if(pA!=null)
		pA = pA->GetAttributeClass("icons");
	if(pA==null) return;
	BI_COLOR_VERTEX *pVHBuf = (BI_COLOR_VERTEX*)rs->LockVertexBuffer(m_idVBufHealth);
	if(pVHBuf==null) return;

	m_nGunCharge = 0;
	float fLeft = (float)m_left;
	for(int i=0;i<m_nCommandos;i++)
	{
		ATTRIBUTES * pATmp = pA->GetAttributeClass(i);
		if(pATmp==null) break;
		float fHP = pATmp->GetAttributeAsFloat("health");
		float fCharge = pATmp->GetAttributeAsFloat("charge");
		int nGunShootMax = pATmp->GetAttributeAsDword("shootMax");
		int nGunShootCur = pATmp->GetAttributeAsDword("shootCur");
		if(fHP<0.f) fHP=0.f;			if(fHP>1.f) fHP=1.f;
		if(fCharge<0.f) fCharge=0.f;	if(fCharge>1.f) fCharge=1.f;
		int idx = m_nMaxCommandos*4+i*7;
		pVHBuf[idx+3].pos.x = pVHBuf[idx+4].pos.x = fLeft + fCharge*m_width;
		pVHBuf[idx+5].pos.x = pVHBuf[idx+6].pos.x = fLeft + fHP*m_width;
		pVHBuf[idx+3].tu = pVHBuf[idx+4].tu = fCharge;
		pVHBuf[idx+5].tu = pVHBuf[idx+6].tu = fHP;

		float shootW = m_width * .25f;
		for(int k=0; k<nGunShootMax; k++)
		{
			idx = m_nMaxCommandos*(4+7) + m_nGunCharge*4;

			pVHBuf[idx+0].pos.x = pVHBuf[idx+1].pos.x = fLeft + shootW*k;
			pVHBuf[idx+2].pos.x = pVHBuf[idx+3].pos.x = fLeft + shootW*(k+1);

			if(k<nGunShootCur)
			{
				pVHBuf[idx+0].tu = pVHBuf[idx+1].tu = 0.0f;
				pVHBuf[idx+2].tu = pVHBuf[idx+3].tu = 0.5f;
			}
			else
			{
				pVHBuf[idx+0].tu = pVHBuf[idx+1].tu = 0.5f;
				pVHBuf[idx+2].tu = pVHBuf[idx+3].tu = 1.0f;
			}

			m_nGunCharge++;
		}
		fLeft += m_dist;
	}
	m_nCommandos = i;

	rs->UnLockVertexBuffer(m_idVBufHealth);
}

void BATTLE_LAND_INTERFACE::UpdateAlarm()
{
	ATTRIBUTES * pA = api->Entity_GetAttributeClass(&GetID(),"data");
	if(pA==NULL) return;
	bool bRiskAlarm = pA->GetAttributeAsDword("riskAlarm",0) != 0;

	BI_COLOR_VERTEX *pVCBuf = (BI_COLOR_VERTEX*)rs->LockVertexBuffer(m_idVBufCommandos);
	if(!pVCBuf) return;

	int i;
	if(bRiskAlarm)
	{
		DWORD dwCol = GetIntervalColor(m_argbAlarmDarkColor,m_argbAlarmLightColor, m_fCurAlarmBlind);
		for(i=0; i<16; i++)	pVCBuf[i].col = dwCol;
	}
	else	for(i=0; i<16; i++)	pVCBuf[i].col = 0xFF808080;

	pA = pA->GetAttributeClass("icons");
	for(i=0; i<m_nCommandos; i++)
	{
		ATTRIBUTES * pATmp = pA->GetAttributeClass(i);
		if(pATmp==null) continue;

		if( pATmp->GetAttributeAsDword("poison")!=0 )
		{
			DWORD dwCol = GetIntervalColor(m_argbPoisonDarkColor,m_argbPoisonLightColor, m_fCurAlarmBlind);
			for(int n=i*4; n<i*4+4; n++)
			{
				pVCBuf[n].col = dwCol;
			}
		}
	}

	rs->UnLockVertexBuffer(m_idVBufCommandos);
}

void BATTLE_LAND_INTERFACE::EnableMessageIcons(VDATA * pvdat)
{
	if(!m_pMessageIcons || !pvdat) return;
	m_pMessageIcons->SetShowMsg(true);

	ATTRIBUTES * pAttr[4];
	long pLeft[4];

	for(int i=0; i<4; i++)
	{
		pAttr[i] = null;
		pLeft[i] = m_left + i*m_dist;
		if(i<m_nCommandos)	pAttr[i] = pvdat->GetAClass(i);
	}

	m_pMessageIcons->StartData( pAttr, pLeft );
}

void BATTLE_LAND_INTERFACE::DisableMessageIcons()
{
	if(m_pMessageIcons==null) return;
	m_pMessageIcons->SetShowMsg(false);
}
