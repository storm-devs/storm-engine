#include "file_service.h"
#include <exception>

#ifdef _XBOX
bool XProcessFile(const char *_srcDir, const char *_destDir, const char *_mask, const WIN32_FIND_DATA &_findData);
bool XDirCopy(const char *_srcDir, const char *_destDir, const char *_mask);
void CacheData();
bool XFileSTORM_DELETE(const char *_fileName);
bool XProcessFileSTORM_DELETE(const char *_srcDir, const char *_mask, const WIN32_FIND_DATA &_findData);

#include "..\dx9render.h"
extern VDX9RENDER *pDevice;
extern uint32_t dwCacheScreenN;
extern RECT CacheScreenRect;
extern uint32_t dwCacheFiles;
extern bool bCacheOverwrite;
extern long nCacheFonTexId;
extern long nCacheProgressCacheId[10];
extern bool bCacheEstDirsStructure;

#endif

#define COMMENT ';'
#define SECTION_A '['
#define SECTION_B ']'
#define INI_EQUAL '='
#define VOIDSYMS_NUM 2
#define INI_SIGNATURE ";[SE2IF]"
const char INI_LINEFEED[3] = {0xd, 0xa, 0};
const char INI_VOIDSYMS[VOIDSYMS_NUM] = {0x20, 0x9};
char sDriveLetter[8] = "d:\\";

//#define sDriveLetter	"d:\\"

extern FILE_SERVICE File_Service;

void FILE_SERVICE::FlushIniFiles()
{
    for (uint32_t n = 0; n <= Max_File_Index; n++)
    {
        if (OpenFiles[n] == nullptr)
            continue;
        OpenFiles[n]->FlushFile();
    }
}

FILE_SERVICE::FILE_SERVICE()
{
    Files_Num = 0;
    Max_File_Index = 0;
    for (uint32_t n = 0; n < _MAX_OPEN_INI_FILES; n++)
        OpenFiles[n] = nullptr;
}

FILE_SERVICE::~FILE_SERVICE()
{
    Close();
}

HANDLE FILE_SERVICE::_CreateFile(const char *lpFileName, uint32_t dwDesiriedAccess, uint32_t dwShareMode,
                                 uint32_t dwCreationDisposition)
{
    HANDLE fh;
#ifndef _XBOX
    fh = CreateFile(lpFileName, dwDesiriedAccess, dwShareMode, nullptr, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL,
                    nullptr);
    // DWORD er = GetLastError();
#else
    bool bCached;
    char xbox_file_name[MAX_PATH];
    char file_name_buffer[MAX_PATH];
    /*if(sDriveLetter[0])
    {
        strcpy_s(xbox_file_name,sDriveLetter);
        strcat_s(xbox_file_name,lpFileName);
    }
    else strcpy_s(xbox_file_name,lpFileName);//*/
    if (_stricmp(sDriveLetter, XBOXDRIVE_DVD) == 0)
    {
        if (IsCached(lpFileName))
        {
            bCached = true;
            strcpy_s(xbox_file_name, XBOXDRIVE_CACHE);
            strcat_s(xbox_file_name, lpFileName);
        }
        else
        {
            bCached = false;
            strcpy_s(xbox_file_name, XBOXDRIVE_DVD);
            strcat_s(xbox_file_name, lpFileName);
        }
    }
    else
    {
        if (sDriveLetter[0])
        {
            strcpy_s(xbox_file_name, sDriveLetter);
            strcat_s(xbox_file_name, lpFileName);
        }
        else
            strcpy_s(xbox_file_name, lpFileName);
    }

    fh = CreateFile(xbox_file_name, dwDesiriedAccess, dwShareMode, 0, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
    if (fh == INVALID_HANDLE_VALUE && bCached)
    {
        // if file isn't in cache - copy to cache (time distributed caching)

        strcpy_s(file_name_buffer, XBOXDRIVE_DVD);
        strcat_s(file_name_buffer, lpFileName);
        bool bRes = CopyFile(file_name_buffer, xbox_file_name, false) != 0;
        if (bRes)
            fh = CreateFile(xbox_file_name, dwDesiriedAccess, dwShareMode, 0, dwCreationDisposition,
                            FILE_ATTRIBUTE_NORMAL, 0);
        else
            fh = CreateFile(file_name_buffer, dwDesiriedAccess, dwShareMode, 0, dwCreationDisposition,
                            FILE_ATTRIBUTE_NORMAL, 0);

        /*//trace("file cache miss");
        strcpy_s(xbox_file_name,XBOXDRIVE_DVD);
        strcat_s(xbox_file_name,lpFileName);
        fh = CreateFile(xbox_file_name,dwDesiriedAccess,dwShareMode,0,dwCreationDisposition,FILE_ATTRIBUTE_NORMAL,0);
        //*/
    }
#endif
    // if(fh == INVALID_HANDLE_VALUE) if(Exceptions_Mask & _X_NO_FILE) throw std::exception(_X_NO_FILE);
    return fh;
}

void FILE_SERVICE::_CloseHandle(HANDLE hFile)
{
    CloseHandle(hFile);
}

uint32_t FILE_SERVICE::_SetFilePointer(HANDLE hFile, long DistanceToMove, long *lpDistanceToMoveHigh,
                                       uint32_t dwMoveMethod)
{
    return SetFilePointer(hFile, DistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);
}

BOOL FILE_SERVICE::_DeleteFile(const char *lpFileName)
{
#ifdef _XBOX
    char xbox_file_name[MAX_PATH];
    if (sDriveLetter[0])
    {
        strcpy_s(xbox_file_name, sDriveLetter);
        strcat_s(xbox_file_name, lpFileName);
    }
    else
        strcpy_s(xbox_file_name, lpFileName);
    return DeleteFile(xbox_file_name);
#else
    return DeleteFile(lpFileName);
#endif
}

BOOL FILE_SERVICE::_WriteFile(HANDLE hFile, const void *lpBuffer, uint32_t nNumberOfBytesToWrite,
                              uint32_t *lpNumberOfBytesWritten)
{
    uint32_t dwR;
    const auto bRes = WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, (LPDWORD)&dwR, nullptr);
    if (lpNumberOfBytesWritten != nullptr)
        *lpNumberOfBytesWritten = dwR;
    //	if(dwR != nNumberOfBytesToWrite) if(Exceptions_Mask & _X_NO_FILE_WRITE) throw std::exception(_X_NO_FILE_WRITE);
    return bRes;
}

BOOL FILE_SERVICE::_ReadFile(HANDLE hFile, void *lpBuffer, uint32_t nNumberOfBytesToRead, uint32_t *lpNumberOfBytesRead)
{
    uint32_t dwR;
    const auto bRes = ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, (LPDWORD)&dwR, nullptr);
    if (lpNumberOfBytesRead != nullptr)
        *lpNumberOfBytesRead = dwR;
    //	if(dwR != nNumberOfBytesToRead) if(Exceptions_Mask & _X_NO_FILE_READ) throw std::exception(_X_NO_FILE_READ);
    return bRes;
}

HANDLE FILE_SERVICE::_FindFirstFile(const char *lpFileName, LPWIN32_FIND_DATA lpFindFileData)
{
    HANDLE hFile;
#ifndef _XBOX
    hFile = FindFirstFile(lpFileName, lpFindFileData);
#else
    char xbox_file_name[MAX_PATH];
    // strcpy_s(xbox_file_name,"d:\\");
    strcpy_s(xbox_file_name, sDriveLetter);
    strcat_s(xbox_file_name, lpFileName);
    hFile = FindFirstFile(xbox_file_name, lpFindFileData);
#endif
    return hFile;
}

BOOL FILE_SERVICE::_FindNextFile(HANDLE hFindFile, LPWIN32_FIND_DATA lpFindFileData)
{
    return FindNextFile(hFindFile, lpFindFileData);
}

BOOL FILE_SERVICE::_FindClose(HANDLE hFindFile)
{
    return FindClose(hFindFile);
}

BOOL FILE_SERVICE::_FlushFileBuffers(HANDLE hFile)
{
    return FlushFileBuffers(hFile);
}

uint32_t FILE_SERVICE::_GetCurrentDirectory(uint32_t nBufferLength, char *lpBuffer)
{
#ifndef _XBOX
    return GetCurrentDirectory(nBufferLength, lpBuffer);
#else
    // if(nBufferLength > strlen("d:\\") && lpBuffer) strcpy_s(lpBuffer,"d:\\");
    // return strlen("d:\\");

    if (nBufferLength > strlen(sDriveLetter) && lpBuffer)
        strcpy_s(lpBuffer, sDriveLetter);
    return strlen(sDriveLetter);

#endif
}

BOOL FILE_SERVICE::_GetDiskFreeSpaceEx(const char *lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailableToCaller,
                                       PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes)
{
    return GetDiskFreeSpaceEx(lpDirectoryName, lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes,
                              lpTotalNumberOfFreeBytes);
}

UINT FILE_SERVICE::_GetDriveType(const char *lpRootPathName)
{
#ifndef _XBOX
    return GetDriveType(lpRootPathName);
#else
    return 0;
#endif
}

uint32_t FILE_SERVICE::_GetFileSize(HANDLE hFile, uint32_t *lpFileSizeHigh)
{
    return GetFileSize(hFile, (LPDWORD)lpFileSizeHigh);
}

uint32_t FILE_SERVICE::_GetLogicalDrives()
{
#ifndef _XBOX
    return GetLogicalDrives();
#else
    return 0;
#endif
}

uint32_t FILE_SERVICE::_GetLogicalDriveStrings(uint32_t nBufferLength, char *lpBuffer)
{
#ifndef _XBOX
    return GetLogicalDriveStrings(nBufferLength, lpBuffer);
#else
    return 0;
#endif
}

BOOL FILE_SERVICE::_SetCurrentDirectory(const char *lpPathName)
{
#ifndef _XBOX
    return SetCurrentDirectory(lpPathName);
#else
    return 0;
#endif
}

BOOL FILE_SERVICE::_CreateDirectory(const char *lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    return CreateDirectory(lpPathName, lpSecurityAttributes);
}

BOOL FILE_SERVICE::_RemoveDirectory(const char *lpPathName)
{
    return RemoveDirectory(lpPathName);
}

BOOL FILE_SERVICE::_CopyFile(const char *lpExistingFileName, const char *lpNewFileName, bool bFailIfExists)
{
    return CopyFile(lpExistingFileName, lpNewFileName, bFailIfExists);
}

BOOL FILE_SERVICE::_SetFileAttributes(const char *lpFileName, uint32_t dwFileAttributes)
{
    return SetFileAttributes(lpFileName, dwFileAttributes);
}

BOOL FILE_SERVICE::FileExist(const char *file_name)
{
    auto *const fh = _CreateFile(file_name);
    if (fh == INVALID_HANDLE_VALUE)
        return false;
    CloseHandle(fh);
    return true;
}

//------------------------------------------------------------------------------------------------
// inifile objects managment
//

INIFILE *FILE_SERVICE::CreateIniFile(const char *file_name, bool fail_if_exist)
{
    auto *fh = _CreateFile(file_name, GENERIC_READ, 0, OPEN_EXISTING);
    if (fh != INVALID_HANDLE_VALUE && fail_if_exist)
    {
        _CloseHandle(fh);
        return nullptr;
    }
    _CloseHandle(fh);
    fh = _CreateFile(file_name, GENERIC_WRITE, 0, CREATE_NEW);
    if (fh == INVALID_HANDLE_VALUE)
        return nullptr;
    _CloseHandle(fh);
    return OpenIniFile(file_name);
}

INIFILE *FILE_SERVICE::OpenIniFile(const char *file_name)
{
    ////GUARD(FILE_SERVICE::OpenIniFile)
    INIFILE_T *inifile_T;
    uint32_t n;
    //	PUSH_CONTROL(0,0,0)	// core control

    for (n = 0; n <= Max_File_Index; n++)
    {
        if (OpenFiles[n] == nullptr || OpenFiles[n]->GetFileName() == nullptr)
            continue;
        if (_stricmp(OpenFiles[n]->GetFileName(), file_name) == 0)
        {
            OpenFiles[n]->IncReference();

            inifile_T = new INIFILE_T(OpenFiles[n]);
            if (inifile_T == nullptr)
                throw std::exception();
            //			POP_CONTROL(0)
            return inifile_T;
        }
    }

    for (n = 0; n < _MAX_OPEN_INI_FILES; n++)
    {
        if (OpenFiles[n] != nullptr)
            continue;

        OpenFiles[n] = new IFS(this);
        if (OpenFiles[n] == nullptr)
            throw std::exception(); //(FILE_SERVICE::OpenIniFile : no mem A);
        if (!OpenFiles[n]->LoadFile(file_name))
        {
            delete OpenFiles[n];
            OpenFiles[n] = nullptr;
            return nullptr;
        }
        if (Max_File_Index < n)
            Max_File_Index = n;
        OpenFiles[n]->IncReference();
        //		POP_CONTROL(0)
        // INIFILE_T object belonged to entity and must be deleted by entity
        // OpenFiles[n]->inifile_T = new INIFILE_T(OpenFiles[n]);
        // if(OpenFiles[n]->inifile_T == null) throw std::exception();
        // return OpenFiles[n]->inifile_T;

        inifile_T = new INIFILE_T(OpenFiles[n]);
        if (inifile_T == nullptr)
            throw std::exception();
        return inifile_T;
    }
    //	POP_CONTROL(0)
    ////UNGUARD
    return nullptr;
}

void FILE_SERVICE::RefDec(INIFILE *ini_obj)
{
    for (uint32_t n = 0; n <= Max_File_Index; n++)
    {
        if (OpenFiles[n] != ini_obj)
            continue;
        // OpenFiles[n]->SearchData = &OpenFiles[n]->Search;
        if (OpenFiles[n]->GetReference() == 0)
            throw std::exception("Reference error");
        OpenFiles[n]->DecReference();
        if (OpenFiles[n]->GetReference() == 0)
        {
            delete OpenFiles[n];
            OpenFiles[n] = nullptr;
        }
        return;
    }
    throw std::exception("bad inifile object");
    // UNGUARD
}

void FILE_SERVICE::Close()
{
    for (uint32_t n = 0; n < _MAX_OPEN_INI_FILES; n++)
    {
        if (OpenFiles[n] == nullptr)
            continue;
        delete OpenFiles[n];
        OpenFiles[n] = nullptr;
    }
}

BOOL FILE_SERVICE::LoadFile(const char *file_name, char **ppBuffer, uint32_t *dwSize)
{
    if (ppBuffer == nullptr)
        return false;

    auto *const hFile = _CreateFile(file_name);
    if (INVALID_HANDLE_VALUE == hFile)
        return false;
    const auto dwLowSize = _GetFileSize(hFile, nullptr);
    if (dwSize)
        *dwSize = dwLowSize;
    if (dwLowSize == 0)
    {
        *ppBuffer = nullptr;
        return false;
    }

    *ppBuffer = new char[dwLowSize];
    _ReadFile(hFile, *ppBuffer, dwLowSize, nullptr);
    _CloseHandle(hFile);
    return true;
}

BOOL FILE_SERVICE::SetDrive(const char *pDriveName)
{
#ifdef _XBOX
    if (pDriveName == 0)
    {
        strcpy_s(sDriveLetter, XBOXDRIVE_DVD);
        return true;
    }
    if (!pDriveName[0])
    {
        sDriveLetter[0] = 0;
        return true;
    }
    if (strlen(pDriveName) > 4)
        return false;
    strcpy_s(sDriveLetter, pDriveName);
    return true;
#else
    return false;
#endif
}

#ifdef _XBOX

bool XDirCopy(const char *_srcDir, const char *_destDir, const char *_mask)
{

    if (bCacheEstDirsStructure)
    {
        /*if(pDevice)
        {
            dwCacheScreenN++;
            if(dwCacheScreenN > 8) dwCacheScreenN = 0;
            pDevice->RunStart();
            pDevice->ImageBlt(nCacheFonTexId);
            pDevice->ImageBlt(nCacheProgressCacheId[dwCacheScreenN],&CacheScreenRect);
            pDevice->RunEnd();
        }*/
    }

    File_Service.MarkDirectoryCached(_srcDir + 3);

    WIN32_FIND_DATA findData;
    char *srcFilename = new char[strlen(_srcDir) + strlen(_mask) + 2];
    if (!srcFilename)
        return false;

    strcpy_s(srcFilename, _srcDir);
    if (_srcDir[strlen(_srcDir) - 1] != '\\')
        strcat_s(srcFilename, "\\");
    strcat_s(srcFilename, _mask);

    bool bRes;
    bRes = CreateDirectory(_destDir, 0) == TRUE;

    HANDLE findHandle = FindFirstFile(srcFilename, &findData);
    if (findHandle == INVALID_HANDLE_VALUE)
    {
        delete[] srcFilename;
        return false;
    }

    XProcessFile(_srcDir, _destDir, _mask, findData);
    while (FindNextFile(findHandle, &findData) == TRUE)
    {
        XProcessFile(_srcDir, _destDir, _mask, findData);
    }

    delete[] srcFilename;
    FindClose(findHandle);
    return true;
}

bool XDirSTORM_DELETE(const char *_srcDir)
{
    WIN32_FIND_DATA findData;
    char _mask[] = "*.*";
    char *srcFilename = new char[strlen(_srcDir) + strlen(_mask) + 2];
    if (!srcFilename)
        return false;

    strcpy_s(srcFilename, _srcDir);
    if (_srcDir[strlen(_srcDir) - 1] != '\\')
        strcat_s(srcFilename, "\\");
    strcat_s(srcFilename, _mask);

    // bool bRes;
    // bRes = CreateDirectory(_destDir, 0)==TRUE;

    HANDLE findHandle = FindFirstFile(srcFilename, &findData);
    if (findHandle == INVALID_HANDLE_VALUE)
    {
        delete[] srcFilename;
        RemoveDirectory(_srcDir);
        return false;
    }

    XProcessFileSTORM_DELETE(_srcDir, _mask, findData);
    while (FindNextFile(findHandle, &findData) == TRUE)
    {
        XProcessFileSTORM_DELETE(_srcDir, _mask, findData);
    }

    delete[] srcFilename;
    FindClose(findHandle);
    RemoveDirectory(_srcDir);
    return true;
}

bool XFileCopy(const char *_srcName, const char *_destName)
{
    bool bRes;

    if (bCacheEstDirsStructure)
        return true; // only structure of directories is created in this case

    if (dwCacheFiles >= 100)
    {
        dwCacheFiles = 0;
        if (pDevice)
        {
            dwCacheScreenN++;
            if (dwCacheScreenN > 8)
                dwCacheScreenN = 0;
            pDevice->RunStart();
            pDevice->ImageBlt(nCacheFonTexId);
            pDevice->ImageBlt(nCacheProgressCacheId[dwCacheScreenN], &CacheScreenRect);
            pDevice->RunEnd();
        }
    }
    else
        dwCacheFiles++;

    // bRes = CopyFile(_srcName, _destName, TRUE)!=0;
    bRes = CopyFile(_srcName, _destName, !bCacheOverwrite) != 0;

    return bRes;
    // return (CopyFile(_srcName, _destName, TRUE) != 0);
}

bool XFileSTORM_DELETE(const char *_fileName)
{
    bool bRes;
    bRes = DeleteFile(_fileName) != 0;
    return bRes;
}

bool XProcessFile(const char *_srcDir, const char *_destDir, const char *_mask, const WIN32_FIND_DATA &_findData)
{
    if (!strcmp(_findData.cFileName, "."))
        return false;
    if (!strcmp(_findData.cFileName, ".."))
        return false;

    char *srcName = new char[strlen(_srcDir) + strlen(_findData.cFileName) + 2];
    char *destName = new char[strlen(_destDir) + strlen(_findData.cFileName) + 2];

    strcpy_s(srcName, _srcDir);
    if (_srcDir[strlen(_srcDir) - 1] != '\\')
        strcat_s(srcName, "\\");
    strcat_s(srcName, _findData.cFileName);

    strcpy_s(destName, _destDir);
    if (_destDir[strlen(_destDir) - 1] != '\\')
        strcat_s(destName, "\\");
    strcat_s(destName, _findData.cFileName);

    if (!(_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    { // file
        XFileCopy(srcName, destName);
    }
    else
    { // directory
        XDirCopy(srcName, destName, _mask);
    }

    delete[] srcName;
    delete[] destName;

    return true;
}

bool XProcessFileSTORM_DELETE(const char *_srcDir, const char *_mask, const WIN32_FIND_DATA &_findData)
{
    if (!strcmp(_findData.cFileName, "."))
        return false;
    if (!strcmp(_findData.cFileName, ".."))
        return false;

    char *srcName = new char[strlen(_srcDir) + strlen(_findData.cFileName) + 2];

    strcpy_s(srcName, _srcDir);
    if (_srcDir[strlen(_srcDir) - 1] != '\\')
        strcat_s(srcName, "\\");
    strcat_s(srcName, _findData.cFileName);

    if (!(_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    { // file
        XFileSTORM_DELETE(srcName);
    }
    else
    { // directory
        XDirSTORM_DELETE(srcName);
    }

    delete[] srcName;
    return true;
}

#endif

uint32_t FILE_SERVICE::MakeHashValue(const char *string)
{
    uint32_t hval = 0;
    while (*string != 0)
    {
        char v = *string++;
        if ('A' <= v && v <= 'Z')
            v += 'a' - 'A'; // case independent
        hval = (hval << 4) + static_cast<unsigned long>(v);
        const uint32_t g = hval & (static_cast<unsigned long>(0xf) << (32 - 4));
        if (g != 0)
        {
            hval ^= g >> (32 - 8);
            hval ^= g;
        }
    }
    return hval;
}

BOOL FILE_SERVICE::CacheDirectory(const char *pDirName)
{
    return false;
}

void FILE_SERVICE::MarkDirectoryCached(const char *pDirName)
{
}

BOOL FILE_SERVICE::UnCacheDirectory(const char *pDirName)
{
    return false;
}

BOOL FILE_SERVICE::IsCached(const char *pFileName)
{
    return false;
}

//=================================================================================================
INIFILE_T::~INIFILE_T()
{
    File_Service.RefDec(ifs_PTR);
}

void INIFILE_T::AddString(const char *section_name, const char *key_name, const char *string)
{
    ifs_PTR->AddString(section_name, key_name, string);
}

// write string to file, overwrite data if exist, throw EXS exception object if failed
void INIFILE_T::WriteString(const char *section_name, const char *key_name, const char *string)
{
    ifs_PTR->WriteString(section_name, key_name, string);
}

// write long value of key in pointed section if section and key exist, throw EXS object otherwise
void INIFILE_T::WriteLong(const char *section_name, const char *key_name, long value)
{
    ifs_PTR->WriteLong(section_name, key_name, value);
}

// write double value of key in pointed section if section and key exist, throw EXS object otherwise
void INIFILE_T::WriteDouble(const char *section_name, const char *key_name, double value)
{
    ifs_PTR->WriteDouble(section_name, key_name, value);
}

// fill buffer with key value, throw EXS exception object if failed or if section or key doesnt exist
void INIFILE_T::ReadString(const char *section_name, const char *key_name, char *buffer, uint32_t buffer_size)
{
    ifs_PTR->ReadString(&Search, section_name, key_name, buffer, buffer_size);
}

// fill buffer with key value if section and key exist, otherwise fill with def_string and return false
bool INIFILE_T::ReadString(const char *section_name, const char *key_name, char *buffer, uint32_t buffer_size,
                           const char *def_string)
{
    return ifs_PTR->ReadString(&Search, section_name, key_name, buffer, buffer_size, def_string);
}

// continue search from key founded in previous call this function or to function ReadString
// fill buffer with key value if section and key exist, otherwise return false
bool INIFILE_T::ReadStringNext(const char *section_name, const char *key_name, char *buffer, uint32_t buffer_size)
{
    return ifs_PTR->ReadStringNext(&Search, section_name, key_name, buffer, buffer_size);
}

// return long value of key in pointed section if section and key exist, throw EXS object otherwise
long INIFILE_T::GetLong(const char *section_name, const char *key_name)
{
    return ifs_PTR->GetLong(&Search, section_name, key_name);
}

// return long value of key in pointed section if section and key exist, if not - return def_value
long INIFILE_T::GetLong(const char *section_name, const char *key_name, long def_val)
{
    return ifs_PTR->GetLong(&Search, section_name, key_name, def_val);
}

// return double value of key in pointed section if section and key exist, throw EXS object otherwise
double INIFILE_T::GetDouble(const char *section_name, const char *key_name)
{
    return ifs_PTR->GetDouble(&Search, section_name, key_name);
}

// return double value of key in pointed section if section and key exist, if not - return def_value
double INIFILE_T::GetDouble(const char *section_name, const char *key_name, double def_val)
{
    return ifs_PTR->GetDouble(&Search, section_name, key_name, def_val);
}

bool INIFILE_T::GetLongNext(const char *section_name, const char *key_name, long *val)
{
    return ifs_PTR->GetLongNext(&Search, section_name, key_name, val);
}

bool INIFILE_T::GetDoubleNext(const char *section_name, const char *key_name, double *val)
{
    return ifs_PTR->GetDoubleNext(&Search, section_name, key_name, val);
}

// return double value of key in pointed section if section and key exist, throw EXS object otherwise
float INIFILE_T::GetFloat(const char *section_name, const char *key_name)
{
    return ifs_PTR->GetFloat(&Search, section_name, key_name);
}

// return float value of key in pointed section if section and key exist, if not - return def_value
float INIFILE_T::GetFloat(const char *section_name, const char *key_name, float def_val)
{
    return ifs_PTR->GetFloat(&Search, section_name, key_name, def_val);
}

bool INIFILE_T::GetFloatNext(const char *section_name, const char *key_name, float *val)
{
    return ifs_PTR->GetFloatNext(&Search, section_name, key_name, val);
}

void INIFILE_T::DeleteKey(const char *section_name, const char *key_name)
{
    ifs_PTR->DeleteKey(section_name, key_name);
}

void INIFILE_T::DeleteKey(const char *section_name, const char *key_name, const char *key_value)
{
    ifs_PTR->DeleteKey(section_name, key_name, key_value);
}

bool INIFILE_T::TestKey(const char *section_name, const char *key_name, const char *key_value)
{
    return ifs_PTR->TestKey(section_name, key_name, key_value);
}

void INIFILE_T::DeleteSection(const char *section_name)
{
    ifs_PTR->DeleteSection(section_name);
}

bool INIFILE_T::GetSectionName(char *section_name_buffer, long buffer_size)
{
    return ifs_PTR->GetSectionName(section_name_buffer, buffer_size);
}

bool INIFILE_T::GetSectionNameNext(char *section_name_buffer, long buffer_size)
{
    return ifs_PTR->GetSectionNameNext(section_name_buffer, buffer_size);
}

void INIFILE_T::Flush()
{
    ifs_PTR->Flush();
}

bool INIFILE_T::Reload()
{
    return ifs_PTR->Reload();
}

bool INIFILE_T::CaseSensitive(bool v)
{
    return ifs_PTR->CaseSensitive(v);
}

bool INIFILE_T::TestSection(const char *section_name)
{
    return ifs_PTR->TestSection(section_name);
};
