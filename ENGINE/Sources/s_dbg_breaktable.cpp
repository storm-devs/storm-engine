#include "s_dbg_breaktable.h"

extern CORE Core;
extern FILE_SERVICE File_Service;

#define SECTION_NAME "breakpoints"

BREAKPOINTS_TABLE::BREAKPOINTS_TABLE()
{
	fio = &File_Service;
	nPoints = 0;
	pTable = 0;
	ProjectName[0] = 0;
	//bReleased = false;
}

BREAKPOINTS_TABLE::~BREAKPOINTS_TABLE()
{
	Release();
}

void BREAKPOINTS_TABLE::Release()
{
	INIFILE * ini;
	DWORD n;
	char buffer[MAX_PATH];


	//if(nPoints)
	if(ProjectName[0] != 0)
	{
		ini = fio->OpenIniFile(ProjectName);
		if(!ini) ini = fio->CreateIniFile(ProjectName,false);
		if(ini)
		{
			ini->DeleteSection(SECTION_NAME);
			for(n=0;n<nPoints;n++)
			{
				if(!pTable[n].pFileName) continue;
				sprintf(buffer,"%s,%d",pTable[n].pFileName,pTable[n].nLineNumber);
				ini->AddString(SECTION_NAME,"B",buffer);
			}
			delete ini;
		}
		ProjectName[0] = 0;
	}

	if(pTable)
	{
		for(n=0;n<nPoints;n++)
		{
			if(pTable[n].pFileName) delete pTable[n].pFileName;
		}
		delete pTable;
	}
	pTable = 0;
	nPoints = 0;
	//bReleased = true;
}

void BREAKPOINTS_TABLE::UpdateProjectFile()
{
	INIFILE * ini;
	DWORD n;
	char buffer[MAX_PATH];

	
	if(ProjectName[0] != 0)
	{
		ini = fio->OpenIniFile(ProjectName);
		if(!ini) ini = fio->CreateIniFile(ProjectName,false);
		if(ini)
		{
			ini->DeleteSection(SECTION_NAME);
			for(n=0;n<nPoints;n++)
			{
				if(!pTable[n].pFileName) continue;
				sprintf(buffer,"%s,%d",pTable[n].pFileName,pTable[n].nLineNumber);
				ini->AddString(SECTION_NAME,"B",buffer);
			}
			delete ini;
		}
	}
}

bool MakeLineCode(char * buffer, DWORD & nLineCode)
{
	DWORD n;
	if(buffer == 0) return false;
	n = 0;
	while(buffer[n])
	{
		if(buffer[n] == ',')
		{
			if(buffer[n + 1] == 0) return false;
			nLineCode = atol(&buffer[n]+1);
			buffer[n] = 0;
			return true;
		}
		n++;
	}
	return false;
}

bool BREAKPOINTS_TABLE::ReadProject(char * filename)
{
	INIFILE * ini;
	char buffer[MAX_PATH];
	
	DWORD nLineNumber;

	Release();

	ini = fio->OpenIniFile(filename);
	if(ini)
	{
		strcpy(ProjectName,filename);
		if(ini->ReadString(SECTION_NAME,"B",buffer,sizeof(buffer),""))
		{
			if(MakeLineCode(buffer,nLineNumber)) AddBreakPoint(buffer,nLineNumber);
			while(ini->ReadStringNext(SECTION_NAME,"B",buffer,sizeof(buffer)))
			{
				if(MakeLineCode(buffer,nLineNumber)) 
					AddBreakPoint(buffer,nLineNumber);
			}
		}
		delete ini;
		//bReleased = false;
		return true;
	}
	return false;
}

void BREAKPOINTS_TABLE::AddBreakPoint(const char * filename, DWORD line)
{
	DWORD n;

	if(filename == 0) return;
	
	for(n=0;n<nPoints;n++)
	{
		if(pTable[n].nLineNumber != line) continue;
		if(stricmp(pTable[n].pFileName,filename) != 0) continue;
		return;	// already in list
	}

	pTable = (BREAKPOINT_DESC *)RESIZE(pTable,(nPoints + 1)*sizeof(BREAKPOINT_DESC));
	pTable[nPoints].nLineNumber = line;

	pTable[nPoints].pFileName = NEW char[strlen(filename)+1];
	strcpy(pTable[nPoints].pFileName,filename);
	nPoints++;

	//UpdateProjectFile();
}

void BREAKPOINTS_TABLE::FlipBreakPoint(const char * filename, DWORD line)
{
	if(Find(filename,line)) DelBreakPoint(filename,line);
	else AddBreakPoint(filename,line);
}

void BREAKPOINTS_TABLE::DelBreakPoint(const char * filename, DWORD line)
{
	DWORD n;
	if(filename == 0) return;
	for(n=0;n<nPoints;n++)
	{
		if(pTable[n].nLineNumber != line) continue;
		if(stricmp(pTable[n].pFileName,filename) != 0) continue;

		if(pTable[n].pFileName) delete pTable[n].pFileName;

		for(;n<(nPoints - 1);n++)
		{
			pTable[n] = pTable[n + 1];
		}
		nPoints--;

		//UpdateProjectFile();

		return;
	}
}
bool BREAKPOINTS_TABLE::Find(const char * filename, DWORD line)
{
	DWORD n;
	if(filename == 0) return false;
	for(n=0;n<nPoints;n++)
	{
		if(pTable[n].nLineNumber != line) continue;
		if(stricmp(pTable[n].pFileName,filename) != 0) continue;

		return true;
	}
	return false;
}