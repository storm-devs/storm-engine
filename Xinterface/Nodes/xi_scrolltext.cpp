#include <stdio.h>
#include "xi_scrolltext.h"

#define MAX_PICE_RESERV			256
#define MAX_PICE_STRING_SIZE	256

CXI_SCROLLTEXT::CXI_SCROLLTEXT()
{
    m_pScroller = NULL;
    m_nMaxStringes = 0;
	m_dwFontColor = ARGB(255,255,255,255);
	m_pText = NULL;

	m_nPiceQuantity = 0;
	m_pStrList = 0;
	m_nNodeType = NODETYPE_SCROLLTEXT;
}

CXI_SCROLLTEXT::~CXI_SCROLLTEXT()
{
	ReleaseAll();
}

void CXI_SCROLLTEXT::Draw(bool bSelected,dword Delta_Time)
{
	char param[MAX_PICE_STRING_SIZE];
	long curX=0L;
	long curY=0L;
	if(m_bUse)
	{
		for(int i=0; i<m_nPiceQuantity; i++)
		{
			switch(m_pStrList[i].type)
			{
			case PICE_TYPE_STRING:
				{
					long chStart = m_pStrList[i].data.strDescr.startOffset;
					long chQuant = m_pStrList[i].data.strDescr.charQuantity;
					if(chQuant>MAX_PICE_STRING_SIZE-1) chQuant=MAX_PICE_STRING_SIZE-1;
					strncpy(param,&m_pText[chStart],chQuant);
					param[chQuant] = 0;
					m_rs->Print(curX,curY,"%s",param);
				}
				break;
			}
		}
	}
}

bool CXI_SCROLLTEXT::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	SetText("proba string ^fnormal continue proba string one, two, three etc. Continue next stringes from full screen filling");
	SetGlowCursor(false);
	return true;
}

void CXI_SCROLLTEXT::ReleaseAll()
{
	ClearText();
}

void CXI_SCROLLTEXT::ClearText()
{
    m_pScroller = NULL;
    m_nMaxStringes = 0;
	m_dwFontColor = ARGB(255,255,255,255);
	PTR_DELETE(m_pText);

	m_nPiceQuantity = 0;
	PTR_DELETE(m_pStrList);
}

void CXI_SCROLLTEXT::SetText(char *newText)
{
	char param[256];
	STRING_PICE spl[MAX_PICE_RESERV];
	ClearText();

	if(newText!=NULL)
	{
		if( (m_pText=NEW char[strlen(newText)+1]) == NULL )
			_THROW("allocate memory error")
		strcpy(m_pText,newText);

		// получить все части из текста
		char *pCh = m_pText;
		m_idFont = FONT_DEFAULT;
		m_dwFontColor = ARGB(255,255,255,255);
		long idx=0;
		long nBeg=0;
		long nEnd=0;
		long startWidth = 0;
		while(*pCh != 0)
		{
			if(*pCh=='^')
			{
				// поместить уже заполненный текст в отдельный кусок
				if(nBeg!=nEnd)
					startWidth = FillPices(m_pText,nBeg,nEnd-nBeg,idx,spl, startWidth);

				idx++;
				if(nBeg==nEnd)
					nBeg+=3;
				nEnd+=2;
				pCh++;
				switch(*pCh)
				{
				case 'f':
					spl[idx].type = PICE_TYPE_FONTCHANGE;
					GetStringWord(&pCh[1],param,sizeof(param)-1);
					spl[idx].data.fontID = m_idFont = m_rs->LoadFont(param);
					break;
				case 'c':
					break;
				case 'a':
					break;
				}
				idx++;
			}
		}
	}
}

int CXI_SCROLLTEXT::CommandExecute(int wActCode)
{
	return -1;
}

bool CXI_SCROLLTEXT::IsClick(int buttonID,long xPos,long yPos)
{
	return false;
}

void CXI_SCROLLTEXT::ChangePosition( XYRECT &rNewPos )
{
}

void CXI_SCROLLTEXT::SaveParametersToIni()
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

void CXI_SCROLLTEXT::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
    m_pScroller = null;
    m_nMaxStringes = 0;
	m_dwFontColor = ARGB(255,255,255,255);
	m_pText = null;

	m_nPiceQuantity = 0;
	m_pStrList = 0;
}

void CXI_SCROLLTEXT::TextDownShift(float )
{
}

void CXI_SCROLLTEXT::TextUpShift()
{
}

long CXI_SCROLLTEXT::FillPices(char *pt, size_t beg, size_t size, long &idx, STRING_PICE * spl, long wid)
{
	long retVal = 0;

	char param[256];
	char resStr[512];
	long showWidth = m_rect.right-m_rect.left - wid;
	if(showWidth<0) showWidth = m_rect.right-m_rect.left;

	pt[size] = 0; // обозначим конец строки
	while(1)
	{
		char *pstr = &pt[beg];
		long chQuantity = 0;
		resStr[0] = 0;
		while(1)
		{
			long sw = GetStringWord(&pstr[chQuantity],param,sizeof(param)-1);
			if(sw==0) break;
			chQuantity += sw;
			strncpy(resStr,pstr,chQuantity);
			if( (retVal=m_rs->StringWidth(resStr,m_idFont))>showWidth ) { retVal=0; break; };
			if( (unsigned long)(chQuantity+sw) > size ) break;
		}
		if((unsigned long)chQuantity == size)
		{
		}
		else break;
		size -= chQuantity;
		spl[idx].type = PICE_TYPE_STRING;
		spl[idx].data.strDescr.startOffset = beg;
	}

	return 0;
}

long CXI_SCROLLTEXT::GetStringWord(char* pstr, char* buff, size_t size)
{
	unsigned long retVal = 0;

	bool bSpace=false;
	while(*pstr!=0)
	{
		if(*pstr=='^') break;
		if(retVal>=size) break;
		if(*pstr==32) bSpace=true;
		else if(bSpace) break;

		*(buff++) = *(pstr++);
		retVal++;
	}

	if(retVal>=size)
		buff[size-1] = 0;
	else
		buff[retVal] = 0;
	return retVal;
}
