# Automatically configures a new library target using the source files in DIRECTORY
#
# Set NAMESPACE to add a prefix to install targets and folders
function(auto_setup_library LIBRARY_NAME)
    cmake_parse_arguments(PARAM "" "DIRECTORY;SOURCE_DIRECTORY;INCLUDE_DIRECTORY;NAMESPACE;TYPE" "" ${ARGN})

    if(NOT DEFINED PARAM_DIRECTORY)
      set(PARAM_DIRECTORY .)
    endif()

    if(NOT DEFINED PARAM_SOURCE_DIRECTORY)
      set(PARAM_SOURCE_DIRECTORY ${PARAM_DIRECTORY})
    endif()

    if(NOT DEFINED PARAM_INCLUDE_DIRECTORY)
      set(PARAM_INCLUDE_DIRECTORY ${PARAM_DIRECTORY})
    endif()

    if(NOT DEFINED PARAM_TYPE)
      set(PARAM_TYPE STATIC)
    endif()

    get_filename_component(LIBRARY_INCLUDE_DIRECTORY ${PARAM_INCLUDE_DIRECTORY} ABSOLUTE)

    set(GLOBBING_EXPRESSION_SOURCES
        ${PARAM_SOURCE_DIRECTORY}/*.c
        ${PARAM_SOURCE_DIRECTORY}/*.cpp)
    set(GLOBBING_EXPRESSION_HEADERS
        ${PARAM_INCLUDE_DIRECTORY}/*.h
        ${PARAM_INCLUDE_DIRECTORY}/*.hpp)

    file(GLOB_RECURSE TARGET_SOURCES ${GLOBBING_EXPRESSION_SOURCES})
    file(GLOB_RECURSE TARGET_HEADERS ${GLOBBING_EXPRESSION_HEADERS})

    if (NOT TARGET_SOURCES)
        set(LIBRARY_TYPE INTERFACE)
        set(LIBRARY_INCLUDE_TYPE INTERFACE)
    else()
        set(LIBRARY_TYPE ${PARAM_TYPE})
        set(LIBRARY_INCLUDE_TYPE PUBLIC)
    endif()

    add_library(${LIBRARY_NAME} ${LIBRARY_TYPE} ${TARGET_SOURCES} ${TARGET_HEADERS})

    target_include_directories(${LIBRARY_NAME} ${LIBRARY_INCLUDE_TYPE}
            $<BUILD_INTERFACE:${LIBRARY_INCLUDE_DIRECTORY}>
            )

    set_target_properties(${LIBRARY_NAME} PROPERTIES OUTPUT_NAME ${PARAM_NAMESPACE}${LIBRARY_NAME})
    set_target_properties(${LIBRARY_NAME} PROPERTIES DEBUG_POSTFIX d)
    set_target_properties(${LIBRARY_NAME} PROPERTIES CXX_STANDARD 20)


    install(TARGETS ${LIBRARY_NAME} DESTINATION lib)
    install(DIRECTORY ${PARAM_INCLUDE_DIRECTORY}/ DESTINATION include/ FILES_MATCHING PATTERN *.h PATTERN *.hpp)
endfunction()


# Automatically configures a new executable target using the source files in DIRECTORY
#
# Set NAMESPACE to add a prefix to install targets and folders
function(auto_setup_executable EXE_NAME)
    cmake_parse_arguments(PARAM "" "DIRECTORY;SOURCE_DIRECTORY;INCLUDE_DIRECTORY" "" ${ARGN})

    if(NOT DEFINED PARAM_DIRECTORY)
      set(PARAM_DIRECTORY .)
    endif()

    if(NOT DEFINED PARAM_SOURCE_DIRECTORY)
      set(PARAM_SOURCE_DIRECTORY ${PARAM_DIRECTORY})
    endif()

    if(NOT DEFINED PARAM_INCLUDE_DIRECTORY)
      set(PARAM_INCLUDE_DIRECTORY ${PARAM_DIRECTORY})
    endif()

    get_filename_component(LIBRARY_INCLUDE_DIRECTORY ${PARAM_INCLUDE_DIRECTORY} ABSOLUTE)

    set(GLOBBING_EXPRESSION_SOURCES
        ${PARAM_SOURCE_DIRECTORY}/*.c
        ${PARAM_SOURCE_DIRECTORY}/*.cpp)
    set(GLOBBING_EXPRESSION_HEADERS
        ${PARAM_INCLUDE_DIRECTORY}/*.h
        ${PARAM_INCLUDE_DIRECTORY}/*.hpp)

    file(GLOB_RECURSE TARGET_SOURCES ${GLOBBING_EXPRESSION_SOURCES})
    file(GLOB_RECURSE TARGET_HEADERS ${GLOBBING_EXPRESSION_HEADERS})

    add_executable(${EXE_NAME} ${TARGET_SOURCES} ${TARGET_HEADERS})

    set_target_properties(${EXE_NAME} PROPERTIES CXX_STANDARD 20)

    target_include_directories(${EXE_NAME} PUBLIC
                               $<BUILD_INTERFACE:${LIBRARY_INCLUDE_DIRECTORY}>
                               )

    install(TARGETS ${EXE_NAME} DESTINATION bin)
endfunction()

# Add all source files in DIRECTORY to an existing target
function(add_source_directory TARGET_NAME)
  cmake_parse_arguments(PARAM "INSTALL" "DIRECTORY" "" ${ARGN})

  if(NOT DEFINED PARAM_DIRECTORY)
    set(PARAM_DIRECTORY .)
  endif()

  set(GLOBBING_EXPRESSION_SOURCES
      ${PARAM_DIRECTORY}/*.c
      ${PARAM_DIRECTORY}/*.cpp)
  set(GLOBBING_EXPRESSION_HEADERS
      ${PARAM_DIRECTORY}/*.h
      ${PARAM_DIRECTORY}/*.hpp)

    file(GLOB_RECURSE TARGET_SOURCES ${GLOBBING_EXPRESSION_SOURCES})
    file(GLOB_RECURSE TARGET_HEADERS ${GLOBBING_EXPRESSION_HEADERS})

    target_sources(${TARGET_NAME} PRIVATE ${TARGET_SOURCES})
    target_sources(${TARGET_NAME} INTERFACE ${TARGET_HEADERS})

  if(DEFINED PARAM_INSTALL)
    install(DIRECTORY ${PARAM_DIRECTORY}/ DESTINATION include/${PARAM_NAMESPACE}/${LIBRARY_NAME} FILES_MATCHING PATTERN *.h PATTERN *.hpp)
  endif()
endfunction()
