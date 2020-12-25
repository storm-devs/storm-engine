#ifndef _FILE_SERVICE_H_
#define _FILE_SERVICE_H_

#include "ifs.h"
#include "vfile_service.h"

#define _MAX_OPEN_INI_FILES 1024

class INIFILE_T : public INIFILE
{
  public:
    INIFILE_T(IFS *iR)
    {
        ifs_PTR = iR;
        Search.Section = nullptr;
        Search.Key = nullptr;
    };
    ~INIFILE_T();

    SEARCH_DATA Search;

    IFS *ifs_PTR;

    // add string to file
    void AddString(const char *section_name, const char *key_name, const char *string) override;
    // write string to file, overwrite data if exist, throw EXS exception object if failed
    void WriteString(const char *section_name, const char *key_name, const char *string) override;
    // write long value of key in pointed section if section and key exist, throw EXS object otherwise
    void WriteLong(const char *section_name, const char *key_name, long value) override;
    // write double value of key in pointed section if section and key exist, throw EXS object otherwise
    void WriteDouble(const char *section_name, const char *key_name, double value) override;

    // fill buffer with key value, throw EXS exception object if failed or if section or key doesnt exist
    void ReadString(const char *section_name, const char *key_name, char *buffer, uint32_t buffer_size) override;
    // fill buffer with key value if section and key exist, otherwise fill with def_string and return false
    bool ReadString(const char *section_name, const char *key_name, char *buffer, uint32_t buffer_size,
                    const char *def_string) override;
    // continue search from key founded in previous call this function or to function ReadString
    // fill buffer with key value if section and key exist, otherwise return false
    bool ReadStringNext(const char *section_name, const char *key_name, char *buffer, uint32_t buffer_size) override;

    // return long value of key in pointed section if section and key exist, throw EXS object otherwise
    long GetLong(const char *section_name, const char *key_name) override;
    // return long value of key in pointed section if section and key exist, if not - return def_value
    long GetLong(const char *section_name, const char *key_name, long def_val) override;

    // return double value of key in pointed section if section and key exist, throw EXS object otherwise
    double GetDouble(const char *section_name, const char *key_name) override;
    // return double value of key in pointed section if section and key exist, if not - return def_value
    double GetDouble(const char *section_name, const char *key_name, double def_val) override;

    bool GetLongNext(const char *section_name, const char *key_name, long *val) override;
    bool GetDoubleNext(const char *section_name, const char *key_name, double *val) override;

    float GetFloat(const char *section_name, const char *key_name) override;
    float GetFloat(const char *section_name, const char *key_name, float def_val) override;
    bool GetFloatNext(const char *section_name, const char *key_name, float *val) override;

    void DeleteKey(const char *section_name, const char *key_name) override;

    void DeleteKey(const char *section_name, const char *key_name, const char *key_value) override;

    void DeleteSection(const char *section_name) override;

    bool TestKey(const char *section_name, const char *key_name, const char *key_value) override;

    bool GetSectionName(char *section_name_buffer, long buffer_size) override;

    bool GetSectionNameNext(char *section_name_buffer, long buffer_size) override;

    void Flush() override;

    bool Reload() override;

    bool CaseSensitive(bool v) override;

    bool TestSection(const char *section_name) override;
};

class FILE_SERVICE : public VFILE_SERVICE
{
  protected:
    // INIFILE_R * OpenFiles[_MAX_OPEN_INI_FILES];
    IFS *OpenFiles[_MAX_OPEN_INI_FILES];
    uint32_t Files_Num;
    uint32_t Max_File_Index;

  public:
    FILE_SERVICE();
    ~FILE_SERVICE();
    HANDLE _CreateFile(const char *lpFileName, uint32_t dwDesiriedAccess = GENERIC_READ,
                       uint32_t dwShareMode = FILE_SHARE_READ, uint32_t dwCreationDisposition = OPEN_EXISTING) override;
    void _CloseHandle(HANDLE hFile) override;
    uint32_t _SetFilePointer(HANDLE hFile, long DistanceToMove, long *lpDistanceToMoveHigh,
                             uint32_t dwMoveMethod) override;
    BOOL _DeleteFile(const char *lpFileName) override;
    BOOL _WriteFile(HANDLE hFile, const void *lpBuffer, uint32_t nNumberOfBytesToWrite,
                    uint32_t *lpNumberOfBytesWritten) override;
    BOOL _ReadFile(HANDLE hFile, void *lpBuffer, uint32_t nNumberOfBytesToRead, uint32_t *lpNumberOfBytesRead) override;
    HANDLE _FindFirstFile(const char *lpFileName, LPWIN32_FIND_DATA lpFindFileData) override;
    BOOL _FindNextFile(HANDLE hFindFile, LPWIN32_FIND_DATA lpFindFileData) override;
    BOOL _FindClose(HANDLE hFindFile) override;
    BOOL _FlushFileBuffers(HANDLE hFile) override;
    uint32_t _GetCurrentDirectory(uint32_t nBufferLength, char *lpBuffer) override;
    std::string _GetExecutableDirectory() override;
    BOOL _GetDiskFreeSpaceEx(const char *lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailableToCaller,
                             PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes) override;
    UINT _GetDriveType(const char *lpRootPathName) override;
    uint32_t _GetFileSize(HANDLE hFile, uint32_t *lpFileSizeHigh) override;
    uint32_t _GetLogicalDrives(VOID) override;
    uint32_t _GetLogicalDriveStrings(uint32_t nBufferLength, char *lpBuffer) override;
    BOOL _SetCurrentDirectory(const char *lpPathName) override;
    BOOL _CreateDirectory(const char *lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) override;
    BOOL _RemoveDirectory(const char *lpPathName) override;
    BOOL _CopyFile(const char *lpExistingFileName, const char *lpNewFileName, bool bFailIfExists) override;
    BOOL _SetFileAttributes(const char *lpFileName, uint32_t dwFileAttributes) override;
    BOOL FileExist(const char *file_name) override;
    BOOL LoadFile(const char *file_name, char **ppBuffer, uint32_t *dwSize) override;
    // ini files section
    void Close();
    INIFILE *CreateIniFile(const char *file_name, bool fail_if_exist) override;
    INIFILE *OpenIniFile(const char *file_name) override;
    void RefDec(INIFILE *ini_obj);
    void FlushIniFiles();
    BOOL SetDrive(const char *pDriveName) override;

    BOOL CacheDirectory(const char *pDirName) override;
    BOOL UnCacheDirectory(const char *pDirName) override;
    uint32_t MakeHashValue(const char *string);
    BOOL IsCached(const char *pFileName);
    void MarkDirectoryCached(const char *pFileName);
};

#endif