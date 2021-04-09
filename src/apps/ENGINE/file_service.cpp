#include "file_service.h"
#include "core.h"
#include "storm_assert.h"
#include "utf8.h"

#include <exception>
#include <storm/string_compare.hpp>
#include <string>

#define COMMENT ';'
#define SECTION_A '['
#define SECTION_B ']'
#define INI_EQUAL '='
#define VOIDSYMS_NUM 2
#define INI_SIGNATURE ";[SE2IF]"
const char INI_LINEFEED[3] = {0xd, 0xa, 0};
const char INI_VOIDSYMS[VOIDSYMS_NUM] = {0x20, 0x9};

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

std::fstream FILE_SERVICE::_CreateFile(const char *filename, std::ios::openmode mode)
{
    std::filesystem::path path = std::filesystem::u8path(filename);
    std::fstream fileS(path, mode);
    return fileS;
}

void FILE_SERVICE::_CloseFile(std::fstream &fileS)
{
    fileS.close();
}

void FILE_SERVICE::_SetFilePointer(std::fstream &fileS, std::streamoff off, std::ios::seekdir dir)
{
    fileS.seekp(off, dir);
}

int FILE_SERVICE::_DeleteFile(const char *filename)
{
    std::filesystem::path path = std::filesystem::u8path(filename);
    return std::filesystem::remove(path);
}

bool FILE_SERVICE::_WriteFile(std::fstream &fileS, const void *s, std::streamsize count)
{
    fileS.exceptions(std::fstream::failbit | std::fstream::badbit);
    try
    {
        fileS.write(reinterpret_cast<const char *>(s), count);
        return true;
    }
    catch (const std::fstream::failure &e)
    {
        core.tracelog->error("Failed to WriteFile: {}", e.what());
        return false;
    }
}

bool FILE_SERVICE::_ReadFile(std::fstream &fileS, void *s, std::streamsize count)
{
    fileS.exceptions(std::fstream::failbit | std::fstream::badbit);
    try
    {
        fileS.read(reinterpret_cast<char *>(s), count);
        return true;
    }
    catch (const std::fstream::failure &e)
    {
        core.tracelog->error("Failed to ReadFile: {}", e.what());
        return false;
    }
}

bool FILE_SERVICE::_FileOrDirectoryExists(const char *p)
{
    std::filesystem::path path = std::filesystem::u8path(p);
    return std::filesystem::exists(path);
}

std::vector<std::string> FILE_SERVICE::_GetPathsOrFilenamesByMask(const char *sourcePath, const char *mask,
                                                                  bool getPaths, bool onlyDirs, bool onlyFiles)
{
    std::vector<std::string> result;

    const auto fsPaths = _GetFsPathsByMask(sourcePath, mask, getPaths, onlyDirs, onlyFiles);
    for (std::filesystem::path curPath : fsPaths)
    {
        auto u8Path = curPath.u8string();
        result.emplace_back(u8Path.begin(), u8Path.end());
    }

    return result;
}

std::vector<std::filesystem::path> FILE_SERVICE::_GetFsPathsByMask(const char *sourcePath, const char *mask,
                                                                   bool getPaths, bool onlyDirs, bool onlyFiles)
{
    std::vector<std::filesystem::path> result;

    std::filesystem::path srcPath;
    if (sourcePath == nullptr || sourcePath == "")
    {
        srcPath = std::filesystem::current_path();
    }
    else
    {
        srcPath = std::filesystem::u8path(sourcePath);
    }

    std::filesystem::path curPath;
    for (auto &dirEntry : std::filesystem::directory_iterator(srcPath))
    {
        bool thisIsDir = dirEntry.is_directory();
        if ((onlyFiles && thisIsDir) || (onlyDirs && !thisIsDir))
        {
            continue;
        }
        curPath = dirEntry.path();
        if (storm::wildicmp(mask, curPath.filename().string().c_str()))
        {
            if (getPaths)
            {
                result.push_back(curPath);
            }
            else
            {
                result.push_back(curPath.filename());
            }
        }
    }

    return result;
}

std::time_t FILE_SERVICE::_ToTimeT(std::filesystem::file_time_type tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - std::filesystem::file_time_type::clock::now() +
                                                        system_clock::now());
    return system_clock::to_time_t(sctp);
}

std::filesystem::file_time_type FILE_SERVICE::_GetLastWriteTime(const char *filename)
{
    std::filesystem::path path = std::filesystem::u8path(filename);
    return std::filesystem::last_write_time(path);
}

void FILE_SERVICE::_FlushFileBuffers(std::fstream &fileS)
{
    fileS.flush();
}

uint32_t FILE_SERVICE::_GetCurrentDirectory(uint32_t nBufferLength, char *lpBuffer)
{
    wchar_t BufferW[MAX_PATH];
    uint32_t Res = GetCurrentDirectory(nBufferLength, BufferW);
    std::string CurrentDirectory = utf8::ConvertWideToUtf8(BufferW);
    strcpy_s(lpBuffer, nBufferLength, CurrentDirectory.c_str());
    return Res;
}

std::string FILE_SERVICE::_GetExecutableDirectory()
{
    wchar_t BufferW[MAX_PATH];
    uint32_t Res = GetModuleFileName(NULL, BufferW, MAX_PATH);
    Assert(Res);
    std::string ExePath = utf8::ConvertWideToUtf8(BufferW);
    size_t i = ExePath.rfind('\\', ExePath.length());
    if (i != std::string::npos)
    {
        return ExePath.substr(0, i);
    }
    return "";
}

std::uintmax_t FILE_SERVICE::_GetFileSize(const char *filename)
{
    std::filesystem::path path = std::filesystem::u8path(filename);
    return std::filesystem::file_size(path);
}

BOOL FILE_SERVICE::_SetCurrentDirectory(const char *lpPathName)
{
    std::wstring PathNameW = utf8::ConvertUtf8ToWide(lpPathName);
    return SetCurrentDirectory(PathNameW.c_str());
}

BOOL FILE_SERVICE::_CreateDirectory(const char *lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    std::wstring PathNameW = utf8::ConvertUtf8ToWide(lpPathName);
    return CreateDirectory(PathNameW.c_str(), lpSecurityAttributes);
}

std::uintmax_t FILE_SERVICE::_RemoveDirectory(const char *p)
{
    std::filesystem::path path = std::filesystem::u8path(p);
    return std::filesystem::remove_all(path);
}

BOOL FILE_SERVICE::_SetFileAttributes(const char *lpFileName, uint32_t dwFileAttributes)
{
    std::wstring FileNameW = utf8::ConvertUtf8ToWide(lpFileName);
    return SetFileAttributes(FileNameW.c_str(), dwFileAttributes);
}

//------------------------------------------------------------------------------------------------
// inifile objects managment
//

std::unique_ptr<INIFILE> FILE_SERVICE::CreateIniFile(const char *file_name, bool fail_if_exist)
{
    auto fileS = _CreateFile(file_name, std::ios::binary | std::ios::in);
    if (fileS.is_open() && fail_if_exist)
    {
        _CloseFile(fileS);
        return nullptr;
    }
    _CloseFile(fileS);
    fileS = _CreateFile(file_name, std::ios::binary | std::ios::out);
    if (!fileS.is_open())
    {
        core.tracelog->error("Can't create ini file: {}", file_name);
        return nullptr;
    }
    _CloseFile(fileS);
    return OpenIniFile(file_name);
}

std::unique_ptr<INIFILE> FILE_SERVICE::OpenIniFile(const char *file_name)
{
    for (auto n = 0; n <= Max_File_Index; n++)
    {
        if (OpenFiles[n] == nullptr || OpenFiles[n]->GetFileName() == nullptr)
            continue;
        if (_stricmp(OpenFiles[n]->GetFileName(), file_name) == 0)
        {
            OpenFiles[n]->IncReference();

            std::unique_ptr<INIFILE> v(new INIFILE_T(OpenFiles[n]));
            if (!v)
                throw std::exception();
            return v;
        }
    }

    for (auto n = 0; n < _MAX_OPEN_INI_FILES; n++)
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
        //        POP_CONTROL(0)
        // INIFILE_T object belonged to entity and must be deleted by entity
        // OpenFiles[n]->inifile_T = new INIFILE_T(OpenFiles[n]);
        // if(OpenFiles[n]->inifile_T == null) throw std::exception();
        // return OpenFiles[n]->inifile_T;

        std::unique_ptr<INIFILE> v(new INIFILE_T(OpenFiles[n]));
        if (!v)
            throw std::exception();
        return v;
    }
    //    POP_CONTROL(0)
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

    auto fileS = fio->_CreateFile(file_name, std::ios::binary | std::ios::in);
    if (!fileS.is_open())
    {
        core.tracelog->trace("Can't load file: {}", file_name);
        return false;
    }
    const auto dwLowSize = _GetFileSize(file_name);
    if (dwSize)
    {
        *dwSize = dwLowSize;
    }
    if (dwLowSize == 0)
    {
        *ppBuffer = nullptr;
        return false;
    }

    *ppBuffer = new char[dwLowSize];
    _ReadFile(fileS, *ppBuffer, dwLowSize);
    _CloseFile(fileS);
    return true;
}

//=================================================================================================

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
void INIFILE_T::ReadString(const char *section_name, const char *key_name, char *buffer, size_t buffer_size)
{
    ifs_PTR->ReadString(&Search, section_name, key_name, buffer, buffer_size);
}

// fill buffer with key value if section and key exist, otherwise fill with def_string and return false
bool INIFILE_T::ReadString(const char *section_name, const char *key_name, char *buffer, size_t buffer_size,
                           const char *def_string)
{
    return ifs_PTR->ReadString(&Search, section_name, key_name, buffer, buffer_size, def_string);
}

// continue search from key founded in previous call this function or to function ReadString
// fill buffer with key value if section and key exist, otherwise return false
bool INIFILE_T::ReadStringNext(const char *section_name, const char *key_name, char *buffer, size_t buffer_size)
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
