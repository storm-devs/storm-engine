project(artifact)

include_directories(
	#${Storm_SOURCE_DIR}/Common_h
	${DirectX_INCLUDE_DIR}
)

add_library(artifact MODULE 
	Artifact.cpp
	Artifact.h
	bsps.cpp
	bsps.h
	${Storm_SOURCE_DIR}/Common_h/rands.cpp
)

# target_include_directories(artifact PRIVATE common_h)
# target_include_directories(artifact PRIVATE SoundService)
# target_include_directories(artifact PRIVATE extern/directx/include)
