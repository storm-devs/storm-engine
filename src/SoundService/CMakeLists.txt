cmake_minimum_required(VERSION 3.18)

project(soundservice)

add_library(soundservice MODULE 
	A3DCapable.cpp
	A3DCapable.h
	AMP3Coded.cpp
	AMP3Coded.h
	APCMCoded.cpp
	APCMCoded.h
	AStereoCapable.cpp
	AStereoCapable.h
	CMakeLists.txt
	DebugEntity.cpp
	DebugEntity.h
	MP33DSound.cpp
	MP33DSound.h
	MP3StereoSound.cpp
	MP3StereoSound.h
	PCM3DSound.cpp
	PCM3DSound.h
	PCMStereoSound.cpp
	PCMStereoSound.h
	SoundDefines.h
	SoundService.cpp
	SoundService.h
	THashDataMap.cpp
	THashDataMap.h
	TLimitedStream.cpp
	TLimitedStream.h
	TSDArray.h
	TSoundCore.cpp
	TSoundCore.h
	VSound.cpp
	VSound.h
	VSoundService.cpp
	VSoundService.h
	XBOXPCMSound.cpp
	XBOXPCMSound.h
)
target_include_directories(soundservice PRIVATE common_h)
target_include_directories(soundservice PRIVATE SoundService)
target_include_directories(soundservice PRIVATE extern/directx/include)
