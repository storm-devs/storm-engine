#include <stdio.h>
#include "xi_questtexts.h"

CXI_QUESTTEXTS::STRING_DESCRIBER::STRING_DESCRIBER(char* ls)
{
	long sdStrSize = strlen(ls);
	if(sdStrSize==0)
		lineStr = NULL;
	else
	{
		if( (lineStr=NEW char[sdStrSize+1]) == NULL )
		{
			_THROW("allocate memory error");
		}
		strcpy(lineStr,ls);
	}
	complete = false;
	next = NULL;
	strNum = 0;
	prev = NULL;
}
CXI_QUESTTEXTS::STRING_DESCRIBER* CXI_QUESTTEXTS::STRING_DESCRIBER::Add(char* ls,bool complete)
{
	if(ls==NULL) return NULL;
	STRING_DESCRIBER* newSD = NEW STRING_DESCRIBER(ls);
	if(newSD==NULL)
	{
		_THROW("allocate memory error");
	}

	newSD->complete = complete;
	newSD->prev = this;
	newSD->next = next;
	next = newSD;
	newSD->strNum = strNum+1;

	return newSD;
}

bool GetNextIdFromList(char* &sptr, char* bufQuestID,size_t nSizeBufQuestID, char* buf,size_t bufSize, char* dataBuf)
{
	if(sptr==NULL) return false;
	char *sstart = sptr;
	long idSize=0;
	while(*sptr!=0)
	{
		if(*sptr==',')
		{
			sptr++;
			break;
		}
		sptr++;
		idSize++;
	}
	if(idSize==0) return false;
	if(sstart[0]=='@')
	{
		sstart++; idSize--;
		int n;
		if( sstart[0]<'0' || sstart[0]>'9' )
		{ // не дата
			n=0;
			while(sstart[0] && sstart[0]!='@')
			{
				if( n < (long)nSizeBufQuestID )
					bufQuestID[n++] = sstart[0];
				sstart++; idSize--;
			}
			bufQuestID[n] = '\0';
			sstart++; idSize--;
		} else {
			bufQuestID[0] = '\0';
		}

		// дата
		n=0;
		while(sstart[0] && sstart[0]!='@')
		{
			dataBuf[n] = sstart[0];
			sstart++; idSize--; n++;
		}
		dataBuf[n] = '\0';
		sstart++; idSize--;
	} else {
		dataBuf[0] = '\0';
		bufQuestID[0] = '\0';
	}
	if(idSize==0) return false;
	if((size_t)idSize>=bufSize) idSize=bufSize-1;
	strncpy(buf,sstart,idSize);
	buf[idSize]=0;
	return true;
}

static void SubRightWord(char* buf,int fontNum,int width,VDX8RENDER *rs)
{
	if(buf==NULL) return;
	long bufSize = strlen(buf);
	for(char* pEnd = buf+bufSize; pEnd>buf; pEnd--)
	{
		if(*pEnd==' ')
		{
			*pEnd=0;
			if( rs->StringWidth(buf,fontNum) <= width ) return;
		}
	}
}

void CXI_QUESTTEXTS::ReleaseStringes()
{
	while(m_listRoot!=NULL)
	{
		m_listCur = m_listRoot;
		m_listRoot = m_listRoot->next;
		PTR_DELETE(m_listCur->lineStr);
		PTR_DELETE(m_listCur);
	}
	m_listCur=NULL;
	m_nAllTextStrings=0;
}

bool CXI_QUESTTEXTS::GetLineNext(int fontNum,char* &pInStr,char* buf,int bufSize)
{
	if(pInStr==NULL || buf==NULL) return false;
	char *pStart = pInStr;
	bool bYesEOL=false;
	while(*pInStr!=0)
	{
		if(*pInStr==0x0D || *pInStr==0x0A) bYesEOL=true;
		else if(bYesEOL) break;
		pInStr++;
	}
	long lineSize = (long)pInStr-(long)pStart;
	if(lineSize==0) return false;
	if(lineSize>bufSize-1) lineSize=bufSize-1;

	strncpy(buf,pStart,lineSize);
	buf[lineSize]=0;
	long strWidth = m_rs->StringWidth(buf,fontNum);
	if( strWidth <= m_rect.right - m_rect.left ) return true;

	SubRightWord( buf, fontNum, m_rect.right-m_rect.left, m_rs );
	pInStr = pStart+strlen(buf);

	// удалим начальные пробелы
	while(*pInStr!=0 && (unsigned)*pInStr<=' ') pInStr++;

	return true;
}

CXI_QUESTTEXTS::CXI_QUESTTEXTS()
{
	m_nNodeType = NODETYPE_QTEXTS;

	m_idFont = -1;
	m_dwNonCompleteColor = ARGB(255,255,255,255);
	m_dwCompleteColor = ARGB(255,128,128,128);

	m_allStrings = 0;

	m_nAllTextStrings = 0;
	m_listCur = m_listRoot = NULL;
}

CXI_QUESTTEXTS::~CXI_QUESTTEXTS()
{
	ReleaseAll();
}

void CXI_QUESTTEXTS::Draw(bool bSelected,dword Delta_Time)
{
	if(!m_bUse) return;

	long curY = m_rect.top;
	int i=0;
	for(STRING_DESCRIBER* sd=m_listCur;sd!=NULL && i<m_allStrings;sd=sd->next,i++)
	{
		// отобразить строки
		DWORD curColor = sd->complete ? m_dwCompleteColor : m_dwNonCompleteColor;
		if(sd->lineStr!=NULL && sd->lineStr[0]!=0)
			m_rs->ExtPrint( m_idFont,curColor,0,ALIGN_LEFT,true,1.f,m_screenSize.x,m_screenSize.y,
				m_rect.left, curY, "%s",sd->lineStr);
		curY += m_vertOffset;
	}
}

bool CXI_QUESTTEXTS::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	SetGlowCursor(false);
	return true;
}

void CXI_QUESTTEXTS::ReleaseAll()
{
	FONT_RELEASE(m_rs,m_idFont);
	ReleaseStringes();
}

int CXI_QUESTTEXTS::CommandExecute(int wActCode)
{
	if(m_bUse)
	{
		if(m_listCur==NULL) return -1;
		switch(wActCode)
		{
		case ACTION_UPSTEP:
			if(m_listCur->prev!=NULL)
				m_listCur=m_listCur->prev;
			break;
		case ACTION_DOWNSTEP:
			if(m_listCur->next!=NULL)
				m_listCur=m_listCur->next;
			break;
		case ACTION_SPEEDUP:
			{
				for(int i=0;i<m_allStrings;i++)
					if(m_listCur->prev==NULL) break;
					else
						m_listCur=m_listCur->prev;
			}
			break;
		case ACTION_SPEEDDOWN:
			{
				for(int i=0;i<m_allStrings;i++)
					if(m_listCur->next==NULL) break;
					else
						m_listCur=m_listCur->next;
			}
			break;
		}
		api->Event("SetScrollerPos","sf",m_nodeName,(float)m_listCur->strNum/(float)m_nAllTextStrings);
	}
	return -1;
}

bool CXI_QUESTTEXTS::IsClick(int buttonID,long xPos,long yPos)
{
	return false;
}

void CXI_QUESTTEXTS::ChangePosition( XYRECT &rNewPos )
{
	m_rect = rNewPos;
}

void CXI_QUESTTEXTS::SaveParametersToIni()
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

void CXI_QUESTTEXTS::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	char param[255];

	// get line space
	m_vertOffset = GetIniLong(ini1,name1, ini2,name2, "lineSpace", 30);
	if(m_vertOffset==0) m_vertOffset=10;

	// подсчет количества выводимых строк на экране
	m_allStrings = (m_rect.bottom-m_rect.top)/m_vertOffset;

	// get colors
	m_dwCompleteColor = GetIniARGB(ini1,name1, ini2,name2, "completeColor", ARGB(255,128,128,128));
	m_dwNonCompleteColor = GetIniARGB(ini1,name1, ini2,name2, "noncompleteColor", ARGB(255,255,255,255));

	// get font
	m_idFont = -1;
	if( ReadIniString(ini1,name1, ini2,name2, "font", param, sizeof(param),"") )
		m_idFont = m_rs->LoadFont(param);
}

void CXI_QUESTTEXTS::StartQuestShow(ATTRIBUTES * pA,int qn)
{
	if(pA==NULL) return;
	long aq = pA->GetAttributesNum();
	if(qn<0) qn=0;
	if(qn>=aq) qn=aq-1;

	// удалим старые строки
	while(m_listRoot!=NULL)
	{
		m_listCur = m_listRoot;
		m_listRoot = m_listRoot->next;
		PTR_DELETE(m_listCur->lineStr);
		PTR_DELETE(m_listCur);
	}

	ATTRIBUTES *pAttr = pA->GetAttributeClass(qn);
	if(pAttr==NULL) return;

	bool cFlag = pAttr->GetAttributeAsDword("Complete",0)!=0;
	ATTRIBUTES* pATextList = pAttr->GetAttributeClass("Text");
	char* questLogName = pAttr->GetAttribute("LogName");
	if( !questLogName ) questLogName = pAttr->GetThisName();

	array<string> asStringList(_FL);
	if( ptrOwner->QuestFileReader() && pATextList )
	{
		long q = pATextList->GetAttributesNum();
		for( long n=q-1; n>=0; n-- )
		{
			ATTRIBUTES* pAttr = pATextList->GetAttributeClass(n);
			if( !pAttr ) continue;

			// space for aparting records
			if(m_listCur!=NULL)
				m_listCur = m_listCur->Add("",cFlag);

			// весь набор строк для одной записи
			asStringList.DelAll();
			GetStringListForQuestRecord( asStringList, pAttr->GetThisAttr(), pAttr->GetAttribute("UserData") );
			for( long i=0; i<asStringList; i++ )
			{
				// разложим полученную строку на строки влезающие в область вывода
				char* pcStrPtr = (char*)asStringList[i].GetBuffer();
				char newStr[512];
				while( GetLineNext(m_idFont,pcStrPtr,newStr,sizeof(newStr)) )
				{
					if(m_listCur==NULL)
					{
						if( (m_listCur=m_listRoot=NEW STRING_DESCRIBER(newStr)) == NULL )
						{
							_THROW("allocate memory error");
						}
						m_listCur->complete = cFlag;
					}
					else
						m_listCur = m_listCur->Add(newStr,cFlag);
				}
			}
		}
	}
	m_nAllTextStrings = m_listCur==NULL ? 0 : m_listCur->strNum;
	m_listCur = m_listRoot;
	api->Event("SetScrollerPos","sf",m_nodeName,0.f);
}

float CXI_QUESTTEXTS::GetLineStep()
{
	if( m_nAllTextStrings<1 ) return 0.f;
	return 1.f / (float)(m_nAllTextStrings);
}

void CXI_QUESTTEXTS::ScrollerChanged(float fPos)
{
	if(m_listRoot==null || m_nAllTextStrings<=0) return;
	long topStrNum = long(fPos*m_nAllTextStrings);
	if(topStrNum>m_nAllTextStrings)	topStrNum = m_nAllTextStrings;
	if(topStrNum<0) topStrNum=0;
	m_listCur = m_listRoot;
	for(int i=0; i<topStrNum; i++)
		if(m_listCur->next!=null)
			m_listCur = m_listCur->next;
}

dword _cdecl CXI_QUESTTEXTS::MessageProc(long msgcode, MESSAGE & message)
{
	switch(msgcode)
	{
	case 0: // установить верхнюю строку показа в определенный номер
		{
			long topStrNum = message.Long();
			if(m_listRoot==null || m_nAllTextStrings<=0) return 0;
			if(topStrNum>=m_nAllTextStrings)	topStrNum = m_nAllTextStrings-1;
			if(topStrNum<0) topStrNum=0;
			m_listCur = m_listRoot;
			for(int i=0; i<topStrNum; i++)
				if(m_listCur!=null)	m_listCur = m_listCur->next;
			return 0;
		}
		break;

	case 1: // установить верхнюю строку показа на относительный номер
		{
			ScrollerChanged( message.Float() );
			return 0;
		}
		break;
	}
	return -1;
}

void CXI_QUESTTEXTS::GetStringListForQuestRecord( array<string> & asStringList, const char* pcRecText, const char* pcUserData )
{
	char pcQuestID[1024];
	char pcTextID[1024];
	char pcDate[256];

	char* pcTmp = (char*)pcRecText;

	if( GetNextIdFromList(pcTmp, pcQuestID,sizeof(pcQuestID), pcTextID,sizeof(pcTextID)-1, pcDate) )
	{
		asStringList.DelAll();
		if( pcDate[0] ) asStringList.Add(pcDate);
		ptrOwner->QuestFileReader()->GetRecordTextList(asStringList, pcQuestID,pcTextID, pcUserData);
	}
}
