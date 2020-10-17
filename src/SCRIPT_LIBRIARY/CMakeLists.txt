project(scriptlibrary)

add_library(scriptlibrary MODULE 
	script_libriary_test.cpp
	script_libriary_test.h
	vano_files.cpp
	vano_files.h
)

target_include_directories(scriptlibrary PRIVATE ${DirectX_INCLUDE_DIR})