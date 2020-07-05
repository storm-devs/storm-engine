#include "backscene.h"
#include "..\..\common_h\model.h"
#include "..\..\common_h\matrix.h"
#include "..\..\common_h\geometry.h"
#include "..\..\common_h\templates\string.h"
#include "..\xinterface.h"

InterfaceBackScene::LightParam::~LightParam()
{
	bUse=false;
	if( pModel ) {
		api->DeleteEntity( eiModel );
		pModel = 0;
	}
}

void InterfaceBackScene::LightParam::UpdateParams( float fTime )
{
	float fK = 0.f;
	fColorTimer += fTime;
	long jjj = 0;
	while( fColorTimer > fColorPeriod + fAddPeriod )
	{

		if(fColorTimer > fAddPeriodMax*10.0f)
		{
			fColorTimer = fAddPeriodMax*10.0f;
		}

		fColorTimer -= fColorPeriod + fAddPeriod;
		fAddPeriod = FRAND(fAddPeriodMax);//(-0.6f+FRAND(3.0f))*fColorPeriod;
		jjj++;
		if (jjj > 10000) 
		{
			api->Trace("jjj: %f, %f", fColorTimer, fColorPeriod);
			_asm int 3
		}
	}
	float fPer = fColorPeriod + fAddPeriod;
	if( fColorTimer <= .5f*fPer ) fK = 2.f * fColorTimer / fPer;
	else fK = 2.f - 2.f * fColorTimer / fPer;
	lightSource.Diffuse.a = colorMin.a + (colorMax.a - colorMin.a) * fK; // 1.f
	lightSource.Diffuse.r = colorMin.r + (colorMax.r - colorMin.r) * fK; // 1.0f;
	lightSource.Diffuse.g = colorMin.g + (colorMax.g - colorMin.g) * fK; // 1.0f;
	lightSource.Diffuse.b = colorMin.b + (colorMax.b - colorMin.b) * fK; // 0.7f;

	dwFlareColor = dword(fMinFlareColor + (fMaxFlareColor-fMinFlareColor) * fK);
	dwFlareColor = dwFlareColor | (dwFlareColor<<24) | (dwFlareColor<<16) | (dwFlareColor<<8);

	CVECTOR vPos = vLightPos;
	if( pLightSrcNode )
		vPos = pLightSrcNode->glob_mtx * vLightPos;

	lightSource.Position.x = vPos.x;
	lightSource.Position.y = vPos.y;
	lightSource.Position.z = vPos.z;

	fRangeTimer += fTime;
	if( fRangeTimer > fRangePeriod ) fRangeTimer -= fRangePeriod;
	if( fRangeTimer <= .5f*fRangePeriod ) fK = 2.f * fRangeTimer / fRangePeriod;
	else fK = 2.f - 2.f * fRangeTimer / fRangePeriod;
	lightSource.Range = fRangeMin + (fRangeMax - fRangeMin) * fK; // 10.f;
}

InterfaceBackScene::MenuDescr::~MenuDescr()
{
	api->DeleteEntity(eiActive);
	pActive = 0;
	api->DeleteEntity(eiPassive);
	pPassive = 0;
}

void InterfaceBackScene::MenuDescr::Set( CMatrix* pMtx, const char* pcActiveName, const char* pcPassiveName, const char* pcEvent, const char* pcPathName, const char* pcTechniqueName )
{
	if( !pcTechniqueName ) pcTechniqueName = "InterfaceBackScene_Menu";
	sEventName = pcEvent;
	VGEOMETRY* pGeo = (VGEOMETRY*)api->CreateService("Geometry");
	if( pGeo )
		if( pcPathName && pcPathName[0] ) pGeo->SetTexturePath(string("MainMenu\\")+pcPathName+"\\");
		else pGeo->SetTexturePath("MainMenu\\");
	// create active model
	if( pcActiveName )
	{
		api->CreateEntity( &eiActive, "MODELR" );
		api->Send_Message( eiActive, "ls", MSG_MODEL_LOAD_GEO, pcActiveName );
		pActive = (MODEL*)api->GetEntityPointer( &eiActive );
		if( pActive && pMtx )
		{
			pActive->mtx = *pMtx;
			pActive->Update();
			if( pActive->GetNode(0) ) pActive->GetNode(0)->SetTechnique(pcTechniqueName);
		} else {
			api->Trace("Warning! Interface Back Scene: invalid menu model %s or transform matrix", pcActiveName);
		}
	}
	// create passive model
	if( pcPassiveName )
	{
		api->CreateEntity( &eiPassive, "MODELR" );
		api->Send_Message( eiPassive, "ls", MSG_MODEL_LOAD_GEO, pcPassiveName );
		pPassive = (MODEL*)api->GetEntityPointer( &eiPassive );
		if( pPassive && pMtx )
		{
			pPassive->mtx = *pMtx;
			pPassive->Update();
			if( pPassive->GetNode(0) ) pPassive->GetNode(0)->SetTechnique(pcTechniqueName);
		} else {
			api->Trace("Warning! Interface Back Scene: invalid menu model %s or transform matrix", pcPassiveName);
		}
	}
	if( pGeo ) pGeo->SetTexturePath("");
	if( pActive ) bSelectable = true;
}

InterfaceBackScene::InterfaceBackScene() :
	m_aMenuDescr(_FL),
	m_aLights(_FL),
	m_apAniModel(_FL)
{
	m_pModel = null;
	m_pLocators = null;

	m_vCamPos = 0.f;
	m_vCamAng = 0.f;
	m_fCamPerspective = 1.287f;

	m_nFlareTexture = -1;

	//Мухи
	flys = null;
	numFlys = 0;
	maxFlys = 0;
	fly = null;
	numFly = 0;
	flyTex = -1;
}

InterfaceBackScene::~InterfaceBackScene()
{
	RestoreLight();
	api->DeleteEntity( m_eiModel );
	api->DeleteEntity( m_eiLocators );
	m_pLocators = null;
	m_pModel = null;
	ReleaseMenuList();
	m_aLights.DelAllWithPointers();
	m_apAniModel.DelAllWithPointers();

	if( m_nFlareTexture >=0 ) m_pRS->TextureRelease(m_nFlareTexture); m_nFlareTexture = -1;

	if(flyTex >= 0) m_pRS->TextureRelease(flyTex); flyTex = -1;
	if(flys) delete flys; flys = 0;
	if(fly) delete fly; fly = 0;
}

bool InterfaceBackScene::Init()
{
	m_pRS = (VDX8RENDER *)api->CreateService("dx8render");	Assert(m_pRS);
	flyTex = m_pRS->TextureCreate("LocEfx\\firefly.tga");
	m_nFlareTexture = m_pRS->TextureCreate("ShipsFlares\\corona.tga");
	return true;
}

void InterfaceBackScene::Execute(dword Delta_Time)
{
	long nOldMenuIndex = m_nSelectMenuIndex;

	FXYPOINT pntMouse = XINTERFACE::pThis->GetMousePoint();
	if( m_pntOldMouse.x!=pntMouse.x || m_pntOldMouse.y!=pntMouse.y )
	{
		m_pntOldMouse = pntMouse;
		long n = CheckMousePos( pntMouse.x, pntMouse.y );
		if( n>=0 && n!=m_nSelectMenuIndex ) SetNewMenu(n);
	}

	CONTROL_STATE cs;
	api->Controls->GetControlState("IUp",cs);
	if( cs.state==CST_ACTIVATED ) {
		ChoosePrevMenu();
	}
	api->Controls->GetControlState("IDown",cs);
	if( cs.state==CST_ACTIVATED ) {
		ChooseNextMenu();
	}
	api->Controls->GetControlState("ILClick",cs);
	if( cs.state==CST_ACTIVATED ) {
		ExecuteMenu( CheckMousePos( m_pntOldMouse.x, m_pntOldMouse.y ) );
	}
	api->Controls->GetControlState("IAction",cs);
	if( cs.state==CST_ACTIVATED ) {
		ExecuteMenu( m_nSelectMenuIndex );
	}

	if( api->Controls->GetDebugAsyncKeyState(VK_CONTROL)<0 )
	{
		CMatrix mtx;
		mtx.BuildMatrix(m_vCamAng);
		CVECTOR vz = mtx * CVECTOR(0.f,0.f,1.f);
		CVECTOR vx = mtx * CVECTOR(1.f,0.f,0.f);

		float fForwardSpeed = 0.01f * Delta_Time;
		if( api->Controls->GetDebugAsyncKeyState(VK_SHIFT)<0 ) fForwardSpeed *= 10.f;
		if( api->Controls->GetDebugAsyncKeyState(VK_MENU)<0 ) fForwardSpeed *= 0.1f;
		float fSideSpeed = 0.5f * fForwardSpeed;

		if( api->Controls->GetDebugAsyncKeyState('W')<0 ) m_vCamPos += vz * fForwardSpeed;
		if( api->Controls->GetDebugAsyncKeyState('S')<0 ) m_vCamPos -= vz * fForwardSpeed;
		if( api->Controls->GetDebugAsyncKeyState('D')<0 ) m_vCamPos += vx * fSideSpeed;
		if( api->Controls->GetDebugAsyncKeyState('A')<0 ) m_vCamPos -= vx * fSideSpeed;

		float fRotateSpeed = 0.001f * Delta_Time;
		if( api->Controls->GetDebugAsyncKeyState(VK_UP)<0 ) m_vCamAng.x += fRotateSpeed;
		if( api->Controls->GetDebugAsyncKeyState(VK_DOWN)<0 ) m_vCamAng.x -= fRotateSpeed;
		if( api->Controls->GetDebugAsyncKeyState(VK_LEFT)<0 ) m_vCamAng.y -= fRotateSpeed;
		if( api->Controls->GetDebugAsyncKeyState(VK_RIGHT)<0 ) m_vCamAng.y += fRotateSpeed;
	}

	m_pRS->SetCamera( &m_vCamPos, &m_vCamAng, m_fCamPerspective );

	if( nOldMenuIndex != m_nSelectMenuIndex )
		api->Event(ISOUND_EVENT,"l",2); // выбор нового нода

/*	for( long n=0; n<m_apAniModel; n++ )
	{
		if( m_apAniModel[n]->pModel )
			m_apAniModel[n]->pModel->Execute( Delta_Time );
	}*/
}

void InterfaceBackScene::Realize(dword Delta_Time)
{
	long n;

	ProcessedFlys( Delta_Time*.001f );

	// отрисовка модели
	if( m_pModel )
	{
		SetLight();
		m_pRS->SetRenderState(D3DRS_LIGHTING, true);
		m_pModel->Realize(Delta_Time);
		for( n=0; n<m_aLights; n++ ) // показать все фонари
			if( m_aLights[n]->pModel )
			{
				m_aLights[n]->pModel->Realize(Delta_Time);
				FlareShow(n);
			}
		for( n=0; n<m_apAniModel; n++ )
		{
			if( m_apAniModel[n]->pModel ) {
				dword dwTFactor;
				m_pRS->GetRenderState(D3DRS_TEXTUREFACTOR,&dwTFactor);
				if( m_apAniModel[n]->bUseTFactor )
					m_pRS->SetRenderState(D3DRS_TEXTUREFACTOR,m_apAniModel[n]->dwTFactor);
				m_apAniModel[n]->pModel->Realize( Delta_Time );
				m_pRS->SetRenderState(D3DRS_TEXTUREFACTOR,dwTFactor);
			}
		}
		m_pRS->SetRenderState(D3DRS_LIGHTING, false);
		RestoreLight();
	}

	// отрисовка пунктов меню
	for( n=0; n<m_aMenuDescr; n++ )
	{
		if( n==m_nSelectMenuIndex && m_aMenuDescr[n]->pActive )
			m_aMenuDescr[n]->pActive->Realize( Delta_Time );
		else if( m_aMenuDescr[n]->pPassive )
			m_aMenuDescr[n]->pPassive->Realize( Delta_Time );
	}
}

dword _cdecl InterfaceBackScene::ProcessMessage(MESSAGE & message)
{
	long nMsgCode = message.Long();
	char param[1024];
	switch( nMsgCode )
	{

	case 0: // load model
		message.String( sizeof(param), param );
		LoadModel( param );
	break;

	case 1: // set camera
		message.String( sizeof(param), param );
		SetCameraPosition( param );
	break;

	case 2: // set ship position by locator
		message.String( sizeof(param), param );
		SetShipPosition( param, message.AttributePointer() );
	break;

	case 3: // create menu list
		{
			long nStartIdx = message.Long();
			ATTRIBUTES* pA = message.AttributePointer();
			CreateMenuList( nStartIdx, pA );
		}
	break;

	case 4: // controling of menu list
		{
			long nControlCode = message.Long();
			if( nControlCode&1 ) ChooseNextMenu();
			else ChoosePrevMenu();
		}
	break;

	case 5: // get current menu
		return m_nSelectMenuIndex;
	break;

	case 6: // set current menu
		{
			long n = message.Long();
			if( n<0 || n>=m_aMenuDescr || !m_aMenuDescr[n]->bSelectable ) n = -1;
			else m_nSelectMenuIndex = n;
		}
	break;

	case 7: // set selectable flag for menu
		{
			long num = message.Long(); // menu number
			long flag = message.Long(); // selectable state
			SetMenuSelectableState( num, flag!=0 );
		}
	break;

	case 8: // set light source
		{
			message.String( sizeof(param), param ); // light attributes name
			InitLight( AttributesPointer ? AttributesPointer->GetAttributeClass(param) : 0 );
		}
	break;

	case 9: // add animation model
		{
			message.String( sizeof(param), param ); // animation model attributes name
			InitAniModel( AttributesPointer ? AttributesPointer->GetAttributeClass(param) : 0 );
		}
	break;

	case 10: // add model
		{
			message.String( sizeof(param), param ); // model attributes name
			InitStaticModel( AttributesPointer ? AttributesPointer->GetAttributeClass(param) : 0 );
		}
	break;
	}
	return 0;
}

void InterfaceBackScene::LoadModel( const char* pcModelName )
{
	// delete all
	if( m_pModel ) {
		api->DeleteEntity( m_eiModel );
		m_pModel = null;
	}
	if( m_pLocators ) {
		api->DeleteEntity( m_eiLocators );
		m_pLocators = null;
	}
	VGEOMETRY* pGeo = (VGEOMETRY*)api->CreateService("Geometry");
	if( pGeo ) pGeo->SetTexturePath(string("MainMenu\\")+XINTERFACE::pThis->StringService()->GetLanguage()+"\\");
	// create model
	api->CreateEntity( &m_eiModel, "MODELR" );
	api->Send_Message( m_eiModel, "ls", MSG_MODEL_LOAD_GEO, pcModelName );
	m_pModel = (MODEL*)api->GetEntityPointer( &m_eiModel );
	if( pGeo ) pGeo->SetTexturePath("");
	api->LayerAdd( "sun_trace", m_eiModel, 0 );
	api->LayerAdd("rain_drops", m_eiModel, 100);
	// create locators
	api->CreateEntity( &m_eiLocators, "MODELR" );
	string sLocName = string(pcModelName) + "_locators";
	api->Send_Message( m_eiLocators, "ls", MSG_MODEL_LOAD_GEO, sLocName.GetBuffer() );
	m_pLocators = (MODEL*)api->GetEntityPointer( &m_eiLocators );
}

void InterfaceBackScene::SetCameraPosition( const char* pcLocatorName )
{
	//FindLocator( pcLocatorName, 0, &m_vCamPos, &m_vCamAng.y );
	Matrix mtx;
	FindLocator( pcLocatorName, (CMatrix*)&mtx, &m_vCamPos, 0 );
	Vector vAddZ;
	mtx.GetAngles( m_vCamAng.x, m_vCamAng.y, m_vCamAng.z );
	vAddZ = mtx.MulNormal( Vector(0.f,0.f,1.f) );
	vAddZ *= -0.1f;
	m_vCamPos.x += vAddZ.x;
	m_vCamPos.y += vAddZ.y;
	m_vCamPos.z += vAddZ.z;
	//m_vCamAng.x = -4.262f/180.f*PI;
}

void InterfaceBackScene::SetShipPosition( const char* pcLocName, ATTRIBUTES* pAChar )
{
	if( !pcLocName || !pAChar || !m_pLocators ) return;

	ATTRIBUTES* pAPos = pAChar->FindAClass(pAChar,"Ship.Pos");
	if( !pAPos ) pAPos = pAChar->CreateSubAClass(pAChar,"Ship.Pos");
	Assert(pAPos);

	ATTRIBUTES* pAAng = pAChar->FindAClass(pAChar,"Ship.Ang");
	if( !pAAng ) pAAng = pAChar->CreateSubAClass(pAChar,"Ship.Ang");
	Assert(pAAng);

	CVECTOR pos;
	float fYAng;
	if( FindLocator( pcLocName, 0, &pos, &fYAng ) )
	{
		pAPos->SetAttributeUseFloat("x", pos.x);
		pAPos->SetAttributeUseFloat("y", pos.y);
		pAPos->SetAttributeUseFloat("z", pos.z);
		pAAng->SetAttributeUseFloat("y", fYAng );
	}
}

bool InterfaceBackScene::FindLocator( const char* pcLocName, CMatrix* pMtx, CVECTOR* pPos, float* pYAng )
{
	if( !pcLocName || !m_pLocators ) return false;
	for( long n=0; n<100; n++ )
	{
		NODE* pNod = m_pLocators->GetNode(n);
		if( !pNod ) break;
		GEOS::INFO ginf;
		pNod->geo->GetInfo( ginf );
		GEOS::LABEL lbl;
		for( long l=0; l<ginf.nlabels; l++ )
		{
			pNod->geo->GetLabel( l, lbl );
			if( lbl.name && stricmp(pcLocName,lbl.name)==0 )
			{
				if( pMtx ) {
					memcpy( pMtx, &lbl.m, sizeof(float)*4*4 );
				}
				if( pPos ) {
					pPos->x = lbl.m[3][0] + pNod->glob_mtx.matrix[3];
					pPos->y = lbl.m[3][1] + pNod->glob_mtx.matrix[7];
					pPos->z = lbl.m[3][2] + pNod->glob_mtx.matrix[11];
				}
				if( pYAng ) {
					*pYAng = atan2f( lbl.m[2][0], lbl.m[2][2] );
				}
				return true;
			}
		}
	}
	return false;
}

void InterfaceBackScene::SetLocatorPosition( MODEL* pModel, const char* pcLocName, CVECTOR & pos, NODE* &pNodPtr )
{
	pos = 0.f;
	if( pModel && pcLocName )
	{
		for( long n=0; n<100; n++ )
		{
			NODE* pNod = pModel->GetNode(n);
			if( !pNod ) break;
			GEOS::INFO ginf;
			pNod->geo->GetInfo( ginf );
			GEOS::LABEL lbl;
			for( long l=0; l<ginf.nlabels; l++ )
			{
				pNod->geo->GetLabel( l, lbl );
				if( lbl.name && stricmp(pcLocName,lbl.name)==0 )
				{
					pos.x = lbl.m[3][0];
					pos.y = lbl.m[3][1];
					pos.z = lbl.m[3][2];
					pNodPtr = pNod;
					return;
				}
			}
		}
	}
}

void InterfaceBackScene::ReleaseMenuList()
{
	m_aMenuDescr.DelAllWithPointers();
}

void InterfaceBackScene::CreateMenuList( long nStartIndex, ATTRIBUTES* pAMenu )
{
	ReleaseMenuList();
	if( !pAMenu ) return;

	ATTRIBUTES* pA;
	CMatrix mtx;
	long q = pAMenu->GetAttributesNum();
	for( long n=0; n<q; n++ )
	{
		pA = pAMenu->GetAttributeClass(n);
		if( !pA ) continue;
		if( !FindLocator( pA->GetAttribute("locname"), &mtx, 0, 0 ) )
		{
			api->Trace( "Warning! Interface Back scene: Can`t find locator %s", pA->GetAttribute("locname") );
		}
		MenuDescr* pMD = NEW MenuDescr;
		Assert(pMD);;
		pMD->Set( &mtx, pA->GetAttribute("sel"), pA->GetAttribute("norm"), pA->GetAttribute("event"), pA->GetAttribute("path"), pA->GetAttribute("technique") );
		m_aMenuDescr.Add( pMD );
	}
	if( nStartIndex>=0 && nStartIndex<m_aMenuDescr && m_aMenuDescr[nStartIndex]->bSelectable )
		m_nSelectMenuIndex = nStartIndex;
	else m_nSelectMenuIndex = -1;
}

void InterfaceBackScene::ChooseNextMenu()
{
	for( long n=m_nSelectMenuIndex+1; n<m_aMenuDescr; n++ )
		if( m_aMenuDescr[n]->bSelectable )
			break;
	if( n<m_aMenuDescr ) m_nSelectMenuIndex = n;
}

void InterfaceBackScene::ChoosePrevMenu()
{
	for( long n=m_nSelectMenuIndex-1; n>=0; n-- )
		if( m_aMenuDescr[n]->bSelectable )
			break;
	if( n>=0 ) m_nSelectMenuIndex = n;
}

void InterfaceBackScene::SetNewMenu(long nNewSelectIndex)
{
	if( nNewSelectIndex<0 || nNewSelectIndex>=m_aMenuDescr || !m_aMenuDescr[nNewSelectIndex]->bSelectable ) return;
	m_nSelectMenuIndex = nNewSelectIndex;
}

void InterfaceBackScene::SetMenuSelectableState( long nMenuIndex, bool bSelectable )
{
	if( nMenuIndex<0 || nMenuIndex>=m_aMenuDescr ) return;
	if( m_aMenuDescr[nMenuIndex]->bSelectable == bSelectable ) return;
	if( bSelectable && m_aMenuDescr[nMenuIndex]->pActive && m_aMenuDescr[nMenuIndex]->pPassive )
		m_aMenuDescr[nMenuIndex]->bSelectable = true;
	else m_aMenuDescr[nMenuIndex]->bSelectable = false;
}

void InterfaceBackScene::ExecuteMenu( long nMenuIndex )
{
	if( nMenuIndex<0 || nMenuIndex>=m_aMenuDescr ) return;
	api->PostEvent( "backgroundcommand", 1, "s", m_aMenuDescr[nMenuIndex]->sEventName.GetBuffer() );
}

long InterfaceBackScene::CheckMousePos( float fX, float fY )
{
	float fW = (float)XINTERFACE::pThis->GetScreenWidth();
	float fH = (float)XINTERFACE::pThis->GetScreenHeight();
	float fRelX = 2.f * fX / fW - 1.f;
	float fRelY = 2.f * fY / fH - 1.f;

	CMatrix mtxProj;
	m_pRS->GetTransform(D3DTS_PROJECTION,(D3DXMATRIX*)&mtxProj);
	CVECTOR v;
	v.x = fRelX / mtxProj.m[0][0];
	v.y = - fRelY / mtxProj.m[1][1];
	v.z = 1.0f;

	CMatrix mtxView;
	m_pRS->GetTransform(D3DTS_VIEW,(D3DXMATRIX*)&mtxView);
	CVECTOR vDir;
	mtxView.MulToInvNorm(v,vDir);
	mtxView.Transposition();
	CVECTOR vStart = mtxView.Pos();
	CVECTOR vEnd = vStart + vDir * 300.f;

	for( long n=0; n<m_aMenuDescr; n++ )
		if( m_aMenuDescr[n]->bSelectable &&
			m_aMenuDescr[n]->pActive &&
			m_aMenuDescr[n]->pActive->Trace( vStart, vEnd ) <= 1.f )
			return n;
	return -1;
}

void InterfaceBackScene::InitLight( ATTRIBUTES* pAParam )
{
	if( !pAParam ) return;

	LightParam* pLight = NEW LightParam();
	Assert(pLight);

	m_aLights.Add(pLight);

	ZERO(pLight->lightSource);
	pLight->lightSource.Type = D3DLIGHT_POINT;
	pLight->lightSource.Attenuation0 = 0.0f;
	pLight->lightSource.Attenuation1 = 0.0f;
	pLight->lightSource.Attenuation2 = 1.0f;
	pLight->indexLight = -1;

	float fDiv = 1.f / 255.f;
	dword dwTmp = pAParam->GetAttributeAsDword("lightcolormin",0xFFFFFFFF);
	pLight->colorMin.a = ALPHA(dwTmp) * fDiv;
	pLight->colorMin.r = RED(dwTmp) * fDiv;
	pLight->colorMin.g = GREEN(dwTmp) * fDiv;
	pLight->colorMin.b = BLUE(dwTmp) * fDiv;
	dwTmp = pAParam->GetAttributeAsDword("lightcolormax",0xFFFFFFFF);
	pLight->colorMax.a = ALPHA(dwTmp) * fDiv;
	pLight->colorMax.r = RED(dwTmp) * fDiv;
	pLight->colorMax.g = GREEN(dwTmp) * fDiv;
	pLight->colorMax.b = BLUE(dwTmp) * fDiv;
	pLight->fColorPeriod = pAParam->GetAttributeAsFloat("colorperiod",1.f);
	pLight->fAddPeriodMax = pAParam->GetAttributeAsFloat("addcolorperiod",1.f);
	pLight->fColorTimer = 0.f;

	pLight->fRangeMin = pAParam->GetAttributeAsFloat("rangemin",5.f);
	pLight->fRangeMax = pAParam->GetAttributeAsFloat("rangemax",10.f);
	pLight->fRangePeriod = pAParam->GetAttributeAsFloat("rangeperiod",1.f);
	pLight->fRangeTimer = 0.f;

	pLight->fMinFlareColor = pAParam->GetAttributeAsFloat("minflarecolor",200.f);
	pLight->fMaxFlareColor = pAParam->GetAttributeAsFloat("maxflarecolor",255.f);

	pLight->bUse = pAParam->GetAttributeAsDword("turnon",0)!=0;

	// find transform from locator
	CMatrix locMtx;
	FindLocator( pAParam->GetAttribute("locator"), &locMtx, 0, 0 );
	pLight->vLightPos = locMtx.Pos();

	// load model
	char* pcFonarModel = pAParam->GetAttribute("model");
	if( pcFonarModel )
	{
		VGEOMETRY* pGeo = (VGEOMETRY*)api->CreateService("Geometry");
		if( pGeo ) pGeo->SetTexturePath("MainMenu\\");
		// create model
		api->CreateEntity( &pLight->eiModel, "MODELR" );
		api->Send_Message( pLight->eiModel, "ls", MSG_MODEL_LOAD_GEO, pcFonarModel );
		pLight->pModel = (MODEL*)api->GetEntityPointer( &pLight->eiModel );
		if( pGeo ) pGeo->SetTexturePath("");
		if( pLight->pModel )
		{
			pLight->pModel->mtx = locMtx;
			pLight->pModel->Update();

			pLight->pLightSrcNode = pLight->pModel->GetNode(0);
			SetLocatorPosition( pLight->pModel, pAParam->GetAttribute("lightlocator"), pLight->vLightPos, pLight->pLightSrcNode );
			if(m_aLights.Size()>0 && m_aLights[0]->bUse) {
				CVECTOR vFlarePos = pLight->vLightPos;
				SetLocatorPosition( pLight->pModel, pAParam->GetAttribute("flarelocator"), vFlarePos, pLight->pLightSrcNode );
				m_vFlarePos = pLight->pLightSrcNode->glob_mtx * vFlarePos;
				m_fFlareSize = pAParam->GetAttributeAsFloat("flaresize",0.2f);
				AddLampFlys( m_vFlarePos );

				ATTRIBUTES* pA = 0;
				if( AttributesPointer ) pA = AttributesPointer->CreateSubAClass(AttributesPointer,"lightpos");
				if( pA ) {
					pA->SetAttributeUseFloat("x",m_vFlarePos.x);
					pA->SetAttributeUseFloat("y",m_vFlarePos.y);
					pA->SetAttributeUseFloat("z",m_vFlarePos.z);
				}
			}
		} else {
			api->Trace("Warning! Interface Back Scene: invalid torchlight model %s", pcFonarModel);
		}
	}
}

void InterfaceBackScene::SetLight()
{
	long nFreeLightIndex=0;
	D3DCAPS8 d3dcaps;	m_pRS->GetDeviceCaps(&d3dcaps);

	for( long n=0; n<m_aLights; n++ )
	{
		if( m_aLights[n]->bUse ) {
			BOOL bTmp;
			for( ; nFreeLightIndex<(long)d3dcaps.MaxActiveLights; nFreeLightIndex++ )
				if( m_pRS->GetLightEnable(nFreeLightIndex,&bTmp) && bTmp==false ) break;
			if( nFreeLightIndex<(long)d3dcaps.MaxActiveLights ) { // нашли свободный источник
				m_aLights[n]->indexLight = nFreeLightIndex;
				m_pRS->GetLight( nFreeLightIndex, &m_aLights[n]->lightOldSource );
				m_pRS->LightEnable(nFreeLightIndex,true);
				m_aLights[n]->UpdateParams( api->GetDeltaTime()*.001f );
				m_pRS->SetLight(nFreeLightIndex, &m_aLights[n]->lightSource);
			} else m_aLights[n]->indexLight = -1;
		}
	}
}

void InterfaceBackScene::RestoreLight()
{
	for( long n=0; n<m_aLights; n++ )
	{
		if( m_aLights[n]->bUse ) {
			if( m_aLights[n]->indexLight >= 0 )
			{
				m_pRS->SetLight( m_aLights[n]->indexLight, &m_aLights[n]->lightOldSource);
				m_pRS->LightEnable(m_aLights[n]->indexLight,false);
				m_aLights[n]->indexLight = -1;
			}
		}
	}
}

void InterfaceBackScene::FlareShow(long idx)
{
	CVECTOR pos, ang;
	m_pRS->GetCamera(pos, ang, ang.x);
	CMatrix camMtx;
	m_pRS->GetTransform(D3DTS_VIEW, camMtx);

	float dx = m_vFlarePos.x - pos.x;
	float dy = m_vFlarePos.y - pos.y;
	float dz = m_vFlarePos.z - pos.z;

	/*	float d = dx*dx + dy*dy + dz*dz;

	//Дистанция
	float dist = sqrtf(d);
	d = dist / m_fFlareRange;
	if(d > 1.0f) d = 1.0f;
	float alpha = 1.0f;
	if(d < 0.3f) alpha *= 0.2f + 0.8f*d/0.3f;
	if(d > 0.4f){ alpha *= 1.0f - (d - 0.4f)/0.6f; alpha *= alpha; }
	alpha *= 255.f;//ls.corona*255.0f;
	//Коэфициент отклонения
	d = 0.f;//ls.i*0.4f;
	if(d < -0.1f) d = -0.1f;
	if(d > 0.1f) d = 0.1f;
	d += 1.0f;
	//Текущий размер
	float size = d*m_fFlareSize;
	//Прозрачность
	alpha *= d;
	if(alpha < 0.0f) alpha = 0.0f;
	if(alpha > 255.0f) alpha = 255.0f;
	//Позиция
	pos = camMtx * m_vFlarePos;
	//Цвет
	dword c = dword(alpha); c |= (c << 24) | (c << 16) | (c << 8);*/
	//Угол поворота
	float cs, sn;
	float _cs = (dx*camMtx.Vx().z + dz*camMtx.Vz().z);
	float _sn = (dx*camMtx.Vz().z - dz*camMtx.Vx().z);
	float kn = _cs*_cs + _sn*_sn;
	if(kn > 0.0f)
	{
		kn = 1.0f/sqrtf(kn);
		_cs *= kn; _sn *= kn;
		cs = (_cs*_cs - _sn*_sn);
		sn = 2.0f*_cs*_sn;
	}else{
		cs = 1.0f;
		sn = 0.0f;
	}

	//Позиция
	pos = m_vFlarePos;

	dword c = m_aLights[idx]->dwFlareColor;

	buffer[0].pos = pos + CVECTOR(m_fFlareSize*(-cs + sn), m_fFlareSize*(sn + cs), 0.0f);
	buffer[0].color = c;
	buffer[0].u = 0.0f;
	buffer[0].v = 0.0f;
	buffer[1].pos = pos + CVECTOR(m_fFlareSize*(-cs - sn), m_fFlareSize*(sn - cs), 0.0f);
	buffer[1].color = c;
	buffer[1].u = 0.0f;
	buffer[1].v = 1.0f;
	buffer[2].pos = pos + CVECTOR(m_fFlareSize*(cs + sn), m_fFlareSize*(-sn + cs), 0.0f);
	buffer[2].color = c;
	buffer[2].u = 1.0f;
	buffer[2].v = 0.0f;
	buffer[3].pos = buffer[2].pos;
	buffer[3].color = c;
	buffer[3].u = 1.0f;
	buffer[3].v = 0.0f;		
	buffer[4].pos = buffer[1].pos;
	buffer[4].color = c;
	buffer[4].u = 0.0f;
	buffer[4].v = 1.0f;
	buffer[5].pos = pos + CVECTOR(m_fFlareSize*(cs - sn), m_fFlareSize*(-sn - cs), 0.0f);
	buffer[5].color = c;
	buffer[5].u = 1.0f;
	buffer[5].v = 1.0f;
	m_pRS->TextureSet(0,m_nFlareTexture);
	CMatrix mtx; mtx.SetIdentity();
	m_pRS->SetWorld( mtx );
	m_pRS->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, 2, buffer, sizeof(Vertex), "Coronas");
}

void InterfaceBackScene::InitAniModel( ATTRIBUTES* pAParam )
{
	if( !pAParam ) return;

	const char* pcMdlName = pAParam->GetAttribute("model");
	const char* pcAniName = pAParam->GetAttribute("animation");
	const char* pcAniActionName = pAParam->GetAttribute("aniaction");
	if( !pcMdlName ) {
		api->Trace("Warning! Bad model name parameter for ani model into InterfaceBackScene.");
		return;
	}

	CMatrix mtx;
	if( !FindLocator( pAParam->GetAttribute("locator"), &mtx, 0, 0 ) ) mtx.SetIdentity();

	AniModelDescr* pObj = NEW AniModelDescr;
	Assert(pObj);

	ANIMATION* pAniService = (ANIMATION*)api->CreateService("AnimationServiceImp");
	VGEOMETRY* pGeo = (VGEOMETRY*)api->CreateService("Geometry");
	if( pGeo ) pGeo->SetTexturePath("MainMenu\\");
	// create model
	api->CreateEntity( &pObj->ei, "MODELR" );
	api->Send_Message( pObj->ei, "ls", MSG_MODEL_LOAD_GEO, pcMdlName );
	pObj->pModel = (MODEL*)api->GetEntityPointer( &pObj->ei );
	if( pGeo ) pGeo->SetTexturePath("");

	if( pObj->pModel )
	{
		pObj->pModel->mtx = mtx;
		//pObj->pModel->Update();
		if( pcAniName ) {
			api->Send_Message( pObj->ei, "ls", MSG_MODEL_LOAD_ANI, pcAniName );
			if( pcAniActionName ) {
				pObj->pModel->GetAnimation()->Player(0).SetAction(pcAniActionName);
				pObj->pModel->GetAnimation()->Player(0).Play();
			}
		}
	}

	m_apAniModel.Add(pObj);

	//pObj->pModel->GetAnimation()-
}

void InterfaceBackScene::InitStaticModel( ATTRIBUTES* pAParam )
{
	if( !pAParam ) return;

	const char* pcMdlName = pAParam->GetAttribute("model");
	const char* pcTechniqueName = pAParam->GetAttribute("technique");
	if( !pcMdlName ) {
		api->Trace("Warning! Bad model name parameter for static model into InterfaceBackScene.");
		return;
	}

	CMatrix mtx;
	if( !FindLocator( pAParam->GetAttribute("locator"), &mtx, 0, 0 ) ) mtx.SetIdentity();

	AniModelDescr* pObj = NEW AniModelDescr;
	Assert(pObj);

	VGEOMETRY* pGeo = (VGEOMETRY*)api->CreateService("Geometry");
	if( pGeo ) pGeo->SetTexturePath("MainMenu\\");
	// create model
	api->CreateEntity( &pObj->ei, "MODELR" );
	api->Send_Message( pObj->ei, "ls", MSG_MODEL_LOAD_GEO, pcMdlName );
	pObj->pModel = (MODEL*)api->GetEntityPointer( &pObj->ei );
	if( pGeo ) pGeo->SetTexturePath("");

	if( pAParam->GetAttribute("tfactor") ) {
		pObj->bUseTFactor = true;
		pObj->dwTFactor = pAParam->GetAttributeAsDword("tfactor");
	} else {
		pObj->bUseTFactor = false;
	}

	if( pObj->pModel )
	{
		pObj->pModel->mtx = mtx;
		if( pcTechniqueName )
		{
			pObj->pModel->GetNode(0)->SetTechnique( pcTechniqueName );
		}
	}

	m_apAniModel.Add(pObj);
}

//---------------------------------------------------
//Мухи у фанарей
//---------------------------------------------------
void InterfaceBackScene::AddLampFlys(CVECTOR & pos)
{
	//Занимаем массив
	if(numFlys >= maxFlys)
	{
		maxFlys += 8;
		flys = (LampFlys *)RESIZE(flys, maxFlys*sizeof(LampFlys));
	}
	//Заполняем параметры
	//Общие
	flys[numFlys].pos = pos;
	flys[numFlys].radius = 0.2f;//0.6f;
	flys[numFlys].start = numFly;
	flys[numFlys].num = 4 + (rand() & 4);//1 + (rand() & 7);
	numFly += flys[numFlys].num;
	fly = (ParticleFly *)RESIZE(fly, numFly*sizeof(ParticleFly));
	//Каждой мухи
	for(long i = 0; i < flys[numFlys].num; i++)
	{
		ParticleFly & f = fly[flys[numFlys].start + i];
		f.ax = rand()*PIm2/RAND_MAX;
		f.ay = rand()*PIm2/RAND_MAX;
		f.kx = 0.8f + rand()*(0.4f/RAND_MAX);
		if(rand() & 1) f.kx = -f.kx;
		f.ky = 0.8f + rand()*(0.4f/RAND_MAX);
		if(rand() & 1) f.ky = -f.ky;
		f.a = rand()*PIm2/RAND_MAX;
		f.k = 0.8f + rand()*(0.4f/RAND_MAX);
		if(rand() & 1) f.k = -f.k;
		f.angle = 0.0f;
		f.size = 0.03f;
		f.alpha = 1.0f;
		f.frame = float(rand() & 3);
	}
	numFlys++;
}

void InterfaceBackScene::ProcessedFlys(float dltTime)
{
	CMatrix view;
	m_pRS->GetTransform(D3DTS_VIEW, view);
	view.Transposition();
	CVECTOR cam = view.Pos();
	float dax = dltTime*1.3f;
	float day = dltTime*1.4f;
	float da = dltTime*5.6f;
	//Расчитываем
	for(long i = 0; i < numFlys; i++)
	{
		//Коэфициент видимости
		CVECTOR dir = cam - flys[i].pos;
		float k = ~dir;
		if(k > 400.0f) continue;
		k = sqrtf(k);
		if(k > 0.0f) dir *= 1.0f/k;
		k = k/20.0f;
		k = 3.0f*(1.0f - k);
		if(k > 1.0f) k = 1.0f;		
		//Обновляем мух
		ParticleFly * fl = fly + flys[i].start;
		for(long j = 0; j < flys[i].num; j++)
		{
			ParticleFly & f = fl[j];
			//Углы
			f.ax += dax*f.kx; f.ay += day*f.ky; f.a += da*f.k;
			//Радиус
			float r = 1.0f + 0.5f*sinf(f.a) + 0.2f*cosf(f.a*f.k*2.1f);
			r *= flys[i].radius;
			//Позиция
			f.pos.x = flys[i].pos.x + r*sinf(f.ax)*sinf(f.ay);
			f.pos.y = flys[i].pos.y + r*cosf(f.ax)*cosf(f.ay);
			f.pos.z = flys[i].pos.z + r*sinf(f.ax)*cosf(f.ay);
			//Прозрачность
			f.alpha = k*255.0f;
			//Цвет
			CVECTOR tmp = f.pos - flys[i].pos;
			float dst = sqrtf(~tmp);
			if(dst > 0.0f) tmp *= 1.0f/dst;
			float cs = -(tmp | dir) + 0.4f;
			if(dst > flys[i].radius)
			{
				dst = 3.0f*(dst - flys[i].radius)/flys[i].radius;
				if(dst > 1.0f) dst = 1.0f;
				cs *= 1.0f - dst;
			}
			if(cs < 0.0f) cs = 0.0f;
			if(cs > 1.0f) cs = 1.0f;
			f.color = long(cs*255.0f);
			f.color |= (f.color << 16) | (f.color << 8);
			//Кадр
			f.frame += dltTime*f.k*25.0f;
			if(f.frame >= 4.0f) f.frame -= 4.0f;
			//Угл
			f.angle += dltTime*f.k*3.0f;
		}
	}
	//Рисуем
	DrawParticles(fly, numFly, sizeof(ParticleFly), flyTex, "LocFly", true, 4);
}

void InterfaceBackScene::DrawParticles(void * prts, long num, long size, long texture, const char * tech, bool isEx, long numU)
{
	CMatrix camMtx;
	m_pRS->GetTransform(D3DTS_VIEW, camMtx);
	m_pRS->SetTransform(D3DTS_VIEW,CMatrix());
	m_pRS->SetTransform(D3DTS_WORLD,CMatrix());
	m_pRS->TextureSet(0, texture);
	for(long i = 0, n = 0; i < num; i++)
	{
		Particle * parts = (Particle *)prts;
		prts = (char *)prts + size;
		CVECTOR pos = camMtx*parts->pos;
		float size = parts->size*0.5f;
		float sn = sinf(parts->angle);
		float cs = cosf(parts->angle);
		long color = (long(parts->alpha) << 24);
		if(!isEx) color |= 0x00ffffff; else color |= 0x00ffffff & ((ParticleEx *)parts)->color;
		float u1 = 0.0f;
		float u2 = 1.0f;
		if(isEx && numU)
		{
			u2 = 1.0f/float(numU);
			u1 = long(((ParticleEx *)parts)->frame)*u2;
			u2 += u1;
		}
		buffer[n*6 + 0].pos = pos + CVECTOR(size*(-cs + sn), size*(sn + cs), 0.0f);
		buffer[n*6 + 0].color = color;
		buffer[n*6 + 0].u = u1;
		buffer[n*6 + 0].v = 0.0f;
		buffer[n*6 + 1].pos = pos + CVECTOR(size*(-cs - sn), size*(sn - cs), 0.0f);
		buffer[n*6 + 1].color = color;
		buffer[n*6 + 1].u = u1;
		buffer[n*6 + 1].v = 1.0f;
		buffer[n*6 + 2].pos = pos + CVECTOR(size*(cs + sn), size*(-sn + cs), 0.0f);
		buffer[n*6 + 2].color = color;
		buffer[n*6 + 2].u = u2;
		buffer[n*6 + 2].v = 0.0f;
		buffer[n*6 + 3].pos = buffer[n*6 + 2].pos;
		buffer[n*6 + 3].color = color;
		buffer[n*6 + 3].u = u2;
		buffer[n*6 + 3].v = 0.0f;		
		buffer[n*6 + 4].pos = buffer[n*6 + 1].pos;
		buffer[n*6 + 4].color = color;
		buffer[n*6 + 4].u = u1;
		buffer[n*6 + 4].v = 1.0f;
		buffer[n*6 + 5].pos = pos + CVECTOR(size*(cs - sn), size*(-sn - cs), 0.0f);
		buffer[n*6 + 5].color = color;
		buffer[n*6 + 5].u = u2;
		buffer[n*6 + 5].v = 1.0f;
		n++;
		if(n*2 == 256)
		{
			m_pRS->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, n*2, buffer, sizeof(Vertex), (char *)tech);
			n = 0;
		}
	}
	if(n > 0)
	{
		m_pRS->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, n*2, buffer, sizeof(Vertex), (char *)tech);
	}
	m_pRS->SetTransform(D3DTS_VIEW, camMtx);
}
