#include "sound_service.h"

#include <thread>

#include "entity.h"
#include "matrix.h"
#include "rands.h"
#include "v_file_service.h"
#include <fmod_errors.h>

// for debugging
#include "core.h"

#include "debug_entity.h"
#include "math_inlines.h"
#include "math3d/color.h"

CREATE_SERVICE(SoundService)

CREATE_CLASS(SoundVisualisationEntity)

#define DISTANCEFACTOR 1.0f
#define CHECKFMODERR(expr) ErrorHandler(expr, __FILE__, __LINE__, __func__, #expr)

namespace
{

FMOD_RESULT ErrorHandler(const FMOD_RESULT result, const char *file, unsigned line, const char *func,
                                       const char *expr)
{
    if (result != FMOD_OK)
    {
        core.Trace("[%s:%s:%d] %s (%s)", file, func, line, FMOD_ErrorString(result), expr);
    }
    return result;
}

} // namespace

SoundService::SoundService()
{
    fFXVolume = 0.5f;
    fMusicVolume = 0.5f;
    fSpeechVolume = 0.5f;

    fPitch = 1.0f;

    bShowDebugInfo = false;
    OGG_sound[0] = nullptr;
    OGG_sound[1] = nullptr;
    system = nullptr;
    initialized = false;

    vListenerPos.x = 0.0f;
    vListenerPos.y = 0.0f;
    vListenerPos.z = 0.0f;

    vListenerForward.x = 0.0f;
    vListenerForward.y = 0.0f;
    vListenerForward.z = 0.0f;

    vListenerTop.x = 0.0f;
    vListenerTop.y = 0.0f;
    vListenerTop.z = 0.0f;

    FaderParity = false;
}

SoundService::~SoundService()
{
    if (initialized)
    {
        CHECKFMODERR(system->close());
        CHECKFMODERR(system->release());
    }
}

bool SoundService::Init()
{
    initialized = false;

    rs = static_cast<VDX9RENDER *>(core.GetService("DX9RENDER"));

    CHECKFMODERR(FMOD::System_Create(&system));
    unsigned version;
    CHECKFMODERR(system->getVersion(&version));

    if (version < FMOD_VERSION)
    {
        core.Trace("Error : You are using old FMOD version %08x !\n", version);
        return false;
    }
    core.Trace("Using FMOD %08x", FMOD_VERSION);
    CHECKFMODERR(system->setSoftwareChannels(64));
    CHECKFMODERR(system->setOutput(FMOD_OUTPUTTYPE_AUTODETECT));
    CHECKFMODERR(system->init(MAX_SOUNDS_SLOTS, FMOD_INIT_NORMAL, nullptr));
    CHECKFMODERR(system->set3DSettings(1.0, DISTANCEFACTOR, 1.0f));

    if (const auto ini = fio->OpenIniFile(core.EngineIniFileName()))
    {
        fadeTimeInSeconds = ini->GetFloat("sound", "fade_time", 0.5f);
    }

    SoundsActive = 2; // 0 and 1 are special

    InitAliases();
    CreateEntityIfNeed();

    initialized = true;
    return true;
}

void SoundService::RunEnd()
{
    CreateEntityIfNeed();
    // Internal FMOD update
    CHECKFMODERR(system->update());
}

void SoundService::ProcessFader(int idx)
{
    if (IsFree(idx))
        return;

    if (fabsf(PlayingSounds[idx].fFaderCurrentVolume - PlayingSounds[idx].fFaderNeedVolume) < 0.001f)
    {
        if (PlayingSounds[idx].fFaderDeltaInSec < 0)
        {
            unsigned int OGGpos;
            PlayingSounds[idx].channel->getPosition(&OGGpos, FMOD_TIMEUNIT_MS);
            SetOGGPosition(PlayingSounds[idx].Name.c_str(), OGGpos);
            PlayingSounds[idx].channel->stop();
        }
        return;
    }

    const auto fTime = core.GetDeltaTime() * 0.001f;

    PlayingSounds[idx].fFaderCurrentVolume += PlayingSounds[idx].fFaderDeltaInSec * fTime;

    // FADE IN
    if (PlayingSounds[idx].fFaderDeltaInSec > 0)
    {
        if (PlayingSounds[idx].fFaderCurrentVolume > PlayingSounds[idx].fFaderNeedVolume)
        {
            PlayingSounds[idx].fFaderCurrentVolume = PlayingSounds[idx].fFaderNeedVolume;
        }
    }

    // FADE OUT
    if (PlayingSounds[idx].fFaderDeltaInSec < 0)
    {
        if (PlayingSounds[idx].fFaderCurrentVolume < PlayingSounds[idx].fFaderNeedVolume)
        {
            PlayingSounds[idx].fFaderCurrentVolume = PlayingSounds[idx].fFaderNeedVolume;
            unsigned int OGGpos;
            PlayingSounds[idx].channel->getPosition(&OGGpos, FMOD_TIMEUNIT_MS);
            SetOGGPosition(PlayingSounds[idx].Name.c_str(), OGGpos);

            PlayingSounds[idx].channel->stop();
        }
    }
    PlayingSounds[idx].channel->setVolume(PlayingSounds[idx].fFaderCurrentVolume);
}

void SoundService::FreeSound(const TSD_ID idx)
{
    PlayingSounds[idx].bFree = true;
    if (idx >= 2 && idx < SoundsActive)
    {
        std::swap(PlayingSounds[idx], PlayingSounds[SoundsActive - 1]);
        --SoundsActive;
    }
}

bool SoundService::IsFree(const TSD_ID idx) const
{
    return PlayingSounds[idx].bFree;
}

void SoundService::RunStart()
{
    CreateEntityIfNeed();

    if (rs)
    {
        static CVECTOR pos, ang, nose, head;
        static CMatrix view;

        rs->GetTransform(D3DTS_VIEW, view);
        view.Transposition();
        nose = view.Vz();
        head = view.Vy();
        pos = view.Pos();

        SetCameraPosition(pos);
        SetCameraOrientation(nose, head);
    }

    ProcessFader(0);
    ProcessFader(1);

    // release the sounds that have played ...
    for (TSD_ID i = 0; i < SoundsActive; i++)
    {
        if (IsFree(i))
        {
            continue;
        }

        // If it's just paused, don't need to touch it ...
        bool paused = true;

        FMOD_RESULT status = PlayingSounds[i].channel->getPaused(&paused);
        if (status != FMOD_OK)
        {
            if constexpr (TRACE_INFORMATION)
                core.Trace("PlayingSounds[%d].channel 0x%08X %s paused %d status %d", i, PlayingSounds[i].channel,
                           PlayingSounds[i].Name.c_str(), paused, status);
            FreeSound(i);
            --i;

            continue;
        }

        if (paused)
            continue;

        bool is_playing;
        status = CHECKFMODERR(PlayingSounds[i].channel->isPlaying(&is_playing));

        if (!is_playing || status != FMOD_OK)
        {
            PlayingSounds[i].channel = nullptr;

            if (i == 0)
            {
                if (OGG_sound[0] != nullptr)
                {
                    CHECKFMODERR(OGG_sound[0]->release());
                    OGG_sound[0] = nullptr;
                }
            }
            else if (i == 1)
            {
                if (OGG_sound[1] != nullptr)
                {
                    CHECKFMODERR(OGG_sound[1]->release());
                    OGG_sound[1] = nullptr;
                }
            }

            FreeSound(i);
            core.Event("SoundEnded", "l", i + 1);

            --i;
        }
    }
    ProcessSoundSchemes();
}

TSD_ID SoundService::FindEmptySlot()
{
    if (SoundsActive > MAX_SOUNDS_SLOTS)
    {
        core.Trace("SoundService::FindEmptySlot(): no empty slots");
        return -1;
    }

    return SoundsActive++;
}

const char *SoundService::GetRandomName(tAlias *_alias) const
{
    const auto randomFloat = rand(_alias->fMaxProbabilityValue);
    auto currentNameIndex = 0;
    auto currentFloat = 0.f;

    for (;;)
    {
        if (currentFloat > randomFloat)
            break;
        if ((currentNameIndex + 1) >= static_cast<int>(_alias->SoundFiles.size()))
            break;

        currentFloat += _alias->SoundFiles[currentNameIndex].fProbability;
        currentNameIndex++;
    }

    if (currentNameIndex >= static_cast<int>(_alias->SoundFiles.size()))
    {
        currentNameIndex = static_cast<int>(_alias->SoundFiles.size()) - 1;
    }

    return _alias->SoundFiles[currentNameIndex].FileName.c_str();
}

int SoundService::GetAliasIndexByName(const char *szAliasName)
{
    const uint32_t dwSearchHash = MakeHashValue(szAliasName);
    for (size_t i = 0; i < Aliases.size(); i++)
    {
        if (Aliases[i].dwNameHash == dwSearchHash)
        {
            if (Aliases[i].Name == szAliasName)
            {
                return i;
            }
        }
    }
    return -1;
}

TSD_ID SoundService::SoundPlay(const char *_name, eSoundType _type, eVolumeType _volumeType,
                               bool _simpleCache /* = false*/, bool _looped /* = false*/, bool _cached /* = false*/,
                               int32_t _time /* = 0*/, const CVECTOR *_startPosition /* = 0*/,
                               float _minDistance /* = -1.0f*/, float _maxDistance /* = -1.0f*/,
                               int32_t _loopPauseTime /* = 0*/, float _volume, /* = 1.0f*/
                               int32_t _prior)
{
    std::string FileName = _name;

    // aliases don`t contain `\`
    if (!strchr(_name, '\\'))
    {
        // Trying to find in aliases
        const auto AliasIdx = GetAliasIndexByName(_name);
        if (AliasIdx >= 0 && Aliases[AliasIdx].SoundFiles.size() > 0)
        {
            // play sound from the alias ...
            FileName = GetRandomName(&Aliases[AliasIdx]);
            if constexpr (TRACE_INFORMATION)
                core.Trace("Play sound from alias %s", FileName.c_str());

            _minDistance = Aliases[AliasIdx].fMinDistance;
            _maxDistance = Aliases[AliasIdx].fMaxDistance;
            _prior = Aliases[AliasIdx].iPrior;

            if (Aliases[AliasIdx].fVolume > 0.0f)
            {
                _volume = Aliases[AliasIdx].fVolume;
            }
        }
    }

    std::string SoundName = "resource\\sounds\\";
    SoundName += FileName;
    SoundName = fio->ConvertPathResource(SoundName.c_str());

    FMOD::Sound *sound = nullptr;
    auto SoundIdx = 0;

    if (_type == MP3_STEREO)
    {
        // play streamed immediately, without caching and always in 0 slot ...
        try
        {
            uint32_t dwMode = FMOD_LOOP_OFF;
            if (_looped)
                dwMode = FMOD_LOOP_NORMAL;
            const auto status = CHECKFMODERR(
                system->createStream(SoundName.c_str(), FMOD_CREATESAMPLE | FMOD_2D | dwMode, nullptr, &sound));
            if (status != FMOD_OK)
            {
                core.Trace("system->createStream(%s, FMOD_HARDWARE | FMOD_2D | dwMode, 0, &sound)", SoundName.c_str());
                return 0;
            }
        }
        catch (...)
        {
            core.Trace("Internal FMOD error, when create stream. File '%s'", SoundName.c_str());
            return 0;
        }

        auto MusicIdx = 0;
        auto OldMusicIdx = 1;
        if (FaderParity)
        {
            MusicIdx = 1;
            OldMusicIdx = 0;
        }
        FaderParity = !FaderParity;

        if (OGG_sound[OldMusicIdx])
        {
            SoundStop(OldMusicIdx + 1, _time);
        }

        SoundIdx = MusicIdx;
        OGG_sound[SoundIdx] = sound;
        PlayingSounds[SoundIdx].fFaderNeedVolume = _volume * fMusicVolume;
        PlayingSounds[SoundIdx].fFaderCurrentVolume = 0.0f;
        PlayingSounds[SoundIdx].fFaderDeltaInSec = (_volume * fMusicVolume) / (_time * 0.001f);
    }
    else
    {
        // For all other sounds, take from the cache
        const auto CacheIdx = GetFromCache(SoundName.c_str(), _type);
        if (CacheIdx < 0)
        {
            return 0;
        }

        SoundIdx = FindEmptySlot();
        if (SoundIdx < 0)
        {
            core.Trace("Can't find empty slots for sound !!!!");
            return 0;
        }
        sound = SoundCache[CacheIdx].sound;
    }

    //--------
    const TSD_ID SoundID = SoundIdx + 1;

    PlayingSounds[SoundIdx].type = _volumeType;
    PlayingSounds[SoundIdx].fSoundVolume = _volume;

    // start to play the sound, but paused ...
    const auto status = CHECKFMODERR(system->playSound(sound, nullptr, true, &PlayingSounds[SoundIdx].channel));
    if (status != FMOD_OK)
    {
        core.Trace("system->playSound(FMOD_CHANNEL_FREE, sound, true, &PlayingSounds[%d].channel)", SoundIdx);
    }

    if (SoundIdx <= 1)
    {
        const auto OGGpos = GetOGGPosition(SoundName.c_str());
        PlayingSounds[SoundIdx].channel->setPosition(OGGpos, FMOD_TIMEUNIT_MS);

        _prior = 0;
    }

    if (_prior < 0)
        _prior = 0;
    if (_prior > 255)
        _prior = 255;

    // put priority ...
    PlayingSounds[SoundIdx].channel->setPriority(_prior);

    // Adjust parameters for 3D channel ...
    if (_type == PCM_3D)
    {
        // If necessary, set the parameters by default ...
        if (_minDistance < 0.0f)
            _minDistance = 0.0f;
        if (_maxDistance < 0.0f)
            _maxDistance = 0.0f;

        CHECKFMODERR(PlayingSounds[SoundIdx].channel->set3DMinMaxDistance(_minDistance * DISTANCEFACTOR,
                                                                          _maxDistance * DISTANCEFACTOR));

        FMOD_VECTOR vVelocity = {0.0f, 0.0f, 0.0f};
        FMOD_VECTOR vPosition{};
        if (_startPosition != nullptr) {
            vPosition.x = _startPosition->x;
            vPosition.y = _startPosition->y;
            vPosition.z = _startPosition->z;
        }
        CHECKFMODERR(PlayingSounds[SoundIdx].channel->set3DAttributes(&vPosition, &vVelocity));
    }

    switch (_volumeType)
    {
    case VOLUME_FX:
        _volume *= fFXVolume;
        break;
    case VOLUME_MUSIC:
        _volume *= fMusicVolume;
        break;
    case VOLUME_SPEECH:
        _volume *= fSpeechVolume;
        break;
    default:
        _volume *= 1.0f;
        break;
    }

    if (_time <= 0)
    {
        CHECKFMODERR(PlayingSounds[SoundIdx].channel->setVolume(_volume));
    }
    else
    {
        CHECKFMODERR(PlayingSounds[SoundIdx].channel->setVolume(0));
    }

    CHECKFMODERR(PlayingSounds[SoundIdx].channel->setPitch(fPitch));

    PlayingSounds[SoundIdx].Name = std::move(SoundName);
    PlayingSounds[SoundIdx].sound_type = _type;

    // If not just caching ... then unpause ...
    if (!_simpleCache)
    {
        CHECKFMODERR(PlayingSounds[SoundIdx].channel->setPaused(false));
    }
    else
    {
        CHECKFMODERR(PlayingSounds[SoundIdx].channel->setPaused(true));
    }

    PlayingSounds[SoundIdx].bFree = false;
    // set the looping ..
    if (_looped)
    {
        CHECKFMODERR(PlayingSounds[SoundIdx].channel->setMode(FMOD_LOOP_NORMAL));
    }
    else
    {
        CHECKFMODERR(PlayingSounds[SoundIdx].channel->setMode(FMOD_LOOP_OFF));
    }

    // ---------- loop through all sounds looking for the one with the same channel --------------
    for (TSD_ID j = 0; j < MAX_SOUNDS_SLOTS; j++)
    {
        if (j == SoundIdx)
            continue;

        if (PlayingSounds[j].channel == PlayingSounds[SoundIdx].channel)
        {
            // note that the sound is thrown out ...
            // so as not to stop him ...
            FreeSound(j);
            --j;
        }
    }
    // Returning the sound ID ...
    return SoundID;
}

void SoundService::SoundSet3DParam(TSD_ID _id, eSoundMessage _message, const void *_op)
{
    if (_id == 0)
        return;
    _id--;

    if (IsFree(_id))
        return;

    auto vVelocity = CVECTOR(0.0f, 0.0f, 0.0f);

    switch (_message)
    {
    case SM_MAX_DISTANCE: {
        float maxDistance = *((float *)_op);
        CHECKFMODERR(PlayingSounds[_id].channel->set3DMinMaxDistance(NULL, maxDistance));
        break;
    }

    case SM_MIN_DISTANCE: {
        float minDistance = *((float *)_op);
        CHECKFMODERR(PlayingSounds[_id].channel->set3DMinMaxDistance(minDistance, NULL));
        break;
    }

    case SM_POSITION: {
        FMOD_VECTOR pos;
        auto *fPtr = (float *)_op;
        pos.x = *(fPtr + 0);
        pos.y = *(fPtr + 1);
        pos.z = *(fPtr + 2);
        FMOD_VECTOR vVelocity = {0.0f, 0.0f, 0.0f};
        CHECKFMODERR(PlayingSounds[_id].channel->set3DAttributes(&pos, &vVelocity));
        break;
    }
    }
}

void SoundService::SoundRestart(TSD_ID _id)
{
    if (_id == 0)
        return;
    _id--;
    if constexpr (TRACE_INFORMATION)
        core.Trace("Sound restart !");
}

void SoundService::SoundRelease(TSD_ID _id)
{
    if (_id == 0)
        return;
    _id--;
    if constexpr (TRACE_INFORMATION)
        core.Trace("Sound release !");
}

void SoundService::SoundSetVolume(TSD_ID _id, float _volume)
{
    if (_id == 0)
    {
        for (TSD_ID i = 0; i < SoundsActive; i++)
        {
            if (IsFree(i))
            {
                continue;
            }

            PlayingSounds[i].fSoundVolume = _volume;

            switch (PlayingSounds[i].type)
            {
            case VOLUME_FX:
                _volume *= fFXVolume;
                break;
            case VOLUME_MUSIC:
                _volume *= fMusicVolume;
                break;
            case VOLUME_SPEECH:
                _volume *= fSpeechVolume;
                break;
            default:
                _volume *= 1.0f;
                break;
            }

            if (i <= 1)
            {
                PlayingSounds[i].fFaderNeedVolume = _volume;
                PlayingSounds[i].fFaderCurrentVolume = PlayingSounds[i].fFaderNeedVolume;
            }

            CHECKFMODERR(PlayingSounds[i].channel->setVolume(_volume));
        }
        return;
    }

    _id--;
    if constexpr (TRACE_INFORMATION)
        core.Trace("Sound set volume !");
    if (_id <= 1)
    {
        if (fabsf(PlayingSounds[_id].fFaderCurrentVolume - PlayingSounds[_id].fFaderNeedVolume) > 0.001f)
        {
            return;
        }
    }

    if (IsFree(_id))
        return;

    PlayingSounds[_id].fSoundVolume = _volume;

    switch (PlayingSounds[_id].type)
    {
    case VOLUME_FX:
        _volume *= fFXVolume;
        break;
    case VOLUME_MUSIC:
        _volume *= fMusicVolume;
        break;
    case VOLUME_SPEECH:
        _volume *= fSpeechVolume;
        break;
    default:
        _volume *= 1.0f;
        break;
    }
    CHECKFMODERR(PlayingSounds[_id].channel->setVolume(_volume));
}

bool SoundService::SoundIsPlaying(TSD_ID _id)
{
    if (_id == 0)
        return false;
    _id--;
    return !IsFree(_id);
}

void SoundService::SoundResume(TSD_ID _id, int32_t _time /* = 0*/)
{
    if (_id == 0 || _id == 1 || _id == -1)
    {
        for (TSD_ID i = 0; i < SoundsActive; i++)
        {
            if (IsFree(i))
            {
                continue;
            }

            CHECKFMODERR(PlayingSounds[i].channel->setPaused(false));
        }
        return;
    }

    _id--;
    if (IsFree(_id))
    {
        core.Trace("Can't sound resume %d !!!! Sound is not playing !!!!", _id);
        return;
    }
    CHECKFMODERR(PlayingSounds[_id].channel->setPaused(false));
}

float SoundService::SoundGetPosition(TSD_ID _id)
{
    if (_id == 0)
        return 0;
    _id--;
    if (IsFree(_id))
        return 0;

    unsigned int SoundPositionInMilisecond;
    CHECKFMODERR(PlayingSounds[_id].channel->getPosition(&SoundPositionInMilisecond, FMOD_TIMEUNIT_MS));
    return 0;
}

void SoundService::SetCameraPosition(const CVECTOR &_cameraPosition)
{
    vListenerPos.x = _cameraPosition.x;
    vListenerPos.y = _cameraPosition.y;
    vListenerPos.z = _cameraPosition.z;

    CHECKFMODERR(system->set3DListenerAttributes(0, &vListenerPos, nullptr, nullptr, nullptr));
}

void SoundService::SetCameraOrientation(const CVECTOR &_nose, const CVECTOR &_head)
{
    const auto nose = !_nose;
    const auto head = !_head;

    vListenerForward.x = nose.x;
    vListenerForward.y = nose.y;
    vListenerForward.z = nose.z;

    vListenerTop.x = head.x;
    vListenerTop.y = head.y;
    vListenerTop.z = head.z;

    CHECKFMODERR(system->set3DListenerAttributes(0, nullptr, nullptr, &vListenerForward, &vListenerTop));
}

void SoundService::SetMasterVolume(float _fxVolume, float _musicVolume, float _speechVolume)
{
    if (_fxVolume < 0.0f)
        _fxVolume = 0.0f;
    if (_fxVolume > 1.0f)
        _fxVolume = 1.0f;

    if (_musicVolume < 0.0f)
        _musicVolume = 0.0f;
    if (_musicVolume > 1.0f)
        _musicVolume = 1.0f;

    if (_speechVolume < 0.0f)
        _speechVolume = 0.0f;
    if (_speechVolume > 1.0f)
        _speechVolume = 1.0f;

    fFXVolume = _fxVolume;
    fMusicVolume = _musicVolume;
    fSpeechVolume = _speechVolume;

    for (TSD_ID i = 0; i < SoundsActive; i++)
    {
        if (IsFree(i))
        {
            continue;
        }


        // update volume.....
        float _volume = PlayingSounds[i].fSoundVolume;

        switch (PlayingSounds[i].type)
        {
        case VOLUME_FX:
            _volume *= fFXVolume;
            break;
        case VOLUME_MUSIC:
            _volume *= fMusicVolume;
            break;
        case VOLUME_SPEECH:
            _volume *= fSpeechVolume;
            break;
        default:
            _volume *= 1.0f;
            break;
        }

        const auto status = CHECKFMODERR(PlayingSounds[i].channel->setVolume(_volume));
        if (status != FMOD_OK)
        {
            FreeSound(i);
            --i;
        }
    }
    if constexpr (TRACE_INFORMATION)
        core.Trace("Set master volume");
}

void SoundService::GetMasterVolume(float *_fxVolume, float *_musicVolume, float *_speechVolume)
{
    *_fxVolume = fFXVolume;
    *_musicVolume = fMusicVolume;
    *_speechVolume = fSpeechVolume;
    if constexpr (TRACE_INFORMATION)
        core.Trace("Get master volume");
}

void SoundService::SetPitch(float _pitch)
{
    fPitch = (_pitch >= 0.0f) ? _pitch : 0.0f;

    for (TSD_ID i = 0; i < SoundsActive; i++)
    {
        if (IsFree(i))
        {
            continue;
        }

        const auto status = CHECKFMODERR(PlayingSounds[i].channel->setPitch(fPitch));
        if (status != FMOD_OK)
        {
            FreeSound(i);
            --i;
        }
    }

    if constexpr (TRACE_INFORMATION)
        core.Trace("Set pitch");
}

float SoundService::GetPitch()
{
    if constexpr (TRACE_INFORMATION)
        core.Trace("Get pitch");
    return fPitch;
}

TSD_ID SoundService::SoundDuplicate(TSD_ID _sourceID)
{
    _sourceID--;
    core.Trace("Sound duplicate");
    return 0;
}

void SoundService::SetEnabled(bool _enabled)
{
}

void SoundService::SetActiveWithFade(const bool active)
{
    if (fadeTimeInSeconds == 0.0f)
    {
        return;
    }

    if (active)
    {
        system->mixerResume();
    }

    FMOD::ChannelGroup* mastergroup;
    CHECKFMODERR(system->getMasterChannelGroup(&mastergroup));
    unsigned long long parentclock; // type should match with getDSPClock param from fmod.hpp
    int rate;
    system->getSoftwareFormat(&rate, nullptr, nullptr);
    mastergroup->getDSPClock(nullptr, &parentclock);

    const auto dsp_clock_start = parentclock;
    const auto dsp_clock_end = static_cast<unsigned long long>(parentclock + fadeTimeInSeconds * rate);

    if (active)
    {
        mastergroup->setDelay(dsp_clock_start, 0, false);
    }

    for (const auto& PlayingSound : PlayingSounds)
    {
        if (PlayingSound.bFree)
            continue;

        if (active)
        {
            PlayingSound.channel->addFadePoint(dsp_clock_start, 0.0f);
            PlayingSound.channel->addFadePoint(dsp_clock_end, 1.0f);
        }
        else
        {
            PlayingSound.channel->addFadePoint(dsp_clock_start, 1.0f);
            PlayingSound.channel->addFadePoint(dsp_clock_end, 0.0f);
        }
    }

    if (!active)
    {
        mastergroup->setDelay(dsp_clock_start, dsp_clock_end, false);
        system->mixerSuspend();
    }
}

void SoundService::SoundStop(TSD_ID _id, int32_t _time)
{
    bool is_playing; // boal fix

    if (_id == 0)
    {
        // --------- remove all sounds -----------------------------------------
        int start = 0;
        if (_time > 0)
        {
            float fVol = 0.0f;
            PlayingSounds[0].channel->getVolume(&fVol);
            PlayingSounds[0].fFaderNeedVolume = 0.0f;
            PlayingSounds[0].fFaderCurrentVolume = fVol;
            PlayingSounds[0].fFaderDeltaInSec = -fVol / (_time * 0.001f);

            if (PlayingSounds[0].fFaderDeltaInSec <= 0.00001f)
            {
                if (OGG_sound[0])
                {
                    CHECKFMODERR(OGG_sound[0]->release());
                    OGG_sound[0] = nullptr;
                }
            }

            PlayingSounds[1].channel->getVolume(&fVol);
            PlayingSounds[1].fFaderNeedVolume = 0.0f;
            PlayingSounds[1].fFaderCurrentVolume = fVol;
            PlayingSounds[1].fFaderDeltaInSec = -fVol / (_time * 0.001f);

            if (PlayingSounds[1].fFaderDeltaInSec <= 0.00001f)
            {
                if (OGG_sound[1])
                {
                    CHECKFMODERR(OGG_sound[1]->release());
                    OGG_sound[1] = nullptr;
                }
            }
            start = 2;
        }
        else
        {
            start = 0;
        }

     for (TSD_ID i = 0; i < SoundsActive; i++)
        {
            if (IsFree(i))
            {
                continue;
            }


            if (i <= 1)
            {
                unsigned int OGGpos;
                PlayingSounds[i].channel->getPosition(&OGGpos, FMOD_TIMEUNIT_MS);
                SetOGGPosition(PlayingSounds[i].Name.c_str(), OGGpos);
            }

            FMOD_RESULT status = PlayingSounds[i].channel->isPlaying(&is_playing);
            if (!is_playing || status != FMOD_OK) // boal fix
            {
                if constexpr (TRACE_INFORMATION)
                    core.Trace("PlayingSounds[%d].channel 0x%08X %s status %d", i, PlayingSounds[i].channel,
                               PlayingSounds[i].Name.c_str(), status);

                FreeSound(i);
                --i;
            }
            else
            {
                CHECKFMODERR(PlayingSounds[i].channel->stop());
            }
        }

        if (_time <= 0)
        {
            if (OGG_sound[0])
            {
                CHECKFMODERR(OGG_sound[0]->release());
                OGG_sound[0] = nullptr;
            }

            if (OGG_sound[1])
            {
                CHECKFMODERR(OGG_sound[1]->release());
                OGG_sound[1] = nullptr;
            }
        }
        // --------- remove all sounds -----------------------------------------
        return;
    }

    _id--;

    // Delete the selected
    if (IsFree(_id))
        return;

    if (_time > 0)
    {
        float fVol = 0.0f;
        PlayingSounds[_id].channel->getVolume(&fVol);
        PlayingSounds[_id].fFaderNeedVolume = 0.0f;
        PlayingSounds[_id].fFaderCurrentVolume = fVol;
        PlayingSounds[_id].fFaderDeltaInSec = -fVol / (_time * 0.001f);
    }
    else
    {
        if (_id <= 1)
        {
            unsigned int OGGpos;
            PlayingSounds[_id].channel->getPosition(&OGGpos, FMOD_TIMEUNIT_MS);
            SetOGGPosition(PlayingSounds[_id].Name.c_str(), OGGpos);
        }

        auto status = FMOD_OK;
        if (!IsFree(_id)) // boal fix
        {
            status = CHECKFMODERR(PlayingSounds[_id].channel->isPlaying(&is_playing));
            if (!is_playing || status != FMOD_OK) // boal fix
            {
                FreeSound(_id);
            }
            else
            {
                status = CHECKFMODERR(PlayingSounds[_id].channel->stop());
            }
        }

        if (_id == 0 && OGG_sound[0] && status == FMOD_OK)
        {
            CHECKFMODERR(OGG_sound[0]->release());
            OGG_sound[0] = nullptr;
        }

        if (_id == 1 && OGG_sound[1] && status == FMOD_OK)
        {
            CHECKFMODERR(OGG_sound[1]->release());
            OGG_sound[1] = nullptr;
        }
    }
}

void SoundService::AnalyseNameStringAndAddToAlias(tAlias *_alias, const char *in_string) const
{
    static char tempString2[COMMON_STRING_LENGTH];
    strncpy_s(tempString2, in_string, COMMON_STRING_LENGTH);

    tAliasSound snd;

    char *col = strchr(tempString2, ',');
    if (!col)
    {
        // only name, without delays
        _alias->fMaxProbabilityValue += DEFAULT_PROBABILITY;
        snd.fProbability = DEFAULT_PROBABILITY;
        snd.FileName = tempString2;

        if constexpr (TRACE_INFORMATION)
            core.Trace("  -> sound %s", snd.FileName.c_str());
        _alias->SoundFiles.push_back(snd);

        return;
    }

    // try to convert forthcoming number
    float n1;
    if (sscanf(++col, "%f", &n1) == 1)
    {
        snd.fProbability = n1;
        _alias->fMaxProbabilityValue += n1;
    }

    *(--col) = 0; // truncate at first ','
    snd.FileName = tempString2;
    if constexpr (TRACE_INFORMATION)
        core.Trace("  -> sound %s, %f", snd.FileName.c_str(), snd.fProbability);
    _alias->SoundFiles.push_back(snd);
}

void SoundService::AddAlias(INIFILE &_iniFile, char *_sectionName)
{
    static char tempString[COMMON_STRING_LENGTH];
    if (!_sectionName)
        return;

    if constexpr (TRACE_INFORMATION)
        core.Trace("Add sound alias %s", _sectionName);
    Aliases.push_back(tAlias{});
    tAlias &alias = Aliases.back();
    alias.Name = _sectionName;
    alias.dwNameHash = MakeHashValue(alias.Name.c_str());
    alias.fMaxDistance = _iniFile.GetFloat(_sectionName, "maxDistance", -1.0f);
    alias.fMinDistance = _iniFile.GetFloat(_sectionName, "minDistance", -1.0f);
    alias.fVolume = _iniFile.GetFloat(_sectionName, "volume", -1.0f);
    alias.iPrior = _iniFile.GetInt(_sectionName, "prior", 128);
    alias.fMaxProbabilityValue = 0.0f;

    if (_iniFile.ReadString(_sectionName, "name", tempString, COMMON_STRING_LENGTH, ""))
    {
        AnalyseNameStringAndAddToAlias(&alias, tempString);
        while (_iniFile.ReadStringNext(_sectionName, "name", tempString, COMMON_STRING_LENGTH))
            AnalyseNameStringAndAddToAlias(&alias, tempString);
    }
}

void SoundService::LoadAliasFile(const char *_filename)
{
    const static int SECTION_NAME_LENGTH = 128;
    static char sectionName[SECTION_NAME_LENGTH];

    std::string iniName = ALIAS_DIRECTORY;
    iniName += _filename;

    if constexpr (TRACE_INFORMATION)
        core.Trace("Find sound alias file %s", iniName.c_str());
    auto aliasIni = fio->OpenIniFile(iniName.c_str());
    if (!aliasIni)
        return;

    if (aliasIni->GetSectionName(sectionName, SECTION_NAME_LENGTH))
    {
        AddAlias(*aliasIni, sectionName);
        while (aliasIni->GetSectionNameNext(sectionName, SECTION_NAME_LENGTH))
        {
            AddAlias(*aliasIni, sectionName);
        }
    }
}

void SoundService::InitAliases()
{
    const auto vFilenames = fio->_GetPathsOrFilenamesByMask(ALIAS_DIRECTORY, "*.ini", false);
    for (std::string curName : vFilenames)
    {
        LoadAliasFile(curName.c_str());
    }
}

void SoundService::CreateEntityIfNeed()
{
    auto Debugentid_t = core.GetEntityId("SoundVisualisationEntity");
    if (!Debugentid_t)
    {
        Debugentid_t = core.CreateEntity("SoundVisualisationEntity");
        auto *pDebugEntity = static_cast<SoundVisualisationEntity *>(core.GetEntityPointer(Debugentid_t));
        pDebugEntity->SetMasterSoundService(this);
        pDebugEntity->Wakeup();
    }
}

void SoundService::DebugDraw()
{
    if (core.Controls->GetDebugAsyncKeyState('J') < 0)
    {
        bShowDebugInfo = !bShowDebugInfo;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    if (!bShowDebugInfo)
        return;

    bool bShowDistances = true;

    int Ypos = 38 + 16;
    int Count = 0;
    int RealCount = 0;

    FMOD_VECTOR lpos, lvel, lforward, lup;
    system->get3DListenerAttributes(0, &lpos, &lvel, &lforward, &lup);

    auto vListener = CVECTOR(lpos.x, lpos.y, lpos.z);

    FMOD_CPU_USAGE usage;
    system->getCPUUsage(&usage);
    float fTotal = usage.dsp + usage.stream + usage.geometry + usage.update + usage.convolution1 + usage.convolution1;

    int CurrentAlloc, PeakAlloc;
    FMOD::Memory_GetStats(&CurrentAlloc, &PeakAlloc);

    CVECTOR list_pos;
    list_pos.x = lpos.x;
    list_pos.y = lpos.y;
    list_pos.z = lpos.z;
    // rs->DrawSphere(list_pos, 4.0f, 0xFF008000);

    rs->Print(0, 0, "CPU Usage %3.2f Mem: %3.2f Kb, MemPeak %3.2f Kb, Cached %d sounds", fTotal, CurrentAlloc / 1024.0f,
              PeakAlloc / 1024.0f, SoundCache.size());
    rs->Print(0, 16, "position  %3.2f, %3.2f, %3.2f, forward %3.2f, %3.2f, %3.2f, up %3.2f, %3.2f, %3.2f", lpos.x,
              lpos.y, lpos.z, lforward.x, lforward.y, lforward.z, lup.x, lup.y, lup.z);

    CMatrix ind;
    ind.SetIdentity();
    rs->SetWorld(ind);

    for (TSD_ID i = 0; i < SoundsActive; i++)
    {
        if (IsFree(i))
        {
            continue;
        }

        bool bVirtual;
        PlayingSounds[i].channel->isVirtual(&bVirtual);

        bool paused;
        PlayingSounds[i].channel->getPaused(&paused);

        float fMin, fMax;
        PlayingSounds[i].channel->get3DMinMaxDistance(&fMin, &fMax);

        FMOD_MODE sound_mode;
        PlayingSounds[i].channel->getMode(&sound_mode);
        bool bIsLooped = false;
        if ((sound_mode & FMOD_LOOP_NORMAL) != 0)
            bIsLooped = true;

        float fVol;
        PlayingSounds[i].channel->getVolume(&fVol);

        bool bPlaying;
        PlayingSounds[i].channel->isPlaying(&bPlaying);

        unsigned int position;
        PlayingSounds[i].channel->getPosition(&position, FMOD_TIMEUNIT_MS);

        if (!bVirtual)
            RealCount++;
        Count++;

        FMOD_VECTOR pos, vel;
        PlayingSounds[i].channel->get3DAttributes(&pos, &vel);

        float audib = 0;
        PlayingSounds[i].channel->getAudibility(&audib);

        int prior;
        PlayingSounds[i].channel->getPriority(&prior);

        if (PlayingSounds[i].sound_type == PCM_3D)
        {
            // 0xFFFFFF00 plays but cannot be heard
            // 0xFFFF0000 not playing
            // 0xFF00FF00 is playing

            auto drawColor = Color(0xFFFF0000);

            if (!bVirtual)
            {
                // morph between yellow audib 0 and green audib 1 colors
                auto Zero = Color(0xFFFFFF00);
                auto Full = Color(0xFF00FF00);
                drawColor.Lerp(Zero, Full, audib);
            }

            CVECTOR vec_pos;
            vec_pos.x = pos.x;
            vec_pos.y = pos.y;
            vec_pos.z = pos.z;
            rs->DrawSphere(vec_pos, 0.2f, drawColor.GetDword());

            if (bShowDistances && audib > 0.001f)
            {
                rs->SetWorld(ind);
                Draw2DCircle(vec_pos, drawColor.GetDword(), fMin, drawColor.GetDword(), fMax);
            }

            float fDistance = CVECTOR(vListener - vec_pos).GetLength();

            DebugPrint3D(vec_pos, 30.0f, 2, 1.0f, drawColor.GetDword(), 1.0f, "%s", PlayingSounds[i].Name.c_str());
            DebugPrint3D(vec_pos, 30.0f, 0, 1.0f, drawColor.GetDword(), 1.0f, "%3.2f m", fDistance);
            DebugPrint3D(vec_pos, 30.0f, 4, 1.0f, drawColor.GetDword(), 1.0f, "prior: %d", prior);
        }
        else
        {
            rs->Print(0, Ypos,
                      "[%d] [vol:%f] [Pos:%d] [Loop:%d] [Play:%d] sound [Pause:%d][Vir:%d][Aud:%f] '%s' 0x%08X", i,
                      fVol, position, bIsLooped, bPlaying, paused, bVirtual, audib, PlayingSounds[i].Name.c_str(),
                      PlayingSounds[i].channel);
            Ypos += 16;
        }

        // rs->Print(0, Ypos, "pos %f, %f, %f", pos.x, pos.y, pos.z);
        // Ypos += 16;
        // rs->Print(0, Ypos, "min %f,  max %f", fMin, fMax);
        // Ypos += 16;

        // rs->DrawSphere(vec_pos, fMax, 0xFFFF0000);
    }

    rs->Print(0, 32, "Real sounds %d, Total sounds %d", RealCount, Count);
    rs->Print(800, 0, "Sound schemes %d", SoundSchemeChannels.size());
    Ypos = 16;
    for (size_t i = 0; i < SoundSchemeChannels.size(); i++)
    {
        rs->Print(810, Ypos, "[%d] %s", i, SoundSchemeChannels[i].soundName.c_str());
        Ypos += 16;
    }
}

int SoundService::GetFromCache(const char *szName, eSoundType _type)
{
    const uint32_t dwSearchHash = MakeHashValue(szName);

    for (size_t i = 0; i < SoundCache.size(); i++)
    {
        if (SoundCache[i].type != _type)
            continue;
        if (SoundCache[i].dwNameHash == dwSearchHash)
        {
            if (SoundCache[i].Name == szName)
            {
                SoundCache[i].fTimeFromLastPlay = 0.0;
                return i;
            }
        }
    }

    FMOD_MODE mode = FMOD_DEFAULT;
    if (_type == PCM_3D)
    {
        mode = mode | FMOD_3D | FMOD_3D_LINEARROLLOFF;
    }

    if (_type == PCM_STEREO)
    {
        mode = mode | FMOD_2D;
    }

    tSoundCache Cache;
    CHECKFMODERR(system->createSound(szName, mode, nullptr, &Cache.sound));

    if (Cache.sound == nullptr)
    {
        core.Trace("Problem with sound loading !!! '%s'", szName);
        return -1;
    }
    Cache.type = _type;
    Cache.Name = szName;
    Cache.dwNameHash = dwSearchHash;
    Cache.fTimeFromLastPlay = 0.0f;

    SoundCache.push_back(Cache);

    return (SoundCache.size() - 1);
}

// Write text
void SoundService::DebugPrint3D(const CVECTOR &pos3D, float rad, int32_t line, float alpha, uint32_t color, float scale,
                                const char *format, ...) const
{
    static char buf[256];
    // print to the buffer
    va_list args;
    va_start(args, format);
    int32_t len = vsnprintf(buf, sizeof(buf) - 1, format, args);
    va_end(args);
    buf[sizeof(buf) - 1] = 0;
    // Looking for a point position on the screen
    static CMatrix mtx, view, prj;
    static D3DVIEWPORT9 vp;
    MTX_PRJ_VECTOR vrt;
    rs->GetTransform(D3DTS_VIEW, view);
    rs->GetTransform(D3DTS_PROJECTION, prj);
    mtx.EqMultiply(view, prj);
    view.Transposition();
    float dist = ~(pos3D - view.Pos());
    if (dist >= rad * rad)
        return;
    const float d = view.Vz() | view.Pos();
    if ((pos3D | view.Vz()) < d)
        return;
    rs->GetViewport(&vp);
    mtx.Projection((CVECTOR *)&pos3D, &vrt, 1, vp.Width * 0.5f, vp.Height * 0.5f, sizeof(CVECTOR),
                   sizeof(MTX_PRJ_VECTOR));
    // Looking for a position
    const int32_t fh = rs->CharHeight(FONT_DEFAULT) / 2;
    vrt.y -= (line + 0.5f) * fh;
    // Transparency
    const float kDist = 0.75f;
    if (alpha < 0.0f)
        alpha = 0.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;
    if (dist > kDist * kDist * rad * rad)
    {
        dist = 1.0f - (sqrtf(dist) - kDist * rad) / (rad - kDist * rad);
        alpha *= dist;
    }
    if (alpha <= 0.0f)
        return;
    color = (static_cast<uint32_t>(alpha * 255.0f) << 24) | (color & 0xffffff);
    // print the text
    rs->ExtPrint(FONT_DEFAULT, color, 0x00000000, PR_ALIGN_CENTER, false, scale, 0, 0, static_cast<int32_t>(vrt.x),
                 static_cast<int32_t>(vrt.y), buf);
}

void SoundService::Draw2DCircle(const CVECTOR &center, uint32_t dwColor, float fRadius, uint32_t dwColor2,
                                float fRadius2) const
{
    float fDelta = 0.2f;
    RS_LINE line[2];
    CVECTOR vStart;
    CVECTOR vEnd;
    CVECTOR vStartPoint;

    vStartPoint = CVECTOR(cosf(0) * fRadius, 0.0f, sinf(0) * fRadius);
    vStartPoint += center;
    for (float Angle = fDelta; Angle <= (PI * 2); Angle += fDelta)
    {
        auto vPoint = CVECTOR(cosf(Angle) * fRadius, 0.0f, sinf(Angle) * fRadius);
        vPoint += center;

        vStart = vStartPoint;
        vEnd = vPoint;

        line[0].vPos = vStart;
        line[0].dwColor = dwColor;
        line[1].vPos = vEnd;
        line[1].dwColor = dwColor;
        rs->DrawLines(line, 1, "Line");
        // rs->DrawVector(vStart, vEnd, dwColor);
        vStartPoint = vPoint;
    }

    vStart = vStartPoint;
    vEnd = CVECTOR(cosf(0) * fRadius, 0.0f, sinf(0) * fRadius);
    vEnd += center;

    line[0].vPos = vStart;
    line[0].dwColor = dwColor;
    line[1].vPos = vEnd;
    line[1].dwColor = dwColor;
    rs->DrawLines(line, 1, "Line");

    vStartPoint = CVECTOR(cosf(0) * fRadius2, 0.0f, sinf(0) * fRadius2);
    vStartPoint += center;
    for (float Angle = fDelta; Angle <= (PI * 2); Angle += fDelta)
    {
        auto vPoint = CVECTOR(cosf(Angle) * fRadius2, 0.0f, sinf(Angle) * fRadius2);
        vPoint += center;

        vStart = vStartPoint;
        vEnd = vPoint;

        line[0].vPos = vStart;
        line[0].dwColor = dwColor2;
        line[1].vPos = vEnd;
        line[1].dwColor = dwColor2;
        rs->DrawLines(line, 1, "Line");
        // rs->DrawVector(vStart, vEnd, dwColor);
        vStartPoint = vPoint;
    }

    vStart = vStartPoint;
    vEnd = CVECTOR(cosf(0) * fRadius2, 0.0f, sinf(0) * fRadius2);
    vEnd += center;

    line[0].vPos = vStart;
    line[0].dwColor = dwColor2;
    line[1].vPos = vEnd;
    line[1].dwColor = dwColor2;
    rs->DrawLines(line, 1, "Line");

    line[0].vPos = CVECTOR(fRadius, 0.0f, 0.0f) + center;
    line[0].dwColor = dwColor;
    line[1].vPos = CVECTOR(fRadius2, 0.0f, 0.0f) + center;
    line[1].dwColor = dwColor;
    rs->DrawLines(line, 1, "Line");

    line[0].vPos = CVECTOR(-fRadius, 0.0f, 0.0f) + center;
    line[0].dwColor = dwColor;
    line[1].vPos = CVECTOR(-fRadius2, 0.0f, 0.0f) + center;
    line[1].dwColor = dwColor;
    rs->DrawLines(line, 1, "Line");

    line[0].vPos = CVECTOR(0.0f, 0.0f, fRadius) + center;
    line[0].dwColor = dwColor;
    line[1].vPos = CVECTOR(0.0f, 0.0f, fRadius2) + center;
    line[1].dwColor = dwColor;
    rs->DrawLines(line, 1, "Line");

    line[0].vPos = CVECTOR(0.0f, 0.0f, -fRadius) + center;
    line[0].dwColor = dwColor;
    line[1].vPos = CVECTOR(0.0f, 0.0f, -fRadius2) + center;
    line[1].dwColor = dwColor;
    rs->DrawLines(line, 1, "Line");

    // rs->DrawVector(vStart, vEnd, dwColor);
}

//--------------------------------------------------------------------
bool SoundService::SFLB_SetScheme(const char *_schemeName)
{
    ResetScheme();
    return AddScheme(_schemeName);
}

bool SoundService::SetScheme(const char *_schemeName)
{
    return SFLB_SetScheme(_schemeName);
}

//--------------------------------------------------------------------
void SoundService::ResetScheme()
{
    SoundSchemeChannels.clear();
}

//--------------------------------------------------------------------
bool SoundService::AddScheme(const char *_schemeName)
{
    static char tempString[COMMON_STRING_LENGTH];
    auto ini = fio->OpenIniFile(SCHEME_INI_NAME);

    if (!ini)
        return false;

    if (ini->ReadString(const_cast<char *>(_schemeName), SCHEME_KEY_NAME, tempString, COMMON_STRING_LENGTH, ""))
    {
        AddSoundSchemeChannel(tempString);
        while (ini->ReadStringNext(const_cast<char *>(_schemeName), SCHEME_KEY_NAME, tempString, COMMON_STRING_LENGTH))
            AddSoundSchemeChannel(tempString);
    }

    if (ini->ReadString(const_cast<char *>(_schemeName), SCHEME_KEY_NAME_LOOP, tempString, COMMON_STRING_LENGTH, ""))
    {
        AddSoundSchemeChannel(tempString, true);
        while (ini->ReadStringNext(const_cast<char *>(_schemeName), SCHEME_KEY_NAME_LOOP, tempString,
                                   COMMON_STRING_LENGTH))
            AddSoundSchemeChannel(tempString, true);
    }

    return true;
}

bool SoundService::AddSoundSchemeChannel(char *in_string, bool _looped /*= false*/)
{
    static char tempString2[COMMON_STRING_LENGTH];
    strncpy_s(tempString2, in_string, COMMON_STRING_LENGTH);

    char *col = strchr(tempString2, ',');
    if (!col)
    {
        // only name, without delays
        tSoundSchemeChannel NewChannel;
        NewChannel.soundName = tempString2;
        NewChannel.minDelayTime = SCHEME_MIN_DELAY;
        NewChannel.maxDelayTime = SCHEME_MAX_DELAY;
        NewChannel.timeToNextPlay =
            static_cast<int32_t>(rand(static_cast<float>(NewChannel.maxDelayTime - NewChannel.minDelayTime)));
        NewChannel.volume = 1.f;
        NewChannel.looped = _looped;
        if (_looped)
            NewChannel.timeToNextPlay = 0;

        SoundSchemeChannels.push_back(NewChannel);
        return true;
    }

    tSoundSchemeChannel NewChannel;

    // try to convert forthcoming numbers
    int n1, n2;
    float f1;
    const int numbersConverted = sscanf(++col, "%d, %d, %f", &n1, &n2, &f1);
    switch (numbersConverted)
    {
    case 1:
        NewChannel.minDelayTime = SCHEME_MIN_DELAY;
        NewChannel.maxDelayTime = n1 * 1000;
        NewChannel.volume = 1.f;
        break;
    case 2:
        NewChannel.minDelayTime = n1 * 1000;
        NewChannel.maxDelayTime = n2 * 1000;
        NewChannel.volume = 1.f;
        break;
    case 3:
        NewChannel.minDelayTime = n1 * 1000;
        NewChannel.maxDelayTime = n2 * 1000;
        NewChannel.volume = f1;
        break;
    }
    *(--col) = 0; // truncate at first ','
    NewChannel.soundName = tempString2;
    NewChannel.looped = _looped;

    if (_looped)
        NewChannel.timeToNextPlay = 0;
    else
        NewChannel.timeToNextPlay =
            static_cast<int32_t>(rand(static_cast<float>(NewChannel.maxDelayTime - NewChannel.minDelayTime)));

    SoundSchemeChannels.push_back(NewChannel);
    return true;
}

void SoundService::ProcessSoundSchemes()
{
    // handle schemes
    const uint32_t dTime = core.GetDeltaTime();

    for (size_t i = 0; i < SoundSchemeChannels.size(); i++)
    {
        if (SoundSchemeChannels[i].looped)
        {
            if (SoundSchemeChannels[i].timeToNextPlay)
                continue;
            SoundSchemeChannels[i].timeToNextPlay = -1;
            SoundPlay(SoundSchemeChannels[i].soundName.c_str(), PCM_STEREO, VOLUME_FX, false, true, false, 0, nullptr,
                      -1.f, -1.f, 0, SoundSchemeChannels[i].volume);
        }
        else
        {
            if (SoundSchemeChannels[i].timeToNextPlay > 0)
            {
                SoundSchemeChannels[i].timeToNextPlay -= dTime;
            }
            else
            {
                SoundSchemeChannels[i].timeToNextPlay =
                    SoundSchemeChannels[i].minDelayTime +
                    static_cast<int32_t>(rand(
                        static_cast<float>(SoundSchemeChannels[i].maxDelayTime - SoundSchemeChannels[i].minDelayTime)));
                SoundPlay(SoundSchemeChannels[i].soundName.c_str(), PCM_STEREO, VOLUME_FX, false, false, false, 0,
                          nullptr, -1.f, -1.f, 0, SoundSchemeChannels[i].volume);
            }
        } // looped
    }
}

int SoundService::GetOGGPositionIndex(const char *szName)
{
    const uint32_t dwHash = MakeHashValue(szName);

    for (size_t i = 0; i < OGGPosition.size(); i++)
    {
        if (OGGPosition[i].dwHash == dwHash)
        {
            if (OGGPosition[i].Name == szName)
            {
                return i;
            }
        }
    }
    return -1;
}

unsigned int SoundService::GetOGGPosition(const char *szName)
{
    const int idx = GetOGGPositionIndex(szName);
    if (idx >= 0)
    {
        return OGGPosition[idx].position;
    }
    return 0;
}

void SoundService::SetOGGPosition(const char *szName, unsigned int pos)
{
    const int idx = GetOGGPositionIndex(szName);
    if (idx >= 0)
    {
        OGGPosition[idx].position = pos;
        return;
    }

    PlayedOGG ogg;
    ogg.dwHash = MakeHashValue(szName);
    ogg.Name = szName;
    ogg.position = pos;

    OGGPosition.push_back(ogg);
}
