#ifndef _BREAKPOINTS_TABLE_H_
#define _BREAKPOINTS_TABLE_H_

#include "core.h"

struct BREAKPOINT_DESC
{
	DWORD nLineNumber;
	char * pFileName;
};

class BREAKPOINTS_TABLE
{
	DWORD nPoints;
	BREAKPOINT_DESC * pTable;
	char ProjectName[MAX_PATH];
	VFILE_SERVICE * fio;
	bool bReleased;
public:
	 BREAKPOINTS_TABLE();
	~BREAKPOINTS_TABLE();
	bool ReadProject(char * filename);
	void AddBreakPoint(const char * filename, DWORD line);
	void DelBreakPoint(const char * filename, DWORD line);
	bool Find(const char * filename, DWORD line);
	void FlipBreakPoint(const char * filename, DWORD line);
	void Release();
	bool CanBreak(){if(nPoints > 0) return true; return false;};
	void UpdateProjectFile();

};

#endif