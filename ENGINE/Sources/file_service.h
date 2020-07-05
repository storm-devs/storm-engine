#ifndef _FILE_SERVICE_H_
#define _FILE_SERVICE_H_

#include "..\..\common_h\vfile_service.h"
#include "ifs.h"


#define _MAX_OPEN_INI_FILES		1024

class INIFILE_T : public INIFILE
{
public:
	 INIFILE_T(IFS * iR)
	 {
		ifs_PTR = iR;
		Search.Section = 0;
		Search.Key = 0;
	 };
	~INIFILE_T();
	
	SEARCH_DATA Search;

	IFS * ifs_PTR;

	// add string to file
	void	AddString(char * section_name, char * key_name, char * string);
	// write string to file, overwrite data if exist, throw EXS exception object if failed
	void	WriteString(char * section_name, char * key_name, char * string);
	// write long value of key in pointed section if section and key exist, throw EXS object otherwise
	void	WriteLong(char * section_name, char * key_name, long value);
	// write double value of key in pointed section if section and key exist, throw EXS object otherwise
	void	WriteDouble(char * section_name, char * key_name,double value);
	
	// fill buffer with key value, throw EXS exception object if failed or if section or key doesnt exist
	void	ReadString(char * section_name, char * key_name, char * buffer, dword buffer_size);
	// fill buffer with key value if section and key exist, otherwise fill with def_string and return false
	bool	ReadString(char * section_name, char * key_name, char * buffer, dword buffer_size, char * def_string);
	// continue search from key founded in previous call this function or to function ReadString
	// fill buffer with key value if section and key exist, otherwise return false
	bool	ReadStringNext(char * section_name, char * key_name, char * buffer, dword buffer_size);

	// return long value of key in pointed section if section and key exist, throw EXS object otherwise
	long	GetLong(char * section_name, char * key_name);
	// return long value of key in pointed section if section and key exist, if not - return def_value
	long	GetLong(char * section_name, char * key_name, long def_val);

	// return double value of key in pointed section if section and key exist, throw EXS object otherwise
	double	GetDouble(char * section_name, char * key_name);
	// return double value of key in pointed section if section and key exist, if not - return def_value
	double	GetDouble(char * section_name, char * key_name, double def_val);

	bool GetLongNext(char * section_name, char * key_name, long * val);
	bool GetDoubleNext(char * section_name, char * key_name, double * val);


	float	GetFloat(char * section_name, char * key_name);
	float	GetFloat(char * section_name, char * key_name, float def_val);
	bool	GetFloatNext(char * section_name, char * key_name, float * val);


	void DeleteKey(char * section_name, char * key_name);

	void DeleteKey(char * section_name, char * key_name, char * key_value);

	void DeleteSection(char * section_name);

	bool TestKey(char * section_name, char * key_name, char * key_value);

	bool GetSectionName(char * section_name_buffer, long buffer_size);

	bool GetSectionNameNext(char * section_name_buffer, long buffer_size);

	void Flush();

	bool Reload();

	bool CaseSensitive(bool v);

	bool TestSection(char * section_name);

};

#define CACHEDIRSTABLESIZE	1024
struct HASHLINEDESC
{
	HASHLINEDESC(){dwElementsNum = 0; pDirName = 0;};
	DWORD dwElementsNum;
	char ** pDirName;
};

class FILE_SERVICE : public VFILE_SERVICE
{
protected:
	//INIFILE_R * OpenFiles[_MAX_OPEN_INI_FILES];
	IFS * OpenFiles[_MAX_OPEN_INI_FILES];
	dword Files_Num;
	dword Max_File_Index;

	HASHLINEDESC CacheDirs[CACHEDIRSTABLESIZE];

public:

	
	FILE_SERVICE();
	~FILE_SERVICE();
	HANDLE _CreateFile(LPCTSTR lpFileName,DWORD dwDesiriedAccess = GENERIC_READ,DWORD dwShareMode = FILE_SHARE_READ,DWORD dwCreationDisposition = OPEN_EXISTING);
	void   _CloseHandle(HANDLE hFile);
	DWORD  _SetFilePointer(HANDLE hFile,long DistanceToMove,PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod);
	BOOL   _DeleteFile(LPCTSTR lpFileName);
	BOOL   _WriteFile(HANDLE hFile,LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten);
	BOOL   _ReadFile(HANDLE hFile,LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead);
	HANDLE _FindFirstFile(LPCTSTR lpFileName,LPWIN32_FIND_DATA lpFindFileData);
	BOOL   _FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData);
	BOOL   _FindClose(HANDLE hFindFile);
	BOOL   _FlushFileBuffers(HANDLE hFile);
	DWORD  _GetCurrentDirectory(DWORD nBufferLength,LPTSTR lpBuffer);
	BOOL   _GetDiskFreeSpaceEx(LPCTSTR lpDirectoryName,PULARGE_INTEGER lpFreeBytesAvailableToCaller,
		     PULARGE_INTEGER lpTotalNumberOfBytes,PULARGE_INTEGER lpTotalNumberOfFreeBytes);
	UINT   _GetDriveType(LPCTSTR lpRootPathName);
	DWORD  _GetFileSize(HANDLE hFile,LPDWORD lpFileSizeHigh);
	DWORD  _GetLogicalDrives(VOID);
	DWORD  _GetLogicalDriveStrings(DWORD nBufferLength,LPTSTR lpBuffer);
	BOOL   _SetCurrentDirectory(LPCTSTR lpPathName);
	BOOL   _CreateDirectory(LPCTSTR lpPathName,LPSECURITY_ATTRIBUTES lpSecurityAttributes);
	BOOL   _RemoveDirectory(LPCTSTR lpPathName);
 	BOOL   _CopyFile(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,BOOL bFailIfExists);
	BOOL   _SetFileAttributes(LPCTSTR lpFileName,DWORD dwFileAttributes);
	BOOL FileExist(const char * file_name);
	BOOL LoadFile(const char * file_name, char * * ppBuffer, dword * dwSize);
	// ini files section
	void  Close();
	INIFILE * CreateIniFile(const char * file_name, bool fail_if_exist);
	INIFILE * OpenIniFile(const char * file_name);
	void	  RefDec(INIFILE * ini_obj);
	void FlushIniFiles();
	BOOL SetDrive(const char * pDriveName);

	BOOL CacheDirectory(const char * pDirName);
	BOOL UnCacheDirectory(const char * pDirName);
	DWORD MakeHashValue(const char * string);
	BOOL IsCached(const char * pFileName);
	void MarkDirectoryCached(const char * pFileName);

};

#endif