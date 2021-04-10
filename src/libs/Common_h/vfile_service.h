#pragma once

#include <Windows.h>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

class INIFILE;

class VFILE_SERVICE
{
  public:
    // VFILE_SERVICE()= 0;
    virtual ~VFILE_SERVICE()
    {
    }

    virtual std::fstream _CreateFile(const char *filename, std::ios::openmode mode) = 0;
    virtual void _CloseFile(std::fstream &fileS) = 0;
    virtual void _SetFilePointer(std::fstream &fileS, std::streamoff off, std::ios::seekdir dir) = 0;
    virtual int _DeleteFile(const char *filename) = 0;
    virtual bool _WriteFile(std::fstream &fileS, const void *s, std::streamsize count) = 0;
    virtual bool _ReadFile(std::fstream &fileS, void *s, std::streamsize count) = 0;
    virtual bool _FileOrDirectoryExists(const char *p) = 0;
    virtual std::vector<std::string> _GetPathsOrFilenamesByMask(const char *sourcePath, const char *mask, bool getPaths,
                                                                bool onlyDirs = false, bool onlyFiles = true) = 0;
    virtual std::vector<std::filesystem::path> _GetFsPathsByMask(const char *sourcePath, const char *mask,
                                                                 bool getPaths, bool onlyDirs = false,
                                                                 bool onlyFiles = true) = 0;
    virtual std::time_t _ToTimeT(std::filesystem::file_time_type tp) = 0;
    virtual std::filesystem::file_time_type _GetLastWriteTime(const char *filename) = 0;
    virtual void _FlushFileBuffers(std::fstream &fileS) = 0;
    virtual uint32_t _GetCurrentDirectory(uint32_t nBufferLength, char *lpBuffer) = 0;
    virtual std::string _GetExecutableDirectory() = 0;
    virtual std::uintmax_t _GetFileSize(const char *filename) = 0;
    virtual BOOL _SetCurrentDirectory(const char *lpPathName) = 0;
    virtual BOOL _CreateDirectory(const char *lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) = 0;
    virtual std::uintmax_t _RemoveDirectory(const char *p) = 0;
    virtual BOOL _SetFileAttributes(const char *lpFileName, uint32_t dwFileAttributes) = 0;
    virtual BOOL LoadFile(const char *file_name, char **ppBuffer, uint32_t *dwSize = nullptr) = 0;

    // ini files section
    virtual std::unique_ptr<INIFILE> CreateIniFile(const char *file_name, bool fail_if_exist) = 0;
    virtual std::unique_ptr<INIFILE> OpenIniFile(const char *file_name) = 0;
};

//------------------------------------------------------------------------------------------------
// handle with text files in format:
//
// ; commentary
// [section name 1]
// key_name1 = string1
// ...
// key_nameN = stringN
// ...
// [section name N]
// ...
// spacebar and tab symbols ignored:
// for key_name: from line start to first significant symbol and from end of key_name to '=' symbol
// for string: from symbol '=' to first significant symbol and at the end of line
//
// If section name is null, functions search in all keys

class INIFILE
{
  public:
    virtual ~INIFILE() = default;

    // add string to file
    virtual void AddString(const char *section_name, const char *key_name, const char *string) = 0;
    // write string to file, overwrite data if exist, throw EXS exception object if failed
    virtual void WriteString(const char *section_name, const char *key_name, const char *string) = 0;
    // write long value of key in pointed section if section and key exist, throw EXS object otherwise
    virtual void WriteLong(const char *section_name, const char *key_name, long value) = 0;
    // write double value of key in pointed section if section and key exist, throw EXS object otherwise
    virtual void WriteDouble(const char *section_name, const char *key_name, double value) = 0;

    // fill buffer with key value, throw EXS exception object if failed or if section or key doesnt exist
    virtual void ReadString(const char *section_name, const char *key_name, char *buffer, size_t buffer_size) = 0;

    // fill buffer with key value if section and key exist, otherwise fill with def_string and return false
    virtual bool ReadString(const char *section_name, const char *key_name, char *buffer, size_t buffer_size,
                            const char *def_string) = 0;

    virtual std::size_t ForEachString(const char *section_name, const char *key_name,
                                      std::function<bool(std::size_t, char *)> f) = 0;

    virtual std::size_t ForEachString(const char *section_name, const char *key_name,
                                      std::function<void(char *)> f) = 0;

    // return long value of key in pointed section if section and key exist, throw EXS object otherwise
    virtual long GetLong(const char *section_name, const char *key_name) = 0;
    // return long value of key in pointed section if section and key exist, if not - return def_value
    virtual long GetLong(const char *section_name, const char *key_name, long def_val) = 0;

    // return double value of key in pointed section if section and key exist, throw EXS object otherwise
    virtual double GetDouble(const char *section_name, const char *key_name) = 0;
    // return double value of key in pointed section if section and key exist, if not - return def_value
    virtual double GetDouble(const char *section_name, const char *key_name, double def_val) = 0;

    virtual float GetFloat(const char *section_name, const char *key_name) = 0;
    virtual float GetFloat(const char *section_name, const char *key_name, float def_val) = 0;

    // virtual void    SetSearch(void *)= 0;

    virtual void DeleteKey(const char *section_name, const char *key_name) = 0;

    virtual void DeleteSection(const char *section_name) = 0;

    virtual bool TestKey(const char *section_name, const char *key_name) = 0;

    virtual bool GetSectionName(char *section_name_buffer, long buffer_size) = 0;
    virtual bool GetSectionNameNext(char *section_name_buffer, long buffer_size) = 0;
};

//
extern VFILE_SERVICE *fio;
