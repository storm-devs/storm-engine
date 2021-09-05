#pragma once

#include <string_view>
#include <tuple>

#include "data.h"
#include "message.h"
#include "s_deftab.h"
#include "s_eventmsg.h"
#include "s_eventtab.h"
#include "s_functab.h"
#include "s_postevents.h"
#include "s_stack.h"
#include "s_vartab.h"
#include "script_libriary.h"
#include "string_codec.h"
#include "strings_list.h"
#include "token.h"
#include "logging.hpp"

#include "storm/ringbuffer_stack.hpp"

#define BCODE_BUFFER_BLOCKSIZE 4096
#define IOBUFFER_SIZE 65535

struct SEGMENT_DESC
{
    const char *name;
    uint32_t offset;
    uint32_t size;
    uint32_t id;
    uint32_t lines;
    bool bUnload;
    //--------------
    char *pData;
    char *pCode;
    uint32_t BCode_Program_size;
    uint32_t BCode_Buffer_size;

    STRINGS_LIST *Files_list;
};

struct OFFSET_INFO
{
    uint32_t offset;
    uint32_t segment_id;
};

struct EXTDATA_HEADER
{
    char sFileInfo[32];
    uint32_t dwExtDataOffset;
    uint32_t dwExtDataSize;
};

/*struct DEFINE_INFO
{
    uint32_t deftype;
    uint32_t data4b;
};*/

struct DOUBLE_DWORD
{
    uint32_t dw1;
    uint32_t dw2;
};

class SLIBHOLDER
{
  public:
    std::unique_ptr<SCRIPT_LIBRIARY> library;
    std::string name;

    SLIBHOLDER(SCRIPT_LIBRIARY *library, std::string name)
        : library(library), name(std::move(name))
    {
    }
};

class S_DEBUG;

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
    friend S_DEBUG;

  public:
    bool bBreakOnError;

    COMPILER();
    ~COMPILER();

    VSTRING_CODEC *GetVSC()
    {
        return &SCodec;
    }

    char *LoadFile(const char *file_name, uint32_t &file_size, bool bFullPath = false);
    // char *    AppendProgram(char * base_program, long base_program_size, char * append_program, long
    // append_program_size, long& new_program_size);
    bool AppendProgram(char *&pBase_program, uint32_t &Base_program_size, const char *pAppend_program,
                       uint32_t &Append_program_size, bool bAddLinefeed);
    void Trace(const char *data_PTR, ...);
    void DTrace(const char *data_PTR, ...);
    void SetError(const char *data_PTR, ...);
    void SetWarning(const char *data_PTR, ...);
    bool CreateProgram(const char *file_name);
    bool AddProgramFile(const char *file_name);

    void UnloadSegment(const char *segment_name);
    uint32_t GetSegmentIndex(uint32_t segment_id);

    void ProcessFrame(uint32_t DeltaTime);
    bool Run();
    void Release();
    void SetProgramDirectory(const char *dir_name);
    VDATA *ProcessEvent(const char *event_name, MESSAGE message);
    VDATA *ProcessEvent(const char *event_name);
    void SetEventHandler(const char *event_name, const char *func_name, long flag, bool bStatic = false);
    void DelEventHandler(const char *event_name, const char *func_name);

    bool Completed()
    {
        return bCompleted;
    }

    char *GetName();
    void ExitProgram();
    void ClearEvents();

    uint32_t GetIntFunctionCode(const char *func_name);
    DATA *BC_CallIntFunction(uint32_t func_code, DATA *&pVResult, uint32_t arguments);

    uint32_t GetIntFunctionsNum();
    bool InitInternalFunctions();

    bool ProcessDebugExpression(const char *pExpression, DATA &Result);
    bool SetOnDebugExpression(const char *pLValue, const char *pRValue, DATA &Result);
    bool ProcessDebugExpression0(const char *pExpression, DATA &Result);
    bool Compile(SEGMENT_DESC &Segment, char *pInternalCode = nullptr, uint32_t pInternalCodeSize = 0);
    void CompileToken(SEGMENT_DESC &Segment, S_TOKEN_TYPE Token_type, uint32_t data_blocks_num = 0, ...);
    void ResizeBCodeBuffer(SEGMENT_DESC &Segment, uint32_t add_size);
    bool BC_LoadSegment(const char *file_name);
    bool BC_SegmentIsLoaded(const char *file_name);
    bool BC_Execute(uint32_t function_code, DATA *&pVReturnResult, const char *pDbgExpSource = nullptr);
    bool BC_CallFunction(uint32_t func_code, uint32_t &ip, DATA *&pVResult);
    void FindErrorSource();
    S_TOKEN_TYPE BC_TokenGet(uint32_t &ip, uint32_t &token_data_size);
    S_TOKEN_TYPE BC_TokenGet();

    S_TOKEN_TYPE TokenLastReadResult;
    uint32_t TLR_DataOffset;
    const char *pRunCodeBase;
    bool TokenIs(S_TOKEN_TYPE test);

    S_TOKEN_TYPE TokenType()
    {
        return TokenLastReadResult;
    }

    S_TOKEN_TYPE NextTokenType();
    S_TOKEN_TYPE SkipAuxiliaryTokens();

    bool BC_ProcessExpression(DATA *value);
    void BC_ProcessExpression_L0(DATA *value);
    void BC_ProcessExpression_L1(DATA *value, bool bSkip = false);
    void BC_ProcessExpression_L2(DATA *value, bool bSkip = false);
    void BC_ProcessExpression_L3(DATA *value, bool bSkip = false);
    void BC_ProcessExpression_L4(DATA *value, bool bSkip = false);
    void BC_ProcessExpression_L5(DATA *value, bool bSkip = false);
    void BC_ProcessExpression_L6(DATA *value, bool bSkip = false);
    void BC_ProcessExpression_L7(DATA *value, bool bSkip = false);

    /*    void SkipExpression_L1();
        void SkipExpression_L2();
        void SkipExpression_L3();
        void SkipExpression_L4();
        void SkipExpression_L5();
        void SkipExpression_L6();
        void SkipExpression_L7();
    */
    //-----------------------------------------------------
    uint32_t CurrentFuncCode;
    void CompileNumber(SEGMENT_DESC &Segment);
    void CompileFloatNumber(SEGMENT_DESC &Segment);
    void CompileString(SEGMENT_DESC &Segment);
    bool CompileUnknown(SEGMENT_DESC &Segment);
    bool CompileBlock(SEGMENT_DESC &Segment, bool &bFunctionBlock, uint32_t &inout, S_TOKEN_TYPE bound_type,
                      uint32_t continue_jump, uint32_t break_offset, STRINGS_LIST &BreakTable);
    S_TOKEN_TYPE CompileAuxiliaryTokens(SEGMENT_DESC &Segment); //, bool & bFunctionBlock, DWORD & inout);
    bool BC_Jump(SEGMENT_DESC &Segment, uint32_t offset);
    void UpdateOffsets(SEGMENT_DESC &Segment, STRINGS_LIST &list, uint32_t offset, const char *sname = nullptr);
    S_TOKEN_TYPE DetectUnknown(uint32_t &code);

    void DumpAttributes(ATTRIBUTES *pA, long level);

    bool IsIntFuncVarArgsNum(uint32_t code);
    uint32_t GetInternalFunctionArgumentsNum(uint32_t code);
    bool CreateMessage(MESSAGE *pMs, uint32_t stack_offset, uint32_t vindex, bool s2s = false);
    void ProcessEvent(const char *event_name, MESSAGE *pMs);

    bool SaveState(std::fstream &fileS);
    bool LoadState(std::fstream &fileS);
    bool OnLoad();
    void SaveDataDebug(char *data_PTR, ...);
    void SaveData(const void *data_PTR, uint32_t data_size);
    bool ReadData(void *data_PTR, uint32_t data_size);
    void SaveString(const char *pS);
    char *ReadString();

    void SaveVariable(DATA *pV, bool bdim = false);
    bool ReadVariable(char *name, /*DWORD code,*/ bool bdim = false, uint32_t a_index = 0);
    bool FindReferencedVariable(DATA *pRef, uint32_t &var_index, uint32_t &array_index);
    bool FindReferencedVariableByRootA(ATTRIBUTES *pA, uint32_t &var_index, uint32_t &array_index);
    ATTRIBUTES *TraceARoot(ATTRIBUTES *pA, const char *&pAccess);
    void SaveAttributesData(ATTRIBUTES *pRoot);
    void ReadAttributesData(ATTRIBUTES *pRoot, ATTRIBUTES *pParent);
    void WriteVDword(uint32_t v);
    uint32_t ReadVDword();

    void AddPostEvent(S_EVENTMSG *pEM);

    void LoadPreprocess();

    void CopyOffsets(SEGMENT_DESC &Segment, STRINGS_LIST &srclist, STRINGS_LIST &dstlist, const char *sname);

    void FormatDialog(char *file_name);
    void FormatAllDialog(const char *directory_name);

    // bool SetSaveData(const char * file_name, const char * save_data);
    // bool GetSaveData(const char * file_name, DATA * pV);

    bool SetSaveData(const char *file_name, void *save_data, long data_size);
    void *GetSaveData(const char *file_name, long &data_size);

    void AddRuntimeEvent();

    uint32_t SetScriptFunction(IFUNCINFO *pFuncInfo);

    bool CompileExpression(SEGMENT_DESC &Segment);
    bool CompileExpression_L0(SEGMENT_DESC &Segment);
    bool CompileExpression_L1(SEGMENT_DESC &Segment);
    bool CompileExpression_L2(SEGMENT_DESC &Segment);
    bool CompileExpression_L3(SEGMENT_DESC &Segment);
    bool CompileExpression_L4(SEGMENT_DESC &Segment);
    bool CompileExpression_L5(SEGMENT_DESC &Segment);
    bool CompileExpression_L6(SEGMENT_DESC &Segment);
    bool CompileExpression_L7(SEGMENT_DESC &Segment);

    DATA *GetOperand(const char *pCodeBase, uint32_t &ip, S_TOKEN_TYPE *pTokenType = nullptr);

    // writes down current script stack (internal functions+script functions+events) to logfile
    // currently used for collecting additional crash info
    // TODO: use it for internal errors also
    void collectCallStack();

private:
    COMPILER_STAGE CompilerStage;
    STRINGS_LIST LabelTable;
    // STRINGS_LIST EventTable;
    STRINGS_LIST LabelUpdateTable;
    TOKEN Token;
    MESSAGE *pEventMessage;
    std::vector<SEGMENT_DESC> SegmentTable;
    uint32_t SegmentsNum;
    uint32_t RunningSegmentID;
    uint32_t InstructionPointer;

    char *pBuffer;
    uint32_t dwCurPointer, dwMaxSize;

    char *ProgramDirectory;
    bool bCompleted;
    bool bEntityUpdate;
    char *pDebExpBuffer;
    uint32_t nDebExpBufferSize;

    FuncInfo *pRun_fi; // running function info
    FuncTable FuncTab;
    VarTable VarTab;
    S_DEFTAB DefTab;
    S_STACK SStack;
    S_EVENTTAB EventTab;
    // TCLASS_LIST<S_EVENTMSG> EventMsg;
    POSTEVENTS_LIST EventMsg;
    std::vector<SLIBHOLDER> LibriaryFuncs;

    STRING_CODEC SCodec;

    bool bRuntimeLog;
    uint32_t nRuntimeLogEventsBufferSize;
    uint32_t nRuntimeLogEventsNum;
    std::vector<uint32_t> pRuntimeLogEvent;
    uint32_t nRuntimeTicks;

    bool bFirstRun;
    bool bScriptTrace;
    bool bWriteCodeFile;
    bool bDebugInfo;
    char DebugSourceFileName[MAX_PATH];
    char gs[MAX_PATH];
    uint32_t DebugSourceLine;
    char *pCompileTokenTempBuffer;

    // HANDLE hSaveFileFileHandle;

    bool bDebugExpressionRun;
    bool bTraceMode;

    bool bEventsBreak;

    char DebugTraceFileName[MAX_PATH];
    uint32_t nDebugTraceLineCode;

    uint32_t nIOBufferSize;
    uint32_t nIOFullSize;
    char *pIOBuffer;

    // script registers
    DATA rAX;
    DATA rBX;
    DATA ExpressionResult;
    ATTRIBUTES *rAP;

    // loggers
    storm::logging::logger_ptr logTrace_;
    storm::logging::logger_ptr logError_;
    storm::logging::logger_ptr logStack_;

    // backtrace stack
    // NB: pointers are safe as long as we pop elements before they expire
    static constexpr size_t CALLSTACK_SIZE = 64U;
    storm::ringbuffer_stack<std::tuple<const char *, size_t, const char *>, CALLSTACK_SIZE> callStack_;
};
