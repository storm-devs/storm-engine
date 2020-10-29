project(mleaks)

add_library(mleaks MODULE 
	MLeaks.cpp
	MLeaks.h
	MLeaksDlg.cpp
	MLeaksDlg.h
	StdAfx.cpp
	StdAfx.h
	resource.h
)
target_include_directories(mleaks PRIVATE common_h)
target_include_directories(mleaks PRIVATE SoundService)
target_include_directories(mleaks PRIVATE extern/directx/include)
