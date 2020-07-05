#include "Island.h"

MapZipper::MapZipper()
{
	pWordTable = null;
	pRealData = null;
}

MapZipper::~MapZipper()
{
	UnInit();
}

void MapZipper::UnInit()
{
	DELETE(pWordTable);
	DELETE(pRealData);
}

void MapZipper::DoZip(byte * pSrc, dword _dwSizeX)
{
	dword	i, j, k, x, y, xx, yy;

	UnInit();

	dword	dwRealIndex = 0;

	dwSizeX = _dwSizeX;

	dwBlockSize = 8;
	dwBlockShift = Number2Shift(dwBlockSize);

	dwDX = dwSizeX >> dwBlockShift;

	dwShiftNumBlocksX = Number2Shift(dwDX);
	
	pWordTable = NEW word[dwDX * dwDX];
	pRealData = NEW byte[dwSizeX * dwSizeX];
	for (i=0; i<dwDX * dwDX; i++)
	{
		y = i / dwDX;
		x = i - y * dwDX;
		dword dwStart = (y << dwBlockShift) * dwSizeX + (x << dwBlockShift);
		
		bool bTest = true;
		byte byTest;
		for (j=0; j<dwBlockSize * dwBlockSize; j++)
		{
			yy = j >> dwBlockShift;
			xx = j - (yy << dwBlockShift);
			byte byRes = pSrc[dwStart + yy * dwSizeX + xx];
			if (j == 0) byTest = byRes;
			if (byTest != byRes)
			{
				bTest = false;
				pWordTable[i] = word(dwRealIndex);
				for (k=0; k<dwBlockSize * dwBlockSize; k++)
				{
					yy = k >> dwBlockShift;
					xx = k - (yy << dwBlockShift);
					pRealData[dwRealIndex * dwBlockSize * dwBlockSize + k] = pSrc[dwStart + yy * dwSizeX + xx];
				}
				dwRealIndex++;
				break;
			}
		}
		if (bTest)
			pWordTable[i] = word(0x8000) | word(byTest);
	}
	dwNumRealBlocks = dwRealIndex;
	pRealData = (byte*)RESIZE(pRealData, dwRealIndex * dwBlockSize * dwBlockSize);

	for (y=0; y<_dwSizeX; y++)
		for (x=0; x<_dwSizeX; x++)
		{
			if (Get(x,y) != pSrc[x + y * _dwSizeX])
				_asm int 3
		}
}

byte MapZipper::Get(dword dwX, dword dwY)
{
	if (!pWordTable) return 255;
	word wRes = pWordTable[((dwY >> dwBlockShift) << dwShiftNumBlocksX) + (dwX >> dwBlockShift)];
	if (wRes & 0x8000) return byte(wRes & 0xFF);
	dword x = dwX - ((dwX >> dwBlockShift) << dwBlockShift);
	dword y = dwY - ((dwY >> dwBlockShift) << dwBlockShift);

	byte byRes = pRealData[((dword(wRes) << dwBlockShift) << dwBlockShift) + (y << dwBlockShift) + x];

	return byRes;
}

bool MapZipper::Load(string sFileName)
{
	UnInit();

	HANDLE hFile = fio->_CreateFile(sFileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
	if (hFile == INVALID_HANDLE_VALUE) return false;
	fio->_ReadFile(hFile, &dwSizeX, sizeof(dwSizeX), null);
	fio->_ReadFile(hFile, &dwDX, sizeof(dwDX), null);
	fio->_ReadFile(hFile, &dwBlockSize, sizeof(dwBlockSize), null);
	fio->_ReadFile(hFile, &dwBlockShift, sizeof(dwBlockShift), null);
	fio->_ReadFile(hFile, &dwShiftNumBlocksX, sizeof(dwShiftNumBlocksX), null);
	fio->_ReadFile(hFile, &dwNumRealBlocks, sizeof(dwNumRealBlocks), null);
	pWordTable = NEW word[dwDX * dwDX];
	fio->_ReadFile(hFile, pWordTable, sizeof(word) * dwDX * dwDX, null);
	pRealData = NEW byte[dwNumRealBlocks * dwBlockSize * dwBlockSize];
	fio->_ReadFile(hFile, pRealData, sizeof(byte) * dwNumRealBlocks * dwBlockSize * dwBlockSize, null);
	fio->_CloseHandle(hFile);
	return true;
}

bool MapZipper::Save(string sFileName)
{
	HANDLE hFile = fio->_CreateFile(sFileName, GENERIC_WRITE, FILE_SHARE_READ, OPEN_ALWAYS);
	if (hFile == INVALID_HANDLE_VALUE) return false;
	fio->_WriteFile(hFile, &dwSizeX, sizeof(dwSizeX), null);
	fio->_WriteFile(hFile, &dwDX, sizeof(dwDX), null);
	fio->_WriteFile(hFile, &dwBlockSize, sizeof(dwBlockSize), null);
	fio->_WriteFile(hFile, &dwBlockShift, sizeof(dwBlockShift), null);
	fio->_WriteFile(hFile, &dwShiftNumBlocksX, sizeof(dwShiftNumBlocksX), null);
	fio->_WriteFile(hFile, &dwNumRealBlocks, sizeof(dwNumRealBlocks), null);
	fio->_WriteFile(hFile, pWordTable, sizeof(word) * dwDX * dwDX, null);
	fio->_WriteFile(hFile, pRealData, sizeof(byte) * dwNumRealBlocks * dwBlockSize * dwBlockSize, null);
	fio->_CloseHandle(hFile);
	return true;
}