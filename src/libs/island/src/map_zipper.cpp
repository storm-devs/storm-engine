#include "island.h"
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

    auto fileS = fio->_CreateFile(sFileName.c_str(), std::ios::binary | std::ios::in);
    if (!fileS.is_open())
    {
        return false;
    }
    fio->_ReadFile(fileS, &dwSizeX, sizeof(dwSizeX));
    fio->_ReadFile(fileS, &dwDX, sizeof(dwDX));
    fio->_ReadFile(fileS, &dwBlockSize, sizeof(dwBlockSize));
    fio->_ReadFile(fileS, &dwBlockShift, sizeof(dwBlockShift));
    fio->_ReadFile(fileS, &dwShiftNumBlocksX, sizeof(dwShiftNumBlocksX));
    fio->_ReadFile(fileS, &dwNumRealBlocks, sizeof(dwNumRealBlocks));
    pWordTable = new uint16_t[dwDX * dwDX];
    fio->_ReadFile(fileS, pWordTable, sizeof(uint16_t) * dwDX * dwDX);
    pRealData = static_cast<uint8_t *>(malloc(dwNumRealBlocks * dwBlockSize * dwBlockSize));
    fio->_ReadFile(fileS, pRealData, sizeof(uint8_t) * dwNumRealBlocks * dwBlockSize * dwBlockSize);
    fio->_CloseFile(fileS);
    return true;
}

bool MapZipper::Save(std::string sFileName)
{
    auto fileS = fio->_CreateFile(sFileName.c_str(), std::ios::binary | std::ios::out);
    if (!fileS.is_open())
    {
        return false;
    }
    fio->_WriteFile(fileS, &dwSizeX, sizeof(dwSizeX));
    fio->_WriteFile(fileS, &dwDX, sizeof(dwDX));
    fio->_WriteFile(fileS, &dwBlockSize, sizeof(dwBlockSize));
    fio->_WriteFile(fileS, &dwBlockShift, sizeof(dwBlockShift));
    fio->_WriteFile(fileS, &dwShiftNumBlocksX, sizeof(dwShiftNumBlocksX));
    fio->_WriteFile(fileS, &dwNumRealBlocks, sizeof(dwNumRealBlocks));
    fio->_WriteFile(fileS, pWordTable, sizeof(uint16_t) * dwDX * dwDX);
    fio->_WriteFile(fileS, pRealData, sizeof(uint8_t) * dwNumRealBlocks * dwBlockSize * dwBlockSize);
    fio->_CloseFile(fileS);
    return true;
}
