#include "core.h"
#include "..\..\common_h\vmodule_api.h"
#include "messages.h"
#include "vbuffer.h"
#include "..\..\common_h\memop.h"

#include "..\..\common_h\dx8render.h"

#define CORE_MODULE_WILD_MASK		"*.dll"
#define CORE_DEFAULT_MODULES_PATH	"Modules\\"
#define CORE_DEFAULT_ATOMS_SPACE	128
#define SYSTEM_CRUSH				"SYSTEM_CRUSH"
#define SERVICE_REFERENCE_TRACK		"SRT"
//#define STATE_COMPRESSION_ON		
//#define ENGINE_INI_FILE_NAME			"engine.ini"
extern char ENGINE_INI_FILE_NAME[256];
extern bool bDebugWindow, bAcceleration;
extern bool bNetActive;

#define RDTSCB(x)	{ _asm rdtsc _asm mov x,eax }
#define RDTSCE(x)	{ _asm rdtsc _asm sub eax,x _asm mov x,eax }

// system layer flags

extern bool bActive;
extern VMA * _pModuleClassRoot;
#define SERVICES_REFERENCE_FLAGS	0x1

CREATE_SERVICE(CONTROLS)
#ifdef _XBOX
//dword ATTRIBUTES::nMemSize = 0;
#endif
// this macro incure, that programm control didnt pass via any entity constructor. Each base api function must
// be protected by this macro on development stage. On release stage this macro changed to empty block
#define VALIDATE_API_CALLS {if(Constructor_counter) _THROW(api call from constructor);}	// develop version
//#define VALIDATE_API_CALLS {}	// release version

dword dwNumberScriptCommandsExecuted = 0;


typedef struct 
{
	dword  code;
	void * pointer;
} CODE_AND_POINTER;

CORE::CORE()
{
	Reset_flag = false;
	Initialized = false;
	bEngineIniProcessed = false;
	App_Hwnd = null;
	State_file_name = null;
	ZeroMemory(&CoreState,sizeof(CoreState));
	CoreState.engine_version = ENGINE_VERSION;
	Root_flag = false;
	Atoms_PTR = null;
	SystemMessagesNum = 0;
	Exit_flag = false;
	State_loading = false;
	Constructor_counter = 0;
	Memory_Leak_flag = false;
	Controls = 0;
	fTimeScale = 1.0f;	
	bNetActive = false;
	nSplitScreenshotGrid = 4;
}

CORE::~CORE()
{
	
}

//-------------------------------------------------------------------------------------------------
// external entry points
//-------------------------------------------------------------------------------------------------
void CORE::ResetCore()
{
	GUARD(CORE::ResetCore)

	Reset_flag = false;
	gdi_display.Print(CMS_RESET);
	Initialized = false;
	bEngineIniProcessed = false;

	ReleaseAtoms();
	ReleaseServices();
	ReleaseLayers();

	gdi_display.Switch(true);

	Services_List.Release();
	
	DeleteEntityList.Release();
	DeleteServicesList.Release();
	Services_List.Release();
	DeleteLayerList.Release();
	Control_Stack.Clear();
	
	SystemMessagesNum = 0;
	Scan_Layer_Code = INVALID_LAYER_CODE;

	if(State_file_name) delete(State_file_name); State_file_name = null;

	Constructor_counter = 0;

	Root_flag = false;

	UNGUARD
}

void CORE::CleanUp()
{
	//GUARD(CORE::CleanUp)
	dword n;
	
	Initialized = false;
	bEngineIniProcessed = false;
	
	Control_Stack.Clear();
	
	//Program.Release();
	
	if(Atoms_PTR)
	{
		for(n=0;n<=CoreState.Atoms_max_orbit;n++) 
		{
			if(Atoms_PTR[n] == null) continue;
			// if(Atoms_PTR[n]->as.Service) continue;
			
//			PUSH_CONTROL(Atoms_PTR[n]->atom_id.pointer,Atoms_PTR[n]->atom_id.class_code,CTP_DESTRUCTOR)
			/*#ifndef EX_OFF
			try {
			#endif*/
				EraseEntity(Atoms_PTR[n]->atom_id);
			/*#ifndef EX_OFF
			}
			catch(...)
			{
				TraceCurrent();
				Memory_Service.Free(Atoms_PTR[n]->atom_id.pointer);
				if(Atoms_PTR[n]) delete Atoms_PTR[n]; 
			}
			#endif*/
		}
	}

	ProcessDeleteList();

	//if(Controls) delete Controls; Controls = 0;
	ReleaseServices();
	ReleaseLayers();
	//Program.Release();
	Compiler.Release();
	
	

	
	if(Atoms_PTR) delete Atoms_PTR; Atoms_PTR = null;

	
	Services_List.Release();
	CheckMemoryLeak_Classes();
	DeleteEntityList.Release();
	DeleteServicesList.Release();
	Services_List.Release();
	DeleteLayerList.Release();
	//Control_Stack.Clear();
	if(State_file_name) delete State_file_name; State_file_name = null;
	
	//UNGUARD
}

void CORE::ReleaseServices()
{
	GUARD(CORE::ReleaseServices)
//	SERVICE * service_PTR;
//	dword class_code;

	FreeServices();

/*	service_PTR = Services_List.GetService(class_code);
	while(service_PTR)
	{
		//PUSH_CONTROL(service_PTR,class_code,CTP_DESTRUCTOR)
		#ifndef EX_OFF
		try {
		#endif
			delete service_PTR;
		#ifndef EX_OFF
		}
		catch(...)
		{
			trace("service release error");
			TraceCurrent();
			Memory_Service.Free(service_PTR);
		}
		#endif
		//POP_CONTROL(0)

		service_PTR = Services_List.GetServiceNext(class_code);
	}*/
	UNGUARD
}


void __declspec(noinline) __cdecl CORE::InitBase()
{
	INIFILE * engine_ini;
	char String[_MAX_PATH];

	GUARD(CORE::InitBase())
	
	engine_ini = File_Service.OpenIniFile(ENGINE_INI_FILE_NAME);
 	if(engine_ini != null) 
	{
		if(engine_ini->ReadString(0,"modules path",String,sizeof(String),""))
		{
			Modules_Table.SetModulesPath(String);
			while(engine_ini->ReadStringNext(0,"modules path",String,sizeof(String)))
			{
				Modules_Table.AddModulesPath(String);
			}
		}
		
		if(engine_ini->ReadString(0,"offclass",String,sizeof(String),""))
		{
			ClassesOff.AddString(String);
			while(engine_ini->ReadStringNext(0,"offclass",String,sizeof(String)))
			{
				ClassesOff.AddString(String);
			}
		}

		nSplitScreenshotGrid = engine_ini->GetLong( 0, "SplitScreenShotGrid", 4 );

		delete engine_ini;
	}
	Modules_Table.Load_ModulesTable();	// creating modules table
	LoadClassesTable();					// creating classes table
	UNGUARD
}

void CORE::ReleaseBase()
{
	Compiler.Token.Release();
	ClassesOff.Release();
	Modules_Table.Release();
}

bool __declspec(noinline) __cdecl CORE::LoCheck()
{
	ENTITY_ID test_eid;
	if (!CreateEntity(&test_eid, "LocationP")) return false;
	ENTITY * pE = (ENTITY *)test_eid.pointer;
	pE->Execute(ENGINE_SCRIPT_VERSION);
	DeleteEntity(test_eid);
	return true;
}

bool CORE::Run()
{
	GUARD(CORE::Run())

#ifndef _XBOX
	if (bDebugWindow && api->Controls && api->Controls->GetDebugAsyncKeyState(VK_F9)<0) DumpEntitiesInfo(); // boal
	dwNumberScriptCommandsExecuted = 0;
#endif

	if (bDebugWindow)
		for(dword n=0;n<CoreState.Atoms_max_orbit;n++) 
		{
			if(Atoms_PTR[n] == null) continue;
			Atoms_PTR[n]->as.Execute_ticks_av = 0;
			Atoms_PTR[n]->as.Realize_ticks_av = 0;
		}

/*
	CONTROL_STATE cs;

	if(Controls && Controls->GetControlState("dump",cs))
	{
		if(cs.state == CST_ACTIVATED) Memory_Service.DumpMemoryState();
	}
*/
		//trace("Allocated Memory: %f kb in %d block(s)",
	//	(Memory_Service.Allocated_memory_user + Memory_Service.Allocated_memory_system)/1024.0f,Memory_Service.Blocks);

	if(Exit_flag) return false;				// exit 
	if(Reset_flag) ResetCore();				// reset to initial state
	Timer.Run();							// calc delta time and programm fps

	Memory_Service.MemStat.ProcessTime(Timer.rDelta_Time);

	VDATA * pVCTime = (VDATA *)api->GetScriptVariable("iRealDeltaTime"); 
	if (pVCTime) pVCTime->Set((long)GetRDeltaTime());

	// setup current real computer time
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		VDATA * pVYear = (VDATA *)api->GetScriptVariable("iRealYear"); 
		VDATA * pVMonth = (VDATA *)api->GetScriptVariable("iRealMonth"); 
		VDATA * pVDay = (VDATA *)api->GetScriptVariable("iRealDay"); 

		if (pVYear) pVYear->Set(long(st.wYear));
		if (pVMonth) pVMonth->Set(long(st.wMonth));
		if (pVDay) pVDay->Set(long(st.wDay));
	}

	if (!bNetActive)
	{
#ifndef _XBOX
		if(bAcceleration && Controls && Controls->GetDebugAsyncKeyState('R')<0) Timer.Delta_Time *= 10;
		if(bAcceleration && Controls && Controls->GetDebugAsyncKeyState('Y')<0) Timer.Delta_Time = (DWORD)(Timer.Delta_Time * 0.2f);
#endif
		Timer.Delta_Time = (DWORD)(Timer.Delta_Time * fTimeScale);
		Timer.fDeltaTime *= fTimeScale;
	}

	VDATA * pVData = (VDATA *)GetScriptVariable("fHighPrecisionDeltaTime", null);
	if (pVData) pVData->Set(Timer.fDeltaTime * 0.001f);

	if(!Initialized)
	{
		Initialize();	// initialization at start or after reset
		if (!LoCheck())
			throw;
	}
	if(!bEngineIniProcessed) ProcessEngineIniFile();
	
	Compiler.ProcessFrame(Timer.GetDeltaTime());
	Compiler.ProcessEvent("frame");


	ProcessStateLoading();
	
	ProcessRunStart(SECTION_ALL);
	ProcessExecute();						// transfer control to objects via Execute() function
	ProcessRealize();						// transfer control to objects via Realize() function
	
	if (Controls && bActive) 
		Controls->Update(Timer.rDelta_Time);
	
	ProcessSystemMessagesBuffer();			// transfer control to objects via ProcessMessage() function
	ProcessDeleteList();					// delete objects marked for deletion	
	
	if (Controls && bActive) ProcessControls();
	
//	Compiler.ProcessFrame(Timer.Delta_Time);
//	Compiler.ProcessEvent("frame");

	ProcessRunEnd(SECTION_ALL);

	if( Controls && Controls->GetDebugAsyncKeyState(VK_F8)<0 &&
		Controls->GetDebugAsyncKeyState(VK_SHIFT)<0 &&
		Controls->GetDebugAsyncKeyState(VK_CONTROL)<0 )
	{
		Timer.Delta_Time = 0;
		Timer.fDeltaTime = 0.f;

		VDX8RENDER* pRender = (VDX8RENDER*)CreateService("DX8RENDER");
		CVECTOR pos,ang;
		float fPersp;
		pRender->GetCamera(pos,ang,fPersp);

		for( long y=0; y<nSplitScreenshotGrid; y++ )
			for( long x=0; x<nSplitScreenshotGrid; x++ )
			{
				CVECTOR tmpang = ang;
				tmpang.y += ((float)x - (nSplitScreenshotGrid-1)*0.5f) * (fPersp/nSplitScreenshotGrid);
				tmpang.x += ((float)y - (nSplitScreenshotGrid-1)*0.5f) * (fPersp/nSplitScreenshotGrid);
				pRender->SetCamera(&pos,&tmpang,fPersp/nSplitScreenshotGrid);
				pRender->SaveShoot();
				ProcessRealize();
			}

		pRender->SetCamera(&pos,&ang,fPersp);
	}

	CommonLayers.Clean();

	UNGUARD
	return true;
}

void CORE::ProcessControls()
{
	long n;
	CONTROL_STATE cs;
	USER_CONTROL  uc;

	if(!Controls) return;

	for(n=0;n<Controls->GetControlsNum();n++)
	{
		Controls->GetControlState(n,cs);
		if(cs.state == CST_ACTIVATED) 
		{
			Controls->GetControlDesc(n,uc);
			Event("Control Activation","s",uc.name);
		} 
		else
		if(cs.state == CST_INACTIVATED) 
		{
			Controls->GetControlDesc(n,uc);
			Event("Control Deactivation","s",uc.name);
		}

	}
}

static nOffSearch = 0;
void CORE::ProcessSystemMessage(UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	GUARD(CORE::ProcessSystemMessage)
	if(SystemMessagesNum >= SYSTEM_MESSAGE_STACK_SIZE) {trace("SMS overflaw");return;}
	MessageStack[SystemMessagesNum].iMsg = iMsg;
	MessageStack[SystemMessagesNum].lParam = lParam;
	MessageStack[SystemMessagesNum].wParam = wParam;
	SystemMessagesNum++;

	UNGUARD
}//*/

//-------------------------------------------------------------------------------------------------
// internal functions
//-------------------------------------------------------------------------------------------------
bool __declspec(noinline) __cdecl CORE::Initialize()
{
	INIFILE * engine_ini;
	char string[_MAX_PATH];

	GUARD(CORE::Initialize())

	ResetCore();
	gdi_display.Print(CMS_INITIALIZING_CORE);

	engine_ini = File_Service.OpenIniFile(ENGINE_INI_FILE_NAME);
 	if(engine_ini == null) _THROW(no 'engine.ini' file);

	if(!engine_ini->ReadString(0,"modules path",string,sizeof(string),""))
	{
		gdi_display.Print("No 'modules path' key in engine.ini");
		gdi_display.Print("Scanning current directory");

	} else
	{
		Modules_Table.SetModulesPath(string);	// set modules path
		while(engine_ini->ReadStringNext(0,"modules path",string,sizeof(string)))
		{
			Modules_Table.AddModulesPath(string);
		}
	}
	delete engine_ini;

	// create atoms space												
	if(!CreateAtomsTable(CORE_DEFAULT_ATOMS_SPACE)) return false;	

	DeleteServicesList.Init(sizeof(CODE_AND_POINTER),8);
	DeleteEntityList.Init(sizeof(ENTITY_ID),2);
	DeleteLayerList.Init(sizeof(dword),8);

	gdi_display.Print(CMS_INIT_COMPLETE);
	Initialized = true;

	//ProcessRootObjectCreation();
	
	//ProcessEngineIniFile();

	UNGUARD
	return true;
}

bool CORE::LoadCoreState(CORE_STATE cs)
{
	INIFILE * engine_ini;
	char string[_MAX_PATH];

	GUARD(CORE::LoadCoreState)
	ResetCore();
	gdi_display.Print("loading core state");

	engine_ini = File_Service.OpenIniFile(ENGINE_INI_FILE_NAME);
	if(engine_ini == null) _THROW(no 'engine.ini' file);

	if(!engine_ini->ReadString(0,"modules path",string,sizeof(string),0))
	{
		gdi_display.Print("No 'modules path' key in engine.ini");
		gdi_display.Print("Scanning current directory");

	} else
	{
		Modules_Table.SetModulesPath(string);	// set modules path
		while(engine_ini->ReadStringNext(0,"modules path",string,sizeof(string)))
		{
			Modules_Table.AddModulesPath(string);
		}
	}
	delete engine_ini;

	// pc/xbox mod - modules loaded on startup only and unloaded when app terminated
	//Modules_Table.LoadModulesTable();							// creating modules table
	//if(Modules_Table.GetModulesCount() == 0) _THROW(No modules to Run);

	//LoadClassesTable();											// creating classes table
	
	// create atoms space
	if(!CreateAtomsTable(cs.Atoms_space)) THROW;

	DeleteServicesList.Init(sizeof(CODE_AND_POINTER),8);
	DeleteEntityList.Init(sizeof(ENTITY_ID),2);
	DeleteLayerList.Init(sizeof(dword),8);

	CoreState = cs;

	Initialized = true;
	bEngineIniProcessed = true;

	UNGUARD
	return true;
}

void CORE::ProcessRootObjectCreation()
{
	GUARD(CORE::ProcessRootObjectCreation)

	//INIFILE * engine_ini;
	//char string[_MAX_PATH];

	if(Root_flag) return;

	/*engine_ini = File_Service.OpenIniFile(ENGINE_INI_FILE_NAME);
	if(engine_ini == null) _THROW(no 'engine.ini' file);

	if(!engine_ini->ReadString(0,"root object",string,sizeof(string),0))
	{
		delete engine_ini;
		_THROW(No 'root object' key in engine.ini);
	}
	delete engine_ini; 

	gdi_display.Print("Root object: %s",string);
	if(!CreateEntity(0,string)) _THROW(Cant create Root object);//*/
	

	//ProcessEngineIniFile();

	Root_flag = true;
	UNGUARD
}

void CORE::Execute(char * name)
{
	//Program.RunProgram(name);
}

void __declspec(noinline) __cdecl CORE::ProcessEngineIniFile()
{
	GUARD(CORE::ProcessEngineIniFile)
	char String[_MAX_PATH];	
	INIFILE * engine_ini;
	bool res;

	bEngineIniProcessed = true;

	engine_ini = File_Service.OpenIniFile(ENGINE_INI_FILE_NAME);
	if(engine_ini == null) _THROW(no 'engine.ini' file);

	res = engine_ini->ReadString(0,"program_directory",String,sizeof(String),"");
	if(res)
	{
		Compiler.SetProgramDirectory(String);
		//Program.SetProgramDirectory(String);
	}

#ifndef _XBOX
	res = engine_ini->ReadString(0,"controls",String,sizeof(String),"");
	if(res)
	{
		//if(Controls) delete Controls; Controls = 0;
		_CORE_API->Controls = (CONTROLS *)MakeClass(String);
		//if(_CORE_API->Controls == 0) _CORE_API->Controls = new CONTROLS;
		if(_CORE_API->Controls == 0) _CORE_API->Controls = (CONTROLS *)MakeClass("controls");
		
	}
	else
	{
		if(Controls) delete Controls; Controls = 0;

		_CORE_API->Controls = NEW CONTROLS;
	}
#else
	if(!Controls) Controls = (CONTROLS *)MakeClass("xbox_controls");
	if(!Controls) THROW("no xbox_controls");
#endif

	res = engine_ini->ReadString(0,"run",String,sizeof(String),"");
	if(res) 
	{
		//if(!Program.RunProgram(String)) _THROW(fail to run program);
		if(!Compiler.CreateProgram(String)) _THROW(fail to create program);
		if(!Compiler.Run()) _THROW(fail to run program);
		// Тест версии скрипта
		long iScriptVersion = 0xFFFFFFFF;
		VDATA * pVScriptVersion = (VDATA *)api->GetScriptVariable("iScriptVersion");
		if (pVScriptVersion) pVScriptVersion->Get(iScriptVersion);

		if (iScriptVersion != ENGINE_SCRIPT_VERSION)
		{
			ShowCursor(true);
			MessageBox(GetAppHWND(), "Wrong script version", "Error", MB_OK);
			Compiler.ExitProgram();
		}
	} 

	res = engine_ini->ReadString(0,"load class",String,sizeof(String),"");
	if(!res) 
	{
		//delete engine_ini;
		//_THROW(no class for loading);
	}
	while(res)
	{
		CreateEntity(0,String);
		res = engine_ini->ReadStringNext(0,"load class",String,sizeof(String));
	}

	delete engine_ini;
	UNGUARD
}

bool __declspec(noinline) __cdecl CORE::LoadClassesTable()
{
	GUARD(CORE::LoadClassesTable)
	VMA * pModuleClasses;
	VMA * pEClass;
	dword n;
	long hash;

	// set hash for internal classes
	pEClass = _pModuleClassRoot;
	if(pEClass)
	{
		hash = MakeHashValue(pEClass->GetName());

		if(ClassesOff.GetStringCode(pEClass->GetName()) != INVALID_ORDINAL_NUMBER) hash = 0;
		pEClass->SetHash(hash);
	}
	while(pEClass->Next())
	{
		pEClass = pEClass->Next();
		hash = MakeHashValue(pEClass->GetName());
		if(ClassesOff.GetStringCode(pEClass->GetName()) != INVALID_ORDINAL_NUMBER) hash = 0;
		pEClass->SetHash(hash);
	}

	// external classes
	for(n = 0; n < Modules_Table.GetModulesCount(); n++)
	{
		pModuleClasses = Modules_Table.GetClassesRoot(n);
		if(pModuleClasses)
		{
			pEClass = pModuleClasses;
			if(pEClass)
			{
				hash = MakeHashValue(pEClass->GetName());
				if(ClassesOff.GetStringCode(pEClass->GetName()) != INVALID_ORDINAL_NUMBER) hash = 0;
				pEClass->SetHash(hash);
			}
			while(pEClass->Next())
			{
				pEClass = pEClass->Next();
				hash = MakeHashValue(pEClass->GetName());
				if(ClassesOff.GetStringCode(pEClass->GetName()) != INVALID_ORDINAL_NUMBER) hash = 0;
				pEClass->SetHash(hash);
			}
			pEClass->Set(_pModuleClassRoot);
			_pModuleClassRoot = pModuleClasses;
		}
	}
	UNGUARD
	return true;
}


bool __declspec(noinline) __cdecl CORE::CreateAtomsTable(dword _space)
{
	GUARD(CORE::CreateAtomsTable)
	if(Atoms_PTR != null ) return false;

	gdi_display.Print(CMS_CREATE_ATOMS_SPACE,_space);

	// allocate space for atoms table
	CoreState.Atoms_space = _space;

	//Atoms_PTR = (C_ATOM * *)new char[CoreState.Atoms_space*sizeof(C_ATOM*)];
	Atoms_PTR = (C_ATOM * *)NEW char[CoreState.Atoms_space*sizeof(C_ATOM*)];

	if(Atoms_PTR == null) 
	{
		gdi_display.Print(CMS_ERROR); 
		return false;
	}
	else gdi_display.Print_Add(CMS_DONE);
	memset(Atoms_PTR,0,CoreState.Atoms_space*sizeof(C_ATOM*));
	ZeroMemory(&CoreState.Creation_Time,sizeof(CoreState.Creation_Time));

	UNGUARD
	return true;
}

void CORE::ReleaseLayers()
{
	GUARD(CORE::ReleaseLayers)
	CommonLayers.Release();
	UNGUARD
}

void CORE::ReleaseAtoms()
{
	GUARD(CORE::ReleaseAtoms)
	dword n;
	// release atoms and entity objects
	if(Atoms_PTR)
	{
		for(n=0;n<=CoreState.Atoms_max_orbit;n++) 
		{
			if(Atoms_PTR[n] == null) continue;
			//if(Atoms_PTR[n]->as.Service) continue; // free services later
			EraseEntity(Atoms_PTR[n]->atom_id);
		}
	}
	CoreState.Atoms_number = 0;
	CoreState.Atoms_max_orbit = 0;
	CoreState.Atoms_min_free_orbit = 0;
	if(Atoms_PTR) delete Atoms_PTR; Atoms_PTR = null;
	ZeroMemory(&CoreState.Creation_Time,sizeof(CoreState.Creation_Time));
	UNGUARD
}

// called from load state function
void CORE::RestoreEntity(ENTITY_ID entity_id,ATOM_STATE atom_state)
{
	/*
	GUARD(CORE::RestoreEntity) 
	CLASS_SEARCH_DATA class_search_data;
	C_ATOM * atom_PTR;
	VMODULE_API * mapi_PTR;
	ENTITY * Entity_PTR;
	dword class_code;
	
	class_code = entity_id.class_code;
	// access to class informatino
	if(!Classes_Table.GetStringData(class_code,&class_search_data)) _THROW(invalid class);
	
	// xbox
	if((dword)class_search_data.module_code >= Modules_Table.GetModulesCount())
	// load module
	//if(Modules_Table.ModuleReferenceInc(class_search_data.module_code) == 0) 
	{
		//trace("cant load libriary  %s : %s",Modules_Table.GetModuleName(class_search_data.module_code),Classes_Table.GetString(class_code));
		_THROW(invalid module code);
	}

	// create atom structure
	atom_PTR = FitAtom(entity_id,atom_state);
	if(atom_PTR == null) _THROW(cant create atom);

	// clear all layers attribute (object will be added via standart function)
	ZeroMemory(&atom_PTR->as.Layers_mask,sizeof(atom_PTR->as.Layers_mask));

	// obtain module interface class	
	mapi_PTR = Modules_Table.GetModuleAPI(class_search_data.module_code);
	if(mapi_PTR == null) 
	{
		trace("invalid module api class  %s : %s",Modules_Table.GetModuleName(class_search_data.module_code),Classes_Table.GetString(class_code));
		_THROW(invalid module);
	}
	
	// set current entity atom id pointer
	System_Api.entityID_PTR = &atom_PTR->atom_id;
	
	// notify entrance to object constructor
	// PUSH_CONTROL push control operation code will made on base object (entity) constructor

	// create new class, object constructor would be called
	Entity_PTR = null;
	#ifndef EX_OFF
	try { 
	#endif	
		Entity_PTR = (ENTITY*)mapi_PTR->CreateClass(class_search_data.module_class_id,false); 
	#ifndef EX_OFF
	}
	catch(_EXS xobj) 
	{
		TraceCurrent();
		POP_CONTROL(0)
		System_Api.entityID_PTR = null;
		throw;
	}
	catch(...)
	{
		TraceCurrent();
		POP_CONTROL(0)
		System_Api.entityID_PTR = null;
		_THROW(RestoreEntity(Constructor));
	}
	#endif

	System_Api.entityID_PTR = null;
	
	// Push was made in Entity base class constructor
	POP_CONTROL(0)
	
	if(Entity_PTR == null) THROW;

	// created, but object constructor start self destruct process 
	if(atom_PTR->as.Deleted) _THROW(self destruct on load);
	
	UNGUARD
	//*/
}

void CORE::ValidateApiCalls()
{
	GUARD(CORE::ValidateApiCalls)
	if(Constructor_counter) _THROW(api call from constructor);
	UNGUARD
}


void CORE::CheckAutoExceptions(dword xflag)
{
	if(!(Exceptions_Mask & xflag)) return;
	switch(xflag)
	{
		case  _X_NO_MEM:			_THROW(no mem);
		case  _X_NO_FILE:			_THROW(no file);
		case  _X_NO_FILE_READ:		_THROW(cant read from file);
		case  _X_NO_FILE_WRITE:		_THROW(cant write to file);
		case  _X_NO_CREATE_ENTITY:	_THROW(cant create object);
		case  _X_NO_CLASS:			_THROW(cant find class);
		case  _X_NO_SERVICE:		_THROW(cant create service);
		case  _X_NO_ENTITY:			_THROW(cant find entity);
		case  _X_NO_LAYER:			_THROW(no layer);
		default:					_THROW(invalid exceptions flag);
	}
}

bool CORE::CreateEntity(ENTITY_ID * id_PTR, char * class_name)
{
	return CreateEntity(id_PTR,class_name,0);
}

//bool CORE::CreateEntity(ENTITY_ID * id_PTR, char * class_name)
bool CORE::CreateEntity(ENTITY_ID * id_PTR, char * class_name, ATTRIBUTES * attributesPTR)
{
	GUARD(CORE::CreateEntity)
//	CLASS_SEARCH_DATA class_search_data;
	C_ATOM * atom_PTR;
	//VMODULE_API * mapi_PTR;
	ENTITY * Entity_PTR;

	if(id_PTR) memset(id_PTR,0,sizeof(ENTITY_ID));
	if(Constructor_counter)
	{
		trace(class_name);
		VALIDATE_API_CALLS
	}

	// temporary commented for debug purposes
	// if(State_loading) _THROW(attempt to create on load state);
	
	VMA * pClass;
	long  hash;
	bool  bClassFound;

	if(_pModuleClassRoot == 0) return false;

	bClassFound = false;
	hash = MakeHashValue(class_name);
	if(hash == 0) return false;
	pClass = _pModuleClassRoot;
	do
	{
		if(pClass->GetHash() == hash)
		{
			if(stricmp(class_name,pClass->GetName())==0)
			{
				bClassFound = true;
				break;
			}
		}
		pClass = pClass->Next();
	} 
	//while(pClass->Next());
	while(pClass);
	
	// no class of this type
	if(!bClassFound) 
	{
		CheckAutoExceptions(_X_NO_CREATE_ENTITY);
		return false;
	}

	// infinite loop
	//if(Control_Stack.ScanClassCTP(class_code,CTP_CONSTRUCTOR))
	//{
	//	trace("INFINITE LOOP BREAKED");
	//	trace("( attempt to create same object from constructor ) : %s",class_name);
	//	CheckAutoExceptions(_X_NO_CREATE_ENTITY);
	//	return false;
	//}
	
	// create atom structure
	//atom_PTR = CreateAtom(class_code); 
	atom_PTR = CreateAtom(hash); 
	
	// ... throw() system error
	if(atom_PTR == null) _THROW(Cant create Atom);
	atom_PTR->atom_id.pName = pClass->GetName();
	
	// obtain module interface class	
	//mapi_PTR = Modules_Table.GetModuleAPI(class_search_data.module_code);
	
	// ... throw() system error, mark invalid class
	//if(mapi_PTR == null) 
	//{
	//	trace("INVALID MODULE");
	//	trace("( can't create module api class ) ? %s : %s",Modules_Table.GetModuleName(class_search_data.module_code),class_name);
	//	DeleteAtom(atom_PTR);
	//	CheckAutoExceptions(_X_NO_CREATE_ENTITY);
	//	return false;
	//}
	
	// set current entity atom id pointer
	System_Api.entityID_PTR = &atom_PTR->atom_id;
	
	// notify entrance to object constructor
	// PUSH_CONTROL push control operation code will made on base object (entity) constructor

	// create new class, object constructor would be called
	Entity_PTR = null;

	Constructor_counter++;
	#ifndef EX_OFF
	try { 
	#endif
		//Entity_PTR = (ENTITY*)mapi_PTR->CreateClass(class_search_data.module_class_id,false); 
		Entity_PTR = (ENTITY*)pClass->CreateClass(); 
	#ifndef EX_OFF
	}
	catch(_EXS xobj) 
	{
		TraceCurrent();
		if(!Constructor_counter) THROW;
		Constructor_counter--;
		POP_CONTROL(0)
		System_Api.entityID_PTR = null;
		throw;
	}
	catch(...)
	{
		TraceCurrent();
		if(!Constructor_counter) THROW;
		Constructor_counter--;
		POP_CONTROL(0)
		System_Api.entityID_PTR = null;
		_THROW(CreateEntity(Constructor));
	}
	#endif

	if(!Constructor_counter) THROW;
	Constructor_counter--;
	System_Api.entityID_PTR = null;
	
	// Push was made in Entity base class constructor
	POP_CONTROL(0)
	
	// class description already exist, but class not implemented ... write to log 
	if(Entity_PTR == null) 
	{	
		// xbox
		// Modules_Table.ModuleReferenceDec(class_search_data.module_code);
		trace("empty class: %s",class_name);
		_THROW(invalid class);
	}


	// mark, class object is connected to atom
	atom_PTR->as.Connected = true;

	// created, but object constructor start self destruct process (object will be deleted later)
	if(atom_PTR->as.Deleted) 
	{
		CheckAutoExceptions(_X_NO_CREATE_ENTITY);
		return false;
	}

	if(attributesPTR)
	{
		Entity_PTR->AttributesPointer = attributesPTR;
	}
	// call to entity initialize function
	//PUSH_CONTROL(Entity_PTR,class_code,CTP_INIT)
	PUSH_CONTROL(Entity_PTR,hash,CTP_INIT)
	#ifndef EX_OFF
		try { 
	#endif
		
		if(!Entity_PTR->Init())	
		{
			POP_CONTROL(0)
			MarkEntityAsDeleted(Entity_PTR->GetID());	
			CheckAutoExceptions(_X_NO_CREATE_ENTITY);
			return false; 
		}
	#ifndef EX_OFF
		}
		catch(_EXS xobj) 
		{
			TraceCurrent();
			POP_CONTROL(0)
			System_Api.entityID_PTR = null;
			throw;
		}
		catch(...)
		{
			TraceCurrent();
			POP_CONTROL(0)
			System_Api.entityID_PTR = null;
			_THROW(CreateEntity(Init));
		}
	#endif
	
	POP_CONTROL(0)

	

	if(id_PTR)
	{
		memcpy(id_PTR,&atom_PTR->atom_id,sizeof(ENTITY_ID));
	}
	UNGUARD
	return true;
}

// continuously increasing creation time structure making unical time stamp for entity
// exception generated at the end of period
void CORE::CreationTimeInc()
{
	GUARD(CORE::CreationTimeInc)
	dword n;
	bool  transfer;
	n = 0;
	transfer = false;
	do
	{
		if(CoreState.Creation_Time.time[n] == 0xffffffff) transfer = true; else transfer = false;
		CoreState.Creation_Time.time[n]++;
		n++;
	} while (transfer && n < CRTM_DWORDS);
	if(transfer && n >= CRTM_DWORDS) _THROW(Id time overflaw);
	UNGUARD
}

C_ATOM * CORE::CreateAtom(dword class_code)
{
	GUARD(CORE::CreateAtom)
	dword n;

	if(Atoms_PTR == null ) THROW;

	if(CoreState.Atoms_number >= CoreState.Atoms_space)
	{
		dword new_space;
		new_space = 2*CoreState.Atoms_space;
		Atoms_PTR = (C_ATOM **)RESIZE(Atoms_PTR,new_space*sizeof(C_ATOM*));
		if(Atoms_PTR == null) _THROW(Resizing Atoms Space Error);
		memset((char *)Atoms_PTR + CoreState.Atoms_space*sizeof(C_ATOM*),0,CoreState.Atoms_space*sizeof(C_ATOM*));
		CoreState.Atoms_space = new_space;
	}

	for(n=CoreState.Atoms_min_free_orbit;n<CoreState.Atoms_space;n++)
	{
		if(Atoms_PTR[n] != null) continue;

		Atoms_PTR[n] = NEW C_ATOM;
		if(Atoms_PTR[n] == null) THROW;

		if(CoreState.Atoms_max_orbit < n) CoreState.Atoms_max_orbit = n;

		Atoms_PTR[n]->atom_id.atom_position = n;
		if(CoreState.Atoms_min_free_orbit == n) 
		for(;CoreState.Atoms_min_free_orbit<CoreState.Atoms_space;CoreState.Atoms_min_free_orbit++)
		{
			if(Atoms_PTR[CoreState.Atoms_min_free_orbit] == null) break;
		}

		Atoms_PTR[n]->atom_id.creation_time = CoreState.Creation_Time;
		CreationTimeInc();
		Atoms_PTR[n]->atom_id.class_code = class_code;
		Atoms_PTR[n]->as.Deleted = false;
		//Atoms_PTR[n]->as.Service = false;
		Atoms_PTR[n]->atom_id.pointer = 0;
		CoreState.Atoms_number++;
		return Atoms_PTR[n];
	}
	UNGUARD
	return null;
}

C_ATOM * CORE::FitAtom(ENTITY_ID entity_id, ATOM_STATE atom_state)
{
	GUARD(CORE::FitAtom)
	if(Atoms_PTR == null ) THROW;
	if(entity_id.atom_position > CoreState.Atoms_max_orbit) THROW;
	if(Atoms_PTR[entity_id.atom_position] != null) THROW;

	Atoms_PTR[entity_id.atom_position] = NEW C_ATOM;
	if(Atoms_PTR[entity_id.atom_position] == null) THROW;
	Atoms_PTR[entity_id.atom_position]->as = atom_state;
	Atoms_PTR[entity_id.atom_position]->atom_id = entity_id;
	Atoms_PTR[entity_id.atom_position]->atom_id.pointer = null;
	return Atoms_PTR[entity_id.atom_position];
	UNGUARD
	return null;
}

bool CORE::DeleteAtom(C_ATOM * atom_PTR)
{
	GUARD(CORE::DeleteAtom)
	dword ap;
	if(Atoms_PTR == null || atom_PTR == null) return false;
	ap = atom_PTR->atom_id.atom_position;
	if(ap >= CoreState.Atoms_space) THROW;
	if(Atoms_PTR[ap] != atom_PTR) THROW;
	delete Atoms_PTR[ap];
	Atoms_PTR[ap] = null;
	CoreState.Atoms_number--;
	if(CoreState.Atoms_min_free_orbit > ap) CoreState.Atoms_min_free_orbit = ap;
	UNGUARD
	return true;
}

void CORE::CheckMemoryLeak_Classes()
{/*
	GUARD(CORE::CheckMemoryLeak_Classes)
	void * mcheck_PTR;
//	MEM_EXE_STATE mes;
	dword lost_blocks;
	dword lost_mem;
	VMA * pClass;

	lost_blocks = 0;
	lost_mem = 0;

	mcheck_PTR = Memory_Service.GetFirstPointer();
	do
	{
		if(mcheck_PTR)
		{
			if(Memory_Service.GetPointerState(mcheck_PTR,&mes))
			{
				if(mes.ctp != CTP_CORE)
				{
					pClass = FindVMA(mes.class_code);
					if(pClass) strcpy(gstring,pClass->GetName());
					gdi_display.Print("Memory leak %d byte(s):",Memory_Service.GetBlockSize(mcheck_PTR));
					gdi_display.Print("(%x) %s -> %s",
						(dword)mcheck_PTR,gstring,CTP_NAME(mes.ctp));	
					//MEM_BLOCK * Mem_link = (MEM_BLOCK *)((char *)mcheck_PTR - sizeof(MEM_BLOCK));
					gdi_display.Print("file: %s, line: %d",
						Memory_Service.GetFileName(mcheck_PTR),Memory_Service.GetFileLineCode(mcheck_PTR));

						//Mem_link->cs.pFileName,Mem_link->cs.line);	
					lost_blocks++;
					lost_mem += Memory_Service.GetBlockSize(mcheck_PTR);
				}
			}
		}
		mcheck_PTR = Memory_Service.GetNextPointer();
	} while(mcheck_PTR != null);

	if(lost_blocks) 
	{
		gdi_display.Print("%d byte(s) memory in %d block(s) lost",lost_mem,lost_blocks);
		Memory_Leak_flag = true;
	}
	
	UNGUARD*/
}

dword CORE::Class_Name2Code(char * class_name)
{
	//VALIDATE_API_CALLS // no necessary
	return MakeHashValue(class_name);
}

bool CORE::FindClass(ENTITY_ID * id_PTR,char * class_name,dword class_code)
{
	GUARD(CORE::FindClass)

	dword n;
	dword hash;
	
	if(Atoms_PTR == null ) THROW;
	if(class_name != null) hash = MakeHashValue(class_name); 
	else hash = class_code;

	for(n=0;n<=CoreState.Atoms_max_orbit;n++)
	{
		if(Atoms_PTR[n] == null) continue;
		if(Atoms_PTR[n]->as.Deleted) continue;
		if(Atoms_PTR[n]->atom_id.class_code == hash)
		{
			// class found
			Atom_Search_Position = n;
			Atom_Search_Class_Code = hash;
			if(id_PTR) memcpy(id_PTR,&Atoms_PTR[n]->atom_id,sizeof(ENTITY_ID));
			return true;
		}
	}

	if(id_PTR) *id_PTR = ENTITY_ID();

	UNGUARD
	return false;
}

bool CORE::FindClassNext(ENTITY_ID * id_PTR)
{
	GUARD(CORE::FindClassNext)
	VALIDATE_API_CALLS // no necessary
	dword n;

	if(Atoms_PTR == null )
	{
		if(id_PTR) *id_PTR = ENTITY_ID();
		return false;				// sys error
	}
	for(n=(Atom_Search_Position + 1);n<=CoreState.Atoms_max_orbit;n++)
	{
		if(Atoms_PTR[n] == null) continue;//break;
		if(Atoms_PTR[n]->as.Deleted) continue;
		if(Atoms_PTR[n]->atom_id.class_code == Atom_Search_Class_Code)
		{
			// class found
			Atom_Search_Position = n;
			if(id_PTR) memcpy(id_PTR,&Atoms_PTR[n]->atom_id,sizeof(ENTITY_ID));
			return true;
		}
	}
	if(id_PTR) *id_PTR = ENTITY_ID();
	UNGUARD
	return false;
}

bool CORE::CompareID(ENTITY_ID * ida_PTR,ENTITY_ID * idb_PTR)
{
	GUARD(CORE::CompareID)
	VALIDATE_API_CALLS // no necessary
	if(ida_PTR == null || idb_PTR == null) return false;
	if(memcmp(ida_PTR,idb_PTR,sizeof(ENTITY_ID)) == 0) return true;
	UNGUARD
	return false;
}

bool CORE::GetEntity(ENTITY_ID * id_PTR)
{
	GUARD(CORE::GetEntity)
	VALIDATE_API_CALLS // no necessary
	if(Atoms_PTR == null) THROW;
	if(id_PTR == null) return false;

	// scan all entities
	if(Scan_Layer_Code == INVALID_LAYER_CODE)
	{
		Atom_Get_Position = 0;
		do
		{
			if(Atom_Get_Position > CoreState.Atoms_max_orbit) return false;
			if(Atoms_PTR[Atom_Get_Position]) 
			{
				if(!Atoms_PTR[Atom_Get_Position]->as.Deleted)
				{
					memcpy(id_PTR,&Atoms_PTR[Atom_Get_Position]->atom_id,sizeof(ENTITY_ID));
					return true;
				}
			}
			Atom_Get_Position++;
		} while(Atom_Get_Position <= CoreState.Atoms_max_orbit);
	}
	else
	{
		ENTITY_ID * eid_PTR;
		LAYER * lPTR;
		lPTR = CommonLayers.GetLayer(Scan_Layer_Code);
		if(lPTR)
		{
			eid_PTR = lPTR->GetID();
			if(eid_PTR) 
			{
				memcpy(id_PTR,eid_PTR,sizeof(ENTITY_ID));
				return true;
			}
		}
		return false;
	}
	UNGUARD
	return false;
}

bool CORE::GetEntityNext(ENTITY_ID * id_PTR)
{
	GUARD(CORE::GetEntityNext)
	VALIDATE_API_CALLS // no necessary
	if(Atoms_PTR == null ) THROW;
	if(id_PTR == null) return false;

	if(Scan_Layer_Code == INVALID_LAYER_CODE)
	{
		do
		{
			Atom_Get_Position++;
			if(Atom_Get_Position > CoreState.Atoms_max_orbit) return false;
			if(Atoms_PTR[Atom_Get_Position])
			{
				if(!Atoms_PTR[Atom_Get_Position]->as.Deleted)
				{
					memcpy(id_PTR,&Atoms_PTR[Atom_Get_Position]->atom_id,sizeof(ENTITY_ID));
					return true;
				}
			}
		} while(Atom_Get_Position <= CoreState.Atoms_max_orbit);
	}
	else
	{
		ENTITY_ID * eid_PTR;
		//eid_PTR = Layer_Table[Scan_Layer_Code]->GetNextID();
		LAYER * lPTR;
		lPTR = CommonLayers.GetLayer(Scan_Layer_Code);
		if(lPTR)
		{
			eid_PTR = lPTR->GetNextID();
			if(eid_PTR) 
			{
				memcpy(id_PTR,eid_PTR,sizeof(ENTITY_ID));
				return true;
			}
		}
		return false;
	}

	UNGUARD
	return false;
}

bool CORE::SetEntityScanLayer(char * layer_name)
{
	GUARD(CORE::SetEntityScanLayer)
	Scan_Layer_Code = CommonLayers.GetIndex(layer_name);
	//Scan_Layer_Code = GetLayerIndex(layer_name);
	if(Scan_Layer_Code == INVALID_LAYER_CODE) return false;
	UNGUARD
	return true;
}

ENTITY * CORE::GetEntityPointer(ENTITY_ID * id_PTR)
{
	ENTITY * ep;
	GUARD(CORE::GetEntityPointer)
	VALIDATE_API_CALLS
	if(!ValidateEntity(id_PTR)) return null;
	ep = (ENTITY *)Atoms_PTR[id_PTR->atom_position]->atom_id.pointer;
	if(Atoms_PTR[id_PTR->atom_position]->as.Deleted) return null;
	UNGUARD
	return ep;
}

//-------------------------------------------------------------------------------------------------
// Return true, if entity with id pointed by id_PTR exist and false otherwise
//
bool CORE::ValidateEntity(ENTITY_ID * id_PTR)
{
	GUARD(CORE::ValidateEntity)
	//VALIDATE_API_CALLS
	dword pos;
	if(id_PTR) id_PTR->pointer = 0;
	if(id_PTR == null) return false;
	// check fo valid object position and pointer
	pos = id_PTR->atom_position; if(pos >= CoreState.Atoms_space) return false;
	if(Atoms_PTR[pos] == null) return false;
	if(Atoms_PTR[pos]->atom_id.pointer == null) 
	{
		CheckAutoExceptions(_X_NO_ENTITY);
		return false;
	}
	
	// update pointer
	id_PTR->pointer = Atoms_PTR[pos]->atom_id.pointer;

	// compare id structure
	if(memcmp(&Atoms_PTR[pos]->atom_id,id_PTR,sizeof(ENTITY_ID)) != 0) 
	{
		id_PTR->pointer = 0;
		return false;
	}

	

	UNGUARD
	return true;
}

//-------------------------------------------------------------------------------------------------
// Mark entity for further deleting using id
//
bool CORE::MarkEntityAsDeleted(ENTITY_ID entity_id)
{
	GUARD(CORE::MarkEntityAsDeleted(ENTITY_ID))
	PUSH_CONTROL(0,0,0)
	if(!ValidateEntity(&entity_id)) return false;
	Atoms_PTR[entity_id.atom_position]->as.Deleted = true;
	DeleteEntityList.AddData(&entity_id);
	POP_CONTROL(0)
	UNGUARD
	return true;
}

//-------------------------------------------------------------------------------------------------
// Mark entity for further deleting using object pointer
//
bool CORE::MarkEntityAsDeleted(void * entity_PTR)
{
	GUARD(CORE::MarkEntityAsDeleted(void *))
	ENTITY_ID entity_id;
	if(!Convert_Pointer2ID(entity_PTR,&entity_id)) return false;
	MarkEntityAsDeleted(entity_id);
	UNGUARD
	return true;
}

//-------------------------------------------------------------------------------------------------
// Delete objects (entities and layers) previously assigned for deleting
//
void CORE::ProcessDeleteList()
{
	GUARD(CORE::ProcessDeleteList())
	
	dword deleted_num,n,dwcode;
	ENTITY_ID entity_id;

	// delete entity, added to delete list -------------------------
	GUARD(deleting entities)
	deleted_num = DeleteEntityList.DataNum();
	if(deleted_num != 0)
	{
		for(n=0;n<deleted_num;n++)
		{
			if(!DeleteEntityList.GetData(n,&entity_id)) continue;
			EraseEntity(entity_id);
			deleted_num = DeleteEntityList.DataNum();
			//DeleteEntityList.ShiftLeft();
		}
		DeleteEntityList.Clear();
	}
	UNGUARD
	//--------------------------------------------------------------

	// delete common layers, added to delete list ------------------
	GUARD(deleting layers)
	deleted_num = DeleteLayerList.DataNum();
	if(deleted_num != 0)
	{
		for(n=0;n<deleted_num;n++)
		{
			if(!DeleteLayerList.GetData(n,&dwcode)) continue;
			CommonLayers.Erase(dwcode);
			deleted_num = DeleteLayerList.DataNum();
			//EraseLayer(dwcode);
		}
		DeleteLayerList.Clear();
	}
	UNGUARD
	//-------------------------------------------------------------

	GUARD(deleting services)
	SERVICE * service_PTR;
	CODE_AND_POINTER data;
	deleted_num = DeleteServicesList.DataNum();
	if(deleted_num != 0)
	{
		for(n=0;n<deleted_num;n++)
		{
			if(!DeleteServicesList.GetData(n,&data)) continue;
			service_PTR = (SERVICE *)data.pointer;
			if(!service_PTR) _THROW(invalid service);
			#ifndef EX_OFF
			try {
			#endif
				delete service_PTR;
			#ifndef EX_OFF
			}
			catch(...)
			{
				trace("service release error");
				TraceCurrent();
				Memory_Service.Free(service_PTR);
				//Modules_Table.ModuleReferenceDec(data.code);
				_THROW(invalid service);
			}
			#endif

			// xbox
			// Modules_Table.ModuleReferenceDec(data.code);

		}
		DeleteServicesList.Clear();
	}
	UNGUARD

	

	UNGUARD
}
//-------------------------------------------------------------------------------------------------
// Delete entity
//
bool CORE::EraseEntity(ENTITY_ID entity_id)
{
	GUARD(CORE::EraseEntity)
//	CLASS_SEARCH_DATA class_search_data;

	if(!ValidateEntity(&entity_id)) return false;

//	PUSH_CONTROL(Atoms_PTR[entity_id.atom_position]->atom_id.pointer,entity_id.class_code,CTP_DESTRUCTOR)
	#ifndef EX_OFF
	try {
	#endif
		delete (ENTITY *)Atoms_PTR[entity_id.atom_position]->atom_id.pointer;
	#ifndef EX_OFF
	}
	catch(_EXS xobj) 
	{
		
		Memory_Service.Free(Atoms_PTR[entity_id.atom_position]->atom_id.pointer);
		Atoms_PTR[entity_id.atom_position]->atom_id.pointer = 0;
		DeleteAtom(Atoms_PTR[entity_id.atom_position]);

		TraceCurrent();
//		POP_CONTROL(0)
		throw;
	}
	catch(...)
	{
		
		Memory_Service.Free(Atoms_PTR[entity_id.atom_position]->atom_id.pointer);
		Atoms_PTR[entity_id.atom_position]->atom_id.pointer = 0;
		DeleteAtom(Atoms_PTR[entity_id.atom_position]);

		TraceCurrent();
//		POP_CONTROL(0)
		_THROW(EraseEntity);
	}
	#endif
//	POP_CONTROL(0);
	//Memory_Service.Free(Atoms_PTR[entity_id.atom_position]->atom_id.pointer);

	dword n;
	// remove entity from layers lists
	/*for(n=0;n<CoreState.Layer_max_index;n++)
	{
		if(Layer_Table[n] == null) continue;
		if(Atoms_PTR[entity_id.atom_position]->TstLayerAttribute(n,false)) 
			Layer_Table[n]->Del(entity_id);
	}*/


	for(n=0;n<=CommonLayers.lss.Layer_max_index;n++)
	{
		if(CommonLayers.Layer_Table[n] == null) continue;
		if(Atoms_PTR[entity_id.atom_position]->TstLayerAttribute(n,false)) 
			CommonLayers.Layer_Table[n]->Del(entity_id);
	}

	DeleteAtom(Atoms_PTR[entity_id.atom_position]);

	if(CoreState.Atoms_max_orbit == entity_id.atom_position)
	{
		for(CoreState.Atoms_max_orbit = entity_id.atom_position; CoreState.Atoms_max_orbit > 0;CoreState.Atoms_max_orbit--)
		{
			if(Atoms_PTR[CoreState.Atoms_max_orbit] == null) continue;
			break;
		}
	}
	UNGUARD
	return true;
}

//-------------------------------------------------------------------------------------------------
// function scan all entities untill entity with memory pointer equal to _entity_pointer
// if such entity found function return true
// if id_PTR not null, function fill pointed structure by entity_id
//
bool CORE::Convert_Pointer2ID(void * _entity_pointer,ENTITY_ID * id_PTR)
{
	GUARD(CORE::Convert_Pointer2ID)
	ENTITY * entity_PTR;
	ENTITY_ID eid;
	entity_PTR = (ENTITY *)_entity_pointer;
	eid = entity_PTR->GetID();
	if(Atoms_PTR == null ) THROW;
	if(eid.atom_position > CoreState.Atoms_max_orbit) THROW;
	if(Atoms_PTR[eid.atom_position] == null) THROW;
	if(Atoms_PTR[eid.atom_position]->atom_id.pointer != _entity_pointer) THROW;
	if(id_PTR != null) memcpy(id_PTR,&Atoms_PTR[eid.atom_position]->atom_id,sizeof(ENTITY_ID));
	UNGUARD
	return true;
}

//-------------------------------------------------------------------------------------------------
// Return index (unical code) of layer by name
//
dword CORE::GetLayerIndex(char * layer_name)
{
	GUARD(CORE::GetLayerIndex)
	return CommonLayers.GetIndex(layer_name);
/*	dword n;
	if(layer_name == null) return INVALID_LAYER_CODE;
	for(n=0;n<=CoreState.Layer_max_index;n++)
	{
		if(Layer_Table[n] == null) continue;
		if(stricmp(layer_name,Layer_Table[n]->Name)== 0) return n;
	}*/
	UNGUARD
	return INVALID_LAYER_CODE;
}


bool CORE::VerifyLayer(char * layer_name)
{
	GUARD(CORE::VerifyLayer)
	//VALIDATE_API_CALLS // no necessary
	return CommonLayers.Verify(layer_name);
/*	dword n;

	if(layer_name == null) return false;
	for(n=0;n<=CoreState.Layer_max_index;n++)
	{
		if(Layer_Table[n] == null) continue;
		if(stricmp(layer_name,Layer_Table[n]->Name)== 0) return true;
	}*/
	UNGUARD
	return false;
}

bool CORE::LayerCreate(char * layer_name, bool ordered, bool fail_if_exist)
{
	GUARD(CORE::CreateLayer)
	VALIDATE_API_CALLS
	return CommonLayers.Create(layer_name,ordered,fail_if_exist);
	//return LayerCreate(layer_name,ordered,fail_if_exist,false,0);
	UNGUARD
	return false;
}

// fit common layer
void CORE::FitLayer(dword index,char * layer_name, LAYER_STATE ls)
{
	GUARD(CORE::FitLayer)
	CommonLayers.Fit(index,layer_name,ls);
	UNGUARD
}

bool CORE::LayerCreate(char * layer_name, bool ordered, bool fail_if_exist, bool system, dword system_flags)
{
	GUARD(CORE::CreateLayer)
	if(system) 
	{

	}
	else
	return CommonLayers.Create(layer_name,ordered,fail_if_exist);
	UNGUARD
	return false;
}

C_ATOM * CORE::GetAtom(ENTITY_ID * id_PTR)
{
	if(!ValidateEntity(id_PTR)) return null;
	return Atoms_PTR[id_PTR->atom_position];
}

//------------------------------------------------------------------------------------------------
// Mark layer for deletion before next game loop
//
void CORE::LayerDelete(char * layer_name)
{
	GUARD(CORE::DeleteLayer)
	
	if(CommonLayers.Verify(layer_name))
	{
		ENTITY_ID * id_PTR;
		LAYER * layer_PTR;
		C_ATOM * atom_PTR;
		layer_PTR = CommonLayers.GetLayer(layer_name);
		if(layer_PTR)
		{
			id_PTR = layer_PTR->GetID();
			while(id_PTR)
			{
				atom_PTR = GetAtom(id_PTR);
				if(atom_PTR) atom_PTR->ClrLayerAttribute(CommonLayers.GetIndex(layer_name),false);
				id_PTR = layer_PTR->GetNextID();
			}
		}
	}

	CommonLayers.Delete(layer_name);
	UNGUARD
}

void CORE::LayerSetFlags(char * layer_name, dword flags)
{
	GUARD(CORE::SetLayerFlags)
	CommonLayers.SetFlags(layer_name,flags);
	UNGUARD
}

void CORE::LayerClrFlags(char * layer_name, dword flags)
{
	GUARD(CORE::ClrLayerFlags)
	CommonLayers.ClrFlags(layer_name,flags);
	UNGUARD
}

dword CORE::LayerGetFlags(char * layer_name)
{
	return CommonLayers.GetFlags(layer_name);
}


bool CORE::LayerAdd(const char * layer_name, ENTITY_ID eid, dword priority)
{
	return LayerAdd((char *)layer_name,eid,priority,false);
}

bool CORE::LayerAdd(const char * pLayerName, ENTITY_ID eid, dword priority, bool system)
{
	GUARD(CORE::LayerAdd)
	dword index;

	char * layer_name = (char*)pLayerName;

	if(system)
	{
		
	}
	else
	{
		LayerCreate(layer_name,true,false,false,0);
		index = CommonLayers.GetIndex(layer_name);
		if(index == INVALID_LAYER_CODE) return false;
		if(!ValidateEntity(&eid)) return false;
		if(Atoms_PTR[eid.atom_position]->TstLayerAttribute(index,false)) return true;
		if(!CommonLayers.Add(index,eid,priority)) return false;
		Atoms_PTR[eid.atom_position]->SetLayerAttribute(index,false);
	}
	UNGUARD
	return true;
}

void CORE::LayerDel(const char * layer_name, ENTITY_ID eid)
{
	LayerDel((char*)layer_name,eid,false);
}

void CORE::LayerDel(const char * pLayerName, ENTITY_ID eid,bool system)
{
	GUARD(CORE::LayerDel)
	dword index;

	char * layer_name = (char*)pLayerName;
	if(system) 
	{

	}
	else 
	{
		index = CommonLayers.GetIndex(layer_name);
		CommonLayers.Del(index,eid);
		if(!ValidateEntity(&eid)) return;
		Atoms_PTR[eid.atom_position]->ClrLayerAttribute(index,false);
	}
	UNGUARD
}

bool CORE::LayerDeleteContent(char * layer_name)
{
	GUARD(CORE::DeleteLayerContent)

	if(CommonLayers.Verify(layer_name))
	{
		ENTITY_ID * id_PTR;
		LAYER * layer_PTR;
		layer_PTR = CommonLayers.GetLayer(layer_name);
		if(layer_PTR)
		{
			id_PTR = layer_PTR->GetID();
			while(id_PTR)
			{
				MarkEntityAsDeleted(*id_PTR);
				id_PTR = layer_PTR->GetNextID();
			}
		}
	}

	UNGUARD
	return true;
}

// set layer sleeping time, layer will skip execution till this time
void CORE::LayerSetSleep(char * layer_name,dword sleep_time_ms)
{

}

// on/off execute
void CORE::LayerSetExecute(char * layer_name, bool on)
{
	if(on) LayerSetFlags(layer_name,LRFLAG_EXECUTE);
	else LayerClrFlags(layer_name,LRFLAG_EXECUTE);
}

// on/off realize
void CORE::LayerSetRealize(char * layer_name, bool on)
{
	if(on) LayerSetFlags(layer_name,LRFLAG_REALIZE);
	else LayerClrFlags(layer_name,LRFLAG_REALIZE);
}

VIDWALKER * CORE::LayerGetWalker(char * layer_name)
{
	LAYER * pl;
	pl = CommonLayers.GetLayer(layer_name);
	if(pl == null) 
	{
		CommonLayers.Create(layer_name,true,false);
		pl = CommonLayers.GetLayer(layer_name);
		//trace("ERROR: missing layer [%s]",layer_name);
		//return null;
	}
	return pl->GetWalker();
}
extern CODESOURCE CodeSource;

void * CORE::MemAllocate(size_t size,char * pFileName, unsigned long nLine)
{
	GUARD(CORE::MemAllocate)
	void * pPTR;
	CodeSource.pFileName = pFileName; CodeSource.line = nLine;
	pPTR = Memory_Service.Allocate(size);
	CodeSource.pFileName = 0; CodeSource.line = 0xffffffff;
	return pPTR;
	UNGUARD
	return 0;
}
void CORE::MemFree(void * block_ptr)
{
	if (!block_ptr) return;

	GUARD(CORE::MemFree)
	//if(System_Api.Exceptions) return;			// exception processing going, this line prevent core from
	// generating other exceptions, which can damage control trace procedures. For example: if exceptions 
	// generated in object constructor, system trying to delete this object via delete (MemFree). If, in turn,
	// one more exception occuried in this function, exceptions control would returned to system, not to
	// unguard block and programm would stopped by system, loosing exception path information

	if(Control_Stack.ScanPointer(block_ptr))	// if this block in executed objects list
	{
		if(Services_List.FindNode((SERVICE *)block_ptr))
		{
			// this is service pointer
			//_asm int 3;
		}
		else MarkEntityAsDeleted(block_ptr); // this is entity
		return;
	}
	Memory_Service.Free(block_ptr);
	UNGUARD
}

void * CORE::MemReallocate(void * block_ptr,size_t size,char * pFileName, unsigned long nLine)
{
	GUARD(CORE::MemReallocate)
	void * pPTR;
	CodeSource.pFileName = pFileName; CodeSource.line = nLine;
	pPTR = Memory_Service.Reallocate(block_ptr,size);
	CodeSource.pFileName = 0; CodeSource.line = 0xffffffff;
	return pPTR;
	
	UNGUARD
	return 0;
}

//-------------------------------------------------------------------------------------------------
// exit to system
//
void CORE::Exit()	{	Exit_flag = true;	}
void CORE::Reset()	{	Reset_flag = true;	}

//------------------------------------------------------------------------------------------------
// return application window handle
//
HWND CORE::GetAppHWND() { return App_Hwnd; }
HINSTANCE CORE::GetAppInstance(){return hInstance;}
//------------------------------------------------------------------------------------------------
// add modules search path
//
bool CORE::AddModulesPath(char * _path)
{
	return Modules_Table.AddModulesPath(_path);
}

//------------------------------------------------------------------------------------------------
// Mark entity for following deletion
//
bool CORE::DeleteEntity(ENTITY_ID entity_id)
{
	GUARD(CORE::DeleteEntity)
	VALIDATE_API_CALLS
	if(!ValidateEntity(&entity_id)) return false;
	return MarkEntityAsDeleted(entity_id);
	UNGUARD
	return false;
}

void * CORE::GetUserData(long * data_size)
{
	VALIDATE_API_CALLS
	return null;
}

void * CORE::SetUserData(void * ud_PTR,long data_size)
{
	VALIDATE_API_CALLS
	return null;
}
void CORE::SetTimeScale(float _scale)
{
	VALIDATE_API_CALLS
	fTimeScale = _scale;
}
float CORE::GetTimeScale()
{
	VALIDATE_API_CALLS
	return fTimeScale;
}

//------------------------------------------------------------------------------------------------
// transfer message arguments and program control to entity, specified by Destination id
//
dword _cdecl CORE::Send_Message(ENTITY_ID Destination,char * Format,...)
{
	dword rc;
	GUARD(CORE::Send_Message)
	VALIDATE_API_CALLS
	MESSAGE message;
	CONTROL_BLOCK cb;
	if(!ValidateEntity(&Destination)) return 0;				// check for valid destination
	message.Reset(Format);									// reset message class
	Control_Stack.Read(&cb);								// read top (current) entity information
	if(cb.pointer)
	message.Sender_ID = ((ENTITY *)cb.pointer)->GetID();	// set sender information
	else 
	{
		ZeroMemory(&message.Sender_ID,sizeof(ENTITY_ID));
	}
	va_start(message.args,Format);
	// push into stack new top entity
	PUSH_CONTROL(Destination.pointer,Destination.class_code,CTP_MESSAGE)
	rc = ((ENTITY *)Destination.pointer)->ProcessMessage(message);	// transfer control
	POP_CONTROL(0)	// pop from stack entity
	va_end(message.args);
	UNGUARD
	return rc;
}

dword _cdecl CORE::PostEvent(char * Event_name, dword post_time, char * Format,...)
{
	S_EVENTMSG * pEM;
	MESSAGE_SCRIPT * pMS;
	MESSAGE message;
	bool bAction;

	ENTITY_ID id;

	if(Format != 0) 
	{
		pMS = NEW MESSAGE_SCRIPT;
		va_start(message.args,Format);
		message.Reset(Format);	
		pMS->Reset(Format);

		bAction = true;
		while(bAction)
		{
			switch(message.GetCurrentFormatType())
			{
				
				//-------------------------------------
				case 'l':
					long v;
					v = message.Long();
					pMS->Set((char *)&v);
				break;
				case 'f':
					float f;
					f = message.Float();
					pMS->Set((char *)&f);
				break;
				case 'i':
					id = message.EntityID();
					pMS->Set((char *)&id);
				break;
				case 'e':
					VDATA * e;
					e = message.ScriptVariablePointer();
					pMS->Set((char *)&e);
				break;
				case 's':
					char * s;
					s = message.StringPointer();
					pMS->Set((char *)s);
				break;
				case 'a':
					ATTRIBUTES * a;
					a = message.AttributePointer();
					pMS->Set((char *)&a);
				break;

				//-------------------------------------
				default: bAction = false; break;
			}
		}
		va_end(message.args);
	} else pMS = 0;
	
	pEM = NEW S_EVENTMSG(Event_name,pMS,post_time);
	pEM->bProcess = true;
	Compiler.AddPostEvent(pEM);
	return 0;
}

VDATA * _cdecl CORE::Event(char * Event_name, char * Format,...)
{
	VDATA * pVD;
	GUARD(CORE::Event)
	PUSH_CONTROL(0,0,0)


	pVD = 0;
	if(Format == 0) 
	{
		pVD = Compiler.ProcessEvent(Event_name);

		POP_CONTROL(0)
		return pVD;
	}
	MESSAGE message;
	va_start(message.args,Format);					// 1
	message.Reset(Format);	// reset message class	// 2
	// ....
	pVD = Compiler.ProcessEvent(Event_name,message);

	va_end(message.args);
	POP_CONTROL(0)
	UNGUARD
	return pVD;
}

//------------------------------------------------------------------------------------------------
// mark service class for unloading
//
void CORE::FreeService(char * service_name)
{
	GUARD(CORE::FreeService)
	
	VMA * pClass;
	pClass = FindVMA(service_name);
	if(pClass)
	{
		//pClass->RefDec();
	}

	UNGUARD
}

void * CORE::MakeClass(char * class_name)
{
	VMA * pClass;
	long  hash;

	if(_pModuleClassRoot == 0) return 0;
	
	hash = MakeHashValue(class_name);
	pClass = _pModuleClassRoot;
	while(pClass)
	{
		if(pClass->GetHash() == hash)
		{
			if(stricmp(class_name,pClass->GetName())==0)
			{
				return pClass->CreateClass();
			}
		}
		pClass = pClass->Next();
	}
	return 0;
}


void CORE::FreeServices()
{
	VMA * pClass;
	if(_pModuleClassRoot == 0) return;
	pClass = _pModuleClassRoot;
	do
	{
		if(pClass->Service())
		{
			pClass->Clear();
		}
		pClass = pClass->Next();
	} 
	while(pClass);
	Controls = 0;
}

VMA * CORE::FindVMA(char * class_name)
{
	VMA * pClass;
	long  hash;

	if(_pModuleClassRoot == 0) return 0;
	
	hash = MakeHashValue(class_name);
	pClass = _pModuleClassRoot;
	if(!pClass) return 0;
	do
	{
		if(pClass->GetHash() == hash)
		{
			if(stricmp(class_name,pClass->GetName())==0)
			{
				return pClass;
			}
		}
		pClass = pClass->Next();
		if(!pClass) return 0;
	} while(pClass->Next());
	return 0;
}

VMA * CORE::FindVMA(long hash)
{
	VMA * pClass;

	if(_pModuleClassRoot == 0) return 0;

	pClass = _pModuleClassRoot;
	do
	{
		if(pClass->GetHash() == hash)
		{
			return pClass;
		}
		pClass = pClass->Next();
	} while(pClass->Next());
	return 0;
}

void * CORE::CreateService(char * service_name)
{
	dword class_code;
//	dword module_code;
	SERVICE * service_PTR;
//	VMODULE_API * mapi_PTR;
	VMA * pClass;
	
	GUARD(CORE::CreateService)

	pClass = FindVMA(service_name);
	if(pClass == 0) 
	{
		CheckAutoExceptions(_X_NO_SERVICE);
		return 0;
	}

	if(pClass->GetHash() == 0) 
	{
		CheckAutoExceptions(_X_NO_SERVICE);
		return 0;
	}

	if(pClass->GetReference() > 0) return pClass->CreateClass();

	PUSH_CONTROL(0,0,0)
	service_PTR = (SERVICE *)pClass->CreateClass();
	POP_CONTROL(0)

	class_code = MakeHashValue(service_name);
	pClass->SetHash(class_code);
	PUSH_CONTROL(0,class_code,CTP_INIT)
	#ifndef EX_OFF
	try { 
	#endif
		if(!service_PTR->Init())	
		{ 
			CheckAutoExceptions(_X_NO_SERVICE);
		}	
	#ifndef EX_OFF
	}
	catch(_EXS xobj) 
	{
		TraceCurrent();	POP_CONTROL(0); throw;
	}
	catch(...)
	{
		TraceCurrent();	POP_CONTROL(0); _THROW(CreateService: Init func);
	}
	#endif
	POP_CONTROL(0)

	PUSH_CONTROL(0,0,0)
	Services_List.Add(class_code,class_code,service_PTR);
	POP_CONTROL(0)
	
	UNGUARD
	return service_PTR;
	
}

char buffer_4k[4096];

void _cdecl CORE::BTrace(const char * format, ...)
{
	GUARD(CORE::Trace)
	va_list args;
	va_start(args,format);
	_vsnprintf(buffer_4k,sizeof(buffer_4k) - 4,format,args);
	va_end(args);
	if(!gdi_display.gdi_off) gdi_display.Print(buffer_4k);
	else trace(buffer_4k);
	Beep(2000, 150);
	UNGUARD
}

void _cdecl CORE::Trace(const char * format, ...)
{
	GUARD(CORE::Trace)
	va_list args;
	va_start(args,format);
	_vsnprintf(buffer_4k,sizeof(buffer_4k) - 4,format,args);
	va_end(args);
	if(!gdi_display.gdi_off) gdi_display.Print(buffer_4k);
	else trace(buffer_4k);
	UNGUARD
}

//------------------------------------------------------------------------------------------------
// Transfer programm control to objects via Execute() functions
//
void CORE::ProcessExecute()
{
	GUARD(CORE::ProcessExecute())
	DWORD ticks;
	ATOM_STATE * pAs;

	/*if (GetAsyncKeyState('7') < 0)
	{
		VDATA * pVBlazeArray = (VDATA *)GetScriptVariable("characters", null);
		VDATA * pVBlaze = pVBlazeArray->GetArrayElement(0);
		ATTRIBUTES * pHP = pVBlaze->GetAClass()->FindAClass(pVBlaze->GetAClass(), "Ship.HP");
		pHP->SetBreak(true);
		Sleep(200);
	}*/

	ProcessRunStart(SECTION_EXECUTE);

	dword deltatime = Timer.GetDeltaTime();
	
	LAYER * l_PTR;
	ENTITY_ID * eid_PTR;
	dword n,flags;
	for(n=0;n<=CommonLayers.lss.Layer_max_index;n++)
	{
		l_PTR = CommonLayers.Layer_Table[n];
		if(l_PTR == null) continue;
		flags = l_PTR->ls.Flags & LRFLAG_FROZEN; if(flags != 0) continue;	// skip frozen layers
		flags = l_PTR->ls.Flags & LRFLAG_EXECUTE; if(flags == 0) continue;	// skip non execute layers
		eid_PTR = l_PTR->GetID();
		while(eid_PTR)
		{
			if(ValidateEntity(eid_PTR)) 
			{
				if(!Atoms_PTR[eid_PTR->atom_position]->as.Deleted)
				{
				//trace("Ex St: %s",eid_PTR->pName);
				RDTSCB(ticks)
				PUSH_CONTROL(eid_PTR->pointer,eid_PTR->class_code,CTP_EXECUTE)
				((ENTITY *)eid_PTR->pointer)->Execute(deltatime);
				POP_CONTROL(0)
				RDTSCE(ticks)
				//trace("Ex En: %s",eid_PTR->pName);
				
				pAs = &Atoms_PTR[eid_PTR->atom_position]->as;
				pAs->Execute_ticks_av = ticks;
				if(pAs->Execute_ticks_max < ticks) pAs->Execute_ticks_max = ticks;

				}
			}
			eid_PTR = l_PTR->GetNextID();
		}
	}
//*/
	ProcessRunEnd(SECTION_EXECUTE);
	UNGUARD
}

//------------------------------------------------------------------------------------------------
// Transfer programm control to objects via Realize() functions
//
void CORE::ProcessRealize()
{
	GUARD(CORE::ProcessRealize())
	DWORD ticks;
	ATOM_STATE * pAs;
	ProcessRunStart(SECTION_REALIZE);

	dword deltatime = Timer.GetDeltaTime();

	LAYER * l_PTR;
	ENTITY_ID * eid_PTR;
	dword n,flags;
	for(n=0;n<=CommonLayers.lss.Layer_max_index;n++)
	{
		l_PTR = CommonLayers.Layer_Table[n];
		if(l_PTR == null) continue;
		flags = l_PTR->ls.Flags & LRFLAG_FROZEN; if(flags != 0) continue;	// skip frozen layers
		flags = l_PTR->ls.Flags & LRFLAG_REALIZE; if(flags == 0) continue;	// skip non realize layers
		eid_PTR = l_PTR->GetID();
		while(eid_PTR)
		{
			if(ValidateEntity(eid_PTR)) 
			{
				if(!Atoms_PTR[eid_PTR->atom_position]->as.Deleted)
				{
				//trace("Re St: %s",eid_PTR->pName);
				RDTSCB(ticks)
				PUSH_CONTROL(eid_PTR->pointer,eid_PTR->class_code,CTP_REALIZE)
				((ENTITY *)eid_PTR->pointer)->Realize(deltatime);
				POP_CONTROL(0)
				RDTSCE(ticks)
				//trace("Re En: %s",eid_PTR->pName);

				pAs = &Atoms_PTR[eid_PTR->atom_position]->as;
				pAs->Realize_ticks_av = ticks;
				if(pAs->Realize_ticks_max < ticks) pAs->Realize_ticks_max = ticks;

				}
			}
			eid_PTR = l_PTR->GetNextID();
		}
	}
//	*/
	ProcessRunEnd(SECTION_REALIZE);
	UNGUARD
}

void CORE::ProcessSystemMessagesBuffer()
{
	GUARD(CORE::ProcessSystemMessagesBuffer())
	ProcessRunStart(SECTION_PROCESS_MESSAGE_SYSTEM);

	LAYER * l_PTR;
	ENTITY_ID * eid_PTR;
	dword n,flags,i;
	for(n=0;n<=CommonLayers.lss.Layer_max_index;n++)
	{
		l_PTR = CommonLayers.Layer_Table[n];
		if(l_PTR == null) continue;
		flags = l_PTR->ls.Flags & LRFLAG_FROZEN; if(flags != 0) continue;	// skip frozen layers
		flags = l_PTR->ls.Flags & LRFLAG_SYS_MESSAGES; if(flags == 0) continue;	// skip layers
		eid_PTR = l_PTR->GetID();
		while(eid_PTR)
		{
			if(ValidateEntity(eid_PTR)) 
			{
				if(!Atoms_PTR[eid_PTR->atom_position]->as.Deleted)
				for(i=0;i<SystemMessagesNum;i++) 
				{
					PUSH_CONTROL(eid_PTR->pointer,eid_PTR->class_code,CTP_MESSAGE_SYSTEM)
					((ENTITY *)eid_PTR->pointer)->ProcessMessage(MessageStack[i].iMsg,MessageStack[i].wParam,MessageStack[i].lParam);
					POP_CONTROL(0)
				}
			}
			eid_PTR = l_PTR->GetNextID();
		}
	}
	SystemMessagesNum = 0;

	ProcessRunEnd(SECTION_PROCESS_MESSAGE_SYSTEM);

	UNGUARD
}

void CORE::TraceCurrent()
{
	GUARD(CORE::TraceCurrent)
	CONTROL_BLOCK cb;
	VMA * pClass;
	if(Control_Stack.Read(&cb))
	{
		if(cb.ctp != CTP_CORE) 
		{
			pClass = FindVMA(cb.class_code);
			if(pClass) strcpy(gstring,pClass->GetName());
			trace("Object: %s -> %s",
				//Classes_Table.GetString(cb.class_code),
				gstring,
				CTP_NAME(cb.ctp));
			return;
		}
	}
	trace("Object: CORE"); 
	UNGUARD
}

// OR operation with core exceptions mask inversion, returned current mask state
dword CORE::SetExceptions(dword _flags)
{
	dword Old_EM;
	Old_EM = Exceptions_Mask;
	Exceptions_Mask |= _flags;
	return Old_EM;
}

// AND operation with core exceptions mask, returned current mask state
dword CORE::ClrExceptions(dword _flags)
{
	dword Old_EM;
	Old_EM = Exceptions_Mask;
	Exceptions_Mask &= (~_flags);
	return Old_EM;
}

// save core state
bool CORE::SaveState(char * file_name)
{
	DWORD n,i;
	HANDLE fh;
	ENTITY_STATE_GEN_R esg;
	char FullPath[MAX_PATH];
	char PathBuffer[MAX_PATH];

	if(!file_name) return false;

#ifdef _XBOX
	
	if(file_name[0] == 'U' && file_name[1] ==':')
	{
		strcpy(FullPath,file_name);

	}
	else
	{
		WORD FileBuffer[MAX_PATH];
		for(n=0;file_name[n];n++)
		{
			FileBuffer[n] = file_name[n];
		}
		FileBuffer[n] = 0;
		if(XCreateSaveGame("U:\\",FileBuffer,OPEN_ALWAYS,0,PathBuffer,sizeof(PathBuffer)) != ERROR_SUCCESS)
		{
			trace("cant create save game");
		}
		strcpy(FullPath,PathBuffer);
		strcat(FullPath,file_name);
	}
	
#else
	strcpy(FullPath,file_name);
#endif

	fio->SetDrive(XBOXDRIVE_NONE);
	fh = fio->_CreateFile(FullPath,GENERIC_WRITE|GENERIC_READ,0,CREATE_ALWAYS);
	fio->SetDrive();
	
	if(fh == INVALID_HANDLE_VALUE) return false;
	Compiler.SaveState(fh);
	fio->_CloseHandle(fh);

#ifdef _XBOX
	strcat(PathBuffer,"saveimage.xbx");
	CopyFile("d:\\resource\\textures\\saveimage.xbx",PathBuffer,FALSE);
#endif

#ifndef STATE_COMPRESSION_ON
	return true;
#endif
//*/	
	//----------------------------------------------------------------------------------------------
	// compression
	fh = fio->_CreateFile("tempout.tmp",GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,CREATE_ALWAYS);
	if(fh == INVALID_HANDLE_VALUE) return false;
	Compiler.SaveState(fh);
	DWORD dwR;
	HANDLE pfh;
	char * pSource;
	char * pDestination;

	DWORD nSourceSize;
	DWORD nPackedSize;
	char sPfname[MAX_PATH];

	strcpy(sPfname,PathBuffer);
	strcat(sPfname,"p");
	nSourceSize = fio->_GetFileSize(fh,0);
	

	pSource = (char *)malloc(nSourceSize);
	pDestination = 0;
	nPackedSize = 0;

	fio->_SetFilePointer(fh,0,0,FILE_BEGIN);
	fio->_ReadFile(fh,pSource,nSourceSize,&dwR);
	if(pSource)
	{
		if(Compiler.Compress.Pack(pSource,nSourceSize,pDestination,nPackedSize))
		{
			pfh = fio->_CreateFile(PathBuffer,GENERIC_WRITE,FILE_SHARE_READ,CREATE_ALWAYS);
			
			if(fh != INVALID_HANDLE_VALUE)
			{
				fio->_WriteFile(pfh,pDestination,nPackedSize,&dwR);
				fio->_CloseHandle(pfh);
			}
			free(pDestination);
		}
		free(pSource);
	}
	fio->_CloseHandle(fh);
	fio->_DeleteFile("tempout.tmp");


	return true;	
//============================================================================================================

	esg.Init(fio,fh);

	// write core state structure
	esg.SetState("m",sizeof(CoreState),&CoreState); esg.CloseState();


//============================================================================================================
	fio->_CloseHandle(fh);
	return true;	


	ENTITY_ID id;
	ENTITY_ID * id_PTR;
	ENTITY * entity_PTR;
//	ENTITY_STATE_GEN_R esg;
	

	
//	char * char_PTR;
	dword Priority;
	dword layer_objects;

	GUARD(CORE::SaveState)

	VALIDATE_API_CALLS

	fh = fio->_CreateFile(PathBuffer,GENERIC_WRITE,FILE_SHARE_READ,CREATE_ALWAYS);
	if(fh == INVALID_HANDLE_VALUE) return false;
	
	CoreState.engine_version = ENGINE_VERSION;

	esg.Init(fio,fh);

	// write core state
	esg.SetState("m",sizeof(CoreState),&CoreState);
	esg.CloseState();

	// write classes count

	


//	esg.SetState("u",Classes_Table.GetStringsCount());
	// write classes table
/*	for(n=0;n<Classes_Table.GetStringsCount();n++)
	{
		char_PTR = Classes_Table.GetString(n);
		if(char_PTR == null) THROW;
		esg.SetState("s",char_PTR);
	}
*/
	esg.CloseState();

	// switch on all engine objects
	SetEntityScanLayer(0);
	// write objects id and atoms states
	for(n=0;n<CoreState.Atoms_number;n++)
	{
		if(n == 0) { if(!GetEntity(&id)) THROW;	} 
		else  { if(!GetEntityNext(&id)) THROW;	}
		esg.SetState("m",sizeof(id),&id);
		esg.SetState("m",sizeof(Atoms_PTR[id.atom_position]->as),&Atoms_PTR[id.atom_position]->as);
		esg.CloseState();
	}
	// write common layers info ------------------------------------------------------------------------------
	esg.SetState("u",CommonLayers.lss.Layers_number); esg.CloseState();
	esg.SetState("m",sizeof(CommonLayers.lss),&CommonLayers.lss);
	for(i=0;i<_MAX_LAYERS;i++)
	{
		if(CommonLayers.Layer_Table[i] == null) continue;
		// save layers index, name length, name and layer state
		esg.SetState("uusm",i,strlen(CommonLayers.Layer_Table[i]->Name) + 1,CommonLayers.Layer_Table[i]->Name,
			sizeof(CommonLayers.Layer_Table[i]->ls),&CommonLayers.Layer_Table[i]->ls);
		layer_objects = CommonLayers.Layer_Table[i]->Count;
		// save layer objects count
		esg.SetState("u",layer_objects);
		// save layer objects ids
		for(n=0;n<layer_objects;n++)
		{
			if(n == 0) { id_PTR = CommonLayers.Layer_Table[i]->GetID(&Priority);	}
			else { id_PTR = CommonLayers.Layer_Table[i]->GetNextID(&Priority);	}
			if(id_PTR == null) THROW;
			// save object priority and id
			esg.SetState("um",Priority,sizeof(ENTITY_ID),id_PTR);
		}
		esg.CloseState();
	}
	//---------------------------------------------------------------------------------------------------------


/*
	// at this time services doesnt store/restore data
	// write services --------------------------------------------------
	dword class_code;
	SERVICE * service_PTR;
	esg.SetState("u",Services_List.GetCount());
	for(i=0;i<Services_List.GetCount();i++)
	{
		if(i == 0) service_PTR = Services_List.GetService(&class_code);
		else service_PTR = Services_List.GetNextService(&class_code);
		if(service_PTR == null) THROW;
		esg.SetState("u",class_code);
	}
	for(i=0;i<Services_List.GetCount();i++)
	{
		if(i == 0) service_PTR = Services_List.GetService(&class_code);
		else service_PTR = Services_List.GetNextService(&class_code);
		if(service_PTR == null) THROW;
		esg.SetState("uu",class_code,Services_List.GetRef(service_PTR));
		service_PTR->CreateState(&esg);

	}
	//-------------------------------------------------------------------
*/


	// write objects id and objects data
	for(n=0;n<CoreState.Atoms_number;n++)
	{
		if(n == 0) { if(!GetEntity(&id)) THROW;	} 
		else { if(!GetEntityNext(&id)) THROW;	}
		// save objects id
		esg.SetState("m",sizeof(id),&id);
		entity_PTR = GetEntityPointer(&id);
		if(entity_PTR == null) THROW;
		// transfer control to entity, for saving objects data
		PUSH_CONTROL(entity_PTR,id.class_code,CTP_CREATESTATE)
		entity_PTR->CreateState(&esg);
		POP_CONTROL(0)
		esg.CloseState();
	}

	esg.CloseState();

	fio->_CloseHandle(fh);
	UNGUARD
	return false;
}

// force core to load state file at the start of next game loop, return false if no state file
bool CORE::InitiateStateLoading(char * file_name)
{
	GUARD(CORE::InitiateStateLoading)

	VALIDATE_API_CALLS // no necessary

	HANDLE fh;
	PUSH_CONTROL(0,0,0)
	fio->SetDrive(XBOXDRIVE_NONE);
	fh = fio->_CreateFile(file_name,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
	fio->SetDrive();
	if(fh == INVALID_HANDLE_VALUE) return false;
	fio->_CloseHandle(fh);
	if(State_file_name) delete State_file_name;

	State_file_name = (char *)NEW char[strlen(file_name) + 1];
	if(State_file_name == null) THROW;
	strcpy(State_file_name,file_name);
	POP_CONTROL(0)
	UNGUARD
	return true;
}


void CORE::ProcessStateLoading()
{
	GUARD(CORE::ProcessStateLoading)

	VALIDATE_API_CALLS

	HANDLE fh;
	ENTITY_STATE_R es;
	CORE_STATE cs;
	VBUFFER v_Buff;
	ATOM_STATE atom_state;
	ENTITY_ID entity_id;
	ENTITY * entity_PTR;
	LAYER_STATE ls;

	dword classes_count;
	dword entities_in_state;
	dword n,i;
//	char * char_PTR;
	dword Priority;
	dword layers_num;
	dword size;
	dword layer_index;
	dword layer_objects;
	

	if(!State_file_name) return;

	State_loading = true;
	EraseEntities();
	
	
	char * pUnpacked;
	DWORD nUnpackedSize;
	char * pDestination;
	DWORD nPackedSize;
	HANDLE pfh;
	DWORD dwR;
#ifndef STATE_COMPRESSION_ON	

	fio->SetDrive(XBOXDRIVE_NONE);
	fh = fio->_CreateFile(State_file_name,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
	fio->SetDrive();
	if(fh == INVALID_HANDLE_VALUE) return;
	Compiler.LoadState(fh);
	fio->_CloseHandle(fh);

	if(State_file_name) delete State_file_name; State_file_name = null;
	State_loading = false;

	return;
#endif

//*/

	//------------------------------------------------------------------------------------
	// decompression
	pfh = fio->_CreateFile(State_file_name,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
	if(pfh == INVALID_HANDLE_VALUE) return;
	nPackedSize = fio->_GetFileSize(pfh,0);
	if(nPackedSize == INVALID_FILE_SIZE) { fio->_CloseHandle(pfh); return;}
	pDestination = (char *)malloc(nPackedSize);
	if(pDestination == 0) THROW;
	fio->_ReadFile(pfh,pDestination,nPackedSize,&dwR);
	fio->_CloseHandle(pfh);
	
	pUnpacked = 0;
	nUnpackedSize = 0;
	if(Compiler.Compress.Unpack(pDestination,nPackedSize,pUnpacked,nUnpackedSize))
	{
		pfh = fio->_CreateFile("tempout.tmp",GENERIC_WRITE,FILE_SHARE_READ,CREATE_ALWAYS);
		if(pfh != INVALID_HANDLE_VALUE)
		{
			fio->_WriteFile(pfh,pUnpacked,nUnpackedSize,&dwR);
			fio->_CloseHandle(pfh);
		} else THROW;
		free(pUnpacked);
	} else THROW;
	free(pDestination);

	// open state file
	//fh = fio->_CreateFile(State_file_name,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
	fh = fio->_CreateFile("tempout.tmp",GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
	delete State_file_name; State_file_name = null;
	if(fh == INVALID_HANDLE_VALUE)
	{
		trace(State_file_name);
		__THROW(NON_FATAL,cant open state file);
	}

	//Program.LoadState(fh);
	Compiler.LoadState(fh);
	fio->_CloseHandle(fh);
	fio->_DeleteFile("tempout.tmp");

	if(State_file_name) delete State_file_name; State_file_name = null;
	State_loading = false;

	return;

//====================================================================================

	// init state reader object
	es.Init(fio,fh);

	// read core state
	es.MemoryBlock(sizeof(cs),(char *)&cs);
	
	// check engine version
	if(cs.engine_version != CoreState.engine_version) _THROW(incorrect state file);
	
	// validate space size 
	if(cs.Atoms_max_orbit >= cs.Atoms_space) THROW;
	
	// reset core state and load new
	LoadCoreState(cs);

	// memorize number of entities in state
	entities_in_state = cs.Atoms_number;

	// loaded saved classes count
	classes_count = es.Dword();

	// verify class table size
//	if(classes_count != Classes_Table.GetStringsCount()) _THROW(incorrect state file);

	// verify class table
/*	for(n=0;n<Classes_Table.GetStringsCount();n++)
	{
		// get pointer to loaded class name
		char_PTR = Classes_Table.GetString(n);
		if(char_PTR == null) THROW;
		// set buffer size
		v_Buff.Size(strlen(char_PTR) + 1);
		// read save class name
		es.String(strlen(char_PTR) + 1,v_Buff.Ptr);
		// compare saved class name with currently loaded
		if(stricmp(v_Buff.Ptr,char_PTR) != 0) _THROW(incorrect state file);
	}
*/	
	// restore atom structure and recreate objects
	for(n=0;n<entities_in_state;n++)
	{
		// read atom id
		es.MemoryBlock(sizeof(entity_id),(char *)&entity_id);
		// read atom state
		es.MemoryBlock(sizeof(atom_state),(char *)&atom_state);
		// restore atom and entity
		RestoreEntity(entity_id,atom_state);
	}


	
	// create common layers and read layers data --------------------------------------------------------------
	layers_num = es.Dword();
	es.MemoryBlock(sizeof(CommonLayers.lss),(char *)&CommonLayers.lss);
	for(i=0;i<layers_num;i++)
	{
		// read layer index
		layer_index = es.Dword();
		// read layer name length
		size = es.Dword();
		v_Buff.Size(size);
		// read layer name
		es.String(size,v_Buff.Ptr);
		// read layer state
		es.MemoryBlock(sizeof(ls),(char *)&ls);
		// recreate layer
		CommonLayers.Fit(layer_index,v_Buff.Ptr,ls);
		layer_objects = es.Dword();
		// restore objects information
		for(n=0;n<layer_objects;n++)
		{
			// read object priority
			Priority = es.Dword();
			// read object id
			es.MemoryBlock(sizeof(ENTITY_ID),(char*)&entity_id);
			// assign object to layer
			LayerAdd(v_Buff.Ptr,entity_id,Priority,false);
		}
	}
	//---------------------------------------------------------------------------------------------------------



	// restore services --------------------------------------------------
	// at this time services doesnt store/restore data
	//-------------------------------------------------------------------

	
	// upload objects states
	for(n=0;n<entities_in_state;n++)
	{
		// read entity id
		es.MemoryBlock(sizeof(entity_id),(char *)&entity_id);
		// obtain entity pointer
		entity_PTR = GetEntityPointer(&entity_id);
		if(entity_PTR == null) THROW;
		#ifndef EX_OFF
		try { 
		#endif
			// prepare to transfer control to object
			PUSH_CONTROL(entity_PTR,entity_id.class_code,CTP_LOADSTATE)
			// transfer control for loading object state
			if(!entity_PTR->LoadState(&es))
			{
				// state load error, detected by object
				TraceCurrent();
				_THROW(state loading error);	
			}
			POP_CONTROL(0)
		#ifndef EX_OFF
		}
		catch(_EXS xobj) 
		{
			TraceCurrent();
			POP_CONTROL(0)
			throw;
		}
		catch(...)
		{
			TraceCurrent();
			POP_CONTROL(0)
			_THROW(LoadState);
		}
		#endif
	}

	// close state file
	CloseHandle(fh);
	// inform system, that there is no need to load root object
	Root_flag = true;
	// state loading complete
	State_loading = false;
	gdi_display.Print("state loaded");
	UNGUARD
}

void CORE::ProcessRunStart(dword section_code)
{
	GUARD(CORE::ProcessRunStart)
	SERVICE * service_PTR;
	dword class_code;
	dword section;
	service_PTR = Services_List.GetService(class_code);
	while(service_PTR)
	{
		
		section = service_PTR->RunSection();
		if(section == section_code)
		{
			PUSH_CONTROL(service_PTR,class_code,CTP_RUNSTART)
			service_PTR->RunStart();
			POP_CONTROL(0)
		}
		service_PTR = Services_List.GetServiceNext(class_code);
	}
	UNGUARD
}

void CORE::ProcessRunEnd(dword section_code)
{
	GUARD(CORE::ProcessRunEnd)
	SERVICE * service_PTR;
	dword class_code;
	dword section;
	service_PTR = Services_List.GetService(class_code);
	while(service_PTR)
	{
		section = service_PTR->RunSection();
		if(section == section_code)
		{
			PUSH_CONTROL(service_PTR,class_code,CTP_RUNSTART)
			service_PTR->RunEnd();
			POP_CONTROL(0)
		}
		service_PTR = Services_List.GetServiceNext(class_code);
	}
	UNGUARD
}

void CORE::EngineDisplay(bool on)
{
	gdi_display.Switch(on);
	//InvalidateRect(0,0,false);	// xbox
}

dword CORE::EngineFps()
{
	return Timer.fps;
}

bool CORE::InitObject(ENTITY_ID eid)
{
	ENTITY_ID entity_id;
	ENTITY *  Entity_PTR;
	GUARD(CORE::InitObject)
	
	entity_id = eid;
	if(!ValidateEntity(&entity_id)) return false;
	
	Entity_PTR = (ENTITY *)entity_id.pointer;

	PUSH_CONTROL(Entity_PTR,entity_id.class_code,CTP_INIT)
	#ifndef EX_OFF
	try { 
	#endif
		if(!Entity_PTR->Init())	
		{ 
			MarkEntityAsDeleted(Entity_PTR->GetID());	
			CheckAutoExceptions(_X_NO_CREATE_ENTITY);
			return false; 
		}	
	#ifndef EX_OFF
	}
	catch(_EXS xobj) 
	{
		TraceCurrent();
		POP_CONTROL(0)
		System_Api.entityID_PTR = null;
		throw;
	}
	catch(...)
	{
		TraceCurrent();
		POP_CONTROL(0)
		System_Api.entityID_PTR = null;
		_THROW(cant init);
	}
	#endif
	POP_CONTROL(0)

	UNGUARD
	return true;
}

dword CORE::SetTimer(dword elapse,ENTITY_ID id)
{
	//SetTimer
	return 0;
}

void CORE::SetDeltaTime(long delta_time)
{
	Timer.SetDelta(delta_time);
}

void CORE::SystemMessages(ENTITY_ID eid, bool on)
{
/*	LayerCreate(SYSTEMMESSAGES_LAYER,false,false);
	LayerSetFlags(SYSTEMMESSAGES_LAYER,LRFLAG_SYS_MESSAGES);
	if(on) LayerAdd(SYSTEMMESSAGES_LAYER,eid,0);
	else LayerDel(SYSTEMMESSAGES_LAYER,eid);*/
}

dword CORE::GetDeltaTime()
{
	return Timer.GetDeltaTime();
}

dword CORE::GetRDeltaTime()
{
	return Timer.rDelta_Time;
}

float CORE::GetKeyState(dword key_code, dword * value)
{
	return 0;
	//return Input->GetKeyState(key_code,value);
}

ATTRIBUTES * CORE::Entity_GetAttributeClass(ENTITY_ID * id_PTR, char * name)
{
	ENTITY * pE;
	pE = GetEntityPointer(id_PTR);
	if(pE == 0) return 0;
	if(pE->AttributesPointer == 0) return 0;
	//FindAClass(ATTRIBUTES * pRoot, char * access_string)
	return pE->AttributesPointer->FindAClass(pE->AttributesPointer,name);
	//return pE->AttributesPointer->GetAttributeClass(name);
}

char *	CORE::Entity_GetAttribute(ENTITY_ID * id_PTR, char * name)
{
	ENTITY * pE;
	pE = GetEntityPointer(id_PTR);
	if(pE == 0) return 0;
	if(pE->AttributesPointer == 0) return 0;
	return pE->AttributesPointer->GetAttribute(name);
}

DWORD	CORE::Entity_GetAttributeAsDword(ENTITY_ID * id_PTR, char * name, DWORD def)
{
	ENTITY * pE;
	pE = GetEntityPointer(id_PTR);
	if(pE == 0) return def;
	if(pE->AttributesPointer == 0) return def;
	return pE->AttributesPointer->GetAttributeAsDword(name,def);

}

FLOAT	CORE::Entity_GetAttributeAsFloat(ENTITY_ID * id_PTR, char * name, FLOAT def)
{
	ENTITY * pE;
	pE = GetEntityPointer(id_PTR);
	if(pE == 0) return def;
	if(pE->AttributesPointer == 0) return def;
	return pE->AttributesPointer->GetAttributeAsFloat(name,def);
}

BOOL	CORE::Entity_SetAttribute(ENTITY_ID * id_PTR, char * name, char * attribute)
{
	ENTITY * pE;
	pE = GetEntityPointer(id_PTR);
	if(pE == 0) return false;
	if(pE->AttributesPointer == 0) return false;
	return pE->AttributesPointer->SetAttribute(name,attribute);
}

BOOL	CORE::Entity_SetAttributeUseDword(ENTITY_ID * id_PTR, char * name, DWORD val)
{
	ENTITY * pE;
	pE = GetEntityPointer(id_PTR);
	if(pE == 0) return false;
	if(pE->AttributesPointer == 0) return false;
	return pE->AttributesPointer->SetAttributeUseDword(name,val);
}

BOOL	CORE::Entity_SetAttributeUseFloat(ENTITY_ID * id_PTR, char * name, FLOAT val)
{
	ENTITY * pE;
	pE = GetEntityPointer(id_PTR);
	if(pE == 0) return false;
	if(pE->AttributesPointer == 0) return false;
	return pE->AttributesPointer->SetAttributeUseFloat(name,val);
}

void CORE::Entity_SetAttributePointer(ENTITY_ID * id_PTR, ATTRIBUTES * pA)
{
	ENTITY * pE;
	pE = GetEntityPointer(id_PTR);
	if(pE == 0) return;
	pE->AttributesPointer = pA;
}

dword	CORE::Entity_AttributeChanged(ENTITY_ID * id_PTR, ATTRIBUTES * pA)
{
	ENTITY * pE;
	pE = GetEntityPointer(id_PTR);
	if(pE == 0) return 0;
	return pE->AttributeChanged(pA);
}

ATTRIBUTES * CORE::Entity_GetAttributePointer(ENTITY_ID * id_PTR)
{
	ENTITY * pE;
	pE = GetEntityPointer(id_PTR);
	if(pE == 0) return 0;
	return pE->AttributesPointer;
}

void CORE::EraseEntities()
{
	DWORD n;
	for(n=0;n<=CoreState.Atoms_max_orbit;n++) 
	{
		if(Atoms_PTR[n] == null) continue;
		EraseEntity(Atoms_PTR[n]->atom_id);
	}
}

void CORE::DeleteEntities()
{
	DWORD n;
	for(n=0;n<=CoreState.Atoms_max_orbit;n++) 
	{
		if(Atoms_PTR[n] == null) continue;
		DeleteEntity(Atoms_PTR[n]->atom_id);
	}
}

void CORE::ClearEvents()
{
	//Program.ClearEvents();
	Compiler.ClearEvents();
}

void CORE::AppState(bool state)
{
	if(Controls) Controls->AppState(state);
}

dword CORE::MakeHashValue(const char * string)
{
  dword hval = 0;
  dword g;
  char v;

  while(*string != 0)
  {
	v = *string++;
	if ('A' <= v && v <= 'Z') v += 'a' - 'A';

    hval = (hval<<4) + (unsigned long int)v;
    g = hval & ((unsigned long int) 0xf << (32 - 4));
    if(g != 0)
		{
			hval ^= g >> (32 - 8);
			hval ^= g;
		}
  }
  return hval;
}

//==========================================================================================================================
// end
//==========================================================================================================================

void CORE::DumpEntitiesInfo()
{
	dword n;
	char * ptr;
	VMA * pClass;

	trace("Script commands executed: %d", dwNumberScriptCommandsExecuted);
	
	trace("Entity Dump -----------------------------------");
	trace("Allocated Memory: %f kb in %d block(s)",
		(Memory_Service.Allocated_memory_user + Memory_Service.Allocated_memory_system)/1024.0f,//(1024*1024),
		Memory_Service.Blocks);
	if(!Atoms_PTR) 
	{
		trace("No entities");
		return;
	}
	
	for(n=0;n<=CoreState.Atoms_max_orbit;n++) 
	{
		if(Atoms_PTR[n] == null) continue;
		
		pClass = FindVMA(Atoms_PTR[n]->atom_id.class_code);
		if(pClass) ptr = pClass->GetName();
		//ptr = Classes_Table.GetString(Atoms_PTR[n]->atom_id.class_code);
		if(ptr)
		{
			trace("Class: %s", ptr);
			trace("     : Realize:  Cur( %d ) Max( %d )", Atoms_PTR[n]->as.Realize_ticks_av,Atoms_PTR[n]->as.Realize_ticks_max);
			trace("     : Execute:  Cur( %d ) Max( %d )", Atoms_PTR[n]->as.Execute_ticks_av,Atoms_PTR[n]->as.Execute_ticks_max);

		}
	}

	trace(" ------- Running objects ------- ");
	LAYER * l_PTR;
	ENTITY_ID * eid_PTR;
	dword flags;
	for(n=0;n<=CommonLayers.lss.Layer_max_index;n++)
	{
		l_PTR = CommonLayers.Layer_Table[n];
		if(l_PTR == null) continue;
		
		flags = l_PTR->ls.Flags & LRFLAG_FROZEN; 
		if(flags != 0) 
		{
			trace("LAYER: %s frozen -------------------------------------",l_PTR->Name);
			continue;
		}
		trace("LAYER: %s --------------------------------------",l_PTR->Name);
		eid_PTR = l_PTR->GetID();
		while(eid_PTR)
		{
			trace("ENTITY:");
			if(ValidateEntity(eid_PTR)) 
			{
				pClass = FindVMA(Atoms_PTR[eid_PTR->atom_position]->atom_id.class_code);
				if(pClass) ptr = pClass->GetName();
				trace("        %s",ptr);
				if(Atoms_PTR[eid_PTR->atom_position]->as.Deleted) trace("        deleted");
				
			}
			eid_PTR = l_PTR->GetNextID();
		}
	}

	Sleep(200);
}

void * CORE::GetSaveData(char * file_name, long & data_size)
{
	return Compiler.GetSaveData(file_name,data_size);
	
}
	
bool CORE::SetSaveData(char * file_name, void * data_ptr, long data_size)
{
	return Compiler.SetSaveData(file_name,data_ptr,data_size);
}

void CORE::GetMemoryState(MSTATE * pMemStateStructure)
{
	if(pMemStateStructure != 0)
	{
		long blocks = 0;
		float mem = 0.0;
		for(DWORD n=0;n<SBCNUM;n++)
		{
			blocks += Memory_Service.SBCounter[n];
			mem += Memory_Service.SBCounter[n]*n;

		}
		
		pMemStateStructure->nBlocksNum = blocks;
		pMemStateStructure->nMemorySize = dword(mem);//*/
		

		//pMemStateStructure->nBlocksNum = Memory_Service.Blocks;
		//pMemStateStructure->nMemorySize = Memory_Service.Allocated_memory_user;
	}
}


DWORD CORE::SetScriptFunction(IFUNCINFO * pFuncInfo)
{
	return Compiler.SetScriptFunction(pFuncInfo);
}

void CORE::DeleteScriptFunction(DWORD nFuncHandle)
{
	Compiler.DeleteScriptFunction(nFuncHandle);
}

char * CORE::EngineIniFileName()
{
	return ENGINE_INI_FILE_NAME;
}

DWORD CORE::AttributeName2Code(const char * pAttributeName)
{
	return Compiler.SCodec.Convert(pAttributeName);
}

void * CORE::GetScriptVariable(const char * pVariableName, DWORD * pdwVarIndex)
{
	DWORD dwVarIndex;
	VARINFO vi;
	
	dwVarIndex = Compiler.VarTab.FindVar(pVariableName);
	if(dwVarIndex == INVALID_VAR_CODE) return 0;

	if(!Compiler.VarTab.GetVar(vi,dwVarIndex))
	{
		return 0;
	}
	if(pdwVarIndex) *pdwVarIndex = dwVarIndex;
	return vi.pDClass;
}

void CORE::SetNetActive(bool bActive)
{
	bNetActive = bActive;
}

bool CORE::IsNetActive() const
{
	return bNetActive;
}
