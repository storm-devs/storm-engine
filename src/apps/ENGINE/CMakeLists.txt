project(engine)

find_package(SteamSdk REQUIRED)
find_package(Zlib REQUIRED)
find_package(Fmod REQUIRED) # required to set ${Fmod_LIBRARY_PATHS}

add_executable(${PROJECT_NAME} WIN32
	achievements.cpp
	compiler.cpp
	compiler.h
	Core.cpp
	Core.h
	data.cpp
	data.h
	dstring_codec.h
	expression.cpp
	externs.cpp
	externs.h
	file_service.cpp
	file_service.h
	fs.cpp
	fs.h
	ifs.cpp
	ifs.h
	internal_functions.cpp
	Main.cpp
	message_script.h
	messages.h
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
	services_list.cpp
	services_list.h
	sstring.h
	stats.h
	string_codec.h
	strings_list.cpp
	strings_list.h
	tclass_list.h
	timer.h
	tm_list.cpp
	tm_list.h
	token.cpp
	token.h

	${STORM_RESOURCE_DIR}/engine/resource.rc
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
	${STORM_RESOURCE_DIR}/engine
	${SteamSdk_INCLUDE_DIR}
	${Zlib_INCLUDE_DIR}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE _ENGINE_IN_)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	common_h
	
	animals
	animation
	ballsplash
	battleinterface
	blade
	blot
	collide
	dialog
	geometry
	island
	lighter
	location
	locator
	mast
	model
	particles
	pcs_controls
	renderer
	rigging
	sailors
	script_library
	sea
	sea_ai
	sea_cameras
	sea_operator
	seacreatures
	seafoam
	shadow
	ship
	sink_effect
	sound
	soundservice
	teleport
	tornado
	touch
	waterrings
	weather
	worldmap
	xinterface

	${SteamSdk_LIBRARY}
	${Zlib_LIBRARY}
	comctl32.lib
	dbghelp.lib
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SteamSdk_LIBRARY_PATHS}/steam_api64.dll"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
		
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${Zlib_LIBRARY_PATHS}/zlib1.dll"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
		
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${Fmod_LIBRARY_PATHS}/fmod.dll"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
		
	COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${STORM_SOURCE_DIR}/techniques"
        "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/techniques"
)