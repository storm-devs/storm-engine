project(animation)

find_package(DirectX REQUIRED)

include_directories(
	${Storm_SOURCE_DIR}/Common_h
	${DirectX_INCLUDE_DIR}
)

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

target_link_libraries(animation PRIVATE ${DirectX_D3DX9_LIBRARY})