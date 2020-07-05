#include <stdio.h>
#include "xinterface.h"
#include "scrshoter\scrshoter.h"
#include "nodes\all_xinode.h"
#include "defines.h"
#include "xservice.h"
#include "stringservice\strservice.h"
#include "stringservice\obj_strservice.h"
#include "TextureSequence\TextureSequence.h"
#include "HelpChooser\HelpChooser.h"
#include "InfoHandler.h"
#include "InfoHandler.h"
#include "BackScene\backscene.h"

#include <direct.h>

#ifndef _XBOX
#include "aviplayer\aviplayer.h"
#include <zmouse.h>
#endif

#define CHECK_FILE_NAME "PiratesReadme.txt"

#define TT_TITLE_OFFSET			4
#define TT_TITLE_HEIGHT			32
#define TT_TITLE_STRING_OFFSET	2
#define TT_PIC_UP_OFFSET		8
#define TT_PIC_DOWN_OFFSET		8
#define TT_TEX1_UP_OFFSET		8
#define TT_TEX1_DOWN_OFFSET		8
#define TT_TEX2RECT_UP_OFFSET	4
#define TT_TEX2RECT_DOWN_OFFSET	4
#define TT_TEX2_UP_OFFSET		8
#define TT_TEX2_DOWN_OFFSET		8
#define TT_TEX3_UP_OFFSET		8
#define TT_TEX3_DOWN_OFFSET		8
#define TT_FRAME_DOWN_OFFSET	8
#define TT_TEX4_UP_OFFSET		8
#define TT_TEX4_DOWN_OFFSET		8
#define TT_TEX4RECT_UP_OFFSET	4
#define TT_TEX4RECT_DOWN_OFFSET	4


INTERFACE_FUNCTION
CREATE_SERVICE(STRSERVICE)
CREATE_SCRIPTLIBRIARY(SCRIPT_INTERFACE_FUNCTIONS)
CREATE_CLASS(OBJ_STRSERVICE)
CREATE_CLASS(TextureSequence)
CREATE_CLASS(XINTERFACE)
CREATE_CLASS(SCRSHOTER)
CREATE_CLASS(HELPCHOOSER)
CREATE_CLASS(InfoHandler)
CREATE_CLASS(CONTROLS_CONTAINER)
CREATE_CLASS(InterfaceBackScene)

#ifndef _XBOX
CREATE_CLASS(CAviPlayer)
#endif

#define DEFAULT_IMODE		0
#define CONTEXTHELP_IMODE	1

#define MIN_SCALE 0.1f
#define MAX_SCALE 3.0f

#define FINDBYTES	"qwerty"
static char CriptedName[256] = {'q','w','e','r','t','y',0,(char)65,(char)21,(char)65};//(char)15,(char)23,(char)208,(char)145,(char)65,(char)19,(char)175};//"qwerty\65\15\23\208\145\65\19\175";
static bool g_bIExclusiveMode = false;
static bool DiskCheck = false;

char * XI_ParseStr(char * inStr, char * buf, size_t bufSize, char devChar=',')
{
	if(bufSize<=0 || buf==null) return inStr;
	if(inStr==null) {buf[0]=0; return null;}
	int curSize=0;
	for(char* curStr=inStr; *curStr!=0; curStr++)
	{
		if(*curStr==' ' && curSize==0) continue;
		if(*curStr==devChar || *curStr==0) break;
		if(curSize<(int)bufSize-1) buf[curSize++] = *curStr;
	}
	buf[curSize]=0;
	if(*curStr == devChar) curStr++;
	return curStr;
}

bool CheckPCcd();

XINTERFACE * XINTERFACE::pThis = 0;

XINTERFACE::XINTERFACE() :
	m_aLocksArray(_FL),
	m_asExitKey(_FL)
{
	pThis = this;

	pQuestService = null;
	pPictureService = null;
	pStringService = null;
	bActive = true;

    pRenderService = null;
	m_pNodes = null;
	m_pCurNode = null;
    m_pContHelp = null;
	m_pMonocracyNode = null;

	oldKeyState.dwKeyCode = -1;
	m_bMouseClick = false;
	m_bDblMouseClick = false;

	m_stringes = null;
	m_nStringQuantity = 0;
	m_imgLists = null;

	m_bShowPrevTexture = false;
	m_pTexture = null;
	m_pPrevTexture = null;

    m_pEvents = NULL;

	m_nColumnQuantity = 15;
	m_fWaveAmplitude = 0.1f;
	m_fWavePhase = 0.4f;
	m_fWaveSpeed = 0.2f;
	m_nBlendStepMax = 0x100;
	m_nBlendSpeed = 4;

	m_bNotFirstPress = false;
	m_nPressDelay = 0;
	m_nMaxPressDelay = 300;

	m_nMouseLastClickTimeCur = 0;
	m_nMouseLastClickTimeMax = 300;

    m_bUse = false;
	m_bShowMouse = false;

	m_fBlindFactor = 0.f;
	m_bBlindUp = true;
	bDisableControl = true;

	m_dwCurDeltaTime = 0;
	m_dwGameTimeSec = 0;
	m_dwGameTimeMin = 0;
	m_dwGameTimeHour = 0;

	m_nInterfaceMode = DEFAULT_IMODE;
	m_idHelpTexture = -1;
	m_dwContHelpColor = ARGB(255,128,128,128);
	m_frectDefHelpTextureUV = FXYRECT(0.0, 0.0, 1.0, 1.0);
	m_strDefHelpTextureFile = null;
	DiskCheck = true;//false;

	m_pMouseWeel = null;

	m_pSaveFindRoot = null;

	m_pEditor = null;

	m_pCurToolTipNode = null;
	m_pMouseNode = null;
}

XINTERFACE::~XINTERFACE()
{
	ReleaseOld();

    if(m_pTexture)	pRenderService->Release(m_pTexture);
    if(m_pPrevTexture)	pRenderService->Release(m_pPrevTexture);
	m_pTexture = m_pPrevTexture = null;

	if(pPictureService!=null)
	{
		pPictureService->ReleaseAll();
		delete pPictureService;
		pPictureService = null;
	}

	PTR_DELETE(pQuestService);
	PTR_DELETE( m_pEditor );

	ReleaseSaveFindList();
}

bool XINTERFACE::Init()
{
	GUARD(XINTERFACE::Init())
	SetDevice();
	UNGUARD
	return true;
}
 
void XINTERFACE::SetDevice()
{
	GUARD(XINTERFACE::SetDevice())

	g_idInterface = GetID();
	m_UtilContainer.Init();

    // получить сервис рендера
	pRenderService = (VDX8RENDER *)api->CreateService("dx8render");
	if(!pRenderService)
	{
		THROW("No service: dx8render");
	}

	pStringService = (VSTRSERVICE *)api->CreateService("STRSERVICE");
	if(!pStringService)
	{
		THROW("No service: strservice");
	}

	// Load common parameters
	LoadIni();

	// Create pictures and string lists service
	pPictureService = NEW XSERVICE;
	if(pPictureService==NULL)
	{
		THROW("Not memory allocate");
	}
	pPictureService->Init(pRenderService,dwScreenWidth,dwScreenHeight);

	pQuestService = NEW QUEST_FILE_READER;
	if(pQuestService==NULL)
	{
		THROW("Not memory allocate");
	}
	VDATA * pvd = api->Event("GetQuestTextFileName","");
	if(pvd!=NULL)
	{
		int nq = pvd->GetElementsNum();
		char * pstr;
		for(int i=0; i<nq; i++)
		{
			pvd->Get(pstr,i);
			pQuestService->SetQuestTextFileName(pstr);
		}
	}

	api->LayerCreate("iExecute",true,false);
	api->LayerSetExecute("iExecute",true);
	api->LayerCreate("iRealize",true,false);
	api->LayerSetRealize("iRealize",true);

	api->SystemMessages(GetID(),true);

	if(AttributesPointer)
	{
		ATTRIBUTES * pA = AttributesPointer->GetAttributeClass("GameTime");
		if(pA)
		{
			m_dwGameTimeSec = pA->GetAttributeAsDword("sec",0);
			m_dwGameTimeMin = pA->GetAttributeAsDword("min",0);
			m_dwGameTimeHour = pA->GetAttributeAsDword("hour",0);
		}
	}

	m_pMouseWeel = api->Event("evGetMouseWeel");

	m_pEditor = NEW GIEditor(this);

	UNGUARD
}

bool XINTERFACE::CreateState(ENTITY_STATE_GEN * state_gen)
{
	return true;
}

bool XINTERFACE::LoadState(ENTITY_STATE * state)
{
	return true;
}

void XINTERFACE::Execute(dword Delta_Time)
{
	m_UtilContainer.FrameUpdate();

	Delta_Time = api->GetRDeltaTime();
    if(!bActive) return;
	IncrementGameTime(Delta_Time);
    if(!m_bUse) return;

	// do blind factor
	if(m_bBlindUp)	m_fBlindFactor += m_fBlindSpeed*Delta_Time;
	else	m_fBlindFactor -= m_fBlindSpeed*Delta_Time;
	if(m_fBlindFactor>1.f)	{m_fBlindFactor = 1.f;	m_bBlindUp = false;}
	if(m_fBlindFactor<0.f)	{m_fBlindFactor = 0.f;	m_bBlindUp = true;}

    while(m_pEvents!=NULL)
    {
        api->Event(m_pEvents->sEventName,"ls",m_pEvents->nCommandIndex,m_pEvents->sNodeName);
        if(m_pEvents!=NULL)
        {
            EVENT_ENTITY * pE = m_pEvents;
            m_pEvents = m_pEvents->next;
            delete pE;
        }
    }
    if(!m_bUse) return;

	if( m_nPressDelay > 0 ) {
		m_nPressDelay -= Delta_Time;
		if( m_nPressDelay < 0 ) m_nPressDelay = 0;
	}

	if( m_nMouseLastClickTimeCur > 0 ) {
		m_nMouseLastClickTimeCur -= Delta_Time;
		if( m_nMouseLastClickTimeCur < 0 ) m_nMouseLastClickTimeCur = 0;
	}

    DoControl();

	CINODE * pNod = m_pNodes;
	while(pNod!=NULL)
	{
		pNod->FrameProcess(Delta_Time);
		pNod = pNod->m_next;
	}

	if(m_pMouseWeel) m_pMouseWeel->Set(0L);
}

void XINTERFACE::Realize(dword Delta_Time)
{
    if(!m_bUse || !bActive) return;

	pRenderService->MakePostProcess();

	Delta_Time = api->GetRDeltaTime();

	CMatrix moldv,moldp,moldw;

	dword dwFogFlag;
	pRenderService->GetRenderState(D3DRS_FOGENABLE,&dwFogFlag);
	if(pRenderService->TechniqueExecuteStart("iStartTechnique")) while(pRenderService->TechniqueExecuteNext());

	// Get old transformation
	pRenderService->GetTransform(D3DTS_VIEW,(D3DXMATRIX*)&moldv);
    pRenderService->GetTransform(D3DTS_PROJECTION,(D3DXMATRIX*)&moldp);
	// Set new transformation
    pRenderService->SetTransform(D3DTS_WORLD,(D3DXMATRIX*)&matw);
    pRenderService->SetTransform(D3DTS_VIEW,(D3DXMATRIX*)&matv);
    pRenderService->SetTransform(D3DTS_PROJECTION,(D3DXMATRIX*)&matp);

	DrawNode(m_pNodes,Delta_Time,0,80);

	// Do mouse move
	CINODE* pOldNode = m_pCurNode;
	MouseMove();
	if( pOldNode != m_pCurNode ) {
		api->Event(ISOUND_EVENT,"l",2); // выбор нового нода
	}

	// show dinamic pictures
	XI_ONLYONETEX_VERTEX pV[4];
	for(int i=0;i<4;i++)	pV[i].pos.z = 1.f;
	IMAGE_ENTITY * pImg = m_imgLists;
	dword oldTFactor;
	pRenderService->GetRenderState(D3DRS_TEXTUREFACTOR,&oldTFactor);
	while(pImg!=NULL)
	{
		if(pImg->idTexture!=-1 && pImg->imageID!=-1)
		{
			pRenderService->TextureSet(0,pImg->idTexture);
			FXYRECT frect;
			pPictureService->GetTexturePos(pImg->imageID,frect);
			pV[0].pos.x = pV[2].pos.x = (float)pImg->position.left;
			pV[0].tu = pV[2].tu = frect.left;
			pV[1].pos.x = pV[3].pos.x = (float)pImg->position.right;
			pV[1].tu = pV[3].tu = frect.right;
			pV[0].pos.y = pV[1].pos.y = (float)pImg->position.top;
			pV[0].tv = pV[1].tv = frect.top;
			pV[2].pos.y = pV[3].pos.y = (float)pImg->position.bottom;
			pV[2].tv = pV[3].tv = frect.bottom;
			if(pImg->doBlind)
			{
				pRenderService->SetRenderState(D3DRS_TEXTUREFACTOR,GetBlendColor(pImg->argbBlindMin,pImg->argbBlindMax,m_fBlindFactor));
				pRenderService->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONLYONETEX_FVF,2,pV,sizeof(XI_ONLYONETEX_VERTEX),"iBlindPictures");
			}
			else
			{
				if(pImg->sTechniqueName==NULL)
					pRenderService->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONLYONETEX_FVF,2,pV,sizeof(XI_ONLYONETEX_VERTEX),"iDinamicPictures");
				else
					pRenderService->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONLYONETEX_FVF,2,pV,sizeof(XI_ONLYONETEX_VERTEX),pImg->sTechniqueName);
			}
		}
		pImg = pImg->next;
	}
	pRenderService->SetRenderState(D3DRS_TEXTUREFACTOR,oldTFactor);

	DrawNode(m_pNodes,Delta_Time,81,90);

	// Show dinamic stringes
	if(m_nStringQuantity>0)
	{
		ATTRIBUTES * tmpAttr = api->Entity_GetAttributeClass(&g_idInterface,"strings");

		if(tmpAttr!=null) for(int i=0; i<m_nStringQuantity; i++)
			if(m_stringes[i].bUsed)
			{
				char * tmps = tmpAttr->GetAttribute(m_stringes[i].sStringName);
				pRenderService->ExtPrint(m_stringes[i].fontNum,m_stringes[i].dwColor,0, m_stringes[i].eAlignment,
					true, m_stringes[i].fScale, dwScreenWidth,dwScreenHeight, m_stringes[i].x,
					m_stringes[i].y, "%s",tmpAttr->GetAttribute(m_stringes[i].sStringName) );
			}
	}

	DrawNode(m_pNodes,Delta_Time,91,65536);

	if( m_pCurToolTipNode )
		m_pCurToolTipNode->ShowToolTip();

	if( m_pEditor )
		if( m_pEditor->IsShowMode() ) m_pEditor->Render();
		else m_pEditor->DrawSizeBox();

	// Mouse pointer show
	if(m_bShowMouse)
	{
		pRenderService->TextureSet(0,m_idTex);
		pRenderService->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONLYONETEX_FVF,2,vMouse,sizeof(XI_ONLYONETEX_VERTEX),"iMouseCurShow");
	}

	// Show context help data
	ShowContextHelp();

	if(pRenderService->TechniqueExecuteStart("iExitTechnique")) while(pRenderService->TechniqueExecuteNext());
	pRenderService->SetRenderState(D3DRS_FOGENABLE,dwFogFlag);

	// Restore old transformation
    pRenderService->SetTransform(D3DTS_VIEW,(D3DXMATRIX*)&moldv);
    pRenderService->SetTransform(D3DTS_PROJECTION,(D3DXMATRIX*)&moldp);
}

long oldCurNum = -1L;
dword XINTERFACE::ProcessMessage(MESSAGE & message)
{
	long cod = message.Long();

	switch (cod)
    {
	case MSG_INTERFACE_MSG_TO_NODE:
		{
            char param[256];
            message.String(sizeof(param)-1,param);
			if(m_pNodes!=null)
			{
				CINODE * pNode = m_pNodes->FindNode(param);
				if(pNode!=null)
				{
					long msgCode = message.Long();
					if(msgCode==-1)
					{
						msgCode = message.Long();
						return pNode->CINODE::MessageProc(msgCode,message);
					}
					else	return pNode->MessageProc(msgCode,message);
				}
			}
		}
		break;
    case MSG_INTERFACE_SETHELPSTRING:
        {
            char param[256];
			message.String(sizeof(param)-1,param);
            if(m_pContHelp!=NULL)
                ((CXI_CONTEXTHELP*)m_pContHelp)->SetTempHelp(param);
        }
        break;
	case MSG_INTERFACE_SET_CURRENT_NODE:
		{
			CINODE * pNewCurNode = (CINODE*)message.Pointer();
			if(pNewCurNode!=NULL)
                SetCurNode(pNewCurNode);
		}
		break;
	case MSG_INTERFACE_NEW_CURRENT_NODE:
		{
            char param[256];
            message.String(sizeof(param)-1,param);
			CINODE * pNewCurNode = (m_pNodes!=NULL ? m_pNodes->FindNode(param) : NULL);
			if(pNewCurNode!=NULL)
				SetCurNode(pNewCurNode);
		}
		break;

	case MSG_INTERFACE_SET_NODE_USING:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			int nUsingCode = message.Long();
			CINODE * pTmpNod = (m_pNodes!=NULL ? m_pNodes->FindNode(param) : NULL);
			if(pTmpNod!=NULL)
				pTmpNod->SetUsing(nUsingCode!=0);
		}
		break;

    case MSG_INTERFACE_SCROLL_CHANGE:
        {
            char sNodeName[256];
            message.String(sizeof(sNodeName)-1,sNodeName);
            long nItemNum = message.Long();

			CINODE * pScrollNode = (m_pNodes!=NULL ? m_pNodes->FindNode(sNodeName) : NULL);
            if(pScrollNode!=NULL)
            {
				if( pScrollNode->m_nNodeType == NODETYPE_SCROLLIMAGE )
					((CXI_SCROLLIMAGE*)pScrollNode)->ChangeScroll(nItemNum);
				if( pScrollNode->m_nNodeType == NODETYPE_VIMGSCROLL )
					((CXI_VIMAGESCROLL*)pScrollNode)->ChangeScroll(nItemNum);
            }
        }
        break;

    case MSG_INTERFACE_FOURIMAGE_CHANGE:
        {
            char sNodeName[256];
            message.String(sizeof(sNodeName)-1,sNodeName);
            long nItemNum = message.Long();

            CINODE * pNode = (m_pNodes!=NULL ? m_pNodes->FindNode(sNodeName) : NULL);
            if( pNode!=NULL && pNode->m_nNodeType==NODETYPE_FOURIMAGE )
            {
                ((CXI_FOURIMAGE*)pNode)->ChangeItem(nItemNum);
            }
        }
        break;

    case MSG_INTERFACE_INIT:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			ReleaseOld();
			LoadDialog(param);
            m_bUse = true;
			bDisableControl = true;
		}
        break;

	case MSG_INTERFACE_ENABLE_STRING:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			for(int i=0; i<m_nStringQuantity; i++)
				if( m_stringes[i].sStringName!=NULL && stricmp(param,m_stringes[i].sStringName)==0 )
				{
					m_stringes[i].bUsed = true;
					break;
				}
		}
		break;

	case MSG_INTERFACE_DISABLE_STRING:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			for(int i=0; i<m_nStringQuantity; i++)
				if( m_stringes[i].sStringName!=NULL && stricmp(param,m_stringes[i].sStringName)==0 )
				{
					m_stringes[i].bUsed = false;
					break;
				}
		}
		break;

	case MSG_INTERFACE_RELEASE:
		{
            ReleaseOld();
            m_bUse = false;
			bDisableControl = true;
		}
		break;

    case MSG_INTERFACE_SET_EVENT:
        {
            char param[256];
            message.String(sizeof(param)-1,param);
			char nodeName[256];
			message.String(sizeof(nodeName)-1,nodeName);
			long nCommand = message.Long();
			//
            EVENT_ENTITY * pEvent = NEW EVENT_ENTITY;
            if(pEvent==NULL)	THROW("allocate memory error");
            pEvent->next = m_pEvents;
            m_pEvents = pEvent;
			//
            pEvent->sEventName = NEW char[strlen(param)+1];
            if(pEvent->sEventName==NULL)	THROW("allocate memory error");
            strcpy(pEvent->sEventName,param);
			//
			pEvent->sNodeName = NEW char[strlen(nodeName)+1];
			if(pEvent->sNodeName==NULL)	THROW("allocate memory error");
			strcpy(pEvent->sNodeName,nodeName);
			//
			if( nCommand>=0 && nCommand<COMMAND_QUANTITY )
				pEvent->nCommandIndex = pCommandsList[nCommand].code;
			else pEvent->nCommandIndex = -1;
        }
        break;

	case MSG_INTERFACE_CREATE_STRING:
		{
			char param[256];
			message.String(sizeof(param)-1,param);

			for(int l=0; l<m_nStringQuantity; l++)
			{
				if(m_stringes[l].sStringName!=null && stricmp(m_stringes[l].sStringName,param)==0) break;
			}
			if(l==m_nStringQuantity)
			{
				m_nStringQuantity++;
				m_stringes = (STRING_ENTITY*)RESIZE(m_stringes,m_nStringQuantity*sizeof(STRING_ENTITY));
				if(m_stringes==NULL)	THROW("allocate memory error");

				m_stringes[l].sStringName = NEW char[strlen(param)+1];
				if(m_stringes[l].sStringName == NULL)	THROW("allocate memory error");
				strcpy(m_stringes[l].sStringName,param);
			}
			else
			{
				FONT_RELEASE(pRenderService,m_stringes[l].fontNum);
			}

			message.String(sizeof(param)-1,param);
			m_stringes[l].fontNum = pRenderService->LoadFont(param);
			m_stringes[l].dwColor = (DWORD)message.Long();

			m_stringes[l].bUsed = true;
			m_stringes[l].x = message.Long() + GlobalRect.left;
			m_stringes[l].y = message.Long() + GlobalRect.top;
			switch(message.Long())
			{
			case SCRIPT_ALIGN_RIGHT:
				m_stringes[l].eAlignment = ALIGN_RIGHT;
				break;
			case SCRIPT_ALIGN_CENTER:
				m_stringes[l].eAlignment = ALIGN_CENTER;
				break;
			case SCRIPT_ALIGN_LEFT:
				m_stringes[l].eAlignment = ALIGN_LEFT;
				break;
			}
            m_stringes[l].fScale = message.Float();
		}
		break;
	case MSG_INTERFACE_DELETE_STRING:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			for(int i=0; i<m_nStringQuantity; i++)
			{
				if( m_stringes[i].sStringName!=null && stricmp(m_stringes[i].sStringName,param)==0 )
				{
					PTR_DELETE(m_stringes[i].sStringName);
					FONT_RELEASE(pRenderService,m_stringes[i].fontNum);
					m_nStringQuantity--;
					m_stringes[i].sStringName = m_stringes[m_nStringQuantity].sStringName;
					m_stringes[i].fontNum = m_stringes[m_nStringQuantity].fontNum;
					break;
				}
			}
		}
		break;
	case MSG_INTERFACE_CHANGE_STR_COLOR:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			for(int i=0; i<m_nStringQuantity; i++)
			{
				if( m_stringes[i].sStringName!=null && stricmp(m_stringes[i].sStringName,param)==0 )
				{
					m_stringes[i].dwColor = message.Long();
					break;
				}
			}
		}
		break;
	case MSG_INTERFACE_PLACE_IMAGE:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			// find image
			IMAGE_ENTITY * pImg = m_imgLists;
			while(pImg!=NULL)
			{
				if( pImg->sImageName!=NULL && stricmp(pImg->sImageName,param)==0 ) break;
				pImg = pImg->next;
			}
			bool bGlobRect = message.Long()!=0;
			// get image position
			if(pImg!=NULL)
			{
				pImg->position.left = message.Long();
				pImg->position.top = message.Long();
				pImg->position.right = message.Long();
				pImg->position.bottom = message.Long();

				if(bGlobRect)
				{
					pImg->position.left += GlobalRect.left;
					pImg->position.top += GlobalRect.top;
					pImg->position.right += GlobalRect.left;
					pImg->position.bottom += GlobalRect.top;
				}
			}
		}
		break;
	case MSG_INTERFACE_SET_SELECTABLE: // "lsl"
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			bool bSelectable = message.Long()!=0;
			if(m_pNodes==NULL) break;
			CINODE * pNod = m_pNodes->FindNode(param);
			if(pNod!=NULL)	pNod->m_bSelected = bSelectable;
		}
		break;
	case MSG_INTERFACE_GET_SELECTABLE: // ls
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			if(m_pNodes==NULL) break;
			CINODE * pNod = m_pNodes->FindNode(param);
			if(pNod!=NULL) return (pNod->m_bSelected && pNod->m_bUse);
		}
		break;
	case MSG_INTERFACE_DEL_SCROLLIMAGE:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			long imgNum = message.Long();
			if(m_pNodes==NULL) break;
			CINODE * pNod = m_pNodes->FindNode(param);
			if(pNod!=NULL)
			{
				if( pNod->m_nNodeType == NODETYPE_SCROLLIMAGE )
					((CXI_SCROLLIMAGE*)pNod)->DeleteImage(imgNum);
				if( pNod->m_nNodeType == NODETYPE_VIMGSCROLL )
					((CXI_VIMAGESCROLL*)pNod)->DeleteImage(imgNum);
			}
		}
		break;
	case MSG_INTERFACE_GET_CURRENT_NODE: // "le"
		{
			if(m_pCurNode!=NULL && m_pCurNode->m_nodeName!=NULL)
				message.ScriptVariablePointer()->Set(m_pCurNode->m_nodeName);
			else
				message.ScriptVariablePointer()->Set("");
		}
		break;
	case MSG_INTERFACE_REFRESH_SCROLL: // "ls"
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			if(m_pNodes==NULL) break;
			CINODE * pNod = m_pNodes->FindNode(param);
			if(pNod!=NULL)
			{
				if( pNod->m_nNodeType == NODETYPE_SCROLLIMAGE )
					((CXI_SCROLLIMAGE*)pNod)->RefreshScroll();
				if( pNod->m_nNodeType == NODETYPE_VIMGSCROLL )
					((CXI_VIMAGESCROLL*)pNod)->RefreshScroll();
			}
		}
		break;
	case MSG_INTERFACE_GET_LANGUAGE_STRING:
		{
			char * lngStr;
			message.ScriptVariablePointer()->Get(lngStr);
			message.ScriptVariablePointer()->Set(pStringService->GetString(lngStr));
		}
		break;
	case MSG_INTERFACE_SET_TITLE_STRINGS:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			ATTRIBUTES * pA = message.AttributePointer();
			int tn = message.Long();
			if(m_pNodes==NULL) break;
			CINODE * pNod = m_pNodes->FindNode(param);
			if( pNod!=NULL && pNod->m_nNodeType==NODETYPE_QTITLE )
				((CXI_QUESTTITLE*)pNod)->SetNewTopQuest(pA,tn);
		}
		break;
	case MSG_INTERFACE_CHECK_QRECORD:
		{
			char param[256];
			ATTRIBUTES * pA = message.AttributePointer();
			if(pA==NULL) break;
			message.String(sizeof(param)-1,param);
			char* pText = pA->GetAttribute("Text");
			if(pText==NULL) break;
			char subText[256];
			char* pCur=pText;
			while(true)
			{
				subText[0]=0;
				sscanf(pCur,"%[^,]",subText);
				int subSize = strlen(subText);
				if(stricmp(subText,param)==0) return 1;
				pCur+=subSize;
				if(*pCur==',') pCur++;
				if(subSize==0 || *pCur==0) break;
			}
		}
		break;
	case MSG_INTERFACE_INIT_QTEXT_SHOW:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			ATTRIBUTES *pA = message.AttributePointer();
			int qn = message.Long();
			if(m_pNodes==NULL) break;
			CINODE * pNod = m_pNodes->FindNode(param);
			if( pNod!=NULL && pNod->m_nNodeType==NODETYPE_QTEXTS )
				((CXI_QUESTTEXTS*)pNod)->StartQuestShow(pA,qn);
		}
		break;
	case MSG_INTERFACE_SET_SCROLLER:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			float spos = message.Float();
			if(m_pNodes==NULL) break;
			CINODE * pNod = m_pNodes->FindNode(param);
			if( pNod!=NULL && pNod->m_nNodeType==NODETYPE_SCROLLER )
				((CXI_SCROLLER*)pNod)->SetRollerPos(spos);
		}
		break;

	case MSG_INTERFACE_DO_SAVE_DATA:
		{
			char param1[256];
			message.String(sizeof(param1)-1,param1);
			char param2[256];
			message.String(sizeof(param2)-1,param2);
			SFLB_DoSaveFileData(param1,param2);
		}
		break;
	case MSG_INTERFACE_GET_SAVE_DATA:
		{
			char param1[256];
			message.String(sizeof(param1)-1,param1);
			char param2[256]; param2[0]=0;
			if( SFLB_GetSaveFileData(param1,sizeof(param2),param2) && strlen(param2)>0 )
			{
				message.ScriptVariablePointer()->Set(param2);
			}
			else
			{
				message.ScriptVariablePointer()->Set("");
			}
		}
		break;
	case MSG_INTERFACE_SAVE_FILE_FIND:
		{
			char param[256];
			long saveNum = message.Long();
			long nSaveFileSize;
			char * sSaveName = SaveFileFind(saveNum,param,sizeof(param),nSaveFileSize);
			if(sSaveName!=NULL)
			{
				message.ScriptVariablePointer()->Set(sSaveName);
				message.ScriptVariablePointer()->Set(nSaveFileSize);
				return 1;
			}
		}
		break;
	case MSG_INTERFACE_NEW_SAVE_FILE_NAME:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			if( NewSaveFileName(param) ) return 0;
			return 1;
		}
		break;
	case MSG_INTERFACE_DELETE_SAVE_FILE:
		{
			char param[256];
			message.String(sizeof(param),param);
			DeleteSaveFile(param);
		}
		break;
	case MSG_INTERFACE_SET_FORMATEDTEXT:
		{
			char param[4096];
			message.String(sizeof(param),param);
			CINODE * pNod = m_pNodes ? m_pNodes->FindNode(param) : 0;
			message.String(sizeof(param),param);
			if( pNod && pNod->m_nNodeType==NODETYPE_FORMATEDTEXTS && !pNod->m_bInProcessingMessageForThisNode )
			{
				pNod->m_bInProcessingMessageForThisNode = true;
				((CXI_FORMATEDTEXT*)pNod)->SetFormatedText(param);
				pNod->m_bInProcessingMessageForThisNode = false;
			}
		}
		break;
	case MSG_INTERFACE_SET_BLIND:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			// find image
			IMAGE_ENTITY * pImg = m_imgLists;
			while(pImg!=NULL)
			{
				if( pImg->sImageName!=NULL && stricmp(pImg->sImageName,param)==0 ) break;
				pImg = pImg->next;
			}
			// get image position
			if(pImg!=NULL)
			{
				pImg->doBlind = message.Long()!=0;
				pImg->argbBlindMin = message.Long();
				pImg->argbBlindMax = message.Long();
			}
		}
		break;
	case MSG_INTERFACE_GET_STRWIDTH:
		{
			char param[256]; param[0]=0;
			char param2[256];
			message.String(sizeof(param)-1,param);
			message.String(sizeof(param2)-1,param2);
			float fScale = message.Float();
			int tmpFontID = pRenderService->LoadFont(param2);
			int retVal = pRenderService->StringWidth( param, tmpFontID, fScale );
			pRenderService->UnloadFont(tmpFontID);
			return retVal;
		}
		break;
	case MSG_INTERFACE_CLICK_STATUS:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			if(m_pNodes==NULL) break;
			CINODE * pNod = m_pNodes->FindNode(param);
			if(pNod!=NULL) return pNod->GetClickState();
		}
		break;
	case MSG_INTERFACE_LOCK_NODE:
		{
			if(!m_bUse) break;
			long lockNode = message.Long();
			CINODE * pnod = null;
			if(lockNode>0)
			{
				char param[256];
				message.String(sizeof(param)-1,param);
				pnod = m_pNodes->FindNode(param);
			}
			switch(lockNode)
			{
			case 0:	ExitFromExclusive();	break;
			case 1: SetExclusiveNode(pnod);	break;
			case 2: AddExclusiveNode(pnod);	break;
			}
		}
		break;
	case MSG_INTERFACE_DELETE_PICTURE:
		{
			if(!m_bUse) break;
			char param[256];
			message.String(sizeof(param)-1,param);
			ReleaseDinamicPic(param);
		}
		break;
	case MSG_INTERFACE_GET_FREE_SPACE:
		{
			long retVal = 0;
#ifdef _XBOX
			ULARGE_INTEGER ulFreeBytes, ulTotalBytes, ulTotalFree;
			GetDiskFreeSpaceEx("U:\\", &ulFreeBytes, &ulTotalBytes, &ulTotalFree);
			DWORD blocks = (DWORD)(ulFreeBytes.QuadPart>>14);
			return blocks;
			//return XGetDisplayBlocks("U:\\");
#endif
		}
		break;

	case MSG_INTERFACE_SAVEOPTIONS:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			ATTRIBUTES * pA = message.AttributePointer();
			SaveOptionsFile(param,pA);
		}
		break;

	case MSG_INTERFACE_LOADOPTIONS:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			ATTRIBUTES * pA = message.AttributePointer();
			LoadOptionsFile(param,pA);
		}
		break;

	case MSG_INTERFACE_LAUNCH_DASHBOARD:
		{
#ifdef _XBOX
		LD_LAUNCH_DASHBOARD LaunchDash;
		LaunchDash.dwReason = XLD_LAUNCH_DASHBOARD_MEMORY;
		LaunchDash.dwContext = 0;
		LaunchDash.dwParameter1 = DWORD( 'U' );
		LaunchDash.dwParameter2 = 200;
		XLaunchNewImage( NULL, (PLAUNCH_DATA)(&LaunchDash) );
#endif
		}
		break;

	case MSG_INTERFACE_FILENAME2DATASTR:
		{
			char param[1024];
			message.String(sizeof(param)-1,param);
			VDATA *pvdat = message.ScriptVariablePointer();
			int i=0;
			int nStrLen = strlen(param);
#ifdef _XBOX
			if( nStrLen>2 && param[1]==':' )
			{
				for(i=3; i<nStrLen; i++)
				{
					if(param[i]=='\\') break;
				}
				i++;
			}
			if(i>=nStrLen) i=0;
#endif
			if(pvdat) pvdat->Set(&param[i]);
		}
		break;

	case MSG_INTERFACE_GETTIME:
		{
			char param[1024];
			message.String(sizeof(param)-1,param);
			char param2[1024];
			SYSTEMTIME systTime;
			if(param[0]==0)	GetLocalTime(&systTime);
			else {
				WIN32_FIND_DATA wfd;
#ifdef _XBOX
				api->fio->SetDrive(XBOXDRIVE_NONE);
#endif
				HANDLE h = api->fio->_FindFirstFile( param,&wfd );
				if(h!=INVALID_HANDLE_VALUE)
				{
					FILETIME ftime;
					FileTimeToLocalFileTime(&wfd.ftLastWriteTime, &ftime);
					FileTimeToSystemTime(&ftime, &systTime);
					api->fio->_FindClose(h);
				}
				else GetLocalTime(&systTime);
#ifdef _XBOX
				api->fio->SetDrive();
#endif
			}
			sprintf(param2,"%2.2d:%2.2d:%2.2d", systTime.wHour,systTime.wMinute,systTime.wSecond);
			VDATA *pvdat = message.ScriptVariablePointer();
			if(pvdat) pvdat->Set(param2);

			sprintf(param2,"%2.2d.%2.2d.%d", systTime.wDay,systTime.wMonth,systTime.wYear);
			pvdat = message.ScriptVariablePointer();
			if(pvdat) pvdat->Set(param2);
		}
		break;

	case MSG_INTERFACE_CHECK_AGAIN:
		return CheckPCcd();
		break;

	case MSG_INTERFACE_IS_READY_LOAD:
		return LoadIsExist();
		break;

	case MSG_INTERFACE_SET_TOOLTIP:
		{
			const char* pcHeader = message.StringPointer();
			const char* pcText1 = message.StringPointer();
			long dwTextColor1 = message.Long();
			const char* pcText2 = message.StringPointer();
			long dwTextColor2 = message.Long();
			const char* pcText3 = message.StringPointer();
			long dwTextColor3 = message.Long();
			const char* pcText4 = message.StringPointer();
			long dwTextColor4 = message.Long();
			const char* pcPicTextureName = message.StringPointer();
			const char* pcPicGroupName = message.StringPointer();
			const char* pcPicImageName = message.StringPointer();
			long nPicWidth = message.Long();
			long nPicHeight = message.Long();
			SetTooltip( pcHeader, pcText1,dwTextColor1, pcText2,dwTextColor2, pcText3,dwTextColor3, pcText4,dwTextColor4, pcPicTextureName,pcPicGroupName,pcPicImageName,nPicWidth,nPicHeight );
		}
		break;
    }

    return 0;
}

static const char *RESOURCE_FILENAME = "resource\\ini\\interfaces\\interfaces.ini";
void XINTERFACE::LoadIni()
{
    GUARD(XINTERFACE::LoadIni());
	char	section[256];

#ifndef _XBOX
	char * platform = "PC_SCREEN";
#else
	char * platform = "XBOX_SCREEN";
#endif
	INIFILE * ini;
	ini = api->fio->OpenIniFile((char*)RESOURCE_FILENAME);
	if(!ini) THROW("ini file not found!");

	sprintf(section,"COMMON");

	// установить параметры экрана
	fScale = ini->GetFloat(platform,"fScale",1.f);
	if(fScale<MIN_SCALE || fScale>MAX_SCALE) fScale=1.f;
	dwScreenWidth = ini->GetLong(platform,"wScreenWidth",800);
	dwScreenHeight = ini->GetLong(platform,"wScreenHeight",600);
	GlobalScreenRect.left   = ini->GetLong(platform,"wScreenLeft",0);
	GlobalScreenRect.top    = ini->GetLong(platform,"wScreenTop",600);
	GlobalScreenRect.right  = ini->GetLong(platform,"wScreenRight",800);
	GlobalScreenRect.bottom = ini->GetLong(platform,"wScreenDown",0);

	m_fpMouseOutZoneOffset.x = ini->GetFloat(section,"mouseOutZoneWidth",0.f);
	m_fpMouseOutZoneOffset.y = ini->GetFloat(section,"mouseOutZoneHeight",0.f);
	m_nMouseLastClickTimeMax = ini->GetLong(section,"mouseDblClickInterval",300);

	CMatrix oldmatp;
	pRenderService->GetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&oldmatp);
	GlobalRect.left   = GlobalScreenRect.left;
	GlobalRect.top    = GlobalScreenRect.top;
	GlobalRect.right  = GlobalScreenRect.right;
	GlobalRect.bottom = GlobalScreenRect.bottom;

	matp.m[0][0] = 2.f/dwScreenWidth*fScale;
	matp.m[1][1] = -2.f/dwScreenHeight*fScale;
	matp.m[2][2] = 1.0003f;
	matp.m[2][3] = 1.f; matp.m[3][2]=0.f;
	matp.m[3][3] = 0.f;
	matp.m[3][2] = -1.f;

	matv.m[3][0] = -(GlobalScreenRect.left+GlobalScreenRect.right)/2.f;
	matv.m[3][1] = -(GlobalScreenRect.top+GlobalScreenRect.bottom)/2.f;

	// set key press data
	m_nMaxPressDelay = ini->GetLong(section,"RepeatDelay",500);

	// set mouse cursor
#ifndef _XBOX
	char	param[256];
	ini->ReadString(section,"MousePointer",param,sizeof(param)-1,"");
	char param2[sizeof(param)];
	sscanf(param,"%[^,],%d,size:(%d,%d),pos:(%d,%d)",param2,&m_lMouseSensitive,&MouseSize.x,&MouseSize.y,&m_lXMouse,&m_lYMouse);
	m_idTex = pRenderService->TextureCreate(param2);
	RECT Screen_Rect;
	GetWindowRect(api->GetAppHWND(),&Screen_Rect);
	lock_x = Screen_Rect.left + (Screen_Rect.right - Screen_Rect.left)/2;
	lock_y = Screen_Rect.top + (Screen_Rect.bottom - Screen_Rect.top)/2;
	SetCursorPos(lock_x,lock_y);
	fXMousePos = float(dwScreenWidth/2);	fYMousePos = float(dwScreenHeight/2);
	for(int i=0; i<4; i++)
		vMouse[i].pos.z = 1.f;
	vMouse[0].tu = vMouse[1].tu = 0.f;
	vMouse[2].tu = vMouse[3].tu = 1.f;
	vMouse[0].tv = vMouse[2].tv = 0.f;
	vMouse[1].tv = vMouse[3].tv = 1.f;
	ShowCursor(false);
#endif

	// set blind parameters
	m_fBlindSpeed = ini->GetFloat(section,"BlindTime",1.f);
	if(m_fBlindSpeed<=0.0001f) m_fBlindSpeed = 1.f;
	m_fBlindSpeed = 0.002f/m_fBlindSpeed;

	// set wave parameters
	m_nColumnQuantity	= ini->GetLong(section,"columnQuantity",m_nColumnQuantity);
	m_fWaveAmplitude	= ini->GetFloat(section,"waveAmplitude",m_fWaveAmplitude);
	m_fWavePhase		= ini->GetFloat(section,"wavePhase",m_fWavePhase);
	m_fWaveSpeed		= ini->GetFloat(section,"waveSpeed",m_fWaveSpeed);
	m_nBlendStepMax		= ini->GetLong(section,"waveStepQuantity",m_nBlendStepMax);
	m_nBlendSpeed		= ini->GetLong(section,"blendSpeed",m_nBlendSpeed);


	oldKeyState.dwKeyCode=-1;
	DoControl();

    delete ini;
    UNGUARD
}

void XINTERFACE::LoadDialog(char *sFileName)
{
    GUARD(void XINTERFACE::LoadDialog(char *sFileName));
	char	section[255];
	char	skey[255];
	char	param[255];
	INIFILE * ini, *ownerIni;
	int i;

	if( m_pEditor ) m_pEditor->ReCreate();

	// initialize ini file
	m_sDialogFileName = sFileName;
	ini = api->fio->OpenIniFile(sFileName);
	if(!ini)
	{
		api->Trace( "ini file %s not found!", sFileName );
		api->PostEvent( "exitCancel", 1, null );
		return;
	}
	ownerIni = api->fio->OpenIniFile("RESOURCE\\INI\\INTERFACES\\defaultnode.ini");

	sprintf(section,"MAIN");

	// load items
	CINODE *curnod = m_pNodes;
	int keyNum=1;
	char nodeName[sizeof(param)];

	char crData[sizeof(CriptedName)];
	SetOtherData(crData);

	char * findName = "item";
	if( ini->ReadString(section,findName,skey,sizeof(skey)-1,"") )
	while(true)
	{
		int priority;
		char * tmpStr = skey;
		tmpStr = XI_ParseStr(tmpStr,param,sizeof(param));
		if( (priority=atoi(param)) > 0 ) tmpStr = XI_ParseStr(tmpStr,param,sizeof(param));
		else	priority = 80;
		if(!stricmp(param,"PC") || !stricmp(param,"XBOX") || !stricmp(param,"LANG"))
		{
			bool bThisXBOX = false;
			#ifdef _XBOX
				bThisXBOX = true;
			#endif
				if(!stricmp(param,"PC"))
				{
					if(bThisXBOX) param[0] = 0;
					else	tmpStr = XI_ParseStr(tmpStr,param,sizeof(param));
				}
				else if(!stricmp(param,"XBOX"))
				{
					if(!bThisXBOX) param[0] = 0;
					else	tmpStr = XI_ParseStr(tmpStr,param,sizeof(param));
				}
				else if(!stricmp(param,"LANG"))
				{
					tmpStr = XI_ParseStr(tmpStr,param,sizeof(param));
					char * strLangName = pStringService->GetLanguage();
					if(strLangName==null || stricmp(param,strLangName)) param[0] = 0;
					else	tmpStr = XI_ParseStr(tmpStr,param,sizeof(param));
				}
		}
		tmpStr = XI_ParseStr(tmpStr,nodeName,sizeof(nodeName));
		if(param[0])
			SFLB_CreateNode( ownerIni, ini, param, nodeName, priority );

		i=0;
		if( findName && stricmp(findName,"item")==0 )
		{
			ini->ReadString(section,findName,skey,sizeof(skey)-1);
			for(; i<keyNum; i++)
				if( !ini->ReadStringNext(section,findName,skey,sizeof(skey)-1) ) break;
		}

		if(i<keyNum)
		{	// not more items
			if( findName && stricmp(findName,"item")==0 ) {
				findName = "glow";
				if(m_pGlowCursorNode==null)
				{
					if( !ini->ReadString(section,findName,skey,sizeof(skey)-1,"GLOWCURSOR,GLOWCURSOR") )
					{
						if(ownerIni && !ownerIni->ReadString( "LIST","glow_cursor", skey, sizeof(skey)-1, "GLOWCURSOR,GLOWCURSOR") )
							printf(skey,"GLOWCURSOR,GLOWCURSOR");
					}
					continue;
				}
			}
			break;
		}
		keyNum++;
	}

    // set help data
    if( m_pContHelp!=NULL )
    {
        HELPENTITY* pHlist = ((CXI_CONTEXTHELP*)m_pContHelp)->m_pHelpList;
        long nListSize = ((CXI_CONTEXTHELP*)m_pContHelp)->m_helpQuantity;
        for(int n=0; n<nListSize; n++)
            pHlist[n].pNode = m_pNodes->FindNode(pHlist[n].nodeName);
    }

	// set active item
	if(m_pNodes==NULL)
		m_pCurNode = NULL;
	else
	{
		if( !ini->ReadString(section,"start",param,sizeof(param)-1,"") )
			m_pCurNode = NULL;
		else
			if( (m_pCurNode = m_pNodes->FindNode(param))==NULL )
				m_pCurNode = GetActivingNode(m_pNodes);

		if( m_pEditor )
		{
			if( !ini->ReadString( section, "editnode", param, sizeof(param)-1, "" ) ) m_pEditor->SetEditNode( null );
			m_pEditor->SetEditNode( m_pNodes->FindNode( param ) );
		}
	}
	if( ini->ReadString(section,"DefaultHelp",param,sizeof(param)-1,"") )
	{
		m_strDefHelpTextureFile = NEW char[strlen(param)+1];
		if(m_strDefHelpTextureFile)	strcpy(m_strDefHelpTextureFile,param);
	}
	m_frectDefHelpTextureUV = FXYRECT(0.f,0.f, 1.f,1.f);
	if( ini->ReadString(section,"DefaultHelpUV",param,sizeof(param)-1,"") )
	{
		CINODE::GetDataStr(param, "ffff",
			&m_frectDefHelpTextureUV.left, &m_frectDefHelpTextureUV.top,
			&m_frectDefHelpTextureUV.right, &m_frectDefHelpTextureUV.bottom);
	}

	// set cripted data
	api->Event("CriptData","s",crData);

	if(ini)	delete ini;
	if(ownerIni)	delete ownerIni;
	UNGUARD
}

void XINTERFACE::CreateNode(char *sFileName, char *sNodeType, char *sNodeName, long priority)
{
	INIFILE * ini, *ownerIni;

	// уже есть такой нод
	if( m_pNodes && m_pNodes->FindNode( sNodeName ) ) return;

	ini = 0;
	if( sFileName && sFileName[0] )
	{
		ini = api->fio->OpenIniFile( sFileName );
		if( !ini )
		{
			api->Trace( "ini file %s not found!", sFileName );
			return;
		}
	}
	ownerIni = api->fio->OpenIniFile("RESOURCE\\INI\\INTERFACES\\defaultnode.ini");

	SFLB_CreateNode( ownerIni, ini, sNodeType, sNodeName, priority );

	if(ini)	delete ini;
	if(ownerIni)	delete ownerIni;
}

void __declspec(dllexport) __cdecl XINTERFACE::SFLB_CreateNode(INIFILE* pOwnerIni, INIFILE* pUserIni, char *sNodeType, char *sNodeName, long priority)
{
	if( !sNodeType || !sNodeType[0] )
	{
		api->Trace("Warning! Interface: Can`t create node with null type.");
		return;
	}
	/*if( !pOwnerIni->TestSection( sNodeType ) &&
		!pUserIni->TestSection( sNodeType ) )
	{
		api->Trace("Warning! Interface: Node type %s not present into describe.",sNodeType);
		return;
	}*/

	if( !sNodeName )
		sNodeName = "?";

	char param[1024];
	if( !pOwnerIni->ReadString( sNodeType, "class", param,sizeof(param)-1,param) )
		_snprintf( param, sizeof(param)-1, "%s", sNodeType );

	CINODE* pNewNod = null;
	pNewNod = NewNode(param);
	if( pNewNod )
	{
		pNewNod->ptrOwner = this;
		pNewNod->pPictureService = pPictureService;
		pNewNod->pStringService = pStringService;
		pNewNod->SetPriority( priority );
		XYPOINT xypScreenSize;
		xypScreenSize.x = dwScreenWidth; xypScreenSize.y = dwScreenHeight;
		pNewNod->m_nodeName = NEW char[strlen(sNodeName)+1];
		if( !pNewNod->m_nodeName )	THROW("allocate memory error");
		strcpy( pNewNod->m_nodeName, sNodeName );
		if( !pNewNod->Init( pUserIni,sNodeName, pOwnerIni,sNodeType, pRenderService,GlobalRect,xypScreenSize ) )
		{
			delete pNewNod;
			pNewNod = 0;
		}
		else
		{
			AddNodeToList( pNewNod, priority );
		}

		INIFILE* usedini = pUserIni;
		pNewNod->m_bBreakPress = pNewNod->GetIniBool( pUserIni,sNodeName, pOwnerIni,sNodeType, "bBreakCommand", false );
		if( pNewNod->GetIniBool( pUserIni,sNodeName, pOwnerIni,sNodeType, "moveMouseDoSelect", false ) )
			pNewNod->m_bMouseSelect = true;
		//if( pNewNod->ReadIniString(pUserIni,sNodeName, pOwnerIni,sNodeType, "command", param,sizeof(param)-1, "") ) do
		if( usedini && usedini->ReadString( pNewNod->m_nodeName,"command",param,sizeof(param)-1,"") ) do
		{
			// get command name
			char stmp[sizeof(param)];
			sscanf(param,"%[^,]",stmp);
			// search command
			int nComNum = FindCommand(stmp);
			if(nComNum==-1)	continue;

			pNewNod->m_bSelected = true;
			pNewNod->m_bClickable = true;
			pNewNod->m_pCommands[nComNum].bUse = true;
			// command sequence
			size_t nFirstChar = strlen(stmp)+1;
			while( nFirstChar<strlen(param) )
			{
				sscanf(&param[nFirstChar],"%[^,]",stmp);
				nFirstChar += strlen(stmp)+1;

				// redirect control
				if( !strncmp(stmp,"select:",7) )
				{
					DublicateString( pNewNod->m_pCommands[nComNum].sRetControl, &stmp[7] );
					continue;
				}

				// subnodes control
				if( !strncmp(stmp,"com:",4) )
				{
					char sSubNodName[sizeof(param)];
					int  nSubCommand;	char sSubCommand[sizeof(param)];
					sscanf(stmp,"com:%[^:]:%[^,]",sSubCommand,sSubNodName);
					if( (nSubCommand=FindCommand(sSubCommand))==-1 ) continue;

					CINODE::COMMAND_REDIRECT *pHead = NEW CINODE::COMMAND_REDIRECT;
					ZeroMemory(pHead,sizeof(CINODE::COMMAND_REDIRECT));
					if( pHead==NULL )	THROW("allocate memory error");
					pHead->next = pNewNod->m_pCommands[nComNum].pNextControl;
					pNewNod->m_pCommands[nComNum].pNextControl = pHead;
					DublicateString( pHead->sControlName, sSubNodName );
					pHead->command = pCommandsList[nSubCommand].code;

					continue;
				}

				if( !strncmp(stmp,"event:",6) )
				{
					char sEventName[sizeof(param)];
					sscanf(stmp,"event:%[^,]",sEventName);
					DublicateString( pNewNod->m_pCommands[nComNum].sEventName, sEventName );
					continue;
				}

				if( !strncmp(stmp,"delay:",6) )
				{
					sscanf(stmp,"delay:%d", &pNewNod->m_pCommands[nComNum].nActionDelay);
					continue;
				}

				if( !strncmp(stmp,"sound:",6) )
				{
					sscanf(stmp,"sound:%d", &pNewNod->m_pCommands[nComNum].nSound);
					continue;
				}
			}
		} while(usedini->ReadStringNext(pNewNod->m_nodeName,"command",param,sizeof(param)-1));
	}

	if( m_pEditor && pNewNod ) m_pEditor->AddNode( pNewNod );
}

CINODE* XINTERFACE::NewNode(const char* pcNodType)
{
	if( !pcNodType ) return 0;
	CINODE* pNewNod = 0;
	if(!stricmp(pcNodType,"BUTTON"))				pNewNod = NEW CXI_BUTTON;
	else if(!stricmp(pcNodType,"VIDEO"))			pNewNod = NEW CXI_VIDEO;
	else if(!stricmp(pcNodType,"SCROLLIMAGE"))		pNewNod = NEW CXI_SCROLLIMAGE;
	else if(!stricmp(pcNodType,"IMAGECOLLECTION"))	pNewNod = NEW CXI_IMGCOLLECTION;
	else if(!stricmp(pcNodType,"STRINGCOLLECTION"))	pNewNod = NEW CXI_STRCOLLECTION;
	else if(!stricmp(pcNodType,"FOURIMAGES"))		pNewNod = NEW CXI_FOURIMAGE;
	else if(!stricmp(pcNodType,"RECTANGLE"))		pNewNod = NEW CXI_RECTANGLE;
	else if(!stricmp(pcNodType,"BOUNDER"))			pNewNod = NEW CXI_BOUNDER;
	else if(!stricmp(pcNodType,"TITLE"))			pNewNod = NEW CXI_TITLE;
	else if(!stricmp(pcNodType,"TEXTBUTTON"))		pNewNod = NEW CXI_TEXTBUTTON;
	else if(!stricmp(pcNodType,"SCROLLBAR"))		pNewNod = NEW CXI_SCROLLBAR;
	else if(!stricmp(pcNodType,"LINECOLLECTION"))	pNewNod = NEW CXI_LINECOLLECTION;
	else if(!stricmp(pcNodType,"STATUSLINE"))	    pNewNod = NEW CXI_STATUSLINE;
	else if(!stricmp(pcNodType,"CHANGER"))   	    pNewNod = NEW CXI_CHANGER;
	else if(!stricmp(pcNodType,"PICTURE"))  		pNewNod = NEW CXI_PICTURE;
	else if(!stricmp(pcNodType,"GLOWS"))  			pNewNod = NEW CXI_GLOWER;
	else if(!stricmp(pcNodType,"LRCHANGER"))		pNewNod = NEW CXI_LRCHANGER;
	else if(!stricmp(pcNodType,"TWO_PICTURE"))		pNewNod = NEW CXI_TWOPICTURE;
	else if(!stricmp(pcNodType,"SCROLLER"))			pNewNod = NEW CXI_SCROLLER;
	else if(!stricmp(pcNodType,"QUESTTITLE"))		pNewNod = NEW CXI_QUESTTITLE;
	else if(!stricmp(pcNodType,"QUESTTEXT"))		pNewNod = NEW CXI_QUESTTEXTS;
	else if(!stricmp(pcNodType,"SLIDEPICTURE"))		pNewNod = NEW CXI_SLIDEPICTURE;
	else if(!stricmp(pcNodType,"FORMATEDTEXT"))		pNewNod = NEW CXI_FORMATEDTEXT;
	else if(!stricmp(pcNodType,"EDITBOX"))			pNewNod = NEW CXI_EDITBOX;
	else if(!stricmp(pcNodType,"SLIDER"))			pNewNod = NEW CXI_SLIDELINE;
	else if(!stricmp(pcNodType,"KEYCHOOSER"))		pNewNod = NEW CXI_KEYCHANGER;
	else if(!stricmp(pcNodType,"VIDEORECTANGLE"))	pNewNod = NEW CXI_VIDEORECT;
	else if(!stricmp(pcNodType,"VIMAGESCROLL"))		pNewNod = NEW CXI_VIMAGESCROLL;
	else if(!stricmp(pcNodType,"PCEDITBOX"))		pNewNod = NEW CXI_PCEDITBOX;
	else if(!stricmp(pcNodType,"SCROLLEDPICTURE"))	pNewNod = NEW CXI_SCROLLEDPICTURE;
	else if(!stricmp(pcNodType,"WINDOW"))			pNewNod = NEW CXI_WINDOW;
	else if(!stricmp(pcNodType,"CHECKBUTTON"))		pNewNod = NEW CXI_CHECKBUTTONS;
	else if(!stricmp(pcNodType,"TABLE"))			pNewNod = NEW CXI_TABLE;
	else if(!stricmp(pcNodType,"FRAME"))			pNewNod = NEW CXI_BORDER;
	else if(!stricmp(pcNodType,"CONTEXTHELP"))  	m_pContHelp = pNewNod = NEW CXI_CONTEXTHELP;
	else if(!stricmp(pcNodType,"GLOWCURSOR"))		m_pGlowCursorNode = pNewNod = NEW CXI_GLOWCURSOR;
	else
		api->Trace("Not supported node type:\"%s\"", pcNodType);
	return pNewNod;
}

void XINTERFACE::DeleteNode(const char *pcNodeName)
{
	if( !pcNodeName ) return;
	// ищем нод по имени
	CINODE* pPrevNod = 0;
	for( CINODE* pNod=m_pNodes; pNod; pNod=pNod->m_next ) {
		if( pNod->m_nodeName && stricmp(pNod->m_nodeName,pcNodeName)==0 ) break;
		pPrevNod=pNod;
	}
	// нашли?
	if( pNod )
	{
		// проверка указателей на ссылку к удал€емому ноду
		if( m_pCurNode==pNod ) m_pCurNode=null;
		if( m_pContHelp==pNod ) m_pContHelp=null;
		if( m_pGlowCursorNode==pNod ) m_pGlowCursorNode=null;
		// "вырезаем" найденный нод из списка
		if( pPrevNod ) pPrevNod->m_next = pNod->m_next;
		else m_pNodes = pNod->m_next;
		if( m_pEditor ) m_pEditor->DelNode( pNod );
		// удал€ем нод
		delete pNod;
	}
}

void XINTERFACE::SetTooltip( const char* pcHeader,
				const char* pcText1,dword dwTextColor1,
				const char* pcText2,dword dwTextColor2,
				const char* pcText3,dword dwTextColor3,
				const char* pcText4,dword dwTextColor4,
				const char* pcPicTextureName,const char* pcPicGroupName,const char* pcPicImageName,
				long nPicWidth,long nPicHeight )
{
	CINODE* pTmpNod;
	pTmpNod = FindNode("tooltip_frame",0);
	CXI_BORDER* pNodFrame = pTmpNod ? pTmpNod->m_nNodeType==NODETYPE_BORDER ? (CXI_BORDER*)pTmpNod : 0 : 0;
	pTmpNod = FindNode("tooltip_titlerect",0);
	CXI_RECTANGLE* pNodTitleRect = pTmpNod ? pTmpNod->m_nNodeType==NODETYPE_RECTANGLE ? (CXI_RECTANGLE*)pTmpNod : 0 : 0;
	pTmpNod = FindNode("tooltip_picture",0);
	CXI_PICTURE* pNodPic = pTmpNod ? pTmpNod->m_nNodeType==NODETYPE_PICTURE ? (CXI_PICTURE*)pTmpNod : 0 : 0;
	pTmpNod = FindNode("tooltip_textborder2",0);
	CINODE* pNodTextFrame2 = pTmpNod ? pTmpNod->m_nNodeType==NODETYPE_RECTANGLE ? pTmpNod : 0 : 0;
	pTmpNod = FindNode("tooltip_textborder4",0);
	CINODE* pNodTextFrame4 = pTmpNod ? pTmpNod->m_nNodeType==NODETYPE_RECTANGLE ? pTmpNod : 0 : 0;
	pTmpNod = FindNode("tooltip_titile",0);
	CXI_STRCOLLECTION* pNodTitle = pTmpNod ? pTmpNod->m_nNodeType==NODETYPE_STRINGCOLLECTION ? (CXI_STRCOLLECTION*)pTmpNod : 0 : 0;
	pTmpNod = FindNode("tooltip_text1",0);
	CXI_FORMATEDTEXT* pNodText1 = pTmpNod ? pTmpNod->m_nNodeType==NODETYPE_FORMATEDTEXTS ? (CXI_FORMATEDTEXT*)pTmpNod : 0 : 0;
	pTmpNod = FindNode("tooltip_text2",0);
	CXI_FORMATEDTEXT* pNodText2 = pTmpNod ? pTmpNod->m_nNodeType==NODETYPE_FORMATEDTEXTS ? (CXI_FORMATEDTEXT*)pTmpNod : 0 : 0;
	pTmpNod = FindNode("tooltip_text3",0);
	CXI_FORMATEDTEXT* pNodText3 = pTmpNod ? pTmpNod->m_nNodeType==NODETYPE_FORMATEDTEXTS ? (CXI_FORMATEDTEXT*)pTmpNod : 0 : 0;
	pTmpNod = FindNode("tooltip_text4",0);
	CXI_FORMATEDTEXT* pNodText4 = pTmpNod ? pTmpNod->m_nNodeType==NODETYPE_FORMATEDTEXTS ? (CXI_FORMATEDTEXT*)pTmpNod : 0 : 0;
	if( !pNodFrame || !pNodTitleRect || !pNodPic || !pNodTextFrame2 || !pNodTextFrame4 || !pNodTitle || !pNodText1 || !pNodText2 || !pNodText3 || !pNodText4 ) {
		api->Trace("Warning! Interface::SetTooltip - no precreated node");
		return;
	}
	// set
	pNodTitle->ChangeString( 0, pcHeader );
	if( pcPicTextureName && pcPicTextureName[0] )
		pNodPic->SetNewPicture( false, (char*)pcPicTextureName );
	if( pcPicGroupName && pcPicImageName && pcPicGroupName[0] && pcPicImageName[0] )
		pNodPic->SetNewPictureByGroup( (char*)pcPicGroupName, (char*)pcPicImageName );
	pNodPic->SetPictureSize( nPicWidth,nPicHeight );
	if( pcText1 && pcText1[0] ) {
		pNodText1->SetFormatedText( (char*)pcText1 );
		pNodText1->SetColor( dwTextColor1 );
	}
	if( pcText2 && pcText2[0] ) {
		pNodText2->SetFormatedText( (char*)pcText2 );
		pNodText2->SetColor( dwTextColor2 );
	}
	if( pcText3 && pcText3[0] ) {
		pNodText3->SetFormatedText( (char*)pcText3 );
		pNodText3->SetColor( dwTextColor3 );
	}
	if( pcText4 && pcText4[0] ) {
		pNodText4->SetFormatedText( (char*)pcText4 );
		pNodText4->SetColor( dwTextColor4 );
	}

	XYRECT rTitle, rPic, rTex1, rTex2Rect, rTex2, rTex3, rTex4Rect, rTex4;
	rTitle = pNodTitleRect->m_rect;
	rPic = pNodPic->m_rect;
	rTex1 = pNodText1->m_rect;
	rTex2 = pNodText2->m_rect;
	rTex3 = pNodText3->m_rect;
	rTex4 = pNodText4->m_rect;
	rTex2Rect = pNodTextFrame2->m_rect;
	rTex4Rect = pNodTextFrame4->m_rect;

	long nAllHeight = TT_TITLE_OFFSET + TT_TITLE_HEIGHT;
	rTitle.top = TT_TITLE_OFFSET;
	rTitle.bottom = TT_TITLE_HEIGHT;

	if( nPicHeight > 0 ) {
		rPic.top = nAllHeight + TT_PIC_UP_OFFSET;
		rPic.bottom = nPicHeight;
		nAllHeight = rPic.top + rPic.bottom + TT_PIC_DOWN_OFFSET;
	} else {
		pNodPic->m_bUse = false;
	}

	long nTmpH = pNodText1->GetAllHeight();
	if( nTmpH > 0 ) {
		rTex1.top = nAllHeight + TT_TEX1_UP_OFFSET;
		rTex1.bottom = nTmpH;
		nAllHeight = rTex1.top + rTex1.bottom + TT_TEX1_DOWN_OFFSET;
	} else {
		pNodText1->m_bUse = false;
	}

	nTmpH = pNodText2->GetAllHeight();
	if( nTmpH > 0 ) {
		rTex2Rect.top = nAllHeight + TT_TEX2RECT_UP_OFFSET;
		rTex2.top = rTex2Rect.top + TT_TEX2_UP_OFFSET;
		rTex2.bottom = nTmpH;
		rTex2Rect.bottom = rTex2.top - rTex2Rect.top + rTex2.bottom + TT_TEX2_DOWN_OFFSET;
		nAllHeight = rTex2Rect.top + rTex2Rect.bottom + TT_TEX2RECT_DOWN_OFFSET;
	} else {
		pNodTextFrame2->m_bUse = false;
		pNodText2->m_bUse = false;
	}

	nTmpH = pNodText3->GetAllHeight();
	if( nTmpH > 0 ) {
		rTex3.top = nAllHeight + TT_TEX3_UP_OFFSET;
		rTex3.bottom = nTmpH;
		nAllHeight = rTex3.top + rTex3.bottom + TT_TEX3_DOWN_OFFSET;
	} else {
		pNodText3->m_bUse = false;
	}

	nTmpH = pNodText4->GetAllHeight();
	if( nTmpH > 0 ) {
		rTex4Rect.top = nAllHeight + TT_TEX4RECT_UP_OFFSET;
		rTex4.top = rTex4Rect.top + TT_TEX4_UP_OFFSET;
		rTex4.bottom = nTmpH;
		rTex4Rect.bottom = rTex4.top - rTex4Rect.top + rTex4.bottom + TT_TEX4_DOWN_OFFSET;
		nAllHeight = rTex4Rect.top + rTex4Rect.bottom + TT_TEX4RECT_DOWN_OFFSET;
	} else {
		pNodTextFrame4->m_bUse = false;
		pNodText4->m_bUse = false;
	}

	nAllHeight += TT_FRAME_DOWN_OFFSET;

	// frame
	XYRECT rFrame = pNodFrame->m_rect;
	rFrame.top = (rFrame.top + rFrame.bottom - nAllHeight) / 2;
	rFrame.bottom = rFrame.top + nAllHeight;
	pNodFrame->ChangePosition( rFrame );
	// title
	rTitle.top += rFrame.top;
	rTitle.bottom += rTitle.top;
	pNodTitleRect->ChangePosition( rTitle );
	XYPOINT newTitlePnt;
	newTitlePnt.x = (rTitle.left + rTitle.right) / 2;
	newTitlePnt.y = rTitle.top + TT_TITLE_STRING_OFFSET;
	pNodTitle->ChangeStringPos( 0, newTitlePnt );
	// pic
	rPic.top += rFrame.top;
	rPic.bottom += rPic.top;
	pNodPic->ChangePosition( rPic );
	// text 1
	rTex1.top += rFrame.top;
	rTex1.bottom += rTex1.top;
	pNodText1->ChangePosition( rTex1 );
	// text 2 rect
	rTex2Rect.top += rFrame.top;
	rTex2Rect.bottom += rTex2Rect.top;
	pNodTextFrame2->ChangePosition( rTex2Rect );
	// text 2
	rTex2.top += rFrame.top;
	rTex2.bottom += rTex2.top;
	pNodText2->ChangePosition( rTex2 );
	// text 3
	rTex3.top += rFrame.top;
	rTex3.bottom += rTex3.top;
	pNodText3->ChangePosition( rTex3 );
	// text 4 rect
	rTex4Rect.top += rFrame.top;
	rTex4Rect.bottom += rTex4Rect.top;
	pNodTextFrame4->ChangePosition( rTex4Rect );
	// text 4
	rTex4.top += rFrame.top;
	rTex4.bottom += rTex4.top;
	pNodText4->ChangePosition( rTex4 );
}

void XINTERFACE::ShowWindow( const char* pcWindowName, bool bShow )
{
	if( !m_pNodes ) return;
	CINODE * pNod = m_pNodes->FindNode( pcWindowName );
	if( !pNod ) return;
	if( pNod->m_nNodeType != NODETYPE_WINDOW ) return;

	((CXI_WINDOW*)pNod)->SetShow( bShow );
}

void XINTERFACE::DisableWindow( const char* pcWindowName, bool bDisable )
{
	if( !m_pNodes ) return;
	CINODE * pNod = m_pNodes->FindNode( pcWindowName );
	if( !pNod ) return;
	if( pNod->m_nNodeType != NODETYPE_WINDOW ) return;

	((CXI_WINDOW*)pNod)->SetActive( !bDisable );
}

void XINTERFACE::AddNodeToWindow( const char* pcNodeName, const char* pcWindowName )
{
	if( !m_pNodes ) {
		api->Trace("Warning! Interface::AddNodeToWindow(%s,%s) : Empty node list", pcNodeName, pcWindowName);
		return;
	}

	CINODE * pNod = m_pNodes->FindNode( pcWindowName );
	if( !pNod || pNod->m_nNodeType != NODETYPE_WINDOW ) {
		api->Trace("Warning! Interface::AddNodeToWindow(%s,%s) : Window not found", pcNodeName, pcWindowName);
		return;
	}

	((CXI_WINDOW*)pNod)->AddNode( pcNodeName );
}

CXI_WINDOW* XINTERFACE::FindWindow(const char* pcWindowName)
{
	CINODE * pNod = m_pNodes->FindNode( pcWindowName );
	if( pNod && pNod->m_nNodeType==NODETYPE_WINDOW ) return (CXI_WINDOW*)pNod;
	return 0;
}

bool XINTERFACE::IsWindowActive(const char* pcWindowName)
{
	CINODE * pNod = m_pNodes->FindNode( pcWindowName );
	if( pNod && pNod->m_nNodeType==NODETYPE_WINDOW )
		return ((CXI_WINDOW*)pNod)->GetActive();
	return false;
}

void XINTERFACE::RegistryExitKey( const char* pcKeyName )
{
	m_asExitKey.Add( pcKeyName );
}

long XINTERFACE::StoreNodeLocksWithOff()
{
	long nStoreSlot = m_aLocksArray.Add();
	for( long nStoreCode=0; nStoreCode<1000; nStoreCode++ ) {
		for( long n=0; n<m_aLocksArray; n++ )
			if( m_aLocksArray[n].nSaveCode == nStoreCode ) break;
		if( n==m_aLocksArray ) break;
	}
	m_aLocksArray[nStoreSlot].nSaveCode = nStoreCode;
	for(CINODE* pNod=m_pNodes; pNod; pNod=pNod->m_next)
		//if( !pNod->m_bLockStatus ) {
		if( !pNod->m_bLockedNode ) {
			m_aLocksArray[nStoreSlot].aNode.Add(pNod);
			//pNod->m_bLockStatus = true;
			pNod->m_bLockedNode = true;
		}
	return nStoreCode;
}

void XINTERFACE::RestoreNodeLocks(long nStoreCode)
{
	for( long n=0; n<m_aLocksArray; n++ )
		if( m_aLocksArray[n].nSaveCode == nStoreCode ) break;
	if( n==m_aLocksArray ) return;
	for( long i=0; i<m_aLocksArray[n].aNode; i++ ) {
		//m_aLocksArray[n].aNode[i]->m_bLockStatus = false;
		m_aLocksArray[n].aNode[i]->m_bLockedNode = false;
	}
	m_aLocksArray.DelIndex( n );
}

void XINTERFACE::DrawNode(CINODE* nod,dword Delta_Time, long startPrior, long endPrior)
{
	for( ; nod!=null; nod=nod->m_next)
	{
		if(nod->GetPriority()<startPrior) continue;
		if(nod->GetPriority()>endPrior) break;
		if(nod->m_bUse)
		{
			if(nod==m_pGlowCursorNode)
			{
				nod->Draw(false,0);
				continue;
			}
			if(nod==m_pCurNode)
			{
				if(m_pGlowCursorNode && m_pGlowCursorNode->m_bUse)	m_pGlowCursorNode->Draw(false,Delta_Time);
			}
			nod->Draw(nod==m_pCurNode,Delta_Time);
		}
		else nod->NotUsingTime(Delta_Time);
		DrawNode(nod->m_list,Delta_Time);
	}
}

void XINTERFACE::DoControl()
{
	long nExitKey;
	CONTROL_STATE cs;
	if(!m_bUse) return;

	api->Controls->GetControlState("IContextHelp",cs);

	if(m_nInterfaceMode==CONTEXTHELP_IMODE)
	{
		if( cs.state == CST_ACTIVATED || cs.state == CST_INACTIVATED )
			ReleaseContextHelpData();
		return;
	}

	if( cs.state == CST_ACTIVATED )
	{
		GetContextHelpData();
		return;
	}

	if( m_pEditor && m_pEditor->ProcessControl() ) return;

	if(bDisableControl)
	{
		api->Controls->GetControlState(INTERFACE_CONTROL_RIGHT,cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState(INTERFACE_CONTROL_LEFT,cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState(INTERFACE_CONTROL_UP,cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState(INTERFACE_CONTROL_DOWN,cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState(INTERFACE_CONTROL_LSHIFT,cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState(INTERFACE_CONTROL_RSHIFT,cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState(INTERFACE_CONTROL_ENTER,cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState(INTERFACE_CONTROL_BACK,cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState(INTERFACE_CONTROL_BREAK,cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState(INTERFACE_CONTROL_LCLICK,cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState(INTERFACE_CONTROL_RCLICK,cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState("IStartButton",cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState("IUp2",cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState("IDown2",cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState("IRight2",cs);
		if (cs.state != CST_INACTIVE) return;
		api->Controls->GetControlState("ILeft2",cs);
		if (cs.state != CST_INACTIVE) return;

		for(nExitKey=0; nExitKey<m_asExitKey; nExitKey++)
		{
			api->Controls->GetControlState( (char*)m_asExitKey[nExitKey].GetBuffer(), cs );
			if (cs.state != CST_INACTIVE) return;
		}

		if(!CheckPCcd()) {
			api->Event("evntCheckFailure");
		}
	}
	if(!g_bIExclusiveMode && !DiskCheck) return;
	bDisableControl = false;

	for(nExitKey=0; nExitKey<m_asExitKey; nExitKey++)
	{
		api->Controls->GetControlState( (char*)m_asExitKey[nExitKey].GetBuffer(), cs );
		if( cs.state == CST_ACTIVATED ) {
			api->Event("exitCancel",0);
			break;
		}
	}

	int wActCode = ACTION_NOTHING;

	KEYSTATE curKS;
	curKS.dwKeyCode=0;

	bool bDeClick = false;
	bool bWasFirst = false;
	m_bMouseClick = false;
	m_bDblMouseClick = false;
	api->Controls->GetControlState(INTERFACE_CONTROL_LCLICK,cs);
	if (cs.state == CST_ACTIVATED)
	{
		m_bMouseClick = true;
        m_idButton = MOUSE_LBUTTON;
		bWasFirst = true;
	}
	if (cs.state == CST_INACTIVATED)
	{
		bDeClick = true;
		m_idButton = MOUSE_LBUTTON;
		if( bDeClick && m_pMouseNode && m_pMouseNode->m_bMakeActionInDeclick )
		{
			MouseDeClick();
		}
	}
	if (cs.state == CST_ACTIVE)
	{
		m_bMouseClick = true;
        m_idButton = MOUSE_LBUTTON;
	}

	api->Controls->GetControlState(INTERFACE_CONTROL_RCLICK,cs);
	if(!m_bMouseClick && cs.state == CST_ACTIVATED)
	{
		m_bMouseClick = true;
		m_idButton = MOUSE_RBUTTON;
		bWasFirst = true;
		api->Event("MouseRClickDown");
	}
	if(cs.state == CST_INACTIVATED)
	{
		api->Event("MouseRClickUP");
	}
	if(!m_bMouseClick && cs.state == CST_ACTIVE)
	{
		m_bMouseClick = true;
		m_idButton = MOUSE_RBUTTON;
	}

	if(m_bMouseClick || m_bDblMouseClick)
	{
		MouseClick(bWasFirst);
	}
	else
	{
		bool bFirstPress = false;
		// Get action code CST_ACTIVATED, CST_ACTIVE

			// right press
		api->Controls->GetControlState(INTERFACE_CONTROL_RIGHT,cs);
		if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
			curKS.rightButton = true;
		if (cs.state == CST_ACTIVATED)	bFirstPress = true;
		api->Controls->GetControlState("IRight2",cs);
		if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
			curKS.rightButton = true;
		if (cs.state == CST_ACTIVATED)	bFirstPress = true;
			// left press
		api->Controls->GetControlState(INTERFACE_CONTROL_LEFT,cs);
		if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
			curKS.leftButton = true;
		if (cs.state == CST_ACTIVATED)	bFirstPress = true;
		api->Controls->GetControlState("ILeft2",cs);
		if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
			curKS.leftButton = true;
		if (cs.state == CST_ACTIVATED)	bFirstPress = true;
			// up press
		api->Controls->GetControlState(INTERFACE_CONTROL_UP,cs);
		if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
			curKS.upButton = true;
		if (cs.state == CST_ACTIVATED)	bFirstPress = true;
		api->Controls->GetControlState("IUp2",cs);
		if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
			curKS.upButton = true;
		if (cs.state == CST_ACTIVATED)	bFirstPress = true;
			// down press
		api->Controls->GetControlState(INTERFACE_CONTROL_DOWN,cs);
		if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
			curKS.downButton = true;
		if (cs.state == CST_ACTIVATED)	bFirstPress = true;
		api->Controls->GetControlState("IDown2",cs);
		if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
			curKS.downButton = true;
		if (cs.state == CST_ACTIVATED)	bFirstPress = true;
			// left shift press
		api->Controls->GetControlState(INTERFACE_CONTROL_LSHIFT,cs);
		if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
			curKS.shiftButton = true;
		if (cs.state == CST_ACTIVATED)	bFirstPress = true;
			// right shift press
		api->Controls->GetControlState(INTERFACE_CONTROL_RSHIFT,cs);
		if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
			curKS.shiftButton = true;
		if (cs.state == CST_ACTIVATED)	bFirstPress = true;
			// enter button press
		api->Controls->GetControlState(INTERFACE_CONTROL_ENTER,cs);
		if (cs.state == CST_ACTIVATED)
			curKS.enterButton = true;
		if (cs.state == CST_ACTIVATED)	bFirstPress = true;
		api->Controls->GetControlState("IStartButton",cs);
		if (cs.state == CST_ACTIVATED)
			curKS.enterButton = true;
		if (cs.state == CST_ACTIVATED)	bFirstPress = true;
			// back button press
		api->Controls->GetControlState(INTERFACE_CONTROL_BACK,cs);
		if (cs.state == CST_ACTIVATED)
			curKS.backButton = true;
		if (cs.state == CST_ACTIVATED)	bFirstPress = true;
		api->Controls->GetControlState(INTERFACE_CONTROL_BREAK,cs);
		if (cs.state == CST_ACTIVATED)
			curKS.backButton = true;
		if (cs.state == CST_ACTIVATED)	bFirstPress = true;

		long nWeel = 0;
		if(m_pMouseWeel) m_pMouseWeel->Get(nWeel);
		if( nWeel!=0 && m_pCurNode && m_pCurNode->IsWeelActive() )
		{
			if(nWeel>0) curKS.upButton = true;
			else curKS.downButton = true;
		}

		//
		if(AttributesPointer)
		{
			ATTRIBUTES * pA = AttributesPointer->GetAttributeClass("KeyStates");
			if(pA)	pA->SetAttributeUseDword("shift",curKS.shiftButton);
		}

		if(oldKeyState.dwKeyCode==-1) oldKeyState.dwKeyCode = curKS.dwKeyCode;

		if(oldKeyState.dwKeyCode != curKS.dwKeyCode)
		{
			if(curKS.rightButton)
			{
				if(curKS.shiftButton)	wActCode = ACTION_SPEEDRIGHT;
				else					wActCode = ACTION_RIGHTSTEP;
			}
			if(curKS.leftButton)
			{
				if(curKS.shiftButton)	wActCode = ACTION_SPEEDLEFT;
				else					wActCode = ACTION_LEFTSTEP;
			}
			if(curKS.upButton)
			{
				if(curKS.shiftButton)	wActCode = ACTION_SPEEDUP;
				else					wActCode = ACTION_UPSTEP;
			}
			if(curKS.downButton)
			{
				if(curKS.shiftButton)	wActCode = ACTION_SPEEDDOWN;
				else					wActCode = ACTION_DOWNSTEP;
			}
			if(curKS.enterButton)
				wActCode = ACTION_ACTIVATE;
			if(curKS.backButton)
				wActCode = ACTION_DEACTIVATE;

			if(m_pCurNode==NULL)
			{
				if(wActCode==ACTION_DEACTIVATE)
					api->Event("exitCancel",0);
				return;
			}

			bool bBreakPress=false;
			if(!m_pCurNode->m_bLockStatus && wActCode!=ACTION_NOTHING)
			{
				if(m_nPressDelay==0)
				{
					if(!m_bNotFirstPress)
					{
						m_nPressDelay = m_nMaxPressDelay;
						m_bNotFirstPress = true;
					}
					CINODE * pNewNod = m_pCurNode->DoAction(wActCode,bBreakPress,bFirstPress);
					if(pNewNod!=NULL)
					{
                        SetCurNode(pNewNod);
						bBreakPress = true;
					}
				}
			}
			else
			{
				m_nPressDelay = 0;
				m_bNotFirstPress = false;
			}

			if(bBreakPress || curKS.dwKeyCode==0)	oldKeyState.dwKeyCode = curKS.dwKeyCode;
		}
		else
		{
			m_nPressDelay = 0;
			m_bNotFirstPress = false;
		}
	}
}

CINODE* XINTERFACE::GetActivingNode(CINODE *findRoot)
{
	CINODE *retVal=NULL;

	while(findRoot)
	{
		if(findRoot->m_bClickable)
		{
			retVal=findRoot;
			break;
		}
		if((retVal=GetActivingNode(findRoot->m_list))!=0)	break;

		findRoot = findRoot->m_next;
	}

	return retVal;
}

void XINTERFACE::MouseMove()
{
	if(m_nInterfaceMode==CONTEXTHELP_IMODE) return;
#ifdef _XBOX
	return;
#endif
	CONTROL_STATE csv,csh;
	api->Controls->GetControlState(INTERFACE_MOUSE_VERT,csv);
	api->Controls->GetControlState(INTERFACE_MOUSE_HORZ,csh);

	float fOutX = 0.f;
	float fOutY = 0.f;

	CINODE* pNod = null;

	if(csv.lValue!=0 || csh.lValue!=0)
	{
		m_nMouseLastClickTimeCur = 0;

		fYMousePos -= csv.fValue;
		fXMousePos += csh.fValue;

		if(fYMousePos<0) {
			fOutY = fYMousePos;
			fYMousePos = 0.f;
		}
		if(fYMousePos>dwScreenHeight) {
			fOutY = fYMousePos - dwScreenHeight;
			fYMousePos = (float)dwScreenHeight;
		}

		if(fXMousePos<0) {
			fOutX = fXMousePos;
			fXMousePos = 0.f;
		}
		if(fXMousePos>dwScreenWidth) {
			fOutX = fXMousePos - dwScreenWidth;
			fXMousePos = (float)dwScreenWidth;
		}

		m_bShowMouse = true;
		vMouse[0].pos.x = vMouse[1].pos.x = fXMousePos - MouseSize.x/2;
		vMouse[2].pos.x = vMouse[3].pos.x = fXMousePos + MouseSize.x/2;
		vMouse[0].pos.y = vMouse[2].pos.y = fYMousePos - MouseSize.y/2;
		vMouse[1].pos.y = vMouse[3].pos.y = fYMousePos + MouseSize.y/2;

		m_pCurToolTipNode = 0;
		pNod = m_pNodes ? m_pNodes->FindNode(fXMousePos+m_lXMouse,fYMousePos+m_lYMouse) : NULL;
		while(pNod!=NULL)
		{
			if( pNod->m_bUse )
			{
				if( !m_pCurToolTipNode && pNod->CheckByToolTip( fXMousePos+m_lXMouse, fYMousePos+m_lYMouse ) )
					m_pCurToolTipNode = pNod;
				if( !pNod->m_bLockedNode )
				{
					if(pNod->m_bSelected)
						pNod->MouseThis(fXMousePos+m_lXMouse,fYMousePos+m_lYMouse);
					if( pNod->m_bMouseSelect && pNod->m_bSelected )
					{
						SetCurNode(pNod);
						break;
					}
				}
			}
			if(pNod->m_next==NULL) break;
			pNod = pNod->m_next->FindNode(fXMousePos+m_lXMouse,fYMousePos+m_lYMouse);
		}
	} else {
		if( m_pCurToolTipNode )
			m_pCurToolTipNode->CheckByToolTip( fXMousePos+m_lXMouse, fYMousePos+m_lYMouse );
	}

	ATTRIBUTES* pA = AttributesPointer->GetAttributeClass("mousepos");
	if( !pA ) pA = AttributesPointer->CreateSubAClass( AttributesPointer, "mousepos" );
	if( pA ) {
		pA->SetAttributeUseFloat( "x", fXMousePos + m_lXMouse );
		pA->SetAttributeUseFloat( "y", fYMousePos + m_lYMouse );
	}

	if( fXMousePos < m_fpMouseOutZoneOffset.x )
		fOutX += fXMousePos - m_fpMouseOutZoneOffset.x;
	if( fXMousePos > dwScreenWidth-m_fpMouseOutZoneOffset.x )
		fOutX += fXMousePos - dwScreenWidth + m_fpMouseOutZoneOffset.x;

	if( fYMousePos < m_fpMouseOutZoneOffset.y )
		fOutY += fYMousePos - m_fpMouseOutZoneOffset.y;
	if( fYMousePos > dwScreenHeight-m_fpMouseOutZoneOffset.y )
		fOutY += fYMousePos - dwScreenHeight + m_fpMouseOutZoneOffset.y;

	if( fOutX != 0.f || fOutY != 0.f )
	{
		for( pNod=m_pNodes; pNod!=null; pNod=pNod->m_next )
		{
			pNod->MoveMouseOutScreen( fOutX, fOutY );
		}
	}
}

void XINTERFACE::MouseClick(bool bFirstClick)
{
	if(!bFirstClick && m_nPressDelay>0) return;
	CINODE * clickNod = GetClickNode(m_pNodes, (long)fXMousePos+m_lXMouse,(long)fYMousePos+m_lYMouse);
	if( !clickNod ) {
		if(bFirstClick) m_pMouseNode=0;
		return;
	}
	if(!m_bNotFirstPress) m_bNotFirstPress = true;
	m_nPressDelay = m_nMaxPressDelay;

	if( bFirstClick && m_nMouseLastClickTimeCur>0 && clickNod->CheckCommandUsed(ACTION_MOUSEDBLCLICK) ) {
		m_bDblMouseClick = true;
	}
	m_nMouseLastClickTimeCur = m_nMouseLastClickTimeMax;

	if(clickNod!=NULL)
		if(m_pCurNode==NULL || !m_pCurNode->m_bLockStatus)
		{
			if(bFirstClick)	m_pMouseNode=clickNod;
			else if( m_pMouseNode != clickNod ) return;

			if( m_idButton==MOUSE_LBUTTON && clickNod->m_bMakeActionInDeclick )
			{
				clickNod->MakeLClickPreaction();
			}
			else
			{
				bool bBreakPress;
				CINODE * pNewNod;

				if(m_bDblMouseClick)
					if( m_idButton == MOUSE_LBUTTON )
						pNewNod = clickNod->DoAction(ACTION_MOUSEDBLCLICK,bBreakPress,bFirstClick);
					else pNewNod = clickNod->DoAction(ACTION_MOUSERDBLCLICK,bBreakPress,bFirstClick);
				else
					if( m_idButton == MOUSE_LBUTTON )
						pNewNod = clickNod->DoAction(ACTION_MOUSECLICK,bBreakPress,bFirstClick);
					else pNewNod = clickNod->DoAction(ACTION_MOUSERCLICK,bBreakPress,bFirstClick);

					if(pNewNod!=NULL)
					{
						if(pNewNod!=m_pCurNode)	m_bNotFirstPress=false;
						SetCurNode(pNewNod);
					}
					if(bBreakPress)
					{
						m_bNotFirstPress=false;
						m_bMouseClick = false;
					}
					m_bDblMouseClick = false;
			}
		}
}

void XINTERFACE::MouseDeClick()
{
	if( !m_pMouseNode || !m_pMouseNode->m_bMakeActionInDeclick ) return;

	bool bBreakPress = true;
	CINODE * pNewNod = NULL;
	if( m_idButton == MOUSE_LBUTTON )
		pNewNod = m_pMouseNode->DoAction(ACTION_MOUSECLICK,bBreakPress,true);

	if(pNewNod!=NULL)
	{
		if(pNewNod!=m_pCurNode)	m_bNotFirstPress=false;
		SetCurNode(pNewNod);
	}
	if(bBreakPress)
	{
		m_bNotFirstPress=false;
		m_bMouseClick = false;
	}
}

CINODE * XINTERFACE::GetClickNode(CINODE * searchNod, long xPos, long yPos)
{
	CINODE * findNod = null;

	while( searchNod!=null )
	{
		if( searchNod->m_bUse && !searchNod->m_bLockedNode )
		{
			if( searchNod->m_bSelected && searchNod->m_bClickable && searchNod->CheckCommandUsed(ACTION_MOUSECLICK))
			{
				if( searchNod->IsClick(m_idButton,xPos,yPos) )
					findNod = searchNod;
			}
			if(searchNod->m_list!=null)
			{
				CINODE * tmpNod = GetClickNode(searchNod,xPos,yPos);
				if(tmpNod!=null) findNod=tmpNod;
			}
		}

		searchNod = searchNod->m_next;
	}

	return findNod;
}

void XINTERFACE::ShowPrevTexture()
{
	XI_ONETEX_VERTEX pV[32];
	int idx=0;
	float fAngle = m_fAngle;

	DWORD dwBlendColor = m_nBlendColor;
	if(dwBlendColor>0xFFL)
		dwBlendColor = 0xFFL;
	DWORD dwColor = 0xFFFFFF | (dwBlendColor<<24L);

	float fWaveAmplitude = m_fWaveAmplitude*sinf(3.14f*m_nBlendColor/m_nBlendStepMax);
	for(int ix=0; ix<m_nColumnQuantity+1; ix++)
	{
		pV[idx].color = pV[idx+1].color = dwColor;
		pV[idx].pos.z = pV[idx+1].pos.z = 1.f + fWaveAmplitude*sinf(fAngle);
		fAngle += m_fWavePhase;
		pV[idx].pos.x = pV[idx+1].pos.x = ((float)dwScreenWidth/m_nColumnQuantity)*ix;
		pV[idx].pos.y = 0; pV[idx+1].pos.y = (float)dwScreenHeight;
		pV[idx].tv = 0.f; pV[idx+1].tv = 1.f;
		pV[idx].tu = pV[idx+1].tu = (float)ix/m_nColumnQuantity;
		idx+=2;
	}

	m_fAngle += m_fWaveSpeed;
	m_nBlendColor -= m_nBlendSpeed;
	if(m_nBlendColor<=0L)
	{
		m_nBlendColor=0L;
		m_fAngle = 0.f;
		m_bShowPrevTexture = false;
		if(m_pPrevTexture!=NULL)	pRenderService->Release(m_pPrevTexture);
		m_pPrevTexture = NULL;
	}

	pRenderService->SetTexture(0,m_pPrevTexture);
	pRenderService->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
	pRenderService->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
	pRenderService->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	pRenderService->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pRenderService->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pRenderService->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	pRenderService->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONETEX_FVF,30,pV,sizeof(XI_ONETEX_VERTEX));
}

void XINTERFACE::ReleaseOld()
{
	if( m_pEditor ) m_pEditor->ReCreate();

	ReleaseContextHelpData();
	PTR_DELETE(m_strDefHelpTextureFile);

	oldKeyState.dwKeyCode = 0;
	if(m_stringes!=NULL)
	{
        for(int i=0; i<m_nStringQuantity; i++)
		{
            PTR_DELETE(m_stringes[i].sStringName);
			FONT_RELEASE(pRenderService,m_stringes[i].fontNum);
		}
		PTR_DELETE(m_stringes);
		m_nStringQuantity = 0;
	}

    while(m_pEvents!=null)
    {
        EVENT_ENTITY * ei = m_pEvents;
        m_pEvents = m_pEvents->next;
        delete ei;
    }

	while(m_pNodes!=null)
	{
		m_pNodes->ReleaseAll();
        CINODE * pNod = m_pNodes;
		m_pNodes=m_pNodes->m_next;
		delete pNod;
	}
    m_pCurNode = null;
    m_pContHelp = null;
	m_pGlowCursorNode = null;

	while(m_imgLists!=null)
	{
		PICTURE_TEXTURE_RELEASE(pPictureService,m_imgLists->sImageListName,m_imgLists->idTexture);
		PTR_DELETE(m_imgLists->sImageListName);
		PTR_DELETE(m_imgLists->sImageName);
		PTR_DELETE(m_imgLists->sPicture);
		IMAGE_ENTITY * pCur = m_imgLists;
		m_imgLists = m_imgLists->next;
		delete pCur;
	};

	m_asExitKey.DelAll();

    m_bUse = false;
	m_pMouseNode = null;
}

bool XINTERFACE::SetCurNode(CINODE *pNod)
{
    if(m_pCurNode==pNod) return false;
    m_pCurNode = pNod;

    if(m_pContHelp!=NULL)
        ((CXI_CONTEXTHELP*)m_pContHelp)->ChangeNode(pNod);

    return true;
}

dword XINTERFACE::AttributeChanged(ATTRIBUTES *patr)
{
	if(patr!=NULL && patr->GetParent()!=NULL && patr->GetParent()->GetParent()!=NULL)
	{
		char * sParentName = patr->GetParent()->GetParent()->GetThisName();
		if( sParentName==NULL || stricmp(sParentName,"pictures")!=0 ) return 0;
		char * sImageName = patr->GetParent()->GetThisName();
		if(sImageName==NULL) return 0;
		// find this picture
		IMAGE_ENTITY * pImList = m_imgLists;
		while(pImList!=NULL)
			if(pImList->sImageName!=NULL && stricmp(pImList->sImageName,sImageName)==0) break;
			else pImList=pImList->next;
		// no this picture / create new
		if(pImList==NULL)
		{
			pImList = NEW IMAGE_ENTITY;
			if(pImList==NULL)	{THROW("Allocation memory error");}
			ZeroMemory(pImList,sizeof(IMAGE_ENTITY));
			if( (pImList->sImageName=NEW char[strlen(sImageName)+1]) == NULL )
				{THROW("Allocate memory error");}
			strcpy(pImList->sImageName,sImageName);
			// insert that into images list
			pImList->next = m_imgLists;
			m_imgLists = pImList;
		}
		if( patr->GetThisName()==NULL ) return 0;
		// set picture
		if( stricmp(patr->GetThisName(),"pic") == 0 )
		{
			PTR_DELETE(pImList->sPicture);
			if(patr->GetThisAttr()!=NULL)
			{
				if( (pImList->sPicture=NEW char[strlen(patr->GetThisAttr())+1]) == NULL )
					{THROW("Allocate memory error");}
				strcpy(pImList->sPicture,patr->GetThisAttr());
			}
			if(pImList->sImageListName==NULL) return 0;
			pImList->imageID = pPictureService->GetImageNum(pImList->sImageListName,pImList->sPicture);
		}
		// set texture
		if( stricmp(patr->GetThisName(),"tex") == 0 )
		{
			if(pImList->sImageListName!=NULL) pPictureService->ReleaseTextureID(pImList->sImageListName);
			PTR_DELETE(pImList->sImageListName);
			if(patr->GetThisAttr()!=NULL)
			{
				if( (pImList->sImageListName=NEW char[strlen(patr->GetThisAttr())+1]) == NULL )
					{THROW("Allocate memory error");}
				strcpy(pImList->sImageListName,patr->GetThisAttr());
			}
			pImList->idTexture = pPictureService->GetTextureID(pImList->sImageListName);
			pImList->imageID = pPictureService->GetImageNum(pImList->sImageListName,pImList->sPicture);
		}
	}
	return 0;
}

bool __declspec(dllexport) __cdecl XINTERFACE::SFLB_DoSaveFileData(char * saveName, char * saveData)
{
	if(saveName==null || saveData==null) return false;
	long slen = strlen(saveData)+1;
	if(slen<=1) return false;

	ENTITY_ID ei;
	if( !api->FindClass(&ei,"SCRSHOTER",0) ) return false;
	IDirect3DTexture8 * ptex = (IDirect3DTexture8*)api->Send_Message(ei,"l",MSG_SCRSHOT_MAKE);
	if(ptex==null) return false;

	D3DSURFACE_DESC dscr;
	ptex->GetLevelDesc(0,&dscr);
	long nAllSize = sizeof(SAVE_DATA_HANDLE) + dscr.Size + slen;
	char * pdat = NEW char[nAllSize];
	if(pdat==null)	{ THROW("allocate memory error"); }

	((SAVE_DATA_HANDLE*)pdat)->StringDataSize = slen;
	if(slen>0) memcpy( &pdat[sizeof(SAVE_DATA_HANDLE)], saveData, slen );

	((SAVE_DATA_HANDLE*)pdat)->SurfaceDataSize = dscr.Size;
	if(dscr.Size>0)
	{
		D3DLOCKED_RECT lockRect;
		if( ptex->LockRect(0,&lockRect,null,0) == D3D_OK )
		{
			memcpy( &pdat[sizeof(SAVE_DATA_HANDLE)+slen], lockRect.pBits, dscr.Size );
			ptex->UnlockRect(0);
		}
		else api->Trace("Can`t lock screenshot texture");
	}

	api->SetSaveData(saveName,pdat,nAllSize);
	delete pdat;
	return true;
}

bool __declspec(dllexport) __cdecl XINTERFACE::SFLB_GetSaveFileData(char * saveName, long bufSize, char * buf)
{
	if(buf==null || bufSize<=0) return false;
	long allDatSize = 0;
	char * pdat = (char*)api->GetSaveData(saveName,allDatSize);
	if(pdat==null) return false;

	long strSize = ((SAVE_DATA_HANDLE*)pdat)->StringDataSize;
	if(strSize>=bufSize)
	{
		buf[bufSize-1] = 0;
		strSize = bufSize-1;
	}
	if(strSize>0) memcpy(buf, &pdat[sizeof(SAVE_DATA_HANDLE)], strSize);

	delete pdat;
	return strSize>0;
}

void XINTERFACE::ReleaseSaveFindList()
{
	while(m_pSaveFindRoot)
	{
		SAVE_FIND_DATA * p = m_pSaveFindRoot;
		m_pSaveFindRoot = m_pSaveFindRoot->next;
		PTR_DELETE(p->save_file_name);
		PTR_DELETE(p);
	}
}

void XINTERFACE::AddFindData(char * sSaveFileName, long file_size, FILETIME file_time)
{
	if(!sSaveFileName || sSaveFileName[0]=='\0') return;
	SAVE_FIND_DATA * p = new SAVE_FIND_DATA;
	if(p) {
		p->time = file_time;
		p->file_size = file_size;
		p->next = m_pSaveFindRoot;
		m_pSaveFindRoot = p;
		p->save_file_name = new char[strlen(sSaveFileName)+1];
		if( p->save_file_name )
			strcpy(p->save_file_name,sSaveFileName);
	}
}

void XINTERFACE::Sorting_FindData()
{
	if(!m_pSaveFindRoot) return; // do nothing (empty list)
	bool bMakeSorting = true;
	while(bMakeSorting)
	{
		bMakeSorting = false;
		SAVE_FIND_DATA * pprev = 0;
		for(SAVE_FIND_DATA * pcur = m_pSaveFindRoot; pcur->next; pcur = pcur->next)
		{
			if( CompareFileTime(&pcur->next->time,&pcur->time) > 0 )
			{
				bMakeSorting = true;
				SAVE_FIND_DATA * p1 = pcur;
				SAVE_FIND_DATA * p2 = pcur->next;
				p1->next = p2->next;
				p2->next = p1;
				if(pprev) pprev->next = p2;
				else m_pSaveFindRoot = p2;
				pcur = p2;
			}
			pprev = pcur;
		}
	}
}

XINTERFACE::SAVE_FIND_DATA * XINTERFACE::GetSaveDataByIndex(int n)
{
	SAVE_FIND_DATA * p = m_pSaveFindRoot;
	for(int i=0; i<n; i++)
		if(p)
			p = p->next;
	return p;
}

char * XINTERFACE::SaveFileFind(long saveNum, char * buffer, size_t bufSize, long & fileSize)
{
	if(!m_pSaveFindRoot) // create save file list
	{
		// different function save file find for XBOX or PC
#ifndef _XBOX
		WIN32_FIND_DATA	wfd;
		// get file name for searching (whith full path)
		char param[1024];
		char * sSavePath = AttributesPointer->GetAttribute("SavePath");
		if(sSavePath==NULL) sprintf(param,"*");
		else
		{
			api->fio->_CreateDirectory(sSavePath,0);
			sprintf(param,"%s\\*",sSavePath);
		}
		// start save file finding
		HANDLE h = api->fio->_FindFirstFile(param,&wfd);
		if(h!=INVALID_HANDLE_VALUE)
		{
			do
			{
				// folders not be considers
				if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) continue;
				AddFindData( wfd.cFileName, 0, wfd.ftLastWriteTime );
			} while( api->fio->_FindNextFile(h,&wfd)!=0 );
			// close handle for file finding
			api->fio->_FindClose(h);
		}
#else
		int n;
		long file_sz;
		char sTemp[MAX_PATH], sFullName[MAX_PATH];
		XGAME_FIND_DATA fd;
		// start save file finding
		HANDLE sh = XFindFirstSaveGame("U:\\",&fd);
		if(sh != INVALID_HANDLE_VALUE)
		{
			do
			{
				for(n=0; fd.szSaveGameName[n]; n++)
					sTemp[n] = (char)fd.szSaveGameName[n];
				sTemp[n] = 0;
				if( n>=6 && strnicmp(sTemp,"options",6)!=0 )
				{
					sprintf( sFullName, "%s%s", fd.szSaveGameDirectory, sTemp );
					file_sz = (fd.wfd.nFileSizeLow + (1<<14) - 1) >> 14;
					AddFindData( sFullName, file_sz, fd.wfd.ftLastWriteTime );
				}
			} while( XFindNextSaveGame(sh,&fd) );
			XFindClose(sh);
		}
#endif
		// common part
		Sorting_FindData();
	}

	// default setting for finded file
	fileSize = 0;
	if(buffer) buffer[0] = 0;
	// get data for save file by his index
	SAVE_FIND_DATA * pSFD = GetSaveDataByIndex(saveNum);
	if(!pSFD)
	{
		ReleaseSaveFindList();
		return null;
	}
	fileSize = pSFD->file_size;
	if( pSFD->save_file_name )
	{
		int q = strlen(pSFD->save_file_name);
		if( q >= (int)bufSize ) q = bufSize-1;
		if( q>0 ) strncpy(buffer, pSFD->save_file_name, q);
		if( q>=0 ) buffer[q]=0;
	}
	return buffer;
}

bool XINTERFACE::NewSaveFileName(char * fileName)
{
	if(fileName==null) return false;

#ifdef _XBOX
	XGAME_FIND_DATA fd;
	HANDLE sh = XFindFirstSaveGame("U:\\",&fd);
	if(sh == INVALID_HANDLE_VALUE) return true;

	do	{
		char tmpstr[256];
		for(int n=0; fd.szSaveGameName[n]; n++)	tmpstr[n] = (char)fd.szSaveGameName[n];
		tmpstr[n] = 0;

		if( stricmp(tmpstr,fileName)==0 )
		{
			XFindClose(sh);
			return false;
		}
	} while( XFindNextSaveGame(sh,&fd) );

	XFindClose(sh);
	return true;
#else
	char * sSavePath;
	char param[256];
	WIN32_FIND_DATA	wfd;
	sSavePath = AttributesPointer->GetAttribute("SavePath");

	if(sSavePath==null) sprintf(param,"%s",fileName);
	else sprintf(param,"%s\\%s",sSavePath,fileName);

	HANDLE h = api->fio->_FindFirstFile(param,&wfd);
	if (h == INVALID_HANDLE_VALUE) return true;

	api->fio->_FindClose(h);
	return false;
#endif
}

void XINTERFACE::DeleteSaveFile(char * fileName)
{
	if(fileName==null) return;
	char param[256];
	char * sSavePath = AttributesPointer->GetAttribute("SavePath");
#ifndef _XBOX
	WIN32_FIND_DATA	wfd;
	if(sSavePath==NULL) sprintf(param,"%s",fileName);
	else sprintf(param,"%s\\%s",sSavePath,fileName);
	HANDLE h = api->fio->_FindFirstFile(param,&wfd);
	if (INVALID_HANDLE_VALUE != h)
	{
		api->fio->_FindClose(h);
		api->fio->_DeleteFile(param);
	}
#else
	int i,j,startIdx;
	WCHAR wparam[256];
	startIdx = 0;
	for(i=0; fileName[i]!=0; i++)
		if(fileName[i]=='\\') startIdx = i+1;
	for(j=0,i=startIdx; fileName[i]!=0,j<255; i++,j++) wparam[j] = fileName[i];
	wparam[j] = 0;
	if(startIdx>0)	sprintf(param,"%c:\\",*fileName);
	else param[0] = 0;
	XDeleteSaveGame(param,wparam);
#endif
}

dword XINTERFACE_BASE::GetBlendColor(dword minCol,dword maxCol,float fBlendFactor)
{
	long ad = long(ALPHA(maxCol))	-	long(ALPHA(minCol));
	long rd = long(RED(maxCol))		-	long(RED(minCol));
	long gd = long(GREEN(maxCol))	-	long(GREEN(minCol));
	long bd = long(BLUE(maxCol))	-	long(BLUE(minCol));
	ad = ALPHA(minCol)	+ long(ad*fBlendFactor);
	rd = RED(minCol)	+ long(rd*fBlendFactor);
	gd = GREEN(minCol)	+ long(gd*fBlendFactor);
	bd = BLUE(minCol)	+ long(bd*fBlendFactor);
	return ARGB(ad,rd,gd,bd);
}

void XINTERFACE::SetOtherData(char * cDat)
{
	BYTE xorMul = (BYTE)CriptedName[sizeof(FINDBYTES)];
	BYTE xorAdd = (BYTE)CriptedName[sizeof(FINDBYTES)+1];
	BYTE *crdata = (BYTE*)&CriptedName[sizeof(FINDBYTES)+2];
	BYTE * outDat = (BYTE*)cDat;
	for(int i=0; i<sizeof(CriptedName)-sizeof(FINDBYTES)-2; i++)
	{
		outDat[i] = crdata[i]^xorMul;
		xorMul += xorAdd;
	}
}

void XINTERFACE::AddNodeToList(CINODE * nod,long priority)
{
	if(nod==null) return;
	if(m_pNodes==null || m_pNodes->GetPriority()>priority)
	{
		nod->m_next = m_pNodes;
		m_pNodes = nod;
		return;
	}
	for(CINODE* pnod=m_pNodes; pnod->m_next!=null; pnod=pnod->m_next)
		if(pnod->GetPriority()<=priority && pnod->m_next->GetPriority()>priority) break;
	nod->m_next = pnod->m_next;
	pnod->m_next = nod;
}

void XINTERFACE::SetExclusiveNode(CINODE * nod)
{
	g_bIExclusiveMode = true;
	for(CINODE* pnod=m_pNodes; pnod!=null; pnod=pnod->m_next)
		pnod->m_bLockedNode = true;
	if(nod!=null) nod->m_bLockedNode = false;
}

void XINTERFACE::AddExclusiveNode(CINODE * nod)
{
	if(nod!=null) nod->m_bLockedNode = false;
}

void XINTERFACE::ExitFromExclusive()
{
	g_bIExclusiveMode = false;
	for(CINODE* pnod=m_pNodes; pnod!=null; pnod=pnod->m_next)
		pnod->m_bLockedNode = false;
}

void XINTERFACE::ReleaseDinamicPic(char * sPicName)
{
	if(sPicName==null) return;

	IMAGE_ENTITY * prevImg = null;
	for(IMAGE_ENTITY * findImg=m_imgLists; findImg!=null; findImg=findImg->next)
	{
		if(findImg->sImageName!=null && stricmp(findImg->sImageName,sPicName)==0)	break;
		prevImg = findImg;
	}
	if(findImg==null) return;

	PICTURE_TEXTURE_RELEASE(pPictureService,findImg->sImageListName,findImg->idTexture);
	PTR_DELETE(findImg->sImageListName);
	PTR_DELETE(findImg->sImageName);
	PTR_DELETE(findImg->sPicture);
	if(prevImg==null) m_imgLists = findImg->next;
	else prevImg->next = findImg->next;
}

long FindMaxStrForWidth(VDX8RENDER * pVR, int nW, char * str, int nFontID, float fScale )
{
	if(!pVR || !str || str[0]=='\0') return 0;
	int nPrev = -1;
	for(int curLen=strlen(str); curLen>=0; curLen--)
	{
		if( str[curLen]==32 || curLen==0 )
		{
			int nFirst = curLen;
			if( curLen!=0 ) nFirst++;
			if( pVR->StringWidth(&str[nFirst], nFontID, fScale) < nW ) {
				nPrev = nFirst;
			} else {
				if( nPrev==-1 ) nPrev = nFirst;
				break;
			}
			if( curLen==0 ) break;
		}
	}
	return nPrev;
}

long XINTERFACE::PrintIntoWindow(long wl,long wr, long idFont, DWORD dwFCol, DWORD dwBCol, long align, bool shadow, float scale, long sxs, long sys, long left, long top, char * str, int nWidthForScaleCorrecting, int nSplit)
{
	if(!str) return 0;
	long strWidth = pRenderService->StringWidth(str, idFont, scale);

	// check fontScale
	if( nWidthForScaleCorrecting>0 && strWidth>nWidthForScaleCorrecting ) {
		if(nSplit==0)
		{ // —троку не разбивать, а уменьшить масштаб шрифта
			scale *= (float)nWidthForScaleCorrecting/strWidth;
			strWidth = pRenderService->StringWidth(str, idFont, scale);
		} else
		{ // —троку разбить с выдвижением на заданное число
			int maxWidth = 0;
			int nPrev = -1;
			char strLocTmp[1024];
			strcpy(strLocTmp,str);
			while( nPrev )
			{
				int nStart = FindMaxStrForWidth(pRenderService, nWidthForScaleCorrecting, strLocTmp, idFont, scale);
				int curWidth = PrintIntoWindow(wl,wr,idFont,dwFCol,dwBCol,align,shadow,scale,sxs,sys,left,top, &strLocTmp[nStart]);
				if(curWidth>maxWidth) maxWidth = curWidth;
				if(nStart==0) break;
				while( nStart>0 && strLocTmp[nStart-1] == 0x20 ) nStart--;
				strLocTmp[nStart]='\0';
				top += nSplit;
			}
			return maxWidth;
		}
	}

	long strLeft = left;
	if(align==ALIGN_RIGHT)	strLeft = left - strWidth;
	if(align==ALIGN_CENTER)	strLeft = left - strWidth/2;
	long strRight = strLeft + strWidth;

	if(strLeft>=wl && strRight<=wr)
	{
		return pRenderService->ExtPrint( idFont, dwFCol, dwBCol, align, shadow, scale, sxs, sys,
										left, top, "%s", str );
	}

	char * newStr = str;
	// режем слева
	while(strLeft<wl && newStr!=null && newStr[0]!=0)
	{
		newStr++;
		strWidth = pRenderService->StringWidth(newStr, idFont, scale);
		strLeft = strRight - strWidth;
	}
	// режем справа
	if( newStr != null )
	{
		strRight = strLeft+strWidth;
		long nEnd = strlen(newStr);
		while(nEnd>0 && strRight>wr)
		{
			nEnd--;
			char chOld = newStr[nEnd];
			newStr[nEnd] = 0;
			strWidth = pRenderService->StringWidth(newStr, idFont, scale);
			newStr[nEnd] = chOld;
			strRight = strLeft + strWidth;
		}
		if( nEnd>0 ) {
			char chOld = newStr[nEnd];
			newStr[nEnd] = 0;
			strWidth = pRenderService->ExtPrint( idFont, dwFCol, dwBCol, ALIGN_LEFT, shadow, scale, sxs, sys,
				strLeft, top, "%s", newStr );
			newStr[nEnd] = chOld;
			return strWidth;
		}
		return 0;
	}

	return pRenderService->ExtPrint( idFont, dwFCol, dwBCol, ALIGN_LEFT, shadow, scale, sxs, sys,
									strLeft, top, "%s", newStr );
}

void XINTERFACE::IncrementGameTime(DWORD dwDeltaTime)
{
	bool bYesChange = false;
	m_dwCurDeltaTime += dwDeltaTime;

	while(m_dwCurDeltaTime>=1000)
	{
		m_dwGameTimeSec++;
		m_dwCurDeltaTime -= 1000;
		bYesChange = true;
	}

	while(m_dwGameTimeSec>=60)
	{
		m_dwGameTimeMin++;
		m_dwGameTimeSec -= 60;
	}

	while(m_dwGameTimeMin>=60)
	{
		m_dwGameTimeHour++;
		m_dwGameTimeMin -= 60;
	}

	if(bYesChange && m_bUse)
		api->Event("ievent_SetGameTime","lll", m_dwGameTimeHour, m_dwGameTimeMin, m_dwGameTimeSec);
}

char * AddAttributesStringsToBuffer(char * inBuffer, char * prevStr, ATTRIBUTES * pAttr)
{
	int q = pAttr->GetAttributesNum();
	for(int k=0; k<q; k++)
	{
		ATTRIBUTES * pA = pAttr->GetAttributeClass(k);
		if(pA==null) continue;

		char * attrVal = pA->GetThisAttr();
		char * attrName = pA->GetThisName();
		if( attrName && attrVal && attrVal[0] )
		{
			int nadd = strlen("\n")+strlen(attrName)+strlen("=")+strlen(attrVal)+1;
			if(prevStr!=null) nadd += strlen(prevStr)+strlen(".");
			if(inBuffer!=null) nadd += strlen(inBuffer);

			char * pNew = NEW char[nadd];
			if(pNew==null) continue;
			pNew[0] = 0;

			if(inBuffer)
			{
				strcat(pNew,inBuffer);
				strcat(pNew,"\n");
			}

			if(prevStr)
			{
				strcat(pNew,prevStr);
				strcat(pNew,".");
			}

			strcat(pNew,attrName);
			strcat(pNew,"=");
			strcat(pNew,attrVal);

			if(inBuffer)	delete inBuffer;
			inBuffer = pNew;
		}

		if(pA->GetAttributesNum()!=0)
		{
			char param[512]; param[0]=0;
			if(prevStr)	sprintf(param,"%s.",prevStr);
			strcat(param,pA->GetThisName());
			inBuffer = AddAttributesStringsToBuffer(inBuffer,param,pA);
		}
	}

	return inBuffer;
}

void XINTERFACE::SaveOptionsFile(char * fileName, ATTRIBUTES * pAttr)
{
	HANDLE fh;
	char FullPath[MAX_PATH];

	if(fileName==null || pAttr==null) return;
#ifdef _XBOX
	int n;
	char PathBuffer[MAX_PATH];
	WORD FileBuffer[MAX_PATH];
	for(n=0;fileName[n];n++)
	{
		FileBuffer[n] = fileName[n];
	}
	FileBuffer[n] = 0;
	if(XCreateSaveGame("U:\\",FileBuffer,OPEN_ALWAYS,0,PathBuffer,sizeof(PathBuffer)) != ERROR_SUCCESS)
	{
		api->Trace("cant create options ini file");
		return;
	}
	strcpy(FullPath,PathBuffer);
	strcat(FullPath,fileName);

	strcat(PathBuffer,"saveimage.xbx");
	CopyFile("d:\\resource\\textures\\options.xbx",PathBuffer,FALSE);
#else
	strcpy(FullPath,fileName);
#endif

	api->fio->SetDrive(XBOXDRIVE_NONE);
	PrecreateDirForFile(FullPath);
	fh = api->fio->_CreateFile(FullPath,GENERIC_WRITE,0,CREATE_ALWAYS);
	api->fio->SetDrive();
	if(fh == INVALID_HANDLE_VALUE) return;

	char * pOutBuffer = null;
	DWORD dwSaveSize=0, dwRealSaved=0;

	if(pAttr)	pOutBuffer = AddAttributesStringsToBuffer(null,null,pAttr);

	if(pOutBuffer)
	{
#ifdef _XBOX
		// calculate signature
		XCALCSIG_SIGNATURE xsig;
		HANDLE hSig;
		memset(&xsig,0,sizeof(xsig));
		hSig = XCalculateSignatureBegin( 0 );
		// calculate signature for data
		if(hSig != INVALID_HANDLE_VALUE) XCalculateSignatureUpdate(hSig,(BYTE *)pOutBuffer,strlen(pOutBuffer));
		if(hSig != INVALID_HANDLE_VALUE) XCalculateSignatureEnd(hSig,&xsig);
		// save signature
		api->fio->_WriteFile(fh, &xsig, sizeof(xsig), &dwRealSaved);
#endif

		api->fio->_WriteFile(fh, pOutBuffer, strlen(pOutBuffer), &dwRealSaved);
		delete pOutBuffer;
	}
	api->fio->_CloseHandle(fh);
}

void XINTERFACE::LoadOptionsFile(char * fileName, ATTRIBUTES * pAttr)
{
	HANDLE fh;
	char FullPath[MAX_PATH];

	if(fileName==null || pAttr==null) return;
#ifdef _XBOX
	int n;
	char PathBuffer[MAX_PATH];
	WORD FileBuffer[MAX_PATH];
	for(n=0;fileName[n];n++)
	{
		FileBuffer[n] = fileName[n];
	}
	FileBuffer[n] = 0;
	if(XCreateSaveGame("U:\\",FileBuffer,OPEN_EXISTING,0,PathBuffer,sizeof(PathBuffer)) != ERROR_SUCCESS)
	{
		api->Trace("cant open options ini file");
		return;
	}
	strcpy(FullPath,PathBuffer);
	strcat(FullPath,fileName);
#else
	strcpy(FullPath,fileName);
#endif

	api->fio->SetDrive(XBOXDRIVE_NONE);
	fh = api->fio->_CreateFile(FullPath,GENERIC_READ,0,OPEN_EXISTING);
	api->fio->SetDrive();
	if(fh == INVALID_HANDLE_VALUE) return;

	DWORD dwRealSize;
	DWORD dwSaveSize = api->fio->_GetFileSize(fh,0);
	if(dwSaveSize==0)
	{
		api->Event("evntOptionsBreak");
		api->fio->_CloseHandle(fh);
		return;
	}

	char * pOutBuffer = NEW char[dwSaveSize+1];
	if(pOutBuffer)
	{
		api->fio->_ReadFile(fh, pOutBuffer, dwSaveSize, &dwRealSize);
		if(pAttr)
		{
			char * pBuf = pOutBuffer;
#ifdef _XBOX
			XCALCSIG_SIGNATURE xsig;
			XCALCSIG_SIGNATURE save_xsig;
			HANDLE hSig;

			if( dwSaveSize<=sizeof(save_xsig) || dwRealSize <= sizeof(save_xsig) )
				pBuf = 0;
			else {
				// get signature from saved options
				memcpy( &save_xsig, pBuf, sizeof(save_xsig) );
				pBuf = &pBuf[sizeof(save_xsig)];

				// calculate signature for current title
				memset(&xsig,0,sizeof(xsig));
				hSig = XCalculateSignatureBegin( 0 );
				// calculate signature for data
				if(hSig != INVALID_HANDLE_VALUE) XCalculateSignatureUpdate(hSig,(BYTE *)pBuf,dwSaveSize-sizeof(save_xsig));
				if(hSig != INVALID_HANDLE_VALUE) XCalculateSignatureEnd(hSig,&xsig);

				// compare signatures
				if(memcmp(&xsig,&save_xsig,sizeof(xsig))!=0) {
					//options file is broken
					pBuf = 0; // Do not read info from a file
					api->Event("evntOptionsBreak");
				}
			}
#endif
			while(pBuf && *pBuf!=0)
			{
				char param1[512], param2[512];
				param1[0] = param2[0] = 0;
				sscanf(pBuf,"%[^=]=%[^\n]", param1, param2);
				if( strlen(param1)>0 && strlen(param2)>0 )
				{
					ATTRIBUTES * pA = pAttr->CreateSubAClass(pAttr,param1);
					pA->SetValue(param2);
				}
				//pBuf += strlen(param1)+strlen(param2);
				while(*pBuf && *pBuf!=13 && *pBuf!=10)	pBuf++;
				while(*pBuf && (*pBuf==13 || *pBuf==10)) pBuf++;
			}
		}
		delete pOutBuffer;
	}

	api->fio->_CloseHandle(fh);
}

void XINTERFACE::GetContextHelpData()
{
	ReleaseContextHelpData();

	char * texName = null;

	if(m_pCurNode)
	{
		texName = m_pCurNode->m_strHelpTextureFile;
		m_frectHelpTextureUV = m_pCurNode->m_frectHelpTextureUV;
		if(texName==null)
		{
			VDATA * pvdat = api->Event("ievntGetHelpTexture", "s", m_pCurNode->m_nodeName);
			if(pvdat!=null)	pvdat->Get(texName);
			if(texName!=null && strlen(texName)<1) texName=null;
		}
	}
	if(!texName)
	{
		texName = m_strDefHelpTextureFile;
		m_frectHelpTextureUV = m_frectDefHelpTextureUV;
	}

	if(texName!=null)	m_idHelpTexture = pRenderService->TextureCreate(texName);
	if(m_idHelpTexture!=-1)	m_nInterfaceMode = CONTEXTHELP_IMODE;
}

void XINTERFACE::ReleaseContextHelpData()
{
	TEXTURE_RELEASE(pRenderService,m_idHelpTexture);
	m_nInterfaceMode = DEFAULT_IMODE;
}

void XINTERFACE::ShowContextHelp()
{
	if(m_nInterfaceMode != CONTEXTHELP_IMODE) return;
	if(m_idHelpTexture==-1) return;

	// show picture
	XI_ONLYONETEX_VERTEX pV[4];
	for(int i=0;i<4;i++)	pV[i].pos.z = 1.f;

	dword oldTFactor;
	pRenderService->GetRenderState(D3DRS_TEXTUREFACTOR,&oldTFactor);

	pRenderService->TextureSet(0,m_idHelpTexture);

	pV[0].pos.x = pV[2].pos.x = 0;//(float)GlobalRect.left;
	pV[0].tu = pV[2].tu = m_frectHelpTextureUV.left;

	pV[1].pos.x = pV[3].pos.x = (float)dwScreenWidth;//(float)GlobalRect.right;
	pV[1].tu = pV[3].tu = m_frectHelpTextureUV.right;

	pV[0].pos.y = pV[1].pos.y = 0;//(float)GlobalRect.top;
	pV[0].tv = pV[1].tv = m_frectHelpTextureUV.top;

	pV[2].pos.y = pV[3].pos.y = (float)dwScreenHeight;//(float)GlobalRect.bottom;
	pV[2].tv = pV[3].tv = m_frectHelpTextureUV.bottom;

	pRenderService->SetRenderState(D3DRS_TEXTUREFACTOR,m_dwContHelpColor);
	pRenderService->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONLYONETEX_FVF,2,pV,sizeof(XI_ONLYONETEX_VERTEX),"iBlindPictures");
	pRenderService->SetRenderState(D3DRS_TEXTUREFACTOR,oldTFactor);
}

int XINTERFACE::LoadIsExist()
{
	char * sCurLngName = GetStringService()->GetLanguage();
	if(sCurLngName==null) return 0;

#ifndef _XBOX
	WIN32_FIND_DATA	wfd;
	char param[1024];
	char * sSavePath = AttributesPointer->GetAttribute("SavePath");
	if(sSavePath==NULL) sprintf(param,"*");
	else
	{
		api->fio->_CreateDirectory(sSavePath,0);
		sprintf(param,"%s\\*",sSavePath);
	}

	HANDLE h = api->fio->_FindFirstFile(param,&wfd);
	bool bFindFile = h!=INVALID_HANDLE_VALUE;
	for(int findQ=0; bFindFile; findQ++)
	{
		if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			bFindFile = api->fio->_FindNextFile(h,&wfd)!=0;
			findQ--; continue;
		}

		if(sSavePath==NULL) sprintf(param, "%s", wfd.cFileName);
		else	sprintf(param, "%s\\%s", sSavePath, wfd.cFileName);

		char datBuf[512];
		if( SFLB_GetSaveFileData(param, sizeof(datBuf), datBuf) )
		{
			int nLen = strlen(datBuf);
			for(int i=strlen(datBuf); i>=0 && datBuf[i]!='@'; i--);
			if(i<0) i=0;
			if(datBuf[i]=='@') i++;
			if( stricmp(sCurLngName,&datBuf[i])==0 ) break;
		}

		bFindFile = api->fio->_FindNextFile(h,&wfd)!=0;
	}
	if(h!=INVALID_HANDLE_VALUE)	api->fio->_FindClose(h);
	return bFindFile ? 1 : 0;
#else

	XGAME_FIND_DATA fd;
	HANDLE sh;

	sh = XFindFirstSaveGame("U:\\",&fd);
	bool bFindFile = sh!=INVALID_HANDLE_VALUE;

	while(bFindFile)
	{
		char datBuf[512];
		char stmp[512];
		for(int j=0;fd.szSaveGameName[j];j++) datBuf[j] = (char)fd.szSaveGameName[j];
		datBuf[j] = 0;
		sprintf(stmp, "%s%s", fd.szSaveGameDirectory, datBuf);
		if( SFLB_GetSaveFileData(stmp, sizeof(datBuf), datBuf) )
		{
			int nLen = strlen(datBuf);
			for(int i=strlen(datBuf); i>=0 && datBuf[i]!='@'; i--);
			if(i<0) i=0;
			if(datBuf[i]=='@') i++;
			if( stricmp(sCurLngName,&datBuf[i])==0 ) break;
		}

		bFindFile = XFindNextSaveGame(sh,&fd);
	}
	if(sh!=INVALID_HANDLE_VALUE)	XFindClose(sh);
	return bFindFile ? 1 : 0;
#endif
}

void XINTERFACE::PrecreateDirForFile(const char* pcFullFileName)
{
	if( !pcFullFileName ) return;
	char path[MAX_PATH];
	_snprintf(path,sizeof(path),"%s",pcFullFileName);
	for(long n=strlen(pcFullFileName)-1; n>0; n--)
		if( path[n]=='\\' ) {
			path[n]=0;
			break;
		}
	if( n>0 )
		api->fio->_CreateDirectory( path, 0 );
}

//  онтейнер контролек
CONTROLS_CONTAINER::CONTROLS_CONTAINER()
{
	pContainers = null;
}

CONTROLS_CONTAINER::~CONTROLS_CONTAINER()
{
	CONTEINER_DESCR * pCont = pContainers;
	while(pContainers)
	{
		CONTEINER_DESCR * pCont = pContainers;
		pContainers = pContainers->next;

		while(pCont->pControls)
		{
			CONTEINER_DESCR::CONTROL_DESCR * pCntrl = pCont->pControls;
			pCont->pControls = pCont->pControls->next;
			PTR_DELETE(pCntrl->controlName);
			PTR_DELETE(pCntrl);
		}

		PTR_DELETE(pCont->resultName);

		PTR_DELETE(pCont);
	}
}

bool CONTROLS_CONTAINER::Init()
{
	if( AttributesPointer!=null )
		return CreateConteinerList(AttributesPointer);
	return false;
}

void CONTROLS_CONTAINER::Execute(dword delta_time)
{
	CONTEINER_DESCR * pCont = pContainers;
	while(pCont)
	{
		CONTEINER_DESCR::CONTROL_DESCR * pDescr = pCont->pControls;
		CONTROL_STATE cs;
		CONTROL_STATE csPrev;
		cs.state = CST_INACTIVE;
		cs.fValue = 0.f;
		cs.lValue = 0;
		api->Controls->GetControlState(pCont->resultName,csPrev);
		while(pDescr)
		{
			if(pDescr->controlName)
			{
				CONTROL_STATE insideCS;
				api->Controls->GetControlState(pDescr->controlName, insideCS);

				switch(insideCS.state)
				{
				case CST_ACTIVATED:
					if(csPrev.state!=CST_ACTIVE && csPrev.state!=CST_ACTIVATED)
						cs.state = CST_ACTIVATED;
					else cs.state = CST_ACTIVE;
				break;

				case CST_INACTIVATED:
					if( cs.state!=CST_ACTIVE && cs.state!=CST_ACTIVATED )
					{
						if( csPrev.state!=CST_INACTIVE && csPrev.state!=CST_INACTIVATED )
							cs.state = CST_INACTIVATED;
						else cs.state = CST_INACTIVE;
					}
				break;

				case CST_ACTIVE:
					cs.state = CST_ACTIVE;
				break;
				}

				float fVal = insideCS.fValue * pDescr->fValLimit;
				if(fVal==0.f) fVal = insideCS.lValue * pDescr->fValLimit;
				if(cs.fValue<fVal) cs.fValue = fVal;

				long lVal = insideCS.lValue;
				if(cs.lValue<lVal) cs.lValue = lVal;
			}
			pDescr = pDescr->next;
		}
		if( stricmp(pCont->resultName,"ChrTurnH1")==0 )
			if(cs.state!=CST_INACTIVE)
				cs.state=cs.state;
		api->Controls->SetControlState( pCont->resultName, cs );
		api->Controls->GetControlState( pCont->resultName, csPrev );
		pCont = pCont->next;
	}
}

dword _cdecl CONTROLS_CONTAINER::ProcessMessage(MESSAGE & message)
{
	return 0;
}

bool CONTROLS_CONTAINER::CreateConteinerList(ATTRIBUTES * pA)
{
	if(!pA) return false;

	int q = pA->GetAttributesNum();
	for(int i=0; i<q; i++)
	{
		ATTRIBUTES * pAttr = pA->GetAttributeClass(i);
		if(!pAttr) continue;
		char * containerName = pAttr->GetThisName();
		if(!containerName) continue;
		AddContainer(containerName);
		SetContainerLimitVal(containerName, (float)atof(pAttr->GetThisAttr()));

		int cntSize = pAttr->GetAttributesNum();
		for(int n=0; n<cntSize; n++)
		{
			AddControlsToContainer(containerName, pAttr->GetAttributeName(n), (float)atof(pAttr->GetAttribute(n)));
		}
	}

	return true;
}

void CONTROLS_CONTAINER::AddContainer( char * container )
{
	if(!container) return;

	CONTEINER_DESCR * pCont = FindContainer(container);
	if(pCont) return;

	pCont = pContainers;
	pContainers = NEW CONTEINER_DESCR;
	if(!pContainers) _THROW("allocate memory error");
	pContainers->fMaxVal = 1.f;
	pContainers->pControls = null;
	pContainers->next = pCont;

	pContainers->resultName = NEW char[strlen(container)+1];
	if(!pContainers->resultName) _THROW("allocate memory error");
	strcpy(pContainers->resultName,container);
}

void CONTROLS_CONTAINER::SetContainerLimitVal( char * container, float fLimitVal )
{
	CONTEINER_DESCR * pCont = FindContainer(container);
	if(!pCont) return;
	pCont->fMaxVal = fLimitVal;
}

void CONTROLS_CONTAINER::AddControlsToContainer( char * container, char * controlName, float fValLimit )
{
	if(!container || !controlName) return;
	CONTEINER_DESCR * pCont = FindContainer(container);

	CONTEINER_DESCR::CONTROL_DESCR * pCtrl = pCont->FindControl(controlName);
	if(pCtrl) return;

	pCtrl = pCont->pControls;
	pCont->pControls = NEW CONTEINER_DESCR::CONTROL_DESCR;
	if(!pCont->pControls) _THROW("allocate memory error");

	pCont->pControls->fValLimit = fValLimit;
	pCont->pControls->next = pCtrl;
	pCont->pControls->controlName = NEW char[strlen(controlName)+1];
	if(!pCont->pControls->controlName) _THROW("allocate memory error");
	strcpy( pCont->pControls->controlName, controlName );
}

CONTROLS_CONTAINER::CONTEINER_DESCR * CONTROLS_CONTAINER::FindContainer(char * sContainer)
{
	if(!sContainer) return null;
	CONTEINER_DESCR * pCont = pContainers;
	while(pCont)
		if(pCont->resultName!=null && stricmp(pCont->resultName,sContainer)==0)
			return pCont;
	return null;
}

CONTROLS_CONTAINER::CONTEINER_DESCR::CONTROL_DESCR * CONTROLS_CONTAINER::CONTEINER_DESCR::FindControl(char * cntrlName)
{
	if(!cntrlName) return null;

	CONTROL_DESCR * pCtrl = pControls;
	while(pCtrl)
	{
		if( pCtrl->controlName && stricmp(pCtrl->controlName,cntrlName)==0 ) return pCtrl;
		pCtrl = pCtrl->next;
	}

	return null;
}

bool CheckPCcd()
{
#ifdef _XBOX
	DiskCheck = true;
	return true;
#else
	//return true;
	if( DiskCheck ) return true;

	int drive, curdrive;

	char nameDrv[32];
	strcpy(nameDrv,"A:\\");

	/* Save current drive. */
	curdrive = _getdrive();

	/* If we can switch to the drive, it exists. */
	for( drive = 1; drive < 27; drive++ )
	{
		nameDrv[0] = drive-1 + 'A';
		int nTmp;
		if( (nTmp=GetDriveType(nameDrv))==DRIVE_CDROM )
		{
			char CheckName[1024];
			sprintf(CheckName,"%c:\\%s", drive-1+'A', CHECK_FILE_NAME);
			SetErrorMode(SEM_FAILCRITICALERRORS);
			HANDLE hFile = CreateFile( CheckName, GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
			if(hFile == INVALID_HANDLE_VALUE) {
				SetErrorMode(0);
				continue;
			}
			CloseHandle(hFile);
			DiskCheck = true;
			break;
		}
	}

	/* Restore original drive.*/
   _chdrive( curdrive );

   return drive<27;
#endif
   return false;
}
