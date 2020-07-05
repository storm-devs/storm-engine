#include <stdio.h>
#include "xi_linecollection.h"
#include "..\\defines.h"

CXI_LINECOLLECTION::CXI_LINECOLLECTION() :
	m_aLines(_FL)
{
	m_rs = NULL;
	m_nNodeType = NODETYPE_LINECOLLECTION;
}

CXI_LINECOLLECTION::~CXI_LINECOLLECTION()
{
	ReleaseAll();
}

int CXI_LINECOLLECTION::CommandExecute(int wActCode)
{
	return -1;
}

void CXI_LINECOLLECTION::Draw(bool bSelected,dword Delta_Time)
{
	if(m_bUse)
	{
        m_rs->DrawLines(m_aLines.GetBuffer(),m_aLines.Size()/2,"iLineCollection");
	}
}

bool CXI_LINECOLLECTION::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	// screen position for that is host screen position
	memcpy(&m_rect,&m_hostRect,sizeof(m_hostRect));
	SetGlowCursor(false);
	return true;
}

void CXI_LINECOLLECTION::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	char param[256];
	char param1[256];

    // fill lines structure array
	bool bRelativeRect = !GetIniLong(ini1,name1, ini2,name2, "bAbsoluteRectangle", 0);
    int nCurLine=0;
    if( ini1->ReadString(name1,"line",param,sizeof(param)-1,"") )
        do
        {
            XYRECT scrRect;
			DWORD dwCol=0;
			if( GetMidStr(param, param1, sizeof(param1), "(",")-(") )
				GetDataStr(param1, "ll", &scrRect.left,&scrRect.top);
			if( GetMidStr(param, param1, sizeof(param1), ")-(",")") )
				GetDataStr(param1, "ll", &scrRect.right,&scrRect.bottom);
			if( GetMidStr(param, param1, sizeof(param1), "col:{","}") )
			{
				dwCol = GetColorFromStr(param1, dwCol);
			}
            if(bRelativeRect)	GetRelativeRect(scrRect);

			long n = m_aLines.Add();
			m_aLines.Add();
			m_aLines[n].dwColor = m_aLines[n+1].dwColor = dwCol;
			m_aLines[n].vPos.z = m_aLines[n+1].vPos.z = 1.f;
			m_aLines[n].vPos.x = (float)scrRect.left; m_aLines[n+1].vPos.x = (float)scrRect.right;
			m_aLines[n].vPos.y = (float)scrRect.top;  m_aLines[n+1].vPos.y = (float)scrRect.bottom;

            nCurLine++;
        }
        while( ini1->ReadStringNext(name1,"line",param,sizeof(param)-1) );
}

void CXI_LINECOLLECTION::ReleaseAll()
{
	m_aLines.DelAll();
}

bool CXI_LINECOLLECTION::IsClick(int buttonID,long xPos,long yPos)
{
	return false;
}

void CXI_LINECOLLECTION::ChangePosition( XYRECT &rNewPos )
{
	// no this action
}

void CXI_LINECOLLECTION::SaveParametersToIni()
{
//	char pcWriteParam[2048];

	INIFILE * pIni = api->fio->OpenIniFile( (char*)ptrOwner->m_sDialogFileName.GetBuffer() );
	if( !pIni ) {
		api->Trace( "Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.GetBuffer() );
		return;
	}

	// save position
//	_snprintf( pcWriteParam, sizeof(pcWriteParam), "%d,%d,%d,%d", m_rect.left, m_rect.top, m_rect.right, m_rect.bottom );
//	pIni->WriteString( m_nodeName, "position", pcWriteParam );

	delete pIni;
}

dword _cdecl CXI_LINECOLLECTION::MessageProc(long msgcode, MESSAGE & message)
{
	switch(msgcode)
	{
	case 0: // сменить цвет для линии с номером или всех строк (если номер = -1)
		{
			dword dwColor = message.Long();
			long nLineNum = message.Long();
			if( nLineNum<0 || nLineNum>=(long)m_aLines.Size()/2 ) {
				for( long n=0; n<m_aLines; n++ )
					m_aLines[n].dwColor = dwColor;
			} else {
				m_aLines[nLineNum*2].dwColor = m_aLines[nLineNum*2+1].dwColor = dwColor;
			}
		}
		break;
	case 1: // добавить динию и вернуть ее номер
		{
			dword dwColor = message.Long();
			long nLeft = message.Long();
			long nTop = message.Long();
			long nRight = message.Long();
			long nBottom = message.Long();
			long nLineNum = m_aLines.Add() / 2;
			m_aLines.Add();
			m_aLines[nLineNum*2].dwColor = m_aLines[nLineNum*2+1].dwColor = dwColor;
			m_aLines[nLineNum*2].vPos.z = m_aLines[nLineNum*2+1].vPos.z = 1.f;
			m_aLines[nLineNum*2].vPos.x = (float)nLeft; m_aLines[nLineNum*2+1].vPos.x = (float)nRight;
			m_aLines[nLineNum*2].vPos.y = (float)nTop;  m_aLines[nLineNum*2+1].vPos.y = (float)nBottom;
			return nLineNum;
		}
		break;
	}
	return 0;
}
