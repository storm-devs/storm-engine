#include "Island.h"

bool NetIsland::DoZapSuperGeneratorDecodeFile(char * sname)
{
	HANDLE hFile = fio->_CreateFile(sname, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		TGA_H	tga_head;

		fio->_ReadFile(hFile, &tga_head, sizeof(tga_head), 0);
		dword dwSize = tga_head.width;
		byte * pTempMap = NEW byte[dwSize * dwSize];
		fio->_ReadFile(hFile, pTempMap, dwSize * dwSize, 0);
		fio->_CloseHandle(hFile);

		mzShadow.DoZip(pTempMap, dwSize);
		mzShadow.Save(string(sname) + ".zap");
		DELETE(pTempMap);
	}

	return true;
}

void NetIsland::DoZapSuperGeneratorInnerDecodeFiles(char * sub_dir, char * mask)
{
	char				file_mask[256];
	WIN32_FIND_DATA		wfd;

	sprintf(file_mask,"resource\\foam\\%s%s%s",(sub_dir) ? sub_dir : "", (sub_dir) ? "\\" : "", "*.*");

	HANDLE hFile = FindFirstFile(file_mask, &wfd);
	if (hFile != INVALID_HANDLE_VALUE) 
	{
		do 
		{ 
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (wfd.cFileName[0] == '.') continue;
				sprintf(file_mask,"%s%s%s",(sub_dir) ? sub_dir : "", (sub_dir) ? "\\" : "", wfd.cFileName);
				DoZapSuperGeneratorInnerDecodeFiles(file_mask, mask);
			}
		} while (FindNextFile(hFile,&wfd));
		FindClose(hFile);
	}

	sprintf(file_mask,"resource\\foam\\%s%s%s",(sub_dir) ? sub_dir : "", (sub_dir) ? "\\" : "", mask);

	hFile = FindFirstFile(file_mask, &wfd);
	if (hFile != INVALID_HANDLE_VALUE) 
	{
		do 
		{ 
			sprintf(file_mask,"resource\\foam\\%s\\%s",(sub_dir) ? sub_dir : "",wfd.cFileName);
			DoZapSuperGeneratorDecodeFile(file_mask); 
		} while (FindNextFile(hFile,&wfd));
		FindClose(hFile);
	} 
}

void NetIsland::DoZapSuperGenerator()
{
	DoZapSuperGeneratorInnerDecodeFiles(0, "*.tga");
}
