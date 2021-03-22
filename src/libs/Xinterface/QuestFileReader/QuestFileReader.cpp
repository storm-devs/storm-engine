#include "QuestFileReader.h"
#include <assert.h>

#include "core.h"

#include "storm_assert.h"

#define INVALID_LONG 0

#define TOKEN_INVALID -1
#define TOKEN_VOID 0
#define TOKEN_QUEST 1
#define TOKEN_QTEXT 2
#define TOKEN_UNKNOWN 1000

#define TOKENTABLE_SIZE 2

static struct _TOKENTABLE
{
    const char *token;
    int cod;
} TokenTable[] = {{"#QUEST", TOKEN_QUEST}, {"#TEXT", TOKEN_QTEXT}};

bool IS_DIGIT(char ch);
long GET_DIGIT(char ch);
long GetToken(const char *&ps);
static long GetLongFromString(char *&pInStr);
static void GetSubStringFromString(const char *&pInStr, char *pOutBuf, int bufSize);
static const char *GetNextString(const char *&pInStr);
static char *GetTitleString(char *buf, char *&ptr, size_t &slen);

#define DELETE_PTR(p)                                                                                                  \
    {                                                                                                                  \
        if (p != NULL)                                                                                                 \
            delete p;                                                                                                  \
        p = NULL;                                                                                                      \
    }

long GetToken(const char *&ps)
{
    if (ps == nullptr)
        return TOKEN_INVALID;

    // remove leading spaces and tabs
    while (*ps == 32 || *ps == 9)
        ps++;

    // get the size of the token
    const auto *const ptoken = ps;
    while (*ps != 0 && static_cast<unsigned>(*ps) > 0x20 && *ps != '\r' && *ps != '\n')
        ps++;
    const size_t tokensize = ps - ptoken;
    if (tokensize == 0)
        return TOKEN_VOID;

    for (auto i = 0; i < TOKENTABLE_SIZE; i++)
        if (strncmp(TokenTable[i].token, ptoken, tokensize) == 0)
            return TokenTable[i].cod;

    // check the lexeme for compliance with certain values
    return TOKEN_UNKNOWN;
}

// Functions for working with symbols
bool IS_SPACE(char ch)
{
    if (static_cast<unsigned>(ch) <= ' ')
        return true;
    if (ch == '\r' || ch == '\n')
        return true;
    return false;
}

bool IS_DIGIT(char ch)
{
    if (ch >= '0' && ch <= '9')
        return true;
    return false;
}

long GET_DIGIT(char ch)
{
    return static_cast<long>(ch - '0');
}

// Functions for working with strings
static long GetLongFromString(char *&pInStr)
{
    if (pInStr == nullptr)
        return INVALID_LONG;

    // remove extra spaces
    while (*pInStr <= ' ')
        pInStr++;
    if (!IS_DIGIT(*pInStr))
        return INVALID_LONG;

    auto retVal = 0;
    while (IS_DIGIT(*pInStr))
    {
        retVal = retVal * 10 + GET_DIGIT(*pInStr);
        pInStr++;
    }
    return retVal;
}

void GetSubStringFromString(const char *&pInStr, char *pOutBuf, int bufSize)
{
    if (bufSize <= 0)
        return;
    // remove leading spaces
    while (IS_SPACE(*pInStr))
        pInStr++;
    // rewrite a line in the buffer
    int i;
    for (i = 0; i < bufSize - 1; i++, pInStr++)
    {
        if (IS_SPACE(*pInStr))
            break;
        pOutBuf[i] = *pInStr;
    }
    pOutBuf[i] = 0;
}

static const char *GetNextString(const char *&pInStr)
{
    if (pInStr == nullptr || pInStr[0] == 0)
        return nullptr;

    while (true)
    {
        auto bYesCurierReturn = false;
        auto bEmptyString = true;
        const char *pstart;
        for (pstart = pInStr; *pInStr != 0; pInStr++)
        {
            if (IS_SPACE(*pInStr))
                bEmptyString = false;
            if (*pInStr == '\r')
                bYesCurierReturn = true;
            else if (*pInStr == '\n')
                bYesCurierReturn = true;
            else if (bYesCurierReturn)
                break;
        }
        if (*pInStr != 0 && bEmptyString)
            continue;
        return pstart;
    }

    return nullptr;
}

static const char *GetTitleString(char *buf, const char *&ptr, size_t &slen)
{
    if (buf == nullptr)
    {
        slen = 0;
        return nullptr;
    }
    buf[0] = 0;
    const auto *startp = ptr;
    while (ptr != nullptr)
    {
        // take another line
        const auto *const cstr = GetNextString(ptr);
        if (ptr != cstr && cstr != nullptr)
        {
            // if the resulting string is the title of the quest
            const auto *tmpstr = cstr;
            int tokType = GetToken(tmpstr);
            if (tokType == TOKEN_QUEST)
            {
                // get the id of this quest
                GetSubStringFromString(tmpstr, buf, 256);
                const auto *const retVal = ptr;
                // find the end of the quest title
                while (ptr != nullptr)
                {
                    tmpstr = ptr;
                    tokType = GetToken(tmpstr);
                    if (tokType == TOKEN_QTEXT || tokType == TOKEN_QUEST)
                        break;
                    GetNextString(ptr);
                    if (tmpstr == ptr)
                        break;
                }
                if (ptr != tmpstr)
                    slen = ptr - retVal;
                else if (retVal != nullptr)
                    slen = strlen(retVal);
                else
                    slen = 0;
                if (retVal != nullptr)
                    return retVal;
            }
        }
        else
            break;
    }
    buf[0] = 0;
    slen = 0;
    return nullptr;
}

QUEST_FILE_READER::QUEST_FILE_READER()
{
    m_pFileBuf = nullptr;
}

QUEST_FILE_READER::~QUEST_FILE_READER()
{
    CloseQuestsQuery();
}

bool QUEST_FILE_READER::InitQuestsQuery()
{
    CloseQuestsQuery();

    if (m_aQuest.size() == 0)
    {
        for (long n = 0; n < m_aQuestFileName.size(); n++)
        {
            auto fileS = fio->_CreateFile(m_aQuestFileName[n].c_str(), std::ios::binary | std::ios::in);
            if (!fileS.is_open())
            {
                core.Trace("WARNING! Can`t open quest log file %s", m_aQuestFileName[n].c_str());
                continue;
            }

            const auto filesize = fio->_GetFileSize(m_aQuestFileName[n].c_str());
            if (filesize == 0)
            {
                core.Trace("Empty quest log file %s", m_aQuestFileName[n].c_str());
                fio->_CloseFile(fileS);
                continue;
            }

            long foffset = 0;
            if (m_pFileBuf == nullptr)
            {
                m_pFileBuf = static_cast<char *>(malloc(filesize + 1));
            }
            else
            {
                foffset = strlen(m_pFileBuf);
                m_pFileBuf = static_cast<char *>(realloc(m_pFileBuf, foffset + filesize + 1));
            }

            if (m_pFileBuf == nullptr)
            {
                throw std::exception("allocate memory error");
            }

            if (!fio->_ReadFile(fileS, &m_pFileBuf[foffset], filesize))
            {
                core.Trace("Can`t read quest log file: %s", m_aQuestFileName[n].c_str());
            }
            fio->_CloseFile(fileS);
            m_pFileBuf[foffset + filesize] = 0;
        }
    }

    if (m_pFileBuf == nullptr || m_pFileBuf[0] == 0)
    {
        return false;
    }
    return true;
}

void QUEST_FILE_READER::CloseQuestsQuery()
{
    free(m_pFileBuf);
    m_aQuestData.clear();
    m_sCurQuestTitle.clear();
}

bool QUEST_FILE_READER::GetQuestTitle(const char *questId, const char *questUniqueID, size_t buffSize, char *buffer)
{
    if (!buffer || buffSize == 0 || !questId)
        return false;

    ReadUserData(questUniqueID, -1);

    buffer[0] = 0;

    const auto n = FindQuestByID(questId);
    if (n < 0)
    {
        core.Trace("WARNING! Can`t find title whith ID = %s", questId);
        return false;
    }

    AssembleStringToBuffer(m_aQuest[n].sTitle.c_str(), m_aQuest[n].sTitle.size(), buffer, buffSize, m_aQuestData);
    return true;
}

void QUEST_FILE_READER::GetRecordTextList(std::vector<std::string> &asStringList, const char *pcQuestID,
                                          const char *pcTextID, const char *pcUserData)
{
    const auto nQuestIndex = FindQuestByID(pcQuestID);
    const auto nTextIndex = FindTextByID(nQuestIndex, pcTextID);
    if (nQuestIndex < 0 || nTextIndex < 0)
        return;

    m_sCurQuestTitle = "";
    m_aQuestData.clear();
    FillUserDataList((char *)pcUserData, m_aQuestData);

    char pcTmp[10240]; // boal 4096
    AssembleStringToBuffer(m_aQuest[nQuestIndex].aText[nTextIndex].str.c_str(),
                           m_aQuest[nQuestIndex].aText[nTextIndex].str.size(), pcTmp, sizeof(pcTmp), m_aQuestData);
    asStringList.push_back(pcTmp);
}

bool QUEST_FILE_READER::AssembleStringToBuffer(const char *pSrc, long nSrcSize, char *pBuf, long nBufSize,
                                               std::vector<UserData> &aUserData)
{
    if (!pSrc || !pBuf || nBufSize < 1)
        return false;

    char insertID[256];
    long nSrc, nDst, nIns;
    auto bMakeID = false;

    for (nSrc = 0, nDst = 0; nSrc < nSrcSize && nDst < nBufSize - 1; nSrc++)
    {
        if (pSrc[nSrc] == 0)
            break;
        if (bMakeID)
        {
            // creating an ID for the text insertion
            if (pSrc[nSrc] == '>')
            {
                // end of ID
                insertID[nIns] = 0;
                nDst += AddToBuff(&pBuf[nDst], nBufSize - nDst, GetInsertStringByID(insertID, aUserData));
                bMakeID = false;
            }
            else
            {
                insertID[nIns] = pSrc[nSrc];
                nIns++;
            }
        }
        else
        {
            // copying
            if (pSrc[nSrc] == '@' && pSrc[nSrc + 1] == '<')
            {
                nSrc++;
                bMakeID = true;
                nIns = 0;
            }
            else
            {
                pBuf[nDst] = pSrc[nSrc];
                nDst++;
            }
        }
    }
    pBuf[nDst] = 0;

    return true;
}

const char *QUEST_FILE_READER::GetInsertStringByID(char *pID, std::vector<UserData> &aUserData)
{
    if (!pID)
        return nullptr;
    for (long n = 0; n < aUserData.size(); n++)
        if (aUserData[n].id == pID)
            return aUserData[n].str.c_str();
    return nullptr;
}

long QUEST_FILE_READER::AddToBuff(const char *pDst, long nDstSize, const char *pSrc, long nSrcSize)
{
    if (!pSrc || !pDst)
        return 0;
    if (nSrcSize < 0)
        nSrcSize = strlen(pSrc);
    if (nSrcSize > nDstSize - 1)
        nSrcSize = nDstSize - 1;
    if (nSrcSize > 0)
        strncpy_s((char *)pDst, nDstSize, pSrc, nSrcSize);
    return nSrcSize;
}

void QUEST_FILE_READER::ReadUserData(const char *sQuestName, long nRecordIndex)
{
    if (m_sCurQuestTitle == sQuestName)
        return; // already set for this quest

    m_sCurQuestTitle = sQuestName;
    m_aQuestData.clear();

    if (!sQuestName)
        return;

    auto *pVD = core.Event("evntQuestUserData", "sl", sQuestName, nRecordIndex);
    if (!pVD)
        return;
    auto *const pStr = pVD->GetString();
    if (!pStr || pStr[0] == 0)
        return;

    FillUserDataList(pStr, m_aQuestData);
}

void QUEST_FILE_READER::FillUserDataList(char *sStrData, std::vector<UserData> &aUserData)
{
    if (!sStrData)
        return;

    int n, q, nBegID, nLenID, nBegStr, nLenStr;

    q = strlen(sStrData);

    auto bYesID = false;
    auto bGetID = false;

    for (n = 0; n < q; n++)
    {
        if (bGetID)
        {
            if (sStrData[n] == '>')
            {
                nLenID = n - nBegID;
                bYesID = true;
                bGetID = false;
                nBegStr = n + 1;
            }
        }
        else
        {
            if (sStrData[n] == '@' && sStrData[n + 1] == '<')
            {
                if (bYesID)
                {
                    nLenStr = n - nBegStr;
                    bYesID = false;

                    // write info
                    aUserData.push_back(UserData{});
                    WriteToString(aUserData.back().id, &sStrData[nBegID], &sStrData[nBegID + nLenID]);
                    WriteToString(aUserData.back().str, &sStrData[nBegStr], &sStrData[nBegStr + nLenStr]);
                }
                nBegID = n + 2;
                bGetID = true;
                n++;
            }
        }
    }

    if (bYesID)
    {
        nLenStr = n - nBegStr;
        // write info
        aUserData.push_back(UserData{});
        WriteToString(aUserData.back().id, &sStrData[nBegID], &sStrData[nBegID + nLenID]);
        WriteToString(aUserData.back().str, &sStrData[nBegStr], &sStrData[nBegStr + nLenStr]);
    }
}

void QUEST_FILE_READER::SetQuestTextFileName(const char *pcFileName)
{
    if (pcFileName == nullptr || pcFileName[0] == 0)
    {
        return;
    }

    // check for already included:
    for (long n = 0; n < m_aQuestFileName.size(); n++)
    {
        if (m_aQuestFileName[n] == pcFileName)
        {
            return;
        }
    }

    m_aQuestFileName.push_back(std::string(pcFileName));

    // open this file
    auto fileS = fio->_CreateFile(pcFileName, std::ios::binary | std::ios::in);
    if (!fileS.is_open())
    {
        core.Trace("WARNING! Can`t open quest log file %s", pcFileName);
        return;
    }
    // its size
    const uint32_t filesize = fio->_GetFileSize(pcFileName);
    if (filesize == 0)
    {
        core.Trace("Empty quest log file %s", pcFileName);
        fio->_CloseFile(fileS);
        return;
    }
    // create a buffer for it
    char *pBuf = new char[filesize + 1];
    Assert(pBuf);
    // read into this buffer from a file
    if (!fio->_ReadFile(fileS, pBuf, filesize))
    {
        core.Trace("Can`t read quest log file: %s", pcFileName);
    }
    fio->_CloseFile(fileS);
    pBuf[filesize] = 0;

    AddQuestFromBuffer(pBuf);
    delete[] pBuf;
}

void QUEST_FILE_READER::AddQuestFromBuffer(const char *pcSrcBuffer)
{
    if (!pcSrcBuffer)
        return;

    const char *pcStr = pcSrcBuffer;
    std::string sQuestID;
    std::string sTextID;
    std::string sQuestText;
    std::string sTextText;
    const char *pcPrev = pcStr;
    char tmp[1024];
    long nCurToken = TOKEN_UNKNOWN;

    while (pcStr)
    {
        if (!pcStr[0])
        {
            // check for completion
            if (nCurToken == TOKEN_QTEXT)
            {
                WriteToString(sTextText, pcPrev, pcStr);
                AddToQuestList(sQuestID, sTextID, sQuestText, sTextText);
            }
            break;
        }
        const char *pcLine = GetNextString(pcStr);
        const char *pcToken = pcLine;
        const long nToken = GetToken(pcToken);
        switch (nToken)
        {
        case TOKEN_QUEST: {
            if (nCurToken == TOKEN_QTEXT)
            {
                WriteToString(sTextText, pcPrev, pcLine);
                AddToQuestList(sQuestID, sTextID, sQuestText, sTextText);
            }

            GetSubStringFromString(pcToken, tmp, sizeof(tmp));
            sQuestID = tmp;
            sTextID.clear();
            sQuestText.clear();
            sTextText.clear();
            pcPrev = pcStr;
            nCurToken = TOKEN_QUEST;
        }
        break;

        case TOKEN_QTEXT: {
            if (nCurToken == TOKEN_QTEXT)
            {
                WriteToString(sTextText, pcPrev, pcLine);
                AddToQuestList(sQuestID, sTextID, sQuestText, sTextText);
            }
            if (nCurToken == TOKEN_QUEST)
            {
                WriteToString(sQuestText, pcPrev, pcLine);
            }
            GetSubStringFromString(pcToken, tmp, sizeof(tmp));
            sTextID = tmp;
            sTextText.clear();
            pcPrev = pcStr;
            nCurToken = TOKEN_QTEXT;
        }
        break;
        }
    }
}

void QUEST_FILE_READER::WriteToString(std::string &sDst, const char *pcStart, const char *pcEnd)
{
    if (!pcEnd || !pcStart)
    {
        sDst = "";
        return;
    }
    /*while( pcStart[0]!=0 && (*(UCHAR*)pcStart<=0x20) )
      pcStart++;
    while( pcEnd>pcStart && (*(UCHAR*)(pcEnd-1)<=0x20) )
      pcEnd--;   */ // boal do not remove spaces at the beginning of a line !!!

    const char chTmp = *pcEnd;
    *(char *)pcEnd = 0;
    sDst = pcStart;
    *(char *)pcEnd = chTmp;
}

void QUEST_FILE_READER::AddToQuestList(std::string &sQuestID, std::string &sTextID, std::string &sQuestText,
                                       std::string &sTextText)
{
    if (sQuestID.empty() || sTextID.empty())
        return;
    long q = FindQuestByID(sQuestID.c_str());
    if (q < 0)
    {
        q = m_aQuest.size();
        QuestDescribe descr;
        descr.sHeader = sQuestID;
        descr.sTitle = sQuestText;
        m_aQuest.push_back(descr);
    }

    const long t = FindTextByID(q, sTextID.c_str());
    if (t < 0)
    {
        QuestDescribe::TextDescribe descr;
        descr.id = sTextID;
        descr.str = sTextText;
        m_aQuest[q].aText.push_back(descr);
    }
}

long QUEST_FILE_READER::FindQuestByID(const char *pcQuestID)
{
    for (long n = 0; n < m_aQuest.size(); n++)
        if (m_aQuest[n].sHeader == pcQuestID)
            return n;
    return -1;
}

long QUEST_FILE_READER::FindTextByID(long nQuest, const char *pcTextID)
{
    if (nQuest < 0 || nQuest >= m_aQuest.size())
        return -1;
    for (long n = 0; n < m_aQuest[nQuest].aText.size(); n++)
        if (m_aQuest[nQuest].aText[n].id == pcTextID)
            return n;
    return -1;
}
