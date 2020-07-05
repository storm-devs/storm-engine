#include <stdio.h>
#include "compiler.h"
#include "core.h"
#include "sstring.h"
#include "s_debug.h"

//#define STARFORCE_PROTECTION

#define SKIP_COMMENT_TRACING
#define TRACE_OFF
#define INVALID_SEGMENT_INDEX	0xffffffff
#ifndef INVALID_ARRAY_INDEX
	#define INVALID_ARRAY_INDEX		0xffffffff
#endif
#define INVALID_OFFSET			0xffffffff
#define DSL_INI_VALUE			0
//#define IOBUFFER_SIZE			65535
#define RDTSC_B(x)	{ _asm rdtsc _asm mov x,eax }
#define RDTSC_E(x)	{ _asm rdtsc _asm sub eax,x _asm mov x,eax }
#define WARNINGS_OFF
//#define DTRACEOFF
#define SBUPDATE 4
#define DEF_COMPILE_EXPRESSIONS

#include "..\..\common_h\vapi.h"

#ifdef _XBOX
#include "..\..\common_h\dx8render.h"
#include "..\..\soundservice\vsoundservice.h"
#endif

extern bool bTraceFilesOff;
extern VAPI * _CORE_API;
extern CORE Core;
extern FILE_SERVICE File_Service;
//extern char * FuncNameTable[];
extern INTFUNCDESC IntFuncTable[];
extern S_DEBUG CDebug;
extern dword dwNumberScriptCommandsExecuted;

COMPILER::COMPILER()
{
	CompilerStage = CS_SYSTEM;
	File_Service._DeleteFile(COMPILER_LOG_FILENAME);
	File_Service._DeleteFile(COMPILER_ERRORLOG_FILENAME);
	LabelTable.SetStringDataSize(sizeof(DWORD));
	LabelUpdateTable.SetStringDataSize(sizeof(DOUBLE_DWORD));
	ProgramDirectory = 0;
	pEventMessage = 0;
	bCompleted = false;
	SegmentsNum = 0;
	SegmentTable = 0;
//	FuncTab.KeepNameMode(true);	// true for displaying internal functions names
//	VarTab.KeepNameMode(false);
//	EventTab.KeepNameMode(false);
//	DefTab.KeepNameMode(false);
	InitInternalFunctions();
	bWriteCodeFile = false;
	bDebugInfo = false;
	strcpy(DebugSourceFileName,"no debug information");	
	DebugSourceLine = 0;
	InstructionPointer = 0;
	pCompileTokenTempBuffer = 0;
	pRun_fi = 0;
	pRunCodeBase = 0;
	SStack.SetVCompiler(this);
	VarTab.SetVCompiler(this);
	ClassTab.SetVCompiler(this);
	ClassTab.InitSystemClasses();
	srand(GetTickCount());
	bEntityUpdate = true; 
	pDebExpBuffer = 0;
	nDebExpBufferSize = 0;
	bDebugExpressionRun = false;
	bTraceMode = true;
	DebugTraceFileName[0] = 0;
	nDebugTraceLineCode = 0;
	bBreakOnError = false;
	bRuntimeLog = false;
	nRuntimeLogEventsNum = 0;
	pRuntimeLogEvent = 0;
	nRuntimeTicks = 0;
	nRuntimeLogEventsBufferSize = 0;
	nIOBufferSize = 0;
	pIOBuffer = 0;

	rAX.SetVCompiler(this);
	rBX.SetVCompiler(this);
	ExpressionResult.SetVCompiler(this);
	rAP = 0;
	//bScriptTrace = false;
	bFirstRun = true;
	

}

COMPILER::~COMPILER()
{
	Release();
}

void COMPILER::Release()
{
	DWORD n;
	//DWORD m;
	//FUNCINFO fi;

	// debug log
/*	if(bRuntimeLog)
	{
		double fMaxTime;
		DWORD nFuncCode;
		trace("Script Function Time Usage[func name/code(release mode) : ticks]");
		for(m=0;m<FuncTab.GetFuncNum();m++)
		{
			fMaxTime = 0;
			nFuncCode = 0;
			for(n=0;n<FuncTab.GetFuncNum();n++)
			{
				FuncTab.GetFuncX(fi,n);
				if(fi.fTimeUsage == -1.0) continue;
				if(fMaxTime <= fi.fTimeUsage)
				{
					fMaxTime = fi.fTimeUsage;
					nFuncCode = n;
				}
				//if(fi.name)	trace("  %s : %f",fi.name,fi.fTimeUsage);
				//else trace("  %d : %f",n,fi.fTimeUsage);
			}

			FuncTab.GetFuncX(fi,nFuncCode);
			if(fi.name)	trace("  %s",fi.name); else trace("  %d",n);
			trace("  ticks summary  : %.0f",fi.fTimeUsage);
			trace("  calls          : %d",fi.nNumberOfCalls);
			if(fi.nNumberOfCalls != 0) trace("  average ticks  : %.0f",fi.fTimeUsage/fi.nNumberOfCalls);
			

			FuncTab.SetTimeUsage(nFuncCode,-1.0);
			trace("");

			//if(fi.name)	trace("  %s : %f",fi.name,fi.fTimeUsage);
			//else trace("  %d : %f",n,fi.fTimeUsage);
		}

		if(pRuntimeLogEvent)
		{
			trace("Script Run Time Log [sec : ms]");
			for(n=0;n<nRuntimeLogEventsNum;n++)
			{
				trace("  %d : %d",n,pRuntimeLogEvent[n]);
			}
		}
		bRuntimeLog = false;
	}
*/
	//--------------------------------------------------------
	
	if(SegmentTable)
	{
		for(n=0;n<SegmentsNum;n++)
		{
			if(SegmentTable[n].name) delete SegmentTable[n].name;
			if(SegmentTable[n].pData) delete SegmentTable[n].pData;
			if(SegmentTable[n].pCode) delete SegmentTable[n].pCode;
			if(SegmentTable[n].Files_list) 
			{
				SegmentTable[n].Files_list->Release();
				delete SegmentTable[n].Files_list;
			}
			SegmentTable[n].Files_list = 0;

		}
		delete SegmentTable; SegmentTable = 0;
		SegmentsNum = 0;
	}
	LabelTable.Release();
	LabelUpdateTable.Release();
	Token.Reset();
	if(ProgramDirectory) delete ProgramDirectory; ProgramDirectory = 0;
	RunningSegmentID = INVALID_SEGMENT_INDEX;
	if(pCompileTokenTempBuffer) delete pCompileTokenTempBuffer; pCompileTokenTempBuffer = 0;

	FuncTab.Release();
	VarTab.Release();
	ClassTab.Release();
	DefTab.Release();
	SStack.Release();
	EventTab.Release();
	EventMsg.Release();
	SCodec.Release();
	LibriaryFuncs.Release();

	if(pDebExpBuffer) delete pDebExpBuffer; pDebExpBuffer = 0;
	nDebExpBufferSize = 0;

	
	if(pRuntimeLogEvent) delete pRuntimeLogEvent;
	nRuntimeLogEventsNum = 0;
	pRuntimeLogEvent = 0;
	nRuntimeLogEventsBufferSize = 0;

	rAX.Release();
	rBX.Release();
	ExpressionResult.Release();
	//Token.Release();
}

void __declspec(noinline) __cdecl COMPILER::SetProgramDirectory(char * dir_name)
{
	if(ProgramDirectory) delete ProgramDirectory; ProgramDirectory = 0;
	if(dir_name)
	{
		
		ProgramDirectory = NEW char[strlen(dir_name) + strlen("\\") + 1];
		strcpy(ProgramDirectory,dir_name);
		strcat(ProgramDirectory,"\\");
	}
#ifndef _XBOX
	CDebug.SetProgramDirectory(dir_name);
#endif
}

// load file into memory
char * COMPILER::LoadFile(char * file_name, DWORD & file_size, bool bFullPath)
{
	HANDLE fh;
	DWORD  fsize;
	DWORD  dwR;
	char * pData;
	char buffer[MAX_PATH];
	
	if(!bFullPath)
	{

		if(ProgramDirectory)
		{
			strcpy(buffer,ProgramDirectory);
#ifdef STARFORCE_PROTECTION
			strcat(buffer,"xyz");
#endif
			strcat(buffer,file_name);
		} 
		else
		{

#ifdef STARFORCE_PROTECTION
			strcpy(buffer,"xyz");
			strcat(buffer,file_name);
#else
			strcpy(buffer,file_name);
#endif

			
		}
	}

	file_size = 0;
	if(bFullPath)
	{
		// f expand
		/*DWORD n,m;
		m = 0;
		for(n=0;file_name[n];n++)
		{
			buffer[m] = file_name[n];
			if(buffer[m] == '\\')
			{
				m++;
				buffer[m] = '\\';
			}
			m++;
		}
		buffer[m] = 0;*/

		fh = Core.fio->_CreateFile(file_name,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
	}
		else
	fh = Core.fio->_CreateFile(buffer,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
	
	if(fh == INVALID_HANDLE_VALUE) return 0;
	fsize = Core.fio->_GetFileSize(fh,0);
	if(fsize == INVALID_FILE_SIZE) {Core.fio->_CloseHandle(fh); return 0;}
	
	pData = (char *)NEW char[fsize + 1];
	Core.fio->_ReadFile(fh,pData,fsize,&dwR);
	if(fsize != dwR) {delete pData; Core.fio->_CloseHandle(fh); return 0;}
	Core.fio->_CloseHandle(fh);
	file_size = fsize;
	pData[fsize] = 0;
	return pData;
}

// write to compilation log file
void COMPILER::Trace(char * data_PTR, ...)
{
#ifdef 	TRACE_OFF
	return;
#endif
	if(bTraceFilesOff) return;
	//char LogBuffer[MAX_PATH + MAX_PATH];
	char LogBuffer[4096];
	if(data_PTR == 0) return;
	HANDLE file_h = CreateFile(COMPILER_LOG_FILENAME,GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	SetFilePointer(file_h,0,0,FILE_END);
	va_list args;
	va_start(args,data_PTR);
	_vsnprintf(LogBuffer,sizeof(LogBuffer) - 4,data_PTR,args);
	strcat(LogBuffer,"\x0d\x0a");
	DWORD bytes;
	WriteFile(file_h,LogBuffer,strlen(LogBuffer),&bytes,0);
	va_end(args);
	CloseHandle(file_h);
	_flushall();
}

// write to compilation log file
void COMPILER::DTrace(char * data_PTR, ...)
{
#ifdef DTRACEOFF
	return;
#endif

	if(bTraceFilesOff) return;
	//char LogBuffer[MAX_PATH + MAX_PATH];
	char LogBuffer[4096];
	if(data_PTR == 0) return;
	HANDLE file_h = fio->_CreateFile(COMPILER_LOG_FILENAME,GENERIC_WRITE,FILE_SHARE_READ,OPEN_ALWAYS);
	fio->_SetFilePointer(file_h,0,0,FILE_END);
	va_list args;
	va_start(args,data_PTR);
	_vsnprintf(LogBuffer,sizeof(LogBuffer) - 4,data_PTR,args);
	strcat(LogBuffer,"\x0d\x0a");
	DWORD bytes;
	fio->_WriteFile(file_h,LogBuffer,strlen(LogBuffer),&bytes);
	va_end(args);
	fio->_CloseHandle(file_h);
	_flushall();
}

// append one block of code to another
// return new pointer
/*char * COMPILER::AppendProgram(char * pBase_program, long Base_program_size, char * pAppend_program, long Append_program_size, long& new_program_size)
{
	char * pTemp;
	char * pBase;
	new_program_size = Base_program_size + Append_program_size + 2;
	
	pBase = NEW char[new_program_size];
	pTemp = pBase;
	memcpy(pTemp,pBase_program,Base_program_size);
	pTemp += Base_program_size;
	pTemp[0] = ' ';	pTemp++;	// code blocks separator ' ' to prevent data merging
	memcpy(pTemp,pAppend_program,Append_program_size);
	delete pBase_program;
	delete pAppend_program;
	return pBase;
}*/

bool COMPILER::AppendProgram(char * & pBase_program, DWORD & Base_program_size, char * pAppend_program, DWORD & Append_program_size, bool bAddLinefeed)
{
	DWORD offset;
	offset = Base_program_size;
	if(pAppend_program == 0) return false;
	if(bAddLinefeed)
	{
		pBase_program = (char *)RESIZE(pBase_program,Base_program_size + Append_program_size + 3); // +1 for terminating zero, +1 for 0xd +1 for 0xa
		Base_program_size = Base_program_size + Append_program_size + 2;
		memcpy(&pBase_program[offset],pAppend_program,Append_program_size);
		pBase_program[Base_program_size - 2] = 0xd; // code blocks separator to prevent data merging
		pBase_program[Base_program_size - 1] = 0xa; // code blocks separator to prevent data merging
		pBase_program[Base_program_size] = 0; // terminating zero
		delete pAppend_program;
		Append_program_size += 2;
		return true;
	}
		pBase_program = (char *)RESIZE(pBase_program,Base_program_size + Append_program_size + 2); // +1 for terminating zero, +1 for ';'
		Base_program_size = Base_program_size + Append_program_size + 1;
		memcpy(&pBase_program[offset],pAppend_program,Append_program_size);
		pBase_program[Base_program_size - 1] = ';'; // code blocks separator to prevent data merging
		pBase_program[Base_program_size] = 0; // terminating zero
		delete pAppend_program;
		Append_program_size += 1;
		return true;
}


void COMPILER::SetError(char * data_PTR, ...)
{
	if(bDebugExpressionRun) return;
	if(bTraceFilesOff) return;
	char LogBuffer[MAX_PATH + MAX_PATH];
	char ErrorBuffer[MAX_PATH + MAX_PATH];
	if(data_PTR == 0) return;
	HANDLE file_h = fio->_CreateFile(COMPILER_ERRORLOG_FILENAME,GENERIC_WRITE,FILE_SHARE_READ,OPEN_ALWAYS);
	fio->_SetFilePointer(file_h,0,0,FILE_END);
	va_list args;
	va_start(args,data_PTR);
	_vsnprintf(LogBuffer,sizeof(LogBuffer) - 4,data_PTR,args);
	strcat(LogBuffer,"\x0d\x0a");
	DWORD bytes;
	FindErrorSource();
	
	switch(CompilerStage)
	{
		case CS_SYSTEM:
			wsprintf(ErrorBuffer,"ERROR - file: %s; line: %d",DebugSourceFileName,DebugSourceLine + 1);
		break;
		case CS_COMPILATION:
			wsprintf(ErrorBuffer,"COMPILE ERROR - file: %s; line: %d",DebugSourceFileName,DebugSourceLine + 1);
		break;
		case CS_RUNTIME:
			wsprintf(ErrorBuffer,"RUNTIME ERROR - file: %s; line: %d",DebugSourceFileName,DebugSourceLine + 1);
		break;
	}
	//wsprintf(ErrorBuffer,"ERROR - file: %s; line: %d",DebugSourceFileName,DebugSourceLine + 1);
	lstrcat(ErrorBuffer,"\x0d\x0a");
	fio->_WriteFile(file_h,ErrorBuffer,lstrlen(ErrorBuffer),&bytes);
	fio->_WriteFile(file_h,LogBuffer,lstrlen(LogBuffer),&bytes);
	va_end(args);
	fio->_FlushFileBuffers(file_h);
	fio->_CloseHandle(file_h);
	//_flushall();

#ifndef _XBOX
	if(bBreakOnError) CDebug.SetTraceMode(TMODE_MAKESTEP);
#endif
}

void COMPILER::SetWarning(char * data_PTR, ...)
{
#ifdef WARNINGS_OFF
	return;
#endif
	if(bTraceFilesOff) return;
	if(bDebugExpressionRun) return;
	char LogBuffer[MAX_PATH + MAX_PATH];
	char ErrorBuffer[MAX_PATH + MAX_PATH];
	if(data_PTR == 0) return;
	HANDLE file_h = fio->_CreateFile(COMPILER_LOG_FILENAME,GENERIC_WRITE,FILE_SHARE_READ,OPEN_ALWAYS);
	fio->_SetFilePointer(file_h,0,0,FILE_END);
	va_list args;
	va_start(args,data_PTR);
	_vsnprintf(LogBuffer,sizeof(LogBuffer) - 4,data_PTR,args);
	strcat(LogBuffer,"\x0d\x0a");
	DWORD bytes;
	FindErrorSource();
	
	wsprintf(ErrorBuffer,"WARNING - file: %s; line: %d",DebugSourceFileName,DebugSourceLine + 1);
	lstrcat(ErrorBuffer,"\x0d\x0a");
	fio->_WriteFile(file_h,ErrorBuffer,lstrlen(ErrorBuffer),&bytes);
	fio->_WriteFile(file_h,LogBuffer,lstrlen(LogBuffer),&bytes);
	va_end(args);
	fio->_FlushFileBuffers(file_h);
	fio->_CloseHandle(file_h);
	//_flushall();


/*	if(bDebugExpressionRun) return;
	char LogBuffer[MAX_PATH + MAX_PATH];
	if(data_PTR == 0) return;
	HANDLE file_h = CreateFile(COMPILER_LOG_FILENAME,GENERIC_WRITE,FILE_SHARE_WRITE,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	SetFilePointer(file_h,0,0,FILE_END);
	va_list args;
	va_start(args,data_PTR);
	_vsnprintf(LogBuffer,sizeof(LogBuffer) - 4,data_PTR,args);
	strcat(LogBuffer,"\x0d\x0a");
	DWORD bytes;
	WriteFile(file_h,"WARNING: ",strlen("WARNING: "),&bytes,0);
	WriteFile(file_h,LogBuffer,strlen(LogBuffer),&bytes,0);
	va_end(args);
	CloseHandle(file_h);
	_flushall();*/
}

void COMPILER::LoadPreprocess()
{
	INIFILE * engine_ini;

	engine_ini = _CORE_API->fio->OpenIniFile(api->EngineIniFileName());
	if(engine_ini != null)
	{
		if(engine_ini->GetLong("script","debuginfo",0) == 0) 
		{
			bDebugInfo = false;
			//FuncTab.KeepNameMode(false);
			//VarTab.KeepNameMode(false);
			//DefTab.KeepNameMode(false);
			//EventTab.KeepNameMode(false);
		}
		else 
		{
			bDebugInfo = true;
			//FuncTab.KeepNameMode(true);
			//VarTab.KeepNameMode(true);
			//DefTab.KeepNameMode(true);
			//EventTab.KeepNameMode(true);
		}
		if(engine_ini->GetLong("script","codefiles",0) == 0) bWriteCodeFile = false;
		else bWriteCodeFile = true;
		
		if(engine_ini->GetLong("script","runtimelog",0) == 0) bRuntimeLog = false;
		else bRuntimeLog = true;

		//if(engine_ini->GetLong("script","tracefiles",0) == 0) bScriptTrace = false;
		//else bScriptTrace = true;
		

		delete engine_ini;
	}
#ifndef _XBOX
	INIFILE * ini = Core.fio->OpenIniFile(PROJECT_NAME);
	if(ini)
	{
		bBreakOnError = (ini->GetLong("options","break_on_error",0)==1);
		delete ini;
	}
#endif

}

bool COMPILER::CreateProgram(char * file_name)
{
	bool bRes;
/*	INIFILE * engine_ini;

	engine_ini = _CORE_API->fio->OpenIniFile(api->EngineIniFileName());
	if(engine_ini != null)
	{
		if(engine_ini->GetLong("script","debuginfo",0) == 0) 
		{
			bDebugInfo = false;
			FuncTab.KeepNameMode(false);
			VarTab.KeepNameMode(false);
			DefTab.KeepNameMode(false);
			EventTab.KeepNameMode(false);
		}
		else 
		{
			bDebugInfo = true;
			FuncTab.KeepNameMode(true);
			VarTab.KeepNameMode(true);
			DefTab.KeepNameMode(true);
			EventTab.KeepNameMode(true);
		}
		if(engine_ini->GetLong("script","codefiles",0) == 0) bWriteCodeFile = false;
		else bWriteCodeFile = true;
		

		delete engine_ini;
	}
//*/
	LoadPreprocess();
	bRes = BC_LoadSegment(file_name);

/*	for(DWORD m=0;m<HASH_TABLE_SIZE;m++)
	{
		DTrace("HashIndex[%d]",m);
		for(DWORD n=0;n<SCodec.HTable[m].nStringsNum;n++)
		{
			DTrace(SCodec.HTable[m].ppDat[n]);
		}
	}*/
	return bRes;
}

bool __declspec(noinline) __cdecl COMPILER::Run()
{
	DWORD function_code;
	pRun_fi = 0;
	function_code = FuncTab.FindFunc("Main");
	DATA * pResult;
	bFirstRun = false;
	BC_Execute(function_code,pResult);
	pRun_fi = 0;

	
	


	//DATA Result;
	//Result.SetVCompiler(this);
	//ProcessDebugExpression("Characters[45].id",Result);

	/*for(DWORD m=0;m<HASH_TABLE_SIZE;m++)
	{
		DTrace("HashIndex[%d]",m);
		for(DWORD n=0;n<SCodec.HTable[m].nStringsNum;n++)
		{
			DTrace(SCodec.HTable[m].ppDat[n]);
		}
	}*/

	return true;
}

void COMPILER::FindErrorSource()
{
	DWORD segment_index;
	segment_index = GetSegmentIndex(RunningSegmentID);
	if(segment_index == INVALID_SEGMENT_INDEX) return;
	
	DWORD index;
	DWORD ip;
	DWORD token_data_size;
	DWORD file_line_offset;
	DWORD file_name_size;
	char * pCodeBase;
	S_TOKEN_TYPE Token_type;
	
	ip = 0;
	DebugSourceLine = 0;
	file_line_offset = 0;
	pCodeBase = SegmentTable[segment_index].pCode;
	do
	{
		Token_type = (S_TOKEN_TYPE)pCodeBase[ip]; ip++;
		if((BYTE)pCodeBase[ip] < 0xff) { token_data_size = (BYTE)pCodeBase[ip]; ip++; }
		else {ip++; memcpy(&token_data_size,&pCodeBase[ip],sizeof(dword)); ip += sizeof(dword);}
		
		if(ip >= InstructionPointer)
		{
			//DebugSourceLine -= file_line_offset;
			return;
		}
		switch(Token_type)
		{
			case DEBUG_LINE_CODE:
				memcpy(&DebugSourceLine,&pCodeBase[ip],sizeof(dword));
			break;
			case DEBUG_FILE_NAME:
				index = ip;
				memcpy(&file_line_offset,&pCodeBase[index],sizeof(dword)); index += sizeof(dword);
				memcpy(&file_name_size,&pCodeBase[index],sizeof(dword)); index += sizeof(dword);
				memcpy(DebugSourceFileName,&pCodeBase[index],file_name_size); index += file_name_size;
				DebugSourceFileName[file_name_size] = 0;
				DebugSourceLine = DSL_INI_VALUE;
			break;
		}
		ip += token_data_size;	// step to next instruction
	} while(Token_type != END_OF_PROGRAMM);
	
	return;
}


void COMPILER::SetEventHandler(char * event_name, char * func_name,long flag, bool bStatic)
{
//	DWORD event_code;
	DWORD func_code;
	FUNCINFO fi;

	if(event_name == null)
	{
		SetError("Invalid event name for SetEventHandler");
		return;
	}
	if(func_name == null)
	{
		SetError("Invalid function name for SetEventHandler");
		return;
	}

	func_code = FuncTab.FindFunc(func_name);
	if(func_code == INVALID_FUNC_CODE)
	{
		SetError("Invalid function code in SetEventHandler");
		return;
	}

	if(!FuncTab.GetFunc(fi,func_code))
	{
		SetError("func not found eror");
		return;
	}
	
	EventTab.AddEventHandler(event_name,func_code,fi.segment_id,flag,bStatic);
}

void COMPILER::DelEventHandler(char * event_name, char * func_name)
{
	DWORD func_code;
	if(event_name == null)
	{
		SetError("Bad event name");
		return;
	}

	if(func_name == null)
	{
		SetError("Bad func name");
		return;
	}

	func_code = FuncTab.FindFunc(func_name);
	if(func_code == INVALID_FUNC_CODE)
	{
		SetError("Invalid function code in DelEventHandler");
		return;
	}
	
	EventTab.SetStatus(event_name,func_code,FSTATUS_DELETED);

	long n;
	S_EVENTMSG * pM;
	for(n=0;n<(long)EventMsg.GetClassesNum();n++)
	{
		pM = EventMsg.Read(n);
		if(!pM->pEventName) continue;
		if(pM->ProcessTime(0)) continue;	// skip events, possible executed on this frame
		if(stricmp(pM->pEventName,event_name)==0)
		{
			EventMsg.Del(n);
			n--;
		}
	}
}

VDATA * COMPILER::ProcessEvent(char * event_name)
{
	DWORD n;
	DWORD event_code;
	DWORD func_code;
	VDATA * pVD;
	DATA * pResult;
	MESSAGE * pMem;
	EVENTINFO ei;
	DWORD current_debug_mode;

	DWORD dwRDTSC, nTicks;
	DWORD nTimeOnEvent;

	RDTSC_B(dwRDTSC);

	bEventsBreak = false;
	
	nTimeOnEvent = GetTickCount();
#ifndef _XBOX
	current_debug_mode = CDebug.GetTraceMode();
#else 
	current_debug_mode = 0;
#endif

	pVD = 0;
	if(event_name == null) 
	{
		SetError("Invalid event name in ProcessEvent");
		return 0;
	}

	event_code = EventTab.FindEvent(event_name);
	if(event_code == INVALID_EVENT_CODE) return 0;	// no handlers
	EventTab.GetEvent(ei,event_code);
	for(n=0;n<ei.elements;n++)
	{
		func_code = ei.pFuncInfo[n].func_code;
		if(ei.pFuncInfo[n].status != FSTATUS_NORMAL) continue;
		pMem = pEventMessage;
		if(pMem) 
		{
			pMem->Move2Start();
		}
		
		
		DWORD nStackVars;
		nStackVars = SStack.GetDataNum();	// remember stack elements num
		RDTSC_B(nTicks);
		BC_Execute(ei.pFuncInfo[n].func_code,pResult);
		RDTSC_E(nTicks);
		
		FUNCINFO fi;
		//if(FuncTab.GetFuncX(fi,ei.pFuncInfo[n].func_code))
		if(FuncTab.GetFuncX(fi,func_code))
		{
			if(fi.return_type != TVOID)
			{
				fi.return_type = TVOID;
			}
		}//*/
		
		

		if(EventTab.GetEvent(ei,event_code))	// to be sure event still exist
		{
			if(n < ei.elements)
			{
				FuncTab.AddTime(ei.pFuncInfo[n].func_code,nTicks);
			}
		}

		pEventMessage = pMem;
		if(pResult) pVD = pResult->GetVarPointer();
		if(pResult) SStack.Pop();

		if(nStackVars != SStack.GetDataNum())
		{
			SStack.InvalidateFrom(nStackVars);	// restore stack state - crash situation
			pVD = 0;
			SetError("process event stack error");
		}

		if(bEventsBreak) break;
	}
	
	
	nTimeOnEvent = GetTickCount() - nTimeOnEvent;
	
	nRuntimeTicks += nTimeOnEvent;

	pRun_fi = 0;
#ifndef _XBOX
	if(current_debug_mode == TMODE_CONTINUE) CDebug.SetTraceMode(TMODE_CONTINUE);
	//SetFocus(Core.App_Hwnd);		// VANO CHANGES
#endif

	RDTSC_E(dwRDTSC);

	// VANO CHANGES - remove in release
	if (api->Controls->GetDebugAsyncKeyState('5') < 0 && api->Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0)
	{
		api->Trace("evnt: %d, %s", dwRDTSC, event_name);
	}

	return pVD;
}

void COMPILER::ProcessEvent(char * event_name, MESSAGE * pMs)
{
	pEventMessage = pMs;
	ProcessEvent(event_name);
	pEventMessage = 0;
}

VDATA * COMPILER::ProcessEvent(char * event_name, MESSAGE message)
{
	VDATA * pVD;
	pEventMessage = &message;
	pVD = ProcessEvent(event_name);
	pEventMessage = 0;
	return pVD;
}

char * COMPILER::GetName()
{
	return SegmentTable[0].Files_list->GetString(0);
	//return Files_list.GetString(0);
}

DWORD COMPILER::GetSegmentIndex(DWORD segment_id)
{
	DWORD n;
	for(n=0;n<SegmentsNum;n++)
	{
		if(SegmentTable[n].id == segment_id) return n;
	}
	return INVALID_SEGMENT_INDEX;
}
// function release global variables, functions and labels reference for segment
// mark segment for subsequent unload (on ProcessFrame)
void COMPILER::UnloadSegment(char * segment_name)
{
	DWORD n;
	DWORD segment_id;
//	OFFSET_INFO offset_info;
	
	for(n=0;n<SegmentsNum;n++)
	{
		if(strcmp(SegmentTable[n].name,segment_name)==0)
		{
			segment_id = SegmentTable[n].id;
			SegmentTable[n].bUnload = true;
			FuncTab.InvalidateBySegmentID(segment_id);
			VarTab.InvalidateBySegmentID(segment_id);
			ClassTab.InvalidateBySegmentID(segment_id);
			EventTab.InvalidateBySegmentID(segment_id);
			DefTab.InvalidateBySegmentID(segment_id);
			if(SegmentTable[n].Files_list) 
			{
				SegmentTable[n].Files_list->Release();
			}
			break;
		}
	}
}

bool COMPILER::BC_SegmentIsLoaded(char * file_name)
{
	DWORD n;
	if(file_name == null)
	{
		SetError("Invalid segment name");
		return false;
	}
	for(n=0;n<SegmentsNum;n++)
	{
		if(strcmp(SegmentTable[n].name,file_name)==0) return true;
	}
	return false;
}



bool COMPILER::BC_LoadSegment(char * file_name)
{
	DWORD  n;
	DWORD  id;
	DWORD  index;
	bool   found;

	LabelUpdateTable.Release();

	if(file_name == null)
	{
		SetError("Invalid segment name");
		return false;
	}
	
	//Trace("Loading segment: %s", file_name);

	// check for already loaded
	for(n=0;n<SegmentsNum;n++)
	{
		if(strcmp(SegmentTable[n].name,file_name)==0) 
		{
			SetWarning("Segment already loaded: %s",file_name);
			return true;
		}
	}

	// compute new segment id --------------------------
	id = 0;
	found = false;
	while(!found)
	{
		found = true;
		for(n=0;n<SegmentsNum;n++)
		{
			if(SegmentTable[n].id == id) found = false;
		}
		if(!found) id++;
	}

	index = SegmentsNum;
	SegmentsNum++;
	SegmentTable = (SEGMENT_DESC *)RESIZE(SegmentTable,SegmentsNum*sizeof(SEGMENT_DESC));
	
	SegmentTable[index].name = NEW char[strlen(file_name) + 1];
	strcpy(SegmentTable[index].name,file_name);
	SegmentTable[index].id = id;
	SegmentTable[index].bUnload = false;
	SegmentTable[index].pData = 0;
	SegmentTable[index].pCode = 0;
	SegmentTable[index].BCode_Program_size = 0;
	SegmentTable[index].BCode_Buffer_size = 0;
	
	SegmentTable[index].Files_list = NEW STRINGS_LIST;
	SegmentTable[index].Files_list->SetStringDataSize(sizeof(OFFSET_INFO));
	bool bRes;
	bRes = Compile(SegmentTable[index]);
	if(!bRes)
	{
		delete SegmentTable[index].name;
		delete SegmentTable[index].Files_list;
		SegmentsNum--;
		SegmentTable = (SEGMENT_DESC *)RESIZE(SegmentTable,SegmentsNum*sizeof(SEGMENT_DESC));
		FuncTab.InvalidateBySegmentID(id);
		VarTab.InvalidateBySegmentID(id);
		ClassTab.InvalidateBySegmentID(id);
		EventTab.InvalidateBySegmentID(id);
		DefTab.InvalidateBySegmentID(id);
	}
	return bRes;
}

bool COMPILER::ProcessDebugExpression(char * pExpression, DATA & Result)
{
	if(pExpression == 0) return false;

	DWORD nDataSize;

	nDataSize = strlen(pExpression) + strlen("return ") + 2;
	if(nDataSize > nDebExpBufferSize)
	{
		pDebExpBuffer = (char *)RESIZE(pDebExpBuffer,nDataSize);
		nDebExpBufferSize = nDataSize;
	}
	wsprintf(pDebExpBuffer,"return %s;",pExpression);
	return ProcessDebugExpression0(pDebExpBuffer,Result);

}

bool COMPILER::SetOnDebugExpression(char * pLValue, char * pRValue, DATA & Result)
{
	if(pLValue == 0 || pRValue == 0) return false;

	DWORD nDataSize;

	nDataSize = strlen(pLValue) + strlen(pRValue) + 5;
	if(nDataSize > nDebExpBufferSize)
	{
		pDebExpBuffer = (char *)RESIZE(pDebExpBuffer,nDataSize);
		nDebExpBufferSize = nDataSize;
	}
	wsprintf(pDebExpBuffer,"%s = %s;",pLValue,pRValue);
	return ProcessDebugExpression0(pDebExpBuffer,Result);
}

bool COMPILER::ProcessDebugExpression0(char * pExpression, DATA & Result)
{
	SEGMENT_DESC Segment;
	STRINGS_LIST DbgLocalSL;
	bool bRes;
	DATA * pResult;

	pResult = 0;

	if(pExpression == 0) return false;

	bDebugExpressionRun = true;

	ZeroMemory(&Segment,sizeof(Segment));
	Segment.name = "Debug Expression";
	Segment.id = RunningSegmentID;
	Segment.bUnload = false;
	Segment.pData = 0;
	Segment.pCode = 0;
	Segment.BCode_Program_size = 0;
	Segment.BCode_Buffer_size = 0;
	Segment.Files_list = &DbgLocalSL;
	Segment.Files_list->SetStringDataSize(sizeof(OFFSET_INFO));
	
	
	if(pRun_fi)
	{
		CurrentFuncCode = pRun_fi->code;
	}

	try
	{
		bRes = Compile(Segment,pDebExpBuffer,strlen(pDebExpBuffer) + 1);
	}
	catch(...)
	{
		return false;
	}
	
	if(!bRes)
	{
		if(Segment.pCode) delete Segment.pCode;
		bDebugExpressionRun = false;
		return false;
	}

	

	//DWORD	ip;
//	DWORD	mem_ip;
	DWORD	mem_InstructionPointer;
	FUNCINFO * mem_pfi;
	char *	mem_codebase;
	//DWORD mem_CurrentFuncCode;
		

	// save current pointers values
	mem_InstructionPointer = InstructionPointer;
	//mem_ip = ip;
	mem_pfi = pRun_fi;
	mem_codebase = pRunCodeBase;
	//mem_CurrentFuncCode = CurrentFuncCode;

	//trace("Segment.pCode : %s",pDebExpBuffer);

	//DWORD old_data_num;
	//old_data_num = SStack.GetDataNum(); trace("old_data_num: %d",old_data_num);

	DWORD stackN;

	stackN = SStack.GetDataNum();

	try
	{
		bRes = BC_Execute(INVALID_FUNC_CODE,pResult,Segment.pCode);
	}
	//old_data_num = SStack.GetDataNum(); trace("new_data_num: %d",old_data_num);
	
	catch(...)
	{
		bRes = false;
	}

	// restore
	pRun_fi = mem_pfi;	
	InstructionPointer = mem_InstructionPointer;
	//ip = mem_ip;
	//RunningSegmentID = pRun_fi->segment_id;
	pRunCodeBase = mem_codebase;

	if(Segment.pCode) delete Segment.pCode;

	if(pResult) 
	{
		Result.Copy(pResult);
		//SStack.Pop();
	}
	SStack.InvalidateFrom(stackN);

	bDebugExpressionRun = false;
	return bRes;
}

void COMPILER::ProcessFrame(DWORD DeltaTime)
{
	DWORD n,i;

	if(Core.Timer.Ring) AddRuntimeEvent();
	
	for(n=0;n<SegmentsNum;n++)
	{
		if(!SegmentTable[n].bUnload) continue;
		// unload segment of program
		if(SegmentTable[n].pData) delete SegmentTable[n].pData;
		if(SegmentTable[n].pCode) delete SegmentTable[n].pCode;
		if(SegmentTable[n].name) delete SegmentTable[n].name;
		if(SegmentTable[n].Files_list) delete SegmentTable[n].Files_list;
		SegmentTable[n].Files_list = 0;

		for(i=n;i<(SegmentsNum-1);i++)
		{
			SegmentTable[i] = SegmentTable[i+1];
		}
		SegmentsNum--;
		SegmentTable = (SEGMENT_DESC *)RESIZE(SegmentTable,SegmentsNum*sizeof(SEGMENT_DESC));
		// reset search
		n = 0;
	}

	EventTab.ProcessFrame();

	long ln;
	S_EVENTMSG * pMsg;
	for(ln=0;ln<(long)EventMsg.GetClassesNum();ln++)
	{
		pMsg = EventMsg.Read(ln);
		if(pMsg->bInvalide) continue;
		if(pMsg->bProcess == false)
		{
			pMsg->ProcessTime(DeltaTime);
			continue;
		}
		if(pMsg->ProcessTime(DeltaTime))
		{
			pMsg->Invalidate();
			ProcessEvent(pMsg->pEventName,pMsg->pMessageClass);
			//EventMsg.Del(ln);
			//ln--;
			/*ProcessEvent(pMsg->pEventName,pMsg->pMessageClass);
			EventMsg.Del(ln);
			ln--;*/
		}
	}
	EventMsg.RemoveInvalidated();
}

void COMPILER::ResizeBCodeBuffer(SEGMENT_DESC& Segment,DWORD add_size)
{
	if((Segment.BCode_Program_size + add_size) >= Segment.BCode_Buffer_size)
	{
		Segment.BCode_Buffer_size += BCODE_BUFFER_BLOCKSIZE;
		Segment.pCode = (char *)RESIZE(Segment.pCode,Segment.BCode_Buffer_size);
	}
}

void COMPILER::CompileToken(SEGMENT_DESC& Segment,S_TOKEN_TYPE Token_type, DWORD data_blocks_num,...)
{
	DWORD write_size;
	DWORD data_size;
	DWORD n;
	va_list args;
	char * data_ptr;
	
	//DTrace("token: %s : %s",Token.GetTypeName(Token_type),Token.GetData());
	
	// write instruction (1 byte) ----------------------------------------------
	write_size = 1;
	ResizeBCodeBuffer(Segment,write_size);
	Segment.pCode[Segment.BCode_Program_size] = Token_type;
	Segment.BCode_Program_size += write_size;
	//--------------------------------------------------------------------------
	
	if(data_blocks_num == 0)
	{
		// write byte length of token data -------------------------------------
		write_size = 1;
		ResizeBCodeBuffer(Segment,write_size);
		Segment.pCode[Segment.BCode_Program_size] = 0;
		Segment.BCode_Program_size += write_size;
		return;
	}

	pCompileTokenTempBuffer = (char *)RESIZE(pCompileTokenTempBuffer,data_blocks_num * (sizeof(char *) + sizeof(dword)));
	
	// gather data blocks information and count total data size ----------------
	va_start(args,data_blocks_num);
	write_size = 0;
	for(n=0;n<data_blocks_num;n++)
	{
		data_ptr = va_arg(args,char*);
		data_size = va_arg(args,dword);
		memcpy(pCompileTokenTempBuffer + n*(sizeof(char*)+sizeof(dword)),&data_ptr,sizeof(char*));
		memcpy(pCompileTokenTempBuffer + n*(sizeof(char*)+sizeof(dword)) + sizeof(char*),&data_size,sizeof(dword));
		write_size += data_size;
	}
	va_end(args);
	//--------------------------------------------------------------------------

	// write total length of token
	if(write_size < 0xff)
	{
		// write byte length of token data
		ResizeBCodeBuffer(Segment,1);
		Segment.pCode[Segment.BCode_Program_size] = (BYTE)write_size;
		Segment.BCode_Program_size += 1;
	}
	else
	{
		// write dword length of token data
		ResizeBCodeBuffer(Segment,sizeof(dword) + 1);
		Segment.pCode[Segment.BCode_Program_size] = (BYTE)0xff;
		Segment.BCode_Program_size += 1;
		memcpy(&Segment.pCode[Segment.BCode_Program_size],&write_size,sizeof(dword));
		Segment.BCode_Program_size += sizeof(dword);
	}
	//--------------------------------------------------------------------------

	// write token data blocks
	ResizeBCodeBuffer(Segment,write_size);
	for(n=0;n<data_blocks_num;n++)
	{
		memcpy(&data_ptr,pCompileTokenTempBuffer + n*(sizeof(char*)+sizeof(dword)),sizeof(char*));
		memcpy(&data_size,pCompileTokenTempBuffer + n*(sizeof(char*)+sizeof(dword)) + sizeof(char*),sizeof(dword));
		memcpy(&Segment.pCode[Segment.BCode_Program_size],data_ptr,data_size);
		Segment.BCode_Program_size += data_size;
	}
	
}

bool COMPILER::InitInternalFunctions()
{
	DWORD n;
	DWORD func_code;
	DWORD internal_functions_num;
	FUNCINFO fi;
	
	// register internal functions ------------
	
	internal_functions_num = GetIntFunctionsNum();
	for(n=0;n<internal_functions_num;n++)
	{
		fi.segment_id = INTERNAL_SEGMENT_ID;
		//fi.name = FuncNameTable[n];
		fi.name = IntFuncTable[n].pName;
		fi.offset = n;
		fi.arguments = GetInternalFunctionArgumentsNum(n);
		fi.decl_file_name = "engine";
		fi.decl_line = 0;
		fi.return_type = IntFuncTable[n].ReturnType;
		func_code = FuncTab.AddFunc(fi);

		if(func_code == INVALID_FUNC_CODE)
		{
			func_code = FuncTab.FindFunc(fi.name);
			FuncTab.GetFunc(fi,func_code);
			//SetError("Duplicate function name: %s",fi.name);
			SetError("Function [%s] already declared in: %s line %d",fi.name,fi.decl_file_name,fi.decl_line);
			return false;
		}
	}
	return true;
}

bool COMPILER::Compile(SEGMENT_DESC& Segment, char * pInternalCode, DWORD pInternalCodeSize)
{
	char dbg_file_name[MAX_PATH];
	char file_name[MAX_PATH];
	char func_name[MAX_PATH];
	char var_name[MAX_PATH];
	char * pProgram;
	char * pApend_file;
	char * pSegmentSource;
	FUNCINFO fi;
	VARINFO vi;
	LVARINFO lvi;
	CLASSINFO ci;
	CLASSINFO cci;
	DWORD SegmentSize;
	DWORD Program_size;
	DWORD func_code;
	DWORD var_code;
	DWORD def_code;
	DWORD inout;
	DWORD bracket_inout;
	DWORD n;
	DWORD file_code;
	DWORD Control_offset;
	DWORD Current_line;
	DWORD Append_file_size;
	STRINGS_LIST BlockTable;
	S_TOKEN_TYPE Token_type;
	DEFINFO di;
	CLASS_COMPONENT cc;

	CompilerStage = CS_COMPILATION;

	RunningSegmentID = INVALID_SEGMENT_INDEX;

#ifdef _XBOX
	if(bFirstRun)
	{
		VDX8RENDER * pDX = (VDX8RENDER *)api->CreateService("dx8render");
		if(pDX) pDX->ProgressView();
	}
#endif


//	bool bCDStop;
	bool bFunctionBlock;
	long  lvalue;
	float fvalue;
	
	
	Control_offset = 0;
	strcpy(file_name,Segment.name);
	
	if(pInternalCode == 0)
	{
		Segment.Files_list->AddUnicalString(file_name);
		file_code = Segment.Files_list->GetStringCode(file_name);
		pProgram = 0;
		Program_size = 0;
		pSegmentSource = LoadFile(file_name,SegmentSize);
		AppendProgram(pProgram,Program_size,pSegmentSource,SegmentSize,true);
		Segment.pData = pProgram;
		if(pProgram == 0) 
		{
			SetError("file not found: %s",file_name);
			return false;
		}
		CurrentFuncCode = INVALID_FUNC_CODE;
	} 
	else
	{
		pProgram = pInternalCode;
		Program_size = pInternalCodeSize;
		Segment.pData = pProgram;
	}
	
	
	//CurrentFuncCode = INVALID_FUNC_CODE;

	
	// register functions and variables ---------
	strcpy(func_name,"null");
	inout = 0;
	bracket_inout = 0;
	bFunctionBlock = false;
	//pProgram = Segment.pData;
	Token.SetProgram(pProgram,pProgram);
	if(bDebugInfo) 
	{
		strcpy(DebugSourceFileName,Segment.name);	
	}
	DebugSourceLine = DSL_INI_VALUE;
	bool bExtern;
	bool bImport;
	bExtern = false;
	bImport = false;
	bool bDotFlag;
	bDotFlag = false;
	do
	{
		Token_type = Token.Get();
		bDotFlag = false;
		if(Token_type == DOT)
		{
			Token_type = Token.Get();
			bDotFlag = true;
		}
		switch(Token_type)
		{
		case COMMENT:
			DebugSourceLine += Token.TokenLines();
		break;
		case LABEL:
			Token.StepBack();
			Token.StepBack();	
			Token.Get();	// label name

			if(!LabelTable.AddUnicalString(Token.GetData())) SetError("duplicate label: %s",Token.GetData());
			Token.Get();
		break;
		case CASE_COMMAND:
			Token.Get();	// condition value
			if(Token.Get()!= LABEL) {SetError("invalid case syntax");return false;}
		break;
		case SEPARATOR:
			if(bFunctionBlock && bExtern) 
			{
				bFunctionBlock = 0;
				bExtern = false;
			}
			if(bFunctionBlock && bImport) 
			{
				bFunctionBlock = 0;
				bImport = false;
			}
		break;
		case DEBUG_LINEFEED:
			DebugSourceLine++;
		break;
		case DEBUG_FILE_NAME:
			DebugSourceLine = DSL_INI_VALUE;
			strcpy(DebugSourceFileName,Token.GetData());
		break;
		case INCLUDE_LIBRIARY:
			SCRIPT_LIBRIARY * pLib;
			SLIBHOLDER * pH;
			bool bFound;
			VMA * pClass;


			//-----------------------------------------------------
			// check if already loaded
			bFound = false;
			for(n=0;n<LibriaryFuncs.GetClassesNum();n++)
			{
				pH = LibriaryFuncs.Read(n);
				if(stricmp(pH->pName,Token.GetData())==0)
				{
					// already loaded
					bFound = true;
					break;
				}
			}
			if(bFound) break;
			//-----------------------------------------------------

			pClass = Core.FindVMA(Token.GetData());
			if(!pClass)
			{
				SetWarning("cant load libriary '%s'",Token.GetData()); 
				break;
			}
				
			pLib = (SCRIPT_LIBRIARY *)pClass->CreateClass();
			if(pLib) pLib->Init();

			pH = NEW SLIBHOLDER;
			pH->pLib = pLib;
			pH->SetName(Token.GetData());
			LibriaryFuncs.Add(pH);

		break;
		case INCLIDE_FILE:
			if(Segment.Files_list->AddUnicalString(Token.GetData()))
			{
				file_code = Segment.Files_list->GetStringCode(Token.GetData());
				Control_offset = (DWORD)Token.GetProgramControl() - (DWORD)Token.GetProgramBase();	// store program scan point
				pApend_file = LoadFile(Token.GetData(),Append_file_size);
				if(pApend_file == 0) 
				{
					SetError("can't load file: %s", Token.GetData());
					return false;
				}
				else
				{
					if(bDebugInfo)
					{
						DWORD aps;
						strcpy(dbg_file_name,"#file \"");
						strcat(dbg_file_name,Token.GetData());
						strcat(dbg_file_name,"\";");
						aps = strlen(dbg_file_name);
						char * pTempS;
						pTempS = NEW char[aps + 1];
						strcpy(pTempS,dbg_file_name);
						AppendProgram(pProgram,Program_size,pTempS,aps,false);
					}
					AppendProgram(pProgram,Program_size,pApend_file,Append_file_size,true);
					Segment.pData = pProgram;
					Token.SetProgram(pProgram,pProgram + Control_offset);
				}
			}
		break;
		case DEFINE_COMMAND:	// create define
			//DefineTable
			if(inout == 0)
			{
				strcpy(var_name,Token.GetData());
				di.name = var_name;
				di.segment_id = Segment.id;
				Token.Get();
				di.deftype = Token.GetType();
				bool bMinus = false;
				if(Token.GetType() == OP_MINUS) 
				{
					bMinus = true;
					Token.Get();
					di.deftype = Token.GetType();
				}
				switch(Token.GetType())
				{
					case NUMBER:
						lvalue = atol(Token.GetData());	
						if(bMinus) lvalue = - lvalue;
						memcpy(&di.data4b,&lvalue,sizeof(long));
					break;
					case FLOAT_NUMBER:
						fvalue = (float)atof(Token.GetData()); 
						if(bMinus) fvalue = - fvalue;
						memcpy(&di.data4b,&fvalue,sizeof(float));
					break;
					case STRING:
						
						di.data4b = (dword)(NEW char[strlen(Token.GetData()) + 1]);
						strcpy((char *)di.data4b,Token.GetData());
					break;
					case OP_MINUS:

					break;
				}
				def_code = DefTab.AddDef(di);
				if(def_code == INVALID_DEF_CODE)
				{
					SetError("define redefinition: %s",di.name);
					return false;
				}
			}
		break;

		/*case DEFINE_COMMAND:	// create define
			if(inout == 0)
			{
				strcpy(var_name,Token.GetData());
				Token.Get();
				if(Token.GetType() == NUMBER || Token.GetType() == FLOAT_NUMBER || Token.GetType() == STRING)
				{
					vi.type = Token.GetType();
					vi.elements = 1;
					vi.name = var_name;
					vi.segment_id = Segment.id;
					var_code = VarTab.AddVar(vi);
					if(var_code == INVALID_VAR_CODE)
					{
						SetError("Duplicate define name: %s",vi.name);
						return false;
					}
					VarTab.GetVarX(vi,var_code);
					vi.pDClass->Set(Token.GetData());
					vi.pDClass->Convert(Token.GetType());
				}
			}
		break;*/
		case EXTERN:
			Token_type = Token.Get();
			bExtern = true;
		case IMPORT:
			if(!bExtern)
			{
				Token_type = Token.Get();
				bImport = true;
			}
		case VAR_AREFERENCE:
		case VAR_REFERENCE:
		case VAR_INTEGER:
		case VAR_FLOAT:
		case VAR_STRING:
		case VAR_OBJECT:
		case TVOID:
			if(inout == 0)
			{
				if(!bFunctionBlock)
				{
					Token.Get();
					if(Token.GetType() == OPEN_BRACKET)
					{
						// this is function header
						Token.StepBack();
						Token.StepBack();
						Token.Get();		// function name
						bFunctionBlock = true;		// we are stepped into func area
						
						strcpy(func_name,Token.GetData());
						
						fi.segment_id = Segment.id;
						if(bExtern) fi.offset = INVALID_FUNC_OFFSET;
						else 
						if(bImport)	fi.offset = INVALID_FUNC_OFFSET;
						else
							fi.offset = Segment.BCode_Program_size;
						fi.name = func_name;
						fi.return_type = Token_type;
						fi.arguments = 0;
						fi.decl_file_name = DebugSourceFileName;
						fi.decl_line = DebugSourceLine;
						fi.pImportedFunc = 0;
						func_code = FuncTab.AddFunc(fi);

						if(func_code == INVALID_FUNC_CODE)
						{
							func_code = FuncTab.FindFunc(fi.name);
							FuncTab.GetFunc(fi,func_code);
							//SetError("Duplicate function name: %s",fi.name);
							SetError("Function [%s] already declared in: %s line %d",fi.name,fi.decl_file_name,fi.decl_line);

							return false;
						}
						break;
					} else Token.StepBack();
					
					// global variable
					Token.StepBack();
					do
					{
						Token.Get();
						if(Token.GetData() == 0)
						{
							SetError("Invalid variable name");
							return false;
						}
						strcpy(var_name,Token.GetData());
						vi.name = var_name;
						vi.segment_id = Segment.id;
						vi.elements = 1;
						vi.type = Token_type;
						vi.bArray = false;
						Token.Get();
						if(Token.GetType() == SQUARE_OPEN_BRACKET)
						{
							Token.Get();
							if(Token.GetType() != NUMBER)
							{
								def_code = DefTab.FindDef(Token.GetData());
								if(def_code != INVALID_DEF_CODE)
								{
									
									DefTab.GetDef(di,def_code);
									if(di.deftype == NUMBER)
									{
										lvalue = di.data4b;
									}
									else
									{
										SetError("Invalid array (%s) size", vi.name);
										return false;
									}
								} 
								else
								{
									SetError("Invalid array (%s) size", vi.name);
									return false;
								}
							} else lvalue = atol(Token.GetData());
							vi.elements = lvalue;
							vi.bArray = true;
							Token.Get();	// SQUARE_CLOSE_BRACKET
							Token.Get();
						}
						var_code = VarTab.AddVar(vi);
						if(var_code == INVALID_VAR_CODE)
						{
							SetError("Duplicate variable name: %s",vi.name);
							return false;
						}
						VarTab.GetVarX(vi,var_code);
						bool bNeg;
						if(Token.GetType() == OP_EQUAL)
						{
							// global var initialization
							if(vi.elements > 1)
							{
								// array
								if(SkipAuxiliaryTokens() != BLOCK_IN)
								{
									SetError("Invalid array '%s' initialization",vi.name);
									return false;
								}
								DWORD aindex;
								//bool bNeg;
								aindex = 0;
								
								SkipAuxiliaryTokens();
								while(Token.GetType()!=END_OF_PROGRAMM)
								{
									if(Token.GetType() == OP_MINUS)
									{
										bNeg = true;
										Token.Get();
									} else bNeg = false;
									if(Token.GetType() == BLOCK_OUT) break;
									if(Token.GetType() == END_OF_PROGRAMM) break;
									
									switch(Token.GetType())
									{
										case TRUE_CONST:
											if(vi.type != VAR_INTEGER) 
											{
												SetError("Invalid array '%s' initialization parameter",vi.name);
												return false;
											}
											vi.pDClass->Set((long)1);
										break;
										case FALSE_CONST:
											if(vi.type != VAR_INTEGER) 
											{
												SetError("Invalid array '%s' initialization parameter",vi.name);
												return false;
											}
											vi.pDClass->Set((long)0);
										break;

										case NUMBER:
											if(vi.type != VAR_INTEGER) 
											{
												SetError("Invalid array '%s' initialization parameter",vi.name);
												return false;
											}
											if(bNeg)
											vi.pDClass->Set(-atol(Token.GetData()),aindex);
												else
											vi.pDClass->Set(atol(Token.GetData()),aindex);
											aindex++;
										break;
										case FLOAT_NUMBER:
											if(vi.type != VAR_FLOAT) 
											{
												SetError("Invalid array '%s' initialization parameter",vi.name);
												return false;
											}
											if(bNeg)
											vi.pDClass->Set(-(float)atof(Token.GetData()),aindex);
												else
											vi.pDClass->Set((float)atof(Token.GetData()),aindex);
											aindex++;
										break;
										case STRING:
											if(vi.type != VAR_STRING)
											{
												SetError("Invalid array '%s' initialization parameter",vi.name);
												return false;
											}
											vi.pDClass->Set(Token.GetData(),aindex);
											aindex++;
										break;
										case UNKNOWN:

										break;
										default:
										break;
									}
									SkipAuxiliaryTokens();
									if(Token.GetType() == BLOCK_OUT) break;
									if(Token.GetType() != COMMA) 
									{
										SetError("Invalid array '%s' initialization parameters list",vi.name);
										return false;
									}
									SkipAuxiliaryTokens();
								}
								SkipAuxiliaryTokens();



							}
							else
							{
								// single variable
								if(Token.Get() == OP_MINUS)
								{
									bNeg = true;
									Token.Get();
								} else bNeg = false;

								
								switch(Token.GetType())
								{
									case TRUE_CONST:
										if(vi.type != VAR_INTEGER) break;
										vi.pDClass->Set((long)1);
									break;
									case FALSE_CONST:
										if(vi.type != VAR_INTEGER) break;
										vi.pDClass->Set((long)0);
									break;
									case NUMBER:
										if(vi.type != VAR_INTEGER) break;
										if(bNeg)
										vi.pDClass->Set(-atol(Token.GetData()));
											else
										vi.pDClass->Set(atol(Token.GetData()));
									break;
									case FLOAT_NUMBER:
										if(vi.type != VAR_FLOAT) break;
										if(bNeg)
										vi.pDClass->Set(-(float)atof(Token.GetData()));
											else
										vi.pDClass->Set((float)atof(Token.GetData()));
									break;
									case STRING:
										if(vi.type != VAR_STRING) break;
										vi.pDClass->Set(Token.GetData());
									break;
									case UNKNOWN:

									break;
									default:
									break;
								}
								Token.Get();
							}

						}
					} while (Token.GetType() == COMMA);
					Token.StepBack();
					break;
				} 
				else
				{
					// function argument list
					// compute number of arguments
					//if(bImport || !bExtern)
					{
						lvi.type = Token_type;
						lvi.name = Token.GetData();
						lvi.elements = 1;
						if(bExtern) FuncTab.AddFuncArg(func_code,lvi,true);
							else
						FuncTab.AddFuncArg(func_code,lvi);
					}
				}
			}
			else
			{
				// local variables
				// compute number of local vars
				lvi.type = Token_type;
				Token.StepBack();
				do
				{
					Token.Get();
					if(Token.GetData() == 0)
					{
						SetError("Invalid variable name");
						return false;
					}
					strcpy(var_name,Token.GetData());
					lvi.name = var_name;
					lvi.elements = 1;
					lvi.bArray = false;
					Token.Get();
					if(Token.GetType() == SQUARE_OPEN_BRACKET)
					{
						Token.Get();
						if(Token.GetType() != NUMBER)
						{
							def_code = DefTab.FindDef(Token.GetData());
							if(def_code != INVALID_DEF_CODE)
							{
								
								DefTab.GetDef(di,def_code);
								if(di.deftype == NUMBER)
								{
									lvalue = di.data4b;
								}
								else
								{
									SetError("Invalid array (%s) size", lvi.name);
									return false;
								}
							} 
							else
							{
								SetError("Invalid array (%s) size", lvi.name);
								return false;
							}
						} else lvalue = atol(Token.GetData());
						lvi.elements = lvalue;
						lvi.bArray = true;
						Token.Get();	// SQUARE_CLOSE_BRACKET
						Token.Get();
					}
					if(!FuncTab.AddFuncVar(func_code,lvi))
					{
						SetError("Duplicate variable name: %s",lvi.name);
						return false;
					}

				} while (Token.GetType() == COMMA);
				Token.StepBack();
			}
		break;
		// class declaration -------------------
		case CLASS_DECL:
			if(bDotFlag) break;

			// get class name
			Token.Get();	
			if(Token.GetData() == 0) { SetError("Invalid class name"); break; }
			memset(&ci,0,sizeof(ci));
			
			ci.name = (char *)NEW char[strlen(Token.GetData())+1];
			strcpy(ci.name,Token.GetData());
		
			// start processing class components
			if(SkipAuxiliaryTokens() != BLOCK_IN) {SetError("missed '{'"); return false;}

			while(SkipAuxiliaryTokens() != BLOCK_OUT)
			{
				memset(&cc,0,sizeof(cc));
				Token.StepBack();				// step on last token, for ReGet operation
				Token_type = Token.Get(true);	// get with token string keeped
				
				// get component class ID (class registred only and must be filled with data at the end 
				// of first pass)
				memset(&cci,0,sizeof(cci));
				cci.name = Token.GetData();
				cc.nID = ClassTab.AddClass(cci,true);


				// start processing components with the same class type
				do
				{

					// now get the component name
					if(Token.Get() != UNKNOWN) {SetError("wrong variable or function name"); return false;};
					// note: component name memory will be deleted by ClassTab
					
					cc.name = (char *)NEW char[strlen(Token.GetData())+1];
					strcpy(cc.name,Token.GetData());
					cc.nElements = 1;	// assumed for all except arrays

					// now we determine type of component - variable or function
					switch(Token.Get())
					{
						case SEPARATOR:
							// this is single variable declaration
							// add class component
							cc.nFlags = CCF_VARIABLE;
							ci.nComponentsNum++;
							ci.pComponent = (CLASS_COMPONENT *)RESIZE(ci.pComponent,ci.nComponentsNum * sizeof(CLASS_COMPONENT));
							ci.pComponent[ci.nComponentsNum - 1] = cc;

						break;
						case OPEN_BRACKET:
							// this is function declaration
							// for now just skip 
							while(Token.Get()!=SEPARATOR){};
						break;
						case SQUARE_OPEN_BRACKET:
							// this is array declaration
							switch(Token.Get())	// array dimension
							{
								case NUMBER:
									cc.nElements = atol(Token.GetData());
								break;
								case UNKNOWN:
									if(!DefTab.GetDef(di,DefTab.FindDef(Token.GetData())))
									{
										SetError("invalid array size");
										return false;
									}
									if(di.deftype != NUMBER)
									{
										SetError("invalid array size");
										return false;
									}
									cc.nElements = di.data4b;
								break;
								default:
									SetError("invalid array size");
								return false;
							}
							
							Token.Get();	// square close bracket
							Token.Get();	// next token
							cc.nFlags = CCF_VARARRAY;
							ci.nComponentsNum++;
							ci.pComponent = (CLASS_COMPONENT *)RESIZE(ci.pComponent,ci.nComponentsNum * sizeof(CLASS_COMPONENT));
							ci.pComponent[ci.nComponentsNum - 1] = cc;

						break;
						case COMMA:
							// this is several variables declaration
							// add class component
							cc.nFlags = CCF_VARIABLE;
							ci.nComponentsNum++;
							ci.pComponent = (CLASS_COMPONENT *)RESIZE(ci.pComponent,ci.nComponentsNum * sizeof(CLASS_COMPONENT));
							ci.pComponent[ci.nComponentsNum - 1] = cc;

						break;
						
						default:
							break;
					}
				
				} while(Token.GetType() != SEPARATOR);
			
			} 

			if(SkipAuxiliaryTokens() != SEPARATOR) {SetError("missed ';'"); return false;}
			ClassTab.AddClass(ci);

			if(ci.pComponent) delete ci.pComponent;
			delete ci.name;

		break;


		case BLOCK_IN: inout++; break;
		case BLOCK_OUT: 
			inout--; 
			if(inout == 0) bFunctionBlock = false;	// sign if we are out of function
		break;
		default: break;
		}
	} while(Token_type != END_OF_PROGRAMM);


	if(inout != 0)
	{
		SetError("Missing '}' in segment"); 
		return false;
	}

	//-----------------------------------------



	Current_line = 1;
	//Current_line = DSL_INI_VALUE;
	DebugSourceLine = DSL_INI_VALUE;
	DWORD fnsize;

	if(bDebugInfo)
	{
		strcpy(DebugSourceFileName,Segment.name);
		fnsize = strlen(Segment.name);
		CompileToken(Segment,DEBUG_FILE_NAME,3,(char *)&DebugSourceLine,sizeof(dword),
				(char *)&fnsize,sizeof(dword),
				Segment.name,strlen(Segment.name));
		CompileToken(Segment,DEBUG_LINE_CODE,1,(char *)&DebugSourceLine,sizeof(dword));
		//DebugSourceLine++;
	}

	DWORD block_io_code;
	block_io_code = 0;
	inout = 0;
	bracket_inout = 0;
	bFunctionBlock = false;
	pProgram = Segment.pData;
	Token.SetProgram(pProgram,pProgram);

	
	
	STRINGS_LIST BreakUpdateTable;
	if(!CompileBlock(Segment,bFunctionBlock,inout,TVOID,INVALID_OFFSET,INVALID_OFFSET,BreakUpdateTable))
		return false;
	DOUBLE_DWORD ddw;
	DWORD label_offset;
	for(n=0;n<LabelUpdateTable.GetStringsCount();n++)
	{
		LabelUpdateTable.GetStringData(n,&ddw);
		LabelTable.GetStringData(ddw.dw1,&label_offset);
		memcpy(&Segment.pCode[ddw.dw2],&label_offset,sizeof(dword));
	}


	if(pInternalCode == 0)	
	{
		if(Segment.pData) delete Segment.pData;	Segment.pData = 0;
	}
	HANDLE fh;
	DWORD dwR;
	if(bWriteCodeFile)
	{
		
		_splitpath(Segment.name,0,0,file_name,0);
		strcat(file_name,".b");
		fh = CreateFile(file_name,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		if(fh != INVALID_HANDLE_VALUE)
		{
			WriteFile(fh,Segment.pCode,Segment.BCode_Program_size,&dwR,0);
			CloseHandle(fh);
		}
	}
	return true;
}

S_TOKEN_TYPE COMPILER::SkipAuxiliaryTokens()
{
	S_TOKEN_TYPE Token_type;
	do
	{
		Token_type = Token.Get();
		switch(Token_type)
		{
			case DEBUG_LINEFEED:
			case DEBUG_FILE_NAME:
			case COMMENT:
			case INCLIDE_FILE:
			case WHITESPACE:
			break;
			default: return Token_type;
		}
	} while(Token_type != END_OF_PROGRAMM);
	return Token_type;
}

S_TOKEN_TYPE COMPILER::CompileAuxiliaryTokens(SEGMENT_DESC& Segment)//, bool & bFunctionBlock, DWORD & inout)
{
	DWORD fnsize;
	S_TOKEN_TYPE Token_type;

	do
	{
		Token_type = Token.Get();
		switch(Token_type)
		{
			case DEBUG_LINEFEED:
				if(!bDebugInfo) break;
				DebugSourceLine++;
				CompileToken(Segment,DEBUG_LINE_CODE,1,(char *)&DebugSourceLine,sizeof(dword));
			break;
			case DEBUG_FILE_NAME:
				if(!bDebugInfo) break;
				DebugSourceLine = DSL_INI_VALUE;
				if(Token.GetData() == 0)
				{
					SetError("Invalid DBG_FILE_NAME");
					return DEBUG_FILE_NAME;
				}
				strcpy(DebugSourceFileName,Token.GetData());
				fnsize = strlen(Token.GetData());

				CompileToken(Segment,DEBUG_FILE_NAME,3,(char *)&DebugSourceLine,sizeof(dword),
					(char *)&fnsize,sizeof(dword),
					Token.GetData(),strlen(Token.GetData()));
			break;
			case COMMENT:
				DebugSourceLine += Token.TokenLines();
			break;
			case INCLIDE_FILE:
			case WHITESPACE:
				// skip compiling of auxiliary tokens
			break;
			default: return Token_type;
		}
	} while(Token_type != END_OF_PROGRAMM);
	return Token_type;
}

void COMPILER::CopyOffsets(SEGMENT_DESC& Segment, STRINGS_LIST & srclist, STRINGS_LIST & dstlist, char * sname)
{
	DWORD n;
	DWORD i;
	DWORD jump_voffset;
	if(sname == 0)
	{
		for(n=0;n<srclist.GetStringsCount();n++)
		{
			srclist.GetStringData(n,(char *)&jump_voffset);
			
			i = dstlist.GetStringsCount();
			dstlist.AddString("c");
			dstlist.SetStringData(i,(char *)&jump_voffset);
		}
		return;
	}
	
	for(n=0;n<srclist.GetStringsCount();n++)
	{
		if(strcmp(sname,srclist.GetString(n))!=0) continue;

		srclist.GetStringData(n,(char *)&jump_voffset);
		
		i = dstlist.GetStringsCount();
		dstlist.AddString("c");
		dstlist.SetStringData(i,(char *)&jump_voffset);
	}
}


void COMPILER::UpdateOffsets(SEGMENT_DESC& Segment, STRINGS_LIST & list, DWORD offset, char * sname)
{
	DWORD n;
	DWORD jump_voffset;
	if(sname == 0)
	{
		for(n=0;n<list.GetStringsCount();n++)
		{
			list.GetStringData(n,(char *)&jump_voffset);
			memcpy(&Segment.pCode[jump_voffset],&offset,sizeof(dword));
		}
		return;
	}
	for(n=0;n<list.GetStringsCount();n++)
	{
		if(strcmp(sname,list.GetString(n))!=0) continue;
		list.GetStringData(n,(char *)&jump_voffset);
		memcpy(&Segment.pCode[jump_voffset],&offset,sizeof(dword));
	}

}

bool COMPILER::CompileBlock(SEGMENT_DESC& Segment, bool & bFunctionBlock, DWORD & inout,S_TOKEN_TYPE bound_type, DWORD continue_jump, DWORD break_offset, STRINGS_LIST & BreakTable)
{
	S_TOKEN_TYPE Token_type;
//	S_TOKEN_TYPE Block_bound;
//	DWORD fnsize;
	DWORD awcode;
	DWORD var_code;
	DWORD balance_bracket;
	DWORD balance_block;
	DWORD jump_offset;
	DWORD update_offset;
	DWORD vdword;
	DWORD n;
	DWORD unk_code;
	DWORD func_args;
	STRINGS_LIST BreakUpdateTable;
	BreakUpdateTable.SetStringDataSize(sizeof(DWORD));
	bool bExtern;
	bool bImport;
	long lvalue;
	DWORD def_code;
	DEFINFO di;
	DWORD dwRCode;
//	VARINFO vi;

/*
#ifdef _XBOX
	VDX8RENDER * pDX = (VDX8RENDER *)api->CreateService("dx8render");
	if(pDX) pDX->ProgressView();
#endif
*/
	VARINFO vi;
	LVARINFO lvi;

	jump_offset = 0xffbadbad;

	balance_block = 0;
	balance_bracket = 0;

	switch(bound_type)
	{
		case CLOSE_BRACKET: balance_bracket = 1; break;
		case BLOCK_OUT: balance_block = 1; break;
	}

	bExtern = false;
	bImport = false;
	do
	{
		//Token_type = Token.Get();
		Token_type = CompileAuxiliaryTokens(Segment/*,bFunctionBlock,inout*/);
		

		switch(Token_type)
		{
		case EVENT_HANDLER:
			if(Token.Get()!=OPEN_BRACKET) { SetError("Invalid event handler"); return false;}
			
			if(Token.Get() == STRING)
			{
				if(Token.GetData()==0) { SetError("Invalid event handler"); return false;}
				strcpy(gs,Token.GetData());
			} 
			else
			{
				if(Token.GetData()==0) { SetError("Invalid event handler"); return false;}
				def_code = DefTab.FindDef(Token.GetData());
				if(def_code != INVALID_DEF_CODE)
				{
					DefTab.GetDef(di,def_code);
					if(di.deftype != STRING) { SetError("Invalid event handler"); return false;}
					if(di.data4b == 0) { SetError("Invalid define"); return false;}
						else
					strcpy(gs,(char *)di.data4b);
				}
				else { SetError("Invalid event handler"); return false;}
			}
			if(Token.Get() != COMMA) { SetError("Invalid event handler"); return false;}
			if(Token.Get() == STRING)
			{
				if(Token.GetData()==0) { SetError("Invalid event handler"); return false;}
				//SetEventHandler(gs,Token.GetData(),1,true);
				SetEventHandler(gs,Token.GetData(),0,true);
			} else { SetError("Invalid event handler"); return false;}
			
			if(Token.Get()!=CLOSE_BRACKET) { SetError("Invalid event handler"); return false;}
		break;

		case GOTO_COMMAND:
			//CompileToken(Segment,Token_type,1,Token.GetData(),strlen(Token.GetData())+1);
			//LabelUpdateTable
			Token.Get();	// label name
			DOUBLE_DWORD ddw;
			ddw.dw1 = LabelTable.GetStringCode(Token.GetData());
			n = LabelUpdateTable.GetStringsCount();
			LabelUpdateTable.AddString("j");
			ddw.dw2 = Segment.BCode_Program_size + 2; 
			LabelUpdateTable.SetStringData(n,(char *)&ddw);
			vdword = 0xffffffff;
			CompileToken(Segment,JUMP,1,(char *)&vdword,sizeof(dword));
		break;
		case BREAK_COMMAND:
			if(break_offset == INVALID_OFFSET) {SetError("invalid break"); return false;}
			vdword = Segment.BCode_Program_size + 2;
			n = BreakTable.GetStringsCount();
			BreakTable.AddString("b");
			BreakTable.SetStringData(n,(char *)&vdword);
			vdword = INVALID_OFFSET;
			CompileToken(Segment,JUMP,1,(char *)&vdword,sizeof(dword));
			if(bound_type == BREAK_COMMAND)
			{
				return true;
			}
		break;
		case CONTINUE_COMMAND:
			if(continue_jump == INVALID_OFFSET) {SetError("invalid continue"); return false;}
			vdword = Segment.BCode_Program_size + 2;
			n = BreakTable.GetStringsCount();
			BreakTable.AddString("c");
			BreakTable.SetStringData(n,(char *)&vdword);
			vdword = INVALID_OFFSET;
			CompileToken(Segment,JUMP,1,(char *)&vdword,sizeof(dword));
			//CompileToken(Segment,JUMP,1,(char *)&continue_jump,sizeof(dword));
		break;
		case SEPARATOR:
			CompileToken(Segment,Token_type);
			if(bound_type == SEPARATOR)
			{
				return true;
			}
		break;
		case OPEN_BRACKET:
			balance_bracket++;
			CompileToken(Segment,Token_type);
		break;
		case CLOSE_BRACKET:
			balance_bracket--;
			CompileToken(Segment,Token_type);
			if(bound_type == CLOSE_BRACKET)	
			{ 
				if(balance_bracket == 0) 
				{
					return true;	
				}
			}
		break;
		case BLOCK_IN: 
			inout++; 
			balance_block++;
		break;
		case BLOCK_OUT: 
			
			
			if(inout == 0)
			{
				SetError("syntax error");
				return false;
			}
			inout--; 

			if(balance_block == 0)
			{
				switch(bound_type)
				{
					case CLOSE_BRACKET: SetError("missed ')'"); break;
					case BLOCK_OUT: SetError("missed '}'"); break;
					default: SetError("syntax error"); break;
				}
				return false;
			}
			balance_block--;
			if(inout == 0) 
			{
				CompileToken(Segment,FUNCTION_RETURN_VOID);
				bFunctionBlock = false;	// sign if we are out of function
				break;
			}
			if(bound_type == BLOCK_OUT)	
			{ 
				if(balance_block == 0)
				{
					return true;
				}
			}
		break;
/*
		case IF_BLOCK:
			CompileToken(Segment,Token_type);
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type != OPEN_BRACKET){SetError("missed '('"); return false;}
			CompileToken(Segment,OPEN_BRACKET);
			if(!CompileBlock(Segment,bFunctionBlock,inout,CLOSE_BRACKET,INVALID_OFFSET,INVALID_OFFSET,BreakTable)) return false;
			CompileToken(Segment,PUSH_EXPRESULT);	// store for possible else

			update_offset = Segment.BCode_Program_size + 2;
			CompileToken(Segment,JUMP_Z,1,(char *)&jump_offset,sizeof(dword));
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type == BLOCK_IN)
			{
				inout++;
				if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,continue_jump,break_offset,BreakTable)) return false;
			} 
			else
			{
				Token.StepBack();
				if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,continue_jump,break_offset,BreakTable)) return false;
			}
			memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(dword));
			CompileToken(Segment,POP_EXPRESULT);	// pop 
		break;
		case ELSE_BLOCK:
			update_offset = Segment.BCode_Program_size + 2;
			CompileToken(Segment,JUMP_NZ,1,(char *)&jump_offset,sizeof(dword));
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type == BLOCK_IN)
			{
				inout++;
				if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,continue_jump,break_offset,BreakTable)) return false;
			} 
			else
			{
				Token.StepBack();
				if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,continue_jump,break_offset,BreakTable)) return false;
			}
			memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(dword));
		break;
		case WHILE_BLOCK:
			BreakUpdateTable.Release();
			jump_offset = Segment.BCode_Program_size;
			CompileToken(Segment,Token_type);
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type != OPEN_BRACKET){SetError("missed '('"); return false;}
			CompileToken(Segment,OPEN_BRACKET);
			if(!CompileBlock(Segment,bFunctionBlock,inout,CLOSE_BRACKET,INVALID_OFFSET,INVALID_OFFSET,BreakUpdateTable)) return false;
			
			update_offset = Segment.BCode_Program_size + 2;
			CompileToken(Segment,JUMP_Z,1,(char *)&jump_offset,sizeof(dword));
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type == BLOCK_IN)
			{
				inout++;
				if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,jump_offset,6,BreakUpdateTable)) return false;
			} 
			else { SetError("missed '{'");	return false; }
			CompileToken(Segment,JUMP,1,(char *)&jump_offset,sizeof(dword));
			memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(dword));
			UpdateOffsets(Segment,BreakUpdateTable,Segment.BCode_Program_size,"b");
			UpdateOffsets(Segment,BreakUpdateTable,jump_offset,"c");
			BreakUpdateTable.Release();
		break;
		case FOR_BLOCK:
			BreakUpdateTable.Release();
			
			// '('
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type != OPEN_BRACKET){SetError("missed '('"); return false;}
			
			// var = initial_value
			if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,INVALID_OFFSET,INVALID_OFFSET,BreakTable)) return false;
			
			jump_offset = Segment.BCode_Program_size;
			// condition
			CompileToken(Segment,PROCESS_EXPRESSION);
			if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,INVALID_OFFSET,INVALID_OFFSET,BreakTable)) return false;
			// jump if zero
			update_offset = Segment.BCode_Program_size + 2;
			CompileToken(Segment,JUMP_Z,1,(char *)&jump_offset,sizeof(dword));

			
			S_TOKEN_TYPE sttResult;	// var for inc/dec op
			S_TOKEN_TYPE forOp;
			if(Token.Get()!=UNKNOWN) {SetError("invalid 'for' syntax");return false;};

			sttResult = DetectUnknown(var_code);
			if(!(sttResult == VARIABLE || sttResult == LOCAL_VARIABLE)) {SetError("invalid 'for' syntax");return false;};

			forOp = Token.Get();
			if(!(forOp == OP_INC || forOp == OP_DEC)) {SetError("invalid 'for' syntax");return false;};
			if(CompileAuxiliaryTokens(Segment) != CLOSE_BRACKET) {SetError("invalid 'for' syntax");return false;};
			
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type == BLOCK_IN)
			{
				inout++;
				if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,jump_offset,6,BreakUpdateTable)) return false;
			} 
			else { SetError("missed '{'");	return false; }

			DWORD forcont_offset;
			forcont_offset = Segment.BCode_Program_size;
			CompileToken(Segment,sttResult,1,(char *)&var_code,sizeof(dword));
			CompileToken(Segment,forOp);
			CompileToken(Segment,JUMP,1,(char *)&jump_offset,sizeof(dword));
			memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(dword));
			UpdateOffsets(Segment,BreakUpdateTable,Segment.BCode_Program_size,"b");
			UpdateOffsets(Segment,BreakUpdateTable,forcont_offset,"c");
			BreakUpdateTable.Release();
		break;
		case SWITCH_COMMAND:
			BreakUpdateTable.Release();
			CompileToken(Segment,Token_type);
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type != OPEN_BRACKET){SetError("missed '('"); return false;}
			CompileToken(Segment,OPEN_BRACKET);
			if(!CompileBlock(Segment,bFunctionBlock,inout,CLOSE_BRACKET,INVALID_OFFSET,INVALID_OFFSET,BreakUpdateTable)) return false;
			CompileToken(Segment,PUSH_EXPRESULT);
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type == BLOCK_IN)
			{
				inout++;
				//if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,jump_offset,6,BreakUpdateTable)) return false;
				if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,continue_jump,6,BreakUpdateTable)) return false;
			} 
			else { SetError("missed '{'");	return false; }
			UpdateOffsets(Segment,BreakUpdateTable,Segment.BCode_Program_size,"b");
			CopyOffsets(Segment,BreakUpdateTable,BreakTable,"c");
			//UpdateOffsets(Segment,BreakUpdateTable,continue_jump,"c");	// -> ***
			CompileToken(Segment,POP_VOID);
			BreakUpdateTable.Release();
		break;
		case CASE_COMMAND:
			//CompileToken(Segment,Token_type);
			Token_type = CompileAuxiliaryTokens(Segment);
			CompileToken(Segment,PROCESS_EXPRESSION);
			switch(Token_type)
			{
				case UNKNOWN:	CompileUnknown(Segment); break;
				case NUMBER:	CompileNumber(Segment); break;
				case FLOAT_NUMBER: CompileFloatNumber(Segment); break;
				case STRING:	CompileString(Segment); break;
				break;
				default: SetError("invalid case"); return false;
			}
			if(Token.Get() != LABEL){ SetError("missed ':'");	return false; }
			CompileToken(Segment,SEPARATOR);
			CompileToken(Segment,STACK_COMPARE);

			update_offset = Segment.BCode_Program_size + 2;
			CompileToken(Segment,JUMP_Z,1,(char *)&jump_offset,sizeof(dword));

			//if(!CompileBlock(Segment,bFunctionBlock,inout,BREAK_COMMAND,INVALID_OFFSET,0,BreakTable)) return false;
			// ??? why INVALID_OFFSET on continue_jump here? dont remember; anyway now change to normal
			if(!CompileBlock(Segment,bFunctionBlock,inout,BREAK_COMMAND,continue_jump,0,BreakTable)) return false;

			memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(dword));
		break;

*/
		case IF_BLOCK:
			
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type != OPEN_BRACKET){SetError("missed '('"); return false;}

			CompileExpression(Segment);
			
			if(Token.GetType() != CLOSE_BRACKET) {SetError("missed ')'"); return false;}

			CompileToken(Segment,STACK_READ);
			CompileToken(Segment,EX);

			update_offset = Segment.BCode_Program_size + 2;
			CompileToken(Segment,JUMP_Z,1,(char *)&jump_offset,sizeof(dword));
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type == BLOCK_IN)
			{
				inout++;
				if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,continue_jump,break_offset,BreakTable)) return false;
			} 
			else
			{
				Token.StepBack();
				if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,continue_jump,break_offset,BreakTable)) return false;
			}
			memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(dword));
			//CompileToken(Segment,STACK_POP);
			//CompileToken(Segment,EX);

			CompileToken(Segment,POP_EXPRESULT);	// pop 
		break;
		case ELSE_BLOCK:
			update_offset = Segment.BCode_Program_size + 2;
			CompileToken(Segment,JUMP_NZ,1,(char *)&jump_offset,sizeof(dword));
			Token_type = CompileAuxiliaryTokens(Segment/*,bFunctionBlock,inout*/);
			if(Token_type == BLOCK_IN)
			{
				inout++;
				if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,continue_jump,break_offset,BreakTable)) return false;
			} 
			else
			{
				Token.StepBack();
				if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,continue_jump,break_offset,BreakTable)) return false;
			}
			memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(dword));
		break;


		case WHILE_BLOCK:
			BreakUpdateTable.Release();
			jump_offset = Segment.BCode_Program_size;
			
			//CompileToken(Segment,Token_type);
			
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type != OPEN_BRACKET){SetError("missed '('"); return false;}
			//CompileToken(Segment,OPEN_BRACKET);
			CompileExpression(Segment);
			CompileToken(Segment,STACK_POP);
			CompileToken(Segment,EX);

			if(Token.GetType() != CLOSE_BRACKET){SetError("missed ')'"); return false;}
			
			//if(!CompileBlock(Segment,bFunctionBlock,inout,CLOSE_BRACKET,INVALID_OFFSET,INVALID_OFFSET,BreakUpdateTable)) return false;
			
			update_offset = Segment.BCode_Program_size + 2;
			CompileToken(Segment,JUMP_Z,1,(char *)&jump_offset,sizeof(dword));
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type == BLOCK_IN)
			{
				inout++;
				if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,jump_offset,6,BreakUpdateTable)) return false;
			} 
			else 
			{ 
				//SetError("missed '{'");	return false; 
				Token.StepBack();
				if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,jump_offset,6,BreakUpdateTable)) return false;
				// **** ?
			}
			CompileToken(Segment,JUMP,1,(char *)&jump_offset,sizeof(dword));
			memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(dword));
			UpdateOffsets(Segment,BreakUpdateTable,Segment.BCode_Program_size,"b");
			UpdateOffsets(Segment,BreakUpdateTable,jump_offset,"c");
			BreakUpdateTable.Release();
		break;
		case FOR_BLOCK:
			BreakUpdateTable.Release();
			
			// '('
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type != OPEN_BRACKET){SetError("missed '('"); return false;}
			
			// var = initial_value
			if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,INVALID_OFFSET,INVALID_OFFSET,BreakTable)) return false;
						
			jump_offset = Segment.BCode_Program_size;

			// condition
			//CompileToken(Segment,PROCESS_EXPRESSION);
			//if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,INVALID_OFFSET,INVALID_OFFSET,BreakTable)) return false;
			CompileExpression(Segment);
			CompileToken(Segment,STACK_POP);
			CompileToken(Segment,EX);
			
			// jump if zero
			update_offset = Segment.BCode_Program_size + 2;
			CompileToken(Segment,JUMP_Z,1,(char *)&jump_offset,sizeof(dword));

			
			S_TOKEN_TYPE sttResult;	// var for inc/dec op
			S_TOKEN_TYPE forOp;
			if(Token.Get()!=UNKNOWN) {SetError("invalid 'for' syntax");return false;};

			sttResult = DetectUnknown(var_code);
			if(!(sttResult == VARIABLE || sttResult == LOCAL_VARIABLE)) {SetError("invalid 'for' syntax");return false;};

			forOp = Token.Get();
			if(!(forOp == OP_INC || forOp == OP_DEC)) {SetError("invalid 'for' syntax");return false;};
			if(CompileAuxiliaryTokens(Segment) != CLOSE_BRACKET) {SetError("invalid 'for' syntax");return false;};
			
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type == BLOCK_IN)
			{
				inout++;
				if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,jump_offset,6,BreakUpdateTable)) return false;
			} 
			else 
			{ 
				Token.StepBack();
				if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,jump_offset,6,BreakUpdateTable)) return false;
				//SetError("missed '{'");	return false; 
				// **** ?
			}

			DWORD forcont_offset;
			forcont_offset = Segment.BCode_Program_size;
			CompileToken(Segment,sttResult,1,(char *)&var_code,sizeof(dword));
			CompileToken(Segment,forOp);
			CompileToken(Segment,JUMP,1,(char *)&jump_offset,sizeof(dword));
			memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(dword));
			UpdateOffsets(Segment,BreakUpdateTable,Segment.BCode_Program_size,"b");
			UpdateOffsets(Segment,BreakUpdateTable,forcont_offset,"c");
			BreakUpdateTable.Release();
		break;
		case SWITCH_COMMAND:
			BreakUpdateTable.Release();
			//CompileToken(Segment,Token_type);
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type != OPEN_BRACKET){SetError("missed '('"); return false;}
			
			//CompileToken(Segment,OPEN_BRACKET);

			//if(!CompileBlock(Segment,bFunctionBlock,inout,CLOSE_BRACKET,INVALID_OFFSET,INVALID_OFFSET,BreakUpdateTable)) return false;
			//CompileToken(Segment,PUSH_EXPRESULT);
			CompileExpression(Segment);

			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type == BLOCK_IN)
			{
				inout++;
				//if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,jump_offset,6,BreakUpdateTable)) return false;
				if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,continue_jump,6,BreakUpdateTable)) return false;
			} 
			else { SetError("missed '{'");	return false; }
			UpdateOffsets(Segment,BreakUpdateTable,Segment.BCode_Program_size,"b");
			CopyOffsets(Segment,BreakUpdateTable,BreakTable,"c");
			//UpdateOffsets(Segment,BreakUpdateTable,continue_jump,"c");	// -> ***
			CompileToken(Segment,POP_VOID);
			BreakUpdateTable.Release();
		break;
		case CASE_COMMAND:
			//CompileToken(Segment,Token_type);
			//Token_type = CompileAuxiliaryTokens(Segment);
			CompileExpression(Segment);
			CompileToken(Segment,STACK_POP);
			CompileToken(Segment,EX);

			/*CompileToken(Segment,PROCESS_EXPRESSION);
			switch(Token_type)
			{
				case UNKNOWN:	CompileUnknown(Segment); break;
				case NUMBER:	CompileNumber(Segment); break;
				case FLOAT_NUMBER: CompileFloatNumber(Segment); break;
				case STRING:	CompileString(Segment); break;
				break;
				default: SetError("invalid case"); return false;
			}

			if(Token.Get() != LABEL){ SetError("missed ':'");	return false; }*/

			if(Token.GetType() != LABEL){ SetError("missed ':'");	return false; }

			//CompileToken(Segment,SEPARATOR);
			CompileToken(Segment,STACK_COMPARE);

			update_offset = Segment.BCode_Program_size + 2;
			CompileToken(Segment,JUMP_Z,1,(char *)&jump_offset,sizeof(dword));

			//if(!CompileBlock(Segment,bFunctionBlock,inout,BREAK_COMMAND,INVALID_OFFSET,0,BreakTable)) return false;
			// ??? why INVALID_OFFSET on continue_jump here? dont remember; anyway now change to normal
			if(!CompileBlock(Segment,bFunctionBlock,inout,BREAK_COMMAND,continue_jump,0,BreakTable)) return false;

			memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(dword));
		break;
		case INVALID_TOKEN:
			SetError("bad token");
			return false;
		break;
		
		/*case CALL:
			CompileToken(Segment,Token_type);
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type != UNKNOWN)
			{
				SetError("invalid string call syntax");
				return false;
			}
			Token_type = DetectUnknown(unk_code);
			if(!(Token_type == VARIABLE || Token_type == LOCAL_VARIABLE))
			{
				SetError("invalid string call syntax");
				return false;
			}
			CompileToken(Segment,Token_type,1,(char *)&unk_code,sizeof(dword));
		break;*/

		case CALL:
			
			S_TOKEN_TYPE sttVar;
			DWORD dwCallVarCode;


			//CompileToken(Segment,Token_type);
			Token_type = CompileAuxiliaryTokens(Segment);
			if(Token_type != UNKNOWN)
			{
				SetError("invalid dynamic call");
				return false;
			}
			sttVar = DetectUnknown(dwCallVarCode);
			if(!(sttVar == VARIABLE || sttVar == LOCAL_VARIABLE))
			{
				SetError("invalid dynamic call");
				return false;
			}
			//======================================================

			if(CompileAuxiliaryTokens(Segment)!= OPEN_BRACKET) {SetError("missing '('"); return false;}
			if(Token.Get() == CLOSE_BRACKET)
			{
				//function w/o arguments
				func_args = 0;
			}
			else
			{
				// function with arguments
				bool bNext;
				bNext = true;
				func_args = 1;
				Token.StepBack();
				do
				{
					CompileExpression(Segment);
					switch(Token.GetType())
					{
						case CLOSE_BRACKET:
							bNext = false;
						break;
						case COMMA:
							func_args++;
						break;
						default:
							SetError("invalid syntax");
						return false;
					}
				} while(bNext);
			}
			//CompileToken(Segment,CALL_FUNCTION,1,(char *)&func_code,sizeof(dword));
			//CompileToken(Segment,ARGS_NUM,1,(char *)&func_args,sizeof(DWORD));
			//if(fi.return_type!=TVOID) CompileToken(Segment,STACK_POP_VOID);
			//======================================================

			
			CompileToken(Segment,CALL);
			CompileToken(Segment,Token_type,1,(char *)&dwCallVarCode,sizeof(DWORD));
			CompileToken(Segment,ARGS_NUM,1,(char *)&func_args,sizeof(DWORD));
			CompileToken(Segment,POP_NZ);	// pop return value if function returned value

		break;

		case UNKNOWN:
			DWORD func_code;
			DWORD func_args;
			FUNCINFO fi;
		


			if(DetectUnknown(func_code) == CALL_FUNCTION)
			{
				if(CompileAuxiliaryTokens(Segment)!= OPEN_BRACKET) {SetError("missing '('"); return false;}
				if(Token.Get() == CLOSE_BRACKET)
				{
					//function w/o arguments
					func_args = 0;
				}
				else
				{
					// function with arguments
					bool bNext;
					bNext = true;
					func_args = 1;
					Token.StepBack();
					do
					{
						CompileExpression(Segment);
						switch(Token.GetType())
						{
							case CLOSE_BRACKET:
								bNext = false;
							break;
							case COMMA:
								func_args++;
							break;
							default:
								SetError("invalid syntax");
							return false;
						}
					} while(bNext);
				}

				if(!FuncTab.GetFuncX(fi,func_code)){SetError("function not found"); return false;}

				if(fi.offset == INVALID_FUNC_OFFSET)
				{
					// external function declared but not compiled yet
					fi.arguments = fi.ext_args;
				}

				// off for debug needs
				
				if(fi.arguments != func_args)
				{
					switch(fi.segment_id)
					{
						case INTERNAL_SEGMENT_ID:
							if(!IsIntFuncVarArgsNum(func_code))
							{
								SetError("function '%s(args:%d)' doesnt accept %d arguments",fi.name,fi.arguments,func_args);
								return false;
							}
						break;
						case IMPORTED_SEGMENT_ID:
							// skip imported funcs checking for now
						break;
						default:
							SetError("function %s(args:%d) doesnt accept %d arguments",fi.name,fi.arguments,func_args); 
						return false;
					
					}



					/*if(fi.segment_id != INTERNAL_SEGMENT_ID)
					{
						// not internal function cant accept different arguments number
						SetError("function %s(args:%d) doesnt accept %d arguments",fi.name,fi.arguments,func_args); 
						return false;
					}
					else
					{
						if(!IsIntFuncVarArgsNum(func_code))
						{
							SetError("function '%s(args:%d)' doesnt accept %d arguments",fi.name,fi.arguments,func_args);
							return false;
						}

					}*/

				}//*/

				CompileToken(Segment,CALL_FUNCTION,1,(char *)&func_code,sizeof(dword));
				CompileToken(Segment,ARGS_NUM,1,(char *)&func_args,sizeof(DWORD));
				if(fi.return_type!=TVOID) CompileToken(Segment,STACK_POP_VOID);
				
				
			}
			else
			if(!CompileUnknown(Segment)) return false;
		break;
		
		case EXTERN:
				Token_type = Token.Get();
				bExtern = true;
		case IMPORT:
			if(!bExtern)
			{
				Token_type = Token.Get();
				bImport = true;
			}
		case VAR_REFERENCE:
		case VAR_AREFERENCE:
		case VAR_INTEGER:	// create global variable
		case VAR_FLOAT:
		case VAR_STRING:
		case VAR_OBJECT:
		case TVOID:
			//DWORD unk_code;
			Token_type = DetectUnknown(unk_code);
			if(Token_type == CALL_FUNCTION)
			{
				// function declaration

				bFunctionBlock = true;
				
				CurrentFuncCode = FuncTab.FindFunc(Token.GetData());
				if(!bExtern)
				if(!bImport)
				if(!FuncTab.SetFuncOffset(Token.GetData(),Segment.BCode_Program_size))
				{
					SetError("Invalid function name: %s",Token.GetData());
					return false;
				}
				bExtern = false;
				bImport = false;
				// skip function arguments list in function declaration
				if(Token.Get() != OPEN_BRACKET) {SetError("Invalid syntax"); return false;};
				
				do
				{
					Token_type = Token.Get();
					switch(Token_type)
					{
						case VAR_INTEGER:
						case VAR_STRING:
						case VAR_FLOAT:
						case VAR_OBJECT:
						case VAR_REFERENCE:
						case VAR_AREFERENCE:
						case UNKNOWN:
						case COMMA:
						case CLOSE_BRACKET:
						case DEBUG_LINEFEED:
						break;
						default: SetError("Invalid function declaration"); return false;
					}
				} while(Token_type != CLOSE_BRACKET);
			}
			else
			{
				if(Token_type == LOCAL_VARIABLE)
				{
					if(Token.Get() == OP_EQUAL)
					{
						// initialization
						CompileToken(Segment,Token_type,1,(char *)&unk_code,sizeof(dword));
					}
					Token.StepBack();
				}
			}
			// ... skip var or func declaration
		break;


		case FUNCTION_RETURN:
		case FUNCTION_RETURN_VOID:
			Token_type = Token.Get();
			if(Token_type == SEPARATOR) CompileToken(Segment,FUNCTION_RETURN_VOID);
			else 
			{
				Token.StepBack();
				CompileExpression(Segment);	// this will cause function result be pushed into stack
				CompileToken(Segment,STACK_POP);
				CompileToken(Segment,EX);
				CompileToken(Segment,FUNCTION_RETURN);
			}
			Token.StepBack();	// ???
			
		break;

		//case MAKEREF_COMMAND: CompileToken(Segment,Token_type); break;
		//case MAKEAREF_COMMAND: CompileToken(Segment,Token_type); break;

		
		case OP_BOOL_NEG:
		//case OP_EQUAL:
		case OP_BOOL_EQUAL:
		case OP_GREATER:
		case OP_GREATER_OR_EQUAL:
		case OP_LESSER:
		case OP_LESSER_OR_EQUAL:
		case OP_NOT_EQUAL:
		case OP_MINUS:
		case OP_PLUS:
		case OP_MULTIPLY:
		case OP_DIVIDE:
		case OP_POWER:
		case OP_MODUL:
		case OP_INC:
		case OP_DEC:
/*		case OP_INCADD:
		case OP_DECADD:
		case OP_MULTIPLYEQ:
		case OP_DIVIDEEQ://*/
		case OP_BOOL_AND:
		case OP_BOOL_OR:
		//case SQUARE_OPEN_BRACKET:
		//case SQUARE_CLOSE_BRACKET:
		case COMMA:
		case AND:
		case END_OF_PROGRAMM:
			CompileToken(Segment,Token_type);
		break;


//=============================================================================

		case OP_INCADD:
		case OP_DECADD:
		case OP_MULTIPLYEQ:
		case OP_DIVIDEEQ:
			//CompileToken(Segment,Token_type);
			CompileExpression(Segment);
			CompileToken(Segment,Token_type);
			//CompileToken(Segment,LEFT_OPERAND);
			//CompileToken(Segment,STACK_TOP);
			CompileToken(Segment,STACK_POP_VOID);
			if(Token.GetType() == bound_type)
			{
				Token.StepBack();
			}

		break;
//*/
		
		case OP_EQUAL:
			if(inout == 0 && !bDebugExpressionRun) break; // skip global vars initialization
			//CompileToken(Segment,Token_type);
			CompileExpression(Segment);

			CompileToken(Segment,MOVE);
			CompileToken(Segment,LEFT_OPERAND);
			CompileToken(Segment,STACK_TOP);
			CompileToken(Segment,STACK_POP_VOID);
			Token.StepBack();
		

		break;

//		case OP_EQUAL:
//			CompileToken(Segment,Token_type);
//		break;


		case SQUARE_OPEN_BRACKET:
			if(!CompileExpression(Segment))
			{
				return false;
			}
			CompileToken(Segment,STACK_POP);
			CompileToken(Segment,LEFTOP_INDEX);
			
		break;
		case SQUARE_CLOSE_BRACKET:
			SetError("']' without matching '['");
		break;

		//case MAKEREF_COMMAND: CompileToken(Segment,Token_type); break;
		case MAKEREF_COMMAND: 
			DWORD dwRCode1,dwRCode2;
			S_TOKEN_TYPE sttVarType1,sttVarType2;

			//CompileToken(Segment,Token_type); 
			if(Token.Get() != OPEN_BRACKET) {SetError("missed '('"); return false;};

			if(Token.Get() != UNKNOWN) {SetError("invalid 1st operand"); return false;};

			sttVarType1 = DetectUnknown(dwRCode1);

			if(!(sttVarType1 == VARIABLE || sttVarType1 == LOCAL_VARIABLE)) {SetError("invalid 1st operand"); return false;}
			
			/*CompileToken(Segment,STACK_ALLOC);
			CompileToken(Segment,SETREF);
			CompileToken(Segment,STACK_TOP);
			CompileToken(Segment,sttVarType,1,(char *)&dwRCode,sizeof(DWORD));*/
/*			if(sttVarType == VARIABLE)
			{
				if(!VarTab.GetVar(vi,*((dword *)&pCodeBase[ip]))) { SetError("Global variable not found"); break; }
				pRef = vi.pDClass;
			} 
			else
			{
				pRef = SStack.Read(pRun_fi->stack_offset,*((dword *)&pCodeBase[ip]));
				if(pRef == 0) { SetError("Local variable not found"); return false; }
			}
*/	
			
//			if(!pRef->IsReference()) { SetError("makeref to non reference variable"); return false; }
			if(Token.Get() != COMMA) {SetError("missed ','"); return false;};

			if(Token.Get() != UNKNOWN) {SetError("invalid 1st operand"); return false;};

			sttVarType2 = DetectUnknown(dwRCode2);

			if(!(sttVarType2 == VARIABLE || sttVarType2 == LOCAL_VARIABLE)) {SetError("invalid 2st operand"); return false;}

			if(Token.Get() == SQUARE_OPEN_BRACKET)
			{
				CompileExpression(Segment);

				CompileToken(Segment,STACK_POP);
				CompileToken(Segment,BX);

				CompileToken(Segment,SETREF_BXINDEX);
				CompileToken(Segment,sttVarType1,1,(char *)&dwRCode1,sizeof(DWORD));
				CompileToken(Segment,sttVarType2,1,(char *)&dwRCode2,sizeof(DWORD));
	
				if(Token.GetType()!=SQUARE_CLOSE_BRACKET) { SetError("missed ']'"); return false; }
				if(Token.Get() != CLOSE_BRACKET) {SetError("missed ')'"); return false;};

			}
			else
			{
				if(Token.GetType() != CLOSE_BRACKET) {SetError("missed ')'"); return false;};
				CompileToken(Segment,SETREF);
				CompileToken(Segment,sttVarType1,1,(char *)&dwRCode1,sizeof(DWORD));
				CompileToken(Segment,sttVarType2,1,(char *)&dwRCode2,sizeof(DWORD));
			}



			/*	if(BC_TokenGet()== SQUARE_OPEN_BRACKET)
				{
					if(!BC_ProcessExpression(&ExpressionResult)) { SetError("Invalid array index"); return false; }
					if(!ExpressionResult.Get(array_index)) { SetError("Invalid array index"); return false; }
					if(TokenType()!= SQUARE_CLOSE_BRACKET){ SetError("missed ']'"); return false; }
					BC_TokenGet();
					pVar = (DATA *)pVar->GetArrayElement(array_index);
				}
				pRef->SetReference(pVar->GetVarPointer());
				if(TokenType() != CLOSE_BRACKET) {SetError("missed ')'"); return false;};
*/


		break;

		//case MAKEAREF_COMMAND: CompileToken(Segment,Token_type); break;

		case MAKEAREF_COMMAND: 

			//CompileToken(Segment,Token_type);	// debug
			S_TOKEN_TYPE sttVarType;
			
			if(Token.Get() != OPEN_BRACKET) {SetError("missed '('"); return false;}
			Token_type = Token.Get();
			if(Token_type!=UNKNOWN) {SetError("invalid 1st argument"); return false;}
			Token_type = DetectUnknown(dwRCode);
			if(!(Token_type == VARIABLE || Token_type == LOCAL_VARIABLE)) {SetError("invalid 1st argument"); return false;}

			if(Token_type == VARIABLE)
			{
				VarTab.GetVarX(vi,dwRCode);
				if(!(vi.type == VAR_AREFERENCE || vi.type == VAR_REFERENCE)) {SetError("not aref or ref"); return false;}
			}
			else
			{
				FuncTab.GetVar(lvi,CurrentFuncCode,dwRCode);
				if(!(lvi.type == VAR_AREFERENCE || lvi.type == VAR_REFERENCE)) {SetError("not aref or ref"); return false;}

			}

			// set ref to stack top
			CompileToken(Segment,STACK_ALLOC);
			CompileToken(Segment,SETREF);
			CompileToken(Segment,STACK_TOP);
			CompileToken(Segment,Token_type,1,(char *)&dwRCode,sizeof(DWORD));
			
			if(Token.Get() != COMMA) {SetError("missed ','"); return false;}


			sttVarType = Token.Get();
			if(sttVarType!=UNKNOWN) {SetError("invalid 2d argument"); return false;}
			sttVarType = DetectUnknown(dwRCode);
			if(!(sttVarType == VARIABLE || sttVarType == LOCAL_VARIABLE)) {SetError("invalid 2d argument"); return false;}

			


			Token_type = Token.Get();
			if(Token_type == SQUARE_OPEN_BRACKET)
			{
				CompileExpression(Segment);
				if(Token.GetType()!=SQUARE_CLOSE_BRACKET) {SetError("missing ']'"); return false;}
				//....
				CompileToken(Segment,STACK_POP);
				CompileToken(Segment,BX);

				// move ap to AP register
				CompileToken(Segment,MOVEAP_BXINDEX);
				CompileToken(Segment,AP);
				CompileToken(Segment,sttVarType,1,(char *)&dwRCode,sizeof(DWORD));
				Token_type = Token.Get();

				CompileToken(Segment,PUSH_OBJID_BXINDEX);
				CompileToken(Segment,sttVarType,1,(char *)&dwRCode,sizeof(DWORD));
				
			}
			else
			{
				// bove atributes root to AP
				CompileToken(Segment,MOVEAP);
				CompileToken(Segment,AP);
				CompileToken(Segment,sttVarType,1,(char *)&dwRCode,sizeof(DWORD));

				CompileToken(Segment,PUSH_OBJID);
				CompileToken(Segment,sttVarType,1,(char *)&dwRCode,sizeof(DWORD));
			}

			
			if(Token_type != DOT)
			{
				// set attribute pointer to referenced var
				CompileToken(Segment,SETAREF);
				CompileToken(Segment,STACK_TOP);
				CompileToken(Segment,AP);
				CompileToken(Segment,STACK_POP_VOID);
				break;
			}

			// advance ap

			sttResult = Token.Get();
			DWORD dwAWCode;
			DWORD dwVarCode;
			do
			{
				if(Token.GetType() == OPEN_BRACKET)
				{
					// access var
					sttResult = Token.Get();
					if(sttResult != UNKNOWN){ SetError("Invalid access var syntax"); return false; }
					sttResult = DetectUnknown(dwVarCode);
					if(!(sttResult == VARIABLE || sttResult == LOCAL_VARIABLE)) { SetError("not variable: %s",Token.GetData()); return false; }
					//CompileToken(Segment,ADVANCE_AP);
					CompileToken(Segment,VERIFY_AP);
					CompileToken(Segment,sttResult,1,&dwVarCode,sizeof(DWORD));
					if(Token.Get()!=CLOSE_BRACKET){ SetError("missing ')'"); return false; };
				}
				else
				{
					// access word
					if(Token.GetData() == 0) { SetError("Invalid access string"); return false;	}
					Token.LowCase();
					dwAWCode = SCodec.Convert(Token.GetData());
					//CompileToken(Segment,ADVANCE_AP);
					CompileToken(Segment,VERIFY_AP);
					CompileToken(Segment,ACCESS_WORD_CODE,1,&dwAWCode,sizeof(DWORD));
				}
				sttResult = Token.Get();
				if(sttResult == DOT) Token.Get();	// get next token (after dot)
			}while(sttResult == DOT);

			CompileToken(Segment,SETAREF);
			CompileToken(Segment,STACK_TOP);
			CompileToken(Segment,AP);
			CompileToken(Segment,STACK_POP_VOID);
		break;
//=============================================================================




		case DOT:
			Token_type= Token.Get();
			if(Token_type == OPEN_BRACKET)
			{
				// access word inside variable
				Token.Get();	// variable name
				// check for valid local variable 
				var_code = FuncTab.FindVar(CurrentFuncCode,Token.GetData());
				if(var_code != INVALID_VAR_CODE)
				{
					Token.Get();
					if(Token.GetType() != CLOSE_BRACKET)
					{
						SetError("no mathcing ')'");
						return false;
					}
					CompileToken(Segment,ACCESS_VAR);
					CompileToken(Segment,LOCAL_VARIABLE,1,(char *)&var_code,sizeof(dword));
					break;
				}

				var_code = VarTab.FindVar(Token.GetData());
				if(var_code != INVALID_VAR_CODE)
				{
					Token.Get();
					if(Token.GetType() != CLOSE_BRACKET)
					{
						SetError("no mathcing ')'");
						return false;
					}
					CompileToken(Segment,ACCESS_VAR);
					CompileToken(Segment,VARIABLE,1,(char *)&var_code,sizeof(dword));
					break;
				}
			
				SetError("Invalid access string");
				return false;
			}
			// else its access word
			if(Token.GetData() == 0)
			{
				SetError("Invalid access string");
				return false;
			}
			//CompileToken(Segment,ACCESS_WORD,1,Token.GetData(),strlen(Token.GetData())+1);

			Token.LowCase();
			awcode = SCodec.Convert(Token.GetData());
			CompileToken(Segment,ACCESS_WORD_CODE,1,&awcode,sizeof(DWORD));
		break;

		case TRUE_CONST:
			lvalue = 1;
			CompileToken(Segment,NUMBER,1,(char *)&lvalue,sizeof(long));
		break;
		case FALSE_CONST:
			lvalue = 0;
			CompileToken(Segment,NUMBER,1,(char *)&lvalue,sizeof(long));
		break;

		// write value
		case NUMBER: CompileNumber(Segment); break;
		case FLOAT_NUMBER: CompileFloatNumber(Segment);	break;
		case STRING: CompileString(Segment); break;
		
		case LABEL:	// register label

		break;
		case DEFINE_COMMAND:	// create define

		break;

		case CLASS_DECL:
			// skip class declaration
			Token.Get();	// class name
			long nBalance;
			if(SkipAuxiliaryTokens() != BLOCK_IN) {SetError("invalid class declaration"); return false;}
			nBalance = 1;
			while(nBalance > 0)
			{
				switch(Token.Get())
				{
					case BLOCK_IN: nBalance++; break;
					case BLOCK_OUT: nBalance--; break;
				}
			}
		break;

		default: 
			
		break;
		}
	} while(Token_type != END_OF_PROGRAMM);

	return true;
}

bool COMPILER::TokenIs(S_TOKEN_TYPE test)
{
	if(TokenLastReadResult == test) return true;
	return false;
}

bool COMPILER::BC_Jump(SEGMENT_DESC& Segment, DWORD offset)
{
	if(offset >= Segment.BCode_Program_size)
	{
		SetError("invalid jump");
		return false;
	}
	InstructionPointer = offset;
	return true;
}

S_TOKEN_TYPE COMPILER::BC_TokenGet(DWORD & ip, DWORD & token_data_size)
{
	// function read token type and data size, advance InstructionPointer to next token
	// set ip to token data
	// set token data size value and return token type
	TokenLastReadResult = (S_TOKEN_TYPE)pRunCodeBase[InstructionPointer];
	//Trace("Token: %s", Token.GetTypeName(TokenLastReadResult));
	InstructionPointer++;
	if((BYTE)pRunCodeBase[InstructionPointer] < 0xff) 
	{ 
		token_data_size = (BYTE)pRunCodeBase[InstructionPointer]; 
		InstructionPointer++; 
	}
	else 
	{
		InstructionPointer++; 
		memcpy(&token_data_size,&pRunCodeBase[InstructionPointer],sizeof(dword)); 
		InstructionPointer += sizeof(dword);
	}
	ip = InstructionPointer;
	TLR_DataOffset = InstructionPointer;
	InstructionPointer += token_data_size;
	return TokenLastReadResult;
}

S_TOKEN_TYPE COMPILER::BC_TokenGet()
{
	// short version of function for calls what doesnt need to work with token data
	DWORD token_data_size;
	TokenLastReadResult = (S_TOKEN_TYPE)pRunCodeBase[InstructionPointer];
	//Trace("Token: %s", Token.GetTypeName(TokenLastReadResult));
	InstructionPointer++;
	if((BYTE)pRunCodeBase[InstructionPointer] < 0xff) 
	{ 
		token_data_size = (BYTE)pRunCodeBase[InstructionPointer]; 
		InstructionPointer++; 
	}
	else 
	{
		InstructionPointer++; 
		memcpy(&token_data_size,&pRunCodeBase[InstructionPointer],sizeof(dword)); 
		InstructionPointer += sizeof(dword);
	}
	TLR_DataOffset = InstructionPointer;
	InstructionPointer += token_data_size;
	return TokenLastReadResult;
}

S_TOKEN_TYPE COMPILER::NextTokenType()
{
	return (S_TOKEN_TYPE)pRunCodeBase[InstructionPointer];
}



bool COMPILER::BC_CallFunction(DWORD func_code, DWORD & ip, DATA * & pVResult)
{
	FUNCINFO call_fi;
	DWORD mem_ip;
	DWORD mem_InstructionPointer;
	FUNCINFO * mem_pfi;
//	DATA * pV;
	char * mem_codebase;
	DWORD arguments;
	DWORD check_sp;

#ifndef _XBOX
	DWORD nDebugEnterMode;
#endif

	CompilerStage = CS_RUNTIME;

	// get func info 
	if(!FuncTab.GetFunc(call_fi,func_code)) { SetError("Invalid function call"); return false; }
	//if(stricmp(call_fi.name,"InitRandomShipsNames")==0)
	//{
		//_asm int 3;
	//}//*/


	
	// number f arguments pushed into stack for this function call
	if(BC_TokenGet()!=ARGS_NUM) {SetError("missing args_num token");return false;};
	arguments = *((long *)&pRunCodeBase[TLR_DataOffset]);


	check_sp = SStack.GetDataNum() - arguments;
/*
// arguments number check done on compilation stage now
// arguments pushed into stack before this function call

	// push function arguments into stack ----------------------------------------------
	if(BC_TokenGet() != OPEN_BRACKET)  { SetError("missed '('"); return false; }
	arguments = 0;
	check_sp = SStack.GetDataNum();
	call_fi.stack_offset = check_sp;
	
	if(NextTokenType()!= CLOSE_BRACKET)
	{
		while(TokenType() != CLOSE_BRACKET)
		{
			pV = SStack.Push();
			BC_ProcessExpression(pV);
			arguments++;
		}
	} else 
	{
		BC_TokenGet();
	}
	if(arguments != call_fi.arguments)
	{
		if(call_fi.segment_id != INTERNAL_SEGMENT_ID)
		{
			SetError("function '%s(args:%d)' doesnt accept %d arguments",call_fi.name,call_fi.arguments,arguments);
			return false;
		}
		else
		{
			if(!IsIntFuncVarArgsNum(func_code))
			{
				SetError("function '%s(args:%d)' doesnt accept %d arguments",call_fi.name,call_fi.arguments,arguments);
				return false;
			}
		}
	}
	//---------------------------------------------------------------------------------------
//*/
	
	// save current pointers values
	mem_InstructionPointer = InstructionPointer;
	mem_ip = ip;
	mem_pfi = pRun_fi;
	mem_codebase = pRunCodeBase;
	
#ifndef _XBOX	
	nDebugEnterMode = CDebug.GetTraceMode();
#endif
	DWORD nTicks;
	if(call_fi.segment_id == INTERNAL_SEGMENT_ID)
	{
		if(bRuntimeLog) FuncTab.AddCall(func_code);
		//BC_CallIntFunction(func_code,pVResult,arguments);
		RDTSC_B(nTicks);
		BC_CallIntFunction(func_code,pVResult,arguments);
		RDTSC_E(nTicks);
		FuncTab.AddTime(func_code,nTicks);
	} 
	else
	if(call_fi.segment_id == IMPORTED_SEGMENT_ID)
	{
		pVResult = 0;
		RDTSC_B(nTicks);
		//BC_CallIntFunction(func_code,pVResult,arguments);
		DWORD nResult;
		nResult = call_fi.pImportedFunc(&SStack);
		if(nResult == IFUNCRESULT_OK)
		{
			if(call_fi.return_type != TVOID)
			{
				pVResult = SStack.Read();
			}
		}
		RDTSC_E(nTicks);
		FuncTab.AddTime(func_code,nTicks);
	}
	else
	{
		

		//BC_Execute(func_code,pVResult);
		RDTSC_B(nTicks);
		BC_Execute(func_code,pVResult);
		RDTSC_E(nTicks);
		FuncTab.AddTime(func_code,nTicks);
	}
#ifndef _XBOX
	if(nDebugEnterMode == TMODE_MAKESTEP)
	{
		CDebug.SetTraceMode(TMODE_MAKESTEP);
	}
#endif




	if(pVResult)
	{
		if(check_sp != (SStack.GetDataNum()-1))
		{
			SetError("function '%s' stack error",call_fi.name);

			pRun_fi = mem_pfi;	
			InstructionPointer = mem_InstructionPointer;
			ip = mem_ip;
			
			RunningSegmentID = pRun_fi->segment_id;
			pRunCodeBase = mem_codebase;
			
			//return false;	// debug!
		}
	}
	else
	{
		if(check_sp != SStack.GetDataNum())
		{
			SetError("function '%s' stack error",call_fi.name);
			pRun_fi = mem_pfi;	
			InstructionPointer = mem_InstructionPointer;
			ip = mem_ip;
			
			RunningSegmentID = pRun_fi->segment_id;
			pRunCodeBase = mem_codebase;
			

			//return false;// debug!
		}
	}
//*/
	// restore
	pRun_fi = mem_pfi;	
	InstructionPointer = mem_InstructionPointer;
	ip = mem_ip;
	if(pRun_fi) RunningSegmentID = pRun_fi->segment_id;
	pRunCodeBase = mem_codebase;
	
	return true;
}

bool COMPILER::BC_Execute(DWORD function_code, DATA * & pVReturnResult, char * pDbgExpSource)
{
	GUARD(BC_Execute)
	DWORD inout;
	DWORD segment_index;
	DWORD token_data_size;
	DWORD ip;
	DWORD n;
	DWORD jump_offset;
	DWORD func_code;
	DWORD nLeftOperandCode;
	DWORD bLeftOperandType;
	long  nLeftOperandIndex;
	S_TOKEN_TYPE Token_type;
	FUNCINFO fi;
	VARINFO vi;
	DATA * pV;
	DATA * pVResult;
//	DATA   ExpressionResult;	// while compile expression not ready, each function have its own register
	char * pCodeBase;
	bool  bExit;
	long  lvalue;
	S_TOKEN_TYPE vtype;
	DATA * pVV;
	DWORD  var_code;
	char * pAccess_string;
//	long array_index;
//	DATA * pRef;
	DATA * pVar;
	ENTITY_ID eid;
//	ATTRIBUTES * pRoot;
	ATTRIBUTES * pLeftOperandAClass;
	bool bDebugWaitForThisFunc;
	DATA * pVDst;
	DATA * pVSrc;
	bool bUseIndex;
	long dwBXIndex;



	CompilerStage = CS_RUNTIME;

	if(bRuntimeLog) FuncTab.AddCall(function_code);

	bDebugWaitForThisFunc = false;

	//ExpressionResult.SetVCompiler(this);
	pVReturnResult = 0;
	pVResult = 0;

	ZeroMemory(&fi,sizeof(fi));

	if(pDbgExpSource == 0)
	{

	//ZeroMemory(&fi,sizeof(fi));
	if(!FuncTab.GetFunc(fi,function_code))
	{
		SetError("Invalid function: %s",fi.name);
		return false;
	}

	if(fi.offset == INVALID_FUNC_OFFSET)
	{
		SetError("Function (%s) isnt loaded",fi.name);
		return false;
	}

	if(fi.segment_id == INTERNAL_SEGMENT_ID)
	{
		SetError("Function (%s) is internal",fi.name);
		return false;
	}

	if(fi.segment_id == IMPORTED_SEGMENT_ID)
	{
		SetError("Function (%s) is imported",fi.name);
		return false;
	}

	segment_index = GetSegmentIndex(fi.segment_id);
	if(segment_index == INVALID_SEGMENT_INDEX)
	{
		SetError("Function (%s) segment not loaded",fi.name);
		return false;
	}
	if(SegmentTable[segment_index].pCode == 0)
	{
		SetError("Segment (%s) not loaded",SegmentTable[segment_index].name);
		return false;
	}
	
	//Trace("-----------------------------------------------------------------");
	//Trace("Execute function: %s",fi.name);

	RunningSegmentID = fi.segment_id;
	fi.stack_offset = SStack.GetDataNum() - fi.arguments;	// set stack offset
	
	// check arguments types
	for(n=0;n<fi.arguments;n++)
	{
		if(fi.pLocal[n].type == VAR_REFERENCE) continue;
		pV = SStack.Read(fi.stack_offset,n);
		if(pV->GetType() != fi.pLocal[n].type)
		{
			pV = pV->GetVarPointer();
			if(!pV)
			{
				SetError("uninitialized function argument");
				return false;
			}

			if(fi.pLocal[n].type == VAR_AREFERENCE && pV->GetType() == VAR_OBJECT) continue;

			if(pV->GetType() != fi.pLocal[n].type)
			{
				
				SetWarning("wrong type of argument %d  %s(%s) <-- [%s]",n,fi.name,Token.GetTypeName(fi.pLocal[n].type),
					Token.GetTypeName(pV->GetType()));
			}

		}
	}
	
	for(n=fi.arguments;n<fi.var_num;n++)
	{
		pV = SStack.Push();
		pV->SetType(fi.pLocal[n].type,fi.pLocal[n].elements);
	}

	pRun_fi	 = &fi;	// set pointer to 'this' function info

	InstructionPointer = fi.offset;

	pCodeBase = SegmentTable[segment_index].pCode;
	pRunCodeBase = pCodeBase;

	}
	else
	{
		InstructionPointer = 0;
		pCodeBase = pDbgExpSource;
		pRunCodeBase = pCodeBase;
	}

	inout = 0;
	bExit = false;
	
	
	nLeftOperandIndex = INVALID_ARRAY_INDEX;
	nLeftOperandCode = INVALID_VAR_CODE;
	bLeftOperandType = 0xffffffff;
	pLeftOperandAClass = 0;

	//S_TOKEN_TYPE oldToken_type;
	do
	{
		dwNumberScriptCommandsExecuted++;
		//oldToken_type = Token_type;
		Token_type = BC_TokenGet(ip,token_data_size);

		switch(Token_type)
		{
			case ARGS_NUM:
				_asm int 3;
			break;
			case STACK_COMPARE:
				pV = SStack.Read();
				ExpressionResult.CompareAndSetResult(pV,OP_BOOL_EQUAL);
			break;
			case POP_EXPRESULT:
				pV = SStack.Pop();
				ExpressionResult.ClearType();
				ExpressionResult.Copy(pV);
			break;
			case PUSH_EXPRESULT:
				pV = SStack.Push();
				pV->Copy(&ExpressionResult);
			break;
			case POP_NZ:
				ExpressionResult.Convert(VAR_INTEGER);
				ExpressionResult.Get(lvalue);
				if(lvalue == 0) break;
				SStack.Pop();
			break;
			case POP_VOID:
				SStack.Pop();
			break;
			case JUMP:
				memcpy(&jump_offset,&pCodeBase[ip],sizeof(dword));
				if(!BC_Jump(SegmentTable[segment_index],jump_offset)) return false;
			break;
			case JUMP_Z:
				memcpy(&jump_offset,&pCodeBase[ip],sizeof(dword));
				ExpressionResult.Convert(VAR_INTEGER);
				ExpressionResult.Get(lvalue);
				if(lvalue) break;
				if(!BC_Jump(SegmentTable[segment_index],jump_offset)) return false;
			break;
			case JUMP_NZ:
				memcpy(&jump_offset,&pCodeBase[ip],sizeof(dword));
				ExpressionResult.Convert(VAR_INTEGER);
				ExpressionResult.Get(lvalue);
				if(lvalue == 0) break;
				if(!BC_Jump(SegmentTable[segment_index],jump_offset)) return false;
			break;
			case LOCAL_VARIABLE:
				pLeftOperandAClass = 0;						// reset attribute
				nLeftOperandIndex = INVALID_ARRAY_INDEX;	// reset index
				nLeftOperandCode = *((long *)&pRunCodeBase[TLR_DataOffset]);
				bLeftOperandType = LOCAL_VARIABLE;
			break;
			case VARIABLE:
				pLeftOperandAClass = 0;						// reset attribute
				nLeftOperandIndex = INVALID_ARRAY_INDEX;	// reset index
				nLeftOperandCode = *((long *)&pRunCodeBase[TLR_DataOffset]);
				bLeftOperandType = VARIABLE;
			break;
			case ACCESS_WORD_CODE:
				if(nLeftOperandCode == INVALID_VAR_CODE) { SetError("no Lvalue"); return false; }

				if(nLeftOperandIndex == INVALID_ARRAY_INDEX)
				{
					if(pLeftOperandAClass == 0)
					{
						
						if(bLeftOperandType == VARIABLE)
						{
							if(!VarTab.GetVar(vi,nLeftOperandCode))	{ SetError("Global variable not found"); return false; }
							pV = vi.pDClass;
						}
						else
						{
							pV = SStack.Read(pRun_fi->stack_offset,nLeftOperandCode);
							if(pV == 0) { SetError("Local variable not found"); return false; }
						}
						pLeftOperandAClass = pV->GetAClass();
					}
					if(pLeftOperandAClass == 0) {SetError("AClass ERROR n1"); return false; }
					pLeftOperandAClass = pLeftOperandAClass->VerifyAttributeClassByCode(*((long *)&pRunCodeBase[TLR_DataOffset]));
					break;
				} 
				else
				{
					if(pLeftOperandAClass == 0)
					{
						
						if(bLeftOperandType == VARIABLE)
						{
							if(!VarTab.GetVar(vi,nLeftOperandCode))	{ SetError("Global variable not found"); return false; }
							pV = vi.pDClass;
						}
						else
						{
							pV = SStack.Read(pRun_fi->stack_offset,nLeftOperandCode);
							if(pV == 0) { SetError("Local variable not found"); return false; }
						}
						pLeftOperandAClass = pV->GetAClass(nLeftOperandIndex);
					}
					if(pLeftOperandAClass == 0) {SetError("AClass ERROR n1"); return false; }
					pLeftOperandAClass = pLeftOperandAClass->VerifyAttributeClassByCode(*((long *)&pRunCodeBase[TLR_DataOffset]));
					break;
				}
			break;
			case ACCESS_WORD:
				if(nLeftOperandCode == INVALID_VAR_CODE) { SetError("no Lvalue"); return false; }
				if(nLeftOperandIndex == INVALID_ARRAY_INDEX)
				{
					if(pLeftOperandAClass == 0)
					{
						
						if(bLeftOperandType == VARIABLE)
						{
							if(!VarTab.GetVar(vi,nLeftOperandCode))	{ SetError("Global variable not found"); return false; }
							pV = vi.pDClass;
						}
						else
						{
							pV = SStack.Read(pRun_fi->stack_offset,nLeftOperandCode);
							if(pV == 0) { SetError("Local variable not found"); return false; }
						}
						pLeftOperandAClass = pV->GetAClass();
					}
					if(pLeftOperandAClass == 0) {SetError("AClass ERROR n1"); return false; }
					pLeftOperandAClass = pLeftOperandAClass->VerifyAttributeClass((char *)&pRunCodeBase[TLR_DataOffset]);
					break;
				} 
				else
				{
					if(pLeftOperandAClass == 0)
					{
						
						if(bLeftOperandType == VARIABLE)
						{
							if(!VarTab.GetVar(vi,nLeftOperandCode))	{ SetError("Global variable not found"); return false; }
							pV = vi.pDClass;
						}
						else
						{
							pV = SStack.Read(pRun_fi->stack_offset,nLeftOperandCode);
							if(pV == 0) { SetError("Local variable not found"); return false; }
						}
						pLeftOperandAClass = pV->GetAClass(nLeftOperandIndex);
					}
					if(pLeftOperandAClass == 0) {SetError("AClass ERROR n1"); return false; }
					pLeftOperandAClass = pLeftOperandAClass->VerifyAttributeClass((char *)&pRunCodeBase[TLR_DataOffset]);
					break;
				}
			break;
			case ACCESS_VAR:
				if(nLeftOperandCode == INVALID_VAR_CODE) { SetError("no Lvalue"); return false; }
				if(nLeftOperandIndex == INVALID_ARRAY_INDEX)
				{
					if(pLeftOperandAClass == 0)
					{
						
						if(bLeftOperandType == VARIABLE)
						{
							if(!VarTab.GetVar(vi,nLeftOperandCode))	{ SetError("Global variable not found"); return false; }
							pV = vi.pDClass;
						}
						else
						{
							pV = SStack.Read(pRun_fi->stack_offset,nLeftOperandCode);
							if(pV == 0) { SetError("Local variable not found"); return false; }
						}
						pLeftOperandAClass = pV->GetAClass();
					}
					if(pLeftOperandAClass == 0) {SetError("AClass ERROR n1"); return false; }

					vtype = BC_TokenGet();
					var_code = *((long *)&pRunCodeBase[TLR_DataOffset]);
					if(!(vtype == VARIABLE || vtype == LOCAL_VARIABLE)){SetError("invalid access var");return false;}
					if(vtype == VARIABLE)
					{
						if(!VarTab.GetVar(vi,var_code))	{ SetError("Global variable not found"); return false; }
						pVV = vi.pDClass;
					}
					else
					{
						pVV = SStack.Read(pRun_fi->stack_offset,var_code);
						if(pVV == 0) { SetError("Local variable not found"); return false; }
					}
					pVV->Get(pAccess_string);

					//pLeftOperandAClass = pLeftOperandAClass->VerifyAttributeClass(pAccess_string);
					pLeftOperandAClass = pLeftOperandAClass->CreateSubAClass(pLeftOperandAClass,pAccess_string);
					break;
				} 
				else
				{
					if(pLeftOperandAClass == 0)
					{
						
						if(bLeftOperandType == VARIABLE)
						{
							if(!VarTab.GetVar(vi,nLeftOperandCode))	{ SetError("Global variable not found"); return false; }
							pV = vi.pDClass;
						}
						else
						{
							pV = SStack.Read(pRun_fi->stack_offset,nLeftOperandCode);
							if(pV == 0) { SetError("Local variable not found"); return false; }
						}
						pLeftOperandAClass = pV->GetAClass(nLeftOperandIndex);
					}
					if(pLeftOperandAClass == 0) {SetError("AClass ERROR n1"); return false; }
					
					vtype = BC_TokenGet();
					var_code = *((long *)&pRunCodeBase[TLR_DataOffset]);
					if(!(vtype == VARIABLE || vtype == LOCAL_VARIABLE)){SetError("invalid access var");return false;}
					if(vtype == VARIABLE)
					{
						if(!VarTab.GetVar(vi,var_code))	{ SetError("Global variable not found"); return false; }
						pVV = vi.pDClass;
					}
					else
					{
						pVV = SStack.Read(pRun_fi->stack_offset,var_code);
						if(pVV == 0) { SetError("Local variable not found"); return false; }
					}
					pVV->Get(pAccess_string);

					//pLeftOperandAClass = pLeftOperandAClass->VerifyAttributeClass(pAccess_string);
					pLeftOperandAClass = pLeftOperandAClass->CreateSubAClass(pLeftOperandAClass,pAccess_string);
				
					break;
				}
			break;
			case DEBUG_LINE_CODE:
#ifndef _XBOX
				if(Core.Exit_flag) return false;
				if(pDbgExpSource) break;
				if(bDebugExpressionRun) break;
				memcpy(&nDebugTraceLineCode,&pCodeBase[ip],sizeof(dword));
				if(bTraceMode)
				{
					if(CDebug.GetTraceMode() == TMODE_MAKESTEP || CDebug.GetTraceMode() == TMODE_MAKESTEP_OVER)
					{
						if(CDebug.GetTraceMode() == TMODE_MAKESTEP_OVER && bDebugWaitForThisFunc == false) break;

						if(!CDebug.IsDebug()) CDebug.OpenDebugWindow(Core.GetAppInstance());
						//else 
						ShowWindow(CDebug.GetWindowHandle(),SW_NORMAL);

						CDebug.SetTraceLine(nDebugTraceLineCode);
						CDebug.BreakOn(fi.decl_file_name,nDebugTraceLineCode);
						CDebug.SetTraceMode(TMODE_WAIT);
						while(CDebug.GetTraceMode() == TMODE_WAIT)
						{
							Sleep(40);
						}
						if(CDebug.GetTraceMode() == TMODE_MAKESTEP_OVER) bDebugWaitForThisFunc = true;
						else bDebugWaitForThisFunc = false;

					
					}
					else
					if(CDebug.Breaks.CanBreak())
					{
						// check for breakpoint
						if(CDebug.Breaks.Find(fi.decl_file_name,nDebugTraceLineCode))
						{
							if(!CDebug.IsDebug()) CDebug.OpenDebugWindow(Core.GetAppInstance());
							
							ShowWindow(CDebug.GetWindowHandle(),SW_NORMAL);
							//CDebug.OpenDebugWindow(Core.hInstance);
							CDebug.SetTraceMode(TMODE_WAIT);
							CDebug.BreakOn(fi.decl_file_name,nDebugTraceLineCode);
							
							while(CDebug.GetTraceMode() == TMODE_WAIT) {Sleep(40);}	// wait for debug thread decision
							if(CDebug.GetTraceMode() == TMODE_MAKESTEP_OVER) bDebugWaitForThisFunc = true;
							else bDebugWaitForThisFunc = false;
							
						}
					}
				}
#else
				memcpy(&nDTLCode,&pCodeBase[ip],sizeof(dword));

#endif
			break;
			case DEBUG_FILE_NAME:

				/*if(!bTraceMode) break;

				long index;
				long file_line_offset;
				long file_name_size;
				index = ip;
				memcpy(&file_line_offset,&pCodeBase[index],sizeof(dword)); index += sizeof(dword);
				memcpy(&file_name_size,&pCodeBase[index],sizeof(dword)); index += sizeof(dword);
				memcpy(DebugTraceFileName,&pCodeBase[index],file_name_size); index += file_name_size;
				DebugTraceFileName[file_name_size] = 0;
				//DebugSourceLine = 1;*/
			break;
			case BLOCK_IN:
			case BLOCK_OUT: 
			//case DEBUG_FILE_NAME:
				SetError("Unallowed instraction");
				return false;
			break;
			default: SetError("Incorrect instruction code"); return false;

			case CALL:	// undetermined function call
				vtype = BC_TokenGet();	// read variable
				var_code = *((long *)&pRunCodeBase[TLR_DataOffset]);	// var code
				if(vtype == VARIABLE)
				{
					if(!VarTab.GetVar(vi,var_code))	{ SetError("Global variable not found"); return false; }
					pVV = vi.pDClass;
				}
				else
				{
					pVV = SStack.Read(pRun_fi->stack_offset,var_code);
					if(pVV == 0) { SetError("Local variable not found"); return false; }
				}
				pVV = pVV->GetVarPointer();
				if(pVV->GetType() != VAR_STRING) { SetError("string call argument isnt string var"); return false; }
				pVV->Get(pAccess_string);	// func name
				func_code = FuncTab.FindFunc(pAccess_string);
				if(func_code == INVALID_FUNC_CODE) { SetError("function '%s' not found",pAccess_string); return false; }
				pVResult = 0;
				if(!BC_CallFunction(func_code,ip,pVResult)) return false;
				if(pVResult) 
				{
					ExpressionResult.Set((long)1);
					//SStack.Pop();
				} else ExpressionResult.Set((long)0);
			break;
			case CALL_FUNCTION:
				memcpy(&func_code,&pCodeBase[ip],sizeof(dword));
				pVResult = 0;
				if(!BC_CallFunction(func_code,ip,pVResult)) return false;
				//if(pVResult) SStack.Pop();
			break;
			case FUNCTION_RETURN_VOID: 
				if(fi.return_type != TVOID)
				{
					SetError("function must return value"); return false;
				}
				//for(n=0;n<fi.var_num;n++) SStack.Pop();
				SStack.InvalidateFrom(fi.stack_offset);

				return true;
			case FUNCTION_RETURN:
				if(fi.return_type == TVOID)
				{
					SetError("void function return value"); 
					
					return false;
				}
				// at this moment result expression placed in EX register
				
				if(pDbgExpSource == 0)	// skip stack unwind for dbg expression process	// ????????
						SStack.InvalidateFrom(fi.stack_offset);

				// copy result into stack
				pV = SStack.Push();
				pV->Copy(&ExpressionResult);
				pVReturnResult = pV;

				// check return type
				if(pDbgExpSource == 0)	// skip test for dbg expression process
				if(fi.return_type != pV->GetType())
				{
					SetError("%s function return %s value",
						Token.GetTypeName(fi.return_type),
						Token.GetTypeName(pV->GetType())); 
					
					return false;
				}

				/*if(BC_ProcessExpression(&ExpressionResult))
				{
					if(pDbgExpSource == 0)	// skip stack unwind for dbg expression process
						SStack.InvalidateFrom(fi.stack_offset);
					pV = SStack.Push();
					pV->Copy(&ExpressionResult);
					pVReturnResult = pV;

					if(pDbgExpSource == 0)	// skip test for dbg expression process
					if(fi.return_type != pV->GetType())
					{
						SetError("%s function return %s value",
							Token.GetTypeName(fi.return_type),
							Token.GetTypeName(pV->GetType())); 
						
						return false;
					}
				} 
				else 
				{
					SetError("incorrect return expression"); 
					
					return false;
				}*/

				
				
			return true;
			//case PROCESS_EXPRESSION:
				//BC_ProcessExpression(&ExpressionResult);
			//break;
			/*case WHILE_BLOCK:
				if(BC_TokenGet(ip,token_data_size) != OPEN_BRACKET){SetError("missing '('"); return false;};
				BC_ProcessExpression(&ExpressionResult);
			break;
			case IF_BLOCK:
				if(BC_TokenGet(ip,token_data_size) != OPEN_BRACKET){SetError("missing '('"); return false;};
				BC_ProcessExpression(&ExpressionResult);
			break;
			case SWITCH_COMMAND:
				if(BC_TokenGet(ip,token_data_size) != OPEN_BRACKET){SetError("missing '('"); return false;};
				BC_ProcessExpression(&ExpressionResult);
			break;*/


			case OP_MULTIPLYEQ:
			case OP_DIVIDEEQ:
			case OP_INCADD:
			case OP_DECADD:
				S_TOKEN_TYPE OpType;
				OpType = Token_type;
				if(nLeftOperandCode == INVALID_VAR_CODE) { SetError("no Lvalue"); return false;	}
				if(bLeftOperandType == VARIABLE)
				{
					if(!VarTab.GetVar(vi,nLeftOperandCode))	{ SetError("Global variable not found"); return false; }
					pV = vi.pDClass;
				}
				else
				{
					pV = SStack.Read(pRun_fi->stack_offset,nLeftOperandCode);
					if(pV == 0) { SetError("Local variable not found"); return false; }
				}
				if(nLeftOperandIndex != INVALID_ARRAY_INDEX)
				{
					pV = (DATA*)pV->GetArrayElement(nLeftOperandIndex);
					if(pV == 0) { SetError("invalid array index"); return false; }
					
				}
				//BC_ProcessExpression(&ExpressionResult);
				pVV = SStack.Read();
				switch(OpType)
				{
					case OP_INCADD:		pV->Plus(pVV); break;
					case OP_DECADD:		pV->Minus(pVV); break;
					case OP_MULTIPLYEQ: pV->Multiply(pVV); break;
					case OP_DIVIDEEQ: pV->Divide(pVV); break;
				}
				
			break;//*/

			
			/*case OP_MULTIPLYEQ:
			case OP_DIVIDEEQ:
			case OP_INCADD:
			case OP_DECADD:
				S_TOKEN_TYPE OpType;
				OpType = Token_type;
				if(nLeftOperandCode == INVALID_VAR_CODE) { SetError("no Lvalue"); return false;	}
				if(bLeftOperandType == VARIABLE)
				{
					if(!VarTab.GetVar(vi,nLeftOperandCode))	{ SetError("Global variable not found"); return false; }
					pV = vi.pDClass;
				}
				else
				{
					pV = SStack.Read(pRun_fi->stack_offset,nLeftOperandCode);
					if(pV == 0) { SetError("Local variable not found"); return false; }
				}
				if(nLeftOperandIndex != INVALID_ARRAY_INDEX)
				{
					pV = (DATA*)pV->GetArrayElement(nLeftOperandIndex);
					if(pV == 0) { SetError("invalid array index"); return false; }
					
				}
				BC_ProcessExpression(&ExpressionResult);
				switch(OpType)
				{
					case OP_INCADD:		pV->Plus(&ExpressionResult); break;
					case OP_DECADD:		pV->Minus(&ExpressionResult); break;
					case OP_MULTIPLYEQ: pV->Multiply(&ExpressionResult); break;
					case OP_DIVIDEEQ: pV->Divide(&ExpressionResult); break;
				}
				
			break;//*/
			case OP_INC:
				if(nLeftOperandCode == INVALID_VAR_CODE) { SetError("no Lvalue"); return false;	}
				if(bLeftOperandType == VARIABLE)
				{
					if(!VarTab.GetVar(vi,nLeftOperandCode))	{ SetError("Global variable not found"); return false; }
					pV = vi.pDClass;
				}
				else
				{
					pV = SStack.Read(pRun_fi->stack_offset,nLeftOperandCode);
					if(pV == 0) { SetError("Local variable not found"); return false; }
				}
				if(nLeftOperandIndex == INVALID_ARRAY_INDEX) pV->Inc();
				else 
				{
					pV = (DATA*)pV->GetArrayElement(nLeftOperandIndex);
					if(pV == 0) { SetError("invalid array index"); return false; }
					pV->Inc();
				}
			break;
			case OP_DEC:
				if(nLeftOperandCode == INVALID_VAR_CODE) { SetError("no Lvalue"); return false;	}
				if(bLeftOperandType == VARIABLE)
				{
					if(!VarTab.GetVar(vi,nLeftOperandCode))	{ SetError("Global variable not found"); return false; }
					pV = vi.pDClass;
				}
				else
				{
					pV = SStack.Read(pRun_fi->stack_offset,nLeftOperandCode);
					if(pV == 0) { SetError("Local variable not found"); return false; }
				}
				if(nLeftOperandIndex == INVALID_ARRAY_INDEX) pV->Dec();
				else 
				{
					pV = (DATA*)pV->GetArrayElement(nLeftOperandIndex);
					if(pV == 0) { SetError("invalid array index"); return false; }
					pV->Dec();
				}
			break;
			/*case OP_EQUAL:
				if(nLeftOperandCode == INVALID_VAR_CODE) { SetError("no Lvalue"); return false;	}
				if(BC_ProcessExpression(&ExpressionResult))
				{
					if(pLeftOperandAClass != 0)
					{
						// set attribute class value
						char * pChar;
						// 2 signs precision --------------------------------
							if(ExpressionResult.GetType() == VAR_FLOAT)
							{
								float fV1;
								ExpressionResult.Get(fV1);
								sprintf(gs,"%.7f",fV1);
								ExpressionResult.Set(gs);
								
							}
						// 2 signs precision --------------------------------
						ExpressionResult.Convert(VAR_STRING);
						ExpressionResult.Get(pChar);
						pLeftOperandAClass->SetValue(pChar);

					
						if(bLeftOperandType == VARIABLE)
						{
							if(!VarTab.GetVar(vi,nLeftOperandCode))	{ SetError("Global variable not found"); return false; }
							pV = vi.pDClass;
						}
						else
						{
							pV = SStack.Read(pRun_fi->stack_offset,nLeftOperandCode);
							if(pV == 0) { SetError("Local variable not found"); return false; }
						}
						if(nLeftOperandIndex != INVALID_ARRAY_INDEX) pV = (DATA*)pV->GetArrayElement(nLeftOperandIndex);
						if(pV == 0) { SetError("bad array element"); return false; }
						pV->Get(eid);
						//_CORE_API->Entity_AttributeChanged(&eid,pLeftOperandAClass->GetThisName());
						if(bEntityUpdate) Core.Entity_AttributeChanged(&eid,pLeftOperandAClass);
						break;
					}
					// copy value to variable
					if(bLeftOperandType == VARIABLE)
					{
						if(!VarTab.GetVar(vi,nLeftOperandCode))	{ SetError("Global variable not found"); return false; }
						pV = vi.pDClass;
					}
					else
					{
						pV = SStack.Read(pRun_fi->stack_offset,nLeftOperandCode);
						if(pV == 0) { SetError("Local variable not found"); return false; }
					}
					
					if(pV->GetType() == VAR_REFERENCE && ExpressionResult.GetType() == VAR_REFERENCE)
					{
						pV->SetReference(ExpressionResult.GetReference());
						break;
					}

					pVV = ExpressionResult.GetVarPointer();

					
					DATA * pV1;
					pV1 = pV->GetVarPointer();
					if(pV1->GetType() != pVV->GetType())
					{
						// ??? think about a little later
						if(ExpressionResult.IsReference()) ExpressionResult.RefConvert();
						if(!ExpressionResult.Convert(pV1->GetType()))	/// ?????????? 
						{
							break;	// skip unallowed conversion, write to log(in Convert func)
						}
						pVV = ExpressionResult.GetVarPointer();	// ??????????? ->
						//ExpressionResult.RefConvert();
						//if(!pVV->Convert(ExpressionResult.GetType()))
						//{
						//	break;	// skip unallowed conversion, write to log(in Convert func)
						//}
					}

					if(nLeftOperandIndex == INVALID_ARRAY_INDEX) 
					{
						pV->Copy(pVV);
					}
					else 
					{
						pV->CopyOnElement(pVV,nLeftOperandIndex);
					}
				}
			break;*/
			/*case SQUARE_OPEN_BRACKET:
				if(!BC_ProcessExpression(&ExpressionResult)) { SetError("Invalid array index"); return false; }
				if(!ExpressionResult.Get(nLeftOperandIndex)) { SetError("Invalid array index"); return false; }
			break;*/

			/*case MAKEREF_COMMAND:
				if(BC_TokenGet() != OPEN_BRACKET) {SetError("missed '('"); return false;};
				Token_type = BC_TokenGet(ip,token_data_size);
				if(Token_type == VARIABLE)
				{
					if(!VarTab.GetVar(vi,*((dword *)&pCodeBase[ip]))) { SetError("Global variable not found"); break; }
					pRef = vi.pDClass;
				} 
				else
				{
					pRef = SStack.Read(pRun_fi->stack_offset,*((dword *)&pCodeBase[ip]));
					if(pRef == 0) { SetError("Local variable not found"); return false; }
				}
				if(!pRef->IsReference()) { SetError("makeref to non reference variable"); return false; }
				
				if(BC_TokenGet() != COMMA) {SetError("missed ','"); return false;};

				Token_type = BC_TokenGet(ip,token_data_size);
				if(Token_type == VARIABLE)
				{
					if(!VarTab.GetVar(vi,*((dword *)&pCodeBase[ip])))	{ SetError("Global variable not found"); break; }
					pVar = vi.pDClass;
				} 
				else
				{
					pVar = SStack.Read(pRun_fi->stack_offset,*((dword *)&pCodeBase[ip]));
					if(pVar == 0) { SetError("Local variable not found"); return false; }
				}

				if(BC_TokenGet()== SQUARE_OPEN_BRACKET)
				{
					if(!BC_ProcessExpression(&ExpressionResult)) { SetError("Invalid array index"); return false; }
					if(!ExpressionResult.Get(array_index)) { SetError("Invalid array index"); return false; }
					if(TokenType()!= SQUARE_CLOSE_BRACKET){ SetError("missed ']'"); return false; }
					BC_TokenGet();
					pVar = (DATA *)pVar->GetArrayElement(array_index);
				}
				pRef->SetReference(pVar->GetVarPointer());
				if(TokenType() != CLOSE_BRACKET) {SetError("missed ')'"); return false;};
			break;

			case MAKEAREF_COMMAND:
				Token_type = Token_type;

			break;

				if(BC_TokenGet() != OPEN_BRACKET) {SetError("missed '('"); return false;};
				Token_type = BC_TokenGet(ip,token_data_size);
				if(Token_type == VARIABLE)
				{
					if(!VarTab.GetVar(vi,*((dword *)&pCodeBase[ip]))) { SetError("Global variable not found"); break; }
					pRef = vi.pDClass;
				} 
				else
				{
					pRef = SStack.Read(pRun_fi->stack_offset,*((dword *)&pCodeBase[ip]));
					if(pRef == 0) { SetError("Local variable not found"); return false; }
				}
				if(!pRef->IsAReference()) { SetError("makeref to non A reference variable"); return false; }
				
				if(BC_TokenGet() != COMMA) {SetError("missed ','"); return false;};

				Token_type = BC_TokenGet(ip,token_data_size);
				if(Token_type == VARIABLE)
				{
					if(!VarTab.GetVar(vi,*((dword *)&pCodeBase[ip])))	{ SetError("Global variable not found"); break; }
					pVar = vi.pDClass;
					if(pVar == 0) { SetError("Global variable not found"); return false; }
				} 
				else
				{
					pVar = SStack.Read(pRun_fi->stack_offset,*((dword *)&pCodeBase[ip]));
					if(pVar == 0) { SetError("Local variable not found"); return false; }
				}

				if(BC_TokenGet()== SQUARE_OPEN_BRACKET)
				{
					if(!BC_ProcessExpression(&ExpressionResult)) { SetError("Invalid array index"); return false; }
					if(!ExpressionResult.Get(array_index)) { SetError("Invalid array index"); return false; }
					if(TokenType()!= SQUARE_CLOSE_BRACKET){ SetError("missed ']'"); return false; }
					BC_TokenGet();
					pVar = (DATA*)pVar->GetArrayElement(array_index);
					if(pVar == 0) { SetError("Invalid array index"); return false; }
				}
				pRoot = pVar->GetAClass();
				if(pRoot == 0) {SetError("AClass ERROR n1"); return false; }
				//while(TokenType() == ACCESS_WORD)
				//{
				//	pRoot = pRoot->CreateSubAClass(pRoot,(char *)&pRunCodeBase[TLR_DataOffset]);
				//	BC_TokenGet();
				//}
				while(TokenType() == ACCESS_WORD_CODE || TokenType() == ACCESS_WORD || TokenType() == ACCESS_VAR)
				{
					if(TokenType() == ACCESS_WORD_CODE)
					{
						pRoot = pRoot->VerifyAttributeClassByCode(*((long *)&pRunCodeBase[TLR_DataOffset]));
						// pRoot = pRoot->CreateSubAClass(pRoot,(char *)&pRunCodeBase[TLR_DataOffset]);
					} else
					if(TokenType() == ACCESS_WORD)
					{
						pRoot = pRoot->CreateSubAClass(pRoot,(char *)&pRunCodeBase[TLR_DataOffset]);
					} else
					if(TokenType() == ACCESS_VAR)
					{
						vtype = BC_TokenGet();
						var_code = *((long *)&pRunCodeBase[TLR_DataOffset]);
						if(!(vtype == VARIABLE || vtype == LOCAL_VARIABLE)){SetError("invalid access var");return false;}
						if(vtype == VARIABLE)
						{
							if(!VarTab.GetVar(vi,var_code))	{ SetError("Global variable not found"); return false; }
							pVV = vi.pDClass;
						}
						else
						{
							pVV = SStack.Read(pRun_fi->stack_offset,var_code);
							if(pVV == 0) { SetError("Local variable not found"); return false; }
						}
						pVV->Get(pAccess_string);
						pRoot = pRoot->CreateSubAClass(pRoot,pAccess_string);
					}

					BC_TokenGet();
				}
				pRef->SetAReference(pRoot);
				
				pVar->Get(eid);
				pRef->Set(eid);
				if(TokenType() != CLOSE_BRACKET) {SetError("missed ')'"); return false;};
			break;*/
			
			//-----------------------------------------------------------------------
			
			case GOTO_COMMAND:
			break;
			case CONTINUE_COMMAND:
			break;
			case BREAK_COMMAND:
			break;
				
			
			case CASE_COMMAND:
			return false;
				
			case SEPARATOR:
			break;
			case UNKNOWN:
			break;
			case VAR_OBJECT:
			case VAR_STRING:
			case VAR_FLOAT:
			case VAR_INTEGER:
			break;
			case OPEN_BRACKET:
			break;
			case CLOSE_BRACKET:
			break;
			case COMMA:

			break;
			case DOT:

			break;
/*			// off start whean ready compile expression
			case OP_BOOL_EQUAL:
			case OP_GREATER:
			case OP_GREATER_OR_EQUAL:
			case OP_LESSER:
			case OP_LESSER_OR_EQUAL:
			case OP_NOT_EQUAL:
			case OP_MINUS:
			case OP_PLUS:
			case OP_MULTIPLY:
			case OP_DIVIDE:
			case OP_POWER:
			case OP_MODUL:
				

			break;//*/

			//----------------------------------------------
			case NUMBER:
				
			break;
			case FLOAT_NUMBER:
				
			break;
			case STRING:
				
			break;
			//----------------------------------------------

			case PUSH_OBJID_BXINDEX:
			case PUSH_OBJID:

				if(Token_type == PUSH_OBJID_BXINDEX) 
				{
					bUseIndex = true;
					rBX.Get(dwBXIndex);
				}
				else bUseIndex = false;


				pV = SStack.Push();
				pV->ClearType();

				pVV = GetOperand(pCodeBase,ip,&Token_type);


				if(!(Token_type == VARIABLE || Token_type == LOCAL_VARIABLE)) 
				{
					SetError("invalid arg for push_objid");
					return false;
				}
				if(bUseIndex)
				{
					pVV = pVV->GetArrayElement(dwBXIndex);
				}

				if(!(pVV->GetType() == VAR_AREFERENCE || pVV->GetType() == VAR_REFERENCE || pVV->GetType() == VAR_OBJECT)) {SetError("not aref or ref or object"); }
				
				pVV->Get(eid);
				pV->Set(eid);
			break;
			// process expression section ------------------

			case SETAREF:

				pV = SStack.Pop();
				pV->Get(eid);

				// destination
				Token_type = BC_TokenGet(ip,token_data_size);
				switch(Token_type)
				{
					case STACK_TOP:
						pVDst = SStack.Read();
					break;
					default:
						SetError("invalid argument for setaref (dst)");
					return false;
				}
				if(!pVDst) {SetError("invalid argument for setaref (dst)");	return false;}
				pVDst = pVDst->GetVarPointer();
				if(!pVDst) {SetError("invalid argument for setaref (dst)");	return false;}
				if(pVDst->GetType()!= VAR_AREFERENCE){SetError("isnt aref");	return false;}

				// source
				Token_type = BC_TokenGet(ip,token_data_size);
				switch(Token_type)
				{
					case AP:
						pVDst->Set(eid);
						pVDst->SetAReference(rAP);
						
						
					break;
					default:
						SetError("invalid argument for setref (src)");
					return false;
				}

				
			break;
			case SETREF_BXINDEX:
			case SETREF:

				if(Token_type == SETREF_BXINDEX) 
				{
					bUseIndex = true;
					rBX.Get(dwBXIndex);
				}
				else bUseIndex = false;

				pVDst = 0;
				pVSrc = 0;

				// destination
				Token_type = BC_TokenGet(ip,token_data_size);
				switch(Token_type)
				{
					case STACK_TOP:
						pVDst = SStack.Read();
						pVDst->ClearType();
						pVDst->SetType(VAR_REFERENCE);
					break;
					case VARIABLE:
						if(!VarTab.GetVar(vi,*((dword *)&pCodeBase[ip])))	{ SetError("Global variable not found"); return false; }
						pVDst = vi.pDClass;
						//if(bUseIndex) pVDst = pVDst->GetArrayElement(dwBXIndex);
						if(!pVDst) return false;
						if(pVDst->GetType()!=VAR_REFERENCE) {SetError("'%s' isnt reference",vi.name);return false;}
					break;
					case LOCAL_VARIABLE:
						pVDst = SStack.Read(pRun_fi->stack_offset,*((dword *)&pCodeBase[ip]));
						if(pVDst == 0) { SetError("Local variable not found"); return false; }
						//if(bUseIndex) pVDst = pVDst->GetArrayElement(dwBXIndex);
						if(!pVDst) return false;
						if(pVDst->GetType()!=VAR_REFERENCE) {SetError("'%s' isnt reference",vi.name);return false;}
					break;
					default:
						SetError("invalid argument for setref (dst)");
					return false;
				}
				if(!pVDst) {SetError("invalid argument for setref (dst)");	return false;}

				// source
				Token_type = BC_TokenGet(ip,token_data_size);
				switch(Token_type)
				{
					case VARIABLE:
						if(!VarTab.GetVar(vi,*((dword *)&pCodeBase[ip])))	{ SetError("Global variable not found"); return false; }
						pVSrc = vi.pDClass;
						if(bUseIndex) pVSrc = pVSrc->GetArrayElement(dwBXIndex);
						if(!pVSrc) return false;
					break;
					case LOCAL_VARIABLE:
						pVSrc = SStack.Read(pRun_fi->stack_offset,*((dword *)&pCodeBase[ip]));
						if(pVSrc == 0) { SetError("Local variable not found"); return false; }
						if(bUseIndex) pVSrc = pVSrc->GetArrayElement(dwBXIndex);
						if(!pVSrc) return false;
					break;
					default:
						SetError("invalid argument for setref (src)");
					return false;
				}

				pVDst->SetReference(pVSrc->GetVarPointer());

			break;
			case MOVE:

				pVDst = 0;
				pVSrc = 0;
				// find destination
				Token_type = BC_TokenGet(ip,token_data_size);
				switch(Token_type)
				{
					case LEFT_OPERAND:
						if(nLeftOperandCode == INVALID_VAR_CODE) { SetError("no Lvalue"); return false;	}

						if(bLeftOperandType == VARIABLE)
						{
							if(!VarTab.GetVar(vi,nLeftOperandCode))	{ SetError("Global variable not found"); return false; }
							pVDst = vi.pDClass;
						}
						else
						{
							if(pRun_fi)
							{
								pVDst = SStack.Read(pRun_fi->stack_offset,nLeftOperandCode);
								if(pVDst == 0) { SetError("Local variable not found"); return false; }
							}else{
								pVDst = 0;
								SetError("Internal error, line 4808! Current function not defined..."); return false;
							}							
							
						}
					break;
					/*case EX:
						ExpressionResult.ClearType();
						pVDst = &ExpressionResult;

					break;*/
					default:
						SetError("invalid DST");
					return false;
				}
				
				if(pVDst == 0) { SetError("no LValue"); return false; }

				// find source
				Token_type = BC_TokenGet(ip,token_data_size);
				switch(Token_type)
				{
					case STACK_TOP:
						pVSrc = SStack.Read();
						if(!pVSrc) return false;
					break;
					default:
						SetError("incorrect SRC");
					return false;
				}

				// check for attribute operations
				if(pLeftOperandAClass != 0)
				{
					// set attribute class value
					char * pChar;

					// use ExpressionResult as temporary register
					ExpressionResult.ClearType();
					ExpressionResult.Copy(pVSrc);

					// 2 signs precision --------------------------------
					if(ExpressionResult.GetType() == VAR_FLOAT)
					{
						float fV1;
						ExpressionResult.Get(fV1);
						sprintf(gs,"%.7f",fV1);
						ExpressionResult.Set(gs);	
					}
					// 2 signs precision --------------------------------
					ExpressionResult.Convert(VAR_STRING);
					if(!ExpressionResult.Get(pChar)) break;
					pLeftOperandAClass->SetValue(pChar);
					
					if(nLeftOperandIndex != INVALID_ARRAY_INDEX) pVDst = pVDst->GetArrayElement(nLeftOperandIndex);
					if(!pVDst) return false;

					pVDst->Get(eid);
					if(bEntityUpdate)
					{
						Core.Entity_AttributeChanged(&eid,pLeftOperandAClass);
					}
					break;
				}

				

				// test for references operations
				if(pVSrc->GetType() == VAR_REFERENCE && pVDst->GetType() == VAR_REFERENCE)
				{
					pVDst->SetReference(pVSrc->GetReference());
					break;
				}
				
				// find real dst
				pVDst = pVDst->GetVarPointer();
				if(!pVDst) {SetError("no DST");return false;}

				if(nLeftOperandIndex != INVALID_ARRAY_INDEX) pVDst = pVDst->GetArrayElement(nLeftOperandIndex);
				if(!pVDst) return false;

				// test for allowed data conversion
				DATA * pVTTSrc;
				DATA * pVTTDst;
				pVTTSrc = pVSrc->GetVarPointer();
				pVTTDst = pVDst->GetVarPointer();
					
				if(pVTTSrc->GetType() != pVTTDst->GetType())
				{
					// ??? think about a little later
					ExpressionResult.ClearType();
					ExpressionResult.Copy(pVSrc);
					if(ExpressionResult.IsReference()) ExpressionResult.RefConvert();
					if(!ExpressionResult.Convert(pVTTDst->GetType()))	/// ?????????? 
					{
						break;	// skip unallowed conversion, write to log(in Convert func)
					}
					pVSrc = ExpressionResult.GetVarPointer();	// converted source
					
				}

				// if not attribute operation proceed to variables copy

				//if(nLeftOperandIndex != INVALID_ARRAY_INDEX) pVDst = pVDst->GetArrayElement(nLeftOperandIndex);
				//if(!pVDst) return false;
				pVDst->Copy(pVSrc);
					

			break;
			
			case STACK_ALLOC:
				SStack.Push();
			break;

			
			case STACK_WRITE_BXINDEX:
			case STACK_PUSH:
			case STACK_WRITE:

				if(Token_type == STACK_WRITE_BXINDEX) 
				{
					bUseIndex = true;
					rBX.Get(dwBXIndex);
				}
				else bUseIndex = false;
			
				
				if(Token_type == STACK_PUSH) pV = SStack.Push();
				else pV = SStack.Read();

				Token_type = BC_TokenGet(ip,token_data_size);
				switch(Token_type)
				{
					case NUMBER:
						pV->Set(*((long *)&pRunCodeBase[TLR_DataOffset]));
					break;
					case FLOAT_NUMBER:
						pV->Set(*((float *)&pRunCodeBase[TLR_DataOffset]));
					break;
					case STRING:
						pV->Set((char *)&pRunCodeBase[TLR_DataOffset + 4]);	// 4 - string length
					break;
					case VARIABLE:
						if(!VarTab.GetVar(vi,*((dword *)&pCodeBase[ip])))	{ SetError("Global variable not found"); break; }
						pVV = vi.pDClass;
						//pVV = pVV->GetVarPointer();
						//if(!pVV) { SetError("invalid ref"); break; }
						if(pVV->IsReference())
						{
							if(bUseIndex)
							{
								pVV = pVV->GetVarPointer();
								pVV = pVV->GetArrayElement(dwBXIndex);
								if(!pVV) { SetError("invalid ref"); break; }

								pV->SetType(VAR_REFERENCE);
								pV->SetReference(pVV);
							}
							else
							{
								pV->SetType(VAR_REFERENCE);
								pV->SetReference(pVV->GetReference());
							}
							break;
						}

						if(bUseIndex)
						{
							pVV = pVV->GetArrayElement(dwBXIndex);
							if(!pVV) { SetError("invalid array index"); break; }
							pV->Copy(pVV);
						}
						else pV->Copy(pVV);
						
					break;
					case LOCAL_VARIABLE:
						pVar = SStack.Read(pRun_fi->stack_offset,*((dword *)&pCodeBase[ip]));
						if(pVar == 0) { SetError("Local variable not found"); return false; }
						if(pVar->IsReference())
						{
							if(bUseIndex)
							{
								pVar = pVar->GetVarPointer();
								pVar = pVar->GetArrayElement(dwBXIndex);
								if(!pVar) { SetError("invalid ref"); break; }

								pV->SetType(VAR_REFERENCE);
								pV->SetReference(pVar);
							}
							else
							{
								pV->SetType(VAR_REFERENCE);
								pV->SetReference(pVar->GetReference());
							}
							break;
						}
						
						//if(!pVar) { SetError("invalid ref"); break; }

						if(bUseIndex)
						{
							pVar = pVar->GetArrayElement(dwBXIndex);
							if(!pVar) { SetError("invalid array index"); break; }
							pV->Copy(pVar);
						}
						else pV->Copy(pVar);
				
					break;
					case AX:
						pV->Copy(&rAX);
					break;
					case BX:
						pV->Copy(&rBX);
					break;
					case EX:
						pV->Copy(&ExpressionResult);
					break;
					case AP_VALUE:
						if(!rAP) 
						{
							SetError("no rAP data"); 
							pV->Set("error");
							break; /*return false;*/
						}
						pV->Set(rAP->GetThisAttr());
					break;
					default:
						SetError("invalid argument for STACK_PUSH");
					return false;
				}
			break;

			case STACK_POP_VOID:
				SStack.Pop();
			break;
			case STACK_POP:
			case STACK_READ:
				if (SStack.GetDataNum() == 0) break;
				if(Token_type == STACK_POP) pV = SStack.Pop();
				else pV = SStack.Read();
				
				Token_type = BC_TokenGet(ip,token_data_size);
				switch(Token_type)
				{
					case VARIABLE:
						if(!VarTab.GetVar(vi,*((dword *)&pCodeBase[ip])))	{ SetError("Global variable not found"); break; }
						vi.pDClass->Copy(pV);
					break;
					case LOCAL_VARIABLE:
						pVar = SStack.Read(pRun_fi->stack_offset,*((dword *)&pCodeBase[ip]));
						if(pVar == 0) { SetError("Local variable not found"); return false; }
						pVar->Copy(pV);
					break;
					case AX:
						rAX.ClearType();
						rAX.Copy(pV);
					break;
					case BX:
						rBX.ClearType();
						rBX.Copy(pV);
					break;
					case EX:
						ExpressionResult.ClearType();
						ExpressionResult.Copy(pV);
					break;
					case LEFTOP_INDEX:
						pV->Convert(VAR_INTEGER);
						pV->Get(nLeftOperandIndex);
					break;
					default:
						SetError("invalid argument for STACK_POP");
					return false;
				}
			break;
			
			case OP_BOOL_CONVERT:
				pV = GetOperand(pCodeBase,ip); if(!pV) return false;
				pV->BoolConvert();
			break;
			case OP_BOOL_AND:
			case OP_BOOL_OR:
				pV = GetOperand(pCodeBase,ip); if(!pV) return false;
				pVV = GetOperand(pCodeBase,ip); if(!pVV) return false;

				pV->BoolConvert();
				pV->RefConvert();
				pV->CompareAndSetResult(pVV,Token_type);
			break;
			case OP_NOT_EQUAL:
			case OP_LESSER_OR_EQUAL:
			case OP_LESSER:
			case OP_GREATER_OR_EQUAL:
			case OP_GREATER:
			case OP_BOOL_EQUAL:
				pV = GetOperand(pCodeBase,ip); if(!pV) return false;
				pVV = GetOperand(pCodeBase,ip); if(!pVV) return false;
				pV->RefConvert();
				pV->CompareAndSetResult(pVV,Token_type);
			break;
			case OP_PLUS:
				pV = GetOperand(pCodeBase,ip); if(!pV) return false;
				pVV = GetOperand(pCodeBase,ip); if(!pVV) return false;
				pV->Plus(pVV);
			break;
			case OP_MINUS:
				pV = GetOperand(pCodeBase,ip); if(!pV) return false;
				pVV = GetOperand(pCodeBase,ip); if(!pVV) return false;
				pV->Minus(pVV);
			break;
			case OP_MULTIPLY:
				pV = GetOperand(pCodeBase,ip); if(!pV) return false;
				pVV = GetOperand(pCodeBase,ip); if(!pVV) return false;
				pV->Multiply(pVV);
			break;
			case OP_DIVIDE:
				pV = GetOperand(pCodeBase,ip); if(!pV) return false;
				pVV = GetOperand(pCodeBase,ip); if(!pVV) return false;
				pV->Divide(pVV);
			break;
			case OP_MODUL:
				pV = GetOperand(pCodeBase,ip); if(!pV) return false;
				pVV = GetOperand(pCodeBase,ip); if(!pVV) return false;
				pV->Modul(pVV);
			break;
			case OP_POWER:
				pV = GetOperand(pCodeBase,ip); if(!pV) return false;
				pVV = GetOperand(pCodeBase,ip); if(!pVV) return false;
				pV->Power(pVV);
			break;
			case OP_SMINUS:
				pV = GetOperand(pCodeBase,ip); if(!pV) return false;
				pV->Inverse();
			break;
			case OP_BOOL_NEG:
				pV = GetOperand(pCodeBase,ip); if(!pV) return false;
				pV->Neg();
			break;
			
			case MOVEAP_BXINDEX:
			case MOVEAP:

				if(Token_type == MOVEAP_BXINDEX) 
				{
					bUseIndex = true;
					rBX.Get(dwBXIndex);
				}
				else bUseIndex = false;

				pV = GetOperand(pCodeBase,ip,&Token_type);
				switch(Token_type)
				{
					case VARIABLE:
					case LOCAL_VARIABLE:
						SetError("not implemented");
					return false;
					break;
					case AP:
						pVV = GetOperand(pCodeBase,ip,&Token_type);
						if(Token_type == AP) break;
						if(!pVV) return false;
						if(bUseIndex) pVV = pVV->GetArrayElement(dwBXIndex);
						if(!pVV) {SetError("invalid array index"); return false;}
						rAP = pVV->GetAClass();
					break;
				}
			break;
			case VERIFY_AP:
			case ADVANCE_AP:

				S_TOKEN_TYPE sttV = Token_type;

				char * pChar;
				if(!rAP) 
				{

					SetError("null ap"); 
					BC_TokenGet();	
					break; /*return false;*/
				}

				Token_type = BC_TokenGet();
				switch(Token_type)
				{
					case ACCESS_WORD_CODE:
						if(sttV == VERIFY_AP)
						rAP = rAP->VerifyAttributeClassByCode(*((long *)&pRunCodeBase[TLR_DataOffset]));
							else
						rAP = rAP->GetAttributeClassByCode(*((long *)&pRunCodeBase[TLR_DataOffset]));
						if(!rAP) SetError("missed attribute: %s",SCodec.Convert(*((long *)&pRunCodeBase[TLR_DataOffset])));
					break;
					case VARIABLE:
						if(!VarTab.GetVar(vi,*((long *)&pRunCodeBase[TLR_DataOffset])))	{ SetError("Global variable not found"); break; }
						ExpressionResult.ClearType();
						ExpressionResult.Copy(vi.pDClass);
						if(!ExpressionResult.Convert(VAR_STRING)) { SetError("invalid type for attribute var"); break; /*return false;*/ }
						ExpressionResult.Get(pChar);
						if(sttV == VERIFY_AP)
							rAP = rAP->CreateSubAClass(rAP,pChar);
						else
							rAP = rAP->FindAClass(rAP,pChar);
							
						/*if(sttV == VERIFY_AP)
						rAP = rAP->VerifyAttributeClass(pChar);
							else
						rAP = rAP->GetAttributeClass(pChar);*/
						if(!rAP) SetError("missed attribute: %s",pChar);
					break;
					case LOCAL_VARIABLE:
						pV = SStack.Read(pRun_fi->stack_offset,*((long *)&pRunCodeBase[TLR_DataOffset]));
						if(pV == 0) { SetError("Local variable not found"); break; /*return false;*/ }

						ExpressionResult.ClearType();
						ExpressionResult.Copy(pV);
						if(!ExpressionResult.Convert(VAR_STRING)) { SetError("invalid type for attribute var"); break; /*return false;*/ }
						ExpressionResult.Get(pChar);

						if(sttV == VERIFY_AP)
							rAP = rAP->CreateSubAClass(rAP,pChar);
						else
							rAP = rAP->FindAClass(rAP,pChar);
							

						/*if(sttV == VERIFY_AP)
							rAP = rAP->VerifyAttributeClass(pChar);
						else 
						rAP = rAP->GetAttributeClass(pChar);*/

						if(!rAP) SetError("missed attribute: %s",pChar);

					break;
					default:
						SetError("invalid arg type for ADVANCE_AP");
					return false;
				}

				//pLeftOperandAClass = rAP;
				
			break;
			
			//*/
			//----------------------------------------------

		}

	} while(Token_type != END_OF_PROGRAMM && bExit != true);
	
	if(Token_type == END_OF_PROGRAMM && inout != 0) SetError("Unexpected end of programm");
	UNGUARD
	return false;
}



void COMPILER::CompileNumber(SEGMENT_DESC& Segment)
{
	long lvalue;
	lvalue = atol(Token.GetData());
	CompileToken(Segment,NUMBER,1,(char *)&lvalue,sizeof(long));
}

void COMPILER::CompileFloatNumber(SEGMENT_DESC& Segment)
{
	float fvalue;
	fvalue = (float)atof(Token.GetData());
	CompileToken(Segment,FLOAT_NUMBER,1,(char *)&fvalue,sizeof(float));
}

void COMPILER::CompileString(SEGMENT_DESC& Segment)
{
	dword string_size;
	char * pData;
	pData = Token.GetData();
	if(pData != 0) 
	{
		string_size = strlen(Token.GetData()) + 1;
		CompileToken(Segment,STRING,2,(char *)&string_size,sizeof(dword),Token.GetData(),string_size);
	}
	else
	{
		string_size = strlen("") + 1;
		CompileToken(Segment,STRING,2,(char *)&string_size,sizeof(dword),"",string_size);
	}
}

bool COMPILER::CompileUnknown(SEGMENT_DESC& Segment)
{
	DWORD func_code;
	DWORD var_code;
	DWORD def_code;
	DWORD lab_code;

	// check for valid function code
	func_code = FuncTab.FindFunc(Token.GetData());
	
	if(func_code != INVALID_FUNC_CODE)
	{
		// this is function call
		CompileToken(Segment,CALL_FUNCTION,1,(char *)&func_code,sizeof(dword));
		//Trace("function call: %s ; code = %d",Token.GetData(),func_code);
		return true;
	}

	// check for valid local variable 
	var_code = FuncTab.FindVar(CurrentFuncCode,Token.GetData());
	if(var_code != INVALID_VAR_CODE)
	{
		CompileToken(Segment,LOCAL_VARIABLE,1,(char *)&var_code,sizeof(dword));
		//Trace("local variable: %s ; code = %d",Token.GetData(),var_code);
		return true;
	}

	var_code = VarTab.FindVar(Token.GetData());
	if(var_code != INVALID_VAR_CODE)
	{
		// this is gloabl variable
		CompileToken(Segment,VARIABLE,1,(char *)&var_code,sizeof(dword));
		//Trace("variable: %s ; code = %d",Token.GetData(),var_code);
		return true;
	}

	//def_code = DefineTable.GetStringCode(Token.GetData());
	def_code = DefTab.FindDef(Token.GetData());
	if(def_code != INVALID_DEF_CODE)
	{
		DEFINFO di;
		//DefineTable.GetStringData(def_code,&di);
		DefTab.GetDef(di,def_code);
		switch(di.deftype)
		{
			case NUMBER:
				CompileToken(Segment,NUMBER,1,(char *)&di.data4b,sizeof(dword));
			break;
			case FLOAT_NUMBER:
				CompileToken(Segment,FLOAT_NUMBER,1,(char *)&di.data4b,sizeof(dword));
			break;
			case STRING:
				dword string_size;
				string_size = strlen((char *)di.data4b) + 1;
				CompileToken(Segment,STRING,2,(char *)&string_size,sizeof(dword),(char *)di.data4b,string_size);
			break;
		}
		return true;
	}

	lab_code = LabelTable.GetStringCode(Token.GetData());
	if(lab_code != INVALID_ORDINAL_NUMBER)
	{
		LabelTable.SetStringData(lab_code,&Segment.BCode_Program_size);	
		return true;
	}

	SetError("Undeclared identifier: %s",Token.GetData());
	return false;
}

S_TOKEN_TYPE COMPILER::DetectUnknown(DWORD & code)
{
	DWORD func_code;
	DWORD var_code;
	DWORD def_code;

	// check for valid function code
	func_code = FuncTab.FindFunc(Token.GetData());
	
	if(func_code != INVALID_FUNC_CODE)
	{
		// this is function call
		code = func_code;
		return CALL_FUNCTION;
	}

	// check for valid local variable 
	var_code = FuncTab.FindVar(CurrentFuncCode,Token.GetData());
	if(var_code != INVALID_VAR_CODE)
	{
		code = var_code;
		return LOCAL_VARIABLE;
	}

	var_code = VarTab.FindVar(Token.GetData());
	if(var_code != INVALID_VAR_CODE)
	{
		code = var_code;
		return VARIABLE;
	}

	def_code = DefTab.FindDef(Token.GetData());
	if(def_code != INVALID_DEF_CODE)
	{
		code = def_code;
		return DEFINE_VAL;
		/*DEFINFO di;
		DefTab.GetDef(di,def_code);
		switch(di.deftype)
		{
			case NUMBER:
				CompileToken(Segment,NUMBER,1,(char *)&di.data4b,sizeof(dword));
			break;
			case FLOAT_NUMBER:
				CompileToken(Segment,FLOAT_NUMBER,1,(char *)&di.data4b,sizeof(dword));
			break;
			case STRING:
				dword string_size;
				string_size = strlen((char *)di.data4b) + 1;
				CompileToken(Segment,STRING,2,(char *)&string_size,sizeof(dword),(char *)di.data4b,string_size);
			break;
		}
		return true;*/
	}

	return UNKNOWN;
}

void COMPILER::ExitProgram()
{
	DWORD function_code;
	function_code = FuncTab.FindFunc("ExitMain");
	if(function_code != INVALID_FUNC_CODE)
	{
		DATA * pResult;
		BC_Execute(function_code,pResult);
	}
	_CORE_API->Exit();
}

void COMPILER::ClearEvents()
{
	//EventTab.Release();	
	EventTab.Clear();	
}


void COMPILER::SaveDataDebug(char * data_PTR, ...)
{
	char LogBuffer[MAX_PATH + MAX_PATH];
	if(data_PTR == 0) return;
	//fio->_SetFilePointer(hSaveFileFileHandle,0,0,FILE_END);
	va_list args;
	va_start(args,data_PTR);
	_vsnprintf(LogBuffer,sizeof(LogBuffer) - 4,data_PTR,args);
	strcat(LogBuffer,"\x0d\x0a");
	//fio->_WriteFile(hSaveFileFileHandle,LogBuffer,strlen(LogBuffer),&bytes);
	va_end(args);
}

void COMPILER::SaveData(const void * data_PTR, DWORD data_size)
{
	if(data_PTR == 0) return;

	if (dwCurPointer + data_size > dwMaxSize)
	{
		dword dwNewAllocate = (1 + (dwCurPointer + data_size) / (1024*1024)) * (1024*1024);
		pBuffer = (char*)RESIZE(pBuffer, dwNewAllocate);
		dwMaxSize = dwNewAllocate;
	}

	if (data_size < 16)
	{
		char * pWriteBuffer = &pBuffer[dwCurPointer];
		_asm
		{
			mov ecx, data_size
			mov edi, pWriteBuffer
			mov esi, data_PTR

			rep movsb
		}
	}
	else
		memcpy(&pBuffer[dwCurPointer], data_PTR, data_size);

	dwCurPointer += data_size;
}

bool COMPILER::ReadData(void * data_PTR, DWORD data_size)
{
	if(data_PTR == 0) 
	{
		dwCurPointer += data_size;
		return true;
	}
	if (dwCurPointer + data_size > dwMaxSize) return false;

	if (data_size < 16)
	{
		char * pReadBuffer = &pBuffer[dwCurPointer];
		_asm
		{
			mov ecx, data_size
			mov edi, data_PTR
			mov esi, pReadBuffer

			rep movsb
		}
	}
	else
		memcpy(data_PTR, &pBuffer[dwCurPointer], data_size);

	dwCurPointer += data_size;

	return true; 
}

bool COMPILER::FindReferencedVariable(DATA * pRef, DWORD & var_index, DWORD & array_index)
{
	VARINFO vi;	
	DWORD nVarNum;
	DWORD n;
	DWORD i;

	nVarNum = VarTab.GetVarNum();
	for(n=0;n<nVarNum;n++)
	{
		VarTab.GetVarX(vi,n);
		if(!vi.pDClass->IsArray())
		{
			if(pRef == vi.pDClass)
			{
				var_index = n;
				array_index = 0xffffffff;
				return true;
			}
		}
		else
		{
			for(i=0;i<vi.elements;i++)
			{
				if(pRef == vi.pDClass->GetArrayElement(i))
				{
					var_index = n;
					array_index = i;
					return true;
				}
			}
		} 
	}
	return false;
}

bool COMPILER::FindReferencedVariableByRootA(ATTRIBUTES * pA, DWORD & var_index, DWORD & array_index)
{
	VARINFO vi;	
	DWORD nVarNum;
	DWORD n;
	DWORD i;

	nVarNum = VarTab.GetVarNum();
	for(n=0;n<nVarNum;n++)
	{
		VarTab.GetVarX(vi,n);
		if(vi.type != VAR_OBJECT) continue;
		if(!vi.pDClass->IsArray())
		{
			if(pA == vi.pDClass->AttributesClass)
			{
				var_index = n;
				array_index = 0xffffffff;	// ***
				return true;
			}
		}
		else
		{
			for(i=0;i<vi.elements;i++)
			{
				if(pA == vi.pDClass->GetArrayElement(i)->AttributesClass)
				{
					var_index = n;
					array_index = i;
					return true;
				}
			}
		} 
	}
	return false;
}

ATTRIBUTES * COMPILER::TraceARoot(ATTRIBUTES * pA, char * & pAccess)
{
	char * pAS;
	long slen;
	
	if(pA == 0) return 0;				// error or invalid argument
	if(pA->GetParent() == 0) return pA;	// root found
	
	slen = strlen(pA->GetThisName())+1;
	
	pAS = NEW char[slen];
	
	strcpy(pAS,pA->GetThisName());
	
	if(pAccess == 0)
	{
		pAccess = pAS;
	} 
	else
	{
		pAS = (char *)RESIZE(pAS,slen + strlen(pAccess) + 1);
		strcat(pAS,".");
		strcat(pAS,pAccess);
		delete pAccess;
		pAccess = pAS;
	}

	
	return TraceARoot(pA->GetParent(),pAccess);
}


void COMPILER::WriteVDword(DWORD v)
{
/*	BYTE nbv;
	if(v < 0xff) { nbv = (BYTE)v; SaveData(&nbv, sizeof(nbv)); } 
	else 
	{
		nbv = 0xff; SaveData(&nbv, sizeof(nbv));
		SaveData(&v, sizeof(v));
	}
*/
	BYTE nbv;
	WORD nwv;
	if(v < 0xfe) { nbv = (BYTE)v; SaveData(&nbv, sizeof(nbv)); } 
	else
	if(v < 0xffff)
	{
		nbv = 0xfe; SaveData(&nbv, sizeof(nbv));
		nwv = (WORD)v;
		SaveData(&nwv, sizeof(nwv));
	}
	else 
	{
		nbv = 0xff; SaveData(&nbv, sizeof(nbv));
		SaveData(&v, sizeof(v));
	}
}

DWORD COMPILER::ReadVDword()
{
	BYTE nbv;
	WORD nwv;
	DWORD v;
	ReadData(&nbv,1);
	//fio->_ReadFile(hSaveFileFileHandle,&nbv,1,&bytes);
	if(nbv < 0xfe) return nbv;
	if(nbv == 0xfe)
	{
		ReadData(&nwv,sizeof(nwv));
		//fio->_ReadFile(hSaveFileFileHandle,&nwv,sizeof(nwv),&bytes);
		return nwv;
	}
	ReadData(&v,sizeof(DWORD));
	//fio->_ReadFile(hSaveFileFileHandle,&v,sizeof(DWORD),&bytes);
	return v;
}

void COMPILER::SaveString(const char * pS)
{
	DWORD n;
	if(pS == 0) 
	{
		WriteVDword(0);
		return;
	}
	n = strlen(pS) + 1;
	WriteVDword(n);
	SaveData(pS,n);
}

char * COMPILER::ReadString()
{
	DWORD n;
	char * pBuffer;
	n = ReadVDword();
	if(n == 0) return 0;
	
	pBuffer = NEW char[n];
	ReadData(pBuffer,n);
	return pBuffer;
}

bool COMPILER::ReadVariable(char * name,/* DWORD code,*/ bool bDim, DWORD a_index)
{
	long   nLongValue;
	float  fFloatValue;
	char * pString;
	DWORD  var_index;
	DWORD  array_index;
//	DWORD  nlen;
	DWORD  n;
	DWORD  nElementsNum;
	ATTRIBUTES * pA;
	S_TOKEN_TYPE eType;
	VARINFO vi;
	VARINFO viRef;
	DATA * pV;
	DATA * pVRef;
	ENTITY_ID eid;
	DWORD var_code;
	bool bSkipVariable;

	
#ifdef _XBOX
		VDX8RENDER * pDX = (VDX8RENDER *)api->CreateService("dx8render");
		if(pDX) pDX->ProgressView();
#endif

	// if(!VarTab.GetVarX(vi,code)) return false;
	// replacing by name search
	
	bSkipVariable = false;
	var_code = VarTab.FindVar(name);
	if(var_code == INVALID_VAR_CODE) 
	{
		SetError("Load warning - variable: '%s' not found",name);
		bSkipVariable = true;
	}
	else
	{
		VarTab.GetVarX(vi,var_code);
		pV = vi.pDClass;
	}

	if(!bDim)	// skip this info for array elements
	{
		//trace("Read[%d]: %s",code,vi.name);
		ReadData(&eType, sizeof(eType));
		if(!bSkipVariable)
		if(vi.type != eType) 
		{
			SetError("load type mismatch"); 
			return false;
		}
		ReadData(&nElementsNum, sizeof(nElementsNum));
		if(!bSkipVariable)
		if(vi.elements != nElementsNum) 
		{
			// ???
			//SetError("load size mismatch"); 
			//return false;
			vi.pDClass->SetElementsNum(nElementsNum);
			vi.elements = nElementsNum;
		}
	} 
	else 
	{
		if(bSkipVariable)
		{
			SetError("Load - lost element");
			return false;
		}
		nElementsNum = 1;
		eType = pV->GetType();
		pV = pV->GetArrayElement(a_index);
	}

	if(nElementsNum > 1)	// array
	{
		// load array elements
		for(n=0;n<nElementsNum;n++)
		{
			if(!ReadVariable(name,/*code,*/true,n)) return false;
		}
		return true;
	}

	ATTRIBUTES * pTA;

	switch(eType)
	{
		case VAR_INTEGER:
			ReadData(&nLongValue, sizeof(nLongValue));
			if(bSkipVariable) break;
			pV->Set(nLongValue);
		break;
		case VAR_FLOAT:
			ReadData(&fFloatValue, sizeof(fFloatValue));
			if(bSkipVariable) break;
			pV->Set(fFloatValue);
		break;
		case VAR_STRING:
			pString = ReadString();
			if(pString) 
			{
				if(!bSkipVariable) pV->Set(pString);
				delete pString;
			}
		break;
		case VAR_OBJECT:
			ReadData(&eid, sizeof(eid));
			if(!bSkipVariable) 
			{	pV->Set(eid);
				
				if(pV->AttributesClass == 0) pV->AttributesClass = NEW ATTRIBUTES(&SCodec);
				ReadAttributesData(pV->AttributesClass,0);
			} 
			else
			{
				
				pTA = NEW ATTRIBUTES(&SCodec);
				ReadAttributesData(pTA,0);
				delete pTA;
			}
		break;
		case VAR_REFERENCE:
			var_index = ReadVDword();
			if(var_index == 0xffffffff) break;	// uninitialized ref
			array_index = ReadVDword();
			if(bSkipVariable) break;
			if(!VarTab.GetVarX(viRef,var_index)) 
			{
				SetError("State read error");
				return false;
			}
			pVRef = viRef.pDClass;
			if(array_index != 0xffffffff)
			{
				pVRef = pVRef->GetArrayElement(array_index);
			}
			pV->SetReference(pVRef);
		break;
		case VAR_AREFERENCE:
			pA = null;
			var_index = ReadVDword();
			if(var_index == 0xffffffff) break;
			array_index = ReadVDword();
			pString = ReadString();
			if(bSkipVariable) 
			{
				if(pString) delete pString;
				break;
			}
			if(!VarTab.GetVarX(viRef,var_index)) 
			{
				if(pString) delete pString;
				SetError("State read error");
				return false;
			}
			pVRef = viRef.pDClass;
			if(array_index != 0xffffffff)
			{
				pVRef = pVRef->GetArrayElement(array_index);
			}
			
			if(pVRef->AttributesClass == 0) pVRef->AttributesClass = NEW ATTRIBUTES(&SCodec);
			if(pString)
			{
				pA = pVRef->AttributesClass->CreateSubAClass(pVRef->AttributesClass,pString);
				delete pString;
			}
			pV->SetAReference(pA);

		break;
	}
	return true;
}

void COMPILER::SaveVariable(DATA * pV, bool bdim)
{
	long   nLongValue;
	float  fFloatValue;
	char * pString;
	DWORD  var_index;
	DWORD  array_index;
	DWORD  n;
	ATTRIBUTES * pA;
	S_TOKEN_TYPE eType;
	ENTITY_ID eid;

	if(pV == 0)
	{
		SetError("Zero Variable Pointer");
		return;
	}
	if(!bdim)	// skip this info for array elements
	{
		eType = pV->GetType();
		SaveData(&eType, sizeof(eType));

		//n = pV->GetElementsNum();
		n = pV->Number_of_elements;
		if(n == 0) n = 1; // for reference variables
		SaveData(&n, sizeof(n));
	}

	if(pV->IsArray())
	{
		for(n=0;n<pV->GetElementsNum();n++)
		{
			//sprintf(gs,"    [index : %d]\n",n);
			//OutputDebugString(gs);
			SaveVariable(pV->GetArrayElement(n),true);
		}
		return;
	}
	
	switch(pV->GetType())
	{
		case VAR_INTEGER:
			pV->Get(nLongValue);
			SaveData(&nLongValue, sizeof(nLongValue));
		break;
		case VAR_FLOAT:
			pV->Get(fFloatValue);
			SaveData(&fFloatValue, sizeof(fFloatValue));
		break;
		case VAR_STRING:
			pV->Get(pString);
			SaveString(pString);
			/*pV->Get(pString);
			if(pString == 0) nlen = 0;
			else nlen = strlen(pString) + 1;
			//WriteVDword(nlen);
			SaveData(&nlen, sizeof(nlen));
			if(nlen) SaveData(pString, nlen);*/
		break;
		case VAR_OBJECT:
			pV->Get(eid);
			SaveData(&eid, sizeof(eid));
			SaveAttributesData(pV->AttributesClass);
		break;
		case VAR_REFERENCE:

			if(pV->GetReference()==0)
			{
				// uninitialized reference
				WriteVDword(0xffffffff);
				return;
			}
			if(!FindReferencedVariable(pV->GetVarPointer(),var_index,array_index))
			{
				SetError("Ghost reference");
				// reference to none
				WriteVDword(0xffffffff);
				return;
			}

			WriteVDword(var_index);
			WriteVDword(array_index);

		break;
		case VAR_AREFERENCE:
			pString = 0;
			try
			{
				pA = TraceARoot(pV->AttributesClass,pString);
			}
			catch(...)
			{
				SetError("Save - ARef to non existing attributes branch");
				WriteVDword(0xffffffff);
				break;
			}
			if(pA == 0)
			{
				WriteVDword(0xffffffff);
				if(pString) delete pString;
				break;
			}
			if(!FindReferencedVariableByRootA(pA,var_index,array_index))
			{
				SetError("Ghost A reference");
				WriteVDword(0xffffffff);
				if(pString) delete pString;
				break;
			}
			WriteVDword(var_index);
			WriteVDword(array_index);
			SaveString(pString);
			if(pString) delete pString;
		break;
	}
}

bool COMPILER::OnLoad()
{
	DWORD function_code;
	function_code = FuncTab.FindFunc("OnLoad");
	DATA * pResult;
	BC_Execute(function_code,pResult);
	return true;
}

bool COMPILER::SaveState(HANDLE fh)
{
	DWORD nVarNum,n;
	DWORD nSegNum;
	VARINFO vi;

	if (pBuffer) delete pBuffer;
	pBuffer = null;

	dwCurPointer = 0;
	dwMaxSize = 0;

	DWORD function_code;
	DATA * pResult;
	function_code = FuncTab.FindFunc("OnSave");
	if(function_code != INVALID_FUNC_CODE) 
		BC_Execute(function_code, pResult);

	EXTDATA_HEADER edh;
	VDATA* pVDat = (VDATA*)api->GetScriptVariable("savefile_info");
	if( pVDat && pVDat->GetString() )
		_snprintf(edh.sFileInfo,sizeof(edh.sFileInfo),"%s",pVDat->GetString());
	else _snprintf(edh.sFileInfo,sizeof(edh.sFileInfo),"save");
	edh.dwExtDataOffset = 0;
	edh.dwExtDataSize = 0;

	fio->_WriteFile(fh, &edh, sizeof(edh), null);
	
	// 1. Program Directory
	SaveString(ProgramDirectory);

	// 4. SCodec data
	WriteVDword(SCodec.GetNum());
	for(n = 0;n < SCodec.GetNum(); n++)
	{
		if(n == 0) SaveString(SCodec.Get());
		else SaveString(SCodec.GetNext());
	}

	nSegNum = 1;//SegmentsNum;
	// 2. Data Segments
	WriteVDword(nSegNum);

	// 3. Segments names
	for(n=0;n<nSegNum;n++)
	{
		if(SegmentTable[n].name)
		{
			SaveString(SegmentTable[n].name);
		}
	}

	// 5. Variables table
	nVarNum = VarTab.GetVarNum();
	WriteVDword(nVarNum);
	
	for(n=0;n<nVarNum;n++)
	{
		VarTab.GetVar(vi,n);
		SaveString(vi.name);	// ***
		SaveVariable(vi.pDClass);
	}


	dword dw2;
	if (dwCurPointer)
	{
		char * pDst = NEW char[dwCurPointer * 2];
		dword dwPackLen = dwCurPointer * 2;
		RDTSC_B(dw2);
		compress2((Bytef*)pDst, &dwPackLen, (Bytef*)pBuffer, dwCurPointer, Z_BEST_COMPRESSION);
		RDTSC_E(dw2);

		fio->_WriteFile(fh, &dwCurPointer, sizeof(dwCurPointer), null);
		fio->_WriteFile(fh, &dwPackLen, sizeof(dwPackLen), null);
		fio->_WriteFile(fh, pDst, dwPackLen, null);
		
		delete pDst;
	}

	if (pBuffer) delete pBuffer;
	pBuffer = null;

	return true;
}

bool COMPILER::LoadState(HANDLE fh)
{
	DWORD n;
	DWORD nVarNum;
	DWORD nSCStringsNum;
	char * pString;

	if(fh == INVALID_HANDLE_VALUE) return false;
	//hSaveFileFileHandle = fh;

	if (pBuffer) delete pBuffer;
	pBuffer = null;

	EXTDATA_HEADER exdh;
	fio->_ReadFile(fh, &exdh, sizeof(exdh), null);

	dword dwPackLen;
	fio->_ReadFile(fh, &dwMaxSize, sizeof(dwMaxSize), null);
	fio->_ReadFile(fh, &dwPackLen, sizeof(dwPackLen), null);
	if( dwPackLen==0 || dwPackLen>0x8000000 ||
		dwMaxSize==0 || dwMaxSize>0x8000000 )
			return false;
	char * pCBuffer = NEW char[dwPackLen];
	pBuffer = NEW char[dwMaxSize];
	fio->_ReadFile(fh, (void*)pCBuffer, dwPackLen, null);
	uncompress((Bytef*)pBuffer, &dwMaxSize, (Bytef*)pCBuffer, dwPackLen);
	delete pCBuffer;
	dwCurPointer = 0;

	// Release all data
	Release();

	// save specific data (name, time, etc)
	//DWORD nSDataSize = ReadVDword();
	//if(nSDataSize) ReadData(0,nSDataSize);

	// skip ext data header
	//EXTDATA_HEADER edh;
	//ReadData(&edh, sizeof(edh));


	// 1. Program Directory
	ProgramDirectory = ReadString();

	// 4. SCodec data
	nSCStringsNum = ReadVDword();
	for(n = 0;n < nSCStringsNum; n++)
	{
		pString = ReadString();
		if(pString)
		{
			SCodec.Convert(pString);
			delete pString;
		}

	}


	// 2. Data Segments
	DWORD nSegments2Load;
	nSegments2Load = ReadVDword();

	// 3.  Segments names
	// 3.a Initialize internal functions
	// 3.b Load preprocess
	InitInternalFunctions();
	LoadPreprocess();


	for(n=0;n<nSegments2Load;n++)
	{
		char * pSegmentName;
		pSegmentName = ReadString();
		if(!BC_LoadSegment(pSegmentName)) return false;
		if(pSegmentName) delete pSegmentName;
	}



	// 5. Variables table, all variables created during previous step, just read value
	nVarNum = ReadVDword();
	for(n=0;n<nVarNum;n++)
	{
		pString = ReadString();
		if(pString == 0) 
		{
			SetError("missing variable name");
			return false;
		}
		ReadVariable(pString/*,n*/);
		delete pString;
	}

	// call to script function "OnLoad()"
	OnLoad();

	if (pBuffer) delete pBuffer;
	pBuffer = null;

	return true;
}

void COMPILER::ReadAttributesData(ATTRIBUTES * pRoot, ATTRIBUTES * pParent)
{
	DWORD nSubClassesNum;
	DWORD n;
	DWORD nNameCode;
	//char * pName;
	char * pValue;

	if(pRoot == 0)
	{
		nSubClassesNum = ReadVDword();
		nNameCode = ReadVDword();

		//DTrace(SCodec.Convert(nNameCode));
		pValue = ReadString();
		pParent->SetAttribute(nNameCode,pValue);
		pRoot = pParent->GetAttributeClassByCode(nNameCode);
		if(pValue) delete pValue;
		for(n=0;n<nSubClassesNum;n++)
		{
			ReadAttributesData(0,pRoot);
		}

		return;
	}

	nSubClassesNum = ReadVDword();
	nNameCode = ReadVDword();
	pValue = ReadString();
	//pRoot->SetAttribute(nNameCode,pValue);

	pRoot->SetNameCode(nNameCode);
	pRoot->SetValue(pValue);

	for(n=0;n<nSubClassesNum;n++)
	{
		//ReadAttributesData(pRoot->GetAttributeClass(n));
		ReadAttributesData(0,pRoot);
	}


	//if(pName) delete pName;
	if(pValue) delete pValue;

}

void COMPILER::SaveAttributesData(ATTRIBUTES * pRoot)
{
	DWORD n;
	if(pRoot == 0) 
	{
		WriteVDword(0);	// number of subclasses
		SaveString(0);	// attribute name
		SaveString(0);	// attribute value
		return;
	}
	WriteVDword(pRoot->GetAttributesNum());	// number of subclasses

	// save attribute name
	//SaveString(pRoot->GetThisName());
	WriteVDword(pRoot->GetThisNameCode());

	// save attribute value
	SaveString(pRoot->GetThisAttr());
	for(n=0;n<pRoot->GetAttributesNum();n++)
	{
		SaveAttributesData(pRoot->GetAttributeClass(n));
	}
}

void COMPILER::AddPostEvent(S_EVENTMSG * pEM)
{
	EventMsg.Add(pEM);
}


bool COMPILER::SetSaveData(char * file_name, void * save_data, long data_size)
{
	EXTDATA_HEADER exdh;
	DWORD dwFileSize;

	fio->SetDrive(XBOXDRIVE_NONE);
	HANDLE fh = Core.fio->_CreateFile(file_name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING);
	fio->SetDrive();
	if (fh == INVALID_HANDLE_VALUE) return false;

	dwFileSize = Core.fio->_GetFileSize(fh, 0);
	VDATA* pVDat = (VDATA*)api->GetScriptVariable("savefile_info");
	if( pVDat && pVDat->GetString() )
		_snprintf(exdh.sFileInfo,sizeof(exdh.sFileInfo),"%s",pVDat->GetString());
	else _snprintf(exdh.sFileInfo,sizeof(exdh.sFileInfo),"save");
	exdh.dwExtDataOffset = dwFileSize;
	exdh.dwExtDataSize = data_size;

	fio->_WriteFile(fh, &exdh, sizeof(exdh), null);
	fio->_SetFilePointer(fh, dwFileSize, 0, FILE_BEGIN);

	char * pDst = NEW char[data_size * 2];
	dword dwPackLen = data_size * 2;
	compress2((Bytef*)pDst, &dwPackLen, (Bytef*)save_data, data_size, Z_BEST_COMPRESSION);

	fio->_WriteFile(fh, &dwPackLen, sizeof(dwPackLen), null);
	fio->_WriteFile(fh, pDst, dwPackLen, null);
	fio->_CloseHandle(fh);

	delete pDst;

	return true;
}

/*bool COMPILER::SetSaveData(char * file_name, void * save_data, long data_size)
						   //const char * file_name, const char * save_data)
{
	EXTDATA_HEADER exdh;
	DWORD dwFileSize;
	DWORD dwOrgDataSize;
	char * pOrgData;

	char sFileName[MAX_PATH];

	if(file_name == 0)
	{
		SetError("invalid save file name");
		return false;
	}
	
#ifdef _XBOX
	DWORD n;
	char sTemp[MAX_PATH];
	XGAME_FIND_DATA fd;
	HANDLE sh;

	if(file_name[0] == 'U' && file_name[1] ==':')
	{
		strcpy(sFileName,file_name);
	}
	else
	{
		bool bFound = false;
		sh = XFindFirstSaveGame("U:\\",&fd);
		if(INVALID_HANDLE_VALUE == sh) return false;
		for(n=0;fd.szSaveGameName[n];n++) sTemp[n] = (char)fd.szSaveGameName[n];	sTemp[n] = 0;
		sprintf(sFileName,"%s%s",fd.szSaveGameDirectory,sTemp);
		if(stricmp(sTemp,file_name)==0) bFound = true;
		while(!bFound)
		{
			if(!XFindNextSaveGame(sh,&fd)) break;

			for(n=0;fd.szSaveGameName[n];n++) sTemp[n] = (char)fd.szSaveGameName[n];	sTemp[n] = 0;
			sprintf(sFileName,"%s%s",fd.szSaveGameDirectory,sTemp);
			if(stricmp(sTemp,file_name)==0) bFound = true;
		}
		if(!bFound) return false;
		XFindClose(sh);
	}
#else
	strcpy(sFileName,file_name);
#endif

	// open save file
	fio->SetDrive(XBOXDRIVE_NONE);
	HANDLE fh = Core.fio->_CreateFile(sFileName,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
	fio->SetDrive();
	if(fh == INVALID_HANDLE_VALUE) return false;

	// get file size
	dwFileSize = Core.fio->_GetFileSize(fh,0);

	// set global handle
//	hSaveFileFileHandle = fh;

	// read save header
	ReadData(&exdh,sizeof(exdh));

	// calc org data size
	if(exdh.dwExtDataSize != 0)
	{
		dwOrgDataSize = dwFileSize - sizeof(exdh) - exdh.dwExtDataSize;
	}
	else
	{
		dwOrgDataSize = dwFileSize - sizeof(exdh);
	}

	// prepare org data buffer
	pOrgData = NEW char[dwOrgDataSize];
	if(pOrgData == 0) 
	{
		SetError("no memory");
		Core.fio->_CloseHandle(fh);
		return false;
	}

	// buffering org data
	ReadData(pOrgData,dwOrgDataSize);

	Core.fio->_CloseHandle(fh);

	// start flushing data to file
	fio->SetDrive(XBOXDRIVE_NONE);
	fh = Core.fio->_CreateFile(sFileName,GENERIC_WRITE,FILE_SHARE_READ,OPEN_EXISTING);
	fio->SetDrive();
	if(fh == INVALID_HANDLE_VALUE) 
	{
		if(pOrgData) delete pOrgData;
		SetError("cant set save ext data");
		return false;
	}

	exdh.dwExtDataOffset = sizeof(exdh) + dwOrgDataSize;
	exdh.dwExtDataSize = data_size;


#ifdef _XBOX
	VSoundService * pSound = (VSoundService *)api->CreateService("soundservice");
	if(pSound)
	{
		for(n=0;n<SBUPDATE;n++)
		{
			pSound->RunStart();
			pSound->RunEnd();
		}
	}

	XCALCSIG_SIGNATURE xsig;
	HANDLE hSig;
	memset(&xsig,0,sizeof(xsig));

	hSig = XCalculateSignatureBegin( 0 );

	// save header
	if(hSig != INVALID_HANDLE_VALUE) XCalculateSignatureUpdate(hSig,(BYTE *)&exdh,sizeof(exdh));
	SaveData(&exdh,sizeof(exdh));

	// save org data
	if(hSig != INVALID_HANDLE_VALUE) XCalculateSignatureUpdate(hSig,(BYTE *)pOrgData,dwOrgDataSize);
	SaveData(pOrgData,dwOrgDataSize);

	// save ext data
	if(hSig != INVALID_HANDLE_VALUE) XCalculateSignatureUpdate(hSig,(BYTE *)save_data,data_size);
	SaveData(save_data,data_size);

	if(hSig != INVALID_HANDLE_VALUE) XCalculateSignatureEnd(hSig,&xsig);

	// save signature
	SaveData(&xsig,sizeof(xsig));
	
#else
	// save header
	SaveData(&exdh,sizeof(exdh));
	// save org data
	SaveData(pOrgData,dwOrgDataSize);
	// save ext data
	SaveData(save_data,data_size);

#endif

	// cleanup
	Core.fio->_CloseHandle(fh);
	if(pOrgData) delete pOrgData;
	return true;
	

}*/

void * COMPILER::GetSaveData(char * file_name, long & data_size)
{
	fio->SetDrive(XBOXDRIVE_NONE);
	HANDLE fh = fio->_CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
	fio->SetDrive();
	if (fh == INVALID_HANDLE_VALUE) 
	{
		SetError("cant open save file: %s",file_name);
		return 0;
	}

	DWORD dwHWFileSize;
	if( fio->_GetFileSize(fh,&dwHWFileSize) < sizeof(EXTDATA_HEADER) + sizeof(dword) ) {
		data_size = 0;
		fio->_CloseHandle(fh);
		return 0;
	}

	dword dw2;
	RDTSC_B(dw2);
	EXTDATA_HEADER exdh;
	fio->_SetFilePointer(fh, 0, 0, FILE_BEGIN);
	fio->_ReadFile(fh, &exdh, sizeof(exdh), null);
	if( exdh.dwExtDataSize <=0 ) {
		data_size = 0;
		fio->_CloseHandle(fh);
		return 0;
	}

	dword dwPackLen;
	fio->_SetFilePointer(fh, exdh.dwExtDataOffset, 0, FILE_BEGIN);
	fio->_ReadFile(fh, &dwPackLen, sizeof(dwPackLen), null);
	if( dwPackLen==0 || dwPackLen>0x8000000 ) {
		data_size = 0;
		fio->_CloseHandle(fh);
		return 0;
	}
	char * pCBuffer = NEW char[dwPackLen];
	fio->_ReadFile(fh, pCBuffer, dwPackLen, null);
	char * pBuffer = NEW char[exdh.dwExtDataSize];
	dword dwDestLen = exdh.dwExtDataSize;
	uncompress((Bytef*)pBuffer, &dwDestLen, (Bytef*)pCBuffer, dwPackLen);
	fio->_CloseHandle(fh);
	delete pCBuffer;
	RDTSC_E(dw2);
	//api->Trace("GetSaveData = %d", dw2);

	data_size = dwDestLen;
	return pBuffer;
}

/*void * COMPILER::GetSaveData(char * file_name, long & data_size)
{
	DWORD n;
#ifdef _XBOX
	XCALCSIG_SIGNATURE xsig;
	XCALCSIG_SIGNATURE save_xsig;
	HANDLE hSig;
	memset(&xsig,0,sizeof(xsig));
	hSig = XCalculateSignatureBegin( 0 );

	VSoundService * pSound = (VSoundService *)api->CreateService("soundservice");
	if(pSound)
	{
		for(n=0;n<SBUPDATE;n++)
		{
			pSound->RunStart();
			pSound->RunEnd();
		}
	}

#endif

	EXTDATA_HEADER exdh;
	char * pExtData;
	if(file_name == 0)
	{
		SetError("invalid save file name");
		return 0;
	}

	// open save file
	fio->SetDrive(XBOXDRIVE_NONE);
	HANDLE fh = Core.fio->_CreateFile(file_name,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
	fio->SetDrive();
	if(fh == INVALID_HANDLE_VALUE) 
	{
		SetError("cant open save file: %s",file_name);
		return 0;
	}

	// set global handle
//	hSaveFileFileHandle = fh;

	// read save header

	memset(&exdh,0,sizeof(exdh));
	if(!ReadData(&exdh,sizeof(exdh)))
	{
		Core.fio->_CloseHandle(fh);
		return 0;
	}
#ifdef _XBOX
	if(hSig != INVALID_HANDLE_VALUE) XCalculateSignatureUpdate(hSig,(BYTE *)&exdh,sizeof(exdh));
#endif

	int fsize = Core.fio->_GetFileSize(fh,0);
	if( fsize < exdh.dwExtDataOffset+exdh.dwExtDataSize )
	{ // extern data header is failed
		Core.fio->_CloseHandle(fh);
		return 0;
	}

	// prepare ext data buffer
	pExtData = NEW char[exdh.dwExtDataSize];
	if(pExtData == 0) 
	{
		SetError("no memory");
		Core.fio->_CloseHandle(fh);
		return 0;
	}

#ifdef _XBOX

	// calculate signature for xbox save
	char * pOrgData;
	DWORD dwOrgDataSize;

	dwOrgDataSize = exdh.dwExtDataOffset - sizeof(exdh);
	pOrgData = NEW char[dwOrgDataSize];
	if(pOrgData == 0) 
	{
		SetError("no memory");
		if(pExtData) delete pExtData;
		Core.fio->_CloseHandle(fh);
		return 0;
	}
	ReadData(pOrgData,dwOrgDataSize);
	if(hSig != INVALID_HANDLE_VALUE) XCalculateSignatureUpdate(hSig,(BYTE *)pOrgData,dwOrgDataSize);
	delete pOrgData;
#else
	// move to ext data
	DWORD dwRes = Core.fio->_SetFilePointer(fh,exdh.dwExtDataOffset,0,FILE_BEGIN);
	if(dwRes == 0xffffffff)
	{
		if(pExtData) delete pExtData; pExtData = 0;
		SetError("invalid seek");
		return 0;
	}
#endif

	// read ext data
	ReadData(pExtData,exdh.dwExtDataSize);

#ifdef _XBOX
	if(hSig != INVALID_HANDLE_VALUE) XCalculateSignatureUpdate(hSig,(BYTE *)pExtData,exdh.dwExtDataSize);

	if(hSig != INVALID_HANDLE_VALUE) XCalculateSignatureEnd(hSig,&xsig);

	ReadData(&save_xsig,sizeof(save_xsig));

	if(memcmp(&xsig,&save_xsig,sizeof(xsig))!=0)
	{
		// save data broken
		Core.fio->_CloseHandle(fh);
		SetError("save signature check failed");
		delete pExtData;
		data_size = 0;
		return 0;
	}
	
#endif


	// cleanup
	Core.fio->_CloseHandle(fh);

	data_size = exdh.dwExtDataSize;
	return pExtData;
}*/

void COMPILER::AddRuntimeEvent()
{
	if(!bRuntimeLog) return;
	nRuntimeLogEventsNum++;
	if(nRuntimeLogEventsNum >= nRuntimeLogEventsBufferSize)
	{
		if(nRuntimeLogEventsBufferSize == 0) nRuntimeLogEventsBufferSize = 180;
		else nRuntimeLogEventsBufferSize *= 2;
		pRuntimeLogEvent = (DWORD *)RESIZE(pRuntimeLogEvent,nRuntimeLogEventsBufferSize*sizeof(DWORD));
	}
	pRuntimeLogEvent[nRuntimeLogEventsNum - 1] = nRuntimeTicks;
	nRuntimeTicks = 0;

}

DWORD COMPILER::SetScriptFunction(IFUNCINFO * pFuncInfo)
{
	FUNCINFO fi;
	S_TOKEN_TYPE TokenType;
	DWORD nFuncHandle;

	if(pFuncInfo->pFuncName == 0)
	{
		SetError("Invalid function name in SetScriptFunction(...)");
		return INVALID_FUNCHANDLE;
	}

	nFuncHandle = FuncTab.FindFunc(pFuncInfo->pFuncName);
	if(nFuncHandle != INVALID_FUNC_CODE)
	{
		FuncTab.GetFuncX(fi,nFuncHandle);
		if(fi.arguments != pFuncInfo->nArguments)
		{
			SetError("arguments num mismatch in function '%s'",pFuncInfo->pFuncName);
			return INVALID_FUNCHANDLE;
		}
	} 
	else
	{
		SetError("native function '%s' not declared",pFuncInfo->pFuncName);
		return INVALID_FUNCHANDLE;

	}

	
	fi.name = pFuncInfo->pFuncName;
	fi.decl_file_name = pFuncInfo->pDeclFileName;
	if(fi.decl_file_name == 0) fi.decl_file_name = "unknown";
	fi.decl_line = pFuncInfo->nDeclLine;
	fi.pImportedFunc = pFuncInfo->pFuncAddress;
	fi.arguments = pFuncInfo->nArguments;
	fi.var_num = 0;
	fi.segment_id = IMPORTED_SEGMENT_ID;
	fi.offset = INVALID_FUNC_OFFSET;
	fi.stack_offset = 0xffffffff;

	if(pFuncInfo->pReturnValueName == 0) fi.return_type = TVOID;
	else
	{
		TokenType = Token.Keyword2TokenType(pFuncInfo->pReturnValueName);
		switch(TokenType)
		{
			case TVOID:
			case VAR_INTEGER:
			case VAR_FLOAT:
			case VAR_STRING:
			case VAR_OBJECT:
			case VAR_REFERENCE:
			case VAR_AREFERENCE:
				fi.return_type = TokenType;
			break;
			default: 
				SetError("Invalid import function '%s' return type '%s'",
					pFuncInfo->pFuncName,pFuncInfo->pReturnValueName);
			return INVALID_FUNCHANDLE;
		}
	}
	

	DWORD funch;
	funch = FuncTab.AddFunc(fi);
	return funch;
}

void COMPILER::DeleteScriptFunction(DWORD nFuncHandle)
{
	FuncTab.InvalidateFunction(nFuncHandle);
}

DATA * COMPILER::GetOperand(char * pCodeBase, DWORD & ip, S_TOKEN_TYPE * pTokenType)
{
	DWORD token_data_size;
	S_TOKEN_TYPE sttResult;
	VARINFO vi;
	DATA * pVar;

	sttResult = BC_TokenGet(ip,token_data_size);
	if(pTokenType) *pTokenType = sttResult;
	switch(sttResult)
	{
		case STACK_TOP:
			pVar = SStack.Read();
		return pVar;
		case VARIABLE:
			if(!VarTab.GetVar(vi,*((dword *)&pCodeBase[ip])))	{ SetError("Global variable not found"); break; }
			if(!vi.pDClass) { SetError("invalid global variable"); break; }
		return vi.pDClass;
		case LOCAL_VARIABLE:
			pVar = SStack.Read(pRun_fi->stack_offset,*((dword *)&pCodeBase[ip]));
			if(pVar == 0) { SetError("Local variable not found"); return false; }
		return pVar;
		case AX: return &rAX;
		case BX: return &rBX;
		case EX: return &ExpressionResult;
		case AP:
		return 0;
		default:
			SetError("invalid operand");
		return 0;
	}
	return 0;
}

void COMPILER::FormatAllDialog(char * directory_name)
{
	HANDLE fh;
	WIN32_FIND_DATA ffd;
	char sFileName[MAX_PATH];
	wsprintf(sFileName,"%s\\*.c",directory_name);
	fh = Core.fio->_FindFirstFile(sFileName,&ffd);
	if(fh != INVALID_HANDLE_VALUE)
	{
		wsprintf(sFileName,"%s\\%s",directory_name,ffd.cFileName);
		FormatDialog(sFileName);
		while(Core.fio->_FindNextFile(fh,&ffd))
		{
			wsprintf(sFileName,"%s\\%s",directory_name,ffd.cFileName);
			FormatDialog(sFileName);
		}
	}
}

void COMPILER::FormatDialog(char * file_name)
{
	DWORD FileSize;
	char * pFileData;
	TOKEN Token;
	S_TOKEN_TYPE Token_type;
	char sFileName[MAX_PATH];
	char buffer[MAX_PATH];
	char sNewLine[] = {0xd,0xa,0};
	HANDLE fh;
	HANDLE fhH;
	DWORD dwR;
	DWORD nTxt,nLnk;
	bool bExportString;
	
	if(file_name == 0) return;

	nTxt = 0;
	nLnk = 0;
	
	//wsprintf(sFileName,"PROGRAM\\%sc",file_name);
	strcpy(sFileName,file_name);


	pFileData = LoadFile(file_name,FileSize,true);
	if(pFileData == 0) { return;}

	fh = Core.fio->_CreateFile(sFileName,GENERIC_WRITE,FILE_SHARE_READ,CREATE_ALWAYS);
	if(fh == INVALID_HANDLE_VALUE) return;

	//wsprintf(sFileName,"PROGRAM\\%s",file_name);
	strcpy(sFileName,file_name);
	sFileName[strlen(sFileName)-1] = 0;
	strcat(sFileName,"h");
	fhH = Core.fio->_CreateFile(sFileName,GENERIC_WRITE,FILE_SHARE_READ,CREATE_ALWAYS);
	if(fhH == INVALID_HANDLE_VALUE) {Core.fio->_CloseHandle(fh); delete pFileData; return;}


	//pFileData = LoadFile(file_name,FileSize,true);
	//if(pFileData == 0) {Core.fio->_CloseHandle(fh); Core.fio->_CloseHandle(fhH); return;}

	DWORD nFullNameLen,n;
	nFullNameLen = strlen(file_name);
	for(n=nFullNameLen;n>0;n--)
	{
		if(file_name[n] == '\\') break;
	}
	wsprintf(sFileName,"DIALOGS%s",(char *)(file_name + n));
	sFileName[strlen(sFileName)-1] = 0;
	strcat(sFileName,"h");
	wsprintf(buffer,"#include \"%s\"",sFileName);

	Core.fio->_WriteFile(fh,buffer,strlen(buffer),&dwR);
	Core.fio->_WriteFile(fh,sNewLine,strlen(sNewLine),&dwR);


	wsprintf(buffer,"string DLG_TEXT[0] = {        ");
	Core.fio->_WriteFile(fhH,buffer,strlen(buffer),&dwR);
	Core.fio->_WriteFile(fhH,sNewLine,strlen(sNewLine),&dwR);

	
	Token.SetProgram(pFileData,pFileData);

	do
	{
		Token_type = Token.FormatGet();
		switch(Token_type)
		{
		case DOT:
			Core.fio->_WriteFile(fh,Token.GetData(),strlen(Token.GetData()),&dwR);
			Token_type = Token.FormatGet();
			if(Token_type != OPEN_BRACKET)
			{
				if(Token.GetData())
				{
					// node text --------------------------------------------
					if(stricmp(Token.GetData(),"text")== 0)
					{
						Core.fio->_WriteFile(fh,Token.GetData(),strlen(Token.GetData()),&dwR);

						//Core.fio->_WriteFile(fhH,sNewLine,strlen(sNewLine),&dwR);
						//wsprintf(sFileName,"// [NODE START] ",nTxt); 
						//Core.fio->_WriteFile(fhH,sFileName,strlen(sFileName),&dwR);
						//Core.fio->_WriteFile(fhH,sNewLine,strlen(sNewLine),&dwR);

						do
						{
							Token_type = Token.FormatGet();
							if(Token_type == STRING)
							{
								bExportString = true;
								if(strlen(Token.GetData()) == 3 && Token.GetData()[1] <= 0x39)
								{
									switch(Token.GetData()[1])
									{
									case '!':
									case ',':
									case '.':
									case '?':
									case '-':
									case ':':
										bExportString = true;
									break;
									default:
										bExportString = false;
									break;
									}
								}
								
								//if(strlen(Token.GetData()) == 3 && Token.GetData()[1] <= 0x39)
								if(!bExportString)
								{
									Core.fio->_WriteFile(fh,Token.GetData(),strlen(Token.GetData()),&dwR);
								}
								else
								{
									Core.fio->_WriteFile(fhH,Token.GetData(),strlen(Token.GetData()),&dwR);
									Core.fio->_WriteFile(fhH,",",strlen(","),&dwR);
									Core.fio->_WriteFile(fhH,sNewLine,strlen(sNewLine),&dwR);
									wsprintf(sFileName,"DLG_TEXT[%d]",nTxt); 
									Core.fio->_WriteFile(fh,sFileName,strlen(sFileName),&dwR);
									nTxt++;
								
								}
								
							} else
							{
								Core.fio->_WriteFile(fh,Token.GetData(),strlen(Token.GetData()),&dwR);
							}
							if(Token_type == END_OF_PROGRAMM) break;
						} while(Token_type != SEPARATOR);
						break;
					}
				}
			}
			if(Token.GetData())
			{
				Core.fio->_WriteFile(fh,Token.GetData(),strlen(Token.GetData()),&dwR);
			}
		break;
		case UNKNOWN:
			if(Token.GetData())
			{
				Core.fio->_WriteFile(fh,Token.GetData(),strlen(Token.GetData()),&dwR);
				if(stricmp(Token.GetData(),"link")==0)
				{
					Token_type = Token.FormatGet();
					Core.fio->_WriteFile(fh,Token.GetData(),strlen(Token.GetData()),&dwR);
					if(Token_type == DOT)
					{
						Token_type = Token.FormatGet();
						if(Token.GetData())
						{
							Core.fio->_WriteFile(fh,Token.GetData(),strlen(Token.GetData()),&dwR);
						}
						if(Token_type == UNKNOWN)
						{
							Token_type = Token.FormatGet();
							if(Token.GetData())
							{
								Core.fio->_WriteFile(fh,Token.GetData(),strlen(Token.GetData()),&dwR);
							}
							if(Token_type != DOT)
							{
								do
								{
									Token_type = Token.FormatGet();
									if(Token_type == STRING)
									{
										bExportString = true;
										if(strlen(Token.GetData()) == 3 && Token.GetData()[1] <= 0x39)
										{
											switch(Token.GetData()[1])
											{
											case '!':
											case ',':
											case '.':
											case '?':
											case '-':
											case ':':
												bExportString = true;
											break;
											default:
												bExportString = false;
											break;
											}
										}
										
										//if(strlen(Token.GetData()) == 3 && Token.GetData()[1] <= 0x39)
										if(!bExportString)
										{
											Core.fio->_WriteFile(fh,Token.GetData(),strlen(Token.GetData()),&dwR);
										}
										else
										{
											Core.fio->_WriteFile(fhH,Token.GetData(),strlen(Token.GetData()),&dwR);
											Core.fio->_WriteFile(fhH,",",strlen(","),&dwR);
											Core.fio->_WriteFile(fhH,sNewLine,strlen(sNewLine),&dwR);
											wsprintf(sFileName,"DLG_TEXT[%d]",nTxt); 
											Core.fio->_WriteFile(fh,sFileName,strlen(sFileName),&dwR);
											nTxt++;
										}

									} else
									{
										Core.fio->_WriteFile(fh,Token.GetData(),strlen(Token.GetData()),&dwR);
									}
									if(Token_type == END_OF_PROGRAMM) break;
								} while(Token_type != SEPARATOR);
								break;


							}

						}
						

					}
					
				}
			}
		break;
		default: 
			if(Token.GetData())
			{
				Core.fio->_WriteFile(fh,Token.GetData(),strlen(Token.GetData()),&dwR);
			}
		break;
		}

	} while(Token_type != END_OF_PROGRAMM);

	delete pFileData;

	wsprintf(buffer,"};");
	Core.fio->_WriteFile(fhH,sNewLine,strlen(sNewLine),&dwR);
	Core.fio->_WriteFile(fhH,buffer,strlen(buffer),&dwR);
	Core.fio->_SetFilePointer(fhH,0,0,FILE_BEGIN);
	wsprintf(buffer,"string DLG_TEXT[%d] = {",nTxt);
	Core.fio->_WriteFile(fhH,buffer,strlen(buffer),&dwR);
	
	Core.fio->_CloseHandle(fhH);
	Core.fio->_CloseHandle(fh);
}

void STRING_CODEC::VariableChanged()
{
	CDebug.SetTraceMode(TMODE_MAKESTEP);
}
