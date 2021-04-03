#pragma once

#include "compiler.h"

class PROGRAM
{
    uint32_t ProgramNum;
    COMPILER **ProgramBlock;
    char *ProgramDirectory;

  public:
    PROGRAM();
    ~PROGRAM();
    void Release();
    bool RunProgram(char *program_name);
    void StopProgram(char *program_name);
    void SetProgramDirectory(char *dir_name);
    VDATA *ProcessEvent(char *event_name, MESSAGE message);
    VDATA *ProcessEvent(char *event_name);
    void ProcessFrame(uint32_t DeltaTime);
    bool Running(char *program_name);
    void ClearEvents();
    bool SaveState(std::fstream &fileS);
    bool LoadState(std::fstream &fileS);
    bool OnLoad();
    void AddPostEvent(S_EVENTMSG *pEM);
};
