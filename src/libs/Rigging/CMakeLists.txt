project(rigging)

add_library(${PROJECT_NAME} MODULE
	Flag.cpp
	Flag.h
	Rope.cpp
	Rope.h
	SAILONE.cpp
	SAILONE.h
	Vant.cpp
	Vant.h
	rigging_define.h
	rigging_main.cpp
	sail.cpp
	sail.h
	script_func.cpp
	script_func.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)