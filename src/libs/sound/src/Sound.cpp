#include "VSoundService.h"
#include "shared/messages.h"

#include "Sound.h"

#include "core.h"

CREATE_CLASS(SOUND)

#define MSG_SOUND_ALIAS_ADD 77017 //"s"          alias_name

//--------------------------------------------------------------------
SOUND::SOUND() : soundService(nullptr)
{
}

//--------------------------------------------------------------------
SOUND::~SOUND()
{
}

//--------------------------------------------------------------------
bool SOUND::Init()
{
    // GUARD(SOUND::Init)

    soundService = static_cast<VSoundService *>(core.CreateService("SoundService"));
    if (!soundService)
        core.Trace("!SOUND: Can`t create sound service");

    renderer = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    EntityManager::AddToLayer(REALIZE, GetId(), -1);

    return true;
    // UNGUARD
}

//--------------------------------------------------------------------
uint64_t SOUND::ProcessMessage(MESSAGE &message)
{
    ////GUARD(SOUND::ProcessMessage)

    if (!soundService)
        return 0;

    auto code = message.Long();
    CVECTOR vector, vector2;
    long temp, temp2, temp3, temp4, vt;
    long id, tempLong;
    float minD, maxD;
    long loopPauseTime;
    float v1, v2, v3;
    float volume;
    VDATA *vd1, *vd2, *vd3;
    uint32_t outValue = 0;

    switch (code)
    {
    case MSG_SOUND_SET_MASTER_VOLUME:
        v1 = message.Float();
        v2 = message.Float();
        v3 = message.Float();
        soundService->SetMasterVolume(v1, v2, v3);
        break;
    case MSG_SOUND_GET_MASTER_VOLUME:
        soundService->GetMasterVolume(&v1, &v2, &v3);
        vd1 = message.ScriptVariablePointer();
        vd2 = message.ScriptVariablePointer();
        vd3 = message.ScriptVariablePointer();
        vd1->Set(v1);
        vd2->Set(v2);
        vd3->Set(v3);
        break;
    case MSG_SOUND_SET_CAMERA_POSITION:
        vector.x = message.Float();
        vector.y = message.Float();
        vector.z = message.Float();
        soundService->SetCameraPosition(vector);
        break;
    case MSG_SOUND_SET_CAMERA_ORIENTATION:
        vector.x = message.Float();
        vector.y = message.Float();
        vector.z = message.Float();
        vector2.x = message.Float();
        vector2.y = message.Float();
        vector2.z = message.Float();
        soundService->SetCameraOrientation(vector, vector2);
        break;
    case MSG_SOUND_PLAY: {
        const std::string &tempString = message.String(); // filename

        temp = message.Long(); // type
        // defaults
        vt = static_cast<int>(VOLUME_FX); // volume type
        temp2 = 0;
        temp3 = 0;
        temp4 = 0;
        tempLong = 0;
        vector.x = 0;
        vector.y = 0;
        vector.z = 0;
        minD = -1.0f;
        maxD = -1.0f;
        loopPauseTime = 0;
        volume = 1.f;
        // try to read as many parameters   as we can
        if (message.GetCurrentFormatType())
            vt = message.Long(); // volume_type
        if (message.GetCurrentFormatType())
            temp2 = message.Long(); // simple_cache
        if (message.GetCurrentFormatType())
            temp3 = message.Long(); // looped
        if (message.GetCurrentFormatType())
            temp4 = message.Long(); // cached
        if (message.GetCurrentFormatType())
            tempLong = message.Long(); // fade_in_time
        // boal fix 28.10.06
        if (temp == SOUND_MP3_STEREO)
        {
            if (temp3) // stereo OGG, looped
            {
                if (message.GetCurrentFormatType())
                    loopPauseTime = message.Long();
                if (message.GetCurrentFormatType())
                    volume = message.Float();
            }
        }
        else
        {
            if (message.GetCurrentFormatType())
                vector.x = message.Float();
            if (message.GetCurrentFormatType())
                vector.y = message.Float();
            if (message.GetCurrentFormatType())
                vector.z = message.Float();
            if (message.GetCurrentFormatType())
                minD = message.Float();
            if (message.GetCurrentFormatType())
                maxD = message.Float();
        }

        outValue = static_cast<uint32_t>(soundService->SoundPlay(
            tempString.c_str(), static_cast<eSoundType>(temp), static_cast<eVolumeType>(vt), (temp2 != 0), (temp3 != 0),
            (temp4 != 0), tempLong, &vector, minD, maxD, loopPauseTime, volume));

        break;
    }
    case MSG_SOUND_STOP:
        id = message.Long();
        tempLong = message.Long();
        soundService->SoundStop(id, tempLong);
        break;
    case MSG_SOUND_RELEASE:
        id = message.Long();
        soundService->SoundRelease(id);
        break;
    case MSG_SOUND_DUPLICATE:
        id = message.Long();
        outValue = static_cast<uint32_t>(soundService->SoundDuplicate(id));
        break;
    case MSG_SOUND_SET_3D_PARAM:
        id = message.Long();
        tempLong = message.Long();
        switch (tempLong)
        {
        case SOUND_PARAM_MAX_DISTANCE:
            vector.x = message.Float();
            // memcpy(&(vector.x) ,message.Pointer(), sizeof(float));
            soundService->SoundSet3DParam(id, SM_MAX_DISTANCE, &(vector.x));
            break;
        case SOUND_PARAM_MIN_DISTANCE:
            vector.x = message.Float();
            // memcpy(&(vector.x) ,message.Pointer(), sizeof(float));
            soundService->SoundSet3DParam(id, SM_MIN_DISTANCE, &(vector.x));
            break;
        case SOUND_PARAM_POSITION:
            vector.x = message.Float();
            vector.y = message.Float();
            vector.z = message.Float();
            // memcpy(&(vector.x) ,message.Pointer(), sizeof(float));
            // memcpy(&(vector.y) ,message.Pointer()+sizeof(float), sizeof(float));
            // memcpy(&(vector.z) ,message.Pointer()+(sizeof(float) << 1), sizeof(float));
            soundService->SoundSet3DParam(id, SM_POSITION, &vector);
            break;
        }
        break;
    case MSG_SOUND_SET_VOLUME:
        id = message.Long();
        vector.x = message.Float();
        soundService->SoundSetVolume(id, vector.x);
        break;
    case MSG_SOUND_IS_PLAYING:
        id = message.Long();
        outValue = soundService->SoundIsPlaying(id);
        break;
    case MSG_SOUND_GET_POSITION:
        id = message.Long();
        outValue = static_cast<uint32_t>(soundService->SoundGetPosition(id) * 100.0f);
        break;
    case MSG_SOUND_RESTART:
        id = message.Long();
        soundService->SoundRestart(id);
        break;
    case MSG_SOUND_RESUME:
        id = message.Long();
        temp = message.Long();
        soundService->SoundResume(id, temp);
        break;
    case MSG_SOUND_SCHEME_RESET:
        soundService->ResetScheme();
        break;
    case MSG_SOUND_SCHEME_SET: {
        const std::string &tempString = message.String();
        soundService->SetScheme(tempString.c_str());
        break;
    }
    case MSG_SOUND_SCHEME_ADD: {
        const std::string &tempString = message.String();
        soundService->AddScheme(tempString.c_str());
        break;
    }

    case MSG_SOUND_SET_ENABLED:
        soundService->SetEnabled(message.Long() != 0);
        break;

    case MSG_SOUND_ALIAS_ADD: {
        const std::string &tempString = message.String();
        soundService->LoadAliasFile(tempString.c_str());
        break;
    }
    }

    return outValue;
    ////UNGUARD
}

//--------------------------------------------------------------------
void SOUND::Realize(uint32_t dTime)
{
    if (!soundService)
        return;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
