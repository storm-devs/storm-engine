project(engine)

set(ZLIB_ROOT "${Storm_EXTERN_DIR}/zlib")

find_package(SteamSdk REQUIRED)
find_package(ZLIB REQUIRED)
find_package(DirectX REQUIRED)

add_executable(engine WIN32 
	achievements.cpp
	C_atom.cpp
	C_atom.h
	classes_table.cpp
	classes_table.h
	common_h.h
	compiler.cpp
	compiler.h
	control_stack.cpp
	control_stack.h
	Core.cpp
	Core.h
	ctp_names.cpp
	dat.h
	data.cpp
	data.h
	dstring_codec.h
	entity_state_R.cpp
	entity_state_R.h
	expression.cpp
	externs.h
	file_service.cpp
	file_service.h
	fsdata_list.h
	gdi_display.cpp
	gdi_display.h
	hashhold.cpp
	hashhold.h
	idwalker.h
	ifs.cpp
	ifs.h
	input.cpp
	input.h
	internal_functions.cpp
	internal_functions.h
	internal_modules.cpp
	internal_modules.h
	layer.cpp
	layer.h
	layer_service.cpp
	layer_service.h
	loopstack.cpp
	loopstack.h
	Main.cpp
	mem_pool.cpp
	mem_pool.h
	mem_utils.cpp
	mem_utils.h
	memory_service.cpp
	memory_service.h
	message_script.h
	messages.h
	modules_table.cpp
	modules_table.h
	pcinput.cpp
	pcinput.h
	program.cpp
	program.h
	resource.h
	s_classtab.cpp
	s_classtab.h
	s_compress.cpp
	s_compress.h
	s_dbg_breaktable.cpp
	s_dbg_breaktable.h
	s_dbg_sourceview.cpp
	s_dbg_sourceview.h
	s_dbg_staticlist.h
	s_dbg_watcherlist.cpp
	s_dbg_watcherlist.h
	s_debug.cpp
	s_debug.h
	s_deftab.cpp
	s_deftab.h
	s_eventmsg.h
	s_eventtab.cpp
	s_eventtab.h
	s_functab.cpp
	s_functab.h
	s_postevents.h
	s_stack.cpp
	s_stack.h
	s_tstack.cpp
	s_tstack.h
	s_vartab.cpp
	s_vartab.h
	safequeue.h
	services_list.cpp
	services_list.h
	sp.h
	sstring.h
	stack.cpp
	stack.h
	stats.cpp
	stats.h
	string_codec.h
	strings_list.cpp
	strings_list.h
	system_api.cpp
	system_api.h
	system_log.cpp
	system_log.h
	tclass_list.h
	timer.h
	tm_list.cpp
	tm_list.h
	token.cpp
	token.h
	vbuffer.h
)
target_include_directories(engine PRIVATE ${Storm_SOURCE_DIR}/Common_h)
target_include_directories(engine PRIVATE ${SteamSdk_INCLUDE_DIR})
target_include_directories(engine PRIVATE ${DirectX_INCLUDE_DIR})

target_compile_definitions(engine PRIVATE _ENGINE_IN_)

target_link_libraries(engine PRIVATE ${SteamSdk_LIBRARY})
target_link_libraries(engine PRIVATE ZLIB::ZLIB)
target_link_libraries(engine PRIVATE comctl32.lib)

add_custom_command(TARGET engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SteamSdk_LIBRARY_PATHS}/steam_api.dll"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
		
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ZLIB_INCLUDE_DIRS}/zlib.dll"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
) 
