#include "SteamApi.hpp"

#ifdef STORM_ENABLE_STEAM
#include <stdexcept>

#include <steam_api.h>

#include "achievements.hpp"
#include "core.h"

namespace steamapi
{

class SteamApiImpl final : public SteamApi
{
  public:
    SteamApiImpl();
    ~SteamApiImpl() override;

    bool isSteamEnabled() override;
    void InitAchievements() override;
    void DeleteAchievements() override;
    uint32_t SetAchievementState(const char *ID) override;
    uint32_t GetAchievementState(const char *ID) override;
    uint32_t SetStatValue(const char *ID, uint32_t Value) override;
    uint32_t GetStatValue(const char *ID) override;
    uint32_t StoreStats() override;
    bool isSteamConnected() override;
    bool ResetStats(bool bAchievementsToo) override;
    bool ClearAchievement(const char *ID) override;

    void InitSteamDLC() override;
    void DeleteSteamDLC() override;
    bool isDLCActive(uint32_t nDLC) override;
    uint32_t getDLCCount() override;
    uint32_t getDLCDataByIndex(uint32_t iDLC) override;
    bool activateGameOverlayDLC(uint32_t nAppId) override;
    void RunCallbacks() override;

  private:
    CSteamStatsAchievements *SteamAchievements_;
    CSteamDLC *SteamDLC_;
};

SteamApiImpl::SteamApiImpl()
{
    // TODO: handle outside
    try
    {
        if (SteamAPI_RestartAppIfNecessary(223330))
        {
            throw std::runtime_error("SteamAPI: unable to restart app");
        }

        if (!SteamAPI_Init())
        {
            throw std::runtime_error("SteamAPI: unable to init");
        }
    }
    catch (const std::exception &e)
    {
        core.Trace("%s", e.what());
    }
    SteamApiImpl::InitAchievements();
    SteamApiImpl::InitSteamDLC();
}

SteamApiImpl::~SteamApiImpl()
{
    SteamAPI_Shutdown();
    DeleteAchievements();
    DeleteSteamDLC();
}

bool SteamApiImpl::isSteamEnabled()
{
    return true;
}

void SteamApiImpl::InitAchievements()
{
    SteamAchievements_ = new CSteamStatsAchievements(ACHIEVEMENTS_NUM);
}

void SteamApiImpl::DeleteAchievements()
{
    delete SteamAchievements_;
}

uint32_t SteamApiImpl::SetAchievementState(const char *ID)
{
    return SteamAchievements_->SetAchievement(ID);
}

uint32_t SteamApiImpl::GetAchievementState(const char *ID)
{
    return SteamAchievements_->GetAchievement(ID);
}

uint32_t SteamApiImpl::SetStatValue(const char *ID, uint32_t Value)
{
    return SteamAchievements_->SetStat(ID, Value);
}

uint32_t SteamApiImpl::GetStatValue(const char *ID)
{
    return SteamAchievements_->GetStat(ID);
}

uint32_t SteamApiImpl::StoreStats()
{
    return SteamAchievements_->StoreStats();
}

bool SteamApiImpl::ResetStats(bool bAchievementsToo)
{
    return SteamAchievements_->ResetStats(bAchievementsToo);
}

bool SteamApiImpl::ClearAchievement(const char *ID)
{
    return SteamAchievements_->ClearAchievement(ID);
}

bool SteamApiImpl::isSteamConnected()
{
    return SteamAchievements_->GetConnected();
}

void SteamApiImpl::InitSteamDLC()
{
    SteamDLC_ = new CSteamDLC();
}

void SteamApiImpl::DeleteSteamDLC()
{
    delete SteamDLC_;
}

bool SteamApiImpl::isDLCActive(uint32_t nDLC)
{
    return SteamDLC_->isDLCInstalled(nDLC);
}

uint32_t SteamApiImpl::getDLCCount()
{
    return SteamDLC_->getDLCCount();
}

uint32_t SteamApiImpl::getDLCDataByIndex(uint32_t iDLC)
{
    return SteamDLC_->bGetDLCDataByIndex(iDLC);
}

bool SteamApiImpl::activateGameOverlayDLC(uint32_t nAppId)
{
    return SteamDLC_->activateGameOverlay(nAppId);
}
void SteamApiImpl::RunCallbacks()
{
    SteamAPI_RunCallbacks();
}

} // namespace steamapi
#endif

namespace steamapi::detail
{

std::unique_ptr<SteamApi> factory(const bool mock)
{
#ifdef STORM_ENABLE_STEAM
    if (!mock)
    {
        return std::make_unique<SteamApiImpl>();
    }
#endif
    return std::make_unique<SteamApi>();
}

} // namespace steamapi::detail