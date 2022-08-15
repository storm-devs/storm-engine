#pragma once

#include "c_vector.h"
#include "sound_defines.h"
#include "v_sound_service.h"
#include "vma.hpp"
#include "dx9render.h"
#include <fmod.hpp>

#include <string>

#define MAX_SOUNDS_SLOTS 4095

class INIFILE;
// debug....
class SoundVisualisationEntity;

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////

class SoundService : public VSoundService
{
    VDX9RENDER *rs = nullptr;

    bool bShowDebugInfo;
    bool initialized;

    FMOD::System *system;
    FMOD::Sound *OGG_sound[2];

    struct tSoundCache
    {
        uint32_t dwNameHash;
        std::string Name;
        FMOD::Sound *sound;
        float fTimeFromLastPlay;
        eSoundType type;

        tSoundCache()
            : type()
        {
            dwNameHash = 0;
            sound = nullptr;
            fTimeFromLastPlay = 0.0f;
        }
    };

    struct tPlayedSound
    {
        float fFaderNeedVolume;
        float fFaderCurrentVolume;
        float fFaderDeltaInSec;

        bool bFree;
        FMOD::Channel *channel;
        eVolumeType type;
        eSoundType sound_type;
        float fSoundVolume;

        // temp
        std::string Name;

        tPlayedSound()
            : sound_type(), fSoundVolume(0)
        {
            bFree = true;
            channel = nullptr;
            type = VOLUME_FX;

            fFaderNeedVolume = 0;
            fFaderCurrentVolume = 0;
            fFaderDeltaInSec = 0;
        }
    };

    tPlayedSound PlayingSounds[MAX_SOUNDS_SLOTS];
    TSD_ID SoundsActive{};

    struct PlayedOGG
    {
        std::string Name;
        uint32_t dwHash;
        unsigned int position;
    };

    std::vector<PlayedOGG> OGGPosition;

    unsigned int GetOGGPosition(const char *szName);
    void SetOGGPosition(const char *szName, unsigned int pos);
    int GetOGGPositionIndex(const char *szName);

    std::vector<tSoundCache> SoundCache;

    int GetFromCache(const char *szName, eSoundType _type);

    bool FaderParity;

    FMOD_VECTOR vListenerPos;
    FMOD_VECTOR vListenerForward;
    FMOD_VECTOR vListenerTop;

    void CreateEntityIfNeed();

    // Aliases ------------------------------------------------------------
    struct tAliasSound
    {
        std::string FileName;
        float fProbability;
    };

    struct tAlias
    {
        std::string Name;
        uint32_t dwNameHash;

        float fMaxProbabilityValue;
        float fMinDistance;
        float fMaxDistance;
        int32_t iPrior;
        float fVolume;
        std::vector<tAliasSound> SoundFiles;

        tAlias()
        {
        }
    };

    std::vector<tAlias> Aliases;

    const char *GetRandomName(tAlias *_alias) const;
    int GetAliasIndexByName(const char *szAliasName);
    void AnalyseNameStringAndAddToAlias(tAlias *_alias, const char *in_string) const;
    void AddAlias(INIFILE &_iniFile, char *_sectionName);
    void LoadAliasFile(const char *_filename) override;
    void InitAliases();

    // Sound Schemes------------------------------------------------------------
    struct tSoundSchemeChannel
    {
        TSD_ID SoundID;
        std::string soundName;
        int32_t minDelayTime;
        int32_t maxDelayTime;
        float volume;
        int32_t timeToNextPlay;
        bool looped; // not working

        tSoundSchemeChannel()
        {
            SoundID = 0;
        }
    };

    std::vector<tSoundSchemeChannel> SoundSchemeChannels;

    bool AddSoundSchemeChannel(char *in_string, bool _looped = false);
    void ProcessSoundSchemes();

    //----------------------------------------------------------------------------

    TSD_ID FindEmptySlot();

    float fFXVolume;
    float fMusicVolume;
    float fSpeechVolume;

    float fPitch;

    float fadeTimeInSeconds = 0.5f;

  public:
    SoundService();
    ~SoundService() override;
    bool SFLB_SetScheme(const char *_schemeName);
    bool Init() override;

    uint32_t RunSection() override
    {
        return SECTION_EXECUTE;
    }

    void RunStart() override;
    void RunEnd() override;

    TSD_ID SoundPlay(const char *_name, eSoundType _type, eVolumeType _volumeType, bool _simpleCache = false,
                     bool _looped = false, bool _cached = false, int32_t _time = 0,
                     const CVECTOR *_startPosition = nullptr, float _minDistance = -1.0f, float _maxDistance = -1.0f,
                     int32_t _loopPauseTime = 0, float _volume = 1.0f, int32_t _prior = 128) override;

    TSD_ID SoundDuplicate(TSD_ID _sourceID) override;
    void SoundSet3DParam(TSD_ID _id, eSoundMessage _message, const void *_op) override;
    void SoundStop(TSD_ID _id, int32_t _time = 0) override;
    void SoundRelease(TSD_ID _id) override;
    void SoundSetVolume(TSD_ID _id, float _volume) override;
    bool SoundIsPlaying(TSD_ID _id) override;
    float SoundGetPosition(TSD_ID _id) override;
    void SoundRestart(TSD_ID _id) override;
    void SoundResume(TSD_ID _id, int32_t _time = 0) override;

    // Service functions
    void SetMasterVolume(float _fxVolume, float _musicVolume, float _speechVolume) override;
    void GetMasterVolume(float *_fxVolume, float *_musicVolume, float *_speechVolume) override;
    void SetPitch(float _pitch) override;
    float GetPitch() override;
    void SetCameraPosition(const CVECTOR &_cameraPosition) override;
    void SetCameraOrientation(const CVECTOR &_nose, const CVECTOR &_head) override;

    // Schemes routines
    void ResetScheme() override;
    bool SetScheme(const char *_schemeName) override;
    bool AddScheme(const char *_schemeName) override;
    void SetEnabled(bool _enabled) override;

    void SetActiveWithFade(bool active) override;

    void DebugDraw();
    void DebugPrint3D(const CVECTOR &pos3D, float rad, int32_t line, float alpha, uint32_t color, float scale,
                      const char *format, ...) const;
    void Draw2DCircle(const CVECTOR &center, uint32_t dwColor, float fRadius, uint32_t dwColor2, float fRadius2) const;

    void ProcessFader(int idx);

    void FreeSound(TSD_ID idx);
    [[nodiscard]] bool IsFree(TSD_ID idx) const;
};
