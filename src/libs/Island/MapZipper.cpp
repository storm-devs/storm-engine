#include "ISLAND.h"
#include "inlines.h"

MapZipper::MapZipper()
{
    pWordTable = nullptr;
    pRealData = nullptr;
}

MapZipper::~MapZipper()
{
    UnInit();
}

void MapZipper::UnInit()
{
    STORM_DELETE(pWordTable);
    free(pRealData);
}

void MapZipper::DoZip(uint8_t *pSrc, uint32_t _dwSizeX)
{
    uint32_t i, j, k, x, y, xx, yy;

    UnInit();

    uint32_t dwRealIndex = 0;

    dwSizeX = _dwSizeX;

    dwBlockSize = 8;
    dwBlockShift = Number2Shift(dwBlockSize);

    dwDX = dwSizeX >> dwBlockShift;

    dwShiftNumBlocksX = Number2Shift(dwDX);

    pWordTable = new uint16_t[dwDX * dwDX];
    pRealData = static_cast<uint8_t *>(malloc(dwSizeX * dwSizeX));
    for (i = 0; i < dwDX * dwDX; i++)
    {
        y = i / dwDX;
        x = i - y * dwDX;
        const auto dwStart = (y << dwBlockShift) * dwSizeX + (x << dwBlockShift);

        auto bTest = true;
        uint8_t byTest;
        for (j = 0; j < dwBlockSize * dwBlockSize; j++)
        {
            yy = j >> dwBlockShift;
            xx = j - (yy << dwBlockShift);
            const auto byRes = pSrc[dwStart + yy * dwSizeX + xx];
            if (j == 0)
                byTest = byRes;
            if (byTest != byRes)
            {
                bTest = false;
                pWordTable[i] = static_cast<uint16_t>(dwRealIndex);
                for (k = 0; k < dwBlockSize * dwBlockSize; k++)
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
            pWordTable[i] = static_cast<uint16_t>(0x8000) | static_cast<uint16_t>(byTest);
    }
    dwNumRealBlocks = dwRealIndex;
    pRealData = static_cast<uint8_t *>(realloc(pRealData, dwRealIndex * dwBlockSize * dwBlockSize));

    for (y = 0; y < _dwSizeX; y++)
        for (x = 0; x < _dwSizeX; x++)
        {
            if (Get(x, y) != pSrc[x + y * _dwSizeX])
                __debugbreak();
        }
}

uint8_t MapZipper::Get(uint32_t dwX, uint32_t dwY)
{
    if (!pWordTable)
        return 255;
    const auto wRes = pWordTable[((dwY >> dwBlockShift) << dwShiftNumBlocksX) + (dwX >> dwBlockShift)];
    if (wRes & 0x8000)
        return static_cast<uint8_t>(wRes & 0xFF);
    const auto x = dwX - ((dwX >> dwBlockShift) << dwBlockShift);
    const auto y = dwY - ((dwY >> dwBlockShift) << dwBlockShift);

    const auto byRes =
        pRealData[((static_cast<uint32_t>(wRes) << dwBlockShift) << dwBlockShift) + (y << dwBlockShift) + x];

    return byRes;
}

bool MapZipper::Load(std::string sFileName)
{
    UnInit();

    auto *const hFile = fio->_CreateFile(sFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;
    fio->_ReadFile(hFile, &dwSizeX, sizeof(dwSizeX), nullptr);
    fio->_ReadFile(hFile, &dwDX, sizeof(dwDX), nullptr);
    fio->_ReadFile(hFile, &dwBlockSize, sizeof(dwBlockSize), nullptr);
    fio->_ReadFile(hFile, &dwBlockShift, sizeof(dwBlockShift), nullptr);
    fio->_ReadFile(hFile, &dwShiftNumBlocksX, sizeof(dwShiftNumBlocksX), nullptr);
    fio->_ReadFile(hFile, &dwNumRealBlocks, sizeof(dwNumRealBlocks), nullptr);
    pWordTable = new uint16_t[dwDX * dwDX];
    fio->_ReadFile(hFile, pWordTable, sizeof(uint16_t) * dwDX * dwDX, nullptr);
    pRealData = static_cast<uint8_t *>(malloc(dwNumRealBlocks * dwBlockSize * dwBlockSize));
    fio->_ReadFile(hFile, pRealData, sizeof(uint8_t) * dwNumRealBlocks * dwBlockSize * dwBlockSize, nullptr);
    fio->_CloseHandle(hFile);
    return true;
}

bool MapZipper::Save(std::string sFileName)
{
    auto *const hFile = fio->_CreateFile(sFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, OPEN_ALWAYS);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;
    fio->_WriteFile(hFile, &dwSizeX, sizeof(dwSizeX), nullptr);
    fio->_WriteFile(hFile, &dwDX, sizeof(dwDX), nullptr);
    fio->_WriteFile(hFile, &dwBlockSize, sizeof(dwBlockSize), nullptr);
    fio->_WriteFile(hFile, &dwBlockShift, sizeof(dwBlockShift), nullptr);
    fio->_WriteFile(hFile, &dwShiftNumBlocksX, sizeof(dwShiftNumBlocksX), nullptr);
    fio->_WriteFile(hFile, &dwNumRealBlocks, sizeof(dwNumRealBlocks), nullptr);
    fio->_WriteFile(hFile, pWordTable, sizeof(uint16_t) * dwDX * dwDX, nullptr);
    fio->_WriteFile(hFile, pRealData, sizeof(uint8_t) * dwNumRealBlocks * dwBlockSize * dwBlockSize, nullptr);
    fio->_CloseHandle(hFile);
    return true;
}
