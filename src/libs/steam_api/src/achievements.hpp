#pragma once

#include <steam/steam_api.h>

#define ACHIEVEMENTS_NUM 71

enum EAchievements
{
    ACH_01 = 0,
    ACH_02 = 1,
    ACH_03 = 2,
    ACH_04 = 3,
    ACH_05 = 4,
    ACH_06 = 5,
    ACH_07 = 6,
    ACH_08 = 7,
    ACH_09 = 8,
    ACH_10 = 9,
    ACH_11 = 10,
    ACH_12 = 11,
    ACH_13 = 12,
    ACH_14 = 13,
    ACH_15 = 14,
    ACH_16 = 15,
    ACH_17 = 16,
    ACH_18 = 17,
    ACH_19 = 18,
    ACH_20 = 19,
    ACH_21 = 20,
    ACH_22 = 21,
    ACH_23 = 22,
    ACH_24 = 23,
    ACH_25 = 24,
    ACH_26 = 25,
    ACH_27 = 26,
    ACH_28 = 27,
    ACH_29 = 28,
    ACH_30 = 29,
    ACH_31 = 30,
    ACH_32 = 31,
    ACH_33 = 32,
    ACH_34 = 33,
    ACH_35 = 34,
    ACH_36 = 35,
    ACH_37 = 36,
    ACH_38 = 37,
    ACH_39 = 38,
    ACH_40 = 39,
    ACH_41 = 40,
    ACH_42 = 41,
    ACH_43 = 42,
    ACH_44 = 43,
    ACH_45 = 44,
    ACH_46 = 45,
    ACH_47 = 46,
    ACH_48 = 47,
    ACH_49 = 48,
    ACH_50 = 49,
    ACH_51 = 50,
    ACH_52 = 51,
    ACH_53 = 52,
    ACH_54 = 53,
    ACH_55 = 54,
    //    ACH_56 = 55,
    ACH_57 = 56,
    ACH_58 = 57,
    ACH_59 = 58,
    ACH_60 = 59,
    ACH_61 = 60,
    ACH_62 = 61,
    ACH_63 = 62,
    ACH_64 = 63,
    ACH_65 = 64,
    ACH_66 = 65,
    ACH_67 = 66,
    ACH_68 = 67,
    ACH_69 = 68,
    ACH_70 = 69,
    ACH_71 = 70,
    //    ACH_72 = 71,
    //    ACH_73 = 72,
    ACH_79 = 79,
};

struct Achievement_t
{
    EAchievements m_eAchievementID;
    const char *m_pchAchievementID;
    char m_rgchName[128];
    char m_rgchDescription[256];
    bool m_bAchieved;
    int m_iIconImage;
};

enum EStatTypes
{
    STAT_INT = 0,
    STAT_FLOAT = 1,
    STAT_AVGRATE = 2,
};

struct Stat_t
{
    int m_ID;
    EStatTypes m_eStatType;
    const char *m_pchStatName;
    int m_iValue;
    float m_flValue;
    float m_flAvgNumerator;
    float m_flAvgDenominator;
};

class ISteamUser;
class ISteamApps;
class ISteamFriends;
class ISteamUtils;

class CSteamStatsAchievements
{
  private:
    int64 m_iAppID;                 // Our current AppID
    Achievement_t *m_pAchievements; // Achievements data
    uint32_t m_iNumAchievements;    // The number of Achievements
    bool m_bInitialized;            // Have we called Request stats and received the callback?
    Stat_t *m_pStats;

  public:
    CSteamStatsAchievements(int NumAchievements);
    ~CSteamStatsAchievements();

    bool RequestStats();
    bool SetAchievement(const char *ID);
    bool GetAchievement(const char *ID);
    bool SetStat(const char *ID, uint32_t value);
    uint32_t GetStat(const char *ID);
    bool StoreStats();
    bool GetConnected();
    bool ResetStats(bool bAchievementsToo);
    bool ClearAchievement(const char *ID);

    STEAM_CALLBACK(CSteamStatsAchievements, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
    STEAM_CALLBACK(CSteamStatsAchievements, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
    STEAM_CALLBACK(CSteamStatsAchievements, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);
};

class CSteamDLC
{
  private:
    bool m_bInitialized;
    uint32_t m_DLCcount;
    char pchName[128];
    bool pbAvailable;
    AppId_t pAppID;
    bool isOverlayActivated;

  public:
    CSteamDLC();
    ~CSteamDLC();

    bool isDLCInstalled(uint32_t nDLC);
    uint32_t bGetDLCDataByIndex(uint32_t iDLC);
    uint32_t getDLCCount();
    bool activateGameOverlay(uint32_t nAppId);

    STEAM_CALLBACK(CSteamDLC, OnOverlayActivated, GameOverlayActivated_t, m_CallbackOverlayActivated);
};
