#include "modules_table.h"

#include "common_h.h"
#include "exs.h"
#include "externs.h"
#include "internal_modules.h"
#include "memop.h"
#include "messages.h"

#define MROOT_DIRECTORY 0xffffffff
#define MODULE_WILD_MASK "*.dll"

MODULES_TABLE::MODULES_TABLE()
{
    Paths_Count = 0;
    Paths_Table = 0;
    pTable = 0;
    nModulesNum = 0;
}

MODULES_TABLE::~MODULES_TABLE()
{
    Release();
}

void MODULES_TABLE::Release()
{
    dword n;

    if (pTable)
    {
        for (n = 0; n < nModulesNum; n++)
            FreeLibrary(pTable[n].hInst);
        delete pTable;
        pTable = 0;
        nModulesNum = 0;
    }

    if (Paths_Table)
    {
        for (n = 0; n < Paths_Count; n++)
            delete Paths_Table[n];
        delete Paths_Table;
    }
    Paths_Count = 0;
    Paths_Table = null;
}

bool MODULES_TABLE::SetModulesPath(char *_name)
{
    GUARD(MODULES_TABLE::SetModulesPath)
    dword n;
    if (Paths_Table)
    {
        for (n = 0; n < Paths_Count; n++)
            delete Paths_Table[n];
        delete Paths_Table;
    }
    Paths_Count = 0;
    Paths_Table = null;
    if (_name == null)
        return true;

    Paths_Table = (char **)NEW char[sizeof(char *)];
    if (Paths_Table == null)
        SE_THROW;

    Paths_Table[0] = (char *)NEW char[strlen(_name) + 1];
    if (Paths_Table[0] == null)
        SE_THROW;
    strcpy(Paths_Table[0], _name);
    Paths_Count = 1;
    UNGUARD
    return true;
}

bool MODULES_TABLE::AddModulesPath(char *_name)
{
    GUARD(MODULES_TABLE::AddModulesPath)
    if (Paths_Table == null)
        return SetModulesPath(_name);
    if (_name == null)
        return false;
    Paths_Table = (char **)RESIZE(Paths_Table, (Paths_Count + 1) * sizeof(char *));
    if (Paths_Table == null)
        SE_THROW;

    Paths_Table[Paths_Count] = (char *)NEW char[strlen(_name) + 1];
    if (Paths_Table[Paths_Count] == null)
        SE_THROW;
    strcpy(Paths_Table[Paths_Count], _name);
    Paths_Count++;
    UNGUARD
    return true;
}

bool MODULES_TABLE::LoadModule(char *_mname, long path_code, MODULE_STATE &ms)
{
    GUARD(MODULES_TABLE::LoadModule)
    HINSTANCE hInst;
    DLLAPIFUNC api_func_PTR;

    char full_name[_MAX_PATH];

    if (_mname == null)
        return false;

    // invalid path code
    if ((dword)path_code >= Paths_Count)
    {
        SET_ERROR("invalid path");
        return false;
    }

    if (strlen(GetPath(path_code)) > 0)
        wsprintf(full_name, "%s\\%s", GetPath(path_code), _mname);
    else
        strcpy(full_name, _mname);

    // gdi_display.Print("Load libriary: %s",full_name);
    // load library
    hInst = LoadLibrary(full_name);
    if (hInst == null)
    {
        SET_ERROR("cant load libriary");
        return false;
    }
    ms.hInst = hInst;
    // get procedure address
    api_func_PTR = (DLLAPIFUNC)GetProcAddress(hInst, "DMAInterface");
    if (api_func_PTR == null)
    {
        SET_ERROR("interface function not found");
        FreeLibrary(hInst);
        return false;
    }
    ms.api_func_PTR = api_func_PTR;
    UNGUARD
    return true;
}

VMA *MODULES_TABLE::GetClassesRoot(dword _n)
{
    if (_n >= nModulesNum)
        return 0;
    return pTable[_n].api_func_PTR(_CORE_API, _VSYSTEM_API);
}

dword MODULES_TABLE::GetModulesCount()
{
    return nModulesNum;
}

void __declspec(noinline) __declspec(dllexport) __cdecl MODULES_TABLE::Load_ModulesTable()
{

    WIN32_FIND_DATA find_data;
    HANDLE hFile;
    long Path_scan;
    long counter;
    long n, i;
    char full_name[_MAX_PATH];
    dword path_code;
    MODULE_STATE ms;

    Path_scan = Paths_Count;
    if (Path_scan == 0)
        Path_scan = 1;
    for (n = 0; n < Path_scan; n++)
    {
        // searching modules directory for modules
        if (Paths_Table == null)
        {
            full_name[0] = 0;
            path_code = n;
            gdi_display.Print("Scanning current directory");
        }
        else
        {
            strcpy(full_name, Paths_Table[n]);
            path_code = n;
            gdi_display.Print("Scanning %s", Paths_Table[n]);
        }

        strcat(full_name, MODULE_WILD_MASK);
        gdi_display.Print_Add("Loading modules...");

        // load modules names into modules table (fixing codes)
        hFile = File_Service._FindFirstFile(full_name, &find_data);

        counter = 0;
        if (hFile != INVALID_HANDLE_VALUE)
        {
            do
            {
                gdi_display.Set_Text(find_data.cFileName);

                ZeroMemory(&ms, sizeof(ms));
                if (LoadModule(find_data.cFileName, path_code, ms))
                {
                    i = nModulesNum;
                    nModulesNum++;
                    pTable = (MODULE_STATE *)RESIZE(pTable, sizeof(MODULE_STATE) * nModulesNum);
                    pTable[i] = ms;
                    pTable[i].api_func_PTR = ms.api_func_PTR;
                    pTable[i].hInst = ms.hInst;
                    counter++;
                }
                else
                {
                    gdi_display.Print(CMS_MODULE_SKIPPED, find_data.cFileName);
                    gdi_display.Print(GET_ERROR);
                    Sleep(ERROR_MESSAGE_DELAY);
                }

            } while (File_Service._FindNextFile(hFile, &find_data));
            File_Service._FindClose(hFile);
        }

        gdi_display.Print("Loaded %d", counter);
    }
}

char *MODULES_TABLE::GetPath(dword path_code)
{
    if (path_code >= Paths_Count)
        return null;
    return Paths_Table[path_code];
}
