project(engine)

set(ZLIB_ROOT "${Storm_EXTERN_DIR}/zlib")

find_package(SteamSdk REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Fmod REQUIRED) # required to set ${Fmod_LIBRARY_PATHS}

add_executable(${PROJECT_NAME} WIN32
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

	${Storm_RESOURCE_DIR}/engine/resource.rc
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
	${Storm_RESOURCE_DIR}/engine
	${SteamSdk_INCLUDE_DIR}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE _ENGINE_IN_)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	common_h

	${SteamSdk_LIBRARY}
	ZLIB::ZLIB
	comctl32.lib
	dbghelp.lib
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SteamSdk_LIBRARY_PATHS}/steam_api.dll"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
		
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ZLIB_INCLUDE_DIRS}/zlib.dll"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
		
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${Fmod_LIBRARY_PATHS}/fmod.dll"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
		
	COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${Storm_SOURCE_DIR}/techniques"
        "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/techniques"
)