project(sea_cameras)

add_library(${PROJECT_NAME} STATIC
	Common_Camera.h
	DeckCamera.cpp
	DeckCamera.h
	FreeCamera.cpp
	FreeCamera.h
	SEA_CAMERAS.cpp
	SEA_CAMERAS.h
	ShipCamera.cpp
	ShipCamera.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)