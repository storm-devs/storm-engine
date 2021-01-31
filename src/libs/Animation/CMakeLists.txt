project(animation)

add_library(${PROJECT_NAME} STATIC
	ActionInfo.cpp
	ActionInfo.h
	ActionPlayerImp.cpp
	ActionPlayerImp.h
	an_file.h
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
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)