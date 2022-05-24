cmake_minimum_required(VERSION 3.19.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ##############################################################################
# normalize specified booleans to be set to either True or False
# ##############################################################################
macro(normalize_booleans)
set(argn "${ARGN}")
foreach(arg IN LISTS argn)
    if(${arg})
	  set(${arg} True)
	else()
	  set(${arg} False)
	endif()
  endforeach()
endmacro()

# ##############################################################################
# add all subdirs using add_subdirectory
# ##############################################################################
macro(_subdirlist out)
  file(
    GLOB glob_result
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *)
  foreach(path ${glob_result})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${path})
      list(APPEND dirs ${path})
    endif()
  endforeach()
  set(${out} ${dirs})
endmacro()

macro(add_all_subdirectories)
  _subdirlist(dirs)

  foreach(dir ${dirs})
    add_subdirectory(${dir})
  endforeach()
endmacro()

# ##############################################################################
# setup individual project with globbing
# ##############################################################################
macro(_set_ide_folder target folder)
  set_target_properties(${target} PROPERTIES FOLDER ${folder})
endmacro()

macro(_get_relative_path_folder out path)
  get_filename_component(dir ${path} DIRECTORY)
  file(RELATIVE_PATH PROJECT_RELATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}"
       "${dir}")

  set(base_folders "${SRC_DIRS};${PUBLIC_INCLUDE_DIRS};${TESTSUITE_DIRS};${RESOURCE_DIRS}")
  list(JOIN base_folders "|" folders_match)

  string(REGEX REPLACE "((${folders_match})\/?)" "" ${out}
                       "${PROJECT_RELATIVE_PATH}")
endmacro()

macro(_process_sources group out only_headers)
  set(files_list "${ARGN}")
  if(files_list)
    # add to srcs list
    list(APPEND ${out} ${ARGN})

    # mark as HEADER_FILE_ONLY if specified
    set_source_files_properties(${files_list} PROPERTIES HEADER_FILE_ONLY
                                                         ${only_headers})

    # map to source group (basically for Visual Studio)
    if(NOT group STREQUAL "")
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

  # parse testsuite dirs
  foreach(dir ${TESTSUITE_DIRS})
    foreach(ext ${HEADER_FILE_EXTENSIONS})
      list(APPEND test_path ${dir}/${ext})
    endforeach()
    foreach(ext ${SOURCE_FILE_EXTENSIONS})
      list(APPEND test_path ${dir}/${ext})
    endforeach()

    file(GLOB_RECURSE glob_result CONFIGURE_DEPENDS ${test_path})
    list(APPEND test_files ${glob_result})
  endforeach()

  # parse resource dirs
  foreach(dir ${RESOURCE_DIRS})
    file(GLOB_RECURSE glob_result CONFIGURE_DEPENDS ${dir}/*)
    list(APPEND resource_files ${glob_result})
  endforeach()

  # add collected files
  _process_sources("Public Headers" SRCS YES ${public_include_files})
  _process_sources("Source Files" SRCS NO ${src_files})
  _process_sources("Resource Files" SRCS NO ${resource_files})
  _process_sources("" TEST_SRCS NO ${test_files})

  list(REMOVE_DUPLICATES SRCS)
  list(REMOVE_DUPLICATES TEST_SRCS)

  if(NOT SRCS)
    message(
      WARNING
        "[StormSetup] ${_SETUP_TARGET_NAME} has no source or public header files. Skipping..."
    )
    return()
  endif()

  if(src_files)
    set(_SETUP_HAS_SRC TRUE)
  endif()
  if(test_files)
    set(_SETUP_HAS_TESTSUITE TRUE)
  endif()

  message(
    STATUS
      "[StormSetup] Collected sources for \"${_SETUP_TARGET_NAME}\": ${SRCS} (testsuite: ${TEST_SRCS})"
  )
endmacro()

macro(STORM_SETUP)
  set(options SHARED)
  set(oneValueArgs TARGET_NAME TYPE)
  set(multiValueArgs DEPENDENCIES TEST_DEPENDENCIES LINKER_FLAGS)
  cmake_parse_arguments(_SETUP "${options}" "${oneValueArgs}"
                        "${multiValueArgs}" ${ARGN})

  if(NOT _SETUP_TARGET_NAME)
    message(FATAL_ERROR "[StormSetup] No 'TARGET_NAME' specified!")
  endif()

  _collect_sources()

  if(_SETUP_SHARED)
    set(lib_mode SHARED)
  else()
    set(lib_mode STATIC)
  endif()

  if(NOT _SETUP_HAS_SRC)
    set(lib_scope INTERFACE)
    set(lib_mode INTERFACE)
  else()
    set(lib_scope PUBLIC)
  endif()

  if(${_SETUP_TYPE} STREQUAL "executable")
    add_executable("${_SETUP_TARGET_NAME}" WIN32 ${SRCS})
    _set_ide_folder("${_SETUP_TARGET_NAME}" "Executables")
  elseif(${_SETUP_TYPE} STREQUAL "library")
    add_library("${_SETUP_TARGET_NAME}" ${lib_mode} ${SRCS})
    _set_ide_folder("${_SETUP_TARGET_NAME}" "Libs")
  elseif(${_SETUP_TYPE} STREQUAL "storm_module")
    add_library("${_SETUP_TARGET_NAME}" ${lib_mode} ${SRCS})
    _set_ide_folder("${_SETUP_TARGET_NAME}" "Modules")
    set(global_modules_list "${global_modules_list};${_SETUP_TARGET_NAME}" CACHE INTERNAL "global_modules_list")
  else()
    message(FATAL_ERROR "[StormSetup] 'TYPE' is incorrect!")
  endif()

  if(_SETUP_HAS_TESTSUITE)
    set(test_target "${_SETUP_TARGET_NAME}-test")

    add_executable(${test_target} ${TEST_SRCS})
    target_link_libraries(${test_target} PRIVATE ${_SETUP_TEST_DEPENDENCIES}
                                                 ${_SETUP_TARGET_NAME})
    _set_ide_folder(${test_target} "Tests")

    # setup post-build hook to run the test after build
    add_custom_command(
      TARGET ${test_target}
      COMMENT "Run ${test_target}"
      POST_BUILD
      COMMAND ${test_target})
  endif()

  set_target_properties(
    "${_SETUP_TARGET_NAME}" PROPERTIES OUTPUT_NAME ${_SETUP_TARGET_NAME} VERSION
                                                                         "1")

  # link storm modules to executable forcing unused symbols to be preserved
  if(${_SETUP_TYPE} STREQUAL "executable")
    if(NOT WIN32)
      list(APPEND target_link_flags "-Wl,--whole-archive")
    endif()
    list(REMOVE_DUPLICATES global_modules_list)
    list(APPEND _SETUP_DEPENDENCIES "${global_modules_list}")
    foreach(module ${global_modules_list})
      if(TARGET ${module})
        # TODO: make it portable? (at least for gcc it is --whole-archive)
        if (WIN32)
          list(APPEND target_link_flags "/WHOLEARCHIVE:${module}")
        else()
          list(APPEND target_link_flags "${CMAKE_BINARY_DIR}/lib/lib${module}.a")
        endif()
      endif()
    endforeach()
    if(NOT WIN32)
      list(APPEND target_link_flags "-Wl,--no-whole-archive")
    endif()
  endif()

  if(_SETUP_DEPENDENCIES)
    target_link_libraries("${_SETUP_TARGET_NAME}" ${lib_scope}
                          ${_SETUP_DEPENDENCIES})
  endif()

  if(_SETUP_LINKER_FLAGS)
    list(APPEND target_link_flags ${_SETUP_LINKER_FLAGS})
  endif()

  if(target_link_flags)
    list(JOIN target_link_flags " " target_link_flags_string)
    set_target_properties(${_SETUP_TARGET_NAME}
                          PROPERTIES LINK_FLAGS ${target_link_flags_string})
  endif()

  if(NOT _SETUP_HAS_SRC)
    set_target_properties("${_SETUP_TARGET_NAME}" PROPERTIES LINKER_LANGUAGE
                                                             CXX)
  endif()

  # install public headers
  foreach(dir ${PUBLIC_INCLUDE_DIRS})
    target_include_directories(
      "${_SETUP_TARGET_NAME}" ${lib_scope}
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${dir}>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${dir}>)
  endforeach()
endmacro()
