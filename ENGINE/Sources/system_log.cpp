#include "system_log.h"
#ifndef _XBOX
#define LOG_FILENAME	"system.log"
#else
#define LOG_FILENAME	"d:\\system.log"
#endif

char Buffer_4k[4096];
bool bTraceFilesOff = false;
void  trace(char * data_PTR,...)
{
	if(bTraceFilesOff) return;
	if(data_PTR == 0) return;
	static BOOL First = true;
	if(First)
	{
		DeleteFile(LOG_FILENAME);
		First = false;
	}

	HANDLE file_h = CreateFile(LOG_FILENAME,GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	SetFilePointer(file_h,0,0,FILE_END);
	va_list args;
	
	va_start(args,data_PTR);
	_vsnprintf(Buffer_4k,sizeof(Buffer_4k) - 4,data_PTR,args);
	strcat(Buffer_4k,"\x0d\x0a");
	DWORD bytes;
	WriteFile(file_h,Buffer_4k,strlen(Buffer_4k),&bytes,0);
	va_end(args);
	CloseHandle(file_h);
	_flushall();
	
}
