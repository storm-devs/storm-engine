cmake_minimum_required(VERSION 3.18)

project(animation)

add_library(animation MODULE 
	ActionInfo.cpp
	ActionInfo.h
	ActionPlayerImp.cpp
	ActionPlayerImp.h
	AnimationImp.cpp
	AnimationImp.h
	AnimationInfo.cpp
	AnimationInfo.h
	AnimationServiceImp.cpp
	AnimationServiceImp.h
	AnimationTimerImp.cpp
	AnimationTimerImp.h
	Bone.cpp
	Bone.h
	UserData.cpp
	UserData.h
)
target_include_directories(animation PRIVATE common_h)
target_include_directories(animation PRIVATE SoundService)
target_include_directories(animation PRIVATE extern/directx/include)
