#ifndef _VFILE_SERVICE_H_
#define _VFILE_SERVICE_H_

#define XBOXDRIVE_NONE		""
#define XBOXDRIVE_CACHE		"z:\\"
#define XBOXDRIVE_DVD		"d:\\"

#ifndef _XBOX 
#include <windows.h>
#else
#include <xtl.h>
#endif

#include "d_types.h"

class INIFILE;

class VFILE_SERVICE
{
public:
	VFILE_SERVICE(){};
	virtual ~VFILE_SERVICE(){};
	virtual HANDLE _CreateFile(LPCTSTR lpFileName,DWORD dwDesiriedAccess = GENERIC_READ,DWORD dwShareMode = FILE_SHARE_READ,DWORD dwCreationDisposition = OPEN_EXISTING)=0;
	virtual void   _CloseHandle(HANDLE hFile)=0;
	virtual DWORD  _SetFilePointer(HANDLE hFile,long DistanceToMove,PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod)=0;
	virtual BOOL   _DeleteFile(LPCTSTR lpFileName)=0;
	virtual BOOL   _WriteFile(HANDLE hFile,LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten)=0;
	virtual BOOL   _ReadFile(HANDLE hFile,LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead)=0;
	virtual HANDLE _FindFirstFile(LPCTSTR lpFileName,LPWIN32_FIND_DATA lpFindFileData)=0;
	virtual BOOL   _FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData)=0;
	virtual BOOL   _FindClose(HANDLE hFindFile)=0;
	virtual BOOL   _FlushFileBuffers(HANDLE hFile)=0;
	virtual DWORD  _GetCurrentDirectory(DWORD nBufferLength,LPTSTR lpBuffer)=0;
	virtual BOOL   _GetDiskFreeSpaceEx(LPCTSTR lpDirectoryName,PULARGE_INTEGER lpFreeBytesAvailableToCaller,
		     PULARGE_INTEGER lpTotalNumberOfBytes,PULARGE_INTEGER lpTotalNumberOfFreeBytes)=0;
	virtual UINT   _GetDriveType(LPCTSTR lpRootPathName)=0;
	virtual DWORD  _GetFileSize(HANDLE hFile,LPDWORD lpFileSizeHigh)=0;
	virtual DWORD  _GetLogicalDrives(VOID)=0;
	virtual DWORD  _GetLogicalDriveStrings(DWORD nBufferLength,LPTSTR lpBuffer)=0;
	virtual BOOL   _SetCurrentDirectory(LPCTSTR lpPathName)=0;
	virtual BOOL   _CreateDirectory(LPCTSTR lpPathName,LPSECURITY_ATTRIBUTES lpSecurityAttributes)=0;
	virtual BOOL   _RemoveDirectory(LPCTSTR lpPathName)=0;
 	virtual BOOL   _CopyFile(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,BOOL bFailIfExists)=0;
	virtual BOOL   _SetFileAttributes(LPCTSTR lpFileName,DWORD dwFileAttributes)= 0;
	virtual INIFILE * OpenIniFile(const char * file_name)= 0;
	virtual BOOL FileExist(const char * file_name)= 0;
	virtual BOOL LoadFile(const char * file_name, char * * ppBuffer, dword * dwSize = 0)= 0;
	virtual INIFILE * CreateIniFile(const char * file_name, bool fail_if_exist)= 0;
	virtual BOOL SetDrive(const char * pDriveName = 0)=0;
	virtual BOOL CacheDirectory(const char * pDirName)=0;
	virtual BOOL UnCacheDirectory(const char * pDirName)=0;


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
	 INIFILE(){};
	virtual ~INIFILE(){};

	// add string to file
	virtual void	AddString(char * section_name, char * key_name, char * string){};
	// write string to file, overwrite data if exist, throw EXS exception object if failed
	virtual void	WriteString(char * section_name, char * key_name, char * string){};
	// write long value of key in pointed section if section and key exist, throw EXS object otherwise
	virtual void	WriteLong(char * section_name, char * key_name, long value){};
	// write double value of key in pointed section if section and key exist, throw EXS object otherwise
	virtual void	WriteDouble(char * section_name, char * key_name,double value){};

	// fill buffer with key value, throw EXS exception object if failed or if section or key doesnt exist
	virtual void	ReadString(char * section_name, char * key_name, char * buffer, dword buffer_size){};
	// fill buffer with key value if section and key exist, otherwise fill with def_string and return false
	virtual bool	ReadString(char * section_name, char * key_name, char * buffer, dword buffer_size, char * def_string){return false;};
	// continue search from key founded in previous call this function or to function ReadString
	// fill buffer with key value if section and key exist, otherwise return false
	virtual bool	ReadStringNext(char * section_name, char * key_name, char * buffer, dword buffer_size){return false;};
	
	// return long value of key in pointed section if section and key exist, throw EXS object otherwise
	virtual long	GetLong(char * section_name, char * key_name){return 0;};
	// return long value of key in pointed section if section and key exist, if not - return def_value
	virtual long	GetLong(char * section_name, char * key_name, long def_val){return 0;};
	// continue scanning for key in section, fill val with long value of key if it found and return true
	// if not - return false
	virtual bool	GetLongNext(char * section_name, char * key_name, long * val){return 0;};

	// return double value of key in pointed section if section and key exist, throw EXS object otherwise
	virtual double	GetDouble(char * section_name, char * key_name){return 0;};
	// return double value of key in pointed section if section and key exist, if not - return def_value
	virtual double	GetDouble(char * section_name, char * key_name, double def_val){return 0;};
	// continue scanning for key in section, fill val with double value of key if it found and return true
	// if not - return false
	virtual bool	GetDoubleNext(char * section_name, char * key_name, double * val){return 0;};

	virtual float	GetFloat(char * section_name, char * key_name){return 0;};
	virtual float	GetFloat(char * section_name, char * key_name, float def_val){return 0;};
	virtual bool	GetFloatNext(char * section_name, char * key_name, float * val){return 0;};


	virtual void	SetSearch(void *){};

	virtual void	DeleteKey(char * section_name, char * key_name){};

	virtual void	DeleteKey(char * section_name, char * key_name, char * key_value){};

	virtual void	DeleteSection(char * section_name){};

	virtual bool	TestKey(char * section_name, char * key_name, char * key_value){return false;};

	virtual bool	GetSectionName(char * section_name_buffer, long buffer_size) {return false;};
	virtual bool	GetSectionNameNext(char * section_name_buffer, long buffer_size) {return false;};

	virtual void	Flush(){};
	virtual bool	Reload(){return false;};
	virtual bool	CaseSensitive(bool yes){return false;};
	virtual bool	TestSection(char * section_name){return false;};



};

#endif