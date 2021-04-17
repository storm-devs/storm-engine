#pragma once

#include "ifs.h"
#include "vfile_service.h"
#include <memory>

#define _MAX_OPEN_INI_FILES 1024

class INIFILE_T : public INIFILE
{
  public:
    INIFILE_T(IFS *iR)
    {
        ifs_PTR = iR;
        Search.Section = nullptr;
        Search.Key = nullptr;
    }

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
    void ReadString(const char *section_name, const char *key_name, char *buffer, size_t buffer_size) override;
    // fill buffer with key value if section and key exist, otherwise fill with def_string and return false
    bool ReadString(const char *section_name, const char *key_name, char *buffer, size_t buffer_size,
                    const char *def_string) override;
    // continue search from key founded in previous call this function or to function ReadString
    // fill buffer with key value if section and key exist, otherwise return false
    bool ReadStringNext(const char *section_name, const char *key_name, char *buffer, size_t buffer_size) override;

    // return long value of key in pointed section if section and key exist, throw EXS object otherwise
    long GetLong(const char *section_name, const char *key_name) override;
    // return long value of key in pointed section if section and key exist, if not - return def_value
    long GetLong(const char *section_name, const char *key_name, long def_val) override;

    // return double value of key in pointed section if section and key exist, throw EXS object otherwise
    double GetDouble(const char *section_name, const char *key_name) override;
    // return double value of key in pointed section if section and key exist, if not - return def_value
    double GetDouble(const char *section_name, const char *key_name, double def_val) override;

    bool GetLongNext(const char *section_name, const char *key_name, long *val) override;

    float GetFloat(const char *section_name, const char *key_name) override;
    float GetFloat(const char *section_name, const char *key_name, float def_val) override;
    bool GetFloatNext(const char *section_name, const char *key_name, float *val) override;

    void DeleteKey(const char *section_name, const char *key_name) override;

    void DeleteKey(const char *section_name, const char *key_name, const char *key_value) override;

    void DeleteSection(const char *section_name) override;

    bool TestKey(const char *section_name, const char *key_name) override;

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
    std::fstream _CreateFile(const char *filename, std::ios::openmode mode) override;
    void _CloseFile(std::fstream &fileS) override;
    void _SetFilePointer(std::fstream &fileS, std::streamoff off, std::ios::seekdir dir) override;
    int _DeleteFile(const char *filename) override;
    bool _WriteFile(std::fstream &fileS, const void *s, std::streamsize count) override;
    bool _ReadFile(std::fstream &fileS, void *s, std::streamsize count) override;
    bool _FileOrDirectoryExists(const char *p) override;
    std::vector<std::string> _GetPathsOrFilenamesByMask(const char *sourcePath, const char *mask, bool getPaths,
                                                        bool onlyDirs = false, bool onlyFiles = true) override;
    std::vector<std::filesystem::path> _GetFsPathsByMask(const char *sourcePath, const char *mask, bool getPaths,
                                                         bool onlyDirs = false, bool onlyFiles = true) override;
    std::time_t _ToTimeT(std::filesystem::file_time_type tp) override;
    std::filesystem::file_time_type _GetLastWriteTime(const char *filename) override;
    void _FlushFileBuffers(std::fstream &fileS) override;
    uint32_t _GetCurrentDirectory(uint32_t nBufferLength, char *lpBuffer) override;
    std::string _GetExecutableDirectory() override;
    std::uintmax_t _GetFileSize(const char *filename) override;
    BOOL _SetCurrentDirectory(const char *lpPathName) override;
    BOOL _CreateDirectory(const char *lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) override;
    std::uintmax_t _RemoveDirectory(const char *p) override;
    BOOL _SetFileAttributes(const char *lpFileName, uint32_t dwFileAttributes) override;
    BOOL LoadFile(const char *file_name, char **ppBuffer, uint32_t *dwSize) override;
    // ini files section
    void Close();
    std::unique_ptr<INIFILE> CreateIniFile(const char *file_name, bool fail_if_exist) override;
    std::unique_ptr<INIFILE> OpenIniFile(const char *file_name) override;
    void RefDec(INIFILE *ini_obj);
    void FlushIniFiles();
};
