#pragma once

#include <string>

#include "core.h"

class CSaveLoad
{
  private:
    char *pSaveBuffer;
    uint32_t dwCurSize, dwMaxSize;
    bool bSave, bLoad;

  public:
    CSaveLoad()
    {
        dwCurSize = 0;
        dwMaxSize = 0;
        bSave = bLoad = false;
        pSaveBuffer = nullptr;
    }

    ~CSaveLoad()
    {
        Close();
    }

    void Close() const
    {
        if (bSave)
        {
            const auto size = dwCurSize * 2 + 8 + 1;
            auto *const pFFSave = new char[size];
            sprintf_s(pFFSave, size, "%.8x", dwCurSize);
            for (uint32_t i = 0; i < dwCurSize; i++)
                sprintf_s(&pFFSave[8 + i * 2], 3, "%.2x", static_cast<uint8_t>(pSaveBuffer[i]));

            auto *pV = core.Event("SeaLoad_GetPointer", "sl", "seasave", -1);
            if (pV)
                pV->GetAClass()->SetAttribute("save", pFFSave);

            delete[] pFFSave;
        }
        free(pSaveBuffer);
    }

    void CreateWrite()
    {
        pSaveBuffer = nullptr;
        dwCurSize = 0;
        dwMaxSize = 0;
        bSave = true;
        bLoad = false;
    }

    void CreateLoad()
    {
        bSave = false;
        bLoad = true;
        dwCurSize = 0;

        auto *pV = core.Event("SeaLoad_GetPointer", "sl", "seasave", -1);
        const char *pSave = pV->GetAClass()->GetAttribute("save");
        uint32_t dwSize;
        char str[256];
        strncpy_s(str, pSave, 8);
        str[8] = 0;
        sscanf(str, "%x", &dwSize);
        pSaveBuffer = static_cast<char *>(malloc(dwSize));
        for (uint32_t i = 0; i < dwSize; i++)
        {
            strncpy_s(str, &pSave[8 + i * 2], 2);
            str[2] = 0;
            uint32_t dwValue;
            sscanf(str, "%x", &dwValue);
            pSaveBuffer[i] = static_cast<char>(dwValue);
        }
    }

    void Write(const void *pBuffer, uint32_t dwSize)
    {
        if (dwMaxSize <= dwCurSize + dwSize)
        {
            dwMaxSize = 2048 * ((dwCurSize + dwSize + 2048) / 2048);
            pSaveBuffer = static_cast<char *>(realloc(pSaveBuffer, dwMaxSize));
        }
        memcpy(&pSaveBuffer[dwCurSize], pBuffer, dwSize);
        dwCurSize += dwSize;
    }

    void Read(void *pBuffer, uint32_t dwSize)
    {
        memcpy(pBuffer, &pSaveBuffer[dwCurSize], dwSize);
        dwCurSize += dwSize;
    }

    // =======================================================================================
    // Save functions
    // =======================================================================================

    void SaveDword(uint32_t dwValue)
    {
        Write(&dwValue, sizeof(dwValue));
    }

    void SaveQword(uint64_t dwValue)
    {
        Write(&dwValue, sizeof(dwValue));
    }

    void SaveFloat(float fValue)
    {
        Write(&fValue, sizeof(fValue));
    }

    void SaveLong(int32_t iValue)
    {
        Write(&iValue, sizeof(iValue));
    }

    void SaveString(const std::string &str)
    {
        if (str.size())
        {
            const uint32_t dwLen = str.size() + 1;
            SaveDword(dwLen);
            Write(str.c_str(), dwLen);
        }
        else
        {
            const uint32_t dwLen = 0;
            SaveDword(dwLen);
        }
    }

    void SaveVector(const CVECTOR &vVector)
    {
        Write((void *)&vVector, sizeof(vVector));
    }

    void SaveBuffer(const char *pBuffer, uint32_t dwSize)
    {
        SaveDword(dwSize);
        Write((void *)pBuffer, dwSize);
    }

    void SaveAPointer(const char *pStr, ATTRIBUTES *pAttribute)
    {
        int32_t iIndex = -1;
        if (pAttribute)
        {
            iIndex = static_cast<int32_t>(pAttribute->GetAttributeAsDword("index", -1));
        }
        SaveLong(iIndex);
        SaveString(pStr);
    }

    // =======================================================================================
    // Load functions
    // =======================================================================================

    uint32_t LoadDword()
    {
        uint32_t dwValue;
        Read(&dwValue, sizeof(dwValue));
        return dwValue;
    }

    uint64_t LoadQword()
    {
        uint64_t qwValue;
        Read(&qwValue, sizeof(qwValue));
        return qwValue;
    }

    float LoadFloat()
    {
        float fValue;
        Read(&fValue, sizeof(fValue));
        return fValue;
    }

    int32_t LoadLong()
    {
        int32_t iValue;
        Read(&iValue, sizeof(iValue));
        return iValue;
    }

    std::string LoadString()
    {
        std::string str;
        uint32_t dwLen;
        Read(&dwLen, sizeof(dwLen));
        if (dwLen == 0)
            return std::string();
        auto *const pBuffer = new char[dwLen];
        Read(pBuffer, dwLen);
        str = pBuffer;
        delete[] pBuffer;
        return str;
    }

    CVECTOR LoadVector()
    {
        CVECTOR v;
        Read(&v, sizeof(v));
        return v;
    }

    void LoadBuffer(char **pBuffer)
    {
        uint32_t dwSize;
        Read(&dwSize, sizeof(dwSize));
        *pBuffer = new char[dwSize];
        Read(*pBuffer, dwSize);
    }

    template <typename T> constexpr void Load2Buffer(T *pBuffer)
    {
        // protection against fools
        static_assert(std::is_trivial_v<T>, "Load2Buffer is only available for trivial types.");
        uint32_t dwSize;
        Read(&dwSize, sizeof(dwSize));
        Read(reinterpret_cast<char *>(pBuffer), dwSize);
    }

    ATTRIBUTES *LoadAPointer(const char *pStr)
    {
        const auto iIndex = LoadLong();
        const auto str = LoadString();
        if (str == "character" && iIndex < 0)
            return nullptr;
        auto *pV = core.Event("SeaLoad_GetPointer", "sl", pStr, iIndex);
        return (pV) ? pV->GetAClass() : nullptr;
    }
};
