//============================================================================================
#include "achievements.hpp"
#include "core.h"

#define _ACH_ID(id, name)                                                                                              \
    {                                                                                                                  \
        id, #id, name, "", 0, 0                                                                                        \
    }
#define _STAT_ID(id, type, name)                                                                                       \
    {                                                                                                                  \
        id, type, name, 0, 0, 0, 0                                                                                     \
    }

Achievement_t g_Achievements[] = {
    _ACH_ID(ACH_01, "ach_01"),
    _ACH_ID(ACH_02, "ach_02"),
    _ACH_ID(ACH_03, "ach_03"),
    _ACH_ID(ACH_04, "ach_04"),
    _ACH_ID(ACH_05, "ach_05"),
    _ACH_ID(ACH_06, "ach_06"),
    _ACH_ID(ACH_07, "ach_07"),
    _ACH_ID(ACH_08, "ach_08"),
    _ACH_ID(ACH_09, "ach_09"),
    _ACH_ID(ACH_10, "ach_10"),
    _ACH_ID(ACH_11, "ach_11"),
    _ACH_ID(ACH_12, "ach_12"),
    _ACH_ID(ACH_13, "ach_13"),
    _ACH_ID(ACH_14, "ach_14"),
    _ACH_ID(ACH_15, "ach_15"),
    _ACH_ID(ACH_16, "ach_16"),
    _ACH_ID(ACH_17, "ach_17"),
    _ACH_ID(ACH_18, "ach_18"),
    _ACH_ID(ACH_19, "ach_19"),
    _ACH_ID(ACH_20, "ach_20"),
    _ACH_ID(ACH_21, "ach_21"),
    _ACH_ID(ACH_22, "ach_22"),
    _ACH_ID(ACH_23, "ach_23"),
    _ACH_ID(ACH_24, "ach_24"),
    _ACH_ID(ACH_25, "ach_25"),
    _ACH_ID(ACH_26, "ach_26"),
    _ACH_ID(ACH_27, "ach_27"),
    _ACH_ID(ACH_28, "ach_28"),
    _ACH_ID(ACH_29, "ach_29"),
    _ACH_ID(ACH_30, "ach_30"),
    _ACH_ID(ACH_31, "ach_31"),
    _ACH_ID(ACH_32, "ach_32"),
    _ACH_ID(ACH_33, "ach_33"),
    _ACH_ID(ACH_34, "ach_34"),
    _ACH_ID(ACH_35, "ach_35"),
    _ACH_ID(ACH_36, "ach_36"),
    _ACH_ID(ACH_37, "ach_37"),
    _ACH_ID(ACH_38, "ach_38"),
    _ACH_ID(ACH_39, "ach_39"),
    _ACH_ID(ACH_40, "ach_40"),
    _ACH_ID(ACH_41, "ach_41"),
    _ACH_ID(ACH_42, "ach_42"),
    _ACH_ID(ACH_43, "ach_43"),
    _ACH_ID(ACH_44, "ach_44"),
    _ACH_ID(ACH_45, "ach_45"),
    _ACH_ID(ACH_46, "ach_46"),
    _ACH_ID(ACH_47, "ach_47"),
    _ACH_ID(ACH_48, "ach_48"),
    _ACH_ID(ACH_49, "ach_49"),
    _ACH_ID(ACH_50, "ach_50"),
    _ACH_ID(ACH_51, "ach_51"),
    _ACH_ID(ACH_52, "ach_52"),
    _ACH_ID(ACH_53, "ach_53"),
    _ACH_ID(ACH_54, "ach_54"),
    _ACH_ID(ACH_55, "ach_55"),
    //    _ACH_ID( ACH_56, "ach_56" ),
    _ACH_ID(ACH_57, "ach_57"),
    _ACH_ID(ACH_58, "ach_58"),
    _ACH_ID(ACH_59, "ach_59"),
    _ACH_ID(ACH_60, "ach_60"),
    _ACH_ID(ACH_61, "ach_61"),
    _ACH_ID(ACH_62, "ach_62"),
    _ACH_ID(ACH_63, "ach_63"),
    _ACH_ID(ACH_64, "ach_64"),
    _ACH_ID(ACH_65, "ach_65"),
    _ACH_ID(ACH_66, "ach_66"),
    _ACH_ID(ACH_67, "ach_67"),
    _ACH_ID(ACH_68, "ach_68"),
    _ACH_ID(ACH_69, "ach_69"),
    _ACH_ID(ACH_70, "ach_70"),
    _ACH_ID(ACH_71, "ach_71"),
    //    _ACH_ID( ACH_72, "ach_72" ),
    //    _ACH_ID( ACH_73, "ach_73" ),
    _ACH_ID(ACH_79, "ach_79"),
};

Stat_t g_Stats[] = {
    _STAT_ID(1, STAT_INT, "stat_01"),
    _STAT_ID(2, STAT_INT, "stat_02"),
    _STAT_ID(3, STAT_INT, "stat_03"),
    _STAT_ID(4, STAT_INT, "stat_04"),
    _STAT_ID(5, STAT_INT, "stat_05"),
    _STAT_ID(6, STAT_INT, "stat_06"),
    _STAT_ID(7, STAT_INT, "stat_07"),
    _STAT_ID(8, STAT_INT, "stat_08"),
    _STAT_ID(9, STAT_INT, "stat_09"),
    _STAT_ID(10, STAT_INT, "stat_10"),
    _STAT_ID(11, STAT_INT, "stat_11"),
    _STAT_ID(12, STAT_INT, "stat_12"),
    _STAT_ID(13, STAT_INT, "stat_13"),
    _STAT_ID(14, STAT_INT, "stat_14"),
    _STAT_ID(15, STAT_INT, "stat_15"),
    _STAT_ID(16, STAT_INT, "stat_16"),
    _STAT_ID(17, STAT_INT, "stat_17"),
    _STAT_ID(18, STAT_INT, "stat_18"),
    _STAT_ID(19, STAT_INT, "stat_19"),
    _STAT_ID(20, STAT_INT, "stat_20"),
    _STAT_ID(21, STAT_INT, "stat_21"),
    _STAT_ID(22, STAT_INT, "stat_22"),
    _STAT_ID(23, STAT_INT, "stat_23"),
    _STAT_ID(24, STAT_INT, "stat_24"),
    _STAT_ID(25, STAT_INT, "stat_25"),
    _STAT_ID(26, STAT_INT, "stat_26"),
    _STAT_ID(27, STAT_INT, "stat_27"),
    _STAT_ID(28, STAT_INT, "stat_28"),
    _STAT_ID(29, STAT_INT, "stat_29"),
    _STAT_ID(30, STAT_INT, "stat_30"),
    _STAT_ID(31, STAT_INT, "stat_31"),
    _STAT_ID(32, STAT_INT, "stat_32"),
    _STAT_ID(33, STAT_INT, "stat_33"),
    _STAT_ID(34, STAT_INT, "stat_34"),
    _STAT_ID(35, STAT_INT, "stat_35"),
    _STAT_ID(36, STAT_INT, "stat_36"),
    _STAT_ID(37, STAT_INT, "stat_37"),
    _STAT_ID(38, STAT_INT, "stat_38"),
    _STAT_ID(39, STAT_INT, "stat_39"),
    _STAT_ID(40, STAT_INT, "stat_40"),
    _STAT_ID(41, STAT_INT, "stat_41"),
    _STAT_ID(42, STAT_INT, "stat_42"),
    _STAT_ID(43, STAT_INT, "stat_43"),
    _STAT_ID(44, STAT_INT, "stat_44"),
    _STAT_ID(45, STAT_INT, "stat_45"),
    _STAT_ID(46, STAT_INT, "stat_46"),
    _STAT_ID(47, STAT_INT, "stat_47"),
    _STAT_ID(48, STAT_INT, "stat_48"),
    _STAT_ID(49, STAT_INT, "stat_49"),
    _STAT_ID(50, STAT_INT, "stat_50"),
    _STAT_ID(51, STAT_INT, "stat_51"),
    _STAT_ID(52, STAT_INT, "stat_52"),
    _STAT_ID(53, STAT_INT, "stat_53"),
    _STAT_ID(54, STAT_INT, "stat_54"),
    _STAT_ID(55, STAT_INT, "stat_55"),
    //    _STAT_ID(56, STAT_INT, "stat_56"),
    _STAT_ID(57, STAT_INT, "stat_57"),
    _STAT_ID(58, STAT_INT, "stat_58"),
    _STAT_ID(59, STAT_INT, "stat_59"),
    _STAT_ID(60, STAT_INT, "stat_60"),
    _STAT_ID(61, STAT_INT, "stat_61"),
    _STAT_ID(62, STAT_INT, "stat_62"),
    _STAT_ID(63, STAT_INT, "stat_63"),
    _STAT_ID(64, STAT_INT, "stat_64"),
    _STAT_ID(65, STAT_INT, "stat_65"),
    _STAT_ID(66, STAT_INT, "stat_66"),
    _STAT_ID(67, STAT_INT, "stat_67"),
    _STAT_ID(68, STAT_INT, "stat_68"),
    _STAT_ID(69, STAT_INT, "stat_69"),
    _STAT_ID(70, STAT_INT, "stat_70"),
    _STAT_ID(71, STAT_INT, "stat_71"),
    //    _STAT_ID(72, STAT_INT, "stat_72"),
    //    _STAT_ID(73, STAT_INT, "stat_73"),
    _STAT_ID(79, STAT_INT, "stat_79"),
};

CSteamStatsAchievements::CSteamStatsAchievements(int NumAchievements)
    : m_iAppID(0), m_bInitialized(false),
      m_CallbackUserStatsReceived(this, &CSteamStatsAchievements::OnUserStatsReceived),
      m_CallbackUserStatsStored(this, &CSteamStatsAchievements::OnUserStatsStored),
      m_CallbackAchievementStored(this, &CSteamStatsAchievements::OnAchievementStored)
{
    m_iAppID = SteamUtils()->GetAppID();
    m_iNumAchievements = NumAchievements;

    for (int iAch = 0; iAch < m_iNumAchievements; ++iAch)
    {
        Achievement_t &ach = g_Achievements[iAch];
        Stat_t &stat = g_Stats[iAch];
    }
    RequestStats();
}

CSteamStatsAchievements::~CSteamStatsAchievements()
{
}

bool CSteamStatsAchievements::GetConnected()
{
    return m_bInitialized;
}

bool CSteamStatsAchievements::RequestStats()
{
    // Is Steam loaded? If not we can't get stats.
    if (SteamUser() == nullptr)
    {
        return false;
    }
    // Is the user logged on?  If not we can't get stats.
    if (!SteamUser()->BLoggedOn())
    {
        return false;
    }
    // Request user stats.
    return SteamUserStats()->RequestCurrentStats();
}

bool CSteamStatsAchievements::StoreStats()
{
    if (m_bInitialized)
    {
        // load stats
        for (int iStat = 0; iStat < m_iNumAchievements; ++iStat)
        {
            Stat_t &stat = g_Stats[iStat];
            switch (stat.m_eStatType)
            {
            case STAT_INT:
                SteamUserStats()->SetStat(stat.m_pchStatName, stat.m_iValue);
                break;

            case STAT_FLOAT:
                SteamUserStats()->SetStat(stat.m_pchStatName, stat.m_flValue);
                break;

            case STAT_AVGRATE:
                SteamUserStats()->UpdateAvgRateStat(stat.m_pchStatName, stat.m_flAvgNumerator, stat.m_flAvgDenominator);
                // The averaged result is calculated for us
                SteamUserStats()->GetStat(stat.m_pchStatName, &stat.m_flValue);
                break;

            default:
                break;
            }
        }
        return SteamUserStats()->StoreStats();
    }
    return false;
}

bool CSteamStatsAchievements::SetAchievement(const char *ID)
{
    // Have we received a call back from Steam yet?
    if (m_bInitialized)
    {
        SteamUserStats()->SetAchievement(ID);
        return SteamUserStats()->StoreStats();
    }
    // If not then we can't set achievements yet
    return false;
}

bool CSteamStatsAchievements::GetAchievement(const char *ID)
{
    if (m_bInitialized)
    {
        Achievement_t ach;
        SteamUserStats()->GetAchievement(ID, &ach.m_bAchieved);
        return ach.m_bAchieved;
    }
    // If not then we can't get achievements yet
    return false;
}

bool CSteamStatsAchievements::SetStat(const char *ID, uint32_t value)
{
    if (m_bInitialized)
    {
        for (int iAch = 0; iAch < m_iNumAchievements; ++iAch)
        {
            Stat_t &stat = g_Stats[iAch];
            if (strcmp(ID, stat.m_pchStatName) == 0)
            {
                stat.m_iValue = value;
                SteamUserStats()->SetStat(stat.m_pchStatName, stat.m_iValue);
                return true;
            }
        }
    }
    return false;
}

uint32_t CSteamStatsAchievements::GetStat(const char *ID)
{
    if (m_bInitialized)
    {
        for (int iAch = 0; iAch < m_iNumAchievements; ++iAch)
        {
            Stat_t &stat = g_Stats[iAch];
            if (strcmp(ID, stat.m_pchStatName) == 0)
            {
                SteamUserStats()->GetStat(stat.m_pchStatName, &stat.m_iValue);
                return stat.m_iValue;
            }
        }
        return -1;
    }
    return -1;
}

bool CSteamStatsAchievements::ResetStats(bool bAchievementsToo)
{
    if (m_bInitialized)
    {
        return SteamUserStats()->ResetAllStats(bAchievementsToo);
    }
    return false;
}

bool CSteamStatsAchievements::ClearAchievement(const char *ID)
{
    if (m_bInitialized)
    {
        return SteamUserStats()->ClearAchievement(ID);
    }
    return false;
}

void CSteamStatsAchievements::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
    // we may get callbacks for other games' stats arriving, ignore them
    if (m_iAppID == pCallback->m_nGameID)
    {
        if (k_EResultOK == pCallback->m_eResult)
        {
            m_bInitialized = true;

            // load achievements
            for (int iAch = 0; iAch < m_iNumAchievements; ++iAch)
            {
                Achievement_t &ach = g_Achievements[iAch];

                SteamUserStats()->GetAchievement(ach.m_pchAchievementID, &ach.m_bAchieved);
                _snprintf(ach.m_rgchName, sizeof(ach.m_rgchName), "%s",
                          SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID, "name"));

                _snprintf(ach.m_rgchDescription, sizeof(ach.m_rgchDescription), "%s",
                          SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID, "desc"));

                Stat_t &stat = g_Stats[iAch];
                switch (stat.m_eStatType)
                {
                case STAT_INT:
                    SteamUserStats()->GetStat(stat.m_pchStatName, &stat.m_iValue);
                    break;

                case STAT_FLOAT:
                case STAT_AVGRATE:
                    SteamUserStats()->GetStat(stat.m_pchStatName, &stat.m_flValue);
                    break;

                default:
                    break;
                }
            }
        }
    }
}

void CSteamStatsAchievements::OnUserStatsStored(UserStatsStored_t *pCallback)
{
    // we may get callbacks for other games' stats arriving, ignore them
    if (pCallback->m_nGameID == m_iAppID)
    {
        if (pCallback->m_eResult == k_EResultOK)
        {
        }
        else if (pCallback->m_eResult == k_EResultInvalidParam)
        {
            UserStatsReceived_t callback;
            callback.m_eResult = k_EResultOK;
            callback.m_nGameID = m_iAppID;
            OnUserStatsReceived(&callback);
        }
    }
}

void CSteamStatsAchievements::OnAchievementStored(UserAchievementStored_t *pCallback)
{
    // we may get callbacks for other games' stats arriving, ignore them
    if (m_iAppID == pCallback->m_nGameID)
    {
        if (pCallback->m_nMaxProgress == 0)
        {
            //            char buffer[128];
            //            _snprintf( buffer, 128, "Achievement '%s' unlocked!", pCallback->m_rgchAchievementName );
            //            trace( buffer );
        }
        else
        {
            //            char buffer[128];
            //            _snprintf( buffer, 128, "Achievement '%s' progress callback, (%d,%d)\n",
            //            pCallback->m_rgchAchievementName, pCallback->m_nCurProgress, pCallback->m_nMaxProgress );
            //            trace( buffer );
        }
    }
}

CSteamDLC::CSteamDLC()
    : m_bInitialized(false), m_DLCcount(0), m_CallbackOverlayActivated(this, &CSteamDLC::OnOverlayActivated)
{
}

CSteamDLC::~CSteamDLC()
{
}

void CSteamDLC::OnOverlayActivated(GameOverlayActivated_t *pCallback)
{
    char buffer[128];
    if (pCallback->m_bActive != 0)
    {
        isOverlayActivated = true;
    }
    else
    {
        isOverlayActivated = false;
    }
    VDATA *pvdat = core.Event("evntSteamOverlayActivated", "l", isOverlayActivated);
}

uint32_t CSteamDLC::getDLCCount()
{
    if (SteamUser() == nullptr)
    {
        return 0;
    }

    m_DLCcount = (uint32_t)SteamApps()->GetDLCCount();

    return m_DLCcount;
}

uint32_t CSteamDLC::bGetDLCDataByIndex(uint32_t iDLC)
{
    if (SteamUser() == nullptr)
    {
        return 0;
    }
    m_bInitialized = SteamApps()->BGetDLCDataByIndex(iDLC, &pAppID, &pbAvailable, pchName, 128);

    if (m_bInitialized)
        return (uint32_t)pAppID;

    return 0;
}

bool CSteamDLC::isDLCInstalled(uint32_t nDLC)
{
    if (SteamUser() == nullptr)
    {
        return false;
    }

    m_bInitialized = (SteamApps()->BIsSubscribedApp(nDLC) && SteamApps()->BIsDlcInstalled(nDLC));

    return m_bInitialized;
}

bool CSteamDLC::activateGameOverlay(uint32_t nAppId)
{
    if (SteamUser() == nullptr)
    {
        return false;
    }

    if (!SteamUtils()->IsOverlayEnabled())
    {
        return false;
    }

    SteamFriends()->ActivateGameOverlayToStore((AppId_t)nAppId, k_EOverlayToStoreFlag_None);

    return true;
}
