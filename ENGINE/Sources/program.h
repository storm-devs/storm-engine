#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include "compiler.h"

class PROGRAM
{
	DWORD ProgramNum;
	COMPILER * * ProgramBlock;
	char * ProgramDirectory;
public:
	 PROGRAM();
	~PROGRAM();
	void Release();
	bool RunProgram(char * program_name);
	void StopProgram(char * program_name);
	void SetProgramDirectory(char * dir_name);
	VDATA * ProcessEvent(char * event_name, MESSAGE message);
	VDATA * ProcessEvent(char * event_name);
	void ProcessFrame(DWORD DeltaTime);
	bool Running(char * program_name);
	void ClearEvents();
	bool SaveState(HANDLE fh);
	bool LoadState(HANDLE fh);
	bool OnLoad();
	void AddPostEvent(S_EVENTMSG * pEM);
};
#endif