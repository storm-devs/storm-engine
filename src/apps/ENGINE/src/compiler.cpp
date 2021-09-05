#include "compiler.h"

#include <cstdio>

#include <zlib.h>

#include "s_debug.h"
#include "logging.hpp"
#include "storm_assert.h"

#define SKIP_COMMENT_TRACING
#define TRACE_OFF
#define INVALID_SEGMENT_INDEX 0xffffffff
#ifndef INVALID_ARRAY_INDEX
#define INVALID_ARRAY_INDEX 0xffffffff
#endif
#define INVALID_OFFSET 0xffffffff
#define DSL_INI_VALUE 0
#define SBUPDATE 4
#define DEF_COMPILE_EXPRESSIONS

// extern char * FuncNameTable[];
extern INTFUNCDESC IntFuncTable[];
extern S_DEBUG * CDebug;
extern uint32_t dwNumberScriptCommandsExecuted;

COMPILER::COMPILER()
{
    CompilerStage = CS_SYSTEM;
    LabelTable.SetStringDataSize(sizeof(uint32_t));
    LabelUpdateTable.SetStringDataSize(sizeof(DOUBLE_DWORD));
    ProgramDirectory = nullptr;
    pEventMessage = nullptr;
    bCompleted = false;
    SegmentsNum = 0;
    //    FuncTab.KeepNameMode(true);    // true for displaying internal functions names
    //    VarTab.KeepNameMode(false);
    //    EventTab.KeepNameMode(false);
    //    DefTab.KeepNameMode(false);
    InitInternalFunctions();
    bWriteCodeFile = false;
    bDebugInfo = false;
    strcpy_s(DebugSourceFileName, "<no debug information>");
    DebugSourceLine = 0;
    InstructionPointer = 0;
    pCompileTokenTempBuffer = nullptr;
    pRun_fi = nullptr;
    pRunCodeBase = nullptr;
    SStack.SetVCompiler(this);
    VarTab.SetVCompiler(this);
    srand(GetTickCount());
    bEntityUpdate = true;
    pDebExpBuffer = nullptr;
    nDebExpBufferSize = 0;
    bDebugExpressionRun = false;
    bTraceMode = true;
    DebugTraceFileName[0] = 0;
    nDebugTraceLineCode = 0;
    bBreakOnError = false;
    bRuntimeLog = false;
    nRuntimeLogEventsNum = 0;
    nRuntimeTicks = 0;
    nRuntimeLogEventsBufferSize = 0;
    nIOBufferSize = 0;
    pIOBuffer = nullptr;

    rAX.SetVCompiler(this);
    rBX.SetVCompiler(this);
    ExpressionResult.SetVCompiler(this);
    rAP = nullptr;
    // bScriptTrace = false;
    bFirstRun = true;
    pBuffer = nullptr;

    using storm::logging::getOrCreateLogger;
    logTrace_ = getOrCreateLogger("compile");
    logError_ = getOrCreateLogger("error");
    logStack_ = getOrCreateLogger("script_stack");
    logStack_->set_pattern("%v");
}

COMPILER::~COMPILER()
{
    Release();
}

void COMPILER::Release()
{
    // DWORD m;
    // FUNCINFO fi;

    // debug log
    /*    if(bRuntimeLog)
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
            //if(fi.name)    trace("  %s : %f",fi.name,fi.fTimeUsage);
            //else trace("  %d : %f",n,fi.fTimeUsage);
          }

          FuncTab.GetFuncX(fi,nFuncCode);
          if(fi.name)    trace("  %s",fi.name); else trace("  %d",n);
          trace("  ticks summary  : %.0f",fi.fTimeUsage);
          trace("  calls          : %d",fi.nNumberOfCalls);
          if(fi.nNumberOfCalls != 0) trace("  average ticks  : %.0f",fi.fTimeUsage/fi.nNumberOfCalls);


          FuncTab.SetTimeUsage(nFuncCode,-1.0);
          trace("");

          //if(fi.name)    trace("  %s : %f",fi.name,fi.fTimeUsage);
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

    for (uint32_t n = 0; n < SegmentsNum; n++)
    {
        delete SegmentTable[n].name;
        delete SegmentTable[n].pData;
        delete SegmentTable[n].pCode;
        if (SegmentTable[n].Files_list)
        {
            SegmentTable[n].Files_list->Release();
            delete SegmentTable[n].Files_list;
        }
        SegmentTable[n].Files_list = nullptr;
    }
    SegmentTable.clear();
    SegmentsNum = 0;
    LabelTable.Release();
    LabelUpdateTable.Release();
    Token.Reset();
    delete ProgramDirectory;
    ProgramDirectory = nullptr;
    RunningSegmentID = INVALID_SEGMENT_INDEX;
    delete pCompileTokenTempBuffer;
    pCompileTokenTempBuffer = nullptr;

    FuncTab.Release();
    VarTab.Release();
    DefTab.Release();
    SStack.Release();
    EventTab.Release();
    EventMsg.Release();
    SCodec.Release();
    LibriaryFuncs.clear();

    delete pDebExpBuffer;
    pDebExpBuffer = nullptr;
    nDebExpBufferSize = 0;

    nRuntimeLogEventsNum = 0;
    nRuntimeLogEventsBufferSize = 0;

    rAX.Release();
    rBX.Release();
    ExpressionResult.Release();
    // Token.Release();
}

void COMPILER::SetProgramDirectory(const char *dir_name)
{
    delete ProgramDirectory;
    ProgramDirectory = nullptr;
    if (dir_name)
    {
        const auto len = strlen(dir_name) + strlen("\\") + 1;
        ProgramDirectory = new char[len];
        strcpy_s(ProgramDirectory, len, dir_name);
        strcat_s(ProgramDirectory, len, "\\");
    }
    CDebug->SetProgramDirectory(dir_name);
}

// load file into memory
char *COMPILER::LoadFile(const char *file_name, uint32_t &file_size, bool bFullPath)
{
    const char *fName;
    char buffer[MAX_PATH];

    if (!bFullPath)
    {
        std::string EngineDir = "storm-engine\\";
        if (strncmp(file_name, EngineDir.c_str(), EngineDir.length()) == 0)
        {
            std::string ExePath = fio->_GetExecutableDirectory() + "resource\\shared\\";
            strcpy_s(buffer, ExePath.c_str());
            strcat_s(buffer, file_name + EngineDir.length());
        }
        else if (ProgramDirectory)
        {
            strcpy_s(buffer, ProgramDirectory);
            strcat_s(buffer, file_name);
        }
        else
        {
            strcpy_s(buffer, file_name);
        }
    }

    file_size = 0;
    if (bFullPath)
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

        fName = file_name;
    }
    else
    {
        fName = &buffer[0];
    }

    auto fileS = fio->_CreateFile(fName, std::ios::binary | std::ios::in);
    if (!fileS.is_open())
    {
        return nullptr;
    }
    const auto fsize = fio->_GetFileSize(fName);

    auto *const pData = static_cast<char *>(new char[fsize + 1]);
    if (!fio->_ReadFile(fileS, pData, fsize))
    {
        delete[] pData;
        fio->_CloseFile(fileS);
        return nullptr;
    }
    fio->_CloseFile(fileS);
    file_size = fsize;
    pData[fsize] = 0;
    return pData;
}

// write to compilation log file
void COMPILER::Trace(const char *data_PTR, ...)
{
#ifdef TRACE_OFF
    return;
#endif
    if (data_PTR == nullptr)
        return;
    char LogBuffer[4096];
    va_list args;
    va_start(args, data_PTR);
    _vsnprintf_s(LogBuffer, sizeof(LogBuffer) - 4, data_PTR, args);
    va_end(args);
    logTrace_->trace(LogBuffer);
}

// write to compilation log file
void COMPILER::DTrace(const char *data_PTR, ...)
{
#ifdef DTRACEOFF
    return;
#endif
    if (data_PTR == nullptr)
        return;
    char LogBuffer[4096];
    va_list args;
    va_start(args, data_PTR);
    _vsnprintf_s(LogBuffer, sizeof(LogBuffer) - 4, data_PTR, args);
    va_end(args);
    logTrace_->trace(LogBuffer);
}

// append one block of code to another
// return new pointer
/*char * COMPILER::AppendProgram(char * pBase_program, long Base_program_size, char * pAppend_program, long
Append_program_size, long& new_program_size)
{
    char * pTemp;
    char * pBase;
    new_program_size = Base_program_size + Append_program_size + 2;

    pBase = new char[new_program_size];
    pTemp = pBase;
    memcpy(pTemp,pBase_program,Base_program_size);
    pTemp += Base_program_size;
    pTemp[0] = ' ';    pTemp++;    // code blocks separator ' ' to prevent data merging
    memcpy(pTemp,pAppend_program,Append_program_size);
    delete pBase_program;
    delete pAppend_program;
    return pBase;
}*/

bool COMPILER::AppendProgram(char *&pBase_program, uint32_t &Base_program_size, const char *pAppend_program,
                             uint32_t &Append_program_size, bool bAddLinefeed)
{
    const auto offset = Base_program_size;
    if (pAppend_program == nullptr)
        return false;
    if (bAddLinefeed)
    {
        // pBase_program = (char *)RESIZE(pBase_program,Base_program_size + Append_program_size + 3); // +1 for
        // terminating zero, +1 for 0xd +1 for 0xa
        auto *const newPtr = new char[Base_program_size + Append_program_size + 3];
        memcpy(newPtr, pBase_program, Base_program_size);
        delete[] pBase_program;
        pBase_program = newPtr;
        Base_program_size = Base_program_size + Append_program_size + 2;
        memcpy(&pBase_program[offset], pAppend_program, Append_program_size);
        pBase_program[Base_program_size - 2] = 0xd; // code blocks separator to prevent data merging
        pBase_program[Base_program_size - 1] = 0xa; // code blocks separator to prevent data merging
        pBase_program[Base_program_size] = 0;       // terminating zero
        delete[] pAppend_program;
        Append_program_size += 2;
        return true;
    }
    // pBase_program = (char *)RESIZE(pBase_program,Base_program_size + Append_program_size + 2); // +1 for terminating
    // zero, +1 for ';'
    auto *const newPtr = new char[Base_program_size + Append_program_size + 2];
    memcpy(newPtr, pBase_program, Base_program_size);
    delete[] pBase_program;
    pBase_program = newPtr;
    Base_program_size = Base_program_size + Append_program_size + 1;
    memcpy(&pBase_program[offset], pAppend_program, Append_program_size);
    pBase_program[Base_program_size - 1] = ';'; // code blocks separator to prevent data merging
    pBase_program[Base_program_size] = 0;       // terminating zero
    delete[] pAppend_program;
    Append_program_size += 1;
    return true;
}

void COMPILER::SetError(const char *data_PTR, ...)
{
    if (bDebugExpressionRun)
        return;
    if (data_PTR == nullptr)
        return;
    char LogBuffer[BUFSIZ];
    char ErrorBuffer[MAX_PATH + BUFSIZ];
    va_list args;
    va_start(args, data_PTR);
    _vsnprintf_s(LogBuffer, sizeof(LogBuffer) - 4, data_PTR, args);
    uint32_t bytes;
    FindErrorSource();

    switch (CompilerStage)
    {
    case CS_SYSTEM:
        sprintf_s(ErrorBuffer, "ERROR in %s(%d): %s", DebugSourceFileName, DebugSourceLine + 1, LogBuffer);
        break;
    case CS_COMPILATION:
        sprintf_s(ErrorBuffer, "COMPILE ERROR in %s(%d): %s", DebugSourceFileName, DebugSourceLine + 1, LogBuffer);
        break;
    case CS_RUNTIME:
        sprintf_s(ErrorBuffer, "RUNTIME ERROR in %s(%d): %s", DebugSourceFileName, DebugSourceLine + 1, LogBuffer);
        break;
    }
    va_end(args);

    logError_->error(ErrorBuffer);

    if (bBreakOnError)
        CDebug->SetTraceMode(TMODE_MAKESTEP);
}

void COMPILER::SetWarning(const char *data_PTR, ...)
{
#ifdef WARNINGS_OFF
    return;
#endif
    if (bDebugExpressionRun)
        return;
    char LogBuffer[BUFSIZ];
    char ErrorBuffer[MAX_PATH + BUFSIZ];
    if (data_PTR == nullptr)
        return;
    va_list args;
    va_start(args, data_PTR);
    _vsnprintf_s(LogBuffer, sizeof(LogBuffer) - 4, data_PTR, args);
    uint32_t bytes;
    FindErrorSource();

    sprintf_s(ErrorBuffer, "WARNING in %s(%d): %s", DebugSourceFileName, DebugSourceLine + 1, LogBuffer);
    va_end(args);

    logTrace_->warn(ErrorBuffer);
}

void COMPILER::LoadPreprocess()
{
    auto engine_ini = fio->OpenIniFile(core.EngineIniFileName());
    if (engine_ini)
    {
        if (engine_ini->GetLong("script", "debuginfo", 0) == 0)
        {
            bDebugInfo = false;
            // FuncTab.KeepNameMode(false);
            // VarTab.KeepNameMode(false);
            // DefTab.KeepNameMode(false);
            // EventTab.KeepNameMode(false);
        }
        else
        {
            bDebugInfo = true;
            // FuncTab.KeepNameMode(true);
            // VarTab.KeepNameMode(true);
            // DefTab.KeepNameMode(true);
            // EventTab.KeepNameMode(true);
        }
        if (engine_ini->GetLong("script", "codefiles", 0) == 0)
            bWriteCodeFile = false;
        else
            bWriteCodeFile = true;

        if (engine_ini->GetLong("script", "runtimelog", 0) == 0)
            bRuntimeLog = false;
        else
            bRuntimeLog = true;

        // if(engine_ini->GetLong("script","tracefiles",0) == 0) bScriptTrace = false;
        // else bScriptTrace = true;
    }

    auto ini = fio->OpenIniFile(PROJECT_NAME);
    if (ini)
    {
        bBreakOnError = (ini->GetLong("options", "break_on_error", 0) == 1);
    }
}

bool COMPILER::CreateProgram(const char *file_name)
{
    /*    INIFILE * engine_ini;

    engine_ini = fio->OpenIniFile(core.EngineIniFileName());
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
    const bool bRes = BC_LoadSegment(file_name);

    /*    for(DWORD m=0;m<HASH_TABLE_SIZE;m++)
      {
        DTrace("HashIndex[%d]",m);
        for(DWORD n=0;n<SCodec.HTable[m].nStringsNum;n++)
        {
          DTrace(SCodec.HTable[m].ppDat[n]);
        }
      }*/
    return bRes;
}

bool COMPILER::Run()
{
    pRun_fi = nullptr;
    const uint32_t function_code = FuncTab.FindFunc("Main");
    DATA *pResult;
    bFirstRun = false;
    BC_Execute(function_code, pResult);
    pRun_fi = nullptr;

    // DATA Result;
    // Result.SetVCompiler(this);
    // ProcessDebugExpression("Characters[45].id",Result);

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
    const uint32_t segment_index = GetSegmentIndex(RunningSegmentID);
    if (segment_index == INVALID_SEGMENT_INDEX)
        return;

    uint32_t index;
    uint32_t token_data_size;
    uint32_t file_line_offset;
    uint32_t file_name_size;
    S_TOKEN_TYPE Token_type;

    uint32_t ip = 0;
    DebugSourceLine = 0;
    file_line_offset = 0;
    char *pCodeBase = SegmentTable[segment_index].pCode;
    do
    {
        Token_type = static_cast<S_TOKEN_TYPE>(pCodeBase[ip]);
        ip++;
        if (static_cast<uint8_t>(pCodeBase[ip]) < 0xff)
        {
            token_data_size = static_cast<uint8_t>(pCodeBase[ip]);
            ip++;
        }
        else
        {
            ip++;
            memcpy(&token_data_size, &pCodeBase[ip], sizeof(uint32_t));
            ip += sizeof(uint32_t);
        }

        if (ip >= InstructionPointer)
        {
            // DebugSourceLine -= file_line_offset;
            return;
        }
        switch (Token_type)
        {
        case DEBUG_LINE_CODE:
            memcpy(&DebugSourceLine, &pCodeBase[ip], sizeof(uint32_t));
            break;
        case DEBUG_FILE_NAME:
            index = ip;
            memcpy(&file_line_offset, &pCodeBase[index], sizeof(uint32_t));
            index += sizeof(uint32_t);
            memcpy(&file_name_size, &pCodeBase[index], sizeof(uint32_t));
            index += sizeof(uint32_t);
            memcpy(DebugSourceFileName, &pCodeBase[index], file_name_size);
            index += file_name_size;
            DebugSourceFileName[file_name_size] = 0;
            DebugSourceLine = DSL_INI_VALUE;
            break;
        }
        ip += token_data_size; // step to next instruction
    } while (Token_type != END_OF_PROGRAMM);
}

void COMPILER::SetEventHandler(const char *event_name, const char *func_name, long flag, bool bStatic)
{
    FuncInfo fi;

    if (event_name == nullptr)
    {
        SetError("Invalid event name for SetEventHandler");
        return;
    }
    if (func_name == nullptr)
    {
        SetError("Invalid function name for SetEventHandler");
        return;
    }

    const uint32_t func_code = FuncTab.FindFunc(func_name);
    if (func_code == INVALID_FUNC_CODE)
    {
        SetError("Invalid function code in SetEventHandler");
        return;
    }

    if (!FuncTab.GetFunc(fi, func_code))
    {
        SetError("func not found eror");
        return;
    }

    EventTab.AddEventHandler(event_name, func_code, fi.segment_id, flag, bStatic);
}

void COMPILER::DelEventHandler(const char *event_name, const char *func_name)
{
    if (event_name == nullptr)
    {
        SetError("Bad event name");
        return;
    }

    if (func_name == nullptr)
    {
        SetError("Bad func name");
        return;
    }

    const uint32_t func_code = FuncTab.FindFunc(func_name);
    if (func_code == INVALID_FUNC_CODE)
    {
        SetError("Invalid function code in DelEventHandler");
        return;
    }

    EventTab.SetStatus(event_name, func_code, FSTATUS_DELETED);

    for (long n = 0; n < static_cast<long>(EventMsg.GetClassesNum()); n++)
    {
        S_EVENTMSG *pM = EventMsg.Read(n);
        if (!pM->pEventName)
            continue;
        if (pM->ProcessTime(0))
            continue; // skip events, possible executed on this frame
        if (_stricmp(pM->pEventName, event_name) == 0)
        {
            EventMsg.Del(n);
            n--;
        }
    }
}

VDATA *COMPILER::ProcessEvent(const char *event_name)
{
    // TODO: only do if stack debug if enabled (should be runtime configurable)
    // push event name to call stack
    storm::ringbuffer_stack_push_guard push_guard(callStack_);
    push_guard.push(std::make_tuple("", 0U, event_name));

    uint32_t event_code;
    uint32_t func_code;
    VDATA *pVD;
    DATA *pResult;
    MESSAGE *pMem;
    EVENTINFO ei;
    uint32_t current_debug_mode;

    uint64_t dwRDTSC, nTicks;

    RDTSC_B(dwRDTSC);

    bEventsBreak = false;

    uint32_t nTimeOnEvent = GetTickCount();
    current_debug_mode = CDebug->GetTraceMode();

    pVD = nullptr;
    if (event_name == nullptr)
    {
        SetError("Invalid event name in ProcessEvent");
        return nullptr;
    }

    event_code = EventTab.FindEvent(event_name);
    if (event_code == INVALID_EVENT_CODE)
        return nullptr; // no handlers
    EventTab.GetEvent(ei, event_code);
    for (uint32_t n = 0; n < ei.elements; n++)
    {
        func_code = ei.pFuncInfo[n].func_code;
        if (ei.pFuncInfo[n].status != FSTATUS_NORMAL)
            continue;
        pMem = pEventMessage;
        if (pMem)
        {
            pMem->Move2Start();
        }

        const uint32_t nStackVars = SStack.GetDataNum(); // remember stack elements num
        RDTSC_B(nTicks);
        BC_Execute(ei.pFuncInfo[n].func_code, pResult);
        RDTSC_E(nTicks);

        FuncInfo fi;
        // if(FuncTab.GetFuncX(fi,ei.pFuncInfo[n].func_code))
        if (FuncTab.GetFuncX(fi, func_code))
        {
            if (fi.return_type != TVOID)
            {
                fi.return_type = TVOID;
            }
        } //*/

        if (EventTab.GetEvent(ei, event_code)) // to be sure event still exist
        {
            if (n < ei.elements)
            {
                if (!FuncTab.AddTime(ei.pFuncInfo[n].func_code, nTicks))
                {
                    core.Trace("Invalid func_code = %u for AddTime", ei.pFuncInfo[n].func_code);
                }
            }
        }

        pEventMessage = pMem;
        if (pResult)
            pVD = pResult->GetVarPointer();
        if (pResult)
            SStack.Pop();

        if (nStackVars != SStack.GetDataNum())
        {
            SStack.InvalidateFrom(nStackVars); // restore stack state - crash situation
            pVD = nullptr;
            SetError("process event stack error");
        }

        if (bEventsBreak)
            break;
    }

    nTimeOnEvent = GetTickCount() - nTimeOnEvent;

    nRuntimeTicks += nTimeOnEvent;

    pRun_fi = nullptr;

    if (current_debug_mode == TMODE_CONTINUE)
        CDebug->SetTraceMode(TMODE_CONTINUE);
    // SetFocus(core.App_Hwnd);        // VANO CHANGES

    RDTSC_E(dwRDTSC);

    // VANO CHANGES - remove in release
    if (core.Controls->GetDebugAsyncKeyState('5') < 0 && core.Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0)
    {
        core.Trace("evnt: %d, %s", dwRDTSC, event_name);
    }

    return pVD;
}

void COMPILER::ProcessEvent(const char *event_name, MESSAGE *pMs)
{
    pEventMessage = pMs;
    ProcessEvent(event_name);
    pEventMessage = nullptr;
}

VDATA *COMPILER::ProcessEvent(const char *event_name, MESSAGE message)
{
    pEventMessage = &message;
    VDATA *pVD = ProcessEvent(event_name);
    pEventMessage = nullptr;
    return pVD;
}

char *COMPILER::GetName()
{
    return SegmentTable[0].Files_list->GetString(0);
    // return Files_list.GetString(0);
}

uint32_t COMPILER::GetSegmentIndex(uint32_t segment_id)
{
    for (uint32_t n = 0; n < SegmentsNum; n++)
    {
        if (SegmentTable[n].id == segment_id)
            return n;
    }
    return INVALID_SEGMENT_INDEX;
}

// function release global variables, functions and labels reference for segment
// mark segment for subsequent unload (on ProcessFrame)
void COMPILER::UnloadSegment(const char *segment_name)
{
    //    OFFSET_INFO offset_info;

    for (uint32_t n = 0; n < SegmentsNum; n++)
    {
        if (strcmp(SegmentTable[n].name, segment_name) == 0)
        {
            const uint32_t segment_id = SegmentTable[n].id;
            SegmentTable[n].bUnload = true;
            FuncTab.InvalidateBySegmentID(segment_id);
            VarTab.InvalidateBySegmentID(segment_id);
            EventTab.InvalidateBySegmentID(segment_id);
            DefTab.InvalidateBySegmentID(segment_id);
            if (SegmentTable[n].Files_list)
            {
                SegmentTable[n].Files_list->Release();
            }
            break;
        }
    }
}

bool COMPILER::BC_SegmentIsLoaded(const char *file_name)
{
    if (file_name == nullptr)
    {
        SetError("Invalid segment name");
        return false;
    }
    for (uint32_t n = 0; n < SegmentsNum; n++)
    {
        if (strcmp(SegmentTable[n].name, file_name) == 0)
            return true;
    }
    return false;
}

bool COMPILER::BC_LoadSegment(const char *file_name)
{
    uint32_t n;

    LabelUpdateTable.Release();

    if (file_name == nullptr)
    {
        SetError("Invalid segment name");
        return false;
    }

    // Trace("Loading segment: %s", file_name);

    // check for already loaded
    for (n = 0; n < SegmentsNum; n++)
    {
        if (strcmp(SegmentTable[n].name, file_name) == 0)
        {
            SetWarning("Segment already loaded: %s", file_name);
            return true;
        }
    }

    // compute new segment id --------------------------
    uint32_t id = 0;
    bool found = false;
    while (!found)
    {
        found = true;
        for (n = 0; n < SegmentsNum; n++)
        {
            if (SegmentTable[n].id == id)
                found = false;
        }
        if (!found)
            id++;
    }

    const uint32_t index = SegmentsNum;
    SegmentsNum++;
    // SegmentTable = (SEGMENT_DESC *)RESIZE(SegmentTable,SegmentsNum*sizeof(SEGMENT_DESC));
    SegmentTable.resize(SegmentsNum);

    // const auto len = strlen(file_name) + 1;
    // SegmentTable[index].name = new char[len];
    // memcpy(SegmentTable[index].name, file_name, len);
    SegmentTable[index].name = _strdup(file_name);
    SegmentTable[index].id = id;
    SegmentTable[index].bUnload = false;
    SegmentTable[index].pData = nullptr;
    SegmentTable[index].pCode = nullptr;
    SegmentTable[index].BCode_Program_size = 0;
    SegmentTable[index].BCode_Buffer_size = 0;

    SegmentTable[index].Files_list = new STRINGS_LIST;
    SegmentTable[index].Files_list->SetStringDataSize(sizeof(OFFSET_INFO));
    const bool bRes = Compile(SegmentTable[index]);
    if (!bRes)
    {
        delete SegmentTable[index].name;
        delete SegmentTable[index].Files_list;
        SegmentsNum--;
        // SegmentTable = (SEGMENT_DESC *)RESIZE(SegmentTable,SegmentsNum*sizeof(SEGMENT_DESC));
        SegmentTable.resize(SegmentsNum);
        FuncTab.InvalidateBySegmentID(id);
        VarTab.InvalidateBySegmentID(id);
        EventTab.InvalidateBySegmentID(id);
        DefTab.InvalidateBySegmentID(id);
    }
    return bRes;
}

bool COMPILER::ProcessDebugExpression(const char *pExpression, DATA &Result)
{
    if (pExpression == nullptr)
        return false;

    const uint32_t nDataSize = strlen(pExpression) + strlen("return ") + 2;
    if (nDataSize > nDebExpBufferSize)
    {
        //    pDebExpBuffer = (char *)RESIZE(pDebExpBuffer,nDataSize);
        auto *const newPtr = new char[nDataSize];
        memcpy(newPtr, pDebExpBuffer, nDebExpBufferSize);
        delete pDebExpBuffer;
        pDebExpBuffer = newPtr;
        nDebExpBufferSize = nDataSize;
    }
    sprintf_s(pDebExpBuffer, nDebExpBufferSize, "return %s;", pExpression);
    return ProcessDebugExpression0(pDebExpBuffer, Result);
}

bool COMPILER::SetOnDebugExpression(const char *pLValue, const char *pRValue, DATA &Result)
{
    if (pLValue == nullptr || pRValue == nullptr)
        return false;

    const uint32_t nDataSize = strlen(pLValue) + strlen(pRValue) + 5;
    if (nDataSize > nDebExpBufferSize)
    {
        // pDebExpBuffer = (char *)RESIZE(pDebExpBuffer,nDataSize);
        auto *const newPtr = new char[nDataSize];
        memcpy(newPtr, pDebExpBuffer, nDebExpBufferSize);
        delete pDebExpBuffer;
        pDebExpBuffer = newPtr;
        nDebExpBufferSize = nDataSize;
    }
    sprintf_s(pDebExpBuffer, nDataSize, "%s = %s;", pLValue, pRValue);
    return ProcessDebugExpression0(pDebExpBuffer, Result);
}

bool COMPILER::ProcessDebugExpression0(const char *pExpression, DATA &Result)
{
    SEGMENT_DESC Segment;
    STRINGS_LIST DbgLocalSL;
    bool bRes;

    DATA *pResult = nullptr;

    if (pExpression == nullptr)
        return false;

    bDebugExpressionRun = true;

    PZERO(&Segment, sizeof(Segment));
    Segment.name = "Debug Expression";
    Segment.id = RunningSegmentID;
    Segment.bUnload = false;
    Segment.pData = nullptr;
    Segment.pCode = nullptr;
    Segment.BCode_Program_size = 0;
    Segment.BCode_Buffer_size = 0;
    Segment.Files_list = &DbgLocalSL;
    Segment.Files_list->SetStringDataSize(sizeof(OFFSET_INFO));

    if (pRun_fi)
    {
        CurrentFuncCode = FuncTab.FindFunc(pRun_fi->name);
    }

    try
    {
        bRes = Compile(Segment, pDebExpBuffer, strlen(pDebExpBuffer) + 1);
    }
    catch (...)
    {
        return false;
    }

    if (!bRes)
    {
        delete Segment.pCode;
        bDebugExpressionRun = false;
        return false;
    }

    // DWORD mem_CurrentFuncCode;

    // save current pointers values
    const uint32_t mem_InstructionPointer = InstructionPointer;
    // mem_ip = ip;
    FuncInfo *mem_pfi = pRun_fi;
    const char *mem_codebase = pRunCodeBase;
    // mem_CurrentFuncCode = CurrentFuncCode;

    // trace("Segment.pCode : %s",pDebExpBuffer);

    // DWORD old_data_num;
    // old_data_num = SStack.GetDataNum(); trace("old_data_num: %d",old_data_num);

    const uint32_t stackN = SStack.GetDataNum();

    try
    {
        bRes = BC_Execute(INVALID_FUNC_CODE, pResult, Segment.pCode);
    }
    // old_data_num = SStack.GetDataNum(); trace("new_data_num: %d",old_data_num);

    catch (...)
    {
        bRes = false;
    }

    // restore
    pRun_fi = mem_pfi;
    InstructionPointer = mem_InstructionPointer;
    // ip = mem_ip;
    // RunningSegmentID = pRun_fi->segment_id;
    pRunCodeBase = mem_codebase;

    delete Segment.pCode;

    if (pResult)
    {
        Result.Copy(pResult);
        // SStack.Pop();
    }
    SStack.InvalidateFrom(stackN);

    bDebugExpressionRun = false;
    return bRes;
}

void COMPILER::ProcessFrame(uint32_t DeltaTime)
{
    if (core.Timer.Ring)
        AddRuntimeEvent();

    for (uint32_t n = 0; n < SegmentsNum; n++)
    {
        if (!SegmentTable[n].bUnload)
            continue;
        // unload segment of program
        delete SegmentTable[n].pData;
        delete SegmentTable[n].pCode;
        delete SegmentTable[n].name;
        delete SegmentTable[n].Files_list;
        SegmentTable[n].Files_list = nullptr;

        for (uint32_t i = n; i < (SegmentsNum - 1); i++)
        {
            SegmentTable[i] = SegmentTable[i + 1];
        }
        SegmentsNum--;
        // SegmentTable = (SEGMENT_DESC *)RESIZE(SegmentTable,SegmentsNum*sizeof(SEGMENT_DESC));
        SegmentTable.resize(SegmentsNum);
        // reset search
        n = 0;
    }

    EventTab.ProcessFrame();

    for (long ln = 0; ln < static_cast<long>(EventMsg.GetClassesNum()); ln++)
    {
        S_EVENTMSG *pMsg = EventMsg.Read(ln);
        if (pMsg->bInvalide)
            continue;
        if (pMsg->bProcess == false)
        {
            pMsg->ProcessTime(DeltaTime);
            continue;
        }
        if (pMsg->ProcessTime(DeltaTime))
        {
            pMsg->Invalidate();
            ProcessEvent(pMsg->pEventName, pMsg->pMessageClass);
            // EventMsg.Del(ln);
            // ln--;
            /*ProcessEvent(pMsg->pEventName,pMsg->pMessageClass);
            EventMsg.Del(ln);
            ln--;*/
        }
    }
    EventMsg.RemoveInvalidated();
}

void COMPILER::ResizeBCodeBuffer(SEGMENT_DESC &Segment, uint32_t add_size)
{
    int64_t needed_memory = static_cast<int64_t>(Segment.BCode_Program_size) + add_size - Segment.BCode_Buffer_size;
    if (needed_memory >= 0)
    {
        const auto needed_blocks = (needed_memory - 1) / BCODE_BUFFER_BLOCKSIZE + 1;
        Segment.BCode_Buffer_size += needed_blocks * BCODE_BUFFER_BLOCKSIZE;
        // Segment.pCode = (char *)RESIZE(Segment.pCode,Segment.BCode_Buffer_size);
        auto *const newPtr = new char[Segment.BCode_Buffer_size];
        memcpy(newPtr, Segment.pCode, Segment.BCode_Program_size);
        delete[] Segment.pCode;
        Segment.pCode = newPtr;
    }
}

void COMPILER::CompileToken(SEGMENT_DESC &Segment, S_TOKEN_TYPE Token_type, uint32_t data_blocks_num, ...)
{
    uint32_t data_size;
    uint32_t n;
    va_list args;
    char *data_ptr;

    // DTrace("token: %s : %s",Token.GetTypeName(Token_type),Token.GetData());

    // write instruction (1 byte) ----------------------------------------------
    uint32_t write_size = 1;
    ResizeBCodeBuffer(Segment, write_size);
    Segment.pCode[Segment.BCode_Program_size] = Token_type;
    Segment.BCode_Program_size += write_size;
    //--------------------------------------------------------------------------

    if (data_blocks_num == 0)
    {
        // write byte length of token data -------------------------------------
        write_size = 1;
        ResizeBCodeBuffer(Segment, write_size);
        Segment.pCode[Segment.BCode_Program_size] = 0;
        Segment.BCode_Program_size += write_size;
        return;
    }

    // pCompileTokenTempBuffer = (char *)RESIZE(pCompileTokenTempBuffer,data_blocks_num * (sizeof(char *) +
    // sizeof(uint32_t)));
    auto *const newPtr = new char[data_blocks_num * (sizeof(char *) + sizeof(uint32_t))];
    delete pCompileTokenTempBuffer;
    pCompileTokenTempBuffer = newPtr;

    // gather data blocks information and count total data size ----------------
    va_start(args, data_blocks_num);
    write_size = 0;
    for (n = 0; n < data_blocks_num; n++)
    {
        data_ptr = va_arg(args, char *);
        data_size = va_arg(args, uint32_t);
        memcpy(pCompileTokenTempBuffer + n * (sizeof(char *) + sizeof(uint32_t)), &data_ptr, sizeof(char *));
        memcpy(pCompileTokenTempBuffer + n * (sizeof(char *) + sizeof(uint32_t)) + sizeof(char *), &data_size,
               sizeof(uint32_t));
        write_size += data_size;
    }
    va_end(args);
    //--------------------------------------------------------------------------

    // write total length of token
    if (write_size < 0xff)
    {
        // write byte length of token data
        ResizeBCodeBuffer(Segment, 1);
        Segment.pCode[Segment.BCode_Program_size] = static_cast<uint8_t>(write_size);
        Segment.BCode_Program_size += 1;
    }
    else
    {
        // write uint32_t length of token data
        ResizeBCodeBuffer(Segment, sizeof(uint32_t) + 1);
        Segment.pCode[Segment.BCode_Program_size] = static_cast<uint8_t>(0xff);
        Segment.BCode_Program_size += 1;
        memcpy(&Segment.pCode[Segment.BCode_Program_size], &write_size, sizeof(uint32_t));
        Segment.BCode_Program_size += sizeof(uint32_t);
    }
    //--------------------------------------------------------------------------

    // write token data blocks
    ResizeBCodeBuffer(Segment, write_size);
    for (n = 0; n < data_blocks_num; n++)
    {
        memcpy(&data_ptr, pCompileTokenTempBuffer + n * (sizeof(char *) + sizeof(uint32_t)), sizeof(char *));
        memcpy(&data_size, pCompileTokenTempBuffer + n * (sizeof(char *) + sizeof(uint32_t)) + sizeof(char *),
               sizeof(uint32_t));
        memcpy(&Segment.pCode[Segment.BCode_Program_size], data_ptr, data_size);
        Segment.BCode_Program_size += data_size;
    }
}

bool COMPILER::InitInternalFunctions()
{
    FuncInfo fi;

    // register internal functions ------------

    const uint32_t internal_functions_num = GetIntFunctionsNum();
    for (uint32_t n = 0; n < internal_functions_num; n++)
    {
        fi.segment_id = INTERNAL_SEGMENT_ID;
        // fi.name = FuncNameTable[n];
        fi.name = IntFuncTable[n].pName;
        fi.offset = n;
        fi.arguments = GetInternalFunctionArgumentsNum(n);
        fi.decl_file_name = "engine";
        fi.decl_line = 0;
        fi.return_type = IntFuncTable[n].ReturnType;
        uint32_t func_code = FuncTab.AddFunc(fi);

        if (func_code == INVALID_FUNC_CODE)
        {
            func_code = FuncTab.FindFunc(fi.name);
            FuncTab.GetFunc(fi, func_code);
            // SetError("Duplicate function name: %s",fi.name);
            SetError("Function [%s] already declared in: %s line %d", fi.name.c_str(), fi.decl_file_name.c_str(),
                     fi.decl_line);
            return false;
        }
    }
    return true;
}

bool COMPILER::Compile(SEGMENT_DESC &Segment, char *pInternalCode, uint32_t pInternalCodeSize)
{
    char dbg_file_name[MAX_PATH];
    char file_name[MAX_PATH];
    char func_name[MAX_PATH];
    char var_name[MAX_PATH];
    char *pProgram;
    char *pApend_file;
    char *pSegmentSource;
    FuncInfo fi;
    VarInfo vi = {};
    const VarInfo *real_var = nullptr;
    LocalVarInfo lvi;
    uint32_t SegmentSize;
    uint32_t Program_size;
    uint32_t func_code;
    uint32_t var_code;
    uint32_t def_code;
    uint32_t inout;
    uint32_t bracket_inout;
    uint32_t n;
    uint32_t file_code;
    ptrdiff_t Control_offset;
    uint32_t Current_line;
    uint32_t Append_file_size;
    STRINGS_LIST BlockTable;
    S_TOKEN_TYPE Token_type;
    DEFINFO di;

    CompilerStage = CS_COMPILATION;

    RunningSegmentID = INVALID_SEGMENT_INDEX;

    //    bool bCDStop;
    bool bFunctionBlock;
    long lvalue;
    float fvalue;

    Control_offset = 0;
    strcpy_s(file_name, Segment.name);

    if (pInternalCode == nullptr)
    {
        Segment.Files_list->AddUnicalString(file_name);
        file_code = Segment.Files_list->GetStringCode(file_name);
        pProgram = nullptr;
        Program_size = 0;
        pSegmentSource = LoadFile(file_name, SegmentSize);
        AppendProgram(pProgram, Program_size, pSegmentSource, SegmentSize, true);
        Segment.pData = pProgram;
        if (pProgram == nullptr)
        {
            SetError("file not found: %s", file_name);
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

    // CurrentFuncCode = INVALID_FUNC_CODE;

    // register functions and variables ---------
    strcpy_s(func_name, "null");
    inout = 0;
    bracket_inout = 0;
    bFunctionBlock = false;
    // pProgram = Segment.pData;
    Token.SetProgram(pProgram, pProgram);
    if (bDebugInfo)
    {
        strcpy_s(DebugSourceFileName, Segment.name);
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
        if (Token_type == DOT)
        {
            Token_type = Token.Get();
            bDotFlag = true;
        }
        switch (Token_type)
        {
        case COMMENT:
            DebugSourceLine += Token.TokenLines();
            break;
        case LABEL:
            Token.StepBack();
            Token.StepBack();
            Token.Get(); // label name

            if (!LabelTable.AddUnicalString(Token.GetData()))
                SetError("duplicate label: %s", Token.GetData());
            Token.Get();
            break;
        case CASE_COMMAND:
            Token.Get(); // condition value
            if (Token.Get() != LABEL)
            {
                SetError("invalid case syntax");
                return false;
            }
            break;
        case SEPARATOR:
            if (bFunctionBlock && bExtern)
            {
                bFunctionBlock = false;
                bExtern = false;
            }
            if (bFunctionBlock && bImport)
            {
                bFunctionBlock = false;
                bImport = false;
            }
            break;
        case DEBUG_LINEFEED:
            DebugSourceLine++;
            break;
        case DEBUG_FILE_NAME:
            DebugSourceLine = DSL_INI_VALUE;
            strcpy_s(DebugSourceFileName, Token.GetData());
            break;
        case INCLUDE_LIBRIARY: {
            SCRIPT_LIBRIARY *pLib;
            VMA *pClass;

            //-----------------------------------------------------
            // check if already loaded
            auto name = std::string_view(Token.GetData());
            auto comparator = [name](const auto &library)
            {
                return storm::iEquals(library.name, name);
            };

            if (std::ranges::find_if(LibriaryFuncs, comparator) != LibriaryFuncs.end())
            {
                break;
            }
            //-----------------------------------------------------

            pClass = core.FindVMA(Token.GetData());
            if (!pClass)
            {
                SetWarning("cant load libriary '%s'", Token.GetData());
                break;
            }

            pLib = static_cast<SCRIPT_LIBRIARY *>(pClass->CreateClass());
            if (pLib)
                pLib->Init();

            LibriaryFuncs.emplace_back(pLib, Token.GetData());

            break;
        }
        case INCLIDE_FILE:
            if (Segment.Files_list->AddUnicalString(Token.GetData()))
            {
                file_code = Segment.Files_list->GetStringCode(Token.GetData());
                Control_offset = Token.GetProgramControl() - Token.GetProgramBase(); // store program scan point
                pApend_file = LoadFile(Token.GetData(), Append_file_size);
                if (pApend_file == nullptr)
                {
                    SetError("can't load file: %s", Token.GetData());
                    return false;
                }
                if (bDebugInfo)
                {
                    uint32_t aps;
                    strcpy_s(dbg_file_name, "#file \"");
                    strcat_s(dbg_file_name, Token.GetData());
                    strcat_s(dbg_file_name, "\";");
                    aps = strlen(dbg_file_name);
                    char *pTempS;
                    pTempS = new char[aps + 1];
                    strcpy_s(pTempS, aps + 1, dbg_file_name);
                    AppendProgram(pProgram, Program_size, pTempS, aps, false);
                }
                AppendProgram(pProgram, Program_size, pApend_file, Append_file_size, true);
                Segment.pData = pProgram;
                Token.SetProgram(pProgram, pProgram + Control_offset);
            }
            break;
        case DEFINE_COMMAND: // create define
            // DefineTable
            if (inout == 0)
            {
                strcpy_s(var_name, Token.GetData());
                di.name = var_name;
                di.segment_id = Segment.id;
                Token.Get();
                di.deftype = Token.GetType();
                bool bMinus = false;
                if (Token.GetType() == OP_MINUS)
                {
                    bMinus = true;
                    Token.Get();
                    di.deftype = Token.GetType();
                }
                di.data4b = {};
                switch (Token.GetType())
                {
                case NUMBER:
                    lvalue = static_cast<long>(atoll(Token.GetData()));
                    if (bMinus)
                        lvalue = -lvalue;
                    memcpy(&di.data4b, &lvalue, sizeof(long));
                    break;
                case FLOAT_NUMBER:
                    fvalue = static_cast<float>(atof(Token.GetData()));
                    if (bMinus)
                        fvalue = -fvalue;
                    memcpy(&di.data4b, &fvalue, sizeof(float));
                    break;
                case STRING: {
                    const auto len = strlen(Token.GetData()) + 1;
                    di.data4b = (uintptr_t) new char[len];
                    memcpy((void *)di.data4b, Token.GetData(), len);
                    break;
                }
                case OP_MINUS:

                    break;
                }
                def_code = DefTab.AddDef(di);
                if (def_code == INVALID_DEF_CODE)
                {
                    SetError("define redefinition: %s", di.name);
                    return false;
                }
            }
            break;

            /*case DEFINE_COMMAND:    // create define
              if(inout == 0)
              {
                strcpy_s(var_name,Token.GetData());
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
            if (!bExtern)
            {
                Token_type = Token.Get();
                bImport = true;
            }
        case VAR_AREFERENCE:
        case VAR_REFERENCE:
        case VAR_INTEGER:
        case VAR_PTR:
        case VAR_FLOAT:
        case VAR_STRING:
        case VAR_OBJECT:
        case TVOID:
            if (inout == 0)
            {
                if (!bFunctionBlock)
                {
                    Token.Get();
                    if (Token.GetType() == OPEN_BRACKET)
                    {
                        // this is function header
                        Token.StepBack();
                        Token.StepBack();
                        Token.Get();           // function name
                        bFunctionBlock = true; // we are stepped into func area

                        strcpy_s(func_name, Token.GetData());

                        fi.segment_id = Segment.id;
                        if (bExtern)
                            fi.offset = INVALID_FUNC_OFFSET;
                        else if (bImport)
                            fi.offset = INVALID_FUNC_OFFSET;
                        else
                            fi.offset = Segment.BCode_Program_size;
                        fi.name = func_name;
                        fi.return_type = Token_type;
                        fi.arguments = 0;
                        fi.decl_file_name = DebugSourceFileName;
                        fi.decl_line = DebugSourceLine;
                        fi.imported_func = nullptr;
                        func_code = FuncTab.AddFunc(fi);

                        if (func_code == INVALID_FUNC_CODE)
                        {
                            func_code = FuncTab.FindFunc(fi.name);
                            FuncTab.GetFunc(fi, func_code);
                            // SetError("Duplicate function name: %s",fi.name);
                            SetError("Function [%s] already declared in: %s line %d", fi.name.c_str(),
                                     fi.decl_file_name.c_str(), fi.decl_line);

                            return false;
                        }
                        break;
                    }
                    Token.StepBack();

                    // global variable
                    Token.StepBack();
                    do
                    {
                        Token.Get();
                        if (Token.GetData() == nullptr)
                        {
                            SetError("Invalid variable name");
                            return false;
                        }
                        vi.name = Token.GetData();
                        vi.segment_id = Segment.id;
                        vi.elements = 1;
                        vi.type = Token_type;
                        Token.Get();
                        if (Token.GetType() == SQUARE_OPEN_BRACKET)
                        {
                            Token.Get();
                            if (Token.GetType() != NUMBER)
                            {
                                def_code = DefTab.FindDef(Token.GetData());
                                if (def_code != INVALID_DEF_CODE)
                                {
                                    DefTab.GetDef(di, def_code);
                                    if (di.deftype == NUMBER)
                                    {
                                        lvalue = di.data4b;
                                    }
                                    else
                                    {
                                        SetError("Invalid array (%s) size", vi.name.c_str());
                                        return false;
                                    }
                                }
                                else
                                {
                                    SetError("Invalid array (%s) size", vi.name.c_str());
                                    return false;
                                }
                            }
                            else
                                lvalue = static_cast<long>(atoll(Token.GetData()));
                            vi.elements = lvalue;
                            Token.Get(); // SQUARE_CLOSE_BRACKET
                            Token.Get();
                        }
                        var_code = VarTab.AddVar(vi);
                        if (var_code == INVALID_VAR_CODE)
                        {
                            SetError("Duplicate variable name: %s", vi.name.c_str());
                            return false;
                        }

                        real_var = VarTab.GetVarX(var_code);
                        if (real_var == nullptr)
                        {
                            SetError("Registered variable %s has invalid code", vi.name.c_str());
                            return false;
                        }

                        bool bNeg;
                        if (Token.GetType() == OP_EQUAL)
                        {
                            // global var initialization
                            if (vi.elements > 1)
                            {
                                // array
                                if (SkipAuxiliaryTokens() != BLOCK_IN)
                                {
                                    SetError("Invalid array '%s' initialization", vi.name.c_str());
                                    return false;
                                }
                                uint32_t aindex;
                                // bool bNeg;
                                aindex = 0;

                                SkipAuxiliaryTokens();
                                while (Token.GetType() != END_OF_PROGRAMM)
                                {
                                    if (Token.GetType() == OP_MINUS)
                                    {
                                        bNeg = true;
                                        Token.Get();
                                    }
                                    else
                                        bNeg = false;
                                    if (Token.GetType() == BLOCK_OUT)
                                        break;
                                    if (Token.GetType() == END_OF_PROGRAMM)
                                        break;

                                    switch (Token.GetType())
                                    {
                                    case TRUE_CONST:
                                        if (vi.type != VAR_INTEGER)
                                        {
                                            SetError("Invalid array '%s' initialization parameter", vi.name.c_str());
                                            return false;
                                        }
                                        real_var->value->Set(static_cast<long>(1));
                                        break;
                                    case FALSE_CONST:
                                        if (vi.type != VAR_INTEGER)
                                        {
                                            SetError("Invalid array '%s' initialization parameter", vi.name.c_str());
                                            return false;
                                        }
                                        real_var->value->Set(static_cast<long>(0));
                                        break;

                                    case NUMBER:
                                        if (vi.type != VAR_INTEGER)
                                        {
                                            SetError("Invalid array '%s' initialization parameter", vi.name.c_str());
                                            return false;
                                        }
                                        if (bNeg)
                                            real_var->value->Set(-atol(Token.GetData()), aindex);
                                        else
                                            real_var->value->Set(static_cast<long>(atoll(Token.GetData())), aindex);
                                        aindex++;
                                        break;
                                    case FLOAT_NUMBER:
                                        if (vi.type != VAR_FLOAT)
                                        {
                                            SetError("Invalid array '%s' initialization parameter", vi.name.c_str());
                                            return false;
                                        }
                                        if (bNeg)
                                            real_var->value->Set(-static_cast<float>(atof(Token.GetData())), aindex);
                                        else
                                            real_var->value->Set(static_cast<float>(atof(Token.GetData())), aindex);
                                        aindex++;
                                        break;
                                    case STRING:
                                        if (vi.type != VAR_STRING)
                                        {
                                            SetError("Invalid array '%s' initialization parameter", vi.name.c_str());
                                            return false;
                                        }
                                        real_var->value->Set(Token.GetData(), aindex);
                                        aindex++;
                                        break;
                                    case UNKNOWN:

                                        break;
                                    default:
                                        break;
                                    }
                                    SkipAuxiliaryTokens();
                                    if (Token.GetType() == BLOCK_OUT)
                                        break;
                                    if (Token.GetType() != COMMA)
                                    {
                                        SetError("Invalid array '%s' initialization parameters list", vi.name.c_str());
                                        return false;
                                    }
                                    SkipAuxiliaryTokens();
                                }
                                SkipAuxiliaryTokens();
                            }
                            else
                            {
                                // single variable
                                if (Token.Get() == OP_MINUS)
                                {
                                    bNeg = true;
                                    Token.Get();
                                }
                                else
                                    bNeg = false;

                                switch (Token.GetType())
                                {
                                case TRUE_CONST:
                                    if (vi.type != VAR_INTEGER)
                                        break;
                                    real_var->value->Set(static_cast<long>(1));
                                    break;
                                case FALSE_CONST:
                                    if (vi.type != VAR_INTEGER)
                                        break;
                                    real_var->value->Set(static_cast<long>(0));
                                    break;
                                case NUMBER:
                                    if (vi.type != VAR_INTEGER)
                                        break;
                                    if (bNeg)
                                        real_var->value->Set(-atol(Token.GetData()));
                                    else
                                        real_var->value->Set(static_cast<long>(atoll(Token.GetData())));
                                    break;
                                case FLOAT_NUMBER:
                                    if (vi.type != VAR_FLOAT)
                                        break;
                                    if (bNeg)
                                        real_var->value->Set(-static_cast<float>(atof(Token.GetData())));
                                    else
                                        real_var->value->Set(static_cast<float>(atof(Token.GetData())));
                                    break;
                                case STRING:
                                    if (vi.type != VAR_STRING)
                                        break;
                                    real_var->value->Set(Token.GetData());
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
                // function argument list
                // compute number of arguments
                // if(bImport || !bExtern)
                {
                    lvi.type = Token_type;
                    lvi.name = Token.GetData();
                    lvi.elements = 1;
                    if (bExtern)
                        FuncTab.AddFuncArg(func_code, lvi, true);
                    else
                        FuncTab.AddFuncArg(func_code, lvi);
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
                    if (Token.GetData() == nullptr)
                    {
                        SetError("Invalid variable name");
                        return false;
                    }
                    lvi.name = Token.GetData();
                    lvi.elements = 1;
                    Token.Get();
                    if (Token.GetType() == SQUARE_OPEN_BRACKET)
                    {
                        Token.Get();
                        if (Token.GetType() != NUMBER)
                        {
                            def_code = DefTab.FindDef(Token.GetData());
                            if (def_code != INVALID_DEF_CODE)
                            {
                                DefTab.GetDef(di, def_code);
                                if (di.deftype == NUMBER)
                                {
                                    lvalue = di.data4b;
                                }
                                else
                                {
                                    SetError("Invalid array (%s) size", lvi.name.c_str());
                                    return false;
                                }
                            }
                            else
                            {
                                SetError("Invalid array (%s) size", lvi.name.c_str());
                                return false;
                            }
                        }
                        else
                            lvalue = static_cast<long>(atoll(Token.GetData()));
                        lvi.elements = lvalue;
                        Token.Get(); // SQUARE_CLOSE_BRACKET
                        Token.Get();
                    }

                    if (lvi.name.empty())
                    {
                        SetWarning("Empty variable name");
                    }
                    else if (!FuncTab.AddFuncVar(func_code, lvi))
                    {
                        SetError("Duplicate variable name: %s", lvi.name.c_str());
                        return false;
                    }
                } while (Token.GetType() == COMMA);
                Token.StepBack();
            }
            break;

        case BLOCK_IN:
            inout++;
            break;
        case BLOCK_OUT:
            inout--;
            if (inout == 0)
                bFunctionBlock = false; // sign if we are out of function
            break;
        default:
            break;
        }
    } while (Token_type != END_OF_PROGRAMM);

    if (inout != 0)
    {
        SetError("Missing '}' in segment");
        return false;
    }

    //-----------------------------------------

    Current_line = 1;
    // Current_line = DSL_INI_VALUE;
    DebugSourceLine = DSL_INI_VALUE;
    uint32_t fnsize;

    if (bDebugInfo)
    {
        strcpy_s(DebugSourceFileName, Segment.name);
        fnsize = strlen(Segment.name);
        CompileToken(Segment, DEBUG_FILE_NAME, 3, (char *)&DebugSourceLine, sizeof(uint32_t), (char *)&fnsize,
                     sizeof(uint32_t), Segment.name, strlen(Segment.name));
        CompileToken(Segment, DEBUG_LINE_CODE, 1, (char *)&DebugSourceLine, sizeof(uint32_t));
        // DebugSourceLine++;
    }

    uint32_t block_io_code;
    block_io_code = 0;
    inout = 0;
    bracket_inout = 0;
    bFunctionBlock = false;
    pProgram = Segment.pData;
    Token.SetProgram(pProgram, pProgram);

    STRINGS_LIST BreakUpdateTable;
    if (!CompileBlock(Segment, bFunctionBlock, inout, TVOID, INVALID_OFFSET, INVALID_OFFSET, BreakUpdateTable))
        return false;
    DOUBLE_DWORD ddw;
    uint32_t label_offset;
    for (n = 0; n < LabelUpdateTable.GetStringsCount(); n++)
    {
        LabelUpdateTable.GetStringData(n, &ddw);
        LabelTable.GetStringData(ddw.dw1, &label_offset);
        memcpy(&Segment.pCode[ddw.dw2], &label_offset, sizeof(uint32_t));
    }

    if (pInternalCode == nullptr)
    {
        delete Segment.pData;
        Segment.pData = nullptr;
    }
    HANDLE fh;
    uint32_t dwR;
    if (bWriteCodeFile)
    {
        _splitpath(Segment.name, nullptr, nullptr, file_name, nullptr);
        strcat_s(file_name, ".b");
        std::wstring FileNameW = utf8::ConvertUtf8ToWide(file_name);
        fh = CreateFile(FileNameW.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL, nullptr);
        if (fh != INVALID_HANDLE_VALUE)
        {
            WriteFile(fh, Segment.pCode, Segment.BCode_Program_size, (LPDWORD)&dwR, nullptr);
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
        switch (Token_type)
        {
        case DEBUG_LINEFEED:
        case DEBUG_FILE_NAME:
        case COMMENT:
        case INCLIDE_FILE:
        case WHITESPACE:
            break;
        default:
            return Token_type;
        }
    } while (Token_type != END_OF_PROGRAMM);
    return Token_type;
}

S_TOKEN_TYPE COMPILER::CompileAuxiliaryTokens(SEGMENT_DESC &Segment) //, bool & bFunctionBlock, DWORD & inout)
{
    uint32_t fnsize;
    S_TOKEN_TYPE Token_type;

    do
    {
        Token_type = Token.Get();
        switch (Token_type)
        {
        case DEBUG_LINEFEED:
            if (!bDebugInfo)
                break;
            DebugSourceLine++;
            CompileToken(Segment, DEBUG_LINE_CODE, 1, (char *)&DebugSourceLine, sizeof(uint32_t));
            break;
        case DEBUG_FILE_NAME:
            if (!bDebugInfo)
                break;
            DebugSourceLine = DSL_INI_VALUE;
            if (Token.GetData() == nullptr)
            {
                SetError("Invalid DBG_FILE_NAME");
                return DEBUG_FILE_NAME;
            }
            strcpy_s(DebugSourceFileName, Token.GetData());
            fnsize = strlen(Token.GetData());

            CompileToken(Segment, DEBUG_FILE_NAME, 3, (char *)&DebugSourceLine, sizeof(uint32_t), (char *)&fnsize,
                         sizeof(uint32_t), Token.GetData(), strlen(Token.GetData()));
            break;
        case COMMENT:
            DebugSourceLine += Token.TokenLines();
            break;
        case INCLIDE_FILE:
        case WHITESPACE:
            // skip compiling of auxiliary tokens
            break;
        default:
            return Token_type;
        }
    } while (Token_type != END_OF_PROGRAMM);
    return Token_type;
}

void COMPILER::CopyOffsets(SEGMENT_DESC &Segment, STRINGS_LIST &srclist, STRINGS_LIST &dstlist, const char *sname)
{
    uint32_t n;
    uint32_t i;
    uint32_t jump_voffset;
    if (sname == nullptr)
    {
        for (n = 0; n < srclist.GetStringsCount(); n++)
        {
            srclist.GetStringData(n, (char *)&jump_voffset);

            i = dstlist.GetStringsCount();
            dstlist.AddString("c");
            dstlist.SetStringData(i, (char *)&jump_voffset);
        }
        return;
    }

    for (n = 0; n < srclist.GetStringsCount(); n++)
    {
        if (strcmp(sname, srclist.GetString(n)) != 0)
            continue;

        srclist.GetStringData(n, (char *)&jump_voffset);

        i = dstlist.GetStringsCount();
        dstlist.AddString("c");
        dstlist.SetStringData(i, (char *)&jump_voffset);
    }
}

void COMPILER::UpdateOffsets(SEGMENT_DESC &Segment, STRINGS_LIST &list, uint32_t offset, const char *sname)
{
    uint32_t n;
    uint32_t jump_voffset;
    if (sname == nullptr)
    {
        for (n = 0; n < list.GetStringsCount(); n++)
        {
            list.GetStringData(n, (char *)&jump_voffset);
            memcpy(&Segment.pCode[jump_voffset], &offset, sizeof(uint32_t));
        }
        return;
    }
    for (n = 0; n < list.GetStringsCount(); n++)
    {
        if (strcmp(sname, list.GetString(n)) != 0)
            continue;
        list.GetStringData(n, (char *)&jump_voffset);
        memcpy(&Segment.pCode[jump_voffset], &offset, sizeof(uint32_t));
    }
}

bool COMPILER::CompileBlock(SEGMENT_DESC &Segment, bool &bFunctionBlock, uint32_t &inout, S_TOKEN_TYPE bound_type,
                            uint32_t continue_jump, uint32_t break_offset, STRINGS_LIST &BreakTable)
{
    S_TOKEN_TYPE Token_type;
    //    S_TOKEN_TYPE Block_bound;
    //    DWORD fnsize;
    uint32_t awcode;
    uint32_t var_code;
    uint32_t balance_bracket;
    uint32_t balance_block;
    uint32_t jump_offset;
    uint32_t update_offset;
    uint32_t vuint32_t;
    uint32_t n;
    uint32_t unk_code;
    uint32_t func_args;
    STRINGS_LIST BreakUpdateTable;
    BreakUpdateTable.SetStringDataSize(sizeof(uint32_t));
    bool bExtern;
    bool bImport;
    long lvalue;
    uint32_t def_code;
    DEFINFO di;
    uint32_t dwRCode;

    const VarInfo *real_var;
    LocalVarInfo lvi;

    jump_offset = 0xffbadbad;

    balance_block = 0;
    balance_bracket = 0;

    switch (bound_type)
    {
    case CLOSE_BRACKET:
        balance_bracket = 1;
        break;
    case BLOCK_OUT:
        balance_block = 1;
        break;
    }

    bExtern = false;
    bImport = false;
    do
    {
        // Token_type = Token.Get();
        Token_type = CompileAuxiliaryTokens(Segment /*,bFunctionBlock,inout*/);

        switch (Token_type)
        {
        case EVENT_HANDLER:
            if (Token.Get() != OPEN_BRACKET)
            {
                SetError("Invalid event handler");
                return false;
            }

            if (Token.Get() == STRING)
            {
                if (Token.GetData() == nullptr)
                {
                    SetError("Invalid event handler");
                    return false;
                }
                strcpy_s(gs, Token.GetData());
            }
            else
            {
                if (Token.GetData() == nullptr)
                {
                    SetError("Invalid event handler");
                    return false;
                }
                def_code = DefTab.FindDef(Token.GetData());
                if (def_code != INVALID_DEF_CODE)
                {
                    DefTab.GetDef(di, def_code);
                    if (di.deftype != STRING)
                    {
                        SetError("Invalid event handler");
                        return false;
                    }
                    if (di.data4b == 0)
                    {
                        SetError("Invalid define");
                        return false;
                    }
                    strcpy_s(gs, (char *)di.data4b);
                }
                else
                {
                    SetError("Invalid event handler");
                    return false;
                }
            }
            if (Token.Get() != COMMA)
            {
                SetError("Invalid event handler");
                return false;
            }
            if (Token.Get() == STRING)
            {
                if (Token.GetData() == nullptr)
                {
                    SetError("Invalid event handler");
                    return false;
                }
                // SetEventHandler(gs,Token.GetData(),1,true);
                SetEventHandler(gs, Token.GetData(), 0, true);
            }
            else
            {
                SetError("Invalid event handler");
                return false;
            }

            if (Token.Get() != CLOSE_BRACKET)
            {
                SetError("Invalid event handler");
                return false;
            }
            break;

        case GOTO_COMMAND:
            // CompileToken(Segment,Token_type,1,Token.GetData(),strlen(Token.GetData())+1);
            // LabelUpdateTable
            Token.Get(); // label name
            DOUBLE_DWORD ddw;
            ddw.dw1 = LabelTable.GetStringCode(Token.GetData());
            n = LabelUpdateTable.GetStringsCount();
            LabelUpdateTable.AddString("j");
            ddw.dw2 = Segment.BCode_Program_size + 2;
            LabelUpdateTable.SetStringData(n, (char *)&ddw);
            vuint32_t = 0xffffffff;
            CompileToken(Segment, JUMP, 1, (char *)&vuint32_t, sizeof(uint32_t));
            break;
        case BREAK_COMMAND:
            if (break_offset == INVALID_OFFSET)
            {
                SetError("invalid break");
                return false;
            }
            vuint32_t = Segment.BCode_Program_size + 2;
            n = BreakTable.GetStringsCount();
            BreakTable.AddString("b");
            BreakTable.SetStringData(n, (char *)&vuint32_t);
            vuint32_t = INVALID_OFFSET;
            CompileToken(Segment, JUMP, 1, (char *)&vuint32_t, sizeof(uint32_t));
            if (bound_type == BREAK_COMMAND)
            {
                return true;
            }
            break;
        case CONTINUE_COMMAND:
            if (continue_jump == INVALID_OFFSET)
            {
                SetError("invalid continue");
                return false;
            }
            vuint32_t = Segment.BCode_Program_size + 2;
            n = BreakTable.GetStringsCount();
            BreakTable.AddString("c");
            BreakTable.SetStringData(n, (char *)&vuint32_t);
            vuint32_t = INVALID_OFFSET;
            CompileToken(Segment, JUMP, 1, (char *)&vuint32_t, sizeof(uint32_t));
            // CompileToken(Segment,JUMP,1,(char *)&continue_jump,sizeof(uint32_t));
            break;
        case SEPARATOR:
            CompileToken(Segment, Token_type);
            if (bound_type == SEPARATOR)
            {
                return true;
            }
            break;
        case OPEN_BRACKET:
            balance_bracket++;
            CompileToken(Segment, Token_type);
            break;
        case CLOSE_BRACKET:
            balance_bracket--;
            CompileToken(Segment, Token_type);
            if (bound_type == CLOSE_BRACKET)
            {
                if (balance_bracket == 0)
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

            if (inout == 0)
            {
                SetError("syntax error");
                return false;
            }
            inout--;

            if (balance_block == 0)
            {
                switch (bound_type)
                {
                case CLOSE_BRACKET:
                    SetError("missed ')'");
                    break;
                case BLOCK_OUT:
                    SetError("missed '}'");
                    break;
                default:
                    SetError("syntax error");
                    break;
                }
                return false;
            }
            balance_block--;
            if (inout == 0)
            {
                CompileToken(Segment, FUNCTION_RETURN_VOID);
                bFunctionBlock = false; // sign if we are out of function
                break;
            }
            if (bound_type == BLOCK_OUT)
            {
                if (balance_block == 0)
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
                  if(!CompileBlock(Segment,bFunctionBlock,inout,CLOSE_BRACKET,INVALID_OFFSET,INVALID_OFFSET,BreakTable))
               return false; CompileToken(Segment,PUSH_EXPRESULT);    // store for possible else

                  update_offset = Segment.BCode_Program_size + 2;
                  CompileToken(Segment,JUMP_Z,1,(char *)&jump_offset,sizeof(uint32_t));
                  Token_type = CompileAuxiliaryTokens(Segment);
                  if(Token_type == BLOCK_IN)
                  {
                    inout++;
                    if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,continue_jump,break_offset,BreakTable))
               return false;
                  }
                  else
                  {
                    Token.StepBack();
                    if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,continue_jump,break_offset,BreakTable))
               return false;
                  }
                  memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(uint32_t));
                  CompileToken(Segment,POP_EXPRESULT);    // pop
                break;
                case ELSE_BLOCK:
                  update_offset = Segment.BCode_Program_size + 2;
                  CompileToken(Segment,JUMP_NZ,1,(char *)&jump_offset,sizeof(uint32_t));
                  Token_type = CompileAuxiliaryTokens(Segment);
                  if(Token_type == BLOCK_IN)
                  {
                    inout++;
                    if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,continue_jump,break_offset,BreakTable))
               return false;
                  }
                  else
                  {
                    Token.StepBack();
                    if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,continue_jump,break_offset,BreakTable))
               return false;
                  }
                  memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(uint32_t));
                break;
                case WHILE_BLOCK:
                  BreakUpdateTable.Release();
                  jump_offset = Segment.BCode_Program_size;
                  CompileToken(Segment,Token_type);
                  Token_type = CompileAuxiliaryTokens(Segment);
                  if(Token_type != OPEN_BRACKET){SetError("missed '('"); return false;}
                  CompileToken(Segment,OPEN_BRACKET);
                  if(!CompileBlock(Segment,bFunctionBlock,inout,CLOSE_BRACKET,INVALID_OFFSET,INVALID_OFFSET,BreakUpdateTable))
               return false;

                  update_offset = Segment.BCode_Program_size + 2;
                  CompileToken(Segment,JUMP_Z,1,(char *)&jump_offset,sizeof(uint32_t));
                  Token_type = CompileAuxiliaryTokens(Segment);
                  if(Token_type == BLOCK_IN)
                  {
                    inout++;
                    if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,jump_offset,6,BreakUpdateTable)) return
               false;
                  }
                  else { SetError("missed '{'");    return false; }
                  CompileToken(Segment,JUMP,1,(char *)&jump_offset,sizeof(uint32_t));
                  memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(uint32_t));
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
                  if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,INVALID_OFFSET,INVALID_OFFSET,BreakTable))
               return false;

                  jump_offset = Segment.BCode_Program_size;
                  // condition
                  CompileToken(Segment,PROCESS_EXPRESSION);
                  if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,INVALID_OFFSET,INVALID_OFFSET,BreakTable))
               return false;
                  // jump if zero
                  update_offset = Segment.BCode_Program_size + 2;
                  CompileToken(Segment,JUMP_Z,1,(char *)&jump_offset,sizeof(uint32_t));


                  S_TOKEN_TYPE sttResult;    // var for inc/dec op
                  S_TOKEN_TYPE forOp;
                  if(Token.Get()!=UNKNOWN) {SetError("invalid 'for' syntax");return false;};

                  sttResult = DetectUnknown(var_code);
                  if(!(sttResult == VARIABLE || sttResult == LOCAL_VARIABLE)) {SetError("invalid 'for' syntax");return
               false;};

                  forOp = Token.Get();
                  if(!(forOp == OP_INC || forOp == OP_DEC)) {SetError("invalid 'for' syntax");return false;};
                  if(CompileAuxiliaryTokens(Segment) != CLOSE_BRACKET) {SetError("invalid 'for' syntax");return false;};

                  Token_type = CompileAuxiliaryTokens(Segment);
                  if(Token_type == BLOCK_IN)
                  {
                    inout++;
                    if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,jump_offset,6,BreakUpdateTable)) return
               false;
                  }
                  else { SetError("missed '{'");    return false; }

                  DWORD forcont_offset;
                  forcont_offset = Segment.BCode_Program_size;
                  CompileToken(Segment,sttResult,1,(char *)&var_code,sizeof(uint32_t));
                  CompileToken(Segment,forOp);
                  CompileToken(Segment,JUMP,1,(char *)&jump_offset,sizeof(uint32_t));
                  memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(uint32_t));
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
                  if(!CompileBlock(Segment,bFunctionBlock,inout,CLOSE_BRACKET,INVALID_OFFSET,INVALID_OFFSET,BreakUpdateTable))
               return false; CompileToken(Segment,PUSH_EXPRESULT); Token_type = CompileAuxiliaryTokens(Segment);
                  if(Token_type == BLOCK_IN)
                  {
                    inout++;
                    //if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,jump_offset,6,BreakUpdateTable)) return
               false; if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,continue_jump,6,BreakUpdateTable)) return
               false;
                  }
                  else { SetError("missed '{'");    return false; }
                  UpdateOffsets(Segment,BreakUpdateTable,Segment.BCode_Program_size,"b");
                  CopyOffsets(Segment,BreakUpdateTable,BreakTable,"c");
                  //UpdateOffsets(Segment,BreakUpdateTable,continue_jump,"c");    // -> ***
                  CompileToken(Segment,POP_VOID);
                  BreakUpdateTable.Release();
                break;
                case CASE_COMMAND:
                  //CompileToken(Segment,Token_type);
                  Token_type = CompileAuxiliaryTokens(Segment);
                  CompileToken(Segment,PROCESS_EXPRESSION);
                  switch(Token_type)
                  {
                    case UNKNOWN:    CompileUnknown(Segment); break;
                    case NUMBER:    CompileNumber(Segment); break;
                    case FLOAT_NUMBER: CompileFloatNumber(Segment); break;
                    case STRING:    CompileString(Segment); break;
                    break;
                    default: SetError("invalid case"); return false;
                  }
                  if(Token.Get() != LABEL){ SetError("missed ':'");    return false; }
                  CompileToken(Segment,SEPARATOR);
                  CompileToken(Segment,STACK_COMPARE);

                  update_offset = Segment.BCode_Program_size + 2;
                  CompileToken(Segment,JUMP_Z,1,(char *)&jump_offset,sizeof(uint32_t));

                  //if(!CompileBlock(Segment,bFunctionBlock,inout,BREAK_COMMAND,INVALID_OFFSET,0,BreakTable)) return
               false;
                  // ??? why INVALID_OFFSET on continue_jump here? dont remember; anyway now change to normal
                  if(!CompileBlock(Segment,bFunctionBlock,inout,BREAK_COMMAND,continue_jump,0,BreakTable)) return false;

                  memcpy(&Segment.pCode[update_offset],&Segment.BCode_Program_size,sizeof(uint32_t));
                break;

            */
        case IF_BLOCK:

            Token_type = CompileAuxiliaryTokens(Segment);
            if (Token_type != OPEN_BRACKET)
            {
                SetError("missed '('");
                return false;
            }

            CompileExpression(Segment);

            if (Token.GetType() != CLOSE_BRACKET)
            {
                SetError("missed ')'");
                return false;
            }

            CompileToken(Segment, STACK_READ);
            CompileToken(Segment, EX);

            update_offset = Segment.BCode_Program_size + 2;
            CompileToken(Segment, JUMP_Z, 1, (char *)&jump_offset, sizeof(uint32_t));
            Token_type = CompileAuxiliaryTokens(Segment);
            if (Token_type == BLOCK_IN)
            {
                inout++;
                if (!CompileBlock(Segment, bFunctionBlock, inout, BLOCK_OUT, continue_jump, break_offset, BreakTable))
                    return false;
            }
            else
            {
                Token.StepBack();
                if (!CompileBlock(Segment, bFunctionBlock, inout, SEPARATOR, continue_jump, break_offset, BreakTable))
                    return false;
            }
            memcpy(&Segment.pCode[update_offset], &Segment.BCode_Program_size, sizeof(uint32_t));
            // CompileToken(Segment,STACK_POP);
            // CompileToken(Segment,EX);

            CompileToken(Segment, POP_EXPRESULT); // pop
            break;
        case ELSE_BLOCK:
            update_offset = Segment.BCode_Program_size + 2;
            CompileToken(Segment, JUMP_NZ, 1, (char *)&jump_offset, sizeof(uint32_t));
            Token_type = CompileAuxiliaryTokens(Segment /*,bFunctionBlock,inout*/);
            if (Token_type == BLOCK_IN)
            {
                inout++;
                if (!CompileBlock(Segment, bFunctionBlock, inout, BLOCK_OUT, continue_jump, break_offset, BreakTable))
                    return false;
            }
            else
            {
                Token.StepBack();
                if (!CompileBlock(Segment, bFunctionBlock, inout, SEPARATOR, continue_jump, break_offset, BreakTable))
                    return false;
            }
            memcpy(&Segment.pCode[update_offset], &Segment.BCode_Program_size, sizeof(uint32_t));
            break;

        case WHILE_BLOCK:
            BreakUpdateTable.Release();
            jump_offset = Segment.BCode_Program_size;

            // CompileToken(Segment,Token_type);

            Token_type = CompileAuxiliaryTokens(Segment);
            if (Token_type != OPEN_BRACKET)
            {
                SetError("missed '('");
                return false;
            }
            // CompileToken(Segment,OPEN_BRACKET);
            CompileExpression(Segment);
            CompileToken(Segment, STACK_POP);
            CompileToken(Segment, EX);

            if (Token.GetType() != CLOSE_BRACKET)
            {
                SetError("missed ')'");
                return false;
            }

            // if(!CompileBlock(Segment,bFunctionBlock,inout,CLOSE_BRACKET,INVALID_OFFSET,INVALID_OFFSET,BreakUpdateTable))
            // return false;

            update_offset = Segment.BCode_Program_size + 2;
            CompileToken(Segment, JUMP_Z, 1, (char *)&jump_offset, sizeof(uint32_t));
            Token_type = CompileAuxiliaryTokens(Segment);
            if (Token_type == BLOCK_IN)
            {
                inout++;
                if (!CompileBlock(Segment, bFunctionBlock, inout, BLOCK_OUT, jump_offset, 6, BreakUpdateTable))
                    return false;
            }
            else
            {
                // SetError("missed '{'");    return false;
                Token.StepBack();
                if (!CompileBlock(Segment, bFunctionBlock, inout, SEPARATOR, jump_offset, 6, BreakUpdateTable))
                    return false;
                // **** ?
            }
            CompileToken(Segment, JUMP, 1, (char *)&jump_offset, sizeof(uint32_t));
            memcpy(&Segment.pCode[update_offset], &Segment.BCode_Program_size, sizeof(uint32_t));
            UpdateOffsets(Segment, BreakUpdateTable, Segment.BCode_Program_size, "b");
            UpdateOffsets(Segment, BreakUpdateTable, jump_offset, "c");
            BreakUpdateTable.Release();
            break;
        case FOR_BLOCK:
            BreakUpdateTable.Release();

            // '('
            Token_type = CompileAuxiliaryTokens(Segment);
            if (Token_type != OPEN_BRACKET)
            {
                SetError("missed '('");
                return false;
            }

            // var = initial_value
            if (!CompileBlock(Segment, bFunctionBlock, inout, SEPARATOR, INVALID_OFFSET, INVALID_OFFSET, BreakTable))
                return false;

            jump_offset = Segment.BCode_Program_size;

            // condition
            // CompileToken(Segment,PROCESS_EXPRESSION);
            // if(!CompileBlock(Segment,bFunctionBlock,inout,SEPARATOR,INVALID_OFFSET,INVALID_OFFSET,BreakTable)) return
            // false;
            CompileExpression(Segment);
            CompileToken(Segment, STACK_POP);
            CompileToken(Segment, EX);

            // jump if zero
            update_offset = Segment.BCode_Program_size + 2;
            CompileToken(Segment, JUMP_Z, 1, (char *)&jump_offset, sizeof(uint32_t));

            S_TOKEN_TYPE sttResult; // var for inc/dec op
            S_TOKEN_TYPE forOp;
            if (Token.Get() != UNKNOWN)
            {
                SetError("invalid 'for' syntax");
                return false;
            }

            sttResult = DetectUnknown(var_code);
            if (!(sttResult == VARIABLE || sttResult == LOCAL_VARIABLE))
            {
                SetError("invalid 'for' syntax");
                return false;
            }

            forOp = Token.Get();
            if (!(forOp == OP_INC || forOp == OP_DEC))
            {
                SetError("invalid 'for' syntax");
                return false;
            }
            if (CompileAuxiliaryTokens(Segment) != CLOSE_BRACKET)
            {
                SetError("invalid 'for' syntax");
                return false;
            }

            Token_type = CompileAuxiliaryTokens(Segment);
            if (Token_type == BLOCK_IN)
            {
                inout++;
                if (!CompileBlock(Segment, bFunctionBlock, inout, BLOCK_OUT, jump_offset, 6, BreakUpdateTable))
                    return false;
            }
            else
            {
                Token.StepBack();
                if (!CompileBlock(Segment, bFunctionBlock, inout, SEPARATOR, jump_offset, 6, BreakUpdateTable))
                    return false;
                // SetError("missed '{'");    return false;
                // **** ?
            }

            uint32_t forcont_offset;
            forcont_offset = Segment.BCode_Program_size;
            CompileToken(Segment, sttResult, 1, (char *)&var_code, sizeof(uint32_t));
            CompileToken(Segment, forOp);
            CompileToken(Segment, JUMP, 1, (char *)&jump_offset, sizeof(uint32_t));
            memcpy(&Segment.pCode[update_offset], &Segment.BCode_Program_size, sizeof(uint32_t));
            UpdateOffsets(Segment, BreakUpdateTable, Segment.BCode_Program_size, "b");
            UpdateOffsets(Segment, BreakUpdateTable, forcont_offset, "c");
            BreakUpdateTable.Release();
            break;
        case SWITCH_COMMAND:
            BreakUpdateTable.Release();
            // CompileToken(Segment,Token_type);
            Token_type = CompileAuxiliaryTokens(Segment);
            if (Token_type != OPEN_BRACKET)
            {
                SetError("missed '('");
                return false;
            }

            // CompileToken(Segment,OPEN_BRACKET);

            // if(!CompileBlock(Segment,bFunctionBlock,inout,CLOSE_BRACKET,INVALID_OFFSET,INVALID_OFFSET,BreakUpdateTable))
            // return false; CompileToken(Segment,PUSH_EXPRESULT);
            CompileExpression(Segment);

            Token_type = CompileAuxiliaryTokens(Segment);
            if (Token_type == BLOCK_IN)
            {
                inout++;
                // if(!CompileBlock(Segment,bFunctionBlock,inout,BLOCK_OUT,jump_offset,6,BreakUpdateTable)) return
                // false;
                if (!CompileBlock(Segment, bFunctionBlock, inout, BLOCK_OUT, continue_jump, 6, BreakUpdateTable))
                    return false;
            }
            else
            {
                SetError("missed '{'");
                return false;
            }
            UpdateOffsets(Segment, BreakUpdateTable, Segment.BCode_Program_size, "b");
            CopyOffsets(Segment, BreakUpdateTable, BreakTable, "c");
            // UpdateOffsets(Segment,BreakUpdateTable,continue_jump,"c");    // -> ***
            CompileToken(Segment, POP_VOID);
            BreakUpdateTable.Release();
            break;
        case CASE_COMMAND:
            // CompileToken(Segment,Token_type);
            // Token_type = CompileAuxiliaryTokens(Segment);
            CompileExpression(Segment);
            CompileToken(Segment, STACK_POP);
            CompileToken(Segment, EX);

            /*CompileToken(Segment,PROCESS_EXPRESSION);
            switch(Token_type)
            {
              case UNKNOWN:    CompileUnknown(Segment); break;
              case NUMBER:    CompileNumber(Segment); break;
              case FLOAT_NUMBER: CompileFloatNumber(Segment); break;
              case STRING:    CompileString(Segment); break;
              break;
              default: SetError("invalid case"); return false;
            }

            if(Token.Get() != LABEL){ SetError("missed ':'");    return false; }*/

            if (Token.GetType() != LABEL)
            {
                SetError("missed ':'");
                return false;
            }

            // CompileToken(Segment,SEPARATOR);
            CompileToken(Segment, STACK_COMPARE);

            update_offset = Segment.BCode_Program_size + 2;
            CompileToken(Segment, JUMP_Z, 1, (char *)&jump_offset, sizeof(uint32_t));

            // if(!CompileBlock(Segment,bFunctionBlock,inout,BREAK_COMMAND,INVALID_OFFSET,0,BreakTable)) return false;
            // ??? why INVALID_OFFSET on continue_jump here? dont remember; anyway now change to normal
            if (!CompileBlock(Segment, bFunctionBlock, inout, BREAK_COMMAND, continue_jump, 0, BreakTable))
                return false;

            memcpy(&Segment.pCode[update_offset], &Segment.BCode_Program_size, sizeof(uint32_t));
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
              CompileToken(Segment,Token_type,1,(char *)&unk_code,sizeof(uint32_t));
            break;*/

        case CALL:

            S_TOKEN_TYPE sttVar;
            uint32_t dwCallVarCode;

            // CompileToken(Segment,Token_type);
            Token_type = CompileAuxiliaryTokens(Segment);
            if (Token_type != UNKNOWN)
            {
                SetError("invalid dynamic call");
                return false;
            }
            sttVar = DetectUnknown(dwCallVarCode);
            if (!(sttVar == VARIABLE || sttVar == LOCAL_VARIABLE))
            {
                SetError("invalid dynamic call");
                return false;
            }
            //======================================================

            if (CompileAuxiliaryTokens(Segment) != OPEN_BRACKET)
            {
                SetError("missing '('");
                return false;
            }
            if (Token.Get() == CLOSE_BRACKET)
            {
                // function w/o arguments
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
                    switch (Token.GetType())
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
                } while (bNext);
            }
            // CompileToken(Segment,CALL_FUNCTION,1,(char *)&func_code,sizeof(uint32_t));
            // CompileToken(Segment,ARGS_NUM,1,(char *)&func_args,sizeof(DWORD));
            // if(fi.return_type!=TVOID) CompileToken(Segment,STACK_POP_VOID);
            //======================================================

            CompileToken(Segment, CALL);
            CompileToken(Segment, Token_type, 1, (char *)&dwCallVarCode, sizeof(uint32_t));
            CompileToken(Segment, ARGS_NUM, 1, (char *)&func_args, sizeof(uint32_t));
            CompileToken(Segment, POP_NZ); // pop return value if function returned value

            break;

        case UNKNOWN: {
            uint32_t func_code;
            uint32_t func_args;
            FuncInfo fi;

            if (DetectUnknown(func_code) == CALL_FUNCTION)
            {
                if (CompileAuxiliaryTokens(Segment) != OPEN_BRACKET)
                {
                    SetError("missing '('");
                    return false;
                }
                if (Token.Get() == CLOSE_BRACKET)
                {
                    // function w/o arguments
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
                        switch (Token.GetType())
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
                    } while (bNext);
                }

                if (!FuncTab.GetFuncX(fi, func_code))
                {
                    SetError("function not found");
                    return false;
                }

                if (fi.offset == INVALID_FUNC_OFFSET)
                {
                    // external function declared but not compiled yet
                    fi.arguments = fi.extern_arguments;
                }

                // off for debug needs

                if (fi.arguments != func_args)
                {
                    switch (fi.segment_id)
                    {
                    case INTERNAL_SEGMENT_ID:
                        if (!IsIntFuncVarArgsNum(func_code))
                        {
                            SetError("function '%s(args:%d)' doesnt accept %d arguments", fi.name.c_str(), fi.arguments,
                                     func_args);
                            return false;
                        }
                        break;
                    case IMPORTED_SEGMENT_ID:
                        // skip imported funcs checking for now
                        break;
                    default:
                        SetError("function %s(args:%d) doesnt accept %d arguments", fi.name.c_str(), fi.arguments,
                                 func_args);
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
                } //*/

                CompileToken(Segment, CALL_FUNCTION, 1, (char *)&func_code, sizeof(uint32_t));
                CompileToken(Segment, ARGS_NUM, 1, (char *)&func_args, sizeof(uint32_t));
                if (fi.return_type != TVOID)
                    CompileToken(Segment, STACK_POP_VOID);
            }
            else if (!CompileUnknown(Segment))
                return false;
        }
        break;

        case EXTERN:
            Token_type = Token.Get();
            bExtern = true;
        case IMPORT:
            if (!bExtern)
            {
                Token_type = Token.Get();
                bImport = true;
            }
        case VAR_REFERENCE:
        case VAR_AREFERENCE:
        case VAR_INTEGER: // create global variable
        case VAR_PTR:
        case VAR_FLOAT:
        case VAR_STRING:
        case VAR_OBJECT:
        case TVOID:
            // DWORD unk_code;
            Token_type = DetectUnknown(unk_code);
            if (Token_type == CALL_FUNCTION)
            {
                // function declaration

                bFunctionBlock = true;

                CurrentFuncCode = FuncTab.FindFunc(Token.GetData());
                if (!bExtern)
                    if (!bImport)
                        if (!FuncTab.SetFuncOffset(Token.GetData(), Segment.BCode_Program_size))
                        {
                            SetError("Invalid function name: %s", Token.GetData());
                            return false;
                        }
                bExtern = false;
                bImport = false;
                // skip function arguments list in function declaration
                if (Token.Get() != OPEN_BRACKET)
                {
                    SetError("Invalid syntax");
                    return false;
                }

                do
                {
                    Token_type = Token.Get();
                    switch (Token_type)
                    {
                    case VAR_INTEGER:
                    case VAR_PTR:
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
                    default:
                        SetError("Invalid function declaration");
                        return false;
                    }
                } while (Token_type != CLOSE_BRACKET);
            }
            else
            {
                if (Token_type == LOCAL_VARIABLE)
                {
                    if (Token.Get() == OP_EQUAL)
                    {
                        // initialization
                        CompileToken(Segment, Token_type, 1, (char *)&unk_code, sizeof(uint32_t));
                    }
                    Token.StepBack();
                }
            }
            // ... skip var or func declaration
            break;

        case FUNCTION_RETURN:
        case FUNCTION_RETURN_VOID:
            Token_type = Token.Get();
            if (Token_type == SEPARATOR)
                CompileToken(Segment, FUNCTION_RETURN_VOID);
            else
            {
                Token.StepBack();
                CompileExpression(Segment); // this will cause function result be pushed into stack
                CompileToken(Segment, STACK_POP);
                CompileToken(Segment, EX);
                CompileToken(Segment, FUNCTION_RETURN);
            }
            Token.StepBack(); // ???

            break;

            // case MAKEREF_COMMAND: CompileToken(Segment,Token_type); break;
            // case MAKEAREF_COMMAND: CompileToken(Segment,Token_type); break;

        case OP_BOOL_NEG:
            // case OP_EQUAL:
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
            /*        case OP_INCADD:
                case OP_DECADD:
                case OP_MULTIPLYEQ:
                case OP_DIVIDEEQ://*/
        case OP_BOOL_AND:
        case OP_BOOL_OR:
            // case SQUARE_OPEN_BRACKET:
            // case SQUARE_CLOSE_BRACKET:
        case COMMA:
        case AND:
        case END_OF_PROGRAMM:
            CompileToken(Segment, Token_type);
            break;

            //=============================================================================

        case OP_INCADD:
        case OP_DECADD:
        case OP_MULTIPLYEQ:
        case OP_DIVIDEEQ:
            // CompileToken(Segment,Token_type);
            CompileExpression(Segment);
            CompileToken(Segment, Token_type);
            // CompileToken(Segment,LEFT_OPERAND);
            // CompileToken(Segment,STACK_TOP);
            CompileToken(Segment, STACK_POP_VOID);
            if (Token.GetType() == bound_type)
            {
                Token.StepBack();
            }

            break;
            //*/

        case OP_EQUAL:
            if (inout == 0 && !bDebugExpressionRun)
                break; // skip global vars initialization
            // CompileToken(Segment,Token_type);
            CompileExpression(Segment);

            CompileToken(Segment, MOVE);
            CompileToken(Segment, LEFT_OPERAND);
            CompileToken(Segment, STACK_TOP);
            CompileToken(Segment, STACK_POP_VOID);
            Token.StepBack();

            break;

            //        case OP_EQUAL:
            //            CompileToken(Segment,Token_type);
            //        break;

        case SQUARE_OPEN_BRACKET:
            if (!CompileExpression(Segment))
            {
                return false;
            }
            CompileToken(Segment, STACK_POP);
            CompileToken(Segment, LEFTOP_INDEX);

            break;
        case SQUARE_CLOSE_BRACKET:
            SetError("']' without matching '['");
            break;

            // case MAKEREF_COMMAND: CompileToken(Segment,Token_type); break;
        case MAKEREF_COMMAND:
            uint32_t dwRCode1, dwRCode2;
            S_TOKEN_TYPE sttVarType1, sttVarType2;

            // CompileToken(Segment,Token_type);
            if (Token.Get() != OPEN_BRACKET)
            {
                SetError("missed '('");
                return false;
            }

            if (Token.Get() != UNKNOWN)
            {
                SetError("invalid 1st operand");
                return false;
            }

            sttVarType1 = DetectUnknown(dwRCode1);

            if (!(sttVarType1 == VARIABLE || sttVarType1 == LOCAL_VARIABLE))
            {
                SetError("invalid 1st operand");
                return false;
            }

            /*CompileToken(Segment,STACK_ALLOC);
            CompileToken(Segment,SETREF);
            CompileToken(Segment,STACK_TOP);
            CompileToken(Segment,sttVarType,1,(char *)&dwRCode,sizeof(DWORD));*/
            /*            if(sttVarType == VARIABLE)
                  {
                    if(!VarTab.GetVar(vi,*((uint32_t *)&pCodeBase[ip]))) { SetError("Global variable not found"); break;
               } pRef = vi.pDClass;
                  }
                  else
                  {
                    pRef = SStack.Read(pRun_fi->stack_offset,*((uint32_t *)&pCodeBase[ip]));
                    if(pRef == 0) { SetError("Local variable not found"); return false; }
                  }
            */

            //            if(!pRef->IsReference()) { SetError("makeref to non reference variable"); return false; }
            if (Token.Get() != COMMA)
            {
                SetError("missed ','");
                return false;
            }

            if (Token.Get() != UNKNOWN)
            {
                SetError("invalid 1st operand");
                return false;
            }

            sttVarType2 = DetectUnknown(dwRCode2);

            if (!(sttVarType2 == VARIABLE || sttVarType2 == LOCAL_VARIABLE))
            {
                SetError("invalid 2st operand");
                return false;
            }

            if (Token.Get() == SQUARE_OPEN_BRACKET)
            {
                CompileExpression(Segment);

                CompileToken(Segment, STACK_POP);
                CompileToken(Segment, BX);

                CompileToken(Segment, SETREF_BXINDEX);
                CompileToken(Segment, sttVarType1, 1, (char *)&dwRCode1, sizeof(uint32_t));
                CompileToken(Segment, sttVarType2, 1, (char *)&dwRCode2, sizeof(uint32_t));

                if (Token.GetType() != SQUARE_CLOSE_BRACKET)
                {
                    SetError("missed ']'");
                    return false;
                }
                if (Token.Get() != CLOSE_BRACKET)
                {
                    SetError("missed ')'");
                    return false;
                }
            }
            else
            {
                if (Token.GetType() != CLOSE_BRACKET)
                {
                    SetError("missed ')'");
                    return false;
                }
                CompileToken(Segment, SETREF);
                CompileToken(Segment, sttVarType1, 1, (char *)&dwRCode1, sizeof(uint32_t));
                CompileToken(Segment, sttVarType2, 1, (char *)&dwRCode2, sizeof(uint32_t));
            }

            /*    if(BC_TokenGet()== SQUARE_OPEN_BRACKET)
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

            // case MAKEAREF_COMMAND: CompileToken(Segment,Token_type); break;

        case MAKEAREF_COMMAND:

            // CompileToken(Segment,Token_type);    // debug
            S_TOKEN_TYPE sttVarType;

            if (Token.Get() != OPEN_BRACKET)
            {
                SetError("missed '('");
                return false;
            }
            Token_type = Token.Get();
            if (Token_type != UNKNOWN)
            {
                SetError("invalid 1st argument");
                return false;
            }
            Token_type = DetectUnknown(dwRCode);
            if (!(Token_type == VARIABLE || Token_type == LOCAL_VARIABLE))
            {
                SetError("invalid 1st argument");
                return false;
            }

            if (Token_type == VARIABLE)
            {
                real_var = VarTab.GetVarX(dwRCode);
                if (real_var == nullptr)
                {
                    SetError("Invalid var code");
                    return false;
                }

                if (!(real_var->type == VAR_AREFERENCE || real_var->type == VAR_REFERENCE))
                {
                    SetError("not aref or ref");
                    return false;
                }
            }
            else
            {
                FuncTab.GetVar(lvi, CurrentFuncCode, dwRCode);
                if (!(lvi.type == VAR_AREFERENCE || lvi.type == VAR_REFERENCE))
                {
                    SetError("not aref or ref");
                    return false;
                }
            }

            // set ref to stack top
            CompileToken(Segment, STACK_ALLOC);
            CompileToken(Segment, SETREF);
            CompileToken(Segment, STACK_TOP);
            CompileToken(Segment, Token_type, 1, (char *)&dwRCode, sizeof(uint32_t));

            if (Token.Get() != COMMA)
            {
                SetError("missed ','");
                return false;
            }

            sttVarType = Token.Get();
            if (sttVarType != UNKNOWN)
            {
                SetError("invalid 2d argument");
                return false;
            }
            sttVarType = DetectUnknown(dwRCode);
            if (!(sttVarType == VARIABLE || sttVarType == LOCAL_VARIABLE))
            {
                SetError("invalid 2d argument");
                return false;
            }

            Token_type = Token.Get();
            if (Token_type == SQUARE_OPEN_BRACKET)
            {
                CompileExpression(Segment);
                if (Token.GetType() != SQUARE_CLOSE_BRACKET)
                {
                    SetError("missing ']'");
                    return false;
                }
                //....
                CompileToken(Segment, STACK_POP);
                CompileToken(Segment, BX);

                // move ap to AP register
                CompileToken(Segment, MOVEAP_BXINDEX);
                CompileToken(Segment, AP);
                CompileToken(Segment, sttVarType, 1, (char *)&dwRCode, sizeof(uint32_t));
                Token_type = Token.Get();

                CompileToken(Segment, PUSH_OBJID_BXINDEX);
                CompileToken(Segment, sttVarType, 1, (char *)&dwRCode, sizeof(uint32_t));
            }
            else
            {
                // bove atributes root to AP
                CompileToken(Segment, MOVEAP);
                CompileToken(Segment, AP);
                CompileToken(Segment, sttVarType, 1, (char *)&dwRCode, sizeof(uint32_t));

                CompileToken(Segment, PUSH_OBJID);
                CompileToken(Segment, sttVarType, 1, (char *)&dwRCode, sizeof(uint32_t));
            }

            if (Token_type != DOT)
            {
                // set attribute pointer to referenced var
                CompileToken(Segment, SETAREF);
                CompileToken(Segment, STACK_TOP);
                CompileToken(Segment, AP);
                CompileToken(Segment, STACK_POP_VOID);
                break;
            }

            // advance ap

            sttResult = Token.Get();
            uint32_t dwAWCode;
            uint32_t dwVarCode;
            do
            {
                if (Token.GetType() == OPEN_BRACKET)
                {
                    // access var
                    sttResult = Token.Get();
                    if (sttResult != UNKNOWN)
                    {
                        SetError("Invalid access var syntax");
                        return false;
                    }
                    sttResult = DetectUnknown(dwVarCode);
                    if (!(sttResult == VARIABLE || sttResult == LOCAL_VARIABLE))
                    {
                        SetError("not variable: %s", Token.GetData());
                        return false;
                    }
                    // CompileToken(Segment,ADVANCE_AP);
                    CompileToken(Segment, VERIFY_AP);
                    CompileToken(Segment, sttResult, 1, &dwVarCode, sizeof(uint32_t));
                    if (Token.Get() != CLOSE_BRACKET)
                    {
                        SetError("missing ')'");
                        return false;
                    }
                }
                else
                {
                    // access word
                    if (Token.GetData() == nullptr)
                    {
                        SetError("Invalid access string");
                        return false;
                    }
                    Token.LowCase();
                    dwAWCode = SCodec.Convert(Token.GetData());
                    // CompileToken(Segment,ADVANCE_AP);
                    CompileToken(Segment, VERIFY_AP);
                    CompileToken(Segment, ACCESS_WORD_CODE, 1, &dwAWCode, sizeof(uint32_t));
                }
                sttResult = Token.Get();
                if (sttResult == DOT)
                    Token.Get(); // get next token (after dot)
            } while (sttResult == DOT);

            CompileToken(Segment, SETAREF);
            CompileToken(Segment, STACK_TOP);
            CompileToken(Segment, AP);
            CompileToken(Segment, STACK_POP_VOID);
            break;
            //=============================================================================

        case DOT:
            Token_type = Token.Get();
            if (Token_type == OPEN_BRACKET)
            {
                // access word inside variable
                Token.Get(); // variable name
                // check for valid local variable
                var_code = FuncTab.FindVar(CurrentFuncCode, Token.GetData());
                if (var_code != INVALID_VAR_CODE)
                {
                    Token.Get();
                    if (Token.GetType() != CLOSE_BRACKET)
                    {
                        SetError("no mathcing ')'");
                        return false;
                    }
                    CompileToken(Segment, ACCESS_VAR);
                    CompileToken(Segment, LOCAL_VARIABLE, 1, (char *)&var_code, sizeof(uint32_t));
                    break;
                }

                var_code = VarTab.FindVar(Token.GetData());
                if (var_code != INVALID_VAR_CODE)
                {
                    Token.Get();
                    if (Token.GetType() != CLOSE_BRACKET)
                    {
                        SetError("no mathcing ')'");
                        return false;
                    }
                    CompileToken(Segment, ACCESS_VAR);
                    CompileToken(Segment, VARIABLE, 1, (char *)&var_code, sizeof(uint32_t));
                    break;
                }

                SetError("Invalid access string");
                return false;
            }
            // else its access word
            if (Token.GetData() == nullptr)
            {
                SetError("Invalid access string");
                return false;
            }
            // CompileToken(Segment,ACCESS_WORD,1,Token.GetData(),strlen(Token.GetData())+1);

            Token.LowCase();
            awcode = SCodec.Convert(Token.GetData());
            CompileToken(Segment, ACCESS_WORD_CODE, 1, &awcode, sizeof(uint32_t));
            break;

        case TRUE_CONST:
            lvalue = 1;
            CompileToken(Segment, NUMBER, 1, (char *)&lvalue, sizeof(long));
            break;
        case FALSE_CONST:
            lvalue = 0;
            CompileToken(Segment, NUMBER, 1, (char *)&lvalue, sizeof(long));
            break;

            // write value
        case NUMBER:
            CompileNumber(Segment);
            break;
        case FLOAT_NUMBER:
            CompileFloatNumber(Segment);
            break;
        case STRING:
            CompileString(Segment);
            break;

        case LABEL: // register label

            break;
        case DEFINE_COMMAND: // create define

            break;

        default:

            break;
        }
    } while (Token_type != END_OF_PROGRAMM);

    return true;
}

bool COMPILER::TokenIs(S_TOKEN_TYPE test)
{
    if (TokenLastReadResult == test)
        return true;
    return false;
}

bool COMPILER::BC_Jump(SEGMENT_DESC &Segment, uint32_t offset)
{
    if (offset >= Segment.BCode_Program_size)
    {
        SetError("invalid jump");
        return false;
    }
    InstructionPointer = offset;
    return true;
}

S_TOKEN_TYPE COMPILER::BC_TokenGet(uint32_t &ip, uint32_t &token_data_size)
{
    // function read token type and data size, advance InstructionPointer to next token
    // set ip to token data
    // set token data size value and return token type
    TokenLastReadResult = static_cast<S_TOKEN_TYPE>(pRunCodeBase[InstructionPointer]);
    // Trace("Token: %s", Token.GetTypeName(TokenLastReadResult));
    InstructionPointer++;
    if (static_cast<uint8_t>(pRunCodeBase[InstructionPointer]) < 0xff)
    {
        token_data_size = static_cast<uint8_t>(pRunCodeBase[InstructionPointer]);
        InstructionPointer++;
    }
    else
    {
        InstructionPointer++;
        memcpy(&token_data_size, &pRunCodeBase[InstructionPointer], sizeof(uint32_t));
        InstructionPointer += sizeof(uint32_t);
    }
    ip = InstructionPointer;
    TLR_DataOffset = InstructionPointer;
    InstructionPointer += token_data_size;
    return TokenLastReadResult;
}

S_TOKEN_TYPE COMPILER::BC_TokenGet()
{
    // short version of function for calls what doesnt need to work with token data
    uint32_t token_data_size;
    TokenLastReadResult = static_cast<S_TOKEN_TYPE>(pRunCodeBase[InstructionPointer]);
    // Trace("Token: %s", Token.GetTypeName(TokenLastReadResult));
    InstructionPointer++;
    if (static_cast<uint8_t>(pRunCodeBase[InstructionPointer]) < 0xff)
    {
        token_data_size = static_cast<uint8_t>(pRunCodeBase[InstructionPointer]);
        InstructionPointer++;
    }
    else
    {
        InstructionPointer++;
        memcpy(&token_data_size, &pRunCodeBase[InstructionPointer], sizeof(uint32_t));
        InstructionPointer += sizeof(uint32_t);
    }
    TLR_DataOffset = InstructionPointer;
    InstructionPointer += token_data_size;
    return TokenLastReadResult;
}

S_TOKEN_TYPE COMPILER::NextTokenType()
{
    return static_cast<S_TOKEN_TYPE>(pRunCodeBase[InstructionPointer]);
}

bool COMPILER::BC_CallFunction(uint32_t func_code, uint32_t &ip, DATA *&pVResult)
{
    FuncInfo call_fi;
    uint32_t mem_ip;
    uint32_t mem_InstructionPointer;
    FuncInfo *mem_pfi;
    //    DATA * pV;
    const char *mem_codebase;
    uint32_t arguments;
    uint32_t check_sp;
    uint32_t nDebugEnterMode;

    CompilerStage = CS_RUNTIME;

    // get func info
    if (!FuncTab.GetFunc(call_fi, func_code))
    {
        SetError("Invalid function call");
        return false;
    }

    // TODO: only do if stack debug if enabled (should be runtime configurable)
    // push function details to call stack
    storm::ringbuffer_stack_push_guard push_guard(callStack_);
    push_guard.push(std::make_tuple(call_fi.decl_file_name.c_str(), call_fi.decl_line, call_fi.name.c_str()));

    // number f arguments pushed into stack for this function call
    if (BC_TokenGet() != ARGS_NUM)
    {
        SetError("missing args_num token");
        return false;
    }
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

    nDebugEnterMode = CDebug->GetTraceMode();
    uint64_t nTicks;
    if (call_fi.segment_id == INTERNAL_SEGMENT_ID)
    {
        if (bRuntimeLog)
        {
            if (!FuncTab.AddCall(func_code))
            {
                core.Trace("Invalid func_code = %u for AddCall", func_code);
            }
        }

        // BC_CallIntFunction(func_code,pVResult,arguments);
        RDTSC_B(nTicks);
        BC_CallIntFunction(func_code, pVResult, arguments);
        RDTSC_E(nTicks);

        if (!FuncTab.AddTime(func_code, nTicks))
        {
            core.Trace("Invalid func_code = %u for AddTime", func_code);
        }
    }
    else if (call_fi.segment_id == IMPORTED_SEGMENT_ID)
    {
        pVResult = nullptr;
        RDTSC_B(nTicks);
        const uint32_t nResult = call_fi.imported_func(&SStack);
        if (nResult == IFUNCRESULT_OK)
        {
            if (call_fi.return_type != TVOID)
            {
                pVResult = SStack.Read();
            }
        }
        RDTSC_E(nTicks);
        if (!FuncTab.AddTime(func_code, nTicks))
        {
            core.Trace("Invalid func_code = %u for AddTime", func_code);
        }
    }
    else
    {
        // BC_Execute(func_code,pVResult);
        RDTSC_B(nTicks);
        BC_Execute(func_code, pVResult);
        RDTSC_E(nTicks);
        if (!FuncTab.AddTime(func_code, nTicks))
        {
            core.Trace("Invalid func_code = %u for AddTime", func_code);
        }
    }
    if (nDebugEnterMode == TMODE_MAKESTEP)
    {
        CDebug->SetTraceMode(TMODE_MAKESTEP);
    }

    if (pVResult)
    {
        if (check_sp != (SStack.GetDataNum() - 1))
        {
            SetError("function '%s' stack error", call_fi.name.c_str());

            pRun_fi = mem_pfi;
            InstructionPointer = mem_InstructionPointer;
            ip = mem_ip;

            RunningSegmentID = pRun_fi->segment_id;
            pRunCodeBase = mem_codebase;

            // return false;    // debug!
        }
    }
    else
    {
        if (check_sp != SStack.GetDataNum())
        {
            SetError("function '%s' stack error", call_fi.name.c_str());
            pRun_fi = mem_pfi;
            InstructionPointer = mem_InstructionPointer;
            ip = mem_ip;

            RunningSegmentID = pRun_fi->segment_id;
            pRunCodeBase = mem_codebase;

            // return false;// debug!
        }
    }
    //*/
    // restore
    pRun_fi = mem_pfi;
    InstructionPointer = mem_InstructionPointer;
    ip = mem_ip;
    if (pRun_fi)
        RunningSegmentID = pRun_fi->segment_id;
    pRunCodeBase = mem_codebase;

    return true;
}

bool COMPILER::BC_Execute(uint32_t function_code, DATA *&pVReturnResult, const char *pDbgExpSource)
{
    // GUARD(BC_Execute)
    uint32_t inout;
    uint32_t segment_index;
    uint32_t token_data_size;
    uint32_t ip;
    uint32_t n;
    uint32_t jump_offset;
    uint32_t func_code;
    uint32_t nLeftOperandCode;
    uint32_t bLeftOperandType;
    long nLeftOperandIndex;
    S_TOKEN_TYPE Token_type;
    FuncInfo fi;
    const VarInfo *real_var;
    DATA *pV;
    DATA *pVResult;
    //    DATA   ExpressionResult;    // while compile expression not ready, each function have its own register
    const char *pCodeBase;
    bool bExit;
    long lvalue;
    S_TOKEN_TYPE vtype;
    DATA *pVV;
    uint32_t var_code;
    const char *pAccess_string;
    //    long array_index;
    //    DATA * pRef;
    DATA *pVar;
    entid_t eid;
    //    ATTRIBUTES * pRoot;
    ATTRIBUTES *pLeftOperandAClass;
    bool bDebugWaitForThisFunc;
    DATA *pVDst;
    DATA *pVSrc;
    bool bUseIndex;
    long dwBXIndex;

    CompilerStage = CS_RUNTIME;

    if (bRuntimeLog)
    {
        if (!FuncTab.AddCall(function_code))
        {
            core.Trace("Invalid function_code = %u for AddCall", function_code);
        }
    }

    bDebugWaitForThisFunc = false;

    // ExpressionResult.SetVCompiler(this);
    pVReturnResult = nullptr;
    pVResult = nullptr;

    if (pDbgExpSource == nullptr)
    {
        if (!FuncTab.GetFunc(fi, function_code))
        {
            SetError("Invalid function: %s", fi.name.c_str());
            return false;
        }

        if (fi.offset == INVALID_FUNC_OFFSET)
        {
            SetError("Function (%s) isnt loaded", fi.name.c_str());
            return false;
        }

        if (fi.segment_id == INTERNAL_SEGMENT_ID)
        {
            SetError("Function (%s) is internal", fi.name.c_str());
            return false;
        }

        if (fi.segment_id == IMPORTED_SEGMENT_ID)
        {
            SetError("Function (%s) is imported", fi.name.c_str());
            return false;
        }

        segment_index = GetSegmentIndex(fi.segment_id);
        if (segment_index == INVALID_SEGMENT_INDEX)
        {
            SetError("Function (%s) segment not loaded", fi.name.c_str());
            return false;
        }
        if (SegmentTable[segment_index].pCode == nullptr)
        {
            SetError("Segment (%s) not loaded", SegmentTable[segment_index].name);
            return false;
        }

        // Trace("-----------------------------------------------------------------");
        // Trace("Execute function: %s",fi.name);

        RunningSegmentID = fi.segment_id;
        fi.stack_offset = SStack.GetDataNum() - fi.arguments; // set stack offset

        // check arguments types
        for (n = 0; n < fi.arguments; n++)
        {
            if (fi.local_vars[n].type == VAR_REFERENCE)
                continue;
            pV = SStack.Read(fi.stack_offset, n);
            if (pV->GetType() != fi.local_vars[n].type)
            {
                pV = pV->GetVarPointer();
                if (!pV)
                {
                    SetError("uninitialized function argument");
                    return false;
                }

                if (fi.local_vars[n].type == VAR_AREFERENCE && pV->GetType() == VAR_OBJECT)
                    continue;

                // TODO: remove and fix
                if (false && pV->GetType() != fi.local_vars[n].type)
                {
                    SetWarning("wrong type of argument %d  %s(%s) <-- [%s]", n, fi.name.c_str(),
                               Token.GetTypeName(fi.local_vars[n].type), Token.GetTypeName(pV->GetType()));
                }
            }
        }

        for (n = fi.arguments; n < fi.local_vars.size(); n++)
        {
            pV = SStack.Push();
            pV->SetType(fi.local_vars[n].type, fi.local_vars[n].elements);
        }

        pRun_fi = &fi; // set pointer to 'this' function info

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
    pLeftOperandAClass = nullptr;

    // S_TOKEN_TYPE oldToken_type;
    do
    {
        dwNumberScriptCommandsExecuted++;
        // oldToken_type = Token_type;
        Token_type = BC_TokenGet(ip, token_data_size);

        switch (Token_type)
        {
        case ARGS_NUM:
            __debugbreak();
            break;
        case STACK_COMPARE:
            pV = SStack.Read();
            ExpressionResult.CompareAndSetResult(pV, OP_BOOL_EQUAL);
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
            if (lvalue == 0)
                break;
            SStack.Pop();
            break;
        case POP_VOID:
            SStack.Pop();
            break;
        case JUMP:
            memcpy(&jump_offset, &pCodeBase[ip], sizeof(uint32_t));
            if (!BC_Jump(SegmentTable[segment_index], jump_offset))
                return false;
            break;
        case JUMP_Z:
            memcpy(&jump_offset, &pCodeBase[ip], sizeof(uint32_t));
            ExpressionResult.Convert(VAR_INTEGER);
            ExpressionResult.Get(lvalue);
            if (lvalue)
                break;
            if (!BC_Jump(SegmentTable[segment_index], jump_offset))
                return false;
            break;
        case JUMP_NZ:
            memcpy(&jump_offset, &pCodeBase[ip], sizeof(uint32_t));
            ExpressionResult.Convert(VAR_INTEGER);
            ExpressionResult.Get(lvalue);
            if (lvalue == 0)
                break;
            if (!BC_Jump(SegmentTable[segment_index], jump_offset))
                return false;
            break;
        case LOCAL_VARIABLE:
            pLeftOperandAClass = nullptr;            // reset attribute
            nLeftOperandIndex = INVALID_ARRAY_INDEX; // reset index
            nLeftOperandCode = *((long *)&pRunCodeBase[TLR_DataOffset]);
            bLeftOperandType = LOCAL_VARIABLE;
            break;
        case VARIABLE:
            pLeftOperandAClass = nullptr;            // reset attribute
            nLeftOperandIndex = INVALID_ARRAY_INDEX; // reset index
            nLeftOperandCode = *((long *)&pRunCodeBase[TLR_DataOffset]);
            bLeftOperandType = VARIABLE;
            break;
        case ACCESS_WORD_CODE:
            if (nLeftOperandCode == INVALID_VAR_CODE)
            {
                SetError("no Lvalue");
                return false;
            }

            if (nLeftOperandIndex == INVALID_ARRAY_INDEX)
            {
                if (pLeftOperandAClass == nullptr)
                {
                    if (bLeftOperandType == VARIABLE)
                    {
                        real_var = VarTab.GetVar(nLeftOperandCode);
                        if (real_var == nullptr)
                        {
                            SetError("Global variable not found");
                            return false;
                        }

                        pV = real_var->value.get();
                    }
                    else
                    {
                        pV = SStack.Read(pRun_fi->stack_offset, nLeftOperandCode);
                        if (pV == nullptr)
                        {
                            SetError("Local variable not found");
                            return false;
                        }
                    }
                    pLeftOperandAClass = pV->GetAClass();
                }
                if (pLeftOperandAClass == nullptr)
                {
                    SetError("AClass ERROR n1");
                    return false;
                }
                pLeftOperandAClass =
                    pLeftOperandAClass->VerifyAttributeClassByCode(*((long *)&pRunCodeBase[TLR_DataOffset]));
                break;
            }
            if (pLeftOperandAClass == nullptr)
            {
                if (bLeftOperandType == VARIABLE)
                {
                    real_var = VarTab.GetVar(nLeftOperandCode);
                    if (real_var == nullptr)
                    {
                        SetError("Global variable not found");
                        return false;
                    }

                    pV = real_var->value.get();
                }
                else
                {
                    pV = SStack.Read(pRun_fi->stack_offset, nLeftOperandCode);
                    if (pV == nullptr)
                    {
                        SetError("Local variable not found");
                        return false;
                    }
                }
                pLeftOperandAClass = pV->GetAClass(nLeftOperandIndex);
            }
            if (pLeftOperandAClass == nullptr)
            {
                SetError("AClass ERROR n1");
                return false;
            }
            pLeftOperandAClass =
                pLeftOperandAClass->VerifyAttributeClassByCode(*((long *)&pRunCodeBase[TLR_DataOffset]));
            break;
            break;
        case ACCESS_WORD:
            if (nLeftOperandCode == INVALID_VAR_CODE)
            {
                SetError("no Lvalue");
                return false;
            }
            if (nLeftOperandIndex == INVALID_ARRAY_INDEX)
            {
                if (pLeftOperandAClass == nullptr)
                {
                    if (bLeftOperandType == VARIABLE)
                    {
                        real_var = VarTab.GetVar(nLeftOperandCode);
                        if (real_var == nullptr)
                        {
                            SetError("Global variable not found");
                            return false;
                        }

                        pV = real_var->value.get();
                    }
                    else
                    {
                        pV = SStack.Read(pRun_fi->stack_offset, nLeftOperandCode);
                        if (pV == nullptr)
                        {
                            SetError("Local variable not found");
                            return false;
                        }
                    }
                    pLeftOperandAClass = pV->GetAClass();
                }
                if (pLeftOperandAClass == nullptr)
                {
                    SetError("AClass ERROR n1");
                    return false;
                }
                pLeftOperandAClass = pLeftOperandAClass->VerifyAttributeClass((char *)&pRunCodeBase[TLR_DataOffset]);
                break;
            }
            if (pLeftOperandAClass == nullptr)
            {
                if (bLeftOperandType == VARIABLE)
                {
                    real_var = VarTab.GetVar(nLeftOperandCode);
                    if (real_var == nullptr)
                    {
                        SetError("Global variable not found");
                        return false;
                    }

                    pV = real_var->value.get();
                }
                else
                {
                    pV = SStack.Read(pRun_fi->stack_offset, nLeftOperandCode);
                    if (pV == nullptr)
                    {
                        SetError("Local variable not found");
                        return false;
                    }
                }
                pLeftOperandAClass = pV->GetAClass(nLeftOperandIndex);
            }
            if (pLeftOperandAClass == nullptr)
            {
                SetError("AClass ERROR n1");
                return false;
            }
            pLeftOperandAClass = pLeftOperandAClass->VerifyAttributeClass((char *)&pRunCodeBase[TLR_DataOffset]);
            break;
            break;
        case ACCESS_VAR:
            if (nLeftOperandCode == INVALID_VAR_CODE)
            {
                SetError("no Lvalue");
                return false;
            }
            if (nLeftOperandIndex == INVALID_ARRAY_INDEX)
            {
                if (pLeftOperandAClass == nullptr)
                {
                    if (bLeftOperandType == VARIABLE)
                    {
                        real_var = VarTab.GetVar(nLeftOperandCode);
                        if (real_var == nullptr)
                        {
                            SetError("Global variable not found");
                            return false;
                        }

                        pV = real_var->value.get();
                    }
                    else
                    {
                        pV = SStack.Read(pRun_fi->stack_offset, nLeftOperandCode);
                        if (pV == nullptr)
                        {
                            SetError("Local variable not found");
                            return false;
                        }
                    }
                    pLeftOperandAClass = pV->GetAClass();
                }
                if (pLeftOperandAClass == nullptr)
                {
                    SetError("AClass ERROR n1");
                    return false;
                }

                vtype = BC_TokenGet();
                var_code = *((long *)&pRunCodeBase[TLR_DataOffset]);
                if (!(vtype == VARIABLE || vtype == LOCAL_VARIABLE))
                {
                    SetError("invalid access var");
                    return false;
                }
                if (vtype == VARIABLE)
                {
                    real_var = VarTab.GetVar(var_code);
                    if (real_var == nullptr)
                    {
                        SetError("Global variable not found");
                        return false;
                    }

                    pVV = real_var->value.get();
                }
                else
                {
                    pVV = SStack.Read(pRun_fi->stack_offset, var_code);
                    if (pVV == nullptr)
                    {
                        SetError("Local variable not found");
                        return false;
                    }
                }
                pVV->Get(pAccess_string);

                // pLeftOperandAClass = pLeftOperandAClass->VerifyAttributeClass(pAccess_string);
                pLeftOperandAClass = pLeftOperandAClass->CreateSubAClass(pLeftOperandAClass, pAccess_string);
                break;
            }
            if (pLeftOperandAClass == nullptr)
            {
                if (bLeftOperandType == VARIABLE)
                {
                    real_var = VarTab.GetVar(nLeftOperandCode);
                    if (real_var == nullptr)
                    {
                        SetError("Global variable not found");
                        return false;
                    }

                    pV = real_var->value.get();
                }
                else
                {
                    pV = SStack.Read(pRun_fi->stack_offset, nLeftOperandCode);
                    if (pV == nullptr)
                    {
                        SetError("Local variable not found");
                        return false;
                    }
                }
                pLeftOperandAClass = pV->GetAClass(nLeftOperandIndex);
            }
            if (pLeftOperandAClass == nullptr)
            {
                SetError("AClass ERROR n1");
                return false;
            }

            vtype = BC_TokenGet();
            var_code = *((long *)&pRunCodeBase[TLR_DataOffset]);
            if (!(vtype == VARIABLE || vtype == LOCAL_VARIABLE))
            {
                SetError("invalid access var");
                return false;
            }
            if (vtype == VARIABLE)
            {
                real_var = VarTab.GetVar(var_code);
                if (real_var == nullptr)
                {
                    SetError("Global variable not found");
                    return false;
                }

                pVV = real_var->value.get();
            }
            else
            {
                pVV = SStack.Read(pRun_fi->stack_offset, var_code);
                if (pVV == nullptr)
                {
                    SetError("Local variable not found");
                    return false;
                }
            }
            pVV->Get(pAccess_string);

            // pLeftOperandAClass = pLeftOperandAClass->VerifyAttributeClass(pAccess_string);
            pLeftOperandAClass = pLeftOperandAClass->CreateSubAClass(pLeftOperandAClass, pAccess_string);

            break;
            break;
        case DEBUG_LINE_CODE:
            if (core.Exit_flag)
                return false;
            if (pDbgExpSource)
                break;
            if (bDebugExpressionRun)
                break;
            memcpy(&nDebugTraceLineCode, &pCodeBase[ip], sizeof(uint32_t));
            if (bTraceMode)
            {
                if (CDebug->GetTraceMode() == TMODE_MAKESTEP || CDebug->GetTraceMode() == TMODE_MAKESTEP_OVER)
                {
                    if (CDebug->GetTraceMode() == TMODE_MAKESTEP_OVER && bDebugWaitForThisFunc == false)
                        break;

                    if (!CDebug->IsDebug())
                        CDebug->OpenDebugWindow(core.GetAppInstance());
                    // else
                    ShowWindow(CDebug->GetWindowHandle(), SW_NORMAL);

                    CDebug->SetTraceLine(nDebugTraceLineCode);
                    CDebug->BreakOn(fi.decl_file_name.c_str(), nDebugTraceLineCode);
                    CDebug->SetTraceMode(TMODE_WAIT);
                    while (CDebug->GetTraceMode() == TMODE_WAIT)
                    {
                        Sleep(40);
                    }
                    if (CDebug->GetTraceMode() == TMODE_MAKESTEP_OVER)
                        bDebugWaitForThisFunc = true;
                    else
                        bDebugWaitForThisFunc = false;
                }
                else if (CDebug->Breaks.CanBreak())
                {
                    // check for breakpoint
                    if (CDebug->Breaks.Find(fi.decl_file_name.c_str(), nDebugTraceLineCode))
                    {
                        if (!CDebug->IsDebug())
                            CDebug->OpenDebugWindow(core.GetAppInstance());

                        ShowWindow(CDebug->GetWindowHandle(), SW_NORMAL);
                        // CDebug->OpenDebugWindow(core.hInstance);
                        CDebug->SetTraceMode(TMODE_WAIT);
                        CDebug->BreakOn(fi.decl_file_name.c_str(), nDebugTraceLineCode);

                        while (CDebug->GetTraceMode() == TMODE_WAIT)
                        {
                            Sleep(40);
                        } // wait for debug thread decision
                        if (CDebug->GetTraceMode() == TMODE_MAKESTEP_OVER)
                            bDebugWaitForThisFunc = true;
                        else
                            bDebugWaitForThisFunc = false;
                    }
                }
            }
            break;
        case DEBUG_FILE_NAME:

            /*if(!bTraceMode) break;

            long index;
            long file_line_offset;
            long file_name_size;
            index = ip;
            memcpy(&file_line_offset,&pCodeBase[index],sizeof(uint32_t)); index += sizeof(uint32_t);
            memcpy(&file_name_size,&pCodeBase[index],sizeof(uint32_t)); index += sizeof(uint32_t);
            memcpy(DebugTraceFileName,&pCodeBase[index],file_name_size); index += file_name_size;
            DebugTraceFileName[file_name_size] = 0;
            //DebugSourceLine = 1;*/
            break;
        case BLOCK_IN:
        case BLOCK_OUT:
            // case DEBUG_FILE_NAME:
            SetError("Unallowed instraction");
            return false;
            break;
        default:
            SetError("Incorrect instruction code");
            return false;

        case CALL:                                               // undetermined function call
            vtype = BC_TokenGet();                               // read variable
            var_code = *((long *)&pRunCodeBase[TLR_DataOffset]); // var code
            if (vtype == VARIABLE)
            {
                real_var = VarTab.GetVar(var_code);
                if (real_var == nullptr)
                {
                    SetError("Global variable not found");
                    return false;
                }

                pVV = real_var->value.get();
            }
            else
            {
                pVV = SStack.Read(pRun_fi->stack_offset, var_code);
                if (pVV == nullptr)
                {
                    SetError("Local variable not found");
                    return false;
                }
            }
            pVV = pVV->GetVarPointer();
            if (pVV->GetType() != VAR_STRING)
            {
                SetError("string call argument isnt string var");
                return false;
            }
            pVV->Get(pAccess_string); // func name
            func_code = FuncTab.FindFunc(pAccess_string);
            if (func_code == INVALID_FUNC_CODE)
            {
                SetError("function '%s' not found", pAccess_string);
                return false;
            }
            pVResult = nullptr;
            if (!BC_CallFunction(func_code, ip, pVResult))
                return false;
            if (pVResult)
            {
                ExpressionResult.Set(static_cast<long>(1));
                // SStack.Pop();
            }
            else
                ExpressionResult.Set(static_cast<long>(0));
            break;
        case CALL_FUNCTION:
            memcpy(&func_code, &pCodeBase[ip], sizeof(uint32_t));
            pVResult = nullptr;
            if (!BC_CallFunction(func_code, ip, pVResult))
                return false;
            // if(pVResult) SStack.Pop();
            break;
        case FUNCTION_RETURN_VOID:
            if (fi.return_type != TVOID)
            {
                SetError("function must return value");
                return false;
            }
            // for(n=0;n<fi.var_num;n++) SStack.Pop();
            SStack.InvalidateFrom(fi.stack_offset);

            return true;
        case FUNCTION_RETURN:
            if (fi.return_type == TVOID)
            {
                SetError("void function return value");

                return false;
            }
            // at this moment result expression placed in EX register

            if (pDbgExpSource == nullptr) // skip stack unwind for dbg expression process    // ????????
                SStack.InvalidateFrom(fi.stack_offset);

            // copy result into stack
            pV = SStack.Push();
            pV->Copy(&ExpressionResult);
            pVReturnResult = pV;

            // check return type
            if (pDbgExpSource == nullptr) // skip test for dbg expression process
                if (fi.return_type != pV->GetType())
                {
                    if (fi.return_type == VAR_INTEGER && pV->GetType() == VAR_PTR)
                    {
                        pV->Convert(VAR_INTEGER);
                        return true;
                    }

                    SetError("%s function return %s value", Token.GetTypeName(fi.return_type),
                             Token.GetTypeName(pV->GetType()));
                    return false;
                }
            return true;
            /*if(BC_ProcessExpression(&ExpressionResult))
            {
              if(pDbgExpSource == 0)    // skip stack unwind for dbg expression process
                SStack.InvalidateFrom(fi.stack_offset);
              pV = SStack.Push();
              pV->Copy(&ExpressionResult);
              pVReturnResult = pV;

              if(pDbgExpSource == 0)    // skip test for dbg expression process
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

            // case PROCESS_EXPRESSION:
            // BC_ProcessExpression(&ExpressionResult);
            // break;
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
            if (nLeftOperandCode == INVALID_VAR_CODE)
            {
                SetError("no Lvalue");
                return false;
            }
            if (bLeftOperandType == VARIABLE)
            {
                real_var = VarTab.GetVar(nLeftOperandCode);
                if (real_var == nullptr)
                {
                    SetError("Global variable not found");
                    return false;
                }

                pV = real_var->value.get();
            }
            else
            {
                pV = SStack.Read(pRun_fi->stack_offset, nLeftOperandCode);
                if (pV == nullptr)
                {
                    SetError("Local variable not found");
                    return false;
                }
            }
            if (nLeftOperandIndex != INVALID_ARRAY_INDEX)
            {
                pV = static_cast<DATA *>(pV->GetArrayElement(nLeftOperandIndex));
                if (pV == nullptr)
                {
                    SetError("invalid array index");
                    return false;
                }
            }
            // BC_ProcessExpression(&ExpressionResult);
            pVV = SStack.Read();
            switch (OpType)
            {
            case OP_INCADD:
                pV->Plus(pVV);
                break;
            case OP_DECADD:
                pV->Minus(pVV);
                break;
            case OP_MULTIPLYEQ:
                pV->Multiply(pVV);
                break;
            case OP_DIVIDEEQ:
                pV->Divide(pVV);
                break;
            }

            break; //*/

            /*case OP_MULTIPLYEQ:
            case OP_DIVIDEEQ:
            case OP_INCADD:
            case OP_DECADD:
              S_TOKEN_TYPE OpType;
              OpType = Token_type;
              if(nLeftOperandCode == INVALID_VAR_CODE) { SetError("no Lvalue"); return false;    }
              if(bLeftOperandType == VARIABLE)
              {
                if(!VarTab.GetVar(vi,nLeftOperandCode))    { SetError("Global variable not found"); return false; }
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
                case OP_INCADD:        pV->Plus(&ExpressionResult); break;
                case OP_DECADD:        pV->Minus(&ExpressionResult); break;
                case OP_MULTIPLYEQ: pV->Multiply(&ExpressionResult); break;
                case OP_DIVIDEEQ: pV->Divide(&ExpressionResult); break;
              }

            break;//*/
        case OP_INC:
            if (nLeftOperandCode == INVALID_VAR_CODE)
            {
                SetError("no Lvalue");
                return false;
            }
            if (bLeftOperandType == VARIABLE)
            {
                real_var = VarTab.GetVar(nLeftOperandCode);
                if (real_var == nullptr)
                {
                    SetError("Global variable not found");
                    return false;
                }

                pV = real_var->value.get();
            }
            else
            {
                pV = SStack.Read(pRun_fi->stack_offset, nLeftOperandCode);
                if (pV == nullptr)
                {
                    SetError("Local variable not found");
                    return false;
                }
            }
            if (nLeftOperandIndex == INVALID_ARRAY_INDEX)
                pV->Inc();
            else
            {
                pV = static_cast<DATA *>(pV->GetArrayElement(nLeftOperandIndex));
                if (pV == nullptr)
                {
                    SetError("invalid array index");
                    return false;
                }
                pV->Inc();
            }
            break;
        case OP_DEC:
            if (nLeftOperandCode == INVALID_VAR_CODE)
            {
                SetError("no Lvalue");
                return false;
            }
            if (bLeftOperandType == VARIABLE)
            {
                real_var = VarTab.GetVar(nLeftOperandCode);
                if (real_var == nullptr)
                {
                    SetError("Global variable not found");
                    return false;
                }

                pV = real_var->value.get();
            }
            else
            {
                pV = SStack.Read(pRun_fi->stack_offset, nLeftOperandCode);
                if (pV == nullptr)
                {
                    SetError("Local variable not found");
                    return false;
                }
            }
            if (nLeftOperandIndex == INVALID_ARRAY_INDEX)
                pV->Dec();
            else
            {
                pV = static_cast<DATA *>(pV->GetArrayElement(nLeftOperandIndex));
                if (pV == nullptr)
                {
                    SetError("invalid array index");
                    return false;
                }
                pV->Dec();
            }
            break;
            /*case OP_EQUAL:
              if(nLeftOperandCode == INVALID_VAR_CODE) { SetError("no Lvalue"); return false;    }
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
                      sprintf_s(gs,"%.7f",fV1);
                      ExpressionResult.Set(gs);

                    }
                  // 2 signs precision --------------------------------
                  ExpressionResult.Convert(VAR_STRING);
                  ExpressionResult.Get(pChar);
                  pLeftOperandAClass->SetValue(pChar);


                  if(bLeftOperandType == VARIABLE)
                  {
                    if(!VarTab.GetVar(vi,nLeftOperandCode))    { SetError("Global variable not found"); return false; }
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
                  //api->Entity_AttributeChanged(&eid,pLeftOperandAClass->GetThisName());
                  if(bEntityUpdate) core.Entity_AttributeChanged(&eid,pLeftOperandAClass);
                  break;
                }
                // copy value to variable
                if(bLeftOperandType == VARIABLE)
                {
                  if(!VarTab.GetVar(vi,nLeftOperandCode))    { SetError("Global variable not found"); return false; }
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
                  if(!ExpressionResult.Convert(pV1->GetType()))    /// ??????????
                  {
                    break;    // skip unallowed conversion, write to log(in Convert func)
                  }
                  pVV = ExpressionResult.GetVarPointer();    // ??????????? ->
                  //ExpressionResult.RefConvert();
                  //if(!pVV->Convert(ExpressionResult.GetType()))
                  //{
                  //    break;    // skip unallowed conversion, write to log(in Convert func)
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
                if(!VarTab.GetVar(vi,*((uint32_t *)&pCodeBase[ip]))) { SetError("Global variable not found"); break; }
                pRef = vi.pDClass;
              }
              else
              {
                pRef = SStack.Read(pRun_fi->stack_offset,*((uint32_t *)&pCodeBase[ip]));
                if(pRef == 0) { SetError("Local variable not found"); return false; }
              }
              if(!pRef->IsReference()) { SetError("makeref to non reference variable"); return false; }

              if(BC_TokenGet() != COMMA) {SetError("missed ','"); return false;};

              Token_type = BC_TokenGet(ip,token_data_size);
              if(Token_type == VARIABLE)
              {
                if(!VarTab.GetVar(vi,*((uint32_t *)&pCodeBase[ip])))    { SetError("Global variable not found"); break;
            } pVar = vi.pDClass;
              }
              else
              {
                pVar = SStack.Read(pRun_fi->stack_offset,*((uint32_t *)&pCodeBase[ip]));
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
                if(!VarTab.GetVar(vi,*((uint32_t *)&pCodeBase[ip]))) { SetError("Global variable not found"); break; }
                pRef = vi.pDClass;
              }
              else
              {
                pRef = SStack.Read(pRun_fi->stack_offset,*((uint32_t *)&pCodeBase[ip]));
                if(pRef == 0) { SetError("Local variable not found"); return false; }
              }
              if(!pRef->IsAReference()) { SetError("makeref to non A reference variable"); return false; }

              if(BC_TokenGet() != COMMA) {SetError("missed ','"); return false;};

              Token_type = BC_TokenGet(ip,token_data_size);
              if(Token_type == VARIABLE)
              {
                if(!VarTab.GetVar(vi,*((uint32_t *)&pCodeBase[ip])))    { SetError("Global variable not found"); break;
            } pVar = vi.pDClass; if(pVar == 0) { SetError("Global variable not found"); return false; }
              }
              else
              {
                pVar = SStack.Read(pRun_fi->stack_offset,*((uint32_t *)&pCodeBase[ip]));
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
              //    pRoot = pRoot->CreateSubAClass(pRoot,(char *)&pRunCodeBase[TLR_DataOffset]);
              //    BC_TokenGet();
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
                    if(!VarTab.GetVar(vi,var_code))    { SetError("Global variable not found"); return false; }
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
        case VAR_PTR:
            break;
        case OPEN_BRACKET:
            break;
        case CLOSE_BRACKET:
            break;
        case COMMA:

            break;
        case DOT:

            break;
            /*            // off start whean ready compile expression
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

            if (Token_type == PUSH_OBJID_BXINDEX)
            {
                bUseIndex = true;
                rBX.Get(dwBXIndex);
            }
            else
                bUseIndex = false;

            pV = SStack.Push();
            pV->ClearType();

            pVV = GetOperand(pCodeBase, ip, &Token_type);

            if (!(Token_type == VARIABLE || Token_type == LOCAL_VARIABLE))
            {
                SetError("invalid arg for push_objid");
                return false;
            }
            if (bUseIndex)
            {
                pVV = pVV->GetArrayElement(dwBXIndex);
            }

            if (!(pVV->GetType() == VAR_AREFERENCE || pVV->GetType() == VAR_REFERENCE || pVV->GetType() == VAR_OBJECT))
            {
                SetError("not aref or ref or object");
            }

            pVV->Get(eid);
            pV->Set(eid);
            break;
            // process expression section ------------------

        case SETAREF:

            pV = SStack.Pop();
            pV->Get(eid);

            // destination
            Token_type = BC_TokenGet(ip, token_data_size);
            switch (Token_type)
            {
            case STACK_TOP:
                pVDst = SStack.Read();
                break;
            default:
                SetError("invalid argument for setaref (dst)");
                return false;
            }
            if (!pVDst)
            {
                SetError("invalid argument for setaref (dst)");
                return false;
            }
            pVDst = pVDst->GetVarPointer();
            if (!pVDst)
            {
                SetError("invalid argument for setaref (dst)");
                return false;
            }
            if (pVDst->GetType() != VAR_AREFERENCE)
            {
                SetError("isnt aref");
                return false;
            }

            // source
            Token_type = BC_TokenGet(ip, token_data_size);
            switch (Token_type)
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

            if (Token_type == SETREF_BXINDEX)
            {
                bUseIndex = true;
                rBX.Get(dwBXIndex);
            }
            else
                bUseIndex = false;

            pVDst = nullptr;
            pVSrc = nullptr;

            // destination
            Token_type = BC_TokenGet(ip, token_data_size);
            switch (Token_type)
            {
            case STACK_TOP:
                pVDst = SStack.Read();
                pVDst->ClearType();
                pVDst->SetType(VAR_REFERENCE);
                break;
            case VARIABLE:
                real_var = VarTab.GetVar(*((uint32_t *)&pCodeBase[ip]));
                if (real_var == nullptr)
                {
                    SetError("Global variable not found");
                    return false;
                }

                pVDst = real_var->value.get();
                // if(bUseIndex) pVDst = pVDst->GetArrayElement(dwBXIndex);
                if (!pVDst)
                    return false;
                if (pVDst->GetType() != VAR_REFERENCE)
                {
                    SetError("'%s' isnt reference", real_var->name.c_str());
                    return false;
                }
                break;
            case LOCAL_VARIABLE:
                pVDst = SStack.Read(pRun_fi->stack_offset, *((uint32_t *)&pCodeBase[ip]));
                if (pVDst == nullptr)
                {
                    SetError("Local variable not found");
                    return false;
                }
                // if(bUseIndex) pVDst = pVDst->GetArrayElement(dwBXIndex);
                if (!pVDst)
                    return false;
                if (pVDst->GetType() != VAR_REFERENCE)
                {
                    SetError("Local variable is not reference");
                    return false;
                }
                break;
            default:
                SetError("invalid argument for setref (dst)");
                return false;
            }
            if (!pVDst)
            {
                SetError("invalid argument for setref (dst)");
                return false;
            }

            // source
            Token_type = BC_TokenGet(ip, token_data_size);
            switch (Token_type)
            {
            case VARIABLE:
                real_var = VarTab.GetVar(*((uint32_t *)&pCodeBase[ip]));
                if (real_var == nullptr)
                {
                    SetError("Global variable not found");
                    return false;
                }

                pVSrc = real_var->value.get();
                if (bUseIndex)
                    pVSrc = pVSrc->GetArrayElement(dwBXIndex);
                if (!pVSrc)
                    return false;
                break;
            case LOCAL_VARIABLE:
                pVSrc = SStack.Read(pRun_fi->stack_offset, *((uint32_t *)&pCodeBase[ip]));
                if (pVSrc == nullptr)
                {
                    SetError("Local variable not found");
                    return false;
                }
                if (bUseIndex)
                    pVSrc = pVSrc->GetArrayElement(dwBXIndex);
                if (!pVSrc)
                    return false;
                break;
            default:
                SetError("invalid argument for setref (src)");
                return false;
            }

            pVDst->SetReference(pVSrc->GetVarPointer());

            break;
        case MOVE:

            pVDst = nullptr;
            pVSrc = nullptr;
            // find destination
            Token_type = BC_TokenGet(ip, token_data_size);
            switch (Token_type)
            {
            case LEFT_OPERAND:
                if (nLeftOperandCode == INVALID_VAR_CODE)
                {
                    SetError("no Lvalue");
                    return false;
                }

                if (bLeftOperandType == VARIABLE)
                {
                    real_var = VarTab.GetVar(nLeftOperandCode);
                    if (real_var == nullptr)
                    {
                        SetError("Global variable not found");
                        return false;
                    }

                    pVDst = real_var->value.get();
                }
                else
                {
                    if (pRun_fi)
                    {
                        pVDst = SStack.Read(pRun_fi->stack_offset, nLeftOperandCode);
                        if (pVDst == nullptr)
                        {
                            SetError("Local variable not found");
                            return false;
                        }
                    }
                    else
                    {
                        pVDst = nullptr;
                        SetError("Internal error, line 4808! Current function not defined...");
                        return false;
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

            if (pVDst == nullptr)
            {
                SetError("no LValue");
                return false;
            }

            // find source
            Token_type = BC_TokenGet(ip, token_data_size);
            switch (Token_type)
            {
            case STACK_TOP:
                pVSrc = SStack.Read();
                if (!pVSrc)
                    return false;
                break;
            default:
                SetError("incorrect SRC");
                return false;
            }

            // check for attribute operations
            if (pLeftOperandAClass != nullptr)
            {
                // set attribute class value
                const char *pChar;

                // use ExpressionResult as temporary register
                ExpressionResult.ClearType();
                ExpressionResult.Copy(pVSrc);

                // 2 signs precision --------------------------------
                if (ExpressionResult.GetType() == VAR_FLOAT)
                {
                    float fV1;
                    ExpressionResult.Get(fV1);
                    sprintf_s(gs, "%.7f", fV1);
                    ExpressionResult.Set(gs);
                }
                // 2 signs precision --------------------------------
                ExpressionResult.Convert(VAR_STRING);
                if (!ExpressionResult.Get(pChar))
                    break;
                pLeftOperandAClass->SetValue(pChar);

                if (nLeftOperandIndex != INVALID_ARRAY_INDEX)
                    pVDst = pVDst->GetArrayElement(nLeftOperandIndex);
                if (!pVDst)
                    return false;

                pVDst->Get(eid);
                if (bEntityUpdate)
                {
                    core.Entity_AttributeChanged(eid, pLeftOperandAClass);
                }
                break;
            }

            // test for references operations
            if (pVSrc->GetType() == VAR_REFERENCE && pVDst->GetType() == VAR_REFERENCE)
            {
                pVDst->SetReference(pVSrc->GetReference());
                break;
            }

            // find real dst
            pVDst = pVDst->GetVarPointer();
            if (!pVDst)
            {
                SetError("no DST");
                return false;
            }

            if (nLeftOperandIndex != INVALID_ARRAY_INDEX)
                pVDst = pVDst->GetArrayElement(nLeftOperandIndex);
            if (!pVDst)
                return false;

            // test for allowed data conversion
            DATA *pVTTSrc;
            DATA *pVTTDst;
            pVTTSrc = pVSrc->GetVarPointer();
            pVTTDst = pVDst->GetVarPointer();

            if (pVTTSrc->GetType() != pVTTDst->GetType())
            {
                // ??? think about a little later
                ExpressionResult.ClearType();
                ExpressionResult.Copy(pVSrc);
                if (ExpressionResult.IsReference())
                    ExpressionResult.RefConvert();
                if (!ExpressionResult.Convert(pVTTDst->GetType())) /// ??????????
                {
                    break; // skip unallowed conversion, write to log(in Convert func)
                }
                pVSrc = ExpressionResult.GetVarPointer(); // converted source
            }

            // if not attribute operation proceed to variables copy

            // if(nLeftOperandIndex != INVALID_ARRAY_INDEX) pVDst = pVDst->GetArrayElement(nLeftOperandIndex);
            // if(!pVDst) return false;
            pVDst->Copy(pVSrc);

            break;

        case STACK_ALLOC:
            SStack.Push();
            break;

        case STACK_WRITE_BXINDEX:
        case STACK_PUSH:
        case STACK_WRITE:

            if (Token_type == STACK_WRITE_BXINDEX)
            {
                bUseIndex = true;
                rBX.Get(dwBXIndex);
            }
            else
                bUseIndex = false;

            if (Token_type == STACK_PUSH)
                pV = SStack.Push();
            else
                pV = SStack.Read();

            Token_type = BC_TokenGet(ip, token_data_size);
            switch (Token_type)
            {
            case NUMBER:
                pV->Set(*((long *)&pRunCodeBase[TLR_DataOffset]));
                break;
            case FLOAT_NUMBER:
                pV->Set(*((float *)&pRunCodeBase[TLR_DataOffset]));
                break;
            case STRING:
                pV->Set((char *)&pRunCodeBase[TLR_DataOffset + 4]); // 4 - string length
                break;
            case VARIABLE:
                real_var = VarTab.GetVar(*((uint32_t *)&pCodeBase[ip]));
                if (real_var == nullptr)
                {
                    SetError("Global variable not found");
                    break;
                }

                pVV = real_var->value.get();
                // pVV = pVV->GetVarPointer();
                // if(!pVV) { SetError("invalid ref"); break; }
                if (pVV->IsReference())
                {
                    if (bUseIndex)
                    {
                        pVV = pVV->GetVarPointer();
                        pVV = pVV->GetArrayElement(dwBXIndex);
                        if (!pVV)
                        {
                            SetError("invalid ref");
                            break;
                        }

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

                if (bUseIndex)
                {
                    pVV = pVV->GetArrayElement(dwBXIndex);
                    if (!pVV)
                    {
                        SetError("invalid array index");
                        break;
                    }
                    pV->Copy(pVV);
                }
                else
                    pV->Copy(pVV);

                break;
            case LOCAL_VARIABLE:
                pVar = SStack.Read(pRun_fi->stack_offset, *((uint32_t *)&pCodeBase[ip]));
                if (pVar == nullptr)
                {
                    SetError("Local variable not found");
                    return false;
                }
                if (pVar->IsReference())
                {
                    if (bUseIndex)
                    {
                        pVar = pVar->GetVarPointer();
                        pVar = pVar->GetArrayElement(dwBXIndex);
                        if (!pVar)
                        {
                            SetError("invalid ref");
                            break;
                        }

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

                // if(!pVar) { SetError("invalid ref"); break; }

                if (bUseIndex)
                {
                    pVar = pVar->GetArrayElement(dwBXIndex);
                    if (!pVar)
                    {
                        SetError("invalid array index");
                        break;
                    }
                    pV->Copy(pVar);
                }
                else
                    pV->Copy(pVar);

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
                if (!rAP)
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
            if (SStack.GetDataNum() == 0)
                break;
            if (Token_type == STACK_POP)
                pV = SStack.Pop();
            else
                pV = SStack.Read();

            Token_type = BC_TokenGet(ip, token_data_size);
            switch (Token_type)
            {
            case VARIABLE:
                real_var = VarTab.GetVar(*((uint32_t *)&pCodeBase[ip]));
                if (real_var == nullptr)
                {
                    SetError("Global variable not found");
                    break;
                }

                real_var->value->Copy(pV);
                break;
            case LOCAL_VARIABLE:
                pVar = SStack.Read(pRun_fi->stack_offset, *((uint32_t *)&pCodeBase[ip]));
                if (pVar == nullptr)
                {
                    SetError("Local variable not found");
                    return false;
                }
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
            pV = GetOperand(pCodeBase, ip);
            if (!pV)
                return false;
            pV->BoolConvert();
            break;
        case OP_BOOL_AND:
        case OP_BOOL_OR:
            pV = GetOperand(pCodeBase, ip);
            if (!pV)
                return false;
            pVV = GetOperand(pCodeBase, ip);
            if (!pVV)
                return false;

            pV->BoolConvert();
            pV->RefConvert();
            pV->CompareAndSetResult(pVV, Token_type);
            break;
        case OP_NOT_EQUAL:
        case OP_LESSER_OR_EQUAL:
        case OP_LESSER:
        case OP_GREATER_OR_EQUAL:
        case OP_GREATER:
        case OP_BOOL_EQUAL:
            pV = GetOperand(pCodeBase, ip);
            if (!pV)
                return false;
            pVV = GetOperand(pCodeBase, ip);
            if (!pVV)
                return false;
            pV->RefConvert();
            pV->CompareAndSetResult(pVV, Token_type);
            break;
        case OP_PLUS:
            pV = GetOperand(pCodeBase, ip);
            if (!pV)
                return false;
            pVV = GetOperand(pCodeBase, ip);
            if (!pVV)
                return false;
            pV->Plus(pVV);
            break;
        case OP_MINUS:
            pV = GetOperand(pCodeBase, ip);
            if (!pV)
                return false;
            pVV = GetOperand(pCodeBase, ip);
            if (!pVV)
                return false;
            pV->Minus(pVV);
            break;
        case OP_MULTIPLY:
            pV = GetOperand(pCodeBase, ip);
            if (!pV)
                return false;
            pVV = GetOperand(pCodeBase, ip);
            if (!pVV)
                return false;
            pV->Multiply(pVV);
            break;
        case OP_DIVIDE:
            pV = GetOperand(pCodeBase, ip);
            if (!pV)
                return false;
            pVV = GetOperand(pCodeBase, ip);
            if (!pVV)
                return false;
            pV->Divide(pVV);
            break;
        case OP_MODUL:
            pV = GetOperand(pCodeBase, ip);
            if (!pV)
                return false;
            pVV = GetOperand(pCodeBase, ip);
            if (!pVV)
                return false;
            pV->Modul(pVV);
            break;
        case OP_POWER:
            pV = GetOperand(pCodeBase, ip);
            if (!pV)
                return false;
            pVV = GetOperand(pCodeBase, ip);
            if (!pVV)
                return false;
            pV->Power(pVV);
            break;
        case OP_SMINUS:
            pV = GetOperand(pCodeBase, ip);
            if (!pV)
                return false;
            pV->Inverse();
            break;
        case OP_BOOL_NEG:
            pV = GetOperand(pCodeBase, ip);
            if (!pV)
                return false;
            pV->Neg();
            break;

        case MOVEAP_BXINDEX:
        case MOVEAP:

            if (Token_type == MOVEAP_BXINDEX)
            {
                bUseIndex = true;
                rBX.Get(dwBXIndex);
            }
            else
                bUseIndex = false;

            pV = GetOperand(pCodeBase, ip, &Token_type);
            switch (Token_type)
            {
            case VARIABLE:
            case LOCAL_VARIABLE:
                SetError("not implemented");
                return false;
                break;
            case AP:
                pVV = GetOperand(pCodeBase, ip, &Token_type);
                if (Token_type == AP)
                    break;
                if (!pVV)
                    return false;
                if (bUseIndex)
                    pVV = pVV->GetArrayElement(dwBXIndex);
                if (!pVV)
                {
                    SetError("invalid array index");
                    return false;
                }
                rAP = pVV->GetAClass();
                break;
            }
            break;
        case VERIFY_AP:
        case ADVANCE_AP:

            S_TOKEN_TYPE sttV = Token_type;

            const char *pChar;
            if (!rAP)
            {
                SetError("null ap");
                BC_TokenGet();
                break; /*return false;*/
            }

            Token_type = BC_TokenGet();
            switch (Token_type)
            {
            case ACCESS_WORD_CODE:
                if (sttV == VERIFY_AP)
                    rAP = rAP->VerifyAttributeClassByCode(*((long *)&pRunCodeBase[TLR_DataOffset]));
                else
                    rAP = rAP->GetAttributeClassByCode(*((long *)&pRunCodeBase[TLR_DataOffset]));
                if (!rAP)
                    SetError("missed attribute: %s", SCodec.Convert(*((long *)&pRunCodeBase[TLR_DataOffset])));
                break;
            case VARIABLE:
                real_var = VarTab.GetVar(*((long *)&pRunCodeBase[TLR_DataOffset]));
                if (real_var == nullptr)
                {
                    SetError("Global variable not found");
                    break;
                }

                ExpressionResult.ClearType();
                ExpressionResult.Copy(real_var->value.get());
                if (!ExpressionResult.Convert(VAR_STRING))
                {
                    SetError("invalid type for attribute var");
                    break; /*return false;*/
                }
                ExpressionResult.Get(pChar);
                if (sttV == VERIFY_AP)
                    rAP = rAP->CreateSubAClass(rAP, pChar);
                else
                    rAP = rAP->FindAClass(rAP, pChar);

                /*if(sttV == VERIFY_AP)
                rAP = rAP->VerifyAttributeClass(pChar);
                  else
                rAP = rAP->GetAttributeClass(pChar);*/
                if (!rAP)
                    SetError("missed attribute: %s", pChar);
                break;
            case LOCAL_VARIABLE:
                pV = SStack.Read(pRun_fi->stack_offset, *((long *)&pRunCodeBase[TLR_DataOffset]));
                if (pV == nullptr)
                {
                    SetError("Local variable not found");
                    break; /*return false;*/
                }

                ExpressionResult.ClearType();
                ExpressionResult.Copy(pV);
                if (!ExpressionResult.Convert(VAR_STRING))
                {
                    SetError("invalid type for attribute var");
                    break; /*return false;*/
                }
                ExpressionResult.Get(pChar);

                if (sttV == VERIFY_AP)
                    rAP = rAP->CreateSubAClass(rAP, pChar);
                else
                    rAP = rAP->FindAClass(rAP, pChar);

                /*if(sttV == VERIFY_AP)
                  rAP = rAP->VerifyAttributeClass(pChar);
                else
                rAP = rAP->GetAttributeClass(pChar);*/

                if (!rAP)
                    SetError("missed attribute: %s", pChar);

                break;
            default:
                SetError("invalid arg type for ADVANCE_AP");
                return false;
            }

            // pLeftOperandAClass = rAP;

            break;

            //*/
            //----------------------------------------------
        }
    } while (Token_type != END_OF_PROGRAMM && bExit != true);

    if (Token_type == END_OF_PROGRAMM && inout != 0)
        SetError("Unexpected end of programm");
    // UNGUARD
    return false;
}

void COMPILER::CompileNumber(SEGMENT_DESC &Segment)
{
    long lvalue;
    lvalue = static_cast<long>(atoll(Token.GetData()));
    CompileToken(Segment, NUMBER, 1, (char *)&lvalue, sizeof(long));
}

void COMPILER::CompileFloatNumber(SEGMENT_DESC &Segment)
{
    float fvalue;
    fvalue = static_cast<float>(atof(Token.GetData()));
    CompileToken(Segment, FLOAT_NUMBER, 1, (char *)&fvalue, sizeof(float));
}

void COMPILER::CompileString(SEGMENT_DESC &Segment)
{
    uint32_t string_size;
    const char *pData = Token.GetData();
    if (pData != nullptr)
    {
        string_size = strlen(Token.GetData()) + 1;
        CompileToken(Segment, STRING, 2, (char *)&string_size, sizeof(uint32_t), Token.GetData(), string_size);
    }
    else
    {
        string_size = strlen("") + 1;
        CompileToken(Segment, STRING, 2, (char *)&string_size, sizeof(uint32_t), "", string_size);
    }
}

bool COMPILER::CompileUnknown(SEGMENT_DESC &Segment)
{
    uint32_t func_code;
    uint32_t var_code;

    // check for valid function code
    func_code = FuncTab.FindFunc(Token.GetData());

    if (func_code != INVALID_FUNC_CODE)
    {
        // this is function call
        CompileToken(Segment, CALL_FUNCTION, 1, (char *)&func_code, sizeof(uint32_t));
        // Trace("function call: %s ; code = %d",Token.GetData(),func_code);
        return true;
    }

    // check for valid local variable
    var_code = FuncTab.FindVar(CurrentFuncCode, Token.GetData());
    if (var_code != INVALID_VAR_CODE)
    {
        CompileToken(Segment, LOCAL_VARIABLE, 1, (char *)&var_code, sizeof(uint32_t));
        // Trace("local variable: %s ; code = %d",Token.GetData(),var_code);
        return true;
    }

    var_code = VarTab.FindVar(Token.GetData());
    if (var_code != INVALID_VAR_CODE)
    {
        // this is gloabl variable
        CompileToken(Segment, VARIABLE, 1, (char *)&var_code, sizeof(uint32_t));
        // Trace("variable: %s ; code = %d",Token.GetData(),var_code);
        return true;
    }

    // def_code = DefineTable.GetStringCode(Token.GetData());
    const uint32_t def_code = DefTab.FindDef(Token.GetData());
    if (def_code != INVALID_DEF_CODE)
    {
        DEFINFO di;
        // DefineTable.GetStringData(def_code,&di);
        DefTab.GetDef(di, def_code);
        switch (di.deftype)
        {
        case NUMBER:
            CompileToken(Segment, NUMBER, 1, (char *)&di.data4b, sizeof(uint32_t));
            break;
        case FLOAT_NUMBER:
            CompileToken(Segment, FLOAT_NUMBER, 1, (char *)&di.data4b, sizeof(uint32_t));
            break;
        case STRING:
            uint32_t string_size;
            string_size = strlen((char *)di.data4b) + 1;
            CompileToken(Segment, STRING, 2, (char *)&string_size, sizeof(uint32_t), (char *)di.data4b, string_size);
            break;
        }
        return true;
    }

    const uint32_t lab_code = LabelTable.GetStringCode(Token.GetData());
    if (lab_code != INVALID_ORDINAL_NUMBER)
    {
        LabelTable.SetStringData(lab_code, &Segment.BCode_Program_size);
        return true;
    }

    SetError("Undeclared identifier: %s", Token.GetData());
    return false;
}

S_TOKEN_TYPE COMPILER::DetectUnknown(uint32_t &code)
{
    // check for valid function code
    const uint32_t func_code = FuncTab.FindFunc(Token.GetData());

    if (func_code != INVALID_FUNC_CODE)
    {
        // this is function call
        code = func_code;
        return CALL_FUNCTION;
    }

    // check for valid local variable
    uint32_t var_code = FuncTab.FindVar(CurrentFuncCode, Token.GetData());
    if (var_code != INVALID_VAR_CODE)
    {
        code = var_code;
        return LOCAL_VARIABLE;
    }

    var_code = VarTab.FindVar(Token.GetData());
    if (var_code != INVALID_VAR_CODE)
    {
        code = var_code;
        return VARIABLE;
    }

    const uint32_t def_code = DefTab.FindDef(Token.GetData());
    if (def_code != INVALID_DEF_CODE)
    {
        code = def_code;
        return DEFINE_VAL;
        /*DEFINFO di;
        DefTab.GetDef(di,def_code);
        switch(di.deftype)
        {
          case NUMBER:
            CompileToken(Segment,NUMBER,1,(char *)&di.data4b,sizeof(uint32_t));
          break;
          case FLOAT_NUMBER:
            CompileToken(Segment,FLOAT_NUMBER,1,(char *)&di.data4b,sizeof(uint32_t));
          break;
          case STRING:
            uint32_t string_size;
            string_size = strlen((char *)di.data4b) + 1;
            CompileToken(Segment,STRING,2,(char *)&string_size,sizeof(uint32_t),(char *)di.data4b,string_size);
          break;
        }
        return true;*/
    }

    return UNKNOWN;
}

void COMPILER::ExitProgram()
{
    const uint32_t function_code = FuncTab.FindFunc("ExitMain");
    if (function_code != INVALID_FUNC_CODE)
    {
        DATA *pResult;
        BC_Execute(function_code, pResult);
    }
    core.Exit();
}

void COMPILER::ClearEvents()
{
    // EventTab.Release();
    EventTab.Clear();
}

void COMPILER::SaveDataDebug(char *data_PTR, ...)
{
    char LogBuffer[MAX_PATH + MAX_PATH];
    if (data_PTR == nullptr)
    {
        return;
    }
    va_list args;
    va_start(args, data_PTR);
    _vsnprintf_s(LogBuffer, sizeof(LogBuffer) - 4, data_PTR, args);
    strcat_s(LogBuffer, "\x0d\x0a");
    va_end(args);
}

void COMPILER::SaveData(const void *data_PTR, uint32_t data_size)
{
    if (data_PTR == nullptr)
        return;

    if (dwCurPointer + data_size > dwMaxSize)
    {
        const uint32_t dwNewAllocate = (1 + (dwCurPointer + data_size) / (1024 * 1024)) * (1024 * 1024);
        // pBuffer = (char*)RESIZE(pBuffer, dwNewAllocate);
        auto *const newPtr = new char[dwNewAllocate];
        memcpy(newPtr, pBuffer, dwMaxSize);
        delete[] pBuffer;
        pBuffer = newPtr;

        dwMaxSize = dwNewAllocate;
    }

    /*    if (data_size < 16)
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
      else*/
    memcpy(&pBuffer[dwCurPointer], data_PTR, data_size);

    dwCurPointer += data_size;
}

bool COMPILER::ReadData(void *data_PTR, uint32_t data_size)
{
    if (data_PTR == nullptr)
    {
        dwCurPointer += data_size;
        return true;
    }
    if (dwCurPointer + data_size > dwMaxSize)
        return false;

    /*    if (data_size < 16)
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
      else*/
    memcpy(data_PTR, &pBuffer[dwCurPointer], data_size);

    dwCurPointer += data_size;

    return true;
}

bool COMPILER::FindReferencedVariable(DATA *pRef, uint32_t &var_index, uint32_t &array_index)
{
    const uint32_t nVarNum = VarTab.GetVarNum();
    for (uint32_t n = 0; n < nVarNum; n++)
    {
        const auto *real_var = VarTab.GetVarX(n);
        if (real_var == nullptr)
        {
            SetError("Invalid var code");
            continue;
        }

        const auto &vi = *real_var;
        if (!vi.value->IsArray())
        {
            if (pRef == vi.value.get())
            {
                var_index = n;
                array_index = 0xffffffff;
                return true;
            }
        }
        else
        {
            for (uint32_t i = 0; i < vi.elements; i++)
            {
                if (pRef == vi.value->GetArrayElement(i))
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

bool COMPILER::FindReferencedVariableByRootA(ATTRIBUTES *pA, uint32_t &var_index, uint32_t &array_index)
{
    VarInfo vi;

    const uint32_t nVarNum = VarTab.GetVarNum();
    for (uint32_t n = 0; n < nVarNum; n++)
    {
        const auto *real_var = VarTab.GetVarX(n);
        if (real_var == nullptr)
        {
            SetError("Invalid var code");
            continue;
        }

        const auto &vi = *real_var;
        if (vi.type != VAR_OBJECT)
            continue;
        if (!vi.value->IsArray())
        {
            if (pA == vi.value->AttributesClass)
            {
                var_index = n;
                array_index = 0xffffffff; // ***
                return true;
            }
        }
        else
        {
            for (uint32_t i = 0; i < vi.elements; i++)
            {
                if (pA == vi.value->GetArrayElement(i)->AttributesClass)
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

ATTRIBUTES *COMPILER::TraceARoot(ATTRIBUTES *pA, const char *&pAccess)
{
    if (pA == nullptr)
        return nullptr; // error or invalid argument
    if (pA->GetParent() == nullptr)
        return pA; // root found
    if (pA->GetThisNameCode() == 0)
        return nullptr; // fix crash at NewGame start

    const long slen = strlen(pA->GetThisName()) + 1;

    char *pAS = new char[slen];

    memcpy(pAS, pA->GetThisName(), slen);

    if (pAccess == nullptr)
    {
        pAccess = pAS;
    }
    else
    {
        const auto len = slen + strlen(pAccess) + 1;
        // pAS = (char *)RESIZE(pAS, len);
        auto *const newPtr = new char[len];
        memcpy(newPtr, pAS, slen);
        delete[] pAS;
        pAS = newPtr;
        strcat_s(pAS, len, ".");
        strcat_s(pAS, len, pAccess);
        delete[] pAccess;
        pAccess = pAS;
    }

    return TraceARoot(pA->GetParent(), pAccess);
}

void COMPILER::WriteVDword(uint32_t v)
{
    /*    BYTE nbv;
      if(v < 0xff) { nbv = (BYTE)v; SaveData(&nbv, sizeof(nbv)); }
      else
      {
        nbv = 0xff; SaveData(&nbv, sizeof(nbv));
        SaveData(&v, sizeof(v));
      }
    */
    uint8_t nbv;
    uint16_t nwv;
    if (v < 0xfe)
    {
        nbv = static_cast<uint8_t>(v);
        SaveData(&nbv, sizeof(nbv));
    }
    else if (v < 0xffff)
    {
        nbv = 0xfe;
        SaveData(&nbv, sizeof(nbv));
        nwv = static_cast<uint16_t>(v);
        SaveData(&nwv, sizeof(nwv));
    }
    else
    {
        nbv = 0xff;
        SaveData(&nbv, sizeof(nbv));
        SaveData(&v, sizeof(v));
    }
}

uint32_t COMPILER::ReadVDword()
{
    uint8_t nbv;
    uint16_t nwv;
    uint32_t v;
    ReadData(&nbv, 1);
    if (nbv < 0xfe)
    {
        return nbv;
    }
    if (nbv == 0xfe)
    {
        ReadData(&nwv, sizeof(nwv));
        return nwv;
    }
    ReadData(&v, sizeof(uint32_t));
    return v;
}

void COMPILER::SaveString(const char *pS)
{
    if (pS == nullptr)
    {
        WriteVDword(0);
        return;
    }
    const uint32_t n = strlen(pS) + 1;
    WriteVDword(n);
    SaveData(pS, n);
}

char *COMPILER::ReadString()
{
    const uint32_t n = ReadVDword();
    if (n == 0)
        return nullptr;

    char *pBuffer = new char[n];
    ReadData(pBuffer, n);
    return pBuffer;
}

bool COMPILER::ReadVariable(char *name, /* DWORD code,*/ bool bDim, uint32_t a_index)
{
    long nLongValue;
    uintptr_t ptrValue;
    float fFloatValue;
    char *pString;
    uint32_t var_index;
    uint32_t array_index;
    uint32_t nElementsNum;
    ATTRIBUTES *pA;
    S_TOKEN_TYPE eType;
    const VarInfo *real_var;
    const VarInfo *real_var_ref;
    DATA *pV;
    DATA *pVRef;
    entid_t eid;
    uint32_t var_code;
    bool bSkipVariable;

    bSkipVariable = false;
    var_code = VarTab.FindVar(name);
    if (var_code == INVALID_VAR_CODE)
    {
        SetError("Load warning - variable: '%s' not found", name);
        bSkipVariable = true;
    }
    else
    {
        real_var = VarTab.GetVarX(var_code);
        if (real_var == nullptr)
        {
            SetError("Load warning - variable: '%s' has invalid var code", name);
            bSkipVariable = true;
        }
        else
        {
            pV = real_var->value.get();
        }
    }

    if (!bDim) // skip this info for array elements
    {
        // trace("Read[%d]: %s",code,vi.name);
        ReadData(&eType, sizeof(eType));
        Assert(eType < S_TOKEN_TYPE::TOKEN_TYPES_COUNT);
        if (!bSkipVariable)
            if (real_var->type != eType)
            {
                SetError("load type mismatch");
                return false;
            }
        ReadData(&nElementsNum, sizeof(nElementsNum));
        if (!bSkipVariable)
            if (real_var->elements != nElementsNum)
            {
                // ???
                // SetError("load size mismatch");
                // return false;
                real_var->value->SetElementsNum(nElementsNum);
                if (!VarTab.SetElementsNum(var_code, nElementsNum))
                {
                    core.Trace("Unable to set elements num for %s", real_var->name.c_str());
                }
            }
    }
    else
    {
        if (bSkipVariable)
        {
            SetError("Load - lost element");
            return false;
        }
        nElementsNum = 1;
        eType = pV->GetType();
        pV = pV->GetArrayElement(a_index);
    }

    if (nElementsNum > 1) // array
    {
        // load array elements
        for (uint32_t n = 0; n < nElementsNum; n++)
        {
            if (!ReadVariable(name, /*code,*/ true, n))
                return false;
        }
        return true;
    }

    switch (eType)
    {
    case VAR_INTEGER:
        ReadData(&nLongValue, sizeof(nLongValue));
        if (bSkipVariable)
            break;
        pV->Set(nLongValue);
        break;
    case VAR_PTR:
        ReadData(&ptrValue, sizeof(ptrValue));
        if (bSkipVariable)
            break;
        pV->SetPtr(ptrValue);
        break;
    case VAR_FLOAT:
        ReadData(&fFloatValue, sizeof(fFloatValue));
        if (bSkipVariable)
            break;
        pV->Set(fFloatValue);
        break;
    case VAR_STRING:
        pString = ReadString();
        if (pString)
        {
            if (!bSkipVariable)
                pV->Set(pString);
            delete[] pString;
        }
        break;
    case VAR_OBJECT:
        ReadData(&eid, sizeof(eid));
        if (!bSkipVariable)
        {
            pV->Set(eid);

            if (pV->AttributesClass == nullptr)
                pV->AttributesClass = new ATTRIBUTES(&SCodec);
            ReadAttributesData(pV->AttributesClass, nullptr);
        }
        else
        {
            ATTRIBUTES *pTA = new ATTRIBUTES(&SCodec);
            ReadAttributesData(pTA, nullptr);
            delete pTA;
        }
        break;
    case VAR_REFERENCE:
        var_index = ReadVDword();
        if (var_index == 0xffffffff)
            break; // uninitialized ref
        array_index = ReadVDword();
        if (bSkipVariable)
            break;
        real_var_ref = VarTab.GetVarX(var_index);
        if (real_var_ref == nullptr)
        {
            SetError("State read error");
            return false;
        }

        pVRef = real_var_ref->value.get();
        if (array_index != 0xffffffff)
        {
            pVRef = pVRef->GetArrayElement(array_index);
        }
        pV->SetReference(pVRef);
        break;
    case VAR_AREFERENCE:
        pA = nullptr;
        var_index = ReadVDword();
        if (var_index == 0xffffffff)
            break;
        array_index = ReadVDword();
        pString = ReadString();
        if (bSkipVariable)
        {
            delete pString;
            break;
        }

        real_var_ref = VarTab.GetVarX(var_index);
        if (real_var_ref == nullptr)
        {
            delete[] pString;
            SetError("State read error");
            return false;
        }

        pVRef = real_var_ref->value.get();
        if (array_index != 0xffffffff)
        {
            pVRef = pVRef->GetArrayElement(array_index);
        }

        if (pVRef->AttributesClass == nullptr)
            pVRef->AttributesClass = new ATTRIBUTES(&SCodec);
        if (pString)
        {
            pA = pVRef->AttributesClass->CreateSubAClass(pVRef->AttributesClass, pString);
            delete[] pString;
        }
        pV->SetAReference(pA);

        break;
    default:
        SetError("Unknown token type: %i", eType);
    }

    return true;
}

void COMPILER::SaveVariable(DATA *pV, bool bdim)
{
    long nLongValue;
    uintptr_t ptrValue;
    float fFloatValue;
    const char *pString;
    uint32_t var_index;
    uint32_t array_index;
    uint32_t n;
    ATTRIBUTES *pA;
    S_TOKEN_TYPE eType;
    entid_t eid;

    if (pV == nullptr)
    {
        SetError("Zero Variable Pointer");
        return;
    }
    if (!bdim) // skip this info for array elements
    {
        eType = pV->GetType();
        SaveData(&eType, sizeof(eType));

        // n = pV->GetElementsNum();
        n = pV->Number_of_elements;
        if (n == 0)
            n = 1; // for reference variables
        SaveData(&n, sizeof(n));
    }

    if (pV->IsArray())
    {
        for (n = 0; n < pV->GetElementsNum(); n++)
        {
            // sprintf_s(gs,"    [index : %d]\n",n);
            // OutputDebugString(gs);
            SaveVariable(pV->GetArrayElement(n), true);
        }
        return;
    }

    switch (pV->GetType())
    {
    case VAR_INTEGER:
        pV->Get(nLongValue);
        SaveData(&nLongValue, sizeof(nLongValue));
        break;
    case VAR_PTR:
        pV->GetPtr(ptrValue);
        SaveData(&ptrValue, sizeof(ptrValue));
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

        if (pV->GetReference() == nullptr)
        {
            // uninitialized reference
            WriteVDword(0xffffffff);
            return;
        }
        if (!FindReferencedVariable(pV->GetVarPointer(), var_index, array_index))
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
        pString = nullptr;
        __try
        {
            pA = TraceARoot(pV->AttributesClass, pString);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            SetError("Save - ARef to non existing attributes branch");
            WriteVDword(0xffffffff);
            break;
        }
        if (pA == nullptr)
        {
            WriteVDword(0xffffffff);
            delete[] pString;
            break;
        }
        if (!FindReferencedVariableByRootA(pA, var_index, array_index))
        {
            SetError("Ghost A reference");
            WriteVDword(0xffffffff);
            delete[] pString;
            break;
        }
        WriteVDword(var_index);
        WriteVDword(array_index);
        SaveString(pString);
        delete[] pString;
        break;
    }
}

bool COMPILER::OnLoad()
{
    const uint32_t function_code = FuncTab.FindFunc("OnLoad");
    DATA *pResult;
    BC_Execute(function_code, pResult);
    return true;
}

bool COMPILER::SaveState(std::fstream &fileS)
{
    uint32_t n;
    delete pBuffer;
    pBuffer = nullptr;

    dwCurPointer = 0;
    dwMaxSize = 0;

    DATA *pResult;
    const uint32_t function_code = FuncTab.FindFunc("OnSave");
    if (function_code != INVALID_FUNC_CODE)
        BC_Execute(function_code, pResult);

    EXTDATA_HEADER edh;
    auto *pVDat = static_cast<VDATA *>(core.GetScriptVariable("savefile_info"));
    if (pVDat && pVDat->GetString())
        sprintf_s(edh.sFileInfo, sizeof(edh.sFileInfo), "%s", pVDat->GetString());
    else
        sprintf_s(edh.sFileInfo, sizeof(edh.sFileInfo), "save");
    edh.dwExtDataOffset = 0;
    edh.dwExtDataSize = 0;

    fio->_WriteFile(fileS, &edh, sizeof(edh));

    // 1. Program Directory
    SaveString(ProgramDirectory);

    // 4. SCodec data
    WriteVDword(SCodec.GetNum());
    for (n = 0; n < SCodec.GetNum(); n++)
    {
        if (n == 0)
            SaveString(SCodec.Get());
        else
            SaveString(SCodec.GetNext());
    }

    const uint32_t nSegNum = 1; // SegmentsNum;
    // 2. Data Segments
    WriteVDword(nSegNum);

    // 3. Segments names
    for (n = 0; n < nSegNum; n++)
    {
        if (SegmentTable[n].name)
        {
            SaveString(SegmentTable[n].name);
        }
    }

    // 5. Variables table
    const uint32_t nVarNum = VarTab.GetVarNum();
    WriteVDword(nVarNum);

    const VarInfo *last_var{ nullptr };
    for (n = 0; n < nVarNum; n++)
    {
        const VarInfo *real_var = VarTab.GetVar(n);
        if (real_var == nullptr)
        {
            real_var = last_var; // preserve old semanthics
        }
        else
        {
            last_var = real_var;
        }

        SaveString(real_var->name.c_str()); // ***
        SaveVariable(real_var->value.get());
    }

    uint64_t dw2;
    if (dwCurPointer)
    {
        char *pDst = new char[dwCurPointer * 2];
        uint32_t dwPackLen = dwCurPointer * 2;
        RDTSC_B(dw2);
        compress2((Bytef *)pDst, (uLongf *)&dwPackLen, (Bytef *)pBuffer, dwCurPointer, Z_BEST_COMPRESSION);
        RDTSC_E(dw2);

        fio->_WriteFile(fileS, &dwCurPointer, sizeof(dwCurPointer));
        fio->_WriteFile(fileS, &dwPackLen, sizeof(dwPackLen));
        fio->_WriteFile(fileS, pDst, dwPackLen);

        delete[] pDst;
    }

    delete[] pBuffer;
    pBuffer = nullptr;

    return true;
}

bool COMPILER::LoadState(std::fstream &fileS)
{
    uint32_t n;
    char *pString;

    delete pBuffer;
    pBuffer = nullptr;

    EXTDATA_HEADER exdh;
    fio->_ReadFile(fileS, &exdh, sizeof(exdh));

    uint32_t dwPackLen;
    fio->_ReadFile(fileS, &dwMaxSize, sizeof(dwMaxSize));
    fio->_ReadFile(fileS, &dwPackLen, sizeof(dwPackLen));
    if (dwPackLen == 0 || dwPackLen > 0x8000000 || dwMaxSize == 0 || dwMaxSize > 0x8000000)
    {
        return false;
    }
    char *pCBuffer = new char[dwPackLen];
    pBuffer = new char[dwMaxSize];
    fio->_ReadFile(fileS, pCBuffer, dwPackLen);
    uncompress((Bytef *)pBuffer, (uLongf *)&dwMaxSize, (Bytef *)pCBuffer, dwPackLen);
    delete[] pCBuffer;
    dwCurPointer = 0;

    // Release all data
    Release();

    // save specific data (name, time, etc)
    // DWORD nSDataSize = ReadVDword();
    // if(nSDataSize) ReadData(0,nSDataSize);

    // skip ext data header
    // EXTDATA_HEADER edh;
    // ReadData(&edh, sizeof(edh));

    // 1. Program Directory
    ProgramDirectory = ReadString();

    // 4. SCodec data
    const uint32_t nSCStringsNum = ReadVDword();
    for (n = 0; n < nSCStringsNum; n++)
    {
        pString = ReadString();
        if (pString)
        {
            Assert(utf8::IsValidUtf8(pString));
            SCodec.Convert(pString);
            delete[] pString;
        }
    }

    const uint32_t nSegments2Load = ReadVDword();

    // 3.  Segments names
    // 3.a Initialize internal functions
    // 3.b Load preprocess
    InitInternalFunctions();
    LoadPreprocess();

    for (n = 0; n < nSegments2Load; n++)
    {
        char *pSegmentName = ReadString();
        Assert(utf8::IsValidUtf8(pSegmentName));
        if (!BC_LoadSegment(pSegmentName))
            return false;
        delete[] pSegmentName;
    }

    // 5. Variables table, all variables created during previous step, just read value
    const uint32_t nVarNum = ReadVDword();
    for (n = 0; n < nVarNum; n++)
    {
        pString = ReadString();
        if (pString == nullptr || strcmp(pString, "") == 0)
        {
            SetError("missing variable name");
            return false;
        }
        Assert(utf8::IsValidUtf8(pString));
        ReadVariable(pString /*,n*/);

        delete[] pString;
    }

    // call to script function "OnLoad()"
    OnLoad();

    delete pBuffer;
    pBuffer = nullptr;

    return true;
}

void COMPILER::ReadAttributesData(ATTRIBUTES *pRoot, ATTRIBUTES *pParent)
{
    uint32_t nSubClassesNum;
    uint32_t n;
    uint32_t nNameCode;
    // char * pName;
    char *pValue;

    if (pRoot == nullptr)
    {
        nSubClassesNum = ReadVDword();
        nNameCode = ReadVDword();

        // DTrace(SCodec.Convert(nNameCode));
        pValue = ReadString();
        pParent->SetAttribute(nNameCode, pValue);
        pRoot = pParent->GetAttributeClassByCode(nNameCode);
        delete[] pValue;
        for (n = 0; n < nSubClassesNum; n++)
        {
            ReadAttributesData(nullptr, pRoot);
        }

        return;
    }

    nSubClassesNum = ReadVDword();
    nNameCode = ReadVDword();
    pValue = ReadString();
    // pRoot->SetAttribute(nNameCode,pValue);

    pRoot->SetNameCode(nNameCode);
    pRoot->SetValue(pValue);

    for (n = 0; n < nSubClassesNum; n++)
    {
        // ReadAttributesData(pRoot->GetAttributeClass(n));
        ReadAttributesData(nullptr, pRoot);
    }

    // if(pName) delete pName;
    delete[] pValue;
}

void COMPILER::SaveAttributesData(ATTRIBUTES *pRoot)
{
    if (pRoot == nullptr)
    {
        WriteVDword(0);      // number of subclasses
        SaveString(nullptr); // attribute name
        SaveString(nullptr); // attribute value
        return;
    }
    WriteVDword(pRoot->GetAttributesNum()); // number of subclasses

    // save attribute name
    // SaveString(pRoot->GetThisName());
    WriteVDword(pRoot->GetThisNameCode());

    // save attribute value
    SaveString(pRoot->GetThisAttr());
    for (uint32_t n = 0; n < pRoot->GetAttributesNum(); n++)
    {
        SaveAttributesData(pRoot->GetAttributeClass(n));
    }
}

void COMPILER::AddPostEvent(S_EVENTMSG *pEM)
{
    EventMsg.Add(pEM);
}

bool COMPILER::SetSaveData(const char *file_name, void *save_data, long data_size)
{
    EXTDATA_HEADER exdh;

    auto fileS = fio->_CreateFile(file_name, std::ios::binary | std::ios::in | std::ios::out);
    if (!fileS.is_open())
    {
        return false;
    }

    const uint32_t dwFileSize = fio->_GetFileSize(file_name);
    auto *pVDat = static_cast<VDATA *>(core.GetScriptVariable("savefile_info"));
    if (pVDat && pVDat->GetString())
        sprintf_s(exdh.sFileInfo, sizeof(exdh.sFileInfo), "%s", pVDat->GetString());
    else
        sprintf_s(exdh.sFileInfo, sizeof(exdh.sFileInfo), "save");
    exdh.dwExtDataOffset = dwFileSize;
    exdh.dwExtDataSize = data_size;

    fio->_WriteFile(fileS, &exdh, sizeof(exdh));
    fio->_SetFilePointer(fileS, dwFileSize, std::ios::beg);

    char *pDst = new char[data_size * 2];
    uint32_t dwPackLen = data_size * 2;
    compress2((Bytef *)pDst, (uLongf *)&dwPackLen, static_cast<Bytef *>(save_data), data_size, Z_BEST_COMPRESSION);

    fio->_WriteFile(fileS, &dwPackLen, sizeof(dwPackLen));
    fio->_WriteFile(fileS, pDst, dwPackLen);
    fio->_CloseFile(fileS);

    delete[] pDst;

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

    strcpy_s(sFileName,file_name);

    // open save file
    fio->SetDrive(XBOXDRIVE_NONE);
    HANDLE fh = fio->_CreateFile(sFileName,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
    fio->SetDrive();
    if(fh == INVALID_HANDLE_VALUE) return false;

    // get file size
    dwFileSize = fio->_GetFileSize(fh,0);

    // set global handle
//    hSaveFileFileHandle = fh;

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
    pOrgData = new char[dwOrgDataSize];
    if(pOrgData == 0)
    {
        SetError("no memory");
        fio->_CloseHandle(fh);
        return false;
    }

    // buffering org data
    ReadData(pOrgData,dwOrgDataSize);

    fio->_CloseHandle(fh);

    // start flushing data to file
    fio->SetDrive(XBOXDRIVE_NONE);
    fh = fio->_CreateFile(sFileName,GENERIC_WRITE,FILE_SHARE_READ,OPEN_EXISTING);
    fio->SetDrive();
    if(fh == INVALID_HANDLE_VALUE)
    {
        if(pOrgData) delete pOrgData;
        SetError("cant set save ext data");
        return false;
    }

    exdh.dwExtDataOffset = sizeof(exdh) + dwOrgDataSize;
    exdh.dwExtDataSize = data_size;

    // save header
    SaveData(&exdh,sizeof(exdh));
    // save org data
    SaveData(pOrgData,dwOrgDataSize);
    // save ext data
    SaveData(save_data,data_size);

    // cleanup
    fio->_CloseHandle(fh);
    if(pOrgData) delete pOrgData;
    return true;


}*/

void *COMPILER::GetSaveData(const char *file_name, long &data_size)
{
    auto fileS = fio->_CreateFile(file_name, std::ios::binary | std::ios::in);
    if (!fileS.is_open())
    {
        SetError("cant open save file: %s", file_name);
        return nullptr;
    }

    if (fio->_GetFileSize(file_name) < sizeof(EXTDATA_HEADER) + sizeof(uint32_t))
    {
        data_size = 0;
        fio->_CloseFile(fileS);
        return nullptr;
    }

    uint64_t dw2;
    RDTSC_B(dw2);
    EXTDATA_HEADER exdh;
    fio->_ReadFile(fileS, &exdh, sizeof(exdh));
    if (exdh.dwExtDataSize <= 0)
    {
        data_size = 0;
        fio->_CloseFile(fileS);
        return nullptr;
    }

    uint32_t dwPackLen;
    fio->_SetFilePointer(fileS, exdh.dwExtDataOffset, std::ios::beg);
    fio->_ReadFile(fileS, &dwPackLen, sizeof(dwPackLen));
    if (dwPackLen == 0 || dwPackLen > 0x8000000)
    {
        data_size = 0;
        fio->_CloseFile(fileS);
        return nullptr;
    }
    char *pCBuffer = new char[dwPackLen];
    fio->_ReadFile(fileS, pCBuffer, dwPackLen);
    char *pBuffer = new char[exdh.dwExtDataSize];
    uint32_t dwDestLen = exdh.dwExtDataSize;
    uncompress((Bytef *)pBuffer, (uLongf *)&dwDestLen, (Bytef *)pCBuffer, dwPackLen);
    fio->_CloseFile(fileS);
    delete[] pCBuffer;
    RDTSC_E(dw2);
    // core.Trace("GetSaveData = %d", dw2);

    data_size = dwDestLen;
    return pBuffer;
}

/*void * COMPILER::GetSaveData(char * file_name, long & data_size)
{
    DWORD n;

    EXTDATA_HEADER exdh;
    char * pExtData;
    if(file_name == 0)
    {
        SetError("invalid save file name");
        return 0;
    }

    // open save file
    fio->SetDrive(XBOXDRIVE_NONE);
    HANDLE fh = fio->_CreateFile(file_name,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
    fio->SetDrive();
    if(fh == INVALID_HANDLE_VALUE)
    {
        SetError("cant open save file: %s",file_name);
        return 0;
    }

    // set global handle
//    hSaveFileFileHandle = fh;

    // read save header

    memset(&exdh,0,sizeof(exdh));
    if(!ReadData(&exdh,sizeof(exdh)))
    {
        fio->_CloseHandle(fh);
        return 0;
    }

    int fsize = fio->_GetFileSize(fh,0);
    if( fsize < exdh.dwExtDataOffset+exdh.dwExtDataSize )
    { // extern data header is failed
        fio->_CloseHandle(fh);
        return 0;
    }

    // prepare ext data buffer
    pExtData = new char[exdh.dwExtDataSize];
    if(pExtData == 0)
    {
        SetError("no memory");
        fio->_CloseHandle(fh);
        return 0;
    }


    // move to ext data
    DWORD dwRes = fio->_SetFilePointer(fh,exdh.dwExtDataOffset,0,FILE_BEGIN);
    if(dwRes == 0xffffffff)
    {
        if(pExtData) delete pExtData; pExtData = 0;
        SetError("invalid seek");
        return 0;
    }

    // read ext data
    ReadData(pExtData,exdh.dwExtDataSize);

    // cleanup
    fio->_CloseHandle(fh);

    data_size = exdh.dwExtDataSize;
    return pExtData;
}*/

void COMPILER::AddRuntimeEvent()
{
    if (!bRuntimeLog)
        return;
    nRuntimeLogEventsNum++;
    if (nRuntimeLogEventsNum >= nRuntimeLogEventsBufferSize)
    {
        if (nRuntimeLogEventsBufferSize == 0)
            nRuntimeLogEventsBufferSize = 180;
        else
            nRuntimeLogEventsBufferSize *= 2;
        // pRuntimeLogEvent = (uint32_t *)RESIZE(pRuntimeLogEvent,nRuntimeLogEventsBufferSize*sizeof(uint32_t));
        pRuntimeLogEvent.resize(nRuntimeLogEventsBufferSize);
    }
    pRuntimeLogEvent[nRuntimeLogEventsNum - 1] = nRuntimeTicks;
    nRuntimeTicks = 0;
}

uint32_t COMPILER::SetScriptFunction(IFUNCINFO *pFuncInfo)
{
    FuncInfo fi;

    if (pFuncInfo->pFuncName == nullptr)
    {
        SetError("Invalid function name in SetScriptFunction(...)");
        return INVALID_FUNCHANDLE;
    }

    const uint32_t nFuncHandle = FuncTab.FindFunc(pFuncInfo->pFuncName);
    if (nFuncHandle != INVALID_FUNC_CODE)
    {
        FuncTab.GetFuncX(fi, nFuncHandle);
        if (fi.arguments != pFuncInfo->nArguments)
        {
            SetError("arguments num mismatch in function '%s'", pFuncInfo->pFuncName);
            return INVALID_FUNCHANDLE;
        }
    }
    else
    {
        SetError("native function '%s' not declared", pFuncInfo->pFuncName);
        return INVALID_FUNCHANDLE;
    }

    fi.name = pFuncInfo->pFuncName;
    fi.imported_func = pFuncInfo->pFuncAddress;
    fi.arguments = pFuncInfo->nArguments;
    fi.segment_id = IMPORTED_SEGMENT_ID;
    fi.offset = INVALID_FUNC_OFFSET;
    fi.stack_offset = 0xffffffff;

    if (pFuncInfo->pReturnValueName == nullptr)
        fi.return_type = TVOID;
    else
    {
        const S_TOKEN_TYPE TokenType = Token.Keyword2TokenType(pFuncInfo->pReturnValueName);
        switch (TokenType)
        {
        case TVOID:
        case VAR_INTEGER:
        case VAR_PTR:
        case VAR_FLOAT:
        case VAR_STRING:
        case VAR_OBJECT:
        case VAR_REFERENCE:
        case VAR_AREFERENCE:
            fi.return_type = TokenType;
            break;
        default:
            SetError("Invalid import function '%s' return type '%s'", pFuncInfo->pFuncName,
                     pFuncInfo->pReturnValueName);
            return INVALID_FUNCHANDLE;
        }
    }

    const uint32_t funch = FuncTab.AddFunc(fi);
    return funch;
}

DATA *COMPILER::GetOperand(const char *pCodeBase, uint32_t &ip, S_TOKEN_TYPE *pTokenType)
{
    uint32_t token_data_size;
    const VarInfo *real_var;
    DATA *pVar;

    const S_TOKEN_TYPE sttResult = BC_TokenGet(ip, token_data_size);
    if (pTokenType)
        *pTokenType = sttResult;
    switch (sttResult)
    {
    case STACK_TOP:
        pVar = SStack.Read();
        return pVar;
    case VARIABLE:
        real_var = VarTab.GetVar(*((uint32_t *)&pCodeBase[ip]));
        if (real_var == nullptr)
        {
            SetError("Global variable not found");
            break;
        }
        if (!real_var->value.get())
        {
            SetError("invalid global variable");
            break;
        }
        return real_var->value.get();
    case LOCAL_VARIABLE:
        pVar = SStack.Read(pRun_fi->stack_offset, *((uint32_t *)&pCodeBase[ip]));
        if (pVar == nullptr)
        {
            SetError("Local variable not found");
            return nullptr;
        }
        return pVar;
    case AX:
        return &rAX;
    case BX:
        return &rBX;
    case EX:
        return &ExpressionResult;
    case AP:
        return nullptr;
    default:
        SetError("invalid operand");
        return nullptr;
    }
    return nullptr;
}

void COMPILER::collectCallStack()
{
    logStack_->trace("Call stack:");
    auto callStackCopy = callStack_;
    while (!callStackCopy.empty())
    {
        const auto &record = callStackCopy.back();

        const auto &filename = std::get<0>(record);
        const auto &line = std::get<1>(record);
        const auto &name = std::get<2>(record);

        if (filename[0] == '\0')
        {
            logStack_->trace("{} (EVENT)", name);
        }
        else if (strcmp(filename, "engine") == 0)
        {
            logStack_->trace("{} (INTERNAL FUNCTION)", name);
        }
        else
        {
            logStack_->trace("{} (FUNCTION) at {}:{}", name, filename, line);
        }

        callStackCopy.pop();
    }
}

void COMPILER::FormatAllDialog(const char *directory_name)
{
    const auto vPaths = fio->_GetPathsOrFilenamesByMask(directory_name, "*.c", true);
    char sFileName[MAX_PATH];
    for (std::string curPath : vPaths)
    {
        sprintf(sFileName, curPath.c_str());
        FormatDialog(sFileName);
    }
}

void COMPILER::FormatDialog(char *file_name)
{
    uint32_t FileSize;
    TOKEN Token;
    S_TOKEN_TYPE Token_type;
    char sFileName[MAX_PATH];
    char buffer[MAX_PATH]{};
    char sNewLine[] = {0xd, 0xa, 0};
    bool bExportString;

    if (file_name == nullptr)
    {
        return;
    }

    uint32_t nTxt = 0;
    uint32_t nLnk = 0;

    // sprintf_s(sFileName,"PROGRAM\\%sc",file_name);
    strcpy_s(sFileName, file_name);

    char *pFileData = LoadFile(file_name, FileSize, true);
    if (pFileData == nullptr)
    {
        return;
    }

    auto fileS = fio->_CreateFile(sFileName, std::ios::binary | std::ios_base::out);
    if (!fileS.is_open())
    {
        return;
    }

    // sprintf_s(sFileName,"PROGRAM\\%s",file_name);
    strcpy_s(sFileName, file_name);
    sFileName[strlen(sFileName) - 1] = 0;
    strcat_s(sFileName, "h");
    auto fileS2 = fio->_CreateFile(sFileName, std::ios::binary | std::ios_base::out);
    if (!fileS2.is_open())
    {
        fio->_CloseFile(fileS);
        delete[] pFileData;
        return;
    }

    // pFileData = LoadFile(file_name,FileSize,true);
    // if(pFileData == 0) {fio->_CloseHandle(fh); fio->_CloseHandle(fhH); return;}

    uint32_t n;
    const uint32_t nFullNameLen = strlen(file_name);
    for (n = nFullNameLen; n > 0; n--)
    {
        if (file_name[n] == '\\')
            break;
    }
    sprintf_s(sFileName, "DIALOGS%s", static_cast<char *>(file_name + n));
    sFileName[strlen(sFileName) - 1] = 0;
    strcat_s(sFileName, "h");

    fio->_WriteFile(fileS, buffer, strlen(buffer));
    fio->_WriteFile(fileS, sNewLine, strlen(sNewLine));

    sprintf_s(buffer, "string DLG_TEXT[0] = {        ");
    fio->_WriteFile(fileS2, buffer, strlen(buffer));
    fio->_WriteFile(fileS2, sNewLine, strlen(sNewLine));

    Token.SetProgram(pFileData, pFileData);

    do
    {
        Token_type = Token.FormatGet();
        switch (Token_type)
        {
        case DOT:
            fio->_WriteFile(fileS, Token.GetData(), strlen(Token.GetData()));
            Token_type = Token.FormatGet();
            if (Token_type != OPEN_BRACKET)
            {
                if (Token.GetData())
                {
                    // node text --------------------------------------------
                    if (_stricmp(Token.GetData(), "text") == 0)
                    {
                        fio->_WriteFile(fileS, Token.GetData(), strlen(Token.GetData()));

                        // fio->_WriteFile(fhH,sNewLine,strlen(sNewLine),&dwR);
                        // sprintf_s(sFileName,"// [NODE START] ",nTxt);
                        // fio->_WriteFile(fhH,sFileName,strlen(sFileName),&dwR);
                        // fio->_WriteFile(fhH,sNewLine,strlen(sNewLine),&dwR);

                        constexpr size_t newline_len = _countof(sNewLine) - 1;
                        do
                        {
                            Token_type = Token.FormatGet();
                            if (Token_type == STRING)
                            {
                                bExportString = true;
                                if (strlen(Token.GetData()) == 3 && Token.GetData()[1] <= 0x39)
                                {
                                    switch (Token.GetData()[1])
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

                                // if(strlen(Token.GetData()) == 3 && Token.GetData()[1] <= 0x39)
                                if (!bExportString)
                                {
                                    fio->_WriteFile(fileS, Token.GetData(), strlen(Token.GetData()));
                                }
                                else
                                {
                                    fio->_WriteFile(fileS2, Token.GetData(), strlen(Token.GetData()));
                                    fio->_WriteFile(fileS2, ",", _countof(",") - 1);
                                    fio->_WriteFile(fileS2, sNewLine, newline_len);
                                    sprintf_s(sFileName, "DLG_TEXT[%d]", nTxt);
                                    fio->_WriteFile(fileS, sFileName, strlen(sFileName));
                                    nTxt++;
                                }
                            }
                            else
                            {
                                fio->_WriteFile(fileS, Token.GetData(), strlen(Token.GetData()));
                            }
                            if (Token_type == END_OF_PROGRAMM)
                            {
                                break;
                            }
                        } while (Token_type != SEPARATOR);
                        break;
                    }
                }
            }
            if (Token.GetData())
            {
                fio->_WriteFile(fileS, Token.GetData(), strlen(Token.GetData()));
            }
            break;
        case UNKNOWN:
            if (Token.GetData())
            {
                fio->_WriteFile(fileS, Token.GetData(), strlen(Token.GetData()));
                if (_stricmp(Token.GetData(), "link") == 0)
                {
                    Token_type = Token.FormatGet();
                    fio->_WriteFile(fileS, Token.GetData(), strlen(Token.GetData()));
                    if (Token_type == DOT)
                    {
                        Token_type = Token.FormatGet();
                        if (Token.GetData())
                        {
                            fio->_WriteFile(fileS, Token.GetData(), strlen(Token.GetData()));
                        }
                        if (Token_type == UNKNOWN)
                        {
                            Token_type = Token.FormatGet();
                            if (Token.GetData())
                            {
                                fio->_WriteFile(fileS, Token.GetData(), strlen(Token.GetData()));
                            }
                            if (Token_type != DOT)
                            {
                                constexpr size_t newline_len = _countof(sNewLine) - 1;
                                do
                                {
                                    Token_type = Token.FormatGet();
                                    if (Token_type == STRING)
                                    {
                                        bExportString = true;
                                        if (strlen(Token.GetData()) == 3 && Token.GetData()[1] <= 0x39)
                                        {
                                            switch (Token.GetData()[1])
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

                                        // if(strlen(Token.GetData()) == 3 && Token.GetData()[1] <= 0x39)
                                        if (!bExportString)
                                        {
                                            fio->_WriteFile(fileS, Token.GetData(), strlen(Token.GetData()));
                                        }
                                        else
                                        {
                                            fio->_WriteFile(fileS2, Token.GetData(), strlen(Token.GetData()));
                                            fio->_WriteFile(fileS2, ",", _countof(",") - 1);
                                            fio->_WriteFile(fileS2, sNewLine, newline_len);
                                            sprintf_s(sFileName, "DLG_TEXT[%d]", nTxt);
                                            fio->_WriteFile(fileS, sFileName, strlen(sFileName));
                                            nTxt++;
                                        }
                                    }
                                    else
                                    {
                                        fio->_WriteFile(fileS, Token.GetData(), strlen(Token.GetData()));
                                    }
                                    if (Token_type == END_OF_PROGRAMM)
                                    {
                                        break;
                                    }
                                } while (Token_type != SEPARATOR);
                                break;
                            }
                        }
                    }
                }
            }
            break;
        default:
            if (Token.GetData())
            {
                fio->_WriteFile(fileS, Token.GetData(), strlen(Token.GetData()));
            }
            break;
        }
    } while (Token_type != END_OF_PROGRAMM);

    delete[] pFileData;

    sprintf_s(buffer, "};");
    fio->_WriteFile(fileS2, sNewLine, strlen(sNewLine));
    fio->_WriteFile(fileS2, buffer, strlen(buffer));
    fio->_SetFilePointer(fileS2, 0, std::ios::beg);
    sprintf_s(buffer, "string DLG_TEXT[%d] = {", nTxt);
    fio->_WriteFile(fileS2, buffer, strlen(buffer));

    fio->_CloseFile(fileS2);
    fio->_CloseFile(fileS);
}

void STRING_CODEC::VariableChanged()
{
    CDebug->SetTraceMode(TMODE_MAKESTEP);
}
