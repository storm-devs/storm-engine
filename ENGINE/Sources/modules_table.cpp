#include "modules_table.h"
#include "externs.h"
#include "messages.h"
#include "internal_modules.h"
#include "..\..\common_h\exs.h"
#include "..\..\common_h\memop.h"


#define MROOT_DIRECTORY		0xffffffff
#define MODULE_WILD_MASK	"*.dll"


#ifdef _XBOX

MODULES_TABLE::MODULES_TABLE() {};
MODULES_TABLE::~MODULES_TABLE(){};
void MODULES_TABLE::Release(){};
bool MODULES_TABLE::SetModulesPath(char * _name){return false;}
bool MODULES_TABLE::AddModulesPath(char * _name){return false;}
bool MODULES_TABLE::LoadModule(char * _mname, long path_code, MODULE_STATE& ms){return false;}
VMA * MODULES_TABLE::GetClassesRoot(dword _n){return 0;}
dword MODULES_TABLE::GetModulesCount(){return 0;}
void MODULES_TABLE::Load_ModulesTable(){};
char * MODULES_TABLE::GetPath(dword path_code){return 0;}

#else

MODULES_TABLE::MODULES_TABLE() 
{
	Paths_Count = 0;
	Paths_Table = 0;
	pTable = 0;
	nModulesNum = 0;
}

MODULES_TABLE::~MODULES_TABLE()
{
	Release();
}

void MODULES_TABLE::Release()
{
	dword n;

	if(pTable)
	{
		for(n = 0; n < nModulesNum; n++) FreeLibrary(pTable[n].hInst);
		delete pTable;
		pTable = 0;
		nModulesNum = 0;
	}

	if(Paths_Table)
	{
		for(n=0;n<Paths_Count;n++) delete Paths_Table[n];
		delete Paths_Table;
	}
	Paths_Count = 0;
	Paths_Table = null;

}

bool MODULES_TABLE::SetModulesPath(char * _name)
{
	GUARD(MODULES_TABLE::SetModulesPath)
	dword n;
	if(Paths_Table)
	{
		for(n=0;n<Paths_Count;n++) delete Paths_Table[n];
		delete Paths_Table;
	}
	Paths_Count = 0;
	Paths_Table = null;
	if(_name == null) return true;


	Paths_Table = (char **)NEW char[sizeof(char *)];
	if(Paths_Table == null) THROW;

	Paths_Table[0] = (char *)NEW char[strlen(_name) + 1];
	if(Paths_Table[0] == null) THROW;
	strcpy(Paths_Table[0],_name);
	Paths_Count = 1;
	UNGUARD
	return true;
}

bool MODULES_TABLE::AddModulesPath(char * _name)
{
	GUARD(MODULES_TABLE::AddModulesPath)
	if(Paths_Table == null) return SetModulesPath(_name);
	if(_name == null) return false;
	Paths_Table = (char **)RESIZE(Paths_Table,(Paths_Count + 1)*sizeof(char *));
	if(Paths_Table == null) THROW;

	Paths_Table[Paths_Count] = (char *)NEW char[strlen(_name) + 1];
	if(Paths_Table[Paths_Count] == null) THROW;
	strcpy(Paths_Table[Paths_Count],_name);
	Paths_Count++;
	UNGUARD
	return true;
}

bool MODULES_TABLE::LoadModule(char * _mname, long path_code, MODULE_STATE& ms)
{

	GUARD(MODULES_TABLE::LoadModule)
	HINSTANCE hInst;
	DLLAPIFUNC api_func_PTR;
//	VMODULE_API * mapi_PTR;

	char full_name[_MAX_PATH];
	
	if(_mname == null) return false;

	// invalid path code
	if((dword)path_code >= Paths_Count) {SET_ERROR("invalid path"); return false;}

	if(strlen(GetPath(path_code)) > 0) wsprintf(full_name,"%s\\%s",GetPath(path_code),_mname);
	else strcpy(full_name,_mname);

	//gdi_display.Print("Load libriary: %s",full_name);
	// load library
	hInst = LoadLibrary(full_name);
	if(hInst == null){SET_ERROR("cant load libriary"); return false;}
	ms.hInst = hInst;
	//gdi_display.Print("load ok");
	// get procedure address
	api_func_PTR = (DLLAPIFUNC)GetProcAddress(hInst,"DMAInterface");
	if(api_func_PTR == null) {SET_ERROR("interface function not found"); FreeLibrary(hInst); return false; }
	ms.api_func_PTR = api_func_PTR;
	UNGUARD
	return true;
	
}

VMA * MODULES_TABLE::GetClassesRoot(dword _n)
{
	if(_n >= nModulesNum) return 0;
	return pTable[_n].api_func_PTR(_CORE_API,_VSYSTEM_API);
}

dword MODULES_TABLE::GetModulesCount()
{
	return nModulesNum;
}

void __declspec(noinline) __declspec(dllexport) __cdecl MODULES_TABLE::Load_ModulesTable()
{

	WIN32_FIND_DATA find_data;
	HANDLE hFile;
	long Path_scan;
	long counter;
	long n,i;
	char full_name[_MAX_PATH];
	dword path_code;
	MODULE_STATE ms;

	Path_scan = Paths_Count;
	if(Path_scan == 0) Path_scan = 1;
	for(n=0;n<Path_scan;n++)
	{
		// searching modules directory for modules
		if(Paths_Table == null)
		{
			full_name[0] = 0;
			path_code = n;
			gdi_display.Print("Scanning current directory");
		}
		else
		{
			strcpy(full_name,Paths_Table[n]);
			path_code = n;
			gdi_display.Print("Scanning %s",Paths_Table[n]);
		}
		
		strcat(full_name,MODULE_WILD_MASK);
		gdi_display.Print_Add("Loading modules...");

		//char gs[256];
		//File_Service._GetCurrentDirectory(sizeof(gs),gs);
		// load modules names into modules table (fixing codes)
		hFile = File_Service._FindFirstFile(full_name,&find_data);

		counter = 0;
		if(hFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				gdi_display.Set_Text(find_data.cFileName);
				
				ZeroMemory(&ms,sizeof(ms));
				if(LoadModule(find_data.cFileName,path_code,ms))
				{
					i = nModulesNum;
					nModulesNum++;
					pTable = (MODULE_STATE *)RESIZE(pTable,sizeof(MODULE_STATE) * nModulesNum);
					pTable[i] = ms;
					pTable[i].api_func_PTR = ms.api_func_PTR;
					pTable[i].hInst = ms.hInst;
					counter++;
				} 
				else
				{
					gdi_display.Print(CMS_MODULE_SKIPPED,find_data.cFileName);
					gdi_display.Print(GET_ERROR);
					Sleep(ERROR_MESSAGE_DELAY);
				}
				
			} while(File_Service._FindNextFile(hFile,&find_data));
			File_Service._FindClose(hFile);
		}
		
		gdi_display.Print("Loaded %d",counter);
	}
}

char * MODULES_TABLE::GetPath(dword path_code)
{
	if(path_code >= Paths_Count) return null;
	return Paths_Table[path_code];
}

#endif

/*
#ifdef _XBOX
#include "..\..\xengine\sources\xboxdll.h"
#endif

#define MROOT_DIRECTORY		0xffffffff
#define MODULE_WILD_MASK	"*.dll"

MODULES_TABLE::MODULES_TABLE() 
{
	list.SetStringDataSize(sizeof(MODULE_STATE));
	Paths_Count = 0;
	Paths_Table = null;
}

MODULES_TABLE::~MODULES_TABLE()
{
	dword n;
	list.Release();
	if(Paths_Table)
	{
		for(n=0;n<Paths_Count;n++) delete Paths_Table[n];
		delete Paths_Table;
	}
}

dword  MODULES_TABLE::GetModuleCode(char * _char_PTR)
{ 
	return list.GetStringCode(_char_PTR);
}

bool MODULES_TABLE::ServiceFlag(dword code)
{
	MODULE_STATE ms;
	if(!list.GetStringData(code,&ms)) return false;
	return ms.service;
}

VMODULE_API * MODULES_TABLE::GetModuleAPI(dword code)
{
	MODULE_STATE ms;
	if(!list.GetStringData(code,&ms)) return null;
	return ms.mapi_PTR;
}

bool MODULES_TABLE::AddModule(char * _char_PTR, dword path_code)
{ 
	MODULE_STATE ms;
	dword code;
	if(!list.AddUnicalString(_char_PTR)) return false;
	code = GetModuleCode(_char_PTR);
	if(code == INVALID_MODULE_CODE) return false;
	if(!list.GetStringData(code,&ms)) return false;
	ms.reference = 0;
	ms.hInst = null;
	ms.path_code = path_code;
	list.SetStringData(code,&ms);	// ?? check
	return true;
}

void MODULES_TABLE::Release()
{
	dword n;
	MODULE_STATE ms;
	for(n=0;n<list.GetStringsCount();n++)
	{
		if(!list.GetStringData(n,&ms)) continue;	// ... sign error! ->
		if(ms.hInst != null) 
		{
			if(ms.mapi_PTR) delete ms.mapi_PTR;
			FreeLibrary(ms.hInst);
		}
	}
	list.Release();
	if(Paths_Table)
	{
		for(n=0;n<Paths_Count;n++) delete Paths_Table[n];
		delete Paths_Table;
	}
	Paths_Count = 0;
	Paths_Table = null;
}
dword MODULES_TABLE::GetModulesCount()
{
	return list.GetStringsCount();
}

char * MODULES_TABLE::GetModuleName(dword code)
{
	MODULE_STATE ms;
	if(!list.GetStringData(code,&ms)) 
	{
		SET_ERROR("invalid module code");
		return null;
	}
	if(ms.path_code >= Paths_Count) 
	{
		SET_ERROR("invalid module path code");
		return null;
	}
	wsprintf(gstring,"%s\\%s",GetPath(ms.path_code),list.GetString(code));

	return gstring;
}

bool MODULES_TABLE::SetModulesPath(char * _name)
{
	GUARD(MODULES_TABLE::SetModulesPath)
	dword n;
	if(Paths_Table)
	{
		for(n=0;n<Paths_Count;n++) delete Paths_Table[n];
		delete Paths_Table;
	}
	Paths_Count = 0;
	Paths_Table = null;
	if(_name == null) return true;

	Paths_Table = (char **)new char[sizeof(char *)];
	if(Paths_Table == null) THROW;
	Paths_Table[0] = (char *)new char[strlen(_name) + 1];
	if(Paths_Table[0] == null) THROW;
	strcpy(Paths_Table[0],_name);
	Paths_Count = 1;
	UNGUARD
	return true;
}

bool MODULES_TABLE::AddModulesPath(char * _name)
{
	GUARD(MODULES_TABLE::AddModulesPath)
	if(Paths_Table == null) return SetModulesPath(_name);
	if(_name == null) return false;
	Paths_Table = (char **)RESIZE(Paths_Table,(Paths_Count + 1)*sizeof(char *));
	if(Paths_Table == null) THROW;
	Paths_Table[Paths_Count] = (char *)new char[strlen(_name) + 1];
	if(Paths_Table[Paths_Count] == null) THROW;
	strcpy(Paths_Table[Paths_Count],_name);
	Paths_Count++;
	UNGUARD
	return true;
}

dword MODULES_TABLE::GetPathsCount()
{
	return Paths_Count;
}
char * MODULES_TABLE::GetPath(dword path_code)
{
	if(path_code >= Paths_Count) return null;
	return Paths_Table[path_code];
}

bool MODULES_TABLE::LoadModule(char * _mname, long path_code, MODULE_STATE& ms)
{
	GUARD(MODULES_TABLE::LoadModule)
	HINSTANCE hInst;
	DLLAPIFUNC api_func_PTR;
	VMODULE_API * mapi_PTR;

	char full_name[_MAX_PATH];
	
	if(_mname == null) return false;

	// invalid path code
	if((dword)path_code >= Paths_Count) {SET_ERROR("invalid path"); return false;}

	if(strlen(GetPath(path_code)) > 0) wsprintf(full_name,"%s\\%s",GetPath(path_code),_mname);
	else strcpy(full_name,_mname);

	//gdi_display.Print("Load libriary: %s",full_name);
	// load library
	hInst = LoadLibrary(full_name);
	if(hInst == null){SET_ERROR("cant load libriary"); return false;}
	ms.hInst = hInst;
	//gdi_display.Print("load ok");
	// get procedure address
	api_func_PTR = (DLLAPIFUNC)GetProcAddress(hInst,"DMAInterface");
	if(api_func_PTR == null) {SET_ERROR("interface function not found"); FreeLibrary(hInst); return false; }
	ms.api_func_PTR = api_func_PTR;

	#ifndef EX_OFF
	try {
	#endif
	mapi_PTR = api_func_PTR(_CORE_API,_VSYSTEM_API); 
	// can't create interface class in module: incorrect module
	if(mapi_PTR == null){ SET_ERROR("can't create module api class"); FreeLibrary(hInst); return false;}
	ms.mapi_PTR = mapi_PTR;
	
	// module compiled with other version of api
	if(mapi_PTR->Build_Version() != MODULE_API_VERSION)
		{ SET_ERROR("invalid module api version"); FreeLibrary(hInst); return false;}
	#ifndef EX_OFF
	}
	catch(...)
	{
		SET_ERROR("invalid module"); FreeLibrary(hInst); return false;
	}
	#endif
	UNGUARD
	return true;
}

long MODULES_TABLE::Load_ModulesTable()
{
	WIN32_FIND_DATA find_data;
	HANDLE hFile;
	long Path_scan;
	long counter;
	long n;
	long module_code;
	char full_name[_MAX_PATH];
	
	dword path_code;
	MODULE_STATE ms;

#ifdef _XBOX	
	// load internal (linked) classes module
	ZeroMemory(&ms,sizeof(ms));
	ms.mapi_PTR = (VMODULE_API *) new INTERNAL_MODULES;
	if(ms.mapi_PTR == null) THROW;
	if(!AddModule("internal",0)) THROW;
	module_code = GetModuleCode("internal");
	if(!list.SetStringData(module_code,&ms)) THROW;//throw "SYS ERROR"; 
#endif

	Path_scan = Paths_Count;
	if(Path_scan == 0) Path_scan = 1;
	for(n=0;n<Path_scan;n++)
	{
		// searching modules directory for modules
		if(Paths_Table == null)
		{
			full_name[0] = 0;
			path_code = n;
			gdi_display.Print("Scanning current directory");
		}
		else
		{
			strcpy(full_name,Paths_Table[n]);
			path_code = n;
			gdi_display.Print("Scanning %s",Paths_Table[n]);
		}
		
		strcat(full_name,MODULE_WILD_MASK);

		hFile = File_Service._FindFirstFile(full_name,&find_data);
		if(hFile == INVALID_HANDLE_VALUE) 
		{
			// no modules in this directory
			gdi_display.Print_Add(" no modules");
			continue;	
		}
		
		counter = 0;
		do
		{
			counter++;
			gdi_display.Set_Text("%s %s",CMS_MODULE_FOUND,find_data.cFileName);
		} while(File_Service._FindNextFile(hFile,&find_data));
		File_Service._FindClose(hFile);
	
		gdi_display.Print_Add(CMS_NN_FOUND,counter);
		gdi_display.Print_Add(", loading...");


		// load modules names into modules table (fixing codes)
		hFile = File_Service._FindFirstFile(full_name,&find_data);
		counter = 0;
		do
		{
			gdi_display.Set_Text(find_data.cFileName);
			
			ZeroMemory(&ms,sizeof(ms));
			if(LoadModule(find_data.cFileName,path_code,ms))
			{
				
				if(!AddModule(find_data.cFileName,path_code))
				{
					gdi_display.Print(CMS_MODULE_SKIPPED,find_data.cFileName);
					gdi_display.Print(CMS_REASON_ALREADY_IN_LIST);
				} 
				else 
				{
					module_code = GetModuleCode(find_data.cFileName);
					ms.path_code = path_code;
					if(!list.SetStringData(module_code,&ms)) THROW;//throw "SYS ERROR"; 
					counter++;
				}

			} else
			{
				gdi_display.Print(CMS_MODULE_SKIPPED,find_data.cFileName);
				gdi_display.Print(GET_ERROR);
				Sleep(ERROR_MESSAGE_DELAY);
			}

			
		} while(File_Service._FindNextFile(hFile,&find_data));

		File_Service._FindClose(hFile);
		if(TST_ERROR) gdi_display.Print("Loaded %d",counter);
		else gdi_display.Print_Add(" loaded %d",counter);
	}
	gdi_display.Print("Modules: %d",GetModulesCount());
	return true;

}

//*/