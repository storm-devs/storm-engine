#ifndef _PROGRAM_SOUND_H_
#define _PROGRAM_SOUND_H_

#define SOUND_INVALID_ID 0

// SoundPlay (..., type, ...)
#define SOUND_WAV_3D     1
#define SOUND_MP3_STEREO 2
#define SOUND_MP3_3D     3
#define SOUND_WAV_STEREO 4

// SoundPlay (..., volume_type, ...)
#define VOLUME_FX        1
#define VOLUME_MUSIC     2
#define VOLUME_SPEECH    3

//SoundSet3DParam(..., type, ...)
#define SOUND_PARAM_MAX_DISTANCE 1
#define SOUND_PARAM_MIN_DISTANCE 2
#define SOUND_PARAM_POSITION     3

#endif