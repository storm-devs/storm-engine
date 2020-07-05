#include <stdio.h>
#include "xi_questtitles.h"

void SubRightWord(char* buf,int fontNum,int width,VDX8RENDER *rs)
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

bool CXI_QUESTTITLE::GetLineNext(int fontNum,char* &pInStr,char* buf,int bufSize)
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

	strncpy(buf,pStart,lineSize);
	buf[lineSize]=0;
	long strWidth = m_rs->StringWidth(buf,fontNum);
	long needWidth = m_rect.right-m_rect.left-m_iconWidth;
	if(strWidth<=needWidth) return true;

	SubRightWord(buf,fontNum,needWidth,m_rs);
	pInStr = pStart+strlen(buf);

	// удалим начальные пробелы
	while(*pInStr!=0 && (unsigned)*pInStr<=' ') pInStr++;

	return true;
}

CXI_QUESTTITLE::CXI_QUESTTITLE()
{
	m_nNodeType = NODETYPE_QTITLE;

	m_idFont = -1;
	m_dwNonCompleteColor = ARGB(255,255,255,255);
	m_dwCompleteColor = ARGB(255,128,128,128);
	m_dwSelectRectangleColor = ARGB(255,0,128,128);

	m_stringQuantity = 0;
	m_allStrings = 0;

	m_strList = NULL;
	m_curIdx=0;
	m_selectOffset = 8;
	m_fontOffset = 4;
	m_bSelected = true;
	m_iconGroupName = null;

	m_nCommonQuantity = 0;
}

CXI_QUESTTITLE::~CXI_QUESTTITLE()
{
	ReleaseAll();
}

void CXI_QUESTTITLE::Draw(bool bSelected,dword Delta_Time)
{
	int i,j;
	if(!m_bUse) return;

	long curY = m_rect.top;
	int lineNum = 0;
	for(i=0; i<m_stringQuantity && lineNum<m_allStrings; i++)
	{
		// отобразить выделение
		if(bSelected && i==m_curIdx)
		{
			XI_NOTEX_VERTEX selV[4];
			for(j=0;j<4;j++)
			{
				selV[j].pos.z = 1.f;
				selV[j].color = m_dwSelectRectangleColor;
			}
			selV[0].pos.x = selV[1].pos.x = (float)m_rect.left-m_selectOffset;
			selV[2].pos.x = selV[3].pos.x = (float)m_rect.right;
			selV[0].pos.y = selV[2].pos.y = (float)curY;
			selV[1].pos.y = selV[3].pos.y = (float)curY + m_strList[i].lineQuantity*m_vertOffset;
			m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_NOTEX_FVF,2,selV,sizeof(XI_NOTEX_VERTEX),"iRectangle");
		}

		// отобразить иконку выполнения
		XI_ONLYONETEX_VERTEX v[4];
		v[0].pos.z = v[1].pos.z = v[2].pos.z = v[3].pos.z = 1.f;
		v[0].pos.x = v[1].pos.x = (float)m_rect.right-m_iconWidth;
		v[2].pos.x = v[3].pos.x = (float)m_rect.right;
		v[0].pos.y = v[2].pos.y = (float)curY+m_iconVOffset;
		v[1].pos.y = v[3].pos.y = (float)curY+m_iconVOffset+m_iconHeight;
		if(m_strList[i].complete)
		{
			v[0].tu = v[1].tu = m_texComplete.left;
			v[2].tu = v[3].tu = m_texComplete.right;
			v[0].tv = v[2].tv = m_texComplete.top;
			v[1].tv = v[3].tv = m_texComplete.bottom;
		}
		else
		{
			v[0].tu = v[1].tu = m_texNonComplete.left;
			v[2].tu = v[3].tu = m_texNonComplete.right;
			v[0].tv = v[2].tv = m_texNonComplete.top;
			v[1].tv = v[3].tv = m_texNonComplete.bottom;
		}
		m_rs->TextureSet(0,m_texId);
		m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONLYONETEX_FVF,2,v,sizeof(XI_ONLYONETEX_VERTEX),"iDinamicPictures");

		// отобразить строки
		DWORD curColor = m_strList[i].complete ? m_dwCompleteColor : m_dwNonCompleteColor;
		if( m_strList[i].dwSpecColor!=0 ) curColor = m_strList[i].dwSpecColor;
		for(j=0;j<m_strList[i].lineQuantity;j++)
		{
			m_rs->ExtPrint(m_idFont,curColor,0,ALIGN_LEFT,true,1.f,m_screenSize.x,m_screenSize.y,
				m_rect.left, curY+m_fontOffset, "%s",m_strList[i].name[j]);
			curY += m_vertOffset;
			lineNum++;
			if(lineNum>=m_allStrings) break;
		}
	}
}

bool CXI_QUESTTITLE::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	SetGlowCursor(false);
	return true;
}

void CXI_QUESTTITLE::ReleaseAll()
{
	FONT_RELEASE(m_rs,m_idFont);
	if(m_strList!=NULL)
		for(int i=0;i<m_stringQuantity;i++)
			for(int j=0;j<m_strList[i].lineQuantity;j++)
				PTR_DELETE(m_strList[i].name[j]);
	PTR_DELETE(m_strList);
	PTR_DELETE(m_iconGroupName);
	m_stringQuantity = 0;
}

int CXI_QUESTTITLE::CommandExecute(int wActCode)
{
	if(m_bUse)
	{
		switch(wActCode)
		{
		case ACTION_UPSTEP:
			if(m_curIdx>0) m_curIdx--;
			else api->Event("QuestTopChange","l",-1);
		break;
		case ACTION_DOWNSTEP:
			if(m_curIdx<m_stringQuantity-1) m_curIdx++;
			else api->Event("QuestTopChange","l",1);
		break;
		case ACTION_SPEEDUP:
			api->Event("QuestTopChange","l",-m_allStrings);
		break;
		case ACTION_SPEEDDOWN:
			api->Event("QuestTopChange","l",m_allStrings);
		break;
		case ACTION_MOUSECLICK:
		case ACTION_ACTIVATE:
			api->Event("QuestActivate","l",m_curIdx);
		break;
		}
	}
	return -1;
}

bool CXI_QUESTTITLE::IsClick(int buttonID,long xPos,long yPos)
{
	if(!m_bUse) return false;
	if(xPos<(m_rect.left-m_selectOffset) || xPos>m_rect.right) return false;

	long top,bottom;
	top = m_rect.top;
	for(int i=0;i<m_stringQuantity;i++)
	{
		bottom = top + m_strList[i].lineQuantity*m_vertOffset;
		if(i==m_curIdx)
			if(yPos>=top && yPos<=bottom) return true;
			else return false;
		top = bottom;
	}
	return false;
}

void CXI_QUESTTITLE::ChangePosition( XYRECT &rNewPos )
{
	m_rect = rNewPos;
}

void CXI_QUESTTITLE::SaveParametersToIni()
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

void CXI_QUESTTITLE::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	char param[255];

	//
	m_selectOffset = GetIniLong(ini1,name1, ini2,name2, "selectOffset", 8);

	// get line space
	m_vertOffset = GetIniLong(ini1,name1, ini2,name2, "lineSpace", 30);
	if(m_vertOffset==0) m_vertOffset=10;

	// подсчет количества выводимых строк на экране
	m_allStrings = (m_rect.bottom-m_rect.top)/m_vertOffset;

	// get golors
	m_dwCompleteColor = GetIniARGB(ini1,name1, ini2,name2, "completeColor", ARGB(255,128,128,128));
	m_dwNonCompleteColor = GetIniARGB(ini1,name1, ini2,name2, "noncompleteColor", ARGB(255,255,255,255));
	m_dwSelectRectangleColor = GetIniARGB(ini1,name1, ini2,name2, "selectColor", ARGB(255,255,255,255));

	// get font
	m_idFont = -1;
	if( ReadIniString(ini1,name1, ini2,name2, "font", param, sizeof(param),"") )
		m_idFont = m_rs->LoadFont(param);
	m_fontOffset = GetIniLong(ini1,name1, ini2,name2, "fontOffset", 4);

	// get image info
	m_iconWidth = GetIniLong(ini1,name1, ini2,name2, "iconWidth", 32);
	m_iconHeight = GetIniLong(ini1,name1, ini2,name2, "iconHeight", 32);
	m_iconVOffset = m_vertOffset/2 - m_iconHeight/2;
	if( ReadIniString(ini1,name1, ini2,name2, "iconGroup", param, sizeof(param),"") )
	{
		m_iconGroupName = NEW char[strlen(param)+1];
		if(m_iconGroupName==null)
			{_THROW("allocate memory error");}
		strcpy(m_iconGroupName,param);
	}
	else	m_iconGroupName = null;
	m_texId = ptrOwner->PictureService()->GetTextureID(m_iconGroupName);

	if( ReadIniString(ini1,name1, ini2,name2, "completeIcon", param, sizeof(param),"") )
	{	ptrOwner->PictureService()->GetTexturePos(m_iconGroupName,param,m_texComplete);
	}
	else
	{
		m_texComplete.left = m_texComplete.top = 0.f;
		m_texComplete.right = m_texComplete.bottom = 1.f;
	}

	if( ReadIniString(ini1,name1, ini2,name2, "noncompleteIcon", param, sizeof(param),"") )
	{	ptrOwner->PictureService()->GetTexturePos(m_iconGroupName,param,m_texNonComplete);
	}
	else
	{
		m_texNonComplete.left = m_texNonComplete.top = 0.f;
		m_texNonComplete.right = m_texNonComplete.bottom = 1.f;
	}
}

void CXI_QUESTTITLE::SetNewTopQuest(ATTRIBUTES * pA,int topNum)
{
	int i;
	m_nCommonQuantity = 0;
	// удалим старые строки
	if(m_strList!=NULL)
	{
		for(i=0;i<m_stringQuantity;i++)
			for(int j=0;j<m_strList[i].lineQuantity;j++)
				PTR_DELETE(m_strList[i].name[j]);
		PTR_DELETE(m_strList);
		m_stringQuantity = 0;
	} // boal перенес наверх, иначе не трется, если квестов нет, а были уже
	
	if(pA==NULL) return;
	long aq = pA->GetAttributesNum();
	if(topNum<0 || topNum>=aq)
	{
		api->Trace("quest number out of range");
		return;
	}
	m_nCommonQuantity = aq;



	char param[256];
	//if(ptrOwner->QuestFileReader())
	{
		//ptrOwner->QuestFileReader()->InitQuestsQuery();

		// расчет количества выводимых строк
		m_stringQuantity = aq-topNum;
		if(m_stringQuantity>m_allStrings)
			m_stringQuantity=m_allStrings;
		if(m_curIdx>=m_stringQuantity) m_curIdx=m_stringQuantity-1;

		// создание массива строк
		if(m_stringQuantity<=0) return;
		if( (m_strList=NEW STRING_DESCRIBER[m_stringQuantity]) == NULL )
		{
			_THROW("allocate memory error");
		}
		// и заполнение этих строк
		int lineNum = 0;
		for(i=0; i<m_stringQuantity; i++)
		{
			m_strList[i].lineQuantity = 0;
			m_strList[i].dwSpecColor = 0;
			ATTRIBUTES * pAttr = pA->GetAttributeClass(topNum+i);
			if(pAttr==NULL)
			{
				m_strList[i].complete = false;
				m_strList[i].lineQuantity = 0;
				continue;
			}
			m_strList[i].dwSpecColor = pAttr->GetAttributeAsDword("color",0);
			m_strList[i].complete = pAttr->GetAttributeAsDword("Complete",0)!=0;
			char * pTmpQuestRecordID = pAttr->GetAttribute("LogName");
			if( !pTmpQuestRecordID ) pTmpQuestRecordID = pAttr->GetThisName();
			if( ptrOwner->QuestFileReader()->GetQuestTitle(pTmpQuestRecordID,pAttr->GetThisName(),sizeof(param)-1,param) )
			{
				int titleSize = strlen(param);
				if(titleSize==0)	m_strList[i].lineQuantity=0;
				else
				{
					char lineName[sizeof(param)];
					char *pstr = param;
					int ln = 0;
					while( GetLineNext(m_idFont,pstr,lineName,sizeof(lineName)) )
					{
						m_strList[i].name[ln] = NEW char[strlen(lineName)+1];
						strcpy(m_strList[i].name[ln],lineName);
						ln++;
					}
					m_strList[i].lineQuantity = ln;
					lineNum += ln;
					if(lineNum>=m_allStrings) m_stringQuantity = i+1;
				}
			}
		}

		//ptrOwner->QuestFileReader()->CloseQuestsQuery();
	}
}

float CXI_QUESTTITLE::GetLineStep()
{
	if( m_nCommonQuantity<=0 ) return 0.f;
	return 1.f / (float)m_nCommonQuantity;
}

void CXI_QUESTTITLE::ScrollerChanged( float fPos )
{
}

void CXI_QUESTTITLE::MouseThis(float fX, float fY)
{
	float left = (float)m_rect.left-m_selectOffset;
	float right = (float)m_rect.right;
	if(fX<left || fX>right) return;

	float top,bottom;
	top = (float)m_rect.top;
	for(int i=0;i<m_stringQuantity;i++)
	{
		bottom = top + m_strList[i].lineQuantity*m_vertOffset;
		if(fY>=top && fY<=bottom)
		{
			if(i!=m_curIdx) m_curIdx=i;
			break;
		}
		top = bottom;
	}
}
