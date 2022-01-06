#pragma once

#include "core_impl.h"
#include <vector>

struct BREAKPOINT_DESC
{
    uint32_t nLineNumber;
    char *pFileName;
};

class BREAKPOINTS_TABLE
{
    uint32_t nPoints;
    std::vector<BREAKPOINT_DESC> pTable;
    char ProjectName[MAX_PATH];
    bool bReleased;

  public:
    BREAKPOINTS_TABLE();
    ~BREAKPOINTS_TABLE();
    bool ReadProject(const char *filename);
    void AddBreakPoint(const char *filename, uint32_t line);
    void DelBreakPoint(const char *filename, uint32_t line);
    bool Find(const char *filename, uint32_t line);
    void FlipBreakPoint(const char *filename, uint32_t line);
    void Release();

    bool CanBreak()
    {
        if (nPoints > 0)
            return true;
        return false;
    };
    void UpdateProjectFile();
};
