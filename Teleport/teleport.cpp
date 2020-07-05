#include "teleport.h"
#include "..\common_h\defines.h"
#include "..\PCS_CONTROLS\pcs_controls.h"

#define DELETE_PTR(x)	if(x) delete x; x=0;

INTERFACE_FUNCTION
CREATE_CLASS(TMPTELEPORT)
CREATE_CLASS(FINDFILESINTODIRECTORY)
CREATE_CLASS(FINDDIALOGNODES)

bool GetStringLine(char * &pStr, char * bufer, long bufSize)
{
	if(pStr==null || bufer==null || bufSize==0) return false;
	bufer[0] = 0;

	char * ps = pStr;
	while(*ps && (*ps==32 || *ps==9 || *ps==10 || *ps==13) ) ps++;
	char * pStart = ps;

	while(*ps && *ps!=10 && *ps!=13) ps++;
	char * pEnd = ps;
	pStr = pEnd;

	if(pEnd==pStart && *ps==0) return false;
	if( (long)(pEnd-pStart)>bufSize-1 ) pEnd = pStart+bufSize-1;
	else bufSize = pEnd-pStart;

	if(bufSize>0)
	{
		strncpy(bufer,pStart,bufSize);
		bufer[bufSize] = 0;
	}
	return true;
}

void GetQuotedString(char * inBuf, char * outBuf, long bufSize)
{
	if(outBuf==null || bufSize<=0) return;
	*outBuf = 0;
	if(inBuf==null) return;

	while(*inBuf && *inBuf!='\"') inBuf++;
	if(*inBuf) inBuf++;

	int bufIdx=0;
	while(*inBuf && *inBuf!='\"' && bufIdx<bufSize-1)
	{
		*outBuf = *inBuf;
		outBuf++; inBuf++;
		bufIdx++;
	}
	*outBuf = 0;
}

TMPTELEPORT::TMPTELEPORT()
{
	rs=NULL;
	m_descrArray=NULL;
	m_nStrQuantity = m_nCurStr = m_nCurShowPos = 0;
}

TMPTELEPORT::~TMPTELEPORT()
{
	ReleaseAll();
}

bool TMPTELEPORT::Init()
{
	rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!rs)	_THROW("No service: dx8render");

	m_leftPos = 20;
	m_topPos = 80;
	m_deltaPos = 30;
	m_showStrQuantity = 10;

	m_nShowType = 0;

	return true;
}

void TMPTELEPORT::Execute(dword Delta_Time)
{
	CONTROL_STATE cs;
	if( ((PCS_CONTROLS*)api->Controls)->m_bIsOffDebugKeys ) return;
	api->Controls->GetControlState("TeleportActive",cs);
	if(cs.state == CST_ACTIVATED)
	{
		if(m_nShowType==0)
		{
			api->Event("TeleportStart","");
			m_nShowType=1;
		}
		else
		{
			ReleaseAll();
			m_nShowType=0;
		}
	}
	long csVal;
	if(api->Controls->GetDebugAsyncKeyState(VK_SHIFT)<0)
		csVal = CST_ACTIVE;
	else
		csVal = CST_ACTIVATED;
	api->Controls->GetControlState("TeleportUp",cs);
	if(cs.state == csVal)
	{
		if(m_nStrQuantity>0)
		{
			if(m_nCurShowPos>0)
				m_nCurShowPos--;
			else
				if(m_nCurStr>0)
					m_nCurStr--;
		}
	}
	api->Controls->GetControlState("TeleportDown",cs);
	if(cs.state == csVal)
	{
		if(m_nStrQuantity>0)
		{
			if( m_nCurStr+m_nCurShowPos < m_nStrQuantity-1 )
				if( m_nCurShowPos < m_showStrQuantity-1 )
					m_nCurShowPos++;
				else
					m_nCurStr++;
		}
	}
	api->Controls->GetControlState("TeleportSelect",cs);
	if(cs.state == CST_ACTIVATED)
	{
		if(m_nStrQuantity>0)
		{
			long n = m_descrArray[m_nCurStr+m_nCurShowPos].num;
			ReleaseAll();
			api->Event("TeleportChoose","l",n);
		}
	}
}

void TMPTELEPORT::Realize(dword Delta_Time)
{
	if(m_nStrQuantity>0)
	{
		int j=0;
		long ftop = m_topPos;
		for(int i=m_nCurStr; i<m_nStrQuantity; i++)
		{
			if(j>=m_showStrQuantity) break;
			if(j==m_nCurShowPos)
				rs->Print(FONT_DEFAULT,ARGB(255,155,155,55),m_leftPos,ftop,"%s",m_descrArray[i].name);
			else
				rs->Print(FONT_DEFAULT,ARGB(255,255,255,255),m_leftPos,ftop,"%s",m_descrArray[i].name);
			ftop += m_deltaPos;
			j++;
		}
	}
}

void TMPTELEPORT::ReleaseAll()
{
	if(m_descrArray!=NULL)
	{
		for(int i=0; i<m_nStrQuantity; i++)
		{
			DELETE_PTR(m_descrArray[i].name);
		}
	}
	DELETE_PTR(m_descrArray);
	m_nStrQuantity = 0;
	m_nCurStr = m_nCurShowPos = 0;
	m_nShowType = 0;
}

dword _cdecl TMPTELEPORT::ProcessMessage(MESSAGE &message)
{
	switch(message.Long())
	{
	case 42222:
		{
			ATTRIBUTES * pA = message.AttributePointer();
			SetShowData(pA);
			if(m_nStrQuantity==0)
				m_nShowType=0;
			else
				m_nShowType=1;
		}
		break;
	}
	return 0;
}

void TMPTELEPORT::SetShowData(ATTRIBUTES * pA)
{
	ReleaseAll();
	m_nStrQuantity = 0;
	if(pA==NULL) return;
	m_nStrQuantity = pA->GetAttributesNum();
	if(m_nStrQuantity==0) return;
	if((m_descrArray=NEW TELEPORT_DESCR[m_nStrQuantity])==NULL)
	{
		_THROW("Allocate memory error");
	}

	for(int i=0;i<m_nStrQuantity;i++)
	{
		char * tmpStr = pA->GetAttribute(i);
		m_descrArray[i].name = NULL;
		m_descrArray[i].num = i;
		if(tmpStr==NULL) continue;
		if( (m_descrArray[i].name=NEW char[strlen(tmpStr)+1]) == NULL )
		{
			_THROW("Allocate memory error");
		}
		strcpy(m_descrArray[i].name,tmpStr);
	}

	SortShowData();
}

void TMPTELEPORT::SortShowData()
{
	if(m_nStrQuantity==0) return;
	bool bContinueSort=true;
	do
	{
		bContinueSort = false;
		for(int i=1; i<m_nStrQuantity; i++)
		{
			if(m_descrArray[i-1].name==NULL) continue;
			if(m_descrArray[i].name==NULL)
			{
				XChange(m_descrArray[i-1],m_descrArray[i]);
				bContinueSort = true;
				continue;
			}
			if(strcmp(m_descrArray[i].name,m_descrArray[i-1].name)<0)
			{
				XChange(m_descrArray[i-1],m_descrArray[i]);
				bContinueSort = true;
			}
		}
	} while(bContinueSort);
}

void TMPTELEPORT::XChange(TELEPORT_DESCR & d1,TELEPORT_DESCR & d2)
{
	int n=d1.num;
	d1.num=d2.num;
	d2.num=n;

	char* nm=d1.name;
	d1.name=d2.name;
	d2.name=nm;
}

bool FINDFILESINTODIRECTORY::Init()
{
	if(AttributesPointer)
	{
		char * dirName = AttributesPointer->GetAttribute("dir");
		char * maskName = AttributesPointer->GetAttribute("mask");
		char fullName[512];
		fullName[0]=0;
		if(dirName) sprintf(fullName,"%s\\",dirName);
		if(maskName) strcat(fullName,maskName);
		else strcat(fullName,"*.*");
		WIN32_FIND_DATA finddat;
		HANDLE hdl = api->fio->_FindFirstFile( fullName, &finddat );
		ATTRIBUTES * pA = AttributesPointer->CreateSubAClass(AttributesPointer,"filelist");
		for(int file_idx=0; hdl!=INVALID_HANDLE_VALUE; file_idx++)
		{
			char sname[32];
			sprintf(sname,"id%d",file_idx);
			if(finddat.cFileName)
				pA->SetAttribute(sname,finddat.cFileName);
			if( !api->fio->_FindNextFile(hdl, &finddat) ) break;
		}
		if(hdl!=INVALID_HANDLE_VALUE) api->fio->_FindClose(hdl);
		return true;
	}
	api->Trace("Attributes Pointer into class FINDFILESINTODIRECTORY = NULL");
	return false;
}

bool FINDDIALOGNODES::Init()
{
	if(AttributesPointer)
	{
		char * fileName = AttributesPointer->GetAttribute("file");
		ATTRIBUTES * pA = AttributesPointer->CreateSubAClass(AttributesPointer,"nodelist");
		if(fileName && pA)
		{
			HANDLE hfile = api->fio->_CreateFile(fileName,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
			if(hfile==INVALID_HANDLE_VALUE)
			{
				api->Trace("WARNING! Can`t dialog file %s",fileName);
				return false;
			}

			long filesize = api->fio->_GetFileSize(hfile,0);
			if(filesize==0)
			{
				api->Trace("Empty dialog file %s",fileName);
				api->fio->_CloseHandle(hfile);
				return false;
			}

			char * fileBuf = NEW char[filesize+1];
			if(fileBuf==null)
			{
				api->Trace("Can`t create buffer for read dialog file %s",fileName);
				api->fio->_CloseHandle(hfile);
				return false;
			}

			DWORD readsize;
			if( api->fio->_ReadFile(hfile,fileBuf,filesize,&readsize) == FALSE ||
				readsize!=(DWORD)filesize )
			{
				api->Trace("Can`t read dialog file: %s",fileName);
				api->fio->_CloseHandle(hfile);
				delete fileBuf;
				return false;
			}
			api->fio->_CloseHandle(hfile);
			fileBuf[filesize] = 0;

			// теперь есть буфер - начнем его анализировать
			char * pStr = fileBuf;
			char param[1024];

			int nodIdx = 0;
			while( GetStringLine(pStr, param, sizeof(param)-1) )
			{
				if( strlen(param)<5 || strnicmp(param,"case",4) ) continue;
				char param2[512];
				GetQuotedString(param,param2,sizeof(param2)-1);
				if(strlen(param2)>0)
				{
					sprintf(param,"id%d",nodIdx);
					nodIdx++;
					pA->SetAttribute(param,param2);
				}
			}

			delete fileBuf;
			return true;
		}
	}
	api->Trace("Attributes Pointer into class FINDDIALOGNODES = NULL");
	return false;
}
