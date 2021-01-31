project(setup)

add_executable(${PROJECT_NAME} WIN32
	Setup.cpp
	Setup.h
	SetupDlg.cpp
	SetupDlg.h
	StdAfx.cpp
	StdAfx.h

	${Storm_RESOURCE_DIR}/setup/setup.rc
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
	${Storm_RESOURCE_DIR}/setup
	)

target_link_options(${PROJECT_NAME} PRIVATE /ENTRY:"wWinMainCRTStartup")
