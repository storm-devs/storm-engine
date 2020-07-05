#include <stdio.h>
#include "xi_strcollection.h"
#include "..\\defines.h"

CXI_STRCOLLECTION::CXI_STRCOLLECTION()
{
	m_rs=null;

	m_pStrDescr=null;
	m_nStr=0;
	m_nNodeType = NODETYPE_STRINGCOLLECTION;

	m_nEditIndex = -1;
}

CXI_STRCOLLECTION::~CXI_STRCOLLECTION()
{
	ReleaseAll();
}

int CXI_STRCOLLECTION::CommandExecute(int wActCode)
{
	return -1;
}

void CXI_STRCOLLECTION::Draw(bool bSelected,dword Delta_Time)
{
	if(m_bUse)
	{
		for(int i=0; i<m_nStr; i++)
			if(m_pStrDescr[i].strStr!=null)
				m_rs->ExtPrint( m_pStrDescr[i].nFontNum, m_pStrDescr[i].foreColor,
				m_pStrDescr[i].backColor, m_pStrDescr[i].wAlignment, m_pStrDescr[i].bShadow, m_pStrDescr[i].fScale,
				m_screenSize.x,m_screenSize.y, m_pStrDescr[i].scrPos.x, m_pStrDescr[i].scrPos.y, "%s",
				m_pStrDescr[i].strStr );
			else
				m_rs->ExtPrint( m_pStrDescr[i].nFontNum, m_pStrDescr[i].foreColor,
				m_pStrDescr[i].backColor, m_pStrDescr[i].wAlignment, m_pStrDescr[i].bShadow, m_pStrDescr[i].fScale,
				m_screenSize.x,m_screenSize.y, m_pStrDescr[i].scrPos.x, m_pStrDescr[i].scrPos.y, "%s",
				pStringService->GetString(m_pStrDescr[i].strNum) );
	}
}

bool CXI_STRCOLLECTION::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	// screen position for that is host screen position
	memcpy(&m_rect,&m_hostRect,sizeof(m_hostRect));
	SetGlowCursor(false);
	return true;
}

void CXI_STRCOLLECTION::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	int i;
	char param[256];

	bool bRelativeRect = !GetIniLong(ini1,name1, ini2,name2, "bAbsoluteRectangle", 0);

	INIFILE *ini = ini1;
	char* name = name1;
	if( !ini1 ) {
		ini = ini2;
		name = name2;
	}

	// get string quantity
	m_nStr=0;
	if( ini && ini->ReadString(name,"string",param,sizeof(param)-1,"") )
		do m_nStr++; while(ini->ReadStringNext(name,"string",param,sizeof(param)-1));

	if(m_nStr)
	{
		m_pStrDescr = NEW STRINGDESCR[m_nStr];

		// Set strings
		int a_fc,r_fc,g_fc,b_fc;
		int a_bc,r_bc,g_bc,b_bc;
		char strName[sizeof(param)];
		char strState[sizeof(param)];
		char fontName[sizeof(param)];
		ini->ReadString(name,"string",param,sizeof(param)-1,"");
		for(i=0; i<m_nStr; i++)
		{
			// set all parameters to zero
			ZeroMemory(&m_pStrDescr[i],sizeof(STRINGDESCR));
			m_pStrDescr[i].nFontNum = -1;

			// read same parameters
			strState[0] = '\0';
			sscanf(param,"%[^,],font:%[^,],pos:{%d,%d},fc:{%d,%d,%d,%d},bc:{%d,%d,%d,%d},scale:%f,state:{%[^}]}",
				strName, fontName, &m_pStrDescr[i].scrPos.x, &m_pStrDescr[i].scrPos.y,
				&a_fc, &r_fc, &g_fc, &b_fc, &a_bc, &r_bc, &g_bc, &b_bc,
				&m_pStrDescr[i].fScale, strState);
			DublicateString( m_pStrDescr[i].sFontName, fontName );
			m_pStrDescr[i].nFontNum = m_rs->LoadFont(fontName);
            if(bRelativeRect)
            {
                m_pStrDescr[i].scrPos.x += m_hostRect.left;
                m_pStrDescr[i].scrPos.y += m_hostRect.top;
            }

			// set foreground & background colors
			m_pStrDescr[i].foreColor = ARGB(a_fc,r_fc,g_fc,b_fc);
			m_pStrDescr[i].backColor = ARGB(a_bc,r_bc,g_bc,b_bc);

			// set states
			for(int k=strlen(strState); k>=0; k--)
			{
				if(strState[k]=='C' || strState[k]=='c')
					m_pStrDescr[i].wAlignment = ALIGN_CENTER;
				if(strState[k]=='R' || strState[k]=='c')
					m_pStrDescr[i].wAlignment = ALIGN_RIGHT;
				if(strState[k]=='S' || strState[k]=='s')
					m_pStrDescr[i].bShadow=true;
			}

			m_pStrDescr[i].strNum = -1;
			if( strName[0]=='#' )
				DublicateString( m_pStrDescr[i].strStr, &strName[1] );
			else
				m_pStrDescr[i].strNum = pStringService->GetStringNum(strName);

			ini->ReadStringNext(name,"string",param,sizeof(param)-1);
		}
	}
}

void CXI_STRCOLLECTION::ReleaseAll()
{
	for(int i=0; i<m_nStr; i++)
	{
		PTR_DELETE(m_pStrDescr[i].strID);
		PTR_DELETE(m_pStrDescr[i].strStr);
		PTR_DELETE(m_pStrDescr[i].sFontName);
		FONT_RELEASE(m_rs,m_pStrDescr[i].nFontNum);
	}
	PTR_DELETE(m_pStrDescr);
	m_nStr = 0;
}

bool CXI_STRCOLLECTION::IsClick(int buttonID,long xPos,long yPos)
{
	return false;
}

bool CXI_STRCOLLECTION::GetInternalNameList( array<string>& aStr )
{
	aStr.DelAll();
	aStr.Add();
	aStr[0] = "All";
	for(long n=0; n<m_nStr; n++)
	{
		if( m_pStrDescr[n].strID ) continue;
		aStr.Add();
		char param[512];
		_snprintf( param,sizeof(param), "%d - %s", n+1, pStringService->GetStringName( m_pStrDescr[n].strNum ) );
		aStr[n+1] = param;
	}
	return aStr.Size() > 1;
}

void CXI_STRCOLLECTION::SetInternalName( string& sName )
{
	if( sName.IsEmpty() ) return;
	if( sName == "All" ) m_nEditIndex = -1;
	else {
		sscanf( sName, "%d -", &m_nEditIndex );
		m_nEditIndex--;
	}
}

void CXI_STRCOLLECTION::ChangePosition( XYRECT &rNewPos )
{
	long nX = rNewPos.left - m_rect.left;
	long nY = rNewPos.top - m_rect.top;
	if( nX != 0 || nY != 0 )
	{
		if( m_nEditIndex>=0 && m_nEditIndex<m_nStr )
		{
			m_pStrDescr[m_nEditIndex].scrPos.x += nX;
			m_pStrDescr[m_nEditIndex].scrPos.y += nY;
			return;
		}
		for( long n=0; n<m_nStr; n++ )
		{
			m_pStrDescr[n].scrPos.x += nX;
			m_pStrDescr[n].scrPos.y += nY;
		}
	}
}

void CXI_STRCOLLECTION::SaveParametersToIni()
{
//	char pcWriteParam[2048];

	INIFILE * pIni = api->fio->OpenIniFile( (char*)ptrOwner->m_sDialogFileName.GetBuffer() );
	if( !pIni ) {
		api->Trace( "Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.GetBuffer() );
		return;
	}

	pIni->DeleteSection( m_nodeName );
	// save position
	char param[2048];
	for( long n=0; n<m_nStr; n++ )
	{
		if( m_pStrDescr[n].strID ) continue; // динамически добавленныя строка

		char pcState[3];
		switch( m_pStrDescr[n].wAlignment )
		{
		case ALIGN_CENTER: pcState[0]='C'; break;
		case ALIGN_RIGHT: pcState[0]='R'; break;
		default: pcState[0]='L';
		}
		if( m_pStrDescr[n].bShadow ) {
			pcState[1] = 'S';
			pcState[2] = 0;
		} else pcState[1] = 0;


		if( m_pStrDescr[n].strStr )
			_snprintf( param, sizeof(param), "#%s,font:%s,pos:{%d,%d},fc:{%d,%d,%d,%d},bc:{%d,%d,%d,%d},scale:%.2f,state:{%s}",
				m_pStrDescr[n].strStr, // strID
				m_pStrDescr[n].sFontName, // font name
				m_pStrDescr[n].scrPos.x, m_pStrDescr[n].scrPos.y, // pos
				ALPHA(m_pStrDescr[n].foreColor), RED(m_pStrDescr[n].foreColor), GREEN(m_pStrDescr[n].foreColor), BLUE(m_pStrDescr[n].foreColor),
				ALPHA(m_pStrDescr[n].backColor), RED(m_pStrDescr[n].backColor), GREEN(m_pStrDescr[n].backColor), BLUE(m_pStrDescr[n].backColor),
				m_pStrDescr[n].fScale,
				pcState	);
		else
			_snprintf( param, sizeof(param), "%s,font:%s,pos:{%d,%d},fc:{%d,%d,%d,%d},bc:{%d,%d,%d,%d},scale:%.2f,state:{%s}",
				pStringService->GetStringName( m_pStrDescr[n].strNum ), // strID
				m_pStrDescr[n].sFontName, // font name
				m_pStrDescr[n].scrPos.x, m_pStrDescr[n].scrPos.y, // pos
				ALPHA(m_pStrDescr[n].foreColor), RED(m_pStrDescr[n].foreColor), GREEN(m_pStrDescr[n].foreColor), BLUE(m_pStrDescr[n].foreColor),
				ALPHA(m_pStrDescr[n].backColor), RED(m_pStrDescr[n].backColor), GREEN(m_pStrDescr[n].backColor), BLUE(m_pStrDescr[n].backColor),
				m_pStrDescr[n].fScale,
				pcState	);
		pIni->AddString( m_nodeName, "string", param );
	}

	delete pIni;
}

dword _cdecl CXI_STRCOLLECTION::MessageProc(long msgcode, MESSAGE & message)
{
	switch(msgcode)
	{
	case 0:	// Add dinamic string
		{
			// string ID & data
			char paramID[256],paramStr[512];
			message.String(sizeof(paramID),paramID);	// msg
			message.String(sizeof(paramStr),paramStr);	// msg
			STRINGDESCR * pstr = CreateNewDinamicString(paramID,paramStr);
			if(pstr==null) return -1;
			// string font
			char fontName[256];
			message.String(sizeof(fontName),fontName);	// msg
			pstr->nFontNum = m_rs->LoadFont(fontName);
			// string pos.x&y, color front&back, alignment, shadow, scale
			pstr->scrPos.x = message.Long() + m_hostRect.left;	// msg
			pstr->scrPos.y = message.Long() + m_hostRect.top;	// msg
			pstr->foreColor = message.Long();	// msg
			pstr->backColor = message.Long();	// msg
			switch(message.Long())				// msg
			{
			case SCRIPT_ALIGN_RIGHT:
				pstr->wAlignment = ALIGN_RIGHT;
			break;
			case SCRIPT_ALIGN_CENTER:
				pstr->wAlignment = ALIGN_CENTER;
			break;
			case SCRIPT_ALIGN_LEFT:
				pstr->wAlignment = ALIGN_LEFT;
			break;
			}
			pstr->bShadow = message.Long()!=0;	// msg
			pstr->fScale = message.Float();		// msg
			// обязательная ширина
			int nWidth = message.Long();		// msg
			if(nWidth>0)
			{
				int realWidth = m_rs->StringWidth(paramStr,pstr->nFontNum,pstr->fScale);
				if(realWidth>nWidth)	pstr->fScale *= (float)(nWidth-1)/realWidth;
			}
		}
		break;
	case 1: // изменить строку по номеру
		{
			long n = message.Long() - 1;
			char param[512];
			message.String( sizeof(param), param );
			ChangeString(n,param);
		}
		break;
	case 2: // скопировать строку с одного места на другое (первая строка имеет номер 1)
		{
			long nDst = message.Long()-1;
			long nSrc = message.Long()-1;
			if( nDst>=0 && nSrc>=0 &&
				nDst<m_nStr && nSrc<m_nStr )
			{
				m_pStrDescr[nDst].strNum = -1;
				DELETE( m_pStrDescr[nDst].strStr );
				if( m_pStrDescr[nSrc].strStr )
					DublicateString( m_pStrDescr[nDst].strStr, m_pStrDescr[nSrc].strStr );
				else
					m_pStrDescr[nDst].strNum = m_pStrDescr[nSrc].strNum;
			}
		}
		break;
	case 3: // поменять цвет строки
		{
			long nStr = message.Long()-1;
			dword nColor = message.Long();
			ChangeStringColor(nStr,nColor);
		}
		break;
	}
	return -1;
}

void CXI_STRCOLLECTION::ChangeString(long num, const char* sValue)
{
	if( num>=0 && num<m_nStr )
	{
		m_pStrDescr[num].strNum = -1;
		DELETE( m_pStrDescr[num].strStr );
		if( sValue ) {
			if( sValue[0]=='#' )
				DublicateString( m_pStrDescr[num].strStr, &sValue[1] );
			else
				m_pStrDescr[num].strNum = pStringService->GetStringNum( sValue );
		}
	}
}

void CXI_STRCOLLECTION::ChangeStringColor(long num, dword dwColor)
{
	if( num>=0 && num<m_nStr )
		m_pStrDescr[num].foreColor = dwColor;
}

void CXI_STRCOLLECTION::ChangeStringPos(long num, XYPOINT& pntNewPos)
{
	if( num>=0 && num<m_nStr )
		m_pStrDescr[num].scrPos = pntNewPos;
}

CXI_STRCOLLECTION::STRINGDESCR * CXI_STRCOLLECTION::CreateNewDinamicString(char * strID, char * strStr)
{
	if(strID==null || strID[0]==0) return null;
	for(int i=0; i<m_nStr; i++)
	{
		if(m_pStrDescr[i].strID!=null && stricmp(m_pStrDescr[i].strID,strID)==0)
			break;
	}
	if(i<m_nStr)
	{
		if(strStr==null || strStr[0]==0)
		{
			PTR_DELETE(m_pStrDescr[i].strID);
			PTR_DELETE(m_pStrDescr[i].strStr);
			FONT_RELEASE(m_rs,m_pStrDescr[i].nFontNum);
			m_nStr--;
			if(m_nStr>i)
				memcpy(&m_pStrDescr[i],&m_pStrDescr[i+1],sizeof(STRINGDESCR)*(m_nStr-i));
			return null;
		}
		FONT_RELEASE(m_rs,m_pStrDescr[i].nFontNum);
		PTR_DELETE(m_pStrDescr[i].strStr);
		m_pStrDescr[i].strStr = NEW char[strlen(strStr)+1];
		if(m_pStrDescr[i].strStr==null)	{THROW("allocate memory error");}
		strcpy(m_pStrDescr[i].strStr,strStr);
		return &m_pStrDescr[i];
	}
	if( strStr==null || strStr[0]==0 ) return null;
	STRINGDESCR * pOld = m_pStrDescr;
	m_nStr++;
	m_pStrDescr = NEW STRINGDESCR[m_nStr];
	if( m_pStrDescr == null )	{THROW("allocate memory error");}
	if( pOld && i )	memcpy(m_pStrDescr,pOld,sizeof(STRINGDESCR)*i);
	if( pOld ) delete pOld;
	ZeroMemory( &m_pStrDescr[i], sizeof(STRINGDESCR) );
	m_pStrDescr[i].nFontNum = -1;
	m_pStrDescr[i].strID = NEW char[strlen(strID)+1];
	m_pStrDescr[i].strStr = NEW char[strlen(strStr)+1];
	if( m_pStrDescr[i].strID==null || m_pStrDescr[i].strStr==null )
	{	THROW("allocate memory error");}
	strcpy( m_pStrDescr[i].strID, strID );
	strcpy( m_pStrDescr[i].strStr, strStr );
	return &m_pStrDescr[i];
}
