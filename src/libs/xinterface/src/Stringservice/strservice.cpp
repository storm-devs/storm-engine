#include "strservice.h"

#include "core.h"

#include "../xinterface.h"
#include "s_import_func.h"
#include "v_s_stack.h"

#include <filesystem>

#define USER_BLOCK_BEGINER '{'
#define USER_BLOCK_ENDING '}'

static const char *sLanguageFile = "resource\\ini\\TEXTS\\language.ini";

static VSTRSERVICE *g_StringServicePointer = nullptr;
static long g_idGlobLanguageFileID = -1;

bool GetStringDescribe(char *inStr, char *strName, char *outStr)
{
    if (strName != nullptr)
        strName[0] = 0;
    if (outStr != nullptr)
        outStr[0] = 0;
    if (strName == nullptr || outStr == nullptr || inStr == nullptr)
    {
        core.Trace("Waring: Invalid parameters %s for ini string parser", inStr);
        return false;
    }

    char *ps1;
    auto strLenght = 0;
    auto bDoEmpty = true;

    // Get string name
    for (ps1 = inStr; *ps1; ps1++)
    {
        const auto ch1 = *ps1;
        if (bDoEmpty && ch1 >= 0 && ch1 <= 32)
            continue;
        bDoEmpty = false;

        if (ch1 == ',')
            break;
        strName[strLenght] = ch1;
        strLenght++;
    }
    if (*ps1)
        ps1++;

    while (strLenght > 0 && strName[strLenght - 1] > 0 && strName[strLenght - 1] <= 32)
        strLenght--;

    if (strLenght <= 0)
    {
        core.Trace("Waring: Invalid name parameter for string: %s", inStr);
        return false;
    }
    strName[strLenght] = 0;

    // Get string value
    strLenght = 0;
    bDoEmpty = true;
    for (; *ps1; ps1++)
    {
        const auto ch1 = *ps1;
        if (bDoEmpty && ch1 != '\"')
            continue;
        if (bDoEmpty)
        {
            bDoEmpty = false;
            continue;
        }

        if (ch1 == '\"')
            break;
        outStr[strLenght] = ch1;
        strLenght++;
    }
    outStr[strLenght] = 0;

    return true;
}

STRSERVICE::STRSERVICE()
{
    m_sLanguage = nullptr;
    m_sIniFileName = nullptr;
    m_sLanguageDir = nullptr;

    m_nStringQuantity = 0;
    m_psStrName = nullptr;
    m_psString = nullptr;

    m_pUsersBlocks = nullptr;

    g_StringServicePointer = this;
    m_nDialogSourceFile = -1;
}

STRSERVICE::~STRSERVICE()
{
    int i;

    CloseUsersStringFile(m_nDialogSourceFile);
    m_nDialogSourceFile = -1;

    if (m_psStrName != nullptr)
    {
        for (i = 0; i < m_nStringQuantity; i++)
            if (m_psStrName[i] != nullptr)
                delete m_psStrName[i];
        delete m_psStrName;
        m_psStrName = nullptr;
    }
    if (m_psString != nullptr)
    {
        for (i = 0; i < m_nStringQuantity; i++)
            if (m_psString[i] != nullptr)
                delete m_psString[i];
        delete m_psString;
        m_psString = nullptr;
    }
    STORM_DELETE(m_sIniFileName);
    STORM_DELETE(m_sLanguage);
    STORM_DELETE(m_sLanguageDir);

    while (m_pUsersBlocks != nullptr)
    {
        auto *pUSB = m_pUsersBlocks;
        m_pUsersBlocks = m_pUsersBlocks->next;
        if (pUSB->psStrName != nullptr)
        {
            for (i = 0; i < pUSB->nStringsQuantity; i++)
                if (pUSB->psStrName[i] != nullptr)
                    delete pUSB->psStrName[i];
            delete pUSB->psStrName;
            pUSB->psStrName = nullptr;
        }
        if (pUSB->psString != nullptr)
        {
            for (i = 0; i < pUSB->nStringsQuantity; i++)
                if (pUSB->psString[i] != nullptr)
                    delete pUSB->psString[i];
            delete pUSB->psString;
            pUSB->psString = nullptr;
        }
        delete pUSB->fileName;
        pUSB->nStringsQuantity = 0;
        delete pUSB;
        pUSB = nullptr;
    }
}

bool STRSERVICE::Init()
{
    // GUARD(bool STRSERVICE::Init())
    LoadIni();
    // UNGUARD
    return true;
}

bool STRSERVICE::LoadState(ENTITY_STATE *state)
{
    return true;
}

bool STRSERVICE::CreateState(ENTITY_STATE_GEN *state_gen)
{
    return true;
}

void STRSERVICE::RunStart()
{
}

void STRSERVICE::RunEnd()
{
}

void STRSERVICE::SetLanguage(const char *sLanguage)
{
    // GUARD(void STRSERVICE::SetLanguage(const char* sLanguage))

    int i;
    char param[2048];

    if (sLanguage == nullptr)
    {
        core.Trace("WARNING! Attempt set empty language");
        return;
    }

    // This language is already set
    if (m_sLanguage != nullptr && _stricmp(sLanguage, m_sLanguage) == 0)
        return;

    // initialize ini file
    auto langIni = fio->OpenIniFile(sLanguageFile);
    if (!langIni)
    {
        core.Trace("ini file %s not found!", sLanguageFile);
        return;
    }

    // set a new name for the language
    STORM_DELETE(m_sLanguage);
    const auto len = strlen(sLanguage) + 1;
    if ((m_sLanguage = new char[len]) == nullptr)
    {
        throw std::runtime_error("Allocate memory error");
    }
    memcpy(m_sLanguage, sLanguage, len);

    while (true)
    {
        // delete old data
        STORM_DELETE(m_sIniFileName);
        STORM_DELETE(m_sLanguageDir);

        // get a directory for text files of a given language
        if (langIni->ReadString("DIRECTORY", m_sLanguage, param, sizeof(param) - 1, ""))
        {
            const auto len = strlen(param) + 1;
            if ((m_sLanguageDir = new char[len]) == nullptr)
            {
                throw std::runtime_error("Allocate memory error");
            }
            memcpy(m_sLanguageDir, param, len);
        }
        else
            core.Trace("WARNING! Not found directory record for language %s", sLanguage);

        // get the name of the ini file with common strings for this language
        if (langIni->ReadString("COMMON", "strings", param, sizeof(param) - 1, ""))
        {
            const auto len = strlen(param) + 1;
            if ((m_sIniFileName = new char[len]) == nullptr)
            {
                throw std::runtime_error("Allocate memory error");
            }
            memcpy(m_sIniFileName, param, len);
        }
        else
            core.Trace("WARNING! Not found common strings file record");

        if (m_sLanguageDir != nullptr && m_sIniFileName != nullptr)
            break;

        // compare the current language with the default
        if (langIni->ReadString("COMMON", "defaultLanguage", param, sizeof(param) - 1, ""))
        {
            if (_stricmp(m_sLanguage, param) == 0)
                break;
            core.Trace("WARNING! Language %s not exist some ini parameters. Language set to default %s", m_sLanguage,
                       param);
            STORM_DELETE(m_sLanguage);
            const auto len = strlen(param) + 1;
            m_sLanguage = new char[len];
            if (m_sLanguage == nullptr)
            {
                throw std::runtime_error("Allocate memory error");
            }
            memcpy(m_sLanguage, param, len);
        }
        else
            break;
    }
    //==========================================================================
    // reread fonts
    //==========================================================================
    auto *RenderService = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (RenderService)
    {
        char fullIniPath[512];
        if (langIni->ReadString("FONTS", m_sLanguage, param, sizeof(param) - 1, ""))
        {
            sprintf_s(fullIniPath, "resource\\ini\\%s", param);
        }
        else
        {
            core.Trace("Warning: Not found font record for language %s", m_sLanguage);
            sprintf_s(fullIniPath, "resource\\ini\\fonts.ini");
        }
        RenderService->SetFontIniFileName(fullIniPath);
    }
    //==========================================================================

    //====================================================================
    // Set language data
    //====================================================================

    // delete old stringes
    if (m_psString != nullptr)
    {
        for (i = 0; i < m_nStringQuantity; i++)
            delete m_psString[i];
        delete m_psString;
        m_psString = nullptr;
    }
    if (m_psStrName != nullptr)
    {
        for (i = 0; i < m_nStringQuantity; i++)
            delete m_psStrName[i];
        delete m_psStrName;
        m_psStrName = nullptr;
    }

    // initialize ini file
    sprintf_s(param, "resource\\ini\\texts\\%s\\%s", m_sLanguageDir, m_sIniFileName);
    auto ini = fio->OpenIniFile(param);
    if (!ini)
    {
        core.Trace("WARNING! ini file \"%s\" not found!", param);
        return;
    }

    // get string quantity
    auto newSize = 0;
    if (ini->ReadString(nullptr, "string", param, sizeof(param) - 1, ""))
        do
        {
            newSize++;
        } while (ini->ReadStringNext(nullptr, "string", param, sizeof(param) - 1));

    // check to right of ini files
    if (newSize != m_nStringQuantity && m_nStringQuantity != 0)
        core.Trace("WARNING: language %s ini file has different size", sLanguage);
    m_nStringQuantity = newSize;

    // create strings & string names arreys
    if (newSize > 0)
    {
        m_psString = new char *[newSize];
        m_psStrName = new char *[newSize];
        if (m_psStrName == nullptr || m_psString == nullptr)
            throw std::runtime_error("Allocate memory error");
    }
    else
    {
        m_psString = nullptr;
        m_psStrName = nullptr;
    }

    // fill stringes
    char strName[sizeof(param)];
    char string[sizeof(param)];
    ini->ReadString(nullptr, "string", param, sizeof(param) - 1, "");
    for (i = 0; i < m_nStringQuantity; i++)
    {
        if (GetStringDescribe(param, strName, string))
        {
            // fill string name
            auto len = strlen(param) + 1;
            m_psStrName[i] = new char[len];
            if (m_psStrName[i] == nullptr)
                throw std::runtime_error("allocate memory error");
            strcpy_s(m_psStrName[i], len, strName);

            // fill string self
            len = strlen(string) + 1;
            m_psString[i] = new char[len];
            if (m_psString[i] == nullptr)
            {
                delete m_psStrName[i];
                throw std::runtime_error("allocate memory error");
            }
            memcpy(m_psString[i], string, len);
        }
        else
        {
            // invalid string
            m_psStrName[i] = nullptr;
            m_psString[i] = nullptr;
        }

        // next string
        ini->ReadStringNext(nullptr, "string", param, sizeof(param) - 1);
    }

    // end of search

    // =======================================================================
    // Re-reading user files
    // =======================================================================
    auto *const pOldURoot = m_pUsersBlocks;
    m_pUsersBlocks = nullptr;
    for (auto *pUSB = pOldURoot; pUSB != nullptr; pUSB = pUSB->next)
    {
        if (pUSB->nref <= 0)
            continue;
        const long newID = OpenUsersStringFile(pUSB->fileName);
        UsersStringBlock *pUTmp;
        for (pUTmp = m_pUsersBlocks; pUTmp != nullptr; pUTmp = pUTmp->next)
            if (pUTmp->blockID == newID)
                break;
        if (pUTmp == nullptr)
        {
            core.Trace("Error: Can`t reinit user language file %s", pUSB->fileName);
            continue;
        }

        pUTmp->blockID = pUSB->blockID;
        pUTmp->nref = pUSB->nref;
        if (pUTmp->nStringsQuantity != pUSB->nStringsQuantity)
        {
            core.Trace("Warning: user strings file %s have different size for new language %s", pUTmp->fileName,
                       m_sLanguage);
            int itmp1, itmp2;
            for (itmp1 = 0; itmp1 < pUTmp->nStringsQuantity; itmp1++)
            {
                if (pUTmp->psStrName[itmp1] == nullptr)
                    continue;
                for (itmp2 = 0; itmp2 < pUSB->nStringsQuantity; itmp2++)
                {
                    if (pUSB->psStrName[itmp2] == nullptr)
                        continue;
                    if (_stricmp(pUSB->psStrName[itmp2], pUTmp->psStrName[itmp1]) == 0)
                        break;
                }
                if (itmp2 >= pUSB->nStringsQuantity)
                    core.Trace(">>> string <%s> not found into strings file", pUTmp->psStrName[itmp1]);
            }

            for (itmp1 = 0; itmp1 < pUSB->nStringsQuantity; itmp1++)
            {
                if (pUSB->psStrName[itmp1] == nullptr)
                    continue;
                for (int itmp2 = 0; itmp2 < pUTmp->nStringsQuantity; itmp2++)
                {
                    if (pUTmp->psStrName[itmp2] == nullptr)
                        continue;
                    if (_stricmp(pUTmp->psStrName[itmp2], pUSB->psStrName[itmp1]) == 0)
                        break;
                }
                if (itmp2 >= pUTmp->nStringsQuantity)
                    core.Trace(">>> string <%s> is new into strings file", pUSB->psStrName[itmp1]);
            }
        }
    }
    // Delete old user files
    UsersStringBlock *pNewURoot = m_pUsersBlocks;
    m_pUsersBlocks = pOldURoot;
    while (m_pUsersBlocks)
    {
        m_pUsersBlocks->nref = 1;
        CloseUsersStringFile(m_pUsersBlocks->blockID);
    }
    m_pUsersBlocks = pNewURoot;
    //=======================================================================

    // UNGUARD
}

char *STRSERVICE::GetLanguage()
{
    if (m_sLanguage != nullptr)
        return m_sLanguage;
    return nullptr;
}

char *STRSERVICE::GetString(const char *stringName, char *sBuffer, size_t bufferSize)
{
    // GUARD(char* STRSERVICE::GetString(const char* stringName, char* sBuffer, size_t bufferSize))

    if (stringName != nullptr)
        for (int i = 0; i < m_nStringQuantity; i++)
            if (!_stricmp(m_psStrName[i], stringName))
            {
                auto len = strlen(m_psString[i]) + 1;
                if (sBuffer == nullptr)
                    bufferSize = 0;
                if (bufferSize < len)
                    len = bufferSize;

                if (len > 0)
                    strcpy_s(sBuffer, bufferSize, m_psString[i]);

                return m_psString[i];
            }

    return nullptr;
    // UNGUARD
}

void STRSERVICE::SetDialogSourceFile(const char *fileName)
{
    CloseUsersStringFile(m_nDialogSourceFile);
    m_nDialogSourceFile = OpenUsersStringFile((char *)fileName);
}

void STRSERVICE::LoadIni()
{
    // GUARD(void STRSERVICE::LoadIni())

    char param[256];

    // initialize ini file
    auto ini = fio->OpenIniFile(sLanguageFile);
    if (!ini)
    {
        core.Trace("Error: Language ini file not found!");
        return;
    }

    char sGlobalUserFileName[256];
    if (!ini->ReadString("COMMON", "GlobalFile", sGlobalUserFileName, sizeof(sGlobalUserFileName) - 1, ""))
    {
        sGlobalUserFileName[0] = 0;
        core.Trace("WARNING! Language ini file have not global file name");
    }

    // Get default language name
    if (!ini->ReadString("COMMON", "defaultLanguage", param, sizeof(param) - 1, ""))
    {
        core.Trace("WARNING! Language ini file have not default language.");
        strcpy_s(param, "English");
    }

    if (param[0] != 0)
        SetLanguage(param);

    if (sGlobalUserFileName[0])
        g_idGlobLanguageFileID = OpenUsersStringFile(sGlobalUserFileName);

    SetDialogSourceFile("dialogsource.txt");

    // UNGUARD
}

long STRSERVICE::GetStringNum(const char *stringName)
{
    // GUARD(long STRSERVICE::GetStringNum(const char* stringName))

    if (stringName != nullptr)
        for (long i = 0; i < m_nStringQuantity; i++)
            if (!_stricmp(m_psStrName[i], stringName))
                return i;
    return -1L;

    // UNGUARD
}

char *STRSERVICE::GetString(long strNum)
{
    // GUARD(char* STRSERVICE::GetString(long strNum))

    if (strNum < 0 || strNum >= m_nStringQuantity)
        return nullptr;
    return m_psString[strNum];

    // UNGUARD
}

char *STRSERVICE::GetStringName(long strNum)
{
    // GUARD(char* STRSERVICE::GetStringName(long strNum))

    if (strNum < 0 || strNum >= m_nStringQuantity)
        return nullptr;
    return m_psStrName[strNum];

    // UNGUARD
}

long STRSERVICE::OpenUsersStringFile(const char *fileName)
{
    int i;
    if (fileName == nullptr)
    {
        return -1;
    }

    UsersStringBlock *pPrev = nullptr;
    UsersStringBlock *itUSB;
    for (itUSB = m_pUsersBlocks; itUSB != nullptr; itUSB = itUSB->next)
    {
        if (itUSB->fileName != nullptr && _stricmp(itUSB->fileName, fileName) == 0)
        {
            break;
        }
        pPrev = itUSB;
    }
    if (itUSB != nullptr)
    {
        itUSB->nref++;
        return itUSB->blockID;
    }

    auto pUSB = std::make_unique<UsersStringBlock>();

    // strings reading
    char param[512];
    sprintf_s(param, "resource\\ini\\TEXTS\\%s\\%s", m_sLanguageDir, fileName);
    auto fileS = fio->_CreateFile(param, std::ios::binary | std::ios::in);
    if (!fileS.is_open())
    {
        spdlog::warn("WARNING! Strings file \"{}\" does not exist", fileName);
        return -1;
    }

    const long filesize = fio->_GetFileSize(param);

    if (filesize <= 0)
    {
        spdlog::warn("WARNING! Strings file \"{}\" has zero size", fileName);
        return -1;
    }

    auto fileBuf = new char[filesize + 1];
    if (fileBuf == nullptr)
    {
        throw std::runtime_error("Allocate memory error");
    }

    if (!fio->_ReadFile(fileS, fileBuf, filesize))
    {
        core.Trace("Can`t read strings file: %s", fileName);
        fio->_CloseFile(fileS);
        STORM_DELETE(fileBuf);
        return -1;
    }
    fio->_CloseFile(fileS);
    fileBuf[filesize] = 0;

    pUSB->nref = 1;
    const auto len = strlen(fileName) + 1;
    pUSB->fileName = new char[len];
    if (pUSB->fileName == nullptr)
    {
        throw std::runtime_error("Allocate memory error");
    }
    memcpy(pUSB->fileName, fileName, len);
    pUSB->blockID = GetFreeUsersID();

    long stridx = 0;
    pUSB->psStrName = nullptr;
    pUSB->psString = nullptr;
    for (pUSB->nStringsQuantity = 0;; pUSB->nStringsQuantity++)
    {
        if (!GetNextUsersString(fileBuf, stridx, nullptr, nullptr))
        {
            break;
        }
    }
    if (pUSB->nStringsQuantity == 0)
    {
        core.Trace("WARNING! Strings file \"%s\" not contain strings", fileName);
    }
    else
    {
        pUSB->psStrName = new char *[pUSB->nStringsQuantity];
        pUSB->psString = new char *[pUSB->nStringsQuantity];
        if (pUSB->psStrName == nullptr || pUSB->psString == nullptr)
        {
            throw std::runtime_error("Allocate memory error");
        }
        stridx = 0;
        for (i = 0; i < pUSB->nStringsQuantity; i++)
        {
            GetNextUsersString(fileBuf, stridx, &pUSB->psStrName[i], &pUSB->psString[i]);
        }
    }

    STORM_DELETE(fileBuf);

    const long block_id = pUSB->blockID;
    pUSB->next = nullptr;
    if (pPrev == nullptr)
    {
        m_pUsersBlocks = pUSB.release();
    }
    else
    {
        pPrev->next = pUSB.release();
    }
    return block_id;
}

void STRSERVICE::CloseUsersStringFile(long id)
{
    int i;
    if (id == -1)
        return;

    UsersStringBlock *pPrev = nullptr;
    UsersStringBlock *pUSB;
    for (pUSB = m_pUsersBlocks; pUSB != nullptr; pUSB = pUSB->next)
    {
        if (pUSB->blockID == id)
            break;
        pPrev = pUSB;
    }
    if (pUSB == nullptr)
        return;
    pUSB->nref--;
    if (pUSB->nref > 0)
        return;

    if (pPrev == nullptr)
        m_pUsersBlocks = pUSB->next;
    else
        pPrev->next = pUSB->next;

    if (pUSB->fileName != nullptr)
    {
        delete pUSB->fileName;
        pUSB->fileName = nullptr;
    }
    if (pUSB->psStrName != nullptr)
    {
        for (i = 0; i < pUSB->nStringsQuantity; i++)
            if (pUSB->psStrName[i] != nullptr)
                delete pUSB->psStrName[i];
        delete pUSB->psStrName;
        pUSB->psStrName = nullptr;
    }
    if (pUSB->psString != nullptr)
    {
        for (i = 0; i < pUSB->nStringsQuantity; i++)
            if (pUSB->psString[i] != nullptr)
                delete pUSB->psString[i];
        delete pUSB->psString;
        pUSB->psString = nullptr;
    }
    delete pUSB;
}

char *STRSERVICE::TranslateFromUsers(long id, const char *inStr)
{
    int i;
    if (inStr == nullptr || id == -1)
        return nullptr;
    UsersStringBlock *pUSB;
    for (pUSB = m_pUsersBlocks; pUSB != nullptr; pUSB = pUSB->next)
        if (pUSB->blockID == id)
            break;
    if (pUSB == nullptr)
        return nullptr;

    for (i = 0; i < pUSB->nStringsQuantity; i++)
        if (pUSB->psStrName[i] != nullptr && _stricmp(pUSB->psStrName[i], inStr) == 0)
            break;
    if (i < pUSB->nStringsQuantity)
        return pUSB->psString[i];
    return nullptr;
}

long STRSERVICE::GetFreeUsersID() const
{
    int id;
    for (id = 0;; id++)
    {
        UsersStringBlock *pUSB;
        for (pUSB = m_pUsersBlocks; pUSB != nullptr; pUSB = pUSB->next)
            if (pUSB->blockID == id)
                break;
        if (pUSB == nullptr)
            break;
    }
    return id;
}

bool STRSERVICE::GetNextUsersString(char *src, long &idx, char **strName, char **strData) const
{
    char *tmpStr;
    char *dataBeg;
    char *dataEnd;
    char *nameBeg;
    char *nameEnd;

    if (strName != nullptr)
        *strName = nullptr;
    if (strData != nullptr)
        *strData = nullptr;
    if (src == nullptr || idx < 0)
        return false;

    dataBeg = strchr(&src[idx], USER_BLOCK_BEGINER);
    if (dataBeg == nullptr)
    {
        idx = strlen(src);
        return false;
    }
    dataBeg += 1;
    dataEnd = strchr(dataBeg, USER_BLOCK_ENDING);
    if (dataEnd == nullptr)
    {
        idx = strlen(src);
        return false;
    }
    dataEnd -= 1;

    // find ID name
    nameEnd = nameBeg = &src[idx];
    if ((tmpStr = strchr(nameEnd, '\n')) < dataBeg)
        do
        {
            if (tmpStr == nullptr)
                break;
            nameBeg = nameEnd;
            nameEnd = tmpStr;
        } while ((tmpStr = strchr(nameEnd + 2, '\n')) < dataBeg);
    idx = dataEnd - src + 2;

    // get rid of the spaces on the left
    while (nameBeg <= nameEnd)
    {
        if (*nameBeg == '\n')
        {
            nameBeg++;
            continue;
        }
        if (*nameBeg == '\r')
        {
            nameBeg++;
            continue;
        }
        if (*nameBeg == '\t')
        {
            nameBeg++;
            continue;
        }
        if (*nameBeg == ' ')
        {
            nameBeg++;
            continue;
        }
        break;
    }
    if (nameEnd - nameBeg <= 1)
        nameEnd = dataBeg - 2;
    // get rid of the spaces on the right
    while (nameBeg < nameEnd)
    {
        if (*nameBeg == '\n')
        {
            nameBeg++;
            continue;
        }
        if (*nameBeg == '\r')
        {
            nameBeg++;
            continue;
        }
        if (*nameBeg == '\t')
        {
            nameBeg++;
            continue;
        }
        if (*nameBeg == ' ')
        {
            nameBeg++;
            continue;
        }
        if (*nameEnd == '\n')
        {
            nameEnd--;
            continue;
        }
        if (*nameEnd == '\r')
        {
            nameEnd--;
            continue;
        }
        if (*nameEnd == '\t')
        {
            nameEnd--;
            continue;
        }
        if (*nameEnd == ' ')
        {
            nameEnd--;
            continue;
        }
        break;
    }
    // get rid of spaces on the left and right in the data
    while (dataBeg < dataEnd)
    {
        if (*dataEnd == ' ' || *dataEnd == '\t' || *dataEnd == '\r' || *dataEnd == '\n')
        {
            dataEnd--;
            continue;
        }
        if (*dataBeg == ' ' || *dataBeg == '\t' || *dataBeg == '\r' || *dataBeg == '\n')
        {
            dataBeg++;
            continue;
        }
        break;
    }

    if (strName != nullptr && nameBeg <= nameEnd)
    {
        *strName = new char[nameEnd - nameBeg + 2];
        if (*strName == nullptr)
            throw std::runtime_error("Allocate memory error");
        strncpy_s(*strName, nameEnd - nameBeg + 2, nameBeg, nameEnd - nameBeg + 1);
        strName[0][nameEnd - nameBeg + 1] = 0;
    }

    if (strData != nullptr && dataBeg <= dataEnd)
    {
        *strData = new char[dataEnd - dataBeg + 2];
        if (*strData == nullptr)
            throw std::runtime_error("Allocate memory error");
        strncpy_s(*strData, dataEnd - dataBeg + 2, dataBeg, dataEnd - dataBeg + 1);
        strData[0][dataEnd - dataBeg + 1] = 0;
        Assert(utf8::IsValidUtf8(*strData));
    }

    return true;
}

//===============================================================
// SCRIPT LIBS SECTION
// ===============================================================
// DESCRIPTION OF FUNCTIONS:
// ==============================================================

// Get the current language
uint32_t _Language_GetLanguage(VS_STACK *pS)
{
    char *strLangName = g_StringServicePointer->GetLanguage();

    auto pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;
    pVR->Set(strLangName);

    return IFUNCRESULT_OK;
}

// Open language file
uint32_t _Language_OpenFile(VS_STACK *pS)
{
    auto pLngFileName = (VDATA *)pS->Pop();
    if (!pLngFileName)
        return IFUNCRESULT_FAILED;
    const char *strLngFileName = nullptr;
    pLngFileName->Get(strLngFileName);

    // get the ID for the given file
    const long nLngFileID = g_StringServicePointer->OpenUsersStringFile(strLngFileName);

    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;
    pVR->Set(nLngFileID);

    return IFUNCRESULT_OK;
}

// Close language file
uint32_t _Language_CloseFile(VS_STACK *pS)
{
    auto pLngFileID = (VDATA *)pS->Pop();
    if (!pLngFileID)
        return IFUNCRESULT_FAILED;
    long nLngFileID = -1;
    pLngFileID->Get(nLngFileID);

    g_StringServicePointer->CloseUsersStringFile(nLngFileID);

    return IFUNCRESULT_OK;
}

// Interpret a string using a language file
uint32_t _Language_ConvertString(VS_STACK *pS)
{
    auto pInStr = (VDATA *)pS->Pop();
    if (!pInStr)
        return IFUNCRESULT_FAILED;
    const char *strInStr = nullptr;
    pInStr->Get(strInStr);

    auto *pLngFileID = (VDATA *)pS->Pop();
    if (!pLngFileID)
        return IFUNCRESULT_FAILED;
    long nLngFileID = -1;
    pLngFileID->Get(nLngFileID);

    char *strOutStr = g_StringServicePointer->TranslateFromUsers(nLngFileID, strInStr);

    auto pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;
    if (strOutStr)
        pVR->Set(strOutStr);
    else
        pVR->Set("\0");

    return IFUNCRESULT_OK;
}

// Interpret a string using a common language file
uint32_t _XI_ConvertString(VS_STACK *pS)
{
    auto *pInStr = (VDATA *)pS->Pop();
    if (!pInStr)
        return IFUNCRESULT_FAILED;
    const char *strInStr = nullptr;
    pInStr->Get(strInStr);

    char *strOutStr = g_StringServicePointer->GetString(strInStr);

    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;
    if (strOutStr)
        pVR->Set(strOutStr);
    else
        pVR->Set("\0");

    return IFUNCRESULT_OK;
}

// Set language
uint32_t _Language_SetLanguage(VS_STACK *pS)
{
    auto pLngName = (VDATA *)pS->Pop();
    if (!pLngName)
        return IFUNCRESULT_FAILED;
    const char *strLngName = nullptr;
    pLngName->Get(strLngName);

    g_StringServicePointer->SetLanguage(strLngName);

    return IFUNCRESULT_OK;
}

// Get the ID of the Global Language File
uint32_t _GlobalLngFileID(VS_STACK *pS)
{
    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;
    pVR->Set(g_idGlobLanguageFileID);

    return IFUNCRESULT_OK;
}

// Get the ID of the Global Language File
uint32_t _LanguageGetFaderPic(VS_STACK *pS)
{
    auto pPicName = (VDATA *)pS->Pop();
    if (!pPicName)
        return IFUNCRESULT_FAILED;
    const char *strPicName = nullptr;
    pPicName->Get(strPicName);

    char newPicName[MAX_PATH];
    newPicName[0] = 0;
    if (strPicName != nullptr && strPicName[0] != 0)
    {
        if (g_StringServicePointer->GetLanguage() != nullptr)
        {
            int nInLen;
            for (nInLen = strlen(strPicName); nInLen > 0; nInLen--)
                if (strPicName[nInLen - 1] == '\\')
                    break;
            if (nInLen > 0)
            {
                strncpy_s(newPicName, strPicName, nInLen);
                newPicName[nInLen] = 0;
            }
            strcat_s(newPicName, g_StringServicePointer->GetLanguage());
            strcat_s(newPicName, "\\");
            strcat_s(newPicName, &strPicName[nInLen]);
        }
        else
        {
            strcpy_s(newPicName, strPicName);
        }
    }

    auto *pVR = (VDATA *)pS->Push();
    if (!pVR)
        return IFUNCRESULT_FAILED;
    if (newPicName)
        pVR->Set(newPicName);
    else
        pVR->Set("\0");

    return IFUNCRESULT_OK;
}

// Set color grading for the game
uint32_t _SetColorCorrection(VS_STACK *pS)
{
    auto pBright = (VDATA *)pS->Pop();
    if (!pBright)
        return IFUNCRESULT_FAILED;
    float fBright = 1.f;
    pBright->Get(fBright);

    auto pGamma = (VDATA *)pS->Pop();
    if (!pGamma)
        return IFUNCRESULT_FAILED;
    float fGamma = 1.f;
    pGamma->Get(fGamma);

    auto *pContrast = (VDATA *)pS->Pop();
    if (!pContrast)
        return IFUNCRESULT_FAILED;
    float fContrast = 1.f;
    pContrast->Get(fContrast);

    auto pVR = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!pVR)
        return IFUNCRESULT_FAILED;

    pVR->SetColorParameters(fGamma, fBright, fContrast);
    return IFUNCRESULT_OK;
}

// Set mouse sensitivity
uint32_t _SetMouseSensitivity(VS_STACK *pS)
{
    auto *pYSens = (VDATA *)pS->Pop();
    if (!pYSens)
        return IFUNCRESULT_FAILED;
    float fYSens = 1.f;
    pYSens->Get(fYSens);

    auto *pXSens = (VDATA *)pS->Pop();
    if (!pXSens)
        return IFUNCRESULT_FAILED;
    float fXSens = 1.f;
    pXSens->Get(fXSens);

    auto pCntrl = static_cast<CONTROLS *>(core.CreateService("PCS_CONTROLS"));
    if (!pCntrl)
        return IFUNCRESULT_FAILED;

    pCntrl->SetMouseSensivityX(fXSens);
    pCntrl->SetMouseSensivityY(fYSens);

    return IFUNCRESULT_OK;
}

// Set invert to key
uint32_t _ControlMakeInvert(VS_STACK *pS)
{
    auto *pControlFlag = (VDATA *)pS->Pop();
    if (!pControlFlag)
        return IFUNCRESULT_FAILED;
    long nControlFlag = 0;
    pControlFlag->Get(nControlFlag);

    auto pControlName = (VDATA *)pS->Pop();
    if (!pControlName)
        return IFUNCRESULT_FAILED;
    const char *sCntrlName = nullptr;
    pControlName->Get(sCntrlName);
    if (!sCntrlName)
        return IFUNCRESULT_FAILED;

    auto pCntrl = static_cast<CONTROLS *>(core.CreateService("PCS_CONTROLS"));
    if (!pCntrl)
        return IFUNCRESULT_FAILED;

    const int n = pCntrl->CreateControl(sCntrlName);
    if (nControlFlag != 0)
        pCntrl->SetControlFlags(n, 0);
    else
        pCntrl->SetControlFlags(n, INVERSE_CONTROL);
    return IFUNCRESULT_OK;
}

uint32_t _InterfaceMakeNode(VS_STACK *pS)
{
    VDATA *pDat;

    char *sFileName = nullptr;
    char *sNodeType = nullptr;
    char *sNodeName = nullptr;
    long nPriority = 80;

    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    nPriority = pDat->GetLong();

    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    sNodeName = pDat->GetString();

    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    sNodeType = pDat->GetString();

    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    sFileName = pDat->GetString();

    if (XINTERFACE::pThis != nullptr)
        XINTERFACE::pThis->CreateNode(sFileName, sNodeType, sNodeName, nPriority);

    return IFUNCRESULT_OK;
}

uint32_t _InterfaceDeleteNode(VS_STACK *pS)
{
    auto pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    char *pcNodeName = pDat->GetString();
    if (XINTERFACE::pThis)
        XINTERFACE::pThis->DeleteNode(pcNodeName);
    return IFUNCRESULT_OK;
}

uint32_t _InterfaceWindowShow(VS_STACK *pS)
{
    VDATA *pDat;

    char *sWindowName = nullptr;
    long nShow = 0;

    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    nShow = pDat->GetLong();

    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    sWindowName = pDat->GetString();

    if (XINTERFACE::pThis != nullptr)
        XINTERFACE::pThis->ShowWindow(sWindowName, nShow != 0);

    return IFUNCRESULT_OK;
}

uint32_t _InterfaceWindowDisable(VS_STACK *pS)
{
    VDATA *pDat;

    char *sWindowName = nullptr;
    long nShow = 0;

    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    nShow = pDat->GetLong();

    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    sWindowName = pDat->GetString();

    if (XINTERFACE::pThis != nullptr)
        XINTERFACE::pThis->DisableWindow(sWindowName, nShow != 0);

    return IFUNCRESULT_OK;
}

uint32_t _InterfaceIsWindowEnable(VS_STACK *pS)
{
    VDATA *pDat;

    char *sWindowName = nullptr;

    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    sWindowName = pDat->GetString();

    bool bActive = true;
    if (XINTERFACE::pThis != nullptr)
    {
        bActive = XINTERFACE::pThis->IsWindowActive(sWindowName);
    }

    pDat = (VDATA *)pS->Push();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    pDat->Set(static_cast<long>(bActive ? 1 : 0));

    return IFUNCRESULT_OK;
}

uint32_t _InterfaceWindowAddNode(VS_STACK *pS)
{
    VDATA *pDat;

    char *sWindowName = nullptr;
    char *sNodeName = nullptr;

    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    sNodeName = pDat->GetString();

    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    sWindowName = pDat->GetString();

    if (XINTERFACE::pThis != nullptr)
        XINTERFACE::pThis->AddNodeToWindow(sNodeName, sWindowName);

    return IFUNCRESULT_OK;
}

uint32_t _InterfaceCreateFolder(VS_STACK *pS)
{
    VDATA *pDat;
    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    char *sFolderName = pDat->GetString();

    // precreate directory
    const char *pcCurPtr = sFolderName;
    while ((pcCurPtr = strchr(pcCurPtr, '\\')) != nullptr)
    {
        const char tmpchr = pcCurPtr[0];
        ((char *)pcCurPtr)[0] = 0;
        std::wstring FolderNameW = utf8::ConvertUtf8ToWide(sFolderName);
        CreateDirectory(FolderNameW.c_str(), nullptr);
        ((char *)pcCurPtr)[0] = tmpchr;
        pcCurPtr++;
    }
    // create self directory
    const long nSuccess = fio->_CreateDirectory(sFolderName);

    pDat = (VDATA *)pS->Push();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    pDat->Set(nSuccess);
    return IFUNCRESULT_OK;
}

uint32_t _InterfaceCheckFolder(VS_STACK *pS)
{
    VDATA *pDat;
    pDat = (VDATA *)pS->Pop();
    if (!pDat)
    {
        return IFUNCRESULT_FAILED;
    }
    char *sFolderName = pDat->GetString();
    long nSuccess = fio->_FileOrDirectoryExists(sFolderName);
    pDat = (VDATA *)pS->Push();
    if (!pDat)
    {
        return IFUNCRESULT_FAILED;
    }
    pDat->Set(nSuccess);
    return IFUNCRESULT_OK;
}

bool DeleteFolderWithCantainment(const char *sFolderName)
{
    return (fio->_RemoveDirectory(sFolderName) > 0);
}

uint32_t _InterfaceDeleteFolder(VS_STACK *pS)
{
    VDATA *pDat;
    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    char *sFolderName = pDat->GetString();
    // long nSuccess = fio->_RemoveDirectory(sFolderName);
    const long nSuccess = DeleteFolderWithCantainment(sFolderName);
    pDat = (VDATA *)pS->Push();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    pDat->Set(nSuccess);
    return IFUNCRESULT_OK;
}

uint32_t _InterfaceFindFolders(VS_STACK *pS)
{
    VDATA *pDat;
    pDat = (VDATA *)pS->Pop();
    if (!pDat)
    {
        return IFUNCRESULT_FAILED;
    }
    ATTRIBUTES *pA = pDat->GetAClass();
    pDat = (VDATA *)pS->Pop();
    if (!pDat)
    {
        return IFUNCRESULT_FAILED;
    }
    char *sFindTemplate = pDat->GetString();
    std::filesystem::path p = std::filesystem::u8path(sFindTemplate);
    const auto mask = p.filename().string();
    const auto vFilenames =
        fio->_GetPathsOrFilenamesByMask(p.remove_filename().string().c_str(), mask.c_str(), false, true, false);
    long n = 0;
    for (std::string curName : vFilenames)
    {
        char pctmp[64];
        sprintf_s(pctmp, "f%d", n++);
        pA->SetAttribute(pctmp, curName.c_str());
    }
    pDat = (VDATA *)pS->Push();
    if (!pDat)
    {
        return IFUNCRESULT_FAILED;
    }
    const long nSuccess = (pA->GetAttributesNum() > 0);
    pDat->Set(nSuccess);
    return IFUNCRESULT_OK;
}

uint32_t _DialogAssembleStr(VS_STACK *pS)
{
    VDATA *pDat;
    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    const char *pcParam = pDat->GetString();
    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    const char *pcID = pDat->GetString();

    std::string param;
    const char *pcStr =
        g_StringServicePointer ? static_cast<STRSERVICE *>(g_StringServicePointer)->TranslateForDialog(pcID) : pcID;
    if (pcStr)
    {
        std::vector<storm::QuestFileReader::UserData> userData;
        if (pcParam)
            storm::QuestFileReader::FillUserDataList((char *)pcParam, userData);
        storm::QuestFileReader::AssembleStringToBuffer(pcStr, param, userData);
        userData.clear();
    }

    pDat = (VDATA *)pS->Push();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    pDat->Set(param.c_str());
    return IFUNCRESULT_OK;
}

uint32_t _DialogAddParamToStr(VS_STACK *pS)
{
    VDATA *pDat;
    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    const char *pcDatVal = pDat->GetString();
    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    const char *pcDatID = pDat->GetString();
    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    const char *pcSrcStr = pDat->GetString();

    char param[4096];
    param[0] = 0;
    if (pcDatID && pcDatVal)
    {
        if (pcSrcStr && pcSrcStr[0] != 0)
        {
            sprintf_s(param, sizeof(param), "%s@<%s>%s", pcSrcStr, pcDatID, pcDatVal);
        }
        else
        {
            sprintf_s(param, sizeof(param), "@<%s>%s", pcDatID, pcDatVal);
        }
    }

    pDat = (VDATA *)pS->Push();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    pDat->Set(param);
    return IFUNCRESULT_OK;
}

uint32_t _StoreNodeLocksWithOff(VS_STACK *pS)
{
    long nStoreIndex = -1;
    if (XINTERFACE::pThis != nullptr)
        nStoreIndex = XINTERFACE::pThis->StoreNodeLocksWithOff();
    auto pDat = (VDATA *)pS->Push();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    pDat->Set(nStoreIndex);
    return IFUNCRESULT_OK;
}

uint32_t _RestoreNodeLocks(VS_STACK *pS)
{
    auto pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    const long nStoreIndex = pDat->GetLong();
    if (XINTERFACE::pThis != nullptr)
        XINTERFACE::pThis->RestoreNodeLocks(nStoreIndex);
    return IFUNCRESULT_OK;
}

uint32_t _IsKeyPressed(VS_STACK *pS)
{
    // get input data
    auto pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    const char *strKeyName = nullptr;
    pDat->Get(strKeyName);
    // data process
    bool bIsPressed = false;
    if (strKeyName)
    {
        if (_stricmp(strKeyName, "shift") == 0)
        {
            bIsPressed = (GetAsyncKeyState(VK_SHIFT) < 0);
        }
        else if (_stricmp(strKeyName, "control") == 0)
        {
            bIsPressed = (GetAsyncKeyState(VK_CONTROL) < 0);
        }
        else if (_stricmp(strKeyName, "alt") == 0)
        {
            bIsPressed = (GetAsyncKeyState(VK_MENU) < 0);
        }
    }
    // set return data
    pDat = (VDATA *)pS->Push();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    pDat->Set(static_cast<long>(bIsPressed));
    return IFUNCRESULT_OK;
}

uint32_t _RegistryExitKey(VS_STACK *pS)
{
    // get input data
    auto *pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    const char *strKeyName = nullptr;
    pDat->Get(strKeyName);
    if (XINTERFACE::pThis != nullptr)
        XINTERFACE::pThis->RegistryExitKey(strKeyName);
    return IFUNCRESULT_OK;
}

uint32_t _AddControlTreeNode(VS_STACK *pS)
{
    auto pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    const float fTimeOut = pDat->GetFloat();

    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    const char *pcOutControl;
    pDat->Get(pcOutControl);

    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    const char *pcBaseControl;
    pDat->Get(pcBaseControl);

    pDat = (VDATA *)pS->Pop();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    const long nParent = pDat->GetLong();

    const long nNodIdx = core.Controls->AddControlTreeNode(nParent, pcBaseControl, pcOutControl, fTimeOut);

    // set return data
    pDat = (VDATA *)pS->Push();
    if (!pDat)
        return IFUNCRESULT_FAILED;
    pDat->Set(nNodIdx);

    return IFUNCRESULT_OK;
}

//==============================================================
bool SCRIPT_INTERFACE_FUNCTIONS::Init()
{
    IFUNCINFO sIFuncInfo;

    sIFuncInfo.pReturnValueName = "string";
    sIFuncInfo.pFuncName = "LanguageGetLanguage";
    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncAddress = _Language_GetLanguage;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncName = "LanguageOpenFile";
    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncAddress = _Language_OpenFile;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncName = "LanguageCloseFile";
    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncAddress = _Language_CloseFile;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "string";
    sIFuncInfo.pFuncName = "LanguageConvertString";
    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncAddress = _Language_ConvertString;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncName = "LanguageSetLanguage";
    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncAddress = _Language_SetLanguage;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "string";
    sIFuncInfo.pFuncName = "XI_ConvertString";
    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncAddress = _XI_ConvertString;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncName = "GlobalLngFileID";
    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncAddress = _GlobalLngFileID;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "string";
    sIFuncInfo.pFuncName = "LanguageGetFaderPic";
    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncAddress = _LanguageGetFaderPic;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncName = "XI_SetColorCorrection";
    sIFuncInfo.nArguments = 3;
    sIFuncInfo.pFuncAddress = _SetColorCorrection;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncName = "XI_SetMouseSensitivity";
    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncAddress = _SetMouseSensitivity;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncName = "XI_ControlMakeInvert";
    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncAddress = _ControlMakeInvert;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncName = "XI_MakeNode";
    sIFuncInfo.nArguments = 4;
    sIFuncInfo.pFuncAddress = _InterfaceMakeNode;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncName = "XI_DeleteNode";
    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncAddress = _InterfaceDeleteNode;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncName = "XI_WindowShow";
    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncAddress = _InterfaceWindowShow;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncName = "XI_WindowDisable";
    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncAddress = _InterfaceWindowDisable;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "bool";
    sIFuncInfo.pFuncName = "XI_IsWindowEnable";
    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncAddress = _InterfaceIsWindowEnable;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncName = "XI_WindowAddNode";
    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncAddress = _InterfaceWindowAddNode;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "bool";
    sIFuncInfo.pFuncName = "XI_CreateFolder";
    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncAddress = _InterfaceCreateFolder;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "bool";
    sIFuncInfo.pFuncName = "XI_CheckFolder";
    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncAddress = _InterfaceCheckFolder;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "bool";
    sIFuncInfo.pFuncName = "XI_DeleteFolder";
    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncAddress = _InterfaceDeleteFolder;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "bool";
    sIFuncInfo.pFuncName = "XI_FindFolders";
    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncAddress = _InterfaceFindFolders;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "string";
    sIFuncInfo.pFuncName = "DialogAssembleStr";
    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncAddress = _DialogAssembleStr;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "string";
    sIFuncInfo.pFuncName = "DialogAddParamToStr";
    sIFuncInfo.nArguments = 3;
    sIFuncInfo.pFuncAddress = _DialogAddParamToStr;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncName = "XI_StoreNodeLocksWithOff";
    sIFuncInfo.nArguments = 0;
    sIFuncInfo.pFuncAddress = _StoreNodeLocksWithOff;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncName = "XI_RestoreNodeLocks";
    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncAddress = _RestoreNodeLocks;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "bool";
    sIFuncInfo.pFuncName = "XI_IsKeyPressed";
    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncAddress = _IsKeyPressed;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncName = "XI_RegistryExitKey";
    sIFuncInfo.nArguments = 1;
    sIFuncInfo.pFuncAddress = _RegistryExitKey;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo.pReturnValueName = "int";
    sIFuncInfo.pFuncName = "AddControlTreeNode";
    sIFuncInfo.nArguments = 4;
    sIFuncInfo.pFuncAddress = _AddControlTreeNode;
    core.SetScriptFunction(&sIFuncInfo);

    return true;
}

//===============================================================
//===============================================================
