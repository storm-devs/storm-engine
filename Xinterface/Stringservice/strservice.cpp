#include <stdio.h>
#include "strservice.h"
#include "..\..\common_h\defines.h"
#include "..\xinterface.h"

#define USER_BLOCK_BEGINER	'{'
#define USER_BLOCK_ENDING	'}'

static const char* sLanguageFile = "resource\\ini\\TEXTS\\language.ini";

static VSTRSERVICE * g_StringServicePointer = NULL;
static long g_idGlobLanguageFileID = -1;

bool GetStringDescribe(char * inStr, char* strName, char * outStr)
{
	if(strName!=null) strName[0] = 0;
	if(outStr!=null) outStr[0] = 0;
	if(strName==null || outStr==null || inStr==null) {
		api->Trace("Waring: Invalid parameters %s for ini string parser",inStr);
		return false;
	}

	char * ps1;
	int strLenght = 0;
	bool bDoEmpty = true;

	// Get string name
	for(ps1=inStr; *ps1; ps1++)
	{
		char ch1 = *ps1;
		if(bDoEmpty && ch1>=0 && ch1<=32) continue;
		bDoEmpty = false;

		if(ch1==',') break;
		strName[strLenght] = ch1;
		strLenght++;
	}
	if( *ps1 ) ps1++;

	while(strLenght>0 && strName[strLenght-1]>0 && strName[strLenght-1]<=32)
		strLenght--;
	strName[strLenght] = 0;
	if(strLenght<=0) {
		api->Trace("Waring: Invalid name parameter for string: %s",inStr);
		return false;
	}

	// Get string value
	strLenght = 0;
	bDoEmpty = true;
	for(; *ps1; ps1++)
	{
		char ch1 = *ps1;
		if(bDoEmpty && ch1!='\"') continue;
		if(bDoEmpty) {
			bDoEmpty = false;
			continue;
		}

		if(ch1=='\"') break;
		outStr[strLenght] = ch1;
		strLenght++;
	}
	outStr[strLenght] = 0;

	return true;
}

STRSERVICE::STRSERVICE()
{
	m_sLanguage = null;
	m_sIniFileName = null;
	m_sLanguageDir = null;

	m_nStringQuantity = 0;
	m_psStrName = null;
	m_psString = null;

	m_pUsersBlocks = null;

	g_StringServicePointer = this;
	m_nDialogSourceFile = -1;
}

STRSERVICE::~STRSERVICE()
{
	int i;

	CloseUsersStringFile( m_nDialogSourceFile );
	m_nDialogSourceFile = -1;

	if(m_psStrName!=null)
	{
		for(i=0; i<m_nStringQuantity; i++)
			if(m_psStrName[i]!=null)
				delete m_psStrName[i];
		delete m_psStrName;
		m_psStrName=null;
	}
	if(m_psString!=null)
	{
		for(i=0; i<m_nStringQuantity; i++)
			if(m_psString[i]!=null)
				delete m_psString[i];
		delete m_psString;
		m_psString=null;
	}
	DELETE(m_sIniFileName);
	DELETE(m_sLanguage);
	DELETE(m_sLanguageDir);

	while(m_pUsersBlocks!=null)
	{
		UsersStringBlock * pUSB = m_pUsersBlocks;
		m_pUsersBlocks = m_pUsersBlocks->next;
		if(pUSB->psStrName!=null)
		{
			for(i=0; i<pUSB->nStringsQuantity; i++)
				if(pUSB->psStrName[i]!=null)
					delete pUSB->psStrName[i];
			delete pUSB->psStrName;
			pUSB->psStrName=null;
		}
		if(pUSB->psString!=null)
		{
			for(i=0; i<pUSB->nStringsQuantity; i++)
				if(pUSB->psString[i]!=null)
					delete pUSB->psString[i];
			delete pUSB->psString;
			pUSB->psString=null;
		}
		if(pUSB->fileName!=null) delete pUSB->fileName;
		pUSB->nStringsQuantity=0;
		delete pUSB;
		pUSB = null;
	}
}

bool STRSERVICE::Init()
{
	GUARD(bool STRSERVICE::Init())
	LoadIni();
	UNGUARD
	return true;
}

bool STRSERVICE::LoadState(ENTITY_STATE *state)
{
	return true;
}

bool STRSERVICE::CreateState(ENTITY_STATE_GEN *state_gen)
{
	return true;
}

void STRSERVICE::RunStart()
{
}

void STRSERVICE::RunEnd()
{
}

void STRSERVICE::SetLanguage(const char* sLanguage)
{
	GUARD(void STRSERVICE::SetLanguage(const char* sLanguage))

	int			i;
	INIFILE *	ini;
	char		param[512];

	if(sLanguage==null)
	{
		api->Trace("WARNING! Attempt set empty language");
		return;
	}

	// Уже установлен этот язык
	if( m_sLanguage!=null && stricmp(sLanguage,m_sLanguage)==0 ) return;

	// initialize ini file
	ini = api->fio->OpenIniFile((char*)sLanguageFile);
	if(!ini)
	{
		api->Trace("ini file %s not found!",sLanguageFile);
		return;
	}

	// установим новое имя для языка
	DELETE(m_sLanguage);
	if( (m_sLanguage=NEW char[strlen(sLanguage)+1]) == null ) {THROW("Allocate memory error");}
	strcpy(m_sLanguage,sLanguage);

	while(true)
	{
		// удалим старые данные
		DELETE(m_sIniFileName);
		DELETE(m_sLanguageDir);

		// получим директорию для текстовых файлов данного языка
		if( ini->ReadString("DIRECTORY",m_sLanguage,param,sizeof(param)-1,"") )
		{
			if( (m_sLanguageDir=NEW char[strlen(param)+1]) == null )
				{_THROW("Allocate memory error");}
			strcpy(m_sLanguageDir,param);
		}
		else	api->Trace("WARNING! Not found directory record for language %s",sLanguage);

		// получим имя ини файла со строками общего использования для этого языка
		if( ini->ReadString("COMMON","strings",param,sizeof(param)-1,"") )
		{
			if( (m_sIniFileName=NEW char[strlen(param)+1]) == null )
				{_THROW("Allocate memory error");}
			strcpy(m_sIniFileName,param);
		}
		else	api->Trace("WARNING! Not found common strings file record");

		if(m_sLanguageDir!=null && m_sIniFileName!=null) break;

		// сравним текущий язык с дефолтовым
		if( ini->ReadString("COMMON","defaultLanguage",param,sizeof(param)-1,"") )
		{
			if(stricmp(m_sLanguage,param)==0) break;
			api->Trace("WARNING! Language %s not exist some ini parameters. Language set to default %s",m_sLanguage,param);
			DELETE(m_sLanguage);
			m_sLanguage = NEW char[strlen(param)+1];
			if(m_sLanguage==null) {_THROW("Allocate memory error");}
			strcpy(m_sLanguage,param);
		}
		else break;
	}
	//==========================================================================
	// reread fonts
	//==========================================================================
	VDX8RENDER * RenderService = (VDX8RENDER *)api->CreateService("dx8render");
	if(RenderService)
	{
		char fullIniPath[512];
		if( ini->ReadString("FONTS",m_sLanguage,param,sizeof(param)-1,"") ) {
			sprintf(fullIniPath,"resource\\ini\\%s",param);
		} else {
			api->Trace("Warning: Not found font record for language %s",m_sLanguage);
			sprintf(fullIniPath,"resource\\ini\\fonts.ini");
		}
		RenderService->SetFontIniFileName(fullIniPath);
	}
	//==========================================================================
	delete ini;

	//====================================================================
	// Set language data
	//====================================================================

	// delete old stringes
	if( m_psString!=NULL )
	{
		for(i=0; i<m_nStringQuantity; i++)
			delete m_psString[i];
		delete m_psString;
		m_psString = NULL;
	}
	if( m_psStrName!=NULL )
	{
		for(i=0; i<m_nStringQuantity; i++)
			delete m_psStrName[i];
		delete m_psStrName;
		m_psStrName = NULL;
	}

	// initialize ini file
	sprintf(param,"resource\\ini\\texts\\%s\\%s",m_sLanguageDir,m_sIniFileName);
	ini = api->fio->OpenIniFile(param);
	if(!ini)
	{
		api->Trace("WARNING! ini file \"%s\" not found!",param);
		return;
	}

	// get string quantity
	int newSize = 0;
	if( ini->ReadString(0,"string",param,sizeof(param)-1,"") )
		do newSize++; while(ini->ReadStringNext(0,"string",param,sizeof(param)-1));

	// check to right of ini files
	if(newSize!=m_nStringQuantity && m_nStringQuantity!=0)
		api->Trace("WARNING: language %s ini file has different size",sLanguage);
	m_nStringQuantity = newSize;

	// create strings & string names arreys
	if(newSize>0)
	{
		m_psString = NEW char*[newSize];
		m_psStrName = NEW char*[newSize];
		if(m_psStrName==NULL || m_psString==NULL)
			_THROW("Allocate memory error")
	} else {
		m_psString = null;
		m_psStrName = null;
	}

	// fill stringes
	char strName[sizeof(param)];
	char string[sizeof(param)];
	ini->ReadString(0,"string",param,sizeof(param)-1,"");
	for(i=0; i<m_nStringQuantity; i++)
	{
		if( GetStringDescribe(param,strName,string) )
		{
			// fill string name
			m_psStrName[i] = NEW char[strlen(strName)+1];
			if(m_psStrName==NULL)
				_THROW("allocate memory error")
			strcpy(m_psStrName[i],strName);

			// fill string self
			m_psString[i] = NEW char[strlen(string)+1];
			if(m_psString==NULL)
				_THROW("allocate memory error")
			strcpy(m_psString[i],string);
		} else {
			// invalid string
			m_psStrName[i] = null;
			m_psString[i] = null;
		}

		// next string
		ini->ReadStringNext(0,"string",param,sizeof(param)-1);
	}

	// end of search
	delete ini;


	//=======================================================================
	// Перечитаем пользовательские файлы
	//=======================================================================
	UsersStringBlock * pOldURoot = m_pUsersBlocks;
	m_pUsersBlocks = null;
	for(UsersStringBlock * pUSB=pOldURoot; pUSB!=null; pUSB=pUSB->next)
	{
		if(pUSB->nref<=0) continue;
		long newID = OpenUsersStringFile(pUSB->fileName);

		for(UsersStringBlock * pUTmp=m_pUsersBlocks; pUTmp!=null; pUTmp=pUTmp->next)
			if(pUTmp->blockID==newID) break;
		if(pUTmp==null) {
			api->Trace("Error: Can`t reinit user language file %s",pUSB->fileName);
			continue;
		}

		pUTmp->blockID = pUSB->blockID;
		pUTmp->nref = pUSB->nref;
		if( pUTmp->nStringsQuantity != pUSB->nStringsQuantity )
		{
			api->Trace("Warning: user strings file %s have different size for new language %s",pUTmp->fileName, m_sLanguage);
			for(int itmp1 = 0; itmp1<pUTmp->nStringsQuantity; itmp1++)
			{
				if( pUTmp->psStrName[itmp1]==null ) continue;
				for(int itmp2 = 0; itmp2<pUSB->nStringsQuantity; itmp2++)
				{
					if( pUSB->psStrName[itmp2]==null ) continue;
					if( stricmp(pUSB->psStrName[itmp2],pUTmp->psStrName[itmp1])==0 )
						break;
				}
				if( itmp2 >= pUSB->nStringsQuantity )
					api->Trace(">>> string <%s> not found into strings file",pUTmp->psStrName[itmp1]);
			}

			for(itmp1 = 0; itmp1<pUSB->nStringsQuantity; itmp1++)
			{
				if( pUSB->psStrName[itmp1]==null ) continue;
				for(int itmp2 = 0; itmp2<pUTmp->nStringsQuantity; itmp2++)
				{
					if( pUTmp->psStrName[itmp2]==null ) continue;
					if( stricmp(pUTmp->psStrName[itmp2],pUSB->psStrName[itmp1])==0 )
						break;
				}
				if( itmp2 >= pUTmp->nStringsQuantity )
					api->Trace(">>> string <%s> is new into strings file",pUSB->psStrName[itmp1]);
			}
		}
	}
	// Удалим старые пользовательские файлы
	UsersStringBlock * pNewURoot = m_pUsersBlocks;
	m_pUsersBlocks = pOldURoot;
	while(m_pUsersBlocks)
	{
		m_pUsersBlocks->nref = 1;
		CloseUsersStringFile(m_pUsersBlocks->blockID);
	}
	m_pUsersBlocks = pNewURoot;
	//=======================================================================

	UNGUARD
}

char* STRSERVICE::GetLanguage()
{
	if(m_sLanguage!=null)
		return m_sLanguage;
	return null;
}

char* STRSERVICE::GetString(const char* stringName, char* sBuffer, size_t bufferSize)
{
	GUARD(char* STRSERVICE::GetString(const char* stringName, char* sBuffer, size_t bufferSize))

	if(stringName!=NULL)
		for(int i=0; i<m_nStringQuantity; i++)
			if( !stricmp(m_psStrName[i],stringName) )
			{
				size_t length = strlen(m_psString[i])+1;
				if( sBuffer==NULL ) bufferSize = 0;
				if(bufferSize<length) length=bufferSize;

				if(length>0)
					strncpy(sBuffer,m_psString[i],length);

				return m_psString[i];
			}

	return NULL;
	UNGUARD
}

void STRSERVICE::SetDialogSourceFile(const char * fileName)
{
	CloseUsersStringFile( m_nDialogSourceFile );
	m_nDialogSourceFile = OpenUsersStringFile( (char*)fileName );
}

void STRSERVICE::LoadIni()
{
	GUARD(void STRSERVICE::LoadIni())

	INIFILE *	ini;
	char		param[256];

	// initialize ini file
	ini = _CORE_API->fio->OpenIniFile((char*)sLanguageFile);
	if(!ini) {
		api->Trace("Error: Language ini file not found!");
		return;
	}

	char sGlobalUserFileName[256];
	if( !ini->ReadString("COMMON","GlobalFile",sGlobalUserFileName,sizeof(sGlobalUserFileName)-1,"") )
	{
		sGlobalUserFileName[0] = 0;
		api->Trace("WARNING! Language ini file have not global file name");
	}

	// Get default language name
	if( !ini->ReadString("COMMON","defaultLanguage",param,sizeof(param)-1,"") )
	{
		api->Trace("WARNING! Language ini file have not default language.");
		strcpy(param,"English");
	}
	delete ini;

#ifdef _XBOX
	switch ( XGetLanguage() )
	{
	case XC_LANGUAGE_GERMAN: strcpy(param,"German"); break;
	case XC_LANGUAGE_FRENCH: strcpy(param,"French"); break;
	case XC_LANGUAGE_SPANISH: strcpy(param,"Spanish"); break;
	case XC_LANGUAGE_ENGLISH: strcpy(param,"English"); break;
	//case XC_LANGUAGE_ITALIAN: strcpy(param,"Russian"); break;
//	default: strcpy(param,"English"); break;
	}
#endif
	if(param[0]!=0)	SetLanguage(param);

	if( sGlobalUserFileName[0] )
		g_idGlobLanguageFileID = OpenUsersStringFile(sGlobalUserFileName);

	SetDialogSourceFile("dialogsource.txt");

	UNGUARD
}

long STRSERVICE::GetStringNum(const char* stringName)
{
	GUARD(long STRSERVICE::GetStringNum(const char* stringName))

	if(stringName!=NULL)
		for(long i=0; i<m_nStringQuantity; i++)
			if( !stricmp(m_psStrName[i],stringName) )
				return i;
	return -1L;

	UNGUARD
}

char* STRSERVICE::GetString(long strNum)
{
	GUARD(char* STRSERVICE::GetString(long strNum))

	if(strNum<0 || strNum>=m_nStringQuantity) return NULL;
	return m_psString[strNum];

	UNGUARD
}

char* STRSERVICE::GetStringName(long strNum)
{
	GUARD(char* STRSERVICE::GetStringName(long strNum))

	if(strNum<0 || strNum>=m_nStringQuantity) return NULL;
	return m_psStrName[strNum];

	UNGUARD
}

long STRSERVICE::OpenUsersStringFile(char * fileName)
{
	int i;
	if(fileName==null) return -1;

	UsersStringBlock * pPrev = null;
	for(UsersStringBlock * pUSB=m_pUsersBlocks; pUSB!=null; pUSB=pUSB->next)
		if(pUSB->fileName!=null && stricmp(pUSB->fileName,fileName)==0) break;
		else pPrev=pUSB;
	if(pUSB!=null)
	{
		pUSB->nref++;
		return pUSB->blockID;
	}

	pUSB = NEW UsersStringBlock;
	if(pUSB==null)	{_THROW("Allocate memory error")}

	// strings reading
	char param[512];
	sprintf(param,"resource\\ini\\TEXTS\\%s\\%s",m_sLanguageDir,fileName);
	HANDLE hfile = api->fio->_CreateFile(param,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
	long filesize = api->fio->_GetFileSize(hfile,0);
	if(filesize<=0)
	{
		api->Trace("WARNING! Strings file \"%s\" not exist/or zero size",fileName);
		api->fio->_CloseHandle(hfile);
		return -1;
	}

	char * fileBuf = NEW char[filesize+1];
	if(fileBuf==null) {_THROW("Allocate memory error")}

	long readsize;
	if( api->fio->_ReadFile(hfile,fileBuf,filesize,(DWORD*)&readsize) == FALSE ||
		readsize!=filesize )
	{
		api->Trace("Can`t read strings file: %s",fileName);
		api->fio->_CloseHandle(hfile);
		DELETE(fileBuf);
		return -1;
	}
	api->fio->_CloseHandle(hfile);
	fileBuf[readsize]=0;

	pUSB->nref = 1;
	pUSB->fileName = NEW char[strlen(fileName)+1];
	if(pUSB->fileName==null)	{_THROW("Allocate memory error")}
	strcpy(pUSB->fileName,fileName);
	pUSB->blockID = GetFreeUsersID();

	long stridx = 0;
	pUSB->psStrName = null;
	pUSB->psString = null;
	for(pUSB->nStringsQuantity=0;;pUSB->nStringsQuantity++)
	{
		if( !GetNextUsersString(fileBuf,stridx,null,null) ) break;
	}
	if(pUSB->nStringsQuantity==0) {
		api->Trace("WARNING! Strings file \"%s\" not contain strings",fileName);
	} else {
		pUSB->psStrName = NEW char*[pUSB->nStringsQuantity];
		pUSB->psString = NEW char*[pUSB->nStringsQuantity];
		if(pUSB->psStrName==null || pUSB->psString==null)
		{_THROW("Allocate memory error")}
		stridx = 0;
		for(i=0; i<pUSB->nStringsQuantity; i++)
			GetNextUsersString(fileBuf,stridx,&pUSB->psStrName[i],&pUSB->psString[i]);
	}

	DELETE(fileBuf);

	pUSB->next = 0;
	if(pPrev==null) m_pUsersBlocks = pUSB;
	else	pPrev->next = pUSB;
	return pUSB->blockID;
}

void STRSERVICE::CloseUsersStringFile(long id)
{
	int i;
	if(id==-1) return;

	UsersStringBlock * pPrev = null;
	for(UsersStringBlock * pUSB=m_pUsersBlocks; pUSB!=null; pUSB=pUSB->next)
		if(pUSB->blockID == id) break;
		else pPrev = pUSB;
	if(pUSB==null) return;
	pUSB->nref--;	if(pUSB->nref>0) return;

	if(pPrev==null) m_pUsersBlocks = pUSB->next;
	else pPrev->next = pUSB->next;

	if(pUSB->fileName!=null)	{ delete pUSB->fileName; pUSB->fileName=null; }
	if(pUSB->psStrName!=null)
	{
		for(i=0; i<pUSB->nStringsQuantity; i++)
			if(pUSB->psStrName[i]!=null)
				delete pUSB->psStrName[i];
		delete pUSB->psStrName;
		pUSB->psStrName=null;
	}
	if(pUSB->psString!=null)
	{
		for(i=0; i<pUSB->nStringsQuantity; i++)
			if(pUSB->psString[i]!=null)
				delete pUSB->psString[i];
		delete pUSB->psString;
		pUSB->psString=null;
	}
	delete pUSB;
}

char * STRSERVICE::TranslateFromUsers(long id, char * inStr)
{
	int i;
	if(inStr==null || id==-1) return null;

	for(UsersStringBlock * pUSB=m_pUsersBlocks; pUSB!=null; pUSB=pUSB->next)
		if(pUSB->blockID == id) break;
	if(pUSB==null) return null;

	for(i=0; i<pUSB->nStringsQuantity; i++)
		if(pUSB->psStrName[i]!=null && stricmp(pUSB->psStrName[i],inStr)==0) break;
	if(i<pUSB->nStringsQuantity)	return pUSB->psString[i];
	return null;
}

long STRSERVICE::GetFreeUsersID()
{
	for(int id=0;;id++)
	{
		for(UsersStringBlock * pUSB=m_pUsersBlocks; pUSB!=null; pUSB=pUSB->next)
			if(pUSB->blockID == id) break;
		if(pUSB==null) break;
	}
	return id;
}

bool STRSERVICE::GetNextUsersString(char *src,long &idx,char* *strName,char* *strData)
{
	char * tmpStr;
	char * dataBeg;
	char * dataEnd;
	char * nameBeg;
	char * nameEnd;

	if(strName!=null) *strName = null;
	if(strData!=null) *strData = null;
	if(src==null || idx<0) return false;

	dataBeg = strchr(&src[idx],USER_BLOCK_BEGINER);
	if(dataBeg==null)
	{
		idx = strlen(src);
		return false;
	}
	dataBeg += 1;
	dataEnd = strchr(dataBeg,USER_BLOCK_ENDING);
	if(dataEnd==null)
	{
		idx = strlen(src);
		return false;
	}
	dataEnd -= 1;

	// find ID name
	nameEnd = nameBeg = &src[idx];
	if( (tmpStr=strchr(nameEnd,'\n')) < dataBeg ) do
	{
		if(tmpStr==null) break;
		nameBeg = nameEnd;
		nameEnd = tmpStr;
	} while( (tmpStr=strchr(nameEnd+2,'\n')) < dataBeg );
	idx = dataEnd-src+2;

	// избавимся от пробелов слева
	while(nameBeg<=nameEnd)
	{
		if(*nameBeg==0x0A)	{nameBeg++; continue;}
		if(*nameBeg==0x0D)	{nameBeg++; continue;}
		if(*nameBeg=='\t')	{nameBeg++; continue;}
		if(*nameBeg==' ')	{nameBeg++; continue;}
		break;
	}
	if(nameEnd-nameBeg<=1) nameEnd = dataBeg-2;
	// избавимся от пробелов справа
	while(nameBeg<nameEnd)
	{
		if(*nameBeg==0x0A)	{nameBeg++; continue;}
		if(*nameBeg==0x0D)	{nameBeg++; continue;}
		if(*nameBeg=='\t')	{nameBeg++; continue;}
		if(*nameBeg==' ')	{nameBeg++; continue;}
		if(*nameEnd==0x0A)	{nameEnd--;	continue;}
		if(*nameEnd==0x0D)	{nameEnd--;	continue;}
		if(*nameEnd=='\t') {nameEnd--; continue;}
		if(*nameEnd==' ') {nameEnd--; continue;}
		break;
	}
	// избавимся от пробелов слева и справа в данных
	while(dataBeg<dataEnd)
	{
		if(*dataEnd=='\t')	{dataEnd--; continue;}
		if(*dataEnd==' ')	{dataEnd--; continue;}
		if(*dataBeg=='\t')	{dataBeg++; continue;}
		if(*dataBeg==' ')	{dataBeg++; continue;}
		break;
	}
	if(*dataEnd==0x0A || *dataEnd==0x0D)	dataEnd-=1;
	if(*dataBeg==0x0A || *dataBeg==0x0D)	dataBeg+=2;

	if(strName!=null && nameBeg<=nameEnd)
	{
		*strName = NEW char[nameEnd-nameBeg+2];
		if(*strName==null) {_THROW("Allocate memory error")}
		strncpy(*strName,nameBeg,nameEnd-nameBeg+1);
		strName[0][nameEnd-nameBeg+1] = 0;
	}

	if(strData!=null && dataBeg<=dataEnd)
	{
		*strData = NEW char[dataEnd-dataBeg+2];
		if(*strData==null) {_THROW("Allocate memory error")}
		strncpy(*strData,dataBeg,dataEnd-dataBeg+1);
		strData[0][dataEnd-dataBeg+1] = 0;
	}

	return true;
}


//===============================================================
// SCRIPT LIBS SECTION
//===============================================================
// ОПИСАНИЕ ФУНКЦИЙ :
//==============================================================

	// Получить текущий язык
DWORD __cdecl _Language_GetLanguage(VS_STACK * pS)
{
	char * strLangName = g_StringServicePointer->GetLanguage();

	VDATA * pVR = (VDATA*)pS->Push(); if (!pVR) return IFUNCRESULT_FAILED;
	pVR->Set(strLangName);

	return IFUNCRESULT_OK;
}

	// Открыть языковый файл
DWORD __cdecl _Language_OpenFile(VS_STACK * pS)
{
	VDATA * pLngFileName = (VDATA*)pS->Pop();	if (!pLngFileName) return IFUNCRESULT_FAILED;
	char * strLngFileName=0;	pLngFileName->Get(strLngFileName);

		// получим ID для заданного файла
	long nLngFileID = g_StringServicePointer->OpenUsersStringFile(strLngFileName);

	VDATA * pVR = (VDATA*)pS->Push(); if (!pVR) return IFUNCRESULT_FAILED;
	pVR->Set(nLngFileID);

	return IFUNCRESULT_OK;
}

	// Закрыть языковый файл
DWORD __cdecl _Language_CloseFile(VS_STACK * pS)
{
	VDATA * pLngFileID = (VDATA*)pS->Pop();	if (!pLngFileID) return IFUNCRESULT_FAILED;
	long nLngFileID=-1;	pLngFileID->Get(nLngFileID);

	g_StringServicePointer->CloseUsersStringFile(nLngFileID);

	return IFUNCRESULT_OK;
}

	// Интерпретировать строку используя языковый файл
DWORD __cdecl _Language_ConvertString(VS_STACK * pS)
{
	VDATA * pInStr = (VDATA*)pS->Pop();	if (!pInStr) return IFUNCRESULT_FAILED;
	char * strInStr=0;	pInStr->Get(strInStr);

	VDATA * pLngFileID = (VDATA*)pS->Pop();	if (!pLngFileID) return IFUNCRESULT_FAILED;
	long nLngFileID=-1;	pLngFileID->Get(nLngFileID);

	char * strOutStr = g_StringServicePointer->TranslateFromUsers(nLngFileID,strInStr);

	VDATA * pVR = (VDATA*)pS->Push(); if (!pVR) return IFUNCRESULT_FAILED;
	if(strOutStr)	pVR->Set(strOutStr);
	else pVR->Set("\0");

	return IFUNCRESULT_OK;
}

	// Интерпретировать строку используя общий языковый файл
DWORD __cdecl _XI_ConvertString(VS_STACK * pS)
{
	VDATA * pInStr = (VDATA*)pS->Pop();	if (!pInStr) return IFUNCRESULT_FAILED;
	char * strInStr=0;	pInStr->Get(strInStr);

	char * strOutStr = g_StringServicePointer->GetString(strInStr);

	VDATA * pVR = (VDATA*)pS->Push(); if (!pVR) return IFUNCRESULT_FAILED;
	if(strOutStr)	pVR->Set(strOutStr);
	else pVR->Set("\0");

	return IFUNCRESULT_OK;
}

	// Установить язык
DWORD __cdecl _Language_SetLanguage(VS_STACK * pS)
{
	VDATA * pLngName = (VDATA*)pS->Pop();	if (!pLngName) return IFUNCRESULT_FAILED;
	char * strLngName=0;	pLngName->Get(strLngName);

	g_StringServicePointer->SetLanguage(strLngName);

	return IFUNCRESULT_OK;
}

	// Получить ID Глобального языкового файла
DWORD __cdecl _GlobalLngFileID(VS_STACK * pS)
{
	VDATA * pVR = (VDATA*)pS->Push(); if (!pVR) return IFUNCRESULT_FAILED;
	pVR->Set(g_idGlobLanguageFileID);

	return IFUNCRESULT_OK;
}

	// Получить ID Глобального языкового файла
DWORD __cdecl _LanguageGetFaderPic(VS_STACK * pS)
{
	VDATA * pPicName = (VDATA*)pS->Pop();	if (!pPicName) return IFUNCRESULT_FAILED;
	char * strPicName=0;	pPicName->Get(strPicName);

	char newPicName[MAX_PATH];	newPicName[0]=0;
	if(strPicName!=null && strPicName[0]!=0)
	{
		if(g_StringServicePointer->GetLanguage()!=null)
		{
			for(int nInLen=strlen(strPicName); nInLen>0; nInLen--)
				if(strPicName[nInLen-1]=='\\')
					break;
			if(nInLen>0) {
				strncpy(newPicName,strPicName,nInLen); newPicName[nInLen]=0;
			}
			strcat(newPicName,g_StringServicePointer->GetLanguage());
			strcat(newPicName,"\\");
			strcat(newPicName,&strPicName[nInLen]);
		} else {
			strcpy(newPicName,strPicName);
		}
	}

	VDATA * pVR = (VDATA*)pS->Push(); if (!pVR) return IFUNCRESULT_FAILED;
	if(newPicName)	pVR->Set(newPicName);
	else pVR->Set("\0");

	return IFUNCRESULT_OK;
}


// Установить цветокоррекцию для игры
DWORD __cdecl _SetColorCorrection(VS_STACK * pS)
{
	VDATA * pBright = (VDATA*)pS->Pop();	if (!pBright) return IFUNCRESULT_FAILED;
	float fBright = 1.f;	pBright->Get(fBright);

	VDATA * pGamma = (VDATA*)pS->Pop();	if (!pGamma) return IFUNCRESULT_FAILED;
	float fGamma = 1.f;	pGamma->Get(fGamma);

	VDATA * pContrast = (VDATA*)pS->Pop();	if (!pContrast) return IFUNCRESULT_FAILED;
	float fContrast = 1.f;	pContrast->Get(fContrast);

	VDX8RENDER * pVR = (VDX8RENDER *)api->CreateService("dx8render");
	if( !pVR ) return IFUNCRESULT_FAILED;

	pVR->SetColorParameters(fGamma,fBright,fContrast);
	return IFUNCRESULT_OK;
}

// Установить чувствительность мыши
DWORD __cdecl _SetMouseSensitivity(VS_STACK * pS)
{
	VDATA * pYSens = (VDATA*)pS->Pop();	if (!pYSens) return IFUNCRESULT_FAILED;
	float fYSens = 1.f;	pYSens->Get(fYSens);

	VDATA * pXSens = (VDATA*)pS->Pop();	if (!pXSens) return IFUNCRESULT_FAILED;
	float fXSens = 1.f;	pXSens->Get(fXSens);

	CONTROLS * pCntrl = (CONTROLS *)api->CreateService("PCS_CONTROLS");
	if( !pCntrl ) return IFUNCRESULT_FAILED;

	pCntrl->SetMouseSensivityX(fXSens);
	pCntrl->SetMouseSensivityY(fYSens);

	return IFUNCRESULT_OK;
}

// Установить инвертность на клавишу
DWORD __cdecl _ControlMakeInvert(VS_STACK * pS)
{
	VDATA * pControlFlag = (VDATA*)pS->Pop();	if (!pControlFlag) return IFUNCRESULT_FAILED;
	long nControlFlag = 0;	pControlFlag->Get(nControlFlag);

	VDATA * pControlName = (VDATA*)pS->Pop();	if (!pControlName) return IFUNCRESULT_FAILED;
	char * sCntrlName = null;	pControlName->Get(sCntrlName);
	if(!sCntrlName) return IFUNCRESULT_FAILED;

	CONTROLS * pCntrl = (CONTROLS *)api->CreateService("PCS_CONTROLS");
	if( !pCntrl ) return IFUNCRESULT_FAILED;

	int n = pCntrl->CreateControl(sCntrlName);
	if(nControlFlag!=0)	pCntrl->SetControlFlags(n,0);
	else pCntrl->SetControlFlags(n,INVERSE_CONTROL);
	return IFUNCRESULT_OK;
}

DWORD __cdecl _InterfaceMakeNode(VS_STACK * pS)
{
	VDATA * pDat;

	char *sFileName = 0;
	char *sNodeType = 0;
	char *sNodeName = 0;
	long nPriority = 80;

	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	nPriority = pDat->GetLong();

	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	sNodeName = pDat->GetString();

	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	sNodeType = pDat->GetString();

	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	sFileName = pDat->GetString();

	if( XINTERFACE::pThis != 0 )
		XINTERFACE::pThis->CreateNode( sFileName, sNodeType, sNodeName, nPriority );

	return IFUNCRESULT_OK;
}

DWORD __cdecl _InterfaceDeleteNode(VS_STACK * pS)
{
	VDATA * pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	char* pcNodeName = pDat->GetString();
	if( XINTERFACE::pThis )
		XINTERFACE::pThis->DeleteNode( pcNodeName );
	return IFUNCRESULT_OK;
}

DWORD __cdecl _InterfaceWindowShow(VS_STACK * pS)
{
	VDATA * pDat;

	char *sWindowName = 0;
	long nShow = 0;

	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	nShow = pDat->GetLong();

	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	sWindowName = pDat->GetString();

	if( XINTERFACE::pThis != 0 )
		XINTERFACE::pThis->ShowWindow( sWindowName, nShow!=0 );

	return IFUNCRESULT_OK;
}

DWORD __cdecl _InterfaceWindowDisable(VS_STACK * pS)
{
	VDATA * pDat;

	char *sWindowName = 0;
	long nShow = 0;

	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	nShow = pDat->GetLong();

	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	sWindowName = pDat->GetString();

	if( XINTERFACE::pThis != 0 )
		XINTERFACE::pThis->DisableWindow( sWindowName, nShow!=0 );

	return IFUNCRESULT_OK;
}

DWORD __cdecl _InterfaceIsWindowEnable(VS_STACK * pS)
{
	VDATA * pDat;

	char *sWindowName = 0;

	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	sWindowName = pDat->GetString();

	bool bActive = true;
	if( XINTERFACE::pThis != 0 ) {
		bActive = XINTERFACE::pThis->IsWindowActive(sWindowName);
	}

	pDat = (VDATA*)pS->Push();	if (!pDat) return IFUNCRESULT_FAILED;
	pDat->Set( (long)(bActive?1:0) );

	return IFUNCRESULT_OK;
}

DWORD __cdecl _InterfaceWindowAddNode(VS_STACK * pS)
{
	VDATA * pDat;

	char *sWindowName = 0;
	char *sNodeName = 0;

	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	sNodeName = pDat->GetString();

	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	sWindowName = pDat->GetString();

	if( XINTERFACE::pThis != 0 )
		XINTERFACE::pThis->AddNodeToWindow( sNodeName, sWindowName );

	return IFUNCRESULT_OK;
}

DWORD __cdecl _InterfaceCreateFolder(VS_STACK * pS)
{
	VDATA * pDat;
	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	char *sFolderName = pDat->GetString();

	// precreate directory
	const char* pcCurPtr = sFolderName;
	while( (pcCurPtr=strchr(pcCurPtr,'\\'))!=NULL )
	{
		char tmpchr = pcCurPtr[0];
		((char*)pcCurPtr)[0] = 0;
		CreateDirectory(sFolderName,0);
		((char*)pcCurPtr)[0] = tmpchr;
		pcCurPtr++;
	}
	// create self directory
	long nSuccess = api->fio->_CreateDirectory(sFolderName,0);

	pDat = (VDATA*)pS->Push();	if (!pDat) return IFUNCRESULT_FAILED;
	pDat->Set( nSuccess );
	return IFUNCRESULT_OK;
}

DWORD __cdecl _InterfaceCheckFolder(VS_STACK * pS)
{
	VDATA * pDat;
	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	char *sFolderName = pDat->GetString();
	long nSuccess = false;
	WIN32_FIND_DATA wfd;
	HANDLE h = api->fio->_FindFirstFile(sFolderName,&wfd);
	if( h != INVALID_HANDLE_VALUE )
	{
		api->fio->_FindClose(h);
		nSuccess = true;
	}
	pDat = (VDATA*)pS->Push();	if (!pDat) return IFUNCRESULT_FAILED;
	pDat->Set( nSuccess );
	return IFUNCRESULT_OK;
}

BOOL DeleteFolderWithCantainment(const char* sFolderName)
{
	WIN32_FIND_DATA wfd;
	HANDLE hff = api->fio->_FindFirstFile( sFolderName+string("\\*.*"), &wfd );
	if( hff != INVALID_HANDLE_VALUE ) {
		do {
			string sFileName = sFolderName + string("\\");
			if( wfd.cAlternateFileName[0] ) sFileName += wfd.cAlternateFileName;
			else sFileName += wfd.cFileName;

			if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
				if( wfd.cFileName[0]=='.' ) continue;
				DeleteFolderWithCantainment( sFileName.GetBuffer() );
			} else {
				api->fio->_DeleteFile( sFileName.GetBuffer() );
			}
		} while( api->fio->_FindNextFile(hff,&wfd) );
		api->fio->_FindClose( hff );
	}
	return api->fio->_RemoveDirectory( sFolderName );
}

DWORD __cdecl _InterfaceDeleteFolder(VS_STACK * pS)
{
	VDATA * pDat;
	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	char *sFolderName = pDat->GetString();
	//long nSuccess = api->fio->_RemoveDirectory(sFolderName);
	long nSuccess = DeleteFolderWithCantainment(sFolderName);
	pDat = (VDATA*)pS->Push();	if (!pDat) return IFUNCRESULT_FAILED;
	pDat->Set( nSuccess );
	return IFUNCRESULT_OK;
}

DWORD __cdecl _InterfaceFindFolders(VS_STACK * pS)
{
	VDATA * pDat;
	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	ATTRIBUTES* pA = pDat->GetAClass();
	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	char *sFindTemplate = pDat->GetString();
	WIN32_FIND_DATA wfd;
	HANDLE h = api->fio->_FindFirstFile( sFindTemplate, &wfd );
	long n = 0;
	if( h!=INVALID_HANDLE_VALUE ) do
	{
		if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if( wfd.cFileName[0] != '.' )
			{
				char pctmp[64];
				sprintf( pctmp, "f%d", n++ );
				pA->SetAttribute( pctmp, wfd.cFileName );
			}
		}
	} while( api->fio->_FindNextFile(h,&wfd) );
	if( h!=INVALID_HANDLE_VALUE ) api->fio->_FindClose(h);
	pDat = (VDATA*)pS->Push();	if (!pDat) return IFUNCRESULT_FAILED;
	long nSuccess = (pA->GetAttributesNum()>0);
	pDat->Set( nSuccess );
	return IFUNCRESULT_OK;
}

DWORD __cdecl _DialogAssembleStr(VS_STACK * pS)
{
	VDATA * pDat;
	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	const char* pcParam = pDat->GetString();
	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	const char* pcID = pDat->GetString();

	char param[4096];
	const char* pcStr = g_StringServicePointer ? ((STRSERVICE*)g_StringServicePointer)->TranslateForDialog(pcID) : pcID;
	if( pcStr ) {
		array<QUEST_FILE_READER::UserData> aUserData(_FL);
		if(pcParam) QUEST_FILE_READER::FillUserDataList( (char*)pcParam, aUserData );
		QUEST_FILE_READER::AssembleStringToBuffer(pcStr,strlen(pcStr), param,sizeof(param), aUserData);
		aUserData.DelAll();
	} else param[0] = 0;

	pDat = (VDATA*)pS->Push();	if (!pDat) return IFUNCRESULT_FAILED;
	pDat->Set(param);
	return IFUNCRESULT_OK;
}

DWORD __cdecl _DialogAddParamToStr(VS_STACK * pS)
{
	VDATA * pDat;
	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	const char* pcDatVal = pDat->GetString();
	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	const char* pcDatID = pDat->GetString();
	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	const char* pcSrcStr = pDat->GetString();

	char param[4096];
	param[0] = 0;
	if( pcDatID && pcDatVal ) {
		if( pcSrcStr && pcSrcStr[0]!=0 ) {
			_snprintf( param,sizeof(param), "%s@<%s>%s", pcSrcStr,pcDatID, pcDatVal );
		} else {
			_snprintf( param,sizeof(param), "@<%s>%s", pcDatID, pcDatVal );
		}
	}

	pDat = (VDATA*)pS->Push();	if (!pDat) return IFUNCRESULT_FAILED;
	pDat->Set(param);
	return IFUNCRESULT_OK;
}

DWORD __cdecl _StoreNodeLocksWithOff(VS_STACK * pS)
{
	long nStoreIndex = -1;
	if( XINTERFACE::pThis != 0 )
		nStoreIndex = XINTERFACE::pThis->StoreNodeLocksWithOff();
	VDATA * pDat = (VDATA*)pS->Push();	if (!pDat) return IFUNCRESULT_FAILED;
	pDat->Set(nStoreIndex);
	return IFUNCRESULT_OK;
}

DWORD __cdecl _RestoreNodeLocks(VS_STACK * pS)
{
	VDATA * pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	long nStoreIndex = pDat->GetLong();
	if( XINTERFACE::pThis != 0 ) XINTERFACE::pThis->RestoreNodeLocks( nStoreIndex );
	return IFUNCRESULT_OK;
}

DWORD __cdecl _IsKeyPressed(VS_STACK * pS)
{
	// get input data
	VDATA * pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	char * strKeyName=0;	pDat->Get(strKeyName);
	// data process
	bool bIsPressed = false;
	if( strKeyName ) {
		if( stricmp(strKeyName,"shift")==0 ) {
			bIsPressed = (GetAsyncKeyState(VK_SHIFT) < 0);
		} else if( stricmp(strKeyName,"control")==0 ) {
			bIsPressed = (GetAsyncKeyState(VK_CONTROL) < 0);
		} else if( stricmp(strKeyName,"alt")==0 ) {
			bIsPressed = (GetAsyncKeyState(VK_MENU) < 0);
		}
	}
	// set return data
	pDat = (VDATA*)pS->Push();	if (!pDat) return IFUNCRESULT_FAILED;
	pDat->Set( (long)bIsPressed );
	return IFUNCRESULT_OK;
}

DWORD __cdecl _RegistryExitKey(VS_STACK * pS)
{
	// get input data
	VDATA * pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	char * strKeyName=0;	pDat->Get(strKeyName);
	if( XINTERFACE::pThis != 0 )
		XINTERFACE::pThis->RegistryExitKey( strKeyName );
	return IFUNCRESULT_OK;
}

DWORD __cdecl _AddControlTreeNode(VS_STACK * pS)
{
	VDATA * pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	float fTimeOut = pDat->GetFloat();

	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	char* pcOutControl;		pDat->Get(pcOutControl);

	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	char* pcBaseControl;	pDat->Get(pcBaseControl);

	pDat = (VDATA*)pS->Pop();	if (!pDat) return IFUNCRESULT_FAILED;
	long nParent = pDat->GetLong();

	long nNodIdx = api->Controls->AddControlTreeNode(nParent, pcBaseControl, pcOutControl, fTimeOut);

	// set return data
	pDat = (VDATA*)pS->Push();	if (!pDat) return IFUNCRESULT_FAILED;
	pDat->Set( nNodIdx );

	return IFUNCRESULT_OK;
}


//==============================================================
bool SCRIPT_INTERFACE_FUNCTIONS::Init()
{
	IFUNCINFO sIFuncInfo;

	sIFuncInfo.pReturnValueName = "string";
	sIFuncInfo.pFuncName = "LanguageGetLanguage";
	sIFuncInfo.nArguments = 0;
	sIFuncInfo.pFuncAddress = _Language_GetLanguage;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "int";
	sIFuncInfo.pFuncName = "LanguageOpenFile";
	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncAddress = _Language_OpenFile;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncName = "LanguageCloseFile";
	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncAddress = _Language_CloseFile;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "string";
	sIFuncInfo.pFuncName = "LanguageConvertString";
	sIFuncInfo.nArguments = 2;
	sIFuncInfo.pFuncAddress = _Language_ConvertString;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncName = "LanguageSetLanguage";
	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncAddress = _Language_SetLanguage;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "string";
	sIFuncInfo.pFuncName = "XI_ConvertString";
	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncAddress = _XI_ConvertString;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "int";
	sIFuncInfo.pFuncName = "GlobalLngFileID";
	sIFuncInfo.nArguments = 0;
	sIFuncInfo.pFuncAddress = _GlobalLngFileID;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "string";
	sIFuncInfo.pFuncName = "LanguageGetFaderPic";
	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncAddress = _LanguageGetFaderPic;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncName = "XI_SetColorCorrection";
	sIFuncInfo.nArguments = 3;
	sIFuncInfo.pFuncAddress = _SetColorCorrection;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncName = "XI_SetMouseSensitivity";
	sIFuncInfo.nArguments = 2;
	sIFuncInfo.pFuncAddress = _SetMouseSensitivity;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncName = "XI_ControlMakeInvert";
	sIFuncInfo.nArguments = 2;
	sIFuncInfo.pFuncAddress = _ControlMakeInvert;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncName = "XI_MakeNode";
	sIFuncInfo.nArguments = 4;
	sIFuncInfo.pFuncAddress = _InterfaceMakeNode;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncName = "XI_DeleteNode";
	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncAddress = _InterfaceDeleteNode;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncName = "XI_WindowShow";
	sIFuncInfo.nArguments = 2;
	sIFuncInfo.pFuncAddress = _InterfaceWindowShow;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncName = "XI_WindowDisable";
	sIFuncInfo.nArguments = 2;
	sIFuncInfo.pFuncAddress = _InterfaceWindowDisable;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "bool";
	sIFuncInfo.pFuncName = "XI_IsWindowEnable";
	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncAddress = _InterfaceIsWindowEnable;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncName = "XI_WindowAddNode";
	sIFuncInfo.nArguments = 2;
	sIFuncInfo.pFuncAddress = _InterfaceWindowAddNode;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "bool";
	sIFuncInfo.pFuncName = "XI_CreateFolder";
	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncAddress = _InterfaceCreateFolder;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "bool";
	sIFuncInfo.pFuncName = "XI_CheckFolder";
	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncAddress = _InterfaceCheckFolder;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "bool";
	sIFuncInfo.pFuncName = "XI_DeleteFolder";
	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncAddress = _InterfaceDeleteFolder;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "bool";
	sIFuncInfo.pFuncName = "XI_FindFolders";
	sIFuncInfo.nArguments = 2;
	sIFuncInfo.pFuncAddress = _InterfaceFindFolders;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "string";
	sIFuncInfo.pFuncName = "DialogAssembleStr";
	sIFuncInfo.nArguments = 2;
	sIFuncInfo.pFuncAddress = _DialogAssembleStr;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "string";
	sIFuncInfo.pFuncName = "DialogAddParamToStr";
	sIFuncInfo.nArguments = 3;
	sIFuncInfo.pFuncAddress = _DialogAddParamToStr;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "int";
	sIFuncInfo.pFuncName = "XI_StoreNodeLocksWithOff";
	sIFuncInfo.nArguments = 0;
	sIFuncInfo.pFuncAddress = _StoreNodeLocksWithOff;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncName = "XI_RestoreNodeLocks";
	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncAddress = _RestoreNodeLocks;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "bool";
	sIFuncInfo.pFuncName = "XI_IsKeyPressed";
	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncAddress = _IsKeyPressed;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncName = "XI_RegistryExitKey";
	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncAddress = _RegistryExitKey;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.pReturnValueName = "int";
	sIFuncInfo.pFuncName = "AddControlTreeNode";
	sIFuncInfo.nArguments = 4;
	sIFuncInfo.pFuncAddress = _AddControlTreeNode;
	api->SetScriptFunction(&sIFuncInfo);

	return true;
}
//===============================================================
//===============================================================
