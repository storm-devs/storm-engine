#ifndef _COMPILER_H_
#define _COMPILER_H_

#include "..\..\common_h\message.h"
#include "..\..\common_h\script_libriary.h"
#include "message_script.h"
#include "token.h"
#include "data.h"
#include "loopstack.h"
#include "stack.h"
#include "strings_list.h"
#include "s_functab.h"
#include "s_vartab.h"
#include "s_classtab.h"
#include "s_deftab.h"
#include "s_stack.h"
#include "s_tstack.h"
#include "s_eventtab.h"
#include "tclass_list.h"
#include "s_eventmsg.h"
#include "string_codec.h"
#include "s_compress.h"
#include "s_postevents.h"

#define COMPILER_LOG_FILENAME		"compile.log"
#define COMPILER_ERRORLOG_FILENAME	"error.log"
#define BCODE_BUFFER_BLOCKSIZE		4096
#define IOBUFFER_SIZE			65535


struct SEGMENT_DESC
{
	char * name;
	DWORD  offset;
	DWORD  size;
	DWORD  id;
	DWORD  lines;
	bool   bUnload;
	//--------------
	char * pData;
	char * pCode;
	DWORD BCode_Program_size;
	DWORD BCode_Buffer_size;
 
	STRINGS_LIST * Files_list;
};

struct OFFSET_INFO
{
	DWORD offset;
	DWORD segment_id;
};

struct EXTDATA_HEADER
{
	char sFileInfo[32];
	DWORD dwExtDataOffset;
	DWORD dwExtDataSize;
};
/*struct DEFINE_INFO
{
	dword deftype;
	dword data4b;
};*/

struct DOUBLE_DWORD
{
	DWORD dw1;
	DWORD dw2;
};

class SLIBHOLDER
{
public:
	HINSTANCE hInst;
	SCRIPT_LIBRIARY * pLib;
	char * pName;
	SLIBHOLDER(){pLib = 0;pName = 0;};
	~SLIBHOLDER(){if(pLib) delete pLib; if(pName) delete pName;};
	void SetName(const char * pFileName) {if(pName) delete pName; pName = NEW char[strlen(pFileName)+1];strcpy(pName,pFileName);}
};

#ifndef _XBOX
class S_DEBUG;
#endif

enum COMPILER_STAGE
{
	CS_SYSTEM,
	CS_COMPILATION,
	CS_RUNTIME
};
class CORE;
class COMPILER : public VIRTUAL_COMPILER
{
	friend CORE; 
#ifndef _XBOX
	friend S_DEBUG;
#endif
	COMPILER_STAGE CompilerStage;
	STRINGS_LIST LabelTable;
	//STRINGS_LIST EventTable;
	STRINGS_LIST LabelUpdateTable;
	TOKEN	Token;
	MESSAGE * pEventMessage;
	SEGMENT_DESC * SegmentTable;
	DWORD SegmentsNum;
	DWORD RunningSegmentID;
	DWORD InstructionPointer;

	char * pBuffer;
	dword dwCurPointer, dwMaxSize;

	char * ProgramDirectory;
	bool bCompleted;
	bool bEntityUpdate;
	char * pDebExpBuffer;
	DWORD nDebExpBufferSize;

	FUNCINFO *	pRun_fi;	// running function info
	S_FUNCTAB	FuncTab;
	S_VARTAB	VarTab;
	S_CLASSTAB  ClassTab;
	S_DEFTAB	DefTab;
	S_STACK		SStack;
	S_EVENTTAB	EventTab;
	//TCLASS_LIST<S_EVENTMSG> EventMsg;
	POSTEVENTS_LIST EventMsg;
	TCLASS_LIST<SLIBHOLDER> LibriaryFuncs;
	
	STRING_CODEC SCodec;
	
	bool bRuntimeLog;
	DWORD nRuntimeLogEventsBufferSize;
	DWORD nRuntimeLogEventsNum;
	DWORD * pRuntimeLogEvent;
	DWORD nRuntimeTicks;

	bool bFirstRun;
	bool bScriptTrace;
	bool bWriteCodeFile;
	bool bDebugInfo;
	char DebugSourceFileName[MAX_PATH];
	char gs[MAX_PATH];
	DWORD DebugSourceLine;
	char * pCompileTokenTempBuffer;

	//HANDLE hSaveFileFileHandle;

	bool bDebugExpressionRun;
	bool bTraceMode;
	
	bool bEventsBreak;

	char DebugTraceFileName[MAX_PATH];
	DWORD nDebugTraceLineCode;

	DWORD nIOBufferSize;
	DWORD nIOFullSize;
	char * pIOBuffer;

	// script registers
	DATA rAX;
	DATA rBX;
	DATA ExpressionResult;
	ATTRIBUTES * rAP;

public:

	bool bBreakOnError;
	COMPRESS Compress;

	 COMPILER();
	~COMPILER();

	VSTRING_CODEC * GetVSC(){return &SCodec;}

	char *  LoadFile(char * file_name, DWORD & file_size, bool bFullPath = false);
	//char *	AppendProgram(char * base_program, long base_program_size, char * append_program, long append_program_size, long& new_program_size);
	bool	AppendProgram(char * & pBase_program, DWORD & Base_program_size, char * pAppend_program, DWORD & Append_program_size, bool bAddLinefeed);
	void	Trace(char * data_PTR, ...);
	void	DTrace(char * data_PTR, ...);
	void	SetError(char * data_PTR, ...);
	void	SetWarning(char * data_PTR, ...);
	bool    CreateProgram(char * file_name);
	bool	AddProgramFile(char * file_name);
	
	void	UnloadSegment(char * segment_name);
	DWORD	GetSegmentIndex(DWORD segment_id);


	void	ProcessFrame(DWORD DeltaTime);
	bool	__declspec(dllexport) __cdecl Run();
	void	Release();
	void	__declspec(dllexport) __cdecl SetProgramDirectory(char * dir_name);
	VDATA *	ProcessEvent(char * event_name, MESSAGE message);
	VDATA * ProcessEvent(char * event_name);
	void	SetEventHandler(char * event_name, char * func_name,long flag, bool bStatic = false);
	void	DelEventHandler(char * event_name, char * func_name);
	bool	Completed(){return bCompleted;}
	char *	GetName();
	void	ExitProgram();
	void	ClearEvents();

	
	DWORD  GetIntFunctionCode(char * func_name);
	DATA * BC_CallIntFunction(DWORD func_code,DATA * & pVResult,DWORD arguments);
	
	DWORD  GetIntFunctionsNum();
	bool   InitInternalFunctions();

	bool ProcessDebugExpression(char * pExpression, DATA & Result);
	bool SetOnDebugExpression(char * pLValue, char * pRValue, DATA & Result);
	bool ProcessDebugExpression0(char * pExpression, DATA & Result);
	bool Compile(SEGMENT_DESC& Segment, char * pInternalCode = 0, DWORD pInternalCodeSize = 0);
	void CompileToken(SEGMENT_DESC& Segment,S_TOKEN_TYPE Token_type, DWORD data_blocks_num = 0,...);
	void ResizeBCodeBuffer(SEGMENT_DESC& Segment,DWORD add_size);
	bool BC_LoadSegment(char * file_name);
	bool BC_SegmentIsLoaded(char * file_name);
	bool BC_Execute(DWORD function_code, DATA * & pVReturnResult, char * pDbgExpSource = 0);
	bool BC_CallFunction(DWORD func_code, DWORD & ip, DATA * & pVResult);
	void FindErrorSource();
	S_TOKEN_TYPE BC_TokenGet(DWORD & ip, DWORD & token_data_size);
	S_TOKEN_TYPE BC_TokenGet();

	S_TOKEN_TYPE TokenLastReadResult;
	DWORD TLR_DataOffset;
	char * pRunCodeBase;
	bool TokenIs(S_TOKEN_TYPE test);
	S_TOKEN_TYPE TokenType() {return TokenLastReadResult;}
	S_TOKEN_TYPE NextTokenType();
	S_TOKEN_TYPE SkipAuxiliaryTokens();

	bool BC_ProcessExpression(DATA * value);
	void BC_ProcessExpression_L0(DATA * value);
	void BC_ProcessExpression_L1(DATA * value, bool bSkip = false);
	void BC_ProcessExpression_L2(DATA * value, bool bSkip = false);
	void BC_ProcessExpression_L3(DATA * value, bool bSkip = false);
	void BC_ProcessExpression_L4(DATA * value, bool bSkip = false);
	void BC_ProcessExpression_L5(DATA * value, bool bSkip = false);
	void BC_ProcessExpression_L6(DATA * value, bool bSkip = false);
	void BC_ProcessExpression_L7(DATA * value, bool bSkip = false);

/*	void SkipExpression_L1();
	void SkipExpression_L2();
	void SkipExpression_L3();
	void SkipExpression_L4();
	void SkipExpression_L5();
	void SkipExpression_L6();
	void SkipExpression_L7();
*/
	//-----------------------------------------------------
	DWORD CurrentFuncCode;
	void CompileNumber(SEGMENT_DESC& Segment);
	void CompileFloatNumber(SEGMENT_DESC& Segment);
	void CompileString(SEGMENT_DESC& Segment);
	bool CompileUnknown(SEGMENT_DESC& Segment);
	bool CompileBlock(SEGMENT_DESC& Segment, bool & bFunctionBlock, DWORD & inout,S_TOKEN_TYPE bound_type, DWORD continue_jump, DWORD break_offset, STRINGS_LIST & BreakTable);
	S_TOKEN_TYPE CompileAuxiliaryTokens(SEGMENT_DESC& Segment);//, bool & bFunctionBlock, DWORD & inout);
	bool BC_Jump(SEGMENT_DESC& Segment, DWORD offset);
	void UpdateOffsets(SEGMENT_DESC& Segment, STRINGS_LIST & list, DWORD offset, char * sname = 0);
	S_TOKEN_TYPE DetectUnknown(DWORD & code);

	void DumpAttributes(ATTRIBUTES * pA, long level);

	bool IsIntFuncVarArgsNum(DWORD code);
	DWORD GetInternalFunctionArgumentsNum(DWORD code);
	bool CreateMessage(MESSAGE_SCRIPT * pMs, DWORD stack_offset, DWORD vindex, bool s2s = false);
	void ProcessEvent(char * event_name, MESSAGE * pMs);

	bool SaveState(HANDLE fh);
	bool LoadState(HANDLE fh);
	bool OnLoad();
	void SaveDataDebug(char * data_PTR, ...);
	void SaveData(const void * data_PTR, DWORD data_size);
	bool ReadData(void * data_PTR, DWORD data_size);
	void SaveString(const char * pS);
	char * ReadString();


	void SaveVariable(DATA * pV, bool bdim = false);
	bool ReadVariable(char * name, /*DWORD code,*/ bool bdim = false, DWORD a_index = 0);
	bool FindReferencedVariable(DATA * pRef, DWORD & var_index, DWORD & array_index);
	bool FindReferencedVariableByRootA(ATTRIBUTES * pA, DWORD & var_index, DWORD & array_index);
	ATTRIBUTES * TraceARoot(ATTRIBUTES * pA, char * & pAccess);
	void SaveAttributesData(ATTRIBUTES * pRoot);
	void ReadAttributesData(ATTRIBUTES * pRoot, ATTRIBUTES * pParent);
	void WriteVDword(DWORD v);
	DWORD ReadVDword();

	void AddPostEvent(S_EVENTMSG * pEM);

	void LoadPreprocess();

	void CopyOffsets(SEGMENT_DESC& Segment, STRINGS_LIST & srclist, STRINGS_LIST & dstlist, char * sname);

	void FormatDialog(char * file_name);
	void FormatAllDialog(char * directory_name);

	//bool SetSaveData(const char * file_name, const char * save_data);
	//bool GetSaveData(const char * file_name, DATA * pV);

	bool SetSaveData(char * file_name, void * save_data, long data_size);
	void * GetSaveData(char * file_name, long & data_size);

	void AddRuntimeEvent();

	DWORD SetScriptFunction(IFUNCINFO * pFuncInfo);
	void DeleteScriptFunction(DWORD nFuncHandle);

	bool CompileExpression(SEGMENT_DESC& Segment);
	bool CompileExpression_L0(SEGMENT_DESC& Segment);
	bool CompileExpression_L1(SEGMENT_DESC& Segment);
	bool CompileExpression_L2(SEGMENT_DESC& Segment);
	bool CompileExpression_L3(SEGMENT_DESC& Segment);
	bool CompileExpression_L4(SEGMENT_DESC& Segment);
	bool CompileExpression_L5(SEGMENT_DESC& Segment);
	bool CompileExpression_L6(SEGMENT_DESC& Segment);
	bool CompileExpression_L7(SEGMENT_DESC& Segment);

	DATA * GetOperand(char * pCodeBase, DWORD & ip, S_TOKEN_TYPE * pTokenType = 0);


};

#endif