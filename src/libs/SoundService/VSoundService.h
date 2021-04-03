#pragma once

#include "Cvector.h"
#include "SoundDefines.h"
#include "VSoundService.h"
#include "service.h"
#include <vector>

///////////////////////////////////////////////////////////////////
// DEFINES & TYPES
///////////////////////////////////////////////////////////////////

typedef struct
{
    LPGUID guid;
    char description[COMMON_STRING_LENGTH];
    char driverName[COMMON_STRING_LENGTH];
} tSoundDriverDescriptor;

typedef uint32_t TSD_ID;

typedef struct
{
    long soundsCount, maxSoundsCount, bytesInBuffers, maxBytesInBuffers, bytesCached, maxBytesCached, totalMem, freeMem;
} tSoundStatistics;

///////////////////////////////////////////////////////////////////
// INTERFACE DEFINITION
///////////////////////////////////////////////////////////////////
class VSoundService : public SERVICE
{
  public:
    VSoundService();
    virtual ~VSoundService();
    bool Init() override = 0;
    uint32_t RunSection() override = 0;
    void RunStart() override = 0;

    // Sound entries functions
    //
    // + play returns id of new loaded sound
    // + in most functions _id = 0 equals "apply to all"
    // + play assumes _name a plain file name
    // + for now loop_pause_time works only for OGG-STEREO sounds
    ///////////////////////////////////////////////////////////////
    virtual TSD_ID SoundPlay(const char *_name,
                             eSoundType _type,          // sound type
                             eVolumeType _volumeType,   // volume type
                             bool _simpleCache = false, // cache only, not play
                             bool _looped = false,      // looped?
                             bool _cached = false,      // unload after stoppping?
                             long _time = 0,            // fade in, if _time > 0
                             const CVECTOR *_startPosition = nullptr, float _minDistance = -1.0f,
                             float _maxDistance = -1.0f, long _loopPauseTime = 0, float _volume = 1.0f,
                             long _prior = 128) = 0;

    virtual TSD_ID SoundDuplicate(TSD_ID _sourceID) = 0;
    virtual void SoundSet3DParam(TSD_ID _id, eSoundMessage _message, const void *_op) = 0;
    virtual void SoundStop(TSD_ID _id, long _time = 0) = 0;
    virtual void SoundRelease(TSD_ID _id) = 0;
    virtual void SoundSetVolume(TSD_ID _id, float _volume) = 0;
    virtual bool SoundIsPlaying(TSD_ID _id) = 0;
    virtual float SoundGetPosition(TSD_ID _id) = 0;
    virtual void SoundRestart(TSD_ID _id) = 0;
    virtual void SoundResume(TSD_ID _id, long _time = 0) = 0;

    // Service functions
    virtual void SetMasterVolume(float _fxVolume, float _musicVolume, float _speechVolume) = 0;
    virtual void GetMasterVolume(float *_fxVolume, float *_musicVolume, float *_speechVolume) = 0;
    virtual void SetCameraPosition(const CVECTOR &_cameraPosition) = 0;
    virtual void SetCameraOrientation(const CVECTOR &_nose, const CVECTOR &_head) = 0;

    virtual void ResetScheme() = 0;
    virtual bool SetScheme(const char *_schemeName) = 0;
    virtual bool AddScheme(const char *_schemeName) = 0;

    virtual void SetEnabled(bool _enabled) = 0;
    virtual void LoadAliasFile(const char *_filename) = 0;
    tSoundStatistics soundStatistics;
};

/*
API_SERVICE_START("sound service")
    DECLARE_MAIN_SERVICE(SoundService)
API_SERVICE_END(SoundService)
*/
