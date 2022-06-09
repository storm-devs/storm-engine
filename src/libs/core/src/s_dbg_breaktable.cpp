#ifdef _WIN32 // S_DEBUG
#include "s_dbg_breaktable.h"
#include "file_service.h"

#define SECTION_NAME "breakpoints"

BREAKPOINTS_TABLE::BREAKPOINTS_TABLE()
    : bReleased(false)
{
    nPoints = 0;
    ProjectName[0] = 0;
    // bReleased = false;
}

BREAKPOINTS_TABLE::~BREAKPOINTS_TABLE()
{
    Release();
}

void BREAKPOINTS_TABLE::Release()
{
    uint32_t n;
    char buffer[MAX_PATH];

    // if(nPoints)
    if (ProjectName[0] != 0)
    {
        auto ini = fio->OpenIniFile(ProjectName);
        if (!ini)
            ini = fio->CreateIniFile(ProjectName, false);
        if (ini)
        {
            ini->DeleteSection(SECTION_NAME);
            for (n = 0; n < nPoints; n++)
            {
                if (!pTable[n].pFileName)
                    continue;
                sprintf_s(buffer, "%s,%d", pTable[n].pFileName, pTable[n].nLineNumber);
                ini->AddString(SECTION_NAME, "B", buffer);
            }
        }
        ProjectName[0] = 0;
    }

    for (n = 0; n < nPoints; n++)
    {
        delete[] pTable[n].pFileName;
    }

    pTable.clear();
    nPoints = 0;
    // bReleased = true;
}

void BREAKPOINTS_TABLE::UpdateProjectFile()
{
    char buffer[MAX_PATH];

    if (ProjectName[0] != 0)
    {
        auto ini = fio->OpenIniFile(ProjectName);
        if (!ini)
            ini = fio->CreateIniFile(ProjectName, false);
        if (ini)
        {
            ini->DeleteSection(SECTION_NAME);
            for (uint32_t n = 0; n < nPoints; n++)
            {
                if (!pTable[n].pFileName)
                    continue;
                sprintf_s(buffer, "%s,%d", pTable[n].pFileName, pTable[n].nLineNumber);
                ini->AddString(SECTION_NAME, "B", buffer);
            }
        }
    }
}

bool MakeLineCode(char *buffer, uint32_t &nLineCode)
{
    if (buffer == nullptr)
        return false;
    uint32_t n = 0;
    while (buffer[n])
    {
        if (buffer[n] == ',')
        {
            if (buffer[n + 1] == 0)
                return false;
            nLineCode = atol(&buffer[n] + 1);
            buffer[n] = 0;
            return true;
        }
        n++;
    }
    return false;
}

bool BREAKPOINTS_TABLE::ReadProject(const char *filename)
{
    char buffer[MAX_PATH];

    uint32_t nLineNumber;

    Release();

    auto ini = fio->OpenIniFile(filename);
    if (ini)
    {
        strcpy_s(ProjectName, filename);
        if (ini->ReadString(SECTION_NAME, "B", buffer, sizeof(buffer), ""))
        {
            if (MakeLineCode(buffer, nLineNumber))
                AddBreakPoint(buffer, nLineNumber);
            while (ini->ReadStringNext(SECTION_NAME, "B", buffer, sizeof(buffer)))
            {
                if (MakeLineCode(buffer, nLineNumber))
                    AddBreakPoint(buffer, nLineNumber);
            }
        }
        // bReleased = false;
        return true;
    }
    return false;
}

void BREAKPOINTS_TABLE::AddBreakPoint(const char *filename, uint32_t line)
{
    if (filename == nullptr)
        return;

    for (uint32_t n = 0; n < nPoints; n++)
    {
        if (pTable[n].nLineNumber != line)
            continue;
        if (!storm::iEquals(pTable[n].pFileName, filename))
            continue;
        return; // already in list
    }

    pTable.resize(nPoints + 1);
    pTable[nPoints].nLineNumber = line;

    const auto len = strlen(filename) + 1;
    pTable[nPoints].pFileName = new char[len];
    memcpy(pTable[nPoints].pFileName, filename, len);
    nPoints++;

    // UpdateProjectFile();
}

void BREAKPOINTS_TABLE::FlipBreakPoint(const char *filename, uint32_t line)
{
    if (Find(filename, line))
        DelBreakPoint(filename, line);
    else
        AddBreakPoint(filename, line);
}

void BREAKPOINTS_TABLE::DelBreakPoint(const char *filename, uint32_t line)
{
    if (filename == nullptr)
        return;
    for (uint32_t n = 0; n < nPoints; n++)
    {
        if (pTable[n].nLineNumber != line)
            continue;
        if (!storm::iEquals(pTable[n].pFileName, filename))
            continue;

        delete[] pTable[n].pFileName;

        for (; n < (nPoints - 1); n++)
        {
            pTable[n] = pTable[n + 1];
        }
        nPoints--;

        // UpdateProjectFile();

        return;
    }
}

bool BREAKPOINTS_TABLE::Find(const char *filename, uint32_t line)
{
    if (filename == nullptr)
        return false;
    for (uint32_t n = 0; n < nPoints; n++)
    {
        if (pTable[n].nLineNumber != line)
            continue;
        if (!storm::iEquals(pTable[n].pFileName, filename))
            continue;

        return true;
    }
    return false;
}
#endif // S_DEBUG
