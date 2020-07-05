#include "program.h"
#include "core.h"

extern CORE Core;

PROGRAM::PROGRAM()
{
	ProgramDirectory = 0;
	ProgramNum = 0;
	DeleteFile(COMPILER_LOG_FILENAME);
	DeleteFile(COMPILER_ERRORLOG_FILENAME);

}

PROGRAM::~PROGRAM()
{
	Release();
}

void PROGRAM::Release()
{
	if(ProgramDirectory) delete ProgramDirectory; ProgramDirectory = 0;
	DWORD n;
	if(ProgramBlock)
	{
		for(n=0;n<ProgramNum;n++)
		{
			ProgramBlock[n]->Release();
			delete ProgramBlock[n];
		}
		delete ProgramBlock;
		ProgramBlock = 0;
	}
}

bool PROGRAM::RunProgram(char * program_name)
{
	DWORD code;

	if(Running(program_name)) return true;
	code = ProgramNum;
	ProgramNum++;
	ProgramBlock = (COMPILER **)RESIZE(ProgramBlock,ProgramNum*sizeof(COMPILER *));

	ProgramBlock[code] = NEW COMPILER;
	ProgramBlock[code]->SetProgramDirectory(ProgramDirectory);
	if(!ProgramBlock[code]->CreateProgram(program_name))
	{
		delete ProgramBlock[code];
		ProgramNum--;
		ProgramBlock = (COMPILER **)RESIZE(ProgramBlock,ProgramNum*sizeof(COMPILER *));
		return false;
	}
	return ProgramBlock[code]->Run();
}

bool PROGRAM::Running(char * program_name)
{
	DWORD n;
	for(n=0;n<ProgramNum;n++)
	{
		if(strcmp(ProgramBlock[n]->GetName(),program_name)==0) return true;
	}
	return false;
}

void PROGRAM::StopProgram(char * program_name)
{

}

void PROGRAM::SetProgramDirectory(char * dir_name)
{
	if(ProgramDirectory) delete ProgramDirectory; ProgramDirectory = 0;
	if(dir_name)
	{

		ProgramDirectory = NEW char[strlen(dir_name) + 1];
		strcpy(ProgramDirectory,dir_name);
	}
}
VDATA * PROGRAM::ProcessEvent(char * event_name, MESSAGE message)
{
	DWORD n;
	VDATA * pVD;
	for(n=0;n<ProgramNum;n++)
	{
		message.Move2Start();
		pVD = ProgramBlock[n]->ProcessEvent(event_name,message);
	}
	return pVD;// ??? maybe a should take off programs mechanizm
}

VDATA *  PROGRAM::ProcessEvent(char * event_name)
{
	VDATA * pVD;
	DWORD n;
	for(n=0;n<ProgramNum;n++)
	{
		pVD = ProgramBlock[n]->ProcessEvent(event_name);
	}
	return pVD;
}

void PROGRAM::ProcessFrame(DWORD DeltaTime)
{
	DWORD n,i,old_size;
	
	old_size = ProgramNum; 
	for(n=0;n<ProgramNum;n++)
	{
		ProgramBlock[n]->ProcessFrame(DeltaTime);
		if(!ProgramBlock[n]->Completed()) continue;
		ProgramBlock[n]->Release();
		delete ProgramBlock[n];
		for(i=n;i<(ProgramNum - 1);i++)
		{
			ProgramBlock[i] = ProgramBlock[i + 1];
		}
		ProgramNum--;
	}
	if(old_size != ProgramNum)
		ProgramBlock = (COMPILER **)RESIZE(ProgramBlock,ProgramNum*sizeof(COMPILER *));
}

void PROGRAM::ClearEvents()
{
	DWORD n;
	for(n=0;n<ProgramNum;n++)
	{
		ProgramBlock[n]->ClearEvents();
	}
}

bool PROGRAM::SaveState(HANDLE fh)
{
	DWORD n;
	Core.fio->_WriteFile(fh,&ProgramNum,sizeof(ProgramNum),0);
	for(n=0;n<ProgramNum;n++)
	{
		ProgramBlock[n]->SaveState(fh);
	}
	return true;
}

bool PROGRAM::LoadState(HANDLE fh)
{
	DWORD n;
	Release();
	Core.fio->_ReadFile(fh,&ProgramNum,sizeof(ProgramNum),0);
	ProgramBlock = (COMPILER **)RESIZE(ProgramBlock,ProgramNum*sizeof(COMPILER *));
	for(n=0;n<ProgramNum;n++)
	{

		ProgramBlock[n] = NEW COMPILER;
		if(!ProgramBlock[n]->LoadState(fh)) return false;
	}
	return true;
}

bool PROGRAM::OnLoad()
{
	DWORD n;
	for(n=0;n<ProgramNum;n++)
	{
		ProgramBlock[n]->OnLoad();
	}
	return true;
}

void PROGRAM::AddPostEvent(S_EVENTMSG * pEM)
{
	DWORD n;
	for(n=0;n<ProgramNum;n++)
	{
		ProgramBlock[n]->AddPostEvent(pEM);
	}
}