#include "ISLAND.h"
#include "tga.h"
#include "vfile_service.h"

bool ISLAND::DoZapSuperGeneratorDecodeFile(const char *sname)
{
    auto *const hFile = fio->_CreateFile(sname, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        TGA_H tga_head;

        fio->_ReadFile(hFile, &tga_head, sizeof(tga_head), nullptr);
        const uint32_t dwSize = tga_head.width;
        auto *pTempMap = new uint8_t[dwSize * dwSize];
        fio->_ReadFile(hFile, pTempMap, dwSize * dwSize, nullptr);
        fio->_CloseHandle(hFile);

        mzShadow.DoZip(pTempMap, dwSize);
        mzShadow.Save(std::string(sname) + ".zap");
        STORM_DELETE(pTempMap);
    }

    return true;
}

void ISLAND::DoZapSuperGeneratorInnerDecodeFiles(const char *sub_dir, const char *mask)
{
    char file_mask[256];
    WIN32_FIND_DATA wfd;

    sprintf_s(file_mask, "resource\\foam\\%s%s%s", (sub_dir) ? sub_dir : "", (sub_dir) ? "\\" : "", "*.*");

    auto *hFile = fio->_FindFirstFile(file_mask, &wfd);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (wfd.cFileName[0] == L'.')
                    continue;

                std::string FileName = utf8::ConvertWideToUtf8(wfd.cFileName);
                sprintf(file_mask, "%s%s%s", (sub_dir) ? sub_dir : "", (sub_dir) ? "\\" : "", FileName.c_str());
                DoZapSuperGeneratorInnerDecodeFiles(file_mask, mask);
            }
        } while (fio->_FindNextFile(hFile, &wfd));
        FindClose(hFile);
    }

    sprintf_s(file_mask, "resource\\foam\\%s%s%s", (sub_dir) ? sub_dir : "", (sub_dir) ? "\\" : "", mask);

    hFile = fio->_FindFirstFile(file_mask, &wfd);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            std::string FileName = utf8::ConvertWideToUtf8(wfd.cFileName);
            sprintf(file_mask, "resource\\foam\\%s\\%s", (sub_dir) ? sub_dir : "", FileName.c_str());
            DoZapSuperGeneratorDecodeFile(file_mask);
        } while (fio->_FindNextFile(hFile, &wfd));
        FindClose(hFile);
    }
}

void ISLAND::DoZapSuperGenerator()
{
    DoZapSuperGeneratorInnerDecodeFiles(nullptr, "*.tga");
}