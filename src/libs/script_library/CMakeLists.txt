project(script_library)

add_library(${PROJECT_NAME} STATIC
	script_libriary_test.cpp
	script_libriary_test.h
	vano_files.cpp
	vano_files.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)