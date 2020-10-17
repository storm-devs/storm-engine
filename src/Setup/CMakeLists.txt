project(setup)

add_executable(setup WIN32 
	Resource.h
	Setup.cpp
	Setup.h
	SetupDlg.cpp
	SetupDlg.h
	StdAfx.cpp
	StdAfx.h
)

target_link_libraries(setup PRIVATE ddraw.lib)