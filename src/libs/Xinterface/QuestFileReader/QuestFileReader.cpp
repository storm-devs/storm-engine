#include "QuestFileReader.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <map>

#include "core.h"

#include "storm_assert.h"

namespace {

enum class Token {
    Invalid,
    Quest,
    Text,

    Unknown,
};

const std::map<std::string_view, Token> kTokenMap = {
    {"#QUEST", Token::Quest},
    {"#TEXT", Token::Text}
};

/// @brief Read text until whitespace or newline
/// @param[in] str Text buffer to read from
/// @param[in,out] it An iterator pointing to place where reading needs to be started,
///                   will be set to character right after readed text in str
/// @return Readed text
/// @note If there is whitespaces in the beginning, they will be trimmed before reading
std::string_view ReadWord(const std::string_view & str, std::string_view::const_iterator &it)
{
    auto begin = std::find_if(it, str.cend(), [](const char ch) { return !isspace(static_cast<unsigned char>(ch)); });
    auto end = std::find_if(begin, str.cend(), [](const char ch) { return isspace(static_cast<unsigned char>(ch)); });

    it = end;

    return std::string_view(begin, end);
}

/// @brief Read text until new token starts ('#' symbol)
/// @param[in] str Text buffer to read from
/// @param[in,out] it An iterator pointing to place where reading needs to be started,
///                   will be set to '#' character right after readed text in str
/// @return Readed text
/// @note If there is whitespaces in the beginning, they will be trimmed before reading. Trailing whitespaces will be trimmed too.
std::string_view ReadText(const std::string_view &str, std::string_view::const_iterator &it)
{
    auto begin = std::find_if(it, str.cend(), [](const char ch) { return !isspace(static_cast<unsigned char>(ch)); });
    auto end = std::find_if(begin, str.cend(), [](const char ch) { return ch == '#'; }); ///< move until next token

    it = end;

    /// Trim trailing whitespaces
    auto text = std::string_view(begin, end);
    end = std::find_if(text.crbegin(), text.crend(), [](const char ch) {
              return !isspace(static_cast<unsigned char>(ch));
          }).base();

    return std::string_view(text.begin(), end); ///< We must use text.begin() because end is now belongs to text
}

/// @brief Read token with all related data
/// @param[in] str Text buffer to read from
/// @param[in,out] it An iterator pointing to place where reading needs to be started,
///                   will be set to character right after readed text in str
/// @return Token, id (questID or textID) and text (quest title for the QUEST token, and text for the TEXT token)
/// @note If there is whitespaces in the beginning, they will be trimmed before reading. Trailing whitespaces will be trimmed too.
std::tuple<Token, std::string_view, std::string_view> ReadFullToken(const std::string_view & str, std::string_view::const_iterator &it) {
    if (str.empty() || it == str.cend())
        return { Token::Invalid, "", "" };

    const auto tokenString = ReadWord(str, it);
    const auto token = kTokenMap.contains(tokenString) ? kTokenMap.at(tokenString) : Token::Unknown;

    const auto id = ReadWord(str, it);
    const auto text = ReadText(str, it);

    return { token, id, text };
};

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
                throw std::runtime_error("allocate memory error");
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
    if (!n.has_value())
    {
        core.Trace("WARNING! Can`t find title whith ID = %s", questId);
        return false;
    }

    const auto & quest = m_aQuest[n.value()];
    AssembleStringToBuffer(quest.sTitle.c_str(), quest.sTitle.size(), buffer, buffSize, m_aQuestData);
    return true;
}

void QUEST_FILE_READER::GetRecordTextList(std::vector<std::string> &asStringList, const char *pcQuestID,
                                          const char *pcTextID, const char *pcUserData)
{
    const auto questNum = FindQuestByID(pcQuestID);
    if (!questNum.has_value())
        return;

    auto & quest = m_aQuest[questNum.value()];
    const auto textNum = FindTextByID(quest, pcTextID);
    if (!textNum.has_value())
        return;

    m_sCurQuestTitle = "";
    m_aQuestData.clear();
    FillUserDataList((char *)pcUserData, m_aQuestData);

    /// @note If text is larger than the buffer size it can result to discarding some bytes of multibyte utf-8 symbol
    /// that results to -1 when calling utf8::Utf8ToCodepoint on that symbol
    char pcTmp[20480];
    const auto & unwrappedText = quest.aText[textNum.value()];
    AssembleStringToBuffer(unwrappedText.str.c_str(), unwrappedText.str.size(), pcTmp, sizeof(pcTmp), m_aQuestData);
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
    for (auto & questFileName : m_aQuestFileName)
    {
        if (questFileName == pcFileName)
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

    AddQuestsFromBuffer(pBuf);
    delete[] pBuf;
}

void QUEST_FILE_READER::AddQuestsFromBuffer(const std::string_view &srcBuffer)
{
    if (srcBuffer.empty())
        return;

    std::string_view curQuestId;
    for (auto it = srcBuffer.cbegin(); it != srcBuffer.cend(); )
    {
        auto [token, id, text] = ReadFullToken(srcBuffer, it);
        if (token == Token::Quest)
        {
            AddQuestToList(id, text);
            curQuestId = id;
        }
        else if (token == Token::Text)
        {
            AddTextToQuest(curQuestId, id, text);
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

void QUEST_FILE_READER::AddQuestToList(const std::string_view &questID, const std::string_view &titleText)
{
    if (questID.empty())
        return;

    auto q = FindQuestByID(questID);
    if (!q.has_value())
    {
        QuestDescribe descr;
        descr.sHeader = questID;
        descr.sTitle = titleText;
        m_aQuest.push_back(descr);
    }
    else
    {
        spdlog::warn("Quest with id \"%s\" is already in list", questID);
    }
}

void QUEST_FILE_READER::AddTextToQuest(const std::string_view &questID, const std::string_view &textID, const std::string_view &questText)
{
    if (questID.empty() || textID.empty())
        return;

    auto q = FindQuestByID(questID);
    if (!q.has_value())
    {
        spdlog::warn("Quest with id \"%s\" is not found in list", questID);
        return;
    }

    auto & quest = m_aQuest[q.value()];
    const auto t = FindTextByID(quest, textID);
    if (!t.has_value())
    {
        QuestDescribe::TextDescribe descr;
        descr.id = textID;
        descr.str = questText;
        quest.aText.push_back(descr);
    }
}

std::optional<size_t> QUEST_FILE_READER::FindQuestByID(const std::string_view &questID)
{
    if (questID.empty())
        return std::nullopt;

    for (size_t i = 0; i < m_aQuest.size(); ++i)
        if (m_aQuest[i].sHeader == questID)
            return i;

    return std::nullopt;
}

std::optional<size_t> QUEST_FILE_READER::FindTextByID(QUEST_FILE_READER::QuestDescribe &nQuest, const std::string_view &textID)
{
    if (textID.empty())
        return std::nullopt;

    auto & questTexts = nQuest.aText;
    for (size_t i = 0; i < questTexts.size(); ++i)
        if (questTexts[i].id == textID)
            return i;

    return std::nullopt;
}
