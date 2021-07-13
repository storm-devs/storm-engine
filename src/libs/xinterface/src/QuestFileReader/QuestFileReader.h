#pragma once

#include "vmodule_api.h"
#include <optional>
#include <string>
#include <vector>

namespace storm
{
class QuestFileReader
{
  public:
    struct UserData
    {
        /// @note Constructor is needed for emplace_back
        UserData(const std::string &id, const std::string &str) : id{id}, str{str}
        {
        }

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

        std::string questID;
        std::string title;
        std::vector<TextDescribe> texts;
    };

  public:
    QuestFileReader();
    ~QuestFileReader();

    void SetQuestTextFileName(const std::string_view &fileName);

    bool GetQuestTitle(const std::string_view &questId, const std::string_view &questUniqueID, std::string &buffer);

    std::string GetRecordText(const std::string_view &questID, const std::string_view &textID,
                              const std::string_view &userData);

    static bool AssembleStringToBuffer(const std::string_view &src, std::string &buffer,
                                       const std::vector<UserData> &userData);
    static std::string GetInsertStringByID(const std::string_view &id, const std::vector<UserData> &userData);
    static void FillUserDataList(const std::string_view &strData, std::vector<UserData> &userData);

  private:
    void ReadUserData(const std::string_view &questID, long recordIndex);

    void AddQuestsFromBuffer(const std::string_view &srcBuffer);
    void AddQuestToList(const std::string_view &questID, const std::string_view &titleText);
    void AddTextToQuest(const std::string_view &questID, const std::string_view &textID,
                        const std::string_view &questText);
    std::optional<size_t> FindQuestByID(const std::string_view &questID);
    std::optional<size_t> FindTextByID(QuestDescribe &nQuest, const std::string_view &textID);

  private:
    std::vector<std::string> questFileNames_;

    std::vector<QuestDescribe> quests_;

    std::string curQuestID_;
    std::vector<UserData> questData_;
};
} // namespace storm
