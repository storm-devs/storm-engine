project(cameradialog)

include_directories(
	${DirectX_INCLUDE_DIR}
)

add_library(cameradialog MODULE 
	CameraDialog.cpp
	CameraDialog.h
)
