#include <stdio.h>
#include "xi_statusline.h"
#include "..\\vxservice.h"

CXI_STATUSLINE::CXI_STATUSLINE()
{
	m_rs=NULL;
	m_sGroupName = NULL;
	m_idTex = -1L;
	m_vBuf = -1L;	m_iBuf = -1L;
	m_nVert = 0;	m_nIndx = 0;
	m_nNodeType = NODETYPE_STATUSLINE;
}

CXI_STATUSLINE::~CXI_STATUSLINE()
{
	ReleaseAll();
}

void CXI_STATUSLINE::Draw(bool bSelected,dword Delta_Time)
{
	if(m_bUse)
	{
		m_rs->TextureSet(0,m_idTex);
		m_rs->DrawBuffer(m_vBuf,sizeof(XI_ONLYONETEX_VERTEX),m_iBuf,0,m_nVert,0,m_nIndx,"iStatusLine");
	}
}

bool CXI_STATUSLINE::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	SetGlowCursor(false);
	return true;
}

void CXI_STATUSLINE::ReleaseAll()
{
	PICTURE_TEXTURE_RELEASE(pPictureService,m_sGroupName,m_idTex);
	VERTEX_BUF_RELEASE(m_rs,m_vBuf);
	INDEX_BUF_RELEASE(m_rs,m_iBuf);
	PTR_DELETE(m_sGroupName);
}

int CXI_STATUSLINE::CommandExecute(int wActCode)
{
	return -1;
}

void CXI_STATUSLINE::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	char param[256];

	// Get texture name and load that texture
	if( ReadIniString(ini1,name1, ini2,name2, "groupName", param, sizeof(param),"") )
	{
		m_sGroupName = NEW char[strlen(param)+1];
		strcpy(m_sGroupName,param);
		m_idTex = pPictureService->GetTextureID(m_sGroupName);
	}

	// Calculate vertex and index quantity
	m_nVert = 2*4;
	m_nIndx = 2*6;
	// Create vertex and index buffers
    m_vBuf = m_rs->CreateVertexBuffer(XI_ONLYONETEX_FVF,m_nVert*sizeof(XI_ONLYONETEX_VERTEX),D3DUSAGE_WRITEONLY);
    m_iBuf = m_rs->CreateIndexBuffer(m_nIndx*2);
	m_nIndx/=3;

	// Lock vertex and index buffers and get pointers to this
	XI_ONLYONETEX_VERTEX *pVBuf = (XI_ONLYONETEX_VERTEX*)m_rs->LockVertexBuffer(m_vBuf);
	WORD				 *pIBuf = (WORD*)m_rs->LockIndexBuffer(m_iBuf);

	if(pVBuf!=null && pIBuf!=null)
	{
		FXYRECT scrRect1,scrRect2;

        // get lenght of filled status line
        m_fLineOffset = GetIniFloat(ini1,name1, ini2,name2, "lineOffset", 0.f);
        float fMediumX = float(m_rect.right-m_rect.left)-m_fLineOffset*2.f;
        ATTRIBUTES *pAttr = _CORE_API->Entity_GetAttributeClass(&g_idInterface,"StatusLine");
        if(pAttr!=NULL)	pAttr = pAttr->GetAttributeClass(m_nodeName);
        if(pAttr!=NULL)
        {
            float fMaxValue = pAttr->GetAttributeAsFloat("Max",0);
            float fMinValue = pAttr->GetAttributeAsFloat("Min",0);
            float fCurValue = pAttr->GetAttributeAsFloat("Value",0);
            if(fMaxValue-fMinValue>0 && fCurValue>=fMinValue)
                fMediumX*=(fCurValue-fMinValue)/(fMaxValue-fMinValue);
        }
        fMediumX += m_fLineOffset;

        // get screen coordinates
        scrRect1.left = (float)m_rect.left;
        scrRect2.left = (float)m_rect.left + fMediumX;
        scrRect1.top = scrRect2.top = (float)m_rect.top;
        scrRect1.right = (float)m_rect.left + fMediumX;
        scrRect2.right = (float)m_rect.right;
        scrRect1.bottom = scrRect2.bottom = (float)m_rect.bottom;

        // get texture coordinates
        fMediumX /= (m_rect.right-m_rect.left);
		if( ReadIniString(ini1,name1, ini2,name2, "filledPicture", param, sizeof(param),"") )
			pPictureService->GetTexturePos(m_sGroupName,param,m_texRect1);
		FXYRECT texRect1; memcpy(&texRect1,&m_texRect1,sizeof(FRECT));
        texRect1.right = m_texRect1.left + (m_texRect1.right-m_texRect1.left)*fMediumX;
        // .. other ..
		if( ReadIniString(ini1,name1, ini2,name2, "emptyPicture", param, sizeof(param),"") )
			pPictureService->GetTexturePos(m_sGroupName,param,m_texRect2);
		FXYRECT texRect2; memcpy(&texRect2,&m_texRect2,sizeof(FRECT));
        texRect2.left = m_texRect2.left + (m_texRect2.right-m_texRect2.left)*fMediumX;

        // fill index buffer
        pIBuf[0] = 0;    pIBuf[4] = pIBuf[1] = 1;     pIBuf[3] = pIBuf[2] = 2;    pIBuf[5] = 3;  // filled rectangle
        pIBuf[6] = 4;    pIBuf[10] = pIBuf[7] = 5;    pIBuf[9] = pIBuf[8] = 6;    pIBuf[11] = 7; // empty rectangle

        // fill vertex buffer
        for(int i=0; i<m_nVert; i++)
            pVBuf[i].pos.z = 1.f;
        // screen and texture coordinates for filled rectangle
        pVBuf[0].pos.x = scrRect1.left;    pVBuf[0].pos.y = scrRect1.top;    pVBuf[0].tu = texRect1.left;    pVBuf[0].tv = texRect1.top;
        pVBuf[1].pos.x = scrRect1.right;   pVBuf[1].pos.y = scrRect1.top;    pVBuf[1].tu = texRect1.right;   pVBuf[1].tv = texRect1.top;
        pVBuf[2].pos.x = scrRect1.left;    pVBuf[2].pos.y = scrRect1.bottom; pVBuf[2].tu = texRect1.left;    pVBuf[2].tv = texRect1.bottom;
        pVBuf[3].pos.x = scrRect1.right;   pVBuf[3].pos.y = scrRect1.bottom; pVBuf[3].tu = texRect1.right;   pVBuf[3].tv = texRect1.bottom;
        // screen and texture coordinates for empty rectangle
        pVBuf[4].pos.x = scrRect2.left;    pVBuf[4].pos.y = scrRect2.top;    pVBuf[4].tu = texRect2.left;    pVBuf[4].tv = texRect2.top;
        pVBuf[5].pos.x = scrRect2.right;   pVBuf[5].pos.y = scrRect2.top;    pVBuf[5].tu = texRect2.right;   pVBuf[5].tv = texRect2.top;
        pVBuf[6].pos.x = scrRect2.left;    pVBuf[6].pos.y = scrRect2.bottom; pVBuf[6].tu = texRect2.left;    pVBuf[6].tv = texRect2.bottom;
        pVBuf[7].pos.x = scrRect2.right;   pVBuf[7].pos.y = scrRect2.bottom; pVBuf[7].tu = texRect2.right;   pVBuf[7].tv = texRect2.bottom;
	}
    else
        _THROW("Can't vertex or index buffer create");

	if(pVBuf!=null) m_rs->UnLockVertexBuffer(m_vBuf);
	if(pIBuf!=null) m_rs->UnLockIndexBuffer(m_iBuf);
}

bool CXI_STATUSLINE::IsClick(int buttonID,long xPos,long yPos)
{
	return false;
}

void CXI_STATUSLINE::ChangePosition( XYRECT &rNewPos )
{
	m_rect = rNewPos;
	Refresh();
}

void CXI_STATUSLINE::SaveParametersToIni()
{
	char pcWriteParam[2048];

	INIFILE * pIni = api->fio->OpenIniFile( (char*)ptrOwner->m_sDialogFileName.GetBuffer() );
	if( !pIni ) {
		api->Trace( "Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.GetBuffer() );
		return;
	}

	// save position
	_snprintf( pcWriteParam, sizeof(pcWriteParam), "%d,%d,%d,%d", m_rect.left, m_rect.top, m_rect.right, m_rect.bottom );
	pIni->WriteString( m_nodeName, "position", pcWriteParam );

	delete pIni;
}

dword _cdecl CXI_STATUSLINE::MessageProc(long msgcode, MESSAGE & message)
{
	switch(msgcode)
	{
	case 0:	Refresh(); break;
	}
	return 0;
}

void CXI_STATUSLINE::Refresh()
{
	if(m_vBuf==-1) return;
	XI_ONLYONETEX_VERTEX *pVBuf = (XI_ONLYONETEX_VERTEX*)m_rs->LockVertexBuffer(m_vBuf);

    ATTRIBUTES *pAttr = api->Entity_GetAttributeClass(&g_idInterface,"StatusLine");
    if(pAttr!=NULL)	pAttr = pAttr->GetAttributeClass(m_nodeName);
    if(pAttr!=NULL)
	{
		float fMediumX = float(m_rect.right-m_rect.left)-m_fLineOffset*2.f;
		float fMaxValue = pAttr->GetAttributeAsFloat("Max",0);
		float fMinValue = pAttr->GetAttributeAsFloat("Min",0);
		float fCurValue = pAttr->GetAttributeAsFloat("Value",0);
		if(fMaxValue>fMinValue && fCurValue>=fMinValue && fCurValue<=fMaxValue)
			fMediumX*=(fCurValue-fMinValue)/(fMaxValue-fMinValue);

		FXYRECT scrRect1,scrRect2;
		// get screen coordinates
		scrRect1.left = (float)m_rect.left;
		scrRect2.left = (float)m_rect.left + fMediumX;
		scrRect1.top = scrRect2.top = (float)m_rect.top;
		scrRect1.right = (float)m_rect.left + fMediumX;
		scrRect2.right = (float)m_rect.right;
		scrRect1.bottom = scrRect2.bottom = (float)m_rect.bottom;

		// get texture coordinates
		fMediumX /= (m_rect.right-m_rect.left);

		FRECT texRect1,texRect2;
		memcpy(&texRect1,&m_texRect1,sizeof(FRECT));
		texRect1.right = m_texRect1.left + (m_texRect1.right-m_texRect1.left)*fMediumX;
		memcpy(&texRect2,&m_texRect2,sizeof(FRECT));
		texRect2.left = m_texRect2.left + (m_texRect2.right-m_texRect2.left)*fMediumX;

		// screen and texture coordinates for filled rectangle
		pVBuf[0].pos.x = scrRect1.left;    pVBuf[0].pos.y = scrRect1.top;    pVBuf[0].tu = texRect1.left;    pVBuf[0].tv = texRect1.top;
		pVBuf[1].pos.x = scrRect1.right;   pVBuf[1].pos.y = scrRect1.top;    pVBuf[1].tu = texRect1.right;   pVBuf[1].tv = texRect1.top;
		pVBuf[2].pos.x = scrRect1.left;    pVBuf[2].pos.y = scrRect1.bottom; pVBuf[2].tu = texRect1.left;    pVBuf[2].tv = texRect1.bottom;
		pVBuf[3].pos.x = scrRect1.right;   pVBuf[3].pos.y = scrRect1.bottom; pVBuf[3].tu = texRect1.right;   pVBuf[3].tv = texRect1.bottom;
		// screen and texture coordinates for empty rectangle
		pVBuf[4].pos.x = scrRect2.left;    pVBuf[4].pos.y = scrRect2.top;    pVBuf[4].tu = texRect2.left;    pVBuf[4].tv = texRect2.top;
		pVBuf[5].pos.x = scrRect2.right;   pVBuf[5].pos.y = scrRect2.top;    pVBuf[5].tu = texRect2.right;   pVBuf[5].tv = texRect2.top;
		pVBuf[6].pos.x = scrRect2.left;    pVBuf[6].pos.y = scrRect2.bottom; pVBuf[6].tu = texRect2.left;    pVBuf[6].tv = texRect2.bottom;
		pVBuf[7].pos.x = scrRect2.right;   pVBuf[7].pos.y = scrRect2.bottom; pVBuf[7].tu = texRect2.right;   pVBuf[7].tv = texRect2.bottom;
	}

	if(pVBuf!=null) m_rs->UnLockVertexBuffer(m_vBuf);
}
