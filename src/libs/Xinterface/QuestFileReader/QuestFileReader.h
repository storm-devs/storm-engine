#pragma once

#include "vmodule_api.h"
#include <string>
#include <vector>

class QUEST_FILE_READER
{
  public:
    struct UserData
    {
        std::string id;
        std::string str;
    };

    struct QuestDescribe
    {
        struct TextDescribe
        {
            std::string id;
            std::string str;
        };

        std::string sHeader;
        std::string sTitle;
        std::vector<TextDescribe> aText;

        QuestDescribe()
        {
        }
    };

  protected:
    std::vector<std::string> m_aQuestFileName;
    char *m_pFileBuf;

    std::vector<QuestDescribe> m_aQuest;

    std::string m_sCurQuestTitle;
    std::vector<UserData> m_aQuestData;

    long m_nDefQuestIndex;
    long m_nCurQuestIndex;
    long m_nCurTextIndex;
    long m_nCurLineIndex;

  public:
    QUEST_FILE_READER();
    ~QUEST_FILE_READER();

    void SetQuestTextFileName(const char *pcFileName);
    bool InitQuestsQuery();
    void CloseQuestsQuery();

    bool GetQuestTitle(const char *questId, const char *questUniqueID, size_t buffSize, char *buffer);

    void GetRecordTextList(std::vector<std::string> &asStringList, const char *pcQuestID, const char *pcTextID,
                           const char *pcUserData);

    static bool AssembleStringToBuffer(const char *pSrc, long nSrcSize, char *pBuf, long nBufSize,
                                       std::vector<UserData> &aUserData);
    static const char *GetInsertStringByID(char *pID, std::vector<UserData> &aUserData);
    static long AddToBuff(const char *pDst, long nDstSize, const char *pSrc, long nSrcSize = -1);
    static void WriteToString(std::string &sDst, const char *pcStart, const char *pcEnd);
    static void FillUserDataList(char *sStrData, std::vector<UserData> &aUserData);

  private:
    void ReadUserData(const char *sQuestName, long nRecordIndex);

    void AddQuestFromBuffer(const char *pcSrcBuffer);
    void AddToQuestList(std::string &sQuestID, std::string &sTextID, std::string &sQuestText, std::string &sTextText);
    long FindQuestByID(const char *pcQuestID);
    long FindTextByID(long nQuest, const char *pcTextID);
};
