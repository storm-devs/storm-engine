#include "file_service.h"
#include "system_log.h"
#include "..\..\common_h\exs.h"
#include "..\..\common_h\memop.h"

#ifdef _XBOX
bool XProcessFile(const char *_srcDir, const char *_destDir, const char *_mask, const WIN32_FIND_DATA &_findData);
bool XDirCopy(const char *_srcDir, const char *_destDir, const char *_mask);
void CacheData();
bool XFileDelete(const char *_fileName);
bool XProcessFileDelete(const char *_srcDir, const char *_mask, const WIN32_FIND_DATA &_findData);

#include "..\..\common_h\dx8render.h"
extern VDX8RENDER * pDevice;
extern DWORD dwCacheScreenN;
extern RECT CacheScreenRect;
extern DWORD dwCacheFiles;
extern bool bCacheOverwrite;
extern long nCacheFonTexId;
extern long nCacheProgressCacheId[10];
extern bool bCacheEstDirsStructure;

#endif

#define		COMMENT			';'
#define		SECTION_A		'['
#define		SECTION_B		']'
#define		INI_EQUAL		'='
#define		VOIDSYMS_NUM	2
#define		INI_SIGNATURE	";[SE2IF]"
const char	INI_LINEFEED[3] = {0xd,0xa,0};
const char	INI_VOIDSYMS[VOIDSYMS_NUM] = {0x20,0x9};
char sDriveLetter[8] = "d:\\";

//#define sDriveLetter	"d:\\"

extern FILE_SERVICE File_Service;

void FILE_SERVICE::FlushIniFiles()
{
	for(dword n=0;n<=Max_File_Index;n++) 
	{
		if(OpenFiles[n] == null) continue;
		OpenFiles[n]->FlushFile();
	}
}

FILE_SERVICE::FILE_SERVICE()
{
	Files_Num = 0;
	Max_File_Index = 0;
	for(dword n=0;n<_MAX_OPEN_INI_FILES;n++) OpenFiles[n] = null;

}
FILE_SERVICE::~FILE_SERVICE()
{
	Close();
}

HANDLE FILE_SERVICE::_CreateFile(LPCTSTR lpFileName,DWORD dwDesiriedAccess,DWORD dwShareMode,DWORD dwCreationDisposition)
{
	HANDLE fh;
#ifndef _XBOX
	fh = CreateFile(lpFileName,dwDesiriedAccess,dwShareMode,0,dwCreationDisposition,FILE_ATTRIBUTE_NORMAL,0);
	//DWORD er = GetLastError();
#else
	bool bCached;
	char xbox_file_name[MAX_PATH];
	char file_name_buffer[MAX_PATH];
	/*if(sDriveLetter[0])	
	{
		strcpy(xbox_file_name,sDriveLetter);
		strcat(xbox_file_name,lpFileName);
	}
	else strcpy(xbox_file_name,lpFileName);//*/
	if(stricmp(sDriveLetter,XBOXDRIVE_DVD)==0)
	{
		if(IsCached(lpFileName))
		{
			bCached = true;
			strcpy(xbox_file_name,XBOXDRIVE_CACHE);
			strcat(xbox_file_name,lpFileName);
		}
		else
		{
			bCached = false;
			strcpy(xbox_file_name,XBOXDRIVE_DVD);
			strcat(xbox_file_name,lpFileName);
		}
	}
	else
	{
		if(sDriveLetter[0])	
		{
			strcpy(xbox_file_name,sDriveLetter);
			strcat(xbox_file_name,lpFileName);
		}
		else strcpy(xbox_file_name,lpFileName);
	}
	

	fh = CreateFile(xbox_file_name,dwDesiriedAccess,dwShareMode,0,dwCreationDisposition,FILE_ATTRIBUTE_NORMAL,0);
	if(fh == INVALID_HANDLE_VALUE && bCached)
	{
		// if file isn't in cache - copy to cache (time distributed caching)
		
		strcpy(file_name_buffer,XBOXDRIVE_DVD);
		strcat(file_name_buffer,lpFileName);
		bool bRes = CopyFile(file_name_buffer,xbox_file_name,false)!=0;
		if(bRes) fh = CreateFile(xbox_file_name,dwDesiriedAccess,dwShareMode,0,dwCreationDisposition,FILE_ATTRIBUTE_NORMAL,0);
		else fh = CreateFile(file_name_buffer,dwDesiriedAccess,dwShareMode,0,dwCreationDisposition,FILE_ATTRIBUTE_NORMAL,0);

		/*//trace("file cache miss");
		strcpy(xbox_file_name,XBOXDRIVE_DVD);
		strcat(xbox_file_name,lpFileName);
		fh = CreateFile(xbox_file_name,dwDesiriedAccess,dwShareMode,0,dwCreationDisposition,FILE_ATTRIBUTE_NORMAL,0);
		//*/
	}
#endif
	//if(fh == INVALID_HANDLE_VALUE) if(Exceptions_Mask & _X_NO_FILE) _THROW(_X_NO_FILE);
	return fh;
}
void   FILE_SERVICE::_CloseHandle(HANDLE hFile)
{
	CloseHandle(hFile);
}
DWORD  FILE_SERVICE::_SetFilePointer(HANDLE hFile,long DistanceToMove,PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod)
{
	return SetFilePointer(hFile,DistanceToMove,lpDistanceToMoveHigh,dwMoveMethod);
}
BOOL   FILE_SERVICE::_DeleteFile(LPCTSTR lpFileName)
{
#ifdef _XBOX
	char xbox_file_name[MAX_PATH];
	if(sDriveLetter[0])	
	{
		strcpy(xbox_file_name,sDriveLetter);
		strcat(xbox_file_name,lpFileName);
	}
	else strcpy(xbox_file_name,lpFileName);
	return DeleteFile(xbox_file_name);
#else
	return DeleteFile(lpFileName);
#endif
}
BOOL   FILE_SERVICE::_WriteFile(HANDLE hFile,LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten)
{
	BOOL bRes;
	dword dwR;
	bRes = WriteFile(hFile,lpBuffer,nNumberOfBytesToWrite,&dwR,0);
	if(lpNumberOfBytesWritten != 0) *lpNumberOfBytesWritten = dwR;
//	if(dwR != nNumberOfBytesToWrite) if(Exceptions_Mask & _X_NO_FILE_WRITE) _THROW(_X_NO_FILE_WRITE);
	return bRes;
}
BOOL   FILE_SERVICE::_ReadFile(HANDLE hFile,LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead)
{
	BOOL bRes;
	dword dwR;
	bRes = ReadFile(hFile,lpBuffer,nNumberOfBytesToRead,&dwR,0);
	if(lpNumberOfBytesRead != 0) *lpNumberOfBytesRead = dwR;
//	if(dwR != nNumberOfBytesToRead) if(Exceptions_Mask & _X_NO_FILE_READ) _THROW(_X_NO_FILE_READ);
	return bRes;
}
HANDLE FILE_SERVICE::_FindFirstFile(LPCTSTR lpFileName,LPWIN32_FIND_DATA lpFindFileData)
{
	HANDLE hFile;
#ifndef _XBOX
	hFile = FindFirstFile(lpFileName,lpFindFileData);
#else
	char xbox_file_name[MAX_PATH];
	//strcpy(xbox_file_name,"d:\\");
	strcpy(xbox_file_name,sDriveLetter);
	strcat(xbox_file_name,lpFileName);
	hFile = FindFirstFile(xbox_file_name,lpFindFileData);
#endif
	return hFile;
}
BOOL   FILE_SERVICE::_FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData)
{
	return FindNextFile(hFindFile,lpFindFileData);
}
BOOL   FILE_SERVICE::_FindClose(HANDLE hFindFile)
{
	return FindClose(hFindFile);
}
BOOL   FILE_SERVICE::_FlushFileBuffers(HANDLE hFile)
{
	return FlushFileBuffers(hFile);
}
DWORD  FILE_SERVICE::_GetCurrentDirectory(DWORD nBufferLength,LPTSTR lpBuffer)
{
#ifndef _XBOX
	return GetCurrentDirectory(nBufferLength,lpBuffer);
#else
	//if(nBufferLength > strlen("d:\\") && lpBuffer) strcpy(lpBuffer,"d:\\");
	//return strlen("d:\\");

	if(nBufferLength > strlen(sDriveLetter) && lpBuffer) strcpy(lpBuffer,sDriveLetter);
	return strlen(sDriveLetter);

#endif
}
BOOL   FILE_SERVICE::_GetDiskFreeSpaceEx(LPCTSTR lpDirectoryName,PULARGE_INTEGER lpFreeBytesAvailableToCaller,
		 PULARGE_INTEGER lpTotalNumberOfBytes,PULARGE_INTEGER lpTotalNumberOfFreeBytes)
{
	return GetDiskFreeSpaceEx(lpDirectoryName,lpFreeBytesAvailableToCaller,
		 lpTotalNumberOfBytes,lpTotalNumberOfFreeBytes);
}
UINT   FILE_SERVICE::_GetDriveType(LPCTSTR lpRootPathName)
{
#ifndef _XBOX
	return GetDriveType(lpRootPathName);
#else
	return 0;
#endif
	
}
DWORD  FILE_SERVICE::_GetFileSize(HANDLE hFile,LPDWORD lpFileSizeHigh)
{
	return GetFileSize(hFile,lpFileSizeHigh);
}
DWORD  FILE_SERVICE::_GetLogicalDrives()
{
#ifndef _XBOX
	return GetLogicalDrives();
#else
	return 0;
#endif
	
}
DWORD  FILE_SERVICE::_GetLogicalDriveStrings(DWORD nBufferLength,LPTSTR lpBuffer)
{
#ifndef _XBOX
	return GetLogicalDriveStrings(nBufferLength,lpBuffer);
#else
	return 0;
#endif
	
}
BOOL   FILE_SERVICE::_SetCurrentDirectory(LPCTSTR lpPathName)
{
#ifndef _XBOX
	return SetCurrentDirectory(lpPathName);
#else
	return 0;
#endif
	
}
BOOL   FILE_SERVICE::_CreateDirectory(LPCTSTR lpPathName,LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	return CreateDirectory(lpPathName,lpSecurityAttributes);
}
BOOL   FILE_SERVICE::_RemoveDirectory(LPCTSTR lpPathName)
{
	return RemoveDirectory(lpPathName);
}
BOOL   FILE_SERVICE::_CopyFile(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,BOOL bFailIfExists)
{
	return CopyFile(lpExistingFileName,lpNewFileName,bFailIfExists);
}

BOOL   FILE_SERVICE::_SetFileAttributes(LPCTSTR lpFileName,DWORD dwFileAttributes)
{
	return SetFileAttributes(lpFileName,dwFileAttributes);
}

BOOL FILE_SERVICE::FileExist(const char * file_name)
{
	HANDLE fh;
	fh = _CreateFile(file_name);
	if(fh == INVALID_HANDLE_VALUE) return false;
	CloseHandle(fh);
	return true;
}

//------------------------------------------------------------------------------------------------
// inifile objects managment 
//

INIFILE * FILE_SERVICE::CreateIniFile(const char * file_name, bool fail_if_exist)
{
	HANDLE fh;
	fh = _CreateFile(file_name,GENERIC_READ,0,OPEN_EXISTING);	
	if(fh != INVALID_HANDLE_VALUE && fail_if_exist) 
	{
		_CloseHandle(fh);
		return null;
	}
	_CloseHandle(fh);
	fh = _CreateFile(file_name,GENERIC_WRITE,0,CREATE_NEW);	
	if(fh == INVALID_HANDLE_VALUE) return null;
	_CloseHandle(fh);
	return OpenIniFile(file_name);	
}

INIFILE * FILE_SERVICE::OpenIniFile(const char * file_name)
{
	//GUARD(FILE_SERVICE::OpenIniFile)
	INIFILE_T * inifile_T;
	dword n;
//	PUSH_CONTROL(0,0,0)	// core control 

	for(n=0;n<=Max_File_Index;n++)
	{
		if( OpenFiles[n]==null || OpenFiles[n]->GetFileName()==null ) continue;
		if(stricmp(OpenFiles[n]->GetFileName(),file_name) == 0) 
		{
			OpenFiles[n]->IncReference();

			inifile_T = NEW INIFILE_T(OpenFiles[n]);
			if(inifile_T == null) THROW;
//			POP_CONTROL(0)
			return inifile_T;
		}
	}

	for(n=0;n<_MAX_OPEN_INI_FILES;n++)
	{
		if(OpenFiles[n] != null) continue;

		OpenFiles[n] = NEW IFS(this);
		if(OpenFiles[n] == null) THROW;//(FILE_SERVICE::OpenIniFile : no mem A);
		if(!OpenFiles[n]->LoadFile(file_name)) 
		{
			delete OpenFiles[n];
			OpenFiles[n] = null;
			return null;
		}
		if(Max_File_Index < n) Max_File_Index = n;
		OpenFiles[n]->IncReference();
//		POP_CONTROL(0)
		// INIFILE_T object belonged to entity and must be deleted by entity
		//OpenFiles[n]->inifile_T = new INIFILE_T(OpenFiles[n]);	
		//if(OpenFiles[n]->inifile_T == null) THROW;
		//return OpenFiles[n]->inifile_T;


		inifile_T = NEW INIFILE_T(OpenFiles[n]);
		if(inifile_T == null) THROW;
		return inifile_T;
	}
//	POP_CONTROL(0)
	//UNGUARD
	return null;
}

void FILE_SERVICE::RefDec(INIFILE * ini_obj)
{
	GUARD(FILE_SERVICE::RefDec)
	dword n;
	for(n=0;n<=Max_File_Index;n++)
	{
		if(OpenFiles[n] != ini_obj) continue;
		//OpenFiles[n]->SearchData = &OpenFiles[n]->Search;
		if(OpenFiles[n]->GetReference() == 0) _THROW(Reference error);
		OpenFiles[n]->DecReference();
		if(OpenFiles[n]->GetReference() == 0) 
		{
			delete OpenFiles[n];
			OpenFiles[n] = null;
		}
		return;
	}
	_THROW(bad inifile object);
	UNGUARD
}

void FILE_SERVICE::Close()
{
	dword n;
	for(n=0;n<_MAX_OPEN_INI_FILES;n++)
	{
		if(OpenFiles[n] == null) continue;
		delete OpenFiles[n];
		OpenFiles[n] = null;
	}
}

BOOL FILE_SERVICE::LoadFile(const char * file_name, char * * ppBuffer, dword * dwSize)
{
	if (ppBuffer == 0) return false;

	HANDLE hFile = _CreateFile(file_name);
	if (INVALID_HANDLE_VALUE == hFile) return false;
	dword dwLowSize = _GetFileSize(hFile,0);
	if (dwSize) *dwSize = dwLowSize;
	if (dwLowSize==0) 
	{
		*ppBuffer = null;
		return false;
	}

	*ppBuffer = NEW char[dwLowSize];
	_ReadFile(hFile,*ppBuffer,dwLowSize,0);
	_CloseHandle(hFile);
	return true;
}

BOOL FILE_SERVICE::SetDrive(const char * pDriveName)
{
#ifdef _XBOX
	if(pDriveName == 0)
	{
		strcpy(sDriveLetter,XBOXDRIVE_DVD);
		return true;
	}
	if(!pDriveName[0]) 
	{
		sDriveLetter[0] = 0;
		return true;
	}
	if(strlen(pDriveName) > 4) return false;
	strcpy(sDriveLetter,pDriveName);
	return true;
#else
	return false;
#endif
}





#ifdef _XBOX

bool XDirCopy(const char *_srcDir, const char *_destDir, const char *_mask)
{

	if(bCacheEstDirsStructure)
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
	char *srcFilename = new char[strlen(_srcDir)+strlen(_mask)+2];
	if (!srcFilename)
		return false;

	strcpy(srcFilename, _srcDir);
	if (_srcDir[strlen(_srcDir)-1] != '\\')
		strcat(srcFilename, "\\");
	strcat(srcFilename, _mask);

	bool bRes;
	bRes = CreateDirectory(_destDir, 0)==TRUE;
	
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

bool XDirDelete(const char *_srcDir)
{
	WIN32_FIND_DATA findData;
	char _mask[] = "*.*";
	char *srcFilename = new char[strlen(_srcDir)+strlen(_mask)+2];
	if (!srcFilename)
		return false;

	strcpy(srcFilename, _srcDir);
	if (_srcDir[strlen(_srcDir)-1] != '\\')
		strcat(srcFilename, "\\");
	strcat(srcFilename, _mask);

	//bool bRes;
	//bRes = CreateDirectory(_destDir, 0)==TRUE;
	
	HANDLE findHandle = FindFirstFile(srcFilename, &findData);
	if (findHandle == INVALID_HANDLE_VALUE)
	{
		delete[] srcFilename;
		RemoveDirectory(_srcDir);
		return false;
	}

	XProcessFileDelete(_srcDir, _mask, findData);
	while (FindNextFile(findHandle, &findData) == TRUE)
	{
		XProcessFileDelete(_srcDir, _mask, findData);
	}

	delete[] srcFilename;
	FindClose(findHandle);
	RemoveDirectory(_srcDir);
	return true;
}

bool XFileCopy(const char *_srcName, const char *_destName)
{
	bool bRes;

	if(bCacheEstDirsStructure) return true; // only structure of directories is created in this case

	if(dwCacheFiles >= 100)
	{
		dwCacheFiles = 0;
		if(pDevice)
		{
			dwCacheScreenN++;
			if(dwCacheScreenN > 8) dwCacheScreenN = 0;
			pDevice->RunStart();
			pDevice->ImageBlt(nCacheFonTexId);
			pDevice->ImageBlt(nCacheProgressCacheId[dwCacheScreenN],&CacheScreenRect);
			pDevice->RunEnd();
		}
	} else dwCacheFiles++;

	//bRes = CopyFile(_srcName, _destName, TRUE)!=0;
	bRes = CopyFile(_srcName, _destName, !bCacheOverwrite)!=0;
	
	return bRes;
	//return (CopyFile(_srcName, _destName, TRUE) != 0);
}

bool XFileDelete(const char *_fileName)
{
	bool bRes;
	bRes = DeleteFile(_fileName)!=0;
	return bRes;
}

bool XProcessFile(const char *_srcDir, const char *_destDir, const char *_mask, const WIN32_FIND_DATA &_findData)
{
	if (!strcmp(_findData.cFileName, "."))
		return false;
	if (!strcmp(_findData.cFileName, ".."))
		return false;

	char *srcName = new char[strlen(_srcDir)+strlen(_findData.cFileName)+2];
	char *destName = new char[strlen(_destDir)+strlen(_findData.cFileName)+2];
	
	strcpy(srcName, _srcDir);
	if (_srcDir[strlen(_srcDir)-1] != '\\')
		strcat(srcName, "\\");
	strcat(srcName, _findData.cFileName);

	strcpy(destName, _destDir);
	if (_destDir[strlen(_destDir)-1] != '\\')
		strcat(destName, "\\");
	strcat(destName, _findData.cFileName);

	if (!(_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{ //file
		XFileCopy(srcName, destName);
	}
	else
	{ //directory
		XDirCopy(srcName, destName, _mask);
	}

	delete[] srcName;
	delete[] destName;

	return true;
}


bool XProcessFileDelete(const char *_srcDir, const char *_mask, const WIN32_FIND_DATA &_findData)
{
	if (!strcmp(_findData.cFileName, "."))
		return false;
	if (!strcmp(_findData.cFileName, ".."))
		return false;

	char *srcName = new char[strlen(_srcDir)+strlen(_findData.cFileName)+2];
	
	strcpy(srcName, _srcDir);
	if (_srcDir[strlen(_srcDir)-1] != '\\')
		strcat(srcName, "\\");
	strcat(srcName, _findData.cFileName);

	if (!(_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{ //file
		XFileDelete(srcName);
	}
	else
	{ //directory
		XDirDelete(srcName);
	}

	delete[] srcName;
	return true;
}

#endif

DWORD FILE_SERVICE::MakeHashValue(const char * string)
{
	DWORD hval = 0;
	DWORD g;
	char v;
	while(*string != 0)
	{
		v = *string++;
		if ('A' <= v && v <= 'Z') v += 'a' - 'A';	// case independent
		hval = (hval<<4) + (unsigned long int)v;
		g = hval & ((unsigned long int) 0xf << (32 - 4));
		if(g != 0)
		{
			hval ^= g >> (32 - 8);
			hval ^= g;
		}
	}
	return hval;
}


BOOL FILE_SERVICE::CacheDirectory(const char * pDirName)
{
#ifndef _XBOX
	return false;
#else
	DWORD dwHashIndex;
	DWORD n;

	char sDNSrc[MAX_PATH];
	char sDNDst[MAX_PATH];
	char sDirectoryName[MAX_PATH];

	strcpy(sDirectoryName,pDirName);

	dwHashIndex = MakeHashValue(pDirName)%CACHEDIRSTABLESIZE;
	for(n=0;n<CacheDirs[dwHashIndex].dwElementsNum;n++)
	{
		if(stricmp(CacheDirs[dwHashIndex].pDirName[n],pDirName)==0) return true; // already cached
	}
	CacheDirs[dwHashIndex].dwElementsNum++;
	CacheDirs[dwHashIndex].pDirName = (char **)RESIZE(CacheDirs[dwHashIndex].pDirName,CacheDirs[dwHashIndex].dwElementsNum*sizeof(char *));
	CacheDirs[dwHashIndex].pDirName[n] = (char *)NEW char[strlen(pDirName) + 1];
	strcpy(CacheDirs[dwHashIndex].pDirName[n],pDirName);
	
	// Copy Directory

	// verify directory path 
	n = 0;
	while(sDirectoryName[n])
	{
		if(sDirectoryName[n] == '\\')
		{
			sDirectoryName[n] = 0;
			strcpy(sDNDst,XBOXDRIVE_CACHE);
			strcat(sDNDst,sDirectoryName);
			bool bRes = CreateDirectory(sDNDst, 0)==TRUE;
			sDirectoryName[n] = '\\';
		}
		n++;
	}
	strcpy(sDNSrc,XBOXDRIVE_DVD);
	strcat(sDNSrc,sDirectoryName);

	strcpy(sDNDst,XBOXDRIVE_CACHE);
	strcat(sDNDst,sDirectoryName);

	XDirCopy(sDNSrc,sDNDst,"*.*");

	

	return true;
#endif
}

void FILE_SERVICE::MarkDirectoryCached(const char * pDirName)
{
#ifdef _XBOX
	DWORD dwLen;
	DWORD dwHashIndex;
	DWORD n;
	char * pTemp;

	if(!pDirName) return;

	pTemp = 0;

	dwLen = strlen(pDirName);
	if(dwLen == 0) return;

	if(pDirName[dwLen-1]== '\\')
	{

		dwHashIndex = MakeHashValue(pDirName)%CACHEDIRSTABLESIZE;
		for(n=0;n<CacheDirs[dwHashIndex].dwElementsNum;n++)
		{
			if(stricmp(CacheDirs[dwHashIndex].pDirName[n],pDirName)==0) return; // already marked
		}
	}
	else
	{
		
		pTemp = (char *)NEW char[dwLen + 2];
		strcpy(pTemp,pDirName);
		strcat(pTemp,"\\");
		dwHashIndex = MakeHashValue(pTemp)%CACHEDIRSTABLESIZE;
		for(n=0;n<CacheDirs[dwHashIndex].dwElementsNum;n++)
		{
			if(stricmp(CacheDirs[dwHashIndex].pDirName[n],pTemp)==0)
			{
				delete pTemp;
				return; // already marked
			}
		}
		//delete pTemp;
	}
	
	CacheDirs[dwHashIndex].dwElementsNum++;
	CacheDirs[dwHashIndex].pDirName = (char **)RESIZE(CacheDirs[dwHashIndex].pDirName,CacheDirs[dwHashIndex].dwElementsNum*sizeof(char *));

	if(pTemp)
	{
		CacheDirs[dwHashIndex].pDirName[n] = pTemp;
	}
	else
	{
		CacheDirs[dwHashIndex].pDirName[n] = (char *)NEW char[strlen(pDirName) + 1];
		strcpy(CacheDirs[dwHashIndex].pDirName[n],pDirName);
	}

	/*if(pDirName[dwLen]== '\\')
	{
		CacheDirs[dwHashIndex].pDirName[n] = (char *)NEW char[strlen(pDirName) + 1];
		strcpy(CacheDirs[dwHashIndex].pDirName[n],pDirName);
	}
	else
	{
		CacheDirs[dwHashIndex].pDirName[n] = (char *)NEW char[strlen(pDirName) + 2];
		strcpy(CacheDirs[dwHashIndex].pDirName[n],pDirName);
		strcat(CacheDirs[dwHashIndex].pDirName[n],"\\");
	}*/
	//trace("cached dir: %s",CacheDirs[dwHashIndex].pDirName[n]);
	
#endif
}

BOOL FILE_SERVICE::UnCacheDirectory(const char * pDirName)
{
#ifndef _XBOX
	return false;
#else
	bool bFound;
	char sDNDst[MAX_PATH];
	DWORD dwHashIndex;
	DWORD n;

	dwHashIndex = MakeHashValue(pDirName)%CACHEDIRSTABLESIZE;
	bFound = false;
	for(n=0;n<CacheDirs[dwHashIndex].dwElementsNum;n++)
	{
		if(stricmp(CacheDirs[dwHashIndex].pDirName[n],pDirName)==0)
		{
			bFound = true;
			break;
		}
	}
	if(!bFound) return true;	// directiry isnt cached

	if(CacheDirs[dwHashIndex].dwElementsNum == 0)
	{
		trace("cache error"); return false;
	}
	
	delete CacheDirs[dwHashIndex].pDirName[n];

	if((CacheDirs[dwHashIndex].dwElementsNum - 1)>n)
	{
		
		CacheDirs[dwHashIndex].pDirName[n] = CacheDirs[dwHashIndex].pDirName[CacheDirs[dwHashIndex].dwElementsNum-1]; 
	}
	CacheDirs[dwHashIndex].dwElementsNum--;
	CacheDirs[dwHashIndex].pDirName = (char **)RESIZE(CacheDirs[dwHashIndex].pDirName,CacheDirs[dwHashIndex].dwElementsNum*sizeof(char *));

	// Remove Directory

	strcpy(sDNDst,XBOXDRIVE_CACHE);
	strcat(sDNDst,pDirName);

	XDirDelete(sDNDst);
	
	return true;

#endif

}

BOOL FILE_SERVICE::IsCached(const char * pFileName)
{
	char sDirName[MAX_PATH];
	DWORD dwLen;
	DWORD dwHashIndex;
	long  n;
	DWORD i;

	dwLen = strlen(pFileName);
	strcpy(sDirName,pFileName);

	for(n=dwLen;n>=0;n--)
	{
		if(sDirName[n] == '\\')
		{
			dwHashIndex = MakeHashValue(sDirName)%CACHEDIRSTABLESIZE;
			for(i=0;i<CacheDirs[dwHashIndex].dwElementsNum;i++)
			{
				if(stricmp(CacheDirs[dwHashIndex].pDirName[i],sDirName)==0) return true;
			}
			return false;
		}
		sDirName[n] = 0;
	}
	return false;
}

//=================================================================================================
INIFILE_T::~INIFILE_T()
{
	File_Service.RefDec(ifs_PTR);
}

void INIFILE_T::AddString(char * section_name, char * key_name, char * string)
{
	ifs_PTR->AddString(section_name,key_name,string);
}
// write string to file, overwrite data if exist, throw EXS exception object if failed
void INIFILE_T::WriteString(char * section_name, char * key_name, char * string)
{
	ifs_PTR->WriteString(section_name,key_name,string);
}
// write long value of key in pointed section if section and key exist, throw EXS object otherwise
void INIFILE_T::WriteLong(char * section_name, char * key_name, long value)
{
	ifs_PTR->WriteLong(section_name,key_name,value);
}

// write double value of key in pointed section if section and key exist, throw EXS object otherwise
void INIFILE_T::WriteDouble(char * section_name, char * key_name,double value)
{
	ifs_PTR->WriteDouble(section_name,key_name,value);
}

// fill buffer with key value, throw EXS exception object if failed or if section or key doesnt exist
void INIFILE_T::ReadString(char * section_name, char * key_name, char * buffer, dword buffer_size)
{
	ifs_PTR->ReadString(&Search,section_name,key_name,buffer,buffer_size);
}

// fill buffer with key value if section and key exist, otherwise fill with def_string and return false
bool INIFILE_T::ReadString(char * section_name, char * key_name, char * buffer, dword buffer_size, char * def_string)
{
	return ifs_PTR->ReadString(&Search, section_name,key_name,buffer,buffer_size,def_string);
}

// continue search from key founded in previous call this function or to function ReadString
// fill buffer with key value if section and key exist, otherwise return false
bool INIFILE_T::ReadStringNext(char * section_name, char * key_name, char * buffer, dword buffer_size)
{
	return ifs_PTR->ReadStringNext(&Search, section_name,key_name,buffer,buffer_size);
}

// return long value of key in pointed section if section and key exist, throw EXS object otherwise
long INIFILE_T::GetLong(char * section_name, char * key_name)
{
	return ifs_PTR->GetLong(&Search, section_name,key_name);
}

// return long value of key in pointed section if section and key exist, if not - return def_value
long INIFILE_T::GetLong(char * section_name, char * key_name, long def_val)
{
	return ifs_PTR->GetLong(&Search, section_name,key_name,def_val);
}

// return double value of key in pointed section if section and key exist, throw EXS object otherwise
double INIFILE_T::GetDouble(char * section_name, char * key_name)
{
	return ifs_PTR->GetDouble(&Search, section_name,key_name);
}

// return double value of key in pointed section if section and key exist, if not - return def_value
double INIFILE_T::GetDouble(char * section_name, char * key_name, double def_val)
{
	return ifs_PTR->GetDouble(&Search, section_name,key_name,def_val);
}

bool INIFILE_T::GetLongNext(char * section_name, char * key_name, long * val)
{
	return ifs_PTR->GetLongNext(&Search, section_name,key_name,val);
}

bool INIFILE_T::GetDoubleNext(char * section_name, char * key_name, double * val)
{
	return ifs_PTR->GetDoubleNext(&Search, section_name,key_name,val);
}


// return double value of key in pointed section if section and key exist, throw EXS object otherwise
float INIFILE_T::GetFloat(char * section_name, char * key_name)
{
	return ifs_PTR->GetFloat(&Search, section_name,key_name);
}

// return float value of key in pointed section if section and key exist, if not - return def_value
float INIFILE_T::GetFloat(char * section_name, char * key_name, float def_val)
{
	return ifs_PTR->GetFloat(&Search, section_name,key_name,def_val);
}
bool INIFILE_T::GetFloatNext(char * section_name, char * key_name, float * val)
{
	return ifs_PTR->GetFloatNext(&Search, section_name,key_name,val);
}


void INIFILE_T::DeleteKey(char * section_name, char * key_name)
{
	ifs_PTR->DeleteKey(section_name,key_name);
}


void INIFILE_T::DeleteKey(char * section_name, char * key_name, char * key_value)
{
	ifs_PTR->DeleteKey(section_name,key_name,key_value);
}


bool INIFILE_T::TestKey(char * section_name, char * key_name, char * key_value)
{
	return ifs_PTR->TestKey(section_name,key_name,key_value);
}

void INIFILE_T::DeleteSection(char * section_name)
{
	ifs_PTR->DeleteSection(section_name);
}

bool INIFILE_T::GetSectionName(char * section_name_buffer, long buffer_size) 
{
	return ifs_PTR->GetSectionName(section_name_buffer,buffer_size);
}

bool INIFILE_T::GetSectionNameNext(char * section_name_buffer, long buffer_size) 
{
	return ifs_PTR->GetSectionNameNext(section_name_buffer,buffer_size);
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

bool INIFILE_T::TestSection(char * section_name)
{
	return ifs_PTR->TestSection(section_name);
};