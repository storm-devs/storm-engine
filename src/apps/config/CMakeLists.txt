project(config)

add_executable(${PROJECT_NAME} WIN32
    Setup.cpp
    Setup.h
    SetupDlg.cpp
    SetupDlg.h
    StdAfx.cpp
    StdAfx.h

    ${STORM_RESOURCE_DIR}/setup/setup.rc
)

target_include_directories(${PROJECT_NAME} PRIVATE ${STORM_RESOURCE_DIR}/setup)

target_compile_definitions(${PROJECT_NAME} PRIVATE _AFXDLL)

target_link_options(${PROJECT_NAME} PRIVATE /ENTRY:wWinMainCRTStartup)
