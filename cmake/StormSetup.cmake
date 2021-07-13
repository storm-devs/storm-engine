cmake_minimum_required(VERSION 3.12.0)

macro(_get_relative_path_folder out path)
    list(APPEND base_folders ${PUBLIC_INCLUDE_DIRS} ${SRC_DIRS})
    list(JOIN base_folders "|" folders_match)
    get_filename_component(dir ${path} DIRECTORY)
    string(REGEX REPLACE "(.*${CMAKE_CURRENT_SOURCE_DIR}/(${folders_match})\/?)" "" ${out} "${dir}")
endmacro()

macro(_process_sources group public_header_only)
    set(files_list "${ARGN}")
    if(files_list)
        # add to srcs list
        list(APPEND SRCS ${ARGN})

        # mark as HEADER_FILE_ONLY if specified 
        set_source_files_properties(${files_list} PROPERTIES HEADER_FILE_ONLY ${public_header_only})

        # map to source group (basically for Visual Studio)
        foreach(file ${files_list})
            _get_relative_path_folder(relative_path ${file})
            if(relative_path)
                string(REPLACE "/" "\\" relative_path ${relative_path})
                source_group("${group}\\${relative_path}" FILES ${file})
            else()
                source_group("${group}" FILES ${file})
            endif()
        endforeach()
    endif()
endmacro()

macro(_collect_sources)
    # parse public include dirs
    foreach(dir ${PUBLIC_INCLUDE_DIRS})
        foreach(ext ${HEADER_FILE_EXTENSIONS})
            list(APPEND public_include_path ${dir}/${ext})
        endforeach()

        file(GLOB_RECURSE glob_result CONFIGURE_DEPENDS ${public_include_path})
        list(APPEND public_include_files ${glob_result})
    endforeach()

    # parse source dirs
    foreach(dir ${SRC_DIRS})
        foreach(ext ${HEADER_FILE_EXTENSIONS})
            list(APPEND src_path ${dir}/${ext})
        endforeach()
        foreach(ext ${SOURCE_FILE_EXTENSIONS})
            list(APPEND src_path ${dir}/${ext})
        endforeach()
        
        file(GLOB_RECURSE glob_result CONFIGURE_DEPENDS ${src_path})
        list(APPEND src_files ${glob_result})
    endforeach()
    if(NOT src_files)
        set(_SETUP_NO_SRC TRUE)
    endif()
    
    # add collected files
    _process_sources("Public Headers" YES ${public_include_files})
    _process_sources("Source Files" NO ${src_files})

    list(REMOVE_DUPLICATES SRCS)
    if(NOT SRCS)
        message(WARNING "[StormSetup] ${_SETUP_TARGET_NAME} has no source or public header files. Skipping...")
        return()
    endif()

    message(STATUS "[StormSetup] Collected sources for \"${_SETUP_TARGET_NAME}\": ${SRCS}")
endmacro()

macro(STORM_SETUP)
    set(options SHARED)
    set(oneValueArgs TARGET_NAME TYPE)
    set(multiValueArgs DEPENDENCIES LINKER_FLAGS)
    cmake_parse_arguments(_SETUP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT _SETUP_TARGET_NAME)
        message(FATAL_ERROR "[StormSetup] No 'TARGET_NAME' specified!")
    endif()

    _collect_sources()

    if(_SETUP_SHARED)
        set(lib_mode SHARED)
    else()
        set(lib_mode STATIC)
    endif()

    if(_SETUP_NO_SRC)
        set(lib_scope INTERFACE)
        set(lib_mode INTERFACE)
    else()
        set(lib_scope PUBLIC)
    endif()

	if(${_SETUP_TYPE} STREQUAL "executable")
		add_executable("${_SETUP_TARGET_NAME}" WIN32 ${SRCS})
	elseif(${_SETUP_TYPE} STREQUAL "library")
		add_library("${_SETUP_TARGET_NAME}" ${lib_mode} ${SRCS})
    elseif(${_SETUP_TYPE} STREQUAL "storm_module")
        add_library("${_SETUP_TARGET_NAME}" ${lib_mode} ${SRCS})
        set_target_properties("${_SETUP_TARGET_NAME}" PROPERTIES
            STORM_MODULE YES
        )
    else()
        message(FATAL_ERROR "[StormSetup] 'TYPE' is incorrect!")
	endif()

	set_target_properties("${_SETUP_TARGET_NAME}" PROPERTIES
        OUTPUT_NAME ${_SETUP_TARGET_NAME} 
        VERSION "1"
    )

    if(_SETUP_DEPENDENCIES)
	    target_link_libraries("${_SETUP_TARGET_NAME}" ${lib_scope} ${_SETUP_DEPENDENCIES})

        if(${_SETUP_TYPE} STREQUAL "executable")
            foreach(dep ${_SETUP_DEPENDENCIES})
                if(TARGET ${dep})
                    get_property(is_storm_module TARGET ${dep} PROPERTY STORM_MODULE SET)
                    if(is_storm_module)
                        # TODO: make it portable? (at least for gcc it is --whole-archive)
                        list(APPEND target_link_flags "/WHOLEARCHIVE:${dep}")
                    endif()
                endif()
            endforeach()
        endif()
    endif()

    if(_SETUP_LINKER_FLAGS)
        list(APPEND target_link_flags ${_SETUP_LINKER_FLAGS})
    endif()

    if(target_link_flags)
        list(JOIN target_link_flags " " target_link_flags_string)
        set_target_properties(${_SETUP_TARGET_NAME} PROPERTIES LINK_FLAGS ${target_link_flags_string})
    endif()

    if(_SETUP_NO_SRC)
        set_target_properties("${_SETUP_TARGET_NAME}" PROPERTIES LINKER_LANGUAGE CXX)
    endif()

    # install public headers
    foreach(dir ${PUBLIC_INCLUDE_DIRS})
        target_include_directories(
            "${_SETUP_TARGET_NAME}" ${lib_scope}
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${dir}>
            $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${dir}>
        )
    endforeach()
 endmacro()